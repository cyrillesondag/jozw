cmake_minimum_required(VERSION 3.0)
project(jozw)

# java jni
find_package(JNI REQUIRED)
include_directories(${JNI_INCLUDE_DIRS})

message(STATUS "JNI : ${JNI_INCLUDE_DIRS}")
# openzwave lib
find_library(OPENZWAVE_LIB REQUIRED PATH_SUFFIXES lib lib64 NAMES libopenzwave.so libopenzwave.dylib)

# openzwave headers
find_path(OPENZWAVE_INCLUDES openzwave/Manager.h)
if(NOT OPENZWAVE_INCLUDES)
    message(FATAL_ERROR "open_zwave library headers not found")
endif()

include_directories(${OPENZWAVE_INCLUDES}/openzwave)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_DIR src/main/native)
set(SOURCE_FILES ${SOURCE_DIR}/org_kyrillos_jozw_Ozw.cpp ${SOURCE_DIR}/org_kyrillos_jozw_Ozw.h)

add_library(jozw SHARED ${SOURCE_FILES})
target_link_libraries(jozw ${OPENZWAVE_LIB})