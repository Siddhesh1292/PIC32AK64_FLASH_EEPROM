#include <stddef.h>                     // Defines NULL
#include <stdbool.h>                    // Defines true
#include <stdlib.h>                     // Defines EXIT_FAILURE
#include "definitions.h"                // SYS function prototypes
#include "driver/flash_eeprom.h"


int main ( void )
{
    SYS_Initialize ( NULL );
    
    FLASH_EEPROM_Init();
    
    TMR1_Start();
    
    while ( true )
    {

    }


    return ( EXIT_FAILURE );
}


