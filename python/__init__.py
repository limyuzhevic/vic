"""
Python bindings for NLM (Neural Learning Machine) C++ neural simulation framework.

This package provides Python access to the NLM brain simulator through pybind11.
The actual C++ extension module must be built first using scikit-build.

Installation:
    pip install scikit-build-core pybind11>=2.11.0
    pip install -e .

Usage:
    import pynlm
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    brain.initialize()
    for i in range(100):
        brain.step(i)
"""

__version__ = "0.1.0"

# Try to import the C++ extension
try:
    import pynlm as pynlm_module
except ImportError:
    pynlm_module = None

# Re-export everything from the C++ extension
if pynlm_module:
    from pynlm import *

# Make sure version is available
__all__ = [
    '__version__',
]

# Add the module's attributes to the package
if pynlm_module:
    # Add __version__ if not present in the C++ module
    if hasattr(pynlm_module, '__version__'):
        __version__ = pynlm_module.__version__
