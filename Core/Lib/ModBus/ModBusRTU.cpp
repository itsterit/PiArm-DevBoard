#include "ModBusRTU.h"
#include "mbcrc/mbcrc.h"

/**
 * @brief                           Конструктор класса ModBusRTU
 *
 * @param[in] ModBusTxCallback      Callback функция вида: void ModBusTxCallback(uint8_t *Dataptr, uint32_t DataSize),
 *                                  используемая для отправки modbus ответа.
 * @param[in] InputRegistersPtr     Указатель на область выходных регистров.
 * @param[in] HoldingRegistersPtr   Указатель на область регистров хранения.
 */
ModBusRTU::ModBusRTU(void (*ModBusTxCallback)(uint8_t *DataPtr, int16_t DataSize),
                     bool (*ModBusWriteCallback)(uint8_t *DataPtr, uint32_t DataSize, uint32_t WriteOffset, uint16_t FileType),
                     uint16_t *InputRegistersPtr, uint16_t *HoldingRegistersPtr)
{
    ModBusTxCallback_ = ModBusTxCallback;
    ModBusWriteCallback_ = ModBusWriteCallback;
    HandlerInputRegistersPtr = InputRegistersPtr;
    HandlerHoldingRegistersPtr = HoldingRegistersPtr;
}
ModBusRTU::ModBusRTU(void (*ModBusTxCallback)(uint8_t *DataPtr, int16_t DataSize),
                     bool (*ModBusSaveCallback)(void),
                     uint16_t *InputRegistersPtr, uint16_t *HoldingRegistersPtr)
{
    ModBusTxCallback_ = ModBusTxCallback;
    ModBusSaveCallback_ = ModBusSaveCallback;
    HandlerInputRegistersPtr = InputRegistersPtr;
    HandlerHoldingRegistersPtr = HoldingRegistersPtr;
}
ModBusRTU::ModBusRTU(void (*ModBusTxCallback)(uint8_t *DataPtr, int16_t DataSize),
                     uint16_t *InputRegistersPtr, uint16_t *HoldingRegistersPtr)
{
    ModBusTxCallback_ = ModBusTxCallback;
    HandlerInputRegistersPtr = InputRegistersPtr;
    HandlerHoldingRegistersPtr = HoldingRegistersPtr;
}

/**
 * @brief                    Обработчик принятого фрейма
 *
 * @param[in]  DataPtr       Указатель на начало массива принятых данных
 * @param[in]  DataSize      Указатель размера массива принятых данных
 * @param[in]  ModbusAddress Адрес устройства
 * @param[out] bool          Вернет 1 в случае если команда корректна и предназначена нам
 */
bool ModBusRTU::FrameHandler(uint8_t *DataPtr, int16_t DataSize, uint16_t ModbusAddress, int16_t BufferSize)
{
    if ((DataSize > 3) && (BufferSize > 5))
    {
        DataPtr_ = &DataPtr[0];
        DataSize_ = DataSize;
        BufferSize_ = BufferSize;

        if (MbCrcCheck(DataPtr, DataSize) && ((DataPtr[0] == ModbusAddress) || (DataPtr[0] == 0)))
        {
            HandlerMbDataLocation = (DataPtr_[3] | (DataPtr_[2] << 8));
            HandlerMbDataAmount = (DataPtr_[5] | (DataPtr_[4] << 8));

            switch (DataPtr_[1])
            {
            case 0x03:
                if (DataSize_ != 8)
                    return 0;
                else
                    ReadHoldingFunc();
                break;

            case 0x04:
                if (DataSize_ != 8)
                    return 0;
                else
                    ReadInputFunc();
                break;

            case 0x10:
                if (WriteHoldingMultFunc())
                    break;

            case 0x06:
                if (WriteHoldingSingleFunc())
                    break;

            default:
                // Принятый код функции не может быть обработан.
                DataPtr_[1] |= 0x80;
                DataPtr_[2] = 0x01;
                DataSize_ = 3;
                break;
            }

            MbInsertCRC((uint8_t *)DataPtr_, DataSize_);
            ModBusTxCallback_((uint8_t *)DataPtr_, DataSize_ + 2);
            return 1;
        }
    }
    return 0;
}

/*-------------------- 0x06 --------------------*/
bool ModBusRTU::WriteHoldingSingleFunc(void)
{
    if (HandlerMbDataLocation <= (BufferSize_ / 2))
    {
        unsigned char HandlerTmpChar = 4;
        uint16_t HandlerTmpShort = 0;
        DataSize_ = 6;

        HandlerTmpShort = (DataPtr_[HandlerTmpChar] << 8) | DataPtr_[HandlerTmpChar + 1];
        HandlerHoldingRegistersPtr[HandlerMbDataLocation] = HandlerTmpShort;
        if (*ModBusSaveCallback_ != 0)
            return ModBusSaveCallback_();
    }
    else
    {
        DataPtr_[1] |= 0x80;
        DataPtr_[2] = 0x02;
        DataSize_ = 3;
    }
    return 1;
}

/*-------------------- 0x10 --------------------*/
bool ModBusRTU::WriteHoldingMultFunc(void)
{
    if (DataSize_ >= (7 + DataPtr_[6] + 2))
    {
        if ((HandlerMbDataLocation + HandlerMbDataAmount) <= (BufferSize_ / 2))
        {
            /* Начало области с данными */
            unsigned char HandlerTmpChar = 7;
            uint16_t HandlerTmpShort = 0;
            while (HandlerMbDataAmount--)
            {
                HandlerTmpShort = (DataPtr_[HandlerTmpChar] << 8) | DataPtr_[HandlerTmpChar + 1];
                HandlerHoldingRegistersPtr[HandlerMbDataLocation] = HandlerTmpShort;
                HandlerMbDataLocation++;
                HandlerTmpChar += 2;
            }
            DataSize_ = 6;
            if (*ModBusSaveCallback_ != 0)
                return ModBusSaveCallback_();
        }
        else
        {
            DataPtr_[1] |= 0x80;
            DataPtr_[2] = 0x02;
            DataSize_ = 3;
        }
    }
    else
    {
        DataPtr_[1] |= 0x80;
        DataPtr_[2] = 0x05;
        DataSize_ = 3;
    }
    return 1;
}

/*-------------------- 0x04 --------------------*/
void ModBusRTU::ReadInputFunc(void)
{
    if (HandlerMbDataLocation <= (MB_INPUT_ADR_MAX))
    {
        if ((HandlerMbDataAmount != 0) && ((HandlerMbDataLocation + HandlerMbDataAmount) <= ((BufferSize_ / 2) - 5)))
        {
            /* Кол-во регистров в ответе */
            DataPtr_[2] = HandlerMbDataAmount * 2;
            /* Начало данных */
            uint8_t *HandlerTmpPtr = (uint8_t *)&DataPtr_[3];
            uint16_t HandlerTmpShort = 0;
            /* Длина ответа */
            DataSize_ = DataPtr_[2] + 3;

            while (HandlerMbDataAmount--)
            {
                HandlerTmpShort = HandlerInputRegistersPtr[HandlerMbDataLocation];
                /* HI часть ответа*/
                *HandlerTmpPtr = (unsigned char)(HandlerTmpShort >> 8);
                HandlerTmpPtr++;
                /* LOW часть ответа, так как tmp_ptr указывает на uint_8t */
                *HandlerTmpPtr = (unsigned char)HandlerTmpShort;
                HandlerTmpPtr++;
                HandlerMbDataLocation++;
            }
        }
        else
        {
            DataPtr_[1] |= 0x80;
            DataPtr_[2] = 0x03;
            DataSize_ = 3;
        }
    }
    else
    {
        DataPtr_[1] |= 0x80;
        DataPtr_[2] = 0x02;
        DataSize_ = 3;
    }
}

/*-------------------- 0x03 --------------------*/
void ModBusRTU::ReadHoldingFunc(void)
{
    if (HandlerMbDataLocation <= MB_HOLDING_ADR_MAX)
    {
        if ((HandlerMbDataAmount != 0) && ((HandlerMbDataLocation + HandlerMbDataAmount) <= ((BufferSize_ / 2) - 5)))
        {
            /* Кол-во регистров в ответе */
            DataPtr_[2] = HandlerMbDataAmount * 2;
            /* Начало данных */
            uint8_t *HandlerTmpPtr = (uint8_t *)&DataPtr_[3];
            uint16_t HandlerTmpShort = 0;
            /* Длина ответа */
            DataSize_ = DataPtr_[2] + 3;

            while (HandlerMbDataAmount--)
            {
                HandlerTmpShort = HandlerHoldingRegistersPtr[HandlerMbDataLocation];
                /* HI часть ответа*/
                *HandlerTmpPtr = (unsigned char)(HandlerTmpShort >> 8);
                HandlerTmpPtr++;
                /* LOW часть ответа, так как tmp_ptr указывает на uint_8t */
                *HandlerTmpPtr = (unsigned char)HandlerTmpShort;
                HandlerTmpPtr++;
                HandlerMbDataLocation++;
            }
        }
        else
        {
            DataPtr_[1] |= 0x80;
            DataPtr_[2] = 0x03;
            DataSize_ = 3;
        }
    }
    else
    {
        DataPtr_[1] |= 0x80;
        DataPtr_[2] = 0x02;
        DataSize_ = 3;
    }
}
