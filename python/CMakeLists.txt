# This file is now redundant. Python bindings are built as part of the main CMake configuration
# from CMakeLists.txt in the project root.
# If you need a separate Python bindings build, create a separate build directory and use the
# NLM_ENABLE_PYTHON_BINDINGS option.

# The main CMakeLists.txt now includes all necessary targets for building NLM with Python bindings
# when NLM_ENABLE_PYTHON_BINDINGS is ON.

message(FATAL_ERROR "This CMakeLists.txt is deprecated. Use the main CMakeLists.txt in the project root.")

# For backward compatibility, if this is the only CMakeLists.txt being used:
# 1. Configure the project with: cmake -DNLM_ENABLE_PYTHON_BINDINGS=ON ..
# 2. The main CMakeLists.txt will handle all Python bindings
# 3. Run: cmake --build . --target pynlm
