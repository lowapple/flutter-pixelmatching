#!/bin/bash

# Get the name of the current directory
current_directory=$(basename "$PWD")

# Check if the current directory is "scripts"
if [ "$current_directory" == "scripts" ]; then
    # Move up to the parent directory
    cd ..
fi

if [ -d "android" ] || [ -d "ios" ]; then
    mkdir -p download
    cd download

    wget -O opencv-4.5.4-android-sdk.zip https://sourceforge.net/projects/opencvlibrary/files/4.5.4/opencv-4.5.4-android-sdk.zip/download
    wget -O opencv-4.5.4-ios-framework.zip https://sourceforge.net/projects/opencvlibrary/files/4.5.4/opencv-4.5.4-ios-framework.zip/download

    unzip opencv-4.5.4-android-sdk.zip
    unzip opencv-4.5.4-ios-framework.zip

    cp -r opencv2.framework ../ios
    cp -r OpenCV-android-sdk/sdk/native/jni/include ../
    mkdir -p ../android/src/main/jniLibs/
    cp -r OpenCV-android-sdk/sdk/native/libs/* ../android/src/main/jniLibs/

    rm -rf opencv-4.5.4-android-sdk.zip
    rm -rf opencv-4.5.4-ios-framework.zip
    rm -rf opencv2.framework
    rm -rf OpenCV-android-sdk
    cd ..
    rm -rf download

    sh scripts/setup_ios.sh
    flutter pub get
    cd example
    flutter pub get
else
    echo "The current directory is not a 'flutter project'"
fi
