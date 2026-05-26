/*
 * File Name: stm32f446xx_gpio_driver.h
 * Description: GPIO Driver Specific Header File
 * Created on: 26-Feb-2026
 * Author: chandan
 */




#ifndef INC_STM32F446XX_GPIO_DRIVER_H_
#define INC_STM32F446XX_GPIO_DRIVER_H_

#include "stm32f446xx.h"


/*********************************************************************
 * @GPIO_PinConfig_t
 *
 * @brief  Configuration structure for a GPIO pin.
 *
 * @Note   This structure is used to configure individual GPIO pins.
 * 			The user must fill this structure and pass it to
 * 			GPIO_Init().
 *
 * 			Each GPIO pin can be configured independently for:
 *
 * 			- Mode
 * 			- Speed
 * 			- Pull-up/Pull-down
 * 			- Output type
 * 			- Alternate functionality
 *
 *********************************************************************/
typedef struct
{
	uint8_t GPIO_PinNumber;      /*!< Specifies the GPIO pin number.
									  Possible values from @GPIO_PIN_NUMBERS */

	uint8_t GPIO_PinMode;        /*!< Specifies the mode of GPIO pin.
									  Possible values from @GPIO_PIN_MODES */

	uint8_t GPIO_PinSpeed;       /*!< Specifies GPIO output speed.
									  Applicable only for output mode.
									  Possible values from @GPIO_PIN_SPEED */

	uint8_t GPIO_PinPuPdControl; /*!< Specifies Pull-up or Pull-down
									  configuration.
									  Possible values from @GPIO_PIN_PUPD */

	uint8_t GPIO_PinOPType;      /*!< Specifies output type:
									  Push-Pull or Open-Drain.
									  Applicable only for output mode.
									  Possible values from @GPIO_PIN_OPTYPE */

	uint8_t GPIO_PinAltFunMode;  /*!< Specifies alternate functionality
									  selection.
									  Possible values from @GPIO_PIN_ALT */

}GPIO_PinConfig_t;



/*********************************************************************
 * @GPIO_Handle_t
 *
 * @brief  Handle structure for a GPIO pin.
 *
 * @Note   This structure stores:
 *
 * 			1. Base address of GPIO port
 * 			2. GPIO pin configuration settings
 *
 * 			This handle is passed to GPIO_Init() API.
 *
 *********************************************************************/
typedef struct
{
	GPIO_RegDef_t *pGPIOx;          /*!< Base address of GPIO port */

	GPIO_PinConfig_t GPIO_PinConfig;/*!< GPIO pin configuration settings */

}GPIO_Handle_t;





/*********************************************************************
 * @GPIO_PIN_NUMBERS
 *
 * @brief  GPIO pin number definitions.
 *
 * @Note   STM32 GPIO ports contain maximum 16 pins:
 *
 * 			GPIO_PIN_NO_0  --> GPIO Pin 0
 * 			GPIO_PIN_NO_15 --> GPIO Pin 15
 *
 *********************************************************************/
#define GPIO_PIN_NO_0       	0U
#define GPIO_PIN_NO_1       	1U
#define GPIO_PIN_NO_2       	2U
#define GPIO_PIN_NO_3       	3U
#define GPIO_PIN_NO_4       	4U
#define GPIO_PIN_NO_5       	5U
#define GPIO_PIN_NO_6       	6U
#define GPIO_PIN_NO_7       	7U
#define GPIO_PIN_NO_8       	8U
#define GPIO_PIN_NO_9       	9U
#define GPIO_PIN_NO_10      	10U
#define GPIO_PIN_NO_11      	11U
#define GPIO_PIN_NO_12      	12U
#define GPIO_PIN_NO_13      	13U
#define GPIO_PIN_NO_14      	14U
#define GPIO_PIN_NO_15      	15U



/*********************************************************************
 * @GPIO_PIN_MODES
 *
 * @brief  GPIO possible pin modes.
 *
 * @Note
 * 			Non-interrupt modes:
 *
 * 			GPIO_MODE_IN      --> Input mode
 * 			GPIO_MODE_OUT     --> General purpose output mode
 * 			GPIO_MODE_ALTFN   --> Alternate function mode
 * 			GPIO_MODE_ANALOG  --> Analog mode
 *
 * 			Interrupt modes:
 *
 * 			GPIO_MODE_IT_FT   --> Interrupt falling edge trigger
 * 			GPIO_MODE_IT_RT   --> Interrupt rising edge trigger
 * 			GPIO_MODE_IT_RFT  --> Interrupt rising + falling trigger
 *
 *********************************************************************/

/* Non-interrupt modes */
#define GPIO_MODE_IN            	0U
#define GPIO_MODE_OUT           	1U
#define GPIO_MODE_ALTFN         	2U
#define GPIO_MODE_ANALOG        	3U

/* Interrupt modes */
#define GPIO_MODE_IT_FT         	4U
#define GPIO_MODE_IT_RT         	5U
#define GPIO_MODE_IT_RFT        	6U



/*********************************************************************
 * @GPIO_PIN_SPEED
 *
 * @brief  GPIO output speed configuration.
 *
 * @Note   These macros configure GPIO output transition speed.
 *
 * 			Higher speed:
 * 			- Faster rise/fall time
 * 			- Higher power consumption
 * 			- More EMI/noise
 *
 *********************************************************************/
#define GPIO_SPEED_LOW          	0U
#define GPIO_SPEED_MEDIUM       	1U
#define GPIO_SPEED_FAST         	2U
#define GPIO_SPEED_HIGH         	3U



/*********************************************************************
 * @GPIO_PIN_PUPD
 *
 * @brief  GPIO Pull-up/Pull-down configuration macros.
 *
 * @Note
 * 			GPIO_NO_PUPD --> No pull resistor
 * 			GPIO_PIN_PU  --> Pull-up enabled
 * 			GPIO_PIN_PD  --> Pull-down enabled
 *
 *********************************************************************/
#define GPIO_PIN_NO_PUPD           	0U
#define GPIO_PIN_PU            		1U
#define GPIO_PIN_PD            		2U



/*********************************************************************
 * @GPIO_PIN_OPTYPE
 *
 * @brief  GPIO output type configuration.
 *
 * @Note
 * 			GPIO_OP_TYPE_PP --> Push-Pull output
 * 			GPIO_OP_TYPE_OD --> Open-Drain output
 *
 *********************************************************************/
#define GPIO_OP_TYPE_PP         	0U
#define GPIO_OP_TYPE_OD         	1U



/*********************************************************************
 * @GPIO_MODE_ALTFNS
 *
 * @brief  GPIO alternate function selection macros.
 *
 * @Note   STM32 GPIO pins support multiple alternate functions
 * 			such as:
 *
 * 			- SPI
 * 			- I2C
 * 			- USART
 * 			- TIM
 * 			- CAN
 * 			- USB
 *
 * 			The exact AF mapping depends on:
 * 			- GPIO pin
 * 			- Peripheral
 * 			- MCU datasheet
 *
 *********************************************************************/
#define GPIO_MODE_ALTFN_AF0		0U
#define GPIO_MODE_ALTFN_AF1		1U
#define GPIO_MODE_ALTFN_AF2		2U
#define GPIO_MODE_ALTFN_AF3		3U
#define GPIO_MODE_ALTFN_AF4		4U
#define GPIO_MODE_ALTFN_AF5		5U
#define GPIO_MODE_ALTFN_AF6		6U
#define GPIO_MODE_ALTFN_AF7		7U
#define GPIO_MODE_ALTFN_AF8		8U
#define GPIO_MODE_ALTFN_AF9		9U
#define GPIO_MODE_ALTFN_AF10	10U
#define GPIO_MODE_ALTFN_AF11	11U
#define GPIO_MODE_ALTFN_AF12	12U
#define GPIO_MODE_ALTFN_AF13	13U
#define GPIO_MODE_ALTFN_AF14	14U
#define GPIO_MODE_ALTFN_AF15	15U




/**************************************************************************************************************
 *                                    APIs SUPPORTED BY THIS DRIVER
 *                   For more information about the APIs check the function definitions
 **************************************************************************************************************/



/*********************************************************************
 * Peripheral Clock Setup APIs
 *********************************************************************/
void GPIO_PeriClockControl(GPIO_RegDef_t *pGPIOx,
						   uint8_t EnorDi);



/*********************************************************************
 * GPIO Initialization and De-Initialization APIs
 *********************************************************************/
void GPIO_Init(GPIO_Handle_t *pGPIOHandle);

void GPIO_DeInit(GPIO_RegDef_t *pGPIOx);



/*********************************************************************
 * GPIO Data Read and Write APIs
 *********************************************************************/
uint8_t GPIO_ReadFromInputPin(GPIO_RegDef_t *pGPIOx,
							  uint8_t PinNumber);

uint16_t GPIO_ReadFromInputPort(GPIO_RegDef_t *pGPIOx);

void GPIO_WriteToOutputPin(GPIO_RegDef_t *pGPIOx,
						   uint8_t PinNumber,
						   uint8_t Value);

void GPIO_WriteToOutputPort(GPIO_RegDef_t *pGPIOx,
						    uint16_t Value);

void GPIO_ToggleOutputPin(GPIO_RegDef_t *pGPIOx,
						  uint8_t PinNumber);



/*********************************************************************
 * GPIO Interrupt Configuration and ISR Handling APIs
 *********************************************************************/
void GPIO_IRQInterruptConfig(uint8_t IRQNumber,
							 uint8_t EnorDi);

void GPIO_IRQPriorityConfig(uint8_t IRQNumber,
							uint32_t IRQPriority);

void GPIO_IRQHandling(uint8_t PinNumber);




#endif /* INC_STM32F446XX_GPIO_DRIVER_H_ */
