cmake_minimum_required(VERSION 3.21)

set(CMAKE_VERBOSE_MAKEFILE ON)

set(CMAKE_TOOLCHAIN_FILE "${CMAKE_CURRENT_SOURCE_DIR}/toolchain-arm-none-eabi.cmake")

project(Running LANGUAGES C CXX ASM)

# Set standards
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

# Paths
set(SDK_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../../../../../SDK")
set(LIBS_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../../Libs")
set(TOUCHGFX_GUI_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../TouchGFX-GUI")
set(OUTPUT_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../../../Output")
set(RESOURCES_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../../../Resources")
set(SCRIPTS_PATH "${SDK_PATH}/Utilities/Scripts")

# App Info
set(APP_ID "A12E9F4C8B7D3A65")
set(APP_NAME "Running")
set(DEV_ID "UNA")

# Set build version using git describe
set(BUILD_VERSION "0.0.0-dev")  # Default when version not recognized
execute_process(COMMAND git rev-parse --git-dir OUTPUT_VARIABLE GIT_DIR ERROR_QUIET)
if(GIT_DIR)
    execute_process(COMMAND git rev-parse --short=7 HEAD OUTPUT_VARIABLE COMMIT_HASH OUTPUT_STRIP_TRAILING_WHITESPACE)
    execute_process(COMMAND git describe --exact-match --tags HEAD RESULT_VARIABLE TAG_RESULT OUTPUT_VARIABLE TAG OUTPUT_STRIP_TRAILING_WHITESPACE ERROR_QUIET)
    if(TAG_RESULT EQUAL 0)
        string(REGEX REPLACE "^v" "" TAG "${TAG}")
        set(BUILD_VERSION "${TAG}")
    else()
        execute_process(COMMAND git describe --always --tags --abbrev=7 OUTPUT_VARIABLE DESC OUTPUT_STRIP_TRAILING_WHITESPACE)
        if(DESC MATCHES "^g")
            string(REGEX REPLACE "^g" "" HASH "${DESC}")
            set(BUILD_VERSION "${HASH}")
        else()
            string(REGEX MATCH "(.+)-g" _ "${DESC}")
            set(TAG "${CMAKE_MATCH_1}")
            string(REGEX MATCH "g(.+)" _ "${DESC}")
            set(HASH "${CMAKE_MATCH_1}")
            string(REGEX REPLACE "^v" "" TAG "${TAG}")
            set(BUILD_VERSION "${TAG}-${HASH}")
        endif()
    endif()
    execute_process(COMMAND git status --porcelain OUTPUT_VARIABLE GIT_STATUS)
    if(GIT_STATUS)
        set(BUILD_VERSION "${BUILD_VERSION}-dirty")
    endif()
endif()

message("BUILD_VERSION: ${BUILD_VERSION}")

# Common compile options (match CubeIDE exactly)
add_compile_options(
    -mcpu=cortex-m33
    -mfpu=fpv5-sp-d16
    -mfloat-abi=hard
    -Os
    -fPIC
    -Wall
    -ffunction-sections
    -fdata-sections
    -fno-exceptions
    -fno-rtti
    -fno-use-cxa-atexit
    -fstack-usage
    -nodefaultlibs
    -nostdlib
    -mthumb
    # -fcyclomatic-complexity
)

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

target_compile_definitions(RunningService.elf PRIVATE
    BUILD_VERSION="${BUILD_VERSION}"
    APP_NAME="${APP_NAME}"
    DEV_ID="${DEV_ID}"
    APP_ID="${APP_ID}"
)

target_link_libraries(RunningService.elf PRIVATE
    -Wl,--start-group
    ${SDK_PATH}/libc++/libstdc++.a
    -Wl,--end-group
)

target_link_options(RunningService.elf PRIVATE
    -T "${CMAKE_CURRENT_SOURCE_DIR}/RunningService.ld"
    -Wl,-Map=${CMAKE_BINARY_DIR}/RunningService.elf.map
    -Wl,--gc-sections
    -nostartfiles
    -nodefaultlibs
    -nostdlib
    -static
    -L${SDK_PATH}/libc++
    -Wl,--emit-relocs
    -Wl,-L${SDK_PATH}/Utilities/Scripts/linker
    -mcpu=cortex-m33
    -mfpu=fpv5-sp-d16
    -mfloat-abi=hard
    -mthumb
)

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

target_link_libraries(RunningGUI.elf PRIVATE
    -Wl,--start-group
    ${SDK_PATH}/libc++/libstdc++.a
    ${TOUCHGFX_GUI_PATH}/touchgfx/lib/core/cortex_m33/gcc/libtouchgfx-float-abi-hard.a
    -Wl,--end-group
)

target_link_options(RunningGUI.elf PRIVATE
    -L${TOUCHGFX_GUI_PATH}/touchgfx/lib/core/cortex_m33/gcc
    -T "${CMAKE_CURRENT_SOURCE_DIR}/RunningGUI.ld"
    -Wl,-Map=${CMAKE_BINARY_DIR}/RunningGUI.elf.map
    -Wl,--gc-sections
    -nostartfiles
    -nodefaultlibs
    -nostdlib
    -static
    -L${SDK_PATH}/libc++
    -Wl,--emit-relocs
    -Wl,-L${SDK_PATH}/Utilities/Scripts/linker
    -mcpu=cortex-m33
    -mfpu=fpv5-sp-d16
    -mfloat-abi=hard
    -mthumb
)

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
