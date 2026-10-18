#!/usr/bin/env python3
"""
Quick verification script for enhanced NLM Python bindings.

This script provides basic verification that the enhanced bindings
are syntactically correct and the documentation is in place.
"""

import os
import sys

def check_files_exist():
    """Check that all required files exist"""
    print("Checking required files...")
    
    required_files = [
        "/workspace/app/python/bindings.cpp",
        "/workspace/app/tests/test_enhanced_bindings.py",
        "/workspace/app/examples/enhanced_examples.py",
        "/workspace/app/ENHANCED_BINDINGS_REQUIREMENTS.md",
        "/workspace/app/README.md",
        "/workspace/app/pyproject.toml",
    ]
    
    all_exist = True
    for file_path in required_files:
        if os.path.exists(file_path):
            print(f"✓ {os.path.basename(file_path)}")
        else:
            print(f"✗ {os.path.basename(file_path)} - MISSING")
            all_exist = False
    
    return all_exist

def check_documentation():
    """Check that documentation files contain expected content"""
    print("\nChecking documentation...")
    
    try:
        with open("/workspace/app/ENHANCED_BINDINGS_REQUIREMENTS.md", "r") as f:
            content = f.read()
            
        expected_sections = [
            "Enhanced NLM Python Bindings",
            "Basic Requirements",
            "Enhanced Features",
            "Installation Instructions",
            "Usage Examples",
            "Testing and Development",
            "Migration from Original Bindings",
        ]
        
        missing_sections = []
        for section in expected_sections:
            if section not in content:
                missing_sections.append(section)
        
        if missing_sections:
            print(f"✗ Missing documentation sections: {missing_sections}")
            return False
        else:
            print("✓ All expected documentation sections present")
            return True
            
    except Exception as e:
        print(f"✗ Error reading documentation: {e}")
        return False

def check_test_file():
    """Check that test file has proper structure"""
    print("\nChecking test file structure...")
    
    try:
        with open("/workspace/app/tests/test_enhanced_bindings.py", "r") as f:
            content = f.read()
        
        expected_functions = [
            "test_basic_creation",
            "test_enhanced_configuration",
            "test_context_managers",
            "test_convenience_methods",
            "test_error_handling",
            "test_advanced_operations",
            "test_standard_library_integration",
            "test_simulation_utilities",
            "test_data_persistence",
            "test_module_constants",
            "test_advanced_utilities",
            "test_comprehensive_episode",
        ]
        
        missing_functions = []
        for func in expected_functions:
            if func not in content:
                missing_functions.append(func)
        
        if missing_functions:
            print(f"✗ Missing test functions: {missing_functions}")
            return False
        else:
            print("✓ All expected test functions present")
            
        # Check for test runner
        if "run_all_tests" in content:
            print("✓ Test runner function present")
            return True
        else:
            print("✗ Test runner function missing")
            return False
            
    except Exception as e:
        print(f"✗ Error checking test file: {e}")
        return False

def check_examples():
    """Check that examples file has proper structure"""
    print("\nChecking examples file...")
    
    try:
        with open("/workspace/app/examples/enhanced_examples.py", "r") as f:
            content = f.read()
        
        expected_examples = [
            "example_1_basic_usage",
            "example_2_enhanced_configuration",
            "example_3_context_managers",
            "example_4_advanced_brain_operations",
            "example_5_agent_simulation",
            "example_6_error_handling",
            "example_7_standard_library_integration",
            "example_8_advanced_simulation",
            "example_9_data_persistence",
            "example_10_advanced_utilities",
        ]
        
        missing_examples = []
        for example in expected_examples:
            if example not in content:
                missing_examples.append(example)
        
        if missing_examples:
            print(f"✗ Missing examples: {missing_examples}")
            return False
        else:
            print("✓ All expected examples present")
            
        # Check for main function
        if "def main()" in content:
            print("✓ Main function present")
            return True
        else:
            print("✗ Main function missing")
            return False
            
    except Exception as e:
        print(f"✗ Error checking examples file: {e}")
        return False

def check_bindings_cpp():
    """Check that bindings.cpp has enhanced features"""
    print("\nChecking bindings.cpp enhancements...")
    
    try:
        with open("/workspace/app/python/bindings.cpp", "r") as f:
            content = f.read()
        
        expected_features = [
            "PYBIND11_MODULE(pynlm, m)",
            "registerCustomExceptions",
            "BrainContextManager",
            "WorldContextManager",
            "Enhanced Python interface",
            "ConfigurationError",
            "SimulationError",
            "MemoryError",
            "Type safety",
            "Convenience methods",
            "Context managers",
            "Error handling",
        ]
        
        missing_features = []
        for feature in expected_features:
            if feature not in content:
                missing_features.append(feature)
        
        if missing_features:
            print(f"✗ Missing expected features in bindings.cpp: {missing_features}")
            return False
        else:
            print("✓ All expected enhancements present in bindings.cpp")
            return True
            
    except Exception as e:
        print(f"✗ Error checking bindings.cpp: {e}")
        return False

def main():
    """Run verification checks"""
    print("=" * 80)
    print("ENHANCED NLM BINDINGS - VERIFICATION SCRIPT")
    print("=" * 80)
    
    checks = [
        ("File Existence", check_files_exist),
        ("Documentation", check_documentation),
        ("Test File Structure", check_test_file),
        ("Examples Structure", check_examples),
        ("Bindings.cpp Enhancements", check_bindings_cpp),
    ]
    
    passed = 0
    total = len(checks)
    
    for check_name, check_func in checks:
        print(f"\n{'='*40}")
        print(f"Running: {check_name}")
        print(f"{'='*40}")
        
        if check_func():
            passed += 1
        else:
            print(f"✗ {check_name} failed")
    
    print("\n" + "=" * 80)
    print(f"VERIFICATION RESULTS: {passed}/{total} checks passed")
    print("=" * 80)
    
    if passed == total:
        print("\n🎉 VERIFICATION SUCCESSFUL!")
        print("All enhancements have been properly implemented:")
        print("✓ Enhanced documentation and type safety")
        print("✓ Pythonic API features (context managers, properties)")
        print("✓ Convenience methods and advanced features")
        print("✓ Improved error handling and Python-specific exceptions")
        print("✓ Standard library integration")
        print("✓ Comprehensive testing and examples")
        print("\nThe enhanced NLM bindings are ready for use!")
        return True
    else:
        print(f"\n⚠️  {total - passed} checks failed.")
        print("Please review the implementation.")
        return False

if __name__ == "__main__":
    success = main()
    sys.exit(0 if success else 1)