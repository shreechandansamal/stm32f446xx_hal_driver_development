/*
 * File Name: stm32f446xx_gpio_driver.h
 * Description: GPIO Driver Specific Header File
 * Created on: 26-Feb-2026
 * Author: chandan
 */




#ifndef INC_STM32F446XX_GPIO_DRIVER_H_
#define INC_STM32F446XX_GPIO_DRIVER_H_

#include "stm32f446xx.h"

/*
 * This is a Configuration structure for a GPIO pin
 */
typedef struct
{
	uint8_t GPIO_PinNumber;         /*!< possible values from @GPIO_PIN_NUMBERS >*/
	uint8_t GPIO_PinMode;           /*!< Input / Output / AF / Analog, possible values from @GPIO_PIN_MODES  >*/
	uint8_t GPIO_PinSpeed;          /*!< Only Output, possible values from @GPIO_PIN_SPEED  >*/
	uint8_t GPIO_PinPuPdControl;    /*!< Input & Output, possible values from @GPIO_PIN_PUPD  >*/
	uint8_t GPIO_PinOPType;         /*!< Only Output, possible values from @GPIO_PIN_OPTYPE  >*/
	uint8_t GPIO_PinAltFunMode;     /*!< Input & Output, possible values from @GPIO_PIN_ALT  >*/
}GPIO_PinConfig_t;


/*
 * This is a Handle structure for a GPIO pin
 */
typedef struct
{
	GPIO_RegDef_t *pGPIOx;             /*!< This holds the address of the GPIO port to which the pin belongs >*/
	GPIO_PinConfig_t GPIO_PinConfig;   /*!< This holds GPIO pin configuration settings >*/
}GPIO_Handle_t;







/*
 * @GPIO_PIN_NUMBERS
 * GPIO pin numbers
 */
#define GPIO_PIN_NO_0       0      /*GPIO Pin 0*/
#define GPIO_PIN_NO_1       1      /*GPIO Pin 1*/
#define GPIO_PIN_NO_2       2      /*GPIO Pin 2*/
#define GPIO_PIN_NO_3       3      /*GPIO Pin 3*/
#define GPIO_PIN_NO_4       4      /*GPIO Pin 4*/
#define GPIO_PIN_NO_5       5      /*GPIO Pin 5*/
#define GPIO_PIN_NO_6       6      /*GPIO Pin 6*/
#define GPIO_PIN_NO_7       7      /*GPIO Pin 7*/
#define GPIO_PIN_NO_8       8      /*GPIO Pin 8*/
#define GPIO_PIN_NO_9       9      /*GPIO Pin 9*/
#define GPIO_PIN_NO_10      10     /*GPIO Pin 10*/
#define GPIO_PIN_NO_11      11     /*GPIO Pin 11*/
#define GPIO_PIN_NO_12      12     /*GPIO Pin 12*/
#define GPIO_PIN_NO_13      13     /*GPIO Pin 13*/
#define GPIO_PIN_NO_14      14     /*GPIO Pin 14*/
#define GPIO_PIN_NO_15      15     /*GPIO Pin 15*/


/*
 * @GPIO_PIN_MODES
 * GPIO pin possible modes
 * from (0-3) those are non-interupt mode, and from (4-6) those are interrupt mode
 */
/*non-interrupt modes*/
#define GPIO_MODE_IN            0  /*Input Mode*/
#define GPIO_MODE_OUT           1  /*Output Mode*/
#define GPIO_MODE_ALTFN         2  /*Alternate Function Mode*/
#define GPIO_MODE_ANALOG        3  /*Analog Mode*/
/*interrupt modes*/
#define GPIO_MODE_IT_FT         4  /*Interrupt Falling Edge Trigger*/
#define GPIO_MODE_IT_RT         5  /*Interrupt Rising Edge Trigger*/
#define GPIO_MODE_IT_RFT        6  /*Interrupt Rising Edge and Falling Edge Trigger*/


/*
 * @GPIO_PIN_SPEED
 * GPIO pin possible output speeds
 */
#define GPIO_SPEED_LOW          0  /*GPIO on Low Speed */
#define GPIO_SPEED_MEDIUM       1  /*GPIO on Medium Speed */
#define GPIO_SPEED_FAST         2  /*GPIO on Fast Speed */
#define GPIO_SPEED_HIGH         3  /*GPIO on High Speed */


/*
 * @GPIO_PIN_PUPD
 * GPIO pin pull-up and pull-down configuration macros
 */
#define GPIO_NO_PUPD           0  /*GPIO on No pull-up and No pull-down*/
#define GPIO_PIN_PU            1  /*GPIO on pull-up*/
#define GPIO_PIN_PD            2  /*GPIO on pull-down*/


/*
 * @GPIO_PIN_OPTYPE
 * GPIO pin possible output types
 */
#define GPIO_OP_TYPE_PP         0  /*Output Type Push-Pull*/
#define GPIO_OP_TYPE_OD         1  /*Output Type Open-Drain*/







/**************************************************************************************************************
 *                                    APIs supported by this driver
 *                   For more information about the APIs check the function definitions
 **************************************************************************************************************/

/*
 * Peripheral Clock setup
 */
void GPIO_PeriClockControl(GPIO_RegDef_t *pGPIOx, uint8_t EnorDi);
/*
 * Init and De-Init
 */
void GPIO_Init(GPIO_Handle_t *pGPIOHandle); /*Initialize port and pin*/
void GPIO_DeInit(GPIO_RegDef_t *pGPIOx);    /*It will de-initialize the GPIO port and it will send back the GPIO port setting to Reset state*/


/*
 * Data read and write
 */
uint8_t GPIO_ReadFromInputPin(GPIO_RegDef_t *pGPIOx, uint8_t PinNumber);               /*Read from Input Pin*/
uint16_t GPIO_ReadFromInputPort(GPIO_RegDef_t *pGPIOx);                                /*Read from entire Input Port*/
void GPIO_WriteToOutputPin(GPIO_RegDef_t *pGPIOx, uint8_t PinNumber, uint8_t Value);   /*Write to Output Pin*/
void GPIO_WriteToOutputPort(GPIO_RegDef_t *pGPIOx, uint16_t Value);                    /*Write to entire Output Port*/
void GPIO_ToggleOutputPin(GPIO_RegDef_t *pGPIOx, uint8_t PinNumber);				   /*Toggle to the Output Pin*/


/*
 * IRQ Configuration and ISR handling
 */
void GPIO_IRQInterruptConfig(uint8_t IRQNumber, uint8_t EnorDi);		               /*it will Configure the IRQ number of the GPIO pin like enabling it,
																						 desabling it.*/
void GPIO_IRQPriorityConfig(uint8_t IRQNumber, uint32_t IRQPriority);				   /*Priority setting to the IRQ number*/
void GPIO_IRQHandling(uint8_t PinNumber);											   /*it means whenever interrupt occurs so the user application will call
                                                                                         this function inorder to process that interrupt */










#endif /* INC_STM32F446XX_GPIO_DRIVER_H_ */
