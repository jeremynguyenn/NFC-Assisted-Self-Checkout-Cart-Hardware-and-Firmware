/*
 * @file leuart.h
 * @brief Header file for leuart.c.
 *
 * @author: Siddhant Jajoo.
 * @date 11/05/2019
 * @copyright Copyright (c) 2019
 *
 */
#ifndef INC_LEUART_H_
#define INC_LEUART_H_

#include <stdbool.h>
#include "em_leuart.h"


#define LEUART_TX_PORT							(gpioPortD)
#define LEUART_TX_PIN							(10)								/* TX (Pin Number 7) */
#define LEUART_RX_PORT							(gpioPortD)
#define LEUART_RX_PIN							(11)								/* RX (Pin Number 9) */


#define LEUART_BUFFER_MAXSIZE					(512)
#define LEUART_BUFFER_INTERRUPT_SIZE			(10)
#define LEUART_INTERRUPT_TIMER					(1)


/* Variable Declaration */
struct leuart_circbuff
{
	/* A circular buffer of size 512 bytes to store the contents of the data received from the UART register*/
	char buffer[LEUART_BUFFER_MAXSIZE];

	/*The write index for the circular buffer*/
	uint32_t write_index;

	/*The read index for the circular buffer*/
	uint32_t read_index;

	/*The current number of elements is stored here*/
	volatile uint32_t buffer_count;

	/* The buffer count value in order to trigger the gecko_evt_system_external_signal_id event
	   The event would be triggered when buffer_interrupt_count == BUFFER_INTERRUPT_SIZE */
	volatile uint32_t buffer_interrupt_count;

	/*This variable is set true after every BUFFER_INTERRUPT_SIZE bytes are received.
	  If this flag is true the control would be transferred to gecko_evt_system_external_signal_id event */
	volatile bool buffer_interrupt_flag;
};


struct leuart_circbuff leuart_circbuff;					/* Only one instance of leuart buffer since
															there is only one leuart peripheral available i.e LEUART0*/


/* Function declarations */
void leuart_init(void);
void leuart_buffer_push(void);
char leuart_buffer_pop(void);
bool leuart_buffer_empty_status(void);
void leuart_disable(void);
void leuart_loopback_test_blocking(void);
void leuart_loopback_test_non_blocking(void);


/**
 * @brief This function increments the read or write index of the circular buffer based on the
 * maximum size of the circular buffer.
 * @note This function might lead to overwriting of the previous data on index rollover.
 * Developer must make sure to read the old data as soon as possible.
 * @param The read or write index value of the circular buffer
 * @return void
 */
static inline uint32_t leuart_circbuff_index_increment(uint32_t index)
{
	if(index == LEUART_BUFFER_MAXSIZE - 1)
	{
		index = 0;
	}
	else
	{
		index++;
	}

	return index;
}



/**
 * @brief Function to send data using LEUART peripheral.
 * @param LEUART_TypeDef* The LEUART peripheral being used. eg: LEUART0.
 * @param uint8_data 8 bit data to be sent.
 * @return void
 */
static inline void leuart_send(LEUART_TypeDef *leuart, uint8_t data)
{
	uint8_t tx_data = data;
	LEUART_Tx(leuart, tx_data);
}


/**
 * @brief Function to receive data using LEUART Peripheral.
 * @param LEUART_TypeDef* The LEUART peripheral being used. eg: LEUART0.
 * @return data
 */
static inline char leuart_rcv(LEUART_TypeDef *leuart)
{
	return LEUART_Rx(leuart);
}



#endif /* INC_LEUART_H_ */
