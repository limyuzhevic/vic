# Simple test to check if pynlm is properly structured
import pynlm
import sys

# Test basic functionality
print("Testing pynlm import...")
print(f"pynlm version: {pynlm.__version__}")

# Test creating a brain
config = pynlm.createDefaultConfig()
print(f"Config created: {config}")

brain = pynlm.createBrain(config)
print(f"Brain created with {brain.getTotalNeuronCount()} neurons")

# Test that the package structure is correct
print("\nPython package structure check:")
print("✓ pynlm imports successfully")
print("✓ createDefaultConfig() works")
print("✓ createBrain() works")
print("✓ getTotalNeuronCount() works")

print("\nAll basic tests passed!")
print("\nTo continue with improvements, we need to:")
print("1. Create python/__init__.py to make it a proper package")
print("2. Add version information to the Python module")
print("3. Add more comprehensive documentation")
print("4. Add better error handling and Pythonic APIs")
