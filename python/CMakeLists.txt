# This file is kept for backward compatibility.
# The main NLM Python bindings are now built in the root CMakeLists.txt
# as part of the main build system.

# Users can still explicitly build Python bindings using:
# cmake -DNLM_BUILD_PYTHON_BINDINGS=ON .. -DCMAKE_MODULE_PATH=../python

# Empty CMakeLists to avoid build conflicts

message(STATUS "NLM Python bindings are built in the root CMakeLists.txt")
message(STATUS "Use: cmake -DNLM_BUILD_PYTHON_BINDINGS=ON .. to build Python bindings")