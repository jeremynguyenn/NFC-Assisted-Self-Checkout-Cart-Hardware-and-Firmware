/*
 * @file i2c.c
 * @brief This file consists of functions related to I2C0 peripheral.
 *
 * @author: Satya Mehta
 * @date 11/05/2019
 * @copyright Copyright (c) 2019
 *
 */


#include <stdio.h>
#include "em_gpio.h"
#include "em_i2c.h"
#include "em_cmu.h"
#include "em_core.h"
#include "em_cmu.h"
#include "inc/i2c.h"



#define DELAY_TIME						(1000000)


/**
 * @brief Delay function used to add additional delays required in I2C Initialization.
 * @param uint32_t time A random time number tick.
 * @return void
 */
static inline void delay(uint32_t time)
{
	uint32_t i;
	for(i = 0; i < time; i++);
}


/**
 * @brief Function to initialize the GPIO pins required for I2C.
 * @param void
 * @return void
 */
static void i2c_gpio_init(void)
{
	/* GPIO clock */
	CMU_ClockEnable(cmuClock_GPIO, true);

	/* Initializing SCL and SDA pins */
	GPIO_PinModeSet(SCL_PORT, SCL_PIN, gpioModeWiredAnd, 1); 				/* SCL */
	GPIO_PinModeSet(SDA_PORT, SDA_PIN, gpioModeWiredAnd, 1);				/* SDA */
}



/**
 * @brief This function is used to initialize I2C0 peripheral.
 * PortC 10 SCL, PortC 11 SDA is used.
 * @param void
 * @return void
 */
void i2c_init(void)
{
	/* Enabling GPIO required for I2C */
	i2c_gpio_init();

	/*I2C clock*/
	CMU_ClockEnable(cmuClock_I2C0, true);
	CMU_OscillatorEnable(cmuOsc_LFXO, true, true);


	/* Additional delay before ROUTE */
	delay(200000);

	I2C0 -> ROUTEPEN = I2C_ROUTEPEN_SCLPEN | I2C_ROUTEPEN_SDAPEN;
	I2C0->ROUTELOC0 |= (I2C0->ROUTELOC0 & (~_I2C_ROUTELOC0_SCLLOC_MASK))| I2C_ROUTELOC0_SCLLOC_LOC14;
	I2C0->ROUTELOC0 |= (I2C0->ROUTELOC0 & (~_I2C_ROUTELOC0_SDALOC_MASK))| I2C_ROUTELOC0_SDALOC_LOC16;

	/* Default initialization */
	const I2C_Init_TypeDef i2cinitialization = I2C_INIT_DEFAULT;

	I2C_Init(I2C0, &i2cinitialization);
	delay(DELAY_TIME);
	I2C_Enable(I2C0,true);
	delay(DELAY_TIME);

	/*Toggle SCL 9 times as specified in datasheet*/
	for (int i=0; i<9; i++)
	{
		GPIO_PinOutClear(SCL_PORT, SCL_PIN);
		GPIO_PinOutSet(SCL_PORT, SCL_PIN);
	}
	delay(200000);

	if(I2C0->STATE & I2C_STATE_BUSY)
	{
		I2C0->CMD = I2C_CMD_ABORT;
	}


	/* Enabling NACK Interrupt for debugging purposes*/
	NVIC_EnableIRQ(I2C0_IRQn);
	I2C_IntEnable(I2C0, I2C_IFC_NACK);
}


/**
 * @brief This function is a polling write driver for NXP NTAG I2C NFC Device.
 * 16 Bytes are written in a single I2C write transfer.
 * @param add - Address of the starting register you need to write data into. Following 15 bytes will also be written.
 * 		  data - Starting address of the data which is to be written into NFC.
 * @return None.
 */
void i2c_write_poll(uint8_t add, uint8_t *data)
{
	uint8_t i;
	I2C0->CMD = I2C_CMD_START;  					/* sending start bit */
	I2C0->TXDATA = 0x04; 							/* NXP NTAG Address */

	while((I2C0->IF & I2C_IF_ACK) == 0);
	I2C0->IFC |= I2C_IFC_ACK;

	I2C0->TXDATA = add;

	/*Writing 16 Bytes */
	for(i = 0; i < 16; i++)
	{
		while((I2C0->IF & I2C_IF_ACK) == 0);
		I2C0->IFC |= I2C_IFC_ACK;
		I2C0->TXDATA = data[i];
	}

	I2C0->CMD = I2C_CMD_STOP;
	while(I2C0->CMD & I2C_STATUS_PSTOP);
}


/**
 * @brief This function is used to read the session register inside the NFC using the interrupts
 * @param session_register - Address of the session register which is read.
 * @return uint8_t - Returns data read from the session register.
 */
uint8_t read_session(uint8_t session_register)
{
	I2C0->CMD = I2C_CMD_START;  								/* send start bit */
	I2C0->TXDATA = 0x04; 										/* slave address */

	delay(DELAY_TIME);
	if(interrupt_flag_ack && ((I2C0->IF & I2C_IF_ACK) == 0))
	{
		interrupt_flag_ack = 0;
		I2C0->TXDATA = 0xFE;
		delay(DELAY_TIME);
	}
	if(interrupt_flag_ack && ((I2C0->IF & I2C_IF_ACK) == 0))
	{
		interrupt_flag_ack = 0;
		I2C0->TXDATA = session_register;
	}
	if(interrupt_flag_ack && ((I2C0->IF & I2C_IF_ACK) == 0))
	{
		interrupt_flag_ack = 0;
		I2C0->CMD = I2C_CMD_STOP;
	}
	delay(DELAY_TIME); 													/* Required for EEPROM Writing */

	for(int i = 0; i < 10; i++);
	I2C0->CMD = I2C_CMD_START;
	I2C0->TXDATA = NXP_NTAG_R;
	uint8_t data = 0;
	if(interrupt_flag_ack && ((I2C0->IF & I2C_IF_ACK) == 0))
	{
		interrupt_flag_ack = 0;
	}
	while(I2C0->IF & I2C_IF_RXDATAV)
	{
		data = I2C0->RXDATA;
		delay(DELAY_TIME);
	}

	I2C0->CMD = I2C_CMD_ACK;
	I2C0->CMD = I2C_CMD_STOP;
	delay(DELAY_TIME);
	return data;

}


/**
 * @brief This function is used to read the session register inside the NFC using polling method
 * @param session_register - Address of the session register which is read.
 * @return uint8_t - Returns data read from the session register.
 */
uint8_t i2c_read_session_poll(uint8_t session_register)
{
	I2C0->CMD = I2C_CMD_START;  									/* send start bit */
	I2C0->TXDATA = 0x04;

	while((I2C0->IF & I2C_IF_ACK) == 0);
	I2C0->IFC |= I2C_IFC_ACK;
	//if(interrupt_flag_ack)

	I2C0->TXDATA = 0xFE;

	while((I2C0->IF & I2C_IF_ACK) == 0);
	I2C0->IFC |= I2C_IFC_ACK;
	//if(interrupt_flag_ack)

	I2C0->TXDATA = session_register;
	while((I2C0->IF & I2C_IF_ACK) == 0);
	I2C0->IFC |= I2C_IFC_ACK;

	I2C0->CMD = I2C_CMD_STOP;
	delay(DELAY_TIME); 													/* Required for EEPROM Writing */


	I2C0->CMD = I2C_CMD_START;
	I2C0->TXDATA = NXP_NTAG_R;
	uint8_t data;

	while((I2C0->IF & I2C_IF_ACK) == 0);
	I2C0->IFC |= I2C_IFC_ACK;

	while(I2C0->IF & I2C_IF_RXDATAV)
	{
		data = I2C0->RXDATA;
	}

	I2C0->CMD = I2C_CMD_ACK;
	I2C0->CMD = I2C_CMD_STOP;
	delay(DELAY_TIME);
	return data;
}


/**
 * @brief This function is a polling read driver for NXP NTAG I2C NFC Device.
 * 16 Bytes are read in a single I2C read transfer.
 * @param register_address - Address of the starting register you want to read. Following 15 bytes will be also read.
 * @return None.
 */
uint8_t* i2c_read_poll(uint8_t register_address)
{
	uint8_t i;

	/* send start bit and slave address */
	I2C0->CMD = I2C_CMD_START;
	I2C0->TXDATA = 0x04;

	while((I2C0->IF & I2C_IF_ACK) == 0);
	I2C0->IFC |= I2C_IFC_ACK;

	I2C0->TXDATA = register_address;
	while((I2C0->IF & I2C_IF_ACK) == 0);
	I2C0->IFC |= I2C_IFC_ACK;

	I2C0->CMD = I2C_CMD_STOP;
	delay(DELAY_TIME);

	I2C0->CMD = I2C_CMD_START;
	I2C0->TXDATA = NXP_NTAG_R;

	while((I2C0->IF & I2C_IF_ACK) == 0);
	I2C0->IFC |= I2C_IFC_ACK;

	for(i = 0; i < 16; i++)
	{
		while((I2C0->IF & I2C_IF_RXDATAV) == 0);
		read[i] = I2C0->RXDATA;
		I2C0->CMD = I2C_CMD_ACK;
	}


	I2C0->CMD = I2C_CMD_STOP;
	delay(DELAY_TIME);
	while(I2C0->CMD & I2C_STATUS_PSTOP);
	return &read[0];

}


/**
 * @brief This function is used to disable the I2C peripheral.
 * @param None
 * @return None
 */
void i2c_disable(void)
{
	I2C0 -> ROUTEPEN &= ~I2C_ROUTEPEN_SCLPEN;
	I2C0 -> ROUTEPEN &=~ I2C_ROUTEPEN_SDAPEN;
	I2C_Enable(I2C0, false);
	GPIO_PinOutClear(gpioPortC, 10); 					/* SCL line */
	GPIO_PinOutClear(gpioPortC, 11);					/* SDA Line */
}


/**
 * @brief Testing function to check whether I2C is working
 * @note The output printfs should be 0x70, 0x2E, 0x6F, 0x6F, 0x62, 0x08, 0x00, 0x90, 0xE6, 0xb1, 0x29, 0xef, 0x57, 0x0b, 0x00, 0x6D, 0x6F.
 * @param void
 * @return void
 */
void i2c_test_blocking(void)
{
	uint8_t i;
	uint8_t write_nfc_row[16] = {0x70, 0x2E, 0x6F, 0x6F, 0x62, 0x08, 0x00, 0x90, 0xE6, 0xE1, 0xA6, 0xA8, 0xBC, 0xFE, 0x6D, 0x6F};
	uint8_t blue_gecko_addr[6] = {0xb1, 0x29, 0xef, 0x57, 0x0b, 0x00};

	for (i = 7; i <= 12; i++)
	{
		write_nfc_row[i] = blue_gecko_addr[i-7];
	}

	i2c_write_poll(0x01, write_nfc_row);

	delay(DELAY_TIME);

	uint8_t* received_i2c = i2c_read_poll(0x03);
	for (i= 0; i < 16; i++)
	{
		printf("Data into the address %x -- %x\n", (0x03+i), received_i2c[i]);
	}
}


void nxp_ntag_nfc_write(uint8_t *arr, uint8_t row)
{
	i2c_write_poll(row, arr);
}


uint8_t* nxp_ntag_nfc_read(uint8_t *arr, uint8_t row)
{
	arr = i2c_read_poll(row);
	return arr;
}


/**
 * @brief- IRQ Handler for I2C0 Peripheral
 * @param- None
 * @return- None
 */
void I2C0_IRQHandler()
{
	/* Disable All Interrupts */
	CORE_AtomicDisableIrq();

	if(I2C0->IF & I2C_IF_ACK)
	{
		I2C0->IFC |= I2C_IFC_ACK;
		interrupt_flag_ack = 1;
	}
	if(I2C0->IF & I2C_IF_NACK)
	{
		printf("NACK Received\n");
		I2C0->IFC |= I2C_IFC_NACK;
	}

	/* Enable All Interrupts */
	CORE_AtomicEnableIrq();
}

