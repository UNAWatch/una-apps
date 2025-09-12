################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
C:/DT/Solutions/Una/una-watch/Apps/Utility2/GUI/Software/App/TouchGFX-GUI/gui/src/counter_screen/CounterPresenter.cpp \
C:/DT/Solutions/Una/una-watch/Apps/Utility2/GUI/Software/App/TouchGFX-GUI/gui/src/counter_screen/CounterView.cpp 

OBJS += \
./GUI/gui/src/counter_screen/CounterPresenter.o \
./GUI/gui/src/counter_screen/CounterView.o 

CPP_DEPS += \
./GUI/gui/src/counter_screen/CounterPresenter.d \
./GUI/gui/src/counter_screen/CounterView.d 


# Each subdirectory must supply rules for building sources it contributes
GUI/gui/src/counter_screen/CounterPresenter.o: C:/DT/Solutions/Una/una-watch/Apps/Utility2/GUI/Software/App/TouchGFX-GUI/gui/src/counter_screen/CounterPresenter.cpp GUI/gui/src/counter_screen/subdir.mk
	arm-none-eabi-g++ "$<" -mcpu=cortex-m33 -std=gnu++17 -g3 '-DBUILD_VERSION="0.0"' -DLOG_LEVEL=LOG_LEVEL_DEBUG -DLOG_FORMAT=5 -DUSER_APP -c -I../Core/Inc -I../../../../../Common/Libs/Header -I"../../../../../../../SDK/Libs/Header" -I../../../../../../../SDK/Port/TouchGFX -I../../../../../../../SDK/Port/TouchGFX/generated -I../../TouchGFX-GUI/touchgfx/framework/include -I../../TouchGFX-GUI/generated/fonts/include -I../../TouchGFX-GUI/generated/gui_generated/include -I../../TouchGFX-GUI/generated/images/include -I../../TouchGFX-GUI/generated/texts/include -I../../TouchGFX-GUI/generated/videos/include -I../../TouchGFX-GUI/gui/include -O0 -ffunction-sections -fdata-sections -fno-exceptions -fno-rtti -fno-use-cxa-atexit -Wall -fPIC -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@"  -mfpu=fpv5-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
GUI/gui/src/counter_screen/CounterView.o: C:/DT/Solutions/Una/una-watch/Apps/Utility2/GUI/Software/App/TouchGFX-GUI/gui/src/counter_screen/CounterView.cpp GUI/gui/src/counter_screen/subdir.mk
	arm-none-eabi-g++ "$<" -mcpu=cortex-m33 -std=gnu++17 -g3 '-DBUILD_VERSION="0.0"' -DLOG_LEVEL=LOG_LEVEL_DEBUG -DLOG_FORMAT=5 -DUSER_APP -c -I../Core/Inc -I../../../../../Common/Libs/Header -I"../../../../../../../SDK/Libs/Header" -I../../../../../../../SDK/Port/TouchGFX -I../../../../../../../SDK/Port/TouchGFX/generated -I../../TouchGFX-GUI/touchgfx/framework/include -I../../TouchGFX-GUI/generated/fonts/include -I../../TouchGFX-GUI/generated/gui_generated/include -I../../TouchGFX-GUI/generated/images/include -I../../TouchGFX-GUI/generated/texts/include -I../../TouchGFX-GUI/generated/videos/include -I../../TouchGFX-GUI/gui/include -O0 -ffunction-sections -fdata-sections -fno-exceptions -fno-rtti -fno-use-cxa-atexit -Wall -fPIC -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@"  -mfpu=fpv5-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-GUI-2f-gui-2f-src-2f-counter_screen

clean-GUI-2f-gui-2f-src-2f-counter_screen:
	-$(RM) ./GUI/gui/src/counter_screen/CounterPresenter.cyclo ./GUI/gui/src/counter_screen/CounterPresenter.d ./GUI/gui/src/counter_screen/CounterPresenter.o ./GUI/gui/src/counter_screen/CounterPresenter.su ./GUI/gui/src/counter_screen/CounterView.cyclo ./GUI/gui/src/counter_screen/CounterView.d ./GUI/gui/src/counter_screen/CounterView.o ./GUI/gui/src/counter_screen/CounterView.su

.PHONY: clean-GUI-2f-gui-2f-src-2f-counter_screen

