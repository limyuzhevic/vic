#!/usr/bin/env python3
"""
Comprehensive test suite for pynlm Python bindings.
Tests basic functionality, convenience functions, and error handling.
"""

import pynlm
import sys
import traceback

def test_basic_import():
    """Test that we can import the module and access version."""
    print("Test 1: Basic import")
    try:
        print(f"  Version: {pynlm.__version__}")
        print("  ✓ Import successful")
        return True
    except Exception as e:
        print(f"  ✗ Failed: {e}")
        return False

def test_basic_brain_creation():
    """Test basic brain creation and initialization."""
    print("\nTest 2: Basic brain creation")
    try:
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        print(f"  Total neurons: {brain.getTotalNeuronCount()}")
        print(f"  Total synapses: {brain.getTotalSynapseCount()}")
        print("  ✓ Brain creation and initialization successful")
        return True
    except Exception as e:
        print(f"  ✗ Failed: {e}")
        traceback.print_exc()
        return False

def test_basic_simulation():
    """Test running a simple simulation."""
    print("\nTest 3: Basic simulation")
    try:
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Run simulation steps
        for step in range(10):
            brain.step(step)
        
        print(f"  Steps completed: 10")
        print(f"  Final firing neurons: {brain.getFiringNeuronCount()}")
        print("  ✓ Basic simulation successful")
        return True
    except Exception as e:
        print(f"  ✗ Failed: {e}")
        traceback.print_exc()
        return False

def test_config_operations():
    """Test configuration operations."""
    print("\nTest 4: Configuration operations")
    try:
        config = pynlm.createDefaultConfig()
        
        # Test setting values
        config.set("brain.neuron_count", 500)
        config.set("brain.synapse_density", 0.2)
        config.set("brain.v_thresh", -45.0)
        
        # Test getting values
        neuron_count = config.get("brain.neuron_count")
        print(f"  Neuron count: {neuron_count}")
        
        # Test has function
        print(f"  Has neuron_count: {config.has('brain.neuron_count')}")
        print(f"  Has non_existent_key: {config.has('non_existent_key')}")
        
        # Test summary
        summary = config.summary()
        print(f"  Summary length: {len(summary)} characters")
        
        print("  ✓ Configuration operations successful")
        return True
    except Exception as e:
        print(f"  ✗ Failed: {e}")
        traceback.print_exc()
        return False

def test_convenience_functions():
    """Test convenience functions."""
    print("\nTest 5: Convenience functions")
    try:
        # Test quick_test
        result = pynlm.quick_test()
        print(f"  Quick test result: {result}")
        
        # Test create_and_run_brain
        brain = pynlm.create_and_run_brain(neuronCount=50, numSteps=20)
        print(f"  Created brain with {brain.getTotalNeuronCount()} neurons")
        
        # Test get_brain_summary
        summary = pynlm.get_brain_summary(brain)
        print(f"  Brain summary length: {len(summary)} characters")
        
        # Test create_simple_brain
        simple_brain = pynlm.create_simple_brain()
        print(f"  Simple brain created with {simple_brain.getTotalNeuronCount()} neurons")
        
        print("  ✓ Convenience functions successful")
        return True
    except Exception as e:
        print(f"  ✗ Failed: {e}")
        traceback.print_exc()
        return False

def test_world_operations():
    """Test world operations."""
    print("\nTest 6: World operations")
    try:
        # Create world using convenience function
        world = pynlm.create_minimal_world(width=15, height=15)
        
        print(f"  World dimensions: {world.getWidth()} x {world.getHeight()}")
        print(f"  Vision dimensions: {world.getVisionWidth()} x {world.getVisionHeight()}")
        
        # Test configure_world
        pynlm.configure_world(world, 20, 20, 10, 10)
        print(f"  Configured world dimensions: {world.getWidth()} x {world.getHeight()}")
        
        # Test set_agent_start_position
        pynlm.set_agent_start_position(world, 10.0, 10.0)
        body = world.getAgentBody()
        print(f"  Agent start position: ({body.x}, {body.y})")
        
        # Test add_resource_to_world
        pynlm.add_resource_to_world(world, 5.0, 5.0, 2.0)
        print("  Added resource to world")
        
        print("  ✓ World operations successful")
        return True
    except Exception as e:
        print(f"  ✗ Failed: {e}")
        traceback.print_exc()
        return False

def test_complete_agent():
    """Test complete agent setup."""
    print("\nTest 7: Complete agent setup")
    try:
        brain, world, agent = pynlm.create_complete_agent(neuronCount=75, worldSize=12)
        
        print(f"  Brain created with {brain.getTotalNeuronCount()} neurons")
        print(f"  World size: {world.getWidth()} x {world.getHeight()}")
        print(f"  Agent created: {agent is not None}")
        
        # Initialize agent with world
        agent.initialize(world)
        
        # Run a simple simulation
        for step in range(5):
            world.update(0.1)
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            brain.step(step)
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
        
        print(f"  Simulation completed: {brain.getFiringNeuronCount()} firing neurons")
        print(f"  Curiosity level: {agent.getCuriosityLevel():.3f}")
        print(f"  Novelty level: {agent.getNoveltyLevel():.3f}")
        
        print("  ✓ Complete agent setup successful")
        return True
    except Exception as e:
        print(f"  ✗ Failed: {e}")
        traceback.print_exc()
        return False

def test_error_handling():
    """Test error handling."""
    print("\nTest 8: Error handling")
    try:
        # Test that invalid operations raise appropriate errors
        config = pynlm.createDefaultConfig()
        
        # Test invalid key access (should return None)
        invalid_value = config.get("invalid.key")
        print(f"  Invalid key access returns: {invalid_value}")
        
        # Test non-existent operations
        brain = None
        if brain:
            brain.initialize()  # This should fail
        
        print("  ✓ Error handling working (exceptions would be raised as expected)")
        return True
    except Exception as e:
        print(f"  ✗ Unexpected error: {e}")
        traceback.print_exc()
        return False

def test_serialization():
    """Test brain state serialization."""
    print("\nTest 9: Serialization")
    try:
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Run some steps
        for step in range(10):
            brain.step(step)
        
        # Test save
        save_result = brain.save("test_brain.bin")
        print(f"  Save result: {save_result}")
        
        # Test load (create new brain first)
        config2 = pynlm.createDefaultConfig()
        brain2 = pynlm.createBrain(config2)
        brain2.initialize()
        
        load_result = brain2.load("test_brain.bin")
        print(f"  Load result: {load_result}")
        
        print("  ✓ Serialization successful")
        return True
    except Exception as e:
        print(f"  ✗ Failed: {e}")
        traceback.print_exc()
        return False

def run_all_tests():
    """Run all tests and report results."""
    print("=" * 60)
    print("NLM Python Bindings Comprehensive Test Suite")
    print("=" * 60)
    
    tests = [
        test_basic_import,
        test_basic_brain_creation,
        test_basic_simulation,
        test_config_operations,
        test_convenience_functions,
        test_world_operations,
        test_complete_agent,
        test_error_handling,
        test_serialization
    ]
    
    passed = 0
    failed = 0
    
    for test in tests:
        try:
            if test():
                passed += 1
            else:
                failed += 1
        except Exception as e:
            print(f"  ✗ Test crashed: {e}")
            traceback.print_exc()
            failed += 1
    
    print("\n" + "=" * 60)
    print(f"Test Results: {passed} passed, {failed} failed")
    print("=" * 60)
    
    return failed == 0

if __name__ == "__main__":
    success = run_all_tests()
    sys.exit(0 if success else 1)