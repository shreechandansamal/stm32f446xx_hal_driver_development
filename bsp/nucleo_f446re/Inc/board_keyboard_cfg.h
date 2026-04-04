/*
 * board_keyboard_cfg.h
 *
 *  Created on: Feb 26, 2026
 *      Author: chandan
 */

#ifndef NUCLEO_F446RE_INC_BOARD_KEYBOARD_CFG_H_
#define NUCLEO_F446RE_INC_BOARD_KEYBOARD_CFG_H_

#include <stdint.h>

/* Initialize keyboard GPIOs */
void Board_KB_Init(void);

/* Row control */
void Board_KB_SetRowActive(uint8_t row);
void Board_KB_SetAllRowsInactive(void);

/* Column read */
uint8_t Board_KB_ReadColumn(uint8_t col);

#endif /* NUCLEO_F446RE_INC_BOARD_KEYBOARD_CFG_H_ */


