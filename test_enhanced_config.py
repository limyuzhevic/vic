#!/usr/bin/env python3

"""
Test script for the enhanced NLM configuration system.
This script demonstrates the new functionality of the enhanced configuration system.
"""

import sys
sys.path.append('/workspace/cf14c76c-4b1f-4d38-9a96-64cd8a400481/sessions/agent_a4e4922c-844b-4d92-a280-7151db888ce5')

import nlm as nlm

def test_basic_config():
    """Test basic configuration functionality"""
    print("Testing basic configuration...")
    
    # Create a default config
    config = nlm.Config()
    
    # Set some values
    config.set("simulation.learning_rate", 0.001)
    config.set("brain.neuron_count", 1000)
    config.set("output.logging.level", "info")
    
    # Get values
    assert config.get("simulation.learning_rate").value == 0.001
    assert config.get("brain.neuron_count").value == 1000
    assert config.get("output.logging.level").value == "info"
    
    # Check existence
    assert config.has("simulation.learning_rate")
    assert not config.has("nonexistent.key")
    
    # Get all keys
    keys = config.getKeys()
    assert "simulation.learning_rate" in keys
    assert "brain.neuron_count" in keys
    assert "output.logging.level" in keys
    
    # Summary
    summary = config.summary()
    assert "Configuration (3 entries)" in summary
    
    print("✓ Basic configuration test passed")

def test_enhanced_config():
    """Test enhanced configuration functionality"""
    print("Testing enhanced configuration...")
    
    # Create config with preset
    config = nlm.ConfigBuilder().set("simulation.learning_rate", 0.001).set("brain.neuron_count", 1000).build()
    
    # Test validation
    assert config.validate()
    
    # Add a parameter
    param = nlm.ConfigParameter("test.param", nlm.ConfigParameter.Type.Int, "Test parameter", 42, True)
    config.addParameter(param)
    
    # Check parameter exists
    assert config.hasParameter("test.param")
    
    # Get parameter info
    param_info = config.getParameterInfo("test.param")
    assert param_info.key == "test.param"
    assert param_info.required == True
    
    # Test merge
    config2 = nlm.ConfigBuilder().set("test.other", 100).build()
    config.merge(config2)
    
    assert config.has("test.other")
    
    # Test clone
    config3 = config.clone()
    assert config3.has("simulation.learning_rate")
    
    # Test diff
    diff = config.diff(config3)
    assert len(diff) == 0  # No differences
    
    print("✓ Enhanced configuration test passed")

def test_presets():
    """Test configuration presets"""
    print("Testing configuration presets...")
    
    # Test research config
    research_config = nlm.createResearchConfig()
    assert research_config.has("experimental.enabled")
    assert research_config.get("experimental.enabled").value == True
    assert research_config.has("simulation.learning_rate")
    
    # Test production config
    production_config = nlm.createProductionConfig()
    assert production_config.has("safety.checkpoints.enabled")
    assert production_config.has("simulation.learning_rate")
    
    # Test simulation config
    simple_config = nlm.createSimulationConfig("simple")
    assert simple_config.has("brain.neuron_count")
    
    # Test performance config
    performance_config = nlm.createPerformanceConfig()
    assert performance_config.has("performance.threading.enabled")
    
    print("✓ Configuration presets test passed")

def test_python_api():
    """Test Python API functionality"""
    print("Testing Python API...")
    
    # Test create_config_from_dict
    config_dict = {
        "test.int": 42,
        "test.double": 3.14,
        "test.bool": True,
        "test.string": "hello"
    }
    
    config = nlm.create_config_from_dict(config_dict)
    assert config.get("test.int").value == 42
    assert config.get("test.double").value == 3.14
    assert config.get("test.bool").value == True
    assert config.get("test.string").value == "hello"
    
    # Test get_config_presets
    presets = nlm.get_config_presets()
    assert "research" in presets
    assert "production" in presets
    assert "performance" in presets
    
    # Test validate_config
    assert nlm.validate_config(config)
    
    print("✓ Python API test passed")

def test_serialization():
    """Test serialization and deserialization"""
    print("Testing serialization...")
    
    # Create a config with various types
    config = nlm.ConfigBuilder()
    config.set("int_value", 42)
    config.set("double_value", 3.14)
    config.set("bool_value", True)
    config.set("string_value", "test")
    config.set("int_vector", [1, 2, 3])
    config.set("double_vector", [1.1, 2.2, 3.3])
    config.set("string_vector", ["a", "b", "c"])
    config = config.build()
    
    # Test binary serialization
    binary_data = config.toBinary()
    assert len(binary_data) > 0
    
    # Test binary deserialization
    config2 = nlm.Config.fromBinary(binary_data)
    assert config2.has("int_value")
    assert config2.has("double_value")
    assert config2.has("bool_value")
    
    print("✓ Serialization test passed")

def test_error_handling():
    """Test error handling"""
    print("Testing error handling...")
    
    # Test validation errors
    config = nlm.Config()
    
    # Add a parameter with constraints
    param = nlm.ConfigParameter("constrained.param", nlm.ConfigParameter.Type.Int, 
                               "Parameter with constraints", 0, True)
    param.setConstraint("min", 0)
    param.setConstraint("max", 100)
    
    config.addParameter(param)
    
    # Set value within constraints
    config.set("constrained.param", 50)
    assert config.validate()
    
    # Set value outside constraints (should still work but validate will catch it)
    config.set("constrained.param", 150)
    try:
        config.validate()
        assert False, "Validation should have failed"
    except nlm.ConfigValidationException:
        pass  # Expected
    
    print("✓ Error handling test passed")

def main():
    """Run all tests"""
    print("Running enhanced NLM configuration system tests...")
    print("=" * 60)
    
    try:
        test_basic_config()
        test_enhanced_config()
        test_presets()
        test_python_api()
        test_serialization()
        test_error_handling()
        
        print("=" * 60)
        print("All tests passed! ✓")
        print("\nEnhanced configuration system features:")
        print("- Type-safe configuration validation")
        print("- Configuration schema management")
        print("- Environment-specific configurations")
        print("- Feature flag system")
        print("- Configuration groups")
        print("- JSON/YAML serialization/deserialization")
        print("- Configuration backup and restore")
        print("- Configuration diff and comparison")
        print("- Configuration history tracking")
        print("- Enhanced Python bindings with type safety")
        print("- Configuration preset factories")
        print("- Configuration comparison utilities")
        print("- Comprehensive error handling")
        
        return 0
    except Exception as e:
        print(f"Test failed with error: {e}")
        import traceback
        traceback.print_exc()
        return 1

if __name__ == "__main__":
    sys.exit(main())