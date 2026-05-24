/*
 * stm32f446xx_i2c_driver.c
 *
 *  Created on: May 12, 2026
 *      Author: chandan
 */

#include"stm32f446xx_i2c_driver.h"

uint16_t AHB_PreScaler[8U] = {2U, 4U, 8U, 16U, 64U, 128U, 256U, 512U};
uint8_t APB1_PreScaler[4U] = { 2U, 4U, 8U, 16U};



/********************************************************************
 * @Note- private helper function section
 *
 * @brief  - don't declare these prototypes in the header because these are private functions
 *  		 and we don't want to expose it to user application
 *
 * use static keyword to indicate these are private helper function, so if application tries
 * to call them, then compiler will issue an error.
 */
static uint32_t RCC_GetPCLK1Value(void);
static uint32_t  RCC_GetPLLOutputClock(void);
static void I2C_GenerateStartCondition(I2C_RegDef_t *pI2Cx);
static void I2C_GenerateStopCondition(I2C_RegDef_t *pI2Cx);
static void I2C_ExecuteAddressPhase(I2C_RegDef_t *pI2Cx,
									uint8_t SlaveAddr);
static void I2C_ClearADDRFlag(I2C_RegDef_t *pI2Cx);
static void I2C_AckControl(I2C_RegDef_t *pI2Cx,
						   uint8_t EnOrDi);

/***********************************************************************/



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
    I2C_PeriClockControl(pI2CHandle->pI2Cx, ENABLE);

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
static void I2C_AckControl(I2C_RegDef_t *pI2Cx,
						   uint8_t EnOrDi)
{
	if (EnOrDi == ENABLE)
	{
		pI2Cx->CR1 |= (1 << I2C_CR1_ACK);
	}
	else
	{
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
static void I2C_GenerateStopCondition(I2C_RegDef_t *pI2Cx)
{
	pI2Cx->CR1 |= (1U << I2C_CR1_STOP);
}



/*********************************************************************
 * @fn                - I2C_ExecuteAddressPhase
 *
 * @brief             - Set the Slave address and r/nw bit as write
 * 						into the Address Phase.
 *
 * @param[in]         - pI2Cx     : Base address of the I2C peripheral
 * @param[in]         - SlaveAddr  : 7 bit Slave Address
 *
 * @return            - none
 *
 * @Note              - set the 7 bit slave address into the address
 * 						phase, but as the data by default goes to the
 * 						LSB bit, but for us the 8 Bit Address Phase
 * 						consist of 7 bits Slave address and 1 bit r/nw
 * 						so, we need to shift those 7 bits into the MSB
 * 						then rest the LSB bit or make it 0 to indicate
 * 						r/nw bit as write.
 *
 * 						SlaveAddr = SlaveAddr + R/nW
 *
 */
static void I2C_ExecuteAddressPhase(I2C_RegDef_t *pI2Cx,
							        uint8_t SlaveAddr)
{
	SlaveAddr = SlaveAddr << 1; /*shift the 7 bits address moved by 1 bit
								  to the MSB*/
	SlaveAddr &= ~(1); // LSB is R/nW bit which must be set to 0 for WRITE
	pI2Cx->DR = SlaveAddr;
}



/*********************************************************************
 * @fn                - I2C_ClearADDRFlag
 *
 * @brief             - ADDR Bit clear by reading SR1 register followed by SR2 register
 *
 * @param[in]         - pI2Cx     : Base address of the I2C peripheral
 *
 * @return            - none
 *
 * @Note              -
 *
 */
static void I2C_ClearADDRFlag(I2C_RegDef_t *pI2Cx)
{
	//1. read SR1
	(void)pI2Cx->SR1;
	//2. followed by read SR2
	(void)pI2Cx->SR2;
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
		pI2Cx->CR1 |= (1 << I2C_CR1_PE);
		I2C_AckControl(pI2Cx, ENABLE); //Enable ack, this can not be set before PE = 1
	}
	else
	{
		// Disable I2C peripheral
		pI2Cx->CR1 &= ~(1 << I2C_CR1_PE);
	}

}



/*********************************************************************
 * @fn                - I2C_MasterSendData
 *
 * @brief             - Master Transmit data over I2C peripheral using
 *                      blocking/polling method.
 *
 * @param[in]         - pI2Cx : Base address of the I2C peripheral
 * @param[in]		  - pTxBuffer : Pointer to transmit data buffer
 * @param[in]		  - Len : Length of transmit data buffer in bytes
 * @param[in]		  - SlaveAddr : 7 bit slave address
 *
 * @return            - none
 *
 * @Note              -
 *
 */
void I2C_MasterSendData(I2C_Handle_t *pI2CHandle,
						uint8_t *pTxBuffer,
						uint32_t Len,
						uint8_t SlaveAddr)
{
	//1. Generate the START Condition
	I2C_GenerateStartCondition(pI2CHandle->pI2Cx);

	/*2. Confirm that START Generation is completed by checking the SB flag in the SR1
	     Note: Until SB is cleared SCL will be stretched (pulled to LOW) */
	while(I2C_GetFlagStatus(pI2CHandle->pI2Cx, I2C_FLAG_SB) == FLAG_RESET);

	//3. Send the address of the slave with r/nw bit set to w(0) (total 8 bits)
	I2C_ExecuteAddressPhase(pI2CHandle->pI2Cx, SlaveAddr);

	//4. Confirm that address phase is completed by checking the ADDR flag in the SR1
	while(I2C_GetFlagStatus(pI2CHandle->pI2Cx, I2C_FLAG_ADDR) == FLAG_RESET);

	/*5. Clear the ADDR flag according to its software sequence
	 	 Note: Until ADDR is cleared SCL will be stretched (pulled to LOW) */
	I2C_ClearADDRFlag(pI2CHandle->pI2Cx);

	//6. Send the data until Len becomes 0
	while(Len > 0)
	{
		//wait until TXE is set
		while(I2C_GetFlagStatus(pI2CHandle->pI2Cx, I2C_FLAG_TXE) == FLAG_RESET); //Check if transmission data register is empty
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
	while(I2C_GetFlagStatus(pI2CHandle->pI2Cx, I2C_FLAG_TXE) == FLAG_RESET); //Check if data register is empty
	while(I2C_GetFlagStatus(pI2CHandle->pI2Cx, I2C_FLAG_BTF) == FLAG_RESET); //Check if last byte transmission is completed


	/*8. Generate STOP Condition and Master need to wait for the completion of STOP
	 * 	 Condition.
	 * 	 Note: Generating STOP, automatically clears the BTF.
	 */
	I2C_GenerateStopCondition(pI2CHandle->pI2Cx);




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

