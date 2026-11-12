#!/usr/bin/env python3
"""
NLM Verification Script
=======================

This script verifies that the NLM improvements have been successfully implemented.
It checks the key areas that were identified for improvement.
"""

import os
import sys
import json
import subprocess

def check_file_exists(filepath, description):
    """Check if a file exists and return status."""
    if os.path.exists(filepath):
        print(f"✅ {description}: {filepath}")
        return True
    else:
        print(f"❌ {description}: {filepath} - NOT FOUND")
        return False

def check_python_bindings():
    """Check if Python bindings can be imported."""
    print("\n=== Checking Python Bindings ===")
    
    try:
        import pynlm
        print("✅ Python bindings module can be imported")
        
        # Test basic functionality
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        print("✅ Basic Python API functionality works")
        
        # Test configuration features
        test_file = "verification_config.json"
        config.set("test_key", "test_value")
        config.saveToFile(test_file)
        
        new_config = pynlm.createDefaultConfig()
        new_config.loadFromFile(test_file)
        print("✅ Configuration file I/O works")
        
        # Clean up
        os.remove(test_file)
        return True
        
    except Exception as e:
        print(f"❌ Python bindings check failed: {e}")
        return False

def check_improvements_files():
    """Check that all improvement files exist."""
    print("\n=== Checking Improvement Files ===")
    
    required_files = [
        ("build_nlm.sh", "Comprehensive build and verification script"),
        ("python/pynlm.py.in", "Python module template"),
        ("python/CMakeLists.txt", "Python bindings build system"),
        ("scripts/__init__.py", "Scripts package"),
        ("scripts/demo_basic.py", "Basic usage demo"),
        ("scripts/demo_advanced.py", "Advanced features demo"),
        ("IMPROVEMENTS_SUMMARY.md", "Improvements documentation"),
    ]
    
    all_exist = True
    for filepath, description in required_files:
        if not check_file_exists(filepath, description):
            all_exist = False
    
    return all_exist

def check_config_improvements():
    """Check configuration system improvements."""
    print("\n=== Checking Configuration Improvements ===")
    
    config_file = "configs/default.cfg"
    if not check_file_exists(config_file, "Configuration file"):
        return False
    
    # Check for duplicate keys in config file
    with open(config_file, 'r') as f:
        content = f.read()
        lines = content.strip().split('\n')
    
    keys = {}
    duplicates = []
    
    for line in lines:
        if '=' in line and not line.strip().startswith('#'):
            key = line.split('=')[0].strip()
            if key in keys:
                duplicates.append(key)
            else:
                keys[key] = line
    
    if duplicates:
        print(f"❌ Found duplicate keys in config: {duplicates}")
        return False
    else:
        print("✅ Configuration file has no duplicate keys")
    
    return True

def check_build_system():
    """Check build system improvements."""
    print("\n=== Checking Build System ===")
    
    # Check CMakeLists.txt for Python bindings
    cmake_file = "CMakeLists.txt"
    if not check_file_exists(cmake_file, "Main CMakeLists.txt"):
        return False
    
    with open(cmake_file, 'r') as f:
        cmake_content = f.read()
    
    if "NLM_BUILD_PYTHON_BINDINGS" in cmake_content:
        print("✅ CMakeLists.txt has Python bindings option")
    else:
        print("❌ CMakeLists.txt missing Python bindings option")
        return False
    
    if "pyproject.toml" in cmake_content:
        print("✅ CMakeLists.txt references pyproject.toml")
    else:
        print("ℹ️  CMakeLists.txt doesn't reference pyproject.toml (this is okay)")
    
    return True

def main():
    """Main verification function."""
    print("NLM Project Verification Script")
    print("=" * 50)
    
    checks = [
        ("Improvement Files", check_improvements_files),
        ("Build System", check_build_system),
        ("Configuration Improvements", check_config_improvements),
        ("Python Bindings", check_python_bindings),
    ]
    
    passed = 0
    total = len(checks)
    
    for check_name, check_func in checks:
        print(f"\n{'='*20} {check_name} {'='*20}")
        if check_func():
            passed += 1
            print(f"✅ {check_name} PASSED")
        else:
            print(f"❌ {check_name} FAILED")
    
    print("\n" + "=" * 50)
    print(f"VERIFICATION RESULTS: {passed}/{total} checks passed")
    print("=" * 50)
    
    if passed == total:
        print("\n🎉 ALL VERIFICATION CHECKS PASSED!")
        print("\nThe NLM project improvements have been successfully implemented:")
        print("✅ Python bindings are working")
        print("✅ Configuration system enhanced")
        print("✅ Documentation improved")
        print("✅ Build system enhanced")
        print("✅ All demo scripts available")
        print("\nThe NLM framework is ready for use!")
        return 0
    else:
        print(f"\n❌ {total - passed} verification check(s) failed")
        print("\nPlease review the failed checks above and fix any issues.")
        return 1

if __name__ == "__main__":
    sys.exit(main())
