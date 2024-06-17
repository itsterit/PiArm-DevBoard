#include <main.h>
#define MAX(x, y) ((x) > (y) ? (x) : (y))
#define ABS_DIFF(x, y) ((x) > (y) ? ((x) - (y)) : ((y) - (x)))
#define MIN(x, y) ((x) < (y) ? (x) : (y))

#define TIMER_FREQ 1000000
#define BASE_FREQ 2000
#define MIN_FREQ 1000

// Вспомогательные функции
uint32_t filter(uint16_t *array, uint8_t size_amt);
void push_fun(uint16_t *arr_ptr, uint16_t arr_size, uint16_t new_val);

// Вспомогательные переменные
uint16_t new_signal = 0;
uint8_t arr_amt = (sizeof(main_signal.signal) / sizeof(main_signal.signal[0]));
uint8_t arr_size = (sizeof(main_signal.signal));
uint32_t main_val;
uint32_t search_val;
int signal_val;

signal main_signal, search_signal, buzz_signal;
struct timer_flag
{
    bool timer_update_flag;
    bool timer_compare_flag;
} timings;

void search_function()
{
    if (main_signal.signal_point_amt < (arr_amt * 16))
    {
        TIM4->CCR4 = 0;
        if (new_signal)
        {
            // Заполнить массивы
            push_fun(&main_signal.signal[0], arr_size, new_signal);
            push_fun(&search_signal.signal[0], arr_size, new_signal);
            push_fun(&buzz_signal.signal[0], arr_size, new_signal);
            main_signal.signal_point_amt++;
            search_signal.signal_point_amt++;
            buzz_signal.signal_point_amt++;
            // Обнулить переменные
            new_signal = 0;
            timings.timer_compare_flag = 0;
            timings.timer_update_flag = 0;
        }
    }
    else
    {
        uint32_t signal = 0;

        if (new_signal)
        {
            push_fun(&search_signal.signal[0], arr_size, new_signal);
            search_val = filter((uint16_t *)&search_signal.signal[0], arr_amt);

            signal = (ABS_DIFF(buzz_signal.signal[0], new_signal)) * 50;

            new_signal = 0;
        }
        main_val = filter((uint16_t *)&main_signal.signal[0], arr_amt);
        signal_val = ABS_DIFF(main_val, search_val);
        usInputRegisters[INPUT_SEARCH_VALUE] = signal_val;

        if (signal_val > usHoldingRegisters[HOLDING_SENSITIVITY] && timings.timer_update_flag == 0)
        {

            uint32_t freq = (BASE_FREQ > signal)
                                ? (((BASE_FREQ - signal) < MIN_FREQ) ? MIN_FREQ : (BASE_FREQ - signal))
                                : (MIN_FREQ);

            uint32_t timer_arr = TIMER_FREQ / freq;
            TIM4->CCR4 = (timer_arr / 100) * usHoldingRegisters[HOLDING_VOLUME];
            TIM4->ARR = timer_arr;

            if (!usHoldingRegisters[HOLDING_PIN_POINT_MODE])
            {
                timings.timer_compare_flag = 1;
                timings.timer_update_flag = 1;
                TIM1->CR1 |= TIM_CR1_CEN_Msk;
            }
        }
        else
        {
            if ((signal_val < (usHoldingRegisters[HOLDING_SENSITIVITY] - 2)))
            {
                TIM4->CCR4 = 0;
            }
            else if (timings.timer_compare_flag == 0)
            {
                TIM4->CCR4 = 0;
            }
        }
    }
}

extern "C" void TIM1_CC_IRQHandler(void)
{
    TIM1->SR = ~TIM1->SR;
    timings.timer_compare_flag = 0;
}

extern "C" void TIM1_UP_IRQHandler(void)
{
    timings.timer_update_flag = 0;
    TIM1->SR = ~TIM1->SR;
    memcpy(&main_signal.signal[0], &search_signal.signal[0], arr_size);
}

void push_fun(uint16_t *arr_ptr, uint16_t arr_size, uint16_t new_val)
{
    uint16_t add_arr[99];
    add_arr[0] = new_val;
    memcpy(&add_arr[1], arr_ptr, arr_size);
    memcpy(arr_ptr, &add_arr[0], arr_size);
}

uint32_t filter(uint16_t *array, uint8_t size_amt)
{
    uint64_t sum = 0;
    for (uint8_t i = 0; i < size_amt; i++)
        sum += array[i];

    return (sum / size_amt);
}
