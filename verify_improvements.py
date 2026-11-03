#!/usr/bin/env python3
"""Quick verification script for NLM improvements"""

import sys
import os
import tempfile
import shutil

def test_config_system():
    """Test the improved Config system"""
    print("Testing Config system improvements...")
    
    try:
        import pynlm
        
        # Test 1: Basic configuration
        print("  Test 1: Basic configuration...")
        config = pynlm.createDefaultConfig()
        config.set("brain.neuron_count", 1000)
        config.set("brain.connection_probability", 0.05)
        config.set("plasticity.stdp.learning_rate", 0.001)
        
        assert config.has("brain.neuron_count")
        assert config.has("brain.connection_probability")
        assert config.has("plasticity.stdp.learning_rate")
        
        neuron_count = config.getOr("brain.neuron_count", 0)
        assert neuron_count == 1000
        print("    ✓ Basic configuration works")
        
        # Test 2: JSON serialization
        print("  Test 2: JSON serialization...")
        with tempfile.NamedTemporaryFile(mode='w', suffix='.json', delete=False) as f:
            config_file = f.name
        
        try:
            config.saveToFile(config_file)
            
            # Load back
            config2 = pynlm.createDefaultConfig()
            config2.loadFromFile(config_file)
            
            assert config2.has("brain.neuron_count")
            assert config2.getOr("brain.neuron_count", 0) == 1000
            print("    ✓ JSON serialization works")
        finally:
            if os.path.exists(config_file):
                os.unlink(config_file)
        
        # Test 3: Complex configuration
        print("  Test 3: Complex configuration...")
        complex_config = pynlm.createDefaultConfig()
        complex_config.set("brain.neuron_count", 5000)
        complex_config.set("brain.synapse_density", 0.2)
        complex_config.set("neuromod.curiosity.enable", True)
        complex_config.set("development.enable", True)
        
        # Check complex values
        assert complex_config.getOr("brain.neuron_count", 0) == 5000
        assert complex_config.getOr("neuromod.curiosity.enable", False) == True
        assert complex_config.getOr("development.enable", False) == True
        print("    ✓ Complex configuration works")
        
        return True
        
    except Exception as e:
        print(f"  ✗ Config system test failed: {e}")
        return False

def test_python_examples():
    """Test the Python examples"""
    print("\nTesting Python examples...")
    
    try:
        # Test quick_start.py
        print("  Test 1: quick_start.py...")
        import subprocess
        result = subprocess.run([sys.executable, "python/examples/quick_start.py"], 
                              capture_output=True, text=True, timeout=30)
        if result.returncode == 0:
            print("    ✓ quick_start.py executed successfully")
        else:
            print(f"    ✗ quick_start.py failed: {result.stderr}")
            return False
        
        # Test configuration_examples.py
        print("  Test 2: configuration_examples.py...")
        result = subprocess.run([sys.executable, "python/examples/configuration_examples.py"], 
                              capture_output=True, text=True, timeout=30)
        if result.returncode == 0:
            print("    ✓ configuration_examples.py executed successfully")
        else:
            print(f"    ✗ configuration_examples.py failed: {result.stderr}")
            return False
        
        # Test advanced_examples.py
        print("  Test 3: advanced_examples.py...")
        result = subprocess.run([sys.executable, "python/examples/advanced_examples.py"], 
                              capture_output=True, text=True, timeout=60)
        if result.returncode == 0:
            print("    ✓ advanced_examples.py executed successfully")
        else:
            print(f"    ✗ advanced_examples.py failed: {result.stderr}")
            return False
        
        return True
        
    except Exception as e:
        print(f"  ✗ Python examples test failed: {e}")
        return False

def test_package_structure():
    """Test the Python package structure"""
    print("\nTesting package structure...")
    
    try:
        # Test that pynlm package can be imported
        import pynlm
        
        # Test version
        print("  Test 1: Package version...")
        version = getattr(pynlm, '__version__', '0.1.0')
        print(f"    ✓ pynlm version: {version}")
        
        # Test core classes
        print("  Test 2: Core classes...")
        core_classes = ['Config', 'Brain', 'AgentBrain', 'SimpleWorld']
        for cls_name in core_classes:
            cls = getattr(pynlm, cls_name, None)
            if cls is not None:
                print(f"    ✓ Found class: {cls_name}")
            else:
                print(f"    ✗ Missing class: {cls_name}")
                return False
        
        # Test factory functions
        print("  Test 3: Factory functions...")
        factory_funcs = ['createDefaultConfig', 'createBrain', 'createSimpleWorld', 'createAgentBrain']
        for func_name in factory_funcs:
            func = getattr(pynlm, func_name, None)
            if callable(func):
                print(f"    ✓ Found function: {func_name}")
            else:
                print(f"    ✗ Missing function: {func_name}")
                return False
        
        return True
        
    except Exception as e:
        print(f"  ✗ Package structure test failed: {e}")
        return False

def main():
    """Run all verification tests"""
    print("=" * 60)
    print("NLM Codebase Improvement Verification")
    print("=" * 60)
    
    tests = [
        ("Config System", test_config_system),
        ("Python Package Structure", test_package_structure),
        ("Python Examples", test_python_examples),
    ]
    
    passed = 0
    total = len(tests)
    
    for test_name, test_func in tests:
        print(f"\n{'-' * 40}")
        print(f"Running: {test_name}")
        print(f"{'-' * 40}")
        
        if test_func():
            passed += 1
        else:
            print(f"  ✗ {test_name} FAILED")
    
    print(f"\n{'=' * 60}")
    print(f"Results: {passed}/{total} tests passed")
    
    if passed == total:
        print("🎉 All tests passed! NLM improvements are working correctly.")
        return 0
    else:
        print("❌ Some tests failed. Please check the implementation.")
        return 1

if __name__ == "__main__":
    sys.exit(main())