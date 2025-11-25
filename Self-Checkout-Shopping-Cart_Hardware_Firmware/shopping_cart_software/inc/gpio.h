/*
 * @file gpio.h
 * @brief Header file for gpio.c
 *
 * @author: Siddhant Jajoo.
 * @date 12/08/2019
 * @copyright Copyright (c) 2019
 *
 */


#ifndef INC_GPIO_H_
#define INC_GPIO_H_


/* Macros */
#define GPIO_NFC_PORT							(gpioPortC)
#define GPIO_NFC_PIN							(9)								/* Pin number 10 */
#define GPIO_RISING_EDGE						(false)
#define GPIO_FALLING_EDGE						(true)
#define GPIO_INTERRUPT_ENABLE					(true)
#define GPIO_NFC_INTERRUPT_FLAG					(0x01 << GPIO_NFC_PIN)			/*FD Pin interrupt*/


/* Function Declarations */
void gpio_init(void);



#endif /* INC_GPIO_H_ */
