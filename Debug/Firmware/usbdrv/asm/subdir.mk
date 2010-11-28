################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
S_UPPER_SRCS += \
../Firmware/usbdrv/asm/usbdrvasm.S 

ASM_SRCS += \
../Firmware/usbdrv/asm/usbdrvasm.asm 

OBJS += \
./Firmware/usbdrv/asm/usbdrvasm.o 

ASM_DEPS += \
./Firmware/usbdrv/asm/usbdrvasm.d 

S_UPPER_DEPS += \
./Firmware/usbdrv/asm/usbdrvasm.d 


# Each subdirectory must supply rules for building sources it contributes
Firmware/usbdrv/asm/%.o: ../Firmware/usbdrv/asm/%.S
	@echo 'Building file: $<'
	@echo 'Invoking: AVR Assembler'
	avr-gcc -x assembler-with-cpp -I"/home/brunql/EclipseProjects/AmbilightUSB/Firmware/inc" -I"/home/brunql/EclipseProjects/AmbilightUSB/Firmware/usbdrv/inc" -I"/home/brunql/EclipseProjects/AmbilightUSB/Firmware/usbdrv/asm" -g2 -gstabs -mmcu=attiny44 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -c -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Firmware/usbdrv/asm/%.o: ../Firmware/usbdrv/asm/%.asm
	@echo 'Building file: $<'
	@echo 'Invoking: AVR Assembler'
	avr-gcc -x assembler-with-cpp -I"/home/brunql/EclipseProjects/AmbilightUSB/Firmware/inc" -I"/home/brunql/EclipseProjects/AmbilightUSB/Firmware/usbdrv/inc" -I"/home/brunql/EclipseProjects/AmbilightUSB/Firmware/usbdrv/asm" -g2 -gstabs -mmcu=attiny44 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -c -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


