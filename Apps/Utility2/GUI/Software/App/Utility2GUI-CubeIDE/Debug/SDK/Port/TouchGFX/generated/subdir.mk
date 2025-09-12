################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
C:/DT/Solutions/Una/una-watch/SDK/Port/TouchGFX/generated/OSWrappers.cpp \
C:/DT/Solutions/Una/una-watch/SDK/Port/TouchGFX/generated/STM32DMA.cpp \
C:/DT/Solutions/Una/una-watch/SDK/Port/TouchGFX/generated/TouchGFXConfiguration.cpp \
C:/DT/Solutions/Una/una-watch/SDK/Port/TouchGFX/generated/TouchGFXGeneratedHAL.cpp 

OBJS += \
./SDK/Port/TouchGFX/generated/OSWrappers.o \
./SDK/Port/TouchGFX/generated/STM32DMA.o \
./SDK/Port/TouchGFX/generated/TouchGFXConfiguration.o \
./SDK/Port/TouchGFX/generated/TouchGFXGeneratedHAL.o 

CPP_DEPS += \
./SDK/Port/TouchGFX/generated/OSWrappers.d \
./SDK/Port/TouchGFX/generated/STM32DMA.d \
./SDK/Port/TouchGFX/generated/TouchGFXConfiguration.d \
./SDK/Port/TouchGFX/generated/TouchGFXGeneratedHAL.d 


# Each subdirectory must supply rules for building sources it contributes
SDK/Port/TouchGFX/generated/OSWrappers.o: C:/DT/Solutions/Una/una-watch/SDK/Port/TouchGFX/generated/OSWrappers.cpp SDK/Port/TouchGFX/generated/subdir.mk
	arm-none-eabi-g++ "$<" -mcpu=cortex-m33 -std=gnu++17 -g3 '-DBUILD_VERSION="0.0"' -DLOG_LEVEL=LOG_LEVEL_DEBUG -DLOG_FORMAT=5 -DUSER_APP -c -I../Core/Inc -I../../../../../Common/Libs/Header -I"../../../../../../../SDK/Libs/Header" -I../../../../../../../SDK/Port/TouchGFX -I../../../../../../../SDK/Port/TouchGFX/generated -I../../TouchGFX-GUI/touchgfx/framework/include -I../../TouchGFX-GUI/generated/fonts/include -I../../TouchGFX-GUI/generated/gui_generated/include -I../../TouchGFX-GUI/generated/images/include -I../../TouchGFX-GUI/generated/texts/include -I../../TouchGFX-GUI/generated/videos/include -I../../TouchGFX-GUI/gui/include -O0 -ffunction-sections -fdata-sections -fno-exceptions -fno-rtti -fno-use-cxa-atexit -Wall -fPIC -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@"  -mfpu=fpv5-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
SDK/Port/TouchGFX/generated/STM32DMA.o: C:/DT/Solutions/Una/una-watch/SDK/Port/TouchGFX/generated/STM32DMA.cpp SDK/Port/TouchGFX/generated/subdir.mk
	arm-none-eabi-g++ "$<" -mcpu=cortex-m33 -std=gnu++17 -g3 '-DBUILD_VERSION="0.0"' -DLOG_LEVEL=LOG_LEVEL_DEBUG -DLOG_FORMAT=5 -DUSER_APP -c -I../Core/Inc -I../../../../../Common/Libs/Header -I"../../../../../../../SDK/Libs/Header" -I../../../../../../../SDK/Port/TouchGFX -I../../../../../../../SDK/Port/TouchGFX/generated -I../../TouchGFX-GUI/touchgfx/framework/include -I../../TouchGFX-GUI/generated/fonts/include -I../../TouchGFX-GUI/generated/gui_generated/include -I../../TouchGFX-GUI/generated/images/include -I../../TouchGFX-GUI/generated/texts/include -I../../TouchGFX-GUI/generated/videos/include -I../../TouchGFX-GUI/gui/include -O0 -ffunction-sections -fdata-sections -fno-exceptions -fno-rtti -fno-use-cxa-atexit -Wall -fPIC -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@"  -mfpu=fpv5-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
SDK/Port/TouchGFX/generated/TouchGFXConfiguration.o: C:/DT/Solutions/Una/una-watch/SDK/Port/TouchGFX/generated/TouchGFXConfiguration.cpp SDK/Port/TouchGFX/generated/subdir.mk
	arm-none-eabi-g++ "$<" -mcpu=cortex-m33 -std=gnu++17 -g3 '-DBUILD_VERSION="0.0"' -DLOG_LEVEL=LOG_LEVEL_DEBUG -DLOG_FORMAT=5 -DUSER_APP -c -I../Core/Inc -I../../../../../Common/Libs/Header -I"../../../../../../../SDK/Libs/Header" -I../../../../../../../SDK/Port/TouchGFX -I../../../../../../../SDK/Port/TouchGFX/generated -I../../TouchGFX-GUI/touchgfx/framework/include -I../../TouchGFX-GUI/generated/fonts/include -I../../TouchGFX-GUI/generated/gui_generated/include -I../../TouchGFX-GUI/generated/images/include -I../../TouchGFX-GUI/generated/texts/include -I../../TouchGFX-GUI/generated/videos/include -I../../TouchGFX-GUI/gui/include -O0 -ffunction-sections -fdata-sections -fno-exceptions -fno-rtti -fno-use-cxa-atexit -Wall -fPIC -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@"  -mfpu=fpv5-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
SDK/Port/TouchGFX/generated/TouchGFXGeneratedHAL.o: C:/DT/Solutions/Una/una-watch/SDK/Port/TouchGFX/generated/TouchGFXGeneratedHAL.cpp SDK/Port/TouchGFX/generated/subdir.mk
	arm-none-eabi-g++ "$<" -mcpu=cortex-m33 -std=gnu++17 -g3 '-DBUILD_VERSION="0.0"' -DLOG_LEVEL=LOG_LEVEL_DEBUG -DLOG_FORMAT=5 -DUSER_APP -c -I../Core/Inc -I../../../../../Common/Libs/Header -I"../../../../../../../SDK/Libs/Header" -I../../../../../../../SDK/Port/TouchGFX -I../../../../../../../SDK/Port/TouchGFX/generated -I../../TouchGFX-GUI/touchgfx/framework/include -I../../TouchGFX-GUI/generated/fonts/include -I../../TouchGFX-GUI/generated/gui_generated/include -I../../TouchGFX-GUI/generated/images/include -I../../TouchGFX-GUI/generated/texts/include -I../../TouchGFX-GUI/generated/videos/include -I../../TouchGFX-GUI/gui/include -O0 -ffunction-sections -fdata-sections -fno-exceptions -fno-rtti -fno-use-cxa-atexit -Wall -fPIC -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@"  -mfpu=fpv5-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-SDK-2f-Port-2f-TouchGFX-2f-generated

clean-SDK-2f-Port-2f-TouchGFX-2f-generated:
	-$(RM) ./SDK/Port/TouchGFX/generated/OSWrappers.cyclo ./SDK/Port/TouchGFX/generated/OSWrappers.d ./SDK/Port/TouchGFX/generated/OSWrappers.o ./SDK/Port/TouchGFX/generated/OSWrappers.su ./SDK/Port/TouchGFX/generated/STM32DMA.cyclo ./SDK/Port/TouchGFX/generated/STM32DMA.d ./SDK/Port/TouchGFX/generated/STM32DMA.o ./SDK/Port/TouchGFX/generated/STM32DMA.su ./SDK/Port/TouchGFX/generated/TouchGFXConfiguration.cyclo ./SDK/Port/TouchGFX/generated/TouchGFXConfiguration.d ./SDK/Port/TouchGFX/generated/TouchGFXConfiguration.o ./SDK/Port/TouchGFX/generated/TouchGFXConfiguration.su ./SDK/Port/TouchGFX/generated/TouchGFXGeneratedHAL.cyclo ./SDK/Port/TouchGFX/generated/TouchGFXGeneratedHAL.d ./SDK/Port/TouchGFX/generated/TouchGFXGeneratedHAL.o ./SDK/Port/TouchGFX/generated/TouchGFXGeneratedHAL.su

.PHONY: clean-SDK-2f-Port-2f-TouchGFX-2f-generated

