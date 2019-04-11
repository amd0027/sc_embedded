/*
 * SCDriverI2C.h
 *
 *  Created on: Mar 3, 2019
 *      Author: Chris Taylor, Logan Johnson
 */

#ifndef MAIN_SCDRIVERI2C_H_
#define MAIN_SCDRIVERI2C_H_

#include "driver/i2c.h"
#include <cstdlib>

#define I2C_MASTER_FREQ_HZ    100000
#define I2C_MASTER_SCL_IO    22    /*!< gpio number for I2C master clock */
#define I2C_MASTER_SDA_IO    21    /*!< gpio number for I2C master data  */

class SCDriverI2C {
public:
	SCDriverI2C();
	SCDriverI2C(int n);
	virtual ~SCDriverI2C();
	void beginTransaction();
	void endTransaction();
	bool checkSlaveAddress(uint8_t check);
	void setAddress(uint8_t address);
	uint8_t getAddress();
	//void master_read_slave(uint8_t *data_rd, int size, uint8_t slave_address, uint8_t read_reg, int AQS_flag);
	void writeReg(uint8_t slave_address, uint8_t reg, uint8_t value);
	void master_read_XL(uint8_t *data_rd, int size);
	void master_read_AQS(uint8_t *data_rd, int size, uint8_t reg);

	// temp testing functions
	// void master_read_slave(uint8_t *data_rd, int size);


private:
	i2c_config_t master_chars;
	uint8_t slave_addr;
	i2c_cmd_handle_t cmd;
	enum i2c_direction {
		UNKNOWN,
		READ,
		WRITE
	} direction;
};

#endif /* MAIN_SCDRIVERI2C_H_ */
