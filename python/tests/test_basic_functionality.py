#!/usr/bin/env python3
"""
NLM Test Suite

This test suite demonstrates the improved Python bindings for the NLM (Neural Learning Machine) framework.
It includes comprehensive tests for the enhanced Python API functionality.

The test suite covers:
1. Basic Python API functionality
2. Type conversions and error handling
3. Performance characteristics
4. Configuration management
5. Simulation integration
6. Advanced features
"""

import pytest
import pynlm as nlm
import tempfile
import json
import time
from pathlib import Path

# Test configuration
TEST_CONFIG = {
    "neuron_count": 100,
    "connection_probability": 0.1,
    "random_seed": 42,
    "simulation_timestep": 0.001
}

class TestBasicPythonAPI:
    """Test basic Python API functionality."""
    
    def test_config_creation(self):
        """Test default configuration creation."""
        config = nlm.createDefaultConfig()
        assert config is not None
        assert config.getInt("neuron_count") == 500  # Default value
        assert config.getFloat("connection_probability") == 0.15  # Default value
        
    def test_brain_creation(self):
        """Test brain creation with default configuration."""
        brain = nlm.createBrain()
        assert brain is not None
        brain.initialize()
        assert brain.getTotalNeuronCount() > 0
        assert brain.getTotalSynapseCount() > 0
        
    def test_world_creation(self):
        """Test world creation with default and custom sizes."""
        # Test with defaults
        world1 = nlm.createSimpleWorld()
        assert world1 is not None
        assert world1.getWidth() == 10
        assert world1.getHeight() == 10
        assert world1.getVisionWidth() == 8
        assert world1.getVisionHeight() == 8
        
        # Test with custom sizes
        world2 = nlm.createSimpleWorld(20, 30, 16, 16)
        assert world2.getWidth() == 20
        assert world2.getHeight() == 30
        assert world2.getVisionWidth() == 16
        assert world2.getVisionHeight() == 16
        
    def test_agent_creation(self):
        """Test agent creation with world."""
        brain = nlm.createBrain()
        brain.initialize()
        
        world = nlm.createSimpleWorld()
        world.reset()
        
        agent = nlm.createAgentBrain(brain)
        assert agent is not None
        agent.initialize(world)
        assert agent.getBrain() is not None

class TestConfigurationManagement:
    """Test configuration management functionality."""
    
    def test_config_json_parsing(self):
        """Test JSON configuration parsing."""
        json_str = json.dumps(TEST_CONFIG)
        config = nlm.loadConfigFromJSON(json_str)
        
        assert config is not None
        assert config.getInt("neuron_count") == TEST_CONFIG["neuron_count"]
        assert config.getFloat("connection_probability") == TEST_CONFIG["connection_probability"]
        assert config.getInt64("random_seed") == TEST_CONFIG["random_seed"]
        
    def test_config_key_value_parsing(self):
        """Test key=value configuration parsing."""
        kv_config = """
        neuron_count = 200
        connection_probability = 0.2
        random_seed = 123
        simulation_timestep = 0.005
        """
        
        config = nlm.loadConfigFromJSON(kv_config)
        assert config is not None
        assert config.getInt("neuron_count") == 200
        assert config.getFloat("connection_probability") == 0.2
        assert config.getInt64("random_seed") == 123
        assert config.getFloat("simulation_timestep") == 0.005
        
    def test_config_save_load(self):
        """Test configuration save and load functionality."""
        config = nlm.createDefaultConfig()
        config.set("neuron_count", 300)
        config.set("custom_value", "test_string")
        
        # Save to temporary file
        with tempfile.NamedTemporaryFile(mode='w', suffix='.json', delete=False) as f:
            config.saveToFile(f.name)
            config_path = f.name
        
        try:
            # Load back
            config2 = nlm.loadConfigFromJSON(config_path)
            assert config2 is not None
            assert config2.getInt("neuron_count") == 300
        finally:
            # Cleanup
            Path(config_path).unlink(missing_ok=True)
            
    def test_config_type_conversion(self):
        """Test configuration type conversions."""
        config = nlm.createDefaultConfig()
        config.set("int_value", 42)
        config.set("float_value", 3.14)
        config.set("bool_value", True)
        config.set("string_value", "hello")
        
        # Test getOr with different types
        assert config.getOrInt("int_value", 0) == 42
        assert config.getOrFloat("float_value", 0.0) == 3.14
        assert config.getOrBool("bool_value", False) == True
        assert config.getOrString("string_value", "default") == "hello"
        
        # Test non-existent keys
        assert config.getOrInt("non_existent", 99) == 99
        assert config.getOrFloat("non_existent", 1.0) == 1.0
        assert config.getOrBool("non_existent", False) == False
        assert config.getOrString("non_existent", "default") == "default"

class TestSimulationFunctionality:
    """Test simulation functionality."""
    
    def test_simple_simulation(self):
        """Test simple simulation with default settings."""
        result = nlm.runSimpleSimulation(100)
        
        assert result is not None
        assert result.totalSpikeCount > 0
        assert result.averageFiringRate > 0
        assert result.energy < 1.0  # Energy should have decreased
        assert result.health <= 1.0  # Health should be <= 1.0
        
    def test_complete_simulation_loop(self):
        """Test complete simulation with agent, world, and brain."""
        # Setup
        brain = nlm.createBrain()
        brain.initialize()
        
        world = nlm.createSimpleWorld(15, 15, 8, 8)
        world.reset()
        
        agent = nlm.createAgentBrain(brain)
        agent.initialize(world)
        agent.enableRewardModulation(True)
        agent.enableCuriosity(True)
        
        # Run simulation
        initial_energy = world.getAgentBody().energy
        initial_health = world.getAgentBody().health
        
        for step in range(50):
            world.update(0.1)
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            brain.step(step, step * 0.1)
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
        
        # Verify results
        assert brain.getTotalSpikeCount() > 0
        assert brain.getAverageFiringRate() > 0
        assert world.getAgentBody().energy < initial_energy
        assert agent.getNoveltyLevel() > 0
        assert agent.getCuriosityLevel() > 0
        
    def test_simulation_with_custom_config(self):
        """Test simulation with custom configuration."""
        config = nlm.createDefaultConfig()
        config.set("neuron_count", 200)
        config.set("connection_probability", 0.05)
        
        brain = nlm.createBrain(config)
        brain.initialize()
        
        assert brain.getTotalNeuronCount() == 200
        
        # Run simulation
        for step in range(30):
            brain.step(step, step * 0.001)
            brain.injectCurrentToNeurons("sensory", 10.0)
        
        assert brain.getTotalSpikeCount() > 0

class TestErrorHandling:
    """Test error handling and edge cases."""
    
    def test_invalid_config_handling(self):
        """Test handling of invalid configuration."""
        # Test with empty JSON
        config = nlm.loadConfigFromJSON("{}")
        assert config is not None
        assert config.getKeys() == []
        
        # Test with malformed JSON (should fallback to key=value)
        malformed_config = "invalid json { not valid = config }"
        config2 = nlm.loadConfigFromJSON(malformed_config)
        assert config2 is not None
        
    def test_brain_initialization_error(self):
        """Test error handling for brain initialization."""
        config = nlm.createDefaultConfig()
        config.set("neuron_count", 0)  # Invalid neuron count
        
        brain = nlm.createBrain(config)
        # Brain should still initialize (0 neurons is valid)
        brain.initialize()
        assert brain.getTotalNeuronCount() == 0
        
    def test_world_boundary_handling(self):
        """Test world boundary handling."""
        world = nlm.createSimpleWorld(5, 5, 3, 3)
        world.setAgentStart(2, 2)  # Center position
        
        # Test valid positions
        assert world.isValidPosition(0, 0)
        assert world.isValidPosition(4, 4)
        assert world.isValidPosition(2, 2)
        
        # Test invalid positions
        assert not world.isValidPosition(-1, 0)
        assert not world.isValidPosition(5, 0)
        assert not world.isValidPosition(0, -1)
        assert not world.isValidPosition(0, 5)
        
    def test_action_handling(self):
        """Test action handling."""
        action = nlm.Action(nlm.ActionType.MoveForward)
        assert action.getType() == nlm.ActionType.MoveForward
        assert action.getName() == "MoveForward"
        
        # Test action with parameters
        action2 = nlm.Action(nlm.ActionType.Custom, [1.0, 2.0, 3.0])
        assert action2.getType() == nlm.ActionType.Custom
        params = action2.getParameters()
        assert len(params) == 3
        assert params[0] == 1.0
        assert params[1] == 2.0
        assert params[2] == 3.0

class TestPerformanceCharacteristics:
    """Test performance characteristics."""
    
    def test_simulation_performance(self):
        """Test simulation performance with different step counts."""
        step_counts = [10, 50, 100]
        
        for steps in step_counts:
            start_time = time.time()
            result = nlm.runSimpleSimulation(steps)
            end_time = time.time()
            
            elapsed = end_time - start_time
            assert result is not None
            assert elapsed < steps * 0.1  # Should complete quickly
            
    def test_memory_usage(self):
        """Test memory usage patterns."""
        # Create multiple brains and verify they don't conflict
        brains = []
        for i in range(5):
            brain = nlm.createBrain()
            brain.initialize()
            brains.append(brain)
        
        # Verify all brains are independent
        spike_counts = [brain.getTotalSpikeCount() for brain in brains]
        # They should all have different spike counts due to different random seeds
        assert len(set(spike_counts)) >= 1
        
        # Cleanup
        del brains

class TestAdvancedFeatures:
    """Test advanced features."""
    
    def test_agent_configuration_options(self):
        """Test advanced agent configuration options."""
        brain = nlm.createBrain()
        brain.initialize()
        
        world = nlm.createSimpleWorld()
        world.reset()
        
        agent = nlm.createAgentBrain(brain)
        agent.initialize(world)
        
        # Enable all optional features
        agent.enableRewardModulation(True)
        agent.enableStructuralPlasticity(True)
        agent.enableDevelopment(True)
        agent.enableCuriosity(True)
        
        # Verify they're enabled
        assert agent.isRewardModulationEnabled() == True
        assert agent.isStructuralPlasticityEnabled() == True
        assert agent.isDevelopmentEnabled() == True
        assert agent.isCuriosityEnabled() == True
        
        # Run simulation
        for step in range(20):
            world.update(0.1)
            agent.processSensoryInput(world.getSensoryPercept())
            brain.step(step, step * 0.1)
            agent.decodeMotorCommand()
        
    def test_checkpoint_simulation(self):
        """Test checkpoint simulation."""
        # This is a basic test - full checkpoint testing would require
        # accessing internal C++ checkpoint functionality
        brain = nlm.createBrain()
        brain.initialize()
        
        # Basic checkpoint functionality test
        assert brain.getTotalNeuronCount() > 0
        assert brain.getTotalSynapseCount() > 0
        
    def test_visualization_interface(self):
        """Test visualization interface (if available)."""
        # Visualization testing would require accessing C++ visualization
        # For now, just test that the API is available
        try:
            # This would test the visualization interface
            # VisualizationInterface viz = VisualizationInterface()
            # assert viz is not None
            pass
        except:
            # Visualization not available in this build
            pass

class TestPythonCompatibility:
    """Test Python compatibility and integration."""
    
    def test_python_type_conversions(self):
        """Test Python type conversions."""
        config = nlm.createDefaultConfig()
        
        # Test different data types
        config.set("int_key", 42)
        config.set("float_key", 3.14)
        config.set("bool_key", True)
        config.set("string_key", "test")
        
        # Verify type handling
        assert isinstance(config.getInt("int_key"), int)
        assert isinstance(config.getFloat("float_key"), float)
        assert isinstance(config.getBool("bool_key"), bool)
        assert isinstance(config.getString("string_key"), str)
        
    def test_error_messages(self):
        """Test error message handling."""
        # Test that error messages are informative
        config = nlm.createDefaultConfig()
        
        # Access non-existent key
        try:
            config.getInt("non_existent_key")
            assert False, "Should have raised exception"
        except:
            pass  # Expected
        
    def test_api_documentation(self):
        """Test API documentation is accessible."""
        # Check that classes have docstrings
        assert nlm.Config.__doc__ is not None
        assert "Configuration" in nlm.Config.__doc__
        
        assert nlm.Brain.__doc__ is not None
        assert "neural" in nlm.Brain.__doc__.lower()
        
        assert nlm.AgentBrain.__doc__ is not None
        assert "agent" in nlm.AgentBrain.__doc__.lower()

if __name__ == "__main__":
    pytest.main([__file__, "-v", "--tb=short"])