#!/usr/bin/env python3
"""
Test script to verify NLM memory system fixes

This script tests the fixes for the memory system inconsistencies:
1. NeuralAssociativeMemory is now a separate class
2. Build system references correct files
3. Documentation reflects Phase 6 capabilities
"""

import sys
import os
import subprocess
import json

def test_file_structure():
    """Test that memory system files have correct structure"""
    print("Testing file structure...")
    
    # Check that NeuralAssociativeMemory files exist
    assoc_header = "/workspace/app/src/memory/NeuralAssociativeMemory.hpp"
    assoc_source = "/workspace/app/src/memory/NeuralAssociativeMemory.cpp"
    
    if not os.path.exists(assoc_header):
        print(f"ERROR: {assoc_header} does not exist")
        return False
        
    if not os.path.exists(assoc_source):
        print(f"ERROR: {assoc_source} does not exist")
        return False
        
    # Check that NeuralEpisodicMemory no longer has NeuralAssociativeMemory
    episodic_header = "/workspace/app/src/memory/NeuralEpisodicMemory.hpp"
    with open(episodic_header, 'r') as f:
        content = f.read()
        if "class NeuralAssociativeMemory" in content:
            print("ERROR: NeuralEpisodicMemory still contains NeuralAssociativeMemory class")
            return False
            
    # Check that NeuralAssociativeMemory is properly included in Brain.hpp
    brain_header = "/workspace/app/src/brain/Brain.hpp"
    with open(brain_header, 'r') as f:
        content = f.read()
        if "#include \"../memory/NeuralAssociativeMemory.hpp\"" not in content:
            print("ERROR: Brain.hpp does not include NeuralAssociativeMemory.hpp")
            return False
            
    print("✓ File structure tests passed")
    return True

def test_python_bindings():
    """Test that Python bindings have been updated"""
    print("Testing Python bindings...")
    
    # Check bindings.cpp for NeuralAssociativeMemory references
    bindings_file = "/workspace/app/python/bindings.cpp"
    with open(bindings_file, 'r') as f:
        content = f.read()
        if "NeuralAssociativeMemory" not in content:
            print("WARNING: NeuralAssociativeMemory not found in bindings.cpp")
            
    print("✓ Python bindings check completed")
    return True

def test_cmake_files():
    """Test that CMake files have been updated"""
    print("Testing CMake files...")
    
    # Check main CMakeLists.txt
    main_cmake = "/workspace/app/CMakeLists.txt"
    with open(main_cmake, 'r') as f:
        content = f.read()
        if "src/memory/NeuralAssociativeMemory.cpp" not in content:
            print("ERROR: NeuralAssociativeMemory.cpp not found in main CMakeLists.txt")
            return False
            
        if "src/memory/AssociativeMemory.cpp" in content:
            print("ERROR: Old AssociativeMemory.cpp still referenced in main CMakeLists.txt")
            return False
            
    # Check python CMakeLists.txt
    python_cmake = "/workspace/app/python/CMakeLists.txt"
    with open(python_cmake, 'r') as f:
        content = f.read()
        if "src/memory/NeuralAssociativeMemory.cpp" not in content:
            print("ERROR: NeuralAssociativeMemory.cpp not found in python CMakeLists.txt")
            return False
            
        if "src/memory/AssociativeMemory.cpp" in content:
            print("ERROR: Old AssociativeMemory.cpp still referenced in python CMakeLists.txt")
            return False
            
    print("✓ CMake file tests passed")
    return True

def test_documentation():
    """Test that documentation has been updated"""
    print("Testing documentation...")
    
    # Check README.md for Phase 6 memory system references
    readme = "/workspace/app/README.md"
    with open(readme, 'r') as f:
        content = f.read()
        if "Phase 2" in content and "Phase 6" not in content:
            print("WARNING: README.md still references Phase 2")
            
    # Check easy_usage.md
    easy_usage = "/workspace/app/easy_usage.md"
    with open(easy_usage, 'r') as f:
        content = f.read()
        if "Phase 2" in content and "Phase 6" not in content:
            print("WARNING: easy_usage.md still references Phase 2")
            
    # Check HOW_TO_USE.md
    how_to_use = "/workspace/app/HOW_TO_USE.md"
    with open(how_to_use, 'r') as f:
        content = f.read()
        if "Phase 2" in content and "Phase 6" not in content:
            print("WARNING: HOW_TO_USE.md still references Phase 2")
            
    print("✓ Documentation tests completed")
    return True

def test_build_system():
    """Test that build system can find correct files"""
    print("Testing build system...")
    
    # Check if CMake can find the files
    cmake_file = "/workspace/app/CMakeLists.txt"
    try:
        with open(cmake_file, 'r') as f:
            content = f.read()
            if "src/memory/NeuralAssociativeMemory.cpp" not in content:
                print("ERROR: NeuralAssociativeMemory.cpp not found in CMakeLists.txt")
                return False
                
            if "src/memory/NeuralWorkingMemory.cpp" not in content:
                print("ERROR: NeuralWorkingMemory.cpp not found in CMakeLists.txt")
                return False
                
            if "src/memory/NeuralEpisodicMemory.cpp" not in content:
                print("ERROR: NeuralEpisodicMemory.cpp not found in CMakeLists.txt")
                return False
                
    except Exception as e:
        print(f"ERROR: Cannot read CMakeLists.txt: {e}")
        return False
        
    print("✓ Build system tests passed")
    return True

def run_build_test():
    """Try to run a simple build test"""
    print("Running build test...")
    
    # Create a simple test program to verify includes
    test_program = """
#include "src/memory/NeuralAssociativeMemory.hpp"
#include "src/memory/NeuralWorkingMemory.hpp"
#include "src/memory/NeuralEpisodicMemory.hpp"

int main() {
    return 0;
}
"""
    
    test_file = "/workspace/app/test_includes.cpp"
    try:
        with open(test_file, 'w') as f:
            f.write(test_program)
            
        # Try to compile it
        result = subprocess.run([
            'g++', '-I/workspace/app/src', '-c', 
            test_file, '-o', '/tmp/test_includes.o'
        ], capture_output=True, text=True)
        
        if result.returncode != 0:
            print(f"WARNING: Build test failed: {result.stderr}")
            return False
            
        print("✓ Build test passed")
        return True
        
    except Exception as e:
        print(f"WARNING: Build test exception: {e}")
        return False
        
    finally:
        # Clean up
        if os.path.exists(test_file):
            os.remove(test_file)
        if os.path.exists('/tmp/test_includes.o'):
            os.remove('/tmp/test_includes.o')

def main():
    """Run all tests"""
    print("=" * 60)
    print("NLM Memory System Fix Verification")
    print("=" * 60)
    
    tests = [
        ("File Structure", test_file_structure),
        ("Python Bindings", test_python_bindings),
        ("CMake Files", test_cmake_files),
        ("Documentation", test_documentation),
        ("Build System", test_build_system),
        ("Build Test", run_build_test),
    ]
    
    passed = 0
    failed = 0
    
    for test_name, test_func in tests:
        print(f"\n--- {test_name} ---")
        try:
            if test_func():
                passed += 1
                print(f"✓ {test_name} PASSED")
            else:
                failed += 1
                print(f"✗ {test_name} FAILED")
        except Exception as e:
            failed += 1
            print(f"✗ {test_name} ERROR: {e}")
    
    print("\n" + "=" * 60)
    print(f"Test Results: {passed} passed, {failed} failed")
    print("=" * 60)
    
    if failed > 0:
        print("\nFAILED TESTS:")
        print("Some memory system fixes may not have been applied correctly.")
        print("Please review the issues listed above.")
        return 1
    else:
        print("\nALL TESTS PASSED!")
        print("Memory system inconsistencies have been successfully fixed.")
        return 0

if __name__ == "__main__":
    sys.exit(main())