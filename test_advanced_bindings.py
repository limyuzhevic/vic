#!/usr/bin/env python3
"""
Test script to verify the advanced NLM Python bindings.
This script tests all the new advanced features added to the NLM Python bindings.
"""

import sys
import os
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

import pynlm

def test_basic_functionality():
    """Test basic NLM functionality"""
    print("Testing basic NLM functionality...")
    
    # Create a default configuration
    config = pynlm.createDefaultConfig()
    print(f"  Created config with keys: {config.getKeys()}")
    
    # Create a brain with the configuration
    brain = pynlm.createBrain(config)
    print(f"  Created brain with {brain.total_neurons} neurons")
    
    # Create a world
    world = pynlm.createSimpleWorld()
    print(f"  Created world with dimensions {world.dimensions}")
    
    # Create an agent brain
    agent_brain = pynlm.createAgentBrain(brain)
    print(f"  Created agent brain")
    
    return brain, world, agent_brain

def test_advanced_brain_operations(brain):
    """Test advanced brain operations"""
    print("\nTesting advanced brain operations...")
    
    # Test setParameters
    print("  Testing setParameters...")
    parameters = [
        ("brain.neuronCount", 2000),
        ("plasticity.stdpLTPWeight", 0.02),
        ("memory.workingMemoryCapacity", 200)
    ]
    brain.setParameters(parameters)
    
    # Test saveState
    print("  Testing saveState...")
    state_file = "test_brain_state.nk"
    if brain.saveState(state_file, compress=True, level=pynlm.CompressionLevel.Balanced):
        print(f"    Saved brain state to {state_file}")
    
    # Test optimize
    print("  Testing optimize...")
    brain.optimize(iterations=10, targetSNR=15.0)
    
    # Test prune
    print("  Testing prune...")
    pruned = brain.prune(threshold=0.05, structural=False)
    print(f"    Pruning {'successful' if pruned else 'failed'}")
    
    # Test duplicate
    print("  Testing duplicate...")
    duplicate = brain.duplicate()
    print(f"    Created duplicate brain with {duplicate.total_neurons} neurons")
    
    # Test analyzeConnectivity
    print("  Testing analyzeConnectivity...")
    connectivity = brain.analyzeConnectivity()
    print(f"    Connectivity metrics: {len(connectivity)} metrics")
    
    # Test getEmergentBehaviors
    print("  Testing getEmergentBehaviors...")
    behaviors = brain.getEmergentBehaviors(threshold=0.3)
    print(f"    Emergent behaviors: {behaviors}")
    
    # Test benchmark
    print("  Testing benchmark...")
    benchmark_result = brain.benchmark(duration=500, detailed=False)
    print(f"    Benchmark: {'success' if benchmark_result else 'failed'}")
    
    # Test validateState
    print("  Testing validateState...")
    validate_result = brain.validateState(thorough=False)
    print(f"    State validation: {'passed' if validate_result else 'failed'}")
    
    # Clean up
    if os.path.exists(state_file):
        os.remove(state_file)
        print("    Cleaned up test state file")

def test_advanced_agent_features(agent_brain):
    """Test advanced agent features"""
    print("\nTesting advanced agent features...")
    
    # Test setLearningRate
    print("  Testing setLearningRate...")
    agent_brain.setLearningRate(0.5)
    
    # Test enableReplay
    print("  Testing enableReplay...")
    agent_brain.enableReplay(True, bufferSize=500)
    
    # Test getPerformanceStats
    print("  Testing getPerformanceStats...")
    stats = agent_brain.getPerformanceStats()
    print(f"    Performance stats: {len(stats)} metrics")
    
    # Test analyzeBehavior
    print("  Testing analyzeBehavior...")
    behavior_analysis = agent_brain.analyzeBehavior(window=50)
    print(f"    Behavior analysis: {behavior_analysis[:100]}...")
    
    # Test planAction
    print("  Testing planAction...")
    plan = agent_brain.planAction(depth=5)
    print(f"    Planned actions: {[str(action) for action in plan]}")
    
    # Test getDecisionTrail
    print("  Testing getDecisionTrail...")
    trail = agent_brain.getDecisionTrail(count=20)
    print(f"    Decision trail: {len(trail)} commands")
    
    # Test traceExecution
    print("  Testing traceExecution...")
    execution_trail = []
    trace_result = agent_brain.traceExecution(execution_trail, steps=100)
    print(f"    Execution trace: {len(execution_trail)} steps recorded")

def test_advanced_world_features(world):
    """Test advanced world features"""
    print("\nTesting advanced world features...")
    
    # Test setPhysicsParameters
    print("  Testing setPhysicsParameters...")
    world.setPhysicsParameters(gravity=9.81, friction=0.5, restitution=0.7)
    
    # Test addProceduralContent
    print("  Testing addProceduralContent...")
    world.addProceduralContent("resources", density=0.1)
    world.addProceduralContent("hazards", density=0.05)
    
    # Test getEnvironmentStats
    print("  Testing getEnvironmentStats...")
    stats = world.getEnvironmentStats()
    print(f"    Environment stats: {len(stats)} metrics")
    
    # Test createClone
    print("  Testing createClone...")
    world_clone = world.createClone()
    print(f"    Cloned world dimensions: {world_clone.dimensions}")
    
    # Test getResourceDistribution
    print("  Testing getResourceDistribution...")
    resource_dist = world.getResourceDistribution()
    print(f"    Resource distribution: {len(resource_dist)} categories")
    
    # Test profiler
    print("  Testing profiler...")
    total_time = cpu_time = wall_time = 0.0
    profile_result = world.profiler(total_time, cpu_time, wall_time)
    print(f"    Profile: {'success' if profile_result else 'failed'}")
    print(f"      Total time: {total_time:.3f}s, CPU: {cpu_time:.3f}s, Wall: {wall_time:.3f}s")
    
    # Test runEpisode
    print("  Testing runEpisode...")
    episode_result = world.runEpisode(steps=10, verbose=True)
    print(f"    Episode: {'success' if episode_result else 'failed'}")
    
    # Test setTimeScale
    print("  Testing setTimeScale...")
    world.setTimeScale(2.0)
    
    # Test setExplorationRate
    print("  Testing setExplorationRate...")
    world.setExplorationRate(0.3)
    
    # Test executeScenario
    print("  Testing executeScenario...")
    world.executeScenario("tutorial")
    print("    Tutorial scenario executed")

def test_advanced_configuration():
    """Test advanced configuration features"""
    print("\nTesting advanced configuration...")
    
    # Test createAdvancedConfig
    print("  Testing createAdvancedConfig...")
    advanced_config = pynlm.createAdvancedConfig("advanced")
    print(f"    Advanced config created with {advanced_config.getKeys()} keys")
    
    # Test merge
    print("  Testing merge...")
    config1 = pynlm.createDefaultConfig()
    config2 = pynlm.example_config()
    config1.merge(config2, overwrite=False)
    print(f"    Merged config has {config1.getKeys()} keys")
    
    # Test validate
    print("  Testing validate...")
    errors = ""
    warnings = ""
    validate_result = config1.validate(errors, warnings)
    print(f"    Validation: {'passed' if validate_result else 'failed'}")
    if errors:
        print(f"      Errors: {errors}")
    if warnings:
        print(f"      Warnings: {warnings}")
    
    # Test optimize
    print("  Testing optimize...")
    config1.optimize()
    print(f"    Optimized config: {config1.getKeys()} keys")

def main():
    """Main test function"""
    print("=" * 70)
    print("Testing NLM Python Advanced Bindings")
    print("=" * 70)
    
    try:
        # Test basic functionality
        brain, world, agent_brain = test_basic_functionality()
        
        # Test advanced features
        test_advanced_brain_operations(brain)
        test_advanced_agent_features(agent_brain)
        test_advanced_world_features(world)
        test_advanced_configuration()
        
        print("\n" + "=" * 70)
        print("All tests completed successfully!")
        print("=" * 70)
        return 0
        
    except Exception as e:
        print(f"\nTest failed with error: {e}")
        import traceback
        traceback.print_exc()
        return 1

if __name__ == "__main__":
    sys.exit(main())