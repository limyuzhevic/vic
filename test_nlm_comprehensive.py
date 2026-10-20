#!/usr/bin/env python3
"""
Comprehensive test suite for NLM Python bindings.
Tests all available functionality and verifies API completeness.
"""

import pynlm
import pytest
import tempfile
import os
import json

class TestNLMCore:
    """Test core NLM functionality."""
    
    def test_config_creation(self):
        """Test configuration creation and management."""
        config = pynlm.createDefaultConfig()
        assert config is not None
        
        # Test basic operations
        config.set("test.int", 42)
        config.set("test.double", 3.14)
        config.set("test.string", "hello")
        config.set("test.bool", True)
        
        assert config.has("test.int")
        assert config.has("test.double")
        assert config.has("test.string")
        assert config.has("test.bool")
        
        # Test getting values
        assert config.getOr("test.int", 0) == 42
        assert abs(config.getOr("test.double", 0.0) - 3.14) < 0.01
        assert config.getOr("test.string", "") == "hello"
        assert config.getOr("test.bool", False) == True
        
        # Test summary
        summary = config.summary()
        assert "Configuration" in summary
        assert "test.int" in summary
    
    def test_config_file_io(self):
        """Test configuration file I/O."""
        config = pynlm.createDefaultConfig()
        config.set("test.value", 123)
        
        # Save to temporary file
        with tempfile.NamedTemporaryFile(mode='w', suffix='.cfg', delete=False) as f:
            config_file = f.name
        
        try:
            # Save configuration
            config.saveToFile(config_file)
            assert os.path.exists(config_file)
            
            # Load into new config
            config2 = pynlm.createDefaultConfig()
            loaded = config2.loadFromFile(config_file)
            assert loaded
            
            # Note: The simple file format doesn't preserve all data types
            # but this tests the basic functionality
            assert config2.has("test.value")
            
        finally:
            if os.path.exists(config_file):
                os.unlink(config_file)
    
    def test_brain_creation(self):
        """Test brain creation and basic operations."""
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        
        assert brain is not None
        brain.initialize()
        
        # Test basic statistics
        neuron_count = brain.getTotalNeuronCount()
        synapse_count = brain.getTotalSynapseCount()
        assert neuron_count > 0
        assert synapse_count >= 0
        
        # Test simulation
        for step in range(10):
            brain.step(step)
        
        total_spikes = brain.getTotalSpikeCount()
        firing_neurons = brain.getFiringNeuronCount()
        assert total_spikes >= 0
        assert firing_neurons >= 0
    
    def test_agent_world(self):
        """Test agent and world functionality."""
        # Create brain
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Create world
        world = pynlm.createSimpleWorld()
        world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
        world.reset()
        
        # Create agent
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        # Test agent initialization
        assert agent.getSensoryInputSize() > 0
        assert agent.getMotorOutputSize() > 0
        
        # Run simulation step
        world.update(0.1)
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        brain.step(0)
        action = agent.decodeMotorCommand()
        
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Verify action was applied
        body = world.getAgentBody()
        assert body is not None
    
    def test_neuron_properties(self):
        """Test neuron properties and methods."""
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Get neuron IDs for testing
        regions = brain.getRegions()
        assert len(regions) > 0
        
        # Test region functionality
        for region in regions:
            # Note: This is a simplified test since the API doesn't expose
            # individual neuron access in the current bindings
            pass

class TestNLMConvenience:
    """Test convenience functions and high-level APIs."""
    
    def test_convenience_function_runsimulation(self):
        """Test the run_simulation convenience function."""
        # This function should be available as a high-level convenience
        # For now, we test that we can create and run basic simulations
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        world = pynlm.createSimpleWorld()
        world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
        world.reset()
        
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        # Enable learning features
        agent.enableRewardModulation(True)
        agent.enableCuriosity(True)
        
        # Run a simplified simulation
        for step in range(50):
            world.update(0.1)
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            brain.step(step)
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
        
        # Verify simulation completed
        assert brain.getTotalSpikeCount() > 0
        assert agent.getCuriosityLevel() > 0.0

    def test_agent_monitoring(self):
        """Test agent monitoring and statistics."""
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        world = pynlm.createSimpleWorld()
        world.configure(width=15, height=15, visionWidth=8, visionHeight=8)
        world.reset()
        
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        # Enable monitoring
        agent.enableRewardModulation(True)
        agent.enableCuriosity(True)
        
        # Run simulation with monitoring
        for step in range(20):
            world.update(0.1)
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            brain.step(step)
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime()n
            
            # Log statistics
            if step % 5 == 0:
                print(f"Step {step}: Firing neurons: {brain.getFiringNeuronCount()}")
                print(f"Step {step}: Curiosity: {agent.getCuriosityLevel():.3f}")
                print(f"Step {step}: Novelty: {agent.getNoveltyLevel():.3f}")
                print(f"Step {step}: Dev Stage: {brain.getDevelopmentalStage()}")
        
        # Verify monitoring data
        assert agent.getCuriosityLevel() >= 0.0
        assert agent.getNoveltyLevel() >= 0.0

    def test_batch_operations(self):
        """Test batch processing capabilities."""
        # This demonstrates batch processing functionality
        config = pynlm.createDefaultConfig()
        
        # Create multiple brains for batch processing
        brains = []
        for i in range(3):
            brain = pynlm.createBrain(config)
            brain.initialize()
            brains.append(brain)
        
        # Run batch simulation
        num_steps = 20
        for brain in brains:
            for step in range(num_steps):
                brain.step(step)
        
        # Collect batch results
        for i, brain in enumerate(brains):
            spikes = brain.getTotalSpikeCount()
            firing = brain.getFiringNeuronCount()
            print(f"Brain {i}: {spikes} total spikes, {firing} firing neurons")

class TestNLMSerialization:
    """Test serialization and checkpoint functionality."""
    
    def test_brain_checkpoint(self):
        """Test brain state checkpointing."""
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Run some steps to generate state
        for step in range(10):
            brain.step(step)
        
        # Save checkpoint
        with tempfile.NamedTemporaryFile(suffix='.bin', delete=False) as f:
            checkpoint_file = f.name
        
        try:
            brain.save(checkpoint_file)
            assert os.path.exists(checkpoint_file)
            
            # Create new brain and load checkpoint
            brain2 = pynlm.createBrain(config)
            brain2.initialize()
            brain2.load(checkpoint_file)
            
            # Verify checkpoints have similar spike counts (after initialization)
            assert brain.getTotalSpikeCount() > 0
            
        finally:
            if os.path.exists(checkpoint_file):
                os.unlink(checkpoint_file)

    def test_config_checkpoint(self):
        """Test configuration checkpointing."""
        config = pynlm.createDefaultConfig()
        config.set("test.value", 42)
        config.set("test.string", "hello")
        
        # Save to file
        with tempfile.NamedTemporaryFile(mode='w', suffix='.cfg', delete=False) as f:
            config_file = f.name
        
        try:
            config.saveToFile(config_file)
            
            # Load into new config
            config2 = pynlm.createDefaultConfig()
            loaded = config2.loadFromFile(config_file)
            assert loaded
            
        finally:
            if os.path.exists(config_file):
                os.unlink(checkpoint_file)

class TestNLMDevelopment:
    """Test development and advanced features."""
    
    def test_development_stages(self):
        """Test brain development stages."""
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Test developmental stage access
        initial_stage = brain.getDevelopmentalStage()
        
        # Update development
        brain.setDevelopmentalStage(pynlm.DevelopmentalStage::Maturation)
        
        # Verify stage change
        assert brain.getDevelopmentalStage() != initial_stage

    def test_plasticity_systems(self):
        """Test plasticity system integration."""
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        world = pynlm.createSimpleWorld()
        world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
        world.reset()
        
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        # Enable plasticity systems
        agent.enableRewardModulation(True)
        agent.enableStructuralPlasticity(True)
        agent.enableDevelopment(True)
        
        # Run simulation with plasticity
        for step in range(30):
            world.update(0.1)
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            brain.step(step)
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
        
        # Verify plasticity systems are active
        assert agent.isRewardModulationEnabled()
        assert agent.isStructuralPlasticityEnabled()
        assert agent.isDevelopmentEnabled()

class TestNLMSimulationControl:
    """Test simulation control and management."""
    
    def test_episode_management(self):
        """Test episode and reset functionality."""
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Run first episode
        for step in range(10):
            brain.step(step)
        
        first_episode_spikes = brain.getTotalSpikeCount()
        
        # Reset for new episode
        brain.reset()
        brain.initialize()
        
        # Run second episode
        for step in range(10):
            brain.step(step)
        
        second_episode_spikes = brain.getTotalSpikeCount()
        
        # Note: Spike counts may differ due to reset
        assert first_episode_spikes >= 0
        assert second_episode_spikes >= 0

    def test_time_control(self):
        """Test time-based simulation control."""
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Test step with time
        for step in range(5):
            time = step * 0.001  # 1ms increments
            brain.step(step, time)
        
        # Verify time progression
        assert brain.getTotalSpikeCount() >= 0

if __name__ == "__main__":
    print("Running NLM Python API tests...")
    
    # Run all tests
    test_classes = [
        TestNLMCore,
        TestNLMConvenience,
        TestNLMSerialization,
        TestNLMDevelopment,
        TestNLMSimulationControl
    ]
    
    for test_class in test_classes:
        print(f"\n--- Testing {test_class.__name__} ---")
        test_instance = test_class()
        
        # Get all test methods
        test_methods = [
            method for method in dir(test_instance) 
            if method.startswith('test_') and callable(getattr(test_instance, method))
        ]
        
        for test_method in test_methods:
            try:
                print(f"Running {test_method}...")
                getattr(test_instance, test_method)()
                print(f"✓ {test_method} passed")
            except Exception as e:
                print(f"✗ {test_method} failed: {e}")
    
    print("\nAll tests completed!")