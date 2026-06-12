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
 * The application fills this structure and passes it to USART_Init().
 *
 * Each member controls a specific feature of USART such as:
 *
 *    - Mode (TX / RX / TXRX)
 *    - BaudRate
 *    - Number of Stop Bits
 *    - Word Length
 *    - Parity Control
 *    - Hardware Flow Control
 *
 *********************************************************************/
typedef struct
{
	uint8_t USART_Mode;				/*!< USART mode configuration */
	uint32_t USART_BaudRate;		/*!< USART communication baud rate */
	uint8_t USART_NoOfStopBits;		/*!< Number of stop bits */
	uint8_t USART_WordLength;		/*!< Word length (8/9 bits) */
	uint8_t USART_ParityControl;	/*!< Parity configuration */
	uint8_t USART_HWFlowControl;	/*!< RTS/CTS hardware flow control */
}USART_Config_t;



/*********************************************************************
 * @USART Handle Structure
 *********************************************************************
 * This structure acts as a handle for a USART peripheral instance.
 *
 * It is used internally by the driver to manage:
 *
 *    - USART peripheral base address
 *    - Configuration settings
 *    - Tx/Rx buffers
 *    - Tx/Rx lengths
 *    - Busy states
 *
 *********************************************************************/
typedef struct
{
	USART_RegDef_t *pUSARTx;	  /*!< Base address of USART peripheral */
	USART_Config_t USART_Config;  /*!< USART configuration structure */
	uint8_t *pTxBuffer;		  	  /*!< Pointer to Tx buffer */
	uint8_t *pRxBuffer;		  	  /*!< Pointer to Rx buffer */
	uint32_t TxLen;			  	  /*!< Remaining Tx length */
	uint32_t RxLen;			  	  /*!< Remaining Rx length */
	uint8_t TxBusyState;		  /*!< Tx state (READY/BUSY) */
	uint8_t RxBusyState;		  /*!< Rx state (READY/BUSY) */
}USART_Handle_t;




/*********************************************************************
 * @USART Application States
 *********************************************************************/
#define USART_BUSY_IN_TX 					2U
#define USART_BUSY_IN_RX 					1U
#define USART_READY 						0U



/*********************************************************************
 * @USART Application Events
 *********************************************************************
 * These events are used in callback mechanism to notify the
 * application layer about USART operations.
 *********************************************************************/
#define USART_EV_TX_CMPLT   				0U
#define USART_EV_RX_CMPLT   				1U
#define USART_EV_IDLE      					2U
#define USART_EV_CTS       					3U
#define USART_EV_PE        					4U



/*********************************************************************
 * @USART Error Events
 *********************************************************************/
#define USART_ER_FE     					5U
#define USART_ER_NF    	 					6U
#define USART_ER_ORE    					7U




/*********************************************************************
 * @USART_Mode
 *********************************************************************/
#define USART_MODE_ONLY_TX 					0U
#define USART_MODE_ONLY_RX 					1U
#define USART_MODE_TXRX  					2U




/*********************************************************************
 * @USART_BaudRate (Standard Values)
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
#define USART_PARITY_DISABLE    			0U
#define USART_PARITY_EN_EVEN     			1U
#define USART_PARITY_EN_ODD   				2U




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




/*********************************************************************
 * @USART Status Flags
 *********************************************************************
 * These flags are used with USART_GetFlagStatus().
 *********************************************************************/
#define USART_FLAG_PE  					(1U << USART_SR_PE)
#define USART_FLAG_FE  					(1U << USART_SR_FE)
#define USART_FLAG_NF  					(1U << USART_SR_NF)
#define USART_FLAG_ORE    				(1U << USART_SR_ORE)
#define USART_FLAG_IDLE   				(1U << USART_SR_IDLE)
#define USART_FLAG_RXNE  			    (1U << USART_SR_RXNE)
#define USART_FLAG_TC  					(1U << USART_SR_TC)
#define USART_FLAG_TXE  				(1U << USART_SR_TXE)
#define USART_FLAG_LBD  			    (1U << USART_SR_LBD)
#define USART_FLAG_CTS  				(1U << USART_SR_CTS)




/**************************************************************************************************************
 *                          APIs SUPPORTED BY THIS DRIVER
 **************************************************************************************************************/



/*********************************************************************
 * @Peripheral Clock Control
 *********************************************************************/
void USART_PeriClockControl(USART_RegDef_t *pUSARTx,
						    uint8_t EnorDi);



/*********************************************************************
 * @Init and DeInit APIs
 *********************************************************************/
void USART_Init(USART_Handle_t *pUSARTHandle);
void USART_DeInit(USART_RegDef_t *pUSARTx);



/*********************************************************************
 * @Data Send and Receive (Blocking APIs)
 *********************************************************************/
void USART_SendData(USART_Handle_t *pUSARTHandle,
					uint8_t *pTxBuffer,
					uint32_t Len);

void USART_ReceiveData(USART_Handle_t *pUSARTHandle,
					   uint8_t *pRxBuffer,
					   uint32_t Len);



/*********************************************************************
 * @Data Send and Receive (Interrupt APIs)
 *********************************************************************/
uint8_t USART_SendDataIT(USART_Handle_t *pUSARTHandle,
						 uint8_t *pTxBuffer,
						 uint32_t Len);

uint8_t USART_ReceiveDataIT(USART_Handle_t *pUSARTHandle,
							uint8_t *pRxBuffer,
							uint32_t Len);



/*********************************************************************
 * @IRQ Configuration and Handling
 *********************************************************************/
void USART_IRQInterruptConfig(uint8_t IRQNumber,
							  uint8_t EnOrDi);

void USART_IRQPriorityConfig(uint8_t IRQNumber,
						     uint32_t IRQPriority);

void USART_IRQHandling(USART_Handle_t *pUSARTHandle);



/*********************************************************************
 * @USART Peripheral Control APIs
 *********************************************************************/
void USART_PeripheralControl(USART_RegDef_t *pUSARTx,
	    					 uint8_t EnorDi);

uint8_t USART_GetFlagStatus(USART_RegDef_t *pUSARTx,
						  	uint8_t FlagName);

void USART_ClearFlag(USART_RegDef_t *pUSARTx,
					 uint32_t FlagName);

void USART_SetBaudRate(USART_RegDef_t *pUSARTx,
					   uint32_t BaudRate);



/*********************************************************************
 * @Application Callback
 *********************************************************************
 * Application can override this function to handle USART events:
 *
 *    - TX complete
 *    - RX complete
 *    - Error conditions
 *
 *********************************************************************/
void USART_ApplicationEventCallback(USART_Handle_t *pUSARTHandle,
									uint8_t AppEvent);



#endif /* INC_STM32F446XX_USART_DRIVER_H_ */
