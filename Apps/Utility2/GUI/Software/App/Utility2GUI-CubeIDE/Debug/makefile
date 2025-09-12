################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include SDK/Port/TouchGFX/generated/subdir.mk
-include SDK/Port/TouchGFX/subdir.mk
-include SDK/Logger/subdir.mk
-include GUI/gui/src/model/subdir.mk
-include GUI/gui/src/counter_screen/subdir.mk
-include GUI/gui/src/common/subdir.mk
-include GUI/generated/texts/src/subdir.mk
-include GUI/generated/images/src/subdir.mk
-include GUI/generated/gui_generated/src/counter_screen/subdir.mk
-include GUI/generated/gui_generated/src/common/subdir.mk
-include GUI/generated/fonts/src/subdir.mk
-include GUI/build/Debug/obj/subdir.mk
-include Core/Startup/subdir.mk
-include Core/Src/subdir.mk
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


BUILD_ARTIFACT_NAME := Utility2GUI-CubeIDE
BUILD_ARTIFACT_EXTENSION := elf
BUILD_ARTIFACT_PREFIX :=
BUILD_ARTIFACT := $(BUILD_ARTIFACT_PREFIX)$(BUILD_ARTIFACT_NAME)$(if $(BUILD_ARTIFACT_EXTENSION),.$(BUILD_ARTIFACT_EXTENSION),)

# Add inputs and outputs from these tool invocations to the build variables 
EXECUTABLES += \
Utility2GUI-CubeIDE.elf \

MAP_FILES += \
Utility2GUI-CubeIDE.map \

SIZE_OUTPUT += \
default.size.stdout \

OBJDUMP_LIST += \
Utility2GUI-CubeIDE.list \


# All Target
all:
	+@$(MAKE) --no-print-directory main-build && $(MAKE) --no-print-directory post-build

# Main-build Target
main-build: Utility2GUI-CubeIDE.elf secondary-outputs

# Tool invocations
Utility2GUI-CubeIDE.elf Utility2GUI-CubeIDE.map: $(OBJS) $(USER_OBJS) C:\DT\Solutions\Una\una-watch\Apps\Utility2\GUI\Software\App\Utility2GUI-CubeIDE\STM32U575QIIXQ_RAM.ld makefile objects.list $(OPTIONAL_TOOL_DEPS)
	arm-none-eabi-g++ -o "Utility2GUI-CubeIDE.elf" @"objects.list" $(USER_OBJS) $(LIBS) -mcpu=cortex-m33 -T"C:\DT\Solutions\Una\una-watch\Apps\Utility2\GUI\Software\App\Utility2GUI-CubeIDE\STM32U575QIIXQ_RAM.ld" -Wl,-Map="Utility2GUI-CubeIDE.map" -Wl,--gc-sections -nostartfiles -nodefaultlibs -nostdlib -static -L../../../../../../../SDK/libc++ -L../../TouchGFX-GUI/touchgfx/lib/core/cortex_m33/gcc -Wl,--emit-relocs -Wl,-L../../../../../../../SDK/Scripts/Linker  -mfpu=fpv5-sp-d16 -mfloat-abi=hard -mthumb
	@echo 'Finished building target: $@'
	@echo ' '

default.size.stdout: $(EXECUTABLES) makefile objects.list $(OPTIONAL_TOOL_DEPS)
	arm-none-eabi-size  $(EXECUTABLES)
	@echo 'Finished building: $@'
	@echo ' '

Utility2GUI-CubeIDE.list: $(EXECUTABLES) makefile objects.list $(OPTIONAL_TOOL_DEPS)
	arm-none-eabi-objdump -h -S $(EXECUTABLES) > "Utility2GUI-CubeIDE.list"
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) Utility2GUI-CubeIDE.elf Utility2GUI-CubeIDE.list Utility2GUI-CubeIDE.map default.size.stdout
	-@echo ' '

post-build:
	python ../../../../../../../SDK/Scripts/app_packer/app_packer.py -v 0.0 -e Utility2GUI-CubeIDE.elf -o ../../../../Output
	-@echo ' '

secondary-outputs: $(SIZE_OUTPUT) $(OBJDUMP_LIST)

fail-specified-linker-script-missing:
	@echo 'Error: Cannot find the specified linker script. Check the linker settings in the build configuration.'
	@exit 2

warn-no-linker-script-specified:
	@echo 'Warning: No linker script specified. Check the linker settings in the build configuration.'

.PHONY: all clean dependents main-build fail-specified-linker-script-missing warn-no-linker-script-specified post-build

-include ../makefile.targets
