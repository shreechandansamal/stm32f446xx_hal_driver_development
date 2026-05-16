/*
 * stm32f446xx_i2c_driver.c
 *
 *  Created on: May 12, 2026
 *      Author: chandan
 */

#include"stm32f446xx_i2c_driver.h"

uint16_t AHB_PreScaler[8U] = {2U, 4U, 8U, 16U, 64U, 128U, 256U, 512U};
uint8_t APB1_PreScaler[4U] = { 2U, 4U, 8U, 16U};


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
uint32_t  RCC_GetPLLOutputClock()
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
 * @return            - pclk1
 *
 * @Note              - this function is used for calculate the speed
 * 						of APB1 and will be used by FREQ bit of CR2
 * 						register.

 */
uint32_t RCC_GetPCLK1Value(void)
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

	pclk1 =  (SystemClk / ahbp) /apb1p;

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

    //a. Configure ACK Control Bit of CR1
    tempreg |= pI2CHandle->I2C_Config.I2C_ACKControl << I2C_CR1_ACK;
    pI2CHandle->pI2Cx->CR1 = tempreg;

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
    	tempreg |= (ccr_value & 0xFFFU); /*CCR field is 12 bits rest
    	 	 	 	 	 	 	 	 	  need to be masked*/
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
    	tempreg |= (ccr_value & 0xFFFU);
    	pI2CHandle->pI2Cx->CRR = tempreg;
    }

    //e.Trise calculation


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

