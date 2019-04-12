/*
 * SCMotion.cpp
 *
 *  Created on: Mar 16, 2019
 *      Author: Chris Taylor, Alex Dillon
 */

#include "SCMotion.h"

#include <stdio.h>

SCMotion::SCMotion()
{
	i2c.setAddress(0x6b); // 0x6b is address of accelerometer

	// The following function is used to change data output rate from its default of 0 Hz
	i2c.writeReg(0x6b, 0x20, 0x20);	// Accelerometer: set output data rate to 10 Hz. Register 0x20

	// This should initialize the gyroscope, but I'm just going to work with accelerometer for now
	//i2c.writeReg(0x6b, 0x10, 0x80);	// Gyroscope: set output data rate to 238 Hz ("normal mode"). Register 0x10
}

SCMotionRawData SCMotion::SampleAccelerometer()
{
	uint8_t buffer[6];
	int16_t XL_X_val, XL_Y_val, XL_Z_val;

	i2c.beginTransaction();
	i2c.master_read_XL(buffer, 6);//, 0x6b, 0x28, 0);
	i2c.endTransaction();
	XL_X_val = (buffer[1] << 8) | buffer[0];
	XL_Y_val = (buffer[3] << 8) | buffer[2];
	XL_Z_val = (buffer[5] << 8) | buffer[4];
	//printf("XL: %7d %7d %7d\n", XL_X_val, XL_Y_val, XL_Z_val);

	SCMotionRawData result;
	result.x = XL_X_val;
	result.y = XL_Y_val;
	result.z = XL_Z_val;
	return result;
}
