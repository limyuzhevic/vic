#!/usr/bin/env python3
"""
Simple test to verify NLM Python bindings and advanced toolkit functionality.
"""

import sys
import os
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

def test_python_bindings():
    """Test basic NLM Python bindings."""
    try:
        from nlm import (
            createDefaultConfig, 
            createBrain, 
            createSimpleWorld, 
            createAgentBrain
        )
        
        print("✓ NLM Python bindings imported successfully")
        
        # Create a simple brain
        config = createDefaultConfig()
        brain = createBrain(config)
        brain.initialize()
        
        print(f"✓ Brain created with {brain.getTotalNeuronCount()} neurons")
        
        # Create world
        world = createSimpleWorld()
        world.configure(20.0, 20.0, 8, 8)
        world.reset()
        
        print("✓ World created and reset")
        
        # Create agent
        agent = createAgentBrain(brain)
        agent.initialize(world)
        
        print("✓ Agent brain created and initialized")
        
        # Run a simple simulation
        for step in range(10):
            world.update(0.1)
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            brain.step(step, step * 0.001)
            
            motor_cmd = agent.decodeMotorCommand()
            action_result = world.applyMotorCommand(motor_cmd, world.getSimulationTime())
            
            agent.applyRewardModulation(action_result.reward, 0.0)
            agent.updateDevelopment(0.1)
        
        print(f"✓ Simple simulation completed")
        print(f"  - Total spikes: {brain.getTotalSpikeCount()}")
        print(f"  - Average firing rate: {brain.getAverageFiringRate():.3f}")
        print(f"  - Working memory traces: {brain.getWorkingMemory().getActiveTraces() if brain.getWorkingMemory() else 'N/A'}")
        
        return True
        
    except Exception as e:
        print(f"✗ Error testing NLM Python bindings: {e}")
        return False
def test_advanced_toolkit():
    """Test advanced toolkit functionality."""
    try:
        from nlm_advanced_toolkit import NLMToolkit
        
        print("\n✓ Advanced toolkit imported successfully")
        
        toolkit = NLMToolkit()
        
        # Test basic simulation
        print("Testing basic simulation...")
        results = toolkit.run_simulation(steps=50)
        
        print(f"✓ Basic simulation completed")
        print(f"  - Total reward: {results['total_reward']:.3f}")
        print(f"  - Steps completed: {results['steps_completed']}")
        
        # Test benchmarking
        print("\nTesting benchmarking...")
        benchmark_results = toolkit.benchmark_performance([100, 500], [50, 100])
        
        print(f"✓ Benchmark completed {len(benchmark_results['benchmarks'])} tests")
        for bench in benchmark_results['benchmarks']:
            print(f"  - {bench['neurons']} neurons, {bench['steps']} steps: {bench['avg_reward_per_step']:.6f} avg reward/step")
        
        return True
        
    except Exception as e:
        print(f"✗ Error testing advanced toolkit: {e}")
        return False
if __name__ == "__main__":
    print("=" * 60)
    print("NLM Python Bindings and Advanced Toolkit Test")
    print("=" * 60)
    
    success = True
    
    if not test_python_bindings():
        success = False
    
    if not test_advanced_toolkit():
        success = False
    
    print("\n" + "=" * 60)
    if success:
        print("✓ ALL TESTS PASSED - NLM Python integration is working correctly")
    else:
        print("✗ SOME TESTS FAILED - Please check the errors above")
    print("=" * 60)
    
    sys.exit(0 if success else 1)