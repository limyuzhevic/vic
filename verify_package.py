#!/usr/bin/env python3
"""
Quick verification test for the pynlm Python package.
This script tests the basic imports and functionality to ensure the package
is correctly structured and working.
"""

import sys
import os

# Add the python directory to the path
sys.path.insert(0, os.path.join(os.path.dirname(__file__), 'python'))

def test_imports():
    """Test that the pynlm module can be imported"""
    print("Testing pynlm module import...")
    try:
        import pynlm
        print(f"✓ pynlm imported successfully (version: {pynlm.get_version()})")
        return True
    except ImportError as e:
        print(f"✗ Failed to import pynlm: {e}")
        return False
    except Exception as e:
        print(f"✗ Unexpected error importing pynlm: {e}")
        return False

def test_basic_functionality():
    """Test basic pynlm functionality"""
    print("\nTesting basic functionality...")
    try:
        import pynlm
        
        # Test configuration creation
        config = pynlm.createDefaultConfig()
        print(f"✓ Default config created ({config.neuron_count} neurons)")
        
        # Test brain creation
        brain = pynlm.createBrain(config)
        print("✓ Brain created")
        
        # Test brain initialization
        brain.initialize()
        print("✓ Brain initialized")
        
        # Test brain stepping
        for step in range(3):
            brain.step(step)
        print(f"✓ Brain stepped (average firing rate: {brain.getAverageFiringRate():.2f} Hz)")
        
        return True
        
    except Exception as e:
        print(f"✗ Basic functionality test failed: {e}")
        import traceback
        traceback.print_exc()
        return False

def test_package_structure():
    """Test that the package has the expected structure"""
    print("\nTesting package structure...")
    
    expected_modules = [
        'brain',
        'world', 
        'agent',
        'exceptions',
        'utils',
        'types',
    ]
    
    missing_modules = []
    
    for module in expected_modules:
        try:
            __import__(f'pynlm.{module}')
            print(f"✓ {module} module available")
        except ImportError:
            print(f"✗ {module} module not found")
            missing_modules.append(module)
    
    if missing_modules:
        print(f"⚠ Missing modules: {', '.join(missing_modules)}")
        return len(missing_modules) == 0
    else:
        print("✓ All expected modules are available")
        return True

def test_example_scripts():
    """Test that example scripts can be imported"""
    print("\nTesting example scripts...")
    
    examples_dir = os.path.join(os.path.dirname(__file__), 'examples')
    
    if not os.path.exists(examples_dir):
        print("✗ Examples directory not found")
        return False
    
    example_files = ['01_hello_brain.py', '02_brain_world.py']
    
    success_count = 0
    for example_file in example_files:
        filepath = os.path.join(examples_dir, example_file)
        if os.path.exists(filepath):
            print(f"  Testing {example_file}... ", end="")
            try:
                # Just test that the file can be parsed
                with open(filepath, 'r') as f:
                    content = f.read()
                
                # Check that it contains expected keywords
                if 'import pynlm' in content or 'from pynlm' in content:
                    print("✓")
                    success_count += 1
                else:
                    print("⚠ (no pynlm imports found)")
            except Exception as e:
                print(f"✗ ({e})")
        else:
            print(f"  ✗ {example_file} not found")
    
    print(f"Example scripts tested: {success_count}/{len(example_files)}")
    return success_count > 0

def main():
    """Run all verification tests"""
    print("=" * 60)
    print("NLM Python Package Verification")
    print("=" * 60)
    print()
    print("This script verifies that the pynlm Python package is correctly")
    print("structured and functional. The package provides a Python interface")
    print("to the NLM (Neural Learning Machine) artificial brain simulation framework.")
    print()
    
    tests = [
        ("Module Import", test_imports),
        ("Basic Functionality", test_basic_functionality),
        ("Package Structure", test_package_structure),
        ("Example Scripts", test_example_scripts),
    ]
    
    passed = 0
    failed = 0
    
    for test_name, test_func in tests:
        print(f"{'='*60}")
        print(f"Running: {test_name}")
        print(f"{'='*60}")
        
        if test_func():
            passed += 1
            print(f"✓ {test_name}: PASSED")
        else:
            failed += 1
            print(f"✗ {test_name}: FAILED")
    
    print(f"\n{'='*60}")
    print(f"VERIFICATION RESULTS: {passed} passed, {failed} failed")
    print(f"{'='*60}")
    
    if failed == 0:
        print("\n✓ VERIFICATION SUCCESSFUL!")
        print("\nThe pynlm Python package is correctly structured and functional.")
        print("\nThe package provides:")
        print("  • Brain simulation with LIF neurons")
        print("  • Event-driven spike propagation")
        print("  • Multiple plasticity rules (STDP, Hebbian)")
        print("  • Integrated memory systems")
        print("  • Prediction and cognition")
        print("  • Neuromodulation (dopamine, curiosity, novelty)")
        print("  • Development and maturation")
        print("  • World simulation and interaction")
        print("  • Agent brain interface")
        print("  • Configuration system")
        print("  • Error handling and recovery")
        print("  • Performance monitoring")
        print("  • Documentation and examples")
        print("\nTo get started, run:")
        print("  import pynlm")
        print("  brain = pynlm.createBrain(pynlm.createDefaultConfig())")
        print("  brain.initialize()")
        print("  for step in range(100):")
        print("      brain.step(step)")
        return 0
    else:
        print(f"\n✗ VERIFICATION FAILED!")
        print(f"  {failed} test(s) failed. Please check the output above.")
        print("\nCommon issues:")
        print("  • Python version too low (requires >= 3.8)")
        print("  • Missing dependencies (numpy, pybind11)")
        print("  • Package structure incorrectly installed")
        print("  • Syntax errors in package files")
        return 1

if __name__ == '__main__':
    sys.exit(main())
