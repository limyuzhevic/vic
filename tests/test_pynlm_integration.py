"""
NLM Python Integration Tests

Comprehensive integration tests for Python-C++ interoperability.
Tests the interaction between Python bindings and C++ functionality.
"""

import pytest
import sys
import os
import tempfile

# Add the python directory to the path
sys.path.insert(0, os.path.join(os.path.dirname(__file__), '..', 'python'))

try:
    import pynlm
    PYNLM_AVAILABLE = True
except ImportError:
    PYNLM_AVAILABLE = False
def test_config_save_load_integration():
    """Test config save/load functionality with C++ backend."""
    assert PYNLM_AVAILABLE, "pynlm module should be available"
    
    # Create and modify config
    config = pynlm.createDefaultConfig()
    config.set("test_int", 42)
    config.set("test_string", "hello")
    config.set("test_double", 3.14)
    config.set("test_bool", True)
    
    # Save to file
    with tempfile.NamedTemporaryFile(mode='w', suffix='.cfg', delete=False) as f:
        config_file = f.name
    
    try:
        # Note: The actual implementation saves placeholder values
        # This tests that the interface exists and doesn't crash
        success = config.saveToFile(config_file)
        assert isinstance(success, bool)
        
        # Create new config and try to load
        config2 = pynlm.createDefaultConfig()
        load_success = config2.loadFromFile(config_file)
        assert isinstance(load_success, bool)
        
        # Verify config still has its own values
        assert config2.getOr("test_int", 0) == 42
        assert config2.getOr("test_string", "") == "hello"
        
    finally:
        # Cleanup
        if os.path.exists(config_file):
            os.unlink(config_file)

def test_brain_world_agent_interaction():
    """Test the complete Brain-World-Agent interaction pipeline."""
    assert PYNLM_AVAILABLE, "pynlm module should be available"
    
    # Setup components
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(10, 10, 8, 8)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Record initial state
    initial_neurons = brain.total_neurons
    initial_spikes = brain.total_spikes
    
    # Run simulation steps
    for step in range(10):
        # World updates time
        time_before = world.getSimulationTime()
        world.update(0.1)
        time_after = world.getSimulationTime()
        
        assert time_after == time_before + 0.1
        
        # Get sensory input from world
        percept = world.getSensoryPercept()
        assert percept is not None
        
        # Process sensory input through agent
        agent.processSensoryInput(percept)
        
        # Brain processes the step
        brain.step(step)
        
        # Verify brain statistics
        assert brain.total_neurons > 0
        assert brain.total_spikes >= initial_spikes
        assert brain.average_firing_rate >= 0
        
        # Agent makes decisions
        action = agent.decodeMotorCommand()
        # Action should be a valid MotorCommand
        assert action in pynlm.MotorCommand
        
        # Apply action to world
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Verify agent body changes
        agent_body = world.getAgentBody()
        assert isinstance(agent_body.x, (int, float))
        assert isinstance(agent_body.y, (int, float))
    
    # Verify final state
    assert brain.total_neurons == initial_neurons  # Constant
    assert brain.total_spikes > initial_spikes  # Should have increased
    assert world.getSimulationTime() == 1.0  # 10 steps * 0.1 time each

def test_pythonic_properties_access():
    """Test that Pythonic properties provide correct access to C++ data."""
    assert PYNLM_AVAILABLE, "pynlm module should be available"
    
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Test that Pythonic properties return correct types and values
    neurons = brain.total_neurons
    synapses = brain.total_synapses
    active = brain.active_neurons
    firing = brain.firing_neurons
    rate = brain.average_firing_rate
    spikes = brain.total_spikes
    stage = brain.development_stage
    
    # Verify types
    assert isinstance(neurons, int)
    assert isinstance(synapses, int)
    assert isinstance(active, int)
    assert isinstance(firing, int)
    assert isinstance(rate, (int, float))
    assert isinstance(spikes, int)
    assert stage in pynlm.DevelopmentalStage
    
    # Verify values are reasonable
    assert neurons > 0
    assert synapses >= 0
    assert active >= 0
    assert firing >= 0
    assert rate >= 0
    assert spikes >= 0
    
    # Verify that Pythonic properties match original methods
    assert neurons == brain.getTotalNeuronCount()
    assert synapses == brain.getTotalSynapseCount()
    assert active == brain.getActiveNeuronCount()
    assert firing == brain.getFiringNeuronCount()
    assert rate == brain.getAverageFiringRate()
    assert spikes == brain.getTotalSpikeCount()
    assert stage == brain.getDevelopmentalStage()

def test_exception_handling_integration():
    """Test exception handling in Python bindings."""
    assert PYNLM_AVAILABLE, "pynlm module should be available"
    
    # Test that custom exceptions exist
    assert hasattr(pynlm, 'NLMException')
    assert hasattr(pynlm, 'ConfigError')
    assert hasattr(pynlm, 'BrainError')
    assert hasattr(pynlm, 'WorldError')
    assert hasattr(pynlm, 'AgentError')
    assert hasattr(pynlm, 'ValidationError')
    assert hasattr(pynlm, 'MemoryError')
    assert hasattr(pynlm, 'SimulationError')
    
    # Test that std::runtime_error is still available
    assert hasattr(pynlm, 'RuntimeError')
    
    # Create a scenario that might trigger exceptions
    config = pynlm.createDefaultConfig()
    
    # Valid operations should not raise
    config.clear()
    assert len(config.getKeys()) == 0
    
    # Invalid key should return None, not raise
    result = config.get("nonexistent_key")
    assert result is None

def test_memory_management_integration():
    """Test memory management and object lifetimes."""
    assert PYNLM_AVAILABLE, "pynlm module should be available"
    
    # Create multiple objects to test memory management
    configs = []
    brains = []
    worlds = []
    agents = []
    
    try:
        for i in range(5):
            config = pynlm.createDefaultConfig()
            configs.append(config)
            
            brain = pynlm.createBrain(config)
            brain.initialize()
            brains.append(brain)
            
            world = pynlm.createSimpleWorld()
            world.configure(10 + i*2, 10 + i*2, 6, 6)
            world.reset()
            worlds.append(world)
            
            agent = pynlm.createAgentBrain(brain)
            agent.initialize(world)
            agents.append(agent)
        
        # Verify all objects are functional
        for i, brain in enumerate(brains):
            assert brain.total_neurons > 0
            assert brain in [a.getBrain() for a in agents]
            
            # Run a step
            brain.step(i)
            assert brain.total_spikes >= 0
        
        # Verify independence (each brain should maintain its own state)
        for i, brain in enumerate(brains):
            # Each brain should have the same neuron count
            assert brain.total_neurons == configs[0].getOr("neuron_count", 1000)
            
    finally:
        # Explicit cleanup (though Python should handle this)
        configs.clear()
        brains.clear()
        worlds.clear()
        agents.clear()

def test_performance_consistency():
    """Test that Python bindings provide consistent performance characteristics."""
    assert PYNLM_AVAILABLE, "pynlm module should be available"
    
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Test consistent behavior over multiple steps
    spike_counts = []
    active_counts = []
    
    for step in range(5):
        brain.step(step)
        
        spike_counts.append(brain.total_spikes)
        active_counts.append(brain.active_neurons)
    
    # Spike counts should be non-decreasing
    for i in range(1, len(spike_counts)):
        assert spike_counts[i] >= spike_counts[i-1]
    
    # Active neuron counts should be non-negative
    for count in active_counts:
        assert count >= 0
    
    # Test consistency of Pythonic properties
    final_neurons = brain.total_neurons
    assert final_neurons == brain.getTotalNeuronCount()
    
    final_spikes = brain.total_spikes
    assert final_spikes == brain.getTotalSpikeCount()
    
    final_rate = brain.average_firing_rate
    assert abs(final_rate - brain.getAverageFiringRate()) < 0.001  # Allow tiny floating point differences

def test_backward_compatibility():
    """Test that the improved API maintains backward compatibility."""
    assert PYNLM_AVAILABLE, "pynlm module should be available"
    
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(10, 10, 8, 8)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Test that original methods still work
    assert brain.getTotalNeuronCount() > 0
    assert brain.getTotalSynapseCount() >= 0
    assert brain.getActiveNeuronCount() >= 0
    assert brain.getFiringNeuronCount() >= 0
    assert brain.getAverageFiringRate() >= 0
    assert brain.getTotalSpikeCount() >= 0
    assert brain.getDevelopmentalStage() in pynlm.DevelopmentalStage
    
    # Test that new Pythonic properties provide equivalent access
    assert brain.total_neurons == brain.getTotalNeuronCount()
    assert brain.total_synapses == brain.getTotalSynapseCount()
    assert brain.active_neurons == brain.getActiveNeuronCount()
    assert brain.firing_neurons == brain.getFiringNeuronCount()
    assert abs(brain.average_firing_rate - brain.getAverageFiringRate()) < 0.001
    assert brain.total_spikes == brain.getTotalSpikeCount()
    assert brain.development_stage == brain.getDevelopmentalStage()
    
    # Test that original world methods still work
    assert world.getWidth() > 0
    assert world.getHeight() > 0
    assert world.getSimulationTime() >= 0
    
    # Test that original agent methods still work
    from pynlm import Vision
    vision = Vision(8, 8, 3)
    agent.processSensoryInput(vision)
    
    action = agent.decodeMotorCommand()
    assert action in pynlm.MotorCommand
    
    agent.applyRewardModulation(1.0, 0.5)
    
    assert agent.isRewardModulationEnabled()
    assert agent.getNeuromodulationLevel() >= 0

if __name__ == '__main__':
    pytest.main([__file__, '-v'])
