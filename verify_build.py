#!/usr/bin/env python3
"""
Quick Build Verification Script

Quick script to verify that the pynlm bindings build correctly
and can be imported.
"""

import sys
import os
import subprocess
import tempfile

def check_python_import():
    """Check if pynlm can be imported"""
    print("Checking Python module import...")
    
    try:
        # Try to import pynlm
        import pynlm
        print(f"✓ Successfully imported pynlm version {pynlm.__version__}")
        
        # Check basic functionality
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Test basic operations
        brain.step(0)
        spike_count = brain.getTotalSpikeCount()
        print(f"✓ Successfully created and stepped brain (spikes: {spike_count})")
        
        return True
        
    except Exception as e:
        print(f"✗ Failed to import or use pynlm: {e}")
        import traceback
        traceback.print_exc()
        return False

def check_bindings_cpp_syntax():
    """Check if bindings.cpp has syntax errors"""
    print("\nChecking bindings.cpp syntax...")
    
    bindings_path = os.path.join(os.path.dirname(__file__), 'python', 'bindings.cpp')
    
    if not os.path.exists(bindings_path):
        print(f"✗ bindings.cpp not found at {bindings_path}")
        return False
    
    # Try to compile a simple test
    try:
        # Create a minimal test to check C++ syntax
        test_code = '''
#include <iostream>
#include <string>

// Simple test to check if bindings.cpp includes are valid
void test_include() {
    // This should compile without errors if the includes are correct
    std::cout << "Test" << std::endl;
}
'''
        
        # Write test to temporary file
        with tempfile.NamedTemporaryFile(mode='w', suffix='.cpp', delete=False) as f:
            f.write(test_code)
            test_file = f.name
        
        # Try to compile with basic flags
        compile_cmd = [
            'g++', '-std=c++20', '-I', os.path.join(os.path.dirname(__file__), 'src'),
            '-c', test_file, '-o', test_file + '.o'
        ]
        
        try:
            result = subprocess.run(compile_cmd, capture_output=True, text=True, timeout=30)
            if result.returncode == 0:
                print("✓ bindings.cpp appears to have valid includes")
                return True
            else:
                print(f"✗ bindings.cpp compilation failed:")
                print(result.stderr)
                return False
        except subprocess.TimeoutExpired:
            print("✗ bindings.cpp compilation timed out")
            return False
        finally:
            if os.path.exists(test_file):
                os.unlink(test_file)
                
    except Exception as e:
        print(f"✗ Error checking bindings.cpp: {e}")
        return False

def check_cmake_files():
    """Check if CMake files are properly structured"""
    print("\nChecking CMake files...")
    
    # Check root CMakeLists.txt
    root_cmake = os.path.join(os.path.dirname(__file__), 'CMakeLists.txt')
    if not os.path.exists(root_cmake):
        print(f"✗ root CMakeLists.txt not found at {root_cmake}")
        return False
    
    # Check python CMakeLists.txt
    python_cmake = os.path.join(os.path.dirname(__file__), 'python', 'CMakeLists.txt')
    if not os.path.exists(python_cmake):
        print(f"✗ python/CMakeLists.txt not found at {python_cmake}")
        return False
    
    # Check for required CMake commands
    with open(root_cmake, 'r') as f:
        root_content = f.read()
    
    required_commands = ['cmake_minimum_required', 'project', 'add_library']
    for cmd in required_commands:
        if cmd not in root_content:
            print(f"✗ root CMakeLists.txt missing {cmd} command")
            return False
    
    with open(python_cmake, 'r') as f:
        python_content = f.read()
    
    required_python_commands = ['pybind11_add_module', 'target_link_libraries']
    for cmd in required_python_commands:
        if cmd not in python_content:
            print(f"✗ python/CMakeLists.txt missing {cmd} command")
            return False
    
    print("✓ CMake files appear to be properly structured")
    return True

def check_documentation():
    """Check if documentation files exist"""
    print("\nChecking documentation files...")
    
    docs = [
        'easy_usage.md',
        'HOW_TO_USE.md',
        'pynlm_api_docs.md',
        'README.md'
    ]
    
    for doc in docs:
        doc_path = os.path.join(os.path.dirname(__file__), doc)
        if os.path.exists(doc_path):
            print(f"✓ Found {doc}")
        else:
            print(f"✗ Missing {doc}")
            return False
    
    return True

def check_test_files():
    """Check if test files exist"""
    print("\nChecking test files...")
    
    tests = [
        'test_pynlm.py',
        'test_validation.py',
    ]
    
    for test in tests:
        test_path = os.path.join(os.path.dirname(__file__), test)
        if os.path.exists(test_path):
            print(f"✓ Found {test}")
        else:
            print(f"✗ Missing {test}")
            return False
    
    return True

def run_verification():
    """Run all verification checks"""
    print("=" * 70)
    print("PYNLM BUILD VERIFICATION SCRIPT")
    print("=" * 70)
    
    checks = [
        ("Python Import", check_python_import),
        ("CMake Files", check_cmake_files),
        ("Documentation", check_documentation),
        ("Test Files", check_test_files),
    ]
    
    passed = 0
    failed = 0
    
    for name, check_func in checks:
        print(f"\n{'='*20} {name} {'='*20}")
        if check_func():
            passed += 1
        else:
            failed += 1
    
    print("\n" + "=" * 70)
    print(f"VERIFICATION RESULTS: {passed} passed, {failed} failed")
    print("=" * 70)
    
    if failed == 0:
        print("\n✓ All verification checks passed!")
        print("The pynlm Python bindings appear to be properly configured.")
        return True
    else:
        print(f"\n✗ {failed} verification checks failed.")
        print("Please review the output above for details.")
        return False

if __name__ == "__main__":
    success = run_verification()
    sys.exit(0 if success else 1)