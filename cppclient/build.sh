#!/bin/sh

echo "Building the project..."

if [ ! -d "./build" ]; then
	mkdir ./build
fi

cd build

if cmake ../ $@; then
echo "calling make...";
else 
echo "CMake failed!";
cd ..
exit 1 
fi

if make -j7; then
echo "build ready";
else 
echo "build failed!";
cd ..
exit 2
fi


cd ..