cmake_minimum_required(VERSION 3.10)

project(autonomous_vehicle)

# OpenCV
find_package(OpenCV REQUIRED)
set(LIBS ${OpenCV_LIBS})

# onnxruntime
set(LIBS ${LIBS} /home/seyyit/projects/cpp-autonomous-vehicle/lib/libonnxruntime.so)

file(GLOB_RECURSE SOURCE
    src/*.cpp
    src/*.hpp
    src/*.h
    )

set(INC_DIRECTORIES
    ${PROJECT_SOURCE_DIR}/include/
    ${OpenCV_INCLUDE_DIRS}
    )
# Set output directories for executables
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_SOURCE_DIR}/bin/debug)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_SOURCE_DIR}/bin/release)

add_executable(${PROJECT_NAME} ${SOURCE})
include_directories(${PROJECT_NAME} ${INC_DIRECTORIES})
target_link_libraries(${PROJECT_NAME} ${LIBS})
