/*
 * @file barcode.h
 * @brief Header file for barcode.c.
 *
 * @author: Siddhant Jajoo.
 * @date 11/16/2019
 * @copyright Copyright (c) 2019
 *
 */

#ifndef INC_BARCODE_H_
#define INC_BARCODE_H_


#define BARCODE_PREAMBLE		(126)		/* Ascii equivalent of ~ */
#define BARCODE_POSTAMBLE		(96)		/* Ascii equivalent of ` */
#define ASCII_DIGIT_START		(48)		/* Ascii Value for interger 0 */



/* Variable Declarations */
struct barcode_packet									/* Example Packet data to send enclosed in the barcode scanner : ~013046shopping_cart`
 	 	 	 	 	 	 	 	 	 	 	 	 	 	 	 ~ = preamble; 013 = payload_size; 046 = cost; payload = shopping_cart; ` = postamble*/
{
	/* Preamble signifies the start of packet*/
	uint8_t preamble;

	/* The Payload size i.e The name size of the product embedded in the barcode. The maximum name size here
	 * can be of 999 characters*/
	uint8_t payload_size[3];

	/* The Cost size of the product. The maximum cost value a product can have here is 999*/
	uint8_t cost[3];

	/* The payload data pointer containg the payload data string*/
	char* payload;

	/* Postamble signifies the end of data packet*/
	uint8_t postamble;

};


struct barcode_packet barcode_packet;							/*Only one instance of barcode packet since the data
 	 	 	 	 	 	 	 	 	 	 	 	 	 	 	 	 	 is sent sequentially over bluetooth*/


/* Function Declarations */
void barcode_test_blocking(void);
void barcode_test_blocking_scanning(void);
int barcode_packet_create(struct barcode_packet* barcode_packet, int * payload_size);
char* itoa(int num, char* str, int base);
void swap(char *x, char *y);
#endif /* INC_BARCODE_H_ */
