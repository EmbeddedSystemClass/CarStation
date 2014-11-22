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

#define ADC_GRP1_NUM_CHANNELS   1
#define ADC_GRP1_BUF_DEPTH      8

static adcsample_t	voltages[ADC_GRP1_NUM_CHANNELS * ADC_GRP1_BUF_DEPTH];;

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
  ADC_SMPR1_SMP_AN10(ADC_SAMPLE_1P5),
  0,                            /* SMPR2 */
  ADC_SQR1_NUM_CH(ADC_GRP1_NUM_CHANNELS),
  0,                            /* SQR2 */
  ADC_SQR3_SQ1_N(ADC_CHANNEL_IN11)
};

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
