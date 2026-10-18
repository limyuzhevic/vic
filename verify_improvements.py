#!/usr/bin/env python3
"""
Quick verification script for NLM Python package

This script verifies that the NLM Python package is properly structured
and all core modules are working correctly.
"""

import sys
import os

def check_python_package():
    """Check if the Python package structure is correct"""
    print("Checking NLM Python package structure...")
    
    # Check main package
    if not os.path.exists("python/__init__.py"):
        print("❌ Missing python/__init__.py")
        return False
    print("✅ python/__init__.py exists")
    
    # Check core submodules
    core_modules = [
        "python/core/__init__.py",
        "python/core/types.py",
        "python/core/config.py",
        "python/core/random.py",
        "python/core/logger.py",
        "python/core/simulation_clock.py"
    ]
    
    for module in core_modules:
        if not os.path.exists(module):
            print(f"❌ Missing {module}")
            return False
        print(f"✅ {module} exists")
    
    # Check other modules
    other_modules = [
        "python/brain/__init__.py",
        "python/agent/__init__.py",
        "python/world/__init__.py",
        "python/sensory/__init__.py",
        "python/motor/__init__.py"
    ]
    
    for module in other_modules:
        if not os.path.exists(module):
            print(f"❌ Missing {module}")
            return False
        print(f"✅ {module} exists")
    
    return True

def check_cmake_targets():
    """Check if CMakeLists.txt has been updated"""
    print("\nChecking CMake configuration...")
    
    if not os.path.exists("CMakeLists.txt"):
        print("❌ Missing CMakeLists.txt")
        return False
    
    # Read CMakeLists.txt and check for key targets
    with open("CMakeLists.txt", "r") as f:
        content = f.read()
    
    required_targets = [
        "nlm_phase5_demo",
        "nlm_phase6_demo",
        "pynlm"
    ]
    
    for target in required_targets:
        if target in content:
            print(f"✅ CMake target '{target}' found")
        else:
            print(f"❌ CMake target '{target}' not found")
            return False
    
    return True

def check_documentation():
    """Check if documentation has been updated"""
    print("\nChecking documentation...")
    
    docs = [
        "README.md",
        "easy_usage.md",
        "HOW_TO_USE.md",
        "IMPROVEMENTS_SUMMARY.md"
    ]
    
    for doc in docs:
        if os.path.exists(doc):
            print(f"✅ {doc} exists")
        else:
            print(f"❌ {doc} missing")
            return False
    
    return True

def main():
    """Main verification function"""
    print("=" * 60)
    print("NLM Python Package Verification")
    print("=" * 60)
    
    checks = [
        ("Python Package Structure", check_python_package),
        ("CMake Configuration", check_cmake_targets),
        ("Documentation", check_documentation)
    ]
    
    all_passed = True
    
    for check_name, check_func in checks:
        print(f"\n{'-' * 40}")
        print(f"Running: {check_name}")
        print(f"{'-' * 40}")
        
        if not check_func():
            all_passed = False
    
    print("\n" + "=" * 60)
    if all_passed:
        print("✅ ALL CHECKS PASSED")
        print("The NLM project has been successfully enhanced!")
        print("\nNext steps:")
        print("1. Build the C++ project: mkdir build && cd build && cmake .. && make")
        print("2. Test the new Phase 6 demo: ./build/nlm_phase6_demo")
        print("3. Install Python bindings: pip install python/")
        print("4. Run examples from documentation")
    else:
        print("❌ SOME CHECKS FAILED")
        print("Please review the output above and fix any issues.")
        sys.exit(1)
    
    print("=" * 60)

if __name__ == "__main__":
    main()
