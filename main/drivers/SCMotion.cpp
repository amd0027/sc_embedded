/*
 * SCMotion.cpp
 *
 *  Created on: Mar 16, 2019
 *      Author: Chris Taylor, Alex Dillon
 */

#include "SCMotion.h"

SCMotion::SCMotion()
{
	printf("     0  1  2  3  4  5  6  7  8  9  a  b  c  d  e  f\n");
	printf("00:         ");
	for (uint8_t i = 3; i < 0x78; i++)
	{
		if (i % 16 == 0) {
			printf("\n%.2x:", i);
		}
		if (i2c.checkSlaveAddress(i)) {
			printf(" %.2x", i);
			//i2c.setAddress(i);
		} else {
			printf(" --");
		}
	}
	printf("\n\n");

	i2c.setAddress(0x6b); // 0x6b is address of accelerometer

	// The following function is used to change data output rate from its default of 0 Hz
	i2c.writeReg(0x6b, 0x20, 0x20);	// Accelerometer: set output data rate to 10 Hz. Register 0x20

	// This should initialize the gyroscope, but I'm just going to work with accelerometer for now
	//i2c.writeReg(0x6b, 0x10, 0x80);	// Gyroscope: set output data rate to 238 Hz ("normal mode"). Register 0x10
}

void SCMotion::Sample()
{
	uint8_t *buffer = (uint8_t*)malloc(6);
	int16_t XL_X_val, XL_Y_val, XL_Z_val;

	i2c.beginTransaction();
	i2c.master_read_slave(buffer, 6);
	i2c.endTransaction();
	XL_X_val = (buffer[1] << 8) | buffer[0];
	XL_Y_val = (buffer[3] << 8) | buffer[2];
	XL_Z_val = (buffer[5] << 8) | buffer[4];
	printf("XL: %7d %7d %7d\n", XL_X_val, XL_Y_val, XL_Z_val);

	//TODO: Return something
}

