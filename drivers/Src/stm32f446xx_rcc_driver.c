/*
 * stm32f446xx_rcc_driver.c
 *
 *  Created on: Jun 7, 2026
 *      Author: chandan
 */

#include "stm32f446xx_rcc_driver.h"

uint16_t AHB_PreScaler[8U] = {2U, 4U, 8U, 16U, 64U, 128U, 256U, 512U};
uint8_t APB1_PreScaler[4U] = { 2U, 4U, 8U, 16U};

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
 *
 */
uint32_t  RCC_GetPLLOutputClock()
{
	return 0;
}



/*********************************************************************
 * @fn                - RCC_GetPCLK1Value
 *
 * @brief             - Calculate the current APB1 peripheral clock
 *                      frequency.
 *
 * @param[in]         - none
 *
 * @return            - APB1 peripheral clock frequency in Hertz (Hz)
 *
 * @Note              -
 *                      1. This function determines the clock source
 *                         currently selected as the System Clock
 *                         (SYSCLK).
 *
 *                      2. Supported clock sources:
 *
 *                           - HSI (High-Speed Internal Oscillator)
 *                           - HSE (High-Speed External Oscillator)
 *                           - PLL Output Clock
 *
 *                      3. The function reads the SWS bits in the
 *                         RCC_CFGR register to identify the active
 *                         system clock source.
 *
 *                      4. After determining SYSCLK frequency, the
 *                         configured AHB prescaler is applied to
 *                         calculate the HCLK frequency.
 *
 *                      5. The configured APB1 prescaler is then
 *                         applied to obtain the final PCLK1
 *                         frequency.
 *
 *                      6. Clock relationship:
 *
 *                              SYSCLK
 *                                 |
 *                                 v
 *                               AHB Prescaler
 *                                 |
 *                                 v
 *                                HCLK
 *                                 |
 *                                 v
 *                              APB1 Prescaler
 *                                 |
 *                                 v
 *                               PCLK1
 *
 *                      7. PCLK1 is used by peripherals connected
 *                         to the APB1 bus such as:
 *
 *                           - I2C1 / I2C2 / I2C3
 *                           - USART2 / USART3 / UART4 / UART5
 *                           - SPI2 / SPI3
 *                           - TIM2-TIM7 / TIM12-TIM14
 *                           - Other APB1 peripherals
 *
 *                      8. This function is commonly used by
 *                         peripheral drivers when timing-related
 *                         register values must be calculated
 *                         dynamically.
 *
 *                      9. Example use cases:
 *
 *                           - I2C CCR calculation
 *                           - I2C TRISE calculation
 *                           - USART baud rate calculation
 *                           - Timer frequency calculations
 *
 *                     10. PLL clock calculation is delegated to
 *                         RCC_GetPLLOutputClock().
 *
 *                     11. The returned frequency reflects the
 *   					   current RCC clock configuration at
 *   					   runtime and may change if the system
 *   					   clock configuration is modified.
 *
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

	pclk1 =  (SystemClk / ahbp) / apb1p;

	return pclk1;
}



/*********************************************************************
 * @fn                - RCC_GetPCLK2Value
 *
 * @brief             - Calculate the current APB2 peripheral clock
 *                      frequency.
 *
 * @param[in]         - none
 *
 * @return            - APB2 peripheral clock frequency in Hertz (Hz)
 *
 * @Note              -
 *                      1. This function determines the clock source
 *                         currently selected as the System Clock
 *                         (SYSCLK).
 *
 *                      2. Supported clock sources:
 *
 *                           - HSI (High-Speed Internal Oscillator)
 *                           - HSE (High-Speed External Oscillator)
 *                           - PLL Output Clock
 *
 *                      3. The function reads the SWS bits in the
 *                         RCC_CFGR register to identify the active
 *                         system clock source.
 *
 *                      4. After determining SYSCLK frequency, the
 *                         configured AHB prescaler is applied to
 *                         calculate the HCLK frequency.
 *
 *                      5. The configured APB1 prescaler is then
 *                         applied to obtain the final PCLK2
 *                         frequency.
 *
 *                      6. Clock relationship:
 *
 *                              SYSCLK
 *                                 |
 *                                 v
 *                               AHB Prescaler
 *                                 |
 *                                 v
 *                                HCLK
 *                                 |
 *                                 v
 *                              APB2 Prescaler
 *                                 |
 *                                 v
 *                               PCLK1
 *
 *                      7. PCLK2 is used by peripherals connected
 *                         to the APB2 bus such as:
 *
 *                           - USART1 /USART6
 *                           - SPI1 / SPI4
 *                           - TIM1 / TIM8-TIM11
 *                           - Other APB1 peripherals
 *
 *                      8. This function is commonly used by
 *                         peripheral drivers when timing-related
 *                         register values must be calculated
 *                         dynamically.
 *
 *                      9. Example use cases:
 *
 *                           - USART baud rate calculation
 *                           - Timer frequency calculations
 *
 *                     10. PLL clock calculation is delegated to
 *                         RCC_GetPLLOutputClock().
 *
 *                     11. The returned frequency reflects the
 *   					   current RCC clock configuration at
 *   					   runtime and may change if the system
 *   					   clock configuration is modified.
 *
 */
uint32_t RCC_GetPCLK2Value(void)
{
	uint32_t pclk2,SystemClk;
	uint16_t ahbp,apb2p;
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
		apb2p = 1U;
	}else
	{
		apb2p = APB1_PreScaler[temp - 4U];
	}

	pclk2 =  (SystemClk / ahbp) / apb2p;

	return pclk2;
}






