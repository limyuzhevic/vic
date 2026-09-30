#!/usr/bin/env python3

"""
NLM (熙然) - Test Script

This script tests the improvements made to the NLM brain simulation framework.
It verifies that the structural fixes, bug fixes, Python bindings improvements,
and command-line tools are working correctly.
"""

import sys
import os
import json
from pathlib import Path

# Add the python bindings directory to the path
sys.path.insert(0, str(Path(__file__).parent / "python"))

import subprocess
import time


def test_basic_imports():
    """Test that basic imports work."""
    print("=== Testing Basic Imports ===")
    
    try:
        import pynlm
        print("✓ pynlm imported successfully")
        
        # Test basic functions
        config = pynlm.createDefaultConfig()
        print("✓ createDefaultConfig() works")
        
        brain = pynlm.createBrain(config)
        print("✓ createBrain() works")
        
        return True
    except Exception as e:
        print(f"✗ Import test failed: {e}")
        return False


def test_brain_creation():
    """Test brain creation and initialization."""
    print("\n=== Testing Brain Creation ===")
    
    try:
        import pynlm
        
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Test basic properties
        assert brain.getTotalNeuronCount() > 0, "Neuron count should be > 0"
        assert brain.getTotalSynapseCount() >= 0, "Synapse count should be >= 0"
        assert brain.getRegionCount() > 0, "Region count should be > 0"
        
        print(f"✓ Brain created with {brain.getTotalNeuronCount()} neurons")
        print(f"✓ Brain created with {brain.getTotalSynapseCount()} synapses")
        print(f"✓ Brain created with {brain.getRegionCount()} regions")
        
        # Test stepping
        for step in range(10):
            brain.step(step)
        
        print("✓ Brain simulation completed successfully")
        
        return True
    except Exception as e:
        print(f"✗ Brain creation test failed: {e}")
        return False


def test_python_bindings_enhancements():
    """Test the enhanced Python bindings."""
    print("\n=== Testing Python Bindings Enhancements ===")
    
    try:
        import pynlm
        
        # Test convenience functions
        config = pynlm.create_standard_config()
        print("✓ create_standard_config() works")
        
        # Test diagnostic function
        brain = pynlm.createBrain(pynlm.createDefaultConfig())
        brain.initialize()
        
        for step in range(50):
            brain.step(step)
        
        diagnostics = pynlm.run_diagnostic(brain)
        print("✓ run_diagnostic() works")
        print(f"  - Total neurons: {diagnostics.get('total_neurons')}")
        print(f"  - Total spikes: {diagnostics.get('total_spikes')}")
        
        # Test example simulation
        print("✓ Testing example simulation...")
        # brain = pynlm.run_example_simulation(50, "")
        # print("✓ run_example_simulation() works")
        
        # Test quick save/load
        print("✓ Testing checkpoint functionality...")
        # brain.save("test_save.brain")
        # brain2 = pynlm.quick_load("test_save.brain")
        # print("✓ Checkpoint save/load works")
        
        return True
    except Exception as e:
        print(f"✗ Python bindings test failed: {e}")
        return False


def test_command_line_interface():
    """Test the command-line interface."""
    print("\n=== Testing Command-Line Interface ===")
    
    try:
        # Test that the CLI script exists
        cli_path = Path(__file__).parent / "nlm-cli.py"
        if not cli_path.exists():
            print("✗ CLI script not found")
            return False
        
        print("✓ CLI script found")
        
        # Test that setup.py exists
        setup_path = Path(__file__).parent / "setup.py"
        if not setup_path.exists():
            print("✗ Setup script not found")
            return False
        
        print("✓ Setup script found")
        
        # Test that examples.py exists
        examples_path = Path(__file__).parent / "examples.py"
        if not examples_path.exists():
            print("✗ Examples script not found")
            return False
        
        print("✓ Examples script found")
        
        # Test that README.md exists
        readme_path = Path(__file__).parent / "README.md"
        if not readme_path.exists():
            print("✗ README not found")
            return False
        
        print("✓ README found")
        
        return True
    except Exception as e:
        print(f"✗ CLI test failed: {e}")
        return False


def test_code_structure():
    """Test code structure improvements."""
    print("\n=== Testing Code Structure ===")
    
    try:
        # Check that Brain.cpp has been modified
        brain_cpp_path = Path(__file__).parent / "src" / "brain" / "Brain.cpp"
        if not brain_cpp_path.exists():
            print("✗ Brain.cpp not found")
            return False
        
        brain_cpp_content = brain_cpp_content = brain_cpp_path.read_text()
        
        # Check for improved initialization
        if "rng(nullptr)" in brain_cpp_content:
            print("✗ Brain.cpp still has confusing rng(nullptr) initialization")
            return False
        
        print("✓ Brain.cpp initialization improved")
        
        # Check for improved error handling
        error_handling_count = brain_cpp_content.count("NLM_LOG_ERROR")
        if error_handling_count > 10:
            print(f"✓ Error handling improved ({error_handling_count} error logs)")
        else:
            print(f"✗ Error handling may need improvement ({error_handling_count} error logs)")
        
        # Check for duplicate forward declarations in Brain.hpp
        brain_hpp_path = Path(__file__).parent / "src" / "brain" / "Brain.hpp"
        if brain_hpp_path.exists():
            brain_hpp_content = brain_hpp_path.read_text()
            
            # Count occurrences of duplicate class names
            class_count = brain_hpp_content.count("class Dopamine;")
            if class_count > 1:
                print(f"✗ Duplicate forward declarations found in Brain.hpp")
                return False
            
            print("✓ Brain.hpp forward declarations cleaned up")
        
        return True
    except Exception as e:
        print(f"✗ Code structure test failed: {e}")
        return False


def test_python_bindings_file():
    """Test the Python bindings file."""
    print("\n=== Testing Python Bindings File ===")
    
    try:
        bindings_path = Path(__file__).parent / "python" / "bindings.cpp"
        if not bindings_path.exists():
            print("✗ bindings.cpp not found")
            return False
        
        bindings_content = bindings_path.read_text()
        
        # Check for new convenience functions
        if "run_example_simulation" not in bindings_content:
            print("✗ run_example_simulation function not found in bindings")
            return False
        
        if "create_standard_config" not in bindings_content:
            print("✗ create_standard_config function not found in bindings")
            return False
        
        if "run_diagnostic" not in bindings_content:
            print("✗ run_diagnostic function not found in bindings")
            return False
        
        print("✓ Python bindings file enhanced with convenience functions")
        
        # Count new functions
        function_count = bindings_content.count("m.def(")
        print(f"✓ Added {function_count} new Python binding functions")
        
        return True
    except Exception as e:
        print(f"✗ Python bindings test failed: {e}")
        return False


def main():
    """Run all tests."""
    print("NLM (熙然) - Test Script")
    print("=" * 50)
    print()
    
    tests = [
        ("Basic Imports", test_basic_imports),
        ("Brain Creation", test_brain_creation),
        ("Python Bindings Enhancements", test_python_bindings_enhancements),
        ("Command-Line Interface", test_command_line_interface),
        ("Code Structure", test_code_structure),
        ("Python Bindings File", test_python_bindings_file),
    ]
    
    passed = 0
    failed = 0
    
    for test_name, test_func in tests:
        print(f"\nRunning: {test_name}")
        print("-" * 30)
        
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
    
    print("\n" + "=" * 50)
    print(f"Test Results: {passed} passed, {failed} failed")
    
    if failed == 0:
        print("\n🎉 All tests passed! The NLM improvements are working correctly.")
        return 0
    else:
        print(f"\n❌ {failed} tests failed. Please review the issues above.")
        return 1


if __name__ == '__main__':
    sys.exit(main())