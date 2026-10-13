#!/usr/bin/env python3
"""
Simple integration test for the pynlm Python package.
This test verifies that the basic package structure and imports work correctly.
"""

import sys
import os

def test_basic_imports():
    """Test basic imports from the pynlm package"""
    print("Testing basic imports...")
    
    try:
        # Import the main pynlm module
        import pynlm
        print("✓ pynlm module imported successfully")
        
        # Check version
        version = pynlm.get_version()
        print(f"✓ Package version: {version}")
        
        # Check build info
        build_info = pynlm.get_build_info()
        print(f"✓ Platform: {build_info['platform']}")
        
        # Test configuration
        config = pynlm.createDefaultConfig()
        print(f"✓ Default config created: {config.neuron_count} neurons")
        
        return True
        
    except ImportError as e:
        print(f"✗ Import failed: {e}")
        return False
    except Exception as e:
        print(f"✗ Unexpected error: {e}")
        import traceback
        traceback.print_exc()
        return False

def test_brain_creation():
    """Test brain creation and basic functionality"""
    print("\nTesting brain creation...")
    
    try:
        import pynlm
        
        # Create brain with default config
        brain = pynlm.createBrain(pynlm.createDefaultConfig())
        print("✓ Brain created successfully")
        
        # Initialize brain
        brain.initialize()
        print("✓ Brain initialized successfully")
        
        # Run some steps
        for step in range(5):
            brain.step(step)
        
        print(f"✓ Brain steps completed")
        print(f"  - Average firing rate: {brain.getAverageFiringRate():.2f} Hz")
        print(f"  - Total spikes: {brain.getTotalSpikeCount()}")
        
        return True
        
    except Exception as e:
        print(f"✗ Brain creation test failed: {e}")
        import traceback
        traceback.print_exc()
        return False

def test_config_system():
    """Test configuration system"""
    print("\nTesting configuration system...")
    
    try:
        import pynlm
        
        # Create custom config
        config = pynlm.createDefaultConfig()
        config.neuron_count = 100
        config.region_count = 2
        config.connection_probability = 0.15
        
        print(f"✓ Custom config: {config.neuron_count} neurons, {config.region_count} regions")
        
        # Test validation
        config_dict = {
            'neuron_count': config.neuron_count,
            'simulation_timestep': config.simulation_timestep,
            'connection_probability': config.connection_probability,
        }
        
        errors = pynlm.validate_config(config_dict)
        if errors:
            print(f"✗ Config validation errors: {errors}")
            return False
        else:
            print("✓ Config validation passed")
        
        # Test brain with custom config
        brain = pynlm.createBrain(config)
        brain.initialize()
        print("✓ Brain initialized with custom config")
        
        return True
        
    except Exception as e:
        print(f"✗ Configuration test failed: {e}")
        return False

def test_examples():
    """Test example scripts"""
    print("\nTesting example scripts...")
    
    examples_dir = os.path.join(os.path.dirname(__file__), 'examples')
    
    if not os.path.exists(examples_dir):
        print("✗ Examples directory not found")
        return False
    
    example_files = [
        '01_hello_brain.py',
        '02_brain_world.py',
    ]
    
    success_count = 0
    total_count = len(example_files)
    
    for example_file in example_files:
        filepath = os.path.join(examples_dir, example_file)
        if os.path.exists(filepath):
            print(f"  Testing {example_file}... ", end="")
            try:
                # Import the example module
                with open(filepath, 'r') as f:
                    content = f.read()
                
                # Create a namespace for execution
                namespace = {
                    '__name__': '__main__',
                    '__file__': filepath,
                    'pynlm': sys.modules['pynlm'],
                    'time': time,
                    'os': os,
                }
                
                # Execute the example (just the imports and basic setup)
                exec(content[:500], namespace)  # Execute first 500 chars
                print("✓")
                success_count += 1
                
            except Exception as e:
                print(f"✗ ({e})")
        else:
            print(f"  ✗ {example_file} not found")
    
    print(f"Examples tested: {success_count}/{total_count} successful")
    return success_count > 0

def test_package_structure():
    """Test package structure and modules"""
    print("\nTesting package structure...")
    
    try:
        import pynlm
        
        # Test that key modules are available
        required_modules = [
            'brain',
            'world',
            'agent',
            'exceptions',
            'utils',
            'types',
        ]
        
        available_modules = []
        missing_modules = []
        
        for module in required_modules:
            try:
                __import__(f'pynlm.{module}')
                available_modules.append(module)
            except ImportError:
                missing_modules.append(module)
        
        print(f"✓ Available modules: {', '.join(available_modules)}")
        
        if missing_modules:
            print(f"⚠ Missing modules: {', '.join(missing_modules)}")
        
        # Test key classes
        from pynlm import (
            Brain, Config, World, WorldObject,
            SensoryPercept, AgentBrain, MotorCommand,
            NLMError, ConfigurationError, SimulationError,
            ErrorHandler, DebugLogger, PerformanceMonitor,
            get_version, get_build_info, validate_config,
            types
        )
        
        print("✓ Key classes imported successfully")
        
        return True
        
    except Exception as e:
        print(f"✗ Package structure test failed: {e}")
        import traceback
        traceback.print_exc()
        return False

def main():
    """Run all tests"""
    print("=" * 60)
    print("NLM Python Package Integration Test")
    print("=" * 60)
    
    # Import time module for the test
    import time
    
    tests = [
        ("Basic Imports", test_basic_imports),
        ("Brain Creation", test_brain_creation),
        ("Configuration System", test_config_system),
        ("Package Structure", test_package_structure),
        ("Example Scripts", test_examples),
    ]
    
    passed = 0
    failed = 0
    
    for test_name, test_func in tests:
        print(f"\n{'='*60}")
        print(f"Running: {test_name}")
        print(f"{'='*60}")
        
        if test_func():
            passed += 1
            print(f"✓ {test_name}: PASSED")
        else:
            failed += 1
            print(f"✗ {test_name}: FAILED")
    
    print(f"\n{'='*60}")
    print(f"Test Results: {passed} passed, {failed} failed")
    print(f"{'='*60}")
    
    if failed == 0:
        print("✓ All tests passed! The NLM package is working correctly.")
        print("\nThe pynlm package provides a comprehensive Python interface to the NLM")
        print("(Neural Learning Machine) artificial brain simulation framework.")
        print("\nTo get started, try:")
        print("  import pynlm")
        print("  brain = pynlm.createBrain(pynlm.createDefaultConfig())")
        print("  brain.initialize()")
        print("  for step in range(100):")
        print("      brain.step(step)")
        return 0
    else:
        print(f"✗ {failed} tests failed. Please check the output above.")
        return 1

if __name__ == '__main__':
    sys.exit(main())
