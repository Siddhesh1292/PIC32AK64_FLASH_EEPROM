


#ifndef DRIVER_UART_H
#define	DRIVER_UART_H

#ifdef	__cplusplus
extern "C" {
#endif

#include "stdint.h"
    
void send_byte_uart(void);      // send bytes one by one till no. of bytes is zero

void recieve_command_uart(void); //receive bytes and create command

void send_data_uart(uint8_t *, int); //start address of data to be sent, no. of bytes

#ifdef	__cplusplus
}
#endif

#endif	/* DRIVER_UART_H */

