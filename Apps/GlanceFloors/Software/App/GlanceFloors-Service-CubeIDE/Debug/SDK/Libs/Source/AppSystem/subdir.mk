################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
G:/temp/0/Una/una-watch-develop/SDK/Libs/Source/AppSystem/AtExitImpl.cpp \
G:/temp/0/Una/una-watch-develop/SDK/Libs/Source/AppSystem/EntryPoint/Service/main.cpp \
G:/temp/0/Una/una-watch-develop/SDK/Libs/Source/AppSystem/system.cpp 

S_SRCS += \
G:/temp/0/Una/una-watch-develop/SDK/Libs/Source/AppSystem/startup_user_app.s 

S_DEPS += \
./SDK/Libs/Source/AppSystem/startup_user_app.d 

OBJS += \
./SDK/Libs/Source/AppSystem/AtExitImpl.o \
./SDK/Libs/Source/AppSystem/main.o \
./SDK/Libs/Source/AppSystem/startup_user_app.o \
./SDK/Libs/Source/AppSystem/system.o 

CPP_DEPS += \
./SDK/Libs/Source/AppSystem/AtExitImpl.d \
./SDK/Libs/Source/AppSystem/main.d \
./SDK/Libs/Source/AppSystem/system.d 


# Each subdirectory must supply rules for building sources it contributes
SDK/Libs/Source/AppSystem/AtExitImpl.o: G:/temp/0/Una/una-watch-develop/SDK/Libs/Source/AppSystem/AtExitImpl.cpp SDK/Libs/Source/AppSystem/subdir.mk
	arm-none-eabi-g++ "$<" -mcpu=cortex-m33 -std=gnu++17 -g3 '-DBUILD_VERSION="0.0.2"' -c -I../../../Libs/Header -I"../../../../../../SDK/Libs/Header" -Os -ffunction-sections -fdata-sections -fno-exceptions -fno-rtti -fno-use-cxa-atexit -Wall -fPIC -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@"  -mfpu=fpv5-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
SDK/Libs/Source/AppSystem/main.o: G:/temp/0/Una/una-watch-develop/SDK/Libs/Source/AppSystem/EntryPoint/Service/main.cpp SDK/Libs/Source/AppSystem/subdir.mk
	arm-none-eabi-g++ "$<" -mcpu=cortex-m33 -std=gnu++17 -g3 '-DBUILD_VERSION="0.0.2"' -c -I../../../Libs/Header -I"../../../../../../SDK/Libs/Header" -Os -ffunction-sections -fdata-sections -fno-exceptions -fno-rtti -fno-use-cxa-atexit -Wall -fPIC -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@"  -mfpu=fpv5-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
SDK/Libs/Source/AppSystem/startup_user_app.o: G:/temp/0/Una/una-watch-develop/SDK/Libs/Source/AppSystem/startup_user_app.s SDK/Libs/Source/AppSystem/subdir.mk
	arm-none-eabi-gcc -mcpu=cortex-m33 -g3 -c -fPIC -x assembler-with-cpp -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@"  -mfpu=fpv5-sp-d16 -mfloat-abi=hard -mthumb -o "$@" "$<"
SDK/Libs/Source/AppSystem/system.o: G:/temp/0/Una/una-watch-develop/SDK/Libs/Source/AppSystem/system.cpp SDK/Libs/Source/AppSystem/subdir.mk
	arm-none-eabi-g++ "$<" -mcpu=cortex-m33 -std=gnu++17 -g3 '-DBUILD_VERSION="0.0.2"' -c -I../../../Libs/Header -I"../../../../../../SDK/Libs/Header" -Os -ffunction-sections -fdata-sections -fno-exceptions -fno-rtti -fno-use-cxa-atexit -Wall -fPIC -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@"  -mfpu=fpv5-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-SDK-2f-Libs-2f-Source-2f-AppSystem

clean-SDK-2f-Libs-2f-Source-2f-AppSystem:
	-$(RM) ./SDK/Libs/Source/AppSystem/AtExitImpl.cyclo ./SDK/Libs/Source/AppSystem/AtExitImpl.d ./SDK/Libs/Source/AppSystem/AtExitImpl.o ./SDK/Libs/Source/AppSystem/AtExitImpl.su ./SDK/Libs/Source/AppSystem/main.cyclo ./SDK/Libs/Source/AppSystem/main.d ./SDK/Libs/Source/AppSystem/main.o ./SDK/Libs/Source/AppSystem/main.su ./SDK/Libs/Source/AppSystem/startup_user_app.d ./SDK/Libs/Source/AppSystem/startup_user_app.o ./SDK/Libs/Source/AppSystem/system.cyclo ./SDK/Libs/Source/AppSystem/system.d ./SDK/Libs/Source/AppSystem/system.o ./SDK/Libs/Source/AppSystem/system.su

.PHONY: clean-SDK-2f-Libs-2f-Source-2f-AppSystem

