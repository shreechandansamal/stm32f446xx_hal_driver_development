/*
 * stm32f446xx_spi_driver.c
 *
 *  Created on: Apr 8, 2026
 *      Author: chandan
 */

#include "stm32f446xx_spi_driver.h"
#include <assert.h>


/********************************************************************
 * @Note- private helper function section
 *
 * @brief  - don't declare these prototypes in the header because these are private functions
 *  		 and we don't want to expose it to user application
 *
 * use static keyword to indicate these are private helper function, so if application tries
 * to call them, then compiler will issue an error.
 */
static void spi_txe_interrupt_handle(SPI_Handle_t *pSPIHandle);
static void spi_rxne_interrupt_handle(SPI_Handle_t *pSPIHandle);
static void spi_ovr_err_interrupt_handle(SPI_Handle_t *pSPIHandle);

/***********************************************************************/





/*********************************************************************
 * @fn                - SPI_PeriClockControl
 *
 * @brief             - Enable or Disable the peripheral clock for
 *                      the given SPI peripheral.
 *
 * @param[in]         - pSPIx    : Base address of the SPI peripheral
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
 *                      3. Before accessing or configuring any SPI
 *                         register, the corresponding peripheral
 *                         clock must be enabled.
 *
 *                      4. If peripheral clock is not enabled:
 *                           - Register access may not work properly
 *                           - Peripheral configuration will fail
 *                           - SPI communication will not operate
 *
 *                      5. SPI1 and SPI4 are connected to APB2 bus.
 *
 *                      6. SPI2 and SPI3 are connected to APB1 bus.
 *
 *                      7. Disabling the peripheral clock stops the
 *                         clock supply to the SPI peripheral and
 *                         reduces dynamic power consumption.
 *
 *                      8. Peripheral clock should not be disabled
 *                         during an active SPI communication.
 *
 */
void SPI_PeriClockControl(SPI_RegDef_t *pSPIx,
                          uint8_t EnorDi)
{
    if(EnorDi == ENABLE)
    {
        if(pSPIx == SPI1)
        {
            SPI1_PCLK_EN();
        }
        else if(pSPIx == SPI2)
        {
            SPI2_PCLK_EN();
        }
        else if(pSPIx == SPI3)
        {
            SPI3_PCLK_EN();
        }
        else if(pSPIx == SPI4)
        {
            SPI4_PCLK_EN();
        }
    }
    else
    {
        if(pSPIx == SPI1)
        {
            SPI1_PCLK_DI();
        }
        else if(pSPIx == SPI2)
        {
            SPI2_PCLK_DI();
        }
        else if(pSPIx == SPI3)
        {
            SPI3_PCLK_DI();
        }
        else if(pSPIx == SPI4)
        {
            SPI4_PCLK_DI();
        }
    }
}


/*********************************************************************
 * @fn                - SPI_Init
 *
 * @brief             - Initialize the SPI peripheral with the
 *                      user-defined configuration parameters.
 *
 * @param[in]         - pSPIHandle : Pointer to SPI handle structure
 *                                   containing SPI peripheral base
 *                                   address and configuration data.
 *
 * @return            - none
 *
 * @Note              -
 *                      1. This function configures the SPI_CR1
 *                         register based on the user configuration.
 *
 *                      2. Before configuring the SPI peripheral,
 *                         the corresponding peripheral clock is
 *                         enabled internally.
 *
 *                      3. This function only configures the SPI
 *                         peripheral registers.
 *                         It does NOT enable the SPI peripheral.
 *
 *                      4. SPI peripheral is enabled separately using:
 *
 *                              SPI_PeripheralControl()
 *
 *                      5. The following SPI parameters are configured:
 *
 *                           - Device Mode
 *                               Master / Slave
 *
 *                           - Bus Configuration
 *                               Full Duplex
 *                               Half Duplex
 *                               Simplex RX Only
 *
 *                           - Serial Clock Speed
 *                               Baud Rate Prescaler
 *
 *                           - DFF (Data Frame Format)
 *                               8-bit / 16-bit communication
 *
 *                           - CPOL (Clock Polarity)
 *                               Idle clock state
 *
 *                           - CPHA (Clock Phase)
 *                               Sampling edge selection
 *
 *                           - SSM (Software Slave Management)
 *                               Hardware/Software NSS handling
 *
 *                      6. Full Duplex Mode:
 *                           - Separate transmit and receive paths
 *                           - Simultaneous Tx and Rx possible
 *
 *                      7. Half Duplex Mode:
 *                           - Single bidirectional data line
 *                           - Controlled using BIDIMODE bit
 *
 *                      8. Simplex RX Only Mode:
 *                           - Only receive operation enabled
 *                           - MOSI transmission path disabled
 *
 *                      9. DFF bit controls SPI frame size:
 *                           - 0 : 8-bit data frame
 *                           - 1 : 16-bit data frame
 *
 *                     10. CPOL and CPHA together determine
 *                         the SPI communication mode:
 *
 *                              Mode 0 -> CPOL=0 CPHA=0
 *                              Mode 1 -> CPOL=0 CPHA=1
 *                              Mode 2 -> CPOL=1 CPHA=0
 *                              Mode 3 -> CPOL=1 CPHA=1
 *
 *                     11. If Master mode with SSM enabled is used,
 *                         SSI bit must also be set before enabling
 *                         the SPI peripheral to avoid MODF error.
 *
 *                     12. GPIO pins must be configured in Alternate
 *                         Function mode before SPI communication.
 *
 */
void SPI_Init(SPI_Handle_t *pSPIHandle)
{
    // Enable peripheral clock
    SPI_PeriClockControl(pSPIHandle->pSPIx,
    					 ENABLE);

    // Configure SPI_CR1 register
    uint32_t cr1_value = 0;

    // a. Configure device mode
    cr1_value |= pSPIHandle->SPIConfig.SPI_DeviceMode << SPI_CR1_MSTR;

    // b. Configure bus configuration
    if(pSPIHandle->SPIConfig.SPI_BusConfig == SPI_BUS_CONFIG_FULL_DUPLEX)
    {
        // Clear BIDIMODE and RXONLY
        cr1_value &= ~(1U << SPI_CR1_BIDIMODE);
        cr1_value &= ~(1U << SPI_CR1_RXONLY);
    }
    else if(pSPIHandle->SPIConfig.SPI_BusConfig == SPI_BUS_CONFIG_HALF_DUPLEX)
    {
        // Clear RXONLY
        cr1_value &= ~(1U << SPI_CR1_RXONLY);

        // Set BIDIMODE
        cr1_value |= (1U << SPI_CR1_BIDIMODE);
    }
    else if(pSPIHandle->SPIConfig.SPI_BusConfig == SPI_BUS_CONFIG_SIMPLEX_RXONLY)
    {
        // Clear BIDIMODE
        cr1_value &= ~(1U << SPI_CR1_BIDIMODE);

        // Set RXONLY
        cr1_value |= (1U << SPI_CR1_RXONLY);
    }

    // c. Configure baud rate
    cr1_value |= pSPIHandle->SPIConfig.SPI_SclkSpeed << SPI_CR1_BR;

    // d. Configure DFF
    cr1_value |= pSPIHandle->SPIConfig.SPI_DFF << SPI_CR1_DFF;

    // e. Configure CPOL
    cr1_value |= pSPIHandle->SPIConfig.SPI_CPOL << SPI_CR1_CPOL;

    // f. Configure CPHA
    cr1_value |= pSPIHandle->SPIConfig.SPI_CPHA << SPI_CR1_CPHA;

    // g. Configure SSM
    cr1_value |= pSPIHandle->SPIConfig.SPI_SSM << SPI_CR1_SSM;

    // h. Write configuration into CR1 register
    pSPIHandle->pSPIx->CR1 = cr1_value;
}



/*********************************************************************
 * @fn                - SPI_DeInit
 *
 * @brief             - Reset the selected SPI peripheral registers
 *                      to their default reset values.
 *
 * @param[in]         - pSPIx : Base address of the SPI peripheral
 *
 * @return            - none
 *
 * @Note              -
 *                      1. This function forces the SPI peripheral
 *                         into hardware reset state.
 *
 *                      2. All SPI registers are restored to their
 *                         default reset values defined by hardware.
 *
 *                      3. This operation is generally performed using
 *                         RCC peripheral reset control registers.
 *
 *                      4. After de-initialization:
 *                           - SPI configuration is lost
 *                           - Communication stops
 *                           - Control/status registers return to
 *                             reset state
 *
 *                      5. SPI must be initialized again using
 *                         SPI_Init() before reuse.
 *
 *                      6. This function is useful when:
 *                           - Re-configuring SPI peripheral
 *                           - Recovering from communication faults
 *                           - Restarting peripheral operation
 *                           - Returning peripheral to clean state
 *
 *                      7. De-initialization should NOT be performed
 *                         during an active SPI transaction.
 *
 *                      8. Any ongoing communication may be corrupted
 *                         if reset occurs before BUSY flag clears.
 *
 *                      9. It is recommended to:
 *
 *                              Wait until BSY = 0
 *
 *                         before resetting the SPI peripheral.
 *
 *                     10. Peripheral clock may still remain enabled
 *                         after de-initialization depending on RCC
 *                         reset implementation.
 *
 */
void SPI_DeInit(SPI_RegDef_t *pSPIx)
{
    if(pSPIx == SPI1)
    {
        SPI1_REG_RESET();
    }
    else if(pSPIx == SPI2)
    {
        SPI2_REG_RESET();
    }
    else if(pSPIx == SPI3)
    {
        SPI3_REG_RESET();
    }
    else if(pSPIx == SPI4)
    {
        SPI4_REG_RESET();
    }
}



/*********************************************************************
 * @fn                - SPI_GetFlagStatus
 *
 * @brief             - Get the status of a specific SPI status flag.
 *
 * @param[in]         - pSPIx     : Base address of the SPI peripheral
 * @param[in]         - FlagName  : SPI status flag to check
 *
 * @return            - FLAG_SET   : Flag is set
 *                      FLAG_RESET : Flag is reset
 *
 * @Note              -
 *                      1. This function checks the SPI Status
 *                         Register (SPI_SR) for a specific flag.
 *
 *                      2. SPI status flags indicate the current
 *                         state of SPI communication hardware.
 *
 *                      3. Common SPI flags:
 *
 *                           SPI_FLAG_TXE
 *                               Transmit buffer empty
 *
 *                           SPI_FLAG_RXNE
 *                               Receive buffer not empty
 *
 *                           SPI_FLAG_BUSY
 *                               SPI peripheral busy
 *
 *                           SPI_FLAG_OVR
 *                               Overrun error occurred
 *
 *                      4. TXE Flag:
 *                           - Set when transmit buffer becomes empty
 *                           - Indicates next data can be loaded
 *                             into DR register
 *
 *                      5. RXNE Flag:
 *                           - Set when new data is received
 *                           - Indicates received data is available
 *                             in DR register
 *
 *                      6. BSY/BUSY Flag:
 *                           - Set during active SPI communication
 *                           - Cleared when communication completely
 *                             finishes
 *
 *                      7. OVR Flag:
 *                           - Set when received data is not read
 *                             before next data arrives
 *                           - Causes data loss condition
 *
 *                      8. This API is commonly used in polling-based
 *                         SPI communication.
 *
 *                      9. Example:
 *
 *                              while(SPI_GetFlagStatus
 *                                   (SPI2, SPI_FLAG_TXE)
 *                                   == FLAG_RESET);
 *
 *                         Waits until TX buffer becomes empty.
 *
 *                     10. Status flags are hardware controlled and
 *                         updated automatically by SPI peripheral.
 *
 */
uint8_t SPI_GetFlagStatus(SPI_RegDef_t *pSPIx,
                          uint8_t FlagName)
{
    if(pSPIx->SR & FlagName)
    {
        return FLAG_SET;
    }

    return FLAG_RESET;
}



/*********************************************************************
 * @fn                - SPI_SendData
 *
 * @brief             - Transmit data over SPI peripheral using
 *                      blocking/polling method.
 *
 * @param[in]         - pSPIx      : Base address of the SPI peripheral
 * @param[in]         - pTxBuffer  : Pointer to transmit data buffer
 * @param[in]         - Len        : Length of transmit data buffer
 *                                   in bytes
 *
 * @return            - none
 *
 * @Note              -
 *                      1. This is a Blocking/Polling based API.
 *
 *                      2. CPU continuously waits until transmission
 *                         of all data frames completes.
 *
 *                      3. Function execution does not return until
 *                         entire transmit buffer is sent.
 *
 *                      4. The API checks TXE (Transmit Buffer Empty)
 *                         flag before loading next data into DR.
 *
 *                      5. TXE flag indicates:
 *
 *                              DR is empty
 *
 *                         and ready for next transmission.
 *
 *                      6. Data written into DR register is internally
 *                         moved into the SPI Shift Register for
 *                         serial transmission through MOSI line.
 *
 *                      7. SPI communication is Full-Duplex by nature.
 *                         Therefore during transmission:
 *
 *                              Data is also simultaneously received
 *
 *                         from the slave device.
 *
 *                      8. This API only handles transmission.
 *                         Any simultaneously received data is ignored.
 *
 *                      9. If received data is not read properly,
 *                         OVR (Overrun) flag may occur in some cases.
 *
 *                     10. Supports both:
 *
 *                              8-bit DFF mode
 *                              16-bit DFF mode
 *
 *                     11. In 16-bit mode:
 *                           - Two bytes are transmitted together
 *                           - Buffer pointer increments by 2
 *                           - Length decreases by 2
 *
 *                     12. In 8-bit mode:
 *                           - One byte transmitted at a time
 *                           - Buffer pointer increments by 1
 *                           - Length decreases by 1
 *
 *                     13. This API is suitable for:
 *                           - Small data transfers
 *                           - Simple applications
 *                           - Sensor configuration
 *                           - Register writes
 *                           - Learning/debugging SPI
 *
 *                     14. Limitations of Polling Method:
 *                           - CPU remains busy waiting
 *                           - Inefficient for large transfers
 *                           - Blocks other tasks
 *                           - Not suitable for real-time systems
 *
 *                     15. Better alternatives for larger or efficient
 *                         communication:
 *
 *                              Interrupt-based SPI
 *                              DMA-based SPI
 *
 *                     16. Compared to
 *                         SPI_TransmitReceiveData():
 *
 *                           This API:
 *                               - Only focuses on Tx
 *                               - Ignores received data
 *                               - Simpler implementation
 *
 *                           TransmitReceive API:
 *                               - Handles Full-Duplex properly
 *                               - Captures received data
 *                               - Useful for SPI read operations
 *                                 and slave responses
 *
 *                     17. Master mode generates SPI clock during
 *                         transmission automatically.
 *
 *                     18. Slave mode transmission occurs only when
 *                         external master provides clock pulses.
 *
 */
void SPI_SendData(SPI_RegDef_t *pSPIx,
                  uint8_t *pTxBuffer,
                  uint32_t Len)
{
	// In 16-bit SPI mode, data length must be even (2 bytes per frame)
	if( (pSPIx->CR1 & (1U << SPI_CR1_DFF)) && (Len & 1U) )
	{
	    return;
	}

    while(Len > 0U)
    {
        // 1. Wait until TXE is set
        while(SPI_GetFlagStatus(pSPIx,
                                SPI_FLAG_TXE) == FLAG_RESET);

        // 2. Check DFF bit
        if(pSPIx->CR1 & (1U << SPI_CR1_DFF))
        {
            // ---------------- 16-bit DFF ----------------

            // Load 16-bit data into DR
        	*((__IO uint16_t *)&pSPIx->DR) = *((uint16_t*)pTxBuffer);

            // Reduce length by 2 bytes
            Len -= 2U;

            // Move buffer pointer
            pTxBuffer += 2U;
        }
        else
        {
            // ---------------- 8-bit DFF ----------------

            // Load 8-bit data into DR
        	*((__IO uint8_t *)&pSPIx->DR) = *pTxBuffer;

            // Reduce length by 1 byte
            Len--;

            // Move buffer pointer
            pTxBuffer++;
        }
    }
}



 /*********************************************************************
 * @fn                - SPI_SendDataIT
 *
 * @brief             - Transmit SPI data using Interrupt mode
 *                      (Non-Blocking communication).
 *
 * @param[in]         - pSPIHandle : Pointer to SPI handle structure
 * @param[in]         - pTxBuffer  : Pointer to transmit data buffer
 * @param[in]         - Len        : Length of transmit data buffer
 *                                   in bytes
 *
 * @return            - Current transmission state
 *                           SPI_READY
 *                           SPI_BUSY_IN_TX
 *
 * @Note              -
 *                      1. This is a Non-Blocking/Interrupt based API.
 *
 *                      2. Unlike polling-based SPI_SendData(),
 *                         this function does NOT wait until all data
 *                         transmission completes.
 *
 *                      3. Function returns immediately after enabling
 *                         SPI TX interrupt.
 *
 *                      4. Actual data transmission is performed by
 *                         SPI Interrupt Service Routine (ISR).
 *
 *                      5. This API only:
 *                           - Stores transmission information
 *                           - Updates SPI state
 *                           - Enables interrupt generation
 *
 *                      6. ISR handles:
 *                           - Writing data into DR register
 *                           - Managing buffer pointer
 *                           - Updating remaining length
 *                           - Closing transmission
 *
 *                      7. Transmission flow:
 *
 *                              Application
 *                                   ↓
 *                           SPI_SendDataIT()
 *                                   ↓
 *                           TXEIE Enabled
 *                                   ↓
 *                           TXE Interrupt Occurs
 *                                   ↓
 *                              ISR Executes
 *                                   ↓
 *                           Data loaded into DR
 *                                   ↓
 *                           Hardware shifts data
 *
 *                      8. TXE interrupt occurs whenever:
 *
 *                              TXE = 1
 *
 *                         meaning transmit buffer becomes empty.
 *
 *                      9. The API stores buffer information inside
 *                         SPI handle structure because ISR needs
 *                         persistent access after function returns.
 *
 *                     10. TxState variable prevents multiple
 *                         simultaneous transmissions using the
 *                         same SPI peripheral.
 *
 *                     11. If SPI is already transmitting:
 *
 *                              SPI_BUSY_IN_TX
 *
 *                         is returned and new transmission request
 *                         is rejected.
 *
 *                     12. This avoids:
 *                           - Buffer corruption
 *                           - State corruption
 *                           - Mixed SPI transactions
 *
 *                     13. Interrupt-based communication provides:
 *
 *                           Advantages:
 *                           -----------
 *                           - CPU remains free for other tasks
 *                           - Better responsiveness
 *                           - Efficient multitasking
 *                           - Suitable for RTOS systems
 *                           - Better for larger transfers
 *
 *                           Disadvantages:
 *                           --------------
 *                           - More complex implementation
 *                           - Requires ISR management
 *                           - State handling required
 *                           - Debugging becomes harder
 *
 *                     14. Compared to Polling API:
 *
 *                           Polling:
 *                               CPU continuously waits
 *
 *                           Interrupt:
 *                               CPU works on other tasks
 *                               until interrupt occurs
 *
 *                     15. SPI peripheral and NVIC interrupt must
 *                         already be configured properly before
 *                         using this API.
 *
 *                     16. After transmission completes, ISR should:
 *
 *                           - Disable TXE interrupt
 *                           - Reset TxState
 *                           - Call application callback event
 *
 *                     17. Since SPI is Full-Duplex, data may also
 *                         be received during transmission even though
 *                         this API only handles Tx operation.
 *
 */
uint8_t SPI_SendDataIT(SPI_Handle_t *pSPIHandle,
					uint8_t *pTxBuffer,
					uint32_t Len)
{
 uint8_t state = pSPIHandle->TxState;

 // Allow transmission only if SPI is not busy
 if(state != SPI_BUSY_IN_TX)
 {
	  //[WAP]
//	  // In 16-bit SPI mode, data length must be even
//	  if( (pSPIHandle->pSPIx->CR1 & (1U << SPI_CR1_DFF)) &&
//	   (Len & 1U) )
//	  {
//	   return SPI_ERROR_INVALID_LENGTH;
//	  }

	 // 1. Save Tx buffer address and length
	 pSPIHandle->pTxBuffer = pTxBuffer;
	 pSPIHandle->TxLen = Len;

	 // 2. Mark SPI state as busy in transmission
	 pSPIHandle->TxState = SPI_BUSY_IN_TX;

	 // 3. Enable TXE interrupt
	 pSPIHandle->pSPIx->CR2 |= (1 << SPI_CR2_TXEIE);

	 // 4. Actual transmission handled by ISR
 }

 return state;
}



/*********************************************************************
 * @fn                - SPI_ReceiveData
 *
 * @brief             - Receive data over SPI using Polling method
 *                      (Blocking communication).
 *
 * @param[in]         - pSPIx     : Base address of SPI peripheral
 * @param[in]         - pRxBuffer : Pointer to receive buffer
 * @param[in]         - Len       : Number of bytes to receive
 *
 * @return            - none
 *
 * @Note              -
 *                      1. This is a Blocking/Polling based API.
 *
 *                      2. Function continuously waits until data
 *                         becomes available in SPI receive buffer.
 *
 *                      3. Function returns only after receiving all
 *                         requested bytes.
 *
 *                      4. Reception is controlled using RXNE flag.
 *
 *                              RXNE = Receive Buffer Not Empty
 *
 *                         RXNE becomes SET when new data arrives
 *                         from SPI shift register into DR register.
 *
 *                      5. Reception flow:
 *
 *                              External SPI device sends data
 *                                           ↓
 *                              SPI shift register receives bits
 *                                           ↓
 *                              Data copied into DR register
 *                                           ↓
 *                                   RXNE flag SET
 *                                           ↓
 *                              Software reads DR register
 *                                           ↓
 *                                   RXNE flag CLEARED
 *
 *                      6. Reading DR register clears RXNE flag.
 *
 *                      7. In SPI, receiving clock must always be
 *                         generated by SPI master.
 *
 *                         Therefore:
 *
 *                           Master:
 *                               Must transmit dummy data to receive.
 *
 *                           Slave:
 *                               Receives whenever master clocks data.
 *
 *                      8. This API ONLY reads received data from DR.
 *
 *                         It does NOT generate clock pulses.
 *
 *                      9. If used in SPI Master mode:
 *
 *                           Another transmit operation must generate
 *                           SCK clocks, otherwise RXNE will never set.
 *
 *                     10. DFF bit determines frame format:
 *
 *                           DFF = 0 → 8-bit communication
 *                           DFF = 1 → 16-bit communication
 *
 *                     11. In 8-bit mode:
 *
 *                           - One byte transferred at a time
 *                           - Len decreases by 1
 *                           - Buffer increments by 1
 *
 *                     12. In 16-bit mode:
 *
 *                           - Two bytes transferred at a time
 *                           - Len decreases by 2
 *                           - Buffer increments by 2
 *
 *                     13. Typecasting:
 *
 *                           *(uint16_t *)pRxBuffer
 *
 *                         allows storing full 16-bit data into
 *                         memory correctly.
 *
 *                     14. Polling-based reception is simple but
 *                         CPU remains busy while waiting for data.
 *
 *                     15. Advantages:
 *
 *                           - Simple implementation
 *                           - Easy debugging
 *                           - Good for small transfers
 *
 *                     16. Disadvantages:
 *
 *                           - CPU blocking
 *                           - Wastes processing time
 *                           - Poor multitasking capability
 *                           - Inefficient for large transfers
 *
 *                     17. Compared to Interrupt/DMA:
 *
 *                           Polling:
 *                               CPU continuously checks flags
 *
 *                           Interrupt:
 *                               CPU interrupted only when needed
 *
 *                           DMA:
 *                               Hardware transfers automatically
 *
 *                     18. Important SPI concept:
 *
 *                           SPI is Full-Duplex.
 *
 *                         During every transmission,
 *                         simultaneous reception also occurs.
 *
 *                         Therefore:
 *
 *                           Transmit and Receive happen together.
 *
 *                     19. If received data is not read properly,
 *                         OVR (Overrun Error) may occur.
 *
 *                     20. This API assumes:
 *
 *                           - SPI peripheral already initialized
 *                           - SPI enabled using SPE bit
 *                           - Correct clock generation available
 *
 */
void SPI_ReceiveData(SPI_RegDef_t *pSPIx,
				   uint8_t *pRxBuffer,
				   uint32_t Len)
{
	 // In 16-bit SPI mode, data length must be even (2 bytes per frame)
	 if( (pSPIx->CR1 & (1U << SPI_CR1_DFF)) && (Len & 1U) )
	 {
	     return;
	 }

	 while(Len > 0U)
	 {
	  // 1. Wait until RXNE flag becomes SET
	  while(SPI_GetFlagStatus(pSPIx,
			  	  	  	  	  SPI_FLAG_RXNE) == FLAG_RESET);

	  // 2. Check DFF bit for frame format
	  if(pSPIx->CR1 & (1U << SPI_CR1_DFF))
	  {
		  // ---------------- 16-bit communication ----------------

		  // Read 16-bit data from DR register
		  *(uint16_t *)pRxBuffer = *((__IO uint16_t *)&pSPIx->DR);

		  // Reduce remaining length by 2 bytes
		  Len -= 2U;

		  // Move buffer pointer to next 16-bit location
		  pRxBuffer += 2U;
	  }
	  else
	  {
		  // ---------------- 8-bit communication ----------------

		  // Read 8-bit data from DR register
		  *pRxBuffer = *((__IO uint8_t *)&pSPIx->DR);

		  // Reduce remaining length by 1 byte
		  Len--;

		  // Move buffer pointer to next byte
		  pRxBuffer++;
	  }
	 }
}



/*********************************************************************
 * @fn                - SPI_ReceiveDataIT
 *
 * @brief             - Receive SPI data using Interrupt mode
 *                      (Non-Blocking communication).
 *
 * @param[in]         - pSPIHandle : Pointer to SPI handle structure
 * @param[in]         - pRxBuffer  : Pointer to receive data buffer
 * @param[in]         - Len        : Length of receive data buffer
 *                                   in bytes
 *
 * @return            - Current reception state
 *                           SPI_READY
 *                           SPI_BUSY_IN_RX
 *
 * @Note              -
 *                      1. This is a Non-Blocking/Interrupt based API.
 *
 *                      2. Function returns immediately after enabling
 *                         SPI RX interrupt.
 *
 *                      3. Actual data reception is handled by
 *                         SPI Interrupt Service Routine (ISR).
 *
 *                      4. This API only:
 *                           - Stores receive buffer information
 *                           - Updates SPI reception state
 *                           - Enables RX interrupt generation
 *
 *                      5. ISR handles:
 *                           - Reading data from DR register
 *                           - Copying data into Rx buffer
 *                           - Updating buffer pointer
 *                           - Updating remaining length
 *                           - Closing reception
 *
 *                      6. Reception flow:
 *
 *                              External device sends data
 *                                           ↓
 *                              SPI shift register receives bits
 *                                           ↓
 *                              Data copied into DR register
 *                                           ↓
 *                                   RXNE flag SET
 *                                           ↓
 *                              RXNE interrupt generated
 *                                           ↓
 *                                   ISR Executes
 *                                           ↓
 *                              Data copied to memory buffer
 *
 *                      7. RXNE interrupt occurs whenever:
 *
 *                              RXNE = 1
 *
 *                         meaning receive buffer is not empty.
 *
 *                      8. The API stores buffer information inside
 *                         SPI handle structure because ISR executes
 *                         later after function already returns.
 *
 *                      9. RxState prevents multiple simultaneous
 *                         receptions using the same SPI peripheral.
 *
 *                     10. If SPI is already busy receiving:
 *
 *                              SPI_BUSY_IN_RX
 *
 *                         is returned and new reception request
 *                         is rejected.
 *
 *                     11. This prevents:
 *                           - Buffer corruption
 *                           - Overwriting receive state
 *                           - Invalid SPI transactions
 *
 *                     12. SPI is Full-Duplex communication.
 *
 *                         Therefore during reception:
 *
 *                              Transmission also occurs.
 *
 *                         SPI cannot receive data without clock.
 *
 *                     13. In Master mode:
 *
 *                           Master must generate SCK clocks
 *                           by transmitting dummy data.
 *
 *                         Otherwise:
 *
 *                              RXNE will never set.
 *
 *                     14. In Slave mode:
 *
 *                           External master generates clocks.
 *
 *                     15. This API does NOT generate clock pulses.
 *
 *                         It only handles received data after
 *                         RXNE interrupt occurs.
 *
 *                     16. Interrupt-based reception provides:
 *
 *                           Advantages:
 *                           -----------
 *                           - CPU remains free
 *                           - Better multitasking
 *                           - More efficient than polling
 *                           - Better responsiveness
 *                           - Suitable for RTOS systems
 *
 *                           Disadvantages:
 *                           --------------
 *                           - More complex implementation
 *                           - ISR management required
 *                           - State handling required
 *                           - Harder debugging
 *
 *                     17. Compared to Polling API:
 *
 *                           Polling:
 *                               CPU continuously checks RXNE
 *
 *                           Interrupt:
 *                               CPU interrupted only when
 *                               data arrives
 *
 *                     18. Since SPI receive buffer is small,
 *                         received data must be read quickly.
 *
 *                         Otherwise:
 *
 *                              OVR (Overrun Error)
 *
 *                         may occur.
 *
 *                     19. After reception completes, ISR should:
 *
 *                           - Disable RXNE interrupt
 *                           - Reset RxState
 *                           - Notify application callback
 *
 *                     20. SPI peripheral and NVIC interrupt must
 *                         already be configured properly before
 *                         using this API.
 *
 */
uint8_t SPI_ReceiveDataIT(SPI_Handle_t *pSPIHandle,
						 uint8_t *pRxBuffer,
						 uint32_t Len)
{
   uint8_t state = pSPIHandle->RxState;

   // Allow reception only if SPI is not busy
   if(state != SPI_BUSY_IN_RX)
   {
	   //[WAP]
	   // In 16-bit SPI mode, data length must be even
//	   if( (pSPIHandle->pSPIx->CR1 & (1U << SPI_CR1_DFF)) &&
//		   (Len & 1U) )
//	   {
//		   return SPI_ERROR_INVALID_LENGTH;
//	   }

	   // 1. Save Rx buffer address and length
	   pSPIHandle->pRxBuffer = pRxBuffer;
	   pSPIHandle->RxLen = Len;

	   // 2. Mark SPI state as busy in reception
	   pSPIHandle->RxState = SPI_BUSY_IN_RX;

	   // 3. Enable RXNE interrupt
	   pSPIHandle->pSPIx->CR2 |= (1U << SPI_CR2_RXNEIE);

	   // 4. Actual reception handled by ISR
   }

   return state;
}



/*********************************************************************
 * @fn                - SPI_TransmitReceiveData
 *
 * @brief             - Transmit and Receive SPI data simultaneously
 *                      using Polling method in Full-Duplex mode.
 *
 * @param[in]         - pSPIx      : Base address of SPI peripheral
 * @param[in]         - pTxBuffer  : Pointer to transmit buffer
 * @param[in]         - pRxBuffer  : Pointer to receive buffer
 * @param[in]         - Len        : Number of bytes to exchange
 *
 * @return            - none
 *
 * @Note              -
 *
 *                      1. SPI is a Full-Duplex communication protocol.
 *
 *                         Therefore:
 *
 *                              Transmission and Reception occur
 *                              simultaneously.
 *
 *                      2. During every SPI clock cycle:
 *
 *                           - One bit shifts OUT on MOSI
 *                           - One bit shifts IN on MISO
 *
 *                         using the same clock pulse.
 *
 *                      3. SPI internally uses shift registers.
 *
 *                              Master Shift Register
 *                                         ⇅
 *                              Slave Shift Register
 *
 *                         Data exchange occurs simultaneously
 *                         between both devices.
 *
 *                      4. Because SPI exchanges data frame-by-frame:
 *
 *                              Tx length == Rx length
 *
 *                         must always be maintained.
 *
 *                      5. This API is especially useful for:
 *
 *                           - Sensor communication
 *                           - RFID modules
 *                           - Flash memories
 *                           - Display controllers
 *                           - Register read/write operations
 *                           - Full-Duplex communication systems
 *
 *                      6. SPI receive operation cannot happen
 *                         independently.
 *
 *                         Reception requires clock pulses.
 *
 *                      7. In Master mode:
 *
 *                           Master generates clock.
 *
 *                         Therefore to receive data, master must
 *                         transmit something:
 *
 *                              Real data OR Dummy data.
 *
 *                      8. Example:
 *
 *                              Master sends:
 *                                  0xFF
 *
 *                              Slave responds:
 *                                  Register value
 *
 *                         This is extremely common in SPI devices.
 *
 *                      9. This API performs:
 *
 *                              Transmit + Receive
 *
 *                         together inside same loop.
 *
 *                     10. TXE flag checked first because:
 *
 *                           TXE = Transmit Buffer Empty
 *
 *                         DR register must become empty before
 *                         loading next transmit data.
 *
 *                     11. After transmission starts:
 *
 *                              Hardware shifts data serially
 *                                           ↓
 *                              Incoming data copied into DR
 *                                           ↓
 *                                   RXNE becomes SET
 *
 *                     12. RXNE flag indicates:
 *
 *                              Receive Buffer Not Empty
 *
 *                         meaning received data is ready to read.
 *
 *                     13. Reading DR clears RXNE flag.
 *
 *                     14. Supports both:
 *
 *                              8-bit mode
 *                              16-bit mode
 *
 *                         depending on DFF bit configuration.
 *
 *                     15. In 16-bit mode:
 *
 *                           - Two bytes transferred together
 *                           - Len decreases by 2
 *                           - Buffer increments by 2
 *
 *                     16. In 8-bit mode:
 *
 *                           - One byte transferred at a time
 *                           - Len decreases by 1
 *                           - Buffer increments by 1
 *
 *                     17. volatile uint8_t access used in 8-bit mode:
 *
 *                              *((volatile uint8_t*)&pSPIx->DR)
 *
 *                         because DR register is physically 16-bit
 *                         wide in STM32 hardware.
 *
 *                         This forces compiler to perform exact
 *                         8-bit access operation.
 *
 *                     18. Without volatile/typecast:
 *
 *                           Compiler may perform incorrect
 *                           16-bit memory access.
 *
 *                     19. This API is Polling/Blocking based.
 *
 *                         CPU continuously waits for:
 *
 *                              TXE flag
 *                              RXNE flag
 *
 *                         during entire transaction.
 *
 *                     20. Advantages:
 *
 *                           - Simple implementation
 *                           - Easy debugging
 *                           - Good for small transfers
 *
 *                     21. Disadvantages:
 *
 *                           - CPU blocking
 *                           - Inefficient for large transfers
 *                           - No multitasking efficiency
 *                           - CPU wastes cycles waiting
 *
 *                     22. For high-performance applications:
 *
 *                           - Interrupt mode
 *                           - DMA mode
 *
 *                         is preferred.
 *
 *                     23. Important:
 *
 *                         Received data may not always be useful.
 *
 *                         Example:
 *
 *                           During register write:
 *                               Received byte may be dummy.
 *
 *                           During register read:
 *                               Received byte may be actual data.
 *
 *                     24. This API assumes:
 *
 *                           - SPI already initialized
 *                           - SPE bit enabled
 *                           - Correct CPOL/CPHA configured
 *                           - NSS handling properly managed
 *
 */
void SPI_TransmitReceiveData(SPI_RegDef_t *pSPIx,
							 uint8_t *pTxBuffer,
							 uint8_t *pRxBuffer,
							 uint32_t Len)
{
	 // In 16-bit SPI mode, data length must be even (2 bytes per frame)
	 if( (pSPIx->CR1 & (1U << SPI_CR1_DFF)) && (Len & 1U) )
	 {
	     return;
	 }

	while(Len > 0U)
	{
		// 1. Wait until TXE flag becomes SET
		while(SPI_GetFlagStatus(pSPIx,
								SPI_FLAG_TXE) == FLAG_RESET);

		// 2. Check DFF bit
		if(pSPIx->CR1 & (1U << SPI_CR1_DFF))
		{
			// ================= 16-bit mode =================

			// Load 16-bit data into DR
			*((__IO uint16_t *)&pSPIx->DR) = *((uint16_t *)pTxBuffer);

			// Wait until RXNE becomes SET
			while(SPI_GetFlagStatus(pSPIx,
									SPI_FLAG_RXNE) == FLAG_RESET);

			// Read received 16-bit data
			*((uint16_t *)pRxBuffer) = *((__IO uint16_t *)&pSPIx->DR);

			// Reduce remaining length
			Len -= 2U;

			// Move buffer pointers
			pTxBuffer += 2U;
			pRxBuffer += 2U;
		}
		else
		{
			// ================= 8-bit mode =================

			// Load 8-bit data into DR
			*((__IO uint8_t *)&pSPIx->DR) = *pTxBuffer;

			// Wait until RXNE becomes SET
			while(SPI_GetFlagStatus(pSPIx,
									SPI_FLAG_RXNE) == FLAG_RESET);

			// Read received 8-bit data
			*pRxBuffer = *((__IO uint8_t *)&pSPIx->DR);

			// Reduce remaining length
			Len--;

			// Move buffer pointers
			pTxBuffer++;
			pRxBuffer++;
		}
	}
}


/*********************************************************************
 * @fn                - SPI_PeripheralControl
 *
 * @brief             - Enable or Disable the SPI peripheral
 *                      using SPE bit in CR1 register.
 *
 * @param[in]         - pSPIx   : Base address of SPI peripheral
 * @param[in]         - EnOrDi  : ENABLE or DISABLE macros
 *
 * @return            - none
 *
 * @Note              -
 *
 *                      1. SPI peripheral is controlled using:
 *
 *                              SPE bit (SPI Enable)
 *
 *                         present in:
 *
 *                              SPI_CR1 register
 *
 *                      2. SPE meanings:
 *
 *                              SPE = 1 → SPI Enabled
 *                              SPE = 0 → SPI Disabled
 *
 *                      3. After Reset:
 *
 *                              SPI peripheral remains Disabled
 *
 *                         by default to save power and prevent
 *                         accidental communication.
 *
 *                      4. Before enabling SPI:
 *
 *                         All SPI configurations should be completed.
 *
 *                         Example:
 *
 *                           - Device Mode
 *                           - Baud Rate
 *                           - CPOL
 *                           - CPHA
 *                           - DFF
 *                           - SSM
 *                           - Bus Configuration
 *
 *                      5. Recommended SPI initialization flow:
 *
 *                              Enable SPI peripheral clock
 *                                           ↓
 *                              Configure GPIO pins
 *                                           ↓
 *                              Configure SPI registers
 *                                           ↓
 *                              Enable SPI peripheral (SPE = 1)
 *
 *                      6. Once SPE becomes SET:
 *
 *                              SPI shift engine starts operating.
 *
 *                         SPI becomes capable of:
 *
 *                           - Transmitting data
 *                           - Receiving data
 *                           - Generating clock (Master mode)
 *                           - Monitoring SPI flags
 *
 *                      7. In Master mode:
 *
 *                              SCK generation begins only during
 *                              actual transmission.
 *
 *                         Enabling SPE alone does NOT continuously
 *                         generate SPI clock.
 *
 *                      8. During active SPI communication:
 *
 *                         Some control bits should NOT be modified.
 *
 *                         Otherwise:
 *
 *                           - Communication corruption
 *                           - Framing errors
 *                           - Unpredictable behavior
 *
 *                         may occur.
 *
 *                      9. Therefore it is recommended to configure:
 *
 *                              CR1
 *                              CR2
 *
 *                         before enabling SPE.
 *
 *                     10. Disabling SPI:
 *
 *                              SPE = 0
 *
 *                         stops SPI operation.
 *
 *                     11. Before disabling SPI:
 *
 *                         Ensure ongoing communication completes.
 *
 *                         Usually done by checking:
 *
 *                              BSY flag
 *
 *                              BSY = 0
 *
 *                         meaning SPI is no longer busy.
 *
 *                     12. If SPI disabled while BSY = 1:
 *
 *                           - Last frame may truncate
 *                           - Data corruption may occur
 *                           - Slave synchronization may break
 *
 *                     13. Proper disable sequence:
 *
 *                              Wait until TXE = 1
 *                                           ↓
 *                              Wait until BSY = 0
 *                                           ↓
 *                                   Disable SPE
 *
 *                     14. SPI communication is transaction-based.
 *
 *                         Example:
 *
 *                              NSS LOW
 *                                   ↓
 *                              Data Exchange
 *                                   ↓
 *                              NSS HIGH
 *
 *                         SPE usually remains enabled during
 *                         entire transaction.
 *
 *                     15. Constantly enabling/disabling SPE for
 *                         every byte is NOT recommended.
 *
 *                         Because:
 *
 *                           - Extra overhead
 *                           - Possible synchronization problems
 *                           - Unnecessary complexity
 *
 *                     16. In many real embedded systems:
 *
 *                              SPE enabled once
 *
 *                         during initialization and kept enabled
 *                         permanently until application shutdown.
 *
 *                     17. Power consumption of idle SPI peripheral
 *                         is usually very small compared to active
 *                         communication and GPIO switching.
 *
 *                     18. SPI peripheral operation depends on:
 *
 *                              Peripheral Clock + SPE bit
 *
 *                         Both must be enabled for SPI operation.
 *
 *                     19. This function only controls:
 *
 *                              SPI internal peripheral engine
 *
 *                         It does NOT:
 *
 *                           - Configure GPIO pins
 *                           - Control NSS pin
 *                           - Enable peripheral clock
 *
 *                     20. This API assumes:
 *
 *                           - SPI peripheral clock already enabled
 *                           - SPI registers already configured
 *
 */
void SPI_PeripheralControl(SPI_RegDef_t *pSPIx,
							uint8_t EnOrDi)
{
	if(EnOrDi == ENABLE)
	{
		// Enable SPI peripheral
		pSPIx->CR1 |= (1U << SPI_CR1_SPE);
	}
	else
	{
		// Disable SPI peripheral
		pSPIx->CR1 &= ~(1U << SPI_CR1_SPE);
	}
}



/*********************************************************************
 * @fn                - SPI_SSIConfig
 *
 * @brief             - Configure the SSI (Internal Slave Select)
 *                      bit of the SPI peripheral.
 *
 * @param[in]         - pSPIx     : Base address of the SPI peripheral
 * @param[in]         - EnOrDi    : ENABLE or DISABLE macro
 *
 * @return            - none
 *
 * @Note              -
 *                      1. SSI bit is mainly used when Software Slave
 *                         Management (SSM) is enabled.
 *
 *                      2. When SSM = 1:
 *                           - NSS pin management is done by software
 *                           - Internal NSS signal depends on SSI bit
 *
 *                      3. In Master mode, if SSI is not set while
 *                         SSM is enabled, the SPI peripheral detects:
 *
 *                              NSS = LOW
 *
 *                         and enters MODF (Mode Fault) condition.
 *
 *                      4. To prevent MODF error in Master mode:
 *
 *                              SSM = 1
 *                              SSI = 1
 *
 *                         must be configured before enabling SPI.
 *
 *                      5. Setting SSI bit makes the SPI peripheral
 *                         internally assume:
 *
 *                              NSS = HIGH
 *
 *                         which allows Master mode operation.
 *
 *                      6. This function is generally used when:
 *                           - Hardware NSS management is not used
 *                           - Manual NSS control is preferred
 *                           - Single slave communication is used
 *
 *                      7. If hardware NSS management is used
 *                         (SSM = 0), this function has no effect.
 *
 */
void SPI_SSIConfig(SPI_RegDef_t *pSPIx,
				   uint8_t EnOrDi)
{

	if(EnOrDi == ENABLE)
	{
		pSPIx->CR1 |= (1U << SPI_CR1_SSI);
	}else
	{
		pSPIx->CR1 &= ~(1U << SPI_CR1_SSI);
	}
}



/*********************************************************************
 * @fn                - SPI_SSOEConfig
 *
 * @brief             - Configure SSOE (Slave Select Output Enable)
 *                      bit for hardware NSS management in Master mode.
 *
 * @param[in]         - pSPIx   : Base address of SPI peripheral
 * @param[in]         - EnOrDi  : ENABLE or DISABLE macros
 *
 * @return            - none
 *
 * @Note              -
 *
 *                      1. SSOE stands for:
 *
 *                              Slave Select Output Enable
 *
 *                      2. SSOE bit is present in:
 *
 *                              SPI_CR2 register
 *
 *                      3. SSOE controls automatic hardware management
 *                         of NSS pin in SPI Master mode.
 *
 *                      4. When:
 *
 *                              SSOE = 1
 *
 *                         and SPI peripheral enabled:
 *
 *                              SPE = 1
 *
 *                         hardware automatically drives NSS pin LOW.
 *
 *                      5. When SPI peripheral disabled:
 *
 *                              SPE = 0
 *
 *                         hardware automatically releases NSS pin HIGH.
 *
 *                      6. Therefore hardware internally performs:
 *
 *                              SPE = 1  → NSS LOW
 *                              SPE = 0  → NSS HIGH
 *
 *                      7. This is called:
 *
 *                              Hardware NSS Output Management
 *
 *                      8. Mainly useful in:
 *
 *                              SPI Master Mode
 *
 *                         because Master controls transaction start
 *                         and stop timing.
 *
 *                      9. Typical SPI transaction:
 *
 *                              NSS LOW
 *                                   ↓
 *                              Data Transfer
 *                                   ↓
 *                              NSS HIGH
 *
 *                     10. With SSOE enabled:
 *
 *                         Hardware automatically handles NSS signal.
 *
 *                         Software does NOT need manual GPIO control.
 *
 *                     11. Without SSOE:
 *
 *                         User must manually control NSS pin using GPIO.
 *
 *                         Example:
 *
 *                              GPIO LOW  → Start transaction
 *                              GPIO HIGH → End transaction
 *
 *                     12. Manual NSS control is extremely common
 *                         in real embedded systems because:
 *
 *                           - Multiple slaves may exist
 *                           - Custom timing needed
 *                           - Complex protocols used
 *                           - Better transaction control
 *
 *                     13. Example:
 *
 *                              RC522 RFID module
 *                              NRF24L01+
 *                              SPI Displays
 *                              Flash memories
 *
 *                         often use manual NSS handling.
 *
 *                     14. If SSM = 1:
 *
 *                              Software Slave Management enabled
 *
 *                         NSS pin logic handled internally using:
 *
 *                              SSI bit
 *
 *                         In that case:
 *
 *                              SSOE usually not required.
 *
 *                     15. If SSM = 0:
 *
 *                              Hardware NSS management active
 *
 *                         then SSOE becomes important for Master mode.
 *
 *                     16. Why SSOE important in Master mode:
 *
 *                         If NSS internally becomes LOW unexpectedly,
 *                         hardware may detect:
 *
 *                              MODF (Mode Fault)
 *
 *                         and automatically clear:
 *
 *                              MSTR bit
 *
 *                         causing SPI Master to stop.
 *
 *                     17. SSOE helps hardware maintain proper NSS
 *                         state automatically during SPI operation.
 *
 *                     18. Important distinction:
 *
 *                           SSOE:
 *                               Controls NSS OUTPUT behavior
 *
 *                           SSI:
 *                               Controls internal NSS logic level
 *
 *                           SSM:
 *                               Selects Software/Hardware NSS handling
 *
 *                     19. Common configurations:
 *
 *                         A. Software NSS Management:
 *
 *                              SSM = 1
 *                              SSI = 1
 *
 *                              Manual GPIO NSS optional
 *
 *                         B. Hardware NSS Management:
 *
 *                              SSM = 0
 *                              SSOE = 1
 *
 *                              Hardware controls NSS pin
 *
 *                     20. In Slave mode:
 *
 *                         NSS pin generally acts as INPUT because
 *                         external Master controls slave selection.
 *
 *                     21. This function only configures SSOE bit.
 *
 *                         It does NOT:
 *
 *                           - Enable SPI peripheral
 *                           - Configure GPIO pins
 *                           - Configure SSM/SSI
 *
 *                     22. This API assumes:
 *
 *                           - SPI already initialized
 *                           - Correct SPI mode configured
 *
 */
void SPI_SSOEConfig(SPI_RegDef_t *pSPIx,
                    uint8_t EnOrDi)
{
    if(EnOrDi == ENABLE)
    {
        // Enable automatic NSS output management
        pSPIx->CR2 |= (1U << SPI_CR2_SSOE);
    }
    else
    {
        // Disable automatic NSS output management
        pSPIx->CR2 &= ~(1U << SPI_CR2_SSOE);
    }
}



/*********************************************************************
 * @fn                - SPI_IRQInterruptConfig
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
 *                              SPI1_IRQn
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
 *                     12. This function ONLY configures NVIC.
 *
 *                         It does NOT:
 *
 *                           - Enable peripheral interrupt source
 *                           - Set TXEIE/RXNEIE bits
 *                           - Handle ISR logic
 *
 *                     13. For SPI interrupt communication BOTH needed:
 *
 *                         A. Peripheral interrupt enable
 *
 *                                Example:
 *
 *                                TXEIE = 1
 *                                RXNEIE = 1
 *
 *                         B. NVIC interrupt enable
 *
 *                                SPI_IRQInterruptConfig()
 *
 *                     14. Important difference:
 *
 *                              Peripheral interrupt enable
 *                              decides WHETHER interrupt can occur.
 *
 *                              NVIC interrupt enable
 *                              decides WHETHER CPU can receive it.
 *
 *                     15. Bitwise operation:
 *
 *                              (1U << IRQNumber)
 *
 *                         generates bit mask for required IRQ.
 *
 *                     16. Why '1U' used:
 *
 *                         Unsigned constant avoids signed overflow
 *                         problems during left shift.
 *
 *                     17. ISER registers generally use:
 *
 *                              |= operator
 *
 *                         because:
 *
 *                         We want to preserve previously enabled
 *                         interrupts while enabling new interrupt.
 *
 *                     18. ICER registers are special:
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
 *                     19. Therefore using:
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
 *                     20. Example:
 *
 *                              *NVIC_ICER0 = (1 << 5);
 *
 *                         disables ONLY IRQ5.
 *
 *                         Other bits written as 0 have:
 *
 *                              no effect
 *
 *                     21. Read behavior of ICER:
 *
 *                              0 → interrupt disabled
 *                              1 → interrupt enabled
 *
 *                         But write behavior is:
 *
 *                              0 → no effect
 *                              1 → disable interrupt
 *
 *                     22. This distinction is VERY important in ARM
 *                         peripheral register programming.
 *
 *                     23. Typical SPI interrupt setup:
 *
 *                              SPI_Init()
 *                                   ↓
 *                              SPI_IRQInterruptConfig()
 *                                   ↓
 *                              SPI_SendDataIT()
 *                                   ↓
 *                              ISR handles communication
 *
 *                     24. This API supports IRQ0 to IRQ95.
 *
 */
void SPI_IRQInterruptConfig(uint8_t IRQNumber,
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
 * @fn                - SPI_IRQPriorityConfig
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
 *                              SPI interrupt priority = 2
 *                              UART interrupt priority = 5
 *
 *                         If both interrupts occur together:
 *
 *                              SPI ISR executes first
 *
 *                     23. Higher-priority interrupt can preempt
 *                         lower-priority interrupt.
 *
 *                     24. Example:
 *
 *                              UART ISR running
 *                                   ↓
 *                              SPI interrupt occurs
 *                                   ↓
 *                              SPI has higher priority
 *                                   ↓
 *                              CPU pauses UART ISR
 *                                   ↓
 *                              CPU executes SPI ISR
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
 *                     27. Typical interrupt configuration flow:
 *
 *                              SPI_Init()
 *                                   ↓
 *                              SPI_IRQPriorityConfig()
 *                                   ↓
 *                              SPI_IRQInterruptConfig()
 *                                   ↓
 *                              SPI_SendDataIT()
 *
 *                     28. Why '|=' used:
 *
 *                         To preserve other IRQ priority fields
 *                         inside same IPR register.
 *
 *                     29. Better practice:
 *
 *                         Clear target field before writing new
 *                         priority value.
 *
 *                         Otherwise old bits may remain.
 *
 *                     30. Example improvement:
 *
 *                              Clear target field first
 *                              then write new priority
 *
 */
void SPI_IRQPriorityConfig(uint8_t IRQNumber,
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
 * @fn      		  - SPI_IRQHandling
 *
 * @brief             - Handles all SPI interrupt events generated by
 * 						the SPI peripheral.
 *
 * @param[in]         - Pointer to SPI Handle structure
 *
 * @return            - none
 *
 * @Note              -
 * 						1. This function must be called inside the
 *                         actual MCU interrupt service routine (ISR).
 *
 *                      2. The SPI peripheral can generate multiple
 *                         interrupt events:
 *
 *                           a. TXE  -> Transmit buffer empty
 *                           b. RXNE -> Receive buffer not empty
 *                           c. OVR  -> Overrun error
 *
 *                      3. This function checks:
 *                           - Whether the corresponding status flag
 *                             is SET in SR register.
 *                           - Whether the corresponding interrupt
 *                             source is ENABLED in CR2 register.
 *
 *                      4. If both conditions are true, the related
 *                         helper handler function is called.
 *
 *                      5. Helper functions are kept private/internal
 *                         to the driver because they are implementation
 *                         details and should not be exposed to the
 *                         application layer.
 *
 *                      6. TXE Event:
 *                           Generated when transmit buffer becomes
 *                           empty and new data can be loaded into DR.
 *
 *                      7. RXNE Event:
 *                           Generated when new received data becomes
 *                           available in DR register and must be read.
 *
 *                      8. OVR Event:
 *                           Generated when received data was not read
 *                           before the next frame arrived, causing
 *                           data loss.
 *
 *                      9. Interrupt driven communication allows the
 *                         CPU to perform other tasks while SPI data
 *                         transfer happens in the background.
 *
 *                     10. This function acts like a dispatcher/state
 *                         controller for SPI interrupt events.
 *
 *                     11. Typical Flow:
 *
 *                          Application
 *                               |
 *                               v
 *                        SPI_SendDataIT()
 *                               |
 *                               v
 *                      Enable TXE Interrupt
 *                               |
 *                               v
 *                         Hardware sets TXE
 *                               |
 *                               v
 *                          SPI IRQ occurs
 *                               |
 *                               v
 *                        SPI_IRQHandling()
 *                               |
 *                               +--> TXE handler
 *                               +--> RXNE handler
 *                               +--> OVR handler
 *
 *                     12. This function itself does not directly
 *                         transmit or receive data. Actual DR access
 *                         is performed inside helper functions.
 *
 */
void SPI_IRQHandling(SPI_Handle_t *pSPIHandle)
{
	uint8_t temp1, temp2;

	//1. check for TXE flag
	temp1 = pSPIHandle->pSPIx->SR & (1U << SPI_SR_TXE);
	temp2 = pSPIHandle->pSPIx->CR2 & (1U << SPI_CR2_TXEIE);

	if (temp1 && temp2)
	{
		//handle TXE interrupt
		spi_txe_interrupt_handle(pSPIHandle);
	}

	//2. check for RXNE flag
	temp1 = pSPIHandle->pSPIx->SR & (1U << SPI_SR_RXNE);
	temp2 = pSPIHandle->pSPIx->CR2 & (1U << SPI_CR2_RXNEIE);

	if (temp1 && temp2)
	{
		//handle RXNE interrupt
		spi_rxne_interrupt_handle(pSPIHandle);
	}

	//3. check for OVR flag
	temp1 = pSPIHandle->pSPIx->SR & (1U << SPI_SR_OVR);
	temp2 = pSPIHandle->pSPIx->CR2 & (1U << SPI_CR2_ERRIE);

	if (temp1 && temp2) {
		//handle OVR error interrupt
		spi_ovr_err_interrupt_handle(pSPIHandle);
	}
}



//helper function implementations (static refers private function)

/*********************************************************************
 * @fn      		  - spi_txe_interrupt_handle
 *
 * @brief             - Internal helper function to handle SPI TXE
 * 						(Transmit Buffer Empty) interrupt event.
 *
 * @param[in]         - Pointer to SPI Handle structure which contains
 * 						all SPI configuration and runtime state
 * 						information.
 *
 * @return            - none
 *
 * @Note              -
 * 						1. This is a private/helper function used only
 *                         by SPI_IRQHandling().
 *
 *                      2. This function is executed whenever:
 *
 *                           TXE flag = 1
 *                                   AND
 *                           TXEIE bit = 1
 *
 *                      3. TXE (Transmit Buffer Empty) means:
 *                         The SPI Data Register (DR) is empty and ready
 *                         to accept new data for transmission.
 *
 *                      4. This function performs the actual loading of
 *                         user data into the SPI DR register during
 *                         interrupt-based transmission.
 *
 *                      5. Data flow:
 *
 *                           Memory Buffer
 *                                 |
 *                                 v
 *                             pTxBuffer
 *                                 |
 *                                 v
 *                               DR Register
 *                                 |
 *                                 v
 *                          Shift Register
 *                                 |
 *                                 v
 *                               MOSI Line
 *
 *                      6. Supports both:
 *                           - 8-bit DFF mode
 *                           - 16-bit DFF mode
 *
 *                      7. In 16-bit DFF mode:
 *                           - Two bytes are transmitted at once.
 *                           - TxLen decreases by 2.
 *                           - Buffer pointer increments by 2.
 *
 *                      8. In 8-bit DFF mode:
 *                           - One byte is transmitted at a time.
 *                           - TxLen decreases by 1.
 *                           - Buffer pointer increments by 1.
 *
 *                      9. Why TXE interrupt is useful:
 *
 *                           Instead of continuously polling TXE flag,
 *                           hardware automatically interrupts CPU
 *                           whenever DR becomes empty.
 *
 *                           This allows:
 *                             - Better CPU utilization
 *                             - Background communication
 *                             - Non-blocking transfer
 *
 *                     10. Transmission completion logic:
 *
 *                           When TxLen becomes zero:
 *                             a. Transmission is completed.
 *                             b. TXE interrupt must be disabled.
 *                             c. SPI state must become READY.
 *                             d. Application must be notified.
 *
 *                     11. SPI_CloseTransmission() typically:
 *                           - Disables TXEIE bit
 *                           - Clears Tx buffer pointers
 *                           - Resets TxLen
 *                           - Changes SPI state to READY
 *
 *                     12. SPI_ApplicationEventCallback() is a weak
 *                         callback/event notification mechanism which
 *                         informs the application layer that SPI
 *                         transmission has completed.
 *
 *                     13. Important SPI hardware concept:
 *
 *                           Writing to DR does NOT immediately send
 *                           data on MOSI.
 *
 *                           DR --> Shift Register --> MOSI
 *
 *                           Hardware internally transfers data from
 *                           DR to Shift Register and serially shifts
 *                           it out synchronized with SCK clock.
 *
 *                     14. Interrupt driven transmission sequence:
 *
 *                           Application
 *                               |
 *                               v
 *                        SPI_SendDataIT()
 *                               |
 *                               v
 *                         Enable TXEIE
 *                               |
 *                               v
 *                        TXE interrupt occurs
 *                               |
 *                               v
 *                 spi_txe_interrupt_handle()
 *                               |
 *                               v
 *                        Load next data into DR
 *                               |
 *                               v
 *                        Hardware shifts data out
 *                               |
 *                               v
 *                        TXE becomes SET again
 *                               |
 *                               v
 *                     Next interrupt generated
 *
 *                     15. This function only handles data movement
 *                         between memory and DR register.
 *                         Actual clock generation and serial shifting
 *                         are fully handled by SPI hardware.
 *
 */
static void spi_txe_interrupt_handle(SPI_Handle_t *pSPIHandle)
{
	//1. Check DFF bit in CR1 register
	if(pSPIHandle->pSPIx->CR1 & (1 << SPI_CR1_DFF))
	{
		//================= 16-bit DFF =================

		//Load 16-bit data into DR register
		*((__IO uint16_t *)&pSPIHandle->pSPIx->DR) =
				*((uint16_t *)pSPIHandle->pTxBuffer);

		//Decrease remaining length by 2 bytes
		pSPIHandle->TxLen -= 2U;

		//Move buffer pointer to next 16-bit data
		pSPIHandle->pTxBuffer += 2U;
	}
	else
	{
		//================= 8-bit DFF =================

		//Load 8-bit data into DR register
		*((__IO uint8_t *)&pSPIHandle->pSPIx->DR) =
				*pSPIHandle->pTxBuffer;

		//Decrease remaining length by 1 byte
		pSPIHandle->TxLen--;

		//Move buffer pointer to next byte
		pSPIHandle->pTxBuffer++;
	}

	//2. Check whether transmission completed
	if(!pSPIHandle->TxLen)
	{
		//Close SPI transmission
		SPI_CloseTransmission(pSPIHandle);

		//Notify application layer
		SPI_ApplicationEventCallback(pSPIHandle,
									 SPI_EVENT_TX_CMPLT);
	}
}



/*********************************************************************
 * @fn      		  - spi_rxne_interrupt_handle
 *
 * @brief             - Internal helper function to handle SPI RXNE
 * 						(Receive Buffer Not Empty) interrupt event.
 *
 * @param[in]         - Pointer to SPI Handle structure which contains
 * 						all SPI configuration and runtime state
 * 						information.
 *
 * @return            - none
 *
 * @Note              -
 * 						1. This is a private/helper function used only
 *                         by SPI_IRQHandling().
 *
 *                      2. This function is executed whenever:
 *
 *                           RXNE flag = 1
 *                                   AND
 *                           RXNEIE bit = 1
 *
 *                      3. RXNE (Receive Buffer Not Empty) means:
 *                         New received data is available inside the
 *                         SPI Data Register (DR) and must be read.
 *
 *                      4. Reading DR register clears RXNE flag.
 *
 *                      5. If DR is not read before next data arrives,
 *                         an OVR (Overrun Error) may occur.
 *
 *                      6. This function performs the actual movement
 *                         of received SPI data:
 *
 *                              DR Register
 *                                   |
 *                                   v
 *                              pRxBuffer
 *                                   |
 *                                   v
 *                                Memory
 *
 *                      7. Supports both:
 *                           - 8-bit DFF mode
 *                           - 16-bit DFF mode
 *
 *                      8. In 16-bit DFF mode:
 *                           - Two bytes are received at once.
 *                           - RxLen decreases by 2.
 *                           - Buffer pointer increments by 2.
 *
 *                      9. In 8-bit DFF mode:
 *                           - One byte is received at a time.
 *                           - RxLen decreases by 1.
 *                           - Buffer pointer increments by 1.
 *
 *                     10. Why RXNE interrupt is useful:
 *
 *                           Instead of continuously polling RXNE flag,
 *                           hardware automatically interrupts CPU
 *                           whenever new data arrives.
 *
 *                           This allows:
 *                             - Background reception
 *                             - Better CPU utilization
 *                             - Non-blocking communication
 *
 *                     11. Reception completion logic:
 *
 *                           When RxLen becomes zero:
 *                             a. Reception is completed.
 *                             b. RXNE interrupt must be disabled.
 *                             c. SPI state must become READY.
 *                             d. Application must be notified.
 *
 *                     12. SPI_CloseReception() typically:
 *                           - Disables RXNEIE bit
 *                           - Clears Rx buffer pointers
 *                           - Resets RxLen
 *                           - Changes SPI state to READY
 *
 *                     13. SPI_ApplicationEventCallback() informs the
 *                         application layer that reception completed.
 *
 *                     14. Important SPI concept:
 *
 *                           SPI reception is synchronized with SPI
 *                           transmission.
 *
 *                           A device cannot receive data unless clock
 *                           pulses are generated.
 *
 *                           In Master mode:
 *                             - Clock is generated only while sending.
 *                             - Dummy bytes may need to be transmitted
 *                               to receive incoming data.
 *
 *                     15. SPI receive sequence:
 *
 *                           Slave shifts data onto MISO
 *                                      |
 *                                      v
 *                             SPI Shift Register
 *                                      |
 *                                      v
 *                                  DR Register
 *                                      |
 *                             RXNE flag becomes SET
 *                                      |
 *                                      v
 *                               SPI interrupt occurs
 *                                      |
 *                                      v
 *                    spi_rxne_interrupt_handle()
 *                                      |
 *                                      v
 *                           Copy DR data into memory
 *
 *                     16. This function only handles movement of data
 *                         from DR register to application buffer.
 *                         Actual serial shifting is handled completely
 *                         by SPI hardware.
 *
 *                     17. In Full-Duplex SPI communication:
 *
 *                           Every transmitted frame also generates
 *                           one received frame simultaneously.
 *
 *                           Therefore TXE and RXNE events often occur
 *                           together during communication.
 *
 */
static void spi_rxne_interrupt_handle(SPI_Handle_t *pSPIHandle)
{
	//1. Check DFF bit in CR1 register
	if(pSPIHandle->pSPIx->CR1 & (1U << SPI_CR1_DFF))
	{
		//================= 16-bit DFF =================

		//Read 16-bit data from DR register
		*((uint16_t *)pSPIHandle->pRxBuffer) =
				*((__IO uint16_t *)&pSPIHandle->pSPIx->DR);

		//Decrease remaining length by 2 bytes
		pSPIHandle->RxLen -= 2U;

		//Move buffer pointer to next 16-bit location
		pSPIHandle->pRxBuffer += 2U;
	}
	else
	{
		//================= 8-bit DFF =================

		//Read 8-bit data from DR register
		*(pSPIHandle->pRxBuffer) =
				*((__IO uint8_t *)&pSPIHandle->pSPIx->DR);

		//Decrease remaining length by 1 byte
		pSPIHandle->RxLen--;

		//Move buffer pointer to next byte location
		pSPIHandle->pRxBuffer++;
	}

	//2. Check whether reception completed
	if(!pSPIHandle->RxLen)
	{
		//Close SPI reception
		SPI_CloseReception(pSPIHandle);

		//Notify application layer
		SPI_ApplicationEventCallback(pSPIHandle,
									 SPI_EVENT_RX_CMPLT);
	}
}



/*********************************************************************
 * @fn      		  - spi_ovr_err_interrupt_handle
 *
 * @brief             - Internal helper function to handle SPI OVR
 * 						(Overrun Error) interrupt event.
 *
 * @param[in]         - Pointer to SPI Handle structure which contains
 * 						all SPI configuration and runtime state
 * 						information.
 *
 * @return            - none
 *
 * @Note              -
 * 						1. This is a private/helper function used only
 *                         by SPI_IRQHandling().
 *
 *                      2. This function is executed whenever:
 *
 *                           OVR flag = 1
 *                                   AND
 *                           ERRIE bit = 1
 *
 *                      3. OVR (Overrun Error) occurs when:
 *
 *                           New SPI data arrives before the previous
 *                           received data was read from DR register.
 *
 *                      4. In SPI communication:
 *
 *                           Shift Register
 *                                  |
 *                                  v
 *                               DR Register
 *
 *                         If DR still contains old unread data and
 *                         another frame arrives from Shift Register,
 *                         hardware cannot move the new data into DR.
 *
 *                         This causes OVR error.
 *
 *                      5. OVR means:
 *                           - Data loss occurred
 *                           - One received frame is lost
 *                           - Application was too slow to read DR
 *
 *                      6. Common causes of OVR:
 *
 *                           a. RXNE flag ignored
 *                           b. CPU too busy
 *                           c. Interrupt latency too high
 *                           d. Continuous SPI clock from Master
 *                           e. Polling loop too slow
 *
 *                      7. According to STM32 SPI hardware rules,
 *                         OVR flag can ONLY be cleared by:
 *
 *                           Step 1: Read DR register
 *                           Step 2: Read SR register
 *
 *                         Sequence is extremely important.
 *
 *                      8. Why DR then SR?
 *
 *                           Hardware internally uses this exact
 *                           sequence as acknowledgment that software
 *                           handled the received data/error.
 *
 *                      9. This function clears OVR only when SPI is
 *                         NOT busy in transmission.
 *
 *                         Because during active transmission:
 *                           - DR may still contain valid TX/RX data
 *                           - Clearing OVR immediately may disturb
 *                             ongoing communication
 *
 *                     10. If SPI is busy in TX:
 *
 *                           Application callback is triggered and
 *                           application must decide when it is safe
 *                           to clear OVR.
 *
 *                     11. Typical OVR sequence:
 *
 *                           RXNE = 1
 *                                |
 *                                v
 *                         Application did NOT read DR
 *                                |
 *                                v
 *                        New SPI frame arrives
 *                                |
 *                                v
 *                             OVR = 1
 *                                |
 *                                v
 *                         SPI interrupt occurs
 *                                |
 *                                v
 *                 spi_ovr_err_interrupt_handle()
 *                                |
 *                                +--> Read DR
 *                                +--> Read SR
 *                                |
 *                                v
 *                         OVR flag cleared
 *
 *                     12. Important:
 *
 *                           OVR mainly affects received data.
 *
 *                           Transmitted data may still successfully
 *                           leave MOSI pin.
 *
 *                     13. In Full-Duplex SPI:
 *
 *                           Every transmission also generates a
 *                           received frame.
 *
 *                           Therefore even "Transmit Only" operations
 *                           may generate OVR if RX data is ignored.
 *
 *                     14. Best practices to avoid OVR:
 *
 *                           - Always read received data
 *                           - Use interrupt/DMA for high-speed SPI
 *                           - Keep ISR execution short
 *                           - Do not block CPU excessively
 *
 *                     15. SPI_ApplicationEventCallback() informs the
 *                         application layer that an OVR error occurred
 *                         so recovery action can be taken.
 *
 */
static void spi_ovr_err_interrupt_handle(SPI_Handle_t *pSPIHandle)
{
	//1. Clear OVR flag only if SPI is not busy in TX
	if(pSPIHandle->TxState != SPI_BUSY_IN_TX)
	{
		//a. Read DR register
		(void)pSPIHandle->pSPIx->DR;

		//b.Followed by SR register read
		(void)pSPIHandle->pSPIx->SR;
	}

	//2. Inform application about OVR error
	SPI_ApplicationEventCallback(pSPIHandle,
								 SPI_EVENT_OVR_ERR);
}



/*********************************************************************
 * @fn      		  - SPI_CloseTransmission
 *
 * @brief             - Closes an ongoing SPI interrupt-based
 * 						transmission operation.
 *
 * @param[in]         - Pointer to SPI Handle structure which contains
 * 						all SPI configuration and runtime state
 * 						information.
 *
 * @return            - none
 *
 * @Note              -
 * 						1. This function is mainly used internally by
 *                         the SPI driver after completion of an
 *                         interrupt-based transmission.
 *
 *                      2. Typically called from:
 *
 *                           spi_txe_interrupt_handle()
 *
 *                         when TxLen becomes zero.
 *
 *                      3. This function does NOT disable the SPI
 *                         peripheral (SPE bit).
 *
 *                         It only closes the current transmission
 *                         process handled by interrupts.
 *
 *                      4. Main responsibilities:
 *
 *                           a. Disable TXE interrupt
 *                           b. Clear transmit buffer pointer
 *                           c. Reset remaining transmit length
 *                           d. Mark SPI state as READY
 *
 *                      5. TXEIE bit is cleared to stop further TXE
 *                         interrupts from continuously triggering.
 *
 *                      6. If TXEIE is not disabled:
 *
 *                           TXE flag remains SET because DR becomes
 *                           empty after transmission.
 *
 *                           This would continuously generate SPI
 *                           interrupts forever.
 *
 *                      7. pTxBuffer is cleared (NULL) because:
 *
 *                           - Transmission is completed
 *                           - Old pointer should not be reused
 *                           - Prevents accidental invalid access
 *
 *                      8. TxLen is reset to zero to indicate:
 *
 *                           No remaining data pending for transmission.
 *
 *                      9. TxState is changed to SPI_READY so that:
 *
 *                           New SPI transmission requests are allowed.
 *
 *                     10. Without resetting TxState:
 *
 *                           Future SPI_SendDataIT() calls may think
 *                           SPI is still busy.
 *
 *                     11. Interrupt transmission flow:
 *
 *                           SPI_SendDataIT()
 *                                  |
 *                                  v
 *                           Enable TXEIE
 *                                  |
 *                                  v
 *                        TXE interrupts generated
 *                                  |
 *                                  v
 *                     spi_txe_interrupt_handle()
 *                                  |
 *                                  v
 *                        TxLen becomes zero
 *                                  |
 *                                  v
 *                      SPI_CloseTransmission()
 *
 *                     12. Important distinction:
 *
 *                           Closing transmission
 *                                     !=
 *                           Disabling SPI peripheral
 *
 *                         SPI hardware may still remain enabled for:
 *                           - Reception
 *                           - Future transfers
 *                           - Full-duplex operation
 *
 *                     13. In Full-Duplex SPI:
 *
 *                           Even after TX completes, RX operations
 *                           may still continue depending on the
 *                           communication design.
 *
 *                     14. This function is part of the driver's
 *                         internal state-machine mechanism for managing
 *                         non-blocking SPI communication.
 *
 */
void SPI_CloseTransmission(SPI_Handle_t *pSPIHandle)
{
	//1. Disable TXE interrupt
	pSPIHandle->pSPIx->CR2 &= ~(1U << SPI_CR2_TXEIE);

	//2. Clear transmit buffer pointer
	pSPIHandle->pTxBuffer = NULL;

	//3. Reset transmit length
	pSPIHandle->TxLen = 0U;

	//4. Mark SPI state as READY
	pSPIHandle->TxState = SPI_READY;
}



/*********************************************************************
 * @fn      		  - SPI_CloseReception
 *
 * @brief             - Closes an ongoing SPI reception process in
 * 						Interrupt mode.
 *
 * @param[in]         - pSPIHandle : Pointer to SPI handle structure
 *                                   which contains SPI peripheral base
 *                                   address, buffers, lengths and
 *                                   current SPI states.
 *
 * @return            - none
 *
 * @Note              -
 * 						1. This API is mainly used internally by the
 *                         SPI driver after completion of interrupt-
 *                         based reception.
 *
 *                      2. This function disables the RXNE interrupt
 *                         generation by clearing RXNEIE bit in CR2.
 *
 *                      3. After disabling RX interrupt, the driver
 *                         clears all reception related software
 *                         resources:
 *
 *                           - Rx buffer pointer
 *                           - Rx length counter
 *                           - Rx busy state
 *
 *                      4. Once this API is executed:
 *
 *                           RxState = SPI_READY
 *
 *                         which means the SPI peripheral is now ready
 *                         for another reception request.
 *
 *                      5. This function does NOT disable the SPI
 *                         peripheral itself (SPE bit remains same).
 *                         Only reception operation is closed.
 *
 *                      6. This API is generally called automatically
 *                         from:
 *
 *                           spi_rxne_interrupt_handle()
 *
 *                         when RxLen becomes zero.
 *
 *                      7. If application wants to start another
 *                         interrupt-based reception, it can now call:
 *
 *                           SPI_ReceiveDataIT()
 *
 *                         safely after this function completes.
 *
 */
void SPI_CloseReception(SPI_Handle_t *pSPIHandle)
{

	//1. Disable RXNE interrupt generation
	pSPIHandle->pSPIx->CR2 &= ~(1U << SPI_CR2_RXNEIE);

	//2. Clear reception buffer pointer
	pSPIHandle->pRxBuffer = NULL;

	//3. Reset remaining receive length
	pSPIHandle->RxLen = 0U;

	//4. Mark SPI reception state as READY
	pSPIHandle->RxState = SPI_READY;
}


/*********************************************************************
 * @fn      		  - SPI_ClearOVRFlag
 *
 * @brief             - Clears the SPI Overrun (OVR) flag.
 *
 * @param[in]         - pSPIx : Base address of the SPI peripheral
 *
 * @return            - none
 *
 * @Note              -
 * 						1. OVR (Overrun) error occurs when new received
 *                         data arrives into the receive buffer before
 *                         the previous received data was read by
 *                         software.
 *
 *                      2. In SPI, received data must be read from DR
 *                         register before next frame completes.
 *
 *                      3. If software fails to read received data in
 *                         time, the hardware sets the OVR flag in SR.
 *
 *                      4. While OVR flag is set:
 *
 *                           - Further SPI reception may stop
 *                           - RXNE behavior becomes abnormal
 *                           - Communication may break
 *
 *                      5. According to STM32 reference manual,
 *                         OVR flag can ONLY be cleared by:
 *
 *                           Step 1 : Read DR register
 *                           Step 2 : Read SR register
 *
 *                         in the same sequence.
 *
 *                      6. Reading only SR or only DR will NOT clear
 *                         the OVR flag.
 *
 *                      7. This API is generally used:
 *
 *                           - Inside OVR interrupt handler
 *                           - Inside application error handling
 *                           - After failed SPI communication
 *
 *
 */
void SPI_ClearOVRFlag(SPI_RegDef_t *pSPIx)
{
	/* Step 1 : Read DR register */
	(void)pSPIx->DR;

	/* Step 2 : Read SR register */
	(void)pSPIx->SR;
}



/*********************************************************************
 * @fn      		  - SPI_ApplicationEventCallback
 *
 * @brief             - Weak callback function for SPI application
 * 						events.
 *
 * @param[in]         - pSPIHandle : Pointer to SPI handle structure
 *                                   which contains SPI peripheral
 *                                   information and states.
 *
 * @param[in]		  - AppEvent : Event generated by SPI driver.
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
 *                      3. This callback is used by the SPI driver to
 *                         notify the application about important SPI
 *                         events.
 *
 *                      4. Common SPI events may include:
 *
 *                           SPI_EVENT_TX_CMPLT
 *                           SPI_EVENT_RX_CMPLT
 *                           SPI_EVENT_OVR_ERR
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
__weak void SPI_ApplicationEventCallback(SPI_Handle_t *pSPIHandle,
										 uint8_t AppEvent)
{
	/* Weak implementation
	   Application may override this function */
}
















