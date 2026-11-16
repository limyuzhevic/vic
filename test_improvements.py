"""Python tests for NLM bindings improvements."""

import sys
import os
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

import pytest
import tempfile
import json
from pynlm import Config, Brain, createDefaultConfig, createBrain, createSimpleWorld, createAgentBrain
class TestConfigImprovements:
    """Test the improved configuration system."""

    def test_config_creation(self):
        """Test basic config creation."""
        config = Config()
        assert not config.has("test_key")
        config.set("test_key", "test_value")
        assert config.has("test_key")

    def test_config_types(self):
        """Test setting and getting different types."""
        config = Config()
        
        # Test integer
        config.set("int_key", 42)
        assert config.getInt("int_key") == 42
        
        # Test float
        config.set("float_key", 3.14)
        assert config.getFloat("float_key") == 3.14
        
        # Test boolean
        config.set("bool_key", True)
        assert config.getBool("bool_key") == True
        
        # Test string
        config.set("string_key", "hello")
        assert config.getString("string_key") == "hello"

    def test_config_dict_load(self):
        """Test loading configuration from dictionary."""
        config = Config()
        config.loadFromDict({
            "key1": "value1",
            "key2": 42,
            "key3": 3.14,
            "key4": True,
            "key5": [1, 2, 3],
            "key6": ["a", "b", "c"]
        })
        
        assert config.getString("key1") == "value1"
        assert config.getInt("key2") == 42
        assert config.getFloat("key3") == 3.14
        assert config.getBool("key4") == True
        assert config.getString("key5") == "[1, 2, 3]"
        assert config.getString("key6") == "[\"a\", \"b\", \"c\"]"

    def test_config_to_dict(self):
        """Test converting config to dictionary."""
        config = Config()
        config.set("int_key", 42)
        config.set("float_key", 3.14)
        config.set("string_key", "hello")
        config.set("bool_key", True)
        
        dict_result = config.toDict()
        assert dict_result["int_key"] == 42
        assert dict_result["float_key"] == 3.14
        assert dict_result["string_key"] == "hello"
        assert dict_result["bool_key"] == True

    def test_config_copy(self):
        """Test config copying."""
        config = Config()
        config.set("key1", "value1")
        config.set("key2", 42)
        
        config_copy = config.copy()
        assert config_copy.getString("key1") == "value1"
        assert config_copy.getInt("key2") == 42
        
        # Modify copy, original should be unchanged
        config_copy.set("key3", "value3")
        assert not config.has("key3")

    def test_config_merge(self):
        """Test config merging."""
        config1 = Config()
        config1.set("key1", "value1")
        config1.set("key2", 42)
        
        config2 = Config()
        config2.set("key2", "override")  # Override existing key
        config2.set("key3", "value3")    # Add new key
        
        config1.merge(config2)
        
        assert config1.getString("key1") == "value1"
        assert config1.getString("key2") == "override"  # Should be overridden
        assert config1.getString("key3") == "value3"    # Should be added

    def test_config_json_save_load(self):
        """Test JSON save/load functionality."""
        config = Config()
        config.set("int_key", 42)
        config.set("float_key", 3.14)
        config.set("string_key", "hello")
        config.set("bool_key", True)
        
        # Save to JSON
        with tempfile.NamedTemporaryFile(mode='w', suffix='.json', delete=False) as f:
            json_file = f.name
        
        try:
            success = config.saveToJSON(json_file)
            assert success
            
            # Load back
            config2 = Config()
            success = config2.loadFromJSON(json_file)
            assert success
            
            assert config2.getInt("int_key") == 42
            assert config2.getFloat("float_key") == 3.14
            assert config2.getString("string_key") == "hello"
            assert config2.getBool("bool_key") == True
        finally:
            os.unlink(json_file)

    def test_config_yaml_save_load(self):
        """Test YAML save/load functionality."""
        config = Config()
        config.set("int_key", 42)
        config.set("float_key", 3.14)
        config.set("string_key", "hello")
        config.set("bool_key", True)
        
        # Save to YAML
        with tempfile.NamedTemporaryFile(mode='w', suffix='.yaml', delete=False) as f:
            yaml_file = f.name
        
        try:
            success = config.saveToYAML(yaml_file)
            assert success
            
            # Load back
            config2 = Config()
            success = config2.loadFromYAML(yaml_file)
            assert success
            
            assert config2.getInt("int_key") == 42
            assert config2.getFloat("float_key") == 3.14
            assert config2.getString("string_key") == "hello"
            assert config2.getBool("bool_key") == True
        finally:
            os.unlink(yaml_file)

    def test_config_validation(self):
        """Test configuration validation."""
        config = Config()
        config.set("required_key", "value")
        config.set("optional_key", "value")
        
        schema = {
            "required_key": "string",
            "optional_key": "string"
        }
        
        errors = config.validateConfig(schema)
        assert len(errors) == 0
        
        # Test missing required key
        schema2 = {
            "required_key": "string",
            "missing_key": "string"
        }
        
        errors = config.validateConfig(schema2)
        assert len(errors) == 1
        assert "Missing required key: missing_key" in str(errors[0])

    def test_config_merge_function(self):
        """Test configuration merging function."""
        config1 = Config()
        config1.set("key1", "value1")
        config1.set("key2", 42)
        
        config2 = Config()
        config2.set("key2", "override")
        config2.set("key3", "value3")
        
        merged = Config.mergeConfigs(config1, config2)
        
        assert merged.getString("key1") == "value1"
        assert merged.getString("key2") == "override"
        assert merged.getString("key3") == "value3"
class TestBrainImprovements:
    """Test the improved brain creation and simulation functions."""

    def test_create_brain_from_config(self):
        """Test creating brain from configuration."""
        config = createDefaultConfig()
        config.set("neuron_count", 100, 3)  # ConfigSource::Default
        config.set("region_count", 1, 3)
        
        brain = createBrain(config)
        assert brain is not None
        
        # Initialize the brain
        brain.initialize()
        
        # Check basic properties
        assert brain.getTotalNeuronCount() == 100
        assert brain.getRegionCount() == 1
        assert brain.getTotalSynapseCount() > 0

    def test_create_brain_from_dict(self):
        """Test creating brain from dictionary."""
        brain = createBrain({
            "neuron_count": 50,
            "region_count": 2,
            "connection_probability": 0.1,
            "simulation_timestep": 0.001,
            "random_seed": 42
        })
        
        assert brain is not None
        brain.initialize()
        
        assert brain.getTotalNeuronCount() == 50
        assert brain.getRegionCount() == 2
        assert brain.getTotalSynapseCount() > 0

    def test_simulation_functions(self):
        """Test simulation utility functions."""
        config = createDefaultConfig()
        config.set("neuron_count", 100, 3)
        config.set("region_count", 1, 3)
        
        brain = createBrain(config)
        brain.initialize()
        
        # Test single step
        spike_count = brain.step(0)
        assert spike_count >= 0
        
        # Test multiple steps
        spike_count = brain.runSimulationSteps(1, 10)
        assert spike_count >= 0

    def test_spike_stats(self):
        """Test spike statistics function."""
        config = createDefaultConfig()
        config.set("neuron_count", 100, 3)
        config.set("region_count", 1, 3)
        
        brain = createBrain(config)
        brain.initialize()
        
        # Run some simulation
        brain.step(0)
        brain.step(1)
        
        stats = brain.getSpikeStats()
        assert "total_spikes" in stats
        assert "active_neurons" in stats
        assert "firing_neurons" in stats
        assert "average_firing_rate" in stats
        assert "excitation_inhibition_ratio" in stats
        
        assert isinstance(stats["total_spikes"], int)
        assert isinstance(stats["active_neurons"], int)
        assert isinstance(stats["firing_neurons"], int)
        assert isinstance(stats["average_firing_rate"], float)
        assert isinstance(stats["excitation_inhibition_ratio"], float)

    def test_region_stats(self):
        """Test region statistics function."""
        config = createDefaultConfig()
        config.set("neuron_count", 100, 3)
        config.set("region_count", 3, 3)
        
        brain = createBrain(config)
        brain.initialize()
        
        region_stats = brain.getRegionStats()
        
        # Should have 3 regions (region_count = 3)
        assert len(region_stats) == 3
        
        for region_id, region_data in region_stats.items():
            assert "neuron_count" in region_data
            assert "synapse_count" in region_data
            assert region_data["neuron_count"] > 0
            assert region_data["synapse_count"] > 0
class TestUtilityFunctions:
    """Test utility functions and helpers."""

    def test_example_config(self):
        """Test example configuration function."""
        example_config = createDefaultConfig()
        
        assert example_config.has("neuron_count")
        assert example_config.has("region_count")
        assert example_config.has("connection_probability")
        assert example_config.has("simulation_timestep")
        assert example_config.has("random_seed")
        
        assert example_config.getInt("neuron_count") == 500
        assert example_config.getInt("region_count") == 1
        assert example_config.getFloat("connection_probability") == 0.15
        assert example_config.getFloat("simulation_timestep") == 0.001
        assert example_config.getInt("random_seed") == 42

    def test_version_info(self):
        """Test version information."""
        import pynlm
        assert pynlm.version == "0.1.0"
        assert pynlm.author == "NLM Authors"
        assert "Python bindings for NLM" in pynlm.description
        assert pynlm.python_api_version == "1.0"

    def test_create_simple_world(self):
        """Test simple world creation."""
        world = createSimpleWorld()
        assert world is not None
        
        # Configure world
        world.configure(100, 100, 50, 50)
        assert world.getWidth() == 100
        assert world.getHeight() == 100
        assert world.getMaxEnergy() > 0

    def test_create_agent_brain(self):
        """Test agent brain creation."""
        config = createDefaultConfig()
        config.set("neuron_count", 100, 3)
        config.set("region_count", 1, 3)
        
        brain = createBrain(config)
        brain.initialize()
        
        world = createSimpleWorld()
        world.configure(200, 200, 50, 50)
        
        agent_brain = createAgentBrain(brain)
        assert agent_brain is not None
        
        # Initialize agent brain
        agent_brain.initialize(world)
        
        # Check sizes
        assert agent_brain.getSensoryInputSize() > 0
        assert agent_brain.getMotorOutputSize() > 0
class TestErrorHandling:
    """Test error handling and edge cases."""

    def test_invalid_config_file(self):
        """Test handling of invalid configuration files."""
        # This should not crash
        config = Config()
        success = config.loadFromFile("non_existent_file.txt")
        assert not success

    def test_invalid_json(self):
        """Test handling of invalid JSON."""
        config = Config()
        success = config.loadFromJSON("invalid { json }")
        assert not success

    def test_invalid_yaml(self):
        """Test handling of invalid YAML."""
        config = Config()
        success = config.loadFromYAML("invalid : yaml : content : [")
        assert not success

    def test_save_invalid_file(self):
        """Test handling of invalid save operations."""
        config = Config()
        # Try to save to an invalid path
        success = config.saveToJSON("/invalid/path/that/does/not/exist/test.json")
        assert not success

    def test_dict_with_invalid_types(self):
        """Test handling of dictionaries with invalid types."""
        config = Config()
        
        # This should handle gracefully (currently converts to string)
        config.loadFromDict({
            "valid_string": "value",
            "valid_int": 42,
            "valid_float": 3.14,
            "valid_bool": True
        })
        
        assert config.getString("valid_string") == "value"
        assert config.getInt("valid_int") == 42
        assert config.getFloat("valid_float") == 3.14
        assert config.getBool("valid_bool") == True
if __name__ == "__main__":
    pytest.main([__file__, "-v"])
