#include <main.h>

void ModBusTxCallback(uint8_t *DataPtr, int16_t DataSize)
{
    usb_as_dma_transmit((uint8_t *)&DataPtr[0], DataSize);
}