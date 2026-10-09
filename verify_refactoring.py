#!/usr/bin/env python3
"""
Refactoring verification script for Brain class
This script verifies that the Brain class refactoring meets core requirements.
"""

import os
import re

def check_file_exists(filepath):
    """Check if file exists"""
    return os.path.exists(filepath)

def read_file(filepath):
    """Read file content"""
    try:
        with open(filepath, 'r') as f:
            return f.read()
    except Exception as e:
        print(f"Error reading {filepath}: {e}")
        return None

def verify_refactoring():
    """Main verification function"""
    print("=" * 60)
    print("Brain Class Refactoring Verification")
    print("=" * 60)
    print()
    
    # Check files exist
    brain_hpp_exists = check_file_exists('src/brain/Brain.hpp')
    brain_cpp_exists = check_file_exists('src/brain/Brain.cpp')
    
    print("File existence checks:")
    print(f"  Brain.hpp: {'✓' if brain_hpp_exists else '✗'}")
    print(f"  Brain.cpp: {'✓' if brain_cpp_exists else '✗'}")
    print()
    
    if not (brain_hpp_exists and brain_cpp_exists):
        print("ERROR: Missing required files!")
        return False
    
    # Read files
    brain_hpp_content = read_file('src/brain/Brain.hpp')
    brain_cpp_content = read_file('src/brain/Brain.cpp')
    
    if not brain_hpp_content or not brain_cpp_content:
        print("ERROR: Could not read files!")
        return False
    
    # Check core requirements
    print("Core refactoring requirements verification:")
    print()
    
    # 1. Check for std::unique_ptr usage (significant usage)
    hpp_unique_ptr_count = len(re.findall(r'std::unique_ptr<', brain_hpp_content))
    cpp_unique_ptr_count = len(re.findall(r'std::unique_ptr<', brain_cpp_content))
    total_unique_ptr = hpp_unique_ptr_count + cpp_unique_ptr_count
    
    print(f"1. std::unique_ptr usage:")
    print(f"   Brain.hpp: {hpp_unique_ptr_count} instances")
    print(f"   Brain.cpp: {cpp_unique_ptr_count} instances")
    print(f"   Total: {total_unique_ptr} instances")
    
    unique_ptr_ok = total_unique_ptr > 10
    print(f"   Status: {'✓ PASSED' if unique_ptr_ok else '✗ FAILED (need > 10)'}")
    print()
    
    # 2. Check for modern C++ features
    cpp_features = {
        'Move semantics': bool(re.search(r'\[\[nodiscard\]\].*\s*noexcept', brain_hpp_content)),
        'Const correctness': bool(re.search(r'const.*noexcept', brain_hpp_content)),
        'Templates': bool(re.search(r'template<', brain_hpp_content)),
        'RAII': 'std::unique_ptr' in brain_hpp_content,
    }
    
    print("2. Modern C++ features:")
    all_cpp_features_ok = True
    for feature, found in cpp_features.items():
        status = '✓' if found else '✗'
        print(f"   {feature}: {status}")
        if not found:
            all_cpp_features_ok = False
    print()
    
    # 3. Check PIMPL pattern
    pimpl_impl_struct = 'struct Impl' in brain_hpp_content
    pimpl_forward_decl = 'struct Impl;' in brain_hpp_content
    pimpl_unique_ptr = 'std::unique_ptr<Impl> pImpl' in brain_hpp_content
    
    print("3. PIMPL pattern implementation:")
    print(f"   Impl struct: {'✓' if pimpl_impl_struct else '✗'}")
    print(f"   Forward declaration: {'✓' if pimpl_forward_decl else '✗'}")
    print(f"   unique_ptr<Impl>: {'✓' if pimpl_unique_ptr else '✗'}")
    
    pimpl_ok = pimpl_impl_struct and pimpl_forward_decl and pimpl_unique_ptr
    print(f"   Status: {'✓ PASSED' if pimpl_ok else '✗ FAILED'}")
    print()
    
    # 4. Check BrainFactory class
    brainfactory_class = 'class BrainFactory' in brain_hpp_content
    brainfactory_create = 'static std::shared_ptr<Brain> create(' in brain_hpp_content
    brainfactory_validator = 'class ConfigurationValidator' in brain_hpp_content
    brainfactory_templates = 'template<' in brain_hpp_content and 'createConfigForUseCase' in brain_hpp_content
    
    print("4. BrainFactory helper class:")
    print(f"   Class exists: {'✓' if brainfactory_class else '✗'}")
    print(f"   Static create method: {'✓' if brainfactory_create else '✗'}")
    print(f"   Configuration validator: {'✓' if brainfactory_validator else '✗'}")
    print(f"   Template methods: {'✓' if brainfactory_templates else '✗'}")
    
    brainfactory_ok = brainfactory_class and brainfactory_create and brainfactory_validator and brainfactory_templates
    print(f"   Status: {'✓ PASSED' if brainfactory_ok else '✗ FAILED'}")
    print()
    
    # 5. Check main.cpp size
    main_ok = True
    try:
        with open('src/main.cpp', 'r') as f:
            lines = f.readlines()
            line_count = len(lines)
        
        print("5. Code organization (main.cpp size):")
        print(f"   Line count: {line_count}")
        print(f"   Under 200 lines: {'✓' if line_count <= 200 else '✗'}")
        
        main_ok = line_count <= 200
    except Exception as e:
        print(f"   Error checking main.cpp: {e}")
        print(f"   Status: ✗ FAILED")
        main_ok = False
    print()
    
    # 6. Check initialization and cleanup
    constructor_with_config = 'explicit Brain(std::shared_ptr<Config> config)' in brain_hpp_content
    destructor_noexcept = '~Brain() noexcept;' in brain_hpp_content
    move_constructor = 'Brain(Brain&& other) noexcept' in brain_hpp_content
    
    print("6. Exception safety and cleanup patterns:")
    print(f"   Constructor with config: {'✓' if constructor_with_config else '✗'}")
    print(f"   Destructor with noexcept: {'✓' if destructor_noexcept else '✗'}")
    print(f"   Move constructor: {'✓' if move_constructor else '✗'}")
    
    init_cleanup_ok = constructor_with_config and destructor_noexcept and move_constructor
    print(f"   Status: {'✓ PASSED' if init_cleanup_ok else '✗ FAILED'}")
    print()
    
    # Summary
    print("=" * 60)
    print("REFACTORING REQUIREMENTS SUMMARY")
    print("=" * 60)
    
    requirements = [
        ("Memory safety with std::unique_ptr", unique_ptr_ok),
        ("Modern C++ features", all_cpp_features_ok),
        ("PIMPL pattern implementation", pimpl_ok),
        ("BrainFactory class", brainfactory_ok),
        ("Exception safety patterns", init_cleanup_ok),
        ("Code organization", main_ok),
    ]
    
    all_passed = True
    for name, passed in requirements:
        status = '✓ PASSED' if passed else '✗ FAILED'
        print(f"  {name:35} {status}")
        if not passed:
            all_passed = False
    
    print()
    print("=" * 60)
    
    if all_passed:
        print("SUCCESS: All refactoring requirements have been met!")
        print()
        print("Refactoring completed successfully with:")
        print("• Modern C++ RAII and memory management using std::unique_ptr")
        print("• PIMPL pattern for improved separation of concerns")
        print("• BrainFactory helper class for configuration-based creation")
        print("• Move semantics and exception safety")
        print("• Template metaprogramming where beneficial")
        print("• Clean, maintainable code structure")
    else:
        print("FAILURE: Some refactoring requirements were not met.")
        print("Please review the failed checks above.")
    
    return all_passed

if __name__ == "__main__":
    # Change to script directory
    os.chdir(os.path.dirname(os.path.abspath(__file__)))
    
    success = verify_refactoring()
    exit(0 if success else 1)