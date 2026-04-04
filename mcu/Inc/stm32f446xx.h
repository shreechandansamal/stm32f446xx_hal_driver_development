/*
 *  File Name: stm32f446xx.h
 *  Description: MCU Specific Header File(Hardware Description Layer)
 *  Created on: Feb 26, 2026
 *  Author: chandan
 */




#ifndef INC_STM32F446XX_H_
#define INC_STM32F446XX_H_

#include <stdint.h>


/*Note: UL and uint32_t are same either can be written, e.g  */

#define __I  volatile const   // Read-only
#define __O  volatile         // Write-only
#define __IO volatile         // Read/Write


/*************Only for testing keyboard matrix, in later lecture we will learn SysTick*****************/
/************* Cortex-M4 Core Peripheral: SysTick *************/

#define SYSTICK_BASEADDR     0xE000E010UL

typedef struct
{
    volatile uint32_t CTRL;    /* Offset 0x00 */
    volatile uint32_t LOAD;    /* Offset 0x04 */
    volatile uint32_t VAL;     /* Offset 0x08 */
    volatile uint32_t CALIB;   /* Offset 0x0C */
} SysTick_RegDef_t;

#define SysTick   ((SysTick_RegDef_t*) SYSTICK_BASEADDR)
/******************************************************************************************************/




/***********************************************************************************************************************************/
/*                                          Processor Specific Details                                                             */
/***********************************************************************************************************************************/
/*
 * ARM Cortex-Mx Processor NVIC ISERx register Addresses
 */
#define NVIC_ISER0                    ((volatile uint32_t *)0xE000E100)
#define NVIC_ISER1                    ((volatile uint32_t *)0xE000E104)
#define NVIC_ISER2                    ((volatile uint32_t *)0xE000E108)
#define NVIC_ISER3                    ((volatile uint32_t *)0xE000E10C)
#define NVIC_ISER4                    ((volatile uint32_t *)0xE000E110)
#define NVIC_ISER5                    ((volatile uint32_t *)0xE000E114)
#define NVIC_ISER6                    ((volatile uint32_t *)0xE000E118)
#define NVIC_ISER7                    ((volatile uint32_t *)0xE000E11C)

/*
 * ARM Cortex-Mx Processor NVIC ICERx register Addresses
 */
#define NVIC_ICER0                    ((volatile uint32_t *)0xE000E180)
#define NVIC_ICER1                    ((volatile uint32_t *)0xE000E184)
#define NVIC_ICER2                    ((volatile uint32_t *)0xE000E188)
#define NVIC_ICER3                    ((volatile uint32_t *)0xE000E18C)
#define NVIC_ICER4                    ((volatile uint32_t *)0xE000E190)
#define NVIC_ICER5                    ((volatile uint32_t *)0xE000E194)
#define NVIC_ICER6                    ((volatile uint32_t *)0xE000E198)
#define NVIC_ICER7                    ((volatile uint32_t *)0xE000E19C)

/*
 * ARM Cortex-Mx Processor NVIC IPRx register Addresses
 */
#define NVIC_IPR_BASE_ADDR            ((volatile uint32_t *)0xE000E400)






/***********************************************************************************************************************************/
/*										      	MCU Specific Details				       								           */
/***********************************************************************************************************************************/
/*
 * number of priority bits implemented in ST Micro-Controller
 */
#define NO_PR_BITS_IMPLEMENTED         4


/************************************************************************************************************************/
/*       										MEMORY MAP													            */
/************************************************************************************************************************/

/*
 * base address of flash and SRAM memories
 */
#define FLASH_BASEADDR                 0x08000000UL                 /*0x0800 0000 - 0x081F FFFF,
																	  Main Memory/Flash Memory size is 512Kbytes*/
#define SRAM1_BASEADDR                 0x20000000UL                 /*0x2000 0000 - 0x2001 BFFF, SRAM1 112Kbytes*/
#define SRAM2_BASEADDR                 0x2001C000UL 			    /*0x2001 C000 - 0x2001 FFFF, SRAM2 16Kbytes*/
#define ROM_BASEADDR                   0x1FFF0000UL                 /*0x1FFF 0000 - 0x1FFF 77FF,
                                                                      ROM/System Memory is 30Kbytes*/


/************************************************************************************************************************/
/*       								        BUS DOMAINS													            */
/************************************************************************************************************************/

/*
 * AHBx and APBx Bus base addresses
 */
#define PERIPH_BASEADDR                	0x40000000UL							/*Base address of peripheral register*/
#define APB1PERIPH_BASEADDR            	(PERIPH_BASEADDR + 0x00000000UL)		/*0x4000 0000 - 0x4000 77FF*/
#define APB2PERIPH_BASEADDR            	(PERIPH_BASEADDR + 0x00010000UL)		/*0x4001 0000 - 0x4001 5FFF*/
#define AHB1PERIPH_BASEADDR            	(PERIPH_BASEADDR + 0x00020000UL)		/*0x4002 0000 - 0x4007 FFFF*/
#define AHB2PERIPH_BASEADDR            	(PERIPH_BASEADDR + 0x10000000UL)		/*0x5000 0000 - 0x5005 03FF*/
#define AHB3PERIPH_BASEADDR            	0xA0000000UL							/*0xA000 0000 - 0xA000 FFFF*/


/************************************************************************************************************************/
/*       								       AHB1 PERIPHERALS												            */
/************************************************************************************************************************/

/* GPIO Port Count */
#define GPIO_PORT_COUNT 8U   // GPIOA to GPIOH

/*
 * AHB1 peripheral base addresses
 */
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


/************************************************************************************************************************/
/*       								       AHB2 PERIPHERALS												            */
/************************************************************************************************************************/

/*
 * AHB2 peripheral base addresses
 */
#define USB_OTG_FS_BASEADDR            (AHB2PERIPH_BASEADDR + 0x0000UL)			/*0x5000 0000 - 0x5003 FFFF*/
#define DCMI_BASEADDR            	   (AHB2PERIPH_BASEADDR + 0x00050000UL)		/*0x5005 0000 - 0x5005 03FF*/


/************************************************************************************************************************/
/*       								       AHB3 PERIPHERALS												            */
/************************************************************************************************************************/

/*
 * AHB3 peripheral base addresses
 */
#define FMC_BASEADDR       			   (AHB3PERIPH_BASEADDR + 0x0000UL)			/*0xA000 0000 - 0xA000 0FFF*/
#define QUADSPI_BASEADDR   			   (AHB3PERIPH_BASEADDR + 0x1000UL)			/*0xA000 1000 - 0xA000 1FFF*/


/************************************************************************************************************************/
/*       								       APB1 PERIPHERALS												            */
/************************************************************************************************************************/

/*
 * APB1 peripheral base addresses
 */
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


/************************************************************************************************************************/
/*       								       APB2 PERIPHERALS												            */
/************************************************************************************************************************/

/*
 * APB2 peripheral base addresses
 */
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


/************************************************************************************************************************/
/*       						    Peripheral Register Definition Structure					   				        */
/************************************************************************************************************************/

/*
 * peripheral register definition structures for RCC
 */
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


/*
 * peripheral register definition structures for GPIO
 */
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
/*
 *peripheral register definition structures for EXTI
 */
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
/*
 *peripheral register definition structures for SYSCFG
 */
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




/*
 * peripheral definitions( Peripheral base address type-casted to xxx_RegDef_t)
 */
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



/**********************************************************ENABLE MACROS FOR PERIPHERALS****************************************************/
/*
 * Clock Enable Macros for GPIOx Peripherals
 */
//#define GPIOA_PCLK_EN()    (RCC->AHB1ENR |= (1U << 0))
//#define GPIOB_PCLK_EN()    (RCC->AHB1ENR |= (1U << 1))
//#define GPIOC_PCLK_EN()    (RCC->AHB1ENR |= (1U << 2))
//#define GPIOD_PCLK_EN()    (RCC->AHB1ENR |= (1U << 3))
//#define GPIOE_PCLK_EN()    (RCC->AHB1ENR |= (1U << 4))
//#define GPIOF_PCLK_EN()    (RCC->AHB1ENR |= (1U << 5))
//#define GPIOG_PCLK_EN()    (RCC->AHB1ENR |= (1U << 6))
//#define GPIOH_PCLK_EN()    (RCC->AHB1ENR |= (1U << 7))

/*
 * Clock Enable Macros for I2Cx Peripherals
 */
#define I2C1_PCLK_EN()      (RCC->APB1ENR |= (1U << 21))
#define I2C2_PCLK_EN()      (RCC->APB1ENR |= (1U << 22))
#define I2C3_PCLK_EN()      (RCC->APB1ENR |= (1U << 23))

/*
 * Clock Enable Macros for SPIx Peripherals
 */
#define SPI1_PCLK_EN()      (RCC->APB2ENR |= (1U << 12))
#define SPI2_PCLK_EN()      (RCC->APB1ENR |= (1U << 14))
#define SPI3_PCLK_EN()      (RCC->APB1ENR |= (1U << 15))
#define SPI4_PCLK_EN()      (RCC->APB2ENR |= (1U << 13))

/*
 * Clock Enable Macros for USARTx Peripherals
 */
#define USART1_PCLK_EN()     (RCC->APB2ENR |= (1U << 4))
#define USART2_PCLK_EN()     (RCC->APB1ENR |= (1U << 17))
#define USART3_PCLK_EN()     (RCC->APB1ENR |= (1U << 18))
#define UART4_PCLK_EN()      (RCC->APB1ENR |= (1U << 19))
#define UART5_PCLK_EN()      (RCC->APB1ENR |= (1U << 20))
#define USART6_PCLK_EN()     (RCC->APB2ENR |= (1U << 5))

/*
 * Clock Enable Macros for SYSCFG Peripherals
 */
#define SYSCFG_PCLK_EN()	(RCC->APB2ENR |= (1U << 14))

/*******************************************************************************************************************************************/


/**********************************************************DISABLE MACROS FOR PERIPHERALS***************************************************/
/*
 * Clock Disable Macros for GPIOx Peripherals
 */
//#define GPIOA_PCLK_DI()     (RCC->AHB1ENR &= ~(1U << 0))
//#define GPIOB_PCLK_DI()     (RCC->AHB1ENR &= ~(1U << 1))
//#define GPIOC_PCLK_DI()     (RCC->AHB1ENR &= ~(1U << 2))
//#define GPIOD_PCLK_DI()     (RCC->AHB1ENR &= ~(1U << 3))
//#define GPIOE_PCLK_DI()     (RCC->AHB1ENR &= ~(1U << 4))
//#define GPIOF_PCLK_DI()     (RCC->AHB1ENR &= ~(1U << 5))
//#define GPIOG_PCLK_DI()     (RCC->AHB1ENR &= ~(1U << 6))
//#define GPIOH_PCLK_DI()     (RCC->AHB1ENR &= ~(1U << 7))

/*
 * Clock Disable Macros for I2Cx Peripherals
 */
#define I2C1_PCLK_DI()      (RCC->APB1ENR &= ~(1U << 21))
#define I2C2_PCLK_DI()      (RCC->APB1ENR &= ~(1U << 22))
#define I2C3_PCLK_DI()      (RCC->APB1ENR &= ~(1U << 23))

/*
 * Clock Disable Macros for SPIx Peripherals
 */
#define SPI1_PCLK_DI()      (RCC->APB2ENR &= ~(1U << 12))
#define SPI2_PCLK_DI()      (RCC->APB1ENR &= ~(1U << 14))
#define SPI3_PCLK_DI()      (RCC->APB1ENR &= ~(1U << 15))
#define SPI4_PCLK_DI()      (RCC->APB2ENR &= ~(1U << 13))
#define SPI5_PCLK_DI()      (RCC->APB2ENR &= ~(1U << 20))

/*
 * Clock Disable Macros for USARTx Peripherals
 */
#define USART1_PCLK_DI()    (RCC->APB2ENR &= ~(1U << 4))
#define USART2_PCLK_DI()    (RCC->APB1ENR &= ~(1U << 17))
#define USART6_PCLK_DI()    (RCC->APB2ENR &= ~(1U << 5))

/*
 * Clock Disable Macros for SYSCFG Peripherals
 */
#define SYSCFG_PCLK_DI()	(RCC->APB2ENR &= ~(1U << 14))

/*******************************************************************************************************************************************/


/*
 * Macro to reset GPIOx peripherals
 */
#define GPIOA_REG_RESET()   do{ (RCC->AHB1RSTR |= (1U << 0)); (RCC->AHB1RSTR &= ~(1U << 0)); }while(0)
#define GPIOB_REG_RESET()   do{ (RCC->AHB1RSTR |= (1U << 1)); (RCC->AHB1RSTR &= ~(1U << 1)); }while(0)
#define GPIOC_REG_RESET()   do{ (RCC->AHB1RSTR |= (1U << 2)); (RCC->AHB1RSTR &= ~(1U << 2)); }while(0)
#define GPIOD_REG_RESET()   do{ (RCC->AHB1RSTR |= (1U << 3)); (RCC->AHB1RSTR &= ~(1U << 3)); }while(0)
#define GPIOE_REG_RESET()   do{ (RCC->AHB1RSTR |= (1U << 4)); (RCC->AHB1RSTR &= ~(1U << 4)); }while(0)
#define GPIOF_REG_RESET()   do{ (RCC->AHB1RSTR |= (1U << 5)); (RCC->AHB1RSTR &= ~(1U << 5)); }while(0)
#define GPIOG_REG_RESET()   do{ (RCC->AHB1RSTR |= (1U << 6)); (RCC->AHB1RSTR &= ~(1U << 6)); }while(0)
#define GPIOH_REG_RESET()   do{ (RCC->AHB1RSTR |= (1U << 7)); (RCC->AHB1RSTR &= ~(1U << 7)); }while(0)


//if else using macro, (condition) ? if :\ means else check for (condition)
#define GPIO_BASEADDR_TO_CODE(GpioPort)   \
( ((GpioPort) == GPIOA)?0: \
  ((GpioPort) == GPIOB)?1: \
  ((GpioPort) == GPIOC)?2: \
  ((GpioPort) == GPIOD)?3: \
  ((GpioPort) == GPIOE)?4: \
  ((GpioPort) == GPIOF)?5: \
  ((GpioPort) == GPIOG)?6: \
  ((GpioPort) == GPIOH)?7:0 )


/*
 * IRQ(Interrupt Request) Numbers of STM32F446xx MCU
 */
#define IRQ_NO_EXTI0                     6   //IRQ number for IRQ_NO_EXTI0
#define IRQ_NO_EXTI1                     7   //IRQ number for IRQ_NO_EXTI1
#define IRQ_NO_EXTI2                     8   //IRQ number for IRQ_NO_EXTI2
#define IRQ_NO_EXTI3                     9   //IRQ number for IRQ_NO_EXTI3
#define IRQ_NO_EXTI4                     10  //IRQ number for IRQ_NO_EXTI4
#define IRQ_NO_EXTI9_5                   23  //IRQ number for IRQ_NO_EXTI9_5
#define IRQ_NO_EXTI15_10                 40  //IRQ number for IRQ_NO_EXTI15_10


/*
 * macro for all the possible priority levels
 */
#define NVIC_IRQ_PRI0                    0
#define NVIC_IRQ_PRI1                    1
#define NVIC_IRQ_PRI2                    2
#define NVIC_IRQ_PRI3                    3
#define NVIC_IRQ_PRI4                    4
#define NVIC_IRQ_PRI5                    5
#define NVIC_IRQ_PRI6                    6
#define NVIC_IRQ_PRI7                    7
#define NVIC_IRQ_PRI8                    8
#define NVIC_IRQ_PRI9                    9
#define NVIC_IRQ_PRI10                   10
#define NVIC_IRQ_PRI11                   11
#define NVIC_IRQ_PRI12                   12
#define NVIC_IRQ_PRI13                   13
#define NVIC_IRQ_PRI14                   14
#define NVIC_IRQ_PRI15                   15



/*
 * some generic macros
 */
#define ENABLE              1
#define DISABLE             0
#define SET                 ENABLE
#define RESET               DISABLE
#define GPIO_PIN_SET        SET
#define GPIO_PIN_RESET      RESET


//GPIO driver specific header file
//#include "stm32f446xx_gpio_driver.h"


#endif /* INC_STM32F446XX_H_ */
