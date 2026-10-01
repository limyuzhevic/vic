#!/usr/bin/env python3
"""
Simple test to verify NLM Python bindings work
"""

import sys
sys.path.insert(0, '/workspace/cf14c76c-4b1f-4d38-9a96-64cd8a400481/sessions/agent_5aedba32-17df-40be-8cbf-0b67a48c50c6/src/pynlm')

import pynlm

def test_basic_functionality():
    print("Testing NLM Python bindings...")
    
    # Test 1: Create default config
    print("Test 1: Creating default config...")
    try:
        config = pynlm.createDefaultConfig()
        print("✓ Config created successfully")
    except Exception as e:
        print(f"✗ Failed to create config: {e}")
        return False
    
    # Test 2: Create brain
    print("Test 2: Creating brain...")
    try:
        brain = pynlm.createBrain()
        print("✓ Brain created successfully")
    except Exception as e:
        print(f"✗ Failed to create brain: {e}")
        return False
    
    # Test 3: Create simple world
    print("Test 3: Creating simple world...")
    try:
        world = pynlm.createSimpleWorld()
        world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
        print("✓ World created successfully")
    except Exception as e:
        print(f"✗ Failed to create world: {e}")
        return False
    
    # Test 4: Create agent brain
    print("Test 4: Creating agent brain...")
    try:
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        print("✓ Agent brain created successfully")
    except Exception as e:
        print(f"✗ Failed to create agent brain: {e}")
        return False
    
    # Test 5: Test types and enums
    print("Test 5: Testing types and enums...")
    try:
        print(f"  - NeuronType.Excitatory = {pynlm.NeuronType.Excitatory}")
        print(f"  - ActionType.MoveForward = {pynlm.ActionType.MoveForward}")
        print(f"  - DevelopmentalStage.Adult = {pynlm.DevelopmentalStage.Adult}")
        print("✓ Types and enums work")
    except Exception as e:
        print(f"✗ Failed to test types: {e}")
        return False
    
    # Test 6: Test agent body
    print("Test 6: Testing agent body...")
    try:
        body = world.get_agent_body()
        print(f"  - Agent position: ({body.x}, {body.y})")
        print(f"  - Agent energy: {body.energy}")
        print("✓ Agent body works")
    except Exception as e:
        print(f"✗ Failed to test agent body: {e}")
        return False
    
    # Test 7: Test sensory percept
    print("Test 7: Testing sensory percept...")
    try:
        percept = world.get_sensory_percept()
        print(f"  - Vision dimensions: {len(percept.get_all_signals())} signals")
        print("✓ Sensory percept works")
    except Exception as e:
        print(f"✗ Failed to test sensory percept: {e}")
        return False
    
    print("\n✅ All tests passed!")
    return True

if __name__ == "__main__":
    success = test_basic_functionality()
    sys.exit(0 if success else 1)
