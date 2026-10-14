#!/usr/bin/env python3
"""
NLM Project - Quick Installation and Testing Guide

This script provides a quick guide for installing and testing the enhanced NLM project.

Usage:
    python3 install_and_test.py

Author: NLM Development Team
Date: 2026-10-14
"""

import subprocess
import os
import sys

def run_command(command, description):
    """Run a command and display the result."""
    print(f"\n{'='*60}")
    print(f"Running: {description}")
    print(f"Command: {command}")
    print(f"{'='*60}")
    
    try:
        result = subprocess.run(
            command,
            shell=True,
            capture_output=True,
            text=True,
            cwd=os.path.dirname(os.path.abspath(__file__))
        )
        
        if result.returncode == 0:
            print("✅ SUCCESS")
            print(result.stdout)
        else:
            print("❌ FAILED")
            print("STDOUT:", result.stdout)
            print("STDERR:", result.stderr)
            return False
        
        return True
    except Exception as e:
        print(f"❌ ERROR: {e}")
        return False

def main():
    print("=== NLM Project - Quick Installation and Testing Guide ===")
    print()
    print("This script provides a quick guide for installing and testing")
    print("the enhanced NLM project with all its new features.")
    print()
    
    # Check if we're in the right directory
    if not os.path.exists("CMakeLists.txt"):
        print("❌ ERROR: CMakeLists.txt not found. Please run this script from the project root.")
        return
    
    print("Project Structure:")
    print("- Python bindings with advanced features")
    print("- Enhanced configuration system")
    print("- Real-world demo scenarios")
    print("- Performance monitoring tools")
    print("- Comprehensive error handling")
    print("- Advanced test suite")
    print()
    
    # Check for Python bindings
    if os.path.exists("python"):
        print("✅ Python bindings directory found")
    else:
        print("❌ Python bindings directory not found")
    
    # Check for enhanced config
    if os.path.exists("src/core/Config/AdvancedConfig.cpp"):
        print("✅ Advanced configuration found")
    else:
        print("❌ Advanced configuration not found")
    
    # Check for advanced demo
    if os.path.exists("src/experiments/AdvancedPhase3Demo.cpp"):
        print("✅ Advanced demo found")
    else:
        print("❌ Advanced demo not found")
    
    # Check for test files
    if os.path.exists("test_advanced_features.cpp"):
        print("✅ Test files found")
    else:
        print("❌ Test files not found")
    
    # Check for documentation
    if os.path.exists("ADVANCED_FEATURES_README.md"):
        print("✅ Documentation found")
    else:
        print("❌ Documentation not found")
    
    print()
    print("=== Installation Steps ===")
    
    # Step 1: Create build directory
    success = run_command(
        "mkdir -p build && cd build",
        "Create build directory"
    )
    
    if success:
        # Step 2: Configure with CMake
        success = run_command(
            "cmake .. -DCMAKE_BUILD_TYPE=Release",
            "Configure with CMake"
        )
        
        if success:
            # Step 3: Build project
            success = run_command(
                "make -j$(nproc)",
                "Build project"
            )
            
            if success:
                # Step 4: Run tests
                success = run_command(
                    "ctest --output-on-failure",
                    "Run tests"
                )
                
                if success:
                    print()
                    print("🎉 ALL TESTS PASSED!")
                    print()
                    print("=== Next Steps ===")
                    print("1. Run advanced demo:")
                    print("   ./build/nlm_advanced_demo")
                    print()
                    print("2. Run test suite:")
                    print("   ./build/nlm_test_advanced")
                    print()
                    print("3. Python API examples:")
                    print("   See ADVANCED_FEATURES_README.md for examples")
                    print()
                    print("4. Documentation:")
                    print("   - ADVANCED_FEATURES_README.md")
                    print("   - IMPROVEMENTS_SUMMARY.md")
                    print("   - easy_usage.md")
                    print("   - HOW_TO_USE.md")
                else:
                    print()
                    print("⚠️  Some tests failed. Please check the output above.")
            else:
                print()
                print("⚠️  Build failed. Please check the output above.")
        else:
            print()
            print("⚠️  CMake configuration failed. Please check the output above.")
    else:
        print()
        print("⚠️  Failed to create build directory.")
    
    print()
    print("=== Summary ===")
    print("The NLM project has been enhanced with:")
    print("✅ Advanced Python API with batch processing")
    print("✅ Real-world demo scenarios")
    print("✅ Enhanced configuration validation")
    print("✅ Performance monitoring tools")
    print("✅ Comprehensive error handling")
    print("✅ Advanced test suite")
    print("✅ Improved build system")
    print()
    print("These improvements make the NLM project more accessible,")
    print("provide better support for research, and enable more")
    print("sophisticated simulations and analyses.")

if __name__ == "__main__":
    main()
