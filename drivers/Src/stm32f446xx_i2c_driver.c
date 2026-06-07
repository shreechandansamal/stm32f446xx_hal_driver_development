/*
 * stm32f446xx_i2c_driver.c
 *
 *  Created on: May 12, 2026
 *      Author: chandan
 */

#include "stm32f446xx_i2c_driver.h"
#include "stm32f446xx_rcc_driver.h"


/********************************************************************
 * @Note- private helper macro section
 */
#define I2C_DIR_WRITE			0U
#define I2C_DIR_READ			1U

/*********************************************************************/



/********************************************************************
 * @Note- private helper function section
 *
 * @brief  - don't declare these prototypes in the header because
 * 			 these are private functions and we don't want to expose
 * 			 it to user application
 *
 * use static keyword to indicate these are private helper function,
 * so if application tries to call them, then compiler will issue an
 * error.
 */
static void I2C_GenerateStartCondition(I2C_RegDef_t *pI2Cx);
static void I2C_ExecuteAddressPhase(I2C_RegDef_t *pI2Cx,
							        uint8_t SlaveAddr,
									uint8_t TransferDir);
static void I2C_ClearADDRFlag(I2C_Handle_t *pI2CHandle);
static void I2C_MasterHandleTXEInterrupt(I2C_Handle_t *pI2CHandle);
static void I2C_MasterHandleRXNEInterrupt(I2C_Handle_t *pI2CHandle);


/*********************************************************************/



/*********************************************************************
 * @fn                - I2C_PeriClockControl
 *
 * @brief             - Enable or Disable the peripheral clock for
 *                      the given I2C peripheral.
 *
 * @param[in]         - pI2Cx    : Base address of the I2C peripheral
 * @param[in]         - EnOrDi   : ENABLE or DISABLE macros
 *
 * @return            - none
 *
 * @Note              -
 *                      1. STM32 peripherals are connected to
 *                         different bus domains such as APB1/APB2.
 *
 *                      2. By default, peripheral clocks are disabled
 *                         after reset in order to reduce power
 *                         consumption.
 *
 *                      3. Before accessing or configuring any I2C
 *                         register, the corresponding peripheral
 *                         clock must be enabled.
 *
 *                      4. Accessing peripheral registers while
 *  					   the clock is disabled may produce
 *  					   undefined results:
 *                           - Register access may not work properly
 *                           - Peripheral configuration will fail
 *                           - I2C communication will not operate
 *
 *                      5. I2C1, I2C2 and I2C3 are connected to APB1 bus.
 *
 *                      6. Disabling the peripheral clock stops the
 *                         clock supply to the I2C peripheral and
 *                         reduces dynamic power consumption.
 *
 *                      7. Peripheral clock should not be disabled
 *                         during an active I2C communication.
 *
 */
void I2C_PeriClockControl(I2C_RegDef_t *pI2Cx,
						  uint8_t EnorDi)
{
    if(EnorDi == ENABLE)
    {
        if(pI2Cx == I2C1)
        {
            I2C1_PCLK_EN();
        }
        else if(pI2Cx == I2C2)
        {
            I2C2_PCLK_EN();
        }
        else if(pI2Cx == I2C3)
        {
            I2C3_PCLK_EN();
        }
    }
    else
    {
        if(pI2Cx == I2C1)
        {
            I2C1_PCLK_DI();
        }
        else if(pI2Cx == I2C2)
        {
            I2C2_PCLK_DI();
        }
        else if(pI2Cx == I2C3)
        {
            I2C3_PCLK_DI();
        }
    }
}



/*********************************************************************
 * @fn                - I2C_Init
 *
 * @brief             - Initialize the I2C peripheral with the
 *                      user-defined configuration parameters.
 *
 * @param[in]         - pI2CHandle : Pointer to I2C handle structure
 *                                   containing I2C peripheral base
 *                                   address and configuration data.
 *
 * @return            - none
 *
 * @Note              -
 *                      1. This function configures the I2C peripheral
 *                         registers according to the user-provided
 *                         configuration.
 *
 *                      2. Before configuring the I2C peripheral,
 *                         the corresponding peripheral clock is
 *                         enabled internally.
 *
 *                      3. This function only configures the I2C
 *                         peripheral registers.
 *                         It does NOT enable the I2C peripheral.
 *
 *                      4. The I2C peripheral is enabled separately
 *                         using:
 *
 *                              I2C_PeripheralControl()
 *
 *                      5. The following I2C parameters are configured:
 *
 *                           - Peripheral Input Clock Frequency
 *                               CR2[FREQ]
 *
 *                           - Device Own Address
 *                               OAR1[ADD]
 *
 *                           - Serial Clock Speed (SCL)
 *                               CCR Register
 *
 *                           - Fast Mode Duty Cycle
 *                               CCR[DUTY]
 *
 *                           - Maximum Rise Time
 *                               TRISE Register
 *
 *                      6. The APB1 peripheral clock frequency is
 *                         obtained dynamically using:
 *
 *                              RCC_GetPCLK1Value()
 *
 *                         This value is required for CCR and TRISE
 *                         calculations.
 *
 *                      7. Own Address configuration is required when
 *                         the device operates as an I2C slave and is
 *                         stored in the OAR1 register.
 *
 *                      8. Both I2C operating modes are supported:
 *
 *                           - Standard Mode (SM)
 *                               Up to 100 kHz
 *
 *                           - Fast Mode (FM)
 *                               Up to 400 kHz
 *
 *                      9. For Standard Mode:
 *
 *                           - CCR is calculated using:
 *
 *                                 CCR = FPCLK1 / (2 × FSCL)
 *
 *                           - SCL high and low periods are equal.
 *
 *                     10. For Fast Mode:
 *
 *                           Two duty cycle options are supported:
 *
 *                           Duty = 0 (Tlow/Thigh = 2)
 *
 *                               CCR = FPCLK1 / (3 × FSCL)
 *
 *                           Duty = 1 (Tlow/Thigh = 16/9)
 *
 *                               CCR = FPCLK1 / (25 × FSCL)
 *
 *                     11. TRISE is configured according to the
 *                         maximum rise-time requirements specified
 *                         in the I2C protocol specification.
 *
 *                           Standard Mode:
 *                               Maximum rise time = 1000 ns
 *
 *                           Fast Mode:
 *                               Maximum rise time = 300 ns
 *
 *                     12. GPIO pins used for SDA and SCL must be
 *                         configured in Alternate Function Open-Drain
 *                         mode before enabling I2C communication.
 *
 *                     13. External pull-up resistors are required
 *                         on SDA and SCL lines because I2C uses
 *                         open-drain signaling.
 *
 *                     14. The I2C peripheral must remain disabled
 *                         (PE = 0) while timing-related registers
 *                         such as CR2, CCR and TRISE are being
 *                         configured.
 *
 *                     15. This driver currently supports
 *   					   7-bit addressing mode.
 *
 */
void I2C_Init(I2C_Handle_t *pI2CHandle)
{

    uint32_t tempreg = 0U;

    //enable the clock for the I2Cx peripheral
    I2C_PeriClockControl(pI2CHandle->pI2Cx,
    					 ENABLE);

    //a. Configure the FREQ field Bit of CR2
    tempreg = 0U;
    tempreg |= RCC_GetPCLK1Value() / 1000000U; /* divide by 1MHz because
    											* to get the MSB number
    											* ex- 16MHz returned by
    											* RCC_GetPCLK1Value so
    											* divide by 1MHz gives
    											* 16, so we want this
    											*/

    pI2CHandle->pI2Cx->CR2 = (tempreg & 0x3FU); /* FREQ field is 6 bits
     	 	 	 	 	 	 	 	 	 	 	 * so we need to set
     	 	 	 	 	 	 	 	 	 	 	 * the tempreg value
     	 	 	 	 	 	 	 	 	 	 	 * into those Bit 5:0
     	 	 	 	 	 	 	 	 	 	 	 * and rest need to be
     	 	 	 	 	 	 	 	 	 	 	 * masked
     	 	 	 	 	 	 	 	 	 	 	 */

    //b. Configure the device own address
    tempreg = 0U;
    tempreg |= pI2CHandle->I2C_Config.I2C_DeviceAddress << I2C_OAR1_ADD_BIT_1_7;
    tempreg |= (1U << 14U); /*As per Reference Manual, we Should
    						  always be kept at 1 by software.*/
    pI2CHandle->pI2Cx->OAR1 = tempreg;

    //c. CCR calculation for SCL speed
    uint16_t ccr_value = 0U;
    tempreg = 0U;

    if(pI2CHandle->I2C_Config.I2C_SCLSpeed <= I2C_SCL_SPEED_SM)
    {
    	//Standard Mode(SM) (Thigh = CCR * TPCLK1, Tlow = CCR * TPCLK1)
    	ccr_value = (RCC_GetPCLK1Value() / (2U * pI2CHandle->I2C_Config.I2C_SCLSpeed));
    }
    else
    {
    	//Fast Mode(FM)
    	tempreg |= (1U << I2C_CCR_FS); //set the F/S[15] bit as FM
    	tempreg |= pI2CHandle->I2C_Config.I2C_FMDutyCycle << I2C_CCR_DUTY;
    	if(pI2CHandle->I2C_Config.I2C_FMDutyCycle == I2C_FM_DUTY_2)
    	{
    		/*Duty 0 : Thigh = CCR * TPCLK1, Tlow = 2 * CCR * TPCLK1
    		 * 		   Tscl = Thigh + Tlow
    		 * 		   Tscl = (CCR * TPCLK1) + (2 * CCR * TPCLK1)
    		 * 		   Tscl = 3 * CCR * TPCLK1
    		 *
    		 * 		   CCR = Tscl / (3 * TPCLK1)
    		 */
    		ccr_value = (RCC_GetPCLK1Value() / (3U * pI2CHandle->I2C_Config.I2C_SCLSpeed));
    	}
    	else
    	{
    		/*Duty 1 : Thigh = 9 * CCR * TPCLK1, Tlow = 16 * CCR * TPCLK1
    		 * 		   Tscl = (9 * CCR * TPCLK1) + (16 * CCR * TPCLK1)
    		 * 		   Tscl = 25 * CCR * TPCLK1
    		 *
    		 * 		   CCR = Tscl / (25 * TPCLK1)
    		 */
    		ccr_value = (RCC_GetPCLK1Value() / (25U * pI2CHandle->I2C_Config.I2C_SCLSpeed));
    	}
    }
	tempreg |= (ccr_value & 0xFFFU); //CCR[11:0]
	pI2CHandle->pI2Cx->CCR = tempreg;

    //d.Trise calculation
	tempreg = 0U;
    if(pI2CHandle->I2C_Config.I2C_SCLSpeed <= I2C_SCL_SPEED_SM)
    {
    	/*Standard Mode(SM) Trise calculation
    	 * Max rise time 1000ns
    	 * (Trise(max) / Tpclk1) + 1 (in time) or (Fpclk1 * Trise(max)) + 1 (in frequency)
    	 */
    	tempreg |= ((RCC_GetPCLK1Value() / 1000000U) + 1U);
    }
    else
    {
    	/*Fast Mode(FM) Trise calculation
    	 * Max rise time 300ns
    	 * (Trise(max) / Tpclk1) + 1 (in time) or (Fpclk1 * Trise(max)) + 1 (in frequency)
    	 */
    	tempreg |= (((RCC_GetPCLK1Value() * 300U) / 1000000000U) + 1U);
    }
    pI2CHandle->pI2Cx->TRISE = (tempreg & 0x3FU); // TRISE[5:0]

}



/*********************************************************************
 * @fn                - I2C_DeInit
 *
 * @brief             - Reset the selected I2C peripheral registers
 *                      to their hardware reset values.
 *
 * @param[in]         - pI2Cx : Base address of the I2C peripheral
 *                              (I2C1, I2C2 or I2C3)
 *
 * @return            - none
 *
 * @Note              -
 *                      1. This function forces the selected I2C
 *                         peripheral into reset state using the
 *                         RCC peripheral reset mechanism.
 *
 *                      2. All I2C control, configuration and status
 *                         registers are restored to their default
 *                         reset values defined by hardware.
 *
 *                      3. Any ongoing I2C communication is aborted
 *                         immediately when the peripheral is reset.
 *
 *                      4. After de-initialization:
 *
 *                           - Peripheral configuration is lost
 *                           - Communication is stopped
 *                           - Control registers are reset
 *                           - Status flags return to reset state
 *
 *                      5. The peripheral must be initialized again
 *                         using I2C_Init() before it can be used for
 *                         communication.
 *
 *                      6. Typical use cases:
 *
 *                           - Re-configuring the peripheral
 *                           - Recovering from bus errors
 *                           - Recovering from software faults
 *                           - Returning the peripheral to a known
 *                             clean state
 *
 *                      7. This function should not be called during
 *                         an active I2C transaction because data loss
 *                         may occur.
 *
 *                      8. GPIO configuration and NVIC
 *  					   configuration are not modified by
 *  					   this function.
 *
 *                      9. Supported peripherals:
 *
 *                           - I2C1
 *                           - I2C2
 *                           - I2C3
 *
 */
void I2C_DeInit(I2C_RegDef_t *pI2Cx)
{
    if(pI2Cx == I2C1)
    {
        I2C1_REG_RESET();
    }
    else if(pI2Cx == I2C2)
    {
        I2C2_REG_RESET();
    }
    else if(pI2Cx == I2C3)
    {
        I2C3_REG_RESET();
    }
}



/*********************************************************************
 * @fn                - I2C_AckControl
 *
 * @brief             - Enable or disable ACK generation by the I2C
 *                      peripheral.
 *
 * @param[in]         - pI2Cx   : Base address of the I2C peripheral
 *
 * @param[in]         - EnOrDi  : I2C_ACK_ENABLE
 *                                I2C_ACK_DISABLE
 *
 * @return            - none
 *
 * @Note              -
 *                      1. Controls the ACK bit in the CR1 register.
 *
 *                      2. When ACK is enabled, the peripheral
 *                         automatically generates an ACK after
 *                         receiving a data byte.
 *
 *                      3. When ACK is disabled, the peripheral
 *  					   does not acknowledge the next received
 *  					   byte, resulting in a NACK being
 *  					   transmitted on the bus.
 *
 *                      4. In Master Receiver mode, ACK is typically:
 *
 *                           - Enabled while receiving multiple bytes
 *                           - Disabled before receiving the final
 *                             byte so that a NACK is generated
 *
 *                      5. In Slave Receiver mode, ACK allows the
 *                         slave to acknowledge bytes received from
 *                         the master.
 *
 *                      6. ACK management is an important part of
 *                         multi-byte I2C reception sequences.
 *
 *                      7. Improper ACK handling may result in:
 *
 *                           - Extra bytes being received
 *                           - Premature transfer termination
 *                           - Bus communication errors
 *
 *                      8. This function only modifies the ACK bit
 *                         and does not affect any other I2C
 *                         configuration.
 *
 */
void I2C_AckControl(I2C_RegDef_t *pI2Cx,
					uint8_t EnOrDi)
{
	if (EnOrDi == I2C_ACK_ENABLE)
	{
		// Enable ACK
		pI2Cx->CR1 |= (1 << I2C_CR1_ACK);
	}
	else
	{
		// Disable ACK
		pI2Cx->CR1 &= ~(1 << I2C_CR1_ACK);
	}
}



/*********************************************************************
 * @fn                - I2C_GetFlagStatus
 *
 * @brief             - Get the status of a specific I2C status flag.
 *
 * @param[in]         - pI2Cx     : Base address of the I2C peripheral
 *
 * @param[in]         - FlagName  : I2C status flag to check
 *
 * @return            - FLAG_SET   : Flag is set
 *                      FLAG_RESET : Flag is reset
 *
 * @Note              -
 *                      1. This function checks the I2C_SR1 register
 *  					   for the specified status flag.
 *
 *                      2. I2C status flags provide information about
 *                         the current state of the peripheral and
 *                         communication progress.
 *
 *                      3. This API is commonly used in polling-based
 *                         I2C communication to wait for specific
 *                         hardware events.
 *
 *                      4. Example:
 *
 *                              while(I2C_GetFlagStatus
 *                                   (I2C2, I2C_FLAG_SB)
 *                                   == FLAG_RESET);
 *
 *                         Waits until START condition generation is
 *                         completed.
 *
 *                      5. Typical flags checked using this API:
 *
 *                           - SB      (Start Bit)
 *                           - ADDR    (Address Sent/Matched)
 *                           - BTF     (Byte Transfer Finished)
 *                           - TXE     (Transmit Data Register Empty)
 *                           - RXNE    (Receive Data Register Not Empty)
 *                           - STOPF   (Stop Detection)
 *
 *                      6. Status flags are hardware controlled and
 *                         updated automatically by the I2C peripheral.
 *
 *                      7. Some I2C flags are cleared by specific
 *                         software sequences rather than by writing
 *                         directly to the flag bit.
 *
 *                         Example:
 *                           - ADDR is cleared by reading SR1
 *                             followed by SR2.
 *
 *                      8. This function only reads the flag status
 *                         and does not modify any peripheral register.
 *
 */
uint8_t I2C_GetFlagStatus(I2C_RegDef_t *pI2Cx,
                          uint8_t FlagName)
{
    if(pI2Cx->SR1 & FlagName)
    {
        return FLAG_SET;
    }

    return FLAG_RESET;
}



/*********************************************************************
 * @fn                - I2C_GenerateStartCondition
 *
 * @brief             - Generate an I2C START condition.
 *
 * @param[in]         - pI2Cx : Base address of the I2C peripheral
 *
 * @return            - none
 *
 * @Note              -
 *                      1. Sets the START bit in the CR1 register.
 *
 *                      2. When the START bit is set, the I2C
 *                         peripheral generates a START condition
 *                         on the bus.
 *
 *                      3. After the START condition is generated,
 *                         the SB (Start Bit) flag in SR1 becomes set.
 *
 *                      4. Software must wait for the SB flag before
 *                         proceeding to the address phase.
 *
 *                      5. After START generation, software should
 *  	   	   	   	   	   clear the SB flag by performing the
 *  	   	   	   	   	   address phase.
 *
 *  	   	   	   	   	   Until the address phase begins, the I2C
 *  	   	   	   	   	   peripheral may hold the communication
 *  	   	   	   	   	   sequence and prevent further bus progress.
 *
 *                      6. The START bit is cleared automatically by
 *                         hardware after the START condition has been
 *                         generated.
 *
 *                      7. Whether the generated START becomes an
 *                         Initial START or a Repeated START depends
 *                         on the current bus state.
 *
 *                      8. This helper function only requests START
 *  	   	   	   	   	   generation by setting the START bit.
 *
 *  	   	   	   	   	   Actual START timing is handled by the
 *  	   	   	   	   	   I2C hardware.
 *
 */
static void I2C_GenerateStartCondition(I2C_RegDef_t *pI2Cx)
{
	pI2Cx->CR1 |= (1U << I2C_CR1_START);
}



/*********************************************************************
 * @fn                - I2C_GenerateStopCondition
 *
 * @brief             - Generate a STOP condition on the I2C bus.
 *
 * @param[in]         - pI2Cx : Base address of the I2C peripheral
 *
 * @return            - none
 *
 * @Note              -
 *                      1. This function sets the STOP bit in the
 *                         I2C_CR1 register.
 *
 *                      2. When the STOP bit is set, the I2C
 *                         peripheral generates a STOP condition
 *                         on the bus after the current transfer
 *                         is completed.
 *
 *                      3. A STOP condition indicates the end of the
 *                         current I2C transaction.
 *
 *                      4. After a STOP condition is generated:
 *
 *                           - The bus is released.
 *                           - SDA transitions from LOW to HIGH
 *                             while SCL remains HIGH.
 *                           - Other masters may acquire the bus
 *                             in multi-master systems.
 *
 *                      5. In Master Transmitter mode, STOP is
 *                         typically generated after:
 *
 *                           - TXE = 1
 *                           - BTF = 1
 *
 *                         indicating that all data bytes have been
 *                         completely transmitted.
 *
 *                      6. In Master Receiver mode, STOP is usually
 *                         generated before reception of the final
 *                         byte according to the STM32 I2C receive
 *                         procedure.
 *
 *                      7. If a STOP condition is NOT generated,
 *                         the master retains ownership of the bus.
 *
 *                      8. Retaining control of the bus allows the
 *                         application to generate another START
 *                         condition without releasing the bus.
 *
 *                      9. A START condition generated without a
 *                         preceding STOP condition results in a
 *                         Repeated START (Sr) condition.
 *
 *                     10. Repeated START is commonly used when:
 *
 *                           - Writing a register address and then
 *                             reading data from the same slave.
 *
 *                           - Performing back-to-back transactions
 *                             without releasing the bus.
 *
 *                     11. This function only requests STOP
 *                         generation. Actual bus timing is handled
 *                         automatically by the I2C hardware.
 *
 *                     12. If STOP is not generated after a transfer,
 *                         software may generate another START
 *                         condition which results in a Repeated
 *                         START sequence on the bus.
 *
 */
void I2C_GenerateStopCondition(I2C_RegDef_t *pI2Cx)
{
	pI2Cx->CR1 |= (1U << I2C_CR1_STOP);
}



/*********************************************************************
 * @fn                - I2C_ExecuteAddressPhase
 *
 * @brief             - Send the slave address and transfer direction
 *                      (Read/Write) during the I2C address phase.
 *
 * @param[in]         - pI2Cx       : Base address of the I2C peripheral
 *
 * @param[in]         - SlaveAddr   : 7-bit slave address
 *
 * @param[in]         - TransferDir : Transfer direction
 *                                    I2C_DIR_WRITE
 *                                    I2C_DIR_READ
 *
 * @return            - none
 *
 * @Note              -
 *                      1. In 7-bit addressing mode, the address
 *                         phase transmitted on the bus consists of:
 *
 *                              [A6:A0][R/W]
 *
 *                         where:
 *                              A6:A0 = Slave Address
 *                              R/W   = Transfer Direction
 *
 *                      2. The slave address occupies bits [7:1]
 *                         of the address frame.
 *
 *                      3. The least significant bit (LSB) is the
 *                         Read/Write bit:
 *
 *                              0 -> Write Operation
 *                              1 -> Read Operation
 *
 *                      4. This function prepares the complete
 *                         address frame by:
 *
 *                              - Left shifting the 7-bit slave
 *                                address by one position.
 *
 *                              - Inserting the transfer direction
 *                                into bit position [0].
 *
 *                      5. The completed address frame is written
 *                         into the I2C Data Register (DR) for
 *                         transmission on the bus.
 *
 *                      6. After the address frame is transmitted:
 *
 *                           - Master Mode:
 *                               ADDR flag is set by hardware after
 *                               the addressed slave acknowledges
 *                               the address phase.
 *
 *                           - Slave Mode:
 *                               ADDR flag is set when the received
 *                               address matches one of the slave's
 *                               configured own addresses.
 *
 *                      7. Software must clear the ADDR flag using
 *                         the required software sequence before
 *                         proceeding with data transfer.
 *
 *                      8. This helper function is intended for
 *                         7-bit addressing mode only.
 *
 *                      9. This function is typically called after
 *                         successful START condition generation and
 *                         SB flag detection.
 *
 */
static void I2C_ExecuteAddressPhase(I2C_RegDef_t *pI2Cx,
							        uint8_t SlaveAddr,
									uint8_t TransferDir)
{
	SlaveAddr <<= 1U; /*same as (SlaveAddr = SlaveAddr << 1U), shift
	     	 	 	 	the 7 bits address moved by 1 bit to the MSB*/
	SlaveAddr |= TransferDir; // LSB is R/nW bit 1 = READ, 0 = WRITE
	pI2Cx->DR = SlaveAddr;
}



/*********************************************************************
 * @fn                - I2C_ClearADDRFlag
 *
 * @brief             - Clear the ADDR flag according to the STM32
 *                      I2C hardware clearing sequence.
 *
 * @param[in]         - pI2CHandle : Pointer to I2C handle structure
 *                                   containing peripheral instance
 *                                   and transfer state information.
 *
 * @return            - none
 *
 * @Note              -
 *                      1. The ADDR flag is cleared by a specific
 *                         software sequence:
 *
 *                              Read SR1
 *                              followed by
 *                              Read SR2
 *
 *                      2. Until ADDR is cleared, the peripheral
 *                         stretches SCL LOW and communication
 *                         cannot continue.
 *
 *                      3. This function automatically performs the
 *                         required flag clearing sequence.
 *
 *                      4. During Master Reception of a single byte,
 *                         ACK must be cleared before ADDR is cleared.
 *
 *                      5. For this reason, when:
 *
 *                              TxRxState = I2C_BUSY_IN_RX
 *                              RxSize    = 1
 *
 *                         the function first disables ACK and then
 *                         performs the ADDR clearing sequence.
 *
 *                      6. For all other cases, the function simply
 *                         clears ADDR using the SR1/SR2 read
 *                         sequence.
 *
 *                      7. Applicable in both Master and Slave modes.
 *
 */
static void I2C_ClearADDRFlag(I2C_Handle_t *pI2CHandle)
{
	//check he device mode
	if(pI2CHandle->pI2Cx->SR2 & (1U << I2C_SR2_MSL)) //Bug: here ADDR is getting clear, so need to fix this Bug ASAP
	{
		//device is in master mode
		if (pI2CHandle->TxRxState == I2C_BUSY_IN_RX)
		{
			if(pI2CHandle->RxSize == 1U)
			{
				//a. disable ACK
				I2C_AckControl(pI2CHandle->pI2Cx,
							   DISABLE);

				//b. clear ADDR flag (read SR1, read SR2)
				(void)pI2CHandle->pI2Cx->SR1;
				(void)pI2CHandle->pI2Cx->SR2;
			}
		}
		else
		{
			//clear ADDR flag (read SR1, read SR2)
			(void)pI2CHandle->pI2Cx->SR1;
			(void)pI2CHandle->pI2Cx->SR2;
		}
	}
	else
	{
		//device is in slave mode
		//clear ADDR flag (read SR1, read SR2)
		(void)pI2CHandle->pI2Cx->SR1;
		(void)pI2CHandle->pI2Cx->SR2;
	}
}



/*********************************************************************
 * @fn                - I2C_PeripheralControl
 *
 * @brief             - Enable or disable the I2C peripheral using
 *                      the PE (Peripheral Enable) bit.
 *
 * @param[in]         - pI2Cx   : Pointer to the target I2C peripheral
 *                                instance.
 *
 * @param[in]         - EnOrDi  : ENABLE
 *                                DISABLE
 *
 * @return            - none
 *
 * @Note              -
 *                      1. Controls the PE (Peripheral Enable) bit
 *                         in the CR1 register.
 *
 *                      2. When enabled, the I2C peripheral becomes
 *                         operational and can participate in bus
 *                         communication.
 *
 *                      3. When disabled, the I2C peripheral stops
 *                         all communication activity and ignores
 *                         most protocol-related operations.
 *
 *                      4. Peripheral configuration registers such as
 *                         CR2, CCR and TRISE should be configured
 *                         before enabling the peripheral.
 *
 *                      5. Timing-related configuration must not be
 *                         modified while the peripheral is actively
 *                         communicating on the bus.
 *
 *                      6. The ACK bit becomes effective only when
 *                         the peripheral is enabled (PE = 1).
 *
 *                      7. Therefore, if ACK functionality is
 *                         required, the typical sequence is:
 *
 *                              I2C_Init()
 *                              I2C_PeripheralControl(ENABLE)
 *                              I2C_AckControl(ENABLE)
 *
 *                      8. Disabling the peripheral does not reset
 *                         its configuration registers.
 *
 *                      9. To completely restore the peripheral to
 *                         its hardware reset state, use:
 *
 *                              I2C_DeInit()
 *
 *                     10. This function only controls the PE bit and
 *                         does not modify any other I2C configuration.
 *
 */
void I2C_PeripheralControl(I2C_RegDef_t *pI2Cx,
						   uint8_t EnOrDi)
{
	if(EnOrDi == ENABLE)
	{
		// Enable I2C peripheral
		pI2Cx->CR1 |= (1U << I2C_CR1_PE);

		/* Note: Enable ACK, this can not be set before PE = 1
		 *       in the app after calling I2C_PeripheralControl
		 *       as ENABLE call I2C_AckControl as ENABLE or
		 *       DISABLE neither it wont affect the ACK bit.*/
	}
	else
	{
		// Disable I2C peripheral
		pI2Cx->CR1 &= ~(1U << I2C_CR1_PE);
	}

}



/*********************************************************************
 * @fn                - I2C_MasterSendData
 *
 * @brief             - Transmit data as an I2C Master using the
 *                      polling (blocking) method.
 *
 * @param[in]         - pI2CHandle : Pointer to I2C handle structure
 *                                   containing peripheral instance
 *                                   and configuration information.
 *
 * @param[in]         - pTxBuffer  : Pointer to transmit data buffer.
 *
 * @param[in]         - Len        : Number of bytes to transmit.
 *
 * @param[in]         - SlaveAddr  : 7-bit slave address.
 *
 * @param[in]         - RepeatedStart :
 *                                   I2C_SR_DISABLE
 *                                   I2C_SR_ENABLE
 *
 * @return            - none
 *
 * @Note              -
 *                      1. This API performs a complete I2C Master
 *                         transmission using polling.
 *
 *                      2. The function blocks execution until the
 *                         entire transmission is completed.
 *
 *                      3. Transmission sequence:
 *
 *                           START
 *                              |
 *                           Address + Write
 *                              |
 *                           Data Bytes
 *                              |
 *                       STOP or Repeated START
 *
 *                      4. The START condition is generated first and
 *                         the function waits until the SB flag is set.
 *
 *                      5. The slave address is transmitted with the
 *                         R/W bit cleared (Write operation).
 *
 *                      6. After address transmission, the function
 *                         waits for the ADDR flag and clears it using
 *                         the required software sequence.
 *
 *                      7. Data transmission begins only after the
 *                         address phase has completed successfully.
 *
 *                      8. For each byte:
 *
 *                           - Wait for TXE = 1
 *                           - Write data into DR
 *                           - Advance buffer pointer
 *
 *                      9. TXE indicates that the Data Register (DR)
 *                         is empty and ready to accept the next byte.
 *
 *                     10. After all bytes have been written, the
 *                         function waits for:
 *
 *                              TXE = 1
 *                              BTF = 1
 *
 *                         before terminating the transfer.
 *
 *                     11. BTF (Byte Transfer Finished) indicates:
 *
 *                           - DR is empty
 *                           - Shift Register is empty
 *                           - Last byte transmission is completed
 *
 *                     12. Waiting for BTF ensures that the final
 *                         byte has actually left the peripheral
 *                         before a STOP condition is generated.
 *
 *                     13. If RepeatedStart == I2C_SR_DISABLE:
 *
 *                           - STOP condition is generated.
 *                           - Bus transaction terminates.
 *
 *                     14. If RepeatedStart == I2C_SR_ENABLE:
 *
 *                           - STOP condition is not generated.
 *                           - Bus ownership is retained.
 *                           - Another START condition may be
 *                             generated immediately.
 *
 *                     15. Repeated START is commonly used in
 *                         register-based slave devices where a
 *                         write phase is followed by a read phase
 *                         without releasing the bus.
 *
 *                     16. This API assumes:
 *
 *                           - Peripheral clock is enabled.
 *                           - I2C peripheral is initialized.
 *                           - I2C peripheral is enabled.
 *                           - SDA/SCL GPIO pins are configured.
 *
 *                     17. This function is intended for Master mode
 *                         operation only.
 *
 *                     18. Since this is a polling API, CPU execution
 *                         remains blocked until the transfer is
 *                         completed.
 *
 */
void I2C_MasterSendData(I2C_Handle_t *pI2CHandle,
						uint8_t *pTxBuffer,
						uint32_t Len,
						uint8_t SlaveAddr,
						uint8_t RepeatedStart)
{
	//1. Generate the START Condition
	I2C_GenerateStartCondition(pI2CHandle->pI2Cx);

	/*2. Confirm that START Generation is completed by checking the SB flag in the SR1
	     Note: Until SB is cleared SCL will be stretched (pulled to LOW) */
	while(I2C_GetFlagStatus(pI2CHandle->pI2Cx,
							I2C_FLAG_SB) == FLAG_RESET);

	//3. Send the address of the slave with r/nw bit to WRITE
	I2C_ExecuteAddressPhase(pI2CHandle->pI2Cx,
							SlaveAddr, I2C_DIR_WRITE);

	//4. Confirm that address phase is completed by checking the ADDR flag in the SR1
	while(I2C_GetFlagStatus(pI2CHandle->pI2Cx,
							I2C_FLAG_ADDR) == FLAG_RESET);

	/*5. Clear the ADDR flag according to its software sequence
	 	 Note: Until ADDR is cleared SCL will be stretched (pulled to LOW) */
	I2C_ClearADDRFlag(pI2CHandle);

	//6. Send the data until Len becomes 0
	while(Len > 0)
	{
		//wait until TXE is set
		while(I2C_GetFlagStatus(pI2CHandle->pI2Cx,
								I2C_FLAG_TXE) == FLAG_RESET); //Check if write data register is empty
		//LOAD 1 Data Byte from pTxBuffer into DR register
		pI2CHandle->pI2Cx->DR = *pTxBuffer;
		//Move 1 step for Next Byte from pTxBuffer
		pTxBuffer++;
		//Reduce length by 1 byte
		Len--;
	}

	/*7. When Len becomes zero wait for TXE=1 and BTF=1 before generating the STOP
	 *   Condition.
	 *   Note: TXE=1, BTF=1, means that both SR and DR are empty and next transmission
	 *   should begin
	 *   when BTF=1 SCL will be stretched (pulled LOW)
	 */
	while(I2C_GetFlagStatus(pI2CHandle->pI2Cx,
							I2C_FLAG_TXE) == FLAG_RESET); //Check if write data register is empty

	while(I2C_GetFlagStatus(pI2CHandle->pI2Cx,
							I2C_FLAG_BTF) == FLAG_RESET); //Check if last byte transmission is completed


	/*8. Generate STOP Condition and Master need to wait for the completion of STOP
	 * 	 Condition.
	 * 	 Note: Generating STOP, automatically clears the BTF.
	 */
	if(RepeatedStart == I2C_SR_DISABLE)
	{
		I2C_GenerateStopCondition(pI2CHandle->pI2Cx);
	}

}



/*********************************************************************
 * @fn                - I2C_MasterSendDataIT
 *
 * @brief             - Transmit data as an I2C Master using the
 *                      interrupt-driven (non-blocking) method.
 *
 * @param[in]         - pI2CHandle : Pointer to I2C handle structure
 *                                   containing peripheral instance
 *                                   and runtime transfer state.
 *
 * @param[in]         - pTxBuffer  : Pointer to transmit data buffer.
 *
 * @param[in]         - Len        : Number of bytes to transmit.
 *
 * @param[in]         - SlaveAddr  : 7-bit slave address.
 *
 * @param[in]         - RepeatedStart :
 *                                   I2C_SR_DISABLE
 *                                   I2C_SR_ENABLE
 *
 * @return            - Current I2C communication state:
 *
 *                        I2C_READY
 *                        I2C_BUSY_IN_TX
 *                        I2C_BUSY_IN_RX
 *
 * @Note              -
 *                      1. This API initiates an I2C Master
 *                         transmission using interrupts.
 *
 *                      2. Unlike I2C_MasterSendData(), this function
 *                         does not wait for the transfer to complete.
 *
 *                      3. The function returns immediately after
 *                         setting up the transfer parameters and
 *                         enabling the required interrupts.
 *
 *                      4. Before starting a new transmission, the
 *                         driver checks whether the peripheral is
 *                         already busy with:
 *
 *                           - Transmission
 *                           - Reception
 *
 *                      5. If the peripheral is busy, no new transfer
 *                         is started and the current state is
 *                         returned to the caller.
 *
 *                      6. When the peripheral is available, the
 *                         following transfer information is stored
 *                         inside the handle:
 *
 *                           - Transmit buffer address
 *                           - Transfer length
 *                           - Slave address
 *                           - Repeated START option
 *                           - Current transfer state
 *
 *                      7. After the transfer context is stored,
 *                         a START condition is generated to begin
 *                         the I2C transaction.
 *
 *                      8. Actual address transmission and data
 *                         transfer are performed later by the
 *                         interrupt service routine.
 *
 *                      9. The following interrupt sources are
 *                         enabled:
 *
 *                           ITBUFEN
 *                               Buffer interrupts
 *                               (TXE, RXNE)
 *
 *                           ITEVTEN
 *                               Event interrupts
 *                               (SB, ADDR, BTF, STOPF ...)
 *
 *                           ITERREN
 *                               Error interrupts
 *                               (BERR, ARLO, AF, OVR ...)
 *
 *                     10. Once enabled, the interrupt handler
 *                         becomes responsible for progressing
 *                         the transfer state machine.
 *
 *                     11. The transmission sequence executed by
 *                         the ISR is typically:
 *
 *                              START
 *                                  |
 *                              Address + Write
 *                                  |
 *                              Data Bytes
 *                                  |
 *                          STOP / Repeated START
 *
 *                     12. Transfer completion is normally reported
 *                         through the application callback mechanism.
 *
 *                     13. Since this is a non-blocking API, the CPU
 *                         is free to execute other tasks while the
 *                         transfer is in progress.
 *
 *                     14. The application must not modify or free
 *                         the transmit buffer until the transfer
 *                         completion callback is received.
 *
 *                     15. This API assumes:
 *
 *                           - Peripheral clock is enabled
 *                           - I2C peripheral is initialized
 *                           - I2C peripheral is enabled
 *                           - NVIC configuration is complete
 *                           - I2C IRQ handler is implemented
 *
 */
uint8_t I2C_MasterSendDataIT(I2C_Handle_t *pI2CHandle,
							 uint8_t *pTxBuffer,
							 uint32_t Len,
							 uint8_t SlaveAddr,
							 uint8_t RepeatedStart)
{
	uint8_t busystate = pI2CHandle->TxRxState;

	if( (busystate != I2C_BUSY_IN_TX) && (busystate != I2C_BUSY_IN_RX))
	{
		pI2CHandle->pTxBuffer = pTxBuffer;
		pI2CHandle->TxLen = Len;
		pI2CHandle->TxRxState = I2C_BUSY_IN_TX;
		pI2CHandle->DevAddr = SlaveAddr;
		pI2CHandle->RepeatedStart = RepeatedStart;

		//Implement code to Generate START Condition
		I2C_GenerateStartCondition(pI2CHandle->pI2Cx);

		//Enable ITBUFEN Control Bit
		pI2CHandle->pI2Cx->CR2 |= (1U << I2C_CR2_ITBUFEN);

		//Enable ITEVTEN Control Bit
		pI2CHandle->pI2Cx->CR2 |= (1U << I2C_CR2_ITEVTEN);

		//Enable ITERREN Control Bit
		pI2CHandle->pI2Cx->CR2 |= (1U << I2C_CR2_ITERREN);

	}

	return busystate;
}



/*********************************************************************
 * @fn                - I2C_MasterReceiveData
 *
 * @brief             - Receive data as an I2C Master using the
 *                      blocking/polling method.
 *
 * @param[in]         - pI2CHandle : Pointer to I2C handle structure
 *                                   containing peripheral instance
 *                                   and configuration information.
 *
 * @param[in]         - pRxBuffer  : Pointer to receive buffer.
 *
 * @param[in]         - Len        : Number of bytes to receive.
 *
 * @param[in]         - SlaveAddr  : 7-bit slave address.
 *
 * @param[in]         - RepeatedStart :
 *                                   I2C_SR_DISABLE
 *                                   I2C_SR_ENABLE
 *
 * @return            - none
 *
 * @Note              -
 *                      1. This API performs I2C reception in
 *                         Master Receiver mode using polling.
 *
 *                      2. The function is blocking in nature.
 *                         Execution remains inside the function
 *                         until all requested bytes have been
 *                         received.
 *
 *                      3. Reception sequence:
 *
 *                              START
 *                                  |
 *                              Address + Read
 *                                  |
 *                             Receive Bytes
 *                                  |
 *                          STOP / Repeated START
 *
 *                      4. After generating the START condition,
 *                         software waits for the SB flag before
 *                         transmitting the slave address.
 *
 *                      5. The slave address is transmitted with
 *                         the R/W bit set to READ.
 *
 *                      6. After address acknowledgement, the ADDR
 *                         flag must be cleared according to the
 *                         required software sequence before data
 *                         reception can continue.
 *
 *                      7. Reception of a single byte requires a
 *                         special procedure defined by the STM32
 *                         reference manual.
 *
 *                         For Len == 1:
 *
 *                           - Disable ACK
 *                           - Clear ADDR
 *                           - Wait for RXNE
 *                           - Generate STOP
 *                             (if Repeated START disabled)
 *                           - Read received byte
 *
 *                      8. ACK must be disabled before clearing
 *                         ADDR in a single-byte reception so that
 *                         the peripheral generates a NACK after
 *                         receiving the byte.
 *
 *                      9. For multi-byte reception (Len > 1):
 *
 *                           - ACK remains enabled initially
 *                           - ADDR is cleared
 *                           - Bytes are received one by one
 *
 *                     10. When only two bytes remain:
 *
 *                           - ACK is disabled
 *                           - STOP is generated
 *                             (if Repeated START disabled)
 *
 *                         This allows the peripheral to generate
 *                         a NACK for the final byte and terminate
 *                         the transfer correctly.
 *
 *                     11. RXNE (Receive Buffer Not Empty) is used
 *                         to indicate that a new byte has arrived
 *                         and is available in the data register.
 *
 *                     12. Reading the DR register clears RXNE and
 *                         allows reception of the next byte.
 *
 *                     13. If RepeatedStart is disabled:
 *
 *                           - STOP condition is generated at the
 *                             end of reception.
 *
 *                     14. If RepeatedStart is enabled:
 *
 *                           - STOP condition is not generated.
 *                           - Bus ownership is retained.
 *                           - A subsequent START condition may be
 *                             generated by software.
 *
 *                     15. Repeated START is commonly used for:
 *
 *                           - Register read operations
 *                           - Combined write/read transactions
 *                           - Multi-part I2C transfers
 *
 *                     16. At the end of reception, ACK is restored
 *                         to the user-configured value stored in
 *                         the I2C configuration structure.
 *
 *                     17. Failure to restore ACK may cause future
 *                         receive operations to behave incorrectly.
 *
 *                     18. This API assumes:
 *
 *                           - Peripheral clock is enabled
 *                           - I2C peripheral is initialized
 *                           - I2C peripheral is enabled
 *                           - SDA/SCL pins are properly configured
 *                           - External pull-up resistors are present
 *
 */
void I2C_MasterReceiveData(I2C_Handle_t *pI2CHandle,
						   uint8_t *pRxBuffer,
						   uint32_t Len,
						   uint8_t SlaveAddr,
						   uint8_t RepeatedStart)
{
	//1. Generate the START Condition
	I2C_GenerateStartCondition(pI2CHandle->pI2Cx);

	/*2. Confirm that START Generation is completed by checking the SB flag in the SR1
	     Note: Until SB is cleared SCL will be stretched (pulled to LOW) */
	while(I2C_GetFlagStatus(pI2CHandle->pI2Cx,
							I2C_FLAG_SB) == FLAG_RESET);

	//3. Send the address of the slave with r/nw bit to READ
	I2C_ExecuteAddressPhase(pI2CHandle->pI2Cx,
							SlaveAddr,
							I2C_DIR_READ);

	//4. Confirm that address phase is completed by checking the ADDR flag in the SR1
	while(I2C_GetFlagStatus(pI2CHandle->pI2Cx,
							I2C_FLAG_ADDR) == FLAG_RESET);

	//5. Procedure to read only 1 byte from slave
	if(Len == 1U)
	{
		//a. Disabling ACKing
		I2C_AckControl(pI2CHandle->pI2Cx,
					   I2C_ACK_DISABLE);

		//b. Clear the ADDR flag
		I2C_ClearADDRFlag(pI2CHandle);

		//c. Wait until RxNE becomes 1
		while(I2C_GetFlagStatus(pI2CHandle->pI2Cx,
								I2C_FLAG_RXNE) == FLAG_RESET); //Check if read data register is full

		//d. Generate STOP condition
		if(RepeatedStart == I2C_SR_DISABLE)
		{
			I2C_GenerateStopCondition(pI2CHandle->pI2Cx);
		}
		//e. Read Data into buffer (read 1Byte)
		*pRxBuffer = pI2CHandle->pI2Cx->DR;

	}
	//6. Procedure to read data from slave when Len > 1
	if(Len > 1U)
	{
		//a. Clear the ADDR flag
		I2C_ClearADDRFlag(pI2CHandle);

		//b. Read the Data until Len becomes zero
		for(uint32_t i = Len; i > 0U; i--)
		{
			//c. Wait until RxNE becomes 1
			while(I2C_GetFlagStatus(pI2CHandle->pI2Cx,
									I2C_FLAG_RXNE) == FLAG_RESET);

			//d. if last 2 bytes are remaining
			if(i == 2U)
			{
				//e. Clear the ACK bit (Disabling ACKing)
				I2C_AckControl(pI2CHandle->pI2Cx,
							   I2C_ACK_DISABLE);

				//f. Generate STOP condition
				if(RepeatedStart == I2C_SR_DISABLE)
				{
					I2C_GenerateStopCondition(pI2CHandle->pI2Cx);
				}
			}

			//g. Read the data from data register into buffer
			*pRxBuffer = pI2CHandle->pI2Cx->DR;

			//h. Increment the buffer address
			pRxBuffer++;
		}
	}
	//i. Re-enable ACKing
	if(pI2CHandle->I2C_Config.I2C_ACKControl == I2C_ACK_ENABLE)
	{
		I2C_AckControl(pI2CHandle->pI2Cx,
					   I2C_ACK_ENABLE);
	}
}



/*********************************************************************
 * @fn                - I2C_MasterReceiveDataIT
 *
 * @brief             - Receive data as an I2C Master using the
 *                      interrupt-driven (non-blocking) method.
 *
 * @param[in]         - pI2CHandle : Pointer to I2C handle structure
 *                                   containing peripheral instance
 *                                   and runtime transfer state.
 *
 * @param[in]         - pRxBuffer  : Pointer to receive buffer.
 *
 * @param[in]         - Len        : Number of bytes to receive.
 *
 * @param[in]         - SlaveAddr  : 7-bit slave address.
 *
 * @param[in]         - RepeatedStart :
 *                                   I2C_SR_DISABLE
 *                                   I2C_SR_ENABLE
 *
 * @return            - Current I2C communication state:
 *
 *                        I2C_READY
 *                        I2C_BUSY_IN_TX
 *                        I2C_BUSY_IN_RX
 *
 * @Note              -
 *                      1. This API initiates an I2C Master receive
 *                         operation using interrupts.
 *
 *                      2. Unlike I2C_MasterReceiveData(), this
 *                         function does not wait for reception to
 *                         complete.
 *
 *                      3. The function returns immediately after
 *                         storing the reception parameters and
 *                         enabling the required interrupt sources.
 *
 *                      4. Before starting a new reception, the
 *                         driver verifies that the peripheral is
 *                         not currently busy with:
 *
 *                           - Transmission
 *                           - Reception
 *
 *                      5. If the peripheral is already busy, no new
 *                         reception is started and the current state
 *                         is returned to the caller.
 *
 *                      6. When the peripheral is available, the
 *                         following reception context is stored in
 *                         the handle structure:
 *
 *                           - Receive buffer address
 *                           - Requested receive length
 *                           - Original receive size
 *                           - Slave address
 *                           - Repeated START option
 *                           - Current transfer state
 *
 *                      7. RxLen is used to track the remaining
 *                         number of bytes during reception.
 *
 *                      8. RxSize stores the original reception
 *                         length and is preserved for special
 *                         receive sequences handled inside the ISR.
 *
 *                      9. RxSize is particularly useful when
 *                         implementing:
 *
 *                           - Single-byte reception
 *                           - Two-byte reception
 *                           - ACK/NACK management
 *                           - ADDR flag clearing sequence
 *
 *                     10. After the transfer context is stored,
 *                         a START condition is generated to begin
 *                         the I2C transaction.
 *
 *                     11. Actual address transmission and data
 *                         reception are performed later by the
 *                         interrupt service routine.
 *
 *                     12. The following interrupt sources are
 *                         enabled:
 *
 *                           ITBUFEN
 *                               Buffer interrupts
 *                               (TXE, RXNE)
 *
 *                           ITEVTEN
 *                               Event interrupts
 *                               (SB, ADDR, BTF, STOPF ...)
 *
 *                           ITERREN
 *                               Error interrupts
 *                               (BERR, ARLO, AF, OVR ...)
 *
 *                     13. Once enabled, the interrupt handler
 *                         becomes responsible for progressing
 *                         the receive state machine.
 *
 *                     14. The reception sequence executed by the
 *                         ISR is typically:
 *
 *                              START
 *                                  |
 *                              Address + Read
 *                                  |
 *                             Receive Bytes
 *                                  |
 *                          STOP / Repeated START
 *
 *                     15. ACK/NACK generation for the final bytes
 *                         is handled by the interrupt-driven receive
 *                         state machine according to the remaining
 *                         receive length.
 *
 *                     16. If RepeatedStart is enabled, the ISR will
 *                         complete reception without generating a
 *                         STOP condition, allowing a subsequent
 *                         START condition to be generated.
 *
 *                     17. Transfer completion is normally reported
 *                         through the application callback
 *                         mechanism.
 *
 *                     18. Since this is a non-blocking API, the CPU
 *                         is free to perform other tasks while the
 *                         reception is in progress.
 *
 *                     19. The application must not modify or free
 *                         the receive buffer until the reception
 *                         completion callback is received.
 *
 *                     20. This API assumes:
 *
 *                           - Peripheral clock is enabled
 *                           - I2C peripheral is initialized
 *                           - I2C peripheral is enabled
 *                           - NVIC configuration is complete
 *                           - I2C IRQ handler is implemented
 *
 */
uint8_t I2C_MasterReceiveDataIT(I2C_Handle_t *pI2CHandle,
								uint8_t *pRxBuffer,
								uint32_t Len,
								uint8_t SlaveAddr,
								uint8_t RepeatedStart)
{
	uint8_t busystate = pI2CHandle->TxRxState;

	if( (busystate != I2C_BUSY_IN_TX) && (busystate != I2C_BUSY_IN_RX))
	{
		pI2CHandle->pRxBuffer = pRxBuffer;
		pI2CHandle->RxLen = Len;
		pI2CHandle->TxRxState = I2C_BUSY_IN_RX;
		pI2CHandle->RxSize = Len; //Rxsize is used in the ISR code to manage the data reception
		pI2CHandle->DevAddr = SlaveAddr;
		pI2CHandle->RepeatedStart = RepeatedStart;

		//Implement code to Generate START Condition
		I2C_GenerateStartCondition(pI2CHandle->pI2Cx);

		//Enable ITBUFEN Control Bit
		pI2CHandle->pI2Cx->CR2 |= (1U << I2C_CR2_ITBUFEN);

		//Enable ITEVTEN Control Bit
		pI2CHandle->pI2Cx->CR2 |= (1U << I2C_CR2_ITEVTEN);

		//Enable ITERREN Control Bit
		pI2CHandle->pI2Cx->CR2 |= (1U << I2C_CR2_ITERREN);

	}

	return busystate;
}



/*********************************************************************
 * @fn                - I2C_SlaveSendData
 *
 * @brief             - Load a data byte into the I2C data register
 *                      for transmission in Slave Transmitter mode.
 *
 * @param[in]         - pI2Cx : Base address of the I2C peripheral.
 *
 * @param[in]         - data  : Data byte to be transmitted.
 *
 * @return            - none
 *
 * @Note              -
 *                      1. This function writes a single byte into
 *                         the I2C data register (DR).
 *
 *                      2. The loaded byte is transmitted when the
 *                         master performs a read operation from the
 *                         slave device.
 *
 *                      3. This API is typically called from the
 *                         application layer when
 *  					   I2C_EV_DATA_REQ callback is received:
 *
 *                           - TXE event
 *                           - Slave transmit request callback
 *
 *                      4. The application is responsible for
 *                         providing the next byte whenever the
 *                         transmit data register becomes empty.
 *
 *                      5. Writing to DR clears the TXE condition
 *                         and prepares the next byte for transfer.
 *
 *                      6. This API transmits only one byte.
 *                         Multi-byte slave transmission must be
 *                         managed by the application or ISR logic.
 *
 *                      7. The peripheral must already be configured
 *                         and operating in slave mode before this
 *                         function is called.
 *
 */
void I2C_SlaveSendData(I2C_RegDef_t *pI2Cx,
					   uint8_t data)
{
	//load data to DR
	pI2Cx->DR = data;
}



/*********************************************************************
 * @fn                - I2C_SlaveReceiveData
 *
 * @brief             - Read a received data byte from the I2C data
 *                      register in Slave Receiver mode.
 *
 * @param[in]         - pI2Cx : Base address of the I2C peripheral.
 *
 * @return            - Received data byte.
 *
 * @Note              -
 *                      1. This function reads a single byte from
 *                         the I2C data register (DR).
 *
 *                      2. The received byte must be read after the
 *                         RXNE flag becomes set.
 *
 *                      3. This API is typically called from the
 *                         application layer when
 *  					   I2C_EV_DATA_RCV callback is received:
 *
 *                           - RXNE event
 *                           - Slave receive callback
 *
 *                      4. Reading DR clears the RXNE condition and
 *                         allows the peripheral to receive the next
 *                         byte.
 *
 *                      5. This API receives only one byte.
 *                         Multi-byte slave reception must be managed
 *                         by the application or ISR logic.
 *
 *                      6. The peripheral must already be configured
 *                         and operating in slave mode before this
 *                         function is called.
 *
 *                      7. Failure to read DR after RXNE becomes set
 *                         may prevent subsequent data reception and
 *                         can eventually lead to overrun conditions.
 *
 */
uint8_t I2C_SlaveReceiveData(I2C_RegDef_t *pI2Cx)
{
	//return data from the DR
	return (uint8_t)pI2Cx->DR;
}



/*********************************************************************
 * @fn                - I2C_IRQInterruptConfig
 *
 * @brief             - Configure NVIC interrupt enable or disable
 *                      for the given IRQ number.
 *
 * @param[in]         - IRQNumber : IRQ number of the peripheral
 * @param[in]         - EnorDi    : ENABLE or DISABLE macros
 *
 * @return            - none
 *
 * @Note              -
 *
 *                      1. This function is:
 *
 *                              Processor specific
 *
 *                         because NVIC belongs to ARM Cortex-M
 *                         processor, NOT STM32 peripheral itself.
 *
 *                      2. NVIC stands for:
 *
 *                              Nested Vector Interrupt Controller
 *
 *                      3. NVIC is responsible for:
 *
 *                           - Interrupt enabling
 *                           - Interrupt disabling
 *                           - Interrupt prioritization
 *                           - Interrupt nesting
 *                           - Interrupt preemption
 *
 *                      4. Every interrupt source inside the MCU has
 *                         a unique IRQ number.
 *
 *                         Example:
 *
 *                              I2C1_IRQn
 *                              SPI2_IRQn
 *                              EXTI15_10_IRQn
 *
 *                      5. Cortex-M processor contains:
 *
 *                              ISER registers
 *
 *                         ISER means:
 *
 *                              Interrupt Set Enable Register
 *
 *                      6. ISER registers are used to:
 *
 *                              Enable interrupts
 *
 *                      7. Cortex-M processor also contains:
 *
 *                              ICER registers
 *
 *                         ICER means:
 *
 *                              Interrupt Clear Enable Register
 *
 *                      8. ICER registers are used to:
 *
 *                              Disable interrupts
 *
 *                      9. Each ISER/ICER register controls:
 *
 *                              32 interrupt lines
 *
 *                         Therefore:
 *
 *                              ISER0 → IRQ0  to IRQ31
 *                              ISER1 → IRQ32 to IRQ63
 *                              ISER2 → IRQ64 to IRQ95
 *
 *                         Same concept for:
 *
 *                              ICER0
 *                              ICER1
 *                              ICER2
 *
 *                     10. Example:
 *
 *                              IRQNumber = 35
 *
 *                         belongs to:
 *
 *                              ISER1 bit position 3
 *
 *                         because:
 *
 *                              35 % 32 = 3
 *
 *                     11. Enabling interrupt flow:
 *
 *                              Peripheral Event
 *                                       ↓
 *                              Peripheral sets interrupt flag
 *                                       ↓
 *                              NVIC accepts IRQ
 *                                       ↓
 *                              CPU jumps to ISR
 *
 *
 *                     12. Important difference:
 *
 *                              Peripheral interrupt enable
 *                              decides WHETHER interrupt can occur.
 *
 *                              NVIC interrupt enable
 *                              decides WHETHER CPU can receive it.
 *
 *                     13. Bitwise operation:
 *
 *                              (1U << IRQNumber)
 *
 *                         generates bit mask for required IRQ.
 *
 *                     14. Why '1U' used:
 *
 *                         Unsigned constant avoids signed overflow
 *                         problems during left shift.
 *
 *                     15. ISER registers generally use:
 *
 *                              |= operator
 *
 *                         because:
 *
 *                         We want to preserve previously enabled
 *                         interrupts while enabling new interrupt.
 *
 *                     16. ICER registers are special:
 *
 *                         They are:
 *
 *                              Write-1-to-clear registers
 *
 *                         Meaning:
 *
 *                              Write 1 → disable interrupt
 *                              Write 0 → no effect
 *
 *                     17. Therefore using:
 *
 *                              =
 *
 *                         is perfectly valid for ICER registers.
 *
 *                         No need for:
 *
 *                              |=
 *
 *                         because hardware does NOT require
 *                         read-modify-write operation.
 *
 *                     18. Example:
 *
 *                              *NVIC_ICER0 = (1 << 5);
 *
 *                         disables ONLY IRQ5.
 *
 *                         Other bits written as 0 have:
 *
 *                              no effect
 *
 *                     19. Read behavior of ICER:
 *
 *                              0 → interrupt disabled
 *                              1 → interrupt enabled
 *
 *                         But write behavior is:
 *
 *                              0 → no effect
 *                              1 → disable interrupt
 *
 *                     20. This distinction is VERY important in ARM
 *                         peripheral register programming.
 *
 *
 *                     21. This API supports IRQ0 to IRQ95.
 *
 */
void I2C_IRQInterruptConfig(uint8_t IRQNumber,
							uint8_t EnorDi)
 {
  if(EnorDi == ENABLE)
  {
 	 if(IRQNumber <= 31U)
 	 {
 		 // IRQ0 to IRQ31 → ISER0
 		 *NVIC_ISER0 |= (1U << IRQNumber);
 	 }
 	 else if(IRQNumber > 31U && IRQNumber < 64U)
 	 {
 		 // IRQ32 to IRQ63 → ISER1
 		 *NVIC_ISER1 |= (1U << (IRQNumber % 32U));
 	 }
 	 else if(IRQNumber > 63U && IRQNumber < 96U)
 	 {
 		 // IRQ64 to IRQ95 → ISER2
 		 *NVIC_ISER2 |= (1U << (IRQNumber % 32U * 2U));
 	 }
  }
  else
  {
 	 if(IRQNumber <= 31U)
 	 {
 		 // IRQ0 to IRQ31 → ICER0
 		 *NVIC_ICER0 = (1U << IRQNumber);

 		 /*
 		  * ICER is Write-1-to-Clear register:
 		  *
 		  * Write:
 		  *      0 → No effect
 		  *      1 → Disable interrupt
 		  *
 		  * Therefore '=' is enough.
 		  *
 		  * No need for read-modify-write using '|='.
 		  */
 	 }
 	 else if(IRQNumber > 31U && IRQNumber < 64U)
 	 {
 		 // IRQ32 to IRQ63 → ICER1
 		 *NVIC_ICER1 = (1U << (IRQNumber % 32U));
 	 }
 	 else if(IRQNumber > 63U && IRQNumber < 96U)
 	 {
 		 // IRQ64 to IRQ95 → ICER2
 		 *NVIC_ICER2 = (1U << (IRQNumber % 32U * 2U));
 	 }
  }
 }



/*********************************************************************
 * @fn                - I2C_IRQPriorityConfig
 *
 * @brief             - Configure priority level for a given IRQ
 *                      interrupt in NVIC.
 *
 * @param[in]         - IRQNumber   : IRQ number of the peripheral
 * @param[in]         - IRQPriority : Priority value for the IRQ
 *
 * @return            - none
 *
 * @Note              -
 *
 *                      1. This function is:
 *
 *                              Processor specific
 *
 *                         because interrupt priority management is
 *                         handled by ARM Cortex-M NVIC, not STM32
 *                         peripheral itself.
 *
 *                      2. NVIC stands for:
 *
 *                              Nested Vector Interrupt Controller
 *
 *                      3. NVIC not only enables/disables interrupts,
 *                         it also manages:
 *
 *                           - Interrupt priorities
 *                           - Interrupt nesting
 *                           - Interrupt preemption
 *
 *                      4. Interrupt priority decides:
 *
 *                              Which interrupt should execute first
 *
 *                         when multiple interrupts occur together.
 *
 *                      5. Lower numerical priority value means:
 *
 *                              Higher actual priority
 *
 *                         Example:
 *
 *                              Priority 0 → Highest priority
 *                              Priority 15 → Lowest priority
 *
 *                      6. Cortex-M stores priorities inside:
 *
 *                              IPR registers
 *
 *                         IPR means:
 *
 *                              Interrupt Priority Register
 *
 *                      7. Each IPR register contains:
 *
 *                              4 IRQ priority fields
 *
 *                         because:
 *
 *                              32-bit register
 *                              8 bits per IRQ
 *
 *                              4 × 8 = 32 bits
 *
 *                      8. Example:
 *
 *                              IPR Register Layout
 *
 *                              ---------------------------------
 *                              | IRQ3 | IRQ2 | IRQ1 | IRQ0 |
 *                              ---------------------------------
 *                                 8b     8b     8b     8b
 *
 *                      9. Therefore:
 *
 *                              IRQNumber / 4
 *
 *                         gives:
 *
 *                              Which IPR register to use
 *
 *                     10. Example:
 *
 *                              IRQNumber = 6
 *
 *                              6 / 4 = 1
 *
 *                         therefore:
 *
 *                              IPR1 register used
 *
 *                     11. Each IPR register contains 4 sections.
 *
 *                         Therefore:
 *
 *                              IRQNumber % 4
 *
 *                         gives:
 *
 *                              Which section inside IPR register
 *
 *                     12. Example:
 *
 *                              IRQNumber = 6
 *
 *                              6 % 4 = 2
 *
 *                         therefore:
 *
 *                              section 2 inside IPR1
 *
 *                     13. ARM Cortex-M usually does NOT implement
 *                         all 8 priority bits.
 *
 *                         Example:
 *
 *                              STM32F4 implements only upper 4 bits
 *
 *                     14. Therefore:
 *
 *                              NO_PR_BITS_IMPLEMENTED
 *
 *                         defines actual implemented priority bits.
 *
 *                     15. Example:
 *
 *                              NO_PR_BITS_IMPLEMENTED = 4
 *
 *                         means:
 *
 *                              Only bits [7:4] valid
 *                              Bits [3:0] ignored
 *
 *                     16. Hence priority value must be shifted into
 *                         upper implemented bits.
 *
 *                     17. Shift calculation:
 *
 *                              (8 * iprx_section)
 *
 *                         selects correct IRQ field position.
 *
 *                     18. Additional shift:
 *
 *                              (8 - NO_PR_BITS_IMPLEMENTED)
 *
 *                         aligns priority into upper valid bits.
 *
 *                     19. Example:
 *
 *                              IRQ6
 *                              section = 2
 *
 *                              shift =
 *                              (8 * 2) + (8 - 4)
 *                              = 16 + 4
 *                              = 20
 *
 *                         therefore:
 *
 *                              priority stored at bits [23:20]
 *
 *                     20. Priority register write operation:
 *
 *                              IRQPriority << shift_amount
 *
 *                         moves priority value into correct bit field.
 *
 *                     21. Interrupt priority important for:
 *
 *                           - RTOS scheduling
 *                           - Real-time systems
 *                           - Communication protocols
 *                           - Time-critical events
 *
 *                     22. Example:
 *
 *                              I2C interrupt priority = 2
 *                              UART interrupt priority = 5
 *
 *                         If both interrupts occur together:
 *
 *                              I2C ISR executes first
 *
 *                     23. Higher-priority interrupt can preempt
 *                         lower-priority interrupt.
 *
 *                     24. Example:
 *
 *                              UART ISR running
 *                                   ↓
 *                              I2C interrupt occurs
 *                                   ↓
 *                              I2C has higher priority
 *                                   ↓
 *                              CPU pauses UART ISR
 *                                   ↓
 *                              CPU executes I2C ISR
 *                                   ↓
 *                              CPU resumes UART ISR
 *
 *                     25. This mechanism called:
 *
 *                              Interrupt Preemption
 *
 *                     26. Important:
 *
 *                         This API only configures priority.
 *
 *                         It does NOT:
 *
 *                           - Enable interrupt
 *                           - Disable interrupt
 *                           - Clear interrupt flags
 *
 *
 *                     27. Why '|=' used:
 *
 *                         To preserve other IRQ priority fields
 *                         inside same IPR register.
 *
 *                     28. Better practice:
 *
 *                         Clear target field before writing new
 *                         priority value.
 *
 *                         Otherwise old bits may remain.
 *
 *                     29. Example improvement:
 *
 *                              Clear target field first
 *                              then write new priority
 *
 */
void I2C_IRQPriorityConfig(uint8_t IRQNumber,
 						   uint32_t IRQPriority)
{
  // 1. Find the IPR register index
  uint8_t iprx = IRQNumber / 4U;

  // 2. Find section inside IPR register
  uint8_t iprx_section = IRQNumber % 4U;

  /*
   * 3. Calculate shift amount
   *
   * STM32 implements only upper 4 bits
   * of each 8-bit priority field.
   */
  uint8_t shift_amount =
		  ((8U * iprx_section) +
		  (8U - NO_PR_BITS_IMPLEMENTED));

  // 4. Configure IRQ priority
  *(NVIC_IPR_BASE_ADDR + iprx) |=
		  (IRQPriority << shift_amount);
}



/*********************************************************************
 * @fn                - I2C_SlaveEnableDisableCallbackEvents
 *
 * @brief             - Enable or disable I2C event, buffer and
 *                      error interrupts.
 *
 * @param[in]         - pI2Cx   : Base address of the I2C peripheral.
 *
 * @param[in]         - EnOrDi  : ENABLE
 *                                DISABLE
 *
 * @return            - none
 *
 * @Note              -
 *                      1. This function enables or disables the
 *                         interrupt sources required for callback-
 *                         based I2C slave communication.
 *
 *                      2. When enabled, the I2C peripheral can
 *                         generate interrupt requests for:
 *
 *                           - Event conditions
 *                           - Buffer conditions
 *                           - Error conditions
 *
 *                      3. The following interrupt control bits in
 *                         CR2 are configured:
 *
 *                           ITBUFEN
 *                               Buffer Interrupt Enable
 *
 *                           ITEVTEN
 *                               Event Interrupt Enable
 *
 *                           ITERREN
 *                               Error Interrupt Enable
 *
 *                      4. ITBUFEN enables buffer-related events:
 *
 *                           - TXE (Transmit Buffer Empty)
 *                           - RXNE (Receive Buffer Not Empty)
 *
 *                      5. ITEVTEN enables event-related interrupts:
 *
 *                           - SB (Start Bit)
 *                           - ADDR (Address Matched)
 *                           - BTF (Byte Transfer Finished)
 *                           - STOPF (Stop Detection)
 *                           - Other communication events
 *
 *                      6. ITERREN enables error-related interrupts:
 *
 *                           - BERR (Bus Error)
 *                           - ARLO (Arbitration Lost)
 *                           - AF (Acknowledge Failure)
 *                           - OVR (Overrun/Underrun)
 *                           - TIMEOUT (if supported)
 *
 *                      7. Disabling these interrupt sources does not
 *                         disable the I2C peripheral itself.
 *
 *                      8. When disabled, interrupt requests are no
 *                         longer generated, but software may still
 *                         access the peripheral using polling-based
 *                         APIs.
 *
 *                      9. This function is typically used when the
 *                         peripheral operates in slave mode and the
 *                         application relies on callback functions
 *                         for data transmission and reception.
 *
 *                     10. NVIC interrupt configuration must also be
 *                         completed separately for interrupt-driven
 *                         operation.
 *
 */
void I2C_SlaveEnableDisableCallbackEvents(I2C_RegDef_t *pI2Cx,
 		                                  uint8_t EnorDi)
{
	if(EnorDi == ENABLE)
	{
		//Enable ITBUFEN Control Bit
		pI2Cx->CR2 |= (1U << I2C_CR2_ITBUFEN);

		//Enable ITEVTEN Control Bit
		pI2Cx->CR2 |= (1U << I2C_CR2_ITEVTEN);

		//Enable ITERREN Control Bit
		pI2Cx->CR2 |= (1U << I2C_CR2_ITERREN);
	}
	else
	{
		//Disable ITBUFEN Control Bit
		pI2Cx->CR2 &= ~(1U << I2C_CR2_ITBUFEN);

		//Disable ITEVTEN Control Bit
		pI2Cx->CR2 &= ~(1U << I2C_CR2_ITEVTEN);

		//Disable ITERREN Control Bit
		pI2Cx->CR2 &= ~(1U << I2C_CR2_ITERREN);
	}
}



/*********************************************************************
 * @fn                - I2C_EV_IRQHandling
 *
 * @brief             - Handles I2C event interrupts generated by
 *                      the peripheral.
 *
 * @param[in]         - pI2CHandle : Pointer to I2C handle structure.
 *
 * @return            - none
 *
 * @Note              -
 *                      1. This function must be called from the
 *                         corresponding I2C Event ISR.
 *
 *                      2. Event interrupts are processed only when
 *                         ITEVTEN is enabled.
 *
 *                      3. Buffer related events additionally require
 *                         ITBUFEN to be enabled.
 *
 *                      4. Supported event flags:
 *
 *                           - SB
 *                           - ADDR
 *                           - BTF
 *                           - STOPF
 *                           - TXE
 *                           - RXNE
 *
 *                      5. In Master mode:
 *
 *                           - Executes address phase after SB.
 *                           - Clears ADDR.
 *                           - Handles transmit operations.
 *                           - Handles receive operations.
 *                           - Completes transfers when BTF occurs.
 *
 *                      6. In Slave mode:
 *
 *                           - Detects address match.
 *                           - Detects STOP condition.
 *                           - Generates application callbacks for:
 *
 *                                I2C_EV_DATA_REQ
 *                                I2C_EV_DATA_RCV
 *                                I2C_EV_STOP
 *
 *                      7. Transfer completion events generate:
 *
 *                                I2C_EV_TX_CMPLT
 *                                I2C_EV_RX_CMPLT
 *
 *                         through the application callback.
 *
 *                      8. This function services both Master and
 *                         Slave interrupt-driven communication.
 *
 */
void I2C_EV_IRQHandling(I2C_Handle_t *pI2CHandle)
{

	//Interrupt handling for both master and slave mode of a device

	uint32_t temp1 = 0U, temp2 = 0U, temp3 = 0U;

	temp1 = pI2CHandle->pI2Cx->CR2 & (1U << I2C_CR2_ITEVTEN) ;
	temp2 = pI2CHandle->pI2Cx->CR2 & (1U << I2C_CR2_ITBUFEN) ;

	/* 1. Handle for interrupt generated by SB event
	 *  Note: SB flag is only applicable in Master mode */
	temp3 = pI2CHandle->pI2Cx->SR1 & (1U << I2C_SR1_SB);

	if(temp1 && temp3)
	{
		/* The interrupt is generated because of SB event.
		 * This block will not be executed in slave mode because
		 * for slave SB is always zero. and in this block lets
		 * executed the ADDRESS PHASE as the SB generation has
		 * successfully completed. */
		if(pI2CHandle->TxRxState == I2C_BUSY_IN_TX)
		{
			//Send the address of the slave with r/nw bit to WRITE
			I2C_ExecuteAddressPhase(pI2CHandle->pI2Cx,
									pI2CHandle->DevAddr,
									I2C_DIR_WRITE);
		}
		else if(pI2CHandle->TxRxState == I2C_BUSY_IN_RX)
		{
			//Send the address of the slave with r/nw bit to READ
			I2C_ExecuteAddressPhase(pI2CHandle->pI2Cx,
									pI2CHandle->DevAddr,
									I2C_DIR_READ);
		}

	}


	/* 2. Handle For interrupt generated by ADDR event
	 * Note : When master mode : Address is sent
	 *		  When Slave mode   : Address matched with own address */
	temp3 = pI2CHandle->pI2Cx->SR1 & (1U << I2C_SR1_ADDR);
	if(temp1 && temp3)
	{
		// interrupt is generated because of ADDR event
		I2C_ClearADDRFlag(pI2CHandle);
	}


	//3. Handle For interrupt generated by BTF(Byte Transfer Finished) event
	temp3 = pI2CHandle->pI2Cx->SR1 & (1U << I2C_SR1_BTF);
	if(temp1 && temp3)
	{
		// interrupt is generated because of BTF event
		if(pI2CHandle->TxRxState == I2C_BUSY_IN_TX)
		{
			if(pI2CHandle->pI2Cx->SR1 & (1U << I2C_SR1_TXE))
			{
				//BTF,TXE = 1(set), indication to close the transmission
				if(pI2CHandle->TxLen == 0)
				{
					//a. generate the STOP condition
					if(pI2CHandle->RepeatedStart == I2C_SR_DISABLE)
					{
						I2C_GenerateStopCondition(pI2CHandle->pI2Cx);
					}
					//b. reset all the member elements of the handle structure
					I2C_CloseTransmission(pI2CHandle);

					//c. notify the application about transmission complete
					I2C_ApplicationEventCallback(pI2CHandle,
												 I2C_EV_TX_CMPLT);
				}
			}
		}
		else if(pI2CHandle->TxRxState == I2C_BUSY_IN_RX)
		{
			;//nothing to do RX in BTF, because it is Byte Transfer Finished
		}

	}


	/* 4. Handle For interrupt generated by STOPF event
	 * Note : Stop detection flag is applicable only slave mode .
	 * For master this flag will never be set
	 * The below code block will not be executed by the master
	 * since STOPF will not set in master mode */
	//STOPF will be only set by slave
	temp3 = pI2CHandle->pI2Cx->SR1 & (1U << I2C_SR1_STOPF);
	if(temp1 && temp3)
	{
		/* interrupt is generated because of STOPF event
		 * clear the STOPF,
		 * a. read SR1(already read above pI2Cx->SR1)
		 * b. write to CR1 */
		pI2CHandle->pI2Cx->CR1 |= 0x0000U;/* shouldn't write
		 	 	 	 	 	 	 	 	 	 invalid data*/

		//notify the application that STOP is detected
		I2C_ApplicationEventCallback(pI2CHandle,
									 I2C_EV_STOP);

	}


	//5. Handle For interrupt generated by TXE event
	temp3 = pI2CHandle->pI2Cx->SR1 & (1U << I2C_SR1_TXE);
	if(temp1 && temp2 && temp3)
	{
		/* interrupt is generated because of TXE event,
		 * indication that data register is empty so
		 * software has to put a byte into the DR.
		 */
		if(pI2CHandle->pI2Cx->SR2 & (1U << I2C_SR2_MSL))
		{
			//device is in master mode [MSL = 1]
			/* Confirm that only if the application state
			 * is busy in TX  */
			if(pI2CHandle->TxRxState == I2C_BUSY_IN_TX)
			{
				I2C_MasterHandleTXEInterrupt(pI2CHandle);
			}
		}
		else
		{
			//device is in slave mode [MSL = 0]
			if(pI2CHandle->pI2Cx->SR2 & (1U << I2C_SR2_TRA))
			{
				/* slave is in transmitter,
				 * TRA bit actually influenced by the
				 * address phase we do there the LSB
				 * bit as r(1) or w(0).
				 */
				//notify the application about the TXE
				I2C_ApplicationEventCallback(pI2CHandle,
				   	   	   	   	   	    	 I2C_EV_DATA_REQ);
			}
		}
	}


	//6. Handle For interrupt generated by RXNE event
	temp3 = pI2CHandle->pI2Cx->SR1 & (1U << I2C_SR1_RXNE);
	if(temp1 && temp2 && temp3)
	{
		/* interrupt is generated because of RXNE event,
		 * indication that data register is full so
		 * software has to read a byte from the DR.
		 */

		//device should be in master mode [MSL = 1]
		if(pI2CHandle->pI2Cx->SR2 & (1U << I2C_SR2_MSL))
		{
			/* Confirm that only if the application state
			 * is busy in RX  */
			if(pI2CHandle->TxRxState == I2C_BUSY_IN_RX)
			{
				I2C_MasterHandleRXNEInterrupt(pI2CHandle);
			}
		}
		else
		{
			//device is in slave mode [MSL = 0]
			if(!(pI2CHandle->pI2Cx->SR2 & (1U << I2C_SR2_TRA)))
			{
				/* slave is in slave,
				 * TRA bit actually influenced by the
				 * address phase we do there the LSB
				 * bit as r(1) or w(0).
				 */
				//notify the application about the RXNE
				I2C_ApplicationEventCallback(pI2CHandle,
				   	   	   	   	   	    	 I2C_EV_DATA_RCV);
			}
		}
	}
}



/*********************************************************************
 * @fn                - I2C_ER_IRQHandling
 *
 * @brief             - Handles I2C error interrupts generated by
 *                      the peripheral.
 *
 * @param[in]         - pI2CHandle : Pointer to I2C handle structure.
 *
 * @return            - none
 *
 * @Note              -
 *                      1. This function must be called from the
 *                         corresponding I2C Error ISR.
 *
 *                      2. Error interrupts are processed only when
 *                         ITERREN is enabled.
 *
 *                      3. Supported error conditions:
 *
 *                           - Bus Error (BERR)
 *                           - Arbitration Lost (ARLO)
 *                           - Acknowledge Failure (AF)
 *                           - Overrun/Underrun (OVR)
 *                           - Timeout (TIMEOUT)
 *
 *                      4. When an error occurs:
 *
 *                           a. Error flag is detected.
 *                           b. Error flag is cleared.
 *                           c. Application callback is notified.
 *
 *                      5. Corresponding application events:
 *
 *                           - I2C_ER_BERR
 *                           - I2C_ER_ARLO
 *                           - I2C_ER_AF
 *                           - I2C_ER_OVR
 *                           - I2C_ER_TIMEOUT
 *
 *                      6. This function does not automatically
 *                         restart or recover the transaction.
 *                         Recovery is application dependent.
 *
 */
void I2C_ER_IRQHandling(I2C_Handle_t *pI2CHandle)
{
	uint32_t temp1 = 0U, temp2 = 0U;

    //Know the status of  ITERREN control bit in the CR2
	temp2 = (pI2CHandle->pI2Cx->CR2) & (1U << I2C_CR2_ITERREN);


	//Check for Bus error
	temp1 = (pI2CHandle->pI2Cx->SR1) & (1U << I2C_SR1_BERR);
	if(temp1  && temp2 )
	{
		//This is Bus error

		//Implement the code to clear the buss error flag
		pI2CHandle->pI2Cx->SR1 &= ~(1U << I2C_SR1_BERR);

		//Implement the code to notify the application about the error
	   I2C_ApplicationEventCallback(pI2CHandle,
			   	   	   	   	   	    I2C_ER_BERR);
	}

	//Check for arbitration lost error
	temp1 = (pI2CHandle->pI2Cx->SR1) & (1U << I2C_SR1_ARLO );
	if(temp1  && temp2)
	{
		//This is arbitration lost error

		//Implement the code to clear the arbitration lost error flag
		pI2CHandle->pI2Cx->SR1 &= ~(1U << I2C_SR1_ARLO);

		//Implement the code to notify the application about the error
		I2C_ApplicationEventCallback(pI2CHandle,
									 I2C_ER_ARLO);

	}

	//Check for ACK failure  error
	temp1 = (pI2CHandle->pI2Cx->SR1) & (1U << I2C_SR1_AF);
	if(temp1  && temp2)
	{
		//This is ACK failure error

	    //Implement the code to clear the ACK failure error flag
		pI2CHandle->pI2Cx->SR1 &= ~(1U << I2C_SR1_AF);

		//Implement the code to notify the application about the error
		I2C_ApplicationEventCallback(pI2CHandle,
									 I2C_ER_AF);
	}

	//Check for Overrun/underrun error
	temp1 = (pI2CHandle->pI2Cx->SR1) & (1U << I2C_SR1_OVR);
	if(temp1  && temp2)
	{
		//This is Overrun/underrun

	    //Implement the code to clear the Overrun/underrun error flag
		pI2CHandle->pI2Cx->SR1 &= ~(1U << I2C_SR1_OVR);

		//Implement the code to notify the application about the error
		I2C_ApplicationEventCallback(pI2CHandle,
									 I2C_ER_OVR);
	}

	//Check for Time out error
	temp1 = (pI2CHandle->pI2Cx->SR1) & (1U << I2C_SR1_TIMEOUT);
	if(temp1  && temp2)
	{
		//This is Time out error

	    //Implement the code to clear the Time out error flag
		pI2CHandle->pI2Cx->SR1 &= ~(1U << I2C_SR1_TIMEOUT);

		//Implement the code to notify the application about the error
		I2C_ApplicationEventCallback(pI2CHandle,
									 I2C_ER_TIMEOUT);
	}

}



/*********************************************************************
 * @fn                - I2C_MasterHandleTXEInterrupt
 *
 * @brief             - Internal helper used to service the TXE
 *                      interrupt during interrupt-driven master
 *                      transmission.
 *
 * @param[in]         - pI2CHandle : Pointer to I2C handle structure.
 *
 * @return            - none
 *
 * @Note              -
 *                      1. Called by I2C_EV_IRQHandling().
 *
 *                      2. Executed when:
 *
 *                           - TXE = 1
 *                           - Master is transmitting
 *
 *                      3. Loads the next data byte into DR.
 *
 *                      4. Updates:
 *
 *                           - pTxBuffer
 *                           - TxLen
 *
 *                      5. Actual transfer completion is handled
 *                         later through the BTF event.
 *
 *                      6. Intended for internal driver use only.
 *
 */
static void I2C_MasterHandleTXEInterrupt(I2C_Handle_t *pI2CHandle)
{
	if(pI2CHandle->TxLen > 0U)
	{
		//a. load the data into DR
		pI2CHandle->pI2Cx->DR = *(pI2CHandle->pTxBuffer);

		//b. decrement the TxLen
		pI2CHandle->TxLen--;

		//c. Increment the address buffer
		pI2CHandle->pTxBuffer++;

	}
}



/*********************************************************************
 * @fn                - I2C_MasterHandleRXNEInterrupt
 *
 * @brief             - Internal helper used to service the RXNE
 *                      interrupt during interrupt-driven master
 *                      reception.
 *
 * @param[in]         - pI2CHandle : Pointer to I2C handle structure.
 *
 * @return            - none
 *
 * @Note              -
 *                      1. Called by I2C_EV_IRQHandling().
 *
 *                      2. Executed when:
 *
 *                           - RXNE = 1
 *                           - Master is receiving
 *
 *                      3. Reads received data from DR and stores it
 *                         into the receive buffer.
 *
 *                      4. Handles special STM32 reception sequences
 *                         for:
 *
 *                           - Single-byte reception
 *                           - Multi-byte reception
 *                           - Final byte handling
 *
 *                      5. Generates STOP condition when required
 *                         and repeated start is disabled.
 *
 *                      6. Updates:
 *
 *                           - pRxBuffer
 *                           - RxLen
 *
 *                      7. When reception completes:
 *
 *                           - Closes the receive operation.
 *                           - Restores ACK configuration.
 *                           - Generates I2C_EV_RX_CMPLT callback.
 *
 *                      8. Intended for internal driver use only.
 *
 */
static void I2C_MasterHandleRXNEInterrupt(I2C_Handle_t *pI2CHandle)
{
	if(pI2CHandle->RxSize == 1U)
	{
		//a. read the data from DR
		*pI2CHandle->pRxBuffer = pI2CHandle->pI2Cx->DR;

		//b. decrement the RxLen
		pI2CHandle->RxLen--;

		//c. generate the stop condition
		if(pI2CHandle->RepeatedStart == I2C_SR_DISABLE)
		{
			I2C_GenerateStopCondition(pI2CHandle->pI2Cx);
		}
	}

	if(pI2CHandle->RxSize > 1U)
	{
		if(pI2CHandle->RxLen == 2U)
		{
			//clear the ack bit
			I2C_AckControl(pI2CHandle->pI2Cx,
						   I2C_ACK_DISABLE);
		}

		/* Important:
		 * STOP must be generated when RxLen == 1 in the interrupt
		 * receive path. Generating STOP at RxLen == 2 causes the
		 * next transaction to fail.
		 */
        if (pI2CHandle->RxLen == 1U)
        {
            if (pI2CHandle->RepeatedStart == I2C_SR_DISABLE)
            {
                I2C_GenerateStopCondition(pI2CHandle->pI2Cx);
            }
        }

			//a. read the data from DR
			*pI2CHandle->pRxBuffer = pI2CHandle->pI2Cx->DR;

			//b. decrement the RxLen
			pI2CHandle->RxLen--;

			//c. Increment the address buffer
			pI2CHandle->pRxBuffer++;
	}

	if(pI2CHandle->RxLen == 0U)
	{
		//close the I2C data reception and notify the application
		//a. Close the I2C RX
		I2C_CloseReception(pI2CHandle);

		//b. Notify the application
		I2C_ApplicationEventCallback(pI2CHandle,
									 I2C_EV_RX_CMPLT);
	}
}



/*********************************************************************
 * @fn                - I2C_CloseTransmission
 *
 * @brief             - Terminate an interrupt-driven master transmit
 *                      operation and restore driver state.
 *
 * @param[in]         - pI2CHandle : Pointer to I2C handle structure.
 *
 * @return            - none
 *
 * @Note              -
 *                      1. Disables TX/RX buffer interrupts
 *                         (ITBUFEN).
 *
 *                      2. Disables event interrupts
 *                         (ITEVTEN).
 *
 *                      3. Resets transmission related state
 *                         variables.
 *
 *                      4. Updates driver state to:
 *
 *                              I2C_READY
 *
 *                      5. Does not generate a STOP condition.
 *                         STOP generation must be handled before
 *                         calling this function.
 *
 *                      6. Intended for internal driver use only.
 *
 */
void I2C_CloseTransmission(I2C_Handle_t *pI2CHandle)
{
	//Implement the code to disable ITBUFEN Control Bit
	pI2CHandle->pI2Cx->CR2 &= ~(1U << I2C_CR2_ITBUFEN);

	//Implement the code to disable ITEVTEN Control Bit
	pI2CHandle->pI2Cx->CR2 &= ~(1U << I2C_CR2_ITEVTEN);


	pI2CHandle->TxRxState = I2C_READY;
	pI2CHandle->pTxBuffer = NULL;
	pI2CHandle->TxLen = 0U;
}



/*********************************************************************
 * @fn                - I2C_CloseReception
 *
 * @brief             - Terminate an interrupt-driven master receive
 *                      operation and restore driver state.
 *
 * @param[in]         - pI2CHandle : Pointer to I2C handle structure.
 *
 * @return            - none
 *
 * @Note              -
 *                      1. Disables TX/RX buffer interrupts
 *                         (ITBUFEN).
 *
 *                      2. Disables event interrupts
 *                         (ITEVTEN).
 *
 *                      3. Resets reception related state
 *                         variables.
 *
 *                      4. Updates driver state to:
 *
 *                              I2C_READY
 *
 *                      5. Restores ACK configuration according to
 *                         the user's original driver settings.
 *
 *                      6. Called automatically when interrupt-driven
 *                         reception completes.
 *
 *                      7. Intended for internal driver use only.
 *
 */
void I2C_CloseReception(I2C_Handle_t *pI2CHandle)
{
	//Implement the code to disable ITBUFEN Control Bit
	pI2CHandle->pI2Cx->CR2 &= ~(1U << I2C_CR2_ITBUFEN);

	//Implement the code to disable ITEVTEN Control Bit
	pI2CHandle->pI2Cx->CR2 &= ~(1U << I2C_CR2_ITEVTEN);

	pI2CHandle->TxRxState = I2C_READY;
	pI2CHandle->pRxBuffer = NULL;
	pI2CHandle->RxLen = 0U;
	pI2CHandle->RxSize = 0U;

	//Re-enable ACKing
	if(pI2CHandle->I2C_Config.I2C_ACKControl == I2C_ACK_ENABLE)
	{
		I2C_AckControl(pI2CHandle->pI2Cx,
					   I2C_ACK_ENABLE);
	}

}



/*********************************************************************
 * @fn      		  - I2C_ApplicationEventCallback
 *
 * @brief             - Weak callback function for I2C application
 * 						events.
 *
 * @param[in]         - pI2CHandle : Pointer to I2C handle structure
 *                                   which contains I2C peripheral
 *                                   information and states.
 *
 * @param[in]		  - AppEvent : Event generated by I2C driver.
 *
 * @return            - none
 *
 * @Note              -
 * 						1. This is a weak implementation provided by
 *                         the driver.
 *
 *                      2. Application can override this function by
 *                         implementing the same function in user code.
 *
 *                      3. This callback is used by the I2C driver to
 *                         notify the application about important I2C
 *                         events.
 *
 *                      4. Common I2C events may include:
 *
 *                           I2C_EV_TX_CMPLT
 *                           I2C_EV_RX_CMPLT
 *                           I2C_EV_STOP
 *                           I2C_ER_BERR
 *
 *                      5. Typical application usage:
 *
 *                           - Toggle LED after TX complete
 *                           - Process received data
 *                           - Handle communication errors
 *                           - Restart transactions
 *
 *                      6. '__weak' keyword means:
 *
 *                           - Linker gives low priority to this
 *                             implementation.
 *
 *                           - User application may provide another
 *                             implementation with same function name.
 *
 *                           - User-defined version overrides this
 *                             weak version automatically.
 *
 *                      7. If user does NOT override this function,
 *                         this empty implementation will be used.
 *
 *                      8. This mechanism provides abstraction between
 *                         driver layer and application layer.
 *
 *                      9. Driver becomes reusable because driver does
 *                         not contain application-specific code.
 *
 */
__weak void I2C_ApplicationEventCallback(I2C_Handle_t *pI2CHandle,
										 uint8_t AppEvent)
{
	/* Weak implementation
	   Application may override this function */
}
