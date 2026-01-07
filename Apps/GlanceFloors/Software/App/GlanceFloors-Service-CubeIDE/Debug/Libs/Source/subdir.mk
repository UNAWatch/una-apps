################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
G:/temp/0/Una/una-watch-develop/Apps/GlanceFloors/Software/Libs/Source/Service.cpp 

OBJS += \
./Libs/Source/Service.o 

CPP_DEPS += \
./Libs/Source/Service.d 


# Each subdirectory must supply rules for building sources it contributes
Libs/Source/Service.o: G:/temp/0/Una/una-watch-develop/Apps/GlanceFloors/Software/Libs/Source/Service.cpp Libs/Source/subdir.mk
	arm-none-eabi-g++ "$<" -mcpu=cortex-m33 -std=gnu++17 -g3 '-DBUILD_VERSION="0.0.2"' -c -I../../../Libs/Header -I"../../../../../../SDK/Libs/Header" -Os -ffunction-sections -fdata-sections -fno-exceptions -fno-rtti -fno-use-cxa-atexit -Wall -fPIC -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@"  -mfpu=fpv5-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Libs-2f-Source

clean-Libs-2f-Source:
	-$(RM) ./Libs/Source/Service.cyclo ./Libs/Source/Service.d ./Libs/Source/Service.o ./Libs/Source/Service.su

.PHONY: clean-Libs-2f-Source

