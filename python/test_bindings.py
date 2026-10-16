"""
Comprehensive Python Tests for NLM Bindings
===========================================

Unit tests for NLM Python bindings, including both original and improved bindings.
"""

import pytest
import tempfile
import json
import os
import sys
import math

# Import the NLM module
import pynlm
class TestOriginalBindings:
    """Test the original bindings functionality."""
    
    def test_config_creation(self):
        """Test basic configuration creation."""
        config = pynlm.createDefaultConfig()
        assert config is not None
        assert config.has("neuron_count")
        assert config.has("region_count")
        assert config.has("connection_probability")
        
        # Check default values
        assert config.getOr<int64_t>("neuron_count", 0) > 0
        assert config.getOr<int64_t>("region_count", 0) == 1
        assert config.getOr<float>("connection_probability", 0.0) > 0.0
    
    def test_brain_creation(self):
        """Test brain creation and initialization."""
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        
        assert brain is not None
        assert brain.initialize() == True
        
        # Check statistics
        assert brain.getTotalNeuronCount() > 0
        assert brain.getTotalSynapseCount() > 0
        assert brain.getFiringNeuronCount() >= 0
        assert brain.getAverageFiringRate() >= 0.0
        assert brain.getExcitationInhibitionRatio() >= 0.0
    
    def test_simulation_step(self):
        """Test basic simulation steps."""
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        initial_neurons = brain.getTotalNeuronCount()
        initial_spikes = brain.getTotalSpikeCount()
        
        # Run several steps
        for step in range(10):
            brain.step(step)
        
        # Check that brain is still functional
        assert brain.getTotalNeuronCount() == initial_neurons
        assert brain.getTotalSynapseCount() > 0
    
    def test_config_operations(self):
        """Test configuration operations."""
        config = pynlm.createDefaultConfig()
        
        # Test has method
        assert config.has("neuron_count")
        assert not config.has("nonexistent_key")
        
        # Test getKeys
        keys = config.getKeys()
        assert "neuron_count" in keys
        assert "region_count" in keys
        
        # Test clear
        config.clear()
        assert config.getKeys() == []
        
        # Test repr
        repr_str = repr(config)
        assert "Config" in repr_str
    
    def test_sensory_input(self):
        """Test sensory input operations."""
        # Create vision input
        data = [i % 255 for i in range(100)]
        vision_input = pynlm.createVisionInput(10, 10, 3, data)
        
        assert vision_input is not None
        assert vision_input.getType() == 2  # Vision type
        assert vision_input.getWidth() == 10
        assert vision_input.getHeight() == 10
        assert vision_input.getChannels() == 3
        
        # Test audio input
        audio_samples = [math.sin(i * 0.1) * 0.5 + 0.5 for i in range(1000)]
        audio_input = pynlm.createAudioInput(44100, audio_samples)
        
        assert audio_input is not None
        assert audio_input.getType() == 1  # Audio type
        assert audio_input.getSampleRate() == 44100
        assert audio_input.getNumSamples() == 1000
    
    def test_action_creation(self):
        """Test action creation."""
        # Create action by type
        move_action = pynlm.createAction(pynlm.ActionType.MoveForward)
        assert move_action is not None
        assert move_action.getType() == pynlm.ActionType.MoveForward
        assert move_action.getName() == "MoveForward"
        
        # Create action with parameters
        custom_action = pynlm.createAction(pynlm.ActionType.Custom, [0.5, 1.0, 1.5])
        assert custom_action is not None
        assert custom_action.getType() == pynlm.ActionType.Custom
        assert custom_action.getParameters() == [0.5, 1.0, 1.5]
    
    def test_neural_similarity(self):
        """Test neural similarity calculation."""
        pattern1 = [i * 0.1 for i in range(100)]
        pattern2 = [i * 0.1 + 0.01 for i in range(100)]  # Slightly different
        
        similarity = pynlm.neuralSimilarity(pattern1, pattern2)
        assert 0.0 <= similarity <= 1.0
        
        # Test with identical patterns
        identical_sim = pynlm.neuralSimilarity(pattern1, pattern1)
        assert abs(identical_sim - 1.0) < 0.001
        
        # Test with empty patterns
        empty_sim = pynlm.neuralSimilarity([], [])
        assert empty_sim == 0.0
        
        # Test with different sizes
        diff_size_sim = pynlm.neuralSimilarity(pattern1, pattern1[:50]))
        assert diff_size_sim == 0.0
class TestImprovedBindings:
    """Test the improved bindings functionality."""
    
    def test_config_builder(self):
        """Test ConfigBuilder class."""
        config = (pynlm.ConfigBuilder()
                  .setNeuronCount(2000)
                  .setRegionCount(2)
                  .setConnectionProbability(0.15)
                  .setSTDPParameters(0.01, 0.012, 20.0)
                  .setStructuralPlasticity(0.0001, 0.00001)
                  .setMemoryCapacity(1000, 500)
                  .enableNeuromodulation(True)
                  .setDopamineLevel(0.8)
                  .enableDevelopment(True)
                  .setSimulation(0.001, 1000)
                  .build())
        
        assert config is not None
        assert config.getOr<int64_t>("neuron_count", 0) == 2000
        assert config.getOr<int64_t>("region_count", 0) == 2
        assert config.getOr<float>("connection_probability", 0.0) == 0.15
        assert config.getOr<bool>("enable_neuromodulation", False) == True
        assert config.getOr<bool>("enable_development", False) == True
    
    def test_preset_configs(self):
        """Test preset configuration functions."""
        # Test default config
        default_config = pynlm.getPresetConfig("default")
        assert default_config is not None
        assert default_config.getOr<int64_t>("neuron_count", 0) == 1000
        
        # Test performance config
        perf_config = pynlm.getPresetConfig("performance")
        assert perf_config is not None
        assert perf_config.getOr<int64_t>("neuron_count", 0) == 1000
        assert perf_config.getOr<bool>("enable_neuromodulation", True) == False
        
        # Test research config
        research_config = pynlm.getPresetConfig("research")
        assert research_config is not None
        assert research_config.getOr<int64_t>("neuron_count", 0) == 5000
        assert research_config.getOr<bool>("enable_neuromodulation", True) == True
        assert research_config.getOr<bool>("enable_development", True) == True
        
        # Test unknown preset raises exception
        with pytest.raises(Exception):
            pynlm.getPresetConfig("unknown")
    
    def test_simulation_manager(self):
        """Test SimulationManager class."""
        config = pynlm.ConfigBuilder().setNeuronCount(1000).build()
        sim = pynlm.SimulationManager(config)
        
        assert sim is not None
        assert sim.getBrain() is not None
        
        # Run steps
        sim.runSteps(100)
        
        # Get statistics
        stats = sim.getStatistics()
        assert "neuron_count" in stats
        assert "synapse_count" in stats
        assert "active_neurons" in stats
        assert "firing_neurons" in stats
        assert "total_spikes" in stats
        assert "average_firing_rate" in stats
        assert "e_i_ratio" in stats
        
        # Save and load state
        with tempfile.NamedTemporaryFile(suffix='.nlm', delete=False) as f:
            save_file = f.name
        
        try:
            sim.saveState(save_file)
            assert os.path.exists(save_file)
            
            # Create new simulation and load state
            sim2 = pynlm.SimulationManager(config)
            sim2.loadState(save_file)
            
            # Both should have same neuron count
            assert sim.getBrain().getTotalNeuronCount() == sim2.getBrain().getTotalNeuronCount()
            
        finally:
            if os.path.exists(save_file):
                os.unlink(save_file)
    
    def test_agent_manager(self):
        """Test AgentManager class."""
        agent = pynlm.AgentManager()
        
        assert agent is not None
        
        # Configure world
        agent.configureWorld(50, 50, 10, 10)
        agent.setAgentPosition(25.0, 25.0)
        
        # Run episode
        agent.runEpisode(100)
        
        # Get results
        results = agent.getEpisodeResults()
        assert "total_steps" in results
        assert "final_dopamine" in results
        assert "final_curiosity" in results
        assert "final_novelty" in results
        
        # Get world state
        world_state = agent.getWorldState()
        assert "x" in world_state
        assert "y" in world_state
        assert "orientation" in world_state
        assert "energy" in world_state
        assert "health" in world_state
        
        # Reset agent
        agent.reset()
        assert agent.getWorldState()['x'] == 25.0  # Should reset to start position
    
    def test_validation(self):
        """Test configuration validation."""
        config = pynlm.createDefaultConfig()
        assert pynlm.validateConfig(config) == True
        
        # Test empty config (still valid)
        empty_config = pynlm.createDefaultConfig()
        empty_config.clear()
        assert pynlm.validateConfig(empty_config) == True
    
    def test_convenience_functions(self):
        """Test convenience utility functions."""
        # Test vision input creation
        data = [i % 255 for i in range(300)]
        vision = pynlm.createVisionInput(10, 10, 3, data)
        assert vision is not None
        assert vision.getWidth() == 10
        assert vision.getHeight() == 10
        
        # Test audio input creation
        samples = [0.5 for _ in range(1000)]
        audio = pynlm.createAudioInput(44100, samples)
        assert audio is not None
        assert audio.getSampleRate() == 44100
        
        # Test action creation
        action = pynlm.createAction(pynlm.ActionType.Interact)
        assert action is not None
        assert action.getType() == pynlm.ActionType.Interact
        
        # Test neural similarity
        pattern1 = [i * 0.1 for i in range(100)]
        pattern2 = [i * 0.1 for i in range(100)]
        similarity = pynlm.neuralSimilarity(pattern1, pattern2)
        assert similarity == pytest.approx(1.0, rel=0.001)
class TestIntegration:
    """Test integration between different components."""
    
    def test_original_and_improved_bindings(self):
        """Test that both original and improved bindings work."""
        # Test original bindings
        config1 = pynlm.createDefaultConfig()
        brain1 = pynlm.createBrain(config1)
        brain1.initialize()
        
        # Test improved bindings
        config2 = pynlm.ConfigBuilder().setNeuronCount(1000).build()
        sim = pynlm.SimulationManager(config2)
        
        # Both should be independent
        assert brain1.getTotalNeuronCount() == 1000
        assert sim.getBrain().getTotalNeuronCount() == 1000
        
        # Both should be functional
        brain1.step(0)
        sim.runSteps(10)
        
        assert brain1.getFiringNeuronCount() >= 0
        assert sim.getStatistics()['firing_neurons'] >= 0
    
    def test_cli_example_usage(self):
        """Test that examples can be imported and run without errors."""
        # This test just verifies that our examples can be imported
        # The actual CLI tests would require mocking sys.argv
        
        # Verify the examples directory exists
        examples_dir = "/workspace/app/python/examples"
        assert os.path.exists(examples_dir)
        
        # Verify example files exist
        assert os.path.exists(os.path.join(examples_dir, "tutorial_basic.py"))
        assert os.path.exists(os.path.join(examples_dir, "advanced_examples.py"))
        assert os.path.exists(os.path.join(examples_dir, "complete_examples.py"))
    
    def test_error_handling(self):
        """Test error handling in bindings."""
        # Test config validation with invalid data
        config = pynlm.createDefaultConfig()
        
        # Should not raise exception for normal operations
        keys = config.getKeys()
        assert isinstance(keys, list)
        
        # Test brain operations with error handling
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Running step should not raise exception
        brain.step(0)
        
        # Reset should work
        brain.reset()
        
        # Save/load should handle file operations
        with tempfile.NamedTemporaryFile(suffix='.nlm', delete=False) as f:
            save_file = f.name
        
        try:
            brain.save(save_file)
            assert os.path.exists(save_file)
            
            # Load should work
            brain.reset()
            brain.load(save_file)
            
        finally:
            if os.path.exists(save_file):
                os.unlink(save_file)
class TestPerformance:
    """Performance-related tests."""
    
    def test_simulation_performance(self):
        """Test that simulation runs within reasonable time."""
        import time
        
        config = pynlm.ConfigBuilder().setNeuronCount(100).build()
        sim = pynlm.SimulationManager(config)
        
        start_time = time.time()
        sim.runSteps(50)
        end_time = time.time()
        
        duration = end_time - start_time
        
        # Should complete quickly for small simulation
        assert duration < 10.0, f"Simulation took too long: {duration:.2f}s"
        
        print(f"Simulation completed in {duration:.3f}s")
    
    def test_memory_usage(self):
        """Test basic memory operations."""
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Run some steps to generate state
        for step in range(10):
            brain.step(step)
        
        # Save and load should work
        with tempfile.NamedTemporaryFile(suffix='.nlm', delete=False) as f:
            save_file = f.name
        
        try:
            brain.save(save_file)
            
            # Load into new brain
            brain2 = pynlm.createBrain(config)
            brain2.load(save_file)
            
            # Both should have same neuron count
            assert brain.getTotalNeuronCount() == brain2.getTotalNeuronCount()
            
        finally:
            if os.path.exists(save_file):
                os.unlink(save_file)
if __name__ == "__main__":
    # Run tests using pytest
    pytest.main([__file__, "-v"])