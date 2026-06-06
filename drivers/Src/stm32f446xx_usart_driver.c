/*
 * stm32f446xx_usart_driver.c
 *
 *  Created on: Jun 5, 2026
 *      Author: chandan
 */

#include "stm32f446xx_usart_driver.h"





/*********************************************************************
 * @fn                - USART_PeriClockControl
 *
 * @brief             - Enable or Disable the peripheral clock for
 *                      the given USART peripheral.
 *
 * @param[in]         - pUSARTx  : Base address of the USART peripheral
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
 *                      3. Before accessing or configuring any USART
 *                         register, the corresponding peripheral
 *                         clock must be enabled.
 *
 *                      4. Accessing peripheral registers while
 *  					   the clock is disabled may produce
 *  					   undefined results:
 *                           - Register access may not work properly
 *                           - Peripheral configuration will fail
 *                           - USART communication will not operate
 *
 *                      5. USART1, USART6 are connected to APB2 bus.
 *                         USART2, USART3, UART4, UART5 are connected
 *                         to APB1 bus.
 *
 *                      6. Disabling the peripheral clock stops the
 *                         clock supply to the USART peripheral and
 *                         reduces dynamic power consumption.
 *
 *                      7. Peripheral clock should not be disabled
 *                         during an active USART communication.
 *
 */
void USART_PeriClockControl(USART_RegDef_t *pUSARTx,
						    uint8_t EnorDi)
{
    if(EnorDi == ENABLE)
    {
        if(pUSARTx == USART1)
        {
            USART1_PCLK_EN();
        }
        else if(pUSARTx == USART2)
        {
            USART2_PCLK_EN();
        }
        else if(pUSARTx == USART3)
        {
            USART3_PCLK_EN();
        }
        else if(pUSARTx == UART4)
        {
            UART4_PCLK_EN();
        }
        else if(pUSARTx == UART5)
        {
            UART5_PCLK_EN();
        }
        else if(pUSARTx == USART6)
        {
            USART6_PCLK_EN();
        }
    }
    else
    {
        if(pUSARTx == USART1)
        {
            USART1_PCLK_DI();
        }
        else if(pUSARTx == USART2)
        {
            USART2_PCLK_DI();
        }
        else if(pUSARTx == USART3)
        {
            USART3_PCLK_DI();
        }
        else if(pUSARTx == UART4)
        {
            UART4_PCLK_DI();
        }
        else if(pUSARTx == UART5)
        {
            UART5_PCLK_DI();
        }
        else if(pUSARTx == USART6)
        {
            USART6_PCLK_DI();
        }
    }
}



/*********************************************************************
 * @fn                - USART_DeInit
 *
 * @brief             - Reset the selected USART peripheral registers
 *                      to their hardware reset values.
 *
 * @param[in]         - pUSARTx : Base address of the USART peripheral
 *                                (USART1, USART2, UART4..)
 *
 * @return            - none
 *
 * @Note              -
 *                      1. This function forces the selected USART
 *                         peripheral into reset state using the
 *                         RCC peripheral reset mechanism.
 *
 *                      2. All USART control, configuration and status
 *                         registers are restored to their default
 *                         reset values defined by hardware.
 *
 *                      3. Any ongoing USART communication is aborted
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
 *                         using USART_Init() before it can be used
 *                         for communication.
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
 *                         an active USART transaction because data
 *                         loss may occur.
 *
 *                      8. Supported peripherals:
 *
 *                           - USART1
 *                           - USART2
 *                           - USART3
 *                           - UART4
 *                           - UART5
 *                           - USART6
 *
 */
void USART_DeInit(USART_RegDef_t *pUSARTx)
{
    if(pUSARTx == USART1)
    {
    	USART1_REG_RESET();
    }
    else if(pUSARTx == USART2)
    {
        USART2_REG_RESET();
    }
    else if(pUSARTx == USART3)
    {
        USART3_REG_RESET();
    }
    else if(pUSARTx == UART4)
    {
        UART4_REG_RESET();
    }
    else if(pUSARTx == UART5)
    {
        UART5_REG_RESET();
    }
    else if(pUSARTx == USART6)
    {
        USART6_REG_RESET();
    }
}



/*********************************************************************
 * @fn                - USART_GetFlagStatus
 *
 * @brief             - Get the status of a specific USART status flag.
 *
 * @param[in]         - pUSARTx   : Base address of the USART peripheral
 * @param[in]         - FlagName  : USART status flag to check
 *
 * @return            - FLAG_SET   : Flag is set
 *                      FLAG_RESET : Flag is reset
 *
 * @Note              -
 *                      1. This function checks the USART_SR register
 *  					   for the specified status flag.
 *
 *                      2. USART status flags provide information about
 *                         the current state of the peripheral and
 *                         communication progress.
 *
 *                      3. This API is commonly used in polling-based
 *                         USART communication to wait for specific
 *                         hardware events.
 *
 *                      4. Example:
 *
 *                              while(USART_GetFlagStatus
 *                                   (I2C2, I2C_FLAG_PE)
 *                                   == FLAG_RESET);
 *
 *                      5. Typical flags checked using this API:
 *
 *                           - PE     (Parity error)
 *                           - FE     (Framing error)
 *                           - NF     (Noise detected flag)
 *                           - ORE    (Overrun error)
 *                           - IDLE   (IDLE line detected)
 *                           - RXNE   (Receive Data Register Not Empty)
 *                           - TC     (Transmission complete)
 *                           - TXE    (Transmit Data Register Empty)
 *                           - LBD    (LIN break detection flag)
 *                           - CTS    (CTS flag)
 *
 *                      6. Status flags are hardware controlled and
 *                         updated automatically by the USART peripheral.
 *
 *                      7. This function only reads the flag status
 *                         and does not modify any peripheral register.
 *
 */
uint8_t USART_GetFlagStatus(USART_RegDef_t *pUSARTx,
						  	uint8_t FlagName)
{
    if(pUSARTx->SR & FlagName)
    {
        return FLAG_SET;
    }

    return FLAG_RESET;
}



/*********************************************************************
 * @fn                - USART_PeripheralControl
 *
 * @brief             - Enable or disable the USART peripheral using
 *                      the PE (Peripheral Enable) bit.
 *
 * @param[in]         - pUSARTx : Pointer to the target USART peripheral
 *                                instance.
 * @param[in]         - EnOrDi  : ENABLE or DISABLE macros
 *
 * @return            - none
 *
 * @Note              -
 *                      1. Controls the UE (USART Enable) bit
 *                         in the CR1 register.
 *
 *                      2. When enabled, the USART peripheral becomes
 *                         operational and can participate in bus
 *                         communication.
 *
 *                      3. When disabled, the USART peripheral stops
 *                         all communication activity and ignores
 *                         most protocol-related operations.
 *
 *                      4. Timing-related configuration must not be
 *                         modified while the peripheral is actively
 *                         communicating on the bus.
 *
 *                      5. Disabling the peripheral does not reset
 *                         its configuration registers.
 *
 *                      6. To completely restore the peripheral to
 *                         its hardware reset state, use:
 *
 *                              USART_DeInit()
 *
 *                      7. This function only controls the UE bit and
 *                         does not modify any other I2C configuration.
 *
 */
void USART_PeripheralControl(USART_RegDef_t *pUSARTx,
	    					 uint8_t EnorDi)
{
	if(EnOrDi == ENABLE)
	{
		// Enable USART peripheral
		pUSARTx->CR1 |= (1U << USART_CR1_UE);
	}
	else
	{
		// Disable USART peripheral
		pUSARTx->CR1 &= ~(1U << USART_CR1_UE);
	}

}



/*********************************************************************
 * @fn                - USART_IRQInterruptConfig
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
void USART_IRQInterruptConfig(uint8_t IRQNumber,
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
 * @fn                - USART_IRQPriorityConfig
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
void USART_IRQPriorityConfig(uint8_t IRQNumber,
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

