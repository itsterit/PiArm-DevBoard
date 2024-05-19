#include <main.h>
#include "ModBus/mbcrc/mbcrc.h"
#include "system_handler.h"
#define DATA_SECTOR_START_ADDRESS (0x800FC00)
#define SET_CONFIG_DELAY (1000)

bool write_sector(uint16_t *address, uint16_t *values, uint16_t size);
bool erase_sector(uint32_t sector_start_address);

/**
 * @brief   Отправка ответа мастер устройству по modbus
 * @note    По идее нт защит от перезаписи :D
 */
void ModBusTxCallback(uint8_t *DataPtr, int16_t DataSize)
{
    usb_as_dma_transmit((uint8_t *)&DataPtr[0], DataSize);
}

/**
 * @brief   Сохранение регистров в память контроллера
 * @note    Для проверки целостности будем считать crc уставок
 */
bool ModBusSaveCallback(void)
{
    __disable_irq();
    STOP_GENERATION;
    cur_fault_delay = SET_CONFIG_DELAY;

    usHoldingRegisters[HOLDING_REGISTER_DATA_CRC] = MbCrcCalculate((uint8_t *)&usHoldingRegisters[0], sizeof(usHoldingRegisters) - 2);
    if (erase_sector(DATA_SECTOR_START_ADDRESS) && write_sector((uint16_t *)DATA_SECTOR_START_ADDRESS, &usHoldingRegisters[0], sizeof(usHoldingRegisters)))
    {
        __enable_irq();
        return true;
    }
    __enable_irq();
    return false;
}

/**
 * @brief   Проверка сохраненных данных
 * @note    Если данные некорректные - пишем базовые.
 */
void check_system_parameters()
{
    uint32_t data_adr = DATA_SECTOR_START_ADDRESS;
    for (uint8_t counter = 0; counter < (sizeof(usHoldingRegisters) / sizeof(usHoldingRegisters[0])); counter++)
    {
        usHoldingRegisters[counter] = (*(uint16_t *)data_adr);
        data_adr += 2;
    }

    if (usHoldingRegisters[HOLDING_REGISTER_DATA_CRC] == MbCrcCalculate((uint8_t *)&usHoldingRegisters[0], sizeof(usHoldingRegisters) - 2))
    {
        // Частота ШИМ катушки
        if (usHoldingRegisters[HOLDING_COIL_FREQUENCY] > COIL_FREQUENCY__MAX ||
            usHoldingRegisters[HOLDING_COIL_FREQUENCY] < COIL_FREQUENCY__MIN)
        {
            usHoldingRegisters[HOLDING_COIL_FREQUENCY] = BASE_COIL_FREQUENCY;
        }
        // ШИМ заполнение катушки
        if (usHoldingRegisters[HOLDING_COIL_DUTY] > COIL_DUTY__MAX ||
            usHoldingRegisters[HOLDING_COIL_DUTY] < COIL_DUTY__MIN)
        {
            usHoldingRegisters[HOLDING_COIL_DUTY] = BASE_COIL_DUTY;
        }
        // Громкость динамика
        if (usHoldingRegisters[HOLDING_VOLUME] > VOLUME__MAX ||
            usHoldingRegisters[HOLDING_VOLUME] < VOLUME__MIN)
        {
            usHoldingRegisters[HOLDING_VOLUME] = BASE_VOLUME;
        }
        return;
    }
    usHoldingRegisters[HOLDING_COIL_FREQUENCY] = BASE_COIL_FREQUENCY;
    usHoldingRegisters[HOLDING_COIL_DUTY] = BASE_COIL_DUTY;
    usHoldingRegisters[HOLDING_VOLUME] = BASE_VOLUME;
}

/**
 * @brief   Работа со встроенной flash
 */
bool write_sector(uint16_t *address, uint16_t *values, uint16_t size)
{
    size = size / 2;

    while (size)
    {
        FLASH->KEYR = 0x45670123;
        FLASH->KEYR = 0xCDEF89AB;
        FLASH->CR &= ~FLASH_CR_PER;
        FLASH->CR |= FLASH_CR_PG;

        (*(uint16_t *)address) = (*(uint16_t *)values);

        for (uint16_t err_counter = 0; err_counter < 0xFFF; err_counter++)
        {
            if ((!(FLASH->SR & FLASH_SR_BSY)) &&
                (!(FLASH->SR & FLASH_SR_PGERR) && !(FLASH->SR & FLASH_SR_WRPRTERR)))
            {
                goto write_next;
            }
        }
        return false;

    write_next:
        address++;
        values++;
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
