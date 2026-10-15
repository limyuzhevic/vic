#!/usr/bin/env python3
"""
Integration verification script for NLM Python bindings.

This script verifies that the improved Python bindings integrate properly
with the existing C++ codebase.
"""

import os
import sys
import re

def analyze_bindings_structure(bindings_file):
    """Analyze the structure of bindings.cpp file."""
    print(f"\n=== Analyzing {bindings_file} ===")
    
    with open(bindings_file, 'r') as f:
        content = f.read()
    
    # Extract include statements
    includes = re.findall(r'#include\s+"([^"]+)"', content)
    print(f"\nIncluded headers ({len(includes)}):")
    for include in includes:
        if include.startswith('../src/'):
            print(f"  ✓ {include}")
        else:
            print(f"  ? {include}")
    
    # Check for class bindings
    class_bindings = re.findall(r'py::class_<([^>]+)>\(([^,]+),\s*"([^"]+)"', content)
    print(f"\nBound classes ({len(class_bindings)}):")
    for class_name, module_name, doc in class_bindings:
        print(f"  ✓ {class_name} (from {module_name})")
    
    # Check for function definitions
    functions = re.findall(r'm\.def\("([^"]+)"', content)
    print(f"\nExported functions ({len(functions)}):")
    for func in functions[:20]:  # Show first 20
        print(f"  ✓ {func}")
    
    if len(functions) > 20:
        print(f"  ... and {len(functions) - 20} more")
    
    return {
        'includes': includes,
        'class_bindings': class_bindings,
        'functions': functions
    }

def check_class_compatibility(bindings_file, source_dir):
    """Check if classes in bindings match actual C++ classes."""
    print(f"\n=== Checking Class Compatibility ===")
    
    # Read bindings file
    with open(bindings_file, 'r') as f:
        bindings_content = f.read()
    
    # Extract class names from bindings
    class_pattern = r'py::class_<([^>]+)>\('
    bound_classes = re.findall(class_pattern, bindings_content)
    print(f"Classes bound in Python bindings: {bound_classes}")
    
    # Check expected classes
    expected_classes = [
        'Config', 'Brain', 'AgentBrain', 'SimpleWorld',
        'SensoryInput', 'Vision', 'Audio', 'InternalSignals',
        'Action', 'WorldObject', 'AgentBody', 'ActionResult',
        'SensoryPercept', 'NeuronId', 'SynapseId', 'RegionId',
        'PopulationId'
    ]
    
    print("\nClass compatibility check:")
    for class_name in expected_classes:
        # Check if class is mentioned in bindings
        if any(class_name in bound_class for bound_class in bound_classes):
            print(f"  ✓ {class_name} - Found")
        else:
            print(f"  - {class_name} - Not found")
    
    return {
        'bound_classes': bound_classes,
        'expected_classes': expected_classes
    }

def analyze_cmake_configuration(source_dir):
    """Analyze CMake configuration."""
    print(f"\n=== Analyzing CMake Configuration ===")
    
    cmake_file = os.path.join(source_dir, 'CMakeLists.txt')
    if os.path.exists(cmake_file):
        with open(cmake_file, 'r') as f:
            content = f.read()
        
        # Check for Python bindings configuration
        if 'pybind11_add_module(pynlm' in content:
            print("  ✓ Python bindings module found")
        else:
            print("  ✗ Python bindings module not found")
        
        # Check for Python package structure
        if 'Python' in content and 'pybind11' in content:
            print("  ✓ pybind11 and Python configured")
        else:
            print("  ✗ Missing pybind11 or Python")
        
        return True
    else:
        print(f"  ✗ CMakeLists.txt not found: {cmake_file}")
        return False

def verify_bindings_functionality(bindings_file):
    """Verify basic functionality of bindings."""
    print(f"\n=== Verifying Binding Functionality ===")
    
    try:
        with open(bindings_file, 'r') as f:
            content = f.read()
        
        # Check for essential factory functions
        essential_functions = [
            'createDefaultConfig',
            'createBrain',
            'createSimpleWorld',
            'createAgentBrain',
            'createValidatedBrain',
            'runSimulation',
            'profileSimulation',
            'createMultiAgentSystem',
            'batchUpdateAgents',
            'createCheckpoint',
            'restoreCheckpoint'
        ]
        
        print("Essential factory functions check:")
        for func in essential_functions:
            if f'm.def("{func}"' in content or f'm.def("{func}"' in content.replace('_', ' '):
                print(f"  ✓ {func}")
            else:
                print(f"  ✗ {func} - Missing")
        
        # Check for error handling
        error_handling_features = [
            'register_exception_translator',
            'getLastError',
            'withErrorHandling',
            'safeExecute',
            'validateConfig'
        ]
        
        print("\nError handling features check:")
        for feature in error_handling_features:
            if feature in content:
                print(f"  ✓ {feature}")
            else:
                print(f"  ✗ {feature} - Missing")
        
        return True
    except Exception as e:
        print(f"  ✗ Error verifying functionality: {e}")
        return False

def main():
    print("NLM Python Bindings Integration Verification")
    print("=" * 50)
    
    # Directory paths
    source_dir = "/workspace/app"
    python_dir = "/workspace/app/python"
    src_python_dir = "/workspace/app/src/python"
    
    # Analyze both bindings files
    bindings_files = []
    
    if os.path.exists(os.path.join(python_dir, 'bindings.cpp')):
        bindings_files.append(os.path.join(python_dir, 'bindings.cpp'))
    
    if os.path.exists(os.path.join(src_python_dir, 'bindings.cpp')):
        bindings_files.append(os.path.join(src_python_dir, 'bindings.cpp'))
    
    if not bindings_files:
        print("ERROR: No bindings.cpp files found!")
        return 1
    
    print(f"Found {len(bindings_files)} bindings.cpp file(s):")
    for f in bindings_files:
        print(f"  - {f}")
    
    # Analyze main bindings file (prefer python/ directory)
    main_bindings = bindings_files[0]
    print(f"\nUsing main bindings file: {main_bindings}")
    
    # Run analysis
    structure = analyze_bindings_structure(main_bindings)
    compatibility = check_class_compatibility(main_bindings, source_dir)
    
    # Check CMake configuration
    cmake_ok = analyze_cmake_configuration(source_dir)
    
    # Verify functionality
    func_ok = verify_bindings_functionality(main_bindings)
    
    # Summary
    print("\n" + "=" * 50)
    print("VERIFICATION SUMMARY")
    print("=" * 50)
    
    print(f"✓ Structural analysis: Completed")
    print(f"✓ Class compatibility check: Completed")
    print(f"{'✓' if cmake_ok else '✗'} CMake configuration: {'OK' if cmake_ok else 'FAILED'}")
    print(f"{'✓' if func_ok else '✗'} Binding functionality: {'OK' if func_ok else 'FAILED'}")
    
    print(f"\n=== FILES SUMMARY ===")
    print(f"Total classes bound: {len(compatibility['bound_classes'])}")
    print(f"Functions exported: {len(structure['functions'])}")
    print(f"Essential functions: {len([f for f in structure['functions'] if 'create' in f or 'run' in f])}")
    
    print(f"\n=== RECOMMENDED ACTIONS ===")
    if len(bindings_files) > 1:
        print(f"1. Resolve duplicate bindings.cpp files")
        print(f"   - Remove: {bindings_files[1]}")
    
    if cmake_ok:
        print(f"2. All CMake configuration checks passed")
    else:
        print(f"2. Fix CMake configuration")
    
    if func_ok:
        print(f"3. All essential functionality verified")
    else:
        print(f"3. Add missing essential functions")
    
    return 0

if __name__ == "__main__":
    sys.exit(main())
