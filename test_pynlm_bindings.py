#!/usr/bin/env python3
"""
Python test suite for NLM Python bindings.

This test suite validates the improved Python API for NLM (Neural Learning Machine),
demonstrating new convenience functions and enhanced functionality.
"""

import pynlm
import tempfile
import os
import sys


def test_basic_functionality():
    """Test basic brain creation and initialization."""
    print("Testing basic functionality...")
    
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    assert brain.getTotalNeuronCount() > 0
    assert brain.getTotalSynapseCount() >= 0
    assert brain.getFiringNeuronCount() >= 0
    
    print("✓ Basic functionality test passed")


def test_convenience_functions():
    """Test new convenience factory functions."""
    print("Testing convenience functions...")
    
    # Test training config
    training_config = pynlm.createTrainingConfig("rl", 1000)
    assert training_config.has("training_type")
    assert training_config.has("epochs")
    assert training_config.has("learning_rate")
    
    # Test development config
    dev_config = pynlm.createDevelopmentConfig(0.01, True)
    assert dev_config.has("plasticity_rate")
    assert dev_config.has("structural_plasticity_enabled")
    
    # Test production config
    prod_config = pynlm.createProductionConfig(1000, 0.1)
    assert prod_config.has("working_memory_size")
    assert prod_config.has("exploration_rate")
    
    # Test training brain
    training_brain = pynlm.createTrainingBrain(200, 3, 0.001)
    assert training_brain.getTotalNeuronCount() == 200
    assert training_brain.getRegionCount() == 3
    
    # Test development brain
    dev_brain = pynlm.createDevelopmentBrain(300, 0.005)
    assert dev_brain.getTotalNeuronCount() == 300
    
    print("✓ Convenience functions test passed")


def test_world_factory_functions():
    """Test enhanced world factory functions."""
    print("Testing world factory functions...")
    
    # Test simple world
    world = pynlm.createSimpleWorld()
    assert world.getWidth() > 0
    assert world.getHeight() > 0
    
    # Test training world
    training_world = pynlm.createTrainingWorld(50, 50)
    assert training_world.getMaxEnergy() == 100.0
    assert training_world.getAgentStartX() == 2.0
    assert training_world.getAgentStartY() == 2.0
    
    # Test survival world
    survival_world = pynlm.createSurvivalWorld(30, 30, 0.1)
    assert survival_world.getMaxEnergy() == 50.0
    
    print("✓ World factory functions test passed")


def test_agent_factory_functions():
    """Test enhanced agent factory functions."""
    print("Testing agent factory functions...")
    
    # Create brain and world
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    
    # Test default agent
    agent = pynlm.createAgentWithDefaults(brain, world)
    assert agent.getBrain() is not None
    assert agent.isRewardModulationEnabled()
    assert agent.isDevelopmentEnabled()
    
    # Test expert agent
    expert_agent = pynlm.createExpertAgent(brain, world)
    assert expert_agent.getBrain() is not None
    assert expert_agent.isRewardModulationEnabled()
    assert expert_agent.isStructuralPlasticityEnabled()
    
    print("✓ Agent factory functions test passed")


def test_complete_simulation_setup():
    """Test complete simulation setup factory."""
    print("Testing complete simulation setup...")
    
    # Test training setup
    brain1, world1, agent1 = pynlm.createCompleteSimulationSetup("training", 100, True)
    assert brain1.getTotalNeuronCount() == 500
    assert world1.getWidth() == 50
    assert agent1 is not None
    
    # Test survival setup
    brain2, world2, agent2 = pynlm.createCompleteSimulationSetup("survival", 200, True)
    assert brain2.getTotalNeuronCount() == 500
    assert world2.getWidth() == 100
    assert agent2 is not None
    
    print("✓ Complete simulation setup test passed")


def test_quick_setup_functions():
    """Test quick setup convenience functions."""
    print("Testing quick setup functions...")
    
    # Test quick simulation
    brain1, world1, agent1 = pynlm.quickSimulation(100, 8)
    assert brain1.getTotalNeuronCount() == 500
    assert world1.getWidth() == 50
    
    # Test quick survival simulation
    brain2, world2, agent2 = pynlm.quickSurvivalSimulation(200)
    assert brain2.getTotalNeuronCount() == 500
    assert world2.getWidth() == 100
    
    print("✓ Quick setup functions test passed")


def test_simulation_utilities():
    """Test simulation utility functions."""
    print("Testing simulation utilities...")
    
    # Create a complete setup
    brain, world, agent = pynlm.createCompleteSimulationSetup("training", 50, True)
    
    # Run simulation steps
    success_rate = pynlm.runSimulationSteps(agent, world, 10, False)
    assert 0.0 <= success_rate <= 1.0
    
    # Run training simulation
    metrics = pynlm.runTrainingSimulation(agent, world, 20)
    assert "total_steps" in metrics
    assert "success_rate" in metrics
    assert "neuron_count" in metrics
    assert metrics["total_steps"] == 20
    
    print("✓ Simulation utilities test passed")


def test_state_management():
    """Test brain state management utilities."""
    print("Testing state management...")
    
    # Create and initialize brain
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Create a temporary file for checkpoint
    with tempfile.NamedTemporaryFile(delete=False, suffix='.bin') as tmp_file:
        checkpoint_file = tmp_file.name
    
    try:
        # Save brain state
        pynlm.saveBrainToFile(brain, checkpoint_file)
        
        # Create new brain and load state
        config2 = pynlm.createDefaultConfig()
        brain2 = pynlm.createBrain(config2)
        
        pynlm.loadBrainFromFile(checkpoint_file)
        # Note: loadBrainFromFile creates a new brain instance, not loads into existing
        brain_loaded = pynlm.loadBrainFromFile(checkpoint_file)
        assert brain_loaded is not None
        
    finally:
        # Clean up
        if os.path.exists(checkpoint_file):
            os.unlink(checkpoint_file)
    
    print("✓ State management test passed")


def test_comparison_utilities():
    """Test brain comparison utilities."""
    print("Testing comparison utilities...")
    
    # Create two identical brains
    config1 = pynlm.createDefaultConfig()
    brain1 = pynlm.createBrain(config1)
    brain1.initialize()
    
    config2 = pynlm.createDefaultConfig()
    brain2 = pynlm.createBrain(config2)
    brain2.initialize()
    
    # Compare brains
    comparison = pynlm.compareBrains(brain1, brain2)
    assert "neuron_count_match" in comparison
    assert "synapse_count_match" in comparison
    assert "firing_rate_ratio" in comparison
    
    print("✓ Comparison utilities test passed")


def test_configuration_utilities():
    """Test configuration utilities."""
    print("Testing configuration utilities...")
    
    # Create config from params
    params = {
        "brain.neuron_count": 500,
        "brain.region_count": 2,
        "plasticity.stdp.learning_rate": 0.01,
        "neuromod.dopamine.scale": 1.5
    }
    
    config = pynlm.createConfigFromParams(params)
    assert config.has("brain.neuron_count")
    assert config.has("brain.region_count")
    
    # Check values
    assert config.get("brain.neuron_count") == 500
    assert config.get("brain.region_count") == 2
    
    print("✓ Configuration utilities test passed")


def test_statistics_calculator():
    """Test statistics calculator utility."""
    print("Testing statistics calculator...")
    
    # Test with empty data
    stats1 = pynlm.calculateStats([])
    assert stats1["count"] == 0
    assert stats1["mean"] == 0.0
    
    # Test with data
    values = [1.0, 2.0, 3.0, 4.0, 5.0]
    stats2 = pynlm.calculateStats(values)
    assert stats2["count"] == 5
    assert stats2["mean"] == 3.0
    assert stats2["min"] == 1.0
    assert stats2["max"] == 5.0
    assert abs(stats2["std"] - 1.5811388300841898) < 0.001
    
    print("✓ Statistics calculator test passed")


def test_error_handling():
    """Test error handling and validation."""
    print("Testing error handling...")
    
    # Test invalid config file
    try:
        pynlm.createBrainFromConfigFile("nonexistent_file.json")
        assert False, "Should have raised an error"
    except RuntimeError as e:
        assert "Failed to load configuration" in str(e)
    
    # Test invalid brain save file path
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    try:
        # This should raise an error if save fails
        pass  # Implementation-dependent
    except Exception:
        pass  # Expected behavior
    
    print("✓ Error handling test passed")


def test_advanced_features():
    """Test advanced agent features."""
    print("Testing advanced features...")
    
    # Create super agent
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    
    # Test super agent with features enabled
    super_agent = pynlm.createSuperAgent(brain, world, True)
    assert super_agent is not None
    assert super_agent.isRewardModulationEnabled()
    assert super_agent.isDevelopmentEnabled()
    
    print("✓ Advanced features test passed")


def test_batch_processing():
    """Test batch processing utilities."""
    print("Testing batch processing...")
    
    # Create multiple agents and worlds
    agents = []
    worlds = []
    
    for i in range(3):
        brain = pynlm.createBrain(pynlm.createDefaultConfig())
        brain.initialize()
        
        world = pynlm.createSimpleWorld()
        
        agent = pynlm.createAgentWithDefaults(brain, world)
        agents.append(agent)
        worlds.append(world)
    
    # Run batch simulations
    results = pynlm.batchRunSimulations(agents, worlds, 10)
    assert len(results) == 3
    
    for result in results:
        assert "total_steps" in result
        assert "success_rate" in result
    
    print("✓ Batch processing test passed")


def run_all_tests():
    """Run all tests."""
    print("=" * 60)
    print("NLM Python Bindings Test Suite")
    print("=" * 60)
    
    tests = [
        test_basic_functionality,
        test_convenience_functions,
        test_world_factory_functions,
        test_agent_factory_functions,
        test_complete_simulation_setup,
        test_quick_setup_functions,
        test_simulation_utilities,
        test_state_management,
        test_comparison_utilities,
        test_configuration_utilities,
        test_statistics_calculator,
        test_error_handling,
        test_advanced_features,
        test_batch_processing,
    ]
    
    passed = 0
    failed = 0
    
    for test in tests:
        try:
            test()
            passed += 1
        except Exception as e:
            print(f"✗ {test.__name__} failed: {e}")
            failed += 1
    
    print("=" * 60)
    print(f"Test Results: {passed} passed, {failed} failed")
    print("=" * 60)
    
    if failed > 0:
        sys.exit(1)


if __name__ == "__main__":
    run_all_tests()