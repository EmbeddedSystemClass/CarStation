/*
 * GPS.c
 *
 *	GPS串口处理，判断是否定位，获取时间、经纬度坐标、速度、方向
 *  Created on: 2014年9月14日
 *      Author: daniel
 */

#include <hal.h>
#include "GPS.h"

bool_t InitGPS(void)
{
	bool_t	bRet;

	bRet = CH_SUCCESS;

	return bRet;
}
void EnableGPS(bool_t bEnable)
{
	if (bEnable)
	{
		// 开启GPS模块
		palSetPad(GPIO_GPS_PORT, GPIO_GPS_BIT);
	}
	else
	{
		palClearPad(GPIO_GPS_PORT, GPIO_GPS_BIT);
	}
}

