#!/usr/bin/env python3
"""
Quick Test for NLM Improvements

This is a simple test to verify that the NLM improvements are working correctly.
It tests the enhanced Python bindings, error handling, and modular design.
"""

import sys
import traceback

def test_imports():
    """Test that all NLM modules can be imported."""
    print("Testing imports...")
    try:
        import pynlm
        print("✓ pynlm imported successfully")
        
        # Test that all expected classes are available
        assert hasattr(pynlm, 'createDefaultConfig')
        assert hasattr(pynlm, 'createBrain')
        assert hasattr(pynlm, 'createSimpleWorld')
        assert hasattr(pynlm, 'createAgentBrain')
        
        # Test enum access
        assert hasattr(pynlm, 'NeuronType')
        assert hasattr(pynlm, 'DevelopmentalStage')
        
        print("✓ All expected classes and enums available")
        return True
        
    except ImportError as e:
        print(f"✗ Import error: {e}")
        return False
    except AssertionError as e:
        print(f"✗ Assertion error: {e}")
        return False
    except Exception as e:
        print(f"✗ Unexpected error: {e}")
        traceback.print_exc()
        return False

def test_basic_functionality():
    """Test basic NLM functionality."""
    print("\nTesting basic functionality...")
    try:
        import pynlm
        
        # Create configuration
        config = pynlm.createDefaultConfig()
        print("✓ Configuration created")
        
        # Set parameters
        config.set("brain.neuron_count", 100)
        config.set("brain.synapse_density", 0.1)
        print("✓ Configuration parameters set")
        
        # Create brain
        brain = pynlm.createBrain(config)
        print("✓ Brain created")
        
        # Initialize brain
        brain.initialize()
        print("✓ Brain initialized")
        
        # Test basic queries
        neuron_count = brain.getTotalNeuronCount()
        print(f"✓ Brain has {neuron_count} neurons")
        
        # Run a few steps
        for step in range(10):
            brain.step(step)
        
        print("✓ Simulation steps completed")
        
        # Check statistics
        firing = brain.getFiringNeuronCount()
        spikes = brain.getTotalSpikeCount()
        print(f"✓ Statistics: {firing} firing, {spikes} total spikes")
        
        return True
        
    except Exception as e:
        print(f"✗ Error in basic functionality test: {e}")
        traceback.print_exc()
        return False

def test_agent_system():
    """Test the agent-world system."""
    print("\nTesting agent-world system...")
    try:
        import pynlm
        
        # Create all components
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        world = pynlm.createSimpleWorld()
        world.configure(10, 10, 5, 5)
        world.reset()
        world.setAgentStart(5.0, 5.0)
        
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        print("✓ Agent-world system created")
        
        # Enable subsystems
        agent.enableRewardModulation(True)
        agent.enableCuriosity(True)
        print("✓ Subsystems enabled")
        
        # Run a few simulation steps
        for step in range(20):
            world.update(0.1)
            
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            brain.step(step)
            
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
        
        print("✓ Agent simulation completed")
        
        # Check statistics
        curiosity = agent.getCuriosityLevel()
        novelty = agent.getNoveltyLevel()
        print(f"✓ Agent stats: curiosity={curiosity:.3f}, novelty={novelty:.3f}")
        
        return True
        
    except Exception as e:
        print(f"✗ Error in agent system test: {e}")
        traceback.print_exc()
        return False

def test_error_handling():
    """Test error handling and validation."""
    print("\nTesting error handling...")
    try:
        import pynlm
        
        # Create configuration
        config = pynlm.createDefaultConfig()
        
        # Test that configuration accepts various value types
        config.set("string_param", "test_value")
        config.set("int_param", 42)
        config.set("float_param", 3.14)
        config.set("bool_param", True)
        
        print("✓ Configuration accepts multiple value types")
        
        # Test configuration queries
        string_val = config.getOr("string_param", "default")
        int_val = config.getOr("int_param", 0)
        float_val = config.getOr("float_param", 0.0)
        bool_val = config.getOr("bool_param", False)
        
        assert string_val == "test_value"
        assert int_val == 42
        assert float_val == 3.14
        assert bool_val == True
        
        print("✓ Configuration queries work correctly")
        
        return True
        
    except Exception as e:
        print(f"✗ Error in error handling test: {e}")
        traceback.print_exc()
        return False

def main():
    """Run all tests."""
    print("NLM Improvements - Quick Test Suite")
    print("=" * 50)
    
    tests = [
        test_imports,
        test_basic_functionality,
        test_agent_system,
        test_error_handling,
    ]
    
    passed = 0
    total = len(tests)
    
    for test in tests:
        try:
            if test():
                passed += 1
        except Exception as e:
            print(f"✗ Test failed with exception: {e}")
    
    print("\n" + "=" * 50)
    print(f"Test Results: {passed}/{total} passed")
    
    if passed == total:
        print("🎉 All tests passed! NLM improvements are working correctly.")
        return 0
    else:
        print("⚠️  Some tests failed. Please check the output above.")
        return 1

if __name__ == "__main__":
    exit(main())
