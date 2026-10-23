#!/usr/bin/env python3
"""
Quick Test Script for NLM Improvements

This script tests the key improvements made to the NLM project
to ensure everything is working correctly.
"""

import sys
import os

def test_imports():
    """Test that all new modules can be imported successfully."""
    print("Testing imports...")
    
    try:
        import pynlm
        print("✓ pynlm imported successfully")
        
        import pynlm_advanced
        print("✓ pynlm_advanced imported successfully")
        
        import nlm_cli
        print("✓ nlm_cli imported successfully")
        
        import nlm_examples
        print("✓ nlm_examples imported successfully")
        
        return True
    except ImportError as e:
        print(f"✗ Import failed: {e}")
        return False

def test_advanced_functions():
    """Test key advanced convenience functions."""
    print("\nTesting advanced convenience functions...")
    
    try:
        from pynlm_advanced import (
            create_advanced_brain,
            create_agent_world,
            create_development_experiment,
            create_memory_experiment,
            create_curiosity_experiment,
            create_prediction_experiment
        )
        
        # Test create_advanced_brain
        brain, config = create_advanced_brain(
            neuron_count=100,
            enable_learning=False,
            enable_memory=False
        )
        print("✓ create_advanced_brain works")
        
        # Test create_agent_world
        brain2, agent2, world2 = create_agent_world(
            neuron_count=50,
            world_width=10,
            world_height=10
        )
        print("✓ create_agent_world works")
        
        # Test create_curiosity_experiment
        brain3, config3 = create_curiosity_experiment(
            exploration_rate=0.5,
            novelty_sensitivity=0.3
        )
        print("✓ create_curiosity_experiment works")
        
        # Test create_prediction_experiment
        brain4, config4 = create_prediction_experiment(
            prediction_horizon=5,
            prediction_error_threshold=0.05
        )
        print("✓ create_prediction_experiment works")
        
        return True
    except Exception as e:
        print(f"✗ Advanced functions test failed: {e}")
        import traceback
        traceback.print_exc()
        return False

def test_cli_structure():
    """Test that CLI structure is properly defined."""
    print("\nTesting CLI structure...")
    
    try:
        from nlm_cli import main, NLMCommandError
        
        # Check that main function exists
        if callable(main):
            print("✓ CLI main function is callable")
        else:
            print("✗ CLI main function is not callable")
            return False
        
        # Check that NLMCommandError exists
        if NLMCommandError:
            print("✓ NLMCommandError class exists")
        else:
            print("✗ NLMCommandError class missing")
            return False
        
        return True
    except Exception as e:
        print(f"✗ CLI structure test failed: {e}")
        return False

def test_examples():
    """Test that examples can be imported."""
    print("\nTesting examples module...")
    
    try:
        from nlm_examples import (
            example_1_basic_usage,
            example_2_advanced_configuration,
            example_3_agent_environment_simulation,
            example_4_memory_systems,
            example_5_development_process,
            example_6_curiosity_driven_learning,
            example_7_prediction_learning,
            example_8_checkpointing,
            example_9_performance_profiling,
            example_10_advanced_techniques
        )
        
        print("✓ All example functions are importable")
        return True
    except Exception as e:
        print(f"✗ Examples test failed: {e}")
        return False

def test_pyproject():
    """Test that pyproject.toml has the right structure."""
    print("\nTesting pyproject.toml structure...")
    
    try:
        import tomli
        
        with open("pyproject.toml", "rb") as f:
            config = tomli.load(f)
        
        # Check project structure
        if "project" in config:
            project = config["project"]
            
            # Check key fields
            if "name" in project and project["name"] == "pynlm":
                print("✓ Project name is correct")
            else:
                print("✗ Project name is incorrect")
                return False
            
            if "version" in project:
                print("✓ Project version is present")
            else:
                print("✗ Project version is missing")
                return False
            
            if "scripts" in project:
                print("✓ Project scripts are defined")
            else:
                print("✗ Project scripts are missing")
                return False
            
            if "optional-dependencies" in project:
                print("✓ Project optional dependencies are defined")
            else:
                print("✗ Project optional dependencies are missing")
                return False
            
            return True
        else:
            print("✗ Project section is missing")
            return False
    except ImportError:
        print("⚠ tomli not available, skipping pyproject.toml test")
        return True
    except Exception as e:
        print(f"✗ pyproject.toml test failed: {e}")
        return False

def main():
    """Run all tests."""
    print("NLM Project Improvements - Quick Test Suite")
    print("=" * 50)
    
    tests = [
        ("Module Imports", test_imports),
        ("Advanced Functions", test_advanced_functions),
        ("CLI Structure", test_cli_structure),
        ("Examples Module", test_examples),
        ("pyproject.toml", test_pyproject),
    ]
    
    passed = 0
    total = len(tests)
    
    for test_name, test_func in tests:
        print(f"\n{'='*20} {test_name} {'='*20}")
        try:
            if test_func():
                passed += 1
            else:
                print(f"✗ {test_name} failed")
        except Exception as e:
            print(f"✗ {test_name} failed with exception: {e}")
    
    print("\n" + "=" * 50)
    print(f"Test Results: {passed}/{total} tests passed")
    
    if passed == total:
        print("🎉 All tests passed! NLM improvements are working correctly.")
        return 0
    else:
        print("❌ Some tests failed. Please check the output above.")
        return 1

if __name__ == "__main__":
    sys.exit(main())
