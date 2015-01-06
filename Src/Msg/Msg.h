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

// 消息ID，主处理线程和GUI处理线程，以及其他处理线程，共用一个消息枚举，消息内存也一起合并定义
typedef enum
{
	MSG_RTC_SECOND,
	MSG_GPS,
	MSG_SHT21_INSIDE,
	MSG_SHT21_OUTSIDE,
	MSG_POWER,
	MSG_DOOR,
	MSG_LIGHT,

	// GUI
	MSG_UI_CLOCK,
} enumMsg;

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

	// 温度和湿度
	struct Msg_SHT21
	{
		uint16_t	Temperature;
		uint16_t	Humidity;
	}	SHT21Data;

	struct Msg_GPS_Pos
	{
	}	GPSPos;

	struct Msg_Light
	{
		uint16_t	Light;
	}	Light;

	struct Msg_RTCSecond
	{
		uint32_t	time;
	} RTCSecond;

	struct Msg_UIClock
	{
		uint16_t	Year;
		uint8_t		Month;
		uint8_t		Day;

		uint8_t		Hour;
		uint8_t		Minute;
	} UIClock;

} Msg_Param;

typedef struct
{
	enumMsg		Id;
	Msg_Param	Param;
} Msg;

extern MemoryPool		msgMP;

void InitMsgMemoryPool(void);

#define MSG_NEW			(Msg*)chPoolAlloc(&msgMP)
#define MSG_NEW_I		(Msg*)chPoolAllocI(&msgMP)

#define MSG_FREE(p)		chPoolFree(&msgMP, (void*)p)
#define MSG_FREE_I(p)	chPoolFreeI(&msgMP, (void*)p)

#define MSG_SEND(p)		chMBPost(&main_mb, (msg_t)p, TIME_INFINITE)
#define MSG_SEND_I(p)	chMBPostI(&main_mb, (msg_t)p)

#define GUI_MSG_SEND(p)		chMBPost(&gui_mb, (msg_t)p, TIME_INFINITE)
#define GUI_MSG_SEND_I(p)	chMBPostI(&gui_mb, (msg_t)p)

// 每个mailbox外部定义
extern Mailbox		gui_mb;
extern Mailbox		main_mb;
extern Mailbox		log_mb;
extern Mailbox		file_mb;

#endif /* MSG_H_ */
