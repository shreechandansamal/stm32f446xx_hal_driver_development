# STM32F446xx Bare Metal Driver Development

🚀 Bare-metal driver development for STM32F446RE without using HAL libraries.
This project focuses on low-level register programming, driver abstraction, and embedded system design.

---

## 📊 Current Status

* Version: **v0.1.0**
* Status: GPIO driver complete
* Next Target: SPI Driver

---

## 📌 Overview

This repository contains a custom driver framework built from scratch for the STM32F446RE MCU.
The goal is to deeply understand hardware-level programming and build reusable embedded drivers.

---

## 🧩 Features (Version-wise)

### ✅ v0.1.0 - GPIO Driver

* Peripheral Clock Control APIs
* GPIO initialization (Input / Output modes)
* Push-Pull / Open-Drain configuration
* Speed configuration
* Pull-up / Pull-down configuration
* GPIO read/write APIs
* Interrupt-based GPIO handling (EXTI)
* Multiple test cases

---

## 🏗️ Project Structure

```
.
├── drivers/        → Peripheral drivers (GPIO, SPI, I2C, USART, etc.)
├── bsp/            → Board Support Package (Nucleo-F446RE)
├── applications/   → Application layer code
├── devices/        → External device drivers (keyboard etc.)
├── mcu/            → MCU-specific headers
├── Startup/        → Startup assembly code
├── test/           → Driver test programs
```

---

## 🎯 Goals

* Understand STM32 registers deeply
* Build reusable driver architecture
* Avoid HAL and write everything from scratch
* Prepare for:

  * SPI
  * I2C
  * USART
  * DMA
  * RTOS integration
  * Bootloader development

---

## 🔖 Versioning

This project follows **Semantic Versioning**:

```
MAJOR.MINOR.PATCH
```

| Version | Description                           |
| ------- | ------------------------------------- |
| v0.1.0  | GPIO driver complete                  |
| v0.x.x  | Development phase                     |
| v1.0.0  | Stable driver framework (future goal) |

---

## 🚀 Future Work

* [x] GPIO Driver
* [ ] SPI Driver
* [ ] I2C Driver
* [ ] USART Driver
* [ ] DMA Support
* [ ] RTOS Integration (FreeRTOS)
* [ ] Bootloader Development

---

## 🧪 Testing

Test cases are available under:

```
test/
```

Includes:

* LED toggle (onboard / external)
* Button input (onboard / external)
* Interrupt-based input handling
* Delay behavior testing for both polling and interrupt modes
* Custom Hardware GPIO based Keyboard for GPIO Input/Output Test 
---

## 🛠️ Build & Run

* Toolchain: ARM GCC
* IDE: STM32CubeIDE (or Makefile-based build)

Steps:

1. Import project into STM32CubeIDE
2. Build the project
3. Flash to STM32F446RE board
4. Run test cases from `test/`

---

## ⚙️ Hardware Used

* STM32F446RE (Nucleo Board)

---

## 📚 Learning Focus

This project focuses on:

* Embedded C programming
* Register-level MCU control
* Driver abstraction design
* Interrupt handling

---

## 👨‍💻 Author

Shree Chandan Samal

---

## ⭐ Notes

This is a learning-driven project.
The codebase will evolve as understanding deepens and new features are added.
