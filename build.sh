#!/bin/bash
cd /workspace/cf14c76c-4b1f-4d38-9a96-64cd8a400481/sessions/agent_7e4762c8-c49b-4420-af21-1bd1969e1aae

rm -rf build
mkdir build
cd build

cmake .. -DCMAKE_BUILD_TYPE=Release

if [ $? -eq 0 ]; then
    echo "CMake configuration successful"
    make -j4
    if [ $? -eq 0 ]; then
        echo "Build successful!"
    else
        echo "Build failed"
    fi
else
    echo "CMake configuration failed"
fi
