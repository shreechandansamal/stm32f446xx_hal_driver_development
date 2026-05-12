/*
 * stm32f446xx_i2c_driver.h
 *
 *  Created on: May 12, 2026
 *      Author: chandan
 */

#ifndef INC_STM32F446XX_I2C_DRIVER_H_
#define INC_STM32F446XX_I2C_DRIVER_H_

#include "stm32f446xx.h"



/*********************************************************************
 * @I2C Configuration Structure
 *********************************************************************
 * This structure is used to configure the I2C peripheral.
 *
 * User application fills this structure and passes it to I2C_Init().
 *
 * Each member of this structure represents one configurable feature
 * of the I2C peripheral such as:
 *
 *    - SCL Speed
 *    - Own Slave Address (if device acts as slave)
 *    - ACK Control (because default ACK bit is disable user can enable it)
 *    - Duty Cycle
 *
 *********************************************************************/
typedef struct
{
	uint32_t I2C_SCLSpeed;
	uint8_t  I2C_DeviceAddress;
	uint8_t  I2C_ACKControl;
	uint16_t I2C_FMDutyCycle;
}I2C_Config_t;



/*********************************************************************
 * @I2C Handle Structure
 *********************************************************************
 * This structure is used by the I2C driver to manage:
 *
 *    - I2C peripheral information
 *    - I2C configuration settings
 *
 * This structure acts like a software object/handle for one I2C
 * peripheral instance.
 *
 * Example:
 *
 *    I2C_Handle_t I2C_Handle;
 *
 *********************************************************************/
typedef struct
{
	I2C_RegDef_t *pI2Cx;
	I2C_Config_t I2C_Config;
}I2C_Handle_t;





/*********************************************************************
 * @I2C_SCLSpeed
 *********************************************************************
 * These macros indicate the speed of SCL line speed
 *
 * 	-I2C_SCL_SPEED_SM means Standard Speed Mode is 100KHz
 * 	-I2C_SCL_SPEED_FM4K means Fast Speed Mode with speed 400KHz
 * 	-I2C_SCL_SPEED_FM2K means Fast Speed Mode with speed 200KHz
 *
 *********************************************************************/
#define I2C_SCL_SPEED_SM 					100000
#define I2C_SCL_SPEED_FM4K 					400000
#define I2C_SCL_SPEED_FM2K 					200000




/*********************************************************************
 * @I2C_ACKControl
 *********************************************************************
 * These macros indicate the ACK Enable or Disable
 *
 *********************************************************************/
#define I2C_ACK_EN							1
#define I2C_ACK_DI							0




/*********************************************************************
 * @I2C_FMDutyCycle
 *********************************************************************
 * These macros indicate the Duty Cycle in Fast Mode
 *
 *********************************************************************/
#define I2C_FM_DUTY_2						0
#define I2C_FM_DUTY_16_9					1









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
void I2C_PeriClockControl(I2C_RegDef_t *pI2Cx,
						  uint8_t EnorDi);




/*********************************************************************
 * Initialization and De-Initialization APIs
 *********************************************************************/
void I2C_Init(I2C_Handle_t *pI2CHandle);

void I2C_DeInit(I2C_RegDef_t *pI2Cx);




/*********************************************************************
 * Data Transmission and Reception APIs
 *********************************************************************/





/*********************************************************************
 * IRQ Configuration and ISR Handling APIs
 *********************************************************************/
void I2C_IRQInterruptConfig(uint8_t IRQNumber,
							uint8_t EnorDi);

void I2C_IRQPriorityConfig(uint8_t IRQNumber,
						   uint32_t IRQPriority);




/*********************************************************************
 * I2C Peripheral Control APIs
 *********************************************************************/
void I2C_PeripheralControl(I2C_RegDef_t *pI2Cx,
						   uint8_t EnOrDi);

uint8_t I2C_GetFlagStatus(I2C_RegDef_t *pI2Cx,
						  uint8_t FlagName);





/*********************************************************************
 * I2C Application Callback
 *********************************************************************
 * Application may override this callback function to handle:
 *
 *    - Transmission complete event
 *    - Reception complete event
 *    - Overrun error event
 *    - CRC error event
 *
 *********************************************************************/
void I2C_ApplicationEventCallback(I2C_Handle_t *pI2CHandle,
								  uint8_t AppEvent);

















#endif /* INC_STM32F446XX_I2C_DRIVER_H_ */
