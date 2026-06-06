/*
 * stm32f446xx_usart_driver.h
 *
 *  Created on: Jun 5, 2026
 *      Author: chandan
 */

#ifndef INC_STM32F446XX_USART_DRIVER_H_
#define INC_STM32F446XX_USART_DRIVER_H_

#include "stm32f446xx.h"

/*********************************************************************
 * @USART Configuration Structure
 *********************************************************************
 * This structure is used to configure the USART peripheral.
 *
 * User application fills this structure and passes it to USART_Init().
 *
 * Each member of this structure represents one configurable feature
 * of the USART peripheral such as:
 *
 *    - USART Mode (Tx, Rx, TxRx)
 *    - USART BaudRate (Bps)
 *    - USART No Of Stop Bits(0.5, 1, 1.5, 2)
 *    - USART Word Length (8bits, 9bits)
 *    - USART Parity Control (no, even, odd)
 *    - USART HW Flow Control
 *
 *********************************************************************/
typedef struct
{
	uint8_t USART_Mode;
	uint32_t USART_BaudRate;
	uint8_t USART_NoOfStopBits;
	uint8_t USART_WordLength;
	uint8_t USART_ParityControl;
	uint8_t USART_HWFlowControl;
}USART_Config_t;



/*********************************************************************
 * @USART Handle Structure
 *********************************************************************
 * This structure is used by the USART driver to manage:
 *
 *    - USART peripheral information
 *    - USART configuration settings
 *
 * This structure acts like a software object/handle for one USART
 * peripheral instance.
 *
 * Example:
 *
 *    USART_Handle_t USART_Handle;
 *
 *********************************************************************/
typedef struct
{
	USART_RegDef_t *pUSARTx;		/*!< Holds the base address of the
									     USART peripheral(USART1/UART4...) */
	USART_Config_t USART_Config;	/*!< Holds USART configuration settings */
}I2C_Handle_t;






#endif /* INC_STM32F446XX_USART_DRIVER_H_ */
