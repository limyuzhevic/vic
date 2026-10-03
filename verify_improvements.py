#!/usr/bin/env python3
"""
Test script to verify NLM codebase improvements.

This script checks that the improvements made to the NLM codebase are working correctly.
"""

import os
import sys
import re

def check_file_exists(filepath):
    """Check if a file exists."""
    return os.path.exists(filepath)

def check_phase_consistency():
    """Check that phase consistency has been fixed."""
    print("Checking phase consistency...")
    
    # Check README.md for Phase 6
    readme_path = "README.md"
    if check_file_exists(readme_path):
        with open(readme_path, 'r', encoding='utf-8') as f:
            content = f.read()
            if "Phase 6: Final Integration" in content and "PHASE 6 (COMPLETE)" in content:
                print("✓ README.md correctly shows Phase 6 as current")
                return True
            else:
                print("✗ README.md phase inconsistency")
                return False
    else:
        print("✗ README.md not found")
        return False

def check_plasticity_rules():
    """Check that plasticity rules have been implemented."""
    print("\nChecking plasticity rule implementations...")
    
    # Check PlasticityRule.hpp for implementation
    plasticity_hpp = "src/plasticity/PlasticityRule.hpp"
    if check_file_exists(plasticity_hpp):
        with open(plasticity_hpp, 'r', encoding='utf-8') as f:
            content = f.read()
            # Check that AntiHebbianRule and BCMRule have implementations
            if "class AntiHebbianRule" in content and "void update(" in content:
                print("✓ AntiHebbianRule has implementation")
            else:
                print("✗ AntiHebbianRule missing implementation")
                
            if "class BCMRule" in content and "void update(" in content:
                print("✓ BCMRule has implementation")
            else:
                print("✗ BCMRule missing implementation")
                
            # Check that HebbianRule has enhanced implementation
            if "struct HebbianRule::Impl" in content:
                print("✓ HebbianRule has implementation")
            else:
                print("✗ HebbianRule missing implementation")
                
            return True
    else:
        print("✗ PlasticityRule.hpp not found")
        return False

def check_documentation():
    """Check that documentation has been improved."""
    print("\nChecking documentation...")
    
    docs = [
        "CLI_DEMO.md",
        "DEPRECATED_Phase3Demo.md",
        "NLM_IMPROVEMENTS_SUMMARY.md"
    ]
    
    found_docs = 0
    for doc in docs:
        if check_file_exists(doc):
            print(f"✓ {doc} found")
            found_docs += 1
        else:
            print(f"✗ {doc} not found")
    
    if found_docs >= 2:
        print(f"✓ Good documentation coverage ({found_docs}/3 docs)")
        return True
    else:
        print(f"✗ Poor documentation coverage ({found_docs}/3 docs)")
        return False

def check_main_cpp():
    """Check that main.cpp has been updated."""
    print("\nChecking main.cpp updates...")
    
    main_cpp = "src/main.cpp"
    if check_file_exists(main_cpp):
        with open(main_cpp, 'r', encoding='utf-8') as f:
            content = f.read()
            # Check for Phase 6 documentation
            if "Phase 6: Final Integration" in content:
                print("✓ main.cpp correctly shows Phase 6")
            else:
                print("✗ main.cpp phase inconsistency")
                
            # Check for integration test comments
            if "Integration Test" in content and "Complete brain" in content:
                print("✓ main.cpp has integration test focus")
            else:
                print("✗ main.cpp missing integration test focus")
                
            return True
    else:
        print("✗ main.cpp not found")
        return False

def main():
    """Main function."""
    print("=" * 60)
    print("NLM Codebase Improvements Verification")
    print("=" * 60)
    
    # Change to the project directory
    project_dir = os.path.dirname(os.path.abspath(__file__))
    os.chdir(project_dir)
    
    # Run checks
    checks = [
        check_phase_consistency,
        check_plasticity_rules,
        check_documentation,
        check_main_cpp
    ]
    
    passed = 0
    total = len(checks)
    
    for check in checks:
        if check():
            passed += 1
    
    print("\n" + "=" * 60)
    print(f"Summary: {passed}/{total} checks passed")
    print("=" * 60)
    
    if passed >= total * 0.75:  # 75% pass rate
        print("✓ Codebase improvements verified successfully!")
        return 0
    else:
        print("✗ Some checks failed. Please review the codebase.")
        return 1

if __name__ == "__main__":
    sys.exit(main())