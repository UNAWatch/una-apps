################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
G:/temp/0/Una/una-watch-develop/SDK/Libs/Source/Kernel/KernelBuilder.cpp 

OBJS += \
./SDK/Libs/Source/Kernel/KernelBuilder.o 

CPP_DEPS += \
./SDK/Libs/Source/Kernel/KernelBuilder.d 


# Each subdirectory must supply rules for building sources it contributes
SDK/Libs/Source/Kernel/KernelBuilder.o: G:/temp/0/Una/una-watch-develop/SDK/Libs/Source/Kernel/KernelBuilder.cpp SDK/Libs/Source/Kernel/subdir.mk
	arm-none-eabi-g++ "$<" -mcpu=cortex-m33 -std=gnu++17 -g3 '-DBUILD_VERSION="0.0.2"' -c -I../../../Libs/Header -I"../../../../../../SDK/Libs/Header" -Os -ffunction-sections -fdata-sections -fno-exceptions -fno-rtti -fno-use-cxa-atexit -Wall -fPIC -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@"  -mfpu=fpv5-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-SDK-2f-Libs-2f-Source-2f-Kernel

clean-SDK-2f-Libs-2f-Source-2f-Kernel:
	-$(RM) ./SDK/Libs/Source/Kernel/KernelBuilder.cyclo ./SDK/Libs/Source/Kernel/KernelBuilder.d ./SDK/Libs/Source/Kernel/KernelBuilder.o ./SDK/Libs/Source/Kernel/KernelBuilder.su

.PHONY: clean-SDK-2f-Libs-2f-Source-2f-Kernel

