/*
 * keyboard.h
 *
 *  Created on: Feb 26, 2026
 *      Author: chandan
 */

#ifndef KEYBOARD_5X10_INC_KEYBOARD_H_
#define KEYBOARD_5X10_INC_KEYBOARD_H_

#include <stdint.h>

///* Matrix size (from board) */
//#define KEYBOARD_ROWS   5
//#define KEYBOARD_COLS   10
//
//typedef enum
//{
//    KEY_EVENT_NONE = 0,
//    KEY_EVENT_PRESSED,
//    KEY_EVENT_RELEASED
//} key_event_type_t;
//
//typedef struct
//{
//    uint8_t row;
//    uint8_t col;
//    key_event_type_t type;
//} key_event_t;
//
///* Initialize keyboard driver */
//void Keyboard_Init(void);
//
///* Call periodically (e.g., every 5ms) */
//void Keyboard_Task(void);
//
///* Get next key event (returns 1 if event available) */
//uint8_t Keyboard_GetEvent(key_event_t *event);

void Keyboard_Init(void);
void Keyboard_Task(void);      // Call every 1ms (SysTick)
char Keyboard_GetChar(void);   // Returns 0 if no key

#endif /* KEYBOARD_5X10_INC_KEYBOARD_H_ */
