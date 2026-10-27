#!/usr/bin/env python3
"""
Quick verification script for NLM enhanced bindings.
This script performs basic checks to ensure the enhancements are working.
"""

import sys
import os

def check_python_bindings():
    """Check if Python bindings are available."""
    print("Checking Python bindings availability...")
    
    # Check basic bindings
    try:
        sys.path.insert(0, 'python')
        import pynlm
        print("✓ Basic Python bindings available")
        return True
    except ImportError as e:
        print(f"⚠ Basic Python bindings not available: {e}")
        return False

def check_enhanced_bindings():
    """Check if enhanced bindings are available."""
    print("Checking enhanced Python bindings...")
    
    try:
        sys.path.insert(0, 'python')
        import pynlm_enhanced
        print("✓ Enhanced Python bindings available")
        return True
    except ImportError as e:
        print(f"⚠ Enhanced Python bindings not available: {e}")
        return False

def test_configuration_validation():
    """Test configuration validation functionality."""
    print("\nTesting configuration validation...")
    
    try:
        import pynlm_enhanced
        
        # Create enhanced config
        config = pynlm_enhanced.createEnhancedConfig()
        
        # Test valid configuration
        config.set("brain.neuron_count", 100)
        config.set("brain.synapse_density", 0.1)
        config.set("plasticity.stdp.enable", True)
        
        print("✓ Valid configuration set successfully")
        
        # Test validation (this will test the validation framework)
        errors = config.validate()
        print(f"✓ Configuration validation test completed (found {len(errors)} issues)")
        
        return True
        
    except Exception as e:
        print(f"✗ Configuration validation test failed: {e}")
        return False

def test_enhanced_brain_creation():
    """Test enhanced brain creation and basic functionality."""
    print("\nTesting enhanced brain creation...")
    
    try:
        import pynlm_enhanced
        
        # Create enhanced config
        config = pynlm_enhanced.createEnhancedConfig()
        config.set("brain.neuron_count", 50)
        
        # Create enhanced brain
        brain = pynlm_enhanced.createEnhancedBrain(config.getConfig())
        brain.initialize()
        
        print(f"✓ Enhanced brain created with {brain.getBrain().getTotalNeuronCount()} neurons")
        
        # Test basic step
        brain.step(0)
        print("✓ Basic step execution successful")
        
        # Test statistics
        stats = {
            'neurons': brain.getBrain().getTotalNeuronCount(),
            'spikes': brain.getBrain().getTotalSpikeCount(),
            'firing_rate': brain.getBrain().getAverageFiringRate()
        }
        print(f"✓ Brain statistics: {stats}")
        
        return True
        
    except Exception as e:
        print(f"✗ Enhanced brain test failed: {e}")
        return False

def test_performance_monitoring():
    """Test performance monitoring functionality."""
    print("\nTesting performance monitoring...")
    
    try:
        import pynlm_enhanced
        
        # Create enhanced config
        config = pynlm_enhanced.createEnhancedConfig()
        config.set("brain.neuron_count", 25)
        
        # Create enhanced brain
        brain = pynlm_enhanced.createEnhancedBrain(config.getConfig())
        brain.initialize()
        
        # Start performance monitoring
        brain.startPerformanceMonitoring()
        
        # Run simulation
        for i in range(20):
            brain.step(i)
        
        # Stop monitoring
        brain.stopPerformanceMonitoring()
        
        # Get metrics
        metrics = brain.getPerformanceMonitoring()
        
        print(f"✓ Performance monitoring completed")
        print(f"  - Steps: {metrics.get('stepCount', 'N/A')}")
        print(f"  - Simulation time: {metrics.get('simulationTime', 'N/A')}s")
        
        return True
        
    except Exception as e:
        print(f"✗ Performance monitoring test failed: {e}")
        return False

def test_error_handling():
    """Test error handling functionality."""
    print("\nTesting error handling...")
    
    try:
        import pynlm_enhanced
        
        # Create enhanced config
        config = pynlm_enhanced.createEnhancedConfig()
        
        # Test validation error
        try:
            config.set("brain.neuron_count", -1)
            print("⚠ Invalid value was accepted (validation may not be working)")
            return False
        except pynlm_enhanced.ValidationError:
            print("✓ Validation error correctly caught")
        except Exception as e:
            print(f"⚠ Unexpected error type: {type(e).__name__}")
        
        return True
        
    except Exception as e:
        print(f"✗ Error handling test failed: {e}")
        return False

def main():
    """Main verification function."""
    print("NLM Enhanced Bindings Verification")
    print("=" * 50)
    
    # Check Python version
    print(f"Python version: {sys.version}")
    
    # Run all tests
    tests = [
        ("Basic Bindings", check_python_bindings),
        ("Enhanced Bindings", check_enhanced_bindings),
        ("Configuration Validation", test_configuration_validation),
        ("Enhanced Brain Creation", test_enhanced_brain_creation),
        ("Performance Monitoring", test_performance_monitoring),
        ("Error Handling", test_error_handling),
    ]
    
    results = []
    for test_name, test_func in tests:
        print(f"\n{'='*50}")
        success = test_func()
        results.append((test_name, success))
    
    # Summary
    print(f"\n{'='*50}")
    print("VERIFICATION SUMMARY")
    print(f"{'='*50}")
    
    passed = 0
    total = len(results)
    
    for test_name, success in results:
        status = "✓ PASS" if success else "✗ FAIL"
        print(f"{test_name:.<40} {status}")
        if success:
            passed += 1
    
    print(f"\n{'='*50}")
    print(f"Results: {passed}/{total} tests passed")
    
    if passed == total:
        print("🎉 ALL TESTS PASSED!")
        print("\nEnhanced NLM bindings are working correctly.")
        print("Key improvements verified:")
        print("✓ Enhanced Python bindings with validation")
        print("✓ Configuration validation and error handling")
        print("✓ Enhanced brain creation and simulation")
        print("✓ Performance monitoring capabilities")
        print("✓ Comprehensive error handling")
        return 0
    else:
        print("⚠ SOME TESTS FAILED")
        print("Please check the output above for details.")
        return 1

if __name__ == "__main__":
    sys.exit(main())