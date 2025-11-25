/*
 * @file barcode.c
 * @brief This file consists of functions related to Barcode Sensor.
 *
 * @author: Siddhant Jajoo.
 * @date 11/16/2019
 * @copyright Copyright (c) 2019
 *
 */


#include <stdio.h>
#include <stdlib.h>
#include "inc/barcode.h"
#include "inc/leuart.h"


/**
 * @brief This function fetches the payload size from the barcode packet structure and converts it to an integer.
 * @param struct barcode_packet* barcode_packet The pointer to the barcode packet structure
 * @return The payload size
 */
static int barcode_payload_size_fetch(struct barcode_packet* barcode_packet)
{
	int payload_size = 0;

	barcode_packet->payload_size[0] = leuart_buffer_pop();
	barcode_packet->payload_size[1] = leuart_buffer_pop();
	barcode_packet->payload_size[2] = leuart_buffer_pop();

	payload_size += (barcode_packet->payload_size[0] - ASCII_DIGIT_START) * 100;
	payload_size += (barcode_packet->payload_size[1] - ASCII_DIGIT_START) * 10;
	payload_size += (barcode_packet->payload_size[2] - ASCII_DIGIT_START) * 1;

	return payload_size;

}


/**
 * @brief This function fetches the cost from the barcode packet structure and converts it to an integer.
 * @param struct barcode_packet* barcode_packet The pointer to the barcode packet structure
 * @return The cost
 */
static int barcode_cost_fetch(struct barcode_packet* barcode_packet)
{
	int cost = 0;

	barcode_packet->cost[0] = leuart_buffer_pop();
	barcode_packet->cost[1] = leuart_buffer_pop();
	barcode_packet->cost[2] = leuart_buffer_pop();

	cost += (barcode_packet->cost[0] - ASCII_DIGIT_START) * 100;
	cost += (barcode_packet->cost[1] - ASCII_DIGIT_START) * 10;
	cost += (barcode_packet->cost[2] - ASCII_DIGIT_START) * 1;

	return cost;
}


/**
 * @brief This function creates a barcode packet structure as per the data received from the leuart_buffer on pop()
 * and returns the payload size to a pointer passed as a parameter
 * @param struct barcode_packet* barcode_packet, int *payload_size
 * @return void
 */
int barcode_packet_create(struct barcode_packet* barcode_packet, int *payload_size)
{

	if( barcode_packet->payload == NULL && leuart_circbuff.read_index < LEUART_BUFFER_MAXSIZE)
	{
		int local_payload_size, cost;

		barcode_packet->preamble = leuart_buffer_pop();

		/* Fetching and converting the character data of Payload size and Cost size here into integer digits */
		local_payload_size = barcode_payload_size_fetch(barcode_packet);
		printf("Payload_size: %d\n", local_payload_size);

		cost = barcode_cost_fetch(barcode_packet);
		printf("Cost: %d\n", cost);

		barcode_packet->payload = malloc(sizeof(char) * (local_payload_size + 1));
		if(barcode_packet->payload == NULL)
		{
			printf("ERROR: Cannot Malloc Payload data in barcode_packet_create() function./n");
		}

		*payload_size = local_payload_size;
		return cost;
	}

	return -1;
}


/**
 * @brief barcode testing function in blocking mode by sending data
 * @note Output should be 2,0,0,2,39,1,SS,SS where SS is checksum value and varies as per the data packet.
 * @param void
 * @return void
 */
void barcode_test_blocking(void)
{
	const uint8_t cmd[9] = {0x7E, 0x00, 0x07, 0x01, 0x00, 0x2A, 0x02, 0xD8, 0x0F};


	/* Disable Interrupts over here in order to support blocking */
	if ((LEUART0->IEN & LEUART_IEN_RXDATAV) || (LEUART0->IEN & LEUART_IEN_TXC))
	{
		LEUART_IntDisable(LEUART0, LEUART_IEN_RXDATAV | LEUART_IEN_TXC);
	}


	leuart_send(LEUART0, cmd[0]);
	printf("Data Sent\n");
	leuart_send(LEUART0, cmd[1]);
	printf("Data Sent\n");
	leuart_send(LEUART0, cmd[2]);
	printf("Data Sent\n");
	leuart_send(LEUART0, cmd[3]);
	printf("Data Sent\n");
	leuart_send(LEUART0, cmd[4]);
	printf("Data Sent\n");
	leuart_send(LEUART0, cmd[5]);
	printf("Data Sent\n");
	leuart_send(LEUART0, cmd[6]);
	printf("Data Sent\n");
	leuart_send(LEUART0, cmd[7]);
	printf("Data Sent\n");
	leuart_send(LEUART0, cmd[8]);
	printf("Data Sent\n");


	printf("DATA: %x\n", leuart_rcv(LEUART0));
	printf("DATA: %x\n", leuart_rcv(LEUART0));
	printf("DATA: %x\n", leuart_rcv(LEUART0));
	printf("DATA: %x\n", leuart_rcv(LEUART0));
	printf("DATA: %x\n", leuart_rcv(LEUART0));
	printf("DATA: %x\n", leuart_rcv(LEUART0));
	printf("DATA: %x\n", leuart_rcv(LEUART0));
	printf("DATA: %x\n", leuart_rcv(LEUART0));

}


/**
 * @brief barcode testing function in blocking mode by scanning barcode
 * @note Output should be as per the barcode scanned in the datasheet
 * @param void
 * @return void
 */

void barcode_test_blocking_scanning(void)
{

	/* Disable Interrupts over here in order to support blocking */
	if ((LEUART0->IEN & LEUART_IEN_RXDATAV) || (LEUART0->IEN & LEUART_IEN_TXC))
	{
		LEUART_IntDisable(LEUART0, LEUART_IEN_RXDATAV | LEUART_IEN_TXC);
	}


	printf("Testing Barcode UART by scanning the barcode.\n");

	printf("DATA: %x\n", leuart_rcv(LEUART0));
	printf("DATA: %x\n", leuart_rcv(LEUART0));
	printf("DATA: %x\n", leuart_rcv(LEUART0));
	printf("DATA: %x\n", leuart_rcv(LEUART0));
	printf("DATA: %x\n", leuart_rcv(LEUART0));
	printf("DATA: %x\n", leuart_rcv(LEUART0));
	printf("DATA: %x\n", leuart_rcv(LEUART0));
	printf("DATA: %x\n", leuart_rcv(LEUART0));

}

void swap(char *x, char *y) {
	char t = *x; *x = *y; *y = t;
}

char* reverse(char *buffer, int i, int j)
{
	while (i < j)
		swap(&buffer[i++], &buffer[j--]);

	return buffer;
}


char* itoa(int value, char* buffer, int base)
{
	// invalid input
	if (base < 2 || base > 32)
		return buffer;

	// consider absolute value of number
	int n = abs(value);

	int i = 0;
	while (n)
	{
		int r = n % base;

		if (r >= 10)
			buffer[i++] = 65 + (r - 10);
		else
			buffer[i++] = 48 + r;

		n = n / base;
	}

	// if number is 0
	if (i == 0)
		buffer[i++] = '0';

	// If base is 10 and value is negative, the resulting string
	// is preceded with a minus sign (-)
	// With any other base, value is always considered unsigned
	if (value < 0 && base == 10)
		buffer[i++] = '-';

	buffer[i] = '\0'; // null terminate string

	// reverse the string and return it
	return reverse(buffer, 0, i - 1);
}

