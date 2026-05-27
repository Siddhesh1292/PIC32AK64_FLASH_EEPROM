#ifndef FLASH_EEPROM_H
#define FLASH_EEPROM_H

#include <stdint.h>
#include <stdbool.h>

#define EEPROM_DATA_SIZE      4064
#define FLASH_PAGE_SIZE       4096
#define EEPROM_NUM_PAGES      3

// For PIC32AK6416GC41064, reserve the last three 4 KB flash pages.
#define EEPROM_PAGE1_ADDR     0x80D000
#define EEPROM_PAGE2_ADDR     0x80E000
#define EEPROM_PAGE3_ADDR     0x80F000

#define EEPROM_VALID_MARKER   0xFFFFFFFE

typedef enum
{
    FLASH_EEPROM_OK = 0,
    FLASH_EEPROM_ERROR,
    FLASH_EEPROM_CRC_ERROR,
    FLASH_EEPROM_NO_VALID_PAGE
} FLASH_EEPROM_STATUS;


FLASH_EEPROM_STATUS FLASH_EEPROM_Init(void);
FLASH_EEPROM_STATUS FLASH_EEPROM_Write_Page(uint8_t *data, uint32_t length);
FLASH_EEPROM_STATUS FLASH_EEPROM_Read_Page(uint8_t *data, uint32_t length);
FLASH_EEPROM_STATUS FLASH_EEPROM_WriteWord(uint32_t address, uint32_t data);
FLASH_EEPROM_STATUS FLASH_EEPROM_ReadWord(uint32_t address, uint32_t *data);
FLASH_EEPROM_STATUS FLASH_EEPROM_EraseAll(void);

#endif
