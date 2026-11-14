#!/usr/bin/env python3
"""Python bindings analysis script for NLM project"""

import subprocess
import os
import sys
import json
from pathlib import Path

# Get the working directory
workspace = os.environ.get('PWD', '/workspace/app')
script_dir = Path(__file__).parent
workspace_root = Path(workspace)

print(f"Analyzing NLM Python bindings at {workspace_root}")
print("=" * 60)

# Check if python bindings exist
python_dir = workspace_root / 'python'
bindings_file = python_dir / 'bindings.cpp'

if not bindings_file.exists():
    print("ERROR: Python bindings not found at", bindings_file)
    sys.exit(1)

# Read bindings.cpp
with open(bindings_file, 'r') as f:
    content = f.read()

# Extract module name
module_match = "PYBIND11_MODULE(pynlm, m)"
if "PYBIND11_MODULE(pynlm, m)" in content:
    module_name = "pynlm"
    print(f"Module name: {module_name}")
else:
    print("ERROR: Could not find PYBIND11_MODULE statement")
    sys.exit(1)

# Count bound classes and methods
pybind11_import = False
count = 0
for line in content.split('\n'):
    line = line.strip()
    if line.startswith('py::class_<') or line.startswith('py::enum_') or line.startswith('m.def'):
        count += 1
        if line.startswith('py::class_<') and '(' in line:
            class_name = line.split('class_<')[1].split('>')[0].split(',')[0]
            print(f"  Class: {class_name}")
    if 'pybind11/pybind11.h' in line:
        pybind11_import = True

print(f"\nSummary:")
print(f"  Total bound classes/methods: {count}")
print(f"  pybind11 imported: {pybind11_import}")

# Check for advanced features
advanced_features = {
    'Python bindings file exists': bindings_file.exists(),
    'Module name: pynlm': module_name == 'pynlm',
    'Has NeuronId bindings': 'NeuronId' in content,
    'Has SynapseId bindings': 'SynapseId' in content,
    'Has Brain bindings': 'Brain' in content,
    'Has Config bindings': 'Config' in content,
    'Has AgentBrain bindings': 'AgentBrain' in content,
    'Has SimpleWorld bindings': 'SimpleWorld' in content,
    'Has Sensor bindings': 'SensoryInput' in content,
    'Has Action bindings': 'Action' in content,
    'Has Visualization bindings': 'VisualizationInterface' in content,
    'Has Neuromodulation bindings': 'Dopamine' in content,
    'Has Memory bindings': 'WorkingMemory' in content,
    'Has Prediction bindings': 'PredictionSystem' in content,
    'Has Factory functions': 'createBrain' in content or 'createConfig' in content,
    'Has High-level API': 'run' in content.lower() or 'simulate' in content.lower(),
}

print(f"\nAdvanced features check:")
for feature, present in advanced_features.items():
    status = "✓" if present else "✗"
    print(f"  {status} {feature}")

# Check configuration
print(f"\nConfiguration:")

pyproject = workspace_root / 'pyproject.toml'
if pyproject.exists():
    print("  pyproject.toml exists")
    with open(pyproject, 'r') as f:
        pycontent = f.read()
        if 'scikit-build-core' in pycontent:
            print("  Using scikit-build-core")
        if 'pybind11' in pycontent:
            print("  pybind11 dependency specified")
        if 'NLM_BUILD_PYTHON_BINDINGS=ON' in pycontent:
            print("  Python bindings enabled by default")
else:
    print("  pyproject.toml not found")

# Check CMake
print(f"\nCMake configuration:")
cmake = workspace_root / 'CMakeLists.txt'
if cmake.exists():
    print("  CMakeLists.txt exists")
    with open(cmake, 'r') as f:
        cmake_content = f.read()
        if 'NLM_BUILD_PYTHON_BINDINGS' in cmake_content:
            print("  Python bindings option defined")
        if 'pybind11_add_module(pynlm' in cmake_content:
            print("  pynlm module target defined")
else:
    print("  CMakeLists.txt not found")

print("\n" + "=" * 60)
print("Analysis complete.")
print("\nRecommendations:")
print("1. Add more Pythonic wrappers (e.g., context managers, properties)")
print("2. Add high-level API functions for common tasks")
print("3. Add error handling and validation")
print("4. Add comprehensive documentation")
print("5. Add unit tests for Python bindings")
print("6. Add performance optimizations where needed")
print("7. Add support for more advanced Python features (e.g., numpy arrays)")
