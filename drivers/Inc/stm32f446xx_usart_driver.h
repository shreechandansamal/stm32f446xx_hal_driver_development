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
}USART_Handle_t;









/**************************************************************************************************************
 *                                    APIs Supported By This Driver
 *
 *              For detailed information about each API,
 *              refer to the function definitions in source file.
 *
 **************************************************************************************************************/




/*********************************************************************
 * Peripheral Clock Control APIs
 *********************************************************************/
void USART_PeriClockControl(USART_RegDef_t *pUSARTx,
						    uint8_t EnorDi);




/*********************************************************************
 * Initialization and De-Initialization APIs
 *********************************************************************/
void USART_Init(USART_Handle_t *pUSARTHandle);

void USART_DeInit(USART_RegDef_t *pUSARTx);




/*********************************************************************
 * Data Transmission and Reception APIs
 *********************************************************************/
void USART_SendData(USART_RegDef_t *pUSARTx,
					uint8_t *pTxBuffer,
					uint32_t Len);

void USART_ReceiveData(USART_RegDef_t *pUSARTx,
					   uint8_t *pRxBuffer,
					   uint32_t Len);

uint8_t USART_SendDataIT(USART_Handle_t *pUSARTHandle,
						 uint8_t *pTxBuffer,
						 uint32_t Len);

uint8_t USART_ReceiveDataIT(USART_Handle_t *pUSARTHandle,
							uint8_t *pRxBuffer,
							uint32_t Len);




/*********************************************************************
 * IRQ Configuration and ISR Handling APIs
 *********************************************************************/
void USART_IRQInterruptConfig(uint8_t IRQNumber,
							  uint8_t EnorDi);

void USART_IRQPriorityConfig(uint8_t IRQNumber,
						     uint32_t IRQPriority);

void USART_IRQHandling(USART_Handle_t *pUSARTHandle);




/*********************************************************************
 * USART Peripheral Control APIs
 *********************************************************************/
void USART_PeripheralControl(USART_RegDef_t *pUSARTx,
	    					 uint8_t EnorDi);

uint8_t USART_GetFlagStatus(USART_RegDef_t *pUSARTx,
						  	uint8_t FlagName);

void USART_ClearFlag(USART_RegDef_t *pUSARTx,
					 uint8_t FlagName);




/*********************************************************************
 * I2C Application Callback
 *********************************************************************/
void USART_ApplicationEventCallback(USART_Handle_t *pUSARTHandle,
									uint8_t AppEvent);



#endif /* INC_STM32F446XX_USART_DRIVER_H_ */
