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
#include "gfx.h"
//#include "test.h"

/*
 * Red LED blinker thread, times are in milliseconds.
 */
static WORKING_AREA(waThread1, 128);
static msg_t Thread1(void *arg) {

  (void)arg;
  chRegSetThreadName("blinker");
  while (TRUE) {
    palClearPad(GPIO_LED_INTERNAL_PORT, GPIO_LED_INTERNAL_BIT);
    chThdSleepMilliseconds(500);
    palSetPad(GPIO_LED_INTERNAL_PORT, GPIO_LED_INTERNAL_BIT);
    chThdSleepMilliseconds(500);
  }

  return 0;
}

/*
 * Application entry point.
 */
int main(void) {
	font_t		font;

  /*
   * System initializations.
   * - HAL initialization, this also initializes the configured device drivers
   *   and performs the board-specific initializations.
   * - Kernel initialization, the main() function becomes a thread and the
   *   RTOS is active.
   */
  halInit();
  chSysInit();

  palClearPad(GPIO_LED_INTERNAL_PORT, GPIO_LED_INTERNAL_BIT);
  palSetPad(GPIO_LED_INTERNAL_PORT, GPIO_LED_INTERNAL_BIT);

  /*
   * Activates the serial driver 2 using the driver default configuration.
   */
  sdStart(&SD2, NULL);

  /*
   * Creates the blinker thread.
   */
  chThdCreateStatic(waThread1, sizeof(waThread1), NORMALPRIO, Thread1, NULL);


  // 测试gfx
  gfxInit();
  //gdispDrawLine(0, 0, 0, 50, 1);
  //gdispFlush();
  //gdispDrawLine(50, 50, 50, 0, 1);
  //gdispFlush();

  font = gdispOpenFont("UI2");
  gdispDrawString(10, 10, "This is test.", font, 1);

  //gdispDrawLine(0, 0, 127, 63, 1);
  gdispFlush();


	while(TRUE) {
		chThdSleepMilliseconds(250);
	}

  /*
   * Normal main() thread activity, in this demo it does nothing except
   * sleeping in a loop and check the button state.
   */
  while (TRUE) {
	  /*
    if (palReadPad(GPIOA, GPIOA_BUTTON))
      TestThread(&SD2);
    chThdSleepMilliseconds(500);
    */
  }
}