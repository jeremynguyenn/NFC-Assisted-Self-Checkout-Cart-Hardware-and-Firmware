################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../hardware/kit/common/bsp/bsp_stk.c 

OBJS += \
./hardware/kit/common/bsp/bsp_stk.o 

C_DEPS += \
./hardware/kit/common/bsp/bsp_stk.d 


# Each subdirectory must supply rules for building sources it contributes
hardware/kit/common/bsp/bsp_stk.o: ../hardware/kit/common/bsp/bsp_stk.c
	@echo 'Building file: $<'
	@echo 'Invoking: GNU ARM C Compiler'
	arm-none-eabi-gcc -g -gdwarf-2 -mcpu=cortex-m4 -mthumb -std=c99 '-D__STACK_SIZE=0x800' '-D__StackLimit=0x20000000' '-DHAL_CONFIG=1' '-D__HEAP_SIZE=0xD00' '-DEFR32BG13P632F512GM48=1' -I"D:\LPEDT\Git\Self-Checkout-Shopping-Cart\shopping_cart" -I"D:\LPEDT\Git\Self-Checkout-Shopping-Cart\shopping_cart\platform\Device\SiliconLabs\EFR32BG13P\Include" -I"D:\LPEDT\Git\Self-Checkout-Shopping-Cart\shopping_cart\platform\CMSIS\Include" -I"D:\LPEDT\Git\Self-Checkout-Shopping-Cart\shopping_cart\platform\emdrv\sleep\src" -I"D:\LPEDT\Git\Self-Checkout-Shopping-Cart\shopping_cart\hardware\kit\common\bsp" -I"D:\LPEDT\Git\Self-Checkout-Shopping-Cart\shopping_cart\platform\emdrv\sleep\inc" -I"D:\LPEDT\Git\Self-Checkout-Shopping-Cart\shopping_cart\hardware\kit\common\drivers" -I"D:\LPEDT\Git\Self-Checkout-Shopping-Cart\shopping_cart\hardware\kit\common\halconfig" -I"D:\LPEDT\Git\Self-Checkout-Shopping-Cart\shopping_cart\platform\emlib\src" -I"D:\LPEDT\Git\Self-Checkout-Shopping-Cart\shopping_cart\protocol\bluetooth\ble_stack\inc\common" -I"D:\LPEDT\Git\Self-Checkout-Shopping-Cart\shopping_cart\app\bluetooth\common\util" -I"D:\LPEDT\Git\Self-Checkout-Shopping-Cart\shopping_cart\platform\radio\rail_lib\common" -I"D:\LPEDT\Git\Self-Checkout-Shopping-Cart\shopping_cart\platform\emlib\inc" -I"D:\LPEDT\Git\Self-Checkout-Shopping-Cart\shopping_cart\platform\radio\rail_lib\protocol\ieee802154" -I"D:\LPEDT\Git\Self-Checkout-Shopping-Cart\shopping_cart\platform\Device\SiliconLabs\EFR32BG13P\Source\GCC" -I"D:\LPEDT\Git\Self-Checkout-Shopping-Cart\shopping_cart\platform\Device\SiliconLabs\EFR32BG13P\Source" -I"D:\LPEDT\Git\Self-Checkout-Shopping-Cart\shopping_cart\platform\emdrv\uartdrv\inc" -I"D:\LPEDT\Git\Self-Checkout-Shopping-Cart\shopping_cart\protocol\bluetooth\ble_stack\inc\soc" -I"D:\LPEDT\Git\Self-Checkout-Shopping-Cart\shopping_cart\hardware\kit\EFR32BG13_BRD4104A\config" -I"D:\LPEDT\Git\Self-Checkout-Shopping-Cart\shopping_cart\platform\emdrv\gpiointerrupt\inc" -I"D:\LPEDT\Git\Self-Checkout-Shopping-Cart\shopping_cart\platform\bootloader\api" -I"D:\LPEDT\Git\Self-Checkout-Shopping-Cart\shopping_cart\platform\radio\rail_lib\protocol\ble" -I"D:\LPEDT\Git\Self-Checkout-Shopping-Cart\shopping_cart\platform\emdrv\common\inc" -I"D:\LPEDT\Git\Self-Checkout-Shopping-Cart\shopping_cart\platform\halconfig\inc\hal-config" -I"D:\LPEDT\Git\Self-Checkout-Shopping-Cart\shopping_cart\platform\radio\rail_lib\chip\efr32\efr32xg1x" -I"D:\LPEDT\Git\Self-Checkout-Shopping-Cart\shopping_cart\platform\bootloader" -O0 -Wall -c -fmessage-length=0 -ffunction-sections -fdata-sections -mfpu=fpv4-sp-d16 -mfloat-abi=softfp -MMD -MP -MF"hardware/kit/common/bsp/bsp_stk.d" -MT"hardware/kit/common/bsp/bsp_stk.o" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


