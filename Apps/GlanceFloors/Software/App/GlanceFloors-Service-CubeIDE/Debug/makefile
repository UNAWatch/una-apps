################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include SDK/Libs/Source/SwTimer/subdir.mk
-include SDK/Libs/Source/SensorLayer/subdir.mk
-include SDK/Libs/Source/Logger/subdir.mk
-include SDK/Libs/Source/Kernel/subdir.mk
-include SDK/Libs/Source/AppSystem/subdir.mk
-include Libs/Source/subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(CC_DEPS)),)
-include $(CC_DEPS)
endif
ifneq ($(strip $(C++_DEPS)),)
-include $(C++_DEPS)
endif
ifneq ($(strip $(CCM_DEPS)),)
-include $(CCM_DEPS)
endif
ifneq ($(strip $(C_UPPER_DEPS)),)
-include $(C_UPPER_DEPS)
endif
ifneq ($(strip $(CXX_DEPS)),)
-include $(CXX_DEPS)
endif
ifneq ($(strip $(S_DEPS)),)
-include $(S_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
ifneq ($(strip $(CXXM_DEPS)),)
-include $(CXXM_DEPS)
endif
ifneq ($(strip $(C++M_DEPS)),)
-include $(C++M_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
ifneq ($(strip $(CPP_DEPS)),)
-include $(CPP_DEPS)
endif
endif

-include ../makefile.defs

OPTIONAL_TOOL_DEPS := \
$(wildcard ../makefile.defs) \
$(wildcard ../makefile.init) \
$(wildcard ../makefile.targets) \


BUILD_ARTIFACT_NAME := GlanceFloors
BUILD_ARTIFACT_EXTENSION := elf
BUILD_ARTIFACT_PREFIX :=
BUILD_ARTIFACT := $(BUILD_ARTIFACT_PREFIX)$(BUILD_ARTIFACT_NAME)$(if $(BUILD_ARTIFACT_EXTENSION),.$(BUILD_ARTIFACT_EXTENSION),)

# Add inputs and outputs from these tool invocations to the build variables 
EXECUTABLES += \
GlanceFloors.elf \

MAP_FILES += \
GlanceFloors.map \

SIZE_OUTPUT += \
default.size.stdout \

OBJDUMP_LIST += \
GlanceFloors.list \


# All Target
all:
	+@$(MAKE) --no-print-directory main-build && $(MAKE) --no-print-directory post-build

# Main-build Target
main-build: GlanceFloors.elf secondary-outputs

# Tool invocations
GlanceFloors.elf GlanceFloors.map: $(OBJS) $(USER_OBJS) G:\temp\0\Una\una-watch-develop\Apps\GlanceFloors\Software\App\GlanceFloors-Service-CubeIDE\Linker.ld makefile objects.list $(OPTIONAL_TOOL_DEPS)
	arm-none-eabi-g++ -o "GlanceFloors.elf" @"objects.list" $(USER_OBJS) $(LIBS) -mcpu=cortex-m33 -T"G:\temp\0\Una\una-watch-develop\Apps\GlanceFloors\Software\App\GlanceFloors-Service-CubeIDE\Linker.ld" -Wl,-Map="GlanceFloors.map" -Wl,--gc-sections -nostartfiles -nodefaultlibs -nostdlib -static -L../../../../../../SDK/libc++ -Wl,--emit-relocs -Wl,-L../../../../../../SDK/Utilities/Scripts/linker  -mfpu=fpv5-sp-d16 -mfloat-abi=hard -mthumb
	@echo 'Finished building target: $@'
	@echo ' '

default.size.stdout: $(EXECUTABLES) makefile objects.list $(OPTIONAL_TOOL_DEPS)
	arm-none-eabi-size  $(EXECUTABLES)
	@echo 'Finished building: $@'
	@echo ' '

GlanceFloors.list: $(EXECUTABLES) makefile objects.list $(OPTIONAL_TOOL_DEPS)
	arm-none-eabi-objdump -h -S $(EXECUTABLES) > "GlanceFloors.list"
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) GlanceFloors.elf GlanceFloors.list GlanceFloors.map default.size.stdout
	-@echo ' '

post-build:
	python ../../../../../../SDK/Utilities/Scripts/app_packer/app_packer.py -e GlanceFloors.elf -o ../../../../Output/Tmp -ext srv
	python ../../../../../../SDK/Utilities/Scripts/app_merging/app_merging.py -normal_icon ../../../../Resources/icon_60x60.png -small_icon ../../../../Resources/icon_30x30.png -name GlanceFloors -type Glance -out ../../../../Output -appid 0123ABCD89EF4560 -appver 0.0.2 -scripts ../../../../../../SDK/Utilities/Scripts
	-@echo ' '

secondary-outputs: $(SIZE_OUTPUT) $(OBJDUMP_LIST)

fail-specified-linker-script-missing:
	@echo 'Error: Cannot find the specified linker script. Check the linker settings in the build configuration.'
	@exit 2

warn-no-linker-script-specified:
	@echo 'Warning: No linker script specified. Check the linker settings in the build configuration.'

.PHONY: all clean dependents main-build fail-specified-linker-script-missing warn-no-linker-script-specified post-build

-include ../makefile.targets
