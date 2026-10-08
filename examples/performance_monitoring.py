#!/usr/bin/env python3
"""
NLM - Performance Monitoring and Profiling Examples

This example demonstrates NLM's performance monitoring and profiling capabilities,
including real-time monitoring, bottleneck analysis, and optimization recommendations.
"""

import pynlm

def demonstrate_performance_monitoring():
    """Demonstrate comprehensive performance monitoring."""
    print("=== NLM Performance Monitoring Example ===")
    
    # Create a brain for testing
    config = pynlm.createDefaultConfig()
    config.setInt("brain.neuron_count", 2000)
    config.setFloat("brain.synapse_density", 0.1)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Create world and agent for more realistic simulation
    world = pynlm.createSimpleWorld()
    world.configure(15, 15, 6, 6)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    agent.enableCuriosity(True)
    agent.enableRewardModulation(True)
    
    print("1. Getting initial performance statistics...")
    initial_stats = brain.getPerformanceStats()
    
    print(f"   - Neurons firing: {initial_stats['firing_neurons']}")
    print(f"   - Average firing rate: {initial_stats['average_firing_rate']:.2f} Hz")
    print(f"   - Working memory utilization: {initial_stats['working_memory_utilization']*100:.1f}%")
    print(f"   - Prediction accuracy: {initial_stats['prediction_accuracy']:.3f}")
    print(f"   - Curiosity level: {initial_stats['curiosity_level']:.3f}")
    print(f"   - Novelty level: {initial_stats['novelty_level']:.3f}")
    
    print("\n2. Running performance test...")
    performance_test = brain.runPerformanceTest(200)
    
    print(f"   - Performance score: {performance_test['performance_score']:.3f}/1.0")
    print(f"   - Stability score: {performance_test['stability_score']:.3f}/1.0")
    print(f"   - Firing rate: {performance_test['firing_rate_mean']:.2f} ± {performance_test['firing_rate_std']:.2f} Hz")
    print(f"   - Memory utilization: {performance_test['memory_utilization_mean']*100:.1f}% ± {performance_test['memory_utilization_std']*100:.1f}%")
    print(f"   - Prediction accuracy: {performance_test['prediction_accuracy_mean']:.3f} ± {performance_test['prediction_accuracy_std']:.3f}")
    
    print("\n3. Analyzing for optimization opportunities...")
    optimization_profile = brain.profileForOptimization()
    
    current_perf = optimization_profile['current_performance']
    improvements = optimization_profile['estimated_improvements']
    
    print(f"   - Current performance score: {current_perf['firing_rate']/10.0*0.3 + (1-abs(current_perf['memory_utilization']-0.5))*0.2 + current_perf['prediction_accuracy']*0.5:.3f}")
    print(f"   - Potential score after optimization: {improvements['potential_score_after_optimization']:.3f}")
    print(f"   - Expected improvement: {improvements['expected_improvement_percent']:.1f}%")
    
    print(f"\n   - Optimization priorities:")
    priorities = optimization_profile['optimization_priorities']
    for i, priority in enumerate(priorities):
        print(f"     {i+1}. {priority}")
    
    print("\n4. Identifying performance bottlenecks...")
    bottlenecks = brain.getBottleneckAnalysis()
    
    if bottlenecks:
        print(f"   - Found {len(bottlenecks)} potential bottlenecks:")
        for bottleneck, suggestion in bottlenecks.items():
            print(f"     - {bottleneck.replace('_', ' ').title()}: {suggestion}")
    else:
        print("   - No significant performance bottlenecks detected!")
    
    print("\n5. Getting time profiling estimates...")
    time_profile = brain.getTimeProfiler()
    
    print(f"   - Estimated time per step: {time_profile['time_per_step_ms']:.3f} ms")
    print(f"   - Neural dynamics: {time_profile['neural_dynamics']:.3f} ms")
    print(f"   - Plasticity updates: {time_profile['plasticity_updates']:.3f} ms")
    print(f"   - Memory operations: {time_profile['memory_operations']:.3f} ms")
    print(f"   - Prediction computation: {time_profile['prediction_computation']:.3f} ms")
    print(f"   - Cognition planning: {time_profile['cognition_planning']:.3f} ms")
    print(f"   - Estimated total time (1000 steps): {time_profile['estimated_total_time_ms']/1000:.1f} s")
    
    print("\n6. Getting resource usage estimates...")
    resource_usage = brain.getResourceUsage()
    
    print(f"   - Estimated memory usage: {resource_usage['estimated_memory_mb']:.1f} MB")
    print(f"   - Memory per neuron: {resource_usage['memory_per_neuron_bytes']:.0f} bytes")
    print(f"   - Memory per synapse: {resource_usage['memory_per_synapse_bytes']:.0f} bytes")
    print(f"   - Estimated CPU time per step: {resource_usage['estimated_cpu_time_per_step_ms']:.3f} ms")
    print(f"   - Checkpoint size estimate: {resource_usage['checkpoint_size_estimate_mb']:.1f} MB")
    
    return {
        'brain': brain,
        'world': world,
        'agent': agent,
        'performance_stats': performance_test,
        'optimization_profile': optimization_profile,
        'bottlenecks': bottlenecks,
        'time_profile': time_profile,
        'resource_usage': resource_usage
    }

def demonstrate_brain_comparison():
    """Demonstrate comparing performance across multiple brain configurations."""
    print("\n=== Brain Performance Comparison Example ===")
    
    # Create different brain configurations
    configs = [
        ("Small Brain", pynlm.createSmallBrainConfig()),
        ("Standard Brain", pynlm.createDefaultConfig()),
        ("Large Brain", pynlm.createLargeBrainConfig())
    ]
    
    brains = []
    print("Creating different brain configurations...")
    
    for name, config in configs:
        brain = pynlm.createBrain(config)
        brain.initialize()
        brains.append((name, brain))
        print(f"   - {name}: {config.getIntOr('brain.neuron_count', 0)} neurons")
    
    # Run performance comparison
    comparison_results = brains[0][1].comparePerformance(
        [brain for _, brain in brains],
        "Configuration Comparison Test"
    )
    
    print("\nPerformance comparison results:")
    
    metric_comparison = comparison_results['metric_comparison']
    best_configs = comparison_results['best_configurations']
    
    # Compare key metrics
    print("\n   - Firing Rate:")
    for name, value in metric_comparison['firing_rate'].items():
        if name in ['min', 'max', 'average', 'coefficient_of_variation']:
            print(f"     {name.title()}: {value:.3f}")
    
    print("\n   - Prediction Accuracy:")
    for name, value in metric_comparison['prediction_accuracy'].items():
        if name in ['min', 'max', 'average', 'coefficient_of_variation']:
            print(f"     {name.title()}: {value:.3f}")
    
    print("\n   - Curiosity Level:")
    for name, value in metric_comparison['curiosity_level'].items():
        if name in ['min', 'max', 'average', 'coefficient_of_variation']:
            print(f"     {name.title()}: {value:.3f}")
    
    print("\n   - Memory Utilization:")
    for name, value in metric_comparison['memory_utilization'].items():
        if name in ['min', 'max', 'average', 'coefficient_of_variation']:
            print(f"     {name.title()}: {value*100:.1f}%")
    
    print("\n   - Best Configurations:")
    for brain_id, config in best_configs.items():
        name = configs[int(brain_id)][0]
        print(f"     {name}: Score={config['score']:.3f}, "
              f"Firing={config['firing_rate']:.2f}, "
              f"Accuracy={config['prediction_accuracy']:.3f}, "
              f"Curiosity={config['curiosity_level']:.3f}, "
              f"Memory={config['memory_utilization']*100:.1f}%")
    
    return {
        'configs': configs,
        'brains': brains,
        'comparison_results': comparison_results
    }

def demonstrate_real_time_monitoring():
    """Demonstrate real-time monitoring during simulation."""
    print("\n=== Real-Time Monitoring Example ===")
    
    # Create brain and agent
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(10, 10, 4, 4)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    agent.enableCuriosity(True)
    
    print("Monitoring simulation in real-time...")
    print("Step | Firing Rate | Memory Usage | Prediction Accuracy | Curiosity")
    print("-----|-------------|--------------|---------------------|----------")
    
    # Run simulation with real-time monitoring
    for step in range(50):
        world.update(0.1)
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Get real-time performance data
        stats = brain.getPerformanceStats()
        
        if step % 10 == 0:
            print(f"{step:4d} | {stats['average_firing_rate']:11.2f} | "
                  f"{stats['working_memory_usage']:14d}/{stats['working_memory_capacity']:4d} | "
                  f"{stats['prediction_accuracy']:19.3f} | "
                  f"{stats['curiosity_level']:9.3f}")
    
    # Final analysis
    print("\nReal-time monitoring complete!")
    final_stats = brain.getPerformanceStats()
    bottlenecks = brain.getBottleneckAnalysis()
    
    print(f"\nFinal system health:")
    print(f"  - Stable firing rate: {final_stats['average_firing_rate']:.2f} Hz")
    print(f"  - Memory utilization: {final_stats['working_memory_utilization']*100:.1f}%")
    print(f"  - Prediction accuracy: {final_stats['prediction_accuracy']:.3f}")
    print(f"  - Curiosity level: {final_stats['curiosity_level']:.3f}")
    
    if bottlenecks:
        print(f"\nPerformance issues detected: {len(bottlenecks)}")
        for bottleneck in bottlenecks:
            print(f"  - {bottleneck.replace('_', ' ').title()}")
    else:
        print("\n✓ No performance issues detected!")
    
    return {
        'brain': brain,
        'world': world,
        'agent': agent,
        'final_stats': final_stats,
        'bottlenecks': bottlenecks
    }

def main():
    """Main function to run the performance monitoring examples."""
    print("NLM Performance Monitoring and Profiling Examples")
    print("=" * 60)
    print("This example demonstrates NLM's advanced performance monitoring")
    print("capabilities including real-time monitoring, bottleneck analysis,")
    print("and optimization recommendations.\n")
    
    # Run all performance monitoring examples
    print("Running Performance Monitoring Example...")
    perf_results = demonstrate_performance_monitoring()
    
    print("\nRunning Brain Comparison Example...")
    comparison_results = demonstrate_brain_comparison()
    
    print("\nRunning Real-Time Monitoring Example...")
    realtime_results = demonstrate_real_time_monitoring()
    
    print("\n" + "=" * 60)
    print("Performance Monitoring Examples Complete!")
    print("\nKey features demonstrated:")
    print("  ✓ Real-time performance monitoring and statistics")
    print("  ✓ Comprehensive bottleneck analysis")
    print("  ✓ Optimization recommendations and impact estimation")
    print("  ✓ Time profiling and resource usage estimation")
    print("  ✓ Performance comparison across configurations")
    print("  ✓ Stability analysis and scoring")
    print("  ✓ Real-time monitoring during simulation")
    
    print(f"\nPerformance monitoring capabilities provide:")
    print("  • Early detection of performance issues")
    print("  • Optimization recommendations with expected impact")
    print("  • Comparative analysis across configurations")
    print("  • Real-time monitoring during simulations")
    print("  • Resource usage estimation and planning")
    
    return {
        'performance_monitoring': perf_results,
        'brain_comparison': comparison_results,
        'real_time_monitoring': realtime_results
    }

if __name__ == "__main__":
    main()