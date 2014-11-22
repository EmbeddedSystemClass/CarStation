/*
 * i2c.h
 *
 *  Created on: 2014年10月28日
 *      Author: daniel
 */

#ifndef I2C_H_
#define I2C_H_

#include <ch.h>

bool_t InitI2C(void);

void cmd_light(BaseSequentialStream *chp, int argc, char *argv[]);

#endif /* I2C_H_ */
