#!/usr/bin/env python3
"""
Quick check script to understand the current state and fix the integration.
"""

import os
import sys

def main():
    print("NLM Python Bindings - Quick State Check")
    print("=" * 50)
    
    # Check all directories
    directories = ["/workspace/app/python", "/workspace/app/src/python"]
    
    for dir_path in directories:
        print(f"\nDirectory: {dir_path}")
        if os.path.exists(dir_path):
            contents = os.listdir(dir_path)
            print(f"  Contents: {contents}")
            
            # Check for bindings.py
            if "bindings.py" in contents:
                print(f"  ✓ bindings.py found")
                
                # Check size
                bindings_path = os.path.join(dir_path, "bindings.py")
                size = os.path.getsize(bindings_path)
                print(f"  Size: {size:,} bytes")
            
            # Check for bindings.cpp
            if "bindings.cpp" in contents:
                print(f"  ✓ bindings.cpp found")
            
            # Check for __init__.py
            if "__init__.py" in contents:
                print(f"  ✓ __init__.py found")
                
                # Check if it imports properly
                init_path = os.path.join(dir_path, "__init__.py")
                try:
                    with open(init_path, 'r') as f:
                        content = f.read()
                        if "from .bindings import" in content:
                            print(f"  ✓ __init__.py imports from bindings")
                        else:
                            print(f"  ✗ __init__.py doesn't import bindings")
                except Exception as e:
                    print(f"  ✗ Error reading __init__.py: {e}")
        else:
            print(f"  ✗ Directory not found")
    
    print("\n" + "=" * 50)
    print("ISSUE SUMMARY")
    print("=" * 50)
    
    print("\nBased on the analysis, here's what needs to be done:")
    print("1. There are two bindings.py files - one in /workspace/app/python/ and one in /workspace/app/src/python/")
    print("2. The one in /workspace/app/python/ contains all the improved features")
    print("3. The one in /workspace/app/src/python/ is likely the original version")
    print("4. The CMake configuration in python/ expects the bindings.py to be in the same directory")
    print("5. The __init__.py in python/ imports from .bindings (which will be bindings.cpp)")
    
    print("\nSOLUTION:")
    print("The python/ directory should contain the bindings.py with all the enhanced features.")
    print("The src/python/ directory should be removed or kept as a backup.")
    print("The CMake configuration should build the Python bindings from python/ directory.")
    
    return 0

if __name__ == "__main__":
    sys.exit(main())
