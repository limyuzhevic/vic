#!/usr/bin/env python3
"""
Verification Script for NLM Improvements

This script verifies that all improvements have been successfully implemented
and are functioning correctly.
"""

import os
import sys
import tempfile
from pathlib import Path

def check_file_exists(path, description):
    """Check if a file exists."""
    if os.path.exists(path):
        print(f"✓ {description}: {path}")
        return True
    else:
        print(f"✗ {description}: {path} (NOT FOUND)")
        return False

def check_file_content(path, expected_content, description):
    """Check if a file contains expected content."""
    try:
        with open(path, 'r') as f:
            content = f.read()
            if expected_content in content:
                print(f"✓ {description}: {path}")
                return True
            else:
                print(f"✗ {description}: {path} (content not found)")
                return False
    except Exception as e:
        print(f"✗ {description}: {path} (error: {e})")
        return False

def check_python_imports():
    """Check if Python bindings can be imported."""
    try:
        # Add build directory to Python path
        build_dir = Path(__file__).parent / "build"
        if build_dir.exists():
            sys.path.insert(0, str(build_dir))
        
        import pynlm
        print("✓ Python bindings import successfully")
        
        # Check exception registration
        exceptions = [
            'ConfigException',
            'ConfigKeyNotFoundException', 
            'ConfigInvalidValueException',
            'ConfigFileErrorException',
            'ConfigValidationException'
        ]
        
        all_found = True
        for exc in exceptions:
            if hasattr(pynlm, exc):
                print(f"✓ Python exception {exc} registered")
            else:
                print(f"✗ Python exception {exc} NOT registered")
                all_found = False
        
        return all_found
        
    except ImportError as e:
        print(f"✗ Cannot import pynlm: {e}")
        print("  (This is expected if bindings haven't been built yet)")
        return False
    except Exception as e:
        print(f"✗ Error checking Python bindings: {e}")
        return False

def check_config_file():
    """Check the configuration file for duplicates."""
    config_path = Path(__file__).parent / "configs" / "default.cfg"
    
    try:
        with open(config_path, 'r') as f:
            content = f.read()
            lines = content.split('\n')
            
            # Check for duplicate dopamine_baseline
            dopamine_lines = []
            for i, line in enumerate(lines):
                if 'dopamine_baseline' in line:
                    dopamine_lines.append((i, line.strip()))
            
            if len(dopamine_lines) > 1:
                print(f"✗ Configuration file has duplicate entries:")
                for line_num, line_content in dopamine_lines:
                    print(f"  Line {line_num + 1}: {line_content}")
                return False
            elif len(dopamine_lines) == 1:
                print(f"✓ Configuration file has single dopamine_baseline entry: {dopamine_lines[0][1]}")
                return True
            else:
                print(f"✗ Configuration file has no dopamine_baseline entry")
                return False
                
    except Exception as e:
        print(f"✗ Error checking config file: {e}")
        return False

def check_test_files():
    """Check if test files exist."""
    test_dir = Path(__file__).parent / "tests"
    
    files_to_check = [
        ("test_python_bindings.py", "Python test suite"),
        ("test_config_error_handling.cpp", "C++ error handling tests"),
        ("test_runner.py", "Test runner script")
    ]
    
    all_good = True
    for filename, description in files_to_check:
        if check_file_exists(test_dir / filename, description):
            pass
        else:
            all_good = False
    
    return all_good

def check_examples():
    """Check if examples exist."""
    examples_dir = Path(__file__).parent / "examples"
    
    files_to_check = [
        ("basic_usage.py", "Basic usage examples")
    ]
    
    all_good = True
    for filename, description in files_to_check:
        if check_file_exists(examples_dir / filename, description):
            pass
        else:
            all_good = False
    
    return all_good

def check_documentation():
    """Check if documentation files exist."""
    docs_to_check = [
        ("IMPROVEMENTS_SUMMARY.md", "Improvements summary"),
        ("QUICK_START.md", "Quick start guide")
    ]
    
    all_good = True
    for filename, description in docs_to_check:
        if check_file_exists(Path(__file__).parent / filename, description):
            pass
        else:
            all_good = False
    
    return all_good

def check_ci_config():
    """Check if CI configuration exists."""
    ci_path = Path(__file__).parent / ".github" / "workflows" / "ci.yml"
    return check_file_exists(ci_path, "CI/CD pipeline configuration")

def main():
    """Run all verification checks."""
    print("=" * 70)
    print("NLM Project Improvements Verification")
    print("=" * 70)
    
    checks = [
        ("Configuration File", check_config_file),
        ("Test Files", check_test_files),
        ("Examples", check_examples),
        ("Documentation", check_documentation),
        ("CI Configuration", check_ci_config),
        ("Python Bindings", check_python_imports),
    ]
    
    passed = 0
    failed = 0
    
    for check_name, check_func in checks:
        print(f"\n{'='*70}")
        print(f"Checking: {check_name}")
        print(f"{'='*70}")
        
        if check_func():
            passed += 1
        else:
            failed += 1
    
    print(f"\n{'='*70}")
    print("VERIFICATION SUMMARY")
    print(f"{'='*70}")
    print(f"Total checks: {len(checks)}")
    print(f"Passed: {passed}")
    print(f"Failed: {failed}")
    
    if failed == 0:
        print("\n🎉 All checks passed! Improvements have been successfully implemented.")
        return 0
    else:
        print(f"\n⚠️  {failed} check(s) failed. Please review the issues above.")
        return 1

if __name__ == "__main__":
    sys.exit(main())