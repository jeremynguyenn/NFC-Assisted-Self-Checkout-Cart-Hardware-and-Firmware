################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include src/subdir.mk
-include protocol/bluetooth/lib/EFR32BG13P/GCC/subdir.mk
-include platform/emlib/src/subdir.mk
-include platform/emdrv/sleep/src/subdir.mk
-include platform/Device/SiliconLabs/EFR32BG13P/Source/GCC/subdir.mk
-include platform/Device/SiliconLabs/EFR32BG13P/Source/subdir.mk
-include hardware/kit/common/drivers/subdir.mk
-include hardware/kit/common/bsp/subdir.mk
-include app/bluetooth/common/util/subdir.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 

# All Target
all: shopping_cart.axf

# Tool invocations
shopping_cart.axf: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: GNU ARM C Linker'
	arm-none-eabi-gcc -g -gdwarf-2 -mcpu=cortex-m4 -mthumb -T "C:\Users\satya\SimplicityStudio\v4_workspace\shopping_cart\efr32bg13p632f512gm48.ld" -Xlinker --gc-sections -Xlinker -Map="shopping_cart.map" -mfpu=fpv4-sp-d16 -mfloat-abi=softfp --specs=nano.specs -o shopping_cart.axf -Wl,--start-group "./app/bluetooth/common/util/infrastructure.o" "./app.o" "./application_properties.o" "./gatt_db.o" "./init_app.o" "./init_board.o" "./init_mcu.o" "./main.o" "./pti.o" "./hardware/kit/common/bsp/bsp_stk.o" "./hardware/kit/common/drivers/mx25flash_spi.o" "./hardware/kit/common/drivers/retargetio.o" "./hardware/kit/common/drivers/retargetserial.o" "./platform/Device/SiliconLabs/EFR32BG13P/Source/GCC/startup_efr32bg13p.o" "./platform/Device/SiliconLabs/EFR32BG13P/Source/system_efr32bg13p.o" "./platform/emdrv/sleep/src/sleep.o" "./platform/emlib/src/em_assert.o" "./platform/emlib/src/em_burtc.o" "./platform/emlib/src/em_cmu.o" "./platform/emlib/src/em_core.o" "./platform/emlib/src/em_cryotimer.o" "./platform/emlib/src/em_crypto.o" "./platform/emlib/src/em_emu.o" "./platform/emlib/src/em_eusart.o" "./platform/emlib/src/em_gpio.o" "./platform/emlib/src/em_i2c.o" "./platform/emlib/src/em_leuart.o" "./platform/emlib/src/em_msc.o" "./platform/emlib/src/em_rmu.o" "./platform/emlib/src/em_rtcc.o" "./platform/emlib/src/em_se.o" "./platform/emlib/src/em_system.o" "./platform/emlib/src/em_timer.o" "./platform/emlib/src/em_usart.o" "./src/barcode.o" "./src/gpio.o" "./src/i2c.o" "./src/leuart.o" "C:\Users\satya\SimplicityStudio\v4_workspace\shopping_cart\protocol\bluetooth\lib\EFR32BG13P\GCC\libbluetooth.a" "C:\Users\satya\SimplicityStudio\v4_workspace\shopping_cart\protocol\bluetooth\lib\EFR32BG13P\GCC\libpsstore.a" "C:\Users\satya\SimplicityStudio\v4_workspace\shopping_cart\protocol\bluetooth\lib\EFR32BG13P\GCC\libmbedtls.a" "C:\Users\satya\SimplicityStudio\v4_workspace\shopping_cart\protocol\bluetooth\lib\EFR32BG13P\GCC\binapploader.o" "C:\Users\satya\SimplicityStudio\v4_workspace\shopping_cart\platform\radio\rail_lib\autogen\librail_release\librail_efr32xg13_gcc_release.a" -lm -Wl,--end-group -Wl,--start-group -lgcc -lc -lnosys -Wl,--end-group
	@echo 'Finished building target: $@'
	@echo ' '

	@echo 'Building hex file: shopping_cart.hex'
	arm-none-eabi-objcopy -O ihex "shopping_cart.axf" "shopping_cart.hex"
	@echo ' '

	@echo 'Building bin file: shopping_cart.bin'
	arm-none-eabi-objcopy -O binary "shopping_cart.axf" "shopping_cart.bin"
	@echo ' '

	@echo 'Building s37 file: shopping_cart.s37'
	arm-none-eabi-objcopy -O srec "shopping_cart.axf" "shopping_cart.s37"
	@echo ' '

	@echo 'Running size tool'
	arm-none-eabi-size "shopping_cart.axf"
	@echo ' '

# Other Targets
clean:
	-$(RM) $(EXECUTABLES)$(OBJS)$(C_DEPS) shopping_cart.axf
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets
