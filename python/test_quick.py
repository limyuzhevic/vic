#!/usr/bin/env python3
"""
Quick test script to verify the enhanced Python bindings functionality.
This script provides a simple way to test the basic functionality of pynlm.
"""

import pynlm
import sys

def test_basic():
    """Test basic functionality of enhanced Python bindings."""
    print("Testing basic pynlm functionality...")
    
    try:
        # Test Config
        config = pynlm.createDefaultConfig()
        print("✓ Created default config")
        
        config.set("test.value", 42)
        assert config.getOr("test.value", 0) == 42
        print("✓ Config set/get works")
        
        # Test Brain
        brain = pynlm.createBrain(config)
        print("✓ Created brain")
        
        # Test World
        world = pynlm.createSimpleWorld()
        world.configure(64, 64, 16, 16)
        print("✓ Created world")
        
        # Test Agent
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        print("✓ Created agent brain")
        
        # Test Vision
        vision = pynlm.createVision(10, 10, 3)
        print("✓ Created vision")
        
        # Test Action
        action = pynlm.createAction(pynlm.ActionType.MoveForward)
        print("✓ Created action")
        
        # Test utility functions
        neuron_types = pynlm.getNeuronTypes()
        assert "Excitatory" in neuron_types
        print("✓ Utility functions work")
        
        print("\nAll basic tests passed! ✓")
        return True
        
    except Exception as e:
        print(f"\nTest failed: {e}")
        import traceback
        traceback.print_exc()
        return False

if __name__ == "__main__":
    success = test_basic()
    sys.exit(0 if success else 1)