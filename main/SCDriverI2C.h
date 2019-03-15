/*
 * SCDriverI2C.h
 *
 *  Created on: Mar 3, 2019
 *      Author: Chris Taylor
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
	virtual ~SCDriverI2C();
	void beginTransaction();
	void endTransaction();
	bool checkSlaveAddress(uint8_t check);
	void setAddress(uint8_t address);
	uint8_t getAddress();
	void read(uint8_t* data, size_t length, bool ack);
	void master_read_slave(uint8_t *data_rd, int size);
	void i2c_master_sensor_test(i2c_port_t i2c_num, uint8_t *data_h, uint8_t *data_l);
	void setContinuousMode();

	int buffer_size;
	uint8_t *data_rd;
	uint8_t *data_high, *data_low;

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
