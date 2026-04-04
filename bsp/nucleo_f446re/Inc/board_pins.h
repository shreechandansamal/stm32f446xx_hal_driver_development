/*
 * board_pins.h
 *
 *  Created on: Feb 26, 2026
 *      Author: chandan
 */

#ifndef NUCLEO_F446RE_INC_BOARD_PINS_H_
#define NUCLEO_F446RE_INC_BOARD_PINS_H_

#include "stm32f446xx.h"

/* Matrix size */
#define KB_ROWS    5
#define KB_COLS    10

/* Row Port */
#define KB_ROW_GPIO_PORT    GPIOA

/* Column Port */
#define KB_COL_GPIO_PORT    GPIOC

/* -------- Rows (PA0 - PA4) -------- */
#define KB_ROW0_PIN    GPIO_PIN_NO_0
#define KB_ROW1_PIN    GPIO_PIN_NO_1
#define KB_ROW2_PIN    GPIO_PIN_NO_4
#define KB_ROW3_PIN    GPIO_PIN_NO_6
#define KB_ROW4_PIN    GPIO_PIN_NO_7

/* -------- Columns (PC0 - PC9) -------- */
#define KB_COL0_PIN    GPIO_PIN_NO_0
#define KB_COL1_PIN    GPIO_PIN_NO_1
#define KB_COL2_PIN    GPIO_PIN_NO_2
#define KB_COL3_PIN    GPIO_PIN_NO_3
#define KB_COL4_PIN    GPIO_PIN_NO_4
#define KB_COL5_PIN    GPIO_PIN_NO_5
#define KB_COL6_PIN    GPIO_PIN_NO_6
#define KB_COL7_PIN    GPIO_PIN_NO_7
#define KB_COL8_PIN    GPIO_PIN_NO_8
#define KB_COL9_PIN    GPIO_PIN_NO_9


#endif /* NUCLEO_F446RE_INC_BOARD_PINS_H_ */
