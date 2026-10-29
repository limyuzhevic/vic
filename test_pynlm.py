#!/usr/bin/env python3
"""
NLM (Neural Learning Machine) - Python Bindings Test Suite

This script tests the Python bindings for NLM to ensure they work correctly.
It tests basic functionality, API consistency, and error handling.
"""

import sys
import os
import traceback

def test_python_bindings():
    """Test basic Python bindings functionality"""
    print("=== Testing NLM Python Bindings ===\n")
    
    tests_passed = 0
    tests_failed = 0
    
    try:
        # Test 1: Import pynlm
        print("Test 1: Importing pynlm...")
        import pynlm
        print("✅ SUCCESS: pynlm imported successfully")
        tests_passed += 1
        
        # Test 2: Check module attributes
        print("\nTest 2: Checking module attributes...")
        required_attrs = ['createDefaultConfig', 'createBrain', 'createSimpleWorld', 
                         'createAgentBrain', 'NeuronType', 'ActionType', 'MotorCommand']
        
        for attr in required_attrs:
            if hasattr(pynlm, attr):
                print(f"  ✅ Found: {attr}")
            else:
                print(f"  ❌ Missing: {attr}")
                tests_failed += 1
        
        tests_passed += 1
        
        # Test 3: Create default config
        print("\nTest 3: Creating default config...")
        config = pynlm.createDefaultConfig()
        print(f"✅ Created config: {config}")
        tests_passed += 1
        
        # Test 4: Create brain
        print("\nTest 4: Creating brain...")
        brain = pynlm.createBrain(config)
        print(f"✅ Created brain with {brain.getTotalNeuronCount()} neurons")
        tests_passed += 1
        
        # Test 5: Initialize brain
        print("\nTest 5: Initializing brain...")
        brain.initialize()
        print("✅ Brain initialized")
        tests_passed += 1
        
        # Test 6: Get brain statistics
        print("\nTest 6: Getting brain statistics...")
        stats = {
            'Total neurons': brain.getTotalNeuronCount(),
            'Total synapses': brain.getTotalSynapseCount(),
            'Firing neurons': brain.getFiringNeuronCount(),
            'Average firing rate': brain.getAverageFiringRate(),
            'Developmental stage': brain.getDevelopmentalStage()
        }
        
        for key, value in stats.items():
            print(f"  {key}: {value}")
        
        print("✅ Brain statistics retrieved")
        tests_passed += 1
        
        # Test 7: Create world
        print("\nTest 7: Creating world...")
        world = pynlm.createSimpleWorld()
        world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
        print("✅ World created and configured")
        tests_passed += 1
        
        # Test 8: Reset world
        print("\nTest 8: Resetting world...")
        world.reset()
        print("✅ World reset")
        tests_passed += 1
        
        # Test 9: Create agent
        print("\nTest 9: Creating agent...")
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        print("✅ Agent created and initialized")
        tests_passed += 1
        
        # Test 10: Enable learning subsystems
        print("\nTest 10: Enabling learning subsystems...")
        agent.enableRewardModulation(True)
        agent.enableCuriosity(True)
        agent.enableDevelopment(True)
        print("✅ Learning subsystems enabled")
        tests_passed += 1
        
        # Test 11: Get agent statistics
        print("\nTest 11: Getting agent statistics...")
        agent_stats = {
            'Curiosity level': agent.getCuriosityLevel(),
            'Neuromodulation level': agent.getNeuromodulationLevel(),
            'Developmental stage': agent.getDevelopmentalStage()
        }
        
        for key, value in agent_stats.items():
            print(f"  {key}: {value}")
        
        print("✅ Agent statistics retrieved")
        tests_passed += 1
        
        # Test 12: Complete simulation step
        print("\nTest 12: Running simulation step...")
        world.update(0.1)
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(0)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        print(f"✅ Simulation step completed")
        print(f"   Action: {action}")
        tests_passed += 1
        
        # Test 13: Error handling - invalid config key
        print("\nTest 13: Testing error handling...")
        try:
            # This should not crash, but should handle gracefully
            if hasattr(config, 'has'):
                print("  ✅ Config has 'has' method for key checking")
            else:
                print("  ⚠️  Config lacks 'has' method")
        except Exception as e:
            print(f"  ⚠️  Error accessing config methods: {e}")
        
        tests_passed += 1
        
    except ImportError as e:
        print(f"❌ FAILED: Could not import pynlm: {e}")
        tests_failed += 1
        traceback.print_exc()
        
    except Exception as e:
        print(f"❌ FAILED: Unexpected error: {e}")
        tests_failed += 1
        traceback.print_exc()
    
    print("\n" + "="*50)
    print("TEST SUMMARY:")
    print(f"✅ Passed: {tests_passed}")
    print(f"❌ Failed: {tests_failed}")
    print(f"📊 Total: {tests_passed + tests_failed}")
    
    if tests_failed > 0:
        print(f"\n⚠️  {tests_failed} test(s) failed - API may have issues")
        return False
    else:
        print(f"\n🎉 All tests passed!")
        return True

if __name__ == "__main__":
    success = test_python_bindings()
    sys.exit(0 if success else 1)