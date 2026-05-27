/*******************************************************************************
  Non-Volatile Memory Controller(NVM) PLIB.

  Company:
    Microchip Technology Inc.

  File Name:
    plib_nvm.h

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

#ifndef PLIB_NVM_H
#define PLIB_NVM_H
// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************

#include "device.h"     // For device registers and uint32_t
#include <stdbool.h>    // For bool

// DOM-IGNORE-BEGIN
#ifdef __cplusplus // Provide C++ Compatibility

    extern "C" {

#endif

// DOM-IGNORE-END
/**
  @ingroup  nvm
  @brief    This macro defines the start address of the NVM flash
*/
#define NVM_FLASH_START_ADDRESS    (0x801000U)
/**
  @ingroup  nvm
  @brief    This macro defines the size of the NVM flash
*/
#define NVM_FLASH_SIZE             (0xf000U)
/**
  @ingroup  nvm
  @brief    This macro defines the size of the one NVM flash row
*/
#define NVM_FLASH_ROWSIZE          (128U)
/**
  @ingroup  nvm
  @brief    This macro defines the size of the one NVM flash page
*/
#define NVM_FLASH_PAGESIZE         (1024U)
/**
  @ingroup  nvm
  @brief    This macro defines the smallest word write size in instruction words
*/
#define NVM_FLASH_MIN_WRITE_SIZE   (4U)




/**
  @ingroup  nvm
  @brief    Callback function prototype
*/
typedef void (*NVM_CALLBACK)(uintptr_t context);
typedef struct
{
    NVM_CALLBACK CallbackFunc;
    uintptr_t Context;
}nvmCallbackObjType;

typedef enum
{
    /* No error */
    NVM_ERROR_NONE = 0x0,

    /* NVM write error */
    NVM_ERROR_WRITE = _NVMCON_WRERR_MASK,

    /* NVM Error reported by Flash panel control logic */
    NVM_ERROR_CONTROL_LOGIC = 0x30000,
    
    /* NVM System bus error during row program operation */
    NVM_ERROR_SYSTEM_BUS =  0x40000,

    /* NVM Row programming operation not completed due to warm Reset */
    NVM_ERROR_ROW_PROGRAM = 0x50000

} NVM_ERROR;

/**
 * @ingroup    nvm
 * @brief      Initializes the NVM module
 * @return     none
 */

void NVM_Initialize( void );

/**
 * @ingroup    nvm
 * @brief      Reads specified amount of data from NVM starting from the given address
 * @return     returns true on successful operation else returns false
 */

bool NVM_Read( uint32_t *data, uint32_t length, const uint32_t address );

/**
 * @ingroup    nvm
 * @brief      Writes a quad word(128 bits of data) to the given NVM address
 * @return     returns true on successful operation else returns false
 */

bool NVM_QuadWordWrite( uint32_t *data, uint32_t address );

/**
 * @ingroup    nvm
 * @brief      Writes a row of data to the given NVM address
 * @return     returns true on successful operation else returns false
 */

bool NVM_RowWrite( uint32_t *data, uint32_t address );

/**
 * @ingroup    nvm
 * @brief      Erases a NVM page
 * @return     returns true on successful operation else returns false
 */

bool NVM_PageErase( uint32_t address );

/**
 * @ingroup    nvm
 * @brief      Gets the NVM error encountered if any
 * @return     NVM_ERROR enum
 */

NVM_ERROR NVM_ErrorGet( void );

/**
 * @ingroup    nvm
 * @brief      Checks whether the NVM module is busy(operation in progress) or not
 * @return     returns true if NVM operation is on progress
 */
bool NVM_IsBusy( void );

/**
 * @ingroup    nvm
 * @brief      This function allows application to register an event handling
 *             function for the PLIB to call back when external interrupt occurs.
 *             At any point if application wants to stop the callback,
 *             it can call this function with "callback" value as NULL.
 * @param[in]  callback  - Pointer to the event handler function implemented by the user
 * @param[in]  context   - The value of parameter will be passed back to the
 *                         application unchanged, when the eventHandler function is called.
 *                         It can be used to identify any application specific value.
 */
void NVM_CallbackRegister( NVM_CALLBACK callback, uintptr_t context );

// DOM-IGNORE-BEGIN
#ifdef __cplusplus // Provide C++ Compatibility

    }

#endif

// DOM-IGNORE-END
#endif // PLIB_NVM_H
