#include "flash_eeprom.h"
#include "definitions.h"
#include "sys/kmem"
#include <string.h>

typedef struct {
    uint32_t marker;
    uint32_t sequence;
    uint32_t crc;
    uint32_t length;
    uint32_t reserved;
} EEPROM_HEADER;

static const uint32_t pages[EEPROM_NUM_PAGES] = {
    EEPROM_PAGE1_ADDR,
    EEPROM_PAGE2_ADDR,
    EEPROM_PAGE3_ADDR
};

static uint32_t activePage = 0;

static uint32_t calculate_crc(uint8_t *data, uint32_t len) {
    uint32_t crc = 0;

    for (uint32_t i = 0; i < len; i++) {
        crc ^= data[i];

        for (uint8_t j = 0; j < 8; j++) {
            if (crc & 1)
                crc = (crc >> 1) ^ 0xEDB88320;
            else
                crc >>= 1;
        }
    }

    return crc;
}

static bool flash_wait_complete(void) {
    while (NVM_IsBusy());

    if (NVM_ErrorGet())
        return false;

    return true;
}

static bool erase_page(uint32_t addr) {
    NVM_PageErase(addr);

    return flash_wait_complete();
}

static bool write_word(uint32_t addr, uint32_t data) {
    NVM_WordWrite(data, addr);

    return flash_wait_complete();
}

static bool page_is_valid(uint32_t addr, EEPROM_HEADER *header) {
    memcpy(
            header,
            (void*) KVA0_TO_KVA1(addr),
            sizeof (EEPROM_HEADER)
            );

    if (header->marker != EEPROM_VALID_MARKER)
        return false;

    if (header->sequence == 0xFFFFFFFF)
        return false;

    if (header->length > EEPROM_DATA_SIZE)
        return false;

    if (header->reserved != 0xAAAAAAAA)
        return false;

    /* verify actual stored data CRC */
    static uint8_t tempBuffer[EEPROM_DATA_SIZE];

    memcpy(
            tempBuffer,
            (void*) KVA0_TO_KVA1(addr + sizeof (EEPROM_HEADER)),
            header->length
            );

    uint32_t crc = calculate_crc(tempBuffer, header->length);

    if (crc != header->crc)
        return false;

    return true;
}

static uint32_t find_latest_page(void) {
    EEPROM_HEADER hdr;
    uint32_t maxSeq = 0;
    uint32_t latest = 0xFFFFFFFF;

    for (int i = 0; i < EEPROM_NUM_PAGES; i++) {
        if (page_is_valid(pages[i], &hdr)) {
            if (hdr.sequence > maxSeq) {
                maxSeq = hdr.sequence;
                latest = pages[i];
            }
        }
    }

    return latest;
}

FLASH_EEPROM_STATUS FLASH_EEPROM_Init(void)
{
    activePage = find_latest_page();

    if(activePage == 0xFFFFFFFF)
    {
        for(int i=0;i<EEPROM_NUM_PAGES;i++)
        {
            if(!erase_page(pages[i]))
            {
                return FLASH_EEPROM_ERROR;
            }
        }

        activePage = pages[0];   // FIX
    }

    return FLASH_EEPROM_OK;
}

FLASH_EEPROM_STATUS FLASH_EEPROM_Write_Page(uint8_t *data, uint32_t length) {
    if (length > EEPROM_DATA_SIZE)
        return FLASH_EEPROM_ERROR;

    EEPROM_HEADER oldHdr;
    uint32_t nextPage = 0;
    int currentIndex = -1;

    for (int i = 0; i < EEPROM_NUM_PAGES; i++) {
        if (pages[i] == activePage) {
            currentIndex = i;
            break;
        }
    }

    if (currentIndex == -1) {
        nextPage = pages[0];
    } else {
        nextPage = pages[(currentIndex + 1) % EEPROM_NUM_PAGES];
    }

    if (nextPage == 0)
        return FLASH_EEPROM_ERROR;

    if (!erase_page(nextPage))
        return FLASH_EEPROM_ERROR;

    EEPROM_HEADER newHdr;

    if ((activePage != 0xFFFFFFFF) && page_is_valid(activePage, &oldHdr)) {
        newHdr.sequence = oldHdr.sequence + 1;
    } else {
        newHdr.sequence = 1;
    }

    newHdr.marker = EEPROM_VALID_MARKER;
    newHdr.length = length;
    newHdr.crc = calculate_crc(data, length);
    newHdr.reserved = 0xAAAAAAAA;

    uint32_t writeAddr = nextPage + sizeof (EEPROM_HEADER);

    /* Write DATA first */
    for (uint32_t i = 0; i < length; i += 4) {
        uint32_t word = 0xFFFFFFFF;

        memcpy(&word,
                &data[i],
                (length - i) >= 4 ? 4 : (length - i));

        if (!write_word(writeAddr + i, word)) {
            return FLASH_EEPROM_ERROR;
        }
    }

    /* Write HEADER last -> commit page */
    uint32_t *hdrPtr = (uint32_t*) & newHdr;

    /* Write sequence, crc, length, reserved first */
    for (int i = 1; i < sizeof (EEPROM_HEADER) / 4; i++) {
        if (!write_word(nextPage + (i * 4), hdrPtr[i])) {
            return FLASH_EEPROM_ERROR;
        }
    }

    /* Write marker LAST -> final commit */
    if (!write_word(nextPage, EEPROM_VALID_MARKER)) {
        return FLASH_EEPROM_ERROR;
    }

    activePage = nextPage;
    return FLASH_EEPROM_OK;
}

FLASH_EEPROM_STATUS FLASH_EEPROM_Read_Page(uint8_t *data, uint32_t length) {
    EEPROM_HEADER hdr;

    if (!page_is_valid(activePage, &hdr))
        return FLASH_EEPROM_NO_VALID_PAGE;

    if (length > hdr.length)
        length = hdr.length;

    if (!NVM_Read(
            (uint32_t*) data,
            length,
            activePage + sizeof (EEPROM_HEADER)
            )) {
        return FLASH_EEPROM_ERROR;
    }

    uint32_t crc = calculate_crc(data, length);

    if (crc != hdr.crc)
        return FLASH_EEPROM_CRC_ERROR;

    return FLASH_EEPROM_OK;
}

FLASH_EEPROM_STATUS FLASH_EEPROM_WriteWord(uint32_t address, uint32_t data) {
    if (address >= (EEPROM_DATA_SIZE / 4)) {
        return FLASH_EEPROM_ERROR;
    }

    static uint8_t buffer[EEPROM_DATA_SIZE];
    memset(buffer, 0xFF, EEPROM_DATA_SIZE);

    FLASH_EEPROM_STATUS status;

    status = FLASH_EEPROM_Read_Page(buffer, EEPROM_DATA_SIZE);

    if ((status != FLASH_EEPROM_OK) &&
            (status != FLASH_EEPROM_NO_VALID_PAGE)) {
        return status;
    }

    uint32_t *wordBuffer = (uint32_t*) buffer;

    wordBuffer[address] = data;

    return FLASH_EEPROM_Write_Page(buffer, EEPROM_DATA_SIZE);
}

FLASH_EEPROM_STATUS FLASH_EEPROM_ReadWord(uint32_t address, uint32_t *data) {
    if (address >= (EEPROM_DATA_SIZE / 4)) {
        return FLASH_EEPROM_ERROR;
    }

    static uint8_t buffer[EEPROM_DATA_SIZE];

    FLASH_EEPROM_STATUS status;

    status = FLASH_EEPROM_Read_Page(buffer, EEPROM_DATA_SIZE);

    if (status != FLASH_EEPROM_OK) {
        return status;
    }

    uint32_t *wordBuffer = (uint32_t*) buffer;

    *data = wordBuffer[address];

    return FLASH_EEPROM_OK;
}

FLASH_EEPROM_STATUS FLASH_EEPROM_EraseAll(void) {
    for (int i = 0; i < EEPROM_NUM_PAGES; i++) {
        if (!erase_page(pages[i]))
            return FLASH_EEPROM_ERROR;
    }

    activePage = pages[0];

    return FLASH_EEPROM_OK;
}