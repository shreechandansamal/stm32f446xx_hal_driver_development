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
 * @fn      		  - USART_SetBaudRate
 *
 * @brief             -
 *
 * @param[in]         - pUSARTx  : Base address of the USART peripheral
 * @param[in]         - BaudRate
 *
 * @return            - none
 *
 * @Note              -

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
 * @fn                - USART_Init
 *
 * @brief             - Initialize the USART peripheral with the
 *                      user-defined configuration parameters.
 *
 * @param[in]         - pUSARTHandle : Pointer to USART handle
 * 									   structure containing USART
 * 									   peripheral base address and
 * 									   configuration data.
 *
 * @return            - none
 *
 * @Note              -
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
 * @fn      		  - USART_ClearFlag
 *
 * @brief             - Clear the status of a specific USART status flag.
 *
 * @param[in]         - pUSARTx   : Base address of the USART peripheral
 * @param[in]         - FlagName  : USART status flag to clear
 *
 * @return            -
 *
 * @Note              - Applicable to only USART_CTS_FLAG , USART_LBD_FLAG
 * 						USART_TC_FLAG, USART_TC_FLAG flags
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
 *                         does not modify any other USART configuration.
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
 * @fn      		  - USART_FlushReceiveData
 *
 * @brief             - helps clear RXNE, made by starting garbage
 * 						data in the DR.
 *
 * @param[in]         - pUSARTx : Pointer to the target USART peripheral
 *                                instance.
 *
 * @return            - none
 *
 * @Note              - Read the DR for the first garbage value exist
 * 						in the DR which makes RXNE == SET in the
 * 						starting, by reading the DR it erase that
 * 						garbage value and clear or RESET the RXNE.
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
 * @fn      		  - USART_SendData
 *
 * @brief             - USART Send Data using polling (blocking)
 * 						method.
 *
 * @param[in]         - pUSARTHandle : Pointer to USART handle
 * 									   structure containing USART
 * 									   peripheral base address and
 * 									   configuration data.
 * @param[in]         - pTxBuffer : Pointer to transmit data buffer.
 * @param[in]         - Len : Number of bytes to transmit.
 *
 * @return            - none
 *
 * @Note              -

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
 * @fn      		  - USART_SendDataIT
 *
 * @brief             - USART Send Data using interrupt (non-blocking)
 * 						method.
 *
 * @param[in]         - pUSARTHandle : Pointer to USART handle
 * 									   structure containing USART
 * 									   peripheral base address and
 * 									   configuration data.
 * @param[in]         - pTxBuffer : Pointer to transmit data buffer.
 * @param[in]         - Len : Number of bytes to transmit.
 *
 * @return            - Current USART communication state:
 *
 *                        USART_READY
 *                        USART_BUSY_IN_TX
 *                        USART_BUSY_IN_RX
 *
 * @Note              -
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
 * @fn      		  - USART_ReceiveData
 *
 * @brief             - USART Receive Data using polling (blocking)
 * 						method.
 *
 * @param[in]         - pUSARTHandle : Pointer to USART handle
 * 									   structure containing USART
 * 									   peripheral base address and
 * 									   configuration data.
 * @param[in]         - pRxBuffer : Pointer to receive data buffer.
 * @param[in]         - Len : Number of bytes to transmit.
 *
 * @return            - none
 *
 * @Note              -

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
 * @fn      		  - USART_ReceiveDataIT
 *
 * @brief             - USART Send Data using interrupt (non-blocking)
 * 						method.
 *
 * @param[in]         - pUSARTHandle : Pointer to USART handle
 * 									   structure containing USART
 * 									   peripheral base address and
 * 									   configuration data.
 * @param[in]         - pRxBuffer : Pointer to receive data buffer.
 * @param[in]         - Len : Number of bytes to transmit.
 *
 * @return            - Current USART communication state:
 *
 *                        USART_READY
 *                        USART_BUSY_IN_TX
 *                        USART_BUSY_IN_RX
 *
 * @Note              -
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
 * @fn      		  - USART_IRQHandler
 *
 * @brief             - Handles USART event interrupts generated by
 *                      the peripheral.
 *
 * @param[in]         - pUSARTHandle : Pointer to USART handle structure.
 *
 * @return            - none
 *
 * @Note              -
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
 * @brief             - Weak callback function for I2C application
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
 *                      3. This callback is used by the I2C driver to
 *                         notify the application about important I2C
 *                         events.
 *
 *                      4. Common I2C events may include:
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
