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
#include <shell.h>

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

static const ShellCommand commands[] = {
//  {"mem", cmd_mem},
//  {"threads", cmd_threads},
//  {"test", cmd_test},
//  {"tree", cmd_tree},
  {NULL, NULL}
};
static const ShellConfig shell_cfg1 = {
  (BaseSequentialStream *)&SD1,
  commands
};
#define SHELL_WA_SIZE   THD_WA_SIZE(512)


/*
 * Application entry point.
 */
int main(void) {
	font_t		font;
	GHandle GW1, GW2, GW3;
	int			i;

	SerialConfig	SDConfig;
	Thread *shelltp = NULL;

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
   * Activates the serial driver 1 using the driver default configuration.
   */
  sdStart(&SD1, NULL);

  // 开启蓝牙
  palSetPad(GPIO_BLUETOOTH_PORT, GPIO_BLUETOOTH_BIT);

  /*
   * Shell manager initialization.
   */
  shellInit();

  // 创建Shell线程
  shelltp = shellCreate(&shell_cfg1, SHELL_WA_SIZE, NORMALPRIO);

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

  font = gdispOpenFont("fixed_5x8");
  gdispDrawString(0, 0, "This is test.", font, 1);
  gdispCloseFont(font);
  gdispFlush();

  font = gdispOpenFont("LargeNumbers");
  gdispDrawString(0, 10, "12:30", font, 1);
  gdispCloseFont(font);
  gdispFlush();

  gdispSetPowerMode(powerOff);
  gdispSetPowerMode(powerOn);

  gdispSetContrast(50);
  gdispSetContrast(0);
  gdispSetContrast(100);
  gdispSetContrast(50);

  chThdSleepMilliseconds(3000);

  // test consle
  font = gdispOpenFont("UI2");

  {
	  GWindowInit		wi;

	  wi.show = TRUE;
	  wi.x = 0; wi.y = 0; wi.width = gdispGetWidth(); wi.height = gdispGetHeight();
	  GW1 = gwinConsoleCreate(0, &wi);
  }

  gwinSetFont(GW1, font);
  gwinSetColor(GW1, 1);
  gwinClear(GW1);
  gdispFlush();

  // write console
  i = 0;
  while (TRUE)
  {
	  gwinPrintf(GW1,"Test \033bnumber \033B%d.\r\n", i++);
	  gdispFlush();
	  gfxSleepMilliseconds(1000);
  }

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
