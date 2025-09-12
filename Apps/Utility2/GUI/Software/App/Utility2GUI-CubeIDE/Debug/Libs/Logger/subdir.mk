################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
C:/DT/Solutions/Una/una-watch/Software/Libs/Source/Logger/Logger.c 

C_DEPS += \
./Libs/Logger/Logger.d 

OBJS += \
./Libs/Logger/Logger.o 


# Each subdirectory must supply rules for building sources it contributes
Libs/Logger/Logger.o: C:/DT/Solutions/Una/una-watch/Software/Libs/Source/Logger/Logger.c Libs/Logger/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m33 -std=gnu11 -g3 '-DBUILD_VERSION="0.0"' -DLOG_LEVEL=LOG_LEVEL_DEBUG -DLOG_FORMAT=5 -c -I../Core/Inc -I../../../../../../../Software/Libs/Header -I../../../../../../../Software/Libs/Header/Logger -I../../../../../../../SDK/Libs/Header/Interfaces -O0 -ffunction-sections -fdata-sections -Wall -fPIC -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@"  -mfpu=fpv5-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Libs-2f-Logger

clean-Libs-2f-Logger:
	-$(RM) ./Libs/Logger/Logger.cyclo ./Libs/Logger/Logger.d ./Libs/Logger/Logger.o ./Libs/Logger/Logger.su

.PHONY: clean-Libs-2f-Logger

