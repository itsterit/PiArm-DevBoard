#include <main.h>
bool writeSector(uint32_t Address, void *values, uint16_t size);
// void eraseSector(uint32_t SectorStartAddress)

/**
 * @brief   Отправка ответа мастер устройству по modbus
 * @note    По идее нт защит от перезаписи :D
 */
void ModBusTxCallback(uint8_t *DataPtr, int16_t DataSize)
{
    usb_as_dma_transmit((uint8_t *)&DataPtr[0], DataSize);
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

// void eraseSector(uint32_t SectorStartAddress)
// {
//     FLASH->KEYR = 0x45670123;
//     FLASH->KEYR = 0xCDEF89AB;
//     FLASH->CR &= ~BIT0; // Ensure PG bit is low
//     FLASH->CR |= BIT1;  // set the PER bit
//     FLASH->AR = SectorStartAddress;
//     FLASH->CR |= BIT6; // set the start bit
//     while (FLASH->SR & BIT0)
//         ; // wait while busy
// }