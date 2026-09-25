#!/usr/bin/env python3
"""Test script for NLM high-level convenience APIs."""

import sys
import time

def test_quick_brain_creation():
    """Test the quick brain creation APIs."""
    print("Testing quick brain creation APIs...")
    
    try:
        import pynlm
        
        # Test basic configuration
        print("  Testing createQuickBrain('basic')...")
        brain = pynlm.createQuickBrain("basic")
        brain.initialize()
        print(f"    ✓ Created brain with {brain.getTotalNeuronCount()} neurons")
        
        # Test advanced configuration
        print("  Testing createQuickBrain('advanced')...")
        brain2 = pynlm.createQuickBrain("advanced")
        brain2.initialize()
        print(f"    ✓ Created advanced brain with {brain2.getTotalNeuronCount()} neurons")
        
        # Test focused configuration
        print("  Testing createQuickBrain('focused')...")
        brain3 = pynlm.createQuickBrain("focused")
        brain3.initialize()
        print(f"    ✓ Created focused brain with {brain3.getTotalNeuronCount()} neurons")
        
        # Test training brain
        print("  Testing createTrainingBrain()...")
        brain4 = pynlm.createTrainingBrain(500, enableDevelopment=True)
        brain4.initialize()
        print(f"    ✓ Created training brain with {brain4.getTotalNeuronCount()} neurons")
        
        return True
        
    except Exception as e:
        print(f"  ✗ Error: {e}")
        return False

def test_experiment_templates():
    """Test experiment template APIs."""
    print("\nTesting experiment template APIs...")
    
    try:
        import pynlm
        
        # Test exploration experiment
        print("  Testing createExplorationExperiment()...")
        agent, brain, world = pynlm.createExplorationExperiment()
        brain.initialize()
        agent.initialize(world)
        print(f"    ✓ Created exploration experiment")
        print(f"      - Brain: {brain.getTotalNeuronCount()} neurons")
        print(f"      - Developmental stage: {brain.getDevelopmentalStage()}")
        
        # Test memory experiment
        print("  Testing createMemoryExperiment()...")
        agent2, brain2, world2 = pynlm.createMemoryExperiment()
        brain2.initialize()
        agent2.initialize(world2)
        print(f"    ✓ Created memory experiment")
        print(f"      - Brain: {brain2.getTotalNeuronCount()} neurons")
        print(f"      - Developmental stage: {brain2.getDevelopmentalStage()}")
        
        return True
        
    except Exception as e:
        print(f"  ✗ Error: {e}")
        return False

def test_training_loops():
    """Test training loop APIs."""
    print("\nTesting training loop APIs...")
    
    try:
        import pynlm
        
        # Create experiment setup
        agent, brain, world = pynlm.createExplorationExperiment()
        brain.initialize()
        agent.initialize(world)
        
        # Enable learning subsystems
        agent.enableRewardModulation(True)
        agent.enableCuriosity(True)
        
        # Test single episode
        print("  Testing runEpisode()...")
        reward = pynlm.runEpisode(agent, world, maxSteps=100, verbose=False)
        print(f"    ✓ Episode completed with reward: {reward:.3f}")
        
        # Test training loop
        print("  Testing runTrainingLoop()...")
        start_time = time.time()
        rewards, averages = pynlm.runTrainingLoop(agent, world, numEpisodes=10, stepsPerEpisode=50)
        elapsed = time.time() - start_time
        
        print(f"    ✓ Training loop completed in {elapsed:.2f} seconds")
        print(f"      - Final average reward: {averages[-1]:.3f}")
        print(f"      - Best reward: {max(rewards):.3f}")
        
        return True
        
    except Exception as e:
        print(f"  ✗ Error: {e}")
        return False

def test_memory_management():
    """Test memory management APIs."""
    print("\nTesting memory management APIs...")
    
    try:
        import pynlm
        
        # Create brain and save checkpoint
        print("  Testing saveBrainCheckpoint()...")
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        pynlm.saveBrainCheckpoint(brain, "test_checkpoint.pt")
        print("    ✓ Brain checkpoint saved")
        
        # Load checkpoint
        print("  Testing loadBrainCheckpoint()...")
        loaded_brain = pynlm.loadBrainCheckpoint("test_checkpoint.pt")
        print("    ✓ Brain checkpoint loaded")
        
        # Test memory replay (simplified)
        print("  Testing memory replay...")
        result = pynlm.createMemoryReplay(brain, replaySteps=10, replayRate=10.0)
        print(f"    ✓ Memory replay completed: {result}")
        
        return True
        
    except Exception as e:
        print(f"  ✗ Error: {e}")
        return False

def test_visualization_helpers():
    """Test visualization helper APIs."""
    print("\nTesting visualization helper APIs...")
    
    try:
        import pynlm
        
        # Create brain for metrics collection
        brain = pynlm.createTrainingBrain()
        brain.initialize()
        
        # Test metrics collection
        print("  Testing collectBrainMetrics()...")
        metrics = pynlm.collectBrainMetrics(brain)
        print(f"    ✓ Collected {len(metrics)} metrics")
        
        # Print sample metrics
        sample_metrics = {"total_neurons": "1000", "firing_rate": "%.3f" % metrics.get("average_firing_rate", 0)}
        print(f"    Sample: {sample_metrics}")
        
        # Test visualization data generation
        print("  Testing createVisualizationData()...")
        data = pynlm.createVisualizationData(brain, steps=20, timestep=0.1)
        print(f"    ✓ Generated {len(data)} time series points")
        
        return True
        
    except Exception as e:
        print(f"  ✗ Error: {e}")
        return False

def test_debugging_tools():
    """Test debugging tool APIs."""
    print("\nTesting debugging tool APIs...")
    
    try:
        import pynlm
        
        # Create brain for testing
        brain = pynlm.createTrainingBrain()
        brain.initialize()
        
        # Test brain inspection
        print("  Testing inspectBrainState()...")
        result = pynlm.inspectBrainState(brain)
        print(f"    ✓ Brain inspection completed: {result}")
        
        # Test brain health check
        print("  Testing checkBrainHealth()...")
        healthy = pynlm.checkBrainHealth(brain)
        print(f"    ✓ Brain health check: {'Healthy' if healthy else 'Unhealthy'}")
        
        return True
        
    except Exception as e:
        print(f"  ✗ Error: {e}")
        return False

def test_advanced_apis():
    """Test advanced APIs."""
    print("\nTesting advanced APIs...")
    
    try:
        import pynlm
        
        # Test multi-region brain
        print("  Testing createMultiRegionBrain()...")
        brain = pynlm.createMultiRegionBrain({"cortex", "hippocampus", "amygdala"})
        brain.initialize()
        print(f"    ✓ Created multi-region brain with {brain.getTotalNeuronCount()} neurons")
        
        # Test advanced learning
        print("  Testing enableAdvancedLearning()...")
        result = pynlm.enableAdvancedLearning(brain, True, False)
        print(f"    ✓ Advanced learning enabled: {result}")
        
        return True
        
    except Exception as e:
        print(f"  ✗ Error: {e}")
        return False

def main():
    """Run all tests."""
    print("=" * 60)
    print("NLM High-Level APIs Test Suite")
    print("=" * 60)
    
    tests = [
        ("Quick Brain Creation", test_quick_brain_creation),
        ("Experiment Templates", test_experiment_templates),
        ("Training Loops", test_training_loops),
        ("Memory Management", test_memory_management),
        ("Visualization Helpers", test_visualization_helpers),
        ("Debugging Tools", test_debugging_tools),
        ("Advanced APIs", test_advanced_apis),
    ]
    
    results = []
    for test_name, test_func in tests:
        print(f"\n{'-' * 40}")
        print(f"Running: {test_name}")
        print(f"{'-' * 40}")
        
        try:
            success = test_func()
            results.append((test_name, success))
        except Exception as e:
            print(f"✗ Test failed with exception: {e}")
            results.append((test_name, False))
    
    # Print summary
    print("\n" + "=" * 60)
    print("TEST SUMMARY")
    print("=" * 60)
    
    passed = 0
    total = len(results)
    
    for test_name, success in results:
        status = "✓ PASSED" if success else "✗ FAILED"
        print(f"{status}: {test_name}")
        if success:
            passed += 1
    
    print(f"\nTotal: {passed}/{total} tests passed")
    
    if passed == total:
        print("\n🎉 All tests passed! High-level APIs are working correctly.")
        return 0
    else:
        print(f"\n❌ {total - passed} test(s) failed. Please check the implementation.")
        return 1

if __name__ == "__main__":
    sys.exit(main())
