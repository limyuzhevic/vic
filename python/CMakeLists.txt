# This file is for the Python package structure only
# The actual Python bindings are built by the main CMakeLists.txt

# Python package files to install
install(FILES
    "__init__.py"
    DESTINATION "${Python_SITEARCH}/pynlm"
)