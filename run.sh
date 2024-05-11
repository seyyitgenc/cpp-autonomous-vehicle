#!/bin/bash

# Get the directory where the script is located
PROJECT_DIR=$(dirname "$0")

EXECUTABLE_DIR="${PROJECT_DIR}/bin/"
EXECUTABLE_NAME="autonomous_vehicle"

BUILD_SPEC="release"
BUILD_ARG_PROVIDED=false

while (( "$#" )); do
    case "$1" in --build)
        BUILD_SPEC="$2"
        BUILD_ARG_PROVIDED=true
        if [[ -z "$BUILD_SPEC" || $BUILD_SPEC == --* ]]; then
            echo "Error: Expected argument after --build"
            exit 1
        fi
        if [[ "$BUILD_SPEC" != "debug" && "$BUILD_SPEC" != "release" ]]; then
            echo "Error: --build argument must be 'debug' or 'release'"
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
    echo "-- Warning: --build argument was not provided. Using default build argument as 'release' --"
    echo "-----------------------------------------------------------------------------------------"
fi

./$EXECUTABLE_DIR/$BUILD_SPEC/autonomous_vehicle