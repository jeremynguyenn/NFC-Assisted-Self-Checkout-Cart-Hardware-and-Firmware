/*
 * @file external_events.h
 * @brief This file consists of all the variables and macros required for external events.
 *
 * @author: Siddhant Jajoo.
 * @date 11/16/2019
 * @copyright Copyright (c) 2019
 *
 */

#ifndef INC_EXTERNAL_EVENTS_H_
#define INC_EXTERNAL_EVENTS_H_


/* Event Bits assigned to different Signal Events */
#define EVENT_LEUART						(0x01)
#define EVENT_NFC_GPIO						(0x02)


/* Global Variable for Event Status */
volatile uint32_t external_event;


#endif /* INC_EXTERNAL_EVENTS_H_ */
