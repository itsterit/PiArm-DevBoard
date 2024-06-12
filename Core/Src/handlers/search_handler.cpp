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
signal main_signal, search_signal;
uint16_t new_signal = 0;

void search_function()
{
    uint8_t arr_amt = (sizeof(main_signal.signal) / sizeof(main_signal.signal[0]));
    uint8_t arr_size = (sizeof(main_signal.signal));

    if (main_signal.signal_point_amt < (arr_amt * 4))
    {
        if (new_signal)
        {
            push_fun(&main_signal.signal[0], arr_size, new_signal);
            main_signal.signal_point_amt++;
            new_signal = 0;
            return;
        }
        TIM4->CCR4 = 0;
    }
    else
    {
        if (search_signal.signal_point_amt < arr_amt)
        {
            if (new_signal)
            {
                search_signal.signal[search_signal.signal_point_amt++] = new_signal;
                new_signal = 0;
            }
        }
        else
        {
            push_fun(&search_signal.signal[0], arr_size, new_signal);

            uint32_t main = filter((uint16_t *)&main_signal.signal[0], arr_amt);
            uint32_t search = filter((uint16_t *)&search_signal.signal[0], arr_amt);

            int signal_val = ABS_DIFF(main, search);
            usInputRegisters[INPUT_SEARCH_VALUE] = signal_val;

            if (signal_val > usHoldingRegisters[HOLDING_SENSITIVITY])
            {
                uint32_t signal = signal_val * 2;
                uint32_t freq = (BASE_FREQ > signal)
                                    ? (((BASE_FREQ - signal) < MIN_FREQ) ? MIN_FREQ : (BASE_FREQ - signal))
                                    : (MIN_FREQ);

                uint32_t timer_arr = TIMER_FREQ / freq;
                TIM4->CCR4 = (timer_arr / 100) * usHoldingRegisters[HOLDING_VOLUME];
                TIM4->ARR = timer_arr;
            }
            else
            {
                if (signal_val < usHoldingRegisters[HOLDING_SENSITIVITY] - 1)
                    TIM4->CCR4 = 0;
            }
        }
    }
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
