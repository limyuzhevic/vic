#!/usr/bin/env python3
"""
Test script for advanced NLM Python API.

This script demonstrates the basic functionality of the advanced API
and ensures the core functionality still works.
"""

import sys
import os

# Add the current directory to the path so we can import pynlm
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

import pynlm

def test_basic_functionality():
    """Test basic NLM functionality."""
    print("Testing basic NLM functionality...")
    
    # Create brain
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    brain.initialize()
    
    # Check basic stats
    print(f"  Neurons created: {brain.getTotalNeuronCount()}")
    print(f"  Synapses created: {brain.getTotalSynapseCount()}")
    
    # Run a few steps
    for step in range(10):
        brain.step(step)
    
    print(f"  Average firing rate: {brain.getAverageFiringRate()}")
    print(f"  Total spikes: {brain.getTotalSpikeCount()}")
    
    return brain

def test_advanced_api(brain):
    """Test the advanced API functionality."""
    print("\nTesting advanced API...")
    
    try:
        from python.advanced_api import AdvancedBrainManager, VisualizationHelper
        
        # Test AdvancedBrainManager
        print("  Testing AdvancedBrainManager...")
        manager = AdvancedBrainManager(brain)
        print("    ✓ AdvancedBrainManager created successfully")
        
        # Test VisualizationHelper
        print("  Testing VisualizationHelper...")
        summary = VisualizationHelper.generate_brain_summary(brain)
        print(f"    ✓ Brain summary generated: {len(summary)} sections")
        
        readable = VisualizationHelper.format_metrics_readable(summary)
        print(f"    ✓ Metrics formatted (length: {len(readable)} chars)")
        
        return True
        
    except Exception as e:
        print(f"    ✗ Advanced API test failed: {e}")
        return False

def test_world_simulation():
    """Test world and agent simulation."""
    print("\nTesting world simulation...")
    
    # Create world
    world = pynlm.createSimpleWorld()
    world.configure(width=20.0, height=20.0, visionWidth=8, visionHeight=8)
    world.reset()
    
    # Create brain
    config = pynlm.createDefaultConfig()
    config.set("neuron_count", 100)
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Create agent
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable learning
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    
    # Run simulation
    for step in range(50):
        world.update(0.1)
        
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        brain.step(step)
        
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
        agent.applyRewardModulation(reward, 0.0)
    
    print(f"  Simulation completed successfully")
    print(f"  Final curiosity level: {agent.getCuriosityLevel():.3f}")
    print(f"  Final novelty level: {agent.getNoveltyLevel():.3f}")
    print(f"  Final neuromodulation level: {agent.getNeuromodulationLevel():.3f}")
    
    return True

def test_phase6_demo():
    """Test Phase 6 integration demo."""
    print("\nTesting Phase 6 integration...")
    
    try:
        from python.advanced_api import ExperimentRunner
        
        # Create simple world config
        world_config = {
            'width': 20.0,
            'height': 20.0,
            'visionWidth': 8,
            'visionHeight': 8
        }
        
        # Create brain config
        brain_config = {
            'neuron_count': 500,
            'region_count': 1,
            'connection_probability': 0.1
        }
        
        # Create environment changes
        environment_changes = [
            {
                'phase': 'stable',
                'steps_per_episode': 100
            }
        ]
        
        print("  Running experiment runner...")
        results = ExperimentRunner.run_learning_curve(
            brain_config, world_config, environment_changes, num_episodes=5
        )
        
        print(f"  ✓ Experiment completed with {len(results)} phase(s)")
        print(f"  ✓ Final episode reward: {results[0]['results'][-1]['final_reward']:.3f}")
        
        return True
        
    except Exception as e:
        print(f"  ✗ Phase 6 test failed: {e}")
        import traceback
        traceback.print_exc()
        return False

def main():
    """Run all tests."""
    print("=" * 60)
    print("Advanced NLM Python API Test Suite")
    print("=" * 60)
    
    tests = [
        ("Basic Functionality", test_basic_functionality),
        ("World Simulation", test_world_simulation),
        ("Phase 6 Integration", test_phase6_demo),
    ]
    
    results = []
    
    for test_name, test_func in tests:
        print(f"\n{'=' * 60}")
        print(f"Running: {test_name}")
        print('=' * 60)
        
        try:
            if test_name == "Basic Functionality":
                brain = test_func()
                success = test_advanced_api(brain)
            else:
                success = test_func()
            
            results.append((test_name, success))
            
        except Exception as e:
            print(f"✗ {test_name} failed with error: {e}")
            import traceback
            traceback.print_exc()
            results.append((test_name, False))
    
    # Summary
    print(f"\n{'=' * 60}")
    print("TEST SUMMARY")
    print('=' * 60)
    
    passed = 0
    for test_name, success in results:
        status = "✓ PASSED" if success else "✗ FAILED"
        print(f"{test_name}: {status}")
        if success:
            passed += 1
    
    print(f"\n{passed}/{len(results)} tests passed")
    
    if passed == len(results):
        print("\n🎉 All tests passed! Advanced API is working correctly.")
        return 0
    else:
        print(f"\n❌ {len(results) - passed} test(s) failed.")
        return 1

if __name__ == "__main__":
    sys.exit(main())