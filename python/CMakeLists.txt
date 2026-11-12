# DEPRECATED - Python bindings are now built using the root CMakeLists.txt
# This file is kept for backward compatibility only.
# The proper way to build Python bindings is via pip install . with pyproject.toml

message(WARNING "The python/CMakeLists.txt file is deprecated. Use pip install . with pyproject.toml instead.")

# For anyone who still tries to use this file directly, exit with an error
message(FATAL_ERROR "Please build NLM using pip install . or run cmake from the project root.")
