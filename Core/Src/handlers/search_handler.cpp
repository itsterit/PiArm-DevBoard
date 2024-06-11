#include <main.h>
#define MAX(x, y) ((x) > (y) ? (x) : (y))
#define ABS_DIFF(x, y) ((x) > (y) ? ((x) - (y)) : ((y) - (x)))
#define MIN(x, y) ((x) < (y) ? (x) : (y))

// Вспомогательные функции
int median_filter(uint16_t a, uint16_t b, uint16_t c);
void push_fun(uint16_t *arr_ptr, uint16_t arr_size, uint16_t new_val);

// Вспомогательные переменные
signal main_signal, search_signal;

void search_function()
{
    uint8_t arr_amt = (sizeof(main_signal.signal) / sizeof(main_signal.signal[0]));
    uint8_t arr_size = (sizeof(main_signal.signal));

    if (main_signal.signal_point_amt < (arr_amt * 2))
    {
        if (search_signal.signal[3])
        {
            push_fun(&main_signal.signal[0], arr_size, search_signal.signal[3]);
            main_signal.signal_point_amt++;
            search_signal.signal[3] = 0;
            return;
        }
        TIM4->CCR4 = 0;
    }
    else
    {
        if (search_signal.signal_point_amt < arr_amt)
        {
            if (search_signal.signal[3])
            {
                search_signal.signal[search_signal.signal_point_amt++] = search_signal.signal[3];
                search_signal.signal[3] = 0;
            }
        }
        else
        {
            push_fun(&search_signal.signal[0], arr_size, search_signal.signal[3]);

            int main = median_filter(main_signal.signal[0], main_signal.signal[1], main_signal.signal[2]);
            int search = median_filter(search_signal.signal[0], search_signal.signal[1], search_signal.signal[2]);
            int signal_val = ABS_DIFF(main, search);

            if (signal_val > usHoldingRegisters[HOLDING_SENSITIVITY])
            {
                TIM4->CCR4 = usHoldingRegisters[HOLDING_VOLUME];
                TIM4->ARR = signal_val * 10;
            }
            else
            {
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

int median_filter(uint16_t a, uint16_t b, uint16_t c)
{
    int middle = 0;

    if ((MAX(a, b) == MAX(b, c)))
        middle = MAX(a, c);
    else
        middle = MAX(b, MIN(a, c));
    return middle;
}
