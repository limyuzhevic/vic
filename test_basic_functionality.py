#!/usr/bin/env python3
"""
Basic test script for NLM Python bindings.

This script demonstrates the basic functionality of the NLM Python API
and can be used as a quick verification that the bindings are working correctly.
"""

import sys
import pynlm

def test_basic_functionality():
    """Test basic NLM Python API functionality."""
    print("=== Testing Basic NLM Python API Functionality ===\n")
    
    # Test 1: Configuration creation and operations
    print("1. Testing Configuration Operations...")
    config = pynlm.createDefaultConfig()
    
    # Set configuration values
    config.set("brain.neuron_count", 1000)
    config.set("brain.synapse_density", 0.1)
    config.set("random_seed", 42)
    
    # Get configuration values
    assert config.getOr("brain.neuron_count", 1000) == 1000
    assert abs(config.getOr("brain.synapse_density", 0.1) - 0.1) < 0.001
    assert config.getOr("random_seed", 0) == 42
    
    # Test has method
    assert config.has("brain.neuron_count")
    assert not config.has("brain.nonexistent_key")
    
    print("   ✓ Configuration operations passed\n")
    
    # Test 2: Brain creation and initialization
    print("2. Testing Brain Creation and Initialization...")
    brain = pynlm.createBrain(config)
    assert brain is not None
    
    assert brain.initialize()
    
    # Check initial statistics
    assert brain.getTotalNeuronCount() > 0
    assert brain.getTotalSynapseCount() > 0
    assert brain.getTotalSpikeCount() == 0
    
    print(f"   ✓ Brain initialized with {brain.getTotalNeuronCount()} neurons")
    print("   ✓ Brain initialization passed\n")
    
    # Test 3: Simple simulation
    print("3. Testing Simple Simulation...")
    
    for step in range(10):
        brain.step(step)
    
    assert brain.getTotalSpikeCount() > 0
    assert brain.getAverageFiringRate() >= 0
    
    print(f"   ✓ Simulation completed with {brain.getTotalSpikeCount()} spikes")
    print("   ✓ Simple simulation passed\n")
    
    # Test 4: Agent brain creation
    print("4. Testing Agent Brain Creation...")
    agent = pynlm.createAgentBrain(brain)
    assert agent is not None
    
    print("   ✓ Agent brain created\n")
    
    # Test 5: Simple world
    print("5. Testing Simple World...")
    world = pynlm.createSimpleWorld(width=20, height=20)
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    world.setAgentStart(10.0, 10.0)
    
    assert world.getWidth() == 20
    assert world.getHeight() == 20
    assert world.getSimulationTime() == 0.0
    
    print("   ✓ Simple world configured and initialized\n")
    
    # Test 6: Agent with world
    print("6. Testing Agent with World...")
    agent.initialize(world)
    
    # Test sensory processing
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    
    # Test motor decoding
    action = agent.decodeMotorCommand()
    assert action is not None
    assert action.getType() is not None
    
    # Test world update
    world.update(0.1)
    world.applyMotorCommand(action, world.getSimulationTime())
    
    print("   ✓ Agent successfully processed sensory input and generated motor command\n")
    
    # Test 7: Convenience functions
    print("7. Testing Convenience Functions...")
    
    # Test createSimpleBrain
    simple_brain = pynlm.createSimpleBrain(neuron_count=100)
    assert simple_brain is not None
    assert simple_brain.initialize()
    
    # Test createSimpleAgent
    brain2, world2, agent2 = pynlm.createSimpleAgent(neuron_count=50)
    assert brain2 is not None
    assert world2 is not None
    assert agent2 is not None
    
    agent2.initialize(world2)
    
    # Test runSimulationWithStats
    stats = pynlm.runSimulationWithStats(brain2, world2, agent2, 20)
    assert "avg_firing_rate" in stats
    assert "total_spikes" in stats
    assert "max_firing_rate" in stats
    
    print("   ✓ Convenience functions working correctly\n")
    
    # Test 8: Configuration file operations
    print("8. Testing Configuration File Operations...")
    
    import tempfile
    import os
    
    # Save configuration
    with tempfile.NamedTemporaryFile(mode='w', suffix='.cfg', delete=False) as f:
        config_file = f.name
    
    try:
        assert config.saveToFile(config_file)
        
        # Load configuration
        new_config = pynlm.createDefaultConfig()
        assert new_config.loadFromFile(config_file)
        
        # Verify loaded values
        assert new_config.getOr("brain.neuron_count", 0) == 1000
        
        print("   ✓ Configuration file operations passed\n")
        
    finally:
        if os.path.exists(config_file):
            os.unlink(config_file)
    
    # Test 9: Statistics collection
    print("9. Testing Statistics Collection...")
    
    # Collect statistics
    brain.logStatus()
    
    print("   ✓ Statistics collection passed\n")
    
    # Test 10: Version information
    print("10. Testing Version Information...")
    
    assert hasattr(pynlm, '__version__')
    assert hasattr(pynlm, '__author__')
    assert hasattr(pynlm, '__description__')
    
    print(f"   ✓ Version: {pynlm.__version__}")
    print(f"   ✓ Author: {pynlm.__author__}")
    print(f"   ✓ Description: {pynlm.__description__}")
    print("   ✓ Version information retrieved\n")
    
    print("=== All Basic Functionality Tests Passed ===\n")
    return True

def test_error_handling():
    """Test error handling."""
    print("=== Testing Error Handling ===\n")
    
    # Test 1: Invalid configuration operations
    print("1. Testing Invalid Configuration Operations...")
    config = pynlm.createDefaultConfig()
    
    # These should not raise exceptions
    assert not config.has("invalid..key")
    assert config.validate()
    
    print("   ✓ Error handling for invalid operations passed\n")
    
    return True

def main():
    """Main test function."""
    print("NLM Python Bindings Basic Test Suite")
    print("=" * 50)
    print()
    
    try:
        # Run basic functionality tests
        if test_basic_functionality():
            print("✓ Basic functionality tests PASSED")
        else:
            print("✗ Basic functionality tests FAILED")
            return 1
            
        # Run error handling tests
        if test_error_handling():
            print("✓ Error handling tests PASSED")
        else:
            print("✗ Error handling tests FAILED")
            return 1
            
        print("\n" + "=" * 50)
        print("ALL TESTS PASSED!")
        print("=" * 50)
        print("\nThe NLM Python bindings are working correctly.")
        print("You can now use the NLM API for neural simulation projects.")
        
        return 0
        
    except Exception as e:
        print(f"\n✗ TEST FAILED WITH ERROR: {e}")
        import traceback
        traceback.print_exc()
        return 1

if __name__ == "__main__":
    sys.exit(main())
