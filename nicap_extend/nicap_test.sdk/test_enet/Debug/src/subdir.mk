################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
LD_SRCS += \
../src/lscript.ld 

C_SRCS += \
../src/testperiph.c \
../src/xemacps_example_intr_dma.c \
../src/xemacps_example_util.c \
../src/xscugic_tapp_example.c \
../src/xscutimer_intr_example.c 

OBJS += \
./src/testperiph.o \
./src/xemacps_example_intr_dma.o \
./src/xemacps_example_util.o \
./src/xscugic_tapp_example.o \
./src/xscutimer_intr_example.o 

C_DEPS += \
./src/testperiph.d \
./src/xemacps_example_intr_dma.d \
./src/xemacps_example_util.d \
./src/xscugic_tapp_example.d \
./src/xscutimer_intr_example.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: ARM v7 gcc compiler'
	arm-none-eabi-gcc -Wall -O0 -g3 -c -fmessage-length=0 -MT"$@" -mcpu=cortex-a9 -mfpu=vfpv3 -mfloat-abi=hard -I../../standalone_bsp_0/ps7_cortexa9_0/include -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


