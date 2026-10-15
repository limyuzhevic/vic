#!/usr/bin/env python3
"""
Test script to demonstrate the improved NLM Python bindings.

This script shows how to use the new convenience functions and improved API
to create, run, and analyze NLM neural simulations with minimal code.
"""

import pynlm
import sys
import os

def test_basic_usage():
    """Test basic functionality with the new convenience functions."""
    print("=== Testing Basic NLM Usage ===\n")
    
    # Test 1: Quick smoke test
    print("1. Running quick smoke test...")
    try:
        result = pynlm.quick_test()
        print(f"   ✓ Quick test successful: {result['neurons']} neurons, {result['spikes']} spikes")
    except Exception as e:
        print(f"   ✗ Quick test failed: {e}")
        return False
    
    # Test 2: Simple brain creation and running
    print("\n2. Testing simple brain creation and running...")
    try:
        brain = pynlm.create_and_run_brain(neuron_count=200, steps=500)
        stats = pynlm.get_brain_stats(brain)
        print(f"   ✓ Created brain with {stats['total_neurons']} neurons")
        print(f"   ✓ Ran for 500 steps, {stats['total_spikes']} total spikes")
        print(f"   ✓ Firing neurons: {stats['firing_neurons']}")
    except Exception as e:
        print(f"   ✗ Simple brain test failed: {e}")
        return False
    
    return True

def test_agent_simulation():
    """Test complete agent simulation with world."""
    print("\n=== Testing Agent Simulation ===\n")
    
    try:
        # Create brain and world
        print("1. Creating brain and world...")
        brain = pynlm.createBrain(pynlm.create_simple_config())
        world = pynlm.createSimpleWorld()
        world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
        world.reset()
        world.setAgentStart(10.0, 10.0)
        
        # Create and initialize agent
        print("2. Creating and initializing agent...")
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        # Run simulation
        print("3. Running simulation for 100 steps...")
        for step in range(100):
            world.update(0.1)
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            brain.step(step)
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
        
        # Check results
        stats = pynlm.get_brain_stats(brain)
        print(f"   ✓ Simulation complete!")
        print(f"   ✓ Neurons: {stats['total_neurons']}, Spikes: {stats['total_spikes']}")
        print(f"   ✓ Firing rate: {stats['average_firing_rate']:.2f}")
        print(f"   ✓ Developmental stage: {stats['developmental_stage']}")
        
        return True
    except Exception as e:
        print(f"   ✗ Agent simulation failed: {e}")
        import traceback
        traceback.print_exc()
        return False

def test_phase6_demo():
    """Test the Phase 6 integration demo (this might take longer)."""
    print("\n=== Testing Phase 6 Integration Demo ===\n")
    
    try:
        print("1. Running Phase 6 integration demo...")
        print("   Note: This demo tests all integrated brain systems (500 neurons, 2000 steps)")
        print("   This may take a minute or two...\n")
        
        result = pynlm.run_phase6_demo()
        
        print("2. Integration demo completed!")
        print(f"   ✓ Total reward: {result.totalReward:.2f}")
        print(f"   ✓ Average firing rate: {result.avgFiringRate:.2f}")
        print(f"   ✓ Episodes stored: {result.memoryEpisodesStored:.0f}")
        print(f"   ✓ Dopamine level: {result.dopamineLevel:.2f}")
        
        print("\n   System Integration Status:")
        print(f"   ✓ Working Memory: {'CONNECTED' if result.memoryWorkingMemoryIntegrated else 'DISCONNECTED'}")
        print(f"   ✓ Episodic Memory: {'CONNECTED' if result.memoryEpisodicMemoryIntegrated else 'DISCONNECTED'}")
        print(f"   ✓ Neuromodulation: {'CONNECTED' if result.neuromodulationIntegrated else 'DISCONNECTED'}")
        print(f"   ✓ Prediction: {'CONNECTED' if result.predictionIntegrated else 'DISCONNECTED'}")
        print(f"   ✓ Development: {'CONNECTED' if result.developmentIntegrated else 'DISCONNECTED'}")
        print(f"   ✓ Checkpointing: {'WORKING' if result.checkpointingWorks else 'NOT WORKING'}")
        
        return True
    except Exception as e:
        print(f"   ✗ Phase 6 demo failed: {e}")
        import traceback
        traceback.print_exc()
        return False

def test_save_load():
    """Test brain save and load functionality."""
    print("\n=== Testing Save/Load Functionality ===\n")
    
    try:
        # Create and run a brain
        print("1. Creating and running a brain...")
        brain = pynlm.create_and_run_brain(neuron_count=100, steps=200)
        stats1 = pynlm.get_brain_stats(brain)
        print(f"   ✓ Created brain with {stats1['total_neurons']} neurons")
        
        # Save the brain
        print("2. Saving brain to checkpoint.bin...")
        pynlm.save_brain(brain, "checkpoint.bin")
        print("   ✓ Brain saved successfully")
        
        # Create a new brain and load
        print("3. Creating new brain and loading checkpoint...")
        brain2 = pynlm.load_brain("checkpoint.bin")
        stats2 = pynlm.get_brain_stats(brain2)
        print(f"   ✓ Loaded brain with {stats2['total_neurons']} neurons")
        print(f"   ✓ Spikes: {stats2['total_spikes']} (should be non-zero)")
        
        # Cleanup
        if os.path.exists("checkpoint.bin"):
            os.remove("checkpoint.bin")
            print("   ✓ Cleaned up checkpoint file")
        
        return True
    except Exception as e:
        print(f"   ✗ Save/load test failed: {e}")
        import traceback
        traceback.print_exc()
        return False

def test_learning_experiment():
    """Test the LearningExperiment class."""
    print("\n=== Testing LearningExperiment ===\n")
    
    try:
        # Create experiment and brains
        print("1. Creating learning experiment...")
        experiment = pynlm.LearningExperiment()
        
        # Add a brain
        print("2. Adding brain to experiment...")
        brain = pynlm.createBrain(pynlm.create_minimal_config())
        brain.initialize()
        
        # Run some steps to generate activity
        for i in range(50):
            brain.step(i, i * 0.001f)
        
        experiment.addBrain("test_brain", brain)
        print("   ✓ Added brain to experiment")
        
        # Get summary
        print("3. Getting experiment summary...")
        summary = experiment.getSummary()
        print("   ✓ Experiment summary:")
        for line in summary.split('\n'):
            if line.strip():
                print(f"     {line}")
        
        return True
    except Exception as e:
        print(f"   ✗ LearningExperiment test failed: {e}")
        import traceback
        traceback.print_exc()
        return False

def main():
    """Run all tests."""
    print("=" * 60)
    print("NLM Python Bindings - Improved API Test Suite")
    print("=" * 60)
    print()
    
    tests = [
        ("Basic Usage", test_basic_usage),
        ("Agent Simulation", test_agent_simulation),
        ("Phase 6 Demo", test_phase6_demo),
        ("Save/Load", test_save_load),
        ("LearningExperiment", test_learning_experiment),
    ]
    
    passed = 0
    failed = 0
    
    for test_name, test_func in tests:
        print(f"\n{'='*60}")
        print(f"Running: {test_name}")
        print('='*60)
        
        try:
            if test_func():
                passed += 1
                print(f"\n✓ {test_name} PASSED")
            else:
                failed += 1
                print(f"\n✗ {test_name} FAILED")
        except Exception as e:
            failed += 1
            print(f"\n✗ {test_name} FAILED with exception: {e}")
    
    print("\n" + "="*60)
    print("Test Results Summary")
    print("="*60)
    print(f"Total tests: {len(tests)}")
    print(f"Passed: {passed}")
    print(f"Failed: {failed}")
    print("="*60)
    
    if failed == 0:
        print("\n🎉 All tests passed! The improved Python bindings are working correctly.")
        return 0
    else:
        print(f"\n❌ {failed} test(s) failed. Please check the output above.")
        return 1

if __name__ == "__main__":
    sys.exit(main())
