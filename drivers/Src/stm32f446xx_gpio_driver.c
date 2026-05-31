/*
 * File Name: stm32f446xx_gpio_driver.c
 * Description: GPIO Driver Specific Header File
 * Created on: 26-Feb-2026
 * Author: chandan
 */




#include "stm32f446xx_gpio_driver.h"




/*********************************************************************
 * @fn      		  - GPIO_PeriClockControl
 *
 * @brief             - Enables or disables the peripheral clock
 * 						for the specified GPIO port.
 *
 * @param[in]         - pGPIOx :
 * 						Base address of the GPIO peripheral.
 *
 * @param[in]         - EnOrDi :
 * 						ENABLE  -> Enable peripheral clock
 * 						DISABLE -> Disable peripheral clock
 *
 * @return            - none
 *
 * @Note              -
 * 						1. GPIO peripheral registers cannot be
 * 						   accessed unless the corresponding
 * 						   peripheral clock is enabled.
 *
 * 						2. GPIO peripherals are connected to the
 * 						   AHB1 bus in STM32F446xx MCU.
 *
 * 						3. Clock control is managed through
 * 						   RCC_AHB1ENR register.
 *
 * 						4. Disabling the peripheral clock reduces
 * 						   power consumption when GPIO peripheral
 * 						   is not in use.
 *
 * 						5. Peripheral clock must be enabled before:
 * 								- GPIO initialization
 * 								- Register configuration
 * 								- Read/Write operations
 *
 */
void GPIO_PeriClockControl(GPIO_RegDef_t *pGPIOx,
						   uint8_t EnorDi)
{
	if(EnorDi == ENABLE)
	{
		if(pGPIOx == GPIOA){
			GPIOA_PCLK_EN();

		}else if(pGPIOx == GPIOB){
			GPIOB_PCLK_EN();

		}else if(pGPIOx == GPIOC){
		    GPIOC_PCLK_EN();

	    }else if(pGPIOx == GPIOD){
			GPIOD_PCLK_EN();

		}else if(pGPIOx == GPIOE){
			GPIOE_PCLK_EN();

		}else if(pGPIOx == GPIOF){
			GPIOF_PCLK_EN();

		}else if(pGPIOx == GPIOG){
			GPIOG_PCLK_EN();

		}else if(pGPIOx == GPIOH){
			GPIOH_PCLK_EN();
		}

	}
	else
	{
		if(pGPIOx == GPIOA){
			GPIOA_PCLK_DI();

		}else if(pGPIOx == GPIOB){
			GPIOB_PCLK_DI();

		}else if(pGPIOx == GPIOC){
		    GPIOC_PCLK_DI();

	    }else if(pGPIOx == GPIOD){
			GPIOD_PCLK_DI();

		}else if(pGPIOx == GPIOE){
			GPIOE_PCLK_DI();

		}else if(pGPIOx == GPIOF){
			GPIOF_PCLK_DI();

		}else if(pGPIOx == GPIOG){
			GPIOG_PCLK_DI();

		}else if(pGPIOx == GPIOH){
			GPIOH_PCLK_DI();
		}
	}
}



/*********************************************************************
 * @fn      		  - GPIO_Init
 *
 * @brief             - Initializes and configures a GPIO pin based on
 * 						the configuration parameters provided in the
 * 						GPIO handle structure.
 *
 * @param[in]         - pGPIOHandle : Pointer to GPIO handle structure
 *                                    which contains:
 *                                     - GPIO port base address
 *                                     - GPIO pin number
 *                                     - GPIO pin configuration
 *
 * @return            - none
 *
 * @Note              -
 * 						1. This function configures the following:
 * 						   - GPIO pin mode
 * 						   - GPIO output speed
 * 						   - Pull-up/Pull-down settings
 * 						   - Output type
 * 						   - Alternate function
 * 						   - External interrupt configuration
 *
 * 						2. Before accessing GPIO registers, the
 * 						   peripheral clock for the corresponding
 * 						   GPIO port must be enabled.
 *
 * 						3. Each GPIO pin has dedicated bit fields
 * 						   inside multiple configuration registers:
 *
 * 						   MODER   -> Pin mode selection
 * 						   OTYPER  -> Output type
 * 						   OSPEEDR -> Output speed
 * 						   PUPDR   -> Pull-up/Pull-down
 * 						   AFR[]   -> Alternate function selection
 *
 * 						4. Most GPIO configuration registers use
 * 						   2-bit fields per pin, therefore:
 *
 * 						   shift = (2 * PinNumber)
 *
 * 						   Example:
 * 						   Pin 5 uses bits [11:10]
 *
 * 						5. Alternate function registers AFR[0]/AFR[1]
 * 						   use 4 bits per pin:
 *
 * 						   AFR[0] -> Pins 0 to 7
 * 						   AFR[1] -> Pins 8 to 15
 *
 * 						6. Interrupt modes are configured using EXTI
 * 						   and SYSCFG peripherals:
 *
 * 						   EXTI   -> Edge detection & interrupt mask
 * 						   SYSCFG -> Maps GPIO port to EXTI line
 *
 * 						7. Interrupt trigger types:
 *
 * 						   GPIO_MODE_IT_FT
 * 						   -> Falling edge trigger
 *
 * 						   GPIO_MODE_IT_RT
 * 						   -> Rising edge trigger
 *
 * 						   GPIO_MODE_IT_RFT
 * 						   -> Rising + Falling edge trigger
 *
 * 						8. EXTI lines are shared between ports.
 *
 * 						   Example:
 * 						   PA5, PB5, PC5 all use EXTI5.
 *
 * 						   Therefore SYSCFG EXTICR register is used
 * 						   to select which GPIO port is connected
 * 						   to a particular EXTI line.
 *
 * 						9. Sequence for interrupt configuration:
 *
 * 						   a. Configure edge trigger
 * 						   b. Configure SYSCFG EXTICR
 * 						   c. Unmask interrupt in EXTI_IMR
 * 						   d. Enable IRQ in NVIC
 *
 * 						10. In Alternate Function mode, GPIO pin is
 * 						    controlled by internal peripherals like:
 *
 * 						    - SPI
 * 						    - I2C
 * 						    - USART
 * 						    - TIM
 * 						    - CAN
 * 						    etc.
 *
 * 						11. This API only configures GPIO registers.
 * 						    It does NOT enable NVIC interrupts.
 * 						    NVIC configuration must be done separately
 * 						    using GPIO_IRQInterruptConfig().
 *
 * 						12. Register access explanation:
 *
 * 						    Clearing bits:
 * 						    ----------------
 * 						    REG &= ~(mask)
 *
 * 						    Setting bits:
 * 						    ----------------
 * 						    REG |= value
 *
 * 						    This ensures only target pin fields are
 * 						    modified without affecting other pins.
 *
 */
void GPIO_Init(GPIO_Handle_t *pGPIOHandle)
{
	uint32_t temp = 0U;

	/* Enable the peripheral clock for the GPIO port */
	GPIO_PeriClockControl(pGPIOHandle->pGPIOx,
						  ENABLE);

	/* ============================================================
	 * 1. Configure GPIO pin mode
	 * ============================================================ */
	if(pGPIOHandle->GPIO_PinConfig.GPIO_PinMode <= GPIO_MODE_ANALOG)
	{
		/* ---------------- Non-Interrupt Modes ---------------- */

		/*
		 * MODER register uses 2 bits per pin
		 *
		 * Example:
		 * Pin 3 -> bits [7:6]
		 */

		temp = (pGPIOHandle->GPIO_PinConfig.GPIO_PinMode
				<< (2U * pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber));

		/* Clear existing mode bits */
		pGPIOHandle->pGPIOx->MODER &=
				~(0x3U << (2U * pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber));

		/* Set new mode */
		pGPIOHandle->pGPIOx->MODER |= temp;
	}
	else
	{
		/* ---------------- Interrupt Modes ---------------- */

		if(pGPIOHandle->GPIO_PinConfig.GPIO_PinMode == GPIO_MODE_IT_FT)
		{
			/* Falling edge trigger */

			EXTI->FTSR |=
					(1U << pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber);

			EXTI->RTSR &=
					~(1U << pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber);
		}
		else if(pGPIOHandle->GPIO_PinConfig.GPIO_PinMode == GPIO_MODE_IT_RT)
		{
			/* Rising edge trigger */

			EXTI->RTSR |=
					(1U << pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber);

			EXTI->FTSR &=
					~(1U << pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber);
		}
		else if(pGPIOHandle->GPIO_PinConfig.GPIO_PinMode == GPIO_MODE_IT_RFT)
		{
			/* Rising + Falling edge trigger */

			EXTI->RTSR |=
					(1U << pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber);

			EXTI->FTSR |=
					(1U << pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber);
		}

		/* =====================================================
		 * 2. Configure SYSCFG EXTICR register
		 * ===================================================== */

		/*
		 * temp1 -> EXTICR register number
		 *
		 * EXTI0-3   -> EXTICR[0]
		 * EXTI4-7   -> EXTICR[1]
		 * EXTI8-11  -> EXTICR[2]
		 * EXTI12-15 -> EXTICR[3]
		 */

		uint8_t temp1 =
				pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber / 4U;

		/*
		 * temp2 -> Exact EXTI field position inside EXTICR
		 */

		uint8_t temp2 =
				pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber % 4U;

		/*
		 * Convert GPIO base address into port code
		 *
		 * GPIOA -> 0000
		 * GPIOB -> 0001
		 * GPIOC -> 0010
		 * etc.
		 */

		uint8_t portcode =
				GPIO_BASEADDR_TO_CODE(pGPIOHandle->pGPIOx);

		/* Enable SYSCFG peripheral clock */
		SYSCFG_PCLK_EN();

		/* Clear existing EXTI port selection */
		SYSCFG->EXTICR[temp1] &=
				~(0xFU << (4U * temp2));

		/* Set new GPIO port selection */
		SYSCFG->EXTICR[temp1] |=
				(portcode << (4U * temp2));

		/* =====================================================
		 * 3. Unmask EXTI interrupt line
		 * ===================================================== */

		EXTI->IMR |=
				(1U << pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber);
	}

//	temp = 0U;

	/* ============================================================
	 * 4. Configure output speed
	 * ============================================================ */

	temp = (pGPIOHandle->GPIO_PinConfig.GPIO_PinSpeed
			<< (2U * pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber));

	pGPIOHandle->pGPIOx->OSPEEDR &=
			~(0x3U << (2U * pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber));

	pGPIOHandle->pGPIOx->OSPEEDR |= temp;

//	temp = 0U;

	/* ============================================================
	 * 5. Configure Pull-up / Pull-down settings
	 * ============================================================ */

	temp = (pGPIOHandle->GPIO_PinConfig.GPIO_PinPuPdControl
			<< (2U * pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber));

	pGPIOHandle->pGPIOx->PUPDR &=
			~(0x3U << (2U * pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber));

	pGPIOHandle->pGPIOx->PUPDR |= temp;

//	temp = 0U;

	/* ============================================================
	 * 6. Configure Output Type
	 * ============================================================ */

	/*
	 * Push-Pull  -> 0
	 * Open-Drain -> 1
	 */

	temp = (pGPIOHandle->GPIO_PinConfig.GPIO_PinOPType
			<< pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber);

	pGPIOHandle->pGPIOx->OTYPER &=
			~(0x1U << pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber);

	pGPIOHandle->pGPIOx->OTYPER |= temp;

//	temp = 0U;

	/* ============================================================
	 * 7. Configure Alternate Function
	 * ============================================================ */

	if(pGPIOHandle->GPIO_PinConfig.GPIO_PinMode == GPIO_MODE_ALTFN)
	{
		uint8_t temp1, temp2;

		/*
		 * AFR[0] -> pins 0 to 7
		 * AFR[1] -> pins 8 to 15
		 */

		temp1 =
				pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber / 8U;

		/*
		 * Exact pin field inside AFR register
		 */

		temp2 =
				pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber % 8;

		/* Clear existing alternate function */
		pGPIOHandle->pGPIOx->AFR[temp1] &=
				~(0xFU << (4U * temp2));

		/* Set new alternate function */
		pGPIOHandle->pGPIOx->AFR[temp1] |=
				(pGPIOHandle->GPIO_PinConfig.GPIO_PinAltFunMode
				<< (4U * temp2));
	}
}



/*********************************************************************
 * @fn      		  - GPIO_DeInit
 *
 * @brief             - De-initializes the selected GPIO peripheral
 * 						and resets all GPIO register configurations
 * 						back to their default reset values.
 *
 * @param[in]         - pGPIOx : Base address of the GPIO peripheral
 *                                (GPIOA to GPIOH)
 *
 * @return            - none
 *
 * @Note              -
 * 						1. This function resets the entire GPIO port,
 * 						   NOT a single pin.
 *
 * 						2. All configuration registers of the selected
 * 						   GPIO peripheral are restored to hardware
 * 						   reset state.
 *
 * 						3. Internally this function uses RCC reset
 * 						   control registers:
 *
 * 						   RCC_AHB1RSTR
 *
 * 						   The corresponding GPIO reset bit is:
 *
 * 						   - Set   -> Peripheral enters reset state
 * 						   - Clear -> Peripheral comes out of reset
 *
 * 						4. Resetting the GPIO peripheral clears:
 *
 * 						   - MODER
 * 						   - OTYPER
 * 						   - OSPEEDR
 * 						   - PUPDR
 * 						   - AFR[0]
 * 						   - AFR[1]
 * 						   - ODR
 * 						   etc.
 *
 * 						5. After reset:
 *
 * 						   - Pins return to default state
 * 						   - Alternate functions are removed
 * 						   - Output configurations are lost
 * 						   - Interrupt-related GPIO configuration
 * 						     is removed from GPIO registers
 *
 * 						6. EXTI controller configuration itself is
 * 						   NOT completely reset by this function,
 * 						   because EXTI belongs to a separate
 * 						   peripheral block.
 *
 * 						7. Peripheral clock may still remain enabled
 * 						   after reset depending on RCC logic.
 *
 * 						8. Typical use cases:
 *
 * 						   - Reinitializing GPIO
 * 						   - Recovering from bad configuration
 * 						   - Peripheral shutdown
 * 						   - Returning hardware to safe state
 *
 * 						9. Important:
 *
 * 						   Since the entire GPIO port is reset,
 * 						   all pins of that port are affected.
 *
 * 						   Example:
 * 						   GPIO_DeInit(GPIOA);
 *
 * 						   -> Resets PA0 to PA15 together.
 *
 */
void GPIO_DeInit(GPIO_RegDef_t *pGPIOx)
{

 	if(pGPIOx == GPIOA){

 		/* Reset GPIOA peripheral registers */
 		GPIOA_REG_RESET();

 	}else if(pGPIOx == GPIOB){

 		/* Reset GPIOB peripheral registers */
 		GPIOB_REG_RESET();

 	}else if(pGPIOx == GPIOC){

 		/* Reset GPIOC peripheral registers */
 		GPIOC_REG_RESET();

 	}else if(pGPIOx == GPIOD){

 		/* Reset GPIOD peripheral registers */
 		GPIOD_REG_RESET();

 	}else if(pGPIOx == GPIOE){

 		/* Reset GPIOE peripheral registers */
 		GPIOE_REG_RESET();

 	}else if(pGPIOx == GPIOF){

 		/* Reset GPIOF peripheral registers */
 		GPIOF_REG_RESET();

 	}else if(pGPIOx == GPIOG){

 		/* Reset GPIOG peripheral registers */
 		GPIOG_REG_RESET();

 	}else if(pGPIOx == GPIOH){

 		/* Reset GPIOH peripheral registers */
 		GPIOH_REG_RESET();
 	}
}



/*********************************************************************
 * @fn      		  - GPIO_ReadFromInputPin
 *
 * @brief             - Reads the logic level present on a specific
 * 						GPIO input pin.
 *
 * @param[in]         - pGPIOx    : Base address of GPIO peripheral
 * @param[in]         - PinNumber : GPIO pin number (0 to 15)
 *
 * @return            - uint8_t
 * 						0 -> GPIO_PIN_RESET / Logic LOW
 * 						1 -> GPIO_PIN_SET   / Logic HIGH
 *
 * @Note              -
 * 						1. This function reads the pin state from the
 * 						   Input Data Register (IDR).
 *
 * 						2. Each GPIO pin corresponds to one bit inside
 * 						   the IDR register:
 *
 * 						   IDR bit0  -> Pin 0
 * 						   IDR bit1  -> Pin 1
 * 						   ...
 * 						   IDR bit15 -> Pin 15
 *
 * 						3. To extract a single pin value:
 *
 * 						   a. Shift the target pin bit to bit0 (LSB)
 * 						   b. Mask remaining bits using AND operation
 *
 * 						4. Operation performed internally:
 *
 * 						   value = ((IDR >> PinNumber) & 0x1)
 *
 * 						5. Example:
 *
 * 						   Suppose:
 *
 * 						   IDR = 0b00100000
 *
 * 						   Reading Pin 5:
 *
 * 						   Step 1:
 * 						   IDR >> 5
 *
 * 						   0b00100000 >> 5
 * 						   = 0b00000001
 *
 * 						   Step 2:
 * 						   0b00000001 & 0x1
 * 						   = 1
 *
 * 						   Therefore Pin 5 is HIGH.
 *
 * 						6. Masking with 0x1 ensures only the least
 * 						   significant bit is preserved while all
 * 						   remaining bits become zero.
 *
 * 						7. This API is commonly used for:
 *
 * 						   - Push button reading
 * 						   - Sensor input monitoring
 * 						   - Digital signal sampling
 * 						   - Status pin monitoring
 *
 * 						8. Pin should generally be configured in:
 *
 * 						   - Input mode
 * 						   - Interrupt mode
 * 						   - Alternate function input mode
 *
 * 						9. Even if pin is configured as output,
 * 						   IDR still reflects the actual voltage
 * 						   level present on the pin.
 *
 * 						10. This API reads only ONE pin.
 * 						    To read the entire GPIO port use:
 *
 * 						    GPIO_ReadFromInputPort()
 *
 */
uint8_t GPIO_ReadFromInputPin(GPIO_RegDef_t *pGPIOx,
 							  uint8_t PinNumber)
{
 	uint8_t value;

 	/*
 	 * Step 1:
 	 * Shift target pin bit to LSB position
 	 *
 	 * Step 2:
 	 * Mask all remaining bits except bit0
 	 */

 	value = (uint8_t)((pGPIOx->IDR >> PinNumber) & 0x00000001U);

 	return value;
}



/*********************************************************************
 * @fn      		  - GPIO_ReadFromInputPort
 *
 * @brief             - Reads the complete 16-bit GPIO input port value
 * 						from the Input Data Register (IDR).
 *
 * @param[in]         - pGPIOx : Base address of GPIO peripheral
 *
 * @return            - uint16_t
 * 						Returns the current logic state of all
 * 						GPIO pins of the selected port.
 *
 * @Note              -
 * 						1. This function reads the entire GPIO
 * 						   Input Data Register (IDR).
 *
 * 						2. Each bit in IDR represents one GPIO pin:
 *
 * 						   Bit0  -> Pin 0
 * 						   Bit1  -> Pin 1
 * 						   ...
 * 						   Bit15 -> Pin 15
 *
 * 						3. Unlike GPIO_ReadFromInputPin(),
 * 						   this API does NOT isolate a single pin.
 *
 * 						4. No shifting or masking is required because
 * 						   the complete register value is returned.
 *
 * 						5. Example:
 *
 * 						   Suppose:
 *
 * 						   IDR = 0b0000000000100101
 *
 * 						   Then:
 *
 * 						   Pin0 = HIGH
 * 						   Pin2 = HIGH
 * 						   Pin5 = HIGH
 *
 * 						   Remaining pins = LOW
 *
 * 						6. This API is useful when multiple GPIO
 * 						   pins must be sampled simultaneously.
 *
 * 						7. Common use cases:
 *
 * 						   - Reading DIP switches
 * 						   - Reading keypad rows/columns
 * 						   - Parallel data input
 * 						   - Fast GPIO sampling
 * 						   - Debugging GPIO states
 *
 * 						8. Pins should generally be configured as:
 *
 * 						   - Input mode
 * 						   - Interrupt mode
 * 						   - Alternate function input mode
 *
 * 						9. Even output-configured pins can still
 * 						   be observed through IDR because IDR
 * 						   reflects the actual logic level present
 * 						   on the physical pin.
 *
 * 						10. Only lower 16 bits are meaningful since
 * 						    STM32 GPIO ports contain maximum 16 pins.
 *
 */
uint16_t GPIO_ReadFromInputPort(GPIO_RegDef_t *pGPIOx)
{
	uint16_t value;

	/*
	 * Directly read complete Input Data Register (IDR)
	 * without masking or shifting.
	 */

	value = (uint16_t)pGPIOx->IDR;

	return value;
}



/*********************************************************************
 * @fn      		  - GPIO_WriteToOutputPin
 *
 * @brief             - Writes a logic value to a specific GPIO output
 * 						pin using the Output Data Register (ODR).
 *
 * @param[in]         - pGPIOx    : Base address of GPIO peripheral
 * @param[in]         - PinNumber : GPIO pin number (0 to 15)
 * @param[in]         - Value     : GPIO_PIN_SET   -> Logic HIGH
 * 								   GPIO_PIN_RESET -> Logic LOW
 *
 * @return            - none
 *
 * @Note              -
 * 						1. This function modifies only ONE GPIO pin
 * 						   inside the Output Data Register (ODR).
 *
 * 						2. Each GPIO pin corresponds to one bit
 * 						   position inside ODR:
 *
 * 						   ODR bit0  -> Pin 0
 * 						   ODR bit1  -> Pin 1
 * 						   ...
 * 						   ODR bit15 -> Pin 15
 *
 * 						3. To SET a pin:
 *
 * 						   - Write logic 1 into corresponding bit
 * 						   - Performed using bitwise OR operation
 *
 * 						4. To RESET a pin:
 *
 * 						   - Write logic 0 into corresponding bit
 * 						   - Performed using bitwise AND with
 * 						     inverted mask
 *
 * 						5. Internal operations:
 *
 * 						   SET:
 * 						   ODR |= (1 << PinNumber)
 *
 * 						   RESET:
 * 						   ODR &= ~(1 << PinNumber)
 *
 * 						6. Example:
 *
 * 						   Suppose:
 *
 * 						   PinNumber = 5
 *
 * 						   Then:
 *
 * 						   (1 << 5)
 *
 * 						   = 0b00100000
 *
 * 						   SET operation:
 *
 * 						   ODR |= 0b00100000
 *
 * 						   RESET operation:
 *
 * 						   ODR &= ~(0b00100000)
 *
 * 						7. Pin should be configured in OUTPUT mode
 * 						   before using this API.
 *
 * 						8. Common use cases:
 *
 * 						   - LED control
 * 						   - Chip Select (CS/SS) control
 * 						   - Relay control
 * 						   - Enable/Reset signal control
 * 						   - General digital output
 *
 * 						9. This API performs Read-Modify-Write
 * 						   operation on ODR register.
 *
 * 						10. For atomic GPIO operations,
 * 						    STM32 provides BSRR register which is
 * 						    generally preferred in interrupt or
 * 						    multi-context environments.
 *
 * 						11. Since ODR is modified directly,
 * 						    concurrent access from interrupt and
 * 						    main application may cause race
 * 						    conditions if not handled properly.
 *
 */
void GPIO_WriteToOutputPin(GPIO_RegDef_t *pGPIOx,
						   uint8_t PinNumber,
						   uint8_t Value)
{
	if(Value == GPIO_PIN_SET)
	{
		/*
		 * Write logic HIGH to selected pin
		 * by setting corresponding ODR bit.
		 */

		pGPIOx->ODR |= (1U << PinNumber);
	}
	else
	{
		/*
		 * Write logic LOW to selected pin
		 * by clearing corresponding ODR bit.
		 */

		pGPIOx->ODR &= ~(1U << PinNumber);
	}
}



/*********************************************************************
 * @fn      		  - GPIO_WriteToOutputPort
 *
 * @brief             - Writes a 16-bit value to the complete GPIO
 * 						output port using the Output Data Register
 * 						(ODR).
 *
 * @param[in]         - pGPIOx : Base address of GPIO peripheral
 * @param[in]         - Value  : 16-bit data to be written to ODR
 *
 * @return            - none
 *
 * @Note              -
 * 						1. This function writes directly to the
 * 						   entire GPIO Output Data Register (ODR).
 *
 * 						2. Each bit of the Value corresponds to
 * 						   one GPIO pin:
 *
 * 						   Bit0  -> Pin 0
 * 						   Bit1  -> Pin 1
 * 						   ...
 * 						   Bit15 -> Pin 15
 *
 * 						3. Writing logic:
 *
 * 						   Bit = 1 -> GPIO pin becomes HIGH
 * 						   Bit = 0 -> GPIO pin becomes LOW
 *
 * 						4. Example:
 *
 * 						   Value = 0x0025
 *
 * 						   Binary:
 *
 * 						   0b0000000000100101
 *
 * 						   Result:
 *
 * 						   Pin0 = HIGH
 * 						   Pin2 = HIGH
 * 						   Pin5 = HIGH
 *
 * 						   Remaining pins = LOW
 *
 * 						5. Unlike GPIO_WriteToOutputPin(),
 * 						   this API updates all 16 GPIO pins
 * 						   simultaneously.
 *
 * 						6. This is useful for:
 *
 * 						   - Parallel data output
 * 						   - LCD data buses
 * 						   - LED matrix control
 * 						   - Fast GPIO updates
 * 						   - Multiple pin synchronization
 *
 * 						7. Pins should be configured in OUTPUT
 * 						   mode before using this API.
 *
 * 						8. Since the complete ODR register is
 * 						   overwritten, previous states of all
 * 						   pins will be lost.
 *
 * 						9. Use carefully if some GPIO pins of the
 * 						   same port are used for other purposes.
 *
 * 						10. This operation is faster than writing
 * 						    pins individually because only one
 * 						    register write occurs.
 *
 * 						11. This API performs direct assignment:
 *
 * 						    ODR = Value
 *
 * 						12. Only lower 16 bits are meaningful
 * 						    because STM32 GPIO ports contain
 * 						    maximum 16 pins.
 *
 */
void GPIO_WriteToOutputPort(GPIO_RegDef_t *pGPIOx,
							uint16_t Value)
{
	/*
	 * Copy complete 16-bit value into
	 * Output Data Register (ODR).
	 */

	pGPIOx->ODR = Value;
}



/*********************************************************************
 * @fn      		  - GPIO_ToggleOutputPin
 *
 * @brief             - Toggles the current logic state of a specific
 * 						GPIO output pin.
 *
 * @param[in]         - pGPIOx    : Base address of GPIO peripheral
 * @param[in]         - PinNumber : GPIO pin number (0 to 15)
 *
 * @return            - none
 *
 * @Note              -
 * 						1. This function changes the current state
 * 						   of the selected GPIO pin:
 *
 * 						   HIGH -> LOW
 * 						   LOW  -> HIGH
 *
 * 						2. Toggling is performed using Bitwise XOR
 * 						   operation on the Output Data Register
 * 						   (ODR).
 *
 * 						3. XOR operation behavior:
 *
 * 						   Bit ^ 1 -> Toggles the bit
 * 						   Bit ^ 0 -> No change
 *
 * 						4. Internal operation:
 *
 * 						   ODR ^= (1 << PinNumber)
 *
 * 						5. Example:
 *
 * 						   Assume:
 *
 * 						   PinNumber = 3
 *
 * 						   Mask:
 *
 * 						   (1 << 3)
 * 						   = 0b00001000
 *
 * 						   If current ODR:
 *
 * 						   0b00001000
 *
 * 						   After XOR:
 *
 * 						   0b00000000
 *
 * 						   Pin3 changes from HIGH to LOW.
 *
 * 						6. Similarly:
 *
 * 						   0b00000000 ^ 0b00001000
 * 						   = 0b00001000
 *
 * 						   Pin3 changes from LOW to HIGH.
 *
 * 						7. Pin should be configured in OUTPUT
 * 						   mode before using this API.
 *
 * 						8. Common use cases:
 *
 * 						   - LED blinking
 * 						   - Heartbeat indicators
 * 						   - Signal generation
 * 						   - Debug pin toggling
 * 						   - Timing measurements
 *
 * 						9. This API performs Read-Modify-Write
 * 						   operation on ODR register.
 *
 * 						10. Since ODR is directly modified,
 * 						    concurrent access from interrupts
 * 						    or multiple execution contexts may
 * 						    cause race conditions.
 *
 * 						11. Toggle operation is efficient because
 * 						    it does not require checking current
 * 						    pin state before modification.
 *
 */
void GPIO_ToggleOutputPin(GPIO_RegDef_t *pGPIOx,
						  uint8_t PinNumber)
{
	/*
	 * Toggle the selected GPIO pin
	 * using Bitwise XOR operation.
	 */

	pGPIOx->ODR ^= (1U << PinNumber);
}



/*********************************************************************
 * @fn      		  - GPIO_IRQInterruptConfig
 *
 * @brief             - Enables or disables IRQ interrupt delivery
 * 						from NVIC for a given IRQ number.
 *
 * @param[in]         - IRQNumber : IRQ number associated with the
 * 								   peripheral or EXTI line
 *
 * @param[in]         - EnorDi    : ENABLE  -> Enable IRQ
 * 								   DISABLE -> Disable IRQ
 *
 * @return            - none
 *
 * @Note              -
 * 						1. This function configures NVIC
 * 						   (Nested Vectored Interrupt Controller)
 * 						   registers.
 *
 * 						2. NVIC is a Cortex-M processor feature,
 * 						   therefore this configuration is:
 *
 * 						   Processor Specific
 * 						   NOT MCU Peripheral Specific
 *
 * 						3. Interrupt enable registers:
 *
 * 						   ISER0 -> IRQ   0  to 31
 * 						   ISER1 -> IRQ  32 to 63
 * 						   ISER2 -> IRQ  64 to 95
 *
 * 						4. Interrupt clear/disable registers:
 *
 * 						   ICER0 -> IRQ   0  to 31
 * 						   ICER1 -> IRQ  32 to 63
 * 						   ICER2 -> IRQ  64 to 95
 *
 * 						5. Example:
 *
 * 						   IRQNumber = 35
 *
 * 						   Since:
 *
 * 						   35 > 31 and < 64
 *
 * 						   Therefore:
 *
 * 						   ISER1/ICER1 will be used.
 *
 * 						   Exact bit position:
 *
 * 						   35 % 32 = 3
 *
 * 						   So Bit3 of ISER1/ICER1
 * 						   corresponds to IRQ35.
 *
 * 						6. Enabling interrupt:
 *
 * 						   Writing logic 1 to ISER bit
 * 						   enables the interrupt.
 *
 * 						7. Disabling interrupt:
 *
 * 						   Writing logic 1 to ICER bit
 * 						   disables the interrupt.
 *
 * 						8. Important:
 *
 * 						   ICER registers are Write-Only control
 * 						   registers for interrupt disabling.
 *
 * 						   Therefore:
 *
 * 						   ICER = (1 << IRQNumber)
 *
 * 						   is preferred instead of:
 *
 * 						   ICER |= (1 << IRQNumber)
 *
 * 						   because Read-Modify-Write operation
 * 						   is unnecessary.
 *
 * 						9. ICER register behavior:
 *
 * 						   Write:
 * 						   0 -> No effect
 * 						   1 -> Disable interrupt
 *
 * 						   Read:
 * 						   0 -> Interrupt disabled
 * 						   1 -> Interrupt enabled
 *
 * 						10. This API only enables/disables
 * 						    interrupt delivery from NVIC.
 *
 * 						    Peripheral interrupt generation
 * 						    must still be configured separately.
 *
 * 						11. Common use cases:
 *
 * 						    - GPIO EXTI interrupts
 * 						    - SPI interrupts
 * 						    - UART interrupts
 * 						    - Timer interrupts
 * 						    - DMA interrupts
 *
 */
void GPIO_IRQInterruptConfig(uint8_t IRQNumber,
							 uint8_t EnorDi)
{
	if(EnorDi == ENABLE)
	{
		/*
		 * Enable IRQ in NVIC
		 */

		if(IRQNumber <= 31U)
		{
			/* IRQ0 to IRQ31 -> ISER0 */

			*NVIC_ISER0 |= (1U << IRQNumber);
		}
		else if(IRQNumber > 31U && IRQNumber < 64U)
		{
			/* IRQ32 to IRQ63 -> ISER1 */

			*NVIC_ISER1 |= (1U << (IRQNumber % 32U));
		}
		else if(IRQNumber > 63U && IRQNumber < 96U)
		{
			/* IRQ64 to IRQ95 -> ISER2 */

			*NVIC_ISER2 |= (1U << (IRQNumber % (32U * 2U)));
		}
	}
	else
	{
		/*
		 * Disable IRQ in NVIC
		 */

		if(IRQNumber <= 31U)
		{
			/* IRQ0 to IRQ31 -> ICER0 */

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
			/* IRQ32 to IRQ63 -> ICER1 */

			*NVIC_ICER1 = (1U << (IRQNumber % 32U));
		}
		else if(IRQNumber > 63U && IRQNumber < 96U)
		{
			/* IRQ64 to IRQ95 -> ICER2 */

			*NVIC_ICER2 = (1U << (IRQNumber % (32U * 2U)));
		}
	}
}




 /*********************************************************************
  * @fn      		  - GPIO_IRQPriorityConfig
  *
  * @brief             - Configures the priority level for a given IRQ
  * 						in the NVIC priority registers.
  *
  * @param[in]         - IRQNumber   : IRQ number associated with the
  * 									peripheral or EXTI line
  *
  * @param[in]         - IRQPriority : Priority value to be assigned
  * 									to the IRQ
  *
  * @return            - none
  *
  * @Note              -
  * 						1. This function configures interrupt priority
  * 						   inside NVIC (Nested Vectored Interrupt
  * 						   Controller).
  *
  * 						2. NVIC is a Cortex-M processor feature,
  * 						   therefore this configuration is:
  *
  * 						   Processor Specific
  * 						   NOT MCU Peripheral Specific
  *
  * 						3. Each NVIC priority register (IPR)
  * 						   is 32-bit wide and contains priority
  * 						   configuration for 4 IRQs.
  *
  * 						4. Each IRQ uses 8 bits inside the IPR
  * 						   register:
  *
  * 						   ---------------------------------
  * 						   | IRQ0 | IRQ1 | IRQ2 | IRQ3 |
  * 						   ---------------------------------
  * 						      8b     8b     8b     8b
  *
  * 						5. IRQ register selection:
  *
  * 						   iprx = IRQNumber / 4
  *
  * 						   determines which IPR register
  * 						   contains the required IRQ.
  *
  * 						6. IRQ section selection:
  *
  * 						   iprx_section = IRQNumber % 4
  *
  * 						   determines which 8-bit field
  * 						   inside the selected IPR register
  * 						   belongs to the IRQ.
  *
  * 						7. Example:
  *
  * 						   IRQNumber = 6
  *
  * 						   iprx = 6 / 4 = 1
  * 						   -> IPR1 register
  *
  * 						   iprx_section = 6 % 4 = 2
  * 						   -> 3rd priority field inside IPR1
  *
  * 						8. Important STM32F4 detail:
  *
  * 						   Although each priority field is 8 bits,
  * 						   STM32 implements only upper 4 bits.
  *
  * 						   Lower 4 bits are NOT implemented.
  *
  * 						9. Example:
  *
  * 						   Implemented bits:
  *
  * 						   [7 6 5 4]
  *
  * 						   Not implemented:
  *
  * 						   [3 2 1 0]
  *
  * 						10. Therefore priority value must be
  * 						    shifted to upper nibble.
  *
  * 						11. Shift calculation:
  *
  * 						    shift_amount =
  * 						    (8 * iprx_section)
  * 						    + (8 - NO_PR_BITS_IMPLEMENTED)
  *
  * 						12. For STM32:
  *
  * 						    NO_PR_BITS_IMPLEMENTED = 4
  *
  * 						13. Example:
  *
  * 						    IRQPriority = 5
  *
  * 						    Binary:
  *
  * 						    0b0101
  *
  * 						    After shifting:
  *
  * 						    0b01010000
  *
  * 						14. Lower numerical priority value
  * 						    means HIGHER interrupt priority.
  *
  * 						    Example:
  *
  * 						    Priority 0 -> Highest priority
  * 						    Priority 15 -> Lowest priority
  *
  * 						15. This API only configures NVIC
  * 						    priority.
  *
  * 						    Interrupt generation and enabling
  * 						    must still be configured separately.
  *
  * 						16. Common use cases:
  *
  * 						    - Prioritizing critical interrupts
  * 						    - Real-time systems
  * 						    - Nested interrupt handling
  * 						    - Sensor timing applications
  * 						    - Communication peripherals
  *
  */
 void GPIO_IRQPriorityConfig(uint8_t IRQNumber,
 							uint32_t IRQPriority)
 {
 	/*
 	 * 1. Find the IPR register index
 	 */
 	uint8_t iprx = IRQNumber / 4U;

 	/*
 	 * 2. Find the IRQ section inside the IPR register
 	 */
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

 	/*
 	 * 4. Configure IRQ priority
 	 */
 	*(NVIC_IPR_BASE_ADDR + iprx) |=
 			(IRQPriority << shift_amount);
 }



  /*********************************************************************
   * @fn      		  - GPIO_IRQHandling
   *
   * @brief             - Handles GPIO interrupt generated through
   * 						EXTI lines by clearing the pending bit.
   *
   * @param[in]         - PinNumber : GPIO pin number associated
   * 									with the EXTI line
   *
   * @return            - none
   *
   * @Note              -
   * 						1. GPIO interrupts in STM32 are handled
   * 						   through the EXTI (External Interrupt)
   * 						   peripheral.
   *
   * 						2. When an interrupt event occurs on a GPIO
   * 						   pin configured in interrupt mode, the
   * 						   corresponding EXTI Pending Register (PR)
   * 						   bit becomes SET.
   *
   * 						3. As long as the PR bit remains set,
   * 						   the interrupt remains pending.
   *
   * 						4. Therefore, the interrupt handler must
   * 						   clear the pending bit before exiting
   * 						   the ISR.
   *
   * 						5. EXTI pending bits are cleared by
   * 						   writing logic '1' to the corresponding
   * 						   PR bit position.
   *
   * 						6. Writing '0' has no effect.
   *
   * 						7. Example:
   *
   * 						   If EXTI line 5 generated interrupt:
   *
   * 						   EXTI->PR bit 5 = 1
   *
   * 						   To clear:
   *
   * 						   EXTI->PR |= (1 << 5);
   *
   * 						8. This function only clears the interrupt
   * 						   pending flag.
   *
   * 						9. User-specific interrupt processing
   * 						   such as:
   *
   * 						   - Button handling
   * 						   - Sensor event handling
   * 						   - State changes
   * 						   - Callback execution
   *
   * 						   should be implemented separately
   * 						   inside the actual ISR.
   *
   * 						10. This API is processor dependent because
   * 						    EXTI interrupt handling is tightly
   * 						    coupled with NVIC and Cortex-M interrupt
   * 						    architecture.
   *
   * 						11. Common use cases:
   *
   * 						    - Push button interrupts
   * 						    - Wake-up events
   * 						    - Sensor edge detection
   * 						    - External signal monitoring
   *
   */
  void GPIO_IRQHandling(uint8_t PinNumber)
  {
  	/*
  	 * Clear the EXTI pending register bit
  	 * corresponding to the given pin number
  	 */
  	if(EXTI->PR & (1U << PinNumber))
  	{
  		/*
  		 * Writing '1' clears the pending bit
  		 */
  		EXTI->PR |= (1U << PinNumber);
  	}
  }
