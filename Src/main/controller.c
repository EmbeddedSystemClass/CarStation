/*
 * controller.c
 *
 *  Created on: 2014年11月18日
 *      Author: daniel
 */


#include "controller.h"
#include "Msg/Msg.h"
#include "const.h"

#include <time.h>

// Mailbox定义
#define MAIN_MB_SIZE			20
static void*	main_mb_buffer[MAIN_MB_SIZE];

MAILBOX_DECL(main_mb, main_mb_buffer, MAIN_MB_SIZE);

// 所有的系统用的变量都存放在这里
// RTC时间
static uint32_t		s_RTC = 0;
static uint32_t		s_LastMinute = 0;

// 温度（放大100倍）、湿度（放大100倍）、气压
static	int16_t		s_Temperature_in	= INVALID_TEMPERATURE;
static int16_t		s_Humidity_in		= INVALID_HUMIDITY;
static int16_t		s_Temperature_out	= INVALID_TEMPERATURE;
static int16_t		s_Humidity_out		= INVALID_HUMIDITY;

// GPS坐标，车速，定位状态，卫星个数

// 光照Lux
static uint16_t		s_Light				= 0;

// 电源状态

// 门状态

// 指南针、陀螺仪、加速计数据


void MsgRTCSecond(Msg* msg)
{
	uint32_t		minute;

	s_RTC = msg->Param.RTCSecond.time;

	// 判断是否分钟发生变化，如果有变化，则通知UI刷新
	minute = s_RTC / 60;
	if (minute != s_LastMinute)
	{
		// 分配UI消息，发送UI消息
		struct tm		now;
		Msg*			msg;
		msg_t			err;

		localtime_r((time_t*)&(s_RTC), &now);

		msg = MSG_NEW;
		if (msg)
		{
			msg->Id = MSG_UI_CLOCK;
			msg->Param.UIClock.Year 	= now.tm_year;
			msg->Param.UIClock.Month 	= now.tm_mon;
			msg->Param.UIClock.Day 		= now.tm_mday;
			msg->Param.UIClock.Hour		= now.tm_hour;
			msg->Param.UIClock.Minute 	= now.tm_min;

			err = GUI_MSG_SEND(msg);

			if (err == RDY_OK)
			{
				s_LastMinute = minute;
			}
			else
			{
				MSG_FREE(msg);
			}
		}
	}

	// 执行主定时器任务
	// TODO
}

void MsgGPS(Msg* msg)
{
}

//#define HUMIDITY_CONVERT(h) (double)(-6.0 + (125 * (double)h) / 65536.0) * 100
#define HUMIDITY_CONVERT(h) (-600 + 3125 * h / 16384)
//#define TEMPERATURE_CONVERT(t) (double)(-46.85 + (double)(175.72 * (double)t) / 65536.0) * 100
#define TEMPERATURE_CONVERT(t) (-4685 + (4393 * t) / 16384)

void MsgSHT21(Msg* msg)
{
	int16_t		temperature, humidity;
	int16_t		*c_pTemperature, *c_pHumidity;
	enumMsg		UImsgId;

	if (msg->Id == MSG_SHT21_INSIDE)
	{
		c_pTemperature 	= &s_Temperature_in;
		c_pHumidity		= &s_Humidity_in;
		UImsgId			= MSG_UI_TANDH_IN;
	}
	else if (msg->Id == MSG_SHT21_OUTSIDE)
	{
		c_pTemperature 	= &s_Temperature_out;
		c_pHumidity 	= &s_Humidity_out;
		UImsgId			= MSG_UI_TANDH_OUT;
	}
	else
	{
		return;
	}

	// 计算温度和湿度到正常值
	temperature = (int16_t)TEMPERATURE_CONVERT(msg->Param.SHT21Data.Temperature);
	humidity = (int16_t)HUMIDITY_CONVERT(msg->Param.SHT21Data.Humidity);

	// 判断是否有变化
	if ((temperature != *c_pTemperature) || (humidity != *c_pHumidity))
	{
		msg_t	err;
		Msg*	msg;

		msg = MSG_NEW;
		if (msg)
		{
			msg->Id = UImsgId;
			msg->Param.TandH.Temperature = temperature;
			msg->Param.TandH.Humidity = humidity;

			err = GUI_MSG_SEND(msg);
			if (err)
			{
				s_Temperature_in = temperature;
				s_Humidity_in = humidity;
			}
			else
			{
				MSG_FREE(msg);
			}
		}
	}

}

void MsgSHT21outside(Msg* msg)
{
}

void MsgPower(Msg* msg)
{
}

void MsgDoor(Msg* msg)
{
}

void MsgLight(Msg* msg)
{
	uint16_t		light;

	light = msg->Param.Light.Light;

	if (s_Light != light)
	{
		msg_t		err;
		Msg*		msg;

		msg = MSG_NEW;
		if (msg)
		{
			msg->Id = MSG_UI_LIGHT;
			msg->Param.Light.Light = light;

			err = GUI_MSG_SEND(msg);
			if (err)
			{
				s_Light = light;
			}
			else
			{
				MSG_FREE(msg);
			}
		}
	}
}

static struEntry	Entries[] =
{
	{MSG_RTC_SECOND,		MsgRTCSecond},
	{MSG_GPS,				MsgGPS},
	{MSG_SHT21_INSIDE,		MsgSHT21},
	{MSG_SHT21_OUTSIDE,		MsgSHT21},
	{MSG_POWER,				MsgPower},
	{MSG_DOOR,				MsgDoor},
	{MSG_LIGHT,				MsgLight},
};

// 主处理循环，使用主线程（不需要独立开启一个线程）
void controller_entry(void)
{
	msg_t			ret, msg;
	unsigned int	i;

	// 循环读取Mailbox，然后处理
	while(TRUE) {
		ret = chMBFetch(&main_mb, &msg, MS2ST(100));

		if (ret == RDY_OK)
		{
			// 对消息进行处理
			for (i = 0; i < (sizeof(Entries) / sizeof(Entries[0])); i++)
			{
				if (Entries[i].MsgId == ((Msg*)msg)->Id)
				{
					Entries[i].Entry((Msg*)msg);
					break;
				}
			}

			// 完成处理后，释放Msg
			MSG_FREE(msg);
		}
	}
}


