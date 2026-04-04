/*
 * keyboard.c
 *
 *  Created on: Feb 26, 2026
 *      Author: chandan
 */

//e.g 2: mapped with character, numbers, symbols and shift

#include "keyboard.h"
#include "board_keyboard_cfg.h"
#include "board_pins.h"
#include "stm32f446xx_gpio_driver.h"

/* ================= CONFIG ================= */

#define DEBOUNCE_TIME_MS   20

#define SHIFT_ROW   4
#define SHIFT_COL   0

/* ================ KEYMAPS ================= */

static const char normal_keymap[KB_ROWS][KB_COLS] =
{
    {'1','2','3','4','5','6','7','8','9','0'},
    {'q','w','e','r','t','y','u','i','o','p'},
    {'a','s','d','f','g','h','j','k','l','!'},
    {'*','_','z','x','c','v','b','n','m',','},
    {'_','_','_','-',' ','-','-','_','_','_'}
};

static const char shift_keymap[KB_ROWS][KB_COLS] =
{
    {'!','@','#','$','%','^','&','*','(',')'},
    {'Q','W','E','R','T','Y','U','I','O','P'},
    {'A','S','D','F','G','H','J','K','L','?'},
    {'*','_','Z','X','C','V','B','N','M','.'},
    {'_','_','_','-',' ','-','-','_','_','_'}
};

/* ================ STATE MACHINE ================= */

typedef enum
{
    KEY_IDLE,
    KEY_DEBOUNCE_PRESS,
    KEY_PRESSED,
    KEY_DEBOUNCE_RELEASE
} KeyState_t;

static KeyState_t key_state = KEY_IDLE;

static uint8_t active_row = 0;
static uint8_t active_col = 0;

static uint32_t debounce_counter = 0;
static uint8_t shift_active = 0;

static char key_buffer = 0;

/* ================================================= */

void Keyboard_Init(void)
{
    Board_KB_Init();
}

/* Scan matrix */
static uint8_t ScanMatrix(uint8_t *row, uint8_t *col)
{
    for(uint8_t r = 0; r < KB_ROWS; r++)
    {
        Board_KB_SetRowActive(r);

        for(uint8_t c = 0; c < KB_COLS; c++)
        {
            if(Board_KB_ReadColumn(c) == 0)
            {
                *row = r;
                *col = c;
                return 1;
            }
        }
    }

    return 0;
}

/* Called every 1ms from SysTick */
void Keyboard_Task(void)
{
    uint8_t row, col;
    uint8_t key_found = ScanMatrix(&row, &col);

    switch(key_state)
    {
        case KEY_IDLE:
            if(key_found)
            {
                active_row = row;
                active_col = col;
                debounce_counter = 0;
                key_state = KEY_DEBOUNCE_PRESS;
            }
        break;

        case KEY_DEBOUNCE_PRESS:
            if(key_found &&
               row == active_row &&
               col == active_col)
            {
                if(++debounce_counter >= DEBOUNCE_TIME_MS)
                {
                    key_state = KEY_PRESSED;

                    /* -------- SHIFT TOGGLE -------- */
                    if(active_row == SHIFT_ROW &&
                       active_col == SHIFT_COL)
                    {
                        shift_active ^= 1;

                        if(shift_active)
                            GPIO_WriteToOutputPin(GPIOA, GPIO_PIN_NO_5, SET);
                        else
                            GPIO_WriteToOutputPin(GPIOA, GPIO_PIN_NO_5, RESET);

                        return;  // do not generate character
                    }

                    /* -------- NORMAL KEY -------- */
                    if(shift_active)
                        key_buffer = shift_keymap[active_row][active_col];
                    else
                        key_buffer = normal_keymap[active_row][active_col];
                }
            }
            else
            {
                key_state = KEY_IDLE;
            }
        break;

        case KEY_PRESSED:
            if(!key_found)
            {
                debounce_counter = 0;
                key_state = KEY_DEBOUNCE_RELEASE;
            }
        break;

        case KEY_DEBOUNCE_RELEASE:
            if(!key_found)
            {
                if(++debounce_counter >= DEBOUNCE_TIME_MS)
                {
                    key_state = KEY_IDLE;
                }
            }
            else
            {
                key_state = KEY_PRESSED;
            }
        break;
    }
}

char Keyboard_GetChar(void)
{
    char temp = key_buffer;
    key_buffer = 0;
    return temp;
}






















// e.g 1: Testing pressing any button gives you KEY_EVENT_PRESSED, no key map with real charectres and numbers.

//#include "keyboard.h"
//#include "board_keyboard_cfg.h"
//
//#define DEBOUNCE_TICKS   3     // 3 scans × 5ms = 15ms
//#define EVENT_QUEUE_SIZE 10
//
//typedef enum
//{
//    KEY_IDLE,
//    KEY_DEBOUNCE_PRESS,
//    KEY_PRESSED,
//    KEY_DEBOUNCE_RELEASE
//} key_state_t;
//
//static key_state_t keyStates[KEYBOARD_ROWS][KEYBOARD_COLS];
//static uint8_t debounceCounter[KEYBOARD_ROWS][KEYBOARD_COLS];
//
///* Event queue */
//static key_event_t eventQueue[EVENT_QUEUE_SIZE];
//static uint8_t eventHead = 0;
//static uint8_t eventTail = 0;
//
//
//static void PushEvent(uint8_t row, uint8_t col, key_event_type_t type)
//{
//    uint8_t next = (eventHead + 1) % EVENT_QUEUE_SIZE;
//
//    if(next != eventTail)   // not full
//    {
//        eventQueue[eventHead].row = row;
//        eventQueue[eventHead].col = col;
//        eventQueue[eventHead].type = type;
//        eventHead = next;
//    }
//}
//
//
//void Keyboard_Init(void)
//{
//    Board_KB_Init();
//
//    for(uint8_t r = 0; r < KEYBOARD_ROWS; r++)
//    {
//        for(uint8_t c = 0; c < KEYBOARD_COLS; c++)
//        {
//            keyStates[r][c] = KEY_IDLE;
//            debounceCounter[r][c] = 0;
//        }
//    }
//}
//
//
//void Keyboard_Task(void)
//{
//    for(uint8_t row = 0; row < KEYBOARD_ROWS; row++)
//    {
//        Board_KB_SetRowActive(row);
//
//        for(uint8_t col = 0; col < KEYBOARD_COLS; col++)
//        {
//            uint8_t pressed = (Board_KB_ReadColumn(col) == 0);
//
//            switch(keyStates[row][col])
//            {
//                case KEY_IDLE:
//                    if(pressed)
//                    {
//                        keyStates[row][col] = KEY_DEBOUNCE_PRESS;
//                        debounceCounter[row][col] = DEBOUNCE_TICKS;
//                    }
//                    break;
//
//                case KEY_DEBOUNCE_PRESS:
//                    if(pressed)
//                    {
//                        if(--debounceCounter[row][col] == 0)
//                        {
//                            keyStates[row][col] = KEY_PRESSED;
//                            PushEvent(row, col, KEY_EVENT_PRESSED);
//                        }
//                    }
//                    else
//                    {
//                        keyStates[row][col] = KEY_IDLE;
//                    }
//                    break;
//
//                case KEY_PRESSED:
//                    if(!pressed)
//                    {
//                        keyStates[row][col] = KEY_DEBOUNCE_RELEASE;
//                        debounceCounter[row][col] = DEBOUNCE_TICKS;
//                    }
//                    break;
//
//                case KEY_DEBOUNCE_RELEASE:
//                    if(!pressed)
//                    {
//                        if(--debounceCounter[row][col] == 0)
//                        {
//                            keyStates[row][col] = KEY_IDLE;
//                            PushEvent(row, col, KEY_EVENT_RELEASED);
//                        }
//                    }
//                    else
//                    {
//                        keyStates[row][col] = KEY_PRESSED;
//                    }
//                    break;
//            }
//        }
//    }
//
//    Board_KB_SetAllRowsInactive();
//}
//
//
//uint8_t Keyboard_GetEvent(key_event_t *event)
//{
//    if(eventHead == eventTail)
//        return 0;
//
//    *event = eventQueue[eventTail];
//    eventTail = (eventTail + 1) % EVENT_QUEUE_SIZE;
//
//    return 1;
//}





