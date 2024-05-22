#include <main.h>
#define MAX(x, y) ((x) > (y) ? (x) : (y))
#define ABS_DIFF(x, y) ((x) > (y) ? ((x) - (y)) : ((y) - (x)))
#define MIN(x, y) ((x) < (y) ? (x) : (y))

uint16_t act_coil_current = 0;
int median_filter(uint16_t a, uint16_t b, uint16_t c);
void push_fun(uint16_t *arr_ptr, uint16_t arr_size, uint16_t new_val);
uint16_t mass[80];
double calculate_trapezoidal_area(uint16_t *data, int size);

void search_function()
{
    if (usInputRegisters[9])
    {
        usInputRegisters[9] = 0;

        // Постоянная составляющая
        int area = (calculate_trapezoidal_area(&usInputRegisters[10], 50)) - 39000;

        push_fun(&mass[0], sizeof(mass), area);

        int sum = 0;
        int mas_amt = sizeof(mass) / sizeof(mass[0]);

        for (uint8_t cnt = 0; cnt < mas_amt; cnt++)
            sum += mass[cnt];
        sum = (sum / mas_amt);

        usInputRegisters[4] = sum;
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
