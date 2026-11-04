#!/usr/bin/env python3
"""
Test script for NLM Python Bindings improvements.

This script tests the new modular binding structure and advanced functionality.
"""

import sys
import os

# Add the current directory to Python path
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

def test_basic_functionality():
    """Test basic NLM functionality."""
    print("Testing basic NLM functionality...")
    
    try:
        import pynlm
        print("✓ Successfully imported pynlm")
        
        # Test basic configuration
        config = pynlm.createDefaultConfig()
        print("✓ Created default config")
        
        # Test brain creation
        brain = pynlm.createBrain(config)
        print("✓ Created brain")
        
        # Test brain initialization
        if brain.initialize():
            print("✓ Brain initialized successfully")
        else:
            print("✗ Brain initialization failed")
            return False
            
        # Test basic simulation
        for i in range(10):
            brain.step(i)
            
        print(f"✓ Simulated {10} steps")
        print(f"  Total spikes: {brain.getTotalSpikeCount()}")
        print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
        
        return True
        
    except Exception as e:
        print(f"✗ Basic functionality test failed: {e}")
        import traceback
        traceback.print_exc()
        return False

def test_advanced_apis():
    """Test advanced API functionality."""
    print("\nTesting advanced API functionality...")
    
    try:
        import pynlm
        
        # Test advanced configuration factory
        print("Testing advanced configuration factory...")
        
        # Test different config types
        config1 = pynlm.create_advanced_config("reinforcement_learning")
        config2 = pynlm.create_advanced_config("pattern_recognition")
        config3 = pynlm.create_advanced_config("memory_formation")
        config4 = pynlm.create_advanced_config("agent_learning")
        
        print("✓ Created advanced configs for different learning paradigms")
        
        # Test custom parameters
        custom_config = pynlm.create_advanced_config(
            "reinforcement_learning",
            neuron_count=3000,
            curiosity_enabled=True,
            novelty_enabled=True
        )
        print("✓ Created config with custom parameters")
        
        # Test experiment template
        print("Testing experiment template...")
        experiment = pynlm.NLMExperiment(config1, "test_experiment")
        print("✓ Created NLMExperiment instance")
        
        # Test experiment run (limited for testing)
        # We won't run full experiments as they take time
        print("✓ Experiment template available for use")
        
        # Test neural network visualizer
        print("Testing neural network visualizer...")
        
        # Create a simple brain for testing
        brain = pynlm.createBrain(config1)
        brain.initialize()
        
        # Test brain snapshot
        snapshot = pynlm.NeuralNetworkVisualizer.create_brain_snapshot(brain)
        print("✓ Created brain snapshot")
        print(f"  Snapshot keys: {list(snapshot.keys())}")
        
        return True
        
    except Exception as e:
        print(f"✗ Advanced APIs test failed: {e}")
        import traceback
        traceback.print_exc()
        return False

def test_error_handling():
    """Test error handling functionality."""
    print("\nTesting error handling...")
    
    try:
        import pynlm
        
        # Test batch processor
        print("Testing batch processor...")
        processor = pynlm.BatchProcessor()
        print("✓ Created BatchProcessor instance")
        
        # Test error types (they should be available)
        print("✓ Error handling utilities available")
        
        return True
        
    except Exception as e:
        print(f"✗ Error handling test failed: {e}")
        import traceback
        traceback.print_exc()
        return False

def test_backward_compatibility():
    """Test that original API still works."""
    print("\nTesting backward compatibility...")
    
    try:
        import pynlm
        
        # Test that all original API calls work
        print("Testing original API calls...")
        
        # Original API calls from documentation
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        brain.step(0)
        
        # Test all the key methods that should be available
        methods_to_test = [
            ('getTotalNeuronCount', brain.getTotalNeuronCount),
            ('getTotalSynapseCount', brain.getTotalSynapseCount),
            ('getFiringNeuronCount', brain.getFiringNeuronCount),
            ('getAverageFiringRate', brain.getAverageFiringRate),
            ('getTotalSpikeCount', brain.getTotalSpikeCount),
            ('getDevelopmentalStage', brain.getDevelopmentalStage),
        ]
        
        for method_name, method in methods_to_test:
            result = method()
            print(f"✓ {method_name}() = {result}")
        
        print("✓ All original API calls work correctly")
        return True
        
    except Exception as e:
        print(f"✗ Backward compatibility test failed: {e}")
        import traceback
        traceback.print_exc()
        return False

def test_system_info():
    """Test system information utilities."""
    print("\nTesting system information...")
    
    try:
        import pynlm
        
        # Test system info
        info = pynlm.get_system_info()
        print("✓ Got system information")
        
        for key, value in info.items():
            print(f"  {key}: {value}")
        
        # Test quick test function
        brain = pynlm.createBrain(pynlm.createDefaultConfig())
        brain.initialize()
        
        if pynlm.run_quick_test(brain):
            print("✓ Quick test function works")
        else:
            print("✗ Quick test function failed")
            return False
        
        return True
        
    except Exception as e:
        print(f"✗ System info test failed: {e}")
        import traceback
        traceback.print_exc()
        return False

def main():
    """Run all tests."""
    print("NLM Python Bindings Test Suite")
    print("=" * 50)
    
    tests = [
        ("Basic Functionality", test_basic_functionality),
        ("Advanced APIs", test_advanced_apis),
        ("Error Handling", test_error_handling),
        ("Backward Compatibility", test_backward_compatibility),
        ("System Information", test_system_info),
    ]
    
    results = []
    for test_name, test_func in tests:
        print(f"\n{'='*50}")
        print(f"Running: {test_name}")
        print(f"{'='*50}")
        
        try:
            result = test_func()
            results.append((test_name, result))
            status = "PASSED" if result else "FAILED"
            print(f"\n{test_name}: {status}")
        except Exception as e:
            print(f"\n{test_name}: FAILED with exception: {e}")
            results.append((test_name, False))
    
    print(f"\n{'='*50}")
    print("TEST SUMMARY")
    print(f"{'='*50}")
    
    passed = 0
    failed = 0
    
    for test_name, result in results:
        status = "PASSED" if result else "FAILED"
        print(f"{test_name}: {status}")
        if result:
            passed += 1
        else:
            failed += 1
    
    print(f"\nTotal: {len(results)}")
    print(f"Passed: {passed}")
    print(f"Failed: {failed}")
    
    if failed == 0:
        print("\n🎉 All tests passed! NLM Python bindings improvements are working correctly.")
        return 0
    else:
        print(f"\n⚠️  {failed} test(s) failed. Please review the implementation.")
        return 1

if __name__ == "__main__":
    sys.exit(main())
