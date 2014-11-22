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

#include "ch.h"
#include "hal.h"

#include "SDCard/SDCard.h"
#include "GPS/GPS.h"
#include "shell/car_sh.h"
#include "I2C/myi2c.h"
#include "power/power.h"

//#include "test.h"

/*
 * Red LED blinker thread, times are in milliseconds.
 */
static WORKING_AREA(waThread1, 64);
__attribute__((noreturn))
static msg_t Thread1(void *arg) {

  (void)arg;
  chRegSetThreadName("blinker");
  while (TRUE) {
    palClearPad(GPIO_LED_INTERNAL_PORT, GPIO_LED_INTERNAL_BIT);
    chThdSleepMilliseconds(500);
    palSetPad(GPIO_LED_INTERNAL_PORT, GPIO_LED_INTERNAL_BIT);
    chThdSleepMilliseconds(500);
  }
}



/*
 * Application entry point.
 */
int main(void) {
	/*
	* System initializations.
	* - HAL initialization, this also initializes the configured device drivers
	*   and performs the board-specific initializations.
	* - Kernel initialization, the main() function becomes a thread and the
	*   RTOS is active.
	*/
	halInit();
	chSysInit();

	// 初始化SD Card和mount文件系统
	InitSDCard();

	// 初始化Shell
	InitShell();
	EnableBluetooth(true);

	/*
	* Creates the blinker thread.
	*/
	chThdCreateStatic(waThread1, sizeof(waThread1), NORMALPRIO, Thread1, NULL);

	// 初始化GPS模块
	InitGPS();

	// 初始化电源监控部分
	InitPower();

	// 初始化I2C总线和相关设备
	InitI2C();



	while(TRUE) {
		chThdSleepMilliseconds(250);
	}
}
