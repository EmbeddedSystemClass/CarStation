/*
 * i2c.c
 *
 *  Created on: 2014年10月28日
 *      Author: daniel
 */

#include "myi2c.h"
#include <hal.h>
#include <chprintf.h>

/* I2C1 */
static const I2CConfig i2cfg1 = {
    OPMODE_I2C,
    100000,
    FAST_DUTY_CYCLE_2,
};

/* I2C1 */
static const I2CConfig i2cfg2 = {
    OPMODE_I2C,
    100000,
    FAST_DUTY_CYCLE_2,
};

// I2C设备地址
#define SHT21_I2C_ADDR		0x40	// 在两个I2C都有

#define BH1750_I2C_ADDR		0x23	// I2C1

// GY80在I2C2上
#define BMP085_I2C_ADDR		0x77
#define ADXL345_I2C_ADDR	0x53
#define HMC5883L_I2C_ADDR	0x1E

bool_t InitI2C(void)
{
	bool_t	bRet = TRUE;

	 i2cStart(&I2CD1, &i2cfg1);
	 i2cStart(&I2CD2, &i2cfg2);

	return bRet;
}

// 读取照度传感器
bool_t ReadLightSensor(uint16_t* unLight)
{
	bool_t		bRet 	= false;
	msg_t 		status 	= RDY_OK;
	uint8_t		data[2];

	i2cAcquireBus(&I2CD1);
	do
	{
		// Power on
		data[0] = 1;
		status = i2cMasterTransmitTimeout(&I2CD1, BH1750_I2C_ADDR, data, 1, data, 0, MS2ST(4));
		if (status != RDY_OK)
		{
			break;
		}

		// Measure H2
		data[0] = 0x11;
		status = i2cMasterTransmitTimeout(&I2CD1, BH1750_I2C_ADDR, data, 1, data, 0, MS2ST(4));
		if (status != RDY_OK)
		{
			break;
		}

		// Delay
		chThdSleepMilliseconds(200);

		// Read result
		status = i2cMasterReceiveTimeout(&I2CD1, BH1750_I2C_ADDR, data, 2, MS2ST(10));
		if (status != RDY_OK)
		{
			break;
		}

		// 组合数据
		*unLight = ((uint16_t)(data[0]) << 8) | (uint8_t)(data[1]);

		bRet = true;
	} while (0);




	i2cReleaseBus(&I2CD1);

	return bRet;
}

// 读取温湿度数据
bool_t ReadSHT21(int16_t* pTemperature, int16_t* pHumi)
{
	//
	return false;
}

// 读取数字罗盘

// 读取加速度传感器

// 读取陀螺仪传感器



// cmd
// 读取照度计数值
void cmd_light(BaseSequentialStream *chp, int argc, char *argv[])
{
	uint16_t		light = 0;


	if (ReadLightSensor(&light))
	{
		chprintf(chp, "light = %d lux\r\n", light);
	}
	else
	{
		chprintf(chp, "Read light failed.\r\n");
	}
}
