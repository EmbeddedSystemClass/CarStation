/*
 * block_clock.c
 *
 *  Created on: 2014年11月20日
 *      Author: daniel
 */

#include "block.h"

static void clock_load(const struSize* size, void* param)
{
}

static void clock_unload(void)
{
}

static void clock_msg(void)
{
}


const struBlockFunctions	block_clock =
{
	clock_load,
	clock_unload,
	clock_msg,
	NULL,
};

