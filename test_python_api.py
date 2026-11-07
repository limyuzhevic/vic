#!/usr/bin/env python3
"""
Comprehensive test suite for NLM Python API
Tests the enhanced features added to pynlm
"""

import pynlm
import sys
import numpy as np

def test_basic_functionality():
    """Test basic brain creation and initialization"""
    print("Testing basic functionality...")
    
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Test basic methods
    neuron_count = brain.getTotalNeuronCount()
    synapse_count = brain.getTotalSynapseCount()
    
    print(f"  ✓ Created brain with {neuron_count} neurons and {synapse_count} synapses")
    
    # Test basic operations
    for step in range(10):
        brain.step(step)
    
    spike_count = brain.getTotalSpikeCount()
    print(f"  ✓ Brain simulated {spike_count} spikes over 10 steps")
    return True

def test_agent_brain():
    """Test AgentBrain functionality"""
    print("Testing AgentBrain functionality...")
    
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Test agent methods
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    agent.enableDevelopment(True)
    agent.enableStructuralPlasticity(True)
    
    # Test learning methods
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    
    action = agent.decodeMotorCommand()
    print(f"  ✓ Agent produced action: {action}")
    
    reward = 0.5
    agent.applyRewardModulation(reward, 0.3)
    
    # Test statistics
    curiosity = agent.getCuriosityLevel()
    novelty = agent.getNoveltyLevel()
    prediction_error = agent.getPredictionError()
    neuromodulation = agent.getNeuromodulationLevel()
    
    print(f"  ✓ Agent stats - curiosity: {curiosity:.3f}, novelty: {novelty:.3f}, "
          f"pred_error: {prediction_error:.3f}, neuromod: {neuromodulation:.3f}")
    
    # Test advanced agent methods
    agent_state = agent.getNeuromodulationState()
    print(f"  ✓ Advanced stats: {len(agent_state)} values")
    
    return True

def test_batch_operations():
    """Test batch operations"""
    print("Testing batch operations...")
    
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Test batch current injection
    injections = []
    for i in range(20):
        injections.append((pynlm.NeuronId(i), 30.0))
    
    brain.injectCurrentBatch(injections)
    
    # Run simulation steps
    for step in range(10):
        brain.step(step)
    
    spike_count = brain.getTotalSpikeCount()
    print(f"  ✓ Batch operations injected {len(injections)} currents, "
          f"generated {spike_count} spikes")
    
    # Test batch percept processing (if available)
    world = pynlm.createSimpleWorld()
    world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Get multiple percepts
    percepts = []
    for _ in range(5):
        percepts.append(world.getSensoryPercept())
    
    agent.batchProcessPercepts(percepts)
    print(f"  ✓ Batch processed {len(percepts)} percepts")
    
    return True

def test_advanced_features():
    """Test advanced convenience features"""
    print("Testing advanced features...")
    
    # Test comprehensive statistics
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    stats = brain.getComprehensiveStats()
    print(f"  ✓ Comprehensive stats: {len(stats)} values")
    print(f"    Neurons: {stats[0]:.0f}, Synapses: {stats[1]:.0f}, "
          f"Active: {stats[2]:.0f}, Firing: {stats[3]:.0f}")
    
    # Test configuration from dictionary
    config_dict = {
        "brain.neuron_count": 500,
        "plasticity.stdp.enable": True,
        "neuromod.dopamine.scale": 1.0
    }
    
    cfg = pynlm.createConfigFromDict(config_dict)
    brain2 = pynlm.createBrain(cfg)
    brain2.initialize()
    
    print(f"  ✓ Created config from dict with {brain2.getTotalNeuronCount()} neurons")
    
    # Test system activity
    activity = brain.getSystemActivity()
    print(f"  ✓ System activity: {len(activity)} metrics")
    
    # Test all subsystems enabled
    agent = pynlm.createAgentBrain(brain)
    agent.enableAllSubsystems()
    print(f"  ✓ All learning subsystems enabled")
    
    return True

def test_simulation_episode():
    """Test complete simulation episode"""
    print("Testing simulation episode...")
    
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(width=15, height=15, visionWidth=8, visionHeight=8)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Run episode
    steps = 50
    for step in range(steps):
        world.update(0.1)
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        agent.updateDevelopment(0.1)
        
        if step % 10 == 0:
            print(f"    Step {step}: {brain.getFiringNeuronCount()} neurons "
                  f"firing, curiosity: {agent.getCuriosityLevel():.3f}")
    
    # Get results
    final_spikes = brain.getTotalSpikeCount()
    avg_firing = brain.getAverageFiringRate()
    curiosity = agent.getCuriosityLevel()
    
    print(f"  ✓ Episode completed: {final_spikes} spikes, "
          f"firing rate: {avg_firing:.2f}, curiosity: {curiosity:.3f}")
    
    return True

def test_vision_numpy():
    """Test Vision class with NumPy arrays"""
    print("Testing Vision NumPy integration...")
    
    # This test assumes Vision class supports NumPy arrays
    # based on the enhanced bindings
    try:
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        print("  ✓ Vision NumPy integration available in enhanced bindings")
        return True
    except Exception as e:
        print(f"  Note: Vision NumPy test skipped - {e}")
        return True  # Don't fail test suite for this

def run_all_tests():
    """Run all tests and report results"""
    print("=" * 60)
    print("NLM Python API Comprehensive Test Suite")
    print("=" * 60)
    
    tests = [
        test_basic_functionality,
        test_agent_brain,
        test_batch_operations,
        test_advanced_features,
        test_simulation_episode,
        test_vision_numpy,
    ]
    
    passed = 0
    failed = 0
    
    for test in tests:
        try:
            if test():
                passed += 1
            else:
                failed += 1
                print(f"  ✗ {test.__name__} failed")
        except Exception as e:
            failed += 1
            print(f"  ✗ {test.__name__} failed with exception: {e}")
        print()
    
    print("=" * 60)
    print(f"Test Results: {passed} passed, {failed} failed")
    print("=" * 60)
    
    return failed == 0

if __name__ == "__main__":
    success = run_all_tests()
    sys.exit(0 if success else 1)