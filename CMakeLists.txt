cmake_minimum_required(VERSION 3.10)

project(autonomous_vehicle)

# OpenCV
find_package(OpenCV REQUIRED)
set(LIBS ${OpenCV_LIBS})

# onnxruntime
set(LIBS ${LIBS} ${CMAKE_SOURCE_DIR}/lib/libonnxruntime.so)

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
    ${PROJECT_SOURCE_DIR}/include/
    ${OpenCV_INCLUDE_DIRS}
    ${CMAKE_SOURCE_DIR}/configuration/
    )

# generate config.h
configure_file(${CMAKE_SOURCE_DIR}/configuration/config.h.in ${CMAKE_SOURCE_DIR}/configuration/config.h)

# Set output directories for executables
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_SOURCE_DIR}/bin/debug)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_SOURCE_DIR}/bin/release)

add_executable(${PROJECT_NAME} ${SOURCE})
include_directories(${PROJECT_NAME} ${INC_DIRECTORIES})
target_link_libraries(${PROJECT_NAME} ${LIBS})
