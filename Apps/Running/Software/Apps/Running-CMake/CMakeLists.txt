cmake_minimum_required(VERSION 3.20)

set(CMAKE_VERBOSE_MAKEFILE ON)

set(CMAKE_TOOLCHAIN_FILE "${CMAKE_CURRENT_SOURCE_DIR}/toolchain-arm-none-eabi.cmake")

project(Running LANGUAGES C CXX ASM)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

# Paths
# /home/sd/dt-prj/una/una-apps/Apps/Running/Software/Apps/Running-CMake
set(SDK_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../../../../../SDK")
set(LIBS_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../../Libs")
set(TOUCHGFX_GUI_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../TouchGFX-GUI")
set(OUTPUT_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../../../Output")
set(RESOURCES_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../../../Resources")
set(SCRIPTS_PATH "${SDK_PATH}/Utilities/Scripts")

# App Info
set(APP_ID "A12E9F4C8B7D3A65")
set(APP_NAME "Running")
set(BUILD_VERSION "0.1.5")
set(DEV_ID "UNA")



# Common compile options (match CubeIDE exactly)
add_compile_options(
    -mcpu=cortex-m33
    # -g3  # Remove debug info to match CubeIDE size
    -Os
    -ffunction-sections
    -fdata-sections
    -fno-exceptions
    -fno-rtti
    -fno-use-cxa-atexit
    -Wall
    -fPIC
    -fstack-usage
    -mfpu=fpv5-sp-d16
    -mfloat-abi=hard
    -mthumb
)

# arm-none-eabi-g++ "/workspace/Apps/Running/Software/Libs/Sources/SettingsSerializer.cpp"
#-mcpu=cortex-m33
#-g3 
#-Os
#-ffunction-sections
#-fdata-sections
#-fno-exceptions
#-fno-rtti
#-fno-use-cxa-atexit
#-Wall
#-fPIC
#-fstack-usage
#-mfpu=fpv5-sp-d16
#-mfloat-abi=hard
#-mthumb

#-fcyclomatic-complexity

#-MMD
#-MP
#-c

# arm-none-eabi-g++ "/workspace/Apps/Running/Software/Apps/TouchGFX-GUI/generated/fonts/src/VectorFontRendererBuffers.cpp"
#-mcpu=cortex-m33
#-std=gnu++17
#-g3
#-DLOG_LEVEL=LOG_LEVEL_DEBUG
#-c
#-Os
#-ffunction-sections
#-fdata-sections
#-fno-exceptions
#-fno-rtti
#-fno-use-cxa-atexit
#-Wall
#-fPIC
#-fstack-usage
#-fcyclomatic-complexity
#-MMD
#-MP
#-MF"GUI/TouchGFX-GUI/generated/fonts/src/VectorFontRendererBuffers.d"
#-MT"GUI/TouchGFX-GUI/generated/fonts/src/VectorFontRendererBuffers.o" 
#-mfpu=fpv5-sp-d16
#-mfloat-abi=hard
#-mthumb
#-o "GUI/TouchGFX-GUI/generated/fonts/src/VectorFontRendererBuffers.o"

# Set standards
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
#-std=gnu++17

# Common compile definitions
add_compile_definitions(
    BUILD_VERSION="${BUILD_VERSION}"
    APP_NAME="${APP_NAME}"
    DEV_ID="${DEV_ID}"
    APP_ID="${APP_ID}"
    LOG_LEVEL=LOG_LEVEL_DEBUG
)
# OK!
#'-DBUILD_VERSION="0.1.4-rc11-7-6ca6e9c-dirty"'
#'-DAPP_NAME="Running"'
#'-DDEV_ID="UNA"'
#'-DAPP_ID="A12E9F4C8B7D3A65"'
#-DLOG_LEVEL=LOG_LEVEL_DEBUG




# -----------------------------------------------------------------------------
# RunningService
# -----------------------------------------------------------------------------

set(SERVICE_SOURCES
    "${LIBS_PATH}/Sources/ActivitySummarySerializer.cpp"
    "${LIBS_PATH}/Sources/ActivityWriter.cpp"
    "${LIBS_PATH}/Sources/Service.cpp"
    "${LIBS_PATH}/Sources/SettingsSerializer.cpp"

    "${CMAKE_CURRENT_SOURCE_DIR}/syscalls.cpp"

    "${SDK_PATH}/Libs/Source/AppSystem/AtExitImpl.cpp"
    "${SDK_PATH}/Libs/Source/AppSystem/EntryPoint/Service/main.cpp"
    "${SDK_PATH}/Libs/Source/AppSystem/startup_user_app.s"
    "${SDK_PATH}/Libs/Source/AppSystem/system.cpp"
    "${SDK_PATH}/Libs/Source/FitHelper/FitHelper.cpp"
    "${SDK_PATH}/Libs/Source/JSON/JsonStreamReader.cpp"
    "${SDK_PATH}/Libs/Source/JSON/JsonStreamWriter.cpp"
    "${SDK_PATH}/Libs/Source/Kernel/KernelBuilder.cpp"
    "${SDK_PATH}/Libs/Source/SensorLayer/SensorConnection.cpp"
    "${SDK_PATH}/Libs/Source/TrackMap/TrackMapBuilder.cpp"
    "${SDK_PATH}/Libs/Source/UnaLogger/Logger.cpp"

    "${SDK_PATH}/ThirdParty/FitSDKRelease_21.171.00/c/fit.c"
    "${SDK_PATH}/ThirdParty/FitSDKRelease_21.171.00/c/fit_convert.c"
    "${SDK_PATH}/ThirdParty/FitSDKRelease_21.171.00/c/fit_crc.c"
    "${SDK_PATH}/ThirdParty/FitSDKRelease_21.171.00/c/fit_product.c"

    "${SDK_PATH}/ThirdParty/coreJSON/source/core_json.c"
)

add_executable(RunningService.elf ${SERVICE_SOURCES})

target_include_directories(RunningService.elf PRIVATE
    "${LIBS_PATH}/Header"
    "${SDK_PATH}/Libs/Header"
    "${SDK_PATH}/ThirdParty/FitSDKRelease_21.171.00/c"
    "${SDK_PATH}/ThirdParty/coreJSON/source/include"
    "${TOUCHGFX_GUI_PATH}/gui/include"
)

# OK!
#-I../../../Libs/Header
#-I"../../../../../../SDK/Libs/Header"
#-I"../../../../../../SDK/ThirdParty/FitSDKRelease_21.171.00/c"
#-I"../../../../../../SDK/ThirdParty/coreJSON/source/include"
#-I../../TouchGFX-GUI/gui/include


target_link_libraries(RunningService.elf PRIVATE
    ${SDK_PATH}/libc++/libsupc++.a
    ${SDK_PATH}/libc++/libstdc++.a
)

target_link_options(RunningService.elf PRIVATE
    -mcpu=cortex-m33
    -T "${CMAKE_CURRENT_SOURCE_DIR}/RunningService.ld"
    -T "${SDK_PATH}/Utilities/Scripts/linker/Main/Sections.ld"
    -T "${SDK_PATH}/Utilities/Scripts/linker/LibC/libc_exports_0.0.3.ld"
    -Wl,-Map=${CMAKE_BINARY_DIR}/RunningService.elf.map
    -Wl,--gc-sections
    -Wl,--strip-debug
    -nostartfiles
    -nodefaultlibs
    -nostdlib
    -static
    -L${SDK_PATH}/libc++
    -Wl,--emit-relocs
    -Wl,-L${SDK_PATH}/Utilities/Scripts/linker
    -mfpu=fpv5-sp-d16
    -mfloat-abi=hard
    -mthumb
)

# OK!
#arm-none-eabi-g++
#  -mcpu=cortex-m33
#  -nostartfiles
#  -nodefaultlibs
#  -nostdlib
#  -static
#  -Wl,--gc-sections
#  -Wl,--emit-relocs
#  -Wl,-L../../../../../../SDK/Utilities/Scripts/linker 
#  -L../../../../../../SDK/libc++
#  -mfpu=fpv5-sp-d16
#  -mfloat-abi=hard
#  -mthumb

#  -l:libstdc++.a

#  -o "RunningService-CubeIDE.elf" @"objects.list" 
#  -T"/workspace/Apps/Running/Software/Apps/RunningService-CubeIDE/Linker.ld"
#  -Wl,-Map="RunningService-CubeIDE.map"

# target_link_libraries(RunningService.elf
#     PRIVATE
#     -l:libstdc++.a
#     )

add_custom_command(TARGET RunningService.elf POST_BUILD
    COMMAND python3 ${SCRIPTS_PATH}/app_packer/app_packer.py -e $<TARGET_FILE:RunningService.elf> -o ${CMAKE_CURRENT_BINARY_DIR}/Tmp -ext srv
    COMMENT "Packing RunningService"
)

# -----------------------------------------------------------------------------
# RunningGUI
# -----------------------------------------------------------------------------

file(GLOB_RECURSE GUI_SRC_SOURCES
    "${TOUCHGFX_GUI_PATH}/gui/src/*.cpp"
    "${TOUCHGFX_GUI_PATH}/generated/fonts/src/*.cpp"
    "${TOUCHGFX_GUI_PATH}/generated/gui_generated/src/*.cpp"
    "${TOUCHGFX_GUI_PATH}/generated/images/src/*.cpp"
    "${TOUCHGFX_GUI_PATH}/generated/texts/src/*.cpp"
)
string(REPLACE ";" "\n" GUI_SRC_SOURCES_STR "${GUI_SRC_SOURCES}")
message("GUI SRC:\n${GUI_SRC_SOURCES_STR}")

set(GUI_SOURCES
    ${GUI_SRC_SOURCES}

    "${CMAKE_CURRENT_SOURCE_DIR}/PaintImpl.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/syscalls.cpp"

    "${SDK_PATH}/Libs/Source/AppSystem/AtExitImpl.cpp"
    "${SDK_PATH}/Libs/Source/AppSystem/EntryPoint/TouchGFX/main.cpp"
    "${SDK_PATH}/Libs/Source/AppSystem/startup_user_app.s"
    "${SDK_PATH}/Libs/Source/AppSystem/system.cpp"
    "${SDK_PATH}/Libs/Source/Kernel/KernelBuilder.cpp"
    "${SDK_PATH}/Libs/Source/UnaLogger/Logger.cpp"

    "${SDK_PATH}/Port/TouchGFX/STM32TouchController.cpp"
    "${SDK_PATH}/Port/TouchGFX/TouchGFXCommandProcessor.cpp"
    "${SDK_PATH}/Port/TouchGFX/TouchGFXGPIO.cpp"
    "${SDK_PATH}/Port/TouchGFX/TouchGFXHAL.cpp"

    "${SDK_PATH}/Port/TouchGFX/generated/OSWrappers.cpp"
    "${SDK_PATH}/Port/TouchGFX/generated/STM32DMA.cpp"
    "${SDK_PATH}/Port/TouchGFX/generated/TouchGFXConfiguration.cpp"
    "${SDK_PATH}/Port/TouchGFX/generated/TouchGFXGeneratedHAL.cpp"
)

add_executable(RunningGUI.elf ${GUI_SOURCES})

target_include_directories(RunningGUI.elf PRIVATE
    "${LIBS_PATH}/Header"
    "${SDK_PATH}/Libs/Header"
    "${SDK_PATH}/Port/TouchGFX"
    "${SDK_PATH}/Port/TouchGFX/generated"
    "${TOUCHGFX_GUI_PATH}/touchgfx/framework/include"
    "${TOUCHGFX_GUI_PATH}/generated/fonts/include"
    "${TOUCHGFX_GUI_PATH}/generated/gui_generated/include"
    "${TOUCHGFX_GUI_PATH}/generated/images/include"
    "${TOUCHGFX_GUI_PATH}/generated/texts/include"
    "${TOUCHGFX_GUI_PATH}/generated/videos/include"
    "${TOUCHGFX_GUI_PATH}/gui/include"
)
# OK!
#-I../../../Libs/Header
#-I"../../../../../../SDK/Libs/Header"
#-I"../../../../../../SDK/Port/TouchGFX"
#-I"../../../../../../SDK/Port/TouchGFX/generated"
#-I../../TouchGFX-GUI/touchgfx/framework/include
#-I../../TouchGFX-GUI/generated/fonts/include
#-I../../TouchGFX-GUI/generated/gui_generated/include
#-I../../TouchGFX-GUI/generated/images/include
#-I../../TouchGFX-GUI/generated/texts/include
#-I../../TouchGFX-GUI/generated/videos/include
#-I../../TouchGFX-GUI/gui/include

target_link_libraries(RunningGUI.elf PRIVATE
    ${SDK_PATH}/libc++/libsupc++.a
    ${SDK_PATH}/libc++/libstdc++.a
    ${TOUCHGFX_GUI_PATH}/touchgfx/lib/core/cortex_m33/gcc/libtouchgfx-float-abi-hard.a
)

target_link_options(RunningGUI.elf PRIVATE
    -L${TOUCHGFX_GUI_PATH}/touchgfx/lib/core/cortex_m33/gcc
    -mcpu=cortex-m33
    -T "${CMAKE_CURRENT_SOURCE_DIR}/RunningGUI.ld"
    -T "${SDK_PATH}/Utilities/Scripts/linker/Main/Sections.ld"
    -T "${SDK_PATH}/Utilities/Scripts/linker/LibC/libc_exports_0.0.3.ld"
    -Wl,-Map=${CMAKE_BINARY_DIR}/RunningGUI.elf.map
    -Wl,--gc-sections
    -Wl,--strip-debug
    -nostartfiles
    -nodefaultlibs
    -nostdlib
    -static
    -L${SDK_PATH}/libc++
    -Wl,--emit-relocs
    -Wl,-L${SDK_PATH}/Utilities/Scripts/linker
    -mfpu=fpv5-sp-d16
    -mfloat-abi=hard
    -mthumb
)
# OK!
# arm-none-eabi-g++
#-mcpu=cortex-m33
#-nostartfiles
#-Wl,--gc-sections
#-Wl,--emit-relocs
#-Wl,-L../../../../../../SDK/Utilities/Scripts/linker 
#-mfpu=fpv5-sp-d16
#-mfloat-abi=hard
#-mthumb
#-Wl,-Map="RunningGUI-CubeIDE.map"
#-L../../TouchGFX-GUI/touchgfx/lib/core/cortex_m33/gcc
#-l:libstdc++.a
#-l:libtouchgfx-float-abi-hard.a
#-T"/workspace/Apps/Running/Software/Apps/RunningGUI-CubeIDE/Linker.ld"
#-nodefaultlibs
#-nostdlib
#-static
#-L../../../../../../SDK/libc++

#-o "RunningGUI-CubeIDE.elf" @"objects.list" 

# target_link_libraries(RunningGUI.elf
#     PRIVATE
# )

add_custom_command(TARGET RunningGUI.elf POST_BUILD
    COMMAND python3 ${SCRIPTS_PATH}/app_packer/app_packer.py -e $<TARGET_FILE:RunningGUI.elf> -o ${CMAKE_CURRENT_BINARY_DIR}/Tmp -ext gui
    COMMENT "Packing RunningGUI"
)

# -----------------------------------------------------------------------------
# Final App Merging
# -----------------------------------------------------------------------------

add_custom_target(RunningApp ALL
    DEPENDS RunningService.elf RunningGUI.elf
    COMMAND python3 ${SCRIPTS_PATH}/app_merging/app_merging.py -normal_icon ${RESOURCES_PATH}/icon_60x60.png -small_icon ${RESOURCES_PATH}/icon_30x30.png -name ${APP_NAME} -type Activity -glance_capable -out ${CMAKE_CURRENT_BINARY_DIR} -appid ${APP_ID} -appver ${BUILD_VERSION} -scripts ${SCRIPTS_PATH}
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_BINARY_DIR}/Tmp ${CMAKE_CURRENT_SOURCE_DIR}/../../../Output/Tmp
    COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_BINARY_DIR}/${APP_NAME}_${BUILD_VERSION}.uapp ${CMAKE_CURRENT_SOURCE_DIR}/../../../Output/
    COMMENT "Merging Running application"
)
