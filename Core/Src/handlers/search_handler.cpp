#include <main.h>
#define MAX(x, y) ((x) > (y) ? (x) : (y))
#define ABS_DIFF(x, y) ((x) > (y) ? ((x) - (y)) : ((y) - (x)))
#define MIN(x, y) ((x) < (y) ? (x) : (y))

uint16_t act_coil_current = 0;
int median_filter(uint16_t a, uint16_t b, uint16_t c);
void push_fun(uint16_t *arr_ptr, uint16_t arr_size, uint16_t new_val);
uint16_t mass[80];
double calculate_trapezoidal_area(uint16_t *data, int size);

struct signal
{
    uint16_t signal_point_amt;
    uint16_t signal[50];
} main_signal, search_signal;

void search_function()
{
    if (usInputRegisters[9])
    {
        usInputRegisters[9] = 0;

        if (main_signal.signal_point_amt < (sizeof(main_signal.signal) / sizeof(main_signal.signal[0])))
        {
            main_signal.signal[main_signal.signal_point_amt] = calculate_trapezoidal_area(&usInputRegisters[10], 50);
            main_signal.signal_point_amt++;
        }

        if (search_signal.signal_point_amt < (sizeof(search_signal.signal) / sizeof(search_signal.signal[0])))
        {
            search_signal.signal[search_signal.signal_point_amt] = calculate_trapezoidal_area(&usInputRegisters[10], 50);
            search_signal.signal_point_amt++;
        }

        int area = calculate_trapezoidal_area(&usInputRegisters[10], 50);
        push_fun(&search_signal.signal[0], sizeof(search_signal.signal), area);

        int mas_amt = sizeof(search_signal.signal) / sizeof(search_signal.signal[0]);
        int search_signal_val = 0;
        int main_signal_val = 0;

        for (uint8_t cnt = 0; cnt < mas_amt; cnt++)
            search_signal_val += search_signal.signal[cnt];
        search_signal_val = (search_signal_val / mas_amt);

        for (uint8_t cnt = 0; cnt < mas_amt; cnt++)
            main_signal_val += main_signal.signal[cnt];
        main_signal_val = (main_signal_val / mas_amt);

        usInputRegisters[4] = smooth_value(0.05, ABS_DIFF(search_signal_val, main_signal_val), usInputRegisters[4]);
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

double calculate_trapezoidal_area(uint16_t *data, int size)
{
    double area = 0.0;
    double height = 1;

    for (int i = 1; i < size; i++)
    {
        double base = data[i] + data[i - 1];
        area += (base * 0.5) * height;
    }
    return area;
}
