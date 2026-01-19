# Watch App CMakeLists.txt - Simplified version using SDK
cmake_minimum_required(VERSION 3.21)
include($ENV{WATCH_SDK_PATH}/cmake/watch_project.cmake)

project(Running)

# App configuration
set(APP_ID "A12E9F4C8B7D3A65")
set(APP_NAME "Running")
set(DEV_ID "UNA")

# Build the complete app using SDK functions
watch_build_app()