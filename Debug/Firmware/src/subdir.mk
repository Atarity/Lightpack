################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Firmware/src/74HC595.c \
../Firmware/src/main.c \
../Firmware/src/vusb.c 

OBJS += \
./Firmware/src/74HC595.o \
./Firmware/src/main.o \
./Firmware/src/vusb.o 

C_DEPS += \
./Firmware/src/74HC595.d \
./Firmware/src/main.d \
./Firmware/src/vusb.d 


# Each subdirectory must supply rules for building sources it contributes
Firmware/src/%.o: ../Firmware/src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: AVR Compiler'
	avr-gcc -I"/home/brunql/EclipseProjects/AmbilightUSB/Firmware/inc" -I"/home/brunql/EclipseProjects/AmbilightUSB/Firmware/usbdrv/inc" -Wall -g2 -gstabs -Os -fpack-struct -fshort-enums -std=c99 -funsigned-char -funsigned-bitfields -mmcu=atmega8 -DF_CPU=12000000UL -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -c -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


