#!/usr/bin/env python3
"""
Test script demonstrating the improved NLM configuration system.
This script shows the new JSON configuration capabilities and backward compatibility.
"""

import os
import tempfile
import sys
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

import pynlm

def test_simple_config():
    """Test simple key=value configuration format (backward compatibility)"""
    print("=== Testing Simple Configuration (Backward Compatibility) ===")
    
    # Create brain with default config
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    brain.initialize()
    
    print(f"✓ Created brain with {brain.getTotalNeuronCount()} neurons")
    print(f"✓ Initial firing rate: {brain.getAverageFiringRate():.4f}")
    
    return True

def test_json_config():
    """Test JSON configuration format (new feature)"""
    print("\n=== Testing JSON Configuration (New Feature) ===")
    
    # Create configuration using JSON
    config = pynlm.createDefaultConfig()
    
    # Set various configuration values
    config.set("brain.neuron_count", 1500)
    config.set("brain.region_count", 3)
    config.set("brain.connection_probability", 0.15)
    config.set("plasticity.stdp.learning_rate", 0.002)
    config.set("plasticity.hebbian.enable", True)
    config.set("neuromod.curiosity.enable", True)
    config.set("neuromod.novelty.enable", True)
    
    # Create brain with custom configuration
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    print(f"✓ Created brain with {brain.getTotalNeuronCount()} neurons")
    print(f"✓ Custom configuration loaded successfully")
    print(f"✓ Connection probability: {config.getOr('brain.connection_probability', 0.0):.2f}")
    
    return True

def test_save_load():
    """Test saving and loading configuration"""
    print("\n=== Testing Save/Load Configuration ===")
    
    # Create a complex configuration
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 2000)
    config.set("brain.region_count", 5)
    config.set("plasticity.stdp.enable", True)
    config.set("plasticity.stdp.learning_rate", 0.001)
    config.set("neuromod.dopamine.scale", 1.5)
    config.set("development.enable", True)
    
    # Save to temporary file
    with tempfile.NamedTemporaryFile(mode='w', suffix='.json', delete=False) as f:
        config_path = f.name
        success = config.saveToFile(config_path)
        print(f"✓ Saved configuration to {config_path}")
    
    if not success:
        print("✗ Failed to save configuration")
        return False
    
    # Load from file
    new_config = pynlm.createDefaultConfig()
    success = new_config.loadFromFile(config_path)
    
    if success:
        print(f"✓ Loaded configuration from {config_path}")
        print(f"✓ Loaded neuron count: {new_config.getOr('brain.neuron_count', 0)}")
        print(f"✓ Loaded region count: {new_config.getOr('brain.region_count', 0)}")
    else:
        print(f"✗ Failed to load configuration from {config_path}")
        return False
    
    # Clean up
    os.unlink(config_path)
    
    return True

def test_configuration_summary():
    """Test configuration summary functionality"""
    print("\n=== Testing Configuration Summary ===")
    
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 1000)
    config.set("brain.connection_probability", 0.1)
    config.set("plasticity.stdp.enable", True)
    config.set("neuromod.curiosity.enable", False)
    
    # Get configuration summary
    summary = config.summary()
    print("✓ Configuration summary:")
    print(summary)
    
    return True

def test_backward_compatibility():
    """Test backward compatibility with simple config files"""
    print("\n=== Testing Backward Compatibility ===")
    
    # Create a simple config file (legacy format)
    simple_config_content = """# Simple NLM Configuration
brain.neuron_count = 500
brain.region_count = 1
brain.connection_probability = 0.05
plasticity.stdp.enable = true
"""
    
    with tempfile.NamedTemporaryFile(mode='w', suffix='.nlm', delete=False) as f:
        f.write(simple_config_content)
        simple_config_path = f.name
    
    # Load simple config
    config = pynlm.createDefaultConfig()
    success = config.loadFromFile(simple_config_path)
    
    if success:
        print(f"✓ Successfully loaded simple configuration from {simple_config_path}")
        print(f"✓ Neuron count from simple config: {config.getOr('brain.neuron_count', 0)}")
    else:
        print(f"✗ Failed to load simple configuration from {simple_config_path}")
        return False
    
    # Clean up
    os.unlink(simple_config_path)
    
    return True

def test_advanced_configuration():
    """Test advanced configuration features"""
    print("\n=== Testing Advanced Configuration Features ===")
    
    config = pynlm.createDefaultConfig()
    
    # Test with array values
    config.set("brain.v_thresh_values", [ -50.0, -52.0, -48.0 ])
    config.set("brain.v_rest_values", [ -70.0, -72.0, -68.0 ])
    
    # Test boolean values
    config.set("plasticity.stdp.enable", True)
    config.set("plasticity.hebbian.enable", False)
    config.set("neuromod.curiosity.enable", True)
    
    # Test numeric values
    config.set("brain.initial_weight_mean", 0.5)
    config.set("brain.initial_weight_std", 0.1)
    config.set("plasticity.stdp.learning_rate", 0.001)
    config.set("neuromod.dopamine.scale", 2.0)
    
    print("✓ Set various configuration value types:")
    print("  - Array values (v_thresh_values)")
    print("  - Boolean values (enable flags)")
    print("  - Numeric values (floats and ints)")
    
    # Verify values
    v_thresh_values = config.get("brain.v_thresh_values")
    if v_thresh_values:
        print(f"  - v_thresh_values array: {v_thresh_values.value()}")
    
    return True

def main():
    """Run all configuration tests"""
    print("NLM Configuration System Test Suite")
    print("=" * 50)
    
    tests = [
        test_simple_config,
        test_json_config,
        test_save_load,
        test_configuration_summary,
        test_backward_compatibility,
        test_advanced_configuration,
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
            print(f"✗ Test {test.__name__} failed with exception: {e}")
            failed += 1
    
    print("\n" + "=" * 50)
    print(f"Test Results: {passed} passed, {failed} failed")
    
    if failed == 0:
        print("🎉 All tests passed! Configuration system improvements working correctly.")
        return 0
    else:
        print("❌ Some tests failed. Please check the output above.")
        return 1

if __name__ == "__main__":
    exit(main())