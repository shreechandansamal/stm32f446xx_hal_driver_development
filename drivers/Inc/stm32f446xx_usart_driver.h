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
	uint8_t USART_Mode;				/*!< Holds USART mode settings */
	uint32_t USART_BaudRate;		/*!< Holds USART baud rate
	 	 	 	 	 	 	 	 	 	 settings */
	uint8_t USART_NoOfStopBits;		/*!< Holds USART number of stop
	 	 	 	 	 	 	 	 	 	 bits settings */
	uint8_t USART_WordLength;		/*!< Holds USART word length
	 	 	 	 	 	 	 	 	 	 settings */
	uint8_t USART_ParityControl;	/*!< Holds USART parity bit
	 	 	 	 	 	 	 	 	 	 control settings */
	uint8_t USART_HWFlowControl;	/*!< Holds USART hardware flow
	 	 	 	 	 	 	 	 	 	 control settings */
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
									     USART peripheral (USART1/
									     UART4...) */
	USART_Config_t USART_Config;	/*!< Holds USART configuration
	 	 	 	 	 	 	 	 	 	 settings */
}USART_Handle_t;



/*********************************************************************
 * @USART_Mode
 *********************************************************************/
#define USART_MODE_ONLY_TX 					0U
#define USART_MODE_ONLY_RX 					1U
#define USART_MODE_TXRX  					2U




/*********************************************************************
 * @USART_BaudRate
 *********************************************************************/
#define USART_STD_BAUD_1200					1200U
#define USART_STD_BAUD_2400					2400U
#define USART_STD_BAUD_9600					9600U
#define USART_STD_BAUD_19200 				19200U
#define USART_STD_BAUD_38400 				38400U
#define USART_STD_BAUD_57600 				57600U
#define USART_STD_BAUD_115200 				115200U
#define USART_STD_BAUD_230400 				230400U
#define USART_STD_BAUD_460800 				460800U
#define USART_STD_BAUD_921600 				921600U
#define USART_STD_BAUD_2M 					2000000U
#define USART_STD_BAUD_3M 					3000000U




/*********************************************************************
 * @USART_ParityControl
 *********************************************************************/
#define USART_PARITY_EN_ODD   				2U
#define USART_PARITY_EN_EVEN     			1U
#define USART_PARITY_DISABLE    			0U




/*********************************************************************
 * @USART_WordLength
 *********************************************************************/
#define USART_WORDLEN_8BITS  				0U
#define USART_WORDLEN_9BITS  				1U




/*********************************************************************
 * @USART_NoOfStopBits
 *********************************************************************/
#define USART_STOPBITS_1     				0U
#define USART_STOPBITS_0_5   				1U
#define USART_STOPBITS_2     				2U
#define USART_STOPBITS_1_5   				3U




/*********************************************************************
 * @USART_HWFlowControl
 *********************************************************************/
#define USART_HW_FLOW_CTRL_NONE    			0U
#define USART_HW_FLOW_CTRL_CTS    			1U
#define USART_HW_FLOW_CTRL_RTS    			2U
#define USART_HW_FLOW_CTRL_CTS_RTS			3U




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
