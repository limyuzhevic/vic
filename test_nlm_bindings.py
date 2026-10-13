#!/usr/bin/env python3
"""
Simple test to verify NLM Python bindings.
This script will try to import and use the basic NLM functionality.
"""

import sys
import os

# Add the build directory to the path to try to import pynlm
build_dir = os.path.join(os.path.dirname(__file__), "build")
sys.path.insert(0, build_dir)

def test_basic_bindings():
    """Test basic NLM bindings"""
    try:
        import pynlm
        print("✓ Successfully imported pynlm")
        
        # Test that expected functions exist
        assert hasattr(pynlm, "createDefaultConfig")
        assert hasattr(pynlm, "createBrain")
        assert hasattr(pynlm, "createSimpleWorld")
        assert hasattr(pynlm, "createAgentBrain")
        print("✓ All factory functions exist")
        
        # Test expected attributes
        assert hasattr(pynlm, "NeuronType")
        assert hasattr(pynlm, "DevelopmentalStage")
        assert hasattr(pynlm, "ActionType")
        print("✓ All enums exist")
        
        # Test expected classes
        assert hasattr(pynlm, "Config")
        assert hasattr(pynlm, "Brain")
        assert hasattr(pynlm, "SimpleWorld")
        assert hasattr(pynlm, "AgentBrain")
        print("✓ All classes exist")
        
        # Test version attribute
        if hasattr(pynlm, "__version__"):
            print(f"✓ Version: {pynlm.__version__}")
        
        return True
        
    except ImportError as e:
        print(f"✗ Failed to import pynlm: {e}")
        return False
    except AttributeError as e:
        print(f"✗ Missing expected attribute: {e}")
        return False
    except AssertionError as e:
        print(f"✗ Assertion failed: {e}")
        return False

def test_example_usage():
    """Test the example usage from easy_usage.md"""
    try:
        import pynlm
        
        # Test Example 1: Simplest Brain
        print("\n--- Testing Example 1: Simplest Brain ---")
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        for i in range(10):
            brain.step(i)
        
        print(f"✓ Created and ran brain with {brain.getTotalSpikeCount()} spikes")
        
        # Test Example 2: Brain in a Simple World  
        print("\n--- Testing Example 2: Brain in a Simple World ---")
        brain = pynlm.createBrain(pynlm.createDefaultConfig())
        brain.initialize()
        
        world = pynlm.createSimpleWorld()
        world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
        world.reset()
        
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        for step in range(5):
            world.update(0.1)
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            brain.step(step)
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
        
        print(f"✓ Simulation complete with {brain.getFiringNeuronCount()} firing neurons")
        
        return True
        
    except Exception as e:
        print(f"✗ Example usage test failed: {e}")
        import traceback
        traceback.print_exc()
        return False

def main():
    print("Testing NLM Python Bindings")
    print("=" * 50)
    
    success = True
    success = test_basic_bindings() and success
    success = test_example_usage() and success
    
    print("\n" + "=" * 50)
    if success:
        print("All tests passed! ✓")
        return 0
    else:
        print("Some tests failed! ✗")
        return 1

if __name__ == "__main__":
    sys.exit(main())
