/*
 * board_keyboard_cfg.c
 *
 *  Created on: Feb 26, 2026
 *      Author: chandan
 */


#include "board_keyboard_cfg.h"
#include "board_pins.h"
#include "stm32f446xx_gpio_driver.h"

/* Helper arrays for mapping */
static const uint8_t row_pins[KB_ROWS] = {
    KB_ROW0_PIN,
    KB_ROW1_PIN,
    KB_ROW2_PIN,
    KB_ROW3_PIN,
    KB_ROW4_PIN
};

static const uint8_t col_pins[KB_COLS] = {
    KB_COL0_PIN,
    KB_COL1_PIN,
    KB_COL2_PIN,
    KB_COL3_PIN,
    KB_COL4_PIN,
    KB_COL5_PIN,
    KB_COL6_PIN,
    KB_COL7_PIN,
    KB_COL8_PIN,
    KB_COL9_PIN
};

void Board_KB_Init(void)
{
    GPIO_Handle_t gpio;

    /* Enable BOTH clocks */
    GPIO_PeriClockControl(KB_ROW_GPIO_PORT, ENABLE);
    GPIO_PeriClockControl(KB_COL_GPIO_PORT, ENABLE);

    /* ---------- Configure Rows (Output Push-Pull) ---------- */
    gpio.pGPIOx = KB_ROW_GPIO_PORT;
    gpio.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_OUT;
    gpio.GPIO_PinConfig.GPIO_PinOPType = GPIO_OP_TYPE_PP;
    gpio.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_NO_PUPD;
    gpio.GPIO_PinConfig.GPIO_PinSpeed = GPIO_SPEED_FAST;

    for(uint8_t i = 0; i < KB_ROWS; i++)
    {
        gpio.GPIO_PinConfig.GPIO_PinNumber = row_pins[i];
        GPIO_Init(&gpio);

        /* Default HIGH (inactive) */
        GPIO_WriteToOutputPin(KB_ROW_GPIO_PORT, row_pins[i], SET);
    }

    /* ---------- Configure Columns (Input Pull-up) ---------- */
    gpio.pGPIOx = KB_COL_GPIO_PORT;
    gpio.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_IN;
    gpio.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PIN_PU;

    for(uint8_t i = 0; i < KB_COLS; i++)
    {
        gpio.GPIO_PinConfig.GPIO_PinNumber = col_pins[i];
        GPIO_Init(&gpio);
    }
}

void Board_KB_SetAllRowsInactive(void)
{
    for(uint8_t i = 0; i < KB_ROWS; i++)
    {
        GPIO_WriteToOutputPin(KB_ROW_GPIO_PORT, row_pins[i], SET);
    }
}

void Board_KB_SetRowActive(uint8_t row)
{
    Board_KB_SetAllRowsInactive();

    if(row < KB_ROWS)
    {
        /* Drive selected row LOW */
        GPIO_WriteToOutputPin(KB_ROW_GPIO_PORT, row_pins[row], RESET);
    }
}

uint8_t Board_KB_ReadColumn(uint8_t col)
{
    if(col < KB_COLS)
    {
        return GPIO_ReadFromInputPin(KB_COL_GPIO_PORT, col_pins[col]);
    }

    return 1;  // Not pressed (pull-up default HIGH)
}
