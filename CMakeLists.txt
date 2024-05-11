cmake_minimum_required(VERSION 3.10)

project(autonomous_vehicle)

# OpenCV
find_package(OpenCV REQUIRED)
set(LIBS ${OpenCV_LIBS})

# Check if on aarch64 environment
if(${CMAKE_SYSTEM_PROCESSOR} STREQUAL "aarch64")
    message(STATUS "Building on aarch64 environment")
    set(ONNXRUNTIME_FILE_NAME "onnxruntime-linux-aarch64-1.17.3")
else()
    message(FATAL_ERROR 
        "This project is only compatible with Raspberry Pi 64bit OS. For now there is not support for other OS systems. You can work on it if you wan't to!")
endif()

# onnxruntime
set(ONNXRUNTIME_VERSION "v1.17.3")
set(ONNXRUNTIME_URL "https://github.com/microsoft/onnxruntime/releases/download/${ONNXRUNTIME_VERSION}/${ONNXRUNTIME_FILE_NAME}.tgz")
set(ONNXRUNTIME_ARCHIVE ${CMAKE_BINARY_DIR}/${ONNXRUNTIME_FILE_NAME})
set(ONNXRUNTIME_DIR ${CMAKE_BINARY_DIR}/${ONNXRUNTIME_FILE_NAME})
set(ONNXRUNTIME_INCLUDE_DIRS ${ONNXRUNTIME_DIR}/include/)
set(ONNXRUNTIME_LIBS ${ONNXRUNTIME_DIR}/lib/libonnxruntime.so)

# Print the values of the variables
message(STATUS "onnxruntime information")
message(STATUS "    version: ${ONNXRUNTIME_VERSION}")
message(STATUS "    URL: ${ONNXRUNTIME_URL}")
message(STATUS "    Directory: ${ONNXRUNTIME_DIR}")
message(STATUS "    libraries: ${ONNXRUNTIME_LIBS}")
message(STATUS "    include: ${ONNXRUNTIME_INCLUDE_DIRS}")

# Check if onnxruntime is already installed
find_library(ONNXRUNTIME_LIB NAMES onnxruntime PATHS "${ONNXRUNTIME_DIR}/lib")

if(NOT ONNXRUNTIME_LIB)
    # Download and extract onnxruntime
    file(DOWNLOAD ${ONNXRUNTIME_URL} ${ONNXRUNTIME_FILE_NAME} SHOW_PROGRESS)
    execute_process(COMMAND ${CMAKE_COMMAND} -E tar xzf ${ONNXRUNTIME_FILE_NAME}
                    WORKING_DIRECTORY ${CMAKE_BINARY_DIR})
endif()

# onnxruntime
link_directories(
    ${ONNXRUNTIME_DIR}/lib)
set(LIBS ${LIBS} ${ONNXRUNTIME_LIBS})

# for libcamera opencv comptability
add_subdirectory(${CMAKE_SOURCE_DIR}/LCCV)

# LCCV
set(LIBS ${LIBS} liblccv.so)

# source files
file(GLOB_RECURSE SOURCE
    src/*.cpp
    src/*.hpp
    src/*.h

    src/utils/*.hpp
    src/utils/*.cpp
    src/utils/*.h
    )

# specify include directories
set(INC_DIRECTORIES
    ${CMAKE_SOURCE_DIR}/LCCV/include/
    ${CMAKE_SOURCE_DIR}/configuration/
    
    ${ONNXRUNTIME_INCLUDE_DIRS}
    ${OpenCV_INCLUDE_DIRS}
    ${LIBCAMERA_INCLUDE_DIRS}
    )
# generate config.h
configure_file(${CMAKE_SOURCE_DIR}/configuration/config.h.in ${CMAKE_SOURCE_DIR}/configuration/config.h)

# Set output directories for executables
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_SOURCE_DIR}/bin/debug)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_SOURCE_DIR}/bin/release)

add_executable(${PROJECT_NAME} ${SOURCE})
include_directories(${PROJECT_NAME} ${INC_DIRECTORIES})
target_link_libraries(${PROJECT_NAME} ${LIBS})