/*
 * block_clock.c
 *
 *  Created on: 2014年11月20日
 *      Author: daniel
 */

#include "block.h"

static void clock_load(const struSize* size, void* param)
{
	// TODO:测试代码
	gdispSetClip(size->x, size->y, size->cx, size->cy);

	gdispDrawBox(0, 0, size->cx, size->cy, 1);
	//gdispDrawLine(0, 0, size->cx, size->cy, 1);
}

static void clock_unload(void)
{
}

static void clock_msg(msg_t msg)
{
}


const struBlockFunctions	block_clock =
{
	clock_load,
	clock_unload,
	clock_msg,
	NULL,
};

