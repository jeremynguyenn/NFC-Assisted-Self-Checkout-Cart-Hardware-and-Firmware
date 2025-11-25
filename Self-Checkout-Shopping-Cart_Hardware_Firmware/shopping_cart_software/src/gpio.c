/*
 * @file leuart.c
 * @brief This file consists of functions related to gpio initialization and interrupt handlers.
 *
 * @author: Siddhant Jajoo.
 * @date 12/08/2019
 * @copyright Copyright (c) 2019
 *
 */



#include <stdio.h>
#include "em_gpio.h"
#include "em_core.h"
#include "native_gecko.h"
#include "inc/gpio.h"
#include "inc/external_events.h"


/**
 * @brief This function initializes the GPIO for NFC interrupt.
 * @param void
 * @return void
 */
void gpio_init(void)
{
	/* Configure GPIO for FD pin on NFC Sensor */
	GPIO_PinModeSet( GPIO_NFC_PORT , GPIO_NFC_PIN , gpioModeInputPull , 1 );

	/* Clear all the Gpio interrupts Flags */
	GPIO_IntClear(GPIO_NFC_INTERRUPT_FLAG);

	/* Configure and Enable GPIO Interrupt For Push Button 1 */
	GPIO_IntConfig(GPIO_NFC_PORT, GPIO_NFC_PIN, GPIO_RISING_EDGE, GPIO_FALLING_EDGE, GPIO_INTERRUPT_ENABLE);

	/* Enable NVIC interrupt */
	NVIC_EnableIRQ(GPIO_ODD_IRQn);
}


/**
 * @brief Callback function for GPIO interrupt.
 */
void GPIO_ODD_IRQHandler(void)
{
	/* Disable All Interrupts */
	CORE_AtomicDisableIrq();

	/* Acknowledge and Clear the Interrupt */
	uint32_t flags = GPIO_IntGet();
	GPIO_IntClear(flags);


	if (flags & GPIO_NFC_INTERRUPT_FLAG)
	{
		/* Update the External Event after every NFC FD PIN interrupt */
		external_event |= EVENT_NFC_GPIO;
		gecko_external_signal(external_event);

		/* Disable the GPIO interrupt here to remove multiple NFC interrupts and enable only after bluetooth
		 connection is established or set a software timer to enable enable interrupt after a certain amount of time */
		GPIO_IntDisable(GPIO_NFC_INTERRUPT_FLAG);
	}

	/* Enable All Interrupts */
	CORE_AtomicEnableIrq();

}
