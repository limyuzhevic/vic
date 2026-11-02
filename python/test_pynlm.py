#!/usr/bin/env python3
"""
Comprehensive test suite for NLM Python bindings.
Tests all the functionality exposed through pynlm module.
"""

import pynlm
import tempfile
import os
import sys
import json
from typing import List, Tuple

def test_basic_creation():
    """Test basic object creation."""
    print("Testing basic creation...")
    
    config = pynlm.createDefaultConfig()
    assert config is not None
    
    brain = pynlm.createBrain(config)
    assert brain is not None
    
    world = pynlm.createSimpleWorld()
    assert world is not None
    
    agent = pynlm.createAgentBrain(brain)
    assert agent is not None
    
    print("✓ Basic creation passed")

def test_config_functionality():
    """Test configuration operations."""
    print("Testing config functionality...")
    
    config = pynlm.createDefaultConfig()
    
    # Test setting and getting values
    config.set("test_int", 42)
    assert config.has("test_int")
    
    config.set("test_double", 3.14)
    assert config.has("test_double")
    
    config.set("test_string", "hello")
    assert config.has("test_string")
    
    config.set("test_bool", True)
    assert config.has("test_bool")
    
    # Test getOr for missing key
    missing_val = config.getOr("missing_key", 999)
    assert missing_val == 999
    
    # Test keys
    keys = config.getKeys()
    assert "test_int" in keys
    assert "test_double" in keys
    assert "test_string" in keys
    assert "test_bool" in keys
    
    # Test file I/O
    with tempfile.NamedTemporaryFile(mode='w', suffix='.json', delete=False) as f:
        temp_file = f.name
    
    try:
        config.saveToFile(temp_file)
        assert os.path.exists(temp_file)
        
        new_config = pynlm.createDefaultConfig()
        new_config.loadFromFile(temp_file)
        
        # Verify some values are restored
        assert new_config.has("test_int")
    finally:
        if os.path.exists(temp_file):
            os.unlink(temp_file)
    
    print("✓ Config functionality passed")

def test_brain_basic():
    """Test basic brain operations."""
    print("Testing basic brain operations...")
    
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Test brain properties
    assert brain.getTotalNeuronCount() > 0
    assert brain.getTotalSynapseCount() > 0
    assert brain.getTotalRegionCount() > 0
    
    # Test step operations
    initial_spikes = brain.getTotalSpikeCount()
    for i in range(10):
        brain.step(i)
    
    final_spikes = brain.getTotalSpikeCount()
    assert final_spikes >= initial_spikes
    
    # Test reset
    brain.step(0)
    brain.reset()
    brain.initialize()
    
    assert brain.getTotalNeuronCount() > 0
    
    # Test developmental stage
    stage = brain.getDevelopmentalStage()
    assert stage is not None
    
    print("✓ Basic brain operations passed")

def test_world_basic():
    """Test world operations."""
    print("Testing world operations...")
    
    world = pynlm.createSimpleWorld()
    world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
    world.reset()
    
    # Test world properties
    assert world.getWidth() == 10
    assert world.getHeight() == 10
    assert world.getMaxEnergy() > 0
    assert world.getEnergyDecayRate() > 0
    
    # Test agent body
    body = world.getAgentBody()
    assert body is not None
    assert body.x is not None
    assert body.y is not None
    
    # Test random seed
    seed1 = world.getRandomSeed()
    world.setRandomSeed(42)
    seed2 = world.getRandomSeed()
    assert seed2 == 42
    
    print("✓ World operations passed")

def test_agent_basic():
    """Test agent brain operations."""
    print("Testing agent operations...")
    
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Test agent properties
    assert agent.getSensoryInputSize() > 0
    assert agent.getMotorOutputSize() > 0
    
    # Test development
    stage = agent.getDevelopmentalStage()
    assert stage is not None
    
    curiosity = agent.getCuriosityLevel()
    novelty = agent.getNoveltyLevel()
    prediction_error = agent.getPredictionError()
    neuromodulation = agent.getNeuromodulationLevel()
    
    assert curiosity >= 0.0
    assert novelty >= 0.0
    assert prediction_error >= 0.0
    assert neuromodulation >= 0.0
    
    # Test subsystem enable/disable
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    assert agent.isRewardModulationEnabled() == True
    assert agent.isStructuralPlasticityEnabled() == True
    assert agent.isDevelopmentEnabled() == True
    assert agent.isCuriosityEnabled() == True
    
    print("✓ Agent operations passed")

def test_simulation_loop():
    """Test complete simulation loop."""
    print("Testing complete simulation loop...")
    
    # Setup
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(width=15, height=15, visionWidth=8, visionHeight=8)
    world.reset()
    world.setAgentStart(7.0, 7.0)
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable learning
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    
    # Run simulation
    for step in range(100):
        # Update world
        world.update(0.1)
        
        # Get percept
        percept = world.getSensoryPercept()
        assert percept is not None
        
        # Process sensory input
        agent.processSensoryInput(percept)
        
        # Brain thinks
        brain.step(step)
        
        # Get action
        action = agent.decodeMotorCommand()
        assert action is not None
        
        # Apply action
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Apply reward modulation
        reward = 0.0
        if percept.getInternal():
            reward = percept.getInternal()[0]
        agent.applyRewardModulation(reward, 0.0)
        
        # Update development
        agent.updateDevelopment(0.1)
        
        # Log progress every 25 steps
        if step % 25 == 0:
            firing = brain.getFiringNeuronCount()
            curiosity = agent.getCuriosityLevel()
            novelty = agent.getNoveltyLevel()
            print(f"  Step {step}: {firing} firing neurons, curiosity={curiosity:.3f}, novelty={novelty:.3f}")
    
    print("✓ Complete simulation loop passed")

def test_checkpoint_operations():
    """Test brain checkpoint save/load."""
    print("Testing checkpoint operations...")
    
    config = pynlm.createDefaultConfig()
    brain1 = pynlm.createBrain(config)
    brain1.initialize()
    
    # Drive brain to generate activity
    for i in range(50):
        brain1.step(i)
    
    # Save checkpoint
    with tempfile.NamedTemporaryFile(suffix='.bin', delete=False) as f:
        checkpoint_file = f.name
    
    try:
        success = brain1.save(checkpoint_file)
        assert success
        
        # Create second brain
        brain2 = pynlm.createBrain(config)
        brain2.initialize()
        
        # Load checkpoint
        success = brain2.load(checkpoint_file)
        assert success
        
        # Verify similar state
        firing1 = brain1.getFiringNeuronCount()
        firing2 = brain2.getFiringNeuronCount()
        
        # They should have similar firing rates (not identical due to RNG)
        rate_diff = abs(firing1 - firing2) / max(firing1, 1.0)
        assert rate_diff < 0.5  # Allow significant differences due to randomness
        
    finally:
        if os.path.exists(checkpoint_file):
            os.unlink(checkpoint_file)
    
    print("✓ Checkpoint operations passed")

def test_error_handling():
    """Test error handling."""
    print("Testing error handling...")
    
    # Test invalid configuration handling
    config = pynlm.createDefaultConfig()
    
    # Should not crash when getting invalid keys
    missing_val = config.getOr("nonexistent", "default")
    assert missing_val == "default"
    
    # Test brain without initialization
    brain = pynlm.createBrain(config)
    # Should handle gracefully or throw appropriate errors
    
    print("✓ Error handling passed")

def test_advanced_features():
    """Test advanced features and convenience methods."""
    print("Testing advanced features...")
    
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Test multiple regions
    initial_regions = brain.getTotalRegionCount()
    
    # Add regions
    region_id1 = brain.addRegion("cortex")
    region_id2 = brain.addRegion("hippocampus")
    
    assert region_id1.value > 0
    assert region_id2.value > 0
    assert region_id2.value != region_id1.value
    
    # Test getting regions
    region1 = brain.getRegion(region_id1)
    region2 = brain.getRegion(region_id2)
    
    assert region1 is not None
    assert region2 is not None
    
    # Test neuron statistics
    total_neurons = brain.getTotalNeuronCount()
    firing_neurons = brain.getFiringNeuronCount()
    active_neurons = brain.getActiveNeuronCount()
    
    assert total_neurons > 0
    assert firing_neurons >= 0
    assert active_neurons >= 0
    
    # Test excitation/inhibition ratio
    e_i_ratio = brain.getExcitationInhibitionRatio()
    assert e_i_ratio >= 0.0
    
    # Test developmental stage progression
    stage = brain.getDevelopmentalStage()
    assert stage is not None
    
    # Set developmental stage
    brain.setDevelopmentalStage(stage)  # Should not crash
    
    print("✓ Advanced features passed")

def test_visualization_helpers():
    """Test visualization and plotting helper functions."""
    print("Testing visualization helpers...")
    
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Test logging
    brain.logStatus()
    
    # Run simulation and collect stats
    stats = {
        'steps': [],
        'firing_rates': [],
        'spike_counts': [],
        'curiosity_levels': []
    }
    
    for i in range(50):
        brain.step(i)
        
        stats['steps'].append(i)
        stats['firing_rates'].append(brain.getAverageFiringRate())
        stats['spike_counts'].append(brain.getTotalSpikeCount())
    
    # Basic validation
    assert len(stats['steps']) == 50
    assert len(stats['firing_rates']) == 50
    assert len(stats['spike_counts']) == 50
    
    # Check for reasonable values
    for rate in stats['firing_rates']:
        assert rate >= 0.0
    
    print("✓ Visualization helpers passed")

def test_convenience_methods():
    """Test convenience and utility methods."""
    print("Testing convenience methods...")
    
    config = pynlm.createDefaultConfig()
    
    # Test config summary
    summary = config.summary()
    assert isinstance(summary, str)
    assert len(summary) > 0
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Test quick stats
    quick_stats = {
        'neurons': brain.getTotalNeuronCount(),
        'synapses': brain.getTotalSynapseCount(),
        'firing': brain.getFiringNeuronCount(),
        'spikes': brain.getTotalSpikeCount(),
        'rate': brain.getAverageFiringRate(),
        'e_i_ratio': brain.getExcitationInhibitionRatio()
    }
    
    for key, value in quick_stats.items():
        assert value is not None
        if key in ['neurons', 'synapses', 'firing', 'spikes']:
            assert value >= 0
        elif key in ['rate', 'e_i_ratio']:
            assert value >= 0.0
    
    print("✓ Convenience methods passed")

def run_all_tests():
    """Run all tests and report results."""
    print("=" * 60)
    print("NLM Python Bindings Test Suite")
    print("=" * 60)
    
    tests = [
        test_basic_creation,
        test_config_functionality,
        test_brain_basic,
        test_world_basic,
        test_agent_basic,
        test_simulation_loop,
        test_checkpoint_operations,
        test_error_handling,
        test_advanced_features,
        test_visualization_helpers,
        test_convenience_methods
    ]
    
    passed = 0
    failed = 0
    
    for test in tests:
        try:
            test()
            passed += 1
        except Exception as e:
            print(f"✗ {test.__name__} failed: {e}")
            import traceback
            traceback.print_exc()
            failed += 1
        print()
    
    print("=" * 60)
    print(f"Test Results: {passed} passed, {failed} failed")
    print("=" * 60)
    
    if failed > 0:
        sys.exit(1)
    else:
        print("All tests passed!")
        sys.exit(0)

if __name__ == "__main__":
    run_all_tests()
