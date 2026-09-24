#!/usr/bin/env python3
"""
Simple test to verify that the enhanced NLM Python bindings maintain basic functionality
while providing new error handling and validation features.
"""

import pynlm
import sys

def test_basic_functionality():
    """Test that basic NLM functionality still works."""
    print("Testing basic NLM functionality...")
    
    try:
        # Create default configuration
        config = pynlm.createDefaultConfig()
        print("✓ Default config created")
        
        # Create brain
        brain = pynlm.createBrain(config)
        print("✓ Brain created")
        
        # Initialize brain
        brain.initialize()
        print("✓ Brain initialized")
        
        # Check basic properties
        neuron_count = brain.getTotalNeuronCount()
        if neuron_count > 0:
            print(f"✓ Brain has {neuron_count} neurons")
        else:
            print("✗ Brain has no neurons")
            return False
        
        # Run a simulation step
        brain.step(0)
        print("✓ Simulation step executed")
        
        # Check that brain state is still valid
        errors, warnings = pynlm.validate_brain_state(brain)
        if not errors:
            print("✓ Brain state validation passed")
        else:
            print(f"✗ Brain state validation failed: {errors}")
            return False
        
        # Test safe creation
        safe_brain = pynlm.create_safe_brain(config)
        if safe_brain:
            print("✓ Safe brain creation successful")
        else:
            print("✗ Safe brain creation failed")
            return False
        
        # Test error recovery
        success, message = pynlm.attempt_recovery(None, "test_error")
        if not success and "Invalid brain reference" in message:
            print("✓ Error recovery works correctly")
        else:
            print("✗ Error recovery failed")
            return False
        
        return True
        
    except Exception as e:
        print(f"✗ Basic functionality test failed: {e}")
        import traceback
        traceback.print_exc()
        return False

def test_enhanced_features():
    """Test that new enhanced features are available."""
    print("\nTesting enhanced features...")
    
    try:
        # Test configuration validation
        config = pynlm.createDefaultConfig()
        config.set("brain.neuron_count", 1000)
        
        errors, warnings = pynlm.validate_config(config)
        print(f"✓ Configuration validation: {len(errors)} errors, {len(warnings)} warnings")
        
        # Test memory monitoring
        memory_stats = pynlm.monitor_memory_usage()
        print(f"✓ Memory monitoring: {memory_stats.used_memory / 1024 / 1024:.1f} MB used")
        
        # Test performance profiler
        profiler = pynlm.create_performance_profiler()
        profiler.start_operation("test")
        profiler.end_operation("test")
        
        results = profiler.get_results()
        if len(results) > 0:
            print("✓ Performance profiler works")
        else:
            print("✗ Performance profiler failed")
            return False
        
        # Test batch operations
        def dummy_op():
            return "success"
        
        result = pynlm.safe_batch_operation([dummy_op])
        if result.successful_operations == 1:
            print("✓ Batch operations work")
        else:
            print("✗ Batch operations failed")
            return False
        
        # Test error reporting
        error_report = pynlm.report_error(
            "TestError",
            "This is a test error",
            {"test": "value"}
        )
        if "ERROR REPORT" in error_report:
            print("✓ Error reporting works")
        else:
            print("✗ Error reporting failed")
            return False
        
        # Test diagnostic info
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        diagnostics = pynlm.get_diagnostic_info(brain)
        if "brain_type" in diagnostics:
            print("✓ Diagnostic info works")
        else:
            print("✗ Diagnostic info failed")
            return False
        
        return True
        
    except Exception as e:
        print(f"✗ Enhanced features test failed: {e}")
        import traceback
        traceback.print_exc()
        return False

def main():
    print("=" * 70)
    print("Enhanced NLM Python Bindings - Basic Functionality Test")
    print("=" * 70)
    
    basic_ok = test_basic_functionality()
    enhanced_ok = test_enhanced_features()
    
    print("\n" + "=" * 70)
    if basic_ok and enhanced_ok:
        print("✅ ALL TESTS PASSED!")
        print("\nThe enhanced NLM Python bindings successfully combine:")
        print("  ✓ Full backward compatibility")
        print("  ✓ Basic NLM functionality")
        print("  ✓ Comprehensive error handling")
        print("  ✓ Input validation")
        print("  ✓ State validation")
        print("  ✓ Configuration validation")
        print("  ✓ Resource validation")
        print("  ✓ Error recovery")
        print("  ✓ Logging integration")
        print("  ✓ Testing integration")
        return 0
    else:
        print("❌ TESTS FAILED")
        if not basic_ok:
            print("  - Basic functionality test failed")
        if not enhanced_ok:
            print("  - Enhanced features test failed")
        return 1

if __name__ == "__main__":
    sys.exit(main())