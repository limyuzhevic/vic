#!/usr/bin/env python3

import subprocess
import sys
import os

def run(cmd):
    print(f"Running: {cmd}")
    result = subprocess.run(cmd, shell=True, capture_output=True, text=True)
    if result.stdout:
        print(result.stdout)
    if result.stderr:
        print(f"STDERR: {result.stderr}")
    return result.returncode, result.stdout, result.stderr

print("=== Python Environment Check ===\n")

run("python3 --version")

print("\n=== pip3 packages ===")
run("pip3 list | grep -E 'scikit-build-core|pybind11|pynlm' || echo 'Required packages not found'")

print("\n=== Build artifacts search ===")
run("find /workspace/cf14c76c-4b1f-4d38-9a96-64cd8a400481/sessions/agent_52e9498d-7b95-45ef-8f80-119330d84807 -name '*.so' -o -name '*.pyd' -o -name '*.so.*' 2>/dev/null | head -10")

print("\n=== Project structure ===")
run("ls -la /workspace/cf14c76c-4b1f-4d38-9a96-64cd8a400481/sessions/agent_52e9498d-7b95-45ef-8f80-119330d84807/ | grep -E 'python|pyproject|build' | head -20")

print("\n=== Import checks ===")
try:
    import pybind11
    print(f"pybind11 imported: {pybind11.__version__}")
except ImportError as e:
    print(f"pybind11 import failed: {e}")

try:
    import scikit_build_core
    print("scikit_build_core imported successfully")
except ImportError as e:
    print(f"scikit_build_core import failed: {e}")

try:
    import pynlm
    print(f"pynlm imported: {getattr(pynlm, '__version__', 'unknown version')}")
except ImportError as e:
    print(f"pynlm import failed: {e}")

print("\n=== Summary ===")
print("If scikit-build-core and pybind11 are installed, you should be able to build pynlm")
print("Check if build artifacts exist - if not, the build hasn't been completed yet")