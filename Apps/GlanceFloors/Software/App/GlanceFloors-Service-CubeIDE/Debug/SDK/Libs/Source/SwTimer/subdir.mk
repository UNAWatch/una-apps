################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
G:/temp/0/Una/una-watch-develop/SDK/Libs/Source/SwTimer/SwTimer.cpp \
G:/temp/0/Una/una-watch-develop/SDK/Libs/Source/SwTimer/SwTimerPort.cpp 

OBJS += \
./SDK/Libs/Source/SwTimer/SwTimer.o \
./SDK/Libs/Source/SwTimer/SwTimerPort.o 

CPP_DEPS += \
./SDK/Libs/Source/SwTimer/SwTimer.d \
./SDK/Libs/Source/SwTimer/SwTimerPort.d 


# Each subdirectory must supply rules for building sources it contributes
SDK/Libs/Source/SwTimer/SwTimer.o: G:/temp/0/Una/una-watch-develop/SDK/Libs/Source/SwTimer/SwTimer.cpp SDK/Libs/Source/SwTimer/subdir.mk
	arm-none-eabi-g++ "$<" -mcpu=cortex-m33 -std=gnu++17 -g3 '-DBUILD_VERSION="0.0.2"' -c -I../../../Libs/Header -I"../../../../../../SDK/Libs/Header" -Os -ffunction-sections -fdata-sections -fno-exceptions -fno-rtti -fno-use-cxa-atexit -Wall -fPIC -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@"  -mfpu=fpv5-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
SDK/Libs/Source/SwTimer/SwTimerPort.o: G:/temp/0/Una/una-watch-develop/SDK/Libs/Source/SwTimer/SwTimerPort.cpp SDK/Libs/Source/SwTimer/subdir.mk
	arm-none-eabi-g++ "$<" -mcpu=cortex-m33 -std=gnu++17 -g3 '-DBUILD_VERSION="0.0.2"' -c -I../../../Libs/Header -I"../../../../../../SDK/Libs/Header" -Os -ffunction-sections -fdata-sections -fno-exceptions -fno-rtti -fno-use-cxa-atexit -Wall -fPIC -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@"  -mfpu=fpv5-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-SDK-2f-Libs-2f-Source-2f-SwTimer

clean-SDK-2f-Libs-2f-Source-2f-SwTimer:
	-$(RM) ./SDK/Libs/Source/SwTimer/SwTimer.cyclo ./SDK/Libs/Source/SwTimer/SwTimer.d ./SDK/Libs/Source/SwTimer/SwTimer.o ./SDK/Libs/Source/SwTimer/SwTimer.su ./SDK/Libs/Source/SwTimer/SwTimerPort.cyclo ./SDK/Libs/Source/SwTimer/SwTimerPort.d ./SDK/Libs/Source/SwTimer/SwTimerPort.o ./SDK/Libs/Source/SwTimer/SwTimerPort.su

.PHONY: clean-SDK-2f-Libs-2f-Source-2f-SwTimer

