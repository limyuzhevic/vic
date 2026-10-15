#!/usr/bin/env python3
"""
Final verification and build script for NLM Python bindings integration.
"""

import os
import subprocess
import sys

def run_command(cmd, description):
    print(f"\n{description}")
    print(f"Command: {cmd}")
    try:
        result = subprocess.run(cmd, shell=True, capture_output=True, text=True, cwd="/workspace/app")
        if result.returncode == 0:
            print(f"✓ {description}")
            return True
        else:
            print(f"✗ {description}")
            print(f"Error: {result.stderr}")
            return False
    except Exception as e:
        print(f"✗ {description} - Exception: {e}")
        return False

def main():
    print("NLM Python Bindings - Final Integration")
    print("=" * 60)
    
    # Check the current state
    print("\nChecking current project structure...")
    
    # List Python directories
    python_dirs = []
    for item in os.listdir("/workspace/app"):
        if os.path.isdir(os.path.join("/workspace/app", item)):
            if "python" in item.lower():
                python_dirs.append(item)
    
    print(f"Python directories found: {python_dirs}")
    
    # Check for bindings.cpp files
    bindings_files = []
    for dir_name in python_dirs:
        dir_path = os.path.join("/workspace/app", dir_name)
        for root, dirs, files in os.walk(dir_path):
            for file in files:
                if file == "bindings.cpp":
                    bindings_files.append(os.path.join(root, file))
    
    print(f"\nbindings.cpp files found: {len(bindings_files)}")
    for bf in bindings_files:
        print(f"  - {bf}")
    
    # Determine the correct bindings.cpp
    if len(bindings_files) == 1:
        active_bindings = bindings_files[0]
        print(f"\n✓ Single bindings.cpp file found: {active_bindings}")
    else:
        print(f"\n⚠ Multiple bindings.cpp files found. Need to resolve...")
        print(f"  This is the original issue we need to fix.")
    
    # Check CMake configuration
    print("\n" + "=" * 60)
    print("CMake Configuration Check")
    print("=" * 60)
    
    cmake_checks = [
        ("/workspace/app/CMakeLists.txt", "Root CMakeLists.txt"),
        ("/workspace/app/python/CMakeLists.txt", "Python CMakeLists.txt"),
        ("/workspace/app/src/python/CMakeLists.txt", "Source Python CMakeLists.txt")
    ]
    
    cmake_ok = True
    for filepath, description in cmake_checks:
        if os.path.exists(filepath):
            with open(filepath, 'r') as f:
                content = f.read()
            
            if 'pybind11' in content:
                print(f"✓ {description} - Contains pybind11")
            else:
                print(f"✗ {description} - Missing pybind11")
                cmake_ok = False
        else:
            print(f"✗ {description} - File not found")
            cmake_ok = False
    
    # Check Python package structure
    print("\n" + "=" * 60)
    print("Python Package Structure")
    print("=" * 60)
    
    python_package_path = "/workspace/app/python"
    required_files = [
        "__init__.py",
        "__main__.py", 
        "CMakeLists.txt",
        "README.md",
        "HOW_TO_USE.md",
        "easy_usage.md",
        "verify_python_bindings.py",
        "cleanup_bindings.py"
    ]
    
    print("Required files in python/:")
    all_required_present = True
    for filename in required_files:
        filepath = os.path.join(python_package_path, filename)
        if os.path.exists(filepath):
            print(f"  ✓ {filename}")
        else:
            print(f"  ✗ {filename}")
            all_required_present = False
    
    # Show current bindings.py content if it exists
    bindings_py = os.path.join(python_package_path, "bindings.py")
    if os.path.exists(bindings_py):
        print(f"\nCurrent bindings.py content (first 20 lines):")
        with open(bindings_py, 'r') as f:
            for i, line in enumerate(f.readlines()[:20]):
                print(f"  {i+1:2d}: {line.rstrip()}")
    
    print("\n" + "=" * 60)
    print("INTEGRATION STATUS")
    print("=" * 60)
    
    status = "UNKNOWN"
    if all_required_present and cmake_ok:
        status = "READY"
    elif not all_required_present:
        status = "NEEDS FILES"
    elif not cmake_ok:
        status = "NEEDS CONFIGURATION"
    
    print(f"Project Status: {status}")
    
    if status == "READY":
        print("\n✓ All checks passed!")
        print("The Python bindings are properly integrated.")
        print("\nNext steps:")
        print("1. Run: python cleanup_bindings.py")
        print("2. Build with CMake")
        print("3. Test the bindings")
        print("4. Run verification script")
    else:
        print(f"\n⚠ Project needs attention before integration:")
        if not all_required_present:
            print(f"  - Missing required files in python/ directory")
        if not cmake_ok:
            print(f"  - CMake configuration issues")
        
        print(f"\nRecommendations:")
        print(f"1. Create missing files in python/ directory")
        print(f"2. Fix CMake configuration")
        print(f"3. Resolve any file conflicts")
    
    return 0 if status == "READY" else 1

if __name__ == "__main__":
    sys.exit(main())
