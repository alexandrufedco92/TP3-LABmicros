################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../source/ADC.c \
../source/DAC.c \
../source/FSK_Demodulator.c \
../source/gpio.c \
../source/hardware.c \
../source/main.c 

OBJS += \
./source/ADC.o \
./source/DAC.o \
./source/FSK_Demodulator.o \
./source/gpio.o \
./source/hardware.o \
./source/main.o 

C_DEPS += \
./source/ADC.d \
./source/DAC.d \
./source/FSK_Demodulator.d \
./source/gpio.d \
./source/hardware.d \
./source/main.d 


# Each subdirectory must supply rules for building sources it contributes
source/%.o: ../source/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	arm-none-eabi-gcc -DCR_INTEGER_PRINTF -DSDK_DEBUGCONSOLE=0 -D__MCUXPRESSO -D__USE_CMSIS -DDEBUG -DCPU_MK64FN1M0VDC12 -DCPU_MK64FN1M0VDC12_cm4 -D__REDLIB__ -I"C:\Users\HP\source\repos\TP3-LABmicros\projectsTP3\FSKprueba\source" -I"C:\Users\HP\source\repos\TP3-LABmicros\projectsTP3\FSKprueba" -I"C:\Users\HP\source\repos\TP3-LABmicros\projectsTP3\FSKprueba\startup" -I"C:\Users\HP\source\repos\TP3-LABmicros\projectsTP3\FSKprueba\CMSIS" -O0 -fno-common -g3 -Wall -c -fmessage-length=0 -fno-builtin -ffunction-sections -fdata-sections -Wa,-adhlns="$@.lst" -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -D__REDLIB__ -specs=redlib.specs -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


