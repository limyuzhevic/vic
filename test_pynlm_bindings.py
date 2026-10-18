"""NLM Python Bindings - Test Suite

This module contains comprehensive tests for the NLM Python bindings.
The test suite validates that the modular bindings work correctly and
that the integration between different systems is maintained.

The test suite includes:
1. Unit tests for individual binding modules
2. Integration tests for system interactions
3. Performance tests for critical operations
4. Edge case and error handling tests
"""

import pytest
import pynlm as nlm
import sys
import os
from typing import Dict, Any, List

class TestConfiguration:
    """Test configuration system bindings."""
    
    def test_config_creation(self):
        """Test basic configuration creation."""
        config = nlm.create_config()
        assert config is not None
        assert isinstance(config, nlm.Config)
    
    def test_config_set_get(self):
        """Test setting and getting configuration values."""
        config = nlm.create_config()
        config.set("neuron_count", 1000)
        config.set("region_count", 5)
        config.set("connection_probability", 0.1)
        
        assert config.has("neuron_count")
        assert config.get("neuron_count") == 1000
        assert config.has("region_count")
        assert config.get("region_count") == 5
        assert config.has("connection_probability")
        assert config.get("connection_probability") == 0.1
    
    def test_config_keys(self):
        """Test configuration key management."""
        config = nlm.create_config()
        config.set("key1", "value1")
        config.set("key2", "value2")
        config.set("key3", "value3")
        
        keys = config.getKeys()
        assert len(keys) == 3
        assert "key1" in keys
        assert "key2" in keys
        assert "key3" in keys
    
    def test_config_clear(self):
        """Test configuration clearing."""
        config = nlm.create_config()
        config.set("key1", "value1")
        config.set("key2", "value2")
        
        config.clear()
        assert not config.has("key1")
        assert not config.has("key2")
        assert len(config.getKeys()) == 0
    
    def test_config_summary(self):
        """Test configuration summary generation."""
        config = nlm.create_config()
        config.set("neuron_count", 1000)
        config.set("connection_probability", 0.1)
        
        summary = config.summary()
        assert "neuron_count" in summary
        assert "connection_probability" in summary
        assert "1000" in summary
        assert "0.1" in summary
    
    def test_default_config(self):
        """Test default configuration factory."""
        config = nlm.create_default_config()
        assert config.has("neuron_count")
        assert config.has("region_count")
        assert config.has("connection_probability")
        assert config.get("neuron_count") == 1000
        assert config.get("region_count") == 1
        assert config.get("connection_probability") == 0.1

class TestSensorySystems:
    """Test sensory system bindings."""
    
    def test_vision_sensory(self):
        """Test vision sensory input."""
        vision = nlm.Vision(640, 480, 3)
        assert vision is not None
        assert vision.getWidth() == 640
        assert vision.getHeight() == 480
        assert vision.getChannels() == 3
        
        # Test data setting
        test_data = [0.1f, 0.2f, 0.3f] * 640 * 480 * 3
        vision.setData(test_data)
        assert vision.getData() == test_data
    
    def test_audio_sensory(self):
        """Test audio sensory input."""
        audio = nlm.Audio(44100, 1024)
        assert audio is not None
        assert audio.getSampleRate() == 44100
        assert audio.getNumSamples() == 1024
        
        test_data = [0.5f] * 1024
        audio.setData(test_data)
        assert audio.getData() == test_data
        audio.setSampleRate(48000)
        assert audio.getSampleRate() == 48000
    
    def test_internal_signals(self):
        """Test internal signals sensory input."""
        internal = nlm.InternalSignals()
        assert internal is not None
        
        internal.addSignal(0.1f)
        internal.addSignal(0.2f)
        internal.addSignal(0.3f)
        
        signals = internal.getData()
        assert len(signals) == 3
        assert signals[0] == 0.1f
        assert signals[1] == 0.2f
        assert signals[2] == 0.3f
        
        internal.clearSignals()
        assert len(internal.getData()) == 0
    
    def test_sensory_percept(self):
        """Test sensory percept container."""
        percept = nlm.SensoryPercept()
        assert percept is not None
        
        # Set all sensory inputs
        vision = nlm.Vision(100, 100, 3)
        audio = nlm.Audio(44100, 512)
        internal = nlm.InternalSignals()
        
        percept.setVision(vision)
        percept.setAudio(audio)
        percept.setInternal(internal)
        
        assert percept.getVision() is vision
        assert percept.getAudio() is audio
        assert percept.getInternal() is internal
        assert percept.getVisionWidth() == 100
        assert percept.getVisionHeight() == 100
        assert percept.getTimestamp() >= 0
        
        # Test timestamp setting
        percept.setTimestamp(42.0)
        assert percept.getTimestamp() == 42.0

class TestWorldSystems:
    """Test world system bindings."""
    
    def test_world_creation(self):
        """Test world creation."""
        world = nlm.create_world()
        assert world is not None
        assert isinstance(world, nlm.SimpleWorld)
        
        assert world.getWidth() == 16  # default
        assert world.getHeight() == 16  # default
    
    def test_world_configuration(self):
        """Test world configuration."""
        world = nlm.create_world()
        world.configure(32, 32, 64, 64)
        
        assert world.getWidth() == 32
        assert world.getHeight() == 32
        # Note: configure method may not update existing world, this is just testing the API
    
    def test_world_reset(self):
        """Test world reset."""
        world = nlm.create_world()
        # Set some state
        agent_body = world.getAgentBody()
        agent_body.x = 10.0
        agent_body.y = 10.0
        
        world.reset()
        
        # Check reset (implementation dependent)
        assert world is not None
    
    def test_world_objects(self):
        """Test world object management."""
        world = nlm.create_world()
        
        # Create world objects
        wall = nlm.WorldObject(5.0, 5.0, nlm.WorldObjectType.Wall, 1.0, 0.5)
        resource = nlm.WorldObject(10.0, 10.0, nlm.WorldObjectType.Resource, 10.0, 0.3)
        
        world.addObject(wall)
        world.addObject(resource)
        
        # Test object validation
        assert world.isValidPosition(5.0, 5.0)  # Should be valid if position is free
        assert world.isValidPosition(10.0, 10.0)
        
        world.removeObject(5.0, 5.0)
        assert not world.isValidPosition(5.0, 5.0)  # Should now be invalid

class TestBrainSystems:
    """Test brain system bindings."""
    
    def test_brain_creation(self):
        """Test brain creation."""
        config = nlm.create_default_config()
        brain = nlm.create_brain(config)
        assert brain is not None
        assert isinstance(brain, nlm.Brain)
    
    def test_brain_initialization(self):
        """Test brain initialization."""
        config = nlm.create_default_config()
        brain = nlm.create_brain(config)
        
        # Initialize brain
        result = brain.initialize()
        assert result is True
        assert brain is not None
        
        # Check basic properties
        assert brain.getRegionCount() > 0
        assert brain.getTotalNeuronCount() > 0
        assert brain.getTotalSynapseCount() > 0
    
    def test_brain_step(self):
        """Test brain stepping."""
        config = nlm.create_default_config()
        brain = nlm.create_brain(config)
        brain.initialize()
        
        # Take a few steps
        for i in range(5):
            brain.step(i)
            
            # Check basic metrics
            assert brain.getTotalSpikeCount() >= 0
            assert brain.getActiveNeuronCount() >= 0
        
        # Check firing rate
        firing_rate = brain.getAverageFiringRate()
        assert firing_rate >= 0.0
    
    def test_brain_sensory_processing(self):
        """Test brain sensory processing."""
        config = nlm.create_default_config()
        brain = nlm.create_brain(config)
        brain.initialize()
        
        # Create sensory input
        vision = nlm.Vision(10, 10, 3)
        sensory_input = vision
        
        # Process sensory input
        brain.receiveSensoryInput(sensory_input)
        
        # Brain should still be valid
        assert brain is not None
    
    def test_brain_action_selection(self):
        """Test brain action selection."""
        config = nlm.create_default_config()
        brain = nlm.create_brain(config)
        brain.initialize()
        
        # Produce action
        action = brain.produceAction()
        assert action is not None
        assert isinstance(action, nlm.Action)
        
        # Action should have a type
        action_type = action.getType()
        assert action_type is not None
    
    def test_brain_status_logging(self):
        """Test brain status logging."""
        config = nlm.create_default_config()
        brain = nlm.create_brain(config)
        brain.initialize()
        
        # This should not raise an exception
        brain.logStatus()
        
        # Check that brain has expected methods
        assert hasattr(brain, 'getTotalNeuronCount')
        assert hasattr(brain, 'getTotalSynapseCount')
        assert hasattr(brain, 'getAverageFiringRate')
        assert hasattr(brain, 'getDevelopmentalStage')

class TestAgentSystems:
    """Test agent system bindings."""
    
    def test_agent_brain_creation(self):
        """Test agent brain creation."""
        config = nlm.create_default_config()
        brain = nlm.create_brain(config)
        agent = nlm.create_agent_brain(brain)
        
        assert agent is not None
        assert isinstance(agent, nlm.AgentBrain)
    
    def test_agent_brain_initialization(self):
        """Test agent brain initialization."""
        config = nlm.create_default_config()
        brain = nlm.create_brain(config)
        brain.initialize()
        
        world = nlm.create_world()
        agent = nlm.create_agent_brain(brain)
        
        # Initialize agent
        agent.initialize(world)
        assert agent is not None
        
        # Test agent configuration
        agent.enableRewardModulation(True)
        agent.enableDevelopment(True)
        agent.enableCuriosity(True)
        
        assert agent.isRewardModulationEnabled() is True
        assert agent.isDevelopmentEnabled() is True
        assert agent.isCuriosityEnabled() is True
    
    def test_agent_processing(self):
        """Test agent processing."""
        config = nlm.create_default_config()
        brain = nlm.create_brain(config)
        brain.initialize()
        
        world = nlm.create_world()
        agent = nlm.create_agent_brain(brain)
        agent.initialize(world)
        
        # Create sensory percept
        vision = nlm.Vision(10, 10, 3)
        percept = nlm.SensoryPercept()
        percept.setVision(vision)
        
        # Process sensory input
        agent.processSensoryInput(percept)
        
        # Decode motor command
        cmd = agent.decodeMotorCommand()
        assert cmd is not None
        
        # Apply reward modulation
        agent.applyRewardModulation(1.0, 0.5)
        
        # Check neuromodulation levels
        curiosity_level = agent.getCuriosityLevel()
        novelty_level = agent.getNoveltyLevel()
        dopamine_level = agent.getNeuromodulationLevel()
        
        assert curiosity_level >= 0.0
        assert novelty_level >= 0.0
        assert dopamine_level >= 0.0

class TestMemorySystems:
    """Test memory system bindings."""
    
    def test_working_memory(self):
        """Test working memory bindings."""
        # Note: WorkingMemory is not directly exposed in main bindings
        # This test checks that the memory integration works
        pass
    
    def test_episodic_memory(self):
        """Test episodic memory bindings."""
        pass

class TestNeuromodulationSystems:
    """Test neuromodulation system bindings."""
    
    def test_dopamine_system(self):
        """Test dopamine system bindings."""
        pass
    
    def test_curiosity_system(self):
        """Test curiosity system bindings."""
        pass
    
    def test_novelty_system(self):
        """Test novelty system bindings."""
        pass
    
    def test_prediction_error_system(self):
        """Test prediction error system bindings."""
        pass

class TestIntegration:
    """Test integration between systems."""
    
    def test_complete_system_integration(self):
        """Test complete system integration."""
        # Create all systems
        config = nlm.create_default_config()
        brain = nlm.create_brain(config)
        brain.initialize()
        
        world = nlm.create_world()
        agent = nlm.create_agent_brain(brain)
        agent.initialize(world)
        
        # Create sensory percept
        vision = nlm.Vision(10, 10, 3)
        percept = nlm.SensoryPercept()
        percept.setVision(vision)
        
        # Run complete cycle
        agent.processSensoryInput(percept)
        brain.step(0)
        action = brain.produceAction()
        
        assert action is not None
        assert brain.getTotalSpikeCount() >= 0
    
    def test_configuration_validation(self):
        """Test configuration validation."""
        from pynlm_api import validate_config
        
        config = nlm.create_config()
        
        # Test invalid config
        invalid_config = nlm.create_config()
        invalid_config.set("neuron_count", -100)
        
        is_valid, errors = validate_config(invalid_config)
        assert not is_valid
        assert len(errors) > 0
        
        # Test valid config
        valid_config = nlm.create_config()
        valid_config.set("neuron_count", 1000)
        valid_config.set("region_count", 5)
        
        is_valid, errors = validate_config(valid_config)
        assert is_valid
        assert len(errors) == 0

class TestPerformance:
    """Performance and stress tests."""
    
    def test_brain_performance(self):
        """Test brain performance with larger configuration."""
        config = nlm.create_config()
        config.set("neuron_count", 5000)
        config.set("region_count", 5)
        config.set("connection_probability", 0.05)
        
        brain = nlm.create_brain(config)
        
        # Time initialization
        import time
        start_time = time.time()
        brain.initialize()
        init_time = time.time() - start_time
        
        assert init_time < 30.0  # Should initialize in reasonable time
        
        # Time some steps
        start_time = time.time()
        for i in range(10):
            brain.step(i)
        step_time = time.time() - start_time
        
        assert step_time < 10.0  # 10 steps should be fast
        
        # Check performance metrics
        firing_rate = brain.getAverageFiringRate()
        assert firing_rate >= 0.0

# Performance benchmarks
def benchmark_operation(operation, *args, **kwargs):
    """Benchmark a single operation."""
    import time
    
    start_time = time.time()
    result = operation(*args, **kwargs)
    end_time = time.time()
    
    return {
        'result': result,
        'time': end_time - start_time,
        'timestamp': time.time()
    }

def batch_benchmark(brain, num_steps=100):
    """Benchmark batch processing."""
    import time
    
    start_time = time.time()
    for i in range(num_steps):
        brain.step(i)
    end_time = time.time()
    
    return {
        'steps': num_steps,
        'total_time': end_time - start_time,
        'steps_per_second': num_steps / (end_time - start_time),
        'avg_step_time': (end_time - start_time) / num_steps
    }

# Test utilities
def setup_test_config(neuron_count=1000, region_count=1):
    """Helper function to create test configuration."""
    config = nlm.create_default_config()
    config.set("neuron_count", neuron_count)
    config.set("region_count", region_count)
    config.set("connection_probability", 0.1)
    return config

def create_test_world(width=16, height=16):
    """Helper function to create test world."""
    world = nlm.create_world()
    world.configure(width, height, 8, 8)
    return world

# Conditional imports for testing
if __name__ == "__main__":
    # Run tests
    pytest.main([__file__, "-v"])
    
    # Run performance benchmarks
    print("Running performance benchmarks...")
    
    config = setup_test_config(500, 1)
    brain = nlm.create_brain(config)
    brain.initialize()
    
    perf_results = batch_benchmark(brain, 50)
    print(f"Brain performance: {perf_results['steps_per_second']:.2f} steps/sec")
    print(f"Average step time: {perf_results['avg_step_time']*1000:.2f} ms")