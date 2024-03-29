#!/bin/bash

# Get the name of the current directory
current_directory=$(basename "$PWD")

# Check if the current directory is "scripts"
if [ "$current_directory" == "scripts" ]; then
    # Move up to the parent directory
    cd ..
fi

if [ -d "android" ] || [ -d "ios" ]; then
    src="./src"
    ios="./ios/Classes/src"

    # Remove the source files
    rm -rf "$ios"

    # Create the source directories
    mkdir -p "$ios"

    # Copy files
    rsync -av "$src/" "$ios/"
else
    echo "The current directory is not a 'flutter project'"
fi
