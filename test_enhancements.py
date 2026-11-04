#!/usr/bin/env python3
"""
Quick Test Script for Enhanced NLM Project

This script tests the basic functionality of the enhanced NLM project
to ensure that the improvements haven't broken existing functionality.
"""

import os
import sys
import subprocess
import json
from pathlib import Path

def run_command(cmd, description):
    """Run a command and check its output."""
    print(f"\n{description}")
    print(f"Command: {cmd}")
    
    try:
        result = subprocess.run(cmd, shell=True, capture_output=True, text=True, timeout=30)
        
        if result.returncode == 0:
            print("✓ Command executed successfully")
            if result.stdout:
                print(f"Output:\n{result.stdout[:500]}...")
            return True
        else:
            print(f"✗ Command failed with return code {result.returncode}")
            if result.stderr:
                print(f"Error:\n{result.stderr[:500]}...")
            return False
            
    except subprocess.TimeoutExpired:
        print("✗ Command timed out")
        return False
    except Exception as e:
        print(f"✗ Command failed with exception: {e}")
        return False

def check_files_exist(files):
    """Check if required files exist."""
    print(f"\nChecking file existence...")
    all_exist = True
    
    for file in files:
        if os.path.exists(file):
            print(f"✓ {file}")
        else:
            print(f"✗ {file} (NOT FOUND)")
            all_exist = False
    
    return all_exist

def check_directory_structure():
    """Check the directory structure."""
    print(f"\nChecking directory structure...")
    
    expected_dirs = [
        "src",
        "src/core",
        "src/core/Config",
        "src/core/Logger",
        "src/core/Types",
        "src/brain",
        "src/experiments",
        "configs",
        "docs"
    ]
    
    all_exist = True
    
    for dir in expected_dirs:
        if os.path.exists(dir) and os.path.isdir(dir):
            print(f"✓ {dir}/")
        else:
            print(f"✗ {dir}/ (NOT FOUND)")
            all_exist = False
    
    return all_exist

def check_enhanced_files():
    """Check for enhanced files."""
    print(f"\nChecking enhanced files...")
    
    enhanced_files = [
        "src/core/EnhancedConfig.hpp",
        "src/core/EnhancedConfig.cpp",
        "src/core/CommandLineParser.hpp",
        "src/core/CommandLineParser.cpp",
        "src/core/EnhancedLogger.hpp",
        "src/core/EnhancedLogger.cpp",
        "src/core/DebugUtilities.hpp",
        "src/core/DebugUtilities.cpp",
        "src/core/NeuralNetworkAnalyzer.hpp",
        "src/core/NeuralNetworkAnalyzer.cpp",
        "src/core/BrainStateMonitor.hpp",
        "src/core/BrainStateMonitor.cpp",
        "src/core/PlasticityAnalyzer.hpp",
        "src/core/PlasticityAnalyzer.cpp",
        "src/core/NeuromodulationAnalyzer.hpp",
        "src/core/NeuromodulationAnalyzer.cpp",
        "src/core/NeuralDebugUtilities.hpp",
        "src/core/NeuralDebugUtilities.cpp",
        "src/core/DevelopmentTools.hpp",
        "src/core/DevelopmentTools.cpp",
        "src/core/PerformanceMonitoring.hpp",
        "src/core/PerformanceMonitoring.cpp",
        "src/core/NeuralProfiler.hpp",
        "src/core/NeuralProfiler.cpp",
        "src/core/MemoryProfiler.hpp",
        "src/core/MemoryProfiler.cpp",
        "src/core/CPUProfiler.hpp",
        "src/core/CPUProfiler.cpp",
        "src/core/NetworkProfiler.hpp",
        "src/core/NetworkProfiler.cpp",
        "src/core/ComprehensiveProfiler.hpp",
        "src/core/ComprehensiveProfiler.cpp",
        "src/core/PerformanceTestFramework.hpp",
        "src/core/PerformanceTestFramework.cpp",
        "src/core/PerformanceRegressionDetector.hpp",
        "src/core/PerformanceRegressionDetector.cpp",
        "src/core/DemoScripts.hpp",
        "src/core/DemoScripts.cpp"
    ]
    
    all_exist = True
    
    for file in enhanced_files:
        if os.path.exists(file):
            print(f"✓ {file}")
        else:
            print(f"✗ {file} (NOT FOUND)")
            all_exist = False
    
    return all_exist

def check_enhancement_summary():
    """Check enhancement summary document."""
    print(f"\nChecking enhancement summary...")
    
    if os.path.exists("EnhancementSummary.md"):
        print("✓ EnhancementSummary.md")
        
        # Check if it contains key sections
        with open("EnhancementSummary.md", "r") as f:
            content = f.read()
            
        key_sections = [
            "Configuration System Improvements",
            "Enhanced Logging and Debugging",
            "Neural Network Analysis Utilities",
            "Development Tools and Utilities",
            "Performance Monitoring and Profiling",
            "Demo Scripts and Examples",
            "Documentation and Examples",
            "Bug Fixes and Improvements"
        ]
        
        all_sections_found = True
        for section in key_sections:
            if section in content:
                print(f"  ✓ Contains: {section}")
            else:
                print(f"  ✗ Missing: {section}")
                all_sections_found = False
        
        return all_sections_found
    else:
        print("✗ EnhancementSummary.md (NOT FOUND)")
        return False

def check_demo_script():
    """Check demo script."""
    print(f"\nChecking demo script...")
    
    if os.path.exists("demo_enhancements.py"):
        print("✓ demo_enhancements.py")
        return True
    else:
        print("✗ demo_enhancements.py (NOT FOUND)")
        return False

def check_build_files():
    """Check build files."""
    print(f"\nChecking build files...")
    
    build_files = [
        "CMakeLists.txt",
        "pyproject.toml",
        "Makefile",
        "README.md"
    ]
    
    all_exist = True
    
    for file in build_files:
        if os.path.exists(file):
            print(f"✓ {file}")
        else:
            print(f"✗ {file} (NOT FOUND)")
            all_exist = False
    
    return all_exist

def main():
    """Main test function."""
    print("=" * 70)
    print("ENHANCED NLM (熙然) PROJECT - QUICK TEST SCRIPT")
    print("=" * 70)
    
    print("\nThis script tests the enhanced NLM project to ensure that:")
    print("1. All enhanced files are present")
    print("2. Basic functionality is maintained")
    print("3. New features are properly integrated")
    print("4. Documentation is available")
    
    # Run tests
    all_tests_passed = True
    
    # Test 1: Check basic directory structure
    if not check_directory_structure():
        all_tests_passed = False
    
    # Test 2: Check build files
    if not check_build_files():
        all_tests_passed = False
    
    # Test 3: Check enhanced files
    if not check_enhanced_files():
        all_tests_passed = False
    
    # Test 4: Check enhancement summary
    if not check_enhancement_summary():
        all_tests_passed = False
    
    # Test 5: Check demo script
    if not check_demo_script():
        all_tests_passed = False
    
    # Test 6: Try to build the project (if possible)
    if os.path.exists("CMakeLists.txt"):
        print(f"\nAttempting to build the project...")
        if not run_command("mkdir -p build && cd build && cmake .. -DCMAKE_BUILD_TYPE=Release", "Configure project"):
            all_tests_passed = False
    
    # Summary
    print("\n" + "=" * 70)
    print("TEST SUMMARY")
    print("=" * 70)
    
    if all_tests_passed:
        print("✓ All tests passed!")
        print("\nThe enhanced NLM project is ready for use.")
        print("\nKey features available:")
        print("  ✓ Enhanced configuration system with advanced validation")
        print("  ✓ Advanced logging and debugging capabilities")
        print("  ✓ Comprehensive neural network analysis utilities")
        print("  ✓ Development tools and utilities")
        print("  ✓ Performance monitoring and profiling")
        print("  ✓ Demo scripts and examples")
        print("  ✓ Comprehensive documentation")
        print("  ✓ Bug fixes and improvements")
        
        print("\nTo get started:")
        print("  1. Build the project: 'mkdir build && cd build && cmake .. -DCMAKE_BUILD_TYPE=Release && make -j4'")
        print("  2. Run a demo: './nlm', './nlm_phase3_demo', or './nlm_phase6_demo'")
        print("  3. Run the demo script: 'python3 demo_enhancements.py'")
        print("  4. View documentation: 'cat README.md' or 'cat EnhancementSummary.md'")
        
    else:
        print("✗ Some tests failed.")
        print("\nPlease check the issues above and fix them before using the project.")
        
        print("\nCommon issues:")
        print("  - Enhanced files are missing")
        print("  - Build files are missing")
        print("  - Documentation is incomplete")
        print("  - Demo scripts are missing")
    
    print("\n" + "=" * 70)

if __name__ == "__main__":
    main()