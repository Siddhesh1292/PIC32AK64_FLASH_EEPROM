#include "driver_UART.h"
#include "device.h"
//#include "VCU_CANdata.h"

static unsigned char devsn_string[10] = {0x20, 0,0,0,0, 0,0,0,0, 0x2F};
static unsigned char codesn_string[10] = {0x30, 0,0,0,0, 0,0,0,0, 0x3F};
static unsigned char stat_str[] = {0x10,0x02,0x1F};

static uint32_t codesn0 = 0;
static uint32_t codesn1 = 0;
volatile static uint8_t codekey1 = 1, codekey0 = 0;

#define CODESNH 0x000E000E  //APT 4kW
#define CODESNL 0x000C000C  //12.__
#define CODEKEY1 13
#define CODEKEY0 7


int code_authentication(void)
{
    volatile uint32_t codesn1x = 0;
    
    codesn0 = CODESNL;
    codesn0 = codesn0 - codekey0; 
    codesn0 = codesn0 / codekey1;    

    codesn1 = CODESNH;
    codesn1 = codesn1 - codekey0; 
    codesn1 = codesn1 / codekey1;      

//    VCU_codeV(codesn0, codesn1);    
    
    codesn1x = codesn1;
    codesn1x = (codesn1x >> 16);
    codesn1x = codesn1x & 0x0000FFFF;
    
    //if(codesn1x == 2)  
    return 1; 
    //code verified IGT = 2;
    //else return 0; //invalid code    
}


void CODESN_send(void)
{
    __builtin_memcpy((uint8_t *)(codesn_string + 1), &codesn1, 4);
    __builtin_memcpy((uint8_t *)(codesn_string + 5), &codesn0, 4);
    
    Nop();
    Nop();
    send_data_uart(codesn_string, 10);
}

void DEVSN_send(void)
{
    uint32_t mydevsnL;
    uint32_t mydevsnH;
    mydevsnL    = (uint32_t)DEVSN0;
    mydevsnH    = (uint32_t)DEVSN1;
 __builtin_memcpy((uint8_t *)(devsn_string + 1), &mydevsnH, 4);
 __builtin_memcpy((uint8_t *)(devsn_string + 5), &mydevsnL, 4);
 send_data_uart(devsn_string, 10);
}

void device_state_send(void)
{
    send_data_uart(stat_str,3);
}


 //*/