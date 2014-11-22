/*
 * Pages.c
 *
 *  Created on: 2014年11月20日
 *      Author: daniel
 *
 *      用于定义页面的block组合，以及页面的按键行为
 */


#include "GUI.h"
#include "blocks/block.h"

// 主页
static const struBlock		mainPage[] =
{
	{{0, 0, 128, 32}, &block_clock},
	{{0,32, 128, 32}, &block_gps}
};



// 页面组合
const struPage				Pages[] =
{
	{PAGE_DEBUG, 0, NULL, ACTION_BACK, ACTION_BACK, ACTION_BACK, ACTION_BACK},
	{PAGE_MAIN, sizeof(mainPage)/sizeof(mainPage[0]), mainPage, ACTION_BACK, ACTION_MSG, ACTION_MSG, ACTION_MSG},
};
