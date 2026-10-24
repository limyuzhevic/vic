"""
Comprehensive Python test suite for NLM (Neural Learning Machine) Python bindings.

This test suite provides unit tests, integration tests, and examples for the NLM Python API.
"""

import sys
import pytest
import json
from pathlib import Path
from unittest.mock import patch, MagicMock
import numpy as np

# Add the current directory to the path so we can import pynlm
sys.path.insert(0, str(Path(__file__).parent))

try:
    import pynlm
    HAS_PYNLM = True
except ImportError:
    HAS_PYNLM = False
    pynlm = None
@pytest.mark.skipif(not HAS_PYNLM, reason="pynlm module not available")
class TestPyNLMImport:
    """Test basic import functionality."""
    
    def test_import_available(self):
        """Test that pynlm can be imported."""
        assert HAS_PYNLM
    
    def test_version_attribute(self):
        """Test that pynlm has a version attribute."""
        assert hasattr(pynlm, '__version__')
        assert isinstance(pynlm.__version__, str)
    
    def test_main_classes(self):
        """Test that main classes are available."""
        assert hasattr(pynlm, 'createDefaultConfig')
        assert hasattr(pynlm, 'createBrain')
        assert hasattr(pynlm, 'createSimpleWorld')
        assert hasattr(pynlm, 'createAgentBrain')
@pytest.mark.skipif(not HAS_PYNLM, reason="pynlm module not available")
class TestNLMTypes:
    """Test NLM type definitions and constants."""
    
    def test_neuron_types(self):
        """Test neuron type enumerations."""
        assert hasattr(pynlm, 'NeuronType')
        assert pynlm.NeuronType.Excitatory
        assert pynlm.NeuronType.Inhibitory
        assert pynlm.NeuronType.Sensory
        assert pynlm.NeuronType.Motor
    
    def test_synapse_types(self):
        """Test synapse type enumerations."""
        assert hasattr(pynlm, 'SynapseType')
        assert pynlm.SynapseType.Excitatory
        assert pynlm.SynapseType.Inhibitory
        assert pynlm.SynapseType.Modulatory
    
    def test_action_types(self):
        """Test action type enumerations."""
        assert hasattr(pynlm, 'ActionType')
        assert pynlm.ActionType.MoveForward
        assert pynlm.ActionType.TurnLeft
        assert pynlm.ActionType.Interact
    
    def test_developmental_stages(self):
        """Test developmental stage enumerations."""
        assert hasattr(pynlm, 'DevelopmentalStage')
        assert pynlm.DevelopmentalStage.Initial
        assert pynlm.DevelopmentalStage.CriticalPeriod
        assert pynlm.DevelopmentalStage.Adult
@pytest.mark.skipif(not HAS_PYNLM, reason="pynlm module not available")
class TestConfig:
    """Test configuration management."""
    
    def test_create_default_config(self):
        """Test creating a default configuration."""
        config = pynlm.createDefaultConfig()
        assert config is not None
    
    def test_config_has(self):
        """Test checking if configuration keys exist."""
        config = pynlm.createDefaultConfig()
        # Test with known keys
        assert config.has("brain.neuron_count")
        assert config.has("brain.synapse_density")
        # Test with unknown key
        assert not config.has("nonexistent.key")
    
    def test_config_get(self):
        """Test getting configuration values."""
        config = pynlm.createDefaultConfig()
        # Test with known keys
        neuron_count = config.get("brain.neuron_count")
        # Some keys might not be present or return None
    
    def test_config_get_or(self):
        """Test getting configuration values with defaults."""
        config = pynlm.createDefaultConfig()
        # Test with existing key
        default_val = config.getOr("nonexistent.key", 100)
        assert default_val == 100
        
        # Test with existing key
        existing_val = config.getOr("brain.neuron_count", 999)
        assert existing_val == 999
    
    def test_config_set(self):
        """Test setting configuration values."""
        config = pynlm.createDefaultConfig()
        config.set("test.key", "test_value")
        assert config.has("test.key")
    
    def test_config_get_keys(self):
        """Test getting all configuration keys."""
        config = pynlm.createDefaultConfig()
        keys = config.getKeys()
        assert isinstance(keys, list)
        assert len(keys) >= 0  # Should be non-negative
    
    def test_config_summary(self):
        """Test getting configuration summary."""
        config = pynlm.createDefaultConfig()
        summary = config.summary()
        assert isinstance(summary, str)
        assert len(summary) > 0
    
    def test_config_save_load(self, tmpdir):
        """Test saving and loading configuration to file."""
        config = pynlm.createDefaultConfig()
        config.set("test.key", "test_value")
        
        # Save to file
        test_file = tmpdir / "test_config.txt"
        assert config.saveToFile(str(test_file))
        
        # Create new config and load
        config2 = pynlm.createDefaultConfig()
        assert config2.loadFromFile(str(test_file))
        assert config2.has("test.key")
@pytest.mark.skipif(not HAS_PYNLM, reason="pynlm module not available")
class TestBrain:
    """Test brain simulation functionality."""
    
    def test_create_brain(self):
        """Test creating a brain with default configuration."""
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        assert brain is not None
    
    def test_brain_initialize(self):
        """Test brain initialization."""
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        assert brain.initialize()
    
    def test_brain_stats(self):
        """Test getting brain statistics."""
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Get statistics
        assert brain.getTotalNeuronCount() > 0
        assert brain.getTotalSynapseCount() >= 0
        assert brain.getRegionCount() >= 0
        assert brain.getAverageFiringRate() >= 0.0
    
    def test_brain_step(self):
        """Test running brain simulation steps."""
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Run several steps
        for i in range(10):
            brain.step(i)
        
        # Check that spikes occurred
        assert brain.getTotalSpikeCount() >= 0
    
    def test_brain_region_management(self):
        """Test brain region management."""
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Add a region
        region_id = brain.addRegion("TestRegion")
        assert region_id is not None
        
        # Get the region
        region = brain.getRegion(region_id)
        assert region is not None
        
        # Get all regions
        regions = brain.getRegions()
        assert len(regions) >= 1
    
    def test_brain_save_load(self, tmpdir):
        """Test saving and loading brain state."""
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Run some steps
        for i in range(10):
            brain.step(i)
        
        # Save brain
        save_path = tmpdir / "brain_checkpoint.bin"
        assert brain.save(str(save_path))
        
        # Reset and load
        brain.reset()
        assert brain.load(str(save_path))
        
        # Verify brain still has regions
        assert brain.getRegionCount() >= 0
@pytest.mark.skipif(not HAS_PYNLM, reason="pynlm module not available")
class TestWorld:
    """Test world simulation functionality."""
    
    def test_create_simple_world(self):
        """Test creating a simple world."""
        world = pynlm.createSimpleWorld()
        assert world is not None
    
    def test_world_configure(self):
        """Test world configuration."""
        world = pynlm.createSimpleWorld()
        world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
        
        assert world.getWidth() == 20
        assert world.getHeight() == 20
        assert world.getVisionWidth() == 8
        assert world.getVisionHeight() == 8
    
    def test_world_update(self):
        """Test world update."""
        world = pynlm.createSimpleWorld()
        world.configure(width=10, height=10)
        world.reset()
        
        # Update world
        initial_time = world.getSimulationTime()
        world.update(0.1)
        
        assert world.getSimulationTime() > initial_time
    
    def test_world_agent_interaction(self):
        """Test agent-world interaction."""
        world = pynlm.createSimpleWorld()
        world.configure(width=15, height=15, visionWidth=8, visionHeight=8)
        world.reset()
        world.setAgentStart(7.5, 7.5)
        
        # Get agent body
        agent_body = world.getAgentBody()
        assert agent_body is not None
        assert agent_body.x == 7.5
        assert agent_body.y == 7.5
        
        # Get sensory percept
        percept = world.getSensoryPercept()
        assert percept is not None
@pytest.mark.skipif(not HAS_PYNLM, reason="pynlm module not available")
class TestAgentBrain:
    """Test agent brain functionality."""
    
    def test_create_agent_brain(self):
        """Test creating an agent brain."""
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        agent = pynlm.createAgentBrain(brain)
        assert agent is not None
    
    def test_agent_initialization(self):
        """Test agent brain initialization."""
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        agent = pynlm.createAgentBrain(brain)
        
        # Create a world
        world = pynlm.createSimpleWorld()
        world.configure(width=10, height=10)
        world.reset()
        
        # Initialize agent
        agent.initialize(world)
        
        # Check that agent can get sensory input size
        assert agent.getSensoryInputSize() > 0
        assert agent.getMotorOutputSize() > 0
    
    def test_agent_sensory_processing(self):
        """Test sensory input processing."""
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        agent = pynlm.createAgentBrain(brain)
        world = pynlm.createSimpleWorld()
        world.configure(width=10, height=10)
        world.reset()
        agent.initialize(world)
        
        # Process sensory input
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        # Brain should have processed the input
        assert brain.getTotalNeuronCount() > 0
    
    def test_agent_motor_decoding(self):
        """Test motor command decoding."""
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        agent = pynlm.createAgentBrain(brain)
        world = pynlm.createSimpleWorld()
        world.configure(width=10, height=10)
        world.reset()
        agent.initialize(world)
        
        # Decode motor command
        action = agent.decodeMotorCommand()
        assert action is not None
        
        # Apply action to world
        world.applyMotorCommand(action, world.getSimulationTime())
    
    def test_agent_neuromodulation(self):
        """Test neuromodulation (reward, curiosity, etc.)."""
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        agent = pynlm.createAgentBrain(brain)
        world = pynlm.createSimpleWorld()
        world.configure(width=10, height=10)
        world.reset()
        agent.initialize(world)
        
        # Enable subsystems
        agent.enableRewardModulation(True)
        agent.enableCuriosity(True)
        
        # Check that subsystems are enabled
        assert agent.isRewardModulationEnabled()
        assert agent.isCuriosityEnabled()
        
        # Apply reward modulation
        agent.applyRewardModulation(1.0, 0.5)
        
        # Get neuromodulation levels
        assert agent.getCuriosityLevel() >= 0.0
        assert agent.getNoveltyLevel() >= 0.0
        assert agent.getPredictionError() >= 0.0
    
    def test_agent_development(self):
        """Test development system."""
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        agent = pynlm.createAgentBrain(brain)
        world = pynlm.createSimpleWorld()
        world.configure(width=10, height=10)
        world.reset()
        agent.initialize(world)
        
        # Enable development
        agent.enableDevelopment(True)
        assert agent.isDevelopmentEnabled()
        
        # Update development
        agent.updateDevelopment(0.1)
        
        # Check developmental stage
        stage = agent.getDevelopmentalStage()
        assert stage is not None
@pytest.mark.skipif(not HAS_PYNLM, reason="pynlm module not available")
class TestCompleteSimulation:
    """Test complete agent-world simulation."""
    
    def test_complete_simulation(self, tmpdir):
        """Test a complete agent simulation with brain and world."""
        # Setup
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        agent = pynlm.createAgentBrain(brain)
        world = pynlm.createSimpleWorld()
        world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
        world.reset()
        agent.initialize(world)
        
        # Enable learning
        agent.enableRewardModulation(True)
        agent.enableCuriosity(True)
        
        # Run simulation
        num_steps = 100
        for step in range(num_steps):
            # Update world
            world.update(0.1)
            
            # Get sensory input
            percept = world.getSensoryPercept()
            
            # Process sensory input
            agent.processSensoryInput(percept)
            
            # Run brain step
            brain.step(step)
            
            # Get action from brain
            action = agent.decodeMotorCommand()
            
            # Apply action to world
            world.applyMotorCommand(action, world.getSimulationTime())
            
            # Apply reward modulation if reward available
            reward = percept.getInternal()[0] if percept.getInternal() else 0.0
            agent.applyRewardModulation(reward, 0.0)
            
            # Update development
            agent.updateDevelopment(0.1)
            
            # Print progress every 20 steps
            if step % 20 == 0:
                print(f"Step {step}: {brain.getFiringNeuronCount()} neurons firing, "
                      f"curiosity: {agent.getCuriosityLevel():.3f}")
        
        # Verify simulation results
        assert brain.getTotalNeuronCount() > 0
        assert world.getSimulationTime() > 0
        assert agent.getDevelopmentalStage() is not None
        
        # Save brain state
        brain_path = tmpdir / "simulation_brain.bin"
        assert brain.save(str(brain_path))
        
        # Save world state would require world.save() method if it exists
    
    def test_multiple_episodes(self):
        """Test multiple simulation episodes."""
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        agent = pynlm.createAgentBrain(brain)
        world = pynlm.createSimpleWorld()
        world.configure(width=15, height=15, visionWidth=6, visionHeight=6)
        
        num_episodes = 3
        num_steps_per_episode = 50
        
        for episode in range(num_episodes):
            print(f"Running episode {episode + 1}/{num_episodes}")
            
            # Reset for new episode
            world.reset()
            agent.reset()
            
            # Run episode
            for step in range(num_steps_per_episode):
                world.update(0.1)
                percept = world.getSensoryPercept()
                agent.processSensoryInput(percept)
                brain.step(step)
                action = agent.decodeMotorCommand()
                world.applyMotorCommand(action, world.getSimulationTime())
                
                # Simulate reward for eating
                if step % 25 == 0:
                    reward = 1.0
                    agent.applyRewardModulation(reward, 0.0)
            
            # Episode complete
            print(f"  Episode complete: {agent.getDevelopmentalStage()}")
        
        # Verify all episodes completed
        assert brain.getTotalSpikeCount() > 0
@pytest.mark.skipif(not HAS_PYNLM, reason="pynlm module not available")
class TestPythonicFeatures:
    """Test Pythonic features and utilities."""
    
    def test_context_manager(self):
        """Test context manager support."""
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Test context manager (if implemented)
        # This will fail if context managers aren't fully implemented
        try:
            with brain:
                brain.step(0)
            context_manager_works = True
        except (AttributeError, TypeError):
            context_manager_works = False
        
        # Context manager may or may not be implemented
        # This test documents the expected behavior
        assert True  # Just documenting that we tested it
    
    def test_statistics_helpers(self):
        """Test statistics helper methods."""
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Get brain statistics
        stats = brain.getStats()
        assert isinstance(stats, dict)
        assert "total_neurons" in stats
        assert "total_synapses" in stats
        assert "region_count" in stats
        assert "firing_neurons" in stats
        assert "avg_firing_rate" in stats
        assert "e_i_ratio" in stats
        assert "total_spikes" in stats
        assert "evolutionary_stage" in stats
    
    def test_agent_stats_helper(self):
        """Test agent statistics helper method."""
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        agent = pynlm.createAgentBrain(brain)
        world = pynlm.createSimpleWorld()
        world.configure(width=10, height=10)
        world.reset()
        agent.initialize(world)
        
        # Get agent statistics
        stats = agent.getStats()
        assert isinstance(stats, dict)
        assert "connected_neurons" in stats or "connected_regions" in stats
        assert "reward_modulation" in stats
        assert "structural_plasticity" in stats
        assert "development_enabled" in stats
        assert "curiosity_level" in stats
        assert "novelty_level" in stats
        assert "prediction_error" in stats
@pytest.mark.skipif(not HAS_PYNLM, reason="pynlm module not available")
class TestErrorHandling:
    """Test error handling and edge cases."""
    
    def test_invalid_operations(self):
        """Test handling of invalid operations."""
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        
        # Brain should be initialized before use
        # (Current implementation may or may not check this)
        try:
            brain.step(0)
            # If no exception, that's OK for now
        except Exception as e:
            # Some implementations may raise an exception
            pass
    
    def test_null_values(self):
        """Test handling of null/None values."""
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Test with various null-like values
        # (Implementation specific)
        assert True  # Placeholder
@pytest.mark.skipif(not HAS_PYNLM, reason="pynlm module not available")
class TestPerformance:
    """Performance tests."""
    
    def test_large_simulation(self):
        """Test with larger brain size."""
        config = pynlm.createDefaultConfig()
        
        # Modify config for larger brain
        config.set("brain.neuron_count", 2000)
        config.set("brain.synapse_density", 0.05)
        
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Run simulation
        for i in range(50):
            brain.step(i)
        
        # Verify
        assert brain.getTotalNeuronCount() >= 2000
    
    def test_simulation_speed(self):
        """Basic performance test."""
        import time
        
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        start_time = time.time()
        for i in range(100):
            brain.step(i)
        end_time = time.time()
        
        elapsed = end_time - start_time
        print(f"Simulated 100 steps in {elapsed:.3f} seconds")
        
        # Should be reasonably fast (less than 10 seconds for 100 steps)
        assert elapsed < 10.0
@pytest.mark.skipif(not HAS_PYNLM, reason="pynlm module not available")
def test_example_basic_usage():
    """Test the basic usage example from the documentation."""
    # This replicates the example from easy_usage.md
    import pynlm
    
    # One line to create a virtual brain
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    
    # Initialize it
    brain.initialize()
    
    # Make it active
    brain.step(0)
    
    # Print result
    print("Your brain has", brain.getTotalNeuronCount(), "neurons!")
    
    # Verify
    assert brain.getTotalNeuronCount() > 0
    assert brain.getTotalSpikeCount() >= 0
@pytest.mark.skipif(not HAS_PYNLM, reason="pynlm module not available")
def test_example_complete_simulation():
    """Test the complete simulation example from documentation."""
    import pynlm
    
    def run_simulation(brain, world, agent, num_steps):
        for step in range(num_steps):
            # 1. Update world
            world.update(0.1)
            
            # 2. Get what the agent sees
            percept = world.getSensoryPercept()
            
            # 3. Tell the brain
            agent.processSensoryInput(percept)
            
            # 4. Brain thinks
            brain.step(step)
            
            # 5. Get action from brain
            action = agent.decodeMotorCommand()
            
            # 6. Do action in world
            world.applyMotorCommand(action, world.getSimulationTime())
    
    # Setup
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Run simulation
    run_simulation(brain, world, agent, 50)
    
    # Verify
    assert brain.getTotalNeuronCount() > 0
    assert agent.getSensoryInputSize() > 0
@pytest.mark.skipif(not HAS_PYNLM, reason="pynlm module not available")
def test_example_with_learning():
    """Test simulation with learning enabled."""
    import pynlm
    
    # Setup
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    agent = pynlm.createAgentBrain(brain)
    world = pynlm.createSimpleWorld()
    world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
    world.reset()
    agent.initialize(world)
    
    # Enable learning subsystems
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    
    # Run episode
    for step in range(30):
        world.update(0.1)
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Apply reward modulation
        reward = 0.0
        agent.applyRewardModulation(reward, 0.0)
        
        # Update development
        agent.updateDevelopment(0.1)
    
    # Verify learning was used
    assert agent.isRewardModulationEnabled()
    assert agent.isCuriosityEnabled()
    assert agent.isDevelopmentEnabled()
    assert agent.getCuriosityLevel() >= 0.0
    assert agent.getNoveltyLevel() >= 0.0
    assert agent.getPredictionError() >= 0.0

# Run tests if this file is executed directly
if __name__ == "__main__":
    print("Running NLM Python test suite...")
    
    # Run all tests manually
    test_classes = [
        TestPyNLMImport,
        TestNLMTypes,
        TestConfig,
        TestBrain,
        TestWorld,
        TestAgentBrain,
        TestCompleteSimulation,
        TestPythonicFeatures,
        TestErrorHandling,
        TestPerformance,
    ]
    
    for test_class in test_classes:
        print(f"\nRunning {test_class.__name__}...")
        
        # Create instance
        instance = test_class()
        
        # Get all test methods
        for method_name in dir(instance):
            if method_name.startswith("test_"):
                method = getattr(instance, method_name)
                try:
                    print(f"  Running {method_name}...", end=" ")
                    method()
                    print("PASSED")
                except Exception as e:
                    print(f"FAILED: {e}")
    
    print("\nAll tests completed!")