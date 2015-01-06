/*
 * myRTC.c
 *
 *  Created on: Jan 6, 2015
 *      Author: daniel
 */

#include "myRTC.h"
#include <hal.h>
#include <Msg/Msg.h>

static void rtc_cb(RTCDriver *rtcp, rtcevent_t event)
{
	RTCTime		timespec;
	Msg*		msg;

	switch (event)
	{
	case RTC_EVENT_OVERFLOW:
	case RTC_EVENT_ALARM:
		break;
	case RTC_EVENT_SECOND:
		// 读取RTC，每秒发送一次（判断分钟由主处理模块完成，主处理模块还利用这个1秒消息构建一个秒级的定时器）
		chSysLockFromIsr();
		rtcGetTimeI(rtcp, &timespec);
		msg = MSG_NEW_I;
		if (msg)
		{
			msg->Id = MSG_RTC_SECOND;
			msg->Param.RTCSecond.time = timespec.tv_sec;
			MSG_SEND(msg);
		}

		// 触发Event，用于传感器测量线程定时工作，采集数据给出处理线程
		// TODO

		break;

	default:
		break;
	}
}

bool_t InitRTC(void)
{
	// Set RTC callback
	rtcSetCallback(&RTCD1, rtc_cb);
	return true;
}



