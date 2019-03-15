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
	buffer_size = 2;
	data_rd = (uint8_t*)malloc(buffer_size); // data_rd buffer points to "buffer_size" bytes of available memory
	data_high = (uint8_t*)malloc(1);
	data_low = (uint8_t*)malloc(1);
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
	if(i2c_master_start(cmd) != ESP_OK)	// sends START bit and checks for error
		printf("Error queuing start command!\n");
	direction = UNKNOWN;
}

void SCDriverI2C::endTransaction()
{
	// Finish command queue with a stop command before executing all commands with begin
	if(i2c_master_stop(cmd) != ESP_OK) // sends STOP bit and checks for error
		printf("Error queuing stop command!\n");
	esp_err_t err = i2c_master_cmd_begin(I2C_NUM_0, cmd, 100 / portTICK_PERIOD_MS);
	if (err != ESP_OK)
		printf("Error Sending Commands!: err = %d\n", err);
	i2c_cmd_link_delete(cmd); // delete link between master & slave
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

// currently unused
void SCDriverI2C::read(uint8_t* data, size_t length, bool ack)
{
	/*
	if (direction != READ)
	{
		// Send control bits
		if (i2c_master_write_byte(cmd, (slave_addr << 1) | I2C_MASTER_READ, !ack) != ESP_OK)
			printf("Error entering read mode!\n");
		direction = READ;
	}
	*/

}

/*
 * @brief 	Must be called after beginTransaction() and before endTransaction().
 * 			Handles the I2C reading nonsense of master from slave:
 * 				(1.) Writes control byte containing slave address and WRITE mode. Then waits for ACK.
 * 				(2.) Writes to slave that it will be reading from "data_rd" register. Waits for ACK.
 * 				3. Again writes START bit, device address, but now specifies READ mode. Waits for ACK.
 * 				4. Reads however much data it wants...
 * 				5. Then sends NACK.
 */
void SCDriverI2C::master_read_slave(uint8_t *data_rd, int size)
{

	//data_rd = new uint8_t[2];

	//1.
		if (i2c_master_write_byte(cmd, (slave_addr << 1) | I2C_MASTER_WRITE, I2C_MASTER_ACK) != ESP_OK)
			printf("Error entering read mode (in sending control bits)!\n");


	//2.
		if (i2c_master_write_byte(cmd, 0x28, I2C_MASTER_ACK) != ESP_OK)	// we want to read from 0x28
			printf("Error entering read mode (in sending control bits)!\n");

	// 2.5:	RESEND START BIT
		if(i2c_master_start(cmd) != ESP_OK)	// sends START bit and checks for error
			printf("Error queuing start command!\n");

	//3.
	if (i2c_master_write_byte(cmd, (slave_addr << 1) | I2C_MASTER_READ, 1) != ESP_OK)
		printf("Error entering read mode (in sending control bits)!\n");


	// 4.
	if(size > 1)
	{
		if (i2c_master_read(cmd, data_rd, size-1, I2C_MASTER_ACK) != ESP_OK)
			printf("Error queuing read command (in receiving data from slave)!\n");
	}

	// 5. Read last byte and send NACK
	if (i2c_master_read_byte(cmd, data_rd + size-1, I2C_MASTER_NACK) != ESP_OK)
		printf("Error queuing read command (in receiving last byte from slave)!\n");
}

void SCDriverI2C::setContinuousMode()
{
	//1.
		if (i2c_master_write_byte(cmd, (slave_addr << 1) | I2C_MASTER_WRITE, I2C_MASTER_ACK) != ESP_OK)
			printf("Error entering read mode (in sending control bits)!\n");

	//2.
		if (i2c_master_write_byte(cmd, 0x20 , I2C_MASTER_ACK) != ESP_OK)	// FIFO CTRL REG == 0x2E
			printf("Error entering read mode (in sending control bits)!\n");

	//3.
		if (i2c_master_write_byte(cmd, 0x20 , I2C_MASTER_ACK) != ESP_OK)	// write to FIFO CTRL REG to put into continuous mode.
			printf("Error entering read mode (in sending control bits)!\n");

}


/**
 * @brief test code to operate on sensor
 *
 * 1. set operation mode(e.g One time L-resolution mode)
 * _________________________________________________________________
 * | start | slave_addr + wr_bit + ack | write 1 byte + ack  | stop |
 * --------|---------------------------|---------------------|------|
 * 2. wait more than 24 ms
 * 3. read data
 * ______________________________________________________________________________________
 * | start | slave_addr + rd_bit + ack | read 1 byte + ack  | read 1 byte + nack | stop |
 * --------|---------------------------|--------------------|--------------------|------|
 */
void SCDriverI2C::i2c_master_sensor_test(i2c_port_t i2c_num, uint8_t *data_h, uint8_t *data_l)
{
    int ret;
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, slave_addr << 1 | I2C_MASTER_WRITE, I2C_MASTER_ACK);
    i2c_master_write_byte(cmd, 0xFF, I2C_MASTER_ACK);
    i2c_master_stop(cmd);
    ret = i2c_master_cmd_begin(I2C_NUM_0, cmd, 1000 / portTICK_RATE_MS);
    i2c_cmd_link_delete(cmd);
    /*
    if (ret != ESP_OK) {
        return ret;
    }
    */
    vTaskDelay(50 / portTICK_RATE_MS);
    cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, slave_addr << 1 | I2C_MASTER_READ, I2C_MASTER_ACK);
    i2c_master_read_byte(cmd, data_h, I2C_MASTER_ACK);
    i2c_master_read_byte(cmd, data_l, I2C_MASTER_NACK);
    i2c_master_stop(cmd);
    ret = i2c_master_cmd_begin(I2C_NUM_0, cmd, 1000 / portTICK_RATE_MS);
    i2c_cmd_link_delete(cmd);
    //return ret;
}
