#!/bin/bash

# Get the directory where the script is located
PROJECT_DIR=$(dirname "$0")

BUILD_DIR="${PROJECT_DIR}/build"

BUILD_SPEC="Release"
BUILD_ARG_PROVIDED=false

while (( "$#" )); do
    case "$1" in --build)
        BUILD_SPEC="$2"
        BUILD_ARG_PROVIDED=true
        if [[ -z "$BUILD_SPEC" || $BUILD_SPEC == --* ]]; then
            echo "Error: Expected argument after --build"
            exit 1
        fi
        if [[ "$BUILD_SPEC" != "Debug" && "$BUILD_SPEC" != "Release" ]]; then
            echo "Error: --build argument must be 'Debug' or 'Release'"
            exit 1
        fi
        shift 2
        ;;
        *)
        echo "Unknown argument: $1"
        exit 1
        ;;
    esac
done

if [ "$BUILD_ARG_PROVIDED" = false ] ; then
    echo "-----------------------------------------------------------------------------------------"
    echo "-- Warning: --build argument was not provided. Using default build argument as 'Debug' --"
    echo "-----------------------------------------------------------------------------------------"
fi

mkdir -p  ${BUILD_DIR}

cd ${BUILD_DIR}

cmake .. -DCMAKE_BUILD_TYPE=${BUILD_SPEC}
cmake --build .