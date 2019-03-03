/*
 * SCDriverI2C.cpp
 *
 *  Created on: Mar 3, 2019
 *      Author: Chris Taylor
 */

#include "SCDriverI2C.h"

SCDriverI2C::SCDriverI2C() {
	slave_addr = 0;
	cmd = 0;
	direction = UNKNOWN;
	// Configure master config_t variable
	master_chars.mode = I2C_MODE_MASTER;
	master_chars.sda_pullup_en = GPIO_PULLUP_ENABLE;
	master_chars.scl_pullup_en = GPIO_PULLUP_ENABLE;
	master_chars.master.clk_speed = I2C_MASTER_FREQ_HZ;
	// Pins 21 and 22 are SDA and SCL on our board respectively
	master_chars.sda_io_num = GPIO_NUM_21;
	master_chars.scl_io_num = GPIO_NUM_22;

	if(i2c_param_config(I2C_NUM_0, &master_chars) != ESP_OK)
		printf("Error Configuring Master Parameters!\n");
	if(i2c_driver_install(I2C_NUM_0, I2C_MODE_MASTER, 0, 0, 0) != ESP_OK)
		printf("Error Installing Driver for Master!\n");
}

SCDriverI2C::~SCDriverI2C() {
	// TODO Auto-generated destructor stub
}

void SCDriverI2C::beginTransaction()
{
	// Add create link and start to command queue to begin a transaction
	cmd = i2c_cmd_link_create();
	if(i2c_master_start(cmd) != ESP_OK)
		printf("Error queuing start command!\n");
	direction = UNKNOWN;
}

void SCDriverI2C::endTransaction()
{
	// Finish command queue with a stop command before executing all commands with begin
	if(i2c_master_stop(cmd) != ESP_OK)
		printf("Error queuing stop command!\n");
	esp_err_t err = i2c_master_cmd_begin(I2C_NUM_0, cmd, 100 / portTICK_PERIOD_MS);
	if (err != ESP_OK)
		printf("Error Sending Commands!: err = %d\n", err);
	i2c_cmd_link_delete(cmd);
	direction = UNKNOWN;
}

// Create a simple transaction with one write command executed and verify ack is received from slave
bool SCDriverI2C::checkSlaveAddress(uint8_t check)
{
	// Validate member variable slave_addr if valid address not provided
	if (check == 0) check = slave_addr;
	i2c_cmd_handle_t temp = i2c_cmd_link_create();
	i2c_master_start(temp);
	i2c_master_write_byte(temp, (check << 1) | I2C_MASTER_WRITE, 1 /* expect ack */);
	i2c_master_stop(temp);
	esp_err_t espRc = i2c_master_cmd_begin(I2C_NUM_0, temp, 100 / portTICK_PERIOD_MS);
	i2c_cmd_link_delete(temp);
	return espRc == 0; // Return true if the slave is present and false otherwise.
}

void SCDriverI2C::setAddress(uint8_t address)
{
	slave_addr = address;
}

uint8_t SCDriverI2C::getAddress()
{
	return slave_addr;
}

void SCDriverI2C::read(uint8_t* data, size_t length, bool ack)
{
	if (direction != READ)
	{
		if (i2c_master_write_byte(cmd, (slave_addr << 1) | I2C_MASTER_READ, !ack) != ESP_OK)
			printf("Error entering read mode!\n");
		direction = READ;
	}
	if (i2c_master_read(cmd, data, length, ack?I2C_MASTER_ACK:I2C_MASTER_NACK) != ESP_OK)
		printf("Error queuing read command!\n");
}

