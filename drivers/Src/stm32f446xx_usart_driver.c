/*
 * stm32f446xx_usart_driver.c
 *
 *  Created on: Jun 5, 2026
 *      Author: chandan
 */

#include "stm32f446xx_usart_driver.h"
#include "stm32f446xx_rcc_driver.h"



/********************************************************************
 * @Note- private helper function section
 *
 * @brief  - don't declare these prototypes in the header because these are private functions
 *  		 and we don't want to expose it to user application
 *
 * use static keyword to indicate these are private helper function, so if application tries
 * to call them, then compiler will issue an error.
 */
static void USART_FlushReceiveData(USART_RegDef_t *pUSARTx);

/***********************************************************************/




/*********************************************************************
 * @fn                - USART_SetBaudRate
 *
 * @brief             - Configure the USART baud rate by programming
 *                      the Baud Rate Register (USART_BRR).
 *
 * @param[in]         - pUSARTx   : Base address of the USART peripheral
 * @param[in]         - BaudRate  : Desired communication baud rate
 *
 * @return            - none
 *
 * @Note              -
 *                      1. USART communication requires both transmitter
 *                         and receiver to operate at approximately the
 *                         same baud rate.
 *
 *                      2. This function calculates the BRR register
 *                         value based on:
 *                           - Peripheral clock frequency (PCLK)
 *                           - Desired baud rate
 *                           - Oversampling configuration
 *
 *                      3. The USART peripheral derives its internal
 *                         bit timing from the APB peripheral clock.
 *
 *                      4. Clock source depends on the USART instance:
 *                           USART1, USART6 → APB2 Clock
 *                           USART2, USART3,
 *                           UART4, UART5  → APB1 Clock
 *
 *                      5. The function automatically determines the
 *                         appropriate APB clock domain and obtains
 *                         the corresponding clock frequency.
 *
 *                      6. Baud rate generation is controlled through
 *                         the USART_BRR (Baud Rate Register).
 *
 *                      7. BRR register consists of:
 *                           - Mantissa Portion
 *                           - Fraction Portion
 *                         Together these form the USARTDIV value.
 *
 *                      8. USARTDIV is calculated according to the
 *                         oversampling mode selected in CR1.
 *
 *                      9. Two oversampling modes are supported:
 *                           OVER8 = 0 → Oversampling by 16
 *                           OVER8 = 1 → Oversampling by 8
 *
 *                     10. Oversampling by 16 provides:
 *                           - Better noise immunity
 *                           - Higher sampling accuracy
 *                           - Recommended for most applications
 *
 *                     11. Oversampling by 8 provides:
 *                           - Higher maximum baud rate
 *                           - Reduced timing margin
 *                           - Lower noise tolerance
 *
 *                     12. Fractional calculation includes rounding
 *                         to minimize baud rate error introduced by
 *                         integer arithmetic.
 *
 *                     13. The computed Mantissa value is placed in
 *                         BRR[15:4].
 *
 *                     14. The computed Fraction value is placed in
 *                         BRR[3:0] when OVER8 = 0.
 *
 *                     15. When OVER8 = 1, only BRR[2:0] are used for
 *                         the fractional portion.
 *
 *                     16. Baud rate accuracy depends on:
 *                           - Accuracy of system clock
 *                           - APB clock configuration
 *                           - Desired baud rate
 *                           - Oversampling mode
 *
 *                     17. Large baud rate errors may result in:
 *                           - Framing errors
 *                           - Corrupted data
 *                           - Communication failure
 *
 *                     18. This API should normally be called during
 *                         USART initialization before enabling the
 *                         USART peripheral.
 *
 *                     19. Changing the baud rate while communication
 *                         is active may corrupt ongoing data transfer.
 *
 *                     20. Common baud rates include:
 *                           9600
 *                           19200
 *                           38400
 *                           57600
 *                           115200
 *                           230400
 *                           460800
 *                           921600
 *
 *                     21. It is recommended that both communicating
 *                         devices use identical baud rate and frame
 *                         format settings.
 *
 * @see USART_Init()
 *
 */
void USART_SetBaudRate(USART_RegDef_t *pUSARTx,
					   uint32_t BaudRate)
{

  //Variable to hold the APB clock
  uint32_t PCLKx;

  uint32_t usartdiv;

  //variables to hold Mantissa and Fraction values
  uint32_t M_part, F_part;

  uint32_t tempreg = 0U;

  //Get the value of APB bus clock in to the variable PCLKx
  if(pUSARTx == USART1 || pUSARTx == USART6)
  {
	   //USART1 and USART6 are hanging on APB2 bus
	   PCLKx = RCC_GetPCLK2Value();
  }else
  {
	   PCLKx = RCC_GetPCLK1Value();
  }

  //Check for OVER8 configuration bit
  if(pUSARTx->CR1 & (1U << USART_CR1_OVER8))
  {
	   //OVER8 = 1 , over sampling by 8
	   usartdiv = ((25U * PCLKx) / (2U * BaudRate));
  }else
  {
	   //over sampling by 16
	   usartdiv = ((25U * PCLKx) / (4U * BaudRate));
  }

  //Calculate the Mantissa part
  M_part = usartdiv / 100U;

  //Place the Mantissa part in appropriate bit position . refer USART_BRR
  tempreg |= M_part << 4U;

  //Extract the fraction part
  F_part = (usartdiv - (M_part * 100U));

  //Calculate the final fractional
  if(pUSARTx->CR1 & (1U << USART_CR1_OVER8))
   {
	  //OVER8 = 1 , over sampling by 8
	  F_part = ((( F_part * 8U) + 50U) / 100U) & ((uint8_t)0x07U);

   }else
   {
	   //over sampling by 16
	   F_part = ((( F_part * 16U) + 50U) / 100U) & ((uint8_t)0x0FU);

   }

  //Place the fractional part in appropriate bit position . refer USART_BRR
  tempreg |= F_part;

  //copy the value of tempreg in to BRR register
  pUSARTx->BRR = tempreg;
}



/*********************************************************************
 * @fn                - USART_PeriClockControl
 *
 * @brief             - Enable or disable the peripheral clock for
 *                      the selected USART/UART peripheral.
 *
 * @param[in]         - pUSARTx : Base address of the USART/UART
 *                                peripheral.
 * @param[in]         - EnOrDi  : ENABLE or DISABLE macros.
 *
 * @return            - none
 *
 * @Note              -
 *                      1. STM32 peripherals require a clock source
 *                         before their registers can be accessed or
 *                         configured correctly.
 *
 *                      2. After device reset, peripheral clocks are
 *                         disabled by default to reduce power
 *                         consumption.
 *
 *                      3. This function enables or disables the clock
 *                         supplied to the selected USART peripheral
 *                         through the RCC module.
 *
 *                      4. The peripheral clock must be enabled before:
 *
 *                           - USART_Init()
 *                           - Register configuration
 *                           - Data transmission
 *                           - Data reception
 *                           - Interrupt configuration
 *
 *                      5. Accessing USART registers while the clock
 *                         is disabled may lead to:
 *
 *                           - Invalid register accesses
 *                           - Failed configuration attempts
 *                           - Non-functional communication
 *                           - Undefined peripheral behavior
 *
 *                      6. Peripheral bus mapping on STM32F446xx:
 *
 *                           APB2 Bus:
 *                           ----------
 *                           - USART1
 *                           - USART6
 *
 *                           APB1 Bus:
 *                           ----------
 *                           - USART2
 *                           - USART3
 *                           - UART4
 *                           - UART5
 *
 *                      7. Enabling the peripheral clock does not
 *                         automatically enable the USART peripheral
 *                         itself.
 *
 *                      8. After enabling the clock, the application
 *                         must still configure the peripheral and
 *                         enable USART operation using:
 *
 *                              USART_Init()
 *                              USART_PeripheralControl()
 *
 *                      9. Disabling the peripheral clock stops clock
 *                         propagation to the USART hardware and
 *                         reduces dynamic power consumption.
 *
 *                     10. Any ongoing communication is disrupted if
 *                         the clock is disabled while transmission
 *                         or reception is in progress.
 *
 *                     11. Before disabling the clock, it is
 *                         recommended to ensure that:
 *
 *                           - Transmission is complete
 *                           - No reception is ongoing
 *                           - USART is disabled
 *
 *                     12. Disabling the peripheral clock does not
 *                         necessarily clear USART register contents.
 *                         Register values may remain unchanged until
 *                         a peripheral reset occurs.
 *
 *                     13. This function only controls RCC clock
 *                         gating and does not modify any USART
 *                         configuration register.
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
 * @fn                - USART_Init
 *
 * @brief             - Initialize the USART peripheral according to
 *                      the user-supplied configuration parameters.
 *
 * @param[in]         - pUSARTHandle : Pointer to USART handle
 *                                     structure containing USART
 *                                     peripheral base address and
 *                                     configuration information.
 *
 * @return            - none
 *
 * @Note              -
 *                      1. This function configures all major USART
 *                         operating parameters required before the
 *                         peripheral can be enabled for communication.
 *
 *                      2. The peripheral clock for the selected USART
 *                         instance is enabled automatically before
 *                         configuration begins.
 *
 *                      3. Configuration is performed by programming:
 *
 *                           - CR1 (Control Register 1)
 *                           - CR2 (Control Register 2)
 *                           - CR3 (Control Register 3)
 *                           - BRR (Baud Rate Register)
 *
 *                      4. USART operating mode is configured using:
 *
 *                           USART_MODE_ONLY_TX
 *                           USART_MODE_ONLY_RX
 *                           USART_MODE_TXRX
 *
 *                      5. Depending on the selected mode:
 *
 *                           - TE bit enables transmitter
 *                           - RE bit enables receiver
 *
 *                      6. Word length configuration determines the
 *                         number of data bits present in each frame.
 *
 *                      7. Supported word lengths:
 *
 *                           USART_WORDLEN_8BITS
 *                           USART_WORDLEN_9BITS
 *
 *                      8. Parity configuration is programmed through
 *                         the PCE and PS bits in CR1.
 *
 *                      9. Supported parity modes:
 *
 *                           USART_PARITY_DISABLE
 *                           USART_PARITY_EN_EVEN
 *                           USART_PARITY_EN_ODD
 *
 *                     10. When parity is enabled:
 *
 *                           - Hardware automatically generates
 *                             parity during transmission
 *
 *                           - Hardware automatically checks parity
 *                             during reception
 *
 *                           - User data width is effectively reduced
 *                             by one bit
 *
 *                     11. Stop bit configuration is programmed using
 *                         the STOP field of CR2.
 *
 *                     12. Supported stop bit configurations depend on
 *                         the target USART peripheral and device.
 *
 *                     13. Hardware flow control configuration is
 *                         programmed through CR3.
 *
 *                     14. Supported flow control modes:
 *
 *                           USART_HW_FLOW_CTRL_NONE
 *                           USART_HW_FLOW_CTRL_CTS
 *                           USART_HW_FLOW_CTRL_RTS
 *                           USART_HW_FLOW_CTRL_CTS_RTS
 *
 *                     15. CTS flow control allows the receiver to
 *                         control transmission permission.
 *
 *                     16. RTS flow control allows the transmitter to
 *                         indicate receive buffer availability.
 *
 *                     17. Baud rate configuration is delegated to:
 *
 *                              USART_SetBaudRate()
 *
 *                         which calculates and programs the BRR
 *                         register according to APB clock frequency.
 *
 *                     18. This function only configures the USART
 *                         registers and does NOT enable the USART
 *                         peripheral.
 *
 *                     19. After initialization, the application must
 *                         explicitly enable the peripheral using:
 *
 *                              USART_PeripheralControl()
 *
 *                     20. GPIO pins associated with TX, RX, CTS or
 *                         RTS must be configured separately before
 *                         communication begins.
 *
 *                     21. Both communicating devices must use
 *                         compatible communication parameters:
 *
 *                           - Baud rate
 *                           - Word length
 *                           - Stop bits
 *                           - Parity configuration
 *
 *                     22. Incorrect configuration may result in:
 *
 *                           - Framing errors
 *                           - Parity errors
 *                           - Corrupted data
 *                           - Communication failure
 *
 *                     23. Re-initializing an active USART may disrupt
 *                         ongoing communication and should generally
 *                         be performed only when the peripheral is
 *                         idle or disabled.
 *
 */
void USART_Init(USART_Handle_t *pUSARTHandle)
{

	//temporary variable
	uint32_t tempreg = 0U;

/******************************** Configuration of CR1******************************************/

	//Enable the Clock for given USARTx peripheral
	USART_PeriClockControl(pUSARTHandle->pUSARTx,
						   ENABLE);

	//Enable USART Tx and Rx engines according to the USART_Mode configuration item
	if (pUSARTHandle->USART_Config.USART_Mode == USART_MODE_ONLY_RX)
	{
		//Enable the Receiver bit field
		tempreg|= (1U << USART_CR1_RE);
	}
	else if (pUSARTHandle->USART_Config.USART_Mode == USART_MODE_ONLY_TX)
	{
		//Enable the Transmitter bit field
		tempreg |= (1U << USART_CR1_TE);

	}
	else if (pUSARTHandle->USART_Config.USART_Mode == USART_MODE_TXRX)
	{
		//Enable the both Transmitter and Receiver bit fields
		tempreg |= ((1U << USART_CR1_TE) | (1U << USART_CR1_RE));
	}

    //Configure the Word length configuration item
	tempreg |= pUSARTHandle->USART_Config.USART_WordLength <<  USART_CR1_M;


    //Configuration of parity control bit fields
	if (pUSARTHandle->USART_Config.USART_ParityControl == USART_PARITY_EN_EVEN)
	{
		//Enable the parity control
		tempreg |= ( 1U << USART_CR1_PCE);

		/* Implement the code to Enable EVEN parity
		 * Not required because by default EVEN parity will be selected
		 * once you enable the parity control */

	}
	else if (pUSARTHandle->USART_Config.USART_ParityControl == USART_PARITY_EN_ODD)
	{
		//Enable the parity control
	    tempreg |= (1U << USART_CR1_PCE);

	    //Enable ODD parity
	    tempreg |= (1U << USART_CR1_PS);
	}

   //Program the CR1 register
	pUSARTHandle->pUSARTx->CR1 = tempreg;

/******************************** Configuration of CR2******************************************/

	tempreg = 0U;

	//Configure the number of stop bits inserted during USART frame transmission
	tempreg |= pUSARTHandle->USART_Config.USART_NoOfStopBits << USART_CR2_STOP;

	//Program the CR2 register
	pUSARTHandle->pUSARTx->CR2 = tempreg;

/******************************** Configuration of CR3******************************************/

	tempreg = 0U;

	//Configuration of USART hardware flow control
	if ( pUSARTHandle->USART_Config.USART_HWFlowControl == USART_HW_FLOW_CTRL_CTS)
	{
		//Enable CTS flow control
		tempreg |= (1U << USART_CR3_CTSE);


	}
	else if (pUSARTHandle->USART_Config.USART_HWFlowControl == USART_HW_FLOW_CTRL_RTS)
	{
		//Enable RTS flow control
		tempreg |= (1U << USART_CR3_RTSE);

	}
	else if (pUSARTHandle->USART_Config.USART_HWFlowControl == USART_HW_FLOW_CTRL_CTS_RTS)
	{
		//Enable both CTS and RTS Flow control
		tempreg |= ((1U << USART_CR3_CTSE) | (1U << USART_CR3_RTSE));
	}

	//Program the CR3 register
	pUSARTHandle->pUSARTx->CR3 = tempreg;

/******************************** Configuration of BRR(Baudrate register)******************************************/

	//configure the baud rate
	USART_SetBaudRate(pUSARTHandle->pUSARTx,
					  pUSARTHandle->USART_Config.USART_BaudRate);

}



/*********************************************************************
 * @fn                - USART_DeInit
 *
 * @brief             - Reset the selected USART peripheral to its
 *                      hardware default reset state.
 *
 * @param[in]         - pUSARTx : Pointer to the target USART/UART
 *                                peripheral instance.
 *
 * @return            - none
 *
 * @Note              -
 *                      1. This function forces the selected USART
 *                         peripheral into reset using the RCC
 *                         peripheral reset mechanism.
 *
 *                      2. All USART registers are restored to the
 *                         values defined by hardware after a system
 *                         reset.
 *
 *                      3. Peripheral configuration is completely
 *                         lost after de-initialization.
 *
 *                      4. Configuration items returned to their
 *                         reset values include:
 *                           - Baud rate
 *                           - Word length
 *                           - Stop bits
 *                           - Parity configuration
 *                           - Hardware flow control
 *                           - Interrupt configuration
 *                           - Peripheral enable state
 *
 *                      5. Any ongoing transmission or reception is
 *                         immediately aborted when the peripheral
 *                         reset is asserted.
 *
 *                      6. Pending transmit data may be lost if the
 *                         peripheral is reset before:
 *
 *                              TC = 1
 *
 *                         indicating transmission completion.
 *
 *                      7. Any unread received data present in the
 *                         Data Register will be discarded.
 *
 *                      8. Interrupt enable bits and communication
 *                         states are returned to their reset values.
 *
 *                      9. After de-initialization, the peripheral
 *                         must be configured again using:
 *
 *                              USART_Init()
 *
 *                         before it can be used.
 *
 *                     10. Typical use cases include:
 *                           - Peripheral reconfiguration
 *                           - Recovery from communication faults
 *                           - Recovery from software errors
 *                           - Returning to a known reset state
 *                           - Driver reinitialization
 *
 *                     11. This function should not be called during
 *                         an active communication session unless
 *                         termination of the current transfer is
 *                         intended.
 *
 *                     12. De-initialization does not disable the
 *                         peripheral clock. It only resets the
 *                         peripheral registers through RCC reset
 *                         control logic.
 *
 *                     13. Supported peripherals:
 *                           - USART1
 *                           - USART2
 *                           - USART3
 *                           - UART4
 *                           - UART5
 *                           - USART6
 *
 *                     14. The exact reset operation depends on the
 *                         RCC reset register associated with the
 *                         selected peripheral.
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
 * @brief             - Get the current status of a specified USART
 *                      status flag.
 *
 * @param[in]         - pUSARTx  : Pointer to the target USART
 *                                peripheral instance.
 * @param[in]         - FlagName : USART status flag to check.
 *
 * @return            - FLAG_SET   : Specified flag is set.
 *                      FLAG_RESET : Specified flag is cleared.
 *
 * @Note              -
 *                      1. This function checks the USART Status
 *                         Register (SR) for the specified flag.
 *
 *                      2. USART status flags provide information
 *                         about the current operating state of the
 *                         peripheral and communication progress.
 *
 *                      3. This API is commonly used in polling-based
 *                         communication where software waits for
 *                         specific hardware events.
 *
 *                      4. Typical polling usage:
 *
 *                              while(USART_GetFlagStatus(
 *                                        USART2,
 *                                        USART_FLAG_TXE)
 *                                        == FLAG_RESET);
 *
 *                      5. Common USART status flags include:
 *
 *                           USART_FLAG_PE
 *                               Parity Error
 *
 *                           USART_FLAG_FE
 *                               Framing Error
 *
 *                           USART_FLAG_NF
 *                               Noise Detected
 *
 *                           USART_FLAG_ORE
 *                               Overrun Error
 *
 *                           USART_FLAG_IDLE
 *                               IDLE Line Detected
 *
 *                           USART_FLAG_RXNE
 *                               Receive Data Register
 *                               Not Empty
 *
 *                           USART_FLAG_TC
 *                               Transmission Complete
 *
 *                           USART_FLAG_TXE
 *                               Transmit Data Register
 *                               Empty
 *
 *                           USART_FLAG_LBD
 *                               LIN Break Detection
 *
 *                           USART_FLAG_CTS
 *                               Clear To Send Event
 *
 *                      6. TXE flag indicates that the Data Register
 *                         is empty and ready to accept new transmit
 *                         data.
 *
 *                      7. TC flag indicates that both the Data
 *                         Register and Shift Register are empty and
 *                         the entire frame has been transmitted.
 *
 *                      8. RXNE flag indicates that unread received
 *                         data is available in the Data Register.
 *
 *                      9. Error flags such as PE, FE, NF and ORE
 *                         indicate communication problems that may
 *                         require application intervention.
 *
 *                     10. Status flags are updated automatically by
 *                         USART hardware as communication progresses.
 *
 *                     11. This function only reads the flag state
 *                         and does not modify any peripheral
 *                         register.
 *
 *                     12. The returned flag status represents the
 *                         instantaneous hardware state at the time
 *                         of the function call.
 *
 *                     13. Some flags may change immediately after
 *                         this function returns because USART
 *                         hardware continues operating
 *                         asynchronously.
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
 * @fn                - USART_ClearFlag
 *
 * @brief             - Clear a specified USART status flag.
 *
 * @param[in]         - pUSARTx   : Pointer to the target USART
 *                                peripheral instance.
 * @param[in]         - FlagName : USART status flag to be cleared.
 *
 * @return            - none
 *
 * @Note              -
 *                      1. This function clears selected USART status
 *                         flags by writing to the Status Register
 *                         (SR).
 *
 *                      2. Not all USART status flags can be cleared
 *                         using this method.
 *
 *                      3. This API is intended for flags that are
 *                         cleared through direct software action,
 *                         such as:
 *                           - USART_FLAG_TC
 *                           - USART_FLAG_CTS
 *                           - USART_FLAG_LBD
 *
 *                      4. Some USART flags require special clearing
 *                         sequences defined by the reference manual.
 *
 *                      5. Examples of flags requiring dedicated
 *                         clear sequences include:
 *                           - USART_FLAG_RXNE
 *                           - USART_FLAG_ORE
 *                           - USART_FLAG_FE
 *                           - USART_FLAG_NF
 *                           - USART_FLAG_IDLE
 *
 *                      6. RXNE is typically cleared automatically
 *                         when the application reads the DR register.
 *
 *                      7. ORE, FE, NF and IDLE flags are generally
 *                         cleared by a software sequence involving:
 *
 *                              Read SR
 *                                   followed by
 *                              Read DR
 *
 *                         as specified in the reference manual.
 *
 *                      8. Attempting to clear unsupported flags
 *                         through direct SR modification may have
 *                         no effect or produce unexpected behavior.
 *
 *                      9. This API is commonly used by interrupt
 *                         handlers after processing a specific
 *                         USART event.
 *
 *                     10. For transmission completion handling,
 *                         the TC flag is typically cleared after
 *                         confirming that transmission has fully
 *                         completed.
 *
 *                     11. Applications should consult the device
 *                         reference manual before clearing any
 *                         USART status flag.
 *
 */

void USART_ClearFlag(USART_RegDef_t *pUSARTx,
					 uint32_t FlagName)
{
	pUSARTx->SR &= ~(FlagName);

}



/*********************************************************************
 * @fn                - USART_PeripheralControl
 *
 * @brief             - Enable or disable the USART peripheral by
 *                      controlling the UE (USART Enable) bit.
 *
 * @param[in]         - pUSARTx : Pointer to the target USART
 *                                peripheral instance.
 * @param[in]         - EnOrDi  : ENABLE or DISABLE macro.
 *
 * @return            - none
 *
 * @Note              -
 *                      1. This function controls the USART Enable
 *                         (UE) bit located in the CR1 register.
 *
 *                      2. Setting UE = 1 enables the USART
 *                         peripheral and allows it to perform
 *                         transmission and reception operations.
 *
 *                      3. Clearing UE = 0 disables the USART
 *                         peripheral and stops normal USART
 *                         communication activity.
 *
 *                      4. The USART peripheral must be enabled
 *                         before using:
 *                           - USART_SendData()
 *                           - USART_ReceiveData()
 *                           - USART_SendDataIT()
 *                           - USART_ReceiveDataIT()
 *
 *                      5. Configuration parameters such as:
 *                           - Baud rate
 *                           - Word length
 *                           - Stop bits
 *                           - Parity control
 *                           - Hardware flow control
 *
 *                         should normally be configured before
 *                         enabling the peripheral.
 *
 *                      6. Disabling the USART does not erase or
 *                         reset its configuration registers.
 *
 *                      7. After re-enabling the USART, previously
 *                         configured settings remain intact unless
 *                         modified by software.
 *
 *                      8. This function only controls peripheral
 *                         enable state and does not modify any
 *                         communication parameters.
 *
 *                      9. Disabling the USART during an active
 *                         transmission may result in incomplete
 *                         frame transmission.
 *
 *                     10. It is recommended to wait until:
 *
 *                              TC = 1
 *
 *                         before disabling the peripheral to
 *                         ensure the last frame has completely
 *                         left the shift register.
 *
 *                     11. Any pending receive data may be lost if
 *                         the peripheral is disabled before the
 *                         application reads the Data Register.
 *
 *                     12. This API is commonly used during:
 *                           - Peripheral initialization
 *                           - Peripheral shutdown
 *                           - Power management operations
 *                           - Communication reconfiguration
 *
 *                     13. To restore the USART peripheral to its
 *                         hardware reset state, use:
 *
 *                              USART_DeInit()
 *
 *                         instead of simply disabling it.
 *
 */
void USART_PeripheralControl(USART_RegDef_t *pUSARTx,
	    					 uint8_t EnOrDi)
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
 * @fn                - USART_FlushReceiveData
 *
 * @brief             - Flush any unread data present in the USART
 *                      receive data register before starting a new
 *                      reception.
 *
 * @param[in]         - pUSARTx : Pointer to the target USART
 *                                peripheral instance.
 *
 * @return            - none
 *
 * @Note              -
 *                      1. This helper function removes any pending
 *                         unread data currently present in the
 *                         USART receive path.
 *
 *                      2. The function repeatedly checks:
 *
 *                              RXNE = 1
 *
 *                         which indicates that unread data exists
 *                         in the Data Register (DR).
 *
 *                      3. While RXNE remains set, the function reads
 *                         the DR register to consume the pending
 *                         received data.
 *
 *                      4. Reading DR automatically clears RXNE
 *                         according to USART hardware behavior.
 *
 *                      5. Multiple reads may be required if more
 *                         than one pending receive event exists.
 *
 *                      6. This function is useful before starting a
 *                         fresh receive operation to ensure old data
 *                         does not get interpreted as newly received
 *                         application data.
 *
 *                      7. Without flushing, a receive API may
 *                         immediately consume previously pending
 *                         data and return unexpected results.
 *
 *                      8. Typical sources of stale data include:
 *                           - Previous incomplete receptions
 *                           - Data received before Rx start
 *                           - Terminal characters left in buffer
 *                           - Application timing mismatches
 *
 *                      9. This function only clears unread data
 *                         already present in DR. It does not stop
 *                         future incoming USART frames.
 *
 *                     10. The flush operation should normally be
 *                         performed before enabling a new receive
 *                         transaction when stale RXNE data is not
 *                         desired.
 *
 *                     11. This helper is intended for internal
 *                         driver usage and is typically called by
 *                         interrupt-driven receive APIs.
 *
 *                     12. Reading DR as part of the flush sequence
 *                         may also contribute to clearing certain
 *                         receive-related status conditions as
 *                         defined by the reference manual.
 *
 */
static void USART_FlushReceiveData(USART_RegDef_t *pUSARTx)
{
    volatile uint32_t dummy;

    while(pUSARTx->SR & (1U << USART_SR_RXNE))
    {
        dummy = pUSARTx->DR;
        (void)dummy;
    }
}



/*********************************************************************
 * @fn                - USART_SendData
 *
 * @brief             - Transmit data over USART peripheral using
 *                      polling/blocking method.
 *
 * @param[in]         - pUSARTHandle : Pointer to USART handle
 *                                     structure containing USART
 *                                     peripheral base address and
 *                                     configuration information.
 * @param[in]         - pTxBuffer : Pointer to transmit data buffer.
 * @param[in]         - Len : Number of bytes to transmit.
 *
 * @return            - none
 *
 * @Note              -
 *                      1. This is a Blocking/Polling based API.
 *
 *                      2. CPU continuously waits until all data
 *                         bytes are transferred completely.
 *
 *                      3. Function execution does not return until
 *                         the entire transmit operation finishes.
 *
 *                      4. Before loading new data into the Data
 *                         Register (DR), the API waits for:
 *
 *                              TXE = 1
 *
 *                         indicating the transmit data register is
 *                         empty and ready to accept new data.
 *
 *                      5. TXE (Transmit Data Register Empty) does
 *                         NOT indicate transmission completion.
 *
 *                      6. TXE only indicates that the current data
 *                         has moved from DR into the internal shift
 *                         register.
 *
 *                      7. Actual serial transmission occurs through
 *                         the USART shift register.
 *
 *                      8. After all bytes are loaded into DR, the
 *                         API waits for:
 *
 *                              TC = 1
 *
 *                         before returning.
 *
 *                      9. TC (Transmission Complete) indicates:
 *                           - DR is empty
 *                           - Shift register is empty
 *                           - Last stop bit has been transmitted
 *
 *                     10. Waiting for TC ensures that the complete
 *                         USART frame has physically left the TX pin.
 *
 *                     11. This is especially important before:
 *                           - Disabling USART
 *                           - Entering low-power modes
 *                           - Switching communication direction
 *                           - Reconfiguring USART settings
 *
 *                     12. The API supports both:
 *                           - 8-bit word length
 *                           - 9-bit word length
 *
 *                     13. In 8-bit word length mode:
 *                           - One byte is loaded into DR
 *                           - Buffer pointer increments by 1
 *
 *                     14. In 9-bit word length mode:
 *                           - 9 bits are loaded into DR
 *                           - Data is masked using 0x01FF
 *                           - DR receives only valid data bits
 *
 *                     15. When 9-bit word length is selected and
 *                         parity control is disabled:
 *                           - All 9 bits belong to user data
 *                           - Buffer pointer advances by 2 bytes
 *
 *                     16. When 9-bit word length is selected and
 *                         parity control is enabled:
 *                           - Only 8 bits belong to user data
 *                           - Hardware automatically generates the
 *                             parity bit
 *                           - Buffer pointer advances by 1 byte
 *
 *                     17. Parity generation and insertion are fully
 *                         handled by USART hardware.
 *
 *                     18. This API only performs transmission and
 *                         does not receive any incoming data.
 *
 *                     19. Polling-based communication is suitable
 *                         for:
 *                           - Small data transfers
 *                           - Debug messages
 *                           - Boot-time communication
 *                           - Learning and testing purposes
 *
 *                     20. Limitations of polling method:
 *                           - CPU remains busy waiting
 *                           - Other tasks are blocked
 *                           - Reduced system responsiveness
 *                           - Inefficient for large transfers
 *
 *                     21. For efficient communication in multitasking
 *                         applications, consider:
 *                           - USART_SendDataIT()
 *                           - DMA-based transmission
 *
 *                     22. The USART peripheral must be properly
 *                         initialized and enabled before calling
 *                         this API.
 *
 *                     23. The TX pin must be configured in Alternate
 *                         Function mode with the correct USART AF
 *                         mapping.
 *
 *                     24. Both communicating devices must use the
 *                         same:
 *                           - Baud rate
 *                           - Word length
 *                           - Stop bit configuration
 *                           - Parity settings
 *
 *                     25. Mismatched frame configurations may lead
 *                         to corrupted or invalid data reception.
 *
 */
void USART_SendData(USART_Handle_t *pUSARTHandle,
					uint8_t *pTxBuffer,
					uint32_t Len)
{

	uint16_t *pdata;

   //Loop over until "Len" number of bytes are transferred
	for(uint32_t i = 0U ; i < Len; i++)
	{
		//wait until TXE flag is set in the SR
		while(USART_GetFlagStatus(pUSARTHandle->pUSARTx,
								  USART_FLAG_TXE) == RESET);

         //Check the USART_WordLength item for 9-bit or 8-bit in a frame
		if(pUSARTHandle->USART_Config.USART_WordLength == USART_WORDLEN_9BITS)
		{
			//if 9-bit user data, load the DR with 2-bytes masking the bits other than first 9-bits
			pdata = (uint16_t*) pTxBuffer;
			pUSARTHandle->pUSARTx->DR = (*pdata & (uint16_t)0x01FFU);

			//check parity control
			if(pUSARTHandle->USART_Config.USART_ParityControl == USART_PARITY_DISABLE)
			{
				//No parity, 9-bits of user data will be sent
				//Increment pTxBuffer twice
				pTxBuffer += 2U;
			}
			else
			{
				//Parity bit is used, 8bits of user data will be sent
				//The 9th bit will be replaced by parity bit by the hardware
				pTxBuffer++;
			}
		}
		else
		{
			//8-bit data transfer
			pUSARTHandle->pUSARTx->DR = (*pTxBuffer & (uint8_t)0xFFU);

			//Increment the buffer address
			pTxBuffer++;
		}
	}

	//wait until TC flag is set in the SR
	while(USART_GetFlagStatus(pUSARTHandle->pUSARTx,
							  USART_FLAG_TC) == RESET);
}



/*********************************************************************
 * @fn                - USART_SendDataIT
 *
 * @brief             - Transmit data over USART peripheral using
 *                      interrupt/non-blocking method.
 *
 * @param[in]         - pUSARTHandle : Pointer to USART handle
 *                                     structure containing USART
 *                                     peripheral base address and
 *                                     configuration information.
 * @param[in]         - pTxBuffer : Pointer to transmit data buffer.
 * @param[in]         - Len : Number of bytes to transmit.
 *
 * @return            - Current USART communication state:
 *                        USART_READY
 *                        USART_BUSY_IN_TX
 *                        USART_BUSY_IN_RX
 *
 * @Note              -
 *                      1. This is a Non-Blocking/Interrupt based API.
 *
 *                      2. Unlike USART_SendData(), this function does
 *                         not wait for the entire transmission to
 *                         complete.
 *
 *                      3. Function returns immediately after enabling
 *                         the required USART transmit interrupts.
 *
 *                      4. Actual data transmission is performed by
 *                         the USART Interrupt Service Routine (ISR).
 *
 *                      5. This API only:
 *                           - Stores transmit buffer information
 *                           - Stores transmit length
 *                           - Updates USART state
 *                           - Enables transmit interrupts
 *
 *                      6. The application can continue executing
 *                         other tasks while transmission occurs in
 *                         the background.
 *
 *                      7. Transmission flow:
 *
 *                              Application
 *                                   ↓
 *                           USART_SendDataIT()
 *                                   ↓
 *                           TXEIE Enabled
 *                                   ↓
 *                           TXE Interrupt Occurs
 *                                   ↓
 *                              ISR Executes
 *                                   ↓
 *                           Data Loaded Into DR
 *                                   ↓
 *                           Hardware Shifts Data
 *                                   ↓
 *                           TC Interrupt Occurs
 *                                   ↓
 *                           Transmission Closed
 *                                   ↓
 *                           Application Callback
 *
 *                      8. The API stores transmission information in
 *                         the USART handle because the ISR requires
 *                         persistent access after the function
 *                         returns.
 *
 *                      9. The transmit buffer pointer and remaining
 *                         data length are maintained by the ISR until
 *                         transmission completes.
 *
 *                     10. TXE interrupt is enabled to indicate:
 *
 *                              TXE = 1
 *
 *                         meaning the Data Register is empty and
 *                         ready to accept new transmit data.
 *
 *                     11. Each TXE interrupt allows the ISR to load
 *                         the next data frame into DR.
 *
 *                     12. TC interrupt is enabled to indicate:
 *
 *                              TC = 1
 *
 *                         meaning the final frame has completely
 *                         left the USART shift register.
 *
 *                     13. TC interrupt is used to safely terminate
 *                         the transmission process.
 *
 *                     14. The USART state machine prevents multiple
 *                         simultaneous transmit operations on the
 *                         same peripheral.
 *
 *                     15. If a transmission is already in progress:
 *
 *                              USART_BUSY_IN_TX
 *
 *                         is returned and the new request is ignored.
 *
 *                     16. This prevents:
 *                           - Buffer corruption
 *                           - Data interleaving
 *                           - State corruption
 *                           - Unpredictable communication behavior
 *
 *                     17. Interrupt-driven communication provides:
 *
 *                           Advantages:
 *                           -----------
 *                           - CPU remains free
 *                           - Better responsiveness
 *                           - Improved multitasking
 *                           - Suitable for RTOS systems
 *                           - Efficient for large transfers
 *
 *                           Disadvantages:
 *                           --------------
 *                           - More complex implementation
 *                           - Requires ISR management
 *                           - Requires state tracking
 *                           - Debugging is more difficult
 *
 *                     18. Compared to polling-based transmission:
 *
 *                           Polling:
 *                               CPU continuously waits
 *
 *                           Interrupt:
 *                               CPU performs other work
 *                               until interrupt occurs
 *
 *                     19. USART peripheral and NVIC interrupt must
 *                         be configured properly before using this
 *                         API.
 *
 *                     20. The ISR is responsible for:
 *                           - Loading data into DR
 *                           - Updating buffer pointer
 *                           - Updating remaining length
 *                           - Disabling TX interrupts
 *                           - Restoring USART_READY state
 *                           - Invoking application callback
 *
 *                     21. After transmission completion, the
 *                         application is notified through
 *                         USART_ApplicationEventCallback().
 *
 *                     22. This API only initiates transmission.
 *                         Reception, error handling and interrupt
 *                         processing are performed separately.
 *
 */
uint8_t USART_SendDataIT(USART_Handle_t *pUSARTHandle,
						 uint8_t *pTxBuffer,
						 uint32_t Len)
{
	uint8_t txstate = pUSARTHandle->TxBusyState;

	if(txstate != USART_BUSY_IN_TX)
	{
		pUSARTHandle->TxLen = Len;
		pUSARTHandle->pTxBuffer = pTxBuffer;
		pUSARTHandle->TxBusyState = USART_BUSY_IN_TX;

		//Enable interrupt for TXE
		pUSARTHandle->pUSARTx->CR1 |= (1U << USART_CR1_TXEIE);

		//Enable interrupt for TC
		pUSARTHandle->pUSARTx->CR1 |= (1U << USART_CR1_TCIE);
	}
	return txstate;
}



/*********************************************************************
 * @fn                - USART_ReceiveData
 *
 * @brief             - Receive data from USART peripheral using
 *                      polling/blocking method.
 *
 * @param[in]         - pUSARTHandle : Pointer to USART handle
 *                                     structure containing USART
 *                                     peripheral base address and
 *                                     configuration information.
 * @param[in]         - pRxBuffer : Pointer to receive data buffer.
 * @param[in]         - Len : Number of bytes to receive.
 *
 * @return            - none
 *
 * @Note              -
 *                      1. This is a Blocking/Polling based API.
 *
 *                      2. CPU continuously waits until the requested
 *                         amount of data is received.
 *
 *                      3. Function execution does not return until
 *                         all requested data bytes are copied into
 *                         the receive buffer.
 *
 *                      4. Before reading data from the Data Register
 *                         (DR), the API waits for:
 *
 *                              RXNE = 1
 *
 *                         indicating that received data is available.
 *
 *                      5. RXNE (Receive Data Register Not Empty)
 *                         indicates that unread data is present in
 *                         the USART Data Register.
 *
 *                      6. Once RXNE becomes set, software must read
 *                         the DR register to retrieve the received
 *                         data.
 *
 *                      7. Reading DR automatically clears RXNE in
 *                         hardware, allowing reception of the next
 *                         data frame.
 *
 *                      8. Failure to read received data in time may
 *                         result in:
 *                           - Overrun Error (ORE)
 *                           - Data loss
 *                           - Corrupted communication
 *
 *                      9. The API supports both:
 *                           - 8-bit word length
 *                           - 9-bit word length
 *
 *                     10. In 9-bit word length mode with parity
 *                         disabled:
 *                           - All 9 bits belong to user data
 *                           - DR is masked with 0x01FF
 *                           - Buffer pointer advances by 2 bytes
 *
 *                     11. In 9-bit word length mode with parity
 *                         enabled:
 *                           - 8 bits belong to user data
 *                           - 1 bit is parity information
 *                           - Hardware checks parity automatically
 *                           - Buffer pointer advances by 1 byte
 *
 *                     12. In 8-bit word length mode with parity
 *                         disabled:
 *                           - All 8 bits belong to user data
 *                           - DR is masked with 0x00FF
 *
 *                     13. In 8-bit word length mode with parity
 *                         enabled:
 *                           - 7 bits belong to user data
 *                           - 1 bit is parity information
 *                           - DR is masked with 0x007F
 *
 *                     14. Parity bits are generated and checked
 *                         automatically by USART hardware.
 *
 *                     15. This API only retrieves user data and does
 *                         not expose the parity bit to the application.
 *
 *                     16. The application should ensure that the
 *                         receive buffer is large enough to store
 *                         all incoming data.
 *
 *                     17. Polling-based reception is suitable for:
 *                           - Small data transfers
 *                           - Command reception
 *                           - Debug interfaces
 *                           - Learning and testing purposes
 *
 *                     18. Limitations of polling method:
 *                           - CPU remains busy waiting
 *                           - Other tasks are blocked
 *                           - Reduced system responsiveness
 *                           - Inefficient for large data transfers
 *
 *                     19. For efficient communication in multitasking
 *                         applications, consider:
 *                           - USART_ReceiveDataIT()
 *                           - DMA-based reception
 *
 *                     20. The USART peripheral must be properly
 *                         initialized and enabled before calling
 *                         this API.
 *
 *                     21. The RX pin must be configured in Alternate
 *                         Function mode with the correct USART AF
 *                         mapping.
 *
 *                     22. Both communicating devices must use the
 *                         same:
 *                           - Baud rate
 *                           - Word length
 *                           - Stop bit configuration
 *                           - Parity settings
 *
 *                     23. Mismatched communication settings may
 *                         result in:
 *                           - Framing errors
 *                           - Parity errors
 *                           - Invalid received data
 *
 *                     24. Any stale data already present in DR before
 *                         calling this API will be received first,
 *                         since RXNE indicates unread data currently
 *                         available in the receive register.
 *
 */
void USART_ReceiveData(USART_Handle_t *pUSARTHandle,
					   uint8_t *pRxBuffer,
					   uint32_t Len)
{
   //Loop over until "Len" number of bytes are transferred
	for(uint32_t i = 0U ; i < Len; i++)
	{
		//wait until RXNE flag is set in the SR
		while(USART_GetFlagStatus(pUSARTHandle->pUSARTx,
								  USART_FLAG_RXNE) == RESET);

		/* Check the USART_WordLength to decide whether we are going
		 * to receive 9-bit or 8-bit of data in a frame */
		if(pUSARTHandle->USART_Config.USART_WordLength == USART_WORDLEN_9BITS)
		{
			//9-bit frame
			//Check parity control
			if(pUSARTHandle->USART_Config.USART_ParityControl == USART_PARITY_DISABLE)
			{
				//No parity
				//read only first 9-bits. so, mask the DR with 0x01FF
				*((uint16_t*)pRxBuffer) = (pUSARTHandle->pUSARTx->DR  & (uint16_t)0x01FFU);

				//Increment pTxBuffer twice
				pRxBuffer += 2U;
			}
			else
			{
				//Parity is used, so, 8bits will be of user data and 1-bit is parity
				 *pRxBuffer =  (uint8_t)(pUSARTHandle->pUSARTx->DR  & (uint8_t)0xFFU);

				 //Increment the pRxBuffer
				 pRxBuffer++;
			}
		}
		else
		{
			//8-bit frame
			//check parity control
			if(pUSARTHandle->USART_Config.USART_ParityControl == USART_PARITY_DISABLE)
			{
				//No parity is used , so all 8-bits will be of user data
				//read 8-bits from DR
				*pRxBuffer = (uint8_t)(pUSARTHandle->pUSARTx->DR  & (uint8_t)0xFFU);
			}

			else
			{
				//Parity is used so, 7-bits will be of user data and 1-bit is parity
				//read only 7-bits , hence mask the DR with 0X7F
				 *pRxBuffer = (uint8_t)(pUSARTHandle->pUSARTx->DR  & (uint8_t)0X7FU);

			}

			//Increment the pRxBuffer
			pRxBuffer++;
		}
	}
}



/*********************************************************************
 * @fn                - USART_ReceiveDataIT
 *
 * @brief             - Receive data from USART peripheral using
 *                      interrupt/non-blocking method.
 *
 * @param[in]         - pUSARTHandle : Pointer to USART handle
 *                                     structure containing USART
 *                                     peripheral base address and
 *                                     configuration information.
 * @param[in]         - pRxBuffer : Pointer to receive data buffer.
 * @param[in]         - Len : Number of bytes to receive.
 *
 * @return            - Current USART communication state:
 *                        USART_READY
 *                        USART_BUSY_IN_TX
 *                        USART_BUSY_IN_RX
 *
 * @Note              -
 *                      1. This is a Non-Blocking/Interrupt based API.
 *
 *                      2. Unlike USART_ReceiveData(), this function
 *                         does not wait until all data is received.
 *
 *                      3. Function returns immediately after
 *                         configuring reception parameters and
 *                         enabling the RXNE interrupt.
 *
 *                      4. Actual reception is handled by the USART
 *                         Interrupt Service Routine (ISR).
 *
 *                      5. This API only:
 *                           - Stores receive buffer information
 *                           - Stores receive length
 *                           - Updates USART state
 *                           - Enables RXNE interrupt
 *
 *                      6. The application can continue executing
 *                         other tasks while reception occurs in the
 *                         background.
 *
 *                      7. Reception flow:
 *
 *                              Application
 *                                   ↓
 *                          USART_ReceiveDataIT()
 *                                   ↓
 *                           RXNEIE Enabled
 *                                   ↓
 *                           Data Arrives
 *                                   ↓
 *                           RXNE Interrupt
 *                                   ↓
 *                              ISR Executes
 *                                   ↓
 *                           DR Read By ISR
 *                                   ↓
 *                           Data Stored In Buffer
 *                                   ↓
 *                           Length Becomes Zero
 *                                   ↓
 *                           Reception Closed
 *                                   ↓
 *                           Application Callback
 *
 *                      8. Receive buffer information is stored inside
 *                         the USART handle because the ISR requires
 *                         access to it after this function returns.
 *
 *                      9. The receive buffer pointer and remaining
 *                         length are maintained by the ISR until the
 *                         requested reception completes.
 *
 *                     10. RXNE interrupt is enabled to indicate:
 *
 *                              RXNE = 1
 *
 *                         meaning unread received data is available
 *                         in the USART Data Register.
 *
 *                     11. Each RXNE interrupt allows the ISR to read
 *                         one received frame from DR and store it
 *                         into the application buffer.
 *
 *                     12. Before starting reception, this API calls
 *                         USART_FlushReceiveData().
 *
 *                     13. Flushing removes any previously unread
 *                         data from DR and clears a pending RXNE
 *                         condition before a new receive operation
 *                         begins.
 *
 *                     14. This prevents stale data from a previous
 *                         communication session from being consumed
 *                         as the first byte of the new reception.
 *
 *                     15. Without flushing, a previously pending
 *                         RXNE event could cause the ISR to process
 *                         old data before newly received data arrives.
 *
 *                     16. The USART state machine prevents multiple
 *                         simultaneous receive operations on the
 *                         same peripheral.
 *
 *                     17. If a reception is already in progress:
 *
 *                              USART_BUSY_IN_RX
 *
 *                         is returned and the new receive request
 *                         is ignored.
 *
 *                     18. This prevents:
 *                           - Buffer corruption
 *                           - Lost data
 *                           - State corruption
 *                           - Mixed receive transactions
 *
 *                     19. Interrupt-driven reception provides:
 *
 *                           Advantages:
 *                           -----------
 *                           - CPU remains free
 *                           - Better responsiveness
 *                           - Efficient multitasking
 *                           - Suitable for RTOS systems
 *                           - Efficient for long receptions
 *
 *                           Disadvantages:
 *                           --------------
 *                           - More complex implementation
 *                           - Requires ISR management
 *                           - Requires state tracking
 *                           - Harder debugging
 *
 *                     20. Compared to polling-based reception:
 *
 *                           Polling:
 *                               CPU continuously waits
 *
 *                           Interrupt:
 *                               CPU performs other work
 *                               until data arrives
 *
 *                     21. The USART peripheral and NVIC interrupt
 *                         must be configured properly before using
 *                         this API.
 *
 *                     22. The ISR is responsible for:
 *                           - Reading DR
 *                           - Storing received data
 *                           - Updating buffer pointer
 *                           - Updating remaining length
 *                           - Disabling RXNE interrupt
 *                           - Restoring USART_READY state
 *                           - Invoking application callback
 *
 *                     23. The ISR must correctly handle:
 *                           - 8-bit word length
 *                           - 9-bit word length
 *                           - Parity enabled mode
 *                           - Parity disabled mode
 *
 *                     24. After reception completes, the application
 *                         is notified through
 *                         USART_ApplicationEventCallback().
 *
 *                     25. This API initiates reception only.
 *                         Error handling such as:
 *                           - Overrun Error (ORE)
 *                           - Framing Error (FE)
 *                           - Noise Error (NE)
 *                           - Parity Error (PE)
 *
 *                         is handled separately by the interrupt
 *                         handler and associated callback events.
 *
 */
uint8_t USART_ReceiveDataIT(USART_Handle_t *pUSARTHandle,
							uint8_t *pRxBuffer,
							uint32_t Len)
{
	uint8_t rxstate = pUSARTHandle->RxBusyState;

	if(rxstate != USART_BUSY_IN_RX)
	{
		//this erase the first byte garbage data and clears RXNE
		USART_FlushReceiveData(pUSARTHandle->pUSARTx);

		pUSARTHandle->RxLen = Len;
		pUSARTHandle->pRxBuffer = pRxBuffer;
		pUSARTHandle->RxBusyState = USART_BUSY_IN_RX;

		//Enable interrupt for RXNE
		pUSARTHandle->pUSARTx->CR1 |= (1U << USART_CR1_RXNEIE);
	}
	return rxstate;
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



/*********************************************************************
 * @fn                - USART_IRQHandling
 *
 * @brief             - Central USART interrupt handler responsible
 *                      for processing USART event and error
 *                      interrupts.
 *
 * @param[in]         - pUSARTHandle : Pointer to USART handle
 *                                     structure containing USART
 *                                     peripheral information,
 *                                     configuration and runtime
 *                                     state data.
 *
 * @return            - none
 *
 * @Note              -
 *                      1. This function must be called from the
 *                         corresponding USART ISR.
 *
 *                      2. It serves as the central dispatcher for
 *                         all USART interrupt sources.
 *
 *                      3. The handler checks both:
 *                           - Interrupt Flag Status
 *                           - Interrupt Enable Status
 *
 *                         before servicing an interrupt source.
 *
 *                      4. This prevents processing interrupt events
 *                         that are not currently enabled by software.
 *
 *                      5. Supported interrupt events include:
 *                           - TXE  (Transmit Data Register Empty)
 *                           - TC   (Transmission Complete)
 *                           - RXNE (Receive Data Register Not Empty)
 *                           - CTS  (Clear To Send)
 *                           - IDLE (Idle Line Detection)
 *                           - ORE  (Overrun Error)
 *                           - FE   (Framing Error)
 *                           - NF   (Noise Error)
 *
 *                      6. The handler is responsible for maintaining
 *                         all USART runtime state information used by
 *                         interrupt-driven communication APIs.
 *
 *                      7. This function supports both:
 *                           - Interrupt-driven transmission
 *                           - Interrupt-driven reception
 *
 *                      8. Proper USART initialization and NVIC
 *                         configuration must be completed before
 *                         this handler can operate correctly.
 *
 *                      9. Interrupt processing order implemented in
 *                         this handler:
 *
 *                           1. TC
 *                           2. TXE
 *                           3. RXNE
 *                           4. CTS
 *                           5. IDLE
 *                           6. ORE
 *                           7. FE/NF/Other Errors
 *
 *                     10. The application is notified of completed
 *                         operations and error conditions through
 *                         USART_ApplicationEventCallback().
 *
 *
 *                     -------------------------------
 *                     TRANSMISSION COMPLETE (TC)
 *                     -------------------------------
 *
 *                     11. TC indicates:
 *
 *                              TC = 1
 *
 *                         meaning both:
 *                           - DR is empty
 *                           - Shift Register is empty
 *
 *                         and the final stop bit has completely
 *                         left the TX pin.
 *
 *                     12. TC is used to safely terminate interrupt
 *                         driven transmissions.
 *
 *                     13. When TxLen becomes zero and TC occurs:
 *                           - TC flag is cleared
 *                           - USART state becomes READY
 *                           - Buffer pointers are reset
 *                           - Completion callback is invoked
 *
 *                     14. TC provides the final confirmation that
 *                         transmission has physically completed.
 *
 *
 *                     -------------------------------
 *                     TRANSMIT DATA REGISTER EMPTY
 *                     (TXE)
 *                     -------------------------------
 *
 *                     15. TXE indicates:
 *
 *                              TXE = 1
 *
 *                         meaning DR is empty and ready to accept
 *                         another data frame.
 *
 *                     16. During interrupt-driven transmission,
 *                         TXE drives the actual movement of user
 *                         data from memory into DR.
 *
 *                     17. The handler supports:
 *                           - 8-bit word length
 *                           - 9-bit word length
 *                           - Parity enabled mode
 *                           - Parity disabled mode
 *
 *                     18. In 9-bit mode without parity:
 *                           - 9 bits of user data transmitted
 *                           - Buffer advances by 2 bytes
 *
 *                     19. In 9-bit mode with parity:
 *                           - 8 bits of user data transmitted
 *                           - 9th bit generated by hardware
 *                           - Buffer advances by 1 byte
 *
 *                     20. Once TxLen reaches zero:
 *                           - TXE interrupt is disabled
 *                           - TC interrupt remains active
 *
 *                         so transmission can be finalized using
 *                         the TC event.
 *
 *                     21. This TXE → TC sequence ensures that the
 *                         final frame completely leaves the USART
 *                         before the transmission state is closed.
 *
 *
 *                     -------------------------------
 *                     RECEIVE DATA REGISTER NOT EMPTY
 *                     (RXNE)
 *                     -------------------------------
 *
 *                     22. RXNE indicates:
 *
 *                              RXNE = 1
 *
 *                         meaning unread received data exists in
 *                         the Data Register.
 *
 *                     23. During interrupt-driven reception, RXNE
 *                         drives the transfer of received data from
 *                         DR into the user buffer.
 *
 *                     24. Reading DR automatically clears RXNE and
 *                         allows reception of the next frame.
 *
 *                     25. The handler supports:
 *                           - 8-bit word length
 *                           - 9-bit word length
 *                           - Parity enabled mode
 *                           - Parity disabled mode
 *
 *                     26. In 9-bit mode without parity:
 *                           - DR masked with 0x01FF
 *                           - Buffer advances by 2 bytes
 *
 *                     27. In 9-bit mode with parity:
 *                           - 8 bits of user data extracted
 *                           - Parity bit handled by hardware
 *
 *                     28. In 8-bit mode with parity:
 *                           - User data limited to 7 bits
 *                           - DR masked with 0x007F
 *
 *                     29. When RxLen becomes zero:
 *                           - RXNE interrupt is disabled
 *                           - USART state becomes READY
 *                           - Reception callback is invoked
 *
 *                     30. Any stale DR data should be handled before
 *                         starting a new reception transaction to
 *                         avoid consuming old data as valid input.
 *
 *
 *                     -------------------------------
 *                     CTS EVENT
 *                     -------------------------------
 *
 *                     31. CTS functionality is available only on
 *                         USART instances supporting hardware flow
 *                         control.
 *
 *                     32. CTS event indicates a change in the
 *                         Clear-To-Send control signal.
 *
 *                     33. Upon CTS detection:
 *                           - CTS flag is cleared
 *                           - Application callback is invoked
 *
 *                     34. CTS is commonly used for hardware flow
 *                         control in high-speed communication.
 *
 *
 *                     -------------------------------
 *                     IDLE LINE DETECTION
 *                     -------------------------------
 *
 *                     35. IDLE event occurs when the RX line remains
 *                         inactive for one complete frame duration.
 *
 *                     36. IDLE detection is useful for:
 *                           - Variable length packets
 *                           - Protocol framing
 *                           - DMA reception termination
 *                           - Communication timeout detection
 *
 *                     37. The IDLE flag requires the proper clear
 *                         sequence defined by the reference manual.
 *
 *                     38. Upon IDLE detection:
 *                           - IDLE flag is cleared
 *                           - Application callback is invoked
 *
 *
 *                     -------------------------------
 *                     OVERRUN ERROR (ORE)
 *                     -------------------------------
 *
 *                     39. ORE occurs when newly received data
 *                         arrives before previously received data
 *                         is read from DR.
 *
 *                     40. ORE indicates data loss because at least
 *                         one received frame has been overwritten.
 *
 *                     41. Common causes:
 *                           - Slow software response
 *                           - Interrupt latency
 *                           - Polling delays
 *                           - Long critical sections
 *
 *                     42. This handler reports ORE to the
 *                         application through the callback system.
 *
 *                     43. Error recovery strategy is application
 *                         dependent and therefore not performed
 *                         automatically here.
 *
 *
 *                     -------------------------------
 *                     FRAMING ERROR (FE)
 *                     -------------------------------
 *
 *                     44. FE occurs when the expected stop bit is
 *                         not detected correctly.
 *
 *                     45. Common causes:
 *                           - Baud rate mismatch
 *                           - Line disturbances
 *                           - Communication faults
 *
 *                     46. FE is reported to the application through
 *                         USART_ApplicationEventCallback().
 *
 *
 *                     -------------------------------
 *                     NOISE ERROR (NF)
 *                     -------------------------------
 *
 *                     47. NF indicates excessive noise detected on
 *                         the receive line during frame reception.
 *
 *                     48. Common causes:
 *                           - EMI interference
 *                           - Poor wiring
 *                           - Signal integrity problems
 *
 *                     49. NF is reported to the application through
 *                         USART_ApplicationEventCallback().
 *
 *
 *                     -------------------------------
 *                     APPLICATION CALLBACK MODEL
 *                     -------------------------------
 *
 *                     50. This handler follows an event-driven
 *                         architecture.
 *
 *                     51. Hardware events are translated into
 *                         application callbacks such as:
 *
 *                           USART_EV_TX_CMPLT
 *                           USART_EV_RX_CMPLT
 *                           USART_EV_CTS
 *                           USART_EV_IDLE
 *                           USART_ER_ORE
 *                           USART_ER_FE
 *                           USART_ER_NF
 *
 *                     52. This separation keeps the driver generic
 *                         while allowing application-specific event
 *                         processing outside the driver layer.
 *
 */
void USART_IRQHandling(USART_Handle_t *pUSARTHandle)
{

 	uint32_t temp1 = 0U , temp2 = 0U;
// 	uint32_t temp3;
 	uint16_t *pdata = 0U;

 /*************************Check for TC flag ********************************************/

     //Check the state of TC bit
 	temp1 = pUSARTHandle->pUSARTx->SR & (1U << USART_SR_TC);

 	 //Check the state of TCEIE bit
 	temp2 = pUSARTHandle->pUSARTx->CR1 & (1U << USART_CR1_TCIE);

 	if(temp1 && temp2 )
 	{
 		//This interrupt is because of TC
 		//Close transmission and call application callback if TxLen is zero
 		if ( pUSARTHandle->TxBusyState == USART_BUSY_IN_TX)
 		{
 			//Check the TxLen . If it is zero then close the data transmission
 			if(!pUSARTHandle->TxLen)
 			{
 				//Clear the TC flag
 				USART_ClearFlag(pUSARTHandle->pUSARTx,
 								USART_FLAG_TC);

 				//Reset the application state
 				pUSARTHandle->TxBusyState = USART_READY;

 				//Reset Buffer address
 				pUSARTHandle->pTxBuffer = NULL;

 				//Reset the length to zero
 				pUSARTHandle->TxLen = 0U;

 				//Return the state to application
 				USART_ApplicationEventCallback(pUSARTHandle,
 											   USART_EV_TX_CMPLT);
 			}
 		}
 	}

 /*************************Check for TXE flag ********************************************/

 	//Check the state of TXE bit
 	temp1 = pUSARTHandle->pUSARTx->SR & (1U << USART_SR_TXE);

 	//Check the state of TXEIE bit
 	temp2 = pUSARTHandle->pUSARTx->CR1 & (1U << USART_CR1_TXEIE);


 	if(temp1 && temp2 )
 	{
 		//This interrupt is because of TXE
 		if(pUSARTHandle->TxBusyState == USART_BUSY_IN_TX)
 		{
 			//Keep sending data until Txlen reaches to zero
 			if(pUSARTHandle->TxLen > 0U)
 			{
 				//Check the USART_WordLength item for 9-BIT or 8-BIT in a frame
 				if(pUSARTHandle->USART_Config.USART_WordLength == USART_WORDLEN_9BITS)
 				{
 					//if 9-BIT , load the DR with 2-bytes masking the bits other than first 9-bits
 					pdata = (uint16_t*)pUSARTHandle->pTxBuffer;

 					//loading only first 9-bits , so we have to mask with the value 0x01FF
 					pUSARTHandle->pUSARTx->DR = (*pdata & (uint16_t)0x01FFU);

 					//check for USART_ParityControl
 					if(pUSARTHandle->USART_Config.USART_ParityControl == USART_PARITY_DISABLE)
 					{
 						//No parity is used in this transfer.so, 9-bits of user data will be sent
 						//Increment pTxBuffer twice
 						pUSARTHandle->pTxBuffer += 2U;

 						//Decrement the length
 						pUSARTHandle->TxLen -= 2U;
 					}
 					else
 					{
 						//Parity bit is used in this transfer.so, 8-bits of user data will be sent
 						//The 9th bit will be replaced by parity bit by the hardware
 						pUSARTHandle->pTxBuffer++;

 						//Implement the code to decrement the length
 						pUSARTHandle->TxLen--;
 					}
 				}
 				else
 				{
 					//This is 8-bit data transfer
 					pUSARTHandle->pUSARTx->DR = (*pUSARTHandle->pTxBuffer & (uint8_t)0xFFU);

 					//Increment the buffer address
 					pUSARTHandle->pTxBuffer++;

 					//Decrement the length
 					pUSARTHandle->TxLen--;
 				}

 			}
 			if (pUSARTHandle->TxLen == 0U)
 			{
 				//TxLen is zero
 				//Clear the TXEIE bit (disable interrupt for TXE flag )
 				pUSARTHandle->pUSARTx->CR1 &= ~(1U << USART_CR1_TXEIE);
 			}
 		}
 	}

 /*************************Check for RXNE flag ********************************************/

 	//Check the status of RXNE bit
 	temp1 = pUSARTHandle->pUSARTx->SR & (1U << USART_SR_RXNE);

 	//Check the state of RXNEIE bit
 	temp2 = pUSARTHandle->pUSARTx->CR1 & (1U << USART_CR1_RXNEIE);


 	if(temp1 && temp2 )
 	{
 		//This interrupt is because of rxne
 		if(pUSARTHandle->RxBusyState == USART_BUSY_IN_RX)
 		{
 			//TXE is set so send data
 			if(pUSARTHandle->RxLen > 0U)
 			{
 				//Check the USART_WordLength to decide whether we are going to receive 9-bit of data in a frame or 8-bit
 				if(pUSARTHandle->USART_Config.USART_WordLength == USART_WORDLEN_9BITS)
 				{
 					//We are going to receive 9-bit data in a frame
 					//Now, check are we using USART_ParityControl control or not
 					if(pUSARTHandle->USART_Config.USART_ParityControl == USART_PARITY_DISABLE)
 					{
 						//No parity is used. so, all 9-bits will be of user data

 						//read only first 9-bits so mask the DR with 0x01FF
 						*((uint16_t*) pUSARTHandle->pRxBuffer) = (pUSARTHandle->pUSARTx->DR & (uint16_t)0x01FFU);

 						//Increment the pRxBuffer two times
 						pUSARTHandle->pRxBuffer += 2U;

 						//Decrement the length
 						pUSARTHandle->RxLen -=2U;
 					}
 					else
 					{
 						//Parity is used. so, 8-bits will be of user data and 1-bit is parity
 						 *pUSARTHandle->pRxBuffer = (pUSARTHandle->pUSARTx->DR & (uint8_t)0xFFU);

 						 //Now increment the pRxBuffer
 						 pUSARTHandle->pRxBuffer++;

 						 //Implement the code to decrement the length
 						pUSARTHandle->RxLen--;
 					}
 				}
 				else
 				{
 					//We are going to receive 8bit data in a frame
 					//Now, check are we using USART_ParityControl control or not
 					if(pUSARTHandle->USART_Config.USART_ParityControl == USART_PARITY_DISABLE)
 					{
 						//No parity is used , so all 8-bits will be of user data
 						//read 8-bits from DR
 						 *pUSARTHandle->pRxBuffer = (uint8_t)(pUSARTHandle->pUSARTx->DR  & (uint8_t)0xFFU);
 					}

 					else
 					{
 						//Parity is used, so , 7-bits will be of user data and 1-bit is parity
 						//read only 7-bits , hence mask the DR with 0X7F
 						 *pUSARTHandle->pRxBuffer = (uint8_t)(pUSARTHandle->pUSARTx->DR  & (uint8_t)0x7FU);

 					}

 					//Increment the pRxBuffer
 					pUSARTHandle->pRxBuffer++;

 					//Decrement the length
 					pUSARTHandle->RxLen--;
 				}


 			}//if of >0

 			if(!pUSARTHandle->RxLen)
 			{
 				//Disable the rxne
 				pUSARTHandle->pUSARTx->CR1 &= ~(1U << USART_CR1_RXNEIE);
 				pUSARTHandle->RxBusyState = USART_READY;
 				USART_ApplicationEventCallback(pUSARTHandle,
 											   USART_EV_RX_CMPLT);
 			}
 		}
 	}


 /*************************Check for CTS flag ********************************************/
 //Note : CTS feature is not applicable for UART4 and UART5

 	//Check the status of CTS bit
 	temp1 = pUSARTHandle->pUSARTx->SR & (1U << USART_SR_CTS);

 	//Check the state of CTSE bit
 	temp2 = pUSARTHandle->pUSARTx->CR3 & (1U << USART_CR3_CTSE);

 	//Check the state of CTSIE bit (This bit is not available for UART4 & UART5.)
// 	temp3 = pUSARTHandle->pUSARTx->CR3 & (1U << USART_CR3_CTSIE); // not implemented


 	if(temp1 && temp2 )
 	{
 		//Clear the CTS flag in SR
		USART_ClearFlag(pUSARTHandle->pUSARTx,
						USART_FLAG_CTS);

 		USART_ApplicationEventCallback(pUSARTHandle,
 									   USART_EV_CTS);
 	}

 /*************************Check for IDLE detection flag ********************************************/

 	//Check the status of IDLE bit
 	temp1 = pUSARTHandle->pUSARTx->SR & (1U << USART_SR_IDLE);

 	//Check the state of IDLEIE bit
 	temp2 = pUSARTHandle->pUSARTx->CR3 & (1U << USART_CR3_CTSE);


 	if(temp1 && temp2)
 	{
 		//Clear the IDLE flag. Refer to the RM to understand the clear sequence
 		USART_ClearFlag(pUSARTHandle->pUSARTx,
 								USART_FLAG_IDLE);

 		//this interrupt is because of idle
 		USART_ApplicationEventCallback(pUSARTHandle,
 									   USART_EV_IDLE);
 	}

 /*************************Check for Overrun detection flag ********************************************/

 	//Check the status of ORE bit
 	temp1 = pUSARTHandle->pUSARTx->SR & (1U << USART_SR_ORE);

 	//Check the status of RXNEIE bit
 	temp2 = pUSARTHandle->pUSARTx->CR1 & (1U << USART_CR1_RXNEIE);


 	if(temp1  && temp2 )
 	{
 		//Need not to clear the ORE flag here, instead give an api for the application to clear the ORE flag .
 		//this interrupt is because of Overrun error
 		USART_ApplicationEventCallback(pUSARTHandle,
 									   USART_ER_ORE);
 	}

 /*************************Check for Error Flag ********************************************/

 //Noise Flag, Overrun error and Framing Error in multibuffer communication
 //We dont discuss multibuffer communication in this course. please refer to the RM
 //The blow code will get executed in only if multibuffer mode is used.

 	temp2 =  pUSARTHandle->pUSARTx->CR3 & (1U << USART_CR3_EIE) ;

 	if(temp2 )
 	{
 		temp1 = pUSARTHandle->pUSARTx->SR;
 		if(temp1 & (1U << USART_SR_FE))
 		{
 			/*
 				This bit is set by hardware when a de-synchronization, excessive noise or a break character
 				is detected. It is cleared by a software sequence (an read to the USART_SR register
 				followed by a read to the USART_DR register).
 			*/
 			USART_ApplicationEventCallback(pUSARTHandle,
 										   USART_ER_FE);
 		}

 		if(temp1 & (1U << USART_SR_NF) )
 		{
 			/*
 				This bit is set by hardware when noise is detected on a received frame. It is cleared by a
 				software sequence (an read to the USART_SR register followed by a read to the
 				USART_DR register).
 			*/
 			USART_ApplicationEventCallback(pUSARTHandle,
 										   USART_ER_NF);
 		}

 		if(temp1 & (1U << USART_SR_ORE) )
 		{
 			USART_ApplicationEventCallback(pUSARTHandle,
 										   USART_ER_ORE);
 		}
 	}
}



/*********************************************************************
 * @fn      		   - USART_ApplicationEventCallback
 *
 * @brief             - Weak callback function for USART application
 * 					 	events.
 *
 * @param[in]         - pUSARTHandle : Pointer to USART handle structure
 *                                     which contains USART peripheral
 *                                     information and states.
 *
 * @param[in]		  - AppEvent : Event generated by USART driver.
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
 *                      3. This callback is used by the USART driver to
 *                         notify the application about important USART
 *                         events.
 *
 *                      4. Common USART events may include:
 *
 *                           USART_EV_TX_CMPLT
 *                           USART_EV_RX_CMPLT
 *                           USART_EV_IDLE
 *                           USART_ER_FE
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
 __weak void USART_ApplicationEventCallback(USART_Handle_t *pUSARTHandle,
 										 uint8_t AppEvent)
 {
 	/* Weak implementation
 	   Application may override this function */
 }
