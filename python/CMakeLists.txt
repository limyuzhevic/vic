# Phase 5: The root CMakeLists.txt now builds the pynlm Python module
# when `pip install .` is used, because pyproject.toml sets
# NLM_BUILD_PYTHON_BINDINGS=ON (and has pybind11 >= 2.11.0).
# The standalone python/CMakeLists.txt is no longer needed and would
# conflict with the root targets (nlm_core, nlm_agent, nlm_world) that
# it redefines.
# 
# IMPORTANT: Remove the python/CMakeLists.txt file to avoid build conflicts.
# This file should be deleted, as the root CMakeLists.txt with pyproject.toml
# handles all Python bindings correctly.
