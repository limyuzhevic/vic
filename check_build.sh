#!/bin/bash

# Check Python environment
python3 --version
pip3 --version

# Check if required packages are installed
pip3 list | grep -E "scikit-build-core|pybind11|pynlm" || echo "Required packages not found"

# Check for build artifacts
find /workspace/cf14c76c-4b1f-4d38-9a96-64cd8a400481/sessions/agent_52e9498d-7b95-45ef-8f80-119330d84807 -name "*.so" -o -name "*.pyd" -o -name "*.so.*" 2>/dev/null | head -10

# Check project structure
ls -la /workspace/cf14c76c-4b1f-4d38-9a96-64cd8a400481/sessions/agent_52e9498d-7b95-45ef-8f80-119330d84807/ | grep -E "python|pyproject|build"

# Check if pynlm is importable
try:
    python3 -c "import pynlm; print('pynlm imported successfully')" 2>&1 || echo "pynlm import failed"
except ImportError as e:
    echo "Import error: $e"