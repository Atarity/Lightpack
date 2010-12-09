################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include subdir.mk
-include Firmware/usbdrv/src/subdir.mk
-include Firmware/usbdrv/asm/subdir.mk
-include Firmware/src/subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
ifneq ($(strip $(ASM_DEPS)),)
-include $(ASM_DEPS)
endif
ifneq ($(strip $(S_DEPS)),)
-include $(S_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 
LSS += \
AmbilightUSB.lss \

FLASH_IMAGE += \
AmbilightUSB.hex \

EEPROM_IMAGE += \
AmbilightUSB.eep \

SIZEDUMMY += \
sizedummy \


# All Target
all: AmbilightUSB.elf secondary-outputs

# Tool invocations
AmbilightUSB.elf: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: AVR C Linker'
	avr-gcc -Wl,-Map,AmbilightUSB.map -mmcu=atmega8 -o"AmbilightUSB.elf" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '

AmbilightUSB.lss: AmbilightUSB.elf
	@echo 'Invoking: AVR Create Extended Listing'
	-avr-objdump -h -S AmbilightUSB.elf  >"AmbilightUSB.lss"
	@echo 'Finished building: $@'
	@echo ' '

AmbilightUSB.hex: AmbilightUSB.elf
	@echo 'Create Flash image (ihex format)'
	-avr-objcopy -R .eeprom -O ihex AmbilightUSB.elf  "AmbilightUSB.hex"
	@echo 'Finished building: $@'
	@echo ' '

AmbilightUSB.eep: AmbilightUSB.elf
	@echo 'Create eeprom image (ihex format)'
	-avr-objcopy -j .eeprom --no-change-warnings --change-section-lma .eeprom=0 -O ihex AmbilightUSB.elf  "AmbilightUSB.eep"
	@echo 'Finished building: $@'
	@echo ' '

sizedummy: AmbilightUSB.elf
	@echo 'Invoking: Print Size'
	-avr-size --format=berkeley -t AmbilightUSB.elf
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(OBJS)$(C_DEPS)$(ASM_DEPS)$(EEPROM_IMAGE)$(FLASH_IMAGE)$(ELFS)$(LSS)$(S_DEPS)$(SIZEDUMMY)$(S_UPPER_DEPS) AmbilightUSB.elf
	-@echo ' '

secondary-outputs: $(LSS) $(FLASH_IMAGE) $(EEPROM_IMAGE) $(SIZEDUMMY)

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets
