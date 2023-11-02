#!/bin/bash

# Get the name of the current directory
current_directory=$(basename "$PWD")

# Check if the current directory is "scripts"
if [ "$current_directory" == "scripts" ]; then
    # Move up to the parent directory
    cd ..
fi

if [ -d "android" ] || [ -d "ios" ]; then
    rm -rf ./build
    rm -rf ./download
    rm -rf ./include
    rm -rf ./android/src/main/jniLibs
    chmod +rw ./ios/opencv2.framework
    rm -rf ./ios/opencv2.framework
    flutter clean
    cd example
    flutter clean
    cd ..
else
    echo "The current directory is not a 'flutter project'"
fi