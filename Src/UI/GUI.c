/*
 * GUI.c
 *
 *  Created on: 2014年11月18日
 *      Author: daniel
 */

#include "GUI.h"

// GUI工作线程
static WORKING_AREA(guiThread, 256);
__attribute__((noreturn))
static msg_t gui_Thread(void *arg)
{
	// 初始化gfx
	gfxInit();

	// 循环处理消息
	while (true)
	{
		//
	}
	//chThdExit(0);
}

bool_t InitGUI(void)
{
	//创建GUI线程，由GUI线程进行初始化和后续的工作
	chThdCreateStatic(guiThread, sizeof(guiThread), NORMALPRIO, gui_Thread, NULL);
	return true;
}


void test(void)
{
	font_t		font;
	GHandle GW1, GW2, GW3;
	int			i;

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
}
