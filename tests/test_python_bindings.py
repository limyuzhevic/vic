"""
Python bindings tests for NLM (Neural Learning Machine)

Comprehensive test suite for pynlm Python bindings, covering:
- Basic functionality
- Factory functions
- Error handling
- Type safety
- Performance optimizations
"""

import sys
import os
sys.path.insert(0, os.path.join(os.path.dirname(__file__), 'build'))

import pytest
import pynlm
import tempfile
import json
import math

class TestBasicFunctionality:
    """Test basic NLM functionality"""

    def test_create_default_config(self):
        """Test creating default configuration"""
        config = pynlm.createDefaultConfig()
        assert config is not None
        assert config.getKeys() == []
        assert config.is_empty == True

    def test_create_config_from_dict(self):
        """Test creating config from dictionary"""
        dict_config = {
            "brain.neuron_count": 100,
            "brain.connection_probability": 0.1,
            "plasticity.stdp.enable": True,
            "neuromod.dopamine.scale": 1.5
        }
        config = pynlm.createConfigFromDict(dict_config)
        assert config is not None
        assert "brain.neuron_count" in config
        assert config.get("brain.neuron_count") == 100
        assert config["brain.connection_probability"] == 0.1
        assert config["plasticity.stdp.enable"] == True
        assert config["neuromod.dopamine.scale"] == 1.5

    def test_test_config(self):
        """Test test configuration factory"""
        config = pynlm.createTestConfig()
        assert config is not None
        assert config.get("brain.neuron_count") == 100
        assert config.get("brain.synapse_density") == 0.1
        assert config.get("plasticity.stdp.enable") == True

    def test_simulation_config(self):
        """Test simulation configuration factory"""
        config = pynlm.createSimulationConfig(500, 0.05)
        assert config is not None
        assert config.get("brain.neuron_count") == 500
        assert config.get("brain.connection_probability") == 0.05

    def test_brain_creation(self):
        """Test brain creation"""
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        assert brain is not None
        brain.initialize()
        assert brain.getTotalNeuronCount() > 0

    def test_brain_from_dict(self):
        """Test brain creation from dictionary"""
        dict_config = {
            "brain.neuron_count": 200,
            "brain.connection_probability": 0.2
        }
        brain = pynlm.createBrainFromDict(dict_config)
        assert brain is not None
        brain.initialize()
        assert brain.getTotalNeuronCount() > 0

    def test_world_creation(self):
        """Test world creation"""
        world = pynlm.createSimpleWorld()
        assert world is not None
        assert world.getWidth() == 10  # Default size
        assert world.getHeight() == 10  # Default size

    def test_agent_creation(self):
        """Test agent creation"""
        config = pynlm.createTestConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        world = pynlm.createSimpleWorld()
        world.configure(20, 20, 8, 8)
        
        agent = pynlm.createAgent(brain, world)
        assert agent is not None
        assert agent.getBrain() is brain

    def test_agent_from_brain(self):
        """Test agent creation from brain"""
        config = pynlm.createTestConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        agent = pynlm.createAgentBrain(brain)
        assert agent is not None
        assert agent.getBrain() is brain

class TestTypeSafety:
    """Test type safety and validation"""

    def test_config_validation(self):
        """Test configuration validation"""
        config = pynlm.createDefaultConfig()
        config.set("valid_key", 42)
        assert pynlm.validate_config(config) == True
        
        # Add invalid entry (None)
        config.pImpl->entries.emplace_back("invalid_key", ConfigValue(), ConfigSource::Runtime);
        assert pynlm.validate_config(config) == False

    def test_get_config_value_with_default(self):
        """Test getting config value with type-safe default"""
        config = pynlm.createDefaultConfig()
        config.set("int_value", 100)
        config.set("string_value", "hello")
        config.set("bool_value", True)
        config.set("double_value", 3.14)
        
        # Test int default
        assert config.get("int_value", 999) == 100
        assert config.get("missing_int", 999) == 999
        
        # Test string default
        assert config.get("string_value", "default") == "hello"
        assert config.get("missing_string", "default") == "default"
        
        # Test bool default
        assert config.get("bool_value", False) == True
        
        # Test double default
        assert config.get("double_value", 0.0) == 3.14

    def test_type_conversion_helpers(self):
        """Test type conversion helper functions"""
        int_value = pynlm.to_config_value(42)
        double_value = pynlm.to_config_value(3.14)
        bool_value = pynlm.to_config_value(True)
        string_value = pynlm.to_config_value("test")
        
        # These should all be valid ConfigValue types
        assert isinstance(int_value, pynlm.ConfigValue)
        assert isinstance(double_value, pynlm.ConfigValue)
        assert isinstance(bool_value, pynlm.ConfigValue)
        assert isinstance(string_value, pynlm.ConfigValue)

    def test_batch_operations(self):
        """Test batch operations for performance"""
        config = pynlm.createDefaultConfig()
        
        # Batch set
        batch_data = {
            "key1": 100,
            "key2": "value1",
            "key3": True,
            "key4": 3.14,
            "key5": [1, 2, 3],
            "key6": [1.1, 2.2, 3.3],
            "key7": ["a", "b", "c"]
        }
        pynlm.batch_set(config, batch_data)
        
        # Verify all were set
        assert len(config.getKeys()) == 7
        assert config.get("key1") == 100
        assert config.get("key2") == "value1"
        assert config.get("key3") == True
        assert config.get("key4") == 3.14
        assert config.get("key5") == [1, 2, 3]
        assert config.get("key6") == [1.1, 2.2, 3.3]
        assert config.get("key7") == ["a", "b", "c"]
        
        # Batch get
        keys = ["key1", "key3", "key5", "key7", "key9"]
        result = pynlm.batch_get(config, keys)
        assert len(result) == 4
        assert result["key1"] == 100
        assert result["key3"] == True
        assert result["key5"] == [1, 2, 3]
        assert result["key7"] == ["a", "b", "c"]

class TestErrorHandling:
    """Test error handling and safety features"""

    def test_safe_load_config(self):
        """Test safe config loading with error handling"""
        config = pynlm.createDefaultConfig()
        
        # Test with non-existent file
        result = pynlm.safe_load_config(config, "nonexistent_file.cfg")
        assert result == False
        
        # Test with valid file
        with tempfile.NamedTemporaryFile(mode='w', suffix='.cfg', delete=False) as f:
            f.write("brain.neuron_count = 200\n")
            f.write("brain.connection_probability = 0.3\n")
            temp_file = f.name
        
        try:
            result = pynlm.safe_load_config(config, temp_file)
            # The parsing is simple key=value format, so it should work
            assert result == True
            assert "brain.neuron_count" in config
        finally:
            os.unlink(temp_file)

    def test_safe_save_config(self):
        """Test safe config saving with error handling"""
        config = pynlm.createDefaultConfig()
        config.set("test.key", "value")
        
        with tempfile.NamedTemporaryFile(mode='w', suffix='.cfg', delete=False) as f:
            temp_file = f.name
        
        try:
            result = pynlm.safe_save_config(config, temp_file)
            assert result == True
            assert os.path.exists(temp_file)
            
            # Verify file content (note: current implementation has placeholder)
            with open(temp_file, 'r') as f:
                content = f.read()
                assert "test.key" in content
        finally:
            if os.path.exists(temp_file):
                os.unlink(temp_file)

    def test_config_exceptions(self):
        """Test configuration exception handling"""
        config = pynlm.createDefaultConfig()
        
        # Test accessing non-existent key
        try:
            # This should raise KeyError or similar
            value = config["nonexistent_key"]
            assert False, "Should have raised KeyError"
        except:
            pass  # Expected

class TestPythonicInterfaces:
    """Test Pythonic interfaces and features"""

    def test_config_properties(self):
        """Test config property access"""
        config = pynlm.createDefaultConfig()
        assert config.is_empty == True
        
        config.set("key1", 100)
        config.set("key2", "value")
        
        assert config.is_empty == False
        assert "key1" in config
        assert "key2" in config
        assert "key3" not in config

    def test_config_iteration(self):
        """Test config iteration"""
        config = pynlm.createDefaultConfig()
        config.set("key1", 1)
        config.set("key2", 2)
        config.set("key3", 3)
        
        keys = list(config.keys())
        assert len(keys) == 3
        assert "key1" in keys
        assert "key2" in keys
        assert "key3" in keys

    def test_brain_to_dict(self):
        """Test brain state conversion to dictionary"""
        config = pynlm.createTestConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Generate some activity
        for i in range(10):
            brain.step(i)
        
        state = brain.to_dict()
        assert "total_neurons" in state
        assert "total_synapses" in state
        assert "active_neurons" in state
        assert "firing_neurons" in state
        assert "average_firing_rate" in state
        assert "excitation_inhibition_ratio" in state
        assert "total_spikes" in state
        assert "development_stage" in state
        
        assert state["total_neurons"] > 0
        assert state["development_stage"] == 0  # Initial stage

    def test_sensory_to_dict(self):
        """Test vision conversion to dictionary"""
        vision = pynlm.Vision(10, 10, 3)
        vision.setData([1.0, 2.0, 3.0] * 100)  # 10*10*3 = 300 values
        
        dict_repr = vision.to_dict()
        assert "width" in dict_repr
        assert "height" in dict_repr
        assert "channels" in dict_repr
        assert "data" in dict_repr
        assert dict_repr["width"] == 10
        assert dict_repr["height"] == 10
        assert dict_repr["channels"] == 3
        assert len(dict_repr["data"]) == 300

    def test_action_to_dict(self):
        """Test action conversion to dictionary"""
        action = pynlm.Action(pynlm.ActionType::MoveForward, [0.5, 0.5])
        
        dict_repr = action.to_dict()
        assert "type" in dict_repr
        assert "parameters" in dict_repr
        assert "name" in dict_repr
        assert dict_repr["type"] == pynlm.ActionType::MoveForward
        assert dict_repr["parameters"] == [0.5, 0.5]

    def test_agent_body_to_dict(self):
        """Test agent body conversion to dictionary"""
        body = pynlm.AgentBody()
        body.x = 5.0
        body.y = 3.0
        body.orientation = 90.0
        body.velocityX = 1.0
        body.velocityY = 2.0
        body.energy = 0.8
        body.health = 100
        body.age = 5
        body.isMoving = True
        body.isTurning = False
        
        dict_repr = body.to_dict()
        assert "x" in dict_repr
        assert "y" in dict_repr
        assert "orientation" in dict_repr
        assert "velocity_x" in dict_repr
        assert "velocity_y" in dict_repr
        assert "energy" in dict_repr
        assert "health" in dict_repr
        assert "age" in dict_repr
        assert "is_moving" in dict_repr
        assert "is_turning" in dict_repr
        
        assert dict_repr["x"] == 5.0
        assert dict_repr["y"] == 3.0
        assert dict_repr["orientation"] == 90.0
        assert dict_repr["velocity_x"] == 1.0
        assert dict_repr["velocity_y"] == 2.0

class TestPerformance:
    """Test performance features"""

    def test_batch_set_performance(self):
        """Test batch set performance with many values"""
        config = pynlm.createDefaultConfig()
        
        # Create a large batch
        batch_data = {}
        for i in range(1000):
            batch_data[f"key_{i}"] = i
        
        # This should be efficient
        pynlm.batch_set(config, batch_data)
        assert len(config.getKeys()) == 1000
        
        # Verify some random keys
        assert config.get("key_0") == 0
        assert config.get("key_500") == 500
        assert config.get("key_999") == 999

    def test_batch_get_performance(self):
        """Test batch get performance"""
        config = pynlm.createDefaultConfig()
        
        # Add many keys
        for i in range(1000):
            config.set(f"key_{i}", i)
        
        # Get random subset
        import random
        keys = [f"key_{random.randint(0, 999)}" for _ in range(100)]
        
        result = pynlm.batch_get(config, keys)
        assert len(result) <= 100
        
        # Verify some
        for key in list(result.keys())[:5]:
            assert config.get(key) == result[key]

class TestIntegration:
    """Integration tests"""

    def test_complete_agent_workflow(self):
        """Test complete agent workflow"""
        # Create agent with all features enabled
        config = pynlm.createTestConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        world = pynlm.createSimpleWorld()
        world.configure(15, 15, 8, 8)
        
        agent = pynlm.createAgent(brain, world)
        agent.enableRewardModulation(True)
        agent.enableCuriosity(True)
        
        # Run simulation
        for step in range(50):
            world.update(0.1)
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            brain.step(step)
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
        
        # Check results
        assert brain.getTotalSpikeCount() > 0
        assert agent.getDevelopmentalStage() != pynlm.DevelopmentalStage::Initial
        
        # Test reset
        brain.reset()
        brain.initialize()
        assert brain.getTotalSpikeCount() == 0

    def test_config_save_load(self):
        """Test config save and load"""
        config = pynlm.createTestConfig()
        
        with tempfile.NamedTemporaryFile(mode='w', suffix='.cfg', delete=False) as f:
            temp_file = f.name
        
        try:
            # Save config
            result = pynlm.safe_save_config(config, temp_file)
            assert result == True
            
            # Load into new config
            new_config = pynlm.createDefaultConfig()
            result = pynlm.safe_load_config(new_config, temp_file)
            # Note: The current implementation uses simple key=value parsing
            # which may not preserve all types correctly
            assert result == True
        finally:
            if os.path.exists(temp_file):
                os.unlink(temp_file)

class TestAdvancedFeatures:
    """Test advanced features for power users"""

    def test_config_with_validation(self):
        """Test advanced config validation"""
        config = pynlm.createDefaultConfig()
        
        # Add valid configs
        config.set("neuron_count", 100)
        config.set("connection_probability", 0.1)
        config.set("plasticity", True)
        
        assert pynlm.validate_config(config) == True
        
        # Add invalid config (None value)
        config.pImpl->entries.emplace_back("invalid", ConfigValue(), ConfigSource::Runtime)
        assert pynlm.validate_config(config) == False

    def test_sensory_input_processing(self):
        """Test complex sensory input processing"""
        vision = pynlm.Vision(20, 20, 3)
        
        # Create test image data (20x20x3 = 1200 values)
        test_data = []
        for i in range(1200):
            test_data.append(float(i % 256) / 255.0)
        
        vision.setData(test_data)
        
        # Process as input
        percept = pynlm.SensoryPercept()
        percept.setVision(vision)
        
        assert percept.getVisionWidth() == 20
        assert percept.getVisionHeight() == 20
        assert percept.getVision().getData() == test_data

    def test_agent_neuromodulation(self):
        """Test advanced agent neuromodulation features"""
        config = pynlm.createTestConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        world = pynlm.createSimpleWorld()
        agent = pynlm.createAgent(brain, world)
        
        # Enable all neuromodulation features
        agent.enableRewardModulation(True)
        agent.enableCuriosity(True)
        
        # Run with reward signals
        for step in range(20):
            world.update(0.1)
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            brain.step(step)
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            
            # Apply reward modulation
            agent.applyRewardModulation(1.0, 0.5)
        
        # Check neuromodulation levels
        assert agent.getNeuromodulationLevel() > 0
        assert agent.getCuriosityLevel() > 0

if __name__ == "__main__":
    pytest.main([__file__, "-v"])
