################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../source/ADC.c \
../source/FTM.c \
../source/GPIO.c \
../source/PORT.c \
../source/SysTick.c \
../source/UART.c \
../source/hardware.c \
../source/main.c 

OBJS += \
./source/ADC.o \
./source/FTM.o \
./source/GPIO.o \
./source/PORT.o \
./source/SysTick.o \
./source/UART.o \
./source/hardware.o \
./source/main.o 

C_DEPS += \
./source/ADC.d \
./source/FTM.d \
./source/GPIO.d \
./source/PORT.d \
./source/SysTick.d \
./source/UART.d \
./source/hardware.d \
./source/main.d 


# Each subdirectory must supply rules for building sources it contributes
source/%.o: ../source/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	arm-none-eabi-gcc -DCR_INTEGER_PRINTF -DSDK_DEBUGCONSOLE=0 -D__MCUXPRESSO -D__USE_CMSIS -DDEBUG -DCPU_MK64FN1M0VDC12 -DCPU_MK64FN1M0VDC12_cm4 -D__REDLIB__ -I"/home/jacoby/Documents/MCUXpresso_11.0.0_2516/workspace/ADC/source" -I"/home/jacoby/Documents/MCUXpresso_11.0.0_2516/workspace/ADC" -I"/home/jacoby/Documents/MCUXpresso_11.0.0_2516/workspace/ADC/startup" -I"/home/jacoby/Documents/MCUXpresso_11.0.0_2516/workspace/ADC/CMSIS" -O0 -fno-common -g3 -Wall -c -fmessage-length=0 -fno-builtin -ffunction-sections -fdata-sections -Wa,-adhlns="$@.lst" -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -D__REDLIB__ -fstack-usage -specs=redlib.specs -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


