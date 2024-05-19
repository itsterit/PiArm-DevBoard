#include <main.h>
#define DATA_SECTOR_START_ADDRESS (0x800FC00)

bool writeSector(uint32_t Address, void *values, uint16_t size);
bool erase_sector(uint32_t sector_start_address);

/**
 * @brief   Отправка ответа мастер устройству по modbus
 * @note    По идее нт защит от перезаписи :D
 */
void ModBusTxCallback(uint8_t *DataPtr, int16_t DataSize)
{
    usb_as_dma_transmit((uint8_t *)&DataPtr[0], DataSize);
}

bool ModBusSaveCallback(void)
{
    if (erase_sector(DATA_SECTOR_START_ADDRESS))
    {
        // MB_HOLDING_ADR_MAX

        return true;
    }
    return false;
}

bool writeSector(uint32_t Address, void *values, uint16_t size)
{
    uint16_t *AddressPtr;
    uint16_t *valuePtr;

    AddressPtr = (uint16_t *)Address;
    valuePtr = (uint16_t *)values;
    size = size / 2; // incoming value is expressed in bytes, not 16 bit words

    while (size)
    {
        FLASH->KEYR = 0x45670123;
        FLASH->KEYR = 0xCDEF89AB;
        FLASH->CR &= ~FLASH_CR_PER;
        FLASH->CR |= FLASH_CR_PG;

        *(AddressPtr) = *(valuePtr);

        while (FLASH->SR & FLASH_SR_BSY)
        {
            asm("NOP");
        }

        if (FLASH->SR & FLASH_SR_PGERR)
        {
            return false; // flash not erased to begin with
        }
        if (FLASH->SR & FLASH_SR_WRPRTERR)
        {
            return false; // write protect error
        }
        AddressPtr++;
        valuePtr++;
        size--;
    }

    return true;
}

bool erase_sector(uint32_t sector_start_address)
{
    FLASH->KEYR = 0x45670123;
    FLASH->KEYR = 0xCDEF89AB;

    FLASH->CR &= ~FLASH_CR_PG;
    FLASH->CR |= FLASH_CR_PER;
    FLASH->AR = sector_start_address;
    FLASH->CR |= FLASH_CR_STRT;

    for (uint16_t err_counter = 0; err_counter < 0xFFF; err_counter++)
    {
        if (!(FLASH->SR & FLASH_SR_BSY))
        {
            return 1;
        }
    }
    return 0;
}