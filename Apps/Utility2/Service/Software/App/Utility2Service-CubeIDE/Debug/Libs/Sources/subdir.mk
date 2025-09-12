################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
C:/DT/Solutions/Una/una-watch/Apps/Utility2/Service/Software/Libs/Sources/GSModel.cpp \
C:/DT/Solutions/Una/una-watch/Apps/Utility2/Service/Software/Libs/Sources/Service.cpp 

OBJS += \
./Libs/Sources/GSModel.o \
./Libs/Sources/Service.o 

CPP_DEPS += \
./Libs/Sources/GSModel.d \
./Libs/Sources/Service.d 


# Each subdirectory must supply rules for building sources it contributes
Libs/Sources/GSModel.o: C:/DT/Solutions/Una/una-watch/Apps/Utility2/Service/Software/Libs/Sources/GSModel.cpp Libs/Sources/subdir.mk
	arm-none-eabi-g++ "$<" -mcpu=cortex-m33 -std=gnu++17 -g3 '-DBUILD_VERSION="0.0"' -DLOG_FORMAT=5 -c -I../Core/Inc -I../../../../../Common/Libs/Header -I../../../Libs/Header -I"../../../../../../../SDK/Libs/Header" -O0 -ffunction-sections -fdata-sections -fno-exceptions -fno-rtti -fno-use-cxa-atexit -Wall -fPIC -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@"  -mfpu=fpv5-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
Libs/Sources/Service.o: C:/DT/Solutions/Una/una-watch/Apps/Utility2/Service/Software/Libs/Sources/Service.cpp Libs/Sources/subdir.mk
	arm-none-eabi-g++ "$<" -mcpu=cortex-m33 -std=gnu++17 -g3 '-DBUILD_VERSION="0.0"' -DLOG_FORMAT=5 -c -I../Core/Inc -I../../../../../Common/Libs/Header -I../../../Libs/Header -I"../../../../../../../SDK/Libs/Header" -O0 -ffunction-sections -fdata-sections -fno-exceptions -fno-rtti -fno-use-cxa-atexit -Wall -fPIC -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@"  -mfpu=fpv5-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Libs-2f-Sources

clean-Libs-2f-Sources:
	-$(RM) ./Libs/Sources/GSModel.cyclo ./Libs/Sources/GSModel.d ./Libs/Sources/GSModel.o ./Libs/Sources/GSModel.su ./Libs/Sources/Service.cyclo ./Libs/Sources/Service.d ./Libs/Sources/Service.o ./Libs/Sources/Service.su

.PHONY: clean-Libs-2f-Sources

