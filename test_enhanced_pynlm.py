#!/usr/bin/env python3
"""
Comprehensive Python test suite for NLM (Neural Learning Machine) Python bindings.

This test suite covers all major features of the enhanced Python API.
"""

import pynlm
import tempfile
import os
import sys

def test_basic_brain_creation():
    """Test basic brain creation and initialization."""
    print("Testing basic brain creation...")
    
    # Create brain with default configuration
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    
    # Check initial state
    assert brain.getTotalNeuronCount() > 0, "Brain should have neurons"
    assert brain.getRegionCount() >= 1, "Brain should have at least one region"
    assert brain.getTotalSynapseCount() > 0, "Brain should have synapses"
    
    brain.initialize()
    assert brain.getActiveNeuronCount() >= 0, "Should have some active neurons"
    
    print("✓ Basic brain creation test passed")

def test_pythonic_interface():
    """Test Pythonic interface features."""
    print("Testing Pythonic interface...")
    
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Test using brain as context manager-like pattern
    initial_spikes = brain.getTotalSpikeCount()
    
    # Test batch stepping
    for step in range(10):
        brain.step(step)
    
    assert brain.getTotalSpikeCount() >= initial_spikes, "Should have spikes after stepping"
    
    # Test statistics
    firing_rate = brain.getAverageFiringRate()
    assert 0.0 <= firing_rate <= 100.0, "Firing rate should be reasonable"
    
    # Test developmental stages
    stages = [pynlm.DevelopmentalStage.Initial, 
              pynlm.DevelopmentalStage.CriticalPeriod,
              pynlm.DevelopmentalStage.Maturation,
              pynlm.DevelopmentalStage.Adult,
              pynlm.DevelopmentalStage.Aging]
    
    for stage in stages:
        brain.setDevelopmentalStage(stage)
        assert brain.getDevelopmentalStage() == stage, f"Stage should be {stage}"
    
    print("✓ Pythonic interface test passed")

def test_config_api():
    """Test enhanced Config API."""
    print("Testing enhanced Config API...")
    
    config = pynlm.createDefaultConfig()
    
    # Test type-specific setters
    config.setInt("neuron_count", 500)
    config.setDouble("connection_probability", 0.05)
    config.setBool("plasticity_enable", True)
    config.set("test_string", "hello world")
    config.set("test_vector", [1, 2, 3, 4, 5])
    config.set("test_double_vector", [1.5, 2.5, 3.5])
    
    # Test getters
    assert config.has("neuron_count") == True
    assert config.has("nonexistent_key") == False
    
    neuron_count = config.get("neuron_count")
    assert neuron_count is not None
    assert neuron_count == 500
    
    # Test getOr with default
    missing_value = config.get("missing_key")
    assert missing_value is None
    
    string_value = config.getOr("test_string", "default")
    assert string_value == "hello world"
    
    int_value = config.getOr("missing_int", 42)
    assert int_value == 42
    
    # Test configuration validation
    result = pynlm.validateConfiguration(config)
    assert result[0] == True, "Valid configuration should pass validation"
    
    # Test file operations
    with tempfile.NamedTemporaryFile(mode='w', suffix='.cfg', delete=False) as f:
        config_file = f.name
    
    try:
        # Save configuration
        success = config.saveToFile(config_file)
        assert success == True, "Should save configuration successfully"
        
        # Load configuration
        new_config = pynlm.createDefaultConfig()
        success = new_config.loadFromFile(config_file)
        assert success == True, "Should load configuration successfully"
        
        # Compare basic properties
        assert new_config.get("neuron_count") == config.get("neuron_count")
        
    finally:
        os.unlink(config_file)
    
    print("✓ Enhanced Config API test passed")

def test_world_agent_api():
    """Test world and agent brain integration."""
    print("Testing world and agent brain API...")
    
    # Create brain and world
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    world.setAgentStart(10.0, 10.0)
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Test enabling subsystems
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    agent.enableDevelopment(True)
    agent.enableStructuralPlasticity(True)
    
    assert agent.isRewardModulationEnabled() == True
    assert agent.isCuriosityEnabled() == True
    assert agent.isDevelopmentEnabled() == True
    assert agent.isStructuralPlasticityEnabled() == True
    
    # Test episode running
    curiosity_level = agent.runEpisode(world, 100)
    assert 0.0 <= curiosity_level <= 1.0, "Curiosity level should be between 0 and 1"
    
    # Test sensory processing
    percept = world.getSensoryPercept()
    assert percept is not None, "Should have sensory percept"
    
    # Test motor commands
    action = agent.decodeMotorCommand()
    assert action is not None, "Should decode motor command"
    assert action.getType() in pynlm.MotorCommand.__members__.values(), "Should have valid motor command type"
    
    # Test neuromodulation
    agent.applyRewardModulation(1.0, 0.5)
    neuromod_level = agent.getNeuromodulationLevel()
    assert 0.0 <= neuromod_level <= 1.0
    
    print("✓ World and agent brain API test passed")

def test_benchmarking_api():
    """Test benchmarking and performance utilities."""
    print("Testing benchmarking API...")
    
    # Test brain creation benchmark
    neuron_count, synapse_count, duration = pynlm.benchmarkBrainCreation(100, 2)
    assert neuron_count == 100
    assert synapse_count > 0
    assert duration >= 0.0
    
    # Test network creation
    brain = pynlm.createRandomNetwork(200, 0.1)
    assert brain.getTotalNeuronCount() == 200
    assert brain.getRegionCount() == 1
    
    print("✓ Benchmarking API test passed")

def test_constants_and_metadata():
    """Test module constants and metadata."""
    print("Testing constants and metadata...")
    
    # Test constants
    assert hasattr(pynlm, 'DEFAULT_TIMESTEP')
    assert hasattr(pynlm, 'DEFAULT_SIMULATION_STEPS')
    assert hasattr(pynlm, 'DEFAULT_WORLD_WIDTH')
    assert hasattr(pynlm, 'DEFAULT_WORLD_HEIGHT')
    assert hasattr(pynlm, 'DEFAULT_VISION_SIZE')
    
    # Test version info
    assert hasattr(pynlm, '__version__')
    assert hasattr(pynlm, '__author__')
    assert hasattr(pynlm, '__description__')
    
    # Test enumerations
    assert hasattr(pynlm, 'NeuronType')
    assert hasattr(pynlm, 'ActionType')
    assert hasattr(pynlm, 'DevelopmentalStage')
    assert hasattr(pynlm, 'MotorCommand')
    assert hasattr(pynlm, 'WorldObjectType')
    
    # Test enum values
    assert pynlm.NeuronType.Excitatory is not None
    assert pynlm.ActionType.MoveForward is not None
    assert pynlm.DevelopmentalStage.Initial is not None
    
    print("✓ Constants and metadata test passed")

def test_error_handling():
    """Test error handling and edge cases."""
    print("Testing error handling...")
    
    config = pynlm.createDefaultConfig()
    
    # Test empty key handling
    try:
        config.has("")
        assert False, "Should raise exception for empty key"
    except (ValueError, std::invalid_argument):
        pass  # Expected
    
    try:
        config.get("")
        assert False, "Should raise exception for empty key"
    except (ValueError, std::invalid_argument):
        pass  # Expected
    
    try:
        config.set("", "value")
        assert False, "Should raise exception for empty key"
    except (ValueError, std::invalid_argument):
        pass  # Expected
    
    try:
        config.remove("")
        assert False, "Should raise exception for empty key"
    except (ValueError, std::invalid_argument):
        pass  # Expected
    
    print("✓ Error handling test passed")

def test_advanced_usage_examples():
    """Test advanced usage examples from documentation."""
    print("Testing advanced usage examples...")
    
    # Example 1: Advanced agent with learning
    def advanced_agent_simulation():
        # Create and configure brain
        config = pynlm.createDefaultConfig()
        config.set("brain.neuron_count", 500)
        config.set("brain.synapse_density", 0.2)
        
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Create agent with advanced features
        agent = pynlm.createAgentBrain(brain)
        
        world = pynlm.createSimpleWorld()
        world.configure(width=30, height=30, visionWidth=12, visionHeight=12)
        world.reset()
        world.setAgentStart(15.0, 15.0)
        agent.initialize(world)
        
        # Enable all advanced learning systems
        agent.enableRewardModulation(True)
        agent.enableCuriosity(True)
        agent.enableDevelopment(True)
        agent.enableStructuralPlasticity(True)
        
        # Run simulation with learning
        for step in range(500):
            world.update(0.1)
            
            # Get current state
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            brain.step(step)
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            
            # Apply reward modulation with learning
            reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
            predicted_reward = 0.0
            agent.applyRewardModulation(reward, predicted_reward)
            
            agent.updateDevelopment(0.1)
            
            if step % 100 == 0:
                print(f"Step {step}: Firing neurons: {brain.getFiringNeuronCount()}, "
                      f"Curiosity: {agent.getCuriosityLevel():.3f}, "
                      f"Novelty: {agent.getNoveltyLevel():.3f}, "
                      f"Dev Stage: {brain.getDevelopmentalStage()}")
        
        return {
            'final_curiosity': agent.getCuriosityLevel(),
            'final_novelty': agent.getNoveltyLevel(),
            'final_prediction_error': agent.getPredictionError(),
            'total_spikes': brain.getTotalSpikeCount(),
            'firing_rate': brain.getAverageFiringRate()
        }
    
    results = advanced_agent_simulation()
    
    # Validate results
    assert 0.0 <= results['final_curiosity'] <= 1.0
    assert 0.0 <= results['final_novelty'] <= 1.0
    assert 0.0 <= results['final_prediction_error'] <= 1.0
    assert results['total_spikes'] >= 0
    assert results['firing_rate'] >= 0.0
    
    print("✓ Advanced usage examples test passed")

def run_all_tests():
    """Run all test suites."""
    print("=" * 60)
    print("Running NLM Python Bindings Test Suite")
    print("=" * 60)
    
    tests = [
        test_basic_brain_creation,
        test_pythonic_interface,
        test_config_api,
        test_world_agent_api,
        test_benchmarking_api,
        test_constants_and_metadata,
        test_error_handling,
        test_advanced_usage_examples
    ]
    
    passed = 0
    failed = 0
    
    for test in tests:
        try:
            test()
            passed += 1
        except Exception as e:
            failed += 1
            print(f"✗ {test.__name__} failed: {str(e)}")
            import traceback
            traceback.print_exc()
    
    print("=" * 60)
    print(f"Test Results: {passed} passed, {failed} failed")
    print("=" * 60)
    
    if failed > 0:
        sys.exit(1)
    else:
        print("All tests passed! 🎉")

if __name__ == "__main__":
    run_all_tests()
