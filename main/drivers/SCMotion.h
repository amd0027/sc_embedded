/*
 * SCMotion.h
 *
 *  Created on: Mar 16, 2019
 *      Authors: Chris Taylor, Alex Dillon
 */

#ifndef SCMOTION_H_
#define SCMOTION_H_

#include "SCDriverI2C.h"

struct SCMotionRawData
{
	int x;
	int y;
	int z;
};

class SCMotion {
public:
	SCMotion();
	SCMotionRawData SampleAccelerometer();

private:
	void DumpI2CRegisters();

private:
	SCDriverI2C i2c;

};

#endif /* SCMOTION_H_ */
