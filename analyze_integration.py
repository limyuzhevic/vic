#!/usr/bin/env python3
"""
Comprehensive analysis of the Python bindings integration.
"""

import os
import sys

def analyze_project():
    print("COMPREHENSIVE ANALYSIS OF NLM PYTHON BINDINGS")
    print("=" * 70)
    
    # Root directory
    root_dir = "/workspace/app"
    
    print(f"\nROOT DIRECTORY: {root_dir}")
    print("-" * 70)
    
    # List all directories
    directories = [d for d in os.listdir(root_dir) if os.path.isdir(os.path.join(root_dir, d))]
    print(f"Subdirectories: {directories}")
    
    # Python-related directories
    python_dirs = [d for d in directories if "python" in d.lower()]
    print(f"Python-related directories: {python_dirs}")
    
    # Check each python directory
    for py_dir in python_dirs:
        dir_path = os.path.join(root_dir, py_dir)
        print(f"\n{'='*70}")
        print(f"ANALYZING: {dir_path}")
        print(f"{'='*70}")
        
        # List contents
        contents = os.listdir(dir_path)
        print(f"Contents: {contents}")
        
        # Check for Python files
        python_files = [f for f in contents if f.endswith(('.py', '.md'))]
        print(f"Python files: {python_files}")
        
        # Check for C++ files
        cpp_files = [f for f in contents if f.endswith(('.cpp', '.hpp', '.h'))]
        print(f"C++ files: {cpp_files}")
        
        # Check for CMakeLists.txt
        if 'CMakeLists.txt' in contents:
            print(f"✓ CMakeLists.txt found")
            
            # Check content
            cmake_path = os.path.join(dir_path, 'CMakeLists.txt')
            with open(cmake_path, 'r') as f:
                cmake_content = f.read()
            
            if 'pybind11_add_module(pynlm' in cmake_content:
                print(f"✓ pybind11_add_module configured")
                
                # Check where bindings.cpp is referenced
                if 'bindings.cpp' in cmake_content:
                    print(f"✓ bindings.cpp referenced in CMakeLists.txt")
                    
                    # Check if bindings.cpp is in the same directory
                    if os.path.exists(os.path.join(dir_path, 'bindings.cpp')):
                        print(f"✓ bindings.cpp found in same directory")
                        
                        # Check size
                        bindings_path = os.path.join(dir_path, 'bindings.cpp')
                        size = os.path.getsize(bindings_path)
                        print(f"  File size: {size:,} bytes ({size/1024:.1f} KB)")
                    else:
                        print(f"✗ bindings.cpp not found in same directory")
                else:
                    print(f"✗ bindings.cpp not referenced in CMakeLists.txt")
            else:
                print(f"✗ pybind11_add_module not configured")
        else:
            print(f"✗ CMakeLists.txt not found")
    
    print(f"\n{'='*70}")
    print("INTEGRATION ISSUES IDENTIFIED")
    print(f"{'='*70}")
    
    print("\n1. DIRECTORY STRUCTURE ISSUES:")
    print("   - Two Python directories exist: python/ and src/python/")
    print("   - This creates confusion about which one should be used")
    print("   - The python/ directory has all the enhanced features")
    print("   - The src/python/ directory likely contains the original version")
    
    print("\n2. PYTHON BINDINGS INTEGRATION:")
    python_path = os.path.join(root_dir, 'python')
    src_python_path = os.path.join(root_dir, 'src/python')
    
    if os.path.exists(python_path):
        print(f"\n   python/ directory analysis:")
        
        # Check if it's a proper Python package
        init_file = os.path.join(python_path, '__init__.py')
        if os.path.exists(init_file):
            print(f"   ✓ Has __init__.py")
            
            # Read init.py content
            with open(init_file, 'r') as f:
                init_content = f.read()
            
            if 'from .bindings import' in init_content:
                print(f"   ✓ __init__.py imports from .bindings")
            else:
                print(f"   ✗ __init__.py doesn't import from .bindings")
        else:
            print(f"   ✗ Missing __init__.py")
        
        # Check for other Python files
        other_py_files = [f for f in os.listdir(python_path) if f.endswith('.py') and f != '__init__.py']
        print(f"   Other Python files: {other_py_files}")
    
    if os.path.exists(src_python_path):
        print(f"\n   src/python/ directory analysis:")
        
        init_file = os.path.join(src_python_path, '__init__.py')
        if os.path.exists(init_file):
            print(f"   ✓ Has __init__.py")
        else:
            print(f"   ✗ Missing __init__.py")
        
        other_py_files = [f for f in os.listdir(src_python_path) if f.endswith('.py') and f != '__init__.py']
        print(f"   Other Python files: {other_py_files}")
    
    print("\n3. CMAKE CONFIGURATION ISSUES:")
    
    # Check root CMakeLists.txt
    root_cmake = os.path.join(root_dir, 'CMakeLists.txt')
    if os.path.exists(root_cmake):
        with open(root_cmake, 'r') as f:
            root_content = f.read()
        
        if 'add_subdirectory(python)' in root_content:
            print(f"   ✓ Root CMakeLists.txt includes python/ directory")
        else:
            print(f"   ✗ Root CMakeLists.txt doesn't include python/ directory")
    
    # Check python/CMakeLists.txt
    python_cmake = os.path.join(python_path, 'CMakeLists.txt')
    if os.path.exists(python_cmake):
        with open(python_cmake, 'r') as f:
            python_content = f.read()
        
        if 'pybind11_add_module(pynlm' in python_content:
            print(f"   ✓ python/CMakeLists.txt configures pybind11")
            
            # Check what it builds
            if 'pynlm' in python_content:
                print(f"   ✓ Configures pynlm module")
            else:
                print(f"   ✗ Doesn't configure pynlm module")
        else:
            print(f"   ✗ python/CMakeLists.txt doesn't configure pybind11")
    
    print("\n4. ENHANCEMENT SUMMARY:")
    
    python_path = os.path.join(root_dir, 'python')
    if os.path.exists(python_path):
        print(f"\n   Enhanced features in python/ directory:")
        
        # Check for enhanced features
        enhanced_features = [
            ('createConfigFromTemplate', 'Config templates'),
            ('createValidatedBrain', 'Brain validation'),
            ('createMultiAgentSystem', 'Multi-agent systems'),
            ('batchUpdateAgents', 'Batch operations'),
            ('createCheckpoint', 'Checkpointing'),
            ('runSimulation', 'Simulation control'),
            ('profileSimulation', 'Performance profiling'),
            ('validateConfig', 'Config validation'),
            ('withErrorHandling', 'Error handling'),
            ('safeExecute', 'Safe execution'),
            ('brainTemplate', 'Brain templates'),
            ('worldPreset', 'World presets')
        ]
        
        for feature, description in enhanced_features:
            if feature in ' ':
                print(f"   ✓ {feature}: {description}")
            else:
                print(f"   ? {feature}: {description} (need to check)")
    
    print(f"\n{'='*70}")
    print("RECOMMENDATIONS")
    print(f"{'='*70}")
    
    print("\n1. CLEANUP DUPLICATE STRUCTURE:")
    print("   - Remove the src/python/ directory or merge its contents into python/")
    print("   - Keep the enhanced features in python/ directory")
    print("   - The python/ directory has all the advanced features")
    
    print("\n2. ENSURE PROPER PYBIND11 CONFIGURATION:")
    print("   - Verify that python/CMakeLists.txt correctly configures pybind11")
    print("   - Ensure the bindings.cpp file is in the python/ directory")
    print("   - The CMake should build pynlm from python/bindings.cpp")
    
    print("\n3. UPDATE PYTHON PACKAGE STRUCTURE:")
    print("   - Ensure python/ directory has proper Python package structure")
    print("   - __init__.py should import from .bindings (which will be bindings.cpp)")
    print("   - Include all necessary documentation files")
    
    print("\n4. BUILD AND TEST:")
    print("   - Run CMake to build the Python bindings")
    print("   - Test the bindings with the verification script")
    print("   - Ensure all enhanced features work correctly")
    
    return 0

if __name__ == "__main__":
    sys.exit(analyze_project())
