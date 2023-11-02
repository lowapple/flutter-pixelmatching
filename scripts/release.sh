#!/bin/bash

# Get the name of the current directory
current_directory=$(basename "$PWD")

# Check if the current directory is "scripts"
if [ "$current_directory" == "scripts" ]; then
    # Move up to the parent directory
    cd ..
fi

if [ -d "android" ] || [ -d "ios" ]; then
    sh scripts/setup_ios.sh
    dart pub publish
else
    echo "The current directory is not a 'flutter project'"
fi