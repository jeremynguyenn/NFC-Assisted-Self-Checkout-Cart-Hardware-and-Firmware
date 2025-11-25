/***************************************************************************//**
 * @file
 * @brief Silicon Labs Empty Example Project
 *
 * This example demonstrates the bare minimum needed for a Blue Gecko C application
 * that allows Over-the-Air Device Firmware Upgrading (OTA DFU). The application
 * starts advertising after boot and restarts advertising after a connection is closed.
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc. Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement. This
 * software is distributed to you in Source Code format and is governed by the
 * sections of the MSLA applicable to Source Code.
 *
 ******************************************************************************/


/* Board headers */
#include "init_mcu.h"
#include "init_board.h"
#include "init_app.h"
#include "ble-configuration.h"
#include "board_features.h"

/* Bluetooth stack headers */
#include "bg_types.h"
#include "native_gecko.h"
#include "gatt_db.h"

/* Libraries containing default Gecko configuration values */
#include "em_emu.h"
#include "em_cmu.h"

/* Device initialization header */
#include "hal-config.h"

#if defined(HAL_CONFIG)
#include "bsphalconfig.h"
#else
#include "bspconfig.h"
#endif

/* Application header */
#include "app.h"
#include "retargetserial.h"
#include "em_core.h"
#include <stdio.h>
#include <stdlib.h>
#include "infrastructure.h"

//Custom header files in INC directory
#include "inc/leuart.h"
#include "inc/connection_param.h"
#include "inc/external_events.h"
#include "inc/barcode.h"
#include "inc/i2c.h"
#include "inc/gpio.h"


/* Global Variables */
#define TIMER_CLK_FREQ 							((uint32)32768)				/* Timer Clock Frequency */
#define TIMER_S_TO_TICKS(s)						(TIMER_CLK_FREQ * s)		/* Convert seconds to timer ticks */
#define SOFT_TIMER_LEUART_INTERRUPT				(55)
#define SOFT_TIMER_NFC_INTERRUPT				(56)
#define CART_DEBUG_PRINTS						(1)							/* Comment this line to remove debug prints */*/
#define EXTRA_PAYLOAD_SIZE						(1 + 1 + 3 + 1 + 1)				/* 1 byte for "," , 1 byte for "$", 3 bytes for cost, 1 byte for "\n" , 1 byte to accomodate NULL character*/
#define MAX_BLUETOOTH_SIZE_SEND					(50)						/* This is the maximum bluetooth data size that can be sent in one go */


#ifdef CART_DEBUG_PRINTS
	#define printf(fmt, args...) printf(fmt, ## args)
#else
	#define printf(fmt, args...)
#endif


#ifndef MAX_ADVERTISERS
#define MAX_ADVERTISERS 1
#endif

#ifndef MAX_CONNECTIONS
#define MAX_CONNECTIONS 4
#endif


uint8_t bluetooth_stack_heap[DEFAULT_BLUETOOTH_HEAP(MAX_CONNECTIONS)];


/* Bluetooth stack configuration parameters (see "UG136: Silicon Labs Bluetooth C Application Developer's Guide" for details on each parameter) */
static gecko_configuration_t config = {
  .config_flags = 0,                                   /* Check flag options from UG136 */
#if defined(FEATURE_LFXO)
  .sleep.flags = SLEEP_FLAGS_DEEP_SLEEP_ENABLE,        /* Sleep is enabled */
#else
  .sleep.flags = 0,
#endif // LFXO
  .bluetooth.max_connections = MAX_CONNECTIONS,        /* Maximum number of simultaneous connections */
  .bluetooth.max_advertisers = MAX_ADVERTISERS,        /* Maximum number of advertisement sets */
  .bluetooth.heap = bluetooth_stack_heap,              /* Bluetooth stack memory for connection management */
  .bluetooth.heap_size = sizeof(bluetooth_stack_heap), /* Bluetooth stack memory for connection management */
  .bluetooth.sleep_clock_accuracy = 100,               /* Accuracy of the Low Frequency Crystal Oscillator in ppm. *
                                                       * Do not modify if you are using a module                  */
  .gattdb = &bg_gattdb_data,                           /* Pointer to GATT database */
  .ota.flags = 0,                                      /* Check flag options from UG136 */
  .ota.device_name_len = 3,                            /* Length of the device name in OTA DFU mode */
  .ota.device_name_ptr = "OTA",                        /* Device name in OTA DFU mode */
#if (HAL_PA_ENABLE)
  .pa.config_enable = 1,                               /* Set this to be a valid PA config */
#if defined(FEATURE_PA_INPUT_FROM_VBAT)
  .pa.input = GECKO_RADIO_PA_INPUT_VBAT,               /* Configure PA input to VBAT */
#else
  .pa.input = GECKO_RADIO_PA_INPUT_DCDC,               /* Configure PA input to DCDC */
#endif // defined(FEATURE_PA_INPUT_FROM_VBAT)
#endif // (HAL_PA_ENABLE)
  .rf.flags = GECKO_RF_CONFIG_ANTENNA,                 /* Enable antenna configuration. */
  .rf.antenna = GECKO_RF_ANTENNA,                      /* Select antenna path! */
};



/* Global Variables */
static uint8_t boot_to_dfu = 0;					// Flag for indicating DFU Reset must be performed
static uint8_t connection_handle;
int payload_size = 0;							/* Payload_size to be sent to the android application */
int total_cost = 0;								/* Total cost of the shopping list is stored here */


uint8_t write_nfc_row[16] = {0x03, 0x18, 0xD1, 0x01, 0x14, 0x54, 0x02, 0x65, 0x6E, 0x30, 0x30, 0x3A, 0x30, 0x42, 0x3A, 0x35};
uint8_t write_nfc_row_1[16] = {0x37, 0x3A, 0x45, 0x46, 0x3A, 0x32, 0x39, 0x3A, 0x42, 0x31, 0xFE, 0x4F, 0x00, 0x00, 0x00, 0x00};




/* Function Declarations */
static void handle_gecko_event(uint32_t evt_id, struct gecko_cmd_packet *evt);
static void bt_connection_init(void);
static void bt_server_print_address(void);
static void write_ble_address_to_nfc(void);



/**
 * @brief  Main function
 */
int main(void)
{
  /* Initialize device */
  initMcu();

  /* Initialize board */
  initBoard();

  /* Initialize application */
  initApp();

  /* Initializing Bluetooth Stack Configuration */
  gecko_init(&config);

  /* UART Console Setup for Debugging */
  RETARGET_SerialInit();
  RETARGET_SerialCrLf(true);

  printf("Self Checkout Shopping Cart.\n");
  printf("Team Name: Ashwathama.\n");

  //Initializing Structures Circular Buffer and Barcode Packet to value 0
  memset(&leuart_circbuff, 0, sizeof(struct leuart_circbuff));
  memset(&barcode_packet, 0, sizeof(struct barcode_packet));

  /* Initializing GPIO Interrupts for NFC, LEUART and I2C*/
  gpio_init();
  i2c_init();

  //Starting Software Timer for leuart interrupts.
  //gecko_cmd_hardware_set_soft_timer(TIMER_S_TO_TICKS(1), SOFT_TIMER_LEUART_INTERRUPT, 0);


  while (1)
  {
	  struct gecko_cmd_packet *evt = gecko_wait_event();
	  handle_gecko_event(BGLIB_MSG_ID(evt->header), evt);
  }
}



/**
 * @brief This function handles all the gecko events.
 */
static void handle_gecko_event(uint32_t evt_id, struct gecko_cmd_packet *evt)
{
	switch (evt_id) {
	case gecko_evt_dfu_boot_id:
		printf("Event: gecko_evt_dfu_boot_id\n");
		break;


	case gecko_evt_system_boot_id:
		printf("Event: gecko_evt_system_boot_id\n");

		/*Set up Bluetooth connection parameters and start advertising */
		bt_connection_init();

		break;


	/* This event is generated when a connected client has either
	 * 1) changed a Characteristic Client Configuration, meaning that they have enabled
	 * or disabled Notifications or Indications, or
	 * 2) sent a confirmation upon a successful reception of the indication. */
	case gecko_evt_gatt_server_characteristic_status_id:
		printf("Event: gecko_evt_gatt_server_characteristic_status_id\n");
		break;


	case gecko_evt_le_connection_opened_id:

		printf("Event: gecko_evt_le_connection_opened_id\n");
		char client_address_string[6];

		/* Disabling NFC software timer on successful connection */
		gecko_cmd_hardware_set_soft_timer(0, SOFT_TIMER_NFC_INTERRUPT, 0);

		/* Enabling leuart only after successful connection */
		leuart_init();

		/*Configure Connection Parameters*/
		gecko_cmd_le_connection_set_parameters(evt->data.evt_le_connection_opened.connection,
													CON_INTERVAL_MIN,CON_INTERVAL_MAX,CON_LATENCY,CON_TIMEOUT);
		connection_handle = evt->data.evt_le_connection_opened.connection;

		bd_addr client_address = evt->data.evt_le_connection_opened.address;
		sprintf(client_address_string,"X:X:X:X:%x:%x",client_address.addr[1],client_address.addr[0]);
		printf("Client Address: %s \n", client_address_string);

		break;


	case gecko_evt_sm_bonded_id:
		printf("Event: gecko_evt_sm_bonded_id\n");
		break;


	case gecko_evt_sm_bonding_failed_id:
		printf("Event: gecko_evt_sm_bonding_failed_id\n");
		break;


	case gecko_evt_le_connection_closed_id:
		/* Check if need to boot to dfu mode */
		printf("Event: gecko_evt_le_connection_closed_id\n");
		printf("Disconnected\n");
		total_cost = 0;
		gecko_cmd_system_set_tx_power(0);

		if (boot_to_dfu) {
			/* Enter to DFU OTA mode */
			gecko_cmd_system_reset(2);

		} else {

			/* Stop timer in case client disconnected before indications were turned off */
			gecko_cmd_hardware_set_soft_timer(0, 0, 0);

			/* Start NFC GPIO interrupt to reconnect using NFC */
			GPIO_IntConfig(GPIO_NFC_PORT, GPIO_NFC_PIN, GPIO_RISING_EDGE, GPIO_FALLING_EDGE, GPIO_INTERRUPT_ENABLE);

			/* Disable leuart peripheral over here */
			leuart_disable();
		}
		break;


	case gecko_evt_gatt_server_execute_write_completed_id:
		printf("Event: gecko_evt_gatt_server_execute_write_completed_id\n");
		break;


	case gecko_evt_gatt_service_id:
		printf("Event: gecko_evt_gatt_service_id\n");
		break;


	case gecko_evt_gatt_characteristic_value_id:
		printf("Event: gecko_evt_gatt_characteristic_value_id\n");
		break;


	case gecko_evt_hardware_soft_timer_id:
		switch (evt->data.evt_hardware_soft_timer.handle){
		case SOFT_TIMER_LEUART_INTERRUPT:

			printf("SOFT_TIMER_LEUART_INTERRUPT\n");
			if(!leuart_buffer_empty_status())
			{
				CORE_AtomicDisableIrq();
				external_event |= EVENT_LEUART;
				gecko_external_signal(external_event);
				CORE_AtomicEnableIrq();
			}

			break;

		case SOFT_TIMER_NFC_INTERRUPT:

			printf("SOFT_TIMER_NFC_INTERRUPT\n");
			/* Stop advertising and enable interrupts */
			gecko_cmd_le_gap_stop_advertising(0);
			GPIO_IntConfig(GPIO_NFC_PORT, GPIO_NFC_PIN, GPIO_RISING_EDGE, GPIO_FALLING_EDGE, GPIO_INTERRUPT_ENABLE);


		}
		break;


	case gecko_evt_gatt_server_attribute_value_id:
		printf("Event: gecko_evt_gatt_server_attribute_value_id\n");

		char cost_string[4];
		char *ptr = &cost_string[0];
		ptr = itoa(total_cost, cost_string, 10);
		printf("Total cost decimal: %d\n", total_cost);
		printf("Total Cost String: %s\n",cost_string);
		printf("Received response: %c \n", evt->data.evt_gatt_server_attribute_value.value.data[0]);
		if (evt->data.evt_gatt_server_attribute_value.value.len && (evt->data.evt_gatt_server_attribute_value.value.data[0] == 'B'))
		{
			printf("Sending Bill\n");
			printf("Send Result: %x\n",gecko_cmd_gatt_server_send_characteristic_notification(
					(uint8)connection_handle, (uint8)gattdb_product_name, (uint8)4, (const uint8*)ptr)->result);
		}
		else if (evt->data.evt_gatt_server_attribute_value.value.len && (evt->data.evt_gatt_server_attribute_value.value.data[0] == 'P'))
		{
			printf("Total Cost set to 0\n");
			total_cost = 0;
			gecko_cmd_le_connection_close(connection_handle);		//Closing the connection since payment is completed
		}



		break;


	case gecko_evt_gatt_procedure_completed_id:
		printf("GATT Procedure completed\n");
		break;


	case gecko_evt_system_external_signal_id:
		printf("Event: gecko_evt_system_external_signal_id\n");

		if (evt->data.evt_system_external_signal.extsignals & EVENT_LEUART)
		{

			CORE_AtomicDisableIrq();
			external_event &= ~EVENT_LEUART;
			CORE_AtomicEnableIrq();

			printf("External Signal Event for LEUART received.\n");

			//Initial data - Not necessary to initialize
			uint32_t temp_read_index = leuart_circbuff.read_index;

			/* Read data from leuart_circbuff till it is empty */
			while(!leuart_buffer_empty_status())
			{

				//TODO: Add a Software Timer for UART Receiving external event trigger.
				if(leuart_circbuff.buffer[leuart_circbuff.read_index] == BARCODE_PREAMBLE)
				{
					temp_read_index = leuart_circbuff.read_index + 7;							/* 7 as per the packet structure */

					/* Start making packet here */
					total_cost += barcode_packet_create(&barcode_packet, &payload_size);

				}
				else if(leuart_circbuff.buffer[leuart_circbuff.read_index] == BARCODE_POSTAMBLE)
				{
					/* Temporary packet to send data. Maxmum size BLE can transfer at a time is 20 bytes */
					char packet_send[MAX_BLUETOOTH_SIZE_SEND];
					int total_payload_size = payload_size + EXTRA_PAYLOAD_SIZE;

					memset(packet_send, 0, sizeof(packet_send));

					/* Pop the postamble and the \r character*/
					barcode_packet.postamble = leuart_buffer_pop();
					leuart_buffer_pop();														/* To remove the redundant \r received from the barcode*/

					/* Copy the received buffer data into the packet structure and append a null character to the string at the end */
					memcpy(&barcode_packet.payload[0], &leuart_circbuff.buffer[temp_read_index], payload_size);
					barcode_packet.payload[payload_size] = '\0';								/* Adding a NULL character at the end of string*/


					//TODO: Now Start Sending data over bluetooth here.
					snprintf(packet_send, total_payload_size, "%s,$%s\n", barcode_packet.payload, &barcode_packet.cost[0]);
					printf("Packet to be sent over Bluetooth: %s \n", packet_send);

					do
					{
						if (total_payload_size <= MAX_BLUETOOTH_SIZE_SEND)
						{
							printf("Send Result: %x\n",gecko_cmd_gatt_server_send_characteristic_notification(
									connection_handle, gattdb_product_name, total_payload_size, (const uint8 *)&packet_send[0])->result);
						}
						total_payload_size -= MAX_BLUETOOTH_SIZE_SEND;
					}
					while( total_payload_size > 0 /*&& (total_payload_size / MAX_BLUETOOTH_SIZE_SEND)*/);


					/* Free the barcode_packet data structure after sending data */
					free(barcode_packet.payload);

					/* Initializing the barcode_packet data structure to zero */
					memset(&barcode_packet, 0, sizeof(struct barcode_packet));
				}
				else
				{
					leuart_buffer_pop();														/* To pop remaining data*/
				}
			}
		}

		if (evt->data.evt_system_external_signal.extsignals & EVENT_NFC_GPIO)
		{

			CORE_AtomicDisableIrq();
			external_event &= ~EVENT_NFC_GPIO;
			CORE_AtomicEnableIrq();

			printf("External Signal Event for NFC FD pin interrupt received.\n");

			gecko_cmd_le_gap_start_advertising(0, le_gap_general_discoverable, le_gap_connectable_scannable);

			gecko_cmd_hardware_set_soft_timer(TIMER_S_TO_TICKS(15), SOFT_TIMER_NFC_INTERRUPT, 1);

		}

		break;



	/* Events related to OTA upgrading
			   ----------------------------------------------------------------------------- */

	/* Checks if the user-type OTA Control Characteristic was written.
	 * If written, boots the device into Device Firmware Upgrade (DFU) mode. */
	case gecko_evt_gatt_server_user_write_request_id:
		printf("Write request receieved from the mobile app\n");
		if (evt->data.evt_gatt_server_user_write_request.characteristic == gattdb_ota_control) {
			/* Set flag to enter to OTA mode */
			boot_to_dfu = 1;
			/* Send response to Write Request */
			gecko_cmd_gatt_server_send_user_write_response(
					evt->data.evt_gatt_server_user_write_request.connection,
					gattdb_ota_control,
					bg_err_success);

			/* Close connection to enter to DFU OTA mode */
			gecko_cmd_le_connection_close(evt->data.evt_gatt_server_user_write_request.connection);
		}
		break;

	default:
		break;
	}
}



/**
 * @brief This function prints the Server Bluetooth Public address
 * @param void
 * @return void
 */
static void bt_server_print_address(void)
{
	char server_address_string[6];
	struct gecko_msg_system_get_bt_address_rsp_t *bt_addr = gecko_cmd_system_get_bt_address();
	bd_addr *server_address = &bt_addr->address;

	sprintf(server_address_string,"%x:%x:%x:%x:%x:%x", server_address->addr[5],server_address->addr[4]
												,server_address->addr[3],server_address->addr[2],
												server_address->addr[1],server_address->addr[0]);
	printf("Server Address: %s \n", server_address_string);
}


/**
 * @brief This function initializes the bluetooth connection.
 * The function prints the server address, deletes all previous bondings, sets into bonding mode,
 * sets the transmit power to zero, configures and starts advertising.
 * @param void
 * @return void
 */
static void bt_connection_init(void)
{
	//Prints the Server Bluetooth Public address
	bt_server_print_address();

	//Setting Transmit Power
	gecko_cmd_system_set_tx_power(0);

	// Delete all previous bondings
	gecko_cmd_sm_delete_bondings();

	// Configure Security
	gecko_cmd_sm_configure(SECURITY_CONFIGURE_FLAG,sm_io_capability_noinputnooutput);

	//Set into Bondable Mode
	gecko_cmd_sm_set_bondable_mode(1);

	// Configure and start general advertising and enable connections.
	gecko_cmd_le_gap_set_advertise_timing(ADV_HANDLE, ADV_INTERVAL_MIN, ADV_INTERVAL_MAX, ADV_TIMING_DURATION, ADV_MAXEVENTS);

}


static void write_ble_address_to_nfc(void)
{
	struct gecko_msg_system_get_bt_address_rsp_t *add = gecko_cmd_system_get_bt_address();
	char address[40];
	snprintf(address, 40, "%02x:%02x:%02x:%02x:%02x:%02x",
			add->address.addr[5],
			add->address.addr[4],
			add->address.addr[3],
			add->address.addr[2],
			add->address.addr[1],
			add->address.addr[0]
	);

	uint8_t bluegecko_address[17] = {((add->address.addr[5] & (0xF0)) >> 4), (add->address.addr[5] & (0x0F)), 58, ((add->address.addr[4] & (0xF0)) >> 4), (add->address.addr[4] & (0x0F)), 0x3A,
									 ((add->address.addr[3] & (0xF0)) >> 4), (add->address.addr[3] & (0x0F)), 58, ((add->address.addr[2] & (0xF0)) >> 4), (add->address.addr[2] & (0x0F)), 0x3A,
									 ((add->address.addr[1] & (0xF0)) >> 4), (add->address.addr[1] & (0x0F)), 58, ((add->address.addr[0] & (0xF0)) >> 4), (add->address.addr[0] & (0x0F))};
	char bluegecko_char_address[17];
	printf("Blue Gecko Address : ");
	char *bt_ptr = bluegecko_char_address;
	for(int i = 0; i < 17; i++)
	{
		//printf("%x", bluegecko_address[i]);
		bt_ptr = itoa(bluegecko_address[i], bt_ptr, 16);
		bt_ptr++;
	}
	for (int i = 0; i < 17; i++)
	{
		if (i == 2 || i == 5 || i == 8 || i == 11 || i == 14)
		{
			bluegecko_char_address[i] = ':';
		}
	}
	printf("Blue gecko char add :");
	for (int i = 0; i < 17; i++)
	printf("%d\n", bluegecko_char_address[i]);
	for (int i = 9; i < 16; i++)
		write_nfc_row[i] = bluegecko_char_address[i-9];
	for(int i = 0; i < 10; i++)
		write_nfc_row_1[i] = bluegecko_char_address[i + 7];

//	printf("Writw row 1 %s\n", (char *)write_nfc_row);
	printf("Writw row 2 %s\n", write_nfc_row_1);
	i2c_write_poll(0x02, write_nfc_row_1);
	for(int i = 0; i < 1000000; i++);
	i2c_write_poll(0x01, write_nfc_row);
	printf("Address written in NFC Module\n");
}


/** @} (end addtogroup app) */
/** @} (end addtogroup Application) */
