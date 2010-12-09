################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Firmware/usbdrv/src/oddebug.c \
../Firmware/usbdrv/src/usbdrv.c 

OBJS += \
./Firmware/usbdrv/src/oddebug.o \
./Firmware/usbdrv/src/usbdrv.o 

C_DEPS += \
./Firmware/usbdrv/src/oddebug.d \
./Firmware/usbdrv/src/usbdrv.d 


# Each subdirectory must supply rules for building sources it contributes
Firmware/usbdrv/src/%.o: ../Firmware/usbdrv/src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: AVR Compiler'
	avr-gcc -I"/home/brunql/EclipseProjects/AmbilightUSB/Firmware/inc" -I"/home/brunql/EclipseProjects/AmbilightUSB/Firmware/usbdrv/inc" -Wall -g2 -gstabs -Os -fpack-struct -fshort-enums -std=c99 -funsigned-char -funsigned-bitfields -mmcu=attiny44 -DF_CPU=12000000UL -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -c -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


