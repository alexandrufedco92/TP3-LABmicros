################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../source/ADC.c \
../source/App.c \
../source/Bell202.c \
../source/DAC.c \
../source/FSK_Demodulator.c \
../source/FSK_Modulator.c \
../source/FTM.c \
../source/PIT.c \
../source/SysTick.c \
../source/bitStreamQueue.c \
../source/gpio.c \
../source/measureFreq.c \
../source/timer.c \
../source/uart.c \
../source/waveGen.c 

OBJS += \
./source/ADC.o \
./source/App.o \
./source/Bell202.o \
./source/DAC.o \
./source/FSK_Demodulator.o \
./source/FSK_Modulator.o \
./source/FTM.o \
./source/PIT.o \
./source/SysTick.o \
./source/bitStreamQueue.o \
./source/gpio.o \
./source/measureFreq.o \
./source/timer.o \
./source/uart.o \
./source/waveGen.o 

C_DEPS += \
./source/ADC.d \
./source/App.d \
./source/Bell202.d \
./source/DAC.d \
./source/FSK_Demodulator.d \
./source/FSK_Modulator.d \
./source/FTM.d \
./source/PIT.d \
./source/SysTick.d \
./source/bitStreamQueue.d \
./source/gpio.d \
./source/measureFreq.d \
./source/timer.d \
./source/uart.d \
./source/waveGen.d 


# Each subdirectory must supply rules for building sources it contributes
source/%.o: ../source/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	arm-none-eabi-gcc -DCPU_MK64FN1M0VLL12 -D__USE_CMSIS -DDEBUG -I../source -I../ -I../SDK/CMSIS -I../SDK/startup -O0 -fno-common -g3 -Wall -c -ffunction-sections -fdata-sections -ffreestanding -fno-builtin -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


