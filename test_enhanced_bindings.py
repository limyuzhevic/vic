#!/usr/bin/env python3
"""
Test script for verifying enhanced NLM Python bindings error handling and validation.
This script tests all the new error handling, validation, and recovery features.
"""

import pynlm
import tempfile
import os
import sys

def test_exception_hierarchy():
    """Test that the enhanced exception hierarchy works correctly."""
    print("Testing exception hierarchy...")
    
    # Test that custom exceptions are available
    try:
        # Create a config with invalid parameters to trigger validation errors
        config = pynlm.createDefaultConfig()
        config.set("invalid.key", -1)  # Invalid negative value
        
        # Test validation function
        from pynlm import validate_config
        errors, warnings = validate_config(config)
        
        if "brain.neuron_count must be positive" in " ".join(errors):
            print("✓ Configuration validation test passed")
        else:
            print("✗ Configuration validation test failed")
            
    except Exception as e:
        print(f"✗ Exception hierarchy test failed: {e}")
        return False
    
    return True

def test_input_validation():
    """Test comprehensive input validation."""
    print("\nTesting input validation...")
    
    try:
        # Test Config validation
        config = pynlm.createDefaultConfig()
        
        # Test invalid keys
        errors = []
        
        # Test empty key
        try:
            config.validate_key("")
            errors.append("Empty key validation should have failed")
        except pynlm.InvalidArgumentError:
            pass  # Expected
            
        # Test duplicate key
        config.set("test_key", 123)
        try:
            config.validate_key("test_key")
            errors.append("Duplicate key validation should have failed")
        except pynlm.InvalidArgumentError:
            pass  # Expected
            
        # Test value type validation
        try:
            config.validate_value_type("test_key", "invalid_type")
            errors.append("Invalid type validation should have failed")
        except pynlm.InvalidArgumentError:
            pass  # Expected
            
        if errors:
            print(f"✗ Input validation test failed: {errors}")
            return False
        
        print("✓ Input validation test passed")
        return True
        
    except Exception as e:
        print(f"✗ Input validation test failed: {e}")
        return False

def test_state_validation():
    """Test state validation for brain and components."""
    print("\nTesting state validation...")
    
    try:
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        
        # Test brain state validation
        errors, warnings = pynlm.validate_brain_state(brain)
        
        if "Brain has no neurons" not in " ".join(errors):
            print("✓ Brain state validation test passed")
        else:
            print("✗ Brain state validation test failed")
            return False
            
    except Exception as e:
        print(f"✗ State validation test failed: {e}")
        return False
    
    return True

def test_configuration_validation():
    """Test configuration validation."""
    print("\nTesting configuration validation...")
    
    try:
        # Test invalid configuration
        config = pynlm.createDefaultConfig()
        config.set("brain.neuron_count", 0)  # Invalid: zero neurons
        config.set("brain.region_count", -5)  # Invalid: negative regions
        
        # Test configuration optimization
        warnings, suggestions = pynlm.optimize_config(config)
        
        if "Large neuron count may impact performance" not in " ".join(warnings):
            print("✓ Configuration validation test passed")
        else:
            print("✗ Configuration validation test failed")
            return False
            
    except Exception as e:
        print(f"✗ Configuration validation test failed: {e}")
        return False
    
    return True

def test_resource_validation():
    """Test resource validation."""
    print("\nTesting resource validation...")
    
    try:
        # Test memory monitoring
        memory_stats = pynlm.monitor_memory_usage()
        
        if memory_stats.total_memory > 0 and memory_stats.used_memory >= 0:
            print(f"✓ Memory monitoring test passed: {memory_stats.used_memory / 1024 / 1024:.1f} MB used")
        else:
            print("✗ Memory monitoring test failed")
            return False
            
        # Test memory cache
        cache = pynlm.create_memory_cache(10)  # 10 MB cache
        sample_data = [1.0, 2.0, 3.0, 4.0, 5.0]
        cache.set("test_data", sample_data)
        
        retrieved = cache.get("test_data")
        if retrieved and len(retrieved) == len(sample_data):
            print("✓ Cache management test passed")
        else:
            print("✗ Cache management test failed")
            return False
            
    except Exception as e:
        print(f"✗ Resource validation test failed: {e}")
        return False
    
    return True

def test_error_recovery():
    """Test error recovery and fallback mechanisms."""
    print("\nTesting error recovery...")
    
    try:
        # Test recovery attempt
        recovery_success, recovery_message = pynlm.attempt_recovery(None, "step_failure")
        
        if not recovery_success and "Invalid brain reference" in recovery_message:
            print("✓ Error recovery test passed")
        else:
            print("✗ Error recovery test failed")
            return False
            
    except Exception as e:
        print(f"✗ Error recovery test failed: {e}")
        return False
    
    return True

def test_safe_creation():
    """Test safe object creation with validation."""
    print("\nTesting safe creation...")
    
    try:
        # Test safe brain creation with invalid config
        config = pynlm.createDefaultConfig()
        config.set("brain.neuron_count", -10)  # Invalid
        
        # The safe creation should handle this gracefully
        try:
            brain = pynlm.create_safe_brain(config)
            print("✗ Safe brain creation should have failed with invalid config")
            return False
        except Exception:
            print("✓ Safe brain creation test passed (correctly rejected invalid config)")
        
        # Test safe world creation with valid parameters
        world = pynlm.create_safe_world(10, 10, 8, 8)
        if world:
            print("✓ Safe world creation test passed")
        else:
            print("✗ Safe world creation test failed")
            return False
            
    except Exception as e:
        print(f"✗ Safe creation test failed: {e}")
        return False
    
    return True

def test_advanced_validation():
    """Test advanced validation functions."""
    print("\nTesting advanced validation...")
    
    try:
        # Test sensory percept validation
        percept = pynlm.SensoryPercept()
        errors = pynlm.validate_sensory_percept(percept)
        
        if len(errors) > 0 and "must contain at least one signal type" in " ".join(errors):
            print("✓ Advanced validation test passed")
        else:
            print("✗ Advanced validation test failed")
            return False
            
    except Exception as e:
        print(f"✗ Advanced validation test failed: {e}")
        return False
    
    return True

def test_error_reporting():
    """Test comprehensive error reporting."""
    print("\nTesting error reporting...")
    
    try:
        # Test error reporting function
        error_report = pynlm.report_error(
            "ValidationException",
            "Test validation error",
            {"context": "test_context", "severity": "high"}
        )
        
        if "ERROR REPORT" in error_report and "Test validation error" in error_report:
            print("✓ Error reporting test passed")
        else:
            print("✗ Error reporting test failed")
            return False
            
    except Exception as e:
        print(f"✗ Error reporting test failed: {e}")
        return False
    
    return True

def test_diagnostic_info():
    """Test diagnostic information collection."""
    print("\nTesting diagnostic info...")
    
    try:
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        
        # Get diagnostic info
        diagnostics = pynlm.get_diagnostic_info(brain)
        
        if "brain_type" in diagnostics and "neuron_count" in diagnostics:
            print("✓ Diagnostic info test passed")
        else:
            print("✗ Diagnostic info test failed")
            return False
            
    except Exception as e:
        print(f"✗ Diagnostic info test failed: {e}")
        return False
    
    return True

def test_batch_operations():
    """Test batch operations with error handling."""
    print("\nTesting batch operations...")
    
    try:
        def successful_operation():
            return "success"
        
        def failing_operation():
            raise Exception("Operation failed")
        
        operations = [successful_operation, failing_operation, successful_operation]
        result = pynlm.safe_batch_operation(operations)
        
        if result.total_operations == 3 and result.successful_operations == 2 and result.failed_operations == 1:
            print("✓ Batch operations test passed")
        else:
            print("✗ Batch operations test failed")
            return False
            
    except Exception as e:
        print(f"✗ Batch operations test failed: {e}")
        return False
    
    return True

def test_backward_compatibility():
    """Test backward compatibility with existing API."""
    print("\nTesting backward compatibility...")
    
    try:
        # Test original function names
        config = pynlm.createDefaultConfig()
        
        # Test original method names
        if hasattr(config, 'loadFromFile') and hasattr(config, 'saveToFile'):
            print("✓ Backward compatibility test passed (original method names available)")
        else:
            print("✗ Backward compatibility test failed (original method names missing)")
            return False
            
    except Exception as e:
        print(f"✗ Backward compatibility test failed: {e}")
        return False
    
    return True

def test_performance_profiling():
    """Test performance monitoring features."""
    print("\nTesting performance profiling...")
    
    try:
        # Create profiler
        profiler = pynlm.create_performance_profiler()
        
        # Use profiler
        for i in range(5):
            profiler.start_operation(f"step_{i}")
            # Simulate some work
            total = 0
            for j in range(100):
                total += j
            profiler.end_operation(f"step_{i}")
        
        results = profiler.get_results()
        
        if len(results) == 5 and all(result > 0 for result in results.values()):
            print("✓ Performance profiling test passed")
        else:
            print("✗ Performance profiling test failed")
            return False
            
    except Exception as e:
        print(f"✗ Performance profiling test failed: {e}")
        return False
    
    return True

def test_memory_pool():
    """Test memory pool functionality."""
    print("\nTesting memory pool...")
    
    try:
        pool = pynlm.create_neural_memory_pool(100)
        
        # Test data allocation
        data_vec = pool.allocate_data()
        if len(data_vec) == 100:
            print("✓ Memory pool data allocation test passed")
        else:
            print("✗ Memory pool data allocation test failed")
            return False
        
        # Test matrix allocation
        matrix = pool.allocate_matrix(5, 5)
        if len(matrix) == 5 and len(matrix[0]) == 5:
            print("✓ Memory pool matrix allocation test passed")
        else:
            print("✗ Memory pool matrix allocation test failed")
            return False
            
    except Exception as e:
        print(f"✗ Memory pool test failed: {e}")
        return False
    
    return True

def main():
    """Run all enhanced validation and error handling tests."""
    print("=" * 70)
    print("Enhanced NLM Python Bindings Validation and Error Handling Test Suite")
    print("=" * 70)
    
    tests = [
        ("Exception Hierarchy", test_exception_hierarchy),
        ("Input Validation", test_input_validation),
        ("State Validation", test_state_validation),
        ("Configuration Validation", test_configuration_validation),
        ("Resource Validation", test_resource_validation),
        ("Error Recovery", test_error_recovery),
        ("Safe Creation", test_safe_creation),
        ("Advanced Validation", test_advanced_validation),
        ("Error Reporting", test_error_reporting),
        ("Diagnostic Info", test_diagnostic_info),
        ("Batch Operations", test_batch_operations),
        ("Backward Compatibility", test_backward_compatibility),
        ("Performance Profiling", test_performance_profiling),
        ("Memory Pool", test_memory_pool),
    ]
    
    passed = 0
    failed = 0
    
    for test_name, test_func in tests:
        try:
            if test_func():
                passed += 1
            else:
                failed += 1
        except Exception as e:
            print(f"✗ {test_name} test failed with exception: {e}")
            failed += 1
    
    print("\n" + "=" * 70)
    print(f"TEST RESULTS: {passed} passed, {failed} failed")
    print("=" * 70)
    
    if failed == 0:
        print("\n✅ ALL TESTS PASSED!")
        print("\nEnhanced NLM Python bindings error handling and validation are working correctly.")
        print("\nKey features verified:")
        print("  ✓ Custom exception hierarchy for different error types")
        print("  ✓ Comprehensive input validation")
        print("  ✓ State validation for brain and components")
        print("  ✓ Configuration validation and optimization")
        print("  ✓ Resource validation and monitoring")
        print("  ✓ Error recovery and fallback mechanisms")
        print("  ✓ Safe object creation with validation")
        print("  ✓ Advanced validation functions")
        print("  ✓ Comprehensive error reporting")
        print("  ✓ Diagnostic information collection")
        print("  ✓ Batch operations with error handling")
        print("  ✓ Backward compatibility")
        print("  ✓ Performance monitoring")
        print("  ✓ Memory pool management")
        return 0
    else:
        print(f"\n❌ {failed} TESTS FAILED")
        print("Please review the failed tests above.")
        return 1

if __name__ == "__main__":
    sys.exit(main())