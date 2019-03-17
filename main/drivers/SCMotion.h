/*
 * SCMotion.h
 *
 *  Created on: Mar 16, 2019
 *      Authors: Chris Taylor, Alex Dillon
 */

#ifndef SCMOTION_H_
#define SCMOTION_H_

#include "SCDriverI2C.h"

class SCMotion {
public:
	SCMotion();
	void Sample();

private:
	SCDriverI2C i2c;

};

#endif /* SCMOTION_H_ */
