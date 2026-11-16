#!/usr/bin/env python3
"""
Test script for improved NLM Python bindings
"""

import pynlm
import sys

def test_basic_functionality():
    print("Testing basic NLM functionality...")
    
    # Test 1: Create brain with default config
    print("1. Creating brain with default config...")
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    print(f"   ✓ Brain created with {brain.getTotalNeuronCount()} neurons")
    
    # Test 2: Run basic simulation
    print("2. Running basic simulation...")
    for i in range(10):
        brain.step(i)
    print(f"   ✓ Simulation completed, {brain.getTotalSpikeCount()} spikes generated")
    
    # Test 3: Get brain stats
    print("3. Getting brain statistics...")
    stats = pynlm.analyzeBrainState(brain)
    print(f"   ✓ Analysis complete: {len(stats)} metrics collected")
    
    return True

def test_world_simulation():
    print("\nTesting world simulation functionality...")
    
    # Test 4: Quick setup
    print("4. Quick setup test...")
    brain, world, agent = pynlm.quickSetup(15, 15, 500)
    print(f"   ✓ Quick setup complete: brain with {brain.getTotalNeuronCount()} neurons")
    
    # Test 5: Run episode simulation
    print("5. Running simulation episode...")
    def simple_reward_func(percept):
        # Simple reward based on sensory input
        return 1.0
    
    episode_data = pynlm.runTrainingEpisode(brain, agent, world, 20, simple_reward_func)
    print(f"   ✓ Episode completed with {len(episode_data)} steps")
    
    # Test 6: Analyze memory state
    print("6. Analyzing memory state...")
    memory_stats = pynlm.analyzeMemoryState(brain)
    print(f"   ✓ Memory analysis complete: {len(memory_stats)} memory systems analyzed")
    
    return True

def test_advanced_features():
    print("\nTesting advanced features...")
    
    # Test 7: Create advanced config
    print("7. Testing advanced configuration...")
    config = pynlm.createAdvancedConfig(
        "research_mode",
        enableSTDP=True,
        enableHebbian=True,
        enableStructural=False,
        enableCuriosity=True,
        enableDevelopment=True
    )
    print("   ✓ Advanced config created")
    
    # Test 8: Save brain with timestamp
    print("8. Testing brain saving...")
    saved_path = pynlm.saveBrainWithTimestamp(brain, "test_brain")
    print(f"   ✓ Brain saved to: {saved_path}")
    
    # Test 9: Compare multiple brains
    print("9. Testing brain comparison...")
    brains = [brain, brain]  # Use same brain for demonstration
    comparison = pynlm.compareBrains(brains)
    print(f"   ✓ Comparison complete: {len(comparison)} comparison metrics")
    
    # Test 10: Encode text (Phase 6 feature)
    print("10. Testing text encoding (Phase 6)...")
    text_pattern = pynlm.encodeTextForBrain(agent, "hello")
    print(f"   ✓ Text encoded: {len(text_pattern)} neurons")
    
    return True

def test_benchmark():
    print("\nTesting benchmark functionality...")
    
    # Test  bapak
c    print("11. Testing brain benchmark...")
    benchmark_results = pynlm.benchmarkBrainPerformance(brain, 100, 5)
    print(f"   ✓ Benchmark completed: {benchmark_results['avg_time_per_run']:.3f}s avg per run")
    
    return True

def main():
    print("=" * 60)
    print("NLM Python Bindings Test Suite")
    print("=" * 60)
    
    try:
        test_basic_functionality()
        test_world_simulation()
        test_advanced_features()
        test_benchmark()
        
        print("\n" + "=" * 60)
        print("✅ ALL TESTS PASSED!")
        print("=" * 60)
        return 0
        
    except Exception as e:
        print(f"\n❌ TEST FAILED: {e}")
        import traceback
        traceback.print_exc()
        return 1

if __name__ == "__main__":
    sys.exit(main())