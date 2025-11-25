/*
 * @file connection_param.h
 * @brief This file consists of all the variables and macros required for bluetooth connection.
 *
 * @author: Siddhant Jajoo.
 * @date 11/16/2019
 * @copyright Copyright (c) 2019
 *
 */

#ifndef INC_CONNECTION_PARAM_H_
#define INC_CONNECTION_PARAM_H_


/* Macros for Connection Setup */
#define ADV_HANDLE					(0)
#define ADV_INTERVAL_MIN			(400)
#define ADV_INTERVAL_MAX			(400)
#define ADV_TIMING_DURATION			(0)
#define ADV_MAXEVENTS				(0)


#define CON_INTERVAL_MIN			(60)
#define CON_INTERVAL_MAX			(60)
#define CON_LATENCY					(3)
#define CON_TIMEOUT					(400)

#define SECURITY_CONFIGURE_FLAG		(0x00)




#endif /* INC_CONNECTION_PARAM_H_ */
