#include "INC_CTRl.h"
#include "stm32f4xx.h"
//0:stop 1:up 2:down
void INC_IO_Set(uint8_t state)
{
    enum state
    {
        stop,
        up,
        down
    };
    uint16_t *const gpio_reg = &GPIOF->BSRRL;
    const uint16_t gpio_pinUP = GPIO_Pin_14;
    const uint16_t gpio_pinDown = GPIO_Pin_13;
    if (state == up)
    {
        gpio_reg[1] = gpio_pinDown;
        gpio_reg[0] = gpio_pinUP;
    }
    else if (state == down)
    {
        gpio_reg[0] = gpio_pinDown;
        gpio_reg[1] = gpio_pinUP;
    }
    else
    {
        gpio_reg[1] = gpio_pinDown;
        gpio_reg[1] = gpio_pinUP;
    }
}

void INC_IO_INIT()
{
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOFEN;

    GPIOF->MODER &= ~GPIO_MODER_MODER13;
    GPIOF->MODER &= ~GPIO_MODER_MODER14;

    GPIOF->MODER |= GPIO_MODER_MODER13_0;
    GPIOF->MODER |= GPIO_MODER_MODER14_0;

    INC_IO_Set(0);
}
