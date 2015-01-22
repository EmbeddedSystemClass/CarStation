/*
 * block_clock.c
 *
 *  Created on: 2014年11月20日
 *      Author: daniel
 */

#include "block.h"
#include <chprintf.h>

static void clock_load(const struSize* size, void* param)
{
	// TODO:测试代码
	//gdispSetClip(size->x, size->y, size->cx, size->cy);

	gdispDrawBox(0, 0, size->cx, size->cy, 1);
	//gdispDrawLine(0, 0, size->cx, size->cy, 1);
}

static void clock_unload(void)
{
}

static void clock_msg(Msg* msg)
{
	if (msg->Id == MSG_UI_CLOCK)
	{
		font_t		font;
		char		ctime[6];

		// Clear
		gdispFillArea(0, 0, 128, 32, 0);

		chsnprintf(ctime, 6, "%02d:%02d", msg->Param.UIClock.Hour, msg->Param.UIClock.Minute);
		font = gdispOpenFont("LargeNumbers");
		gdispDrawString(0, 10, ctime, font, 1);
		gdispCloseFont(font);
		gdispFlush();
	}
}


const struBlockFunctions	block_clock =
{
	clock_load,
	clock_unload,
	clock_msg,
	NULL,
};

