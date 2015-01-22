/*
 * i2c.c
 *
 *  Created on: 2014年10月28日
 *      Author: daniel
 */

#include "myi2c.h"
#include <hal.h>
#include <chprintf.h>
#include "Msg/Msg.h"

/* I2C1 */
static const I2CConfig i2cfg1 = {
    OPMODE_I2C,
    100000,
    STD_DUTY_CYCLE,
};

/* I2C1 */
static const I2CConfig i2cfg2 = {
    OPMODE_I2C,
    100000,
    STD_DUTY_CYCLE,
};

// I2C设备地址
#define SHT21_I2C_ADDR		0x40	// 在两个I2C都有

#define BH1750_I2C_ADDR		0x23	// I2C1

// GY80在I2C2上
#define BMP085_I2C_ADDR		0x77
#define ADXL345_I2C_ADDR	0x53
#define HMC5883L_I2C_ADDR	0x1E


// 缓存上次读取的数据，只有在有变化时，才将变化的数据发送到main主处理模块中，减少消息量
static uint16_t		c_Temperature_inside 	= (uint16_t)-1;
static uint16_t		c_Humidity_inside 		= (uint16_t)-1;
static uint16_t		c_Temperature_outside	= (uint16_t)-1;
static uint16_t		c_Humidity_outside		= (uint16_t)-1;

static uint16_t		c_Light					= 0;

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

void GetLight(void)
{
	uint16_t		light;
	Msg*			msg;
	msg_t			err;

	if (!ReadLightSensor(&light))
	{
		// read light failed
		return;
	}

	if (light == c_Light)
	{
		// no change
		return;
	}

	msg = MSG_NEW;
	if (!msg)
	{
		// malloc failed
		return;
	}

	msg->Id = MSG_LIGHT;
	msg->Param.Light.Light = light;

	err = MSG_SEND(msg);

	if (err == RDY_OK)
	{
		// update cache
		c_Light = light;
	}
	else
	{
		// send failed, free msg
		MSG_FREE(msg);
	}
}


// 读取温湿度数据
bool_t ReadSHT21(I2CDriver* i2cp, uint16_t* pTemperature, uint16_t* pHumidity)
{
	bool_t		bRet = false;
	msg_t 		status 	= RDY_OK;
	uint8_t		data[3];

	i2cAcquireBus(i2cp);
	do
	{
		// 阻塞方式测量温度
		data[0] = 0xE3;
		status = i2cMasterTransmitTimeout(i2cp, SHT21_I2C_ADDR, data, 1, NULL, 0, MS2ST(10));
		if (status != RDY_OK)
		{
			break;
		}
		// 直接读取，在读取时会根据测量时间阻塞
		status = i2cMasterReceiveTimeout(i2cp, SHT21_I2C_ADDR, data, 3, MS2ST(100));
		if (status != RDY_OK)
		{
			break;
		}

		// Check CRC

		*pTemperature = *(uint16_t*)data;

		// 阻塞方式测量湿度
		data[0] = 0xE5;
		status = i2cMasterTransmitTimeout(i2cp, SHT21_I2C_ADDR, data, 1, NULL, 0, MS2ST(10));
		if (status != RDY_OK)
		{
			break;
		}
		status = i2cMasterReceiveTimeout(i2cp, SHT21_I2C_ADDR, data, 3, MS2ST(100));
		if (status != RDY_OK)
		{
			break;
		}

		// check CRC

		*pHumidity = *(uint16_t*)data;

		bRet = true;
	} while (0);
	i2cReleaseBus(i2cp);

	return bRet;
}

//#define HUMIDITY_CONVERT(h) (double)(-6.0 + (125 * (double)h) / 65536.0) * 100
#define HUMIDITY_CONVERT(h) (-600 + 3125 * h / 16384)
//#define TEMPERATURE_CONVERT(t) (double)(-46.85 + (double)(175.72 * (double)t) / 65536.0) * 100
#define TEMPERATURE_CONVERT(t) (-4685 + (4393 * t) / 16384)

// Last
void GetTemperatureAndHumidity(void)
{
	bool_t		bRet;
	uint16_t	unTemperature, unHumidity;
	Msg*		msg;
	msg_t		err;

	// inside
	bRet = ReadSHT21(&I2CD2, &unTemperature, &unHumidity);
	if (bRet)
	{
		// 判断是否有变化
		if ((unTemperature != c_Temperature_inside) || (unHumidity != c_Humidity_inside))
		{
			// Send Msg
			msg = MSG_NEW;
			if (msg)
			{
				msg->Id = MSG_SHT21_INSIDE;
				msg->Param.SHT21Data.Temperature = (int16_t)TEMPERATURE_CONVERT(unTemperature);
				msg->Param.SHT21Data.Humidity = (int16_t)HUMIDITY_CONVERT(unHumidity);

				// Send
				err = MSG_SEND(msg);
				if (err == RDY_OK)
				{
					// 成功时才更新内存缓存的值
					c_Temperature_inside = unTemperature;
					c_Humidity_inside = unHumidity;
				}
				else
				{
					// send failed, free msg
					MSG_FREE(msg);
				}
			}
		}
	}

	// outside
	bRet = ReadSHT21(&I2CD1, &unTemperature, &unHumidity);
	if (bRet)
	{
		// 判断是否有变化
		if ((unTemperature != c_Temperature_outside) || (unHumidity != c_Humidity_inside))
		{
			msg = MSG_NEW;
			if (msg)
			{
				msg->Id = MSG_SHT21_OUTSIDE;
				msg->Param.SHT21Data.Temperature = unTemperature;
				msg->Param.SHT21Data.Humidity = unHumidity;

				err = MSG_SEND(msg);
				if (err == RDY_OK)
				{
					// update cache
					c_Temperature_outside = unTemperature;
					c_Humidity_outside = unHumidity;
				}
				else
				{
					MSG_FREE(msg);
				}
			}
		}
	}

}

// 读取数字罗盘

// 读取加速度传感器

// 读取陀螺仪传感器



// cmd
// 读取照度计数值
void cmd_light(BaseSequentialStream *chp, int argc, char *argv[])
{
	(void)argc;
	(void)argv;
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
