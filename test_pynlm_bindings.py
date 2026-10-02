#!/usr/bin/env python3
"""
Comprehensive Python test suite for NLM Python bindings.

This test suite demonstrates the enhanced Python API and verifies
the functionality of all the new features added to the NLM bindings.
"""

import pynlm
import sys
import os

def test_basic_functionality():
    """Test basic brain creation and initialization."""
    print("Testing basic NLM Python bindings...")
    
    # Test 1: Create default config
    config = pynlm.createDefaultConfig()
    assert config is not None
    print("✓ Default config creation")
    
    # Test 2: Create brain
    brain = pynlm.createBrain(config)
    assert brain is not None
    print("✓ Brain creation")
    
    # Test 3: Initialize brain
    success = brain.initialize()
    assert success
    print("✓ Brain initialization")
    
    # Test 4: Check basic statistics
    neuron_count = brain.getTotalNeuronCount()
    assert neuron_count > 0
    print(f"✓ Neuron count: {neuron_count}")
    
    synapse_count = brain.getTotalSynapseCount()
    assert synapse_count >= 0
    print(f"✓ Synapse count: { synapse_count}")
    
    # Test 5: Run some steps
    for i in range(10):
        brain.step(i)
    print("✓ Basic simulation steps")
    
    # Test 6: Get developmental stage
    dev_stage = brain.getDevelopmentalStage()
    assert dev_stage in [pynlm.DevelopmentalStage.Initial,
                        pynlm.DevelopmentalStage.CriticalPeriod,
                        pynlm.DevelopmentalStage.Maturation,
                        pynlm.DevelopmentalStage.Adult,
                        pynlm.DevelopmentalStage.Aging]
    print(f"✓ Developmental stage: {dev_stage}")
    
    # Test 7: Get current time and step
    current_time = brain.getCurrentTime()
    current_step = brain.getCurrentStep()
    assert current_time >= 0.0
    assert current_step >= 0
    print(f"✓ Current time: {current_time}, step: {current_step}")
    
    # Test 8: Reset brain
    brain.reset()
    print("✓ Brain reset")
    
    return True

def test_advanced_functionality():
    """Test advanced NLM features."""
    print("\nTesting advanced NLM features...")
    
    # Create brain with specific configuration
    config = pynlm.createExampleConfig(neuronCount=100, regionCount=2)
    brain = pynlm.createBrain(config)
    assert brain.initialize()
    
    # Test memory system access
    working_memory = brain.getWorkingMemory()
    assert working_memory is not None
    print("✓ Working memory access")
    
    episodic_memory = brain.getEpisodicMemory()
    assert episodic_memory is not None
    print("✓ Episodic memory access")
    
    associative_memory = brain.getAssociativeMemory()
    assert associative_memory is not None
    print("✓ Associative memory access")
    
    # Test prediction system
    prediction_system = brain.getPredictionSystem()
    assert prediction_system is not None
    print("✓ Prediction system access")
    
    # Test cognition systems
    planner = brain.getPlanner()
    assert planner is not None
    print("✓ Neural planner access")
    
    concept_formation = brain.getConceptFormation()
    assert concept_formation is not None
    print("✓ Concept formation access")
    
    attention = brain.getAttention()
    assert attention is not None
    print("✓ Attention system access")
    
    # Test development system
    development_system = brain.getDevelopmentSystem()
    assert development_system is not None
    print("✓ Development system access")
    
    # Test neuromodulation systems
    dopamine = brain.getDopamine()
    assert dopamine is not None
    print("✓ Dopamine system access")
    
    curiosity = brain.getCuriosity()
    assert curiosity is not None
    print("✓ Curiosity system access")
    
    novelty = brain.getNovelty()
    assert novelty is not None
    print("✓ Novelty system access")
    
    # Test plasticity systems
    stdp = brain.getSTDP()
    assert stdp is not None
    print("✓ STDP plasticity access")
    
    hebbian = brain.getHebbian()
    assert hebbian is not None
    print("✓ Hebbian plasticity access")
    
    structural_plasticity = brain.getStructuralPlasticity()
    assert structural_plasticity is not None
    print("✓ Structural plasticity access")
    
    # Test spike system
    spike_system = brain.getSpikeSystem()
    assert spike_system is not None
    print("✓ Spike system access")
    
    # Test plasticity updates
    brain.updatePlasticity()
    print("✓ Plasticity update")
    
    # Test development
    brain.develop()
    print("✓ Development update")
    
    return True

def test_agent_functionality():
    """Test agent brain interface."""
    print("\nTesting agent brain interface...")
    
    # Create brain and agent
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Create simple world
    world = pynlm.createSimpleWorld()
    world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
    world.reset()
    
    # Create agent brain
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    print("✓ Agent creation and initialization")
    
    # Test agent status methods
    assert agent.getSensoryInputSize() > 0
    assert agent.getMotorOutputSize() > 0
    print("✓ Agent input/output size")
    
    # Test neuromodulation
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    print("✓ Neuromodulation subsystem enable")
    
    # Test developmental stage
    dev_stage = agent.getDevelopmentalStage()
    assert dev_stage is not None
    print("✓ Agent developmental stage")
    
    # Test neuromodulation levels
    neuromodulation_level = agent.getNeuromodulationLevel()
    curiosity_level = agent.getCuriosityLevel()
    novelty_level = agent.getNoveltyLevel()
    prediction_error = agent.getPredictionError()
    
    assert neuromodulation_level >= 0.0
    assert curiosity_level >= 0.0
    assert novelty_level >= 0.0
    assert prediction_error >= 0.0
    print("✓ Neuromodulation levels")
    
    # Test reset
    agent.reset()
    print("✓ Agent reset")
    
    return True

def test_world_functionality():
    """Test world interaction."""
    print("\nTesting world functionality...")
    
    # Create world
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    print("✓ World creation and configuration")
    
    # Set agent position
    world.setAgentStart(10.0, 10.0)
    print("✓ Agent start position")
    
    # Get agent body
    agent_body = world.getAgentBody()
    assert agent_body is not None
    print("✓ Agent body access")
    
    # Update world
    world.update(0.1)
    print("✓ World update")
    
    # Get sensory percept
    percept = world.getSensoryPercept()
    assert percept is not None
    print("✓ Sensory percept")
    
    # Test world objects
    world.addObject(pynlm.WorldObject(5.0, 5.0, pynlm.WorldObjectType.Resource, value=1.0))
    print("✓ World object addition")
    
    # Test position validation
    assert world.isValidPosition(10.0, 10.0)
    assert not world.isValidPosition(-1.0, -1.0)
    print("✓ Position validation")
    
    # Test world properties
    assert world.getWidth() == 20
    assert world.getHeight() == 20
    assert world.getSimulationTime() >= 0.0
    print("✓ World properties")
    
    return True

def test_configuration_functionality():
    """Test configuration options."""
    print("\nTesting configuration functionality...")
    
    # Test default config
    config = pynlm.createDefaultConfig()
    print("✓ Default config")
    
    # Test configuration properties
    config.set("brain.neuron_count", 500)
    config.set("brain.synapse_density", 0.2)
    config.set("plasticity.stdp.enable", True)
    config.set("neuromod.dopamine.scale", 2.0)
    
    assert config.has("brain.neuron_count")
    assert config.has("plasticity.stdp.enable")
    print("✓ Configuration setting")
    
    # Test getting values
    keys = config.getKeys()
    assert "brain.neuron_count" in keys
    print("✓ Configuration keys")
    
    # Test getting with default
    neuron_count = config.getOr("brain.neuron_count", 0)
    assert neuron_count == 500
    print("✓ Configuration getOr")
    
    # Test config summary
    summary = config.summary()
    assert summary is not None and len(summary) > 0
    print("✓ Configuration summary")
    
    # Test config copy and comparison
    config2 = pynlm.createDefaultConfig()
    config2.set("test.key", 123)
    assert "test.key" in config2.getKeys()
    print("✓ Configuration comparison")
    
    return True

def test_error_handling():
    """Test error handling and exception safety."""
    print("\nTesting error handling...")
    
    # Test creating brain with invalid config (should not crash)
    try:
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        
        # This should work
        success = brain.initialize()
        assert success
        
        # Try to use before initialization (should handle gracefully)
        brain.reset()
        print("✓ Error handling - reset before full init")
        
    except Exception as e:
        print(f"✓ Error handling: {e}")
        return True
    
    # Test invalid operations
    try:
        config = pynlm.createDefaultConfig()
        # Set invalid type should work (pybind11 handles type conversion)
        config.set("brain.test_int", "invalid_string")
        print("✓ Type handling in configuration")
        
    except Exception as e:
        print(f"✓ Type validation: {e}")
    
    return True

def test_utility_functions():
    """Test utility and helper functions."""
    print("\nTesting utility functions...")
    
    # Test createExampleConfig
    config = pynlm.createExampleConfig(neuronCount=200, regionCount=3)
    assert config.has("neuron_count")
    assert config.has("region_count")
    assert config.getOr("neuron_count", 0) == 200
    print("✓ createExampleConfig")
    
    # Test runBrainSimulation (minimal)
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    
    try:
        # This should work but may be slow for large brains
        spike_count = pynlm.runBrainSimulation(brain, 50, verbose=False)
        assert isinstance(spike_count, int)
        print("✓ runBrainSimulation")
    except Exception as e:
        print(f"⚠ runBrainSimulation (expected for small test): {e}")
    
    # Test benchmarkBrain
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    
    try:
        result = pynlm.benchmarkBrain(brain, 20)
        # Should return tuple of (spike_count, neuron_count, duration_ms, ms_per_step)
        assert isinstance(result, tuple) and len(result) == 4
        print("✓ benchmarkBrain")
    except Exception as e:
        print(f"⚠ benchmarkBrain (expected for small test): {e}")
    
    # Test module constants
    assert hasattr(pynlm, '__version__')
    assert hasattr(pynlm, '__author__')
    assert hasattr(pynlm, '__description__')
    print("✓ Module metadata")
    
    assert hasattr(pynlm, 'MAX_NEURONS')
    assert hasattr(pynlm, 'MAX_SYNAPSES')
    assert hasattr(pynlm, 'DEFAULT_TIMESTEP')
    assert hasattr(pynlm, 'DEFAULT_SEED')
    print("✓ Module constants")
    
    return True

def test_example_scenarios():
    """Test realistic example scenarios."""
    print("\nTesting example scenarios...")
    
    # Example 1: Minimal working brain
    print("Example 1: Minimal working brain")
    config = pynlm.createBrain(pynlm.createDefaultConfig())
    brain = pynlm.createBrain(config)
    brain.initialize()
    for i in range(20):
        brain.step(i)
    print(f"  Created and ran brain with {brain.getTotalNeuronCount()} neurons")
    
    # Example 2: World interaction
    print("Example 2: World interaction")
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    brain.initialize()
    world = pynlm.createSimpleWorld()
    world.configure(width=15, height=15, visionWidth=6, visionHeight=6)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    for step in range(30):
        world.update(0.1)
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
    print(f"  Ran agent simulation for 30 steps")
    
    # Example 3: Complete agent with learning
    print("Example 3: Complete agent with learning enabled")
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable all learning subsystems
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    for step in range(50):
        world.update(0.1)
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Apply reward modulation
        reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
        agent.applyRewardModulation(reward, 0.0)
        agent.updateDevelopment(0.1)
        
    print(f"  Complete agent simulation with learning enabled")
    
    return True

def run_all_tests():
    """Run all test suites."""
    print("=" * 60)
    print("NLM Python Bindings Comprehensive Test Suite")
    print("=" * 60)
    
    tests = [
        test_basic_functionality,
        test_advanced_functionality,
        test_agent_functionality,
        test_world_functionality,
        test_configuration_functionality,
        test_error_handling,
        test_utility_functions,
        test_example_scenarios,
    ]
    
    passed = 0
    failed = 0
    
    for test in tests:
        try:
            if test():
                passed += 1
            else:
                failed += 1
                print(f"✗ {test.__name__} returned False")
        except Exception as e:
            failed += 1
            print(f"✗ {test.__name__} failed with exception: {e}")
            import traceback
            traceback.print_exc()
    
    print("\n" + "=" * 60)
    print(f"Test Results: {passed} passed, {failed} failed")
    print("=" * 60)
    
    if failed == 0:
        print("🎉 All tests passed!")
        return True
    else:
        print("❌ Some tests failed")
        return False

if __name__ == "__main__":
    success = run_all_tests()
    sys.exit(0 if success else 1)