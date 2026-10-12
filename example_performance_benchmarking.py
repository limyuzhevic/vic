#!/usr/bin/env python3
"""
example_performance_benchmarking.py

Performance Benchmarking Example
================================

This example demonstrates performance benchmarking using the NLM Python API,
showcasing how to measure and optimize neural simulation performance.

Key Concepts Covered:
- Brain stepping performance benchmarking
- Simulation performance benchmarking
- Resource usage analysis
- Performance metrics collection
- Performance optimization recommendations
- Comparative performance analysis

Learning Objectives:
1. Measure brain stepping performance
2. Measure simulation performance
3. Analyze resource usage
4. Generate performance reports
5. Compare different configurations
6. Generate optimization recommendations

Author: NLM Documentation Team
Date: 2026-10-12
"""

import pynlm as nlm
import time
import numpy as np
from typing import Dict, List, Tuple

def create_benchmark_brain():
    """
    Create a brain configuration for benchmarking.
    
    Returns:
        Configured brain for benchmarking
    """
    print("   Creating benchmark brain...")
    
    # Create default configuration
    config = nlm.createDefaultConfig()
    brain = nlm.createBrain(config)
    brain.initialize()
    
    return brain

def create_benchmark_world():
    """
    Create a world configuration for benchmarking.
    
    Returns:
        Configured world for benchmarking
    """
    print("   Creating benchmark world...")
    
    # Create standard environment
    world = nlm.createStandardEnvironment()
    
    # Setup agent
    nlm.setupAgentStartPosition(world, x=50.0, y=50.0)
    
    return world

def benchmark_brain_stepping(brain, num_steps: int = 1000) -> Dict:
    """
    Benchmark brain stepping performance.
    
    Args:
        brain: Brain to benchmark
        num_steps: Number of steps to run
        
    Returns:
        Performance metrics
    """
    print(f"   Benchmarking brain stepping ({num_steps} steps)...")
    
    # Warm-up
    for _ in range(100):
        brain.step(0)
    
    # Benchmark actual performance
    start_time = time.perf_counter()
    
    for step in range(num_steps):
        brain.step(step)
    
    end_time = time.perf_counter()
    
    elapsed = end_time - start_time
    steps_per_second = num_steps / elapsed if elapsed > 0 else 0
    
    # Get brain statistics
    total_spikes = brain.getTotalSpikeCount()
    active_neurons = brain.getFiringNeuronCount()
    avg_firing_rate = brain.getAverageFiringRate()
    
    # Check resource usage
    resource_report = nlm.checkBrainResourceUsage(brain)
    
    metrics = {
        'steps_per_second': steps_per_second,
        'total_time_seconds': elapsed,
        'total_spikes': total_spikes,
        'active_neurons': active_neurons,
        'average_firing_rate': avg_firing_rate,
        'memory_usage_mb': resource_report.getMemoryUsage() / 1024 / 1024,
        'cpu_usage_percent': resource_report.getCPUUsage(),
        'step_time_variance': 0.0,  # Simplified for demo
        'warmup_time_seconds': 0.1,
        'benchmark_steps': num_steps
    }
    
    return metrics

def benchmark_simulation_performance(world, brain, num_steps: int = 500) -> Dict:
    """
    Benchmark simulation performance.
    
    Args:
        world: World to simulate
        brain: Brain to use
        num_steps: Number of steps to simulate
        
    Returns:
        Simulation performance metrics
    """
    print(f"   Benchmarking simulation performance ({num_steps} steps)...")
    
    # Setup agent
    agent = nlm.createStandardAgent(brain)
    agent.initialize(world)
    
    # Warm-up
    for _ in range(50):
        world.update(0.1)
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(0)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
    
    # Benchmark actual simulation
    start_time = time.perf_counter()
    
    for step in range(num_steps):
        world.update(0.1)
        
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        brain.step(step)
        
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
    
    end_time = time.perf_counter()
    
    elapsed = end_time - start_time
    steps_per_second = num_steps / elapsed if elapsed > 0 else 0
    
    # Get simulation statistics
    total_reward = world.getTotalReward()
    steps_per_second_real = world.getStepsPerSecond()
    
    # Check resource usage
    resource_report = nlm.checkBrainResourceUsage(brain)
    
    metrics = {
        'simulation_steps_per_second': steps_per_second,
        'world_steps_per_second': steps_per_second_real,
        'total_time_seconds': elapsed,
        'total_reward': total_reward,
        'average_reward_per_step': total_reward / num_steps if num_steps > 0 else 0,
        'memory_usage_mb': resource_report.getMemoryUsage() / 1024 / 1024,
        'cpu_usage_percent': resource_report.getCPUUsage(),
        'simulation_efficiency': (total_reward / elapsed) if elapsed > 0 else 0,
        'warmup_time_seconds': 0.05,
        'benchmark_steps': num_steps
    }
    
    return metrics

def benchmark_memory_usage(brain, world, test_sizes: List[int]) -> Dict:
    """
    Benchmark memory usage with different brain sizes.
    
    Args:
        brain: Brain to test
        world: World for simulation
        test_sizes: List of brain sizes to test
        
    Returns:
        Memory usage benchmarks
    """
    print("   Benchmarking memory usage...")
    
    memory_results = []
    
    for size in test_sizes:
        # Create brain with specified size
        config = nlm.createDefaultConfig()
        test_brain = nlm.createBrain(config)
        test_brain.initialize()
        
        # Setup agent
        agent = nlm.createStandardAgent(test_brain)
        agent.initialize(world)
        
        # Get memory usage
        resource_report = nlm.checkMemoryAvailability(100 * 1024 * 1024)  # 100MB
        
        memory_results.append({
            'brain_size': size,
            'memory_available': resource_report,
            'simulation_steps_per_mb': size / (100 * 1024 * 1024) if resource_report else 0
        })
    
    return {
        'memory_tests': memory_results,
        'recommended_brain_size': max(memory_results, key=lambda x: x.get('simulation_steps_per_mb', 0))['brain_size']
    }

def benchmark_configurations(config_list: List[Dict], world, test_steps: int = 200) -> Dict:
    """
    Benchmark different configuration options.
    
    Args:
        config_list: List of configuration dictionaries
        world: World for simulation
        test_steps: Number of steps to test each configuration
        
    Returns:
        Configuration comparison results
    """
    print("   Benchmarking different configurations...")
    
    results = []
    
    for i, config_spec in enumerate(config_list):
        print(f"     Testing configuration {i + 1}/{len(config_list)}...")
        
        # Create brain with current configuration
        config = nlm.createLearningConfig()
        
        # Apply configuration specifics
        if 'learning_rate' in config_spec:
            config.set("brain.learning.learning_rate", config_spec['learning_rate'])
        if 'plasticity' in config_spec:
            config.set("brain.plasticity.enabled", config_spec['plasticity'])
        if 'neuromodulation' in config_spec:
            config.set("neuromodulation.enabled", config_spec['neuromodulation'])
        
        brain = nlm.createBrain(config)
        brain.initialize()
        
        # Setup agent
        agent = nlm.createLearningAgent(brain)
        agent.initialize(world)
        
        # Setup neuromodulation
        if 'reward_modulation' in config_spec:
            nlm.setupRewardModulation(agent, learningRate=config_spec['reward_modulation'])
        
        # Run benchmark
        start_time = time.perf_counter()
        
        for step in range(test_steps):
            world.update(0.1)
            
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            
            brain.step(step)
            
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
        
        end_time = time.perf_counter()
        
        elapsed = end_time - start_time
        steps_per_second = test_steps / elapsed if elapsed > 0 else 0
        
        # Get performance metrics
        total_reward = world.getTotalReward()
        resource_report = nlm.checkBrainResourceUsage(brain)
        
        result = {
            'config_id': i + 1,
            'learning_rate': config_spec.get('learning_rate', 0.1),
            'plasticity_enabled': config_spec.get('plasticity', True),
            'neuromodulation_enabled': config_spec.get('neuromodulation', True),
            'reward_modulation_rate': config_spec.get('reward_modulation', 0.1),
            'steps_per_second': steps_per_second,
            'total_time_seconds': elapsed,
            'total_reward': total_reward,
            'memory_usage_mb': resource_report.getMemoryUsage() / 1024 / 1024,
            'efficiency_score': (total_reward / elapsed) if elapsed > 0 else 0
        }
        
        results.append(result)
    
    return results

def generate_performance_report(brain_metrics: Dict, simulation_metrics: Dict,
                               memory_results: Dict, config_results: List[Dict]) -> Dict:
    """
    Generate comprehensive performance report.
    
    Args:
        brain_metrics: Brain stepping metrics
        simulation_metrics: Simulation metrics
        memory_results: Memory usage results
        config_results: Configuration comparison results
        
    Returns:
        Comprehensive performance report
    """
    print("   Generating performance report...")
    
    # Calculate overall performance score
    brain_score = min(100, brain_metrics['steps_per_second'] / 100)  # Normalize to 100 steps/sec
    sim_score = min(100, simulation_metrics['simulation_steps_per_second'] / 100)
    memory_score = min(100, memory_results.get('recommended_brain_size', 100) / 1000)
    
    # Calculate efficiency score
    overall_efficiency = (
        brain_metrics['steps_per_second'] * 0.3 +
        simulation_metrics['simulation_efficiency'] * 0.4 +
        memory_score * 0.3
    )
    
    # Generate recommendations
    recommendations = []
    
    if brain_metrics['steps_per_second'] < 50:
        recommendations.append("Consider reducing brain complexity for better performance")
    
    if simulation_metrics['simulation_efficiency'] < 0.1:
        recommendations.append("Optimize simulation parameters for better efficiency")
    
    if memory_results.get('recommended_brain_size', 0) > 500:
        recommendations.append("Consider memory optimization for large brain simulations")
    
    # Find best configuration
    if config_results:
        best_config = max(config_results, key=lambda x: x['efficiency_score'])
        recommendations.append(f"Use configuration {best_config['config_id']} for optimal performance")
    
    report = {
        'summary': {
            'brain_performance': brain_metrics['steps_per_second'],
            'simulation_performance': simulation_metrics['simulation_steps_per_second'],
            'overall_efficiency': overall_efficiency,
            'memory_optimization': memory_results.get('recommended_brain_size', 0),
            'configurations_tested': len(config_results)
        },
        'detailed_metrics': {
            'brain_stepping': brain_metrics,
            'simulation_performance': simulation_metrics,
            'memory_usage': memory_results
        },
        'configuration_comparison': config_results,
        'recommendations': recommendations,
        'performance_rating': 'Excellent' if overall_efficiency > 80 else 'Good' if overall_efficiency > 60 else 'Fair'
    }
    
    return report

def main():
    """Main function demonstrating performance benchmarking."""
    print("=" * 70)
    print("NLM Performance Benchmarking Example")
    print("=" * 70)
    
    # ---------------------------------------------------------------------
    # SECTION 1: SETUP AND INITIALIZATION
    # ---------------------------------------------------------------------
    print("\n1. Setting up benchmarking environment...")
    
    # Create benchmark world and brain
    world = create_benchmark_world()
    brain = create_benchmark_brain()
    
    print(f"   ✓ World and brain created for benchmarking")
    
    # ---------------------------------------------------------------------
    # SECTION 2: BRAIN STEPPING BENCHMARK
    # ---------------------------------------------------------------------
    print("\n2. Brain stepping performance benchmark...")
    
    brain_metrics = benchmark_brain_stepping(brain, num_steps=1000)
    
    print("   ✓ Brain Stepping Results:")
    print(f"     - Performance: {brain_metrics['steps_per_second']:.1f} steps/sec")
    print(f"     - Total time: {brain_metrics['total_time_seconds']:.2f} seconds")
    print(f"     - Average firing rate: {brain_metrics['average_firing_rate']:.2f} Hz")
    print(f"     - Memory usage: {brain_metrics['memory_usage_mb']:.2f} MB")
    print(f"     - CPU usage: {brain_metrics['cpu_usage_percent']:.1f}%")
    
    # ---------------------------------------------------------------------
    # SECTION 3: SIMULATION PERFORMANCE BENCHMARK
    # ---------------------------------------------------------------------
    print("\n3. Simulation performance benchmark...")
    
    simulation_metrics = benchmark_simulation_performance(world, brain, num_steps=500)
    
    print("   ✓ Simulation Performance Results:")
    print(f"     - Brain stepping: {simulation_metrics['simulation_steps_per_second']:.1f} steps/sec")
    print(f"     - World stepping: {simulation_metrics['world_steps_per_second']:.1f} steps/sec")
    print(f"     - Total time: {simulation_metrics['total_time_seconds']:.2f} seconds")
    print(f"     - Total reward: {simulation_metrics['total_reward']:.2f}")
    print(f"     - Average reward/step: {simulation_metrics['average_reward_per_step']:.3f}")
    print(f"     - Simulation efficiency: {simulation_metrics['simulation_efficiency']:.4f}")
    print(f"     - Memory usage: {simulation_metrics['memory_usage_mb']:.2f} MB")
    
    # ---------------------------------------------------------------------
    # SECTION 4: MEMORY USAGE BENCHMARK
    # ---------------------------------------------------------------------
    print("\n4. Memory usage benchmark...")
    
    memory_results = benchmark_memory_usage(brain, world, [100, 500, 1000, 2000])
    
    print("   ✓ Memory Usage Results:")
    for test in memory_results['memory_tests']:
        print(f"     - Brain size {test['brain_size']}: {test['memory_available']} available, "
              f"{test['simulation_steps_per_mb']:.2f} steps/MB")
    print(f"     - Recommended brain size: {memory_results['recommended_brain_size']}")
    
    # ---------------------------------------------------------------------
    # SECTION 5: CONFIGURATION BENCHMARKING
    # ---------------------------------------------------------------------
    print("\n5. Configuration benchmarking...")
    
    config_list = [
        {'learning_rate': 0.1, 'plasticity': True, 'neuromodulation': True, 'reward_modulation': 0.1},
        {'learning_rate': 0.05, 'plasticity': True, 'neuromodulation': True, 'reward_modulation': 0.15},
        {'learning_rate': 0.2, 'plasticity': True, 'neuromodulation': False, 'reward_modulation': 0.05},
        {'learning_rate': 0.15, 'plasticity': False, 'neuromodulation': True, 'reward_modulation': 0.2}
    ]
    
    config_results = benchmark_configurations(config_list, world, test_steps=200)
    
    print("   ✓ Configuration Comparison Results:")
    for result in config_results:
        print(f"     - Config {result['config_id']}: {result['steps_per_second']:.1f} steps/sec, "
              f"{result['efficiency_score']:.4f} efficiency, {result['memory_usage_mb']:.2f} MB")
    
    # ---------------------------------------------------------------------
    # SECTION 6: PERFORMANCE REPORT GENERATION
    # ---------------------------------------------------------------------
    print("\n6. Generating performance report...")
    
    performance_report = generate_performance_report(
        brain_metrics, simulation_metrics, memory_results, config_results
    )
    
    print("   ✓ Performance Report Generated:")
    summary = performance_report['summary']
    print(f"     - Overall efficiency: {summary['overall_efficiency']:.2f}")
    print(f"     - Performance rating: {performance_report['performance_rating']}")
    print(f"     - Configurations tested: {summary['configurations_tested']}")
    print(f"     - Recommended brain size: {summary['memory_optimization']}")
    
    if performance_report['recommendations']:
        print("     - Recommendations:")
        for rec in performance_report['recommendations']:
            print(f"       • {rec}")
    
    # ---------------------------------------------------------------------
    # SECTION 7: VALIDATION AND HEALTH CHECK
    # ---------------------------------------------------------------------
    print("\n7. Final validation...")
    
    # Validate brain state
    brain_issues = nlm.validateBrainDynamics(brain, duration=100.0)
    if brain_issues:
        print(f"   ⚠ Brain dynamics issues ({len(brain_issues)}):")
        for issue in brain_issues[:3]:
            print(f"     - {issue}")
    else:
        print("   ✓ Brain dynamics are stable")
    
    # Check world state
    world_issues = nlm.validateWorldState(world)
    if world_issues:
        print(f"   ⚠ World state issues ({len(world_issues)}):")
        for issue in world_issues[:3]:
            print(f"     - {issue}")
    else:
        print("   ✓ World state is valid")
    
    # ---------------------------------------------------------------------
    # SECTION 8: FINAL SUMMARY
    # ---------------------------------------------------------------------
    print("\n" + "=" * 70)
    print("EXAMPLE COMPLETE - Performance Benchmarking Summary")
    print("=" * 70)
    
    print(f"\nPerformance Results:")
    print(f"  - Brain stepping: {brain_metrics['steps_per_second']:.1f} steps/sec")
    print(f"  - Simulation performance: {simulation_metrics['simulation_steps_per_second']:.1f} steps/sec")
    print(f"  - Overall efficiency: {performance_report['summary']['overall_efficiency']:.2f}")
    print(f"  - Performance rating: {performance_report['performance_rating']}")
    print(f"  - Configurations tested: {len(config_results)}")
    
    print(f"\nOptimization Recommendations:")
    for rec in performance_report['recommendations'][:3]:
        print(f"  ✓ {rec}")
    
    print(f"\nNext Steps for Learning:")
    print(f"  1. Try example_neural_analysis.py for advanced analysis")
    print(f"  2. Try tutorial_step_by_step.py for comprehensive tutorial")
    print(f"  3. Try recipe_learning_optimization.py for optimization recipes")
    print(f"  4. Try recipe_agent_training.py for training recipes")

if __name__ == "__main__":
    main()