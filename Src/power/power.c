/*
 * power.c
 *
 *  Created on: 2014年10月6日
 *      Author: daniel
 */

#include "power.h"
#include <hal.h>
#include <chprintf.h>
#include "Msg/Msg.h"
#include "const.h"

#define ADC_GRP1_NUM_CHANNELS   3
#define ADC_GRP1_BUF_DEPTH      8

static adcsample_t	voltages[ADC_GRP1_NUM_CHANNELS * ADC_GRP1_BUF_DEPTH];

// 缓存电压值（为判断方便，这里将电压数据换算完成）
static int16_t		c_Battery		= INVALID_VOLTAGE;	// 1/2取样
static int16_t		c_CarBattery	= INVALID_VOLTAGE;	// 1/5取样
static int16_t		c_CarAcc		= INVALID_VOLTAGE;	// 1/5取样

static bool_t		c_PowerOn		= false;
static bool_t		c_CarRunning	= false;

static bool_t		s_PowerIsChange	= false;

#define ADC_REF_VOLTAGE			3300		// ADC参考电压
#define POWER_ON_VAL			9000		// 大于9v，就认为开启电源
#define CAR_RUNNING_VAL			1270		// 大于12.7v，认为启动，发电机工作


void PowerCheck(void);

bool_t InitPower(void)
{
	/*
	 *  Activates the ADC1 driver and the temperature sensor.
	 */
	adcStart(&ADCD1, NULL);

	return CH_SUCCESS;
}

// 开启充电（使用12 -> 5v的DCDC变换器的控制端）
void EnableCharge(bool_t bEnable)
{
	if (bEnable)
	{
		palSetPad(GPIO_DCDC_ENABLE_PORT, GPIO_DCDC_ENABLE_BIT);
	}
	else
	{
		palClearPad(GPIO_DCDC_ENABLE_PORT, GPIO_DCDC_ENABLE_BIT);
	}
}

// ADC error callback
static void adcerrorcallback(ADCDriver *adcp, adcerror_t err)
{
	(void)adcp;
	(void)err;

	// TODO:测试
	//Msg		msg;

}


/*
 * 对几个电压进行采样，提供shell使用，同步调用
 * Mode:        Linear buffer, 8 samples of 1 channel, SW triggered.
 * Channels:    IN10.
 */
static const ADCConversionGroup adcgrpcfg1 = {
  FALSE,
  ADC_GRP1_NUM_CHANNELS,
  NULL,
  adcerrorcallback,
  0, 0,                         /* CR1, CR2 */
  ADC_SMPR1_SMP_AN10(ADC_SAMPLE_7P5) | ADC_SMPR1_SMP_AN11(ADC_SAMPLE_7P5) | ADC_SMPR1_SMP_AN12(ADC_SAMPLE_7P5),
  0,                            /* SMPR2 */
  ADC_SQR1_NUM_CH(ADC_GRP1_NUM_CHANNELS),
  0,                            /* SQR2 */
  ADC_SQR3_SQ1_N(ADC_CHANNEL_IN10) | ADC_SQR3_SQ2_N(ADC_CHANNEL_IN11) | ADC_SQR3_SQ3_N(ADC_CHANNEL_IN12)
};

// 测量电压数据，和充电状态，以及充电允许状态，发送给主处理模块
// 在测量完成后，还对当前的电门和发电状态、电池电压状态，判断是否需要开启充电
void GetPowerStatus(void)
{
	int16_t		Vbattery, Vcar12bat, Vcar12acc;
	int32_t		vtemp;
	msg_t		err;

	// 获取电压，并对8次采样取平均值
	err = adcConvert(&ADCD1, &adcgrpcfg1, voltages, ADC_GRP1_BUF_DEPTH);

	if (err == RDY_OK)
	{
		int8_t	i;

		// 取平均值
		Vbattery = Vcar12bat = Vcar12acc = 0;
		for (i = 0; i < ADC_GRP1_BUF_DEPTH; i++)
		{
			// 锂电池
			Vbattery += voltages[i * ADC_GRP1_NUM_CHANNELS + 1];

			// 汽车12V电池
			Vcar12bat += voltages[i * ADC_GRP1_NUM_CHANNELS + 0];

			// 汽车电门输出（主要用于判断是否开启电源）
			Vcar12acc += voltages[i * ADC_GRP1_NUM_CHANNELS + 2];
		}
		Vbattery 	/= ADC_GRP1_BUF_DEPTH;
		Vcar12bat	/= ADC_GRP1_BUF_DEPTH;
		Vcar12acc	/= ADC_GRP1_BUF_DEPTH;

		// 进行电压转换和逻辑判断

		// 换算为实际电压（mv，整数）
		vtemp = 2 * ADC_REF_VOLTAGE * (int32_t)Vbattery / 4096;
		Vbattery = (int16_t)vtemp;
		vtemp = 5 * ADC_REF_VOLTAGE * (int32_t)Vcar12bat / 4096;
		Vcar12bat = (int16_t)vtemp;
		vtemp = 5 * ADC_REF_VOLTAGE * (int32_t)Vcar12acc / 4096;
		Vcar12acc = (int16_t)vtemp;

		if ( (Vbattery != c_Battery) || (Vcar12bat != c_CarBattery) || (Vcar12acc != c_CarAcc) )
		{
			s_PowerIsChange = true;
			c_Battery = Vbattery;
			c_CarBattery = Vcar12bat;
			c_CarAcc = Vcar12acc;

			PowerCheck();
		}

		if (s_PowerIsChange)
		{
			Msg*	msg;
			msg = MSG_NEW;
			if (msg)
			{
				msg->Id = MSG_POWER;
				msg->Param.PowerVoltage.LionBattery	= c_Battery;
				msg->Param.PowerVoltage.CarBattery 	= c_CarBattery;
				msg->Param.PowerVoltage.IsPoweron	= c_PowerOn;
				msg->Param.PowerVoltage.IsCarStart	= c_CarRunning;
				msg->Param.PowerVoltage.IsCharging	= palReadPad(GPIO_DCDC_ENABLE_PORT, GPIO_DCDC_ENABLE_BIT);

				err = MSG_SEND(msg);
				if (err == RDY_OK)
				{
					s_PowerIsChange = false;
				}
				else
				{
					MSG_FREE(msg);
				}
			}
		}


	}
}

void PowerCheck(void)
{
	// 检查是否应该启动充电，是否汽车开启电源，是否汽车启动了
}

void cmd_power(BaseSequentialStream *chp, int argc, char *argv[])
{
	(void)argc;
	(void)argv;

	// 获取所有的电压值  （同步调用方式）
	adcConvert(&ADCD1, &adcgrpcfg1, voltages, ADC_GRP1_BUF_DEPTH);

	// 输出电压值

	// 当前充电允许状态
	chprintf(chp, "Enable charge:%d\r\n", palReadPad(GPIO_DCDC_ENABLE_PORT, GPIO_DCDC_ENABLE_BIT));

	// 当前锂电池充电模块的充满指示状态
	chprintf(chp, "Is Full:%d\r\n", palReadPad(GPIO_CHARGE_FULL_PORT, GPIO_CHARGE_FULL_BIT));
}

void cmd_chargeenable(BaseSequentialStream *chp, int argc, char *argv[])
{
	// 开启或关闭充电
	if (argc == 1)
	{
		// 有一个参数
		if (*argv[0] == '1')
		{
			EnableCharge(true);
			chprintf(chp, "Enable charge.\r\n");
			return;
		}
		else if (*argv[0] == '0')
		{
			EnableCharge(false);
			chprintf(chp, "Disable charge.\r\n");
			return;
		}
	}

	// usage
	chprintf(chp, "chargeenable 1|0\r\n");
}
