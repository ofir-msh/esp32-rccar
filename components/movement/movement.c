#include <stdbool.h>
#include "driver/gpio.h"

#define EN12 GPIO_NUM_32
#define EN34 GPIO_NUM_14
#define A1 GPIO_NUM_33
#define A2 GPIO_NUM_25
#define A3 GPIO_NUM_26
#define A4 GPIO_NUM_27

const int leftMotor[3] = {EN12, A1, A2};
const int rightMotor[3] = {EN34, A3, A4};

void engine(bool enable)
{
    gpio_set_level(leftMotor[0], enable);
    gpio_set_level(rightMotor[0], enable);
}

void go(bool forward)
{
    gpio_set_level(leftMotor[1], forward);
    gpio_set_level(leftMotor[2], !forward);
    gpio_set_level(rightMotor[1], forward);
    gpio_set_level(rightMotor[2], !forward);
}

void rotate(bool left)
{
    gpio_set_level(leftMotor[1], !left);
    gpio_set_level(leftMotor[2], left);
    gpio_set_level(rightMotor[1], left);
    gpio_set_level(rightMotor[2], !left);
}

void initializePins()
{
    for (int i = 0; i < 3; i++)
    {
        gpio_reset_pin(leftMotor[i]);
        gpio_set_direction(leftMotor[i], GPIO_MODE_OUTPUT);
        gpio_reset_pin(rightMotor[i]);
        gpio_set_direction(rightMotor[i], GPIO_MODE_OUTPUT);
    }
}
