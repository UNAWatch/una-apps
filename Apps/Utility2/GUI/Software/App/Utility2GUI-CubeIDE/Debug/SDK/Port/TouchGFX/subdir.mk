################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
C:/DT/Solutions/Una/una-watch/SDK/Port/TouchGFX/STM32TouchController.cpp \
C:/DT/Solutions/Una/una-watch/SDK/Port/TouchGFX/TouchGFXGPIO.cpp \
C:/DT/Solutions/Una/una-watch/SDK/Port/TouchGFX/TouchGFXHAL.cpp 

OBJS += \
./SDK/Port/TouchGFX/STM32TouchController.o \
./SDK/Port/TouchGFX/TouchGFXGPIO.o \
./SDK/Port/TouchGFX/TouchGFXHAL.o 

CPP_DEPS += \
./SDK/Port/TouchGFX/STM32TouchController.d \
./SDK/Port/TouchGFX/TouchGFXGPIO.d \
./SDK/Port/TouchGFX/TouchGFXHAL.d 


# Each subdirectory must supply rules for building sources it contributes
SDK/Port/TouchGFX/STM32TouchController.o: C:/DT/Solutions/Una/una-watch/SDK/Port/TouchGFX/STM32TouchController.cpp SDK/Port/TouchGFX/subdir.mk
	arm-none-eabi-g++ "$<" -mcpu=cortex-m33 -std=gnu++17 -g3 '-DBUILD_VERSION="0.0"' -DLOG_LEVEL=LOG_LEVEL_DEBUG -DLOG_FORMAT=5 -DUSER_APP -c -I../Core/Inc -I../../../../../Common/Libs/Header -I"../../../../../../../SDK/Libs/Header" -I../../../../../../../SDK/Port/TouchGFX -I../../../../../../../SDK/Port/TouchGFX/generated -I../../TouchGFX-GUI/touchgfx/framework/include -I../../TouchGFX-GUI/generated/fonts/include -I../../TouchGFX-GUI/generated/gui_generated/include -I../../TouchGFX-GUI/generated/images/include -I../../TouchGFX-GUI/generated/texts/include -I../../TouchGFX-GUI/generated/videos/include -I../../TouchGFX-GUI/gui/include -O0 -ffunction-sections -fdata-sections -fno-exceptions -fno-rtti -fno-use-cxa-atexit -Wall -fPIC -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@"  -mfpu=fpv5-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
SDK/Port/TouchGFX/TouchGFXGPIO.o: C:/DT/Solutions/Una/una-watch/SDK/Port/TouchGFX/TouchGFXGPIO.cpp SDK/Port/TouchGFX/subdir.mk
	arm-none-eabi-g++ "$<" -mcpu=cortex-m33 -std=gnu++17 -g3 '-DBUILD_VERSION="0.0"' -DLOG_LEVEL=LOG_LEVEL_DEBUG -DLOG_FORMAT=5 -DUSER_APP -c -I../Core/Inc -I../../../../../Common/Libs/Header -I"../../../../../../../SDK/Libs/Header" -I../../../../../../../SDK/Port/TouchGFX -I../../../../../../../SDK/Port/TouchGFX/generated -I../../TouchGFX-GUI/touchgfx/framework/include -I../../TouchGFX-GUI/generated/fonts/include -I../../TouchGFX-GUI/generated/gui_generated/include -I../../TouchGFX-GUI/generated/images/include -I../../TouchGFX-GUI/generated/texts/include -I../../TouchGFX-GUI/generated/videos/include -I../../TouchGFX-GUI/gui/include -O0 -ffunction-sections -fdata-sections -fno-exceptions -fno-rtti -fno-use-cxa-atexit -Wall -fPIC -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@"  -mfpu=fpv5-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
SDK/Port/TouchGFX/TouchGFXHAL.o: C:/DT/Solutions/Una/una-watch/SDK/Port/TouchGFX/TouchGFXHAL.cpp SDK/Port/TouchGFX/subdir.mk
	arm-none-eabi-g++ "$<" -mcpu=cortex-m33 -std=gnu++17 -g3 '-DBUILD_VERSION="0.0"' -DLOG_LEVEL=LOG_LEVEL_DEBUG -DLOG_FORMAT=5 -DUSER_APP -c -I../Core/Inc -I../../../../../Common/Libs/Header -I"../../../../../../../SDK/Libs/Header" -I../../../../../../../SDK/Port/TouchGFX -I../../../../../../../SDK/Port/TouchGFX/generated -I../../TouchGFX-GUI/touchgfx/framework/include -I../../TouchGFX-GUI/generated/fonts/include -I../../TouchGFX-GUI/generated/gui_generated/include -I../../TouchGFX-GUI/generated/images/include -I../../TouchGFX-GUI/generated/texts/include -I../../TouchGFX-GUI/generated/videos/include -I../../TouchGFX-GUI/gui/include -O0 -ffunction-sections -fdata-sections -fno-exceptions -fno-rtti -fno-use-cxa-atexit -Wall -fPIC -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@"  -mfpu=fpv5-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-SDK-2f-Port-2f-TouchGFX

clean-SDK-2f-Port-2f-TouchGFX:
	-$(RM) ./SDK/Port/TouchGFX/STM32TouchController.cyclo ./SDK/Port/TouchGFX/STM32TouchController.d ./SDK/Port/TouchGFX/STM32TouchController.o ./SDK/Port/TouchGFX/STM32TouchController.su ./SDK/Port/TouchGFX/TouchGFXGPIO.cyclo ./SDK/Port/TouchGFX/TouchGFXGPIO.d ./SDK/Port/TouchGFX/TouchGFXGPIO.o ./SDK/Port/TouchGFX/TouchGFXGPIO.su ./SDK/Port/TouchGFX/TouchGFXHAL.cyclo ./SDK/Port/TouchGFX/TouchGFXHAL.d ./SDK/Port/TouchGFX/TouchGFXHAL.o ./SDK/Port/TouchGFX/TouchGFXHAL.su

.PHONY: clean-SDK-2f-Port-2f-TouchGFX

