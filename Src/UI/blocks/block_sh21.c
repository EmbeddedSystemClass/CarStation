/*
 * block_sh21.c
 *
 *  Created on: 2014年11月20日
 *      Author: daniel
 */

#include "block.h"

static void sh21_load(const struSize* size, void* param)
{
}

static void sh21_unload(void)
{
}

static void sh21_msg(Msg* msg)
{
}

const struBlockFunctions	block_sh21 =
{
	sh21_load,
	sh21_unload,
	sh21_msg,
	NULL,
};
