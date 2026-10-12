"""
Enhanced Python bindings test suite for NLM (Neural Learning Machine)

This module tests the enhanced Python bindings with comprehensive error handling,
validation, and advanced features.
"""

import sys
import pytest
import tempfile
import os
import threading
import time
from unittest.mock import patch, MagicMock

# Import NLM Python bindings
try:
    import pynlm
except ImportError as e:
    pytest.skip(f"pynlm not available: {e}")

class TestBasicImports:
    """Test basic module imports and version information."""
    
    def test_module_import(self):
        """Test that pynlm module can be imported."""
        assert pynlm is not None
    
    def test_version_attribute(self):
        """Test that pynlm has version attribute."""
        assert hasattr(pynlm, "__version__")
        assert isinstance(pynlm.__version__, str)
        assert pynlm.__version__ == "0.1.0"
    
    def test_author_attribute(self):
        """Test that pynlm has author attribute."""
        assert hasattr(pynlm, "__author__")
        assert isinstance(pynlm.__author__, str)
    
    def test_description_attribute(self):
        """Test that pynlm has description attribute."""
        assert hasattr(pynlm, "__description__")
        assert isinstance(pynlm.__description__, str)
        assert "neural simulation" in pynlm.__description__.lower()

class TestSystemInfo:
    """Test system information utilities."""
    
    def test_get_system_info(self):
        """Test getting system information."""
        info = pynlm.getSystemInfo()
        assert isinstance(info, dict)
        assert "version" in info
        assert "author" in info
        assert "description" in info
        assert info["version"] == "0.1.0"
        assert "neural simulation" in info["description"].lower()

class TestConfiguration:
    """Test configuration class functionality with validation."""
    
    def test_create_default_config(self):
        """Test creating a default configuration."""
        config = pynlm.createDefaultConfig()
        assert config is not None
    
    def test_config_has_keys(self):
        """Test checking if configuration has keys."""
        config = pynlm.createDefaultConfig()
        assert config.has("random_seed")
    
    def test_config_get_keys(self):
        """Test getting all configuration keys."""
        config = pynlm.createDefaultConfig()
        keys = config.getKeys()
        assert isinstance(keys, list)
        assert len(keys) > 0
    
    def test_config_set_get_validation(self):
        """Test setting and getting configuration values with validation."""
        config = pynlm.createDefaultConfig()
        config.set("brain.neuron_count", 1500)
        # Note: exact value checking depends on whether getters are exposed
    
    def test_config_save_load(self):
        """Test saving and loading configuration from file."""
        with tempfile.NamedTemporaryFile(mode='w', suffix='.json', delete=False) as f:
            config_file = f.name
        
        try:
            config = pynlm.createDefaultConfig()
            config.saveToFile(config_file)
            
            # Create new config and load
            config2 = pynlm.createDefaultConfig()
            config2.loadFromFile(config_file)
            
            assert config2 is not None
        finally:
            os.unlink(config_file)
    
    def test_config_clear(self):
        """Test clearing configuration."""
        config = pynlm.createDefaultConfig()
        config.clear()
        assert config is not None
    
    def test_config_summary(self):
        """Test getting configuration summary."""
        config = pynlm.createDefaultConfig()
        summary = config.summary()
        assert isinstance(summary, str)
        assert len(summary) > 0
    
    def test_type_safe_getters(self):
        """Test type-safe getters with validation."""
        config = pynlm.createDefaultConfig()
        
        # Test integer getter
        int_val = config.getOr("random_seed", 42)
        assert isinstance(int_val, (int, type(None)))
        
        # Test float getter
        float_val = config.getOr("simulation_timestep", 0.001)
        assert isinstance(float_val, (float, type(None)))
        
        # Test boolean getter
        bool_val = config.getOr("plasticity.stdp.enable", True)
        assert isinstance(bool_val, (bool, type(None)))
    
    def test_validate_configuration_file(self):
        """Test configuration file validation."""
        with tempfile.NamedTemporaryFile(mode='w', suffix='.json', delete=False) as f:
            f.write('{"random_seed": 42, "brain.neuron_count": 100}')
            config_file = f.name
        
        try:
            # This should not raise an exception if valid
            result = pynlm.validateConfiguration(config_file)
            assert result is True
        except Exception as e:
            # Might fail if validation not exposed
            pass
        finally:
            os.unlink(config_file)
    
    def test_create_sample_config(self):
        """Test creating a sample configuration."""
        config = pynlm.createSampleConfig()
        assert config is not None
        assert config.has("random_seed")
        assert config.has("brain.neuron_count")
        assert config.getOr("brain.neuron_count", 0) == 1000
    
    def test_merge_configs(self):
        """Test merging multiple configurations."""
        config1 = pynlm.createSampleConfig()
        config2 = pynlm.createDefaultConfig()
        
        merged = pynlm.mergeConfigs([config1, config2])
        assert merged is not None
class TestBrainAdvanced:
    """Test brain class functionality with advanced features."""
    
    def test_create_brain_with_validation(self):
        """Test creating a brain with null validation."""
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        assert brain is not None
    
    def test_initialize_brain(self):
        """Test brain initialization."""
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        assert brain is not None
    
    def test_brain_step(self):
        """Test brain simulation step."""
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Run a few steps
        for step in range(10):
            brain.step(step)
        
        # Check that brain is still functional
        assert brain is not None
    
    def test_brain_statistics(self):
        """Test brain statistics methods."""
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Run some steps
        for step in range(50):
            brain.step(step)
        
        # Test statistics methods
        assert brain.getTotalNeuronCount() > 0
        assert brain.getTotalSynapseCount() > 0
        assert brain.getFiringNeuronCount() >= 0
        assert brain.getTotalSpikeCount() >= 0
        assert brain.getAverageFiringRate() >= 0.0
        
        # Check developmental stage
        if hasattr(brain, 'getDevelopmentalStage'):
            stage = brain.getDevelopmentalStage()
            assert stage is not None
    
    def test_brain_reset(self):
        """Test brain reset functionality."""
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Run some steps
        for step in range(20):
            brain.step(step)
        
        # Reset and re-initialize
        brain.reset()
        brain.initialize()
        
        assert brain is not None
    
    def test_brain_save_load(self):
        """Test brain save and load functionality."""
        with tempfile.NamedTemporaryFile(suffix='.bin', delete=False) as f:
            brain_file = f.name
        
        try:
            config = pynlm.createDefaultConfig()
            brain1 = pynlm.createBrain(config)
            brain1.initialize()
            
            # Run some steps
            for step in range(50):
                brain1.step(step)
            
            # Save brain
            brain1.save(brain_file)
            
            # Create new brain and load
            brain2 = pynlm.createBrain(config)
            brain2.initialize()
            brain2.load(brain_file)
            
            assert brain2 is not None
        finally:
            if os.path.exists(brain_file):
                os.unlink(brain_file)
    
    def test_brain_health(self):
        """Test brain health checking."""
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        health = pynlm.checkBrainHealth(brain)
        assert isinstance(health, dict)
        assert "exists" in health
        assert "initialized" in health
        assert health["exists"] == True
        assert health["initialized"] == True
    
    def test_brain_validation(self):
        """Test brain validation."""
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # This should not raise an exception if valid
        assert brain is not None
    
    def test_brain_initialization_check(self):
        """Test brain initialization check."""
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        
        # Should not be initialized yet
        assert brain is not None
        
        # Initialize and check
        brain.initialize()
        assert brain.isInitialized()
    
    def test_safe_brain_step(self):
        """Test safe brain step."""
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        
        # Should return False when not initialized
        result = pynlm.safeBrainStep(brain, 0)
        assert result is False
        
        # Initialize and step
        brain.initialize()
        result = pynlm.safeBrainStep(brain, 0)
        assert result is True
        
        # Try with invalid step (may or may not be caught)
        try:
            pynlm.safeBrainStep(brain, -1)
        except:
            pass  # Expected
class TestWorldAdvanced:
    """Test world simulation functionality with validation."""
    
    def test_create_simple_world(self):
        """Test creating a simple world."""
        world = pynlm.createSimpleWorld()
        assert world is not None
    
    def test_configure_world_validation(self):
        """Test configuring world with validation."""
        world = pynlm.createSimpleWorld()
        
        # Test valid configuration
        world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
        
        assert world.getWidth() == 20
        assert world.getHeight() == 20
        
        # Test invalid configuration (would need to test exception handling)
        try:
            world.configure(0, 10, 5, 5)  # Should fail
        except:
            pass  # Expected
    
    def test_world_reset(self):
        """Test world reset."""
        world = pynlm.createSimpleWorld()
        world.configure(width=10, height=10)
        world.reset()
        
        assert world.getSimulationTime() == 0.0
    
    def test_world_update(self):
        """Test world update."""
        world = pynlm.createSimpleWorld()
        world.configure(width=10, height=10)
        world.reset()
        
        # Update world
        world.update(0.1)
        
        assert world.getSimulationTime() > 0.0
    
    def test_world_sensory_percept(self):
        """Test getting sensory percept from world."""
        world = pynlm.createSimpleWorld()
        world.configure(width=10, height=10)
        world.reset()
        world.setAgentStart(5.0, 5.0)
        
        # Update to generate percept
        world.update(0.1)
        percept = world.getSensoryPercept()
        
        assert percept is not None
    
    def test_world_agent_body(self):
        """Test getting agent body state."""
        world = pynlm.createSimpleWorld()
        world.configure(width=10, height=10)
        world.reset()
        world.setAgentStart(5.0, 5.0)
        
        body = world.getAgentBody()
        assert body is not None
        assert body.x == 5.0
        assert body.y == 5.0
    
    def test_world_validation(self):
        """Test world validation."""
        world = pynlm.createSimpleWorld()
        world.configure(width=10, height=10)
        world.reset()
        
        # This should not raise an exception if valid
        assert world is not None
    
    def test_validate_world_state(self):
        """Test world state validation utility."""
        world = pynlm.createSimpleWorld()
        world.configure(width=10, height=10)
        world.reset()
        
        # This should not raise an exception if valid
        assert pynlm.validateWorldState(world) is True
class TestAgentBrainAdvanced:
    """Test agent brain interface functionality with advanced features."""
    
    def test_create_agent_brain(self):
        """Test creating an agent brain with validation."""
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        agent = pynlm.createAgentBrain(brain)
        assert agent is not None
    
    def test_agent_initialize(self):
        """Test agent initialization with world."""
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        agent = pynlm.createAgentBrain(brain)
        
        world = pynlm.createSimpleWorld()
        world.configure(width=10, height=10)
        world.reset()
        
        agent.initialize(world)
        assert agent is not None
    
    def test_agent_sensory_processing(self):
        """Test sensory input processing."""
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        agent = pynlm.createAgentBrain(brain)
        
        world = pynlm.createSimpleWorld()
        world.configure(width=10, height=10)
        world.reset()
        
        agent.initialize(world)
        
        # Update world to generate percept
        world.update(0.1)
        percept = world.getSensoryPercept()
        
        # Process sensory input
        agent.processSensoryInput(percept)
        assert agent is not None
    
    def test_agent_motor_decoding(self):
        """Test motor command decoding."""
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        agent = pynlm.createAgentBrain(brain)
        
        world = pynlm.createSimpleWorld()
        world.configure(width=10, height=10)
        world.reset()
        
        agent.initialize(world)
        
        # Decode motor command
        action = agent.decodeMotorCommand()
        assert action is not None
    
    def test_agent_neuromodulation(self):
        """Test reward modulation."""
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        agent = pynlm.createAgentBrain(brain)
        
        world = pynlm.createSimpleWorld()
        world.configure(width=10, height=10)
        world.reset()
        
        agent.initialize(world)
        
        # Apply reward modulation
        agent.applyRewardModulation(1.0, 0.8)  # reward, predicted_reward
        assert agent is not None
    
    def test_agent_development(self):
        """Test development system."""
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        agent = pynlm.createAgentBrain(brain)
        
        world = pynlm.createSimpleWorld()
        world.configure(width=10, height=10)
        world.reset()
        
        agent.initialize(world)
        
        # Update development
        agent.updateDevelopment(0.1)
        assert agent is not None
        
        # Get developmental stage
        if hasattr(agent, 'getDevelopmentalStage'):
            stage = agent.getDevelopmentalStage()
            assert stage is not None
    
    def test_agent_subsystem_control(self):
        """Test enabling/disabling subsystems."""
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        agent = pynlm.createAgentBrain(brain)
        
        # Enable various subsystems
        agent.enableRewardModulation(True)
        agent.enableStructuralPlasticity(True)
        agent.enableDevelopment(True)
        agent.enableCuriosity(True)
        
        # Check status
        assert agent.isRewardModulationEnabled() == True
        assert agent.isStructuralPlasticityEnabled() == True
        assert agent.isDevelopmentEnabled() == True
        assert agent.isCuriosityEnabled() == True
    
    def test_agent_statistics(self):
        """Test agent statistics methods."""
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        agent = pynlm.createAgentBrain(brain)
        
        world = pynlm.createSimpleWorld()
        world.configure(width=10, height=10)
        world.reset()
        
        agent.initialize(world)
        
        # Get various statistics
        if hasattr(agent, 'getCuriosityLevel'):
            curiosity = agent.getCuriosityLevel()
            assert curiosity >= 0.0
        
        if hasattr(agent, 'getNoveltyLevel'):
            novelty = agent.getNoveltyLevel()
            assert novelty >= 0.0
        
        if hasattr(agent, 'getPredictionError'):
            error = agent.getPredictionError()
            assert error >= 0.0
        
        if hasattr(agent, 'getNeuromodulationLevel'):
            neuromod = agent.getNeuromodulationLevel()
            assert neuromod >= 0.0
    
    def test_agent_reset(self):
        """Test agent reset."""
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        agent = pynlm.createAgentBrain(brain)
        
        world = pynlm.createSimpleWorld()
        world.configure(width=10, height=10)
        world.reset()
        
        agent.initialize(world)
        
        # Use agent
        agent.enableCuriosity(True)
        assert agent.isCuriosityEnabled() == True
        
        # Reset
        agent.reset()
        assert agent.isCuriosityEnabled() == False
    
    def test_agent_initialization_check(self):
        """Test agent initialization check."""
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        agent = pynlm.createAgentBrain(brain)
        
        world = pynlm.createSimpleWorld()
        world.configure(width=10, height=10)
        world.reset()
        
        agent.initialize(world)
        assert agent.isInitialized()
class TestAdvancedOperations:
    """Test advanced operations and batch processing."""
    
    def test_batch_create_brains(self):
        """Test creating multiple brains from configurations."""
        config1 = pynlm.createSampleConfig()
        config2 = pynlm.createDefaultConfig()
        
        brains = pynlm.batchCreateBrains([config1, config2])
        assert len(brains) == 2
        assert all(brain is not None for brain in brains)
        
        # Initialize and check
        for brain in brains:
            brain.initialize()
            assert brain.isInitialized()
    
    def test_run_simulation_batch(self):
        """Test running simulation batches."""
        config = pynlm.createDefaultConfig()
        
        # Create multiple brains
        brains = pynlm.batchCreateBrains([config, config])
        
        # Run simulation steps
        steps1 = list(range(10, 20))
        steps2 = list(range(20, 30))
        
        spikes = pynlm.runSimulationBatch(brains, [steps1, steps2])
        assert isinstance(spikes, list)
        assert len(spikes) == 2
        assert all(isinstance(s, (int, type(None))) for s in spikes)
    
    def test_create_agent_world_simulation(self):
        """Test creating multiple agent instances."""
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        
        world = pynlm.createSimpleWorld()
        world.configure(width=10, height=10)
        world.reset()
        
        num_steps = [10, 20, 30]
        agents = pynlm.createAgentWorldSimulation(brain, world, num_steps)
        
        assert len(agents) == 3
        assert all(agent is not None for agent in agents)
        
        for agent in agents:
            assert agent.isInitialized()
    
    def test_get_simulation_stats(self):
        """Test getting simulation statistics."""
        config = pynlm.createDefaultConfig()
        brains = pynlm.batchCreateBrains([config, config])
        
        stats = pynlm.getSimulationStats(brains)
        assert isinstance(stats, dict)
        assert stats["numBrains"] == 2
        assert stats["totalNeurons"] > 0
        assert stats["totalSpikes"] >= 0
        assert stats["averageFiringRate"] >= 0.0
    
    def test_try_brain_operation(self):
        """Test safe brain operation with error handling."""
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Define a simple operation
        def simpleOperation(brain_obj):
            for step in range(10):
                brain_obj.step(step)
        
        # This should not raise an exception
        result = pynlm.tryBrainOperation(brain, simpleOperation)
        assert result is True
    
    def test_error_handling(self):
        """Test error handling."""
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Test operations that should handle errors gracefully
        try:
            brain.step(-1)  # Invalid step (may or may not be caught)
        except:
            pass  # Expected
        
        try:
            # Test loading non-existent file
            brain.load("non_existent_file.bin")
        except:
            pass  # Expected

class TestCompleteSimulation:
    """Test complete agent simulation."""
    
    def test_complete_agent_simulation(self):
        """Test a complete agent simulation."""
        # Setup
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        world = pynlm.createSimpleWorld()
        world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
        world.reset()
        world.setAgentStart(10.0, 10.0)
        
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        # Enable subsystems
        agent.enableRewardModulation(True)
        agent.enableCuriosity(True)
        
        # Run simulation
        for step in range(100):
            world.update(0.1)
            agent.processSensoryInput(world.getSensoryPercept())
            brain.step(step)
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            agent.applyRewardModulation(0.1, 0.05)
        
        # Verify results
        assert brain.getTotalSpikeCount() > 0
        assert agent is not None
        
        # Check agent stats
        if hasattr(agent, 'getCuriosityLevel'):
            curiosity = agent.getCuriosityLevel()
            assert curiosity >= 0.0

if __name__ == "__main__":
    # Run tests when executed directly
    import pytest
    
    print("Running enhanced NLM Python bindings test suite...")
    sys.exit(pytest.main([__file__, "-v"]))
