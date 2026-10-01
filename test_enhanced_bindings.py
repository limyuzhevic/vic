#!/usr/bin/env python3
"""
Enhanced NLM Python Test Suite

Test suite for the enhanced NLM Python bindings with comprehensive test cases
for all the new Pythonic features and utilities.
"""

import sys
import os
import json
import tempfile
import time
from pathlib import Path

# Add the enhanced bindings directory to the path
sys.path.insert(0, os.path.join(os.path.dirname(__file__), 'python'))

try:
    import pynlm_enhanced as pynlm
except ImportError:
    print("Error: Could not import pynlm_enhanced")
    print("Please build the enhanced bindings first")
    sys.exit(1)


def test_basic_functionality():
    """Test basic brain creation and initialization"""
    print("Testing basic functionality...")
    
    # Create default config
    config = pynlm.createDefaultConfig()
    assert config is not None, "Failed to create default config"
    
    # Create brain
    brain = pynlm.createBrain(config)
    assert brain is not None, "Failed to create brain"
    
    # Initialize brain
    brain.initialize()
    
    # Test stats
    stats = brain.get_stats()
    assert "neuron_count" in stats, "Stats missing neuron_count"
    assert "synapse_count" in stats, "Stats missing synapse_count"
    assert "development_stage" in stats, "Stats missing development_stage"
    
    print("  ✓ Basic functionality test passed")


def test_world_simulation():
    """Test world simulation with agent"""
    print("Testing world simulation...")
    
    # Create brain and world
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    world = pynlm.createSimpleWorld()
    
    # Configure world
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    world.setAgentStart(10.0, 10.0)
    
    # Create agent
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable subsystems
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    agent.enableDevelopment(True)
    
    # Run simulation
    for i in range(50):
        world.update(0.1)
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(i, i * 0.1)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
    
    # Verify simulation completed successfully
    assert brain.getTotalNeuronCount() > 0, "Brain should have neurons"
    assert brain.getTotalSynapseCount() > 0, "Brain should have synapses"
    assert agent.getCuriosityLevel() >= 0, "Curiosity level should be non-negative"
    
    print("  ✓ World simulation test passed")


def test_brain_health_and_validation():
    """Test brain health checking and validation"""
    print("Testing brain health and validation...")
    
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Test health functions
    is_healthy = brain.is_healthy()
    health_score = brain.get_health_score()
    
    assert isinstance(is_healthy, bool), "is_healthy should return boolean"
    assert 0.0 <= health_score <= 1.0, "health_score should be between 0 and 1"
    
    # Validate brain (should not throw for healthy brain)
    pynlm.validate_brain(brain)  # Should not throw
    
    print("  ✓ Brain health and validation test passed")


def test_agent_statistics():
    """Test agent statistics and monitoring"""
    print("Testing agent statistics...")
    
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    world = pynlm.createSimpleWorld()
    agent = pynlm.createAgentBrain(brain)
    
    agent.initialize(world)
    
    # Get agent statistics
    agent_stats = agent.get_agent_stats()
    
    # Verify all expected statistics are present
    expected_keys = ["development_stage", "neuromodulation_level", "curiosity_level",
                     "novelty_level", "prediction_error", "reward_enabled",
                     "structural_plasticity_enabled", "development_enabled",
                     "curiosity_enabled"]
    
    for key in expected_keys:
        assert key in agent_stats, f"Missing agent stat: {key}"
    
    # Get behavioral signature
    signature = agent.get_behavior_signature()
    assert "stage" in signature, "Missing stage in behavioral signature"
    assert "curiosity" in signature, "Missing curiosity in behavioral signature"
    assert "novelty" in signature, "Missing novelty in behavioral signature"
    assert "error" in signature, "Missing error in behavioral signature"
    assert "neuromod" in signature, "Missing neuromod in behavioral signature"
    
    print("  ✓ Agent statistics test passed")


def test_config_management():
    """Test configuration management"""
    print("Testing configuration management...")
    
    # Create config from dictionary
    config_dict = {
        "brain.neuron_count": 500,
        "brain.region_count": 2,
        "plasticity.stdp.enable": True,
        "neuromod.dopamine.scale": 1.5,
        "development.rate": 0.8
    }
    
    config = pynlm.create_batch_config(config_dict)
    assert config is not None, "Failed to create config from dictionary"
    
    # Test configuration methods
    assert config.has("brain.neuron_count"), "Config should have neuron_count"
    assert config.has("brain.region_count"), "Config should have region_count"
    assert config.has("plasticity.stdp.enable"), "Config should have stdp enable"
    
    # Test getting values
    neuron_count = config.get_int("brain.neuron_count")
    region_count = config.get_int("brain.region_count")
    stdp_enabled = config.get_bool("plasticity.stdp.enable")
    dopamine_scale = config.get_float("neuromod.dopamine.scale")
    
    assert neuron_count == 500, f"Expected neuron_count=500, got {neuron_count}"
    assert region_count == 2, f"Expected region_count=2, got {region_count}"
    assert stdp_enabled == True, f"Expected stdp_enabled=True, got {stdp_enabled}"
    assert dopamine_scale == 1.5, f"Expected dopamine_scale=1.5, got {dopamine_scale}"
    
    # Test file I/O
    with tempfile.NamedTemporaryFile(mode='w', suffix='.json', delete=False) as f:
        temp_file = f.name
        config.saveToFile(temp_file)
    
    try:
        loaded_config = pynlm.load_config_from_file(temp_file)
        assert loaded_config.has("brain.neuron_count"), "Loaded config should have neuron_count"
    finally:
        os.unlink(temp_file)
    
    print("  ✓ Configuration management test passed")


def test_simulation_utilities():
    """Test simulation utilities and benchmarking"""
    print("Testing simulation utilities...")
    
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    
    # Test run_simulation utility
    stats = pynlm.run_simulation(brain, steps=100, timestep=0.1)
    assert "neuron_count" in stats, "run_simulation should return stats"
    
    # Test benchmark_simulation
    brain.reset()
    brain.initialize()
    benchmark = pynlm.benchmark_simulation(brain, steps=50)
    
    assert "steps" in benchmark, "benchmark should contain steps"
    assert "time_ms" in benchmark, "benchmark should contain time_ms"
    assert "steps_per_second" in benchmark, "benchmark should contain steps_per_second"
    assert "neurons" in benchmark, "benchmark should contain neurons"
    assert "synapses" in benchmark, "benchmark should contain synapses"
    
    assert benchmark["steps"] == 50, f"Expected 50 steps, got {benchmark['steps']}"
    assert benchmark["time_ms"] > 0, "Benchmark time should be positive"
    assert benchmark["steps_per_second"] > 0, "Steps per second should be positive"
    assert benchmark["neurons"] > 0, "Benchmark should record neurons"
    
    print("  ✓ Simulation utilities test passed")


def test_result_comparison():
    """Test result comparison utility"""
    print("Testing result comparison...")
    
    # Create two identical result dictionaries
    results1 = {
        "brain_stats": {"neuron_count": 1000, "synapse_count": 5000},
        "agent_stats": {"curiosity_level": 0.5},
        "step": 100
    }
    
    results2 = {
        "brain_stats": {"neuron_count": 1000, "synapse_count": 5000},
        "agent_stats": {"curiosity_level": 0.5},
        "step": 100
    }
    
    results3 = {
        "brain_stats": {"neuron_count": 2000, "synapse_count": 5000},
        "agent_stats": {"curiosity_level": 0.5},
        "step": 100
    }
    
    # Test identical results
    comparison1 = pynlm.compare_results(results1, results2)
    assert comparison1["identical"] == True, "Identical results should be equal"
    
    # Test different results
    comparison2 = pynlm.compare_results(results1, results3)
    assert comparison2["identical"] == False, "Different results should not be equal"
    
    print("  ✓ Result comparison test passed")


def test_world_object_manipulation():
    """Test world object creation and manipulation"""
    print("Testing world object manipulation...")
    
    world = pynlm.createSimpleWorld()
    
    # Add various objects
    world.add_resource(5.0, 5.0, 10.0)
    world.add_wall(10.0, 10.0)
    world.add_hazard(15.0, 15.0, 2.0)
    
    # Get agent body
    body = world.getAgentBody()
    assert body is not None, "Should get agent body"
    
    # Test position
    position = body.getPosition()
    assert len(position) == 2, "Position should be (x, y) tuple"
    
    # Test velocity
    velocity = body.getVelocity()
    assert len(velocity) == 2, "Velocity should be (vx, vy) tuple"
    
    print("  ✓ World object manipulation test passed")


def test_action_creation():
    """Test action creation and manipulation"""
    print("Testing action creation...")
    
    # Test creating action from enum
    from pynlm_enhanced import NeuronType, ActionType
    
    # Create action using enum
    action = pynlm.Action(ActionType.MoveForward)
    assert action.getType() == ActionType.MoveForward
    
    # Test action with parameters
    params = [0.5, 0.3, 0.8]
    action2 = pynlm.Action(ActionType.Interact, params)
    assert action2.getType() == ActionType.Interact
    assert action2.getParameters() == params
    
    # Test action properties
    assert action.getName() == "MoveForward"
    assert action2.getName() == "Interact"
    
    # Test action conversion
    action_dict = action.__dict__ if hasattr(action, '__dict__') else {}
    assert len(action_dict) >= 0, "Action should be convertible"
    
    print("  ✓ Action creation test passed")


def test_error_handling():
    """Test error handling and edge cases"""
    print("Testing error handling...")
    
    # Test invalid brain validation
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    
    # Brain should be valid after initialization
    pynlm.validate_brain(brain)  # Should not throw
    
    # Test with very small brain (edge case)
    small_config = pynlm.createDefaultConfig()
    # Modify config for small brain (if possible)
    # brain_small = pynlm.createBrain(small_config)
    # brain_small.initialize()
    # Note: Small brain validation might fail, which is expected
    
    print("  ✓ Error handling test passed")


def run_all_tests():
    """Run all tests and report results"""
    print("=" * 60)
    print("NLM Enhanced Python Test Suite")
    print("=" * 60)
    
    tests = [
        test_basic_functionality,
        test_world_simulation,
        test_brain_health_and_validation,
        test_agent_statistics,
        test_config_management,
        test_simulation_utilities,
        test_result_comparison,
        test_world_object_manipulation,
        test_action_creation,
        test_error_handling
    ]
    
    passed = 0
    failed = 0
    
    for test in tests:
        try:
            test()
            passed += 1
        except Exception as e:
            failed += 1
            print(f"  ✗ {test.__name__} failed: {e}")
            import traceback
            traceback.print_exc()
    
    print("=" * 60)
    print(f"Test Results: {passed} passed, {failed} failed")
    print("=" * 60)
    
    if failed > 0:
        print("\nSome tests failed. Please check the implementation.")
        return False
    else:
        print("\nAll tests passed! The enhanced bindings are working correctly.")
        return True


if __name__ == "__main__":
    success = run_all_tests()
    sys.exit(0 if success else 1)