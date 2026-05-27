#include "driver_UART.h"
#include "device_stat.h"
#include "tuning.h"
#include "config/default/peripheral/uart/plib_uart1.h"
#include "device.h"
#include "stdio.h"
#include "string.h"
#include "diagnostics.h"

static int num_of_bytes = 0;
static uint8_t* byte_adr;

void send_data_uart(uint8_t * string_adr, int data_size)
{
    byte_adr = string_adr;          
    num_of_bytes = data_size;
}


void recieve_command_uart(void)
{
    static uint8_t recieved_byte;
    static int command_byte_num = 0;
    static int Utime = 0; 
    static uint8_t uart_command[20] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};        //20 is the maximum command size (20 required for tune command)
    
    if(command_byte_num == 0) memset(&uart_command[0], 0x00, 20);                       //clear the command array
    
    if(UART1_ReceiverIsReady())
    {
        Nop();
        recieved_byte = (uint8_t)UART1_ReadByte();
        Nop();
        
        if((command_byte_num < 20) & (command_byte_num >= 0))
        {            
            switch(command_byte_num)
            {
                case 0 :    //byte0 decides command type
                    switch(recieved_byte)
                    {
                        case 0xFF: //tuning command
                            uart_command[0] = recieved_byte;
                            command_byte_num = command_byte_num + 1;
                        break;
                        
                        case 0x10: //Statues Command
                            uart_command[0] = recieved_byte;
                            command_byte_num = command_byte_num + 1;
                        break;
                        
                        case 0x20: //PICID Command
                            uart_command[0] = recieved_byte;
                            command_byte_num = command_byte_num + 1;
                        break;
                        
                        case 0x30: //CODEID Command
                            uart_command[0] = recieved_byte;
                            command_byte_num = command_byte_num + 1;
                        break;
                        
                        case 0x80: //diagnostic command
                            uart_command[0] = recieved_byte;
                            command_byte_num = command_byte_num + 1;
                        break;

                        default:
                            command_byte_num = 0;   //unrecognized command, clear the byte and await next command
                        break;
                    }
                break;
                
                case 2:
                    uart_command[command_byte_num] = recieved_byte;
                    command_byte_num = command_byte_num + 1;
                    
                    Nop();
                    if(uart_command[0] == 0x10)
                    {
                        if(uart_command[2] == 0x1F)
                        {
                            device_state_send();
                            Nop();
                            Nop();
                        }
                        command_byte_num = 0;       //await next command
                    }
                    else if(uart_command[0] == 0x20)
                    {
                        if(uart_command[2] == 0x2F)
                        {
                            DEVSN_send();
                            Nop();
                            Nop();
                        }
                        command_byte_num = 0;       //await next command
                    }
                    else if(uart_command[0] == 0x30)
                    {
                        if(uart_command[2] == 0x3F)
                        {
                            CODESN_send();
                            Nop();
                            Nop();
                        }
                        command_byte_num = 0;       //await next command
                    }
                    else if(uart_command[0] == 0x80)
                    {
                        if(uart_command[2] == 0x8F)
                        {
                            send_diag_data();
                            Nop();
                            Nop();
                        }
                        command_byte_num = 0;       //await next command
                    }
                    
                break;
                
                case 19: //byte 19 received = complete tuning command received
                    uart_command[command_byte_num] = recieved_byte;                        
                    tune_command(&uart_command[0]);                //call tuning module     
                    command_byte_num = 0;       //await next command
                   
                break;

                default:    //keep on adding bytes to command
                    uart_command[command_byte_num] = recieved_byte;
                    command_byte_num = command_byte_num + 1;
                break;
            }
        }
        else command_byte_num = 0; 
    }
    
    if(command_byte_num > 0)                                             
    {
        Utime++;
        if(Utime > 400) //timeout for any command must send all command bytes within this time
        {
            if(command_byte_num >= 2)
            {
                tune_command(&uart_command[0]);                
            }
            command_byte_num = 0;
            Utime = 0;           
        }
    }
    else    Utime = 0;
}


void send_byte_uart(void)
{
    if(num_of_bytes > 0)                       
    {
        if(UART1_TransmitterIsReady())
        {
            UART1_WriteByte(*byte_adr);
            byte_adr = byte_adr + 1;
            num_of_bytes = num_of_bytes - 1;            
        }
    }    
}
