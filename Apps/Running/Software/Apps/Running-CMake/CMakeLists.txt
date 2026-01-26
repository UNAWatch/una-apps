# Watch App CMakeLists.txt - Simplified version using SDK
cmake_minimum_required(VERSION 3.21)

# Includes sets:
#              UNA_SDK_SOURCES_COMMON
#              UNA_SDK_SOURCES_SERVICE
#              UNA_SDK_SOURCES_GUI
#              UNA_SDK_INCLUDE_DIRS_COMMON
#              UNA_SDK_INCLUDE_DIRS_SERVICE
#              UNA_SDK_INCLUDE_DIRS_GUI
#              SCRIPTS_PATH
include($ENV{UNA_SDK}/cmake/una-app.cmake)
include($ENV{UNA_SDK}/cmake/una-sdk.cmake)

# Determine paths
get_filename_component(APP_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/../../.." ABSOLUTE)

# Includes LIBS_SOURCES, LIBS_INCLUDE_DIRS
include(${APP_ROOT}/Software/Libs/libs.cmake)

# Includes TOUCHGFX_SOURCES, TOUCHGFX_INCLUDE_DIRS
include(${APP_ROOT}/Software/Apps/TouchGFX-GUI/touchgfx.cmake)

project(Running)

# App configuration
set(APP_ID "A12E9F4C8B7D3A65")
set(APP_NAME "Running")
set(DEV_ID "UNA")

# Print variable values to highlight no hidden dependencies
message("APP_ID: ${APP_ID}")
message("APP_NAME: ${APP_NAME}")
message("DEV_ID: ${DEV_ID}")
message("APP_TYPE: ${APP_TYPE}")
message("LIBS_PATH: ${LIBS_PATH}")
message("TOUCHGFX_GUI_PATH: ${TOUCHGFX_GUI_PATH}")
message("OUTPUT_PATH: ${OUTPUT_PATH}")
message("RESOURCES_PATH: ${RESOURCES_PATH}")

# Build the complete app using SDK functions
una_app_build_app()