#!/usr/bin/env python3
"""
Test script for NLM Python bindings
Tests basic functionality of the NLM library
"""

import sys
import traceback

try:
    import pynlm
    print("✓ Successfully imported pynlm")
except ImportError as e:
    print(f"✗ Failed to import pynlm: {e}")
    sys.exit(1)
except Exception as e:
    print(f"✗ Unexpected error importing pynlm: {e}")
    traceback.print_exc()
    sys.exit(1)

def test_basic_functionality():
    """Test basic NLM functionality"""
    print("\n=== Testing Basic NLM Functionality ===")
    
    try:
        # Test 1: Create default config
        print("Test 1: Creating default configuration...")
        config = pynlm.createDefaultConfig()
        print(f"✓ Config created: {config}")
        
        # Test 2: Create brain
        print("\nTest 2: Creating brain...")
        brain = pynlm.createBrain(config)
        print(f"✓ Brain created: {brain}")
        
        # Test 3: Initialize brain
        print("\nTest 3: Initializing brain...")
        brain.initialize()
        print(f"✓ Brain initialized")
        print(f"  Total neurons: {brain.getTotalNeuronCount()}")
        print(f"  Total synapses: {brain.getTotalSynapseCount()}")
        
        # Test 4: Run simulation
        print("\nTest 4: Running simulation...")
        for i in range(10):
            brain.step(i)
        
        print(f"✓ Simulation completed")
        print(f"  Total spikes: {brain.getTotalSpikeCount()}")
        print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
        print(f"  Average firing rate: {brain.getAverageFiringRate():.3f}")
        
        # Test 5: Create SimpleWorld
        print("\nTest 5: Creating SimpleWorld...")
        world = pynlm.createSimpleWorld()
        print(f"✓ World created")
        
        # Configure world
        world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
        print(f"  World configured: {world.getWidth()}x{world.getHeight()}")
        
        # Reset world
        world.reset()
        print(f"✓ World reset")
        
        # Test 6: Create AgentBrain
        print("\nTest 6: Creating AgentBrain...")
        agent = pynlm.createAgentBrain(brain)
        print(f"✓ Agent created")
        
        # Initialize agent
        agent.initialize(world)
        print(f"✓ Agent initialized")
        
        # Test 7: Run agent simulation
        print("\nTest 7: Running agent simulation...")
        world.update(0.1)
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(0)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        print(f"✓ Agent simulation step completed")
        print(f"  Action: {action}")
        print(f"  Curiosity level: {agent.getCuriosityLevel():.3f}")
        print(f"  Novelty level: {agent.getNoveltyLevel():.3f}")
        
        # Test 8: Enable neuromodulation features
        print("\nTest 8: Testing neuromodulation features...")
        agent.enableRewardModulation(True)
        agent.enableStructuralPlasticity(True)
        agent.enableDevelopment(True)
        agent.enableCuriosity(True)
        
        print(f"✓ Neuromodulation features enabled")
        print(f"  Reward modulation: {agent.isRewardModulationEnabled()}")
        print(f"  Structural plasticity: {agent.isStructuralPlasticityEnabled()}")
        print(f"  Development: {agent.isDevelopmentEnabled()}")
        print(f"  Curiosity: {agent.isCuriosityEnabled()}")
        
        # Test 9: Get developmental stage
        print("\nTest 9: Getting developmental stage...")
        stage = brain.getDevelopmentalStage()
        print(f"✓ Developmental stage: {stage}")
        
        agent_stage = agent.getDevelopmentalStage()
        print(f"✓ Agent developmental stage: {agent_stage}")
        
        return True
        
    except Exception as e:
        print(f"✗ Test failed: {e}")
        traceback.print_exc()
        return False

def test_error_handling():
    """Test error handling in Python bindings"""
    print("\n=== Testing Error Handling ===")
    
    try:
        # Test 1: Invalid operation after uninitialized brain
        print("Test 1: Testing error handling...")
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        
        # This should work fine (brain can be created without initialization)
        print("✓ Brain created without initialization (acceptable)")
        
        # Initialize properly
        brain.initialize()
        print("✓ Brain initialized properly")
        
        return True
        
    except Exception as e:
        print(f"✗ Error handling test failed: {e}")
        traceback.print_exc()
        return False

def test_documentation():
    """Test that documentation is accessible"""
    print("\n=== Testing Documentation Access ===")
    
    try:
        # Check module documentation
        doc = pynlm.__doc__
        if doc:
            print(f"✓ Module documentation available ({len(doc)} chars)")
        else:
            print("⚠ No module documentation")
        
        # Check Config class documentation
        config_doc = pynlm.Config.__doc__
        if config_doc:
            print(f"✓ Config class documentation available ({len(config_doc)} chars)")
        else:
            print("⚠ No Config class documentation")
        
        # Check Brain class documentation
        brain_doc = pynlm.Brain.__doc__
        if brain_doc:
            print(f"✓ Brain class documentation available ({len(brain_doc)} chars)")
        else:
            print("⚠ No Brain class documentation")
        
        return True
        
    except Exception as e:
        print(f"✗ Documentation test failed: {e}")
        traceback.print_exc()
        return False

def main():
    """Run all tests"""
    print("=" * 60)
    print("NLM Python Bindings Test Suite")
    print("=" * 60)
    
    tests = [
        ("Basic Functionality", test_basic_functionality),
        ("Error Handling", test_error_handling),
        ("Documentation", test_documentation),
    ]
    
    results = []
    
    for test_name, test_func in tests:
        print(f"\n{'='*60}")
        print(f"Running: {test_name}")
        print(f"{'='*60}")
        
        try:
            success = test_func()
            results.append((test_name, success))
            status = "✓ PASSED" if success else "✗ FAILED"
            print(f"\n{test_name}: {status}")
        except Exception as e:
            print(f"\n{test_name}: ✗ FAILED - {e}")
            results.append((test_name, False))
    
    # Summary
    print("\n" + "=" * 60)
    print("TEST SUMMARY")
    print("=" * 60)
    
    passed = sum(1 for _, success in results if success)
    total = len(results)
    
    for test_name, success in results:
        status = "✓ PASSED" if success else "✗ FAILED"
        print(f"{test_name}: {status}")
    
    print(f"\nTotal: {passed}/{total} tests passed")
    
    if passed == total:
        print("\n🎉 All tests passed!")
        return 0
    else:
        print(f"\n⚠️  {total - passed} test(s) failed")
        return 1

if __name__ == "__main__":
    sys.exit(main())
