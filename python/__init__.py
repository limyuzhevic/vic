import sys
import os

# Add the source directory to the path so we can import from the C++ bindings
sys.path.insert(0, os.path.join(os.path.dirname(__file__), '../../src'))

# Import the bindings module
try:
    from .bindings import pynlm
    print("NLM Python bindings loaded successfully")
except ImportError as e:
    print(f"Error loading NLM bindings: {e}")
    print("Please build the C++ extensions first with:")
    print("  pip install -e .")
    raise