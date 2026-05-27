/*******************************************************************************
  Non-Volatile Memory Controller(NVM) PLIB.

  Company:
    Microchip Technology Inc.

  File Name:
    plib_nvm.c

  Summary:
    Interface definition of NVM Plib.

  Description:
    This file defines the interface for the NVM Plib.
    It allows user to Program, Erase and lock the on-chip Non Volatile Flash
    Memory.
*******************************************************************************/

// DOM-IGNORE-BEGIN
/*******************************************************************************
* Copyright (C) 2025 Microchip Technology Inc. and its subsidiaries.
*
* Subject to your compliance with these terms, you may use Microchip software
* and any derivatives exclusively with Microchip products. It is your
* responsibility to comply with third party license terms applicable to your
* use of third party software (including open source software) that may
* accompany Microchip software.
*
* THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER
* EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED
* WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A
* PARTICULAR PURPOSE.
*
* IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE,
* INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND
* WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS
* BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO THE
* FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN
* ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY,
* THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
*******************************************************************************/
// DOM-IGNORE-END

// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************

#include <string.h>
#include "interrupts.h"
#include "plib_nvm.h"

#define  FLASH_ERASE_PAGE_SIZE_IN_INSTRUCTIONS  1024U
#define  FLASH_ERASE_PAGE_MASK  (~((FLASH_ERASE_PAGE_SIZE_IN_INSTRUCTIONS * 4U) - 1U))
#define  FLASH_ADDRESS_MASK  4U

/* ************************************************************************** */
/* ************************************************************************** */
/* Section: File Scope or Global Data                                         */
/* ************************************************************************** */
/* ************************************************************************** */
// *****************************************************************************

/*******************************************
 * Internal operation type
 ******************************************/
typedef enum
{
    NVM_NO_OPERATION                      = 0x0,
    NVM_WORD_WRITE_OPERATION              = 0x4001,
    NVM_ROW_WRITE_OPERATION               = 0x4002,
    NVM_PAGE_ERASE_OPERATION              = 0x4003,
} NVM_OPERATION_MODE;

volatile static nvmCallbackObjType nvmCallbackObj;
/* ************************************************************************** */
/* ************************************************************************** */
// Section: Local Functions                                                   */
/* ************************************************************************** */
/* ************************************************************************** */

// *****************************************************************************
// *****************************************************************************
// Section: NVM Implementation
// *****************************************************************************
// *****************************************************************************

void NVM_CallbackRegister( NVM_CALLBACK callback, uintptr_t context )
{
    /* Register callback function */
    nvmCallbackObj.CallbackFunc    = callback;
    nvmCallbackObj.Context         = context;
}

void __attribute__((used)) NVM_InterruptHandler( void )
{
    _NVMIF = 0;

    if(nvmCallbackObj.CallbackFunc != NULL)
    {
        uintptr_t context = nvmCallbackObj.Context;
        nvmCallbackObj.CallbackFunc(context);
    }
}


static void NVM_StartOperationAtAddress( uint32_t address,  NVM_OPERATION_MODE operation )
{
    // Set the target Flash address to be operated on (destination).
    NVMADR = address;

    // Set the flash operation:
    /***************************************************************************
     * Page erase: Erases the entire page which includes the target address
     *    (NVMADR) if it is not write-protected.
     * Quad Word (128-bit) program: Programs the 128 bit quad words in NVMDATA0
     *    through NVMDATA3 to flash address selected by NVMADR, if it they are
     *    not write-protected.
     * Row program: Programs the entire row from the physical address in
     *    NVMSRCADR to the flash address selected by NVMADR if it is not
     *    write-protected
     **************************************************************************/

    NVMCON = (uint32_t)operation;

    //Enable interrupt just before initiating the write
    _NVMIE = 1;

    // Initiate write operation
    NVMCONbits.WR = 1U;
}

/* ************************************************************************** */
/* ************************************************************************** */
// Section: Interface Functions                                               */
/* ************************************************************************** */
/* ************************************************************************** */

void NVM_Initialize( void )
{
    NVM_StartOperationAtAddress( NVMADR,  NVM_NO_OPERATION );
}


bool NVM_Read( uint32_t *data, uint32_t length, const uint32_t address )
{
    (void) memcpy(data, (uint32_t*)address, length);

    return true;
}

bool NVM_QuadWordWrite( uint32_t *data, uint32_t address )
{
   bool status = false;
   NVMDATA0 = *(data++);
   NVMDATA1 = *(data++);
   NVMDATA2 = *(data++);
   NVMDATA3 = *(data++);
   if(0U == (address % FLASH_ADDRESS_MASK))
   {
       NVM_StartOperationAtAddress( address,  NVM_WORD_WRITE_OPERATION);
       status = true;
   }

   return status;
}


bool NVM_RowWrite( uint32_t *data, uint32_t address )
{
    bool status = false;
    NVMSRCADR = (uint32_t)data;

    if(0U == (address % FLASH_ADDRESS_MASK))
    {
        NVM_StartOperationAtAddress( address,  NVM_ROW_WRITE_OPERATION);
        status = true;
    }

    return status;
}

bool NVM_PageErase( uint32_t address )
{
    address = (FLASH_ERASE_PAGE_MASK & address);
    NVM_StartOperationAtAddress(address,  NVM_PAGE_ERASE_OPERATION);
    return true;
}

NVM_ERROR NVM_ErrorGet( void )
{
    // mask for WREC and WRERR bits
    return (NVMCON & (_NVMCON_WREC_MASK | _NVMCON_WRERR_MASK));
}

bool NVM_IsBusy( void )
{
    return (bool)NVMCONbits.WR;
}
