################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../source/ADC.c \
../source/App.c \
../source/DAC.c \
../source/FTM.c \
../source/SysTick.c \
../source/comController2pc.c \
../source/pinsHandler.c \
../source/uart.c 

OBJS += \
./source/ADC.o \
./source/App.o \
./source/DAC.o \
./source/FTM.o \
./source/SysTick.o \
./source/comController2pc.o \
./source/pinsHandler.o \
./source/uart.o 

C_DEPS += \
./source/ADC.d \
./source/App.d \
./source/DAC.d \
./source/FTM.d \
./source/SysTick.d \
./source/comController2pc.d \
./source/pinsHandler.d \
./source/uart.d 


# Each subdirectory must supply rules for building sources it contributes
source/%.o: ../source/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	arm-none-eabi-gcc -DCPU_MK64FN1M0VLL12 -D__USE_CMSIS -DDEBUG -I../source -I../ -I../SDK/CMSIS -I../SDK/startup -Og -fno-common -g3 -Wall -c -ffunction-sections -fdata-sections -ffreestanding -fno-builtin -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


