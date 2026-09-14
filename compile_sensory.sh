#!/bin/bash

# Compile all sensory processing files to check for errors

echo "Compiling Vision.cpp..."
g++ -c /workspace/cf14c76c-4b1f-4d38-9a96-64cd8a400481/sessions/agent_f9f272d3-4384-4d88-a1ec-fef2e4cd6bd5/src/sensory/Vision.cpp -std=c++17 -I /workspace/cf14c76c-4b1f-4d38-9a96-64cd8a400481/sessions/agent_f9f272d3-4384-4d88-a1ec-fef2e4cd6bd5/src/sensory

if [ $? -eq 0 ]; then
    echo "Vision.cpp compiled successfully!"
else
    echo "Vision.cpp compilation failed"
fi

echo "\nCompiling Audio.cpp..."
g++ -c /workspace/cf14c76c-4b1f-4d38-9a96-64cd8a400481/sessions/agent_f9f272d3-4384-4d88-a1ec-fef2e4cd6bd5/src/sensory/Audio.cpp -std=c++17 -I /workspace/cf14c76c-4b1f-4d38-9a96-64cd8a400481/sessions/agent_f9f272d3-4384-4d88-a1ec-fef2e4cd6bd5/src/sensory

if [ $? -eq 0 ]; then
    echo "Audio.cpp compiled successfully!"
else
    echo "Audio.cpp compilation failed"
fi

echo "\nCompiling InternalSignals.cpp..."
g++ -c /workspace/cf14c76c-4b1f-4d38-9a96-64cd8a400481/sessions/agent_f9f272d3-4384-4d88-a1ec-fef2e4cd6bd5/src/sensory/InternalSignals.cpp -std=c++17 -I /workspace/cf14c76c-4b1f-4d38-9a96-64cd8a400481/sessions/agent_f9f272d3-4384-4d88-a1ec-fef2e4cd6bd5/src/sensory

if [ $? -eq 0 ]; then
    echo "InternalSignals.cpp compiled successfully!"
else
    echo "InternalSignals.cpp compilation failed"
fi

echo "\nCompiling SensoryInput.cpp..."
g++ -c /workspace/cf14c76c-4b1f-4d38-9a96-64cd8a400481/sessions/agent_f9f272d3-4384-4d88-a1ec-fef2e4cd6bd5/src/sensory/SensoryInput.cpp -std=c++17 -I /workspace/cf14c76c-4b1f-4d38-9a96-64cd8a400481/sessions/agent_f9f272d3-4384-4d88-a1ec-fef2e4cd6bd5/src/sensory

if [ $? -eq 0 ]; then
    echo "SensoryInput.cpp compiled successfully!"
else
    echo "SensoryInput.cpp compilation failed"
fi

echo "\nCompiling SensoryPercept.cpp..."
g++ -c /workspace/cf14c76c-4b1f-4d38-9a96-64cd8a400481/sessions/agent_f9f272d3-4384-4d88-a1ec-fef2e4cd6bd5/src/agent/SensoryPercept.cpp -std=c++17 -I /workspace/cf14c76c-4b1f-4d38-9a96-64cd8a400481/sessions/agent_f9f272d3-4384-4d88-a1ec-fef2e4cd6bd5/src/agent -I /workspace/cf14c76c-4b1f-4d38-9a96-64cd8a400481/sessions/agent_f9f272d3-4384-4d88-a1ec-fef2e4cd6bd5/src/sensory

if [ $? -eq 0 ]; then
    echo "SensoryPercept.cpp compiled successfully!"
else
    echo "SensoryPercept.cpp compilation failed"
fi

echo "\nCompilation complete!"