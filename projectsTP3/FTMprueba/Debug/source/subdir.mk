################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../source/App.c \
../source/FXOS8700CQ.c \
../source/SysTick.c \
../source/accelerometer.c \
../source/boardsInterface.c \
../source/can.c \
../source/comController2pc.c \
../source/coordHAL.c \
../source/i2c.c \
../source/pinsHandler.c \
../source/sphericalPos.c \
../source/spi.c \
../source/timer.c \
../source/uart.c 

OBJS += \
./source/App.o \
./source/FXOS8700CQ.o \
./source/SysTick.o \
./source/accelerometer.o \
./source/boardsInterface.o \
./source/can.o \
./source/comController2pc.o \
./source/coordHAL.o \
./source/i2c.o \
./source/pinsHandler.o \
./source/sphericalPos.o \
./source/spi.o \
./source/timer.o \
./source/uart.o 

C_DEPS += \
./source/App.d \
./source/FXOS8700CQ.d \
./source/SysTick.d \
./source/accelerometer.d \
./source/boardsInterface.d \
./source/can.d \
./source/comController2pc.d \
./source/coordHAL.d \
./source/i2c.d \
./source/pinsHandler.d \
./source/sphericalPos.d \
./source/spi.d \
./source/timer.d \
./source/uart.d 


# Each subdirectory must supply rules for building sources it contributes
source/%.o: ../source/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	arm-none-eabi-gcc -DCPU_MK64FN1M0VLL12 -D__USE_CMSIS -DDEBUG -I../source -I../ -I../SDK/CMSIS -I../SDK/startup -Og -fno-common -g3 -Wall -c -ffunction-sections -fdata-sections -ffreestanding -fno-builtin -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


