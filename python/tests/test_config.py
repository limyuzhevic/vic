#!/usr/bin/env python3
"""
Python bindings tests for Config class
Tests mirror the C++ test_config.cpp functionality
"""

import sys
import os
sys.path.insert(0, os.path.join(os.path.dirname(__file__)))

import pytest
from pynlm import Config

class TestConfig:
    """Test Config class functionality"""
    
    def test_config_creation(self):
        """Test Config creation"""
        config = Config()
        assert not config.has("test")
        print("✓ Config creation test passed")
    
    def test_config_set_and_get(self):
        """Test setting and getting various config values"""
        config = Config()
        
        # Test integer
        config.set("int_value", 42)
        assert config.has("int_value")
        int_val = config.get_int("int_value")
        assert int_val == 42
        
        # Test float
        config.set("double_value", 3.14)
        double_val = config.get_float("double_value")
        assert 3.13 < double_val < 3.15
        
        # Test string
        config.set("string_value", "hello")
        string_val = config.get_string("string_value")
        assert string_val == "hello"
        
        # Test boolean
        config.set("bool_value", True)
        bool_val = config.get_bool("bool_value")
        assert bool_val == True
        
        print("✓ Config set and get test passed")
    
    def test_config_get_or(self):
        """Test getOr functionality"""
        config = Config()
        
        # Test with missing key
        val = config.get_int("missing", 100)
        assert val == 100
        
        # Test with existing key
        config.set("existing", 50)
        val = config.get_int("existing", 100)
        assert val == 50
        
        print("✓ Config getOr test passed")
    
    def test_config_remove(self):
        """Test removing config keys"""
        config = Config()
        
        config.set("test", 123)
        assert config.has("test")
        
        config.remove("test")
        assert not config.has("test")
        
        print("✓ Config remove test passed")
    
    def test_config_keys(self):
        """Test getting config keys"""
        config = Config()
        
        config.set("key1", 1)
        config.set("key2", 2)
        config.set("key3", 3)
        
        keys = config.get_keys()
        assert len(keys) == 3
        assert "key1" in keys
        assert "key2" in keys
        assert "key3" in keys
        
        print("✓ Config keys test passed")
    
    def test_config_dict_like_access(self):
        """Test dict-like access"""
        config = Config()
        
        # Set values using dict-like syntax
        config["neuron_count"] = 1000
        config["connection_probability"] = 0.1
        config["simulation_timestep"] = 0.001
        
        # Get values using dict-like syntax
        assert config["neuron_count"] == "1000"
        assert float(config["connection_probability"]) == 0.1
        assert float(config["simulation_timestep"]) == 0.001
        
        # Test delete
        del config["neuron_count"]
        assert "neuron_count" not in config
        
        print("✓ Config dict-like access test passed")
    
    def test_config_from_dict(self):
        """Test creating config from dictionary"""
        config_data = {
            "neuron_count": 2000,
            "region_count": 5,
            "connection_probability": 0.15,
            "simulation_timestep": 0.0005,
            "enable_structural_plasticity": True,
            "enable_development": False,
            "random_seed": 42
        }
        
        config = Config()
        for key, value in config_data.items():
            config.set(key, value)
        
        # Verify all values were set correctly
        assert config.get_int("neuron_count") == 2000
        assert config.get_int("region_count") == 5
        assert config.get_float("connection_probability") == 0.15
        assert config.get_float("simulation_timestep") == 0.0005
        assert config.get_bool("enable_structural_plasticity") == True
        assert config.get_bool("enable_development") == False
        assert config.get_int("random_seed") == 42
        
        print("✓ Config from dict test passed")
    
    def test_config_validation(self):
        """Test configuration validation"""
        config = Config()
        
        # Set valid values
        config.set("neuron_count", 100)
        config.set("connection_probability", 0.5)
        config.set("simulation_timestep", 0.01)
        
        # Validate schema (simplified)
        schema = {
            "neuron_count": {"type": "int", "min": 10, "max": 100000},
            "connection_probability": {"type": "float", "min": 0.0, "max": 1.0},
            "simulation_timestep": {"type": "float", "min": 0.0001, "max": 0.1},
        }
        
        # This is a placeholder - real validation would be implemented
        # For now, just test that we can validate
        from pynlm.helpers import ConfigValidator
        validator = ConfigValidator(config)
        
        # Test type validation
        assert validator.validate_types()
        
        # Test range validation  
        assert validator.validate_ranges()
        
        # Test all validation
        assert validator.validate_all()
        
        print("✓ Config validation test passed")
    
    def test_config_save_load(self, tmp_path):
        """Test saving and loading config"""
        config = Config()
        config.set("neuron_count", 500)
        config.set("connection_probability", 0.2)
        config.set("enable_structural_plasticity", True)
        
        # Save to file
        config_file = tmp_path / "test_config.txt"
        result = config.save_to_file(str(config_file))
        assert result
        
        # Load from file
        config2 = Config()
        result = config2.load_from_file(str(config_file))
        assert result
        
        # Verify values
        assert config2.get_int("neuron_count") == 500
        assert config2.get_float("connection_probability") == 0.2
        assert config2.get_bool("enable_structural_plasticity") == True
        
        print("✓ Config save/load test passed")
    
    def test_config_summary(self):
        """Test config summary"""
        config = Config()
        config.set("neuron_count", 1000)
        config.set("region_count", 3)
        
        summary = config.summary()
        assert isinstance(summary, str)
        assert "Config:" in summary
        assert "neuron_count" in summary
        assert "region_count" in summary
        
        print("✓ Config summary test passed")
    
    def test_config_clear(self):
        """Test clearing config"""
        config = Config()
        config.set("key1", 1)
        config.set("key2", 2)
        
        config.clear()
        
        assert not config.has("key1")
        assert not config.has("key2")
        assert len(config.get_keys()) == 0
        
        print("✓ Config clear test passed")

if __name__ == "__main__":
    # Run tests manually if script is executed directly
    import sys
    pytest.main([__file__, "-v"])
