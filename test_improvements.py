#!/usr/bin/env python3
"""
Quick verification test for NLM improvements
"""
import sys
import os
sys.path.insert(0, os.path.join(os.path.dirname(__file__), 'python'))

import pynlm

def test_config_improvements():
    """Test the new Config system features"""
    print("Testing NLM Config Improvements...")
    
    # Test 1: Basic Config constructor
    print("1. Testing Config constructor...")
    config = pynlm.Config()
    assert len(config.getKeys()) == 0
    print("   ✓ Config constructor works")
    
    # Test 2: Set and get values
    print("2. Testing set/get operations...")
    config.set("neuron_count", 1000)
    config.set("connection_probability", 0.1)
    config.set("simulation_timestep", 0.001)
    
    assert config.has("neuron_count")
    assert config.get_int("neuron_count") == 1000
    assert abs(config.get_float("connection_probability") - 0.1) < 0.0001
    assert config.get_float("simulation_timestep") == 0.001
    print("   ✓ Set/get operations work")
    
    # Test 3: Load from JSON
    print("3. Testing JSON load...")
    json_str = '{"test_int": 42, "test_float": 3.14, "test_string": "hello"}'
    config2 = pynlm.Config()
    config2.load_from_json(json_str)
    
    assert config2.get_int("test_int") == 42
    assert abs(config2.get_float("test_float") - 3.14) < 0.0001
    assert config2.get_string("test_string") == "hello"
    print("   ✓ JSON load works")
    
    # Test 4: Save to JSON
    print("4. Testing JSON save...")
    config3 = pynlm.Config()
    config3.set("key1", "value1")
    config3.set("key2", 123)
    config3.set("key3", 4.56)
    
    json_str = config3.save_to_json()
    config4 = pynlm.Config()
    config4.load_from_json(json_str)
    
    assert config4.get_string("key1") == "value1"
    assert config4.get_int("key2") == 123
    assert abs(config4.get_float("key3") - 4.56) < 0.0001
    print("   ✓ JSON save works")
    
    # Test 5: Legacy format support
    print("5. Testing legacy format support...")
    # Create a legacy config file
    with open("test_legacy.cfg", "w") as f:
        f.write("# Legacy config file\n")
        f.write("legacy_key = legacy_value\n")
        f.write("legacy_number = 999\n")
    
    config5 = pynlm.Config()
    loaded = config5.load_from_file("test_legacy.cfg")
    assert loaded
    assert config5.get_string("legacy_key") == "legacy_value"
    assert config5.get_int("legacy_number") == 999
    
    # Clean up
    os.remove("test_legacy.cfg")
    print("   ✓ Legacy format support works")
    
    # Test 6: Configuration templates
    print("6. Testing configuration templates...")
    quick_config = pynlm.create_config_from_template("quick_test")
    assert quick_config.get_int("neuron_count") == 100
    print("   ✓ Configuration templates work")
    
    # Test 7: Advanced Python features
    print("7. Testing advanced Python features...")
    config6 = pynlm.Config()
    config6.set("test_int", 42)
    config6.set("test_float", 3.14)
    config6.set("test_string", "hello")
    config6.set("test_bool", True)
    
    # Test to_dict
    config_dict = config6.to_dict()
    assert config_dict["test_int"] == 42
    assert abs(config_dict["test_float"] - 3.14) < 0.0001
    assert config_dict["test_string"] == "hello"
    assert config_dict["test_bool"] == True
    print("   ✓ Advanced Python features work")
    
    print("\n✅ All NLM improvement tests passed!")
    return True

if __name__ == "__main__":
    try:
        success = test_config_improvements()
        sys.exit(0 if success else 1)
    except Exception as e:
        print(f"\n❌ Test failed with error: {e}")
        import traceback
        traceback.print_exc()
        sys.exit(1)
