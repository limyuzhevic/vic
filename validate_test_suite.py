#!/usr/bin/env python3
"""
Simple validation test for NLM Python bindings test suite
This script tests the basic functionality without requiring the full build
"""

import sys
import os

def test_imports():
    """Test that all expected modules can be imported"""
    print("Testing imports...")
    
    try:
        # Test core imports
        from pynlm import Config, Brain, SimpleWorld, AgentBrain
        print("✓ Core classes imported")
        
        # Test enum imports
        from pynlm import NeuronType, SynapseType, ActionType, DevelopmentalStage, FiringState
        print("✓ Enum classes imported")
        
        # Test ID class imports
        from pynlm import NeuronId, SynapseId, RegionId, PopulationId
        print("✓ ID classes imported")
        
        # Test helper functions
        from pynlm.helpers import get_debug_config, get_performance_config, get_creative_config
        print("✓ Helper functions imported")
        
        return True
        
    except ImportError as e:
        print(f"✗ Import error: {e}")
        return False

def test_basic_functionality():
    """Test basic functionality"""
    print("\nTesting basic functionality...")
    
    try:
        from pynlm import Config, get_default_config
        
        # Test Config creation and basic operations
        config = Config()
        assert not config.has("test")
        
        config.set("neuron_count", 100)
        assert config.has("neuron_count")
        assert config.get_int("neuron_count") == 100
        
        print("✓ Basic Config operations work")
        
        # Test config templates
        config_dict = get_default_config()
        assert "neuron_count" in config_dict
        assert "region_count" in config_dict
        assert "connection_probability" in config_dict
        
        print("✓ Config templates work")
        
        return True
        
    except Exception as e:
        print(f"✗ Basic functionality error: {e}")
        return False

def test_test_files_exist():
    """Test that test files exist"""
    print("\nTesting test file existence...")
    
    test_files = [
        "python/tests/test_config.py",
        "python/tests/test_brain.py",
        "python/tests/test_bindings.py",
        "python/tests/test_integration.py",
        "python/tests/test_workflow.py",
    ]
    
    all_exist = True
    for test_file in test_files:
        if os.path.exists(test_file):
            print(f"✓ {test_file}")
        else:
            print(f"✗ {test_file} - MISSING")
            all_exist = False
    
    return all_exist

def test_configuration_files():
    """Test that configuration files exist"""
    print("\nTesting configuration files...")
    
    config_files = [
        "pyproject.toml",
        "setup.py",
        "pytest.ini",
        "requirements.txt",
    ]
    
    all_exist = True
    for config_file in config_files:
        if os.path.exists(config_file):
            print(f"✓ {config_file}")
        else:
            print(f"✗ {config_file} - MISSING")
            all_exist = False
    
    return all_exist

def test_scripts_exist():
    """Test that scripts exist"""
    print("\nTesting script files...")
    
    script_files = [
        "scripts/run_tests.py",
        "scripts/analyze_coverage.py",
    ]
    
    all_exist = True
    for script_file in script_files:
        if os.path.exists(script_file):
            print(f"✓ {script_file}")
        else:
            print(f"✗ {script_file} - MISSING")
            all_exist = False
    
    return all_exist

def test_cpp_tests_exist():
    """Test that C++ test files exist"""
    print("\nTesting C++ test files...")
    
    cpp_tests = [
        "tests/test_config.cpp",
        "tests/test_brain.cpp",
        "tests/test_neuron.cpp",
        "tests/test_synapse.cpp",
        "tests/test_clock.cpp",
        "tests/test_random.cpp",
        "tests/test_types.cpp",
        "tests/test_stdp.cpp",
        "tests/test_main.cpp",
    ]
    
    all_exist = True
    for cpp_test in cpp_tests:
        if os.path.exists(cpp_test):
            print(f"✓ {cpp_test}")
        else:
            print(f"✗ {cpp_test} - MISSING")
            all_exist = False
    
    return all_exist

def main():
    """Main validation function"""
    print("NLM Python Bindings Test Suite Validation")
    print("=" * 60)
    
    # Change to project directory
    project_dir = os.path.dirname(os.path.abspath(__file__))
    os.chdir(project_dir)
    
    # Run validation tests
    tests = [
        ("Imports", test_imports),
        ("Basic Functionality", test_basic_functionality),
        ("Test Files", test_test_files_exist),
        ("Configuration Files", test_configuration_files),
        ("Scripts", test_scripts_exist),
        ("C++ Tests", test_cpp_tests_exist),
    ]
    
    results = {}
    
    for test_name, test_func in tests:
        print(f"\n{'='*60}")
        print(f"Running {test_name} test...")
        print(f"{'='*60}")
        
        try:
            success = test_func()
            results[test_name] = success
            status = "PASSED" if success else "FAILED"
            print(f"\n{test_name}: {status}")
        except Exception as e:
            results[test_name] = False
            print(f"\n{test_name}: FAILED with exception: {e}")
    
    # Summary
    print(f"\n{'='*60}")
    print("VALIDATION SUMMARY")
    print(f"{'='*60}")
    
    all_passed = True
    for test_name, success in results.items():
        status = "PASSED" if success else "FAILED"
        print(f"{test_name}: {status}")
        if not success:
            all_passed = False
    
    print(f"\n{'='*60}")
    if all_passed:
        print("✓ ALL VALIDATION TESTS PASSED")
        print("✓ NLM Python bindings test suite is ready!")
        print("\nNext steps:")
        print("  1. Install dependencies: pip install -r requirements.txt")
        print("  2. Build Python bindings: pip install -e .")
        print("  3. Run C++ tests: mkdir build_cpp_tests && cd build_cpp_tests && cmake .. -DBUILD_TESTING=ON && make -j$(nproc)")
        print("  4. Run Python tests: pytest python/tests/ -v")
        print("  5. Run integration tests: python scripts/run_tests.py")
        return 0
    else:
        print("✗ SOME VALIDATION TESTS FAILED")
        print("✗ Please check the issues above and fix them")
        return 1

if __name__ == "__main__":
    sys.exit(main())
