#!/usr/bin/env python3
"""
NLM Python Bindings - Basic Test Framework

This script provides a basic test to verify that the NLM Python bindings
are working correctly and demonstrates the improved functionality.

Usage:
    python3 test_bindings.py
"""

import sys
import pynlm

def test_basic_import():
    """Test that we can import all required modules"""
    print("Testing basic imports...")
    
    # Test all main classes
    assert hasattr(pynlm, 'createDefaultConfig')
    assert hasattr(pynlm, 'createBrain')
    assert hasattr(pynlm, 'createAgentBrain')
    assert hasattr(pynlm, 'createSimpleWorld')
    
    # Test enumerations
    assert hasattr(pynlm, 'NeuronType')
    assert hasattr(pynlm, 'ActionType')
    assert hasattr(pynlm, 'DevelopmentalStage')
    assert hasattr(pynlm, 'WorldObjectType')
    
    print("  ✓ All basic imports successful")

def test_config_creation():
    """Test configuration creation and manipulation"""
    print("Testing configuration...")
    
    config = pynlm.createDefaultConfig()
    
    # Test basic configuration
    config.set("brain.neuron_count", 100)
    assert config.has("brain.neuron_count")
    assert config.getOr("brain.neuron_count", 0) == 100
    
    # Test string configuration
    config.set("test.string_value", "test_value")
    assert config.has("test.string_value")
    
    # Test float configuration
    config.set("test.float_value", 3.14f)
    assert config.has("test.float_value")
    
    # Test boolean configuration
    config.set("test.bool_value", True)
    assert config.has("test.bool_value")
    
    print("  ✓ Configuration tests passed")

def test_brain_creation():
    """Test brain creation and initialization"""
    print("Testing brain creation...")
    
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 50)
    
    # Create brain
    brain = pynlm.createBrain(config)
    assert brain is not None
    
    # Initialize brain
    success = brain.initialize()
    assert success == True
    
    # Check brain properties
    assert brain.getTotalNeuronCount() > 0
    assert brain.getTotalSynapseCount() >= 0
    assert brain.getActiveNeuronCount() >= 0
    
    # Test brain stepping
    for step in range(10):
        brain.step(step)
        assert brain.getFiringNeuronCount() >= 0
    
    print("  ✓ Brain creation and operation tests passed")

def test_world_creation():
    """Test world creation and operations"""
    print("Testing world creation...")
    
    world = pynlm.createSimpleWorld()
    assert world is not None
    
    # Configure world
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    
    # Reset world
    world.reset()
    
    # Set agent start
    world.setAgentStart(10.0, 10.0)
    
    # Update world
    for timestep in [0.1, 0.2, 0.3]:
        world.update(timestep)
        assert world.getSimulationTime() > 0
        
        # Get sensory percept
        percept = world.getSensoryPercept()
        assert percept is not None
        
        # Get agent body
        body = world.getAgentBody()
        assert body is not None
    
    print("  ✓ World creation and operation tests passed")

def test_agent_creation():
    """Test agent creation and operations"""
    print("Testing agent creation...")
    
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Create agent
    agent = pynlm.createAgentBrain(brain)
    assert agent is not None
    
    # Initialize agent
    world = pynlm.createSimpleWorld()
    world.configure(width=10, height=10, visionWidth=4, visionHeight=4)
    world.reset()
    
    agent.initialize(world)
    
    # Test agent operations
    action = agent.decodeMotorCommand()
    assert action is not None
    
    # Test enable/disable functions
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    # Test subsystem status
    assert agent.getCuriosityLevel() >= 0.0
    assert agent.getNoveltyLevel() >= 0.0
    assert agent.getPredictionError() >= 0.0
    
    print("  ✓ Agent creation and operation tests passed")

def test_enumerations():
    """Test enumerations and constants"""
    print("Testing enumerations...")
    
    # Test neuron types
    neuron_types = [
        pynlm.NeuronType.Excitatory,
        pynlm.NeuronType.Inhibitory,
        pynlm.NeuronType.Sensory,
        pynlm.NeuronType.Motor,
        pynlm.NeuronType.Modulatory,
        pynlm.NeuronType.Internal
    ]
    assert len(neuron_types) == 6
    
    # Test action types
    action_types = [
        pynlm.ActionType.MoveForward,
        pynlm.ActionType.TurnLeft,
        pynlm.ActionType.TurnRight,
        pynlm.ActionType.Interact,
        pynlm.ActionType.Eat,
        pynlm.ActionType.Wait,
        pynlm.ActionType.Attack,
        pynlm.ActionType.Defend,
        pynlm.ActionType.Explore
    ]
    assert len(action_types) > 0
    
    # Test developmental stages
    developmental_stages = [
        pynlm.DevelopmentalStage.Initial,
        pynlm.DevelopmentalStage.CriticalPeriod,
        pynlm.DevelopmentalStage.Maturation,
        pynlm.DevelopmentalStage.Adult,
        pynlm.DevelopmentalStage.Aging
    ]
    assert len(developmental_stages) == 5
    
    # Test world object types
    world_object_types = [
        pynlm.WorldObjectType.Empty,
        pynlm.WorldObjectType.Resource,
        pynlm.WorldObjectType.Hazard,
        pynlm.WorldObjectType.Wall,
        pynlm.WorldObjectType.Marker
    ]
    assert len(world_object_types) == 5
    
    print("  ✓ Enumeration tests passed")

def test_error_handling():
    """Test error handling"""
    print("Testing error handling...")
    
    # Test with invalid parameters
    config = pynlm.createDefaultConfig()
    
    # Set invalid values (should be handled gracefully)
    config.set("brain.neuron_count", -1)  // Negative value
    assert config.has("brain.neuron_count")
    
    config.set("brain.synapse_density", 2.0f)  // > 1.0
    assert config.has("brain.synapse_density")
    
    print("  ✓ Error handling tests passed")

def run_all_tests():
    """Run all tests"""
    print("NLM Python Bindings - Test Framework")
    print("====================================")
    print()
    
    tests = [
        test_basic_import,
        test_config_creation,
        test_brain_creation,
        test_world_creation,
        test_agent_creation,
        test_enumerations,
        test_error_handling
    ]
    
    passed = 0
    failed = 0
    
    for test in tests:
        try:
            test()
            passed += 1
            print("  ✓ PASSED")
        except Exception as e:
            failed += 1
            print(f"  ✗ FAILED: {e}")
            import traceback
            traceback.print_exc()
        print()
    
    print("Test Summary:")
    print(f"  Passed: {passed}")
    print(f"  Failed: {failed}")
    print(f"  Total:  {len(tests)}")
    
    if failed == 0:
        print("\n✓ All tests passed!")
        return True
    else:
        print(f"\n✗ {failed} test(s) failed!")
        return False

if __name__ == "__main__":
    success = run_all_tests()
    sys.exit(0 if success else 1)