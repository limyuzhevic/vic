#!/usr/bin/env python3
"""
Test script for NLM Python bindings (pynlm)
This script tests the basic functionality of the Python bindings.
"""

import sys
import traceback

def test_imports():
    """Test that pynlm can be imported"""
    try:
        import pynlm
        print("✓ Successfully imported pynlm")
        print(f"  Version: {pynlm.__version__}")
        return pynlm
    except Exception as e:
        print(f"✗ Failed to import pynlm: {e}")
        return None

def test_factory_functions(pynlm):
    """Test factory functions"""
    print("\n--- Testing Factory Functions ---")
    
    # Test createDefaultConfig
    try:
        config = pynlm.createDefaultConfig()
        print("✓ createDefaultConfig() - OK")
        print(f"  Config type: {type(config)}")
        print(f"  Has keys: {config.getKeys()}")
    except Exception as e:
        print(f"✗ createDefaultConfig() failed: {e}")
        return False
    
    # Test createBrainWithDefaults
    try:
        brain = pynlm.createBrainWithDefaults()
        print("✓ createBrainWithDefaults() - OK")
        print(f"  Brain type: {type(brain)}")
        print(f"  Neurons: {brain.getTotalNeuronCount()}")
    except Exception as e:
        print(f"✗ createBrainWithDefaults() failed: {e}")
        return False
    
    # Test createSimpleWorldWithDefaults
    try:
        world = pynlm.createSimpleWorldWithDefaults()
        print("✓ createSimpleWorldWithDefaults() - OK")
        print(f"  World type: {type(world)}")
        print(f"  Width: {world.getWidth()}, Height: {world.getHeight()}")
    except Exception as e:
        print(f"✗ createSimpleWorldWithDefaults() failed: {e}")
        return False
    
    # Test createAgentBrain
    try:
        agent = pynlm.createAgentBrain(brain)
        print("✓ createAgentBrain() - OK")
        print(f"  Agent type: {type(agent)}")
    except Exception as e:
        print(f"✗ createAgentBrain() failed: {e}")
        return False
    
    return True

def test_world_object(pynlm):
    """Test WorldObject constructor"""
    print("\n--- Testing WorldObject Constructor ---")
    
    try:
        # Test default constructor
        obj1 = pynlm.WorldObject()
        print("✓ WorldObject() - OK")
        print(f"  Default values: x={obj1.x}, y={obj1.y}, type={obj1.type}")
        
        # Test full constructor
        obj2 = pynlm.WorldObject(10.0, 20.0, pynlm.WorldObjectType.Resource, 5.0, 1.0)
        print("✓ WorldObject(x, y, type, value, radius) - OK")
        print(f"  Values: x={obj2.x}, y={obj2.y}, type={obj2.type}, value={obj2.value}, radius={obj2.radius}")
        
        # Test simplified constructor
        obj3 = pynlm.WorldObject(5.0, 5.0, pynlm.WorldObjectType.Wall)
        print("✓ WorldObject(x, y, type) - OK")
        print(f"  Values: x={obj3.x}, y={obj3.y}, type={obj3.type}")
        
        return True
    except Exception as e:
        print(f"✗ WorldObject tests failed: {e}")
        traceback.print_exc()
        return False

def test_agent_simulation(pynlm):
    """Test agent simulation"""
    print("\n--- Testing Agent Simulation ---")
    
    try:
        # Create components
        brain = pynlm.createBrainWithDefaults()
        world = pynlm.createSimpleWorldWithDefaults()
        agent = pynlm.createAgentBrain(brain)
        
        # Initialize
        brain.initialize()
        agent.initialize(world)
        
        # Run a few steps
        for step in range(10):
            world.update(0.1)
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            brain.step(step)
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            agent.updateDevelopment(0.1)
        
        print("✓ Agent simulation - OK")
        print(f"  Total steps: 10")
        print(f"  Neurons firing: {brain.getFiringNeuronCount()}")
        print(f"  Total spikes: {brain.getTotalSpikeCount()}")
        print(f"  Developmental stage: {brain.getDevelopmentalStage()}")
        
        return True
    except Exception as e:
        print(f"✗ Agent simulation failed: {e}")
        traceback.print_exc()
        return False

def test_benchmark(pynlm):
    """Test benchmark function"""
    print("\n--- Testing Benchmark Function ---")
    
    try:
        brain = pynlm.createBrainWithDefaults()
        stats = pynlm.benchmarkBrain(brain, 100)
        
        print("✓ benchmarkBrain() - OK")
        print(f"  Stats type: {type(stats)}")
        print(f"  Total steps: {stats['total_steps']}")
        print(f"  Total spikes: {stats['total_spikes']}")
        print(f"  Firing neurons: {stats['firing_neurons']}")
        print(f"  Average firing rate: {stats['average_firing_rate']:.2f}")
        print(f"  Total neurons: {stats['total_neurons']}")
        print(f"  Total synapses: {stats['total_synapses']}")
        print(f"  Elapsed ms: {stats['elapsed_ms']}")
        print(f"  Steps per second: {stats['steps_per_second']:.2f}")
        
        return True
    except Exception as e:
        print(f"✗ benchmarkBrain() failed: {e}")
        traceback.print_exc()
        return False

def test_error_handling(pynlm):
    """Test error handling"""
    print("\n--- Testing Error Handling ---")
    
    success = True
    
    # Test null brain for agent brain
    try:
        agent = pynlm.createAgentBrain(None)
        print("✗ Should have thrown exception for null brain")
        success = False
    except Exception as e:
        print(f"✓ Correctly threw exception for null brain: {type(e).__name__}")
    
    # Test null config for brain
    try:
        brain = pynlm.createBrain(None)
        print("✗ Should have thrown exception for null config")
        success = False
    except Exception as e:
        print(f"✓ Correctly threw exception for null config: {type(e).__name__}")
    
    return success

def main():
    """Run all tests"""
    print("=" * 60)
    print("Testing NLM Python Bindings (pynlm)")
    print("=" * 60)
    
    # Test imports
    pynlm = test_imports()
    if not pynlm:
        print("\n❌ Cannot continue without pynlm module")
        return 1
    
    # Run all tests
    results = []
    
    results.append(("Factory Functions", test_factory_functions(pynlm)))
    results.append(("WorldObject", test_world_object(pynlm)))
    results.append(("Agent Simulation", test_agent_simulation(pynlm)))
    results.append(("Benchmark", test_benchmark(pynlm)))
    results.append(("Error Handling", test_error_handling(pynlm)))
    
    # Summary
    print("\n" + "=" * 60)
    print("TEST SUMMARY")
    print("=" * 60)
    
    passed = 0
    total = len(results)
    
    for test_name, result in results:
        status = "✓ PASS" if result else "✗ FAIL"
        print(f"{test_name:20} {status}")
        if result:
            passed += 1
    
    print(f"\nPassed: {passed}/{total}")
    
    if passed == total:
        print("🎉 All tests passed!")
        return 0
    else:
        print(f"❌ {total - passed} tests failed")
        return 1

if __name__ == "__main__":
    sys.exit(main())