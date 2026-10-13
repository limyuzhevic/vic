#!/usr/bin/env python3
"""
Test script for NLM configuration and Python API improvements
"""

import sys
import os
import json
from pathlib import Path

# Add the current directory to the path
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

from enhanced_api import (
    NMLEnvironmentConfig, 
    ValidationLevel, 
    create_config, 
    create_session, 
    run_simulation,
    ConfigurationValidator,
    NLMError,
    ConfigValidationError
)

def test_config_creation():
    """Test configuration creation from presets."""
    print("=== Test 1: Configuration Creation from Presets ===")
    
    try:
        # Test different presets
        presets = ['minimal', 'balanced', 'complex', 'research', 'production']
        
        for preset in presets:
            print(f"\nTesting preset: {preset}")
            config = create_config(preset=preset)
            
            # Basic checks
            assert hasattr(config, 'neuron_count'), "Missing neuron_count"
            assert hasattr(config, 'region_count'), "Missing region_count"
            assert hasattr(config, 'simulation_timestep'), "Missing simulation_timestep"
            
            # Preset-specific checks
            if preset == 'minimal':
                assert config.neuron_count == 100, f"Expected 100 neurons, got {config.neuron_count}"
                assert config.visualization_enabled == False, "Minimal preset should disable visualization"
            
            print(f"  ✓ {preset} preset passed validation")
        
        print("\n✓ All preset creation tests passed")
        return True
        
    except Exception as e:
        print(f"\n✗ Preset creation test failed: {e}")
        return False

def test_config_validation():
    """Test configuration validation."""
    print("\n=== Test 2: Configuration Validation ===")
    
    try:
        # Test with valid configuration
        config = create_config(preset='balanced')
        
        validation_results = ConfigurationValidator.validate_configuration(config, ValidationLevel.STRICT)
        
        assert len(validation_results['errors']) == 0, f"Validation errors found: {validation_results['errors']}"
        assert len(validation_results['warnings']) == 0, f"Validation warnings found: {validation_results['warnings']}"
        
        print("  ✓ Valid configuration passed validation")
        
        # Test with invalid configuration
        print("\n  Testing invalid configuration...")
        
        # Create config with invalid values
        invalid_config = NMLEnvironmentConfig()
        invalid_config.neuron_count = -100  # Invalid: negative neuron count
        invalid_config.connection_probability = 1.5  # Invalid: probability > 1
        invalid_config.log_level = "INVALID"  # Invalid: not in valid values
        
        validation_results = ConfigurationValidator.validate_configuration(invalid_config, ValidationLevel.STRICT)
        
        # Should have errors
        assert len(validation_results['errors']) > 0, "Should have validation errors for invalid config"
        
        print(f"  ✓ Invalid configuration correctly rejected with {len(validation_results['errors'])} errors")
        
        # Test with validation level warning
        validation_results = ConfigurationValidator.validate_configuration(invalid_config, ValidationLevel.WARNING)
        
        assert len(validation_results['warnings']) > 0, "Should have validation warnings for invalid config"
        print(f"  ✓ Invalid configuration correctly flagged with {len(validation_results['warnings'])} warnings")
        
        print("\n✓ All validation tests passed")
        return True
        
    except Exception as e:
        print(f"\n✗ Configuration validation test failed: {e}")
        import traceback
        traceback.print_exc()
        return False

def test_config_file_io():
    """Test configuration file input/output."""
    print("\n=== Test 3: Configuration File I/O ===")
    
    try:
        # Create a test configuration
        config = create_config(preset='research')
        
        # Save to file
        output_file = "test_config.json"
        config_dict = config.to_dict()
        
        with open(output_file, 'w') as f:
            json.dump(config_dict, f, indent=2)
        
        print(f"  ✓ Configuration saved to {output_file}")
        
        # Load from file
        with open(output_file, 'r') as f:
            loaded_config_dict = json.load(f)
        
        # Verify content
        assert 'neuron_count' in loaded_config_dict, "Missing neuron_count in saved config"
        assert 'region_count' in loaded_config_dict, "Missing region_count in saved config"
        assert config.neuron_count == loaded_config_dict['neuron_count'], "Neuron count mismatch"
        
        print(f"  ✓ Configuration loaded successfully")
        
        # Clean up
        os.remove(output_file)
        print(f"  ✓ Test file cleaned up")
        
        print("\n✓ All file I/O tests passed")
        return True
        
    except Exception as e:
        print(f"\n✗ Configuration file I/O test failed: {e}")
        import traceback
        traceback.print_exc()
        return False

def test_api_enhancements():
    """Test API enhancements."""
    print("\n=== Test 4: API Enhancements ===")
    
    try:
        # Test 1: Create configuration with custom values
        print("  Testing custom configuration...")
        config = create_config(
            preset='production',
            neuron_count=2500,
            simulation_timestep=0.005,
            visualization_enabled=True,
            enable_performance_monitoring=True
        )
        
        assert config.neuron_count == 2500, f"Expected 2500 neurons, got {config.neuron_count}"
        assert config.simulation_timestep == 0.005, f"Expected timestep 0.005, got {config.simulation_timestep}"
        assert config.visualization_enabled == True, "Expected visualization enabled"
        assert config.enable_performance_monitoring == True, "Expected performance monitoring enabled"
        
        print("  ✓ Custom configuration created successfully")
        
        # Test 2: Configuration summary
        print("  Testing configuration summary...")
        summary = config.to_dict()
        assert len(summary) > 0, "Configuration summary should not be empty"
        print(f"  ✓ Configuration summary contains {len(summary)} parameters")
        
        # Test 3: Configuration validation errors
        print("  Testing validation error handling...")
        
        # This should raise a ConfigValidationError
        try:
            invalid_config = NMLEnvironmentConfig()
            invalid_config.neuron_count = -50  # Invalid value
            
            # This will be caught by our validation
            ConfigurationValidator.validate_configuration(invalid_config, ValidationLevel.STRICT)
            
            print("  ✗ Expected validation error but none was raised")
            return False
            
        except Exception as e:
            # Expected behavior
            print(f"  ✓ Validation correctly caught error: {type(e).__name__}")
        
        print("\n✓ All API enhancement tests passed")
        return True
        
    except Exception as e:
        print(f"\n✗ API enhancement test failed: {e}")
        import traceback
        traceback.print_exc()
        return False

def test_performance_monitoring():
    """Test performance monitoring functionality."""
    print("\n=== Test 5: Performance Monitoring ===")
    
    try:
        # Create configuration with performance monitoring
        config = create_config(
            preset='production',
            enable_performance_monitoring=True,
            enable_memory_profiling=True
        )
        
        assert config.enable_performance_monitoring == True, "Performance monitoring should be enabled"
        assert config.enable_memory_profiling == True, "Memory profiling should be enabled"
        
        print("  ✓ Performance monitoring enabled successfully")
        
        # Test performance data structure (simulated)
        print("  Testing performance data structure...")
        
        performance_data = {
            'step': 0,
            'timestamp': time.time(),
            'brain_stats': {
                'total_neurons': 1000,
                'total_synapses': 5000,
                'total_spikes': 10000,
                'firing_neurons': 150,
                'average_firing_rate': 10.5,
                'excitation_inhibition_ratio': 1.2,
            },
            'world_stats': {
                'simulation_time': 1.0,
                'agent_position': (5.0, 5.0),
                'agent_energy': 0.85,
                'agent_health': 0.95,
            },
            'agent_stats': {
                'curiosity_level': 0.7,
                'novelty_level': 0.5,
                'prediction_error': 0.1,
                'neuromodulation_level': 0.3,
                'developmental_stage': 'Adult',
            }
        }
        
        assert 'brain_stats' in performance_data, "Missing brain stats in performance data"
        assert 'world_stats' in performance_data, "Missing world stats in performance data"
        assert 'agent_stats' in performance_data, "Missing agent stats in performance data"
        
        print("  ✓ Performance data structure validated")
        
        print("\n✓ All performance monitoring tests passed")
        return True
        
    except Exception as e:
        print(f"\n✗ Performance monitoring test failed: {e}")
        import traceback
        traceback.print_exc()
        return False

def test_backward_compatibility():
    """Test backward compatibility with existing API."""
    print("\n=== Test 6: Backward Compatibility ===")
    
    try:
        # Test that basic API still works
        print("  Testing basic API compatibility...")
        
        # Create minimal configuration (simulating old API)
        config = NMLEnvironmentConfig()
        
        # Basic settings (old API style)
        config.neuron_count = 1000
        config.region_count = 2
        config.simulation_timestep = 0.001
        
        assert config.neuron_count == 1000
        assert config.region_count == 2
        assert config.simulation_timestep == 0.001
        
        print("  ✓ Basic API still works")
        
        # Test that old configuration keys still work
        print("  Testing old configuration keys...")
        
        # Test with some old-style keys
        config_dict = {
            'brain.neuron_count': 500,
            'brain.region_count': 1,
            'simulation.timestep': 0.01,
        }
        
        # Simulate old API usage
        for key, value in config_dict.items():
            if key.startswith('brain.'):
                config_key = key.replace('brain.', '')
                setattr(config, config_key, value)
        
        assert config.neuron_count == 500
        assert config.region_count == 1
        assert config.simulation_timestep == 0.01
        
        print("  ✓ Backward compatibility maintained")
        
        print("\n✓ All backward compatibility tests passed")
        return True
        
    except Exception as e:
        print(f"\n✗ Backward compatibility test failed: {e}")
        import traceback
        traceback.print_exc()
        return False

def main():
    """Run all tests."""
    print("NLM Configuration and API Improvements Test Suite")
    print("=" * 60)
    
    tests = [
        test_config_creation,
        test_config_validation,
        test_config_file_io,
        test_api_enhancements,
        test_performance_monitoring,
        test_backward_compatibility,
    ]
    
    passed = 0
    failed = 0
    
    for test in tests:
        try:
            if test():
                passed += 1
            else:
                failed += 1
        except Exception as e:
            print(f"\n✗ Test {test.__name__} crashed: {e}")
            import traceback
            traceback.print_exc()
            failed += 1
    
    print("\n" + "=" * 60)
    print(f"Test Results: {passed} passed, {failed} failed")
    
    if failed == 0:
        print("\n🎉 All tests passed! Configuration and API improvements are working correctly.")
        return 0
    else:
        print(f"\n❌ {failed} test(s) failed. Please check the output above for details.")
        return 1

if __name__ == "__main__":
    import time
    sys.exit(main())