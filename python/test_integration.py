#!/usr/bin/env python3
"""
Integration test for the improved pynlm Python bindings.
This script tests all the improvements made to the Python bindings.
"""

import sys
import os
import tempfile
import json
import numpy as np
from pathlib import Path

# Add the python directory to the path so we can import pynlm
sys.path.insert(0, str(Path(__file__).parent))

import pynlm

def test_basic_functionality():
    """Test basic functionality of improved bindings."""
    print("Testing basic functionality...")
    
    # Test 1: Create a complete system using factory functions
    brain, world, agent_brain = pynlm.create_nlm_system()
    assert brain is not None
    assert world is not None
    assert agent_brain is not None
    
    # Test 2: Enhanced ID types
    neuron_id = pynlm.NeuronId(42)
    assert neuron_id.value == 42
    assert neuron_id.index() == 42
    assert str(neuron_id) == "42"
    assert repr(neuron_id) == "<NeuronId: 42>"
    
    # Test 3: Enum values
    assert pynlm.NeuronType.Excitatory == pynlm.NeuronType.Excitatory
    assert pynlm.ActionType.MoveForward == pynlm.ActionType.MoveForward
    assert pynlm.DevelopmentalStage.Initial == pynlm.DevelopmentalStage.Initial
    
    # Test 4: Config operations
    config = pynlm.Config()
    config.set("test_key", "test_value")
    config.set("test_int", 123)
    config.set("test_float", 45.67)
    config.set("test_bool", True)
    
    assert config.has("test_key")
    assert config.has("test_int")
    assert config.has("test_float")
    assert config.has("test_bool")
    
    # Test 5: Sensory input
    vision = pynlm.Vision(100, 100, 3)
    assert vision.getWidth() == 100
    assert vision.getHeight() == 100
    assert vision.getChannels() == 3
    
    test_data = list(range(100 * 100 * 3))
    vision.setData(test_data)
    assert vision.getData() == test_data
    
    # Test 6: Action operations
    action = pynlm.Action(pynlm.ActionType.MoveForward, [1.0, 2.0])
    assert action.getType() == pynlm.ActionType.MoveForward
    assert action.getParameters() == [1.0, 2.0]
    assert action.getName() == "MoveForward"
    
    cloned = action.clone()
    assert cloned.getType() == action.getType()
    assert cloned.getParameters() == action.getParameters()
    
    # Test 7: World operations
    world.configure(50, 50, 16, 16)
    world.setAgentStart(10, 20)
    assert world.getWidth() == 50
    assert world.getHeight() == 50
    
    body = world.getAgentBody()
    assert body.x == 10.0
    assert body.y == 20.0
    
    # Test 8: Simulation utilities
    success = pynlm.safe_step(brain, 0, 0.1)
    assert success is True
    
    # Test 9: Constants
    assert pynlm.INVALID_NEURON_ID.value == 0
    assert pynlm.INVALID_SYNAPSE_ID.value == 0
    assert pynlm.INVALID_REGION_ID.value == 0
    assert pynlm.INVALID_POPULATION_ID.value == 0
    
    print("✓ Basic functionality tests passed")

def test_enhanced_interfaces():
    """Test enhanced Python interfaces."""
    print("Testing enhanced interfaces...")
    
    # Test 1: Better string representations
    neuron_id = pynlm.NeuronId(123)
    assert "<NeuronId: 123>" in repr(neuron_id)
    assert "123" in str(neuron_id)
    
    # Test 2: Config with property-like access
    config = pynlm.Config()
    config.set("name", "test")
    config.set("count", 42)
    config.set("active", True)
    
    # Test 3: Action string representations
    action = pynlm.Action(pynlm.ActionType.TurnLeft, [0.5])
    assert "Action type=" in str(action)
    assert "TurnLeft" in action.getName()
    
    # Test 4: AgentBody distance method
    body = pynlm.AgentBody()
    body.x = 10.0
    body.y = 10.0
    distance = body.distanceTo(20.0, 20.0)
    expected = ((10.0)**2 + (10.0)**2)**0.5
    assert abs(distance - expected) < 0.001
    
    print("✓ Enhanced interfaces tests passed")

def test_factory_functions():
    """Test factory functions."""
    print("Testing factory functions...")
    
    # Test 1: Default config creation
    config = pynlm.createDefaultConfig()
    assert config is not None
    assert isinstance(config, pynlm.Config)
    
    # Test 2: Brain creation
    config = pynlm.Config()
    brain = pynlm.createBrain(config)
    assert brain is not None
    assert isinstance(brain, pynlm.Brain)
    assert brain.initialize()
    
    # Test 3: World creation
    world = pynlm.createSimpleWorld()
    assert world is not None
    assert isinstance(world, pynlm.SimpleWorld)
    
    # Test 4: Agent brain creation
    agent_brain = pynlm.createAgentBrain(brain)
    assert agent_brain is not None
    assert isinstance(agent_brain, pynlm.AgentBrain)
    
    # Test 5: Complete system creation
    brain, world, agent_brain = pynlm.create_nlm_system()
    assert brain is not None
    assert world is not None
    assert agent_brain is not None
    
    agent_brain.initialize(world)
    assert agent_brain.getSensoryInputSize() > 0
    assert agent_brain.getMotorOutputSize() > 0
    
    print("✓ Factory functions tests passed")

def test_system_utilities():
    """Test system utilities."""
    print("Testing system utilities...")
    
    # Create system
    brain, world, agent_brain = pynlm.create_nlm_system()
    agent_brain.initialize(world)
    
    # Test 1: Get system statistics
    stats = pynlm.get_system_stats(brain)
    assert isinstance(stats, dict)
    
    required_keys = ["neuron_count", "synapse_count", "active_neurons", 
                     "firing_neurons", "avg_firing_rate", "excitement_inhibition_ratio",
                     "total_spikes", "region_count"]
    
    for key in required_keys:
        assert key in stats
        assert isinstance(stats[key], (int, float))
    
    # Test 2: Safe step
    assert pynlm.safe_step(brain, 0, 0.0)
    assert pynlm.safe_step(brain, 1, 0.1)
    
    # Test 3: Batch processing
    percepts = [pynlm.SensoryPercept() for _ in range(3)]
    actions = pynlm.batch_process_sensory(brain, percepts, 0.01)
    assert len(actions) == 3
    for action in actions:
        assert isinstance(action, pynlm.Action)
    
    # Test 4: Simulation run
    body = pynlm.run_simulation(brain, world, 3, 0.1)
    assert isinstance(body, pynlm.AgentBody)
    assert body.x >= 0 and body.x <= 100
    assert body.y >= 0 and body.y <= 100
    
    print("✓ System utilities tests passed")

def test_error_handling():
    """Test error handling."""
    print("Testing error handling...")
    
    # Test 1: None brain handling
    try:
        pynlm.get_system_stats(None)
        assert False, "Should have raised an exception"
    except pynlm.NLMException:
        pass  # Expected
    
    # Test 2: Invalid operations
    config = pynlm.Config()
    # These should not raise exceptions
    config.has("nonexistent")
    config.getKeys()
    
    # Test 3: Invalid data handling (should be handled gracefully)
    vision = pynlm.Vision(10, 10, 3)
    # Valid data should work
    valid_data = list(range(10 * 10 * 3))
    vision.setData(valid_data)
    
    print("✓ Error handling tests passed")

def test_advanced_features():
    """Test advanced features."""
    print("Testing advanced features...")
    
    # Test 1: Neuromodulation system
    brain, world, agent_brain = pynlm.create_nlm_system()
    agent_brain.initialize(world)
    
    # Enable all systems
    agent_brain.enableRewardModulation(True)
    agent_brain.enableStructuralPlasticity(True)
    agent_brain.enableDevelopment(True)
    agent_brain.enableCuriosity(True)
    
    # Check all systems are enabled
    assert agent_brain.isRewardModulationEnabled()
    assert agent_brain.isStructuralPlasticityEnabled()
    assert agent_brain.isDevelopmentEnabled()
    assert agent_brain.isCuriosityEnabled()
    
    # Test 2: Development stages
    stage = agent_brain.getDevelopmentalStage()
    assert stage in [
        pynlm.DevelopmentalStage.Initial,
        pynlm.DevelopmentalStage.CriticalPeriod,
        pynlm.DevelopmentalStage.Maturation,
        pynlm.DevelopmentalStage.Adult,
        pynlm.DevelopmentalStage.Aging
    ]
    
    # Test 3: Configuration persistence
    config = pynlm.createDefaultConfig()
    config.set("persistent_key", "persistent_value")
    
    # Save and load
    with tempfile.NamedTemporaryFile(mode='w', suffix='.json', delete=False) as f:
        config.saveToFile(f.name)
        temp_file = f.name
    
    try:
        config2 = pynlm.Config()
        config2.loadFromFile(temp_file)
        
        # Verify data
        assert config2.has("persistent_key")
        # Note: Config doesn't have a get() method that returns the value
        # So we can't easily test this without more API access
        
    finally:
        os.unlink(temp_file)
    
    print("✓ Advanced features tests passed")

def test_performance():
    """Test performance-related features."""
    print("Testing performance features...")
    
    # Test 1: Large configuration
    config = pynlm.Config()
    for i in range(100):
        config.set(f"key_{i}", i * 10)
    
    keys = config.getKeys()
    assert len(keys) >= 100  # Should include our keys
    
    # Test 2: Batch operations
    brain, world, agent_brain = pynlm.create_nlm_system()
    agent_brain.initialize(world)
    
    percepts = [pynlm.SensoryPercept() for _ in range(10)]
    actions = pynlm.batch_process_sensory(brain, percepts, 0.001)
    assert len(actions) == 10
    
    # Test 3: Memory efficiency
    original = pynlm.Action(pynlm.ActionType.Eat, [1.0, 2.0, 3.0])
    
    clones = []
    for i in range(50):
        clones.append(original.clone())
    
    # Verify all clones are correct
    for clone in clones:
        assert clone.getType() == original.getType()
        assert clone.getParameters() == original.getParameters()
    
    # Clean up (in Python, this happens automatically)
    del clones
    
    print("✓ Performance tests passed")

def run_all_tests():
    """Run all integration tests."""
    print("=" * 70)
    print("Integration Tests for Improved pynlm Python Bindings")
    print("=" * 70)
    print()
    
    try:
        test_basic_functionality()
        test_enhanced_interfaces()
        test_factory_functions()
        test_system_utilities()
        test_error_handling()
        test_advanced_features()
        test_performance()
        
        print()
        print("=" * 70)
        print("✅ ALL INTEGRATION TESTS PASSED!")
        print("=" * 70)
        print()
        print("The improved pynlm bindings provide:")
        print("• Modular, maintainable code structure")
        print("• Enhanced factory functions for easy system creation")
        print("• Improved ID types with better string representations")
        print("• Comprehensive sensory input handling")
        print("• Action cloning and manipulation")
        print("• World and brain integration")
        print("• Neuromodulation and development support")
        print("• Simulation utilities and error handling")
        print("• Configuration file operations")
        print("• Performance optimizations")
        print()
        return True
        
    except Exception as e:
        print()
        print("=" * 70)
        print("❌ INTEGRATION TESTS FAILED!")
        print("=" * 70)
        print()
        print(f"Error: {e}")
        import traceback
        traceback.print_exc()
        return False

if __name__ == "__main__":
    success = run_all_tests()
    sys.exit(0 if success else 1)