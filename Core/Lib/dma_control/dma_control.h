#ifndef __DMACONTROL_DMACONTROL_H__
#define __DMACONTROL_DMACONTROL_H__
#include <stdint.h>
#include <stm32f103xb.h>

typedef enum
{
    MEM2MEM_Disabled = 0b0,
    MEM2MEM_Enabled = 0b1
} eMEM2MEM_MODE;

typedef enum
{
    PL_Low = 0b00,
    PL_Medium = 0b01,
    PL_High = 0b10,
    PL_VeryHigh = 0b11
} eCHANNEL_PRIORITY_LEVEL;

typedef enum
{
    MSIZE_8bits = 0b00,
    MSIZE_16bits = 0b01,
    MSIZE_32bits = 0b10,
    MSIZE_Reserved = 0b11
} eMSIZE;

typedef enum
{
    PSIZE_8bits = 0b00,
    PSIZE_16bits = 0b01,
    PSIZE_32bits = 0b10,
    PSIZE_Reserved = 0b11
} ePSIZE;

typedef enum
{
    MINC_Disabled = 0b0,
    MINC_Enabled = 0b1
} eMEMORY_INCREMENT_MODE;

typedef enum
{
    PINC_Disabled = 0b0,
    PINC_Enabled = 0b1
} ePERIPHERAL_INCREMENT_MODE;

typedef enum
{
    CIRC_Disabled = 0b0,
    CIRC_Enabled = 0b1
} eCIRCULAR_MODE;

typedef enum
{
    Read_From_Peripheral = 0b0,
    Read_From_Memory = 0b1
} eDATA_TRANSFER_DIRECTION;

typedef enum
{
    TEIE_Disabled = 0b0,
    TEIE_Enabled = 0b1
} eTRANSFER_ERROR_INTERRUPT_ENABLE;

typedef enum
{
    HTIE_Disabled = 0b0,
    HTIE_Enabled = 0b1
} eHALF_TRANSFER_INTERRUPT_ENABLE;

typedef enum
{
    TCIE_Disabled = 0b0,
    TCIE_Enabled = 0b1
} eTRANSFER_COMPLETE_INTERRUPT_ENABLE;

class dma_control
{
private:
    DMA_TypeDef *DMA_num = 0;
    DMA_Channel_TypeDef *DMA_channel = 0;

public:
    dma_control(DMA_TypeDef *DMA_num, DMA_Channel_TypeDef *DMA_channel);
    void dma_set_config(eMEM2MEM_MODE memory_to_memory_mode, eCHANNEL_PRIORITY_LEVEL channel_priority_level,
                        eMSIZE memory_size, ePSIZE peripheral_size, eMEMORY_INCREMENT_MODE memory_increment_mode,
                        ePERIPHERAL_INCREMENT_MODE peripheral_increment_mode, eCIRCULAR_MODE circular_mode,
                        eDATA_TRANSFER_DIRECTION data_transfer_direction,
                        eTRANSFER_ERROR_INTERRUPT_ENABLE transfer_error_interrupt_enable,
                        eHALF_TRANSFER_INTERRUPT_ENABLE half_transfer_interrupt_enable,
                        eTRANSFER_COMPLETE_INTERRUPT_ENABLE transfer_complete_interrupt_enable);
    void dma_start(uint16_t counter, uint32_t *data_pointer, uint32_t *peripheral_pointer);
};

#endif /* __DMACONTROL_DMACONTROL_H__ */
