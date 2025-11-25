/*
 * @file i2c.h
 * @brief Header file for i2c.c
 *
 * @author: Satya Mehta.
 * @date 11/16/2019
 * @copyright Copyright (c) 2019
 *
 */


#ifndef INC_I2C_H_
#define INC_I2C_H_

#include <stdint.h>



#define NXP_NTAG_R (0x05) 										//NXP NTAG NFC Read Command
#define NXP_NTAG_W (0x04) 										//NXP NTAG NFC Write Command


/* SCL and SDA pins for I2C*/
#define SCL_PORT						(gpioPortC)
#define SDA_PORT						(gpioPortC)
#define SCL_PIN							(10)
#define SDA_PIN							(11)


/*Global Variables*/
volatile uint8_t interrupt_flag_ack;
uint8_t read[16];
uint8_t write[16];


/*Function Declarations*/
void i2c_init(void);
void i2c_write_poll(uint8_t add,uint8_t *data);
void i2c_disable(void);
uint8_t* i2c_read_poll(uint8_t register_address);
uint8_t i2c_read_session_poll(uint8_t session_register);
void i2c_test_blocking(void);
uint8_t* nxp_ntag_nfc_read(uint8_t *arr, uint8_t row);
void nxp_ntag_nfc_write(uint8_t *arr, uint8_t row);


#endif /* INC_I2C_H_ */
