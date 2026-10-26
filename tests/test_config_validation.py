#!/usr/bin/env python3
"""
Configuration Validation Test for NLM

This script tests the configuration validation functionality
added to the NLM framework.
"""

import sys
import os
import tempfile
import json
import yaml

# Add the src directory to the path so we can import NLM modules
sys.path.insert(0, os.path.join(os.path.dirname(__file__), 'src'))

import nlm
from nlm import Config, ConfigSource

def test_basic_validation():
    """Test basic configuration validation."""
    print("=== Testing Basic Configuration Validation ===")
    
    config = Config()
    
    # Test valid configuration values
    print("Testing valid configuration values...")
    try:
        config.set("neuron_count", 1000, ConfigSource::Runtime)
        print("✓ Valid neuron_count accepted")
    except Exception as e:
        print(f"✗ Unexpected error: {e}")
        return False
    
    try:
        config.set("simulation_timestep", 0.001, ConfigSource::Runtime)
        print("✓ Valid simulation_timestep accepted")
    except Exception as e:
        print(f"✗ Unexpected error: {e}")
        return False
    
    try:
        config.set("plasticity.learning_rate", 0.01, ConfigSource::Runtime)
        print("✓ Valid plasticity.learning_rate accepted")
    except Exception as e:
        print(f"✗ Unexpected error: {e}")
        return False
    
    try:
        config.set("dopamine_baseline", 0.1, ConfigSource::Runtime)
        print("✓ Valid dopamine_baseline accepted")
    except Exception as e:
        print(f"✗ Unexpected error: {e}")
        return False
    
    # Test invalid neuron_count
    print("\nTesting invalid neuron_count...")
    try:
        config.set("neuron_count", 0, ConfigSource::Runtime)
        print("✗ Invalid neuron_count (0) should have been rejected")
        return False
    except std::invalid_argument as e:
        print(f"✓ Invalid neuron_count correctly rejected: {e}")
    except Exception as e:
        print(f"✗ Unexpected error type: {e}")
        return False
    
    # Test invalid simulation_timestep
    print("\nTesting invalid simulation_timestep...")
    try:
        config.set("simulation_timestep", 0.0, ConfigSource::Runtime)
        print("✗ Invalid simulation_timestep (0.0) should have been rejected")
        return False
    except std::invalid_argument as e:
        print(f"✓ Invalid simulation_timestep correctly rejected: {e}")
    except Exception as e:
        print(f"✗ Unexpected error type: {e}")
        return False
    
    # Test invalid plasticity learning rate
    print("\nTesting invalid plasticity learning rate...")
    try:
        config.set("plasticity.learning_rate", 1.5, ConfigSource::Runtime)
        print("✗ Invalid plasticity.learning_rate (1.5) should have been rejected")
        return False
    except std::invalid_argument as e:
        print(f"✓ Invalid plasticity.learning_rate correctly rejected: {e}")
    except Exception as e:
        print(f"✗ Unexpected error type: {e}")
        return False
    
    # Test invalid dopamine_baseline
    print("\nTesting invalid dopamine_baseline...")
    try:
        config.set("dopamine_baseline", 20.0, ConfigSource::Runtime)
        print("✗ Invalid dopamine_baseline (20.0) should have been rejected")
        return False
    except std::invalid_argument as e:
        print(f"✓ Invalid dopamine_baseline correctly rejected: {e}")
    except Exception as e:
        print(f"✗ Unexpected error type: {e}")
        return False
    
    return True

def test_json_validation():
    """Test JSON configuration validation."""
    print("\n=== Testing JSON Configuration Validation ===")
    
    # Create a temporary JSON configuration file
    config_data = {
        "neuron_count": 500,
        "simulation_timestep": 0.001,
        "plasticity": {
            "learning_rate": 0.01,
            "stdp_ltp_weight": 0.1
        },
        "dopamine_baseline": 0.05
    }
    
    with tempfile.NamedTemporaryFile(mode='w', suffix='.json', delete=False) as f:
        json.dump(config_data, f)
        json_file = f.name
    
    try:
        config = Config()
        result = config.loadFromFile(json_file)
        
        if result:
            print("✓ JSON configuration loaded successfully")
            
            # Check that values were parsed correctly
            neuron_count = config.getOr<int64_t>("neuron_count", -1)
            if neuron_count == 500:
                print("✓ neuron_count parsed correctly")
            else:
                print(f"✗ neuron_count not parsed correctly: {neuron_count}")
                return False
            
            # Test nested keys
            # Note: JSON parsing creates nested keys with "/" separator
            # so we need to check for the nested structure
            keys = config.getKeys()
            print(f"Keys in configuration: {keys}")
            
        else:
            print("✗ Failed to load JSON configuration")
            return False
    
    except Exception as e:
        print(f"✗ Error testing JSON validation: {e}")
        return False
    
    finally:
        # Clean up temporary file
        os.unlink(json_file)
    
    return True

def test_yaml_validation():
    """Test YAML configuration validation."""
    print("\n=== Testing YAML Configuration Validation ===")
    
    # Create a temporary YAML configuration file
    config_data = {
        'neuron_count': 2000,
        'simulation_timestep': 0.002,
        'plasticity': {
            'learning_rate': 0.005,
            'stdp_ltp_weight': 0.05
        },
        'dopamine_baseline': 0.2
    }
    
    with tempfile.NamedTemporaryFile(mode='w', suffix='.yaml', delete=False) as f:
        yaml.dump(config_data, f)
        yaml_file = f.name
    
    try:
        config = Config()
        result = config.loadFromFile(yaml_file)
        
        if result:
            print("✓ YAML configuration loaded successfully")
            
            # Check that values were parsed correctly
            neuron_count = config.getOr<int64_t>("neuron_count", -1)
            if neuron_count == 2000:
                print("✓ neuron_count parsed correctly")
            else:
                print(f"✗ neuron_count not parsed correctly: {neuron_count}")
                return False
            
            keys = config.getKeys()
            print(f"Keys in configuration: {keys}")
            
        else:
            print("✗ Failed to load YAML configuration")
            return False
    
    except Exception as e:
        print(f"✗ Error testing YAML validation: {e}")
        return False
    
    finally:
        # Clean up temporary file
        os.unlink(yaml_file)
    
    return True

def test_backward_compatibility():
    """Test backward compatibility with key=value format."""
    print("\n=== Testing Backward Compatibility ===")
    
    # Create a temporary config file in old format
    config_content = """# NLM Configuration File
neuron_count = 100
simulation_timestep = 0.005
plasticity_learning_rate = 0.02
dopamine_baseline = 0.15
"""
    
    with tempfile.NamedTemporaryFile(mode='w', suffix='.cfg', delete=False) as f:
        f.write(config_content)
        config_file = f.name
    
    try:
        config = Config()
        result = config.loadFromFile(config_file)
        
        if result:
            print("✓ Old format configuration loaded successfully")
            
            # Check that values were parsed
            neuron_count = config.getOr<int64_t>("neuron_count", -1)
            if neuron_count == 100:
                print("✓ neuron_count parsed correctly from old format")
            else:
                print(f"✗ neuron_count not parsed correctly: {neuron_count}")
                return False
            
            keys = config.getKeys()
            print(f"Keys in configuration: {keys}")
            
        else:
            print("✗ Failed to load old format configuration")
            return False
    
    except Exception as e:
        print(f"✗ Error testing backward compatibility: {e}")
        return False
    
    finally:
        # Clean up temporary file
        os.unlink(config_file)
    
    return True

def test_warnings():
    """Test that warnings are generated for potentially problematic values."""
    print("\n=== Testing Warning Generation ===")
    
    config = Config()
    
    # Test warning for large neuron count
    print("Testing warning for large neuron count...")
    try:
        config.set("neuron_count", 15000000, ConfigSource::Runtime)
        print("✓ Large neuron_count accepted (with warning)")
    except Exception as e:
        print(f"✗ Unexpected error: {e}")
        return False
    
    # Test warning for large double value
    print("\nTesting warning for large double value...")
    try:
        config.set("some_double_value", 2000.0, ConfigSource::Runtime)
        print("✓ Large double value accepted (with warning)")
    except Exception as e:
        print(f"✗ Unexpected error: {e}")
        return False
    
    return True

def main():
    """Main function."""
    print("NLM Configuration Validation Test")
    print("=" * 50)
    
    tests = [
        test_basic_validation,
        test_json_validation,
        test_yaml_validation,
        test_backward_compatibility,
        test_warnings
    ]
    
    passed = 0
    failed = 0
    
    for test in tests:
        try:
            if test():
                passed += 1
                print(f"\n✓ {test.__name__} PASSED")
            else:
                failed += 1
                print(f"\n✗ {test.__name__} FAILED")
        except Exception as e:
            failed += 1
            print(f"\n✗ {test.__name__} FAILED with exception: {e}")
    
    print("\n" + "=" * 50)
    print(f"Test Results: {passed} passed, {failed} failed")
    
    if failed > 0:
        sys.exit(1)
    else:
        print("\nAll tests passed!")


if __name__ == "__main__":
    main()
