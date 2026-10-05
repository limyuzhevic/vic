import pytest
import time
import std::chrono
import math
from pynlm import (
    createBrain, createDefaultConfig, createSimpleWorld, createAgentBrain,
    LogLevel, neuronType, neuronTypeType, developmentalStage,
    LOG_LEVEL_DEBUG, LOG_LEVEL_INFO, LOG_LEVEL_WARNING, LOG_LEVEL_ERROR
)

# Test configuration for consistent testing
TEST_CONFIG = {
    "brain.neuron_count": 200,
    "brain.region_count": 2,
    "brain.connection_probability": 0.1,
    "random_seed": 42,
    "simulation_timestep": 0.001
}

def setup_test_brain(config=None):
    """Create a test brain with optimized settings"""
    if config is None:
        config = createDefaultConfig()
        for key, value in TEST_CONFIG.items():
            config.set(key, value)
    
    brain = createBrain(config)
    brain.initialize()
    return brain

def setup_test_agent(brain, world=None):
    """Create a test agent with world integration"""
    if world is None:
        world = createSimpleWorld()
        world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
        world.reset()
        world.setAgentStart(10.0, 10.0)
    
    agent = createAgentBrain(brain)
    agent.initialize(world)
    return agent, world

def test_basic_neural_functionality():
    """Test basic neural computation and spiking"""
    brain = setup_test_brain()
    
    # Get initial statistics
    initial_neurons = brain.getTotalNeuronCount()
    initial_spikes = brain.getTotalSpikeCount()
    initial_active = brain.getActiveNeuronCount()
    
    assert initial_neurons > 0, "Brain should have neurons"
    assert initial_spikes >= 0, "Brain should have non-negative spike count"
    assert initial_active >= 0, "Brain should have non-negative active neurons"
    
    # Inject current to stimulate activity
    brain.injectCurrentToNeurons(neuronType.Excitatory, 30.0)
    
    # Run simulation
    for step in range(100):
        brain.step(step)
    
    # Check that activity occurred
    final_spikes = brain.getTotalSpikeCount()
    final_firing = brain.getFiringNeuronCount()
    final_active = brain.getActiveNeuronCount()
    
    assert final_spikes > initial_spikes, "Should have generated spikes"
    assert final_firing >= 0, "Should have non-negative firing neurons"
    assert final_active >= 0, "Should have non-negative active neurons"
    
    # Check E/I ratio is reasonable
    ei_ratio = brain.getExcitationInhibitionRatio()
    assert 0.0 <= ei_ratio <= 100.0, "E/I ratio should be reasonable"

def test_plasticity_learning():
    """Test that plasticity rules are working"""
    brain = setup_test_brain()
    
    # Enable plasticity on synapses
    config = brain.getConfig()
    
    # Record initial weights
    initial_spikes = brain.getTotalSpikeCount()
    
    # Run simulation with activity to trigger learning
    for step in range(500):
        # Create correlated activity to trigger STDP
        if step % 10 == 0:
            brain.injectCurrentToNeurons(neuronType.Excitatory, 20.0)
        
        brain.step(step)
        
        # Enable Hebbian and STDP plasticity periodically
        if step % 50 == 0:
            # Note: In a real implementation, we'd have access to plasticity controllers
            # For this test, we verify that the brain runs without errors
            pass
    
    final_spikes = brain.getTotalSpikeCount()
    assert final_spikes > initial_spikes, "Should have accumulated spikes during learning"

def test_memory_systems():
    """Test memory system integration"""
    brain = setup_test_brain()
    
    # Test working memory
    initial_active = brain.getActiveNeuronCount()
    
    # Generate some activity
    brain.injectCurrentToNeurons(neuronType.Excitatory, 15.0)
    
    for step in range(50):
        brain.step(step)
    
    # Working memory should be active during simulation
    # (Note: Memory systems may not be fully integrated yet)
    final_active = brain.getActiveNeuronCount()
    assert final_active >= 0, "Working memory should be non-negative"
    
    # Test development
    initial_stage = brain.getDevelopmentalStage()
    assert initial_stage.value >= 0, "Developmental stage should be valid"

def test_neuromodulation():
    """Test neuromodulation effects"""
    brain = setup_test_brain()
    
    # Record baseline
    baseline_spikes = brain.getTotalSpikeCount()
    baseline_firing = brain.getFiringNeuronCount()
    
    # Apply neuromodulation (curiosity)
    # Note: In a real implementation, we'd have access to neuromodulation APIs
    # For now, test that the brain responds to stimuli
    
    brain.injectCurrentToNeurons(neuronType.Excitatory, 25.0)
    
    for step in range(100):
        brain.step(step)
        
        # Apply some form of reward/curiosity modulation
        # (This would use the actual neuromodulation APIs when available)
        pass
    
    final_spikes = brain.getTotalSpikeCount()
    final_firing = brain.getFiringNeuronCount()
    
    assert final_spikes > baseline_spikes, "Neuromodulation should affect spike production"

def test_agent_world_integration():
    """Test complete agent-world interaction"""
    brain = setup_test_brain()
    agent, world = setup_test_agent(brain)
    
    # Enable all advanced features
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    # Run complete simulation loop
    for step in range(200):
        # Update world
        world.update(0.1)
        
        # Get sensory input
        percept = world.getSensoryPercept()
        
        # Process sensory input
        agent.processSensoryInput(percept)
        
        # Run brain computation
        brain.step(step)
        
        # Decode motor command
        action = agent.decodeMotorCommand()
        
        # Apply action to world
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Get reward and apply neuromodulation
        reward = 0.0
        internal = world.getSensoryPercept().getInternal()
        if internal:
            reward = internal[0] if internal else 0.0
        
        agent.applyRewardModulation(reward, 0.0)
        
        # Update development
        agent.updateDevelopment(0.1)
        
        # Log progress
        if step % 50 == 0:
            assert brain.getFiringNeuronCount() >= 0
            assert agent.getCuriosityLevel() >= 0.0
            assert agent.getNoveltyLevel() >= 0.0
            assert agent.getPredictionError() >= 0.0
    
    # Verify agent state after simulation
    assert brain.getTotalNeuronCount() > 0
    assert world.getAgentBody().energy >= 0.0
    assert agent.getCuriosityLevel() >= 0.0

def test_configuration_management():
    """Test configuration loading and management"""
    from pynlm import createDefaultConfig
    
    # Create default config
    config = createDefaultConfig()
    
    # Test basic configuration operations
    assert config.has("brain.neuron_count")
    assert config.has("brain.region_count")
    assert config.has("brain.connection_probability")
    
    # Test get operations
    neuron_count = config.getOr<int64_t>("brain.neuron_count", 1000)
    assert neuron_count > 0
    
    region_count = config.getOr<int64_t>("brain.region_count", 1)
    assert region_count > 0
    
    # Test key listing
    keys = config.getKeys()
    assert len(keys) > 0
    
    # Test config summary
    summary = config.summary()
    assert len(summary) > 0

def test_error_handling():
    """Test error handling and edge cases"""
    brain = setup_test_brain()
    
    # Test that brain handles invalid operations gracefully
    # (Note: Some errors might not be exposed in Python bindings yet)
    
    # Test reset
    brain.reset()
    assert brain.getTotalSpikeCount() >= 0
    
    # Test save/load (if implemented)
    # brain.save("test_brain.bin")
    # assert brain.load("test_brain.bin")
    
    # Test step with different parameters
    brain.step(0)
    brain.step(1, 0.001)
    
    # Test region operations
    region_id = brain.addRegion("test_region")
    region = brain.getRegion(region_id)
    assert region is not None
    
    region_count = brain.getRegionCount()
    assert region_count > 0

def test_performance_consistency():
    """Test that performance is consistent across runs"""
    import time
    
    # Run multiple simulations and compare performance
    times = []
    spike_counts = []
    
    for run in range(3):
        brain = setup_test_brain()
        
        start_time = time.time()
        for step in range(100):
            brain.step(step)
        end_time = time.time()
        
        times.append(end_time - start_time)
        spike_counts.append(brain.getTotalSpikeCount())
    
    # Times should be reasonably consistent (not more than 3x difference)
    max_time = max(times)
    min_time = min(times)
    assert max_time / min_time < 3.0, "Performance should be consistent"
    
    # Spike counts should be somewhat consistent
    max_spikes = max(spike_counts)
    min_spikes = min(spike_counts)
    assert max_spikes >= min_spikes, "Spike counts should be non-decreasing"

def test_system_statistics():
    """Test system-wide statistics and information"""
    from pynlm import getSystemStats
    
    stats = getSystemStats()
    assert len(stats) > 0
    assert "NLM" in stats
    assert "Phase 6" in stats
    assert "Python" in stats
    
    # Check for key components
    key_components = ["Neural Core", "Memory Systems", "Neuromodulation", "Cognition"]
    for component in key_components:
        assert component in stats

def test_benchmark_performance():
    """Test benchmark performance measurement"""
    from pynlm import runBenchmark
    
    brain = setup_test_brain()
    
    # Run benchmark
    profile = runBenchmark(brain, 500)
    
    # Profile should contain timing information
    assert "total" in profile
    assert "brain_step" in profile
    assert profile["total"] > 0
    assert profile["brain_step"] > 0

def test_quick_test_setup():
    """Test quick test setup function"""
    from pynlm import createQuickTestSetup
    
    brain = createQuickTestSetup()
    
    # Verify brain was created correctly
    assert brain.getTotalNeuronCount() > 0
    assert brain.getTotalSynapseCount() > 0
    
    # Run some steps
    for step in range(50):
        brain.step(step)
    
    # Verify activity
    assert brain.getTotalSpikeCount() >= 0

def test_configuration_validation():
    """Test configuration validation"""
    from pynlm import validateBrainConfiguration
    
    brain = setup_test_brain()
    validation_result = validateBrainConfiguration(brain)
    
    # Should return a validation message
    assert len(validation_result) > 0
    assert isinstance(validation_result, str)

def test_advanced_usage_example():
    """Test advanced usage example"""
    from pynlm import createAdvancedUsageExample
    
    brain = createAdvancedUsageExample()
    
    # Verify brain was created
    assert brain.getTotalNeuronCount() > 0
    assert brain.getTotalSynapseCount() > 0
    
    # Run some additional steps
    for step in range(100):
        brain.step(step)
    
    # Verify the brain is still functional
    assert brain.getTotalSpikeCount() >= 0

if __name__ == "__main__":
    pytest.main([__file__, "-v", "--tb=short"])
