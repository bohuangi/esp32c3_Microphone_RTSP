# The following lines of boilerplate have to be in your project's
# CMakeLists in this exact order for cmake to work correctly
cmake_minimum_required(VERSION 3.16)
include_directories(/root/esp/esp-idf/components/esp_timer/include)
set(EXTRA_COMPONENT_DIRS components/components/opus components/Micro-RTSP-Audio)# components/protobuf-c-rpc)
#include($ENV{ADF_PATH}/CMakeLists.txt)
include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(hello_opus)
