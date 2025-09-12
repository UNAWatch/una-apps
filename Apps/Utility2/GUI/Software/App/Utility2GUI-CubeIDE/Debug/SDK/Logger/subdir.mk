################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
C:/DT/Solutions/Una/una-watch/SDK/Libs/Source/Logger/Logger.cpp 

OBJS += \
./SDK/Logger/Logger.o 

CPP_DEPS += \
./SDK/Logger/Logger.d 


# Each subdirectory must supply rules for building sources it contributes
SDK/Logger/Logger.o: C:/DT/Solutions/Una/una-watch/SDK/Libs/Source/Logger/Logger.cpp SDK/Logger/subdir.mk
	arm-none-eabi-g++ "$<" -mcpu=cortex-m33 -std=gnu++17 -g3 '-DBUILD_VERSION="0.0"' -DLOG_LEVEL=LOG_LEVEL_DEBUG -DLOG_FORMAT=5 -DUSER_APP -c -I../Core/Inc -I../../../../../Common/Libs/Header -I"../../../../../../../SDK/Libs/Header" -I../../../../../../../SDK/Port/TouchGFX -I../../../../../../../SDK/Port/TouchGFX/generated -I../../TouchGFX-GUI/touchgfx/framework/include -I../../TouchGFX-GUI/generated/fonts/include -I../../TouchGFX-GUI/generated/gui_generated/include -I../../TouchGFX-GUI/generated/images/include -I../../TouchGFX-GUI/generated/texts/include -I../../TouchGFX-GUI/generated/videos/include -I../../TouchGFX-GUI/gui/include -O0 -ffunction-sections -fdata-sections -fno-exceptions -fno-rtti -fno-use-cxa-atexit -Wall -fPIC -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@"  -mfpu=fpv5-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-SDK-2f-Logger

clean-SDK-2f-Logger:
	-$(RM) ./SDK/Logger/Logger.cyclo ./SDK/Logger/Logger.d ./SDK/Logger/Logger.o ./SDK/Logger/Logger.su

.PHONY: clean-SDK-2f-Logger

