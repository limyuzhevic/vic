#!/bin/bash
set -e

# Activate virtual environment
if [ -f "venv/bin/activate" ]; then
    source venv/bin/activate
fi

# Upgrade pip
echo "Upgrading pip..."
python -m pip install --upgrade pip

# Install scikit-build-core and dependencies
echo "Installing scikit-build-core, pybind11, numpy, pytest..."
python -m pip install scikit-build-core pybind11 numpy pytest

# Install pynlm in development mode
echo "Installing pynlm in development mode..."
python -m pip install -e .
