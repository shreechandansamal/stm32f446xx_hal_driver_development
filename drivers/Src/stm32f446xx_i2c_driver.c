/*
 * stm32f446xx_i2c_driver.c
 *
 *  Created on: May 12, 2026
 *      Author: chandan
 */

#include "stm32f446xx_i2c_driver.h"

uint16_t AHB_PreScaler[8U] = {2U, 4U, 8U, 16U, 64U, 128U, 256U, 512U};
uint8_t APB1_PreScaler[4U] = { 2U, 4U, 8U, 16U};

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
static uint32_t RCC_GetPCLK1Value(void);
static uint32_t  RCC_GetPLLOutputClock(void);
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
 *                      4. If peripheral clock is not enabled:
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
 * @fn      		  - RCC_GetPCLK1Value
 *
 * @brief             - Calculate the speed of the PLL
 *
 * @param[in]         -
 *
 * @return            -
 *
 * @Note              - future implementation

 */
static uint32_t  RCC_GetPLLOutputClock()
{
	return 0;
}



/*********************************************************************
 * @fn      		  - RCC_GetPCLK1Value
 *
 * @brief             - Calculate the Peripheral Clock Speed of APB1
 *
 * @param[in]         - void
 *
 * @return            - pclk1(Hz)
 *
 * @Note              - this function is used for calculate the speed
 * 						of APB1

 */
static uint32_t RCC_GetPCLK1Value(void)
{
	uint32_t pclk1,SystemClk;
	uint16_t ahbp,apb1p;
	uint8_t clksrc,temp;

	//test for the System Clock Source
	clksrc = ((RCC->CFGR >> 2U) & 0x3U);

	if(clksrc == 0U)
	{
		SystemClk = 16000000UL;
	}else if(clksrc == 1U)
	{
		SystemClk = 8000000UL;
	}else if (clksrc == 2U)
	{
		//skipped this because PLL is not required now
		SystemClk = RCC_GetPLLOutputClock();
	}

	//test for AHB Pre-Scaler
	temp = ((RCC->CFGR >> 4U) & 0xFU);

	if(temp < 8U) //system clock not divided
	{
		ahbp = 1U;
	}else
	{
		ahbp = AHB_PreScaler[temp - 8U];
	}



	//test for APB1 Pre-Scaler
	temp = ((RCC->CFGR >> 10U) & 0x7U);

	if(temp < 4U) //system clock not divided
	{
		apb1p = 1U;
	}else
	{
		apb1p = APB1_PreScaler[temp - 4U];
	}

	pclk1 =  (SystemClk / ahbp) / apb1p;

	return pclk1;
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
 *
 */
void I2C_Init(I2C_Handle_t *pI2CHandle)
{

    uint32_t tempreg = 0U;

    //enable the clock for the I2Cx peripheral
    I2C_PeriClockControl(pI2CHandle->pI2Cx,
    					 ENABLE);

/*    //a. Configure ACK Control Bit of CR1 (as ACK bit can be only ENABLE(set) while PE=1,
 * 						                     so this part has shifted to I2C_PeripheralControl
 * 						                     function)
    tempreg |= pI2CHandle->I2C_Config.I2C_ACKControl << I2C_CR1_ACK;
    pI2CHandle->pI2Cx->CR1 = tempreg; */

    //b. Configure the FREQ field Bit of CR2
    tempreg = 0U;
    tempreg |= RCC_GetPCLK1Value() / 1000000U; /*divide by 1MHz because
    											 to get the MSB number
    											 ex- 16MHz returned by
    											 RCC_GetPCLK1Value so
    											 divide by 1MHz gives
    											 16, so we want this*/

    pI2CHandle->pI2Cx->CR2 = (tempreg & 0x3FU); /*FREQ field is 6 bits
     	 	 	 	 	 	 	 	 	 	 	 so we need to set
     	 	 	 	 	 	 	 	 	 	 	 the tempreg value
     	 	 	 	 	 	 	 	 	 	 	 into those Bit 5:0
     	 	 	 	 	 	 	 	 	 	 	 and rest need to be
     	 	 	 	 	 	 	 	 	 	 	 masked*/

    //c. Configure the device own address
    tempreg = 0U;
    tempreg |= pI2CHandle->I2C_Config.I2C_DeviceAddress << I2C_OAR1_ADD_BIT_1_7;
    tempreg |= (1U << 14U); /*As per Reference Manual, we Should
    						  always be kept at 1 by software.*/
    pI2CHandle->pI2Cx->OAR1 = tempreg;

    //d. CCR calculation for SCL speed
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
    		 * 		   CCR = Tscl / (3 * TPCLK1) */
    		ccr_value = (RCC_GetPCLK1Value() / (3U * pI2CHandle->I2C_Config.I2C_SCLSpeed));
    	}
    	else
    	{
    		/*Duty 1 : Thigh = 9 * CCR * TPCLK1, Tlow = 16 * CCR * TPCLK1
    		 * 		   Tscl = (9 * CCR * TPCLK1) + (16 * CCR * TPCLK1)
    		 * 		   Tscl = 25 * CCR * TPCLK1
    		 *
    		 * 		   CCR = Tscl / (25 * TPCLK1) */
    		ccr_value = (RCC_GetPCLK1Value() / (25U * pI2CHandle->I2C_Config.I2C_SCLSpeed));
    	}
    }
	tempreg |= (ccr_value & 0xFFFU); //CCR[11:0]
	pI2CHandle->pI2Cx->CCR = tempreg;

    //e.Trise calculation
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
 *                      to their default reset values.
 *
 * @param[in]         - pI2Cx : Base address of the I2C peripheral
 *
 * @return            - none
 *
 * @Note              -
 *                      1. This function forces the I2C peripheral
 *                         into hardware reset state.
 *
 *                      2. All I2C registers are restored to their
 *                         default reset values defined by hardware.
 *
 *                      3. This operation is generally performed using
 *                         RCC peripheral reset control registers.
 *
 *                      4. After de-initialization:
 *                           - I2C configuration is lost
 *                           - Communication stops
 *                           - Control/status registers return to
 *                             reset state
 *
 *                      5. I2C must be initialized again using
 *                         I2C_Init() before reuse.
 *
 *                      6. This function is useful when:
 *                           - Re-configuring I2C peripheral
 *                           - Recovering from communication faults
 *                           - Restarting peripheral operation
 *                           - Returning peripheral to clean state
 *
 *                      7. De-initialization should NOT be performed
 *                         during an active I2C transaction.
 *
 *                      8. Peripheral clock may still remain enabled
 *                         after de-initialization depending on RCC
 *                         reset implementation.
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



/********************************************************************
 * @fn				  - AckControl
 *
 * @brief			  - Enable or Disable the ACK bit.
 *
 * @param[in]         - pI2Cx   : Base address of I2C peripheral
 * @param[in]         - EnOrDi  : ENABLE or DISABLE macros
 *
 * @return			  - none
 *
 * @Note			  - none
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
 * @param[in]         - FlagName  : I2C status flag to check
 *
 * @return            - FLAG_SET   : Flag is set
 *                      FLAG_RESET : Flag is reset
 *
 * @Note              -
 *                      1. This function checks the I2C Status
 *                         Register (I2C_SR) for a specific flag.
 *
 *                      2. I2C status flags indicate the current
 *                         state of I2C communication hardware.
 *
 *                      3. This API is commonly used in polling-based
 *                         I2C communication.
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
 *                     10. Status flags are hardware controlled and
 *                         updated automatically by I2C peripheral.
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
 * @brief             - Generate Start Condition
 *
 * @param[in]         - pI2Cx : Base address of the I2C peripheral
 *
 * @return            - none
 *
 * @Note              -
 *
 */
static void I2C_GenerateStartCondition(I2C_RegDef_t *pI2Cx)
{
	pI2Cx->CR1 |= (1U << I2C_CR1_START);
}



/*********************************************************************
 * @fn                - I2C_GenerateEndCondition
 *
 * @brief             - Generate End Condition
 *
 * @param[in]         - pI2Cx : Base address of the I2C peripheral
 *
 * @return            - none
 *
 * @Note              -
 *
 */
void I2C_GenerateStopCondition(I2C_RegDef_t *pI2Cx)
{
	pI2Cx->CR1 |= (1U << I2C_CR1_STOP);
}



/*********************************************************************
 * @fn                - I2C_ExecuteAddressPhase
 *
 * @brief             - Set the Slave address and r/nw bit into the
 * 						Address Phase.
 *
 * @param[in]         - pI2Cx     : Base address of the I2C peripheral
 * @param[in]         - SlaveAddr  : 7 bit Slave Address
 * @param[in]         - TransferDir  : Read or Write
 *
 * @return            - none
 *
 * @Note              - set the 7 bit slave address into the address
 * 						phase, but as the data by default goes to the
 * 						LSB bit, but for us the 8 Bit Address Phase
 * 						consist of 7 bits Slave address and 1 bit r/nw
 * 						so, we need to shift those 7 bits into the MSB
 * 						then make the LSB bit 1 or 0 to indicate the
 * 						Direction.
 *
 * 						use TransferDir as:
 * 						I2C_DIR_READ
 * 						I2C_DIR_WRITE
 *
 * 						SlaveAddr = SlaveAddr + R/nW
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
 * @brief             - Clear ADDR flag
 *
 * @param[in]         - pI2CHandle : Pointer to I2C handle structure
 *                                   containing I2C peripheral base
 *                                   address and configuration data.
 *
 * @return            - none
 *
 * @Note              -
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
 * @fn      		  - I2C_PeripheralControl
 *
 * @brief             -	Enable or Disable the I2C peripheral
 *                      using PE bit in CR1 register.
 *
 * @param[in]         - pI2Cx   : Base address of I2C peripheral
 * @param[in]         - EnOrDi  : ENABLE or DISABLE macros
 *
 * @return            - none
 *
 * @Note              -

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
 * @brief             - Master Transmit data over I2C peripheral using
 *                      blocking/polling method.
 *
 * @param[in]         - pI2CHandle : Pointer to I2C handle structure
 *                                   containing I2C peripheral base
 *                                   address and configuration data.
 * @param[in]		  - pTxBuffer : Pointer to transmit data buffer
 * @param[in]		  - Len : Length of transmit data buffer in bytes
 * @param[in]		  - SlaveAddr : 7 bit slave address
 * @param[in]		  - Sr : repeated start enable or disable
 *
 * @return            - none
 *
 * @Note              -
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
 * @brief             - Master Transmit data over I2C peripheral using
 *                      interrupt/non-blocking method.
 *
 * @param[in]         - pI2CHandle : Pointer to I2C handle structure
 *                                   containing I2C peripheral base
 *                                   address and configuration data.
 * @param[in]		  - pTxBuffer : Pointer to transmit data buffer
 * @param[in]		  - Len : Length of transmit data buffer in bytes
 * @param[in]		  - SlaveAddr : 7 bit slave address
 * @param[in]		  - Sr : repeated start enable or disable
 *
 * @return            - none
 *
 * @Note              -
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
 * @brief             - Master Receive data over I2C peripheral using
 *                      blocking/polling method.
 *
 * @param[in]         - pI2CHandle : Pointer to I2C handle structure
 *                                   containing I2C peripheral base
 *                                   address and configuration data.
 * @param[in]		  - pRxBuffer : Pointer to receive data buffer
 * @param[in]		  - Len : Length of receive data buffer in bytes
 * @param[in]		  - SlaveAddr : 7 bit slave address
 * @param[in]		  - Sr : repeated start enable or disable
 *
 * @return            - none
 *
 * @Note              -
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
 * @brief             - Master Receive data over I2C peripheral using
 *                      interrupt/non-blocking method.
 *
 * @param[in]         - pI2CHandle : Pointer to I2C handle structure
 *                                   containing I2C peripheral base
 *                                   address and configuration data.
 * @param[in]		  - pRxBuffer : Pointer to receive data buffer
 * @param[in]		  - Len : Length of receive data buffer in bytes
 * @param[in]		  - SlaveAddr : 7 bit slave address
 * @param[in]		  - Sr : repeated start enable or disable
 *
 * @return            - none
 *
 * @Note              -
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
* @fn                - I2C_EV_IRQHandling
*
* @brief             - Handles all I2C interrupt events generated by
 * 					   the I2C peripheral.
*
* @param[in]         - pI2CHandle : Pointer to I2C handle structure
*                                   containing I2C peripheral base
*                                   address and configuration data.
*
* @return            - none
*
* @Note              -
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
	   Note : Stop detection flag is applicable only slave mode .
	   For master this flag will never be set
	   The below code block will not be executed by the master
	   since STOPF will not set in master mode */
	//STOPF will be only set by slave
	temp3 = pI2CHandle->pI2Cx->SR1 & (1U << I2C_SR1_STOPF);
	if(temp1 && temp3)
	{
		/* interrupt is generated because of STOPF event
		   clear the STOPF,
		   a. read SR1(already read above pI2Cx->SR1)
		   b. write to CR1 */
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
		//device should be in master mode [MSL = 1]
		if(pI2CHandle->pI2Cx->SR2 & (1U << I2C_SR2_MSL))
		{
			/* Confirm that only if the application state
			 * is busy in TX  */
			if(pI2CHandle->TxRxState == I2C_BUSY_IN_TX)
			{
				I2C_MasterHandleTXEInterrupt(pI2CHandle);
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
	}
}



/*********************************************************************
* @fn                - I2C_ER_IRQHandling
*
* @brief             - Handles all I2C interrupt errors generated by
 * 					   the I2C peripheral.
*
* @param[in]         - pI2CHandle : Pointer to I2C handle structure
*                                   containing I2C peripheral base
*                                   address and configuration data.
*
* @return            - none
*
* @Note              -
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
* @brief             - Internal helper function to handle I2C TXE
* 					   (Transmit Buffer Empty) interrupt event.
*
* @param[in]         - pI2CHandle : Pointer to I2C handle structure
*                                   containing I2C peripheral base
*                                   address and configuration data.
*
* @return            - none
*
* @Note              -
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
* @brief             - Internal helper function to handle I2C RXNE
* 					   (Receive Buffer Not Empty) interrupt event.
*
* @param[in]         - pI2CHandle : Pointer to I2C handle structure
*                                   containing I2C peripheral base
*                                   address and configuration data.
*
* @return            - none
*
* @Note              -
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
* @brief             - Closes an ongoing I2C send data in Interrupt
* 					   mode.
*
* @param[in]         - pI2CHandle : Pointer to I2C handle structure
*                                   containing I2C peripheral base
*                                   address and configuration data.
*
* @return            - none
*
* @Note              -
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
* @brief             - Closes an ongoing I2C receive data in Interrupt
* 					   mode.
*
* @param[in]         - pI2CHandle : Pointer to I2C handle structure
*                                   containing I2C peripheral base
*                                   address and configuration data.
*
* @return            - none
*
* @Note              -
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
