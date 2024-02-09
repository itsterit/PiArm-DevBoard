#ifndef _CLOCK_CONTROL_H_
#define _CLOCK_CONTROL_H_
#include <stm32f103xb.h>

class clock_control
{
public:
    typedef enum
    {
        HSI_SELECTED_AS_SYSTEM_CLOCK = 0b00,
        HSE_SELECTED_AS_SYSTEM_CLOCK = 0b01,
        PLL_SELECTED_AS_SYSTEM_CLOCK = 0b10,
    } SYSTEM_CLOCK_SOURCE_Type;

    static bool clock_switch(SYSTEM_CLOCK_SOURCE_Type system_clock_source);

    class hse
    {
    public:
        static bool enable(bool status);
        static bool clock_ready(void);
        static bool clock_bypass_mode(bool status);
        static bool enable_security_system(bool status);
    };
};

#endif /* _CLOCK_CONTROL_H_ */
