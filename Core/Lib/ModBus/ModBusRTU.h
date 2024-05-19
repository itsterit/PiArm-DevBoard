#ifndef __MODBUS_RTU_H__
#define __MODBUS_RTU_H__
#include <stdint.h>

#define MB_INPUT_ADR_MAX    (4000)
#define MB_HOLDING_ADR_MAX  (10)

class ModBusRTU
{
private:
    void (*ModBusTxCallback_)(uint8_t *DataPtr, int16_t DataSize) = 0;
    bool (*ModBusSaveCallback_)(void) = 0;
    bool (*ModBusWriteCallback_)(uint8_t *DataPtr, uint32_t DataSize, uint32_t WriteOffset, uint16_t FileType) = 0;
    volatile uint8_t *DataPtr_;
    volatile int16_t DataSize_;
    volatile int16_t BufferSize_;
    volatile uint16_t *HandlerInputRegistersPtr;
    volatile uint16_t *HandlerHoldingRegistersPtr;
    volatile uint16_t HandlerMbDataLocation;
    volatile uint16_t HandlerMbDataAmount;
    void ReadHoldingFunc(void);        // 0x03
    void ReadInputFunc(void);          // 0x04
    bool WriteHoldingSingleFunc(void); // 0x06
    bool WriteHoldingMultFunc(void);   // 0x10

public:
    ModBusRTU(void (*)(uint8_t *, int16_t), // ModBusTxCallback_
              bool (*)(void),               // ModBusSaveCallback_
              uint16_t *InputRegistersPtr, uint16_t *HoldingRegistersPtr);

    ModBusRTU(void (*)(uint8_t *, int16_t), // ModBusTxCallback_
              uint16_t *InputRegistersPtr, uint16_t *HoldingRegistersPtr);

    bool FrameHandler(uint8_t *DataPtr, int16_t DataSize, uint16_t ModbusAddress, int16_t BufferSize);
};

#endif /* __MODBUS_RTU_H__ */