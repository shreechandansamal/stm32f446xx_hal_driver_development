/*
 * app_keyboard_test.c
 *
 *  Created on: Feb 26, 2026
 *      Author: chandan
 */

#include <stdio.h>
#include "stm32f446xx.h"
#include "keyboard.h"
#include "stm32f446xx_gpio_driver.h"

volatile uint32_t ms_ticks = 0;

void SysTick_Init(void)
{
    uint32_t count = 16000 - 1;  // 16MHz clock

    SysTick->LOAD = count;
    SysTick->VAL  = 0;

    /* Enable SysTick:
       bit 0 = ENABLE
       bit 1 = TICKINT
       bit 2 = CLKSOURCE (1 = processor clock)
    */
    SysTick->CTRL = (1 << 0) | (1 << 1) | (1 << 2);
}


/* SysTick interrupt */
void SysTick_Handler(void)
{
    ms_ticks++;
    Keyboard_Task();   // Run keyboard every 1ms
}

int main(void)
{
    /* LED (PA5) Init */
    GPIO_Handle_t led;

//    GPIO_PeriClockControl(GPIOA, ENABLE);// not needed already defined inside the GPIO_Init()

    led.pGPIOx = GPIOA;
    led.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_5;
    led.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_OUT;
    led.GPIO_PinConfig.GPIO_PinOPType = GPIO_OP_TYPE_PP;
    led.GPIO_PinConfig.GPIO_PinSpeed = GPIO_SPEED_FAST;
    led.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_NO_PUPD;

    GPIO_Init(&led);

    /* Configure SysTick for 1ms interrupt */
	SysTick_Init();   // If running at 16 MHz

    Keyboard_Init();

    printf("Keyboard Ready...\n");

    while(1)
    {
        char key = Keyboard_GetChar();

        if(key)
        {
            printf("Key: %c\n", key);
        }
    }
}












//#include <stdio.h>
//#include "stm32f446xx.h"
//#include "keyboard.h"
//#include "stm32f446xx_gpio_driver.h"
//
//volatile uint32_t g_msTicks = 0;
//volatile uint8_t  g_kbScanFlag = 0;
//
///* ---------------- SysTick Handler ---------------- */
//void SysTick_Handler(void)
//{
//    g_msTicks++;
//
//    if((g_msTicks % 5) == 0)   // 5ms periodic
//    {
//        g_kbScanFlag = 1;
//    }
//}
//
//
//static void SysTick_Init(void)
//{
//    /* Assuming system clock = 16 MHz (HSI default) */
//
//    uint32_t reload = 16000 - 1;  // 1ms @ 16MHz
//
//    SysTick->LOAD  = reload;
//    SysTick->VAL   = 0;
//
//    SysTick->CTRL |= (1 << 2);  // CLKSOURCE = Processor clock
//    SysTick->CTRL |= (1 << 1);  // TICKINT = Enable interrupt
//    SysTick->CTRL |= (1 << 0);  // ENABLE
//}
//
//
//static void LED_Init(void)
//{
//    GPIO_Handle_t led;
//
//    GPIO_PeriClockControl(GPIOC, ENABLE);
//
//    led.pGPIOx = GPIOA;
//    led.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_5;
//    led.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_OUT;
//    led.GPIO_PinConfig.GPIO_PinOPType = GPIO_OP_TYPE_PP;
//    led.GPIO_PinConfig.GPIO_PinSpeed = GPIO_SPEED_FAST;
//    led.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_NO_PUPD;
//
//    GPIO_Init(&led);
//}
//
//int main(void)
//{
//    LED_Init();
//    Keyboard_Init();
//    SysTick_Init();
//
//    key_event_t evt;
//
//    while(1)
//    {
//        if(g_kbScanFlag)
//        {
//            g_kbScanFlag = 0;
//            Keyboard_Task();
//        }
//
//        /* Process events */
//        while(Keyboard_GetEvent(&evt))
//        {
//            if(evt.type == KEY_EVENT_PRESSED)
//            {
//                /* Toggle LED on press */
////                GPIO_ToggleOutputPin(GPIOA, GPIO_PIN_NO_5);
//            	printf("Button Pressed\n");
//            }
//        }
//    }
//}



