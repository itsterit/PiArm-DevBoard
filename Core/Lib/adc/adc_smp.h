#ifndef __ADC_SMP__
#define __ADC_SMP__

typedef enum
{
    SMP_1_5_cycles   = 0b000,
    SMP_7_5_cycles   = 0b001,
    SMP_13_5_cycles  = 0b010,
    SMP_28_5_cycles  = 0b011,
    SMP_41_5_cycles  = 0b100,
    SMP_55_5_cycles  = 0b101,
    SMP_71_5_cycles  = 0b110,
    SMP_239_5_cycles = 0b111,
} ADC_SAMPLING_CONFIG_Type;

#endif /* __ADC_SMP__ */