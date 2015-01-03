/*
 * SensorsThread.c
 *
 *  Created on: Jan 3, 2015
 *      Author: daniel
 */


#include "SensorsThread.h"


static WORKING_AREA(sensorsThread, 128);
static msg_t sensors_Thread(void *arg);

bool_t InitSensorsThread(void)
{
	bool_t		bRet;

	bRet = FALSE;

	// 创建传感器测量线程
	chThdCreateStatic(sensorsThread, sizeof(sensorsThread), NORMALPRIO, sensors_Thread, NULL);

	return bRet;
}

static msg_t sensors_Thread(void *arg)
{
	// 循环定时读取各个传感器，然后将变化的数据Post到主线程
	while (TRUE)
	{
		// Read every 2 seconds
		chThdSleepMilliseconds(2000);

		// Power(12v/5v/battery/door status)

		// SH21(temperature, humidity)(inside and outside)

		// BH1750 (light)

		//
	}

	return 0;
}

