# STM32F446xx Bare Metal Driver Development

🚀 Bare-metal driver development for STM32F446RE without using HAL libraries.
This project focuses on low-level register programming, driver abstraction, interrupt handling, and embedded system design.

---

## 📊 Current Status

* Version: **v0.4.0**
* Status:
  * ✅ GPIO Driver Complete
  * ✅ SPI Driver Complete (Polling + Interrupt APIs)
  * ✅ I2C Driver Complete (Polling + Interrupt APIs)
  * ✅ USART/UART Driver Complete (Polling + Interrupt APIs)

* Next Target: **TIM Driver**

---

## 📌 Overview

This repository contains a custom driver framework built completely from scratch for the STM32F446RE MCU.

The main goal of this project is to:

* Understand STM32 peripherals at register level
* Build reusable and scalable embedded drivers
* Learn industrial-style driver architecture
* Develop strong debugging and low-level programming skills

No HAL or external abstraction libraries are used.

---

## 🧩 Features (Version-wise)

# ✅ v0.1.0 - GPIO Driver

### GPIO Features
* Peripheral Clock Control APIs
* GPIO Initialization APIs
* Input / Output Mode Configuration
* Push-Pull / Open-Drain Configuration
* Speed Configuration
* Pull-up / Pull-down Configuration
* GPIO Read/Write APIs
* Interrupt-Based GPIO Handling (EXTI)
* IRQ Priority Configuration
* Multiple GPIO Test Applications

---

# ✅ v0.2.0 - SPI Driver

### SPI Core Features
* SPI Peripheral Initialization
* Master / Slave Mode
* Full Duplex / Half Duplex / Simplex RX Only
* Clock Polarity (CPOL)
* Clock Phase (CPHA)
* Software Slave Management (SSM)
* Hardware NSS Output Management (SSOE)
* Configurable Baud Rate Prescaler
* 8-bit and 16-bit Data Frame Support

### SPI Polling APIs
* Blocking Transmit API
* Blocking Receive API
* Full-Duplex Transmit/Receive API
* Busy Flag Handling
* Proper TXE/RXNE Synchronization

### SPI Interrupt APIs
* Interrupt-Based Transmission
* Interrupt-Based Reception
* TXE Interrupt Handling
* RXNE Interrupt Handling
* Overrun (OVR) Error Interrupt Handling
* IRQ Enable/Disable APIs
* NVIC Priority Configuration APIs
* Application Callback Mechanism

### SPI Driver Improvements
* Proper Volatile Register Access
* Explicit 8-bit and 16-bit Register Access Handling
* Even-Length Validation for 16-bit Transfers
* Safe Interrupt Close/Reset Handling
* OVR Flag Clearing Sequence
* Improved Register-Level Documentation

---

# ✅ v0.3.0 - I2C Driver

### I2C Core Features
* I2C Peripheral Initialization
* Peripheral Clock Management
* Peripheral Enable/Disable Control
* Standard Mode (100 kHz) Support
* Fast Mode (400 kHz) Support
* Fast Mode Duty Cycle Configuration
* Own Address Configuration
* ACK Enable/Disable Control
* START Condition Generation
* STOP Condition Generation
* Address Phase Management
* APB1 Clock-Based Timing Calculation
* CCR Calculation
* TRISE Calculation

### I2C Polling APIs
* Master Transmit (Blocking)
* Master Receive (Blocking)
* Single-Byte Reception Handling
* Multi-Byte Reception Handling
* ACK/NACK Management
* Repeated START Support
* Address Flag Handling
* TXE/RXNE Polling
* BTF Synchronization
* Communication Completion Handling

### I2C Interrupt APIs
* Master Transmit (Interrupt Mode)
* Master Receive (Interrupt Mode)
* Event Interrupt Handling
* Error Interrupt Handling
* TXE Interrupt Processing
* RXNE Interrupt Processing
* SB Event Handling
* ADDR Event Handling
* BTF Event Handling
* STOPF Event Handling

### I2C Error Handling
* Bus Error (BERR)
* Arbitration Lost (ARLO)
* Acknowledge Failure (AF)
* Overrun/Underrun (OVR)
* Timeout Detection
* Application Error Callback Notification

### I2C Slave Features
* Slave Transmit Support
* Slave Receive Support
* Slave Event Callback Mechanism
* Data Request Callback
* Data Receive Callback
* STOP Detection Callback

### I2C Driver Improvements
* STM32-Compliant ADDR Flag Clearing
* Proper Single-Byte Receive Sequence
* Correct ACK/NACK Timing Control
* Repeated START Transaction Support
* Interrupt-Based State Machine
* Safe Transmission/Reception Close Handling
* Application Callback Architecture
* Enhanced Documentation and Register-Level Comments

---

# ✅ v0.4.0 - USART / UART Driver

### USART Core Features
* USART Peripheral Initialization
* Peripheral Clock Management
* Peripheral Enable/Disable Control
* Baud Rate Configuration
* APB1/APB2 Clock Based Baud Rate Calculation
* Oversampling by 8 Support
* Oversampling by 16 Support
* TX Only / RX Only / TX-RX Modes
* Configurable Stop Bits
* 8-bit Word Length Support
* 9-bit Word Length Support
* Even Parity Support
* Odd Parity Support
* CTS Flow Control
* RTS Flow Control
* CTS/RTS Combined Flow Control

### USART Polling APIs
* Blocking Transmission API
* Blocking Reception API
* TXE Flag Synchronization
* RXNE Flag Synchronization
* Transmission Complete (TC) Handling
* 8-bit Data Transfers
* 9-bit Data Transfers
* Parity-Aware Data Handling

### USART Interrupt APIs
* Interrupt-Based Transmission
* Interrupt-Based Reception
* TXE Interrupt Handling
* TC Interrupt Handling
* RXNE Interrupt Handling
* IRQ Enable/Disable APIs
* NVIC Priority Configuration APIs
* Application Callback Mechanism
* Non-Blocking Communication Support

### USART Event Handling
* Transmission Complete Event
* Reception Complete Event
* CTS Event Detection
* IDLE Line Detection Event

### USART Error Handling
* Overrun Error (ORE)
* Framing Error (FE)
* Noise Error (NF)
* Error Callback Notification

### USART Driver Improvements
* Accurate BRR Register Calculation
* APB1/APB2 Aware Baud Generation
* Support for Oversampling by 8 and 16
* Proper 8-bit and 9-bit Data Access
* Safe Interrupt State Management
* Automatic Interrupt Disable on Completion
* RXNE Flush Utility for Stale Data Handling
* Enhanced Register-Level Documentation

---

## 🏗️ Project Structure

```text
.
├── drivers/        → Peripheral drivers (GPIO, SPI, I2C, USART, TIM, etc.)
├── bsp/            → Board Support Package (Nucleo-F446RE)
├── applications/   → Application layer code
├── devices/        → External device drivers
├── mcu/            → MCU-specific headers
├── Startup/        → Startup assembly code
├── test/           → Driver test programs
```

---

## 🎯 Goals

* Understand STM32 registers deeply
* Build reusable driver architecture
* Avoid HAL and write everything from scratch
* Improve interrupt-driven peripheral handling
* Learn industrial embedded driver design

Prepare drivers for:
* GPIO
* SPI
* I2C
* USART
* TIM
* PWM
* ADC
* CAN
* DMA
* RTOS Integration
* Bootloader Development

---

## 🔖 Versioning

This project follows **Semantic Versioning**:

```text
MAJOR.MINOR.PATCH
```

| Version | Description |
|----------|-------------|
| v0.1.0 | GPIO Driver Complete |
| v0.2.0 | SPI Driver Added |
| v0.3.0 | I2C Driver Added |
| v0.4.0 | USART/UART Driver Added |
| v1.0.0 | Stable Driver Framework (Future Goal) |

---

## 🚀 Future Work

* [x] GPIO Driver
* [x] SPI Driver
* [x] I2C Driver
* [x] USART/UART Driver
* [ ] SPI Driver Refactor (Industrial-Grade IT/DMA Architecture)
* [ ] Timer Driver
* [ ] PWM Driver
* [ ] ADC Driver
* [ ] DMA Support
* [ ] RTOS Integration (FreeRTOS)
* [ ] Bootloader Development

---

## 🧪 Testing

Test applications are available under:

```text
test/
```

### Test Directory

```text
test
├── 001_led_toggle_pp_od_gpio.c
├── 002_inbuilt_led_button_gpio.c
├── 003_external_led_button_gpio.c
├── 004_button_interrupt_gpio.c
├── 005_interrupt_delay_halt_test_gpio.c
├── 006_spi_tx_testing.c
├── 007_spi_master_tx_rx_fd.c
├── 008_spi_slave_rx_tx_fd.c
├── 009_spi_master_tx_rx_fd_it.c
├── 010_spi_slave_rx_tx_fd_it.c
├── 011_i2c_master_tx_testing.c
├── 012_i2c_master_rx_testing.c
├── 013_i2c_master_rx_testing_it.c
├── 014_i2c_slave_tx_string.c
├── 015_i2c_slave_tx_string2.c
├── 016_uart_tx.c
├── 017_uart_case_exchange.c
├── AHT10_I2C.c
├── rtc_app.c 
├── gpio_app_keyboard_test.c
└── RFID-RC522_SPI.c
```

### GPIO Tests

| Test File                            | Description                                   |
| ------------------------------------ | --------------------------------------------- |
| 001_led_toggle_pp_od_gpio.c          | Push-Pull and Open-Drain output validation    |
| 002_inbuilt_led_button_gpio.c        | On-board button and LED interaction           |
| 003_external_led_button_gpio.c       | External button and LED interfacing           |
| 004_button_interrupt_gpio.c          | GPIO interrupt handling using EXTI            |
| 005_interrupt_delay_halt_test_gpio.c | Interrupt latency and blocking behavior study |
| gpio_app_keyboard_test.c             | Matrix keyboard GPIO application testing      |

### SPI Tests

| Test File                    | Description                             |
| ---------------------------- | --------------------------------------- |
| 006_spi_tx_testing.c         | Basic SPI transmission testing          |
| 007_spi_master_tx_rx_fd.c    | Full-duplex master communication        |
| 008_spi_slave_rx_tx_fd.c     | Full-duplex slave communication         |
| 009_spi_master_tx_rx_fd_it.c | Interrupt-driven master communication   |
| 010_spi_slave_rx_tx_fd_it.c  | Interrupt-driven slave communication    |
| RFID-RC522_SPI.c             | RFID-RC522 module interfacing using SPI |

### I2C Tests

| Test File                      | Description                                                         |
| ------------------------------ | ------------------------------------------------------------------- |
| 011_i2c_master_tx_testing.c    | Master transmit operation validation                                |
| 012_i2c_master_rx_testing.c    | Master receive operation validation                                 |
| 013_i2c_master_rx_testing_it.c | Interrupt-driven master receive testing                             |
| 014_i2c_slave_tx_string.c      | Slave transmit functionality                                        |
| 015_i2c_slave_tx_string2.c     | Advanced slave transmit testing                                     |
| AHT10_I2C.c                    | Real sensor interfacing using AHT10 temperature and humidity sensor |
| rtc_app.c                      | Real time clock interfacing using DS1307, bsp, device driver added  |

### USART and UART Tests

| Test File                      | Description                                                         |
| ------------------------------ | ------------------------------------------------------------------- |
| 016_uart_tx.c                  | polling transmit operation validation                               |
| 017_uart_case_exchange.c       | interrupt receive operation validation                              |

### Validation Coverage

#### GPIO

* Input and Output Modes
* Push-Pull Configuration
* Open-Drain Configuration
* External Interrupt Handling
* Button Debouncing Experiments
* GPIO-Based Keyboard Interface

#### SPI

* Polling-Based Communication
* Interrupt-Based Communication
* Master Mode Operation
* Slave Mode Operation
* Full-Duplex Data Transfer
* External Device Communication (RC522 RFID)

#### I2C

* Master Transmit Operations
* Master Receive Operations
* Interrupt-Based Reception
* Slave Transmit Operations
* ACK/NACK Handling
* START and Repeated START Sequences
* Real Sensor Communication (AHT10)
* Multi-Byte Data Transfers
* Event and Error Interrupt Validation

#### USART
* Polling Transmission
* Polling Reception
* Interrupt-Based Communication
* TXE/TC/RXNE Event Handling
* Parity Handling
* Error Event Validation

---

## 🛠️ Build & Run

### Toolchain
* ARM GCC

### IDE
* STM32CubeIDE
* Makefile-Based Builds Possible

### Steps
1. Import project into STM32CubeIDE
2. Build the project
3. Flash to STM32F446RE board
4. Run test applications from `test/`

---

## ⚙️ Hardware Used

* STM32F446RE (Nucleo Board)

---

## 📚 Learning Focus

* Embedded C Programming
* Register-Level MCU Control
* Pointer and Memory Manipulation
* Peripheral Driver Abstraction
* Interrupt Handling
* Embedded Debugging
* Bare-Metal Architecture Design
* Hardware Register Access Patterns

---

## 🧠 Key Concepts Practiced

* Memory-Mapped I/O
* Volatile Register Access
* Pointer Casting
* Register Bit Manipulation
* Interrupt-Driven Communication
* Peripheral State Management
* SPI Communication Protocol
* I2C Communication Protocol
* USART Communication Protocol
* ACK/NACK Handling
* Hardware/Software Synchronization
* Embedded API Design

---

## 👨‍💻 Author

Shree Chandan Samal

---

## ⭐ Notes

This is a learning-driven project focused on understanding embedded systems deeply through practical driver development.

The codebase will continuously evolve as new peripherals, improved architectures, advanced debugging techniques, and industrial-grade embedded concepts are explored.
