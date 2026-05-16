/*
 * stm32f446xx_spi_driver.h
 *
 *  Created on: Apr 8, 2026
 *      Author: chandan
 */

#ifndef INC_STM32F446XX_SPI_DRIVER_H_
#define INC_STM32F446XX_SPI_DRIVER_H_

#include "stm32f446xx.h"



/*********************************************************************
 * @SPI Configuration Structure
 *********************************************************************
 * This structure is used to configure the SPI peripheral.
 *
 * User application fills this structure and passes it to SPI_Init().
 *
 * Each member of this structure represents one configurable feature
 * of the SPI peripheral such as:
 *
 *    - Device mode (Master/Slave)
 *    - Bus configuration
 *    - Clock speed
 *    - Data frame format
 *    - Clock polarity
 *    - Clock phase
 *    - Software slave management
 *
 *********************************************************************/
typedef struct
{
	uint8_t SPI_DeviceMode;     /*!< Specifies SPI device mode.
									 Possible values from @SPI_DeviceMode */

	uint8_t SPI_BusConfig;      /*!< Specifies SPI bus configuration.
									 Possible values from @SPI_BusConfig */

	uint8_t SPI_SclkSpeed;      /*!< Specifies SPI serial clock speed
									 (baud rate).
									 Possible values from @SPI_SclkSpeed */

	uint8_t SPI_DFF;            /*!< Specifies SPI Data Frame Format.
									 Possible values from @SPI_DFF */

	uint8_t SPI_CPOL;           /*!< Specifies clock polarity.
									 Possible values from @SPI_CPOL */

	uint8_t SPI_CPHA;           /*!< Specifies clock phase.
									 Possible values from @SPI_CPHA */

	uint8_t SPI_SSM;            /*!< Specifies Software Slave
									 Management configuration.
									 Possible values from @SPI_SSM */

}SPI_Config_t;



/*********************************************************************
 * @SPI Handle Structure
 *********************************************************************
 * This structure is used by the SPI driver to manage:
 *
 *    - SPI peripheral information
 *    - SPI configuration settings
 *    - Transmission state
 *    - Reception state
 *    - Application Tx/Rx buffers
 *    - Remaining Tx/Rx lengths
 *
 * This structure acts like a software object/handle for one SPI
 * peripheral instance.
 *
 * Example:
 *
 *    SPI_Handle_t SPI2Handle;
 *
 *********************************************************************/
typedef struct
{
	SPI_RegDef_t *pSPIx;        /*!< Holds the base address of the
									 SPI peripheral (SPI1/SPI2/SPI3...) */

	SPI_Config_t SPIConfig;     /*!< Holds SPI configuration settings */

	uint8_t *pTxBuffer;         /*!< Pointer to application transmit
									 buffer */

	uint8_t *pRxBuffer;         /*!< Pointer to application receive
									 buffer */

	uint32_t TxLen;             /*!< Stores remaining number of bytes
									 to transmit */

	uint32_t RxLen;             /*!< Stores remaining number of bytes
									 to receive */

	uint8_t TxState;            /*!< Stores current transmission state */

	uint8_t RxState;            /*!< Stores current reception state */

}SPI_Handle_t;






/*********************************************************************
 * @SPI_Application_States
 *********************************************************************
 * These macros indicate the current status/state of SPI communication.
 *
 * Used mainly in Interrupt-based APIs to avoid collision between
 * multiple transmissions/receptions.
 *
 *********************************************************************/
#define SPI_READY							0U
#define SPI_BUSY_IN_RX						1U
#define SPI_BUSY_IN_TX						2U
//#define SPI_ERROR_INVALID_LENGTH			3U //[WAP]



/*********************************************************************
 * @SPI_Application_Events
 *********************************************************************
 * These macros are used by the SPI driver to notify the application
 * about communication events through callback function:
 *
 *    SPI_ApplicationEventCallback()
 *
 *********************************************************************/
#define SPI_EVENT_TX_CMPLT					1U
#define SPI_EVENT_RX_CMPLT					2U
#define SPI_EVENT_OVR_ERR					3U
#define SPI_EVENT_CRC_ERR					4U




/*********************************************************************
 * @SPI_DeviceMode
 *********************************************************************
 * Possible SPI device operating modes.
 *
 * SPI can operate either as:
 *
 *    - Master
 *    - Slave
 *
 * Controlled using MSTR bit in SPI_CR1 register.
 *
 * After reset, SPI defaults to Slave mode.
 *
 *********************************************************************/
#define SPI_DEVICE_MODE_SLAVE  				0U
#define SPI_DEVICE_MODE_MASTER				1U




/*********************************************************************
 * @SPI_BusConfig
 *********************************************************************
 * Possible SPI bus configurations.
 *
 * SPI supports:
 *
 *    - Full Duplex
 *    - Half Duplex
 *    - Simplex Receive Only
 *
 * Controlled using:
 *
 *    - BIDIMODE
 *    - BIDIOE
 *    - RXONLY
 *
 * bits in SPI_CR1 register.
 *
 *********************************************************************/
#define SPI_BUS_CONFIG_FULL_DUPLEX			1U
#define SPI_BUS_CONFIG_HALF_DUPLEX			2U
#define SPI_BUS_CONFIG_SIMPLEX_RXONLY		3U




/*********************************************************************
 * @SPI_SclkSpeed
 *********************************************************************
 * SPI Serial Clock Speed (Baud Rate)
 *
 * SPI clock is derived from peripheral clock using prescaler divider.
 *
 * SPI Clock = Peripheral Clock / Divider
 *
 * Controlled using BR bit field in SPI_CR1 register.
 *
 *********************************************************************/
#define SPI_SCLK_SPEED_DIV2					0U
#define SPI_SCLK_SPEED_DIV4					1U
#define SPI_SCLK_SPEED_DIV8					2U
#define SPI_SCLK_SPEED_DIV16				3U
#define SPI_SCLK_SPEED_DIV32				4U
#define SPI_SCLK_SPEED_DIV64				5U
#define SPI_SCLK_SPEED_DIV128				6U
#define SPI_SCLK_SPEED_DIV256				7U




/*********************************************************************
 * @SPI_DFF
 *********************************************************************
 * SPI Data Frame Format
 *
 * SPI supports:
 *
 *    - 8-bit communication
 *    - 16-bit communication
 *
 * Controlled using DFF bit in SPI_CR1 register.
 *
 *********************************************************************/
#define SPI_DFF_8BITS						0U
#define SPI_DFF_16BITS						1U




/*********************************************************************
 * @SPI_CPOL
 *********************************************************************
 * SPI Clock Polarity
 *
 * Controls idle state of serial clock line.
 *
 * CPOL = 0 --> Clock idle state LOW
 * CPOL = 1 --> Clock idle state HIGH
 *
 * Controlled using CPOL bit in SPI_CR1 register.
 *
 *********************************************************************/
#define SPI_CPOL_LOW						0U
#define SPI_CPOL_HIGH						1U




/*********************************************************************
 * @SPI_CPHA
 *********************************************************************
 * SPI Clock Phase
 *
 * Controls at which clock edge data is captured/sample.
 *
 * CPHA = 0 --> Data captured at first clock transition
 * CPHA = 1 --> Data captured at second clock transition
 *
 * Controlled using CPHA bit in SPI_CR1 register.
 *
 *********************************************************************/
#define SPI_CPHA_LOW						0U
#define SPI_CPHA_HIGH						1U




/*********************************************************************
 * @SPI_SSM
 *********************************************************************
 * SPI Software Slave Management
 *
 * Controls whether NSS signal is managed:
 *
 *    - By hardware
 *    - By software
 *
 * Controlled using SSM bit in SPI_CR1 register.
 *
 *********************************************************************/
#define SPI_SSM_DI							0U
#define SPI_SSM_EN							1U




/*********************************************************************
 * @SPI_Status_Flags
 *********************************************************************
 * These macros provide masking values for SPI status register (SR)
 * flags.
 *
 * Used with SPI_GetFlagStatus().
 *
 *********************************************************************/
#define SPI_FLAG_TXE						(1U << SPI_SR_TXE)
#define SPI_FLAG_RXNE						(1U << SPI_SR_RXNE)
#define SPI_FLAG_BUSY						(1U << SPI_SR_BSY)





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
void SPI_PeriClockControl(SPI_RegDef_t *pSPIx,
						  uint8_t EnorDi);




/*********************************************************************
 * Initialization and De-Initialization APIs
 *********************************************************************/
void SPI_Init(SPI_Handle_t *pSPIHandle);

void SPI_DeInit(SPI_RegDef_t *pSPIx);




/*********************************************************************
 * Data Transmission and Reception APIs
 *********************************************************************/

/*
 * Blocking / Polling based APIs
 */
void SPI_SendData(SPI_RegDef_t *pSPIx,
				  uint8_t *pTxBuffer,
				  uint32_t Len);

void SPI_ReceiveData(SPI_RegDef_t *pSPIx,
					 uint8_t *pRxBuffer,
					 uint32_t Len);

void SPI_TransmitReceiveData(SPI_RegDef_t *pSPIx,
							 uint8_t *pTxBuffer,
							 uint8_t *pRxBuffer,
							 uint32_t Len);



/*
 * Non-Blocking / Interrupt based APIs
 */
uint8_t SPI_SendDataIT(SPI_Handle_t *pSPIHandle,
					   uint8_t *pTxBuffer,
					   uint32_t Len);

uint8_t SPI_ReceiveDataIT(SPI_Handle_t *pSPIHandle,
						  uint8_t *pRxBuffer,
						  uint32_t Len);




/*********************************************************************
 * IRQ Configuration and ISR Handling APIs
 *********************************************************************/
void SPI_IRQInterruptConfig(uint8_t IRQNumber,
							uint8_t EnorDi);

void SPI_IRQPriorityConfig(uint8_t IRQNumber,
						   uint32_t IRQPriority);

void SPI_IRQHandling(SPI_Handle_t *pSPIHandle);




/*********************************************************************
 * SPI Peripheral Control APIs
 *********************************************************************/
void SPI_PeripheralControl(SPI_RegDef_t *pSPIx,
						   uint8_t EnOrDi);

void SPI_SSIConfig(SPI_RegDef_t *pSPIx,
				   uint8_t EnOrDi);

void SPI_SSOEConfig(SPI_RegDef_t *pSPIx,
					uint8_t EnOrDi);

uint8_t SPI_GetFlagStatus(SPI_RegDef_t *pSPIx,
						  uint8_t FlagName);

void SPI_ClearOVRFlag(SPI_RegDef_t *pSPIx);

void SPI_CloseTransmission(SPI_Handle_t *pSPIHandle);

void SPI_CloseReception(SPI_Handle_t *pSPIHandle);




/*********************************************************************
 * SPI Application Callback
 *********************************************************************
 * Application may override this callback function to handle:
 *
 *    - Transmission complete event
 *    - Reception complete event
 *    - Overrun error event
 *    - CRC error event
 *
 *********************************************************************/
void SPI_ApplicationEventCallback(SPI_Handle_t *pSPIHandle,
								  uint8_t AppEvent);


#endif /* INC_STM32F446XX_SPI_DRIVER_H_ */
