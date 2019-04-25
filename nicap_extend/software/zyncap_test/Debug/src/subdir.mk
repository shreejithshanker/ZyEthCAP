################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
LD_SRCS += \
../src/lscript.ld 

C_SRCS += \
../src/ZyNCAP_pr.c \
../src/devcfg.c \
../src/ff.c \
../src/mmc.c \
../src/xaxidma.c \
../src/xaxidma_bd.c \
../src/xaxidma_bdring.c \
../src/xaxidma_g.c \
../src/xaxidma_sinit.c \
../src/xemacps_example_intr_dma.c \
../src/xemacps_example_util.c \
../src/zycap.c 

OBJS += \
./src/ZyNCAP_pr.o \
./src/devcfg.o \
./src/ff.o \
./src/mmc.o \
./src/xaxidma.o \
./src/xaxidma_bd.o \
./src/xaxidma_bdring.o \
./src/xaxidma_g.o \
./src/xaxidma_sinit.o \
./src/xemacps_example_intr_dma.o \
./src/xemacps_example_util.o \
./src/zycap.o 

C_DEPS += \
./src/ZyNCAP_pr.d \
./src/devcfg.d \
./src/ff.d \
./src/mmc.d \
./src/xaxidma.d \
./src/xaxidma_bd.d \
./src/xaxidma_bdring.d \
./src/xaxidma_g.d \
./src/xaxidma_sinit.d \
./src/xemacps_example_intr_dma.d \
./src/xemacps_example_util.d \
./src/zycap.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: ARM v7 gcc compiler'
	arm-none-eabi-gcc -Wall -O0 -g3 -c -fmessage-length=0 -MT"$@" -mcpu=cortex-a9 -mfpu=vfpv3 -mfloat-abi=hard -I../../standalone_bsp_0/ps7_cortexa9_0/include -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


