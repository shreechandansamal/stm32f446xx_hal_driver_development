/*
 *  File Name: stm32f446xx.h
 *  Description: MCU Specific Header File(Hardware Description Layer)
 *  Created on: Feb 26, 2026
 *  Author: chandan
 */




#ifndef INC_STM32F446XX_H_
#define INC_STM32F446XX_H_

#include <stddef.h>
#include <stdint.h>
#include <assert.h>

/*Note: UL and uint32_t are same either can be written, e.g  */

#define __I  		volatile const   			// Read-only
#define __O  		volatile         			// Write-only
#define __IO 		volatile         			// Read/Write
#define __weak		__attribute__((weak))		// weak attribute for function to override by user application


/*************Only for testing keyboard matrix, in later lecture we will learn SysTick*****************/
/************* Cortex-M4 Core Peripheral: SysTick *************/

//#define SYSTICK_BASEADDR     0xE000E010UL
//
//typedef struct
//{
//    volatile uint32_t CTRL;    /* Offset 0x00 */
//    volatile uint32_t LOAD;    /* Offset 0x04 */
//    volatile uint32_t VAL;     /* Offset 0x08 */
//    volatile uint32_t CALIB;   /* Offset 0x0C */
//} SysTick_RegDef_t;
//
//#define SysTick   ((SysTick_RegDef_t*) SYSTICK_BASEADDR)
/******************************************************************************************************/




/****************************************************************************************************************/
/*                                          Processor Specific Details                                                             */
/****************************************************************************************************************/

/*********************************************************************
 * ARM Cortex-Mx Processor NVIC ISERx register Addresses
 *********************************************************************/
#define NVIC_ISER0                    ((__IO uint32_t *)0xE000E100UL)
#define NVIC_ISER1                    ((__IO uint32_t *)0xE000E104UL)
#define NVIC_ISER2                    ((__IO uint32_t *)0xE000E108UL)
#define NVIC_ISER3                    ((__IO uint32_t *)0xE000E10CUL)
#define NVIC_ISER4                    ((__IO uint32_t *)0xE000E110UL)
#define NVIC_ISER5                    ((__IO uint32_t *)0xE000E114UL)
#define NVIC_ISER6                    ((__IO uint32_t *)0xE000E118UL)
#define NVIC_ISER7                    ((__IO uint32_t *)0xE000E11CUL)



/*********************************************************************
 * ARM Cortex-Mx Processor NVIC ICERx register Addresses
 *********************************************************************/
#define NVIC_ICER0                    ((__IO uint32_t *)0xE000E180UL)
#define NVIC_ICER1                    ((__IO uint32_t *)0xE000E184UL)
#define NVIC_ICER2                    ((__IO uint32_t *)0xE000E188UL)
#define NVIC_ICER3                    ((__IO uint32_t *)0xE000E18CUL)
#define NVIC_ICER4                    ((__IO uint32_t *)0xE000E190UL)
#define NVIC_ICER5                    ((__IO uint32_t *)0xE000E194UL)
#define NVIC_ICER6                    ((__IO uint32_t *)0xE000E198UL)
#define NVIC_ICER7                    ((__IO uint32_t *)0xE000E19CUL)



/*********************************************************************
 * ARM Cortex-Mx Processor NVIC IPRx register Addresses
 *********************************************************************/
#define NVIC_IPR_BASE_ADDR            ((__IO uint32_t *)0xE000E400UL)






/*****************************************************************************************************************/
/*										      	MCU Specific Details				       								           */
/*****************************************************************************************************************/

/*********************************************************************
 * Number of Priority Bits Implemented in ST Micro-Controller
 *********************************************************************/
#define NO_PR_BITS_IMPLEMENTED         4U



/*********************************************************************
 * Flash and SRAM  Base Addresses (MEMORY MAP)
 *********************************************************************/
#define FLASH_BASEADDR                 0x08000000UL                 /*0x0800 0000 - 0x081F FFFF,
																	  Main Memory/Flash Memory size is 512Kbytes*/
#define SRAM1_BASEADDR                 0x20000000UL                 /*0x2000 0000 - 0x2001 BFFF, SRAM1 112Kbytes*/
#define SRAM2_BASEADDR                 0x2001C000UL 			    /*0x2001 C000 - 0x2001 FFFF, SRAM2 16Kbytes*/
#define ROM_BASEADDR                   0x1FFF0000UL                 /*0x1FFF 0000 - 0x1FFF 77FF,
                                                                      ROM/System Memory is 30Kbytes*/


/*********************************************************************
 * AHBx and APBx Bus Base Addresses (BUS DOMAINS)
 *********************************************************************/
#define PERIPH_BASEADDR                	0x40000000UL							/*Base address of peripheral register*/
#define APB1PERIPH_BASEADDR            	(PERIPH_BASEADDR + 0x00000000UL)		/*0x4000 0000 - 0x4000 77FF*/
#define APB2PERIPH_BASEADDR            	(PERIPH_BASEADDR + 0x00010000UL)		/*0x4001 0000 - 0x4001 5FFF*/
#define AHB1PERIPH_BASEADDR            	(PERIPH_BASEADDR + 0x00020000UL)		/*0x4002 0000 - 0x4007 FFFF*/
#define AHB2PERIPH_BASEADDR            	(PERIPH_BASEADDR + 0x10000000UL)		/*0x5000 0000 - 0x5005 03FF*/
#define AHB3PERIPH_BASEADDR            	0xA0000000UL							/*0xA000 0000 - 0xA000 FFFF*/



/*********************************************************************
 * AHB1 Peripheral Base Addresses
 *********************************************************************/
#define GPIOA_BASEADDR                 (AHB1PERIPH_BASEADDR + 0x0000UL)         /*0x4002 0000 - 0x4002 03FF*/
#define GPIOB_BASEADDR                 (AHB1PERIPH_BASEADDR + 0x0400UL)			/*0x4002 0400 - 0x4002 07FF*/
#define GPIOC_BASEADDR                 (AHB1PERIPH_BASEADDR + 0x0800UL)			/*0x4002 0800 - 0x4002 0BFF*/
#define GPIOD_BASEADDR                 (AHB1PERIPH_BASEADDR + 0x0C00UL)			/*0x4002 0C00 - 0x4002 0FFF*/
#define GPIOE_BASEADDR                 (AHB1PERIPH_BASEADDR + 0x1000UL)			/*0x4002 1000 - 0x4002 13FF*/
#define GPIOF_BASEADDR                 (AHB1PERIPH_BASEADDR + 0x1400UL)			/*0x4002 1400 - 0x4002 17FF*/
#define GPIOG_BASEADDR                 (AHB1PERIPH_BASEADDR + 0x1800UL)			/*0x4002 1800 - 0x4002 1BFF*/
#define GPIOH_BASEADDR                 (AHB1PERIPH_BASEADDR + 0x1C00UL)			/*0x4002 1C00 - 0x4002 1FFF*/
#define CRC_BASEADDR                   (AHB1PERIPH_BASEADDR + 0x3000UL)			/*0x4002 3000 - 0x4002 33FF*/
#define RCC_BASEADDR                   (AHB1PERIPH_BASEADDR + 0x3800UL)			/*0x4002 3800 - 0x4002 3BFF*/
#define FLASH_INTERFACE_BASEADDR       (AHB1PERIPH_BASEADDR + 0x3C00UL)			/*0x4002 3C00 - 0x4002 3FFF*/
#define BKPSRAM_BASEADDR               (AHB1PERIPH_BASEADDR + 0x4000UL)			/*0x4002 4000 - 0x4002 4FFF*/
#define DMA1_BASEADDR                  (AHB1PERIPH_BASEADDR + 0x6000UL)			/*0x4002 6000 - 0x4002 63FF*/
#define DMA2_BASEADDR                  (AHB1PERIPH_BASEADDR + 0x6400UL)			/*0x4002 6400 - 0x4002 67FF*/

/* Large AHB1 peripheral blocks */
#define USB_OTG_HS_BASEADDR            (AHB1PERIPH_BASEADDR + 0x00020000UL)		/*0x4004 0000 - 0x4007 FFFF*/



/*********************************************************************
 * AHB2 Peripheral Base Addresses
 *********************************************************************/
#define USB_OTG_FS_BASEADDR            (AHB2PERIPH_BASEADDR + 0x0000UL)			/*0x5000 0000 - 0x5003 FFFF*/
#define DCMI_BASEADDR            	   (AHB2PERIPH_BASEADDR + 0x00050000UL)		/*0x5005 0000 - 0x5005 03FF*/



/*********************************************************************
 * AHB3 Peripheral Base Addresses
 *********************************************************************/
#define FMC_BASEADDR       			   (AHB3PERIPH_BASEADDR + 0x0000UL)			/*0xA000 0000 - 0xA000 0FFF*/
#define QUADSPI_BASEADDR   			   (AHB3PERIPH_BASEADDR + 0x1000UL)			/*0xA000 1000 - 0xA000 1FFF*/



/*********************************************************************
 * APB1 Peripheral Base Addresses
 *********************************************************************/
#define TIM2_BASEADDR                  (APB1PERIPH_BASEADDR + 0x0000UL)          /*0x4000 0000 - 0x4000 03FF*/
#define TIM3_BASEADDR                  (APB1PERIPH_BASEADDR + 0x0400UL)			 /*0x4000 0400 - 0x4000 07FF*/
#define TIM4_BASEADDR                  (APB1PERIPH_BASEADDR + 0x0800UL)			 /*0x4000 0800 - 0x4000 0BFF*/
#define TIM5_BASEADDR                  (APB1PERIPH_BASEADDR + 0x0C00UL)			 /*0x4000 0C00 - 0x4000 0FFF*/
#define TIM6_BASEADDR                  (APB1PERIPH_BASEADDR + 0x1000UL)			 /*0x4000 1000 - 0x4000 13FF*/
#define TIM7_BASEADDR                  (APB1PERIPH_BASEADDR + 0x1400UL)			 /*0x4000 1400 - 0x4000 17FF*/
#define TIM12_BASEADDR                 (APB1PERIPH_BASEADDR + 0x1800UL)			 /*0x4000 1800 - 0x4000 1BFF*/
#define TIM13_BASEADDR                 (APB1PERIPH_BASEADDR + 0x1C00UL)			 /*0x4000 1C00 - 0x4000 1FFF*/
#define TIM14_BASEADDR                 (APB1PERIPH_BASEADDR + 0x2000UL)			 /*0x4000 2000 - 0x4000 23FF*/
#define RTC_BKP_BASEADDR               (APB1PERIPH_BASEADDR + 0x2800UL)			 /*0x4000 2800 - 0x4000 2BFF*/
#define WWDG_BASEADDR                  (APB1PERIPH_BASEADDR + 0x2C00UL)			 /*0x4000 2C00 - 0x4000 2FFF*/
#define IWDG_BASEADDR                  (APB1PERIPH_BASEADDR + 0x3000UL)			 /*0x4000 3000 - 0x4000 33FF*/
#define SPI2_I2S2_BASEADDR             (APB1PERIPH_BASEADDR + 0x3800UL)			 /*0x4000 3800 - 0x4000 3BFF*/
#define SPI3_I2S3_BASEADDR             (APB1PERIPH_BASEADDR + 0x3C00UL)			 /*0x4000 3C00 - 0x4000 3FFF*/
#define SPDIF_RX_BASEADDR              (APB1PERIPH_BASEADDR + 0x4000UL)			 /*0x4000 4000 - 0x4000 43FF*/
#define USART2_BASEADDR                (APB1PERIPH_BASEADDR + 0x4400UL)			 /*0x4000 4400 - 0x4000 47FF*/
#define USART3_BASEADDR                (APB1PERIPH_BASEADDR + 0x4800UL)			 /*0x4000 4800 - 0x4000 4BFF*/
#define UART4_BASEADDR                 (APB1PERIPH_BASEADDR + 0x4C00UL)			 /*0x4000 4C00 - 0x4000 4FFF*/
#define UART5_BASEADDR                 (APB1PERIPH_BASEADDR + 0x5000UL)			 /*0x4000 5000 - 0x4000 53FF*/
#define I2C1_BASEADDR                  (APB1PERIPH_BASEADDR + 0x5400UL)			 /*0x4000 5400 - 0x4000 57FF*/
#define I2C2_BASEADDR                  (APB1PERIPH_BASEADDR + 0x5800UL)			 /*0x4000 5800 - 0x4000 5BFF*/
#define I2C3_BASEADDR                  (APB1PERIPH_BASEADDR + 0x5C00UL)			 /*0x4000 5C00 - 0x4000 5FFF*/
#define CAN1_BASEADDR                  (APB1PERIPH_BASEADDR + 0x6400UL)			 /*0x4000 6400 - 0x4000 67FF*/
#define CAN2_BASEADDR                  (APB1PERIPH_BASEADDR + 0x6800UL)			 /*0x4000 6800 - 0x4000 6BFF*/
#define HDMI_CEC_BASEADDR              (APB1PERIPH_BASEADDR + 0x6C00UL)			 /*0x4000 6C00 - 0x4000 6FFF*/
#define PWR_BASEADDR                   (APB1PERIPH_BASEADDR + 0x7000UL)			 /*0x4000 7000 - 0x4000 73FF*/
#define DAC_BASEADDR                   (APB1PERIPH_BASEADDR + 0x7400UL)			 /*0x4000 7400 - 0x4000 77FF*/



/*********************************************************************
 * APB2 Peripheral Base Addresses
 *********************************************************************/
#define TIM1_BASEADDR                  (APB2PERIPH_BASEADDR + 0x0000UL)           /*0x4001 0000 - 0x4001 03FF*/
#define TIM8_BASEADDR                  (APB2PERIPH_BASEADDR + 0x0400UL)           /*0x4001 0400 - 0x4001 07FF*/
#define USART1_BASEADDR                (APB2PERIPH_BASEADDR + 0x1000UL)			  /*0x4001 1000 - 0x4001 13FF*/
#define USART6_BASEADDR                (APB2PERIPH_BASEADDR + 0x1400UL)    		  /*0x4001 1400 - 0x4001 17FF*/
#define ADC1_ADC2_ADC3_BASEADDR        (APB2PERIPH_BASEADDR + 0x2000UL)		      /*0x4001 2000 - 0x4001 23FF*/
#define SDMMC_BASEADDR                 (APB2PERIPH_BASEADDR + 0x2C00UL)			  /*0x4001 2C00 - 0x4001 2FFF*/
#define SPI1_BASEADDR             	   (APB2PERIPH_BASEADDR + 0x3000UL)           /*0x4001 3000 - 0x4001 33FF*/
#define SPI4_BASEADDR             	   (APB2PERIPH_BASEADDR + 0x3400UL)			  /*0x4001 3400 - 0x4001 37FF*/
#define SYSCFG_BASEADDR                (APB2PERIPH_BASEADDR + 0x3800UL)			  /*0x4001 3800 - 0x4001 3BFF*/
#define EXTI_BASEADDR                  (APB2PERIPH_BASEADDR + 0x3C00UL)			  /*0x4001 3C00 - 0x4001 3FFF*/
#define TIM9_BASEADDR                  (APB2PERIPH_BASEADDR + 0x4000UL)			  /*0x4001 4000 - 0x4001 43FF*/
#define TIM10_BASEADDR                 (APB2PERIPH_BASEADDR + 0x4400UL)			  /*0x4001 4400 - 0x4001 47FF*/
#define TIM11_BASEADDR                 (APB2PERIPH_BASEADDR + 0x4800UL)			  /*0x4001 4800 - 0x4001 4BFF*/
#define SAI1_BASEADDR             	   (APB2PERIPH_BASEADDR + 0x5800UL)			  /*0x4001 5800 - 0x4001 5BFF*/
#define SAI2_BASEADDR             	   (APB2PERIPH_BASEADDR + 0x5C00UL)			  /*0x4001 5C00 - 0x4001 5FFF*/



/*********************************************************************
 * Peripheral Register Definition Structure for RCC
 *********************************************************************/
typedef struct
{
	__IO uint32_t CR;         /*!< RCC clock control register                                    Address offset:0x00 */
	__IO uint32_t PLLCFGR;    /*!< RCC PLL configuration register                                Address offset:0x04 */
	__IO uint32_t CFGR;       /*!< RCC clock configuration register                              Address offset:0x08 */
	__IO uint32_t CIR;        /*!< RCC clock interrupt register                                  Address offset:0x0C */
	__IO uint32_t AHB1RSTR;   /*!< RCC AHB1 peripheral reset register                            Address offset:0x10 */
	__IO uint32_t AHB2RSTR;   /*!< RCC AHB2 peripheral reset register                            Address offset:0x14 */
	__IO uint32_t AHB3RSTR;   /*!< RCC AHB3 peripheral reset register                            Address offset:0x18 */
	uint32_t Reserved1;       /*!< Reserved                                                      Address offset:0x1C */
	__IO uint32_t APB1RSTR;   /*!< RCC APB1 peripheral reset register                            Address offset:0x20 */
	__IO uint32_t APB2RSTR;   /*!< RCC APB2 peripheral reset register                            Address offset:0x24 */
	uint32_t Reserved2[2];    /*!< Reserved                                  Address offset:0x28 Address offset:0x2C */
	__IO uint32_t AHB1ENR;    /*!< RCC AHB1 peripheral clock enable register                     Address offset:0x30 */
	__IO uint32_t AHB2ENR;    /*!< RCC AHB2 peripheral clock enable register                     Address offset:0x34 */
	__IO uint32_t AHB3ENR;    /*!< RCC AHB3 peripheral clock enable register                     Address offset:0x38 */
	uint32_t Reserved3;       /*!< Reserved                                                      Address offset:0x3C */
	__IO uint32_t APB1ENR;    /*!< RCC APB1 peripheral clock enable register                     Address offset:0x40 */
	__IO uint32_t APB2ENR;    /*!< RCC APB2 peripheral clock enable register                     Address offset:0x44 */
	uint32_t Reserved4[2];    /*!< Reserved                                   Address offset:0x48 Address offset:0x4C*/
	__IO uint32_t AHB1LPENR;  /*!< RCC AHB1 peripheral clock enable in low power mode register   Address offset:0x50 */
	__IO uint32_t AHB2LPENR;  /*!< RCC AHB2 peripheral clock enable in low power mode register   Address offset:0x54 */
	__IO uint32_t AHB3LPENR;  /*!< RCC AHB3 peripheral clock enable in low power mode register   Address offset:0x58 */
	uint32_t Reserved5;       /*!< Reserved                                                      Address offset:0x5C */
	__IO uint32_t APB1LPENR;  /*!< RCC APB1 peripheral clock enable in low power mode register   Address offset:0x60 */
	__IO uint32_t APB2LPENR;  /*!< RCC APB2 peripheral clock enabled in low power mode register  Address offset:0x64 */
	uint32_t Reserved6[2];    /*!< Reserved                                  Address offset:0x68 Address offset:0x6C */
	__IO uint32_t BDCR;       /*!< RCC Backup domain control register                            Address offset:0x70 */
	__IO uint32_t CSR;        /*!< RCC clock control & status register                           Address offset:0x74 */
	uint32_t Reserved7[2];    /*!< Reserved                                  Address offset:0x78 Address offset:0x7C */
	__IO uint32_t SSCGR;      /*!< RCC spread spectrum clock generation register                 Address offset:0x80 */
	__IO uint32_t PLLI2SCFGR; /*!< RCC PLLI2S configuration register                             Address offset:0x84 */
	__IO uint32_t PLLSAICFGR; /*!< RCC PLL configuration register                                Address offset:0x88 */
	__IO uint32_t DCKCFGR;    /*!< RCC Dedicated Clocks Configuration Register                   Address offset:0x8C */
	__IO uint32_t CKGATENR;   /*!< RCC clocks gated enable register                              Address offset:0x90 */
	__IO uint32_t DCKCFGR2;   /*!< RCC Dedicated Clocks Configuration Register 2                 Address offset:0x94 */

}RCC_RegDef_t;

_Static_assert(sizeof(RCC_RegDef_t) == 0x98, "RCC_RegDef struct size mismatch!");



/*********************************************************************
 * Peripheral Register Definition Structure for GPIO
 *********************************************************************/
typedef struct
{
	__IO uint32_t MODER;    /*!< GPIO port mode register                                         Address offset:0x00 */
	__IO uint32_t OTYPER;   /*!< GPIO port output type register                                  Address offset:0x04 */
	__IO uint32_t OSPEEDR;  /*!< GPIO port output speed register                                 Address offset:0x08 */
	__IO uint32_t PUPDR;    /*!< GPIO port pull-up/pull-down register                            Address offset:0x0C */
	__I uint32_t IDR;       /*!< GPIO port input data register                                   Address offset:0x10 */
	__IO uint32_t ODR;      /*!< GPIO port output data register                                  Address offset:0x14 */
	__O uint32_t BSRR;      /*!< GPIO port bit set/reset register                                Address offset:0x18 */
	__IO uint32_t LCKR;     /*!< GPIO port configuration lock register                           Address offset:0x1C */
	__IO uint32_t AFR[2];   /*!< AFR[0]: GPIO alternate function low register(AFRL)              Address offset:0x20
	                             AFR[1]: GPIO alternate function high register(AFRH)             Address offset:0x24 */
}GPIO_RegDef_t;

_Static_assert(sizeof(GPIO_RegDef_t) == 0x28, "GPIO_RegDef struct size mismatch!");



/*********************************************************************
 * Peripheral Register Definition Structure for EXTI
 *********************************************************************/
typedef struct
{
	__IO uint32_t IMR;      /*!< interrupt mask register                                         Address offset:0x00 */
	__IO uint32_t EMR;      /*!< event mask register                                             Address offset:0x04 */
	__IO uint32_t RTSR;     /*!< rising trigger selection register                               Address offset:0x08 */
	__IO uint32_t FTSR;     /*!< falling trigger selection register                              Address offset:0x0C */
	__IO uint32_t SWIER;    /*!< software interrupt event register                               Address offset:0x10 */
	__IO uint32_t PR;       /*!< pending register                                                Address offset:0x14 */

}EXTI_RegDef_t;

_Static_assert(sizeof(EXTI_RegDef_t) == 0x18, "EXTI_RegDef struct size mismatch!");



/*********************************************************************
 * Peripheral Register Definition Structure for SYSCFG
 *********************************************************************/
typedef struct
{
	__IO uint32_t MEMRMP;           /*!< SYSCFG memory re-map register                           Address offset:0x00 */
	__IO uint32_t PMC;				/*!< SYSCFG peripheral mode configuration register           Address offset:0x04 */
	__IO uint32_t EXTICR[4];		/*!< SYSCFG external interrupt configuration                 Address offset:0x08-0x14 */
	uint32_t      RESERVED1[2];		/*!< RESERVED                                                Address offset:0x18-0x1C */
	__IO uint32_t CMPCR;			/*!< Compensation cell control register                      Address offset:0x20 */
	uint32_t      RESERVED2[2];		/*!< RESERVED                                                Address offset:0x24-0x28 */
	__IO uint32_t CFGR;				/*!< SYSCFG configuration register                           Address offset:0x2C */

}SYSCFG_RegDef_t;

_Static_assert(sizeof(SYSCFG_RegDef_t) == 0x30, "SYSCFG_RegDef struct size mismatch!");



/*********************************************************************
 * Peripheral Register Definition Structure for SPI
 *********************************************************************/
typedef struct
{
	__IO uint32_t CR1;         /*!< SPI control register 1 (not used in I2S mode)           Address offset:0x00 */
	__IO uint32_t CR2;         /*!< SPI control register 2                                  Address offset:0x04 */
	__IO uint32_t SR;          /*!< SPI status register                                     Address offset:0x08 */
	__IO uint32_t DR;          /*!< SPI data register                                       Address offset:0x0C */
	__IO uint32_t CRCPR;       /*!< SPI CRC polynomial register (not used in I2S mode)      Address offset:0x10 */
	__I  uint32_t RXCRCR;      /*!< SPI RX CRC register (not used in I2S mode)              Address offset:0x14 */
	__I  uint32_t TXCRCR;      /*!< SPI TX CRC register (not used in I2S mode)              Address offset:0x18 */
	__IO uint32_t I2SCFGR;     /*!< SPI_I2S configuration register                          Address offset:0x1C */
	__IO uint32_t I2SPR;       /*!< SPI_I2S pre-scaler register                             Address offset:0x20 */

}SPI_RegDef_t;

_Static_assert(sizeof(SPI_RegDef_t) == 0x24, "SPI_RegDef struct size mismatch!");





/*********************************************************************
 * Peripheral Definitions (Base Address type-casted to xxx_RegDef_t)
 *********************************************************************/
#define GPIOA       ((GPIO_RegDef_t *)GPIOA_BASEADDR)
#define GPIOB       ((GPIO_RegDef_t *)GPIOB_BASEADDR)
#define GPIOC       ((GPIO_RegDef_t *)GPIOC_BASEADDR)
#define GPIOD       ((GPIO_RegDef_t *)GPIOD_BASEADDR)
#define GPIOE       ((GPIO_RegDef_t *)GPIOE_BASEADDR)
#define GPIOF       ((GPIO_RegDef_t *)GPIOF_BASEADDR)
#define GPIOG       ((GPIO_RegDef_t *)GPIOG_BASEADDR)
#define GPIOH       ((GPIO_RegDef_t *)GPIOH_BASEADDR)

#define RCC         ((RCC_RegDef_t *)RCC_BASEADDR)

#define EXTI 		((EXTI_RegDef_t *)EXTI_BASEADDR)

#define SYSCFG 		((SYSCFG_RegDef_t *)SYSCFG_BASEADDR)

#define SPI1        ((SPI_RegDef_t *)SPI1_BASEADDR)
#define SPI2        ((SPI_RegDef_t *)SPI2_I2S2_BASEADDR)
#define SPI3        ((SPI_RegDef_t *)SPI3_I2S3_BASEADDR)
#define SPI4        ((SPI_RegDef_t *)SPI4_BASEADDR)



/*********************************************************************
 * RCC AHB1ENR Register Bit Definitions
 *********************************************************************/

#define RCC_AHB1ENR_GPIOAEN_Pos      0U
#define RCC_AHB1ENR_GPIOAEN_Msk      (1U << RCC_AHB1ENR_GPIOAEN_Pos)

#define RCC_AHB1ENR_GPIOBEN_Pos      1U
#define RCC_AHB1ENR_GPIOBEN_Msk      (1U << RCC_AHB1ENR_GPIOBEN_Pos)

#define RCC_AHB1ENR_GPIOCEN_Pos      2U
#define RCC_AHB1ENR_GPIOCEN_Msk      (1U << RCC_AHB1ENR_GPIOCEN_Pos)

#define RCC_AHB1ENR_GPIODEN_Pos      3U
#define RCC_AHB1ENR_GPIODEN_Msk      (1U << RCC_AHB1ENR_GPIODEN_Pos)

#define RCC_AHB1ENR_GPIOEEN_Pos      4U
#define RCC_AHB1ENR_GPIOEEN_Msk      (1U << RCC_AHB1ENR_GPIOEEN_Pos)

#define RCC_AHB1ENR_GPIOFEN_Pos      5U
#define RCC_AHB1ENR_GPIOFEN_Msk      (1U << RCC_AHB1ENR_GPIOFEN_Pos)

#define RCC_AHB1ENR_GPIOGEN_Pos      6U
#define RCC_AHB1ENR_GPIOGEN_Msk      (1U << RCC_AHB1ENR_GPIOGEN_Pos)

#define RCC_AHB1ENR_GPIOHEN_Pos      7U
#define RCC_AHB1ENR_GPIOHEN_Msk      (1U << RCC_AHB1ENR_GPIOHEN_Pos)



/*********************************************************************
 * RCC APB1ENR Register Bit Definitions
 *********************************************************************/

#define RCC_APB1ENR_I2C1EN_Pos      21U
#define RCC_APB1ENR_I2C1EN_Msk      (1U << RCC_APB1ENR_I2C1EN_Pos)

#define RCC_APB1ENR_I2C2EN_Pos      22U
#define RCC_APB1ENR_I2C2EN_Msk      (1U << RCC_APB1ENR_I2C2EN_Pos)

#define RCC_APB1ENR_I2C3EN_Pos      23U
#define RCC_APB1ENR_I2C3EN_Msk      (1U << RCC_APB1ENR_I2C3EN_Pos)

#define RCC_APB1ENR_SPI2EN_Pos      14U
#define RCC_APB1ENR_SPI2EN_Msk      (1U << RCC_APB1ENR_SPI2EN_Pos)

#define RCC_APB1ENR_SPI3EN_Pos      15U
#define RCC_APB1ENR_SPI3EN_Msk      (1U << RCC_APB1ENR_SPI3EN_Pos)

#define RCC_APB1ENR_USART2EN_Pos    17U
#define RCC_APB1ENR_USART2EN_Msk    (1U << RCC_APB1ENR_USART2EN_Pos)

#define RCC_APB1ENR_USART3EN_Pos    18U
#define RCC_APB1ENR_USART3EN_Msk    (1U << RCC_APB1ENR_USART3EN_Pos)

#define RCC_APB1ENR_UART4EN_Pos    	19U
#define RCC_APB1ENR_UART4EN_Msk    	(1U << RCC_APB1ENR_UART4EN_Pos)

#define RCC_APB1ENR_UART5EN_Pos    	20U
#define RCC_APB1ENR_UART5EN_Msk    	(1U << RCC_APB1ENR_UART5EN_Pos)


/*********************************************************************
 * RCC APB2ENR Register Bit Definitions
 *********************************************************************/

#define RCC_APB2ENR_SYSCFGEN_Pos    14U
#define RCC_APB2ENR_SYSCFGEN_Msk    (1U << RCC_APB2ENR_SYSCFGEN_Pos)

#define RCC_APB2ENR_SPI1EN_Pos      12U
#define RCC_APB2ENR_SPI1EN_Msk      (1U << RCC_APB2ENR_SPI1EN_Pos)

#define RCC_APB2ENR_SPI4EN_Pos      13U
#define RCC_APB2ENR_SPI4EN_Msk      (1U << RCC_APB2ENR_SPI4EN_Pos)

#define RCC_APB2ENR_USART1EN_Pos    4U
#define RCC_APB2ENR_USART1EN_Msk    (1U << RCC_APB2ENR_USART1EN_Pos)

#define RCC_APB2ENR_USART6EN_Pos    5U
#define RCC_APB2ENR_USART6EN_Msk    (1U << RCC_APB2ENR_USART6EN_Pos)



/*********************************************************************
 * RCC AHB1RSTR Register Bit Definitions
 *********************************************************************/

#define RCC_AHB1RSTR_GPIOARST_Pos      0U
#define RCC_AHB1RSTR_GPIOARST_Msk      (1U << RCC_AHB1RSTR_GPIOARST_Pos)

#define RCC_AHB1RSTR_GPIOBRST_Pos      1U
#define RCC_AHB1RSTR_GPIOBRST_Msk      (1U << RCC_AHB1RSTR_GPIOBRST_Pos)

#define RCC_AHB1RSTR_GPIOCRST_Pos      2U
#define RCC_AHB1RSTR_GPIOCRST_Msk      (1U << RCC_AHB1RSTR_GPIOCRST_Pos)

#define RCC_AHB1RSTR_GPIODRST_Pos      3U
#define RCC_AHB1RSTR_GPIODRST_Msk      (1U << RCC_AHB1RSTR_GPIODRST_Pos)

#define RCC_AHB1RSTR_GPIOERST_Pos      4U
#define RCC_AHB1RSTR_GPIOERST_Msk      (1U << RCC_AHB1RSTR_GPIOERST_Pos)

#define RCC_AHB1RSTR_GPIOFRST_Pos      5U
#define RCC_AHB1RSTR_GPIOFRST_Msk      (1U << RCC_AHB1RSTR_GPIOFRST_Pos)

#define RCC_AHB1RSTR_GPIOGRST_Pos      6U
#define RCC_AHB1RSTR_GPIOGRST_Msk      (1U << RCC_AHB1RSTR_GPIOGRST_Pos)

#define RCC_AHB1RSTR_GPIOHRST_Pos      7U
#define RCC_AHB1RSTR_GPIOHRST_Msk      (1U << RCC_AHB1RSTR_GPIOHRST_Pos)



/*********************************************************************
 * RCC APB1RSTR Register Bit Definitions
 *********************************************************************/

#define RCC_APB1RSTR_I2C1RST_Pos      21U
#define RCC_APB1RSTR_I2C1RST_Msk      (1U << RCC_APB1RSTR_I2C1RST_Pos)

#define RCC_APB1RSTR_I2C2RST_Pos      22U
#define RCC_APB1RSTR_I2C2RST_Msk      (1U << RCC_APB1RSTR_I2C2RST_Pos)

#define RCC_APB1RSTR_I2C3RST_Pos      23U
#define RCC_APB1RSTR_I2C3RST_Msk      (1U << RCC_APB1RSTR_I2C3RST_Pos)

#define RCC_APB1RSTR_SPI2RST_Pos      14U
#define RCC_APB1RSTR_SPI2RST_Msk      (1U << RCC_APB1RSTR_SPI2RST_Pos)

#define RCC_APB1RSTR_SPI3RST_Pos      15U
#define RCC_APB1RSTR_SPI3RST_Msk      (1U << RCC_APB1RSTR_SPI3RST_Pos)

#define RCC_APB1RSTR_USART2RST_Pos    17U
#define RCC_APB1RSTR_USART2RST_Msk    (1U << RCC_APB1RSTR_USART2RST_Pos)

#define RCC_APB1RSTR_USART3RST_Pos    18U
#define RCC_APB1RSTR_USART3RST_Msk    (1U << RCC_APB1RSTR_USART3RST_Pos)

#define RCC_APB1RSTR_UART4RST_Pos     19U
#define RCC_APB1RSTR_UART4RST_Msk     (1U << RCC_APB1RSTR_UART4RST_Pos)

#define RCC_APB1RSTR_UART5RST_Pos     20U
#define RCC_APB1RSTR_UART5RST_Msk     (1U << RCC_APB1RSTR_UART5RST_Pos)


/*********************************************************************
 * RCC APB2RSTR Register Bit Definitions
 *********************************************************************/

#define RCC_APB2RSTR_SYSCFGRST_Pos    14U
#define RCC_APB2RSTR_SYSCFGRST_Msk    (1U << RCC_APB2RSTR_SYSCFGRST_Pos)

#define RCC_APB2RSTR_SPI1RST_Pos      12U
#define RCC_APB2RSTR_SPI1RST_Msk      (1U << RCC_APB2RSTR_SPI1RST_Pos)

#define RCC_APB2RSTR_SPI4RST_Pos      13U
#define RCC_APB2RSTR_SPI4RST_Msk      (1U << RCC_APB2RSTR_SPI4RST_Pos)

#define RCC_APB2RSTR_USART1RST_Pos    4U
#define RCC_APB2RSTR_USART1RST_Msk    (1U << RCC_APB2RSTR_USART1RST_Pos)

#define RCC_APB2RSTR_USART6RST_Pos    5U
#define RCC_APB2RSTR_USART6RST_Msk    (1U << RCC_APB2RSTR_USART6RST_Pos)



/*********************************************************************
 * ENABLE MACROS FOR PERIPHERALS
 *********************************************************************/
/*
 * Clock Enable Macros for GPIOx Peripherals
 */
#define GPIOA_PCLK_EN()    (RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN_Msk)
#define GPIOB_PCLK_EN()    (RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN_Msk)
#define GPIOC_PCLK_EN()    (RCC->AHB1ENR |= RCC_AHB1ENR_GPIOCEN_Msk)
#define GPIOD_PCLK_EN()    (RCC->AHB1ENR |= RCC_AHB1ENR_GPIODEN_Msk)
#define GPIOE_PCLK_EN()    (RCC->AHB1ENR |= RCC_AHB1ENR_GPIOEEN_Msk)
#define GPIOF_PCLK_EN()    (RCC->AHB1ENR |= RCC_AHB1ENR_GPIOFEN_Msk)
#define GPIOG_PCLK_EN()    (RCC->AHB1ENR |= RCC_AHB1ENR_GPIOGEN_Msk)
#define GPIOH_PCLK_EN()    (RCC->AHB1ENR |= RCC_AHB1ENR_GPIOHEN_Msk)

/*
 * Clock Enable Macros for I2Cx Peripherals
 */
#define I2C1_PCLK_EN()      (RCC->APB1ENR |= RCC_APB1ENR_I2C1EN_Msk)
#define I2C2_PCLK_EN()      (RCC->APB1ENR |= RCC_APB1ENR_I2C2EN_Msk)
#define I2C3_PCLK_EN()      (RCC->APB1ENR |= RCC_APB1ENR_I2C3EN_Msk)

/*
 * Clock Enable Macros for SPIx Peripherals
 */
#define SPI1_PCLK_EN()      (RCC->APB2ENR |= RCC_APB2ENR_SPI1EN_Msk)
#define SPI2_PCLK_EN()      (RCC->APB1ENR |= RCC_APB1ENR_SPI2EN_Msk)
#define SPI3_PCLK_EN()      (RCC->APB1ENR |= RCC_APB1ENR_SPI3EN_Msk)
#define SPI4_PCLK_EN()      (RCC->APB2ENR |= RCC_APB2ENR_SPI4EN_Msk)

/*
 * Clock Enable Macros for USARTx Peripherals
 */
#define USART1_PCLK_EN()     (RCC->APB2ENR |= RCC_APB2ENR_USART1EN_Msk)
#define USART2_PCLK_EN()     (RCC->APB1ENR |= RCC_APB1ENR_USART2EN_Msk)
#define USART3_PCLK_EN()     (RCC->APB1ENR |= RCC_APB1ENR_USART3EN_Msk)
#define UART4_PCLK_EN()      (RCC->APB1ENR |= RCC_APB1ENR_UART4EN_Msk)
#define UART5_PCLK_EN()      (RCC->APB1ENR |= RCC_APB1ENR_UART5EN_Msk)
#define USART6_PCLK_EN()     (RCC->APB2ENR |= RCC_APB2ENR_USART6EN_Msk)

/*
 * Clock Enable Macros for SYSCFG Peripherals
 */
#define SYSCFG_PCLK_EN()	(RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN_Msk)


/*********************************************************************
 * DISABLE MACROS FOR PERIPHERALS
 *********************************************************************/
/*
 * Clock Disable Macros for GPIOx Peripherals
 */
#define GPIOA_PCLK_DI()     (RCC->AHB1ENR &= ~RCC_AHB1ENR_GPIOAEN_Msk)
#define GPIOB_PCLK_DI()     (RCC->AHB1ENR &= ~RCC_AHB1ENR_GPIOBEN_Msk)
#define GPIOC_PCLK_DI()     (RCC->AHB1ENR &= ~RCC_AHB1ENR_GPIOCEN_Msk)
#define GPIOD_PCLK_DI()     (RCC->AHB1ENR &= ~RCC_AHB1ENR_GPIODEN_Msk)
#define GPIOE_PCLK_DI()     (RCC->AHB1ENR &= ~RCC_AHB1ENR_GPIOEEN_Msk)
#define GPIOF_PCLK_DI()     (RCC->AHB1ENR &= ~RCC_AHB1ENR_GPIOFEN_Msk)
#define GPIOG_PCLK_DI()     (RCC->AHB1ENR &= ~RCC_AHB1ENR_GPIOGEN_Msk)
#define GPIOH_PCLK_DI()     (RCC->AHB1ENR &= ~RCC_AHB1ENR_GPIOHEN_Msk)

/*
 * Clock Disable Macros for I2Cx Peripherals
 */
#define I2C1_PCLK_DI()      (RCC->APB1ENR &= ~RCC_APB1ENR_I2C1EN_Msk)
#define I2C2_PCLK_DI()      (RCC->APB1ENR &= ~RCC_APB1ENR_I2C2EN_Msk)
#define I2C3_PCLK_DI()      (RCC->APB1ENR &= ~RCC_APB1ENR_I2C3EN_Msk)

/*
 * Clock Disable Macros for SPIx Peripherals
 */
#define SPI1_PCLK_DI()      (RCC->APB2ENR &= ~RCC_APB2ENR_SPI1EN_Msk)
#define SPI2_PCLK_DI()      (RCC->APB1ENR &= ~RCC_APB1ENR_SPI2EN_Msk)
#define SPI3_PCLK_DI()      (RCC->APB1ENR &= ~RCC_APB1ENR_SPI3EN_Msk)
#define SPI4_PCLK_DI()      (RCC->APB2ENR &= ~RCC_APB2ENR_SPI4EN_Msk)

/*
 * Clock Disable Macros for USARTx Peripherals
 */
#define USART1_PCLK_DI()    (RCC->APB2ENR &= ~RCC_APB2ENR_USART1EN_Msk)
#define USART2_PCLK_DI()    (RCC->APB1ENR &= ~RCC_APB1ENR_USART2EN_Msk)
#define USART3_PCLK_DI()    (RCC->APB1ENR &= ~RCC_APB1ENR_USART3EN_Msk)
#define UART4_PCLK_DI()     (RCC->APB1ENR &= ~RCC_APB1ENR_UART4EN_Msk)
#define UART5_PCLK_DI()     (RCC->APB1ENR &= ~RCC_APB1ENR_UART5EN_Msk)
#define USART6_PCLK_DI()    (RCC->APB2ENR &= ~RCC_APB2ENR_USART6EN_Msk)

/*
 * Clock Disable Macros for SYSCFG Peripherals
 */
#define SYSCFG_PCLK_DI()	(RCC->APB2ENR &= ~RCC_APB2ENR_SYSCFGEN_Msk)



/*********************************************************************
 * RESET MACROS FOR PERIPHERALS
 *********************************************************************/
/*
 * Macro to reset GPIOx peripherals
 */
#define GPIOA_REG_RESET()   do{ (RCC->AHB1RSTR |= RCC_AHB1RSTR_GPIOARST_Msk); (RCC->AHB1RSTR &= ~RCC_AHB1RSTR_GPIOARST_Msk); }while(0)
#define GPIOB_REG_RESET()   do{ (RCC->AHB1RSTR |= RCC_AHB1RSTR_GPIOBRST_Msk); (RCC->AHB1RSTR &= ~RCC_AHB1RSTR_GPIOBRST_Msk); }while(0)
#define GPIOC_REG_RESET()   do{ (RCC->AHB1RSTR |= RCC_AHB1RSTR_GPIOCRST_Msk); (RCC->AHB1RSTR &= ~RCC_AHB1RSTR_GPIOCRST_Msk); }while(0)
#define GPIOD_REG_RESET()   do{ (RCC->AHB1RSTR |= RCC_AHB1RSTR_GPIODRST_Msk); (RCC->AHB1RSTR &= ~RCC_AHB1RSTR_GPIODRST_Msk); }while(0)
#define GPIOE_REG_RESET()   do{ (RCC->AHB1RSTR |= RCC_AHB1RSTR_GPIOERST_Msk); (RCC->AHB1RSTR &= ~RCC_AHB1RSTR_GPIOERST_Msk); }while(0)
#define GPIOF_REG_RESET()   do{ (RCC->AHB1RSTR |= RCC_AHB1RSTR_GPIOFRST_Msk); (RCC->AHB1RSTR &= ~RCC_AHB1RSTR_GPIOFRST_Msk); }while(0)
#define GPIOG_REG_RESET()   do{ (RCC->AHB1RSTR |= RCC_AHB1RSTR_GPIOGRST_Msk); (RCC->AHB1RSTR &= ~RCC_AHB1RSTR_GPIOGRST_Msk); }while(0)
#define GPIOH_REG_RESET()   do{ (RCC->AHB1RSTR |= RCC_AHB1RSTR_GPIOHRST_Msk); (RCC->AHB1RSTR &= ~RCC_AHB1RSTR_GPIOHRST_Msk); }while(0)



/*
 * Macro to reset SPIx peripherals
 */
#define SPI1_REG_RESET()   do{ (RCC->APB2RSTR |= RCC_APB2RSTR_SPI1RST_Msk); (RCC->APB2RSTR &= ~RCC_APB2RSTR_SPI1RST_Msk); }while(0)
#define SPI2_REG_RESET()   do{ (RCC->APB1RSTR |= RCC_APB1RSTR_SPI2RST_Msk); (RCC->APB1RSTR &= ~RCC_APB1RSTR_SPI2RST_Msk); }while(0)
#define SPI3_REG_RESET()   do{ (RCC->APB1RSTR |= RCC_APB1RSTR_SPI3RST_Msk); (RCC->APB1RSTR &= ~RCC_APB1RSTR_SPI3RST_Msk); }while(0)
#define SPI4_REG_RESET()   do{ (RCC->APB2RSTR |= RCC_APB2RSTR_SPI4RST_Msk); (RCC->APB2RSTR &= ~RCC_APB2RSTR_SPI4RST_Msk); }while(0)



/*********************************************************************
 * GPIO Base Address to Code
 *********************************************************************/
/*
 * GPIO Base address to Code Conversion,
 * if else using macro, (condition) ? if :\ means else check for (condition)
 */
#define GPIO_BASEADDR_TO_CODE(GpioPort)   \
( ((GpioPort) == GPIOA)?0U: \
  ((GpioPort) == GPIOB)?1U: \
  ((GpioPort) == GPIOC)?2U: \
  ((GpioPort) == GPIOD)?3U: \
  ((GpioPort) == GPIOE)?4U: \
  ((GpioPort) == GPIOF)?5U: \
  ((GpioPort) == GPIOG)?6U: \
  ((GpioPort) == GPIOH)?7U:0U )



/*********************************************************************
 * IRQ Numbers (Processor Specific)
 *********************************************************************/
/*
 * IRQ(Interrupt Request) Numbers of EXTI in STM32F446xx MCU
 */
#define IRQ_NO_EXTI0                     6U    /*!< EXTI Line0 interrupt number */
#define IRQ_NO_EXTI1                     7U    /*!< EXTI Line1 interrupt number */
#define IRQ_NO_EXTI2                     8U    /*!< EXTI Line2 interrupt number */
#define IRQ_NO_EXTI3                     9U    /*!< EXTI Line3 interrupt number */
#define IRQ_NO_EXTI4                     10U   /*!< EXTI Line4 interrupt number */
#define IRQ_NO_EXTI9_5                   23U   /*!< EXTI Line5-9 interrupt number */
#define IRQ_NO_EXTI15_10                 40U   /*!< EXTI Line10-15 interrupt number */



/*
 * IRQ(Interrupt Request) Numbers of SPI in STM32F446xx MCU
 */
#define IRQ_NO_SPI1      				 35U   /*!< SPI1 global interrupt number */
#define IRQ_NO_SPI2      				 36U   /*!< SPI2 global interrupt number */
#define IRQ_NO_SPI3      				 51U   /*!< SPI3 global interrupt number */
#define IRQ_NO_SPI4      				 84U   /*!< SPI4 global interrupt number */



/*********************************************************************
 * Priority Levels (Processor Specific)
 *********************************************************************/
#define NVIC_IRQ_PRI0                    0U
#define NVIC_IRQ_PRI1                    1U
#define NVIC_IRQ_PRI2                    2U
#define NVIC_IRQ_PRI3                    3U
#define NVIC_IRQ_PRI4                    4U
#define NVIC_IRQ_PRI5                    5U
#define NVIC_IRQ_PRI6                    6U
#define NVIC_IRQ_PRI7                    7U
#define NVIC_IRQ_PRI8                    8U
#define NVIC_IRQ_PRI9                    9U
#define NVIC_IRQ_PRI10                   10U
#define NVIC_IRQ_PRI11                   11U
#define NVIC_IRQ_PRI12                   12U
#define NVIC_IRQ_PRI13                   13U
#define NVIC_IRQ_PRI14                   14U
#define NVIC_IRQ_PRI15                   15U



/*********************************************************************
 * Some Generic Macros
 *********************************************************************/
#define ENABLE              			1U
#define DISABLE             			0U
#define SET                 			ENABLE
#define RESET               			DISABLE
#define HIGH							ENABLE
#define LOW								DISABLE
#define GPIO_PIN_SET        			SET
#define GPIO_PIN_RESET      			RESET
#define FLAG_SET						SET
#define FLAG_RESET						RESET



/*********************************************************************
 * Bit Position Definition of SPI Peripheral
 *********************************************************************/
/*
 * Bit position definition SPI_CR1
 */
#define SPI_CR1_CPHA					0U
#define SPI_CR1_CPOL					1U
#define SPI_CR1_MSTR					2U
#define SPI_CR1_BR						3U
#define SPI_CR1_SPE						6U
#define SPI_CR1_LSBFIRST				7U
#define SPI_CR1_SSI						8U
#define SPI_CR1_SSM						9U
#define SPI_CR1_RXONLY					10U
#define SPI_CR1_DFF						11U
#define SPI_CR1_CRCNEXT					12U
#define SPI_CR1_CRCEN					13U
#define SPI_CR1_BIDIOE					14U
#define SPI_CR1_BIDIMODE				15U

/*
 * Bit position definition SPI_CR2
 */
#define SPI_CR2_RXDMAEN					0U
#define SPI_CR2_TXDMAEN					1U
#define SPI_CR2_SSOE					2U
#define SPI_CR2_FRF						4U
#define SPI_CR2_ERRIE					5U
#define SPI_CR2_RXNEIE					6U
#define SPI_CR2_TXEIE					7U

/*
 * Bit position definition SPI_SR
 */
#define SPI_SR_RXNE						0U
#define SPI_SR_TXE						1U
#define SPI_SR_CHSIDE					2U
#define SPI_SR_UDR						3U
#define SPI_SR_CRCERR					4U
#define SPI_SR_MODF						5U
#define SPI_SR_OVR						6U
#define SPI_SR_BSY						7U
#define SPI_SR_FRE						8U








//GPIO driver specific header file
//#include "stm32f446xx_gpio_driver.h"


#endif /* INC_STM32F446XX_H_ */
