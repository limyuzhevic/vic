#!/usr/bin/env python3
"""
Setup script for enhanced NLM Python bindings.
This script creates the enhanced Python bindings structure.
"""

import os
import sys

def main():
    print("Setting up enhanced NLM Python bindings...")
    
    # Create directory structure
    directories = [
        "include/nlm",
        "src",
        "tests",
        "docs",
    ]
    
    for directory in directories:
        os.makedirs(directory, exist_ok=True)
        print(f"✓ Created directory: {directory}")
    
    print("\nEnhanced Python bindings structure created successfully!")
    print("\nNext steps:")
    print("1. Update the main CMakeLists.txt to use the new bindings")
    print("2. Replace the old bindings.cpp with the new implementation")
    print("3. Build the bindings: cmake .. && make")
    print("4. Run tests: python tests/test_bindings.py")
    
    return 0

if __name__ == "__main__":
    sys.exit(main())
