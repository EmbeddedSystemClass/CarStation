/*
 * power.h
 *
 *  Created on: 2014年10月6日
 *      Author: daniel
 */

#ifndef POWER_H_
#define POWER_H_

#include <ch.h>

bool_t InitPower(void);
void EnableCharge(bool_t bEnable);
void GetPowerStatus(void);

void cmd_power(BaseSequentialStream *chp, int argc, char *argv[]);
void cmd_chargeenable(BaseSequentialStream *chp, int argc, char *argv[]);

#endif /* POWER_H_ */
