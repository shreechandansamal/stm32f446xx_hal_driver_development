/*
 * File Name: stm32f446xx_gpio_driver.c
 * Description: GPIO Driver Specific Header File
 * Created on: 26-Feb-2026
 * Author: chandan
 */




#include "stm32f446xx_gpio_driver.h"
#include <assert.h>

/*
 * Peripheral Clock setup
 */
/*********************************************************************
 * @fn      		  - GPIO_PeriClockControl
 *
 * @brief             - This function enables or disables peripheral clock for the given GPIO port
 *
 * @param[in]         - base address of the gpio peripheral
 * @param[in]         - ENABLE or DISABLE macros
 *
 * @return            -  none
 *
 * @Note              -  none

 */
void GPIO_PeriClockControl(GPIO_RegDef_t *pGPIOx, uint8_t EnorDi){

	uint8_t code = GPIO_BASEADDR_TO_CODE(pGPIOx);

	assert(code < GPIO_PORT_COUNT);

	if(EnorDi == ENABLE)
	    RCC->AHB1ENR |= (1U << code);
	else
	    RCC->AHB1ENR &= ~(1U << code);
}

/*below is too lengthy if-else ladder  */
//void GPIO_PeriClockControl(GPIO_RegDef_t *pGPIOx, uint8_t EnorDi){
//
//	if(EnorDi == ENABLE){
//		if(pGPIOx == GPIOA){
//			GPIOA_PCLK_EN();
//		}else if(pGPIOx == GPIOB){
//			GPIOB_PCLK_EN();
//		}else if(pGPIOx == GPIOC){
//		    GPIOC_PCLK_EN();
//	    }else if(pGPIOx == GPIOD){
//			GPIOD_PCLK_EN();
//		}else if(pGPIOx == GPIOE){
//			GPIOE_PCLK_EN();
//		}else if(pGPIOx == GPIOH){
//			GPIOH_PCLK_EN();
//		}
//	}else{
//		if(pGPIOx == GPIOA){
//			GPIOA_PCLK_DI();
//		}else if(pGPIOx == GPIOB){
//			GPIOB_PCLK_DI();
//		}else if(pGPIOx == GPIOC){
//		    GPIOC_PCLK_DI();
//	    }else if(pGPIOx == GPIOD){
//			GPIOD_PCLK_DI();
//		}else if(pGPIOx == GPIOE){
//			GPIOE_PCLK_DI();
//		}else if(pGPIOx == GPIOH){
//			GPIOH_PCLK_DI();
//		}
//	}
//}




/*
 * Init and De-Init
 */
/*********************************************************************
 * @fn      		  - GPIO_Init
 *
 * @brief             - It will Initialize the GPIO port
 *
 * @param[in]         - address of  GPIO_Handle_t object base address which hold all the pin configuration
 *
 * @return            - none
 *
 * @Note              - none

 */
void GPIO_Init(GPIO_Handle_t *pGPIOHandle){

	uint32_t temp = 0;

	/*1. Configure the mode of gpio pin*/
	if(pGPIOHandle->GPIO_PinConfig.GPIO_PinMode <= GPIO_MODE_ANALOG){  /* Remember " -> " symbol means de-reference pointer to get the pin mode given by the user, if
	                                                                                it is less than 3, means non-interrupt mode and if greater then interrupt mode */
		 /*non-interrupt mode*/
	     temp = (pGPIOHandle->GPIO_PinConfig.GPIO_PinMode << (2U * pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber)); /* we have to place the mode setting into appropriate
		                                                                                                           pin position and as the port mode reg. each pin
		                                                                                                           needs two bit fields thats why we need to shif the
		                                                                                                           value appropriately, so we need to multiplied by 2 . */

	     pGPIOHandle->pGPIOx->MODER &= ~(0x3U << (2U * pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber)); //clearing bits
	     pGPIOHandle->pGPIOx->MODER |= temp; //setting bits

	}else{
		 /*interrupt mode*/
		 if(pGPIOHandle->GPIO_PinConfig.GPIO_PinMode == GPIO_MODE_IT_FT){
			 //a.Configure the FTSR
			 EXTI->FTSR |= (1U << pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber);
			 //Clear the corresponding RTSR bit
			 EXTI->RTSR &= ~(1U << pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber);

		 }else if(pGPIOHandle->GPIO_PinConfig.GPIO_PinMode == GPIO_MODE_IT_RT){
			 //b.Configure the RTSR
			 EXTI->RTSR |= (1U << pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber);
			 //Clear the corresponding FTSR bit
			 EXTI->FTSR &= ~(1U << pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber);

		 }else if(pGPIOHandle->GPIO_PinConfig.GPIO_PinMode == GPIO_MODE_IT_RFT){
			 //c.Configure both FTSR and RTSR
			 EXTI->RTSR |= (1U << pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber);
			 EXTI->FTSR |= (1U << pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber);
		 }

		 //2.Configure the port selection in SYSCFG_EXTICR
		 uint8_t temp1 = pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber / 4; /* (@PinNumber / 4), Quotient will gives SYSCFG_EXTICR number[0-3]
		  	  	  	  	  	  	  	  	  	  	  	  	  	  	  	  	  	 if it is 0 means SYSCFG_EXTICR1, 1 means SYSCFG_EXTICR1,..,3 means SYSCFG_EXTICR4*/

		 uint8_t temp2 = pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber % 4; /* (@PinNumber % 4), Remainder will gives EXTI number[0-3] inside selected SYSCFG_EXTICR number
		  	  	  	  	  	  	  	  	  	  	  	  	  	  	  	  	  	 4 EXTI are presented inside 4 SYSCFG_EXTICR, which means total 16 EXTI
		  	  	  	  	  	  	  	  	  	  	  	  	  	  	  	  	  	 Ex- if the SYSCFG_EXTICR is 1 which means [SYSCFG_EXTICR2] and inise that if
		  	  	  	  	  	  	  	  	  	  	  	  	  	  	  	  	  	 (@PinNumber % 4) results 1 it means you have selected EXTI5*/

		 uint8_t portcode = GPIO_BASEADDR_TO_CODE(pGPIOHandle->pGPIOx);		/*here we can use the code for configuring the port like example 0000 means PORT A,
		  	  	  	  	  	  	  	  	  	  	  	  	  	  	  	  	  	  0001 means PORT B ,etc*/

		 SYSCFG_PCLK_EN(); //before configuring the SYSCFG register first you have to enable the clock for it!

		 SYSCFG->EXTICR[temp1] &= ~(0xFU << (4U * temp2)); // clear before set
		 SYSCFG->EXTICR[temp1] |= portcode << (4U * temp2); // set new port

		 //3.Enable the EXTI interrupt delivery using IMR(Interrupt Mask Register)
		 EXTI->IMR |= (1U << pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber);
	}
	temp = 0;

	/*2. Configure the speed*/
	temp = (pGPIOHandle->GPIO_PinConfig.GPIO_PinSpeed << (2U * pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber));
	pGPIOHandle->pGPIOx->OSPEEDR &= ~(0x3U << (2U * pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber)); //clearing bits
	pGPIOHandle->pGPIOx->OSPEEDR |= temp;//setting bits
	temp = 0;

	/*3. Configure the pupd settings*/
	temp = (pGPIOHandle->GPIO_PinConfig.GPIO_PinPuPdControl << (2U * pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber));
	pGPIOHandle->pGPIOx->PUPDR &= ~(0x3U << (2U * pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber)); //clearing bits
	pGPIOHandle->pGPIOx->PUPDR |= temp;//setting bits
	temp = 0;

	/*4. Configure the optype*/
	temp = (pGPIOHandle->GPIO_PinConfig.GPIO_PinOPType << (pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber));
	pGPIOHandle->pGPIOx->OTYPER &= ~(0x1U << pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber); //clearing bits
	pGPIOHandle->pGPIOx->OTYPER |= temp;//setting bits
	temp = 0;

	/*5. Configure the alt functionality (if required)*/
	if(pGPIOHandle->GPIO_PinConfig.GPIO_PinMode == GPIO_MODE_ALTFN){
		/*configure the alt function registers*/
		uint8_t temp1,temp2;

		temp1 = pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber / 8;
		temp2 = pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber % 8;
		pGPIOHandle->pGPIOx->AFR[temp1] &= ~(0xFU << (4U * temp2)); //clearing bits
		pGPIOHandle->pGPIOx->AFR[temp1] |= (pGPIOHandle->GPIO_PinConfig.GPIO_PinAltFunMode << (4U * temp2)); // setting bits, multiplied by 4 bcs each pin uses 4 bits.

	}

}

/*********************************************************************
 * @fn      		  - GPIO_DeInit
 *
 * @brief             - It will de-initialize the GPIO port and it will send back the GPIO port setting to Reset state
 *
 * @param[in]         - base address of the gpio peripheral
 *
 * @return            - none
 *
 * @Note              - none

 */
void GPIO_DeInit(GPIO_RegDef_t *pGPIOx){

	        if(pGPIOx == GPIOA){
				GPIOA_REG_RESET();
			}else if(pGPIOx == GPIOB){
				GPIOB_REG_RESET();
			}else if(pGPIOx == GPIOC){
			    GPIOC_REG_RESET();
		    }else if(pGPIOx == GPIOD){
				GPIOD_REG_RESET();
			}else if(pGPIOx == GPIOE){
				GPIOE_REG_RESET();
			}else if(pGPIOx == GPIOH){
				GPIOH_REG_RESET();
			}
}



/*
 * Data read and write
 */
/*********************************************************************
 * @fn      		  - GPIO_ReadFromInputPin
 *
 * @brief             - it will the read the pin value, pin must be in input mode
 *
 * @param[in]         - base address of the gpio peripheral
 * @param[in]         - pin number
 *
 * @return            - 0 or 1
 *
 * @Note              - none

 */
uint8_t GPIO_ReadFromInputPin(GPIO_RegDef_t *pGPIOx, uint8_t PinNumber){
	uint8_t value;
	value = (uint8_t)((pGPIOx->IDR >> PinNumber) & 0x00000001U); /*shifting the IDR by @PinNumber times the IDR will shifted to its LSB or
																  0th position and after that we will do masking with taking care of LSB bit
																  so we make all other bits as 0 except the LSB bit as 1, and then return the value.*/
	return value;

}

/*********************************************************************
 * @fn      		  - GPIO_ReadFromInputPort
 *
 * @brief             - it will read the whole port
 *
 * @param[in]         - base address of the gpio peripheral
 *
 * @return            - 0 or 1
 *
 * @Note              - none

 */
uint16_t GPIO_ReadFromInputPort(GPIO_RegDef_t *pGPIOx){
	uint16_t value;
	value = (uint16_t)pGPIOx->IDR; //here we will return the whole port, so no need to do any right shift and masking
	return value;
}

/*********************************************************************
 * @fn      		  - GPIO_WriteToOutputPin
 *
 * @brief             - it will write value in the pin, pin must be in output mode
 *
 * @param[in]         - base address of the gpio peripheral
 * @param[in]         - pin number
 * @param[in]         - value
 *
 * @return            - none
 *
 * @Note              - none

 */
void GPIO_WriteToOutputPin(GPIO_RegDef_t *pGPIOx, uint8_t PinNumber, uint8_t Value){
	if(Value == GPIO_PIN_SET){
		//write 1 to the output register at the bit field corresponding @PinNumber
		pGPIOx->ODR |= (1U << PinNumber);
	}else{
		//write 0 to the output register at the bit field corresponding @PinNumber
		pGPIOx->ODR &= ~(1U << PinNumber);
	}
}

/*********************************************************************
 * @fn      		  - GPIO_WriteToOutputPort
 *
 * @brief             - it iwill write the value to the whole port
 *
 * @param[in]         - base address of the gpio peripheral
 * @param[in]         - value
 *
 * @return            - none
 *
 * @Note              - none

 */
void GPIO_WriteToOutputPort(GPIO_RegDef_t *pGPIOx, uint16_t Value){
	pGPIOx->ODR = Value;//copy the @Value into ODR register,because you actually writing into the whole port
}

/*********************************************************************
 * @fn      		  - GPIO_ToggleOutputPin
 *
 * @brief             - it will toggle the pin, pin must be in output
 *
 * @param[in]         - base address of the gpio peripheral
 * @param[in]         - pin number
 *
 * @return            - none
 *
 * @Note              - none

 */
void GPIO_ToggleOutputPin(GPIO_RegDef_t *pGPIOx, uint8_t PinNumber){
	//pGPIOx->ODR = pGPIOx->ODR ^ (1 << PinNumber);
	pGPIOx->ODR ^= (1U << PinNumber); // ^ bitwise XOR to toggle the @PinNumber(pin), ^= shorthand notation
}






/*
 * IRQ Configuration and ISR handeling
 */
/*********************************************************************
 * @fn      		  - GPIO_IRQInterruptConfig
 *
 * @brief             -
 *
 * @param[in]         - IRQ number
 * @param[in]         - Enable or Disable
 *
 * @return            - none
 *
 * @Note              - processor specific not mcu specific!!

 */
void GPIO_IRQInterruptConfig(uint8_t IRQNumber, uint8_t EnorDi){

	if(EnorDi == ENABLE){
		if(IRQNumber <= 31){ //IRQ0 to IRQ31
			//program ISER0 register
			*NVIC_ISER0 |= (1U << IRQNumber);

		} else if (IRQNumber > 31 && IRQNumber < 64){ //IRQ32 to IRQ63
			//program ISER1 register
			*NVIC_ISER1 |= (1U << IRQNumber % 32);

		} else if (IRQNumber > 63 && IRQNumber < 96){ //IRQ64 to IRQ95
			//program ISER2 register
			*NVIC_ISER2 |= (1U << IRQNumber % (32 * 2));

		}

	} else {

			if(IRQNumber <= 31){ //IRQ0 to IRQ31
				//program ICER0 register
				*NVIC_ICER0 |= (1U << IRQNumber);

			} else if (IRQNumber > 31 && IRQNumber < 64){ //IRQ32 to IRQ63
				//program ICER1 register
				*NVIC_ICER1 |= (1U << IRQNumber % 32);

			} else if (IRQNumber > 63 && IRQNumber < 96){ //IRQ64 to IRQ95
				//program ICER2 register
				*NVIC_ICER2 |= (1U << IRQNumber % (32 * 2));

			}

	}

  }


/*********************************************************************
 * @fn      		  - GPIO_IRQPriorityConfig
 *
 * @brief             -
 *
 * @param[in]         - IRQ number
 * @param[in]         - IRQ Priority
 *
 * @return            - none
 *
 * @Note              - processor specific not mcu specific!!

 */
void GPIO_IRQPriorityConfig(uint8_t IRQNumber, uint32_t IRQPriority){

	//1. first lets find out the ipr register
	uint8_t iprx = IRQNumber / 4;
	uint8_t iprx_section = IRQNumber % 4;

	uint8_t shift_amount = ((8 * iprx_section) + (8 - NO_PR_BITS_IMPLEMENTED));
	*(NVIC_IPR_BASE_ADDR + iprx) |= (IRQPriority << shift_amount);
}


/*********************************************************************
 * @fn      		  - GPIO_IRQHandling
 *
 * @brief             -
 *
 * @param[in]         - pin number
 *
 * @return            - none
 *
 * @Note              - processor specific not mcu specific!!

 */
void GPIO_IRQHandling(uint8_t PinNumber){
	//clear the EXTI PR register corresponding to the pin number
	if(EXTI->PR & (1U << PinNumber) ){//so, if the PR bit position is corresponding to this @PinNumber is set then the interrupt is really pended
		//that's why clear the pending bit, so you have to right 1 in order to clear in case of PR
		EXTI->PR |= (1U << PinNumber);
	}
}

