/*
    ChibiOS/RT - Copyright (C) 2006-2013 Giovanni Di Sirio

    Licensed under the Apache License, Version 2.0 (the "License");
    you may not use this file except in compliance with the License.
    You may obtain a copy of the License at

        http://www.apache.org/licenses/LICENSE-2.0

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.
*/

#ifndef _BOARD_H_
#define _BOARD_H_

/*
 * Setup for the Chenjianqing's CarStation board.
 */

#define GPIO_LED_INTERNAL_BIT		5
#define GPIO_LED_INTERNAL_PORT		GPIOC

#define GPIO_LED_RED_BIT			6
#define GPIO_LED_RED_PORT			GPIOC

#define GPIO_LED_GREEN_BIT			7
#define GPIO_LED_GREEN_PORT			GPIOC

#define GPIO_GPS_BIT				1
#define GPIO_GPS_PORT				GPIOA

#define GPIO_BLUETOOTH_BIT			8
#define GPIO_BLUETOOTH_PORT			GPIOA


/*
 * Board identifier.
 */
#define BOARD_NAME              "My CarStation"

/*
 * Board frequencies.
 */
#define STM32_LSECLK            32768
#define STM32_HSECLK            8000000

/*
 * MCU type, supported types are defined in ./os/hal/platforms/hal_lld.h.
 */
#define STM32F10X_HD

/*
 * IO pins assignments.
 * *********************TO BE COMPLETED*********************
 */

/*
 * I/O ports initial setup, this configuration is established soon after reset
 * in the initialization code.
 *
 * The digits have the following meaning:
 *   0 - Analog input.
 *   1 - Push Pull output 10MHz.
 *   2 - Push Pull output 2MHz.
 *   3 - Push Pull output 50MHz.
 *   4 - Digital input.
 *   5 - Open Drain output 10MHz.
 *   6 - Open Drain output 2MHz.
 *   7 - Open Drain output 50MHz.
 *   8 - Digital input with PullUp or PullDown resistor depending on ODR.
 *   9 - Alternate Push Pull output 10MHz.
 *   A - Alternate Push Pull output 2MHz.
 *   B - Alternate Push Pull output 50MHz.
 *   C - Reserved.
 *   D - Alternate Open Drain output 10MHz.
 *   E - Alternate Open Drain output 2MHz.
 *   F - Alternate Open Drain output 50MHz.
 * Please refer to the STM32 Reference Manual for details.
 */

/*
 * Port A setup.
 * Everything input except:
 * PA1 - GPS Enable (output)
 * PA2 - USART2 TX GPS
 * PA3 - USART2 RX GPS
 * PA4 - OLED DC (output)
 * PA5 - SPI1 SCK OLED
 * PA6 - OLED RST (output)
 * PA7 - SPI1 MOSI OLED
 * PA8 - Bluetooth RESET (output)
 * PA9 - USART1 TX Bluetooth
 * PA10- USART1 RX Bluetooth
 * PA11- Reserve
 * PA12- Reserve
 */
#define VAL_GPIOACRL            0xB3B38B34      /*  PA7...PA0 */
#define VAL_GPIOACRH            0x444448B3      /* PA15...PA8 */
#define VAL_GPIOAODR            0xFFFFFEFD		 /* GPS Disable,Bluetooth Disable*/

/*
 * Port B setup.
 * Everything input except:
 * PB0 - 左边按键（蓝色引线）（input）
 * PB1 - 右边按键（绿色引线）（input）
 * PB6 - I2C1 SCL
 * PB7 - I2C1 SDA
 * PB10- I2C2 SCL
 * PB11- I2C2 SDA
 * PB8 - 充满指示LED（input）
 * PB14- 蓝牙配对LED（input）
 */
#define VAL_GPIOBCRL            0xFF444444      /*  PB7...PB0 */
#define VAL_GPIOBCRH            0x4444FF44      /* PB15...PB8 */
#define VAL_GPIOBODR            0xFFFFFFFF

/*
 * Port C setup.
 * Everything input except:
 * PC0 - ADC1-10 12v汽车电池（直通）
 * PC1 - ADC1-11 4.7v锂电池
 * PC2 - ADC1-12 12v电门（汽车钥匙在ON位置后才有电）
 * PC3 - 门灯信号（开门 低压？）（暂时使用输入，如果信号不能为低电平，则需要使用AD转换方式检测）
 * PC4 - 电源变换模块12-5 enable(output)
 * PC5 - LED(Internal)(output）
 * PC6 - LED(Red)（output）
 * PC7 - LED(Green)（output）
 * PC8 - SDC(CK)
 * PC9 - SDC(D0)
 * PC10- SDC(D1)
 * PC11- SDC(D2)
 * PC12- SDC(D3)
 */
#define VAL_GPIOCCRL            0x33334000      /*  PC7...PC0 */
#define VAL_GPIOCCRH            0x444BBBBB      /* PC15...PC8 */
#define VAL_GPIOCODR            0xFFFFFFFF

/*
 * Port D setup.
 * Everything input except:
 * PD2 - SDC(CMD)

 */
#define VAL_GPIODCRL            0x44444444      /*  PD7...PD0 */
#define VAL_GPIODCRH            0x44444444      /* PD15...PD8 */
#define VAL_GPIODODR            0xFFFFFFFF

/*
 * Port E setup.
 * Everything input except:
 */
#define VAL_GPIOECRL            0x44444444      /*  PE7...PE0 */
#define VAL_GPIOECRH            0x44444444      /* PE15...PE8 */
#define VAL_GPIOEODR            0xFFFFFFFF

/*
 * Port F setup.
 * Everything input except:
 */
#define VAL_GPIOFCRL            0x44444444      /*  PF7...PF0 */
#define VAL_GPIOFCRH            0x44444444      /* PF15...PF8 */
#define VAL_GPIOFODR            0xFFFFFFFF

#define VAL_GPIOGCRL            0x44444444
#define VAL_GPIOGCRH            0x44444444
#define VAL_GPIOGODR            0xFFFFFFFF

#if !defined(_FROM_ASM_)
#ifdef __cplusplus
extern "C" {
#endif
  void boardInit(void);
#ifdef __cplusplus
}
#endif
#endif /* _FROM_ASM_ */

#endif /* _BOARD_H_ */
