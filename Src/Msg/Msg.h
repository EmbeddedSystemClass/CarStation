/*
 * Msg.h
 *
 *  Created on: 2014年11月18日
 *      Author: daniel
 *      消息格式定义
 */

#ifndef MSG_H_
#define MSG_H_

#include <ch.h>

typedef union
{
	//Power voltage
	struct	Msg_Power
	{
		uint16_t	CarBattery;
		uint16_t	LionBattery;
		bool_t		IsPoweron;
		bool_t		IsCharging;
	}	PowerVoltage;

	struct Msg_DoorOpen
	{
		bool_t		IsOpen;
	}	DoorOpen;
} Msg;

#endif /* MSG_H_ */
