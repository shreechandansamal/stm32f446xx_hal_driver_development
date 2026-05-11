# STM32F446xx Bare Metal Driver Development

🚀 Bare-metal driver development for STM32F446RE without using HAL libraries.
This project focuses on low-level register programming, driver abstraction, interrupt handling, and embedded system design.

---

## 📊 Current Status

* Version: **v0.2.0**
* Status:

  * ✅ GPIO Driver Complete
  * ✅ SPI Driver (Polling + Interrupt APIs)
* Next Target: **I2C Driver**

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
* GPIO initialization APIs
* Input / Output mode configuration
* Push-Pull / Open-Drain configuration
* Speed configuration
* Pull-up / Pull-down configuration
* GPIO read/write APIs
* Interrupt-based GPIO handling (EXTI)
* IRQ priority configuration
* Multiple GPIO test applications

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
* Configurable baud-rate prescaler
* 8-bit and 16-bit Data Frame Format support

---

### SPI Polling APIs

* Blocking transmit API
* Blocking receive API
* Full-duplex transmit/receive API
* Busy flag handling
* Proper TXE/RXNE synchronization

---

### SPI Interrupt APIs

* Interrupt-based transmission
* Interrupt-based reception
* TXE interrupt handling
* RXNE interrupt handling
* Overrun (OVR) error interrupt handling
* IRQ enable/disable APIs
* NVIC priority configuration APIs
* Application callback mechanism

---

### SPI Driver Improvements

* Proper volatile register access for SPI DR register
* Explicit 8-bit and 16-bit register access handling
* Even-length validation for 16-bit transfers
* Safe interrupt close/reset handling
* OVR flag clearing sequence implemented
* Better pointer casting and register access clarity
* Improved register-level comments/documentation

---

## 🏗️ Project Structure

```text
.
├── drivers/        → Peripheral drivers (GPIO, SPI, I2C, USART, etc.)
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
* Prepare for:

  * GPIO
  * SPI
  * I2C
  * USART
  * TIM
  * PWM
  * ADC
  * CAN
  * Low Power
  * DMA
  * RTOS integration
  * Bootloader development
  * more..

---

## 🔖 Versioning

This project follows **Semantic Versioning**:

```text
MAJOR.MINOR.PATCH
```

| Version | Description                           |
| ------- | ------------------------------------- |
| v0.1.0  | GPIO driver complete                  |
| v0.2.0  | SPI polling + interrupt driver added  |
| v0.x.x  | Development phase                     |
| v1.0.0  | Stable driver framework (future goal) |

---

## 🚀 Future Work

* [x] GPIO Driver
* [x] SPI Driver
* [ ] SPI Driver Refactor (Industrial-grade IT/DMA architecture)
* [ ] I2C Driver
* [ ] USART Driver
* [ ] DMA Support
* [ ] RTOS Integration (FreeRTOS)
* [ ] Bootloader Development

---

## 🧪 Testing

Test cases are available under:

```text
test/
```

### Current Test Coverage

#### GPIO Tests

* LED toggle (onboard / external)
* Button input handling
* External interrupt handling
* Pull-up / pull-down validation
* GPIO speed tests

#### SPI Tests

* SPI polling-based transmission
* SPI interrupt-based transmission
* Full-duplex communication testing
* Master-slave communication
* Hardware NSS handling
* OVR error handling
* Data reception using interrupts

---

## 🛠️ Build & Run

### Toolchain

* ARM GCC

### IDE

* STM32CubeIDE
* Makefile-based builds possible

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

This project focuses on:

* Embedded C programming
* Register-level MCU control
* Pointer and memory manipulation
* Peripheral driver abstraction
* Interrupt handling
* Embedded debugging
* Bare-metal architecture design
* Hardware register access patterns

---

## 🧠 Key Concepts Practiced

* Memory mapped IO
* Volatile register access
* Pointer casting
* Register bit manipulation
* Interrupt-driven communication
* Peripheral state management
* Full duplex SPI communication
* Half duplex SPI communication
* Hardware/software synchronization
* Embedded API design

---

## 👨‍💻 Author

Shree Chandan Samal

---

## ⭐ Notes

This is a learning-driven project focused on understanding embedded systems deeply through practical driver development.

The codebase will continuously evolve as new peripherals, better architectures, and advanced embedded concepts are explored.

