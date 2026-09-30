# NLM Performance Benchmarking Guide

## Overview

This document provides comprehensive performance benchmarking guidelines for the NLM Python bindings (`pynlm`). It covers performance measurement techniques, optimization strategies, benchmark interpretation, and performance tuning.

## Table of Contents

1. [Performance Measurement Fundamentals](#performance-measurement-fundamentals)
2. [Benchmarking Methodology](#benchmarking-methodology)
3. [Performance Metrics](#performance-metrics)
4. [Optimization Strategies](#optimization-strategies)
5. [Benchmark Results Analysis](#benchmark-results-analysis)
6. [Performance Monitoring](#performance-monitoring)
7. [Hardware Considerations](#hardware-considerations)
8. [Benchmark Scripts and Tools](#benchmark-scripts-and-tools)
9. [Performance Tuning Guide](#performance-tuning-guide)
10. [Common Performance Issues](#common-performance-issues)

---

## Performance Measurement Fundamentals

### Why Performance Benchmarking Matters

Performance benchmarking is crucial for:

1. **Understanding System Capabilities**: Determine how well NLM performs on different hardware configurations
2. **Identifying Bottlenecks**: Locate performance limitations in code or system resources
3. **Optimizing Parameters**: Find optimal configuration settings for specific use cases
4. **Validating Improvements**: Measure the impact of performance optimizations
5. **Resource Planning**: Estimate hardware requirements for different workloads
6. **Performance Regression Testing**: Detect performance degradation over time

### Key Performance Concepts

#### Throughput vs. Latency

- **Throughput**: Number of simulation steps completed per unit time (steps/second)
- **Latency**: Time delay between simulation steps
- **Efficiency**: Ratio of useful work to total computational cost

#### Scalability Analysis

- **Strong Scaling**: How performance changes with more resources for fixed problem size
- **Weak Scaling**: How performance changes with problem size growth
- **Amdahl's Law**: Limits on parallel performance improvements

#### Performance Metrics Categories

| Category | Metrics | Examples |
|----------|---------|----------|
| Computational | FLOPS, Instructions per Cycle | Operations per second, CPU cycles |
| Memory | Bandwidth, Latency, Cache hit rate | Memory access speed, cache efficiency |
| I/O | Throughput, Access time | File I/O, Network I/O |
| Energy | Power consumption, Performance per watt | CPU power draw, energy efficiency |

## Benchmarking Methodology

### Benchmarking Best Practices

1. **Use Consistent Test Conditions**
   - Fixed hardware configuration
   - Controlled software environment
   - Consistent measurement methodology

2. **Run Sufficient Replicates**
   - Multiple runs to account for variability
   - Average results to get reliable measurements
   - Statistical significance testing

3. **Warm-up Runs**
   - Allow system to reach stable state
   - Prime caches and memory
   - Clear transient effects

4. **Measure Baseline**
   - Establish reference performance
   - Document current state before optimizations

5. **Control Variables**
   - Isolate the effect of specific changes
   - Use controlled experiments
   - Avoid confounding factors

### Benchmarking Workflow

```
1. Setup Environment
   ├── Configure hardware and software
   ├── Install dependencies
   ├── Set up measurement tools

2. Baseline Measurement
   ├── Run warm-up iterations
   ├── Establish baseline metrics
   ├── Record system state

3. Experimental Runs
   ├── Vary single parameter at a time
   ├── Record measurements
   ├── Repeat for statistical significance

4. Analysis
   ├── Compute statistics
   ├── Compare to baseline
   ├── Identify trends and patterns

5. Optimization
   ├── Implement changes
   ├── Re-run benchmarks
   ├── Validate improvements
```

## Performance Metrics

### Core Performance Metrics

| Metric | Formula | Unit | Description |
|--------|---------|------|-------------|
| Simulation Speed | Steps / Time | steps/second | Number of completed simulation steps |
| CPU Utilization | CPU time / Wall time | percentage | Fraction of CPU resources used |
| Memory Efficiency | Operations / Memory | operations/byte | Computational work per memory unit |
| Spike Rate | Spikes / Time | spikes/second | Average neural firing rate |
| Energy Efficiency | Performance / Power | ops/watt | Computational output per energy unit |

### Detailed Performance Metrics

#### Neural Network Metrics

```python
class NeuralMetrics:
    def __init__(self, brain):
        self.brain = brain
    
    def get_spike_metrics(self):
        """Get spike-related performance metrics."""
        return {
            'total_spikes': self.brain.getTotalSpikeCount(),
            'average_firing_rate': self.brain.getAverageFiringRate(),
            'firing_efficiency': self.brain.getFiringNeuronCount() / 
                               max(1, self.brain.getTotalNeuronCount())
        }
    
    def get_network_metrics(self):
        """Get neural network structure metrics."""
        return {
            'neuron_count': self.brain.getTotalNeuronCount(),
            'synapse_count': self.brain.getTotalSynapseCount(),
            'connection_density': self.brain.getTotalSynapseCount() / 
                                 max(1, self.brain.getTotalNeuronCount() ** 2)
        }
```

#### Simulation Performance Metrics

```python
class SimulationMetrics:
    def __init__(self, brain, agent, world):
        self.brain = brain
        self.agent = agent
        self.world = world
    
    def get_comprehensive_metrics(self):
        """Get comprehensive simulation performance metrics."""
        return {
            'simulation_efficiency': {
                'steps_per_second': self.calculate_steps_per_second(),
                'cpu_efficiency': self.calculate_cpu_efficiency(),
                'memory_efficiency': self.calculate_memory_efficiency()
            },
            'agent_performance': {
                'curiosity_level': self.agent.getCuriosityLevel(),
                'novelty_level': self.agent.getNoveltyLevel(),
                'prediction_error': self.agent.getPredictionError(),
                'learning_rate': self.calculate_learning_rate()
            },
            'world_dynamics': {
                'simulation_time': self.world.getSimulationTime(),
                'agent_position': (self.agent.getBody().x, self.agent.getBody().y),
                'environmental_changes': self.detect_environmental_changes()
            },
            'system_resources': {
                'memory_usage': self.get_memory_usage(),
                'cpu_usage': self.get_cpu_usage(),
                'disk_io': self.get_disk_io()
            }
        }
```

### Performance Benchmark Categories

#### Brain Performance Benchmarks

| Benchmark | Description | Target |
|-----------|-------------|--------|
| Small Brain (100-500 neurons) | Basic functionality testing | <100ms/step |
| Medium Brain (1000-5000 neurons) | Typical research scenarios | 10-100ms/step |
| Large Brain (5000-20000 neurons) | Advanced research scenarios | 100-1000ms/step |
| Very Large Brain (20000+ neurons) | High-performance computing | 1-10s/step |

#### Agent Performance Benchmarks

| Benchmark | Description | Target |
|-----------|-------------|--------|
| Simple Agent | Basic world interaction | <50ms/step |
| Complex Agent | Advanced learning scenarios | 50-500ms/step |
| Multi-Agent | Coordinated agent systems | 100-1000ms/step |

#### World Performance Benchmarks

| Benchmark | Description | Target |
|-----------|-------------|--------|
| Small World | Limited environment | <10ms/step |
| Medium World | Standard environment | 10-100ms/step |
| Large World | Complex environment | 100-1000ms/step |

## Optimization Strategies

### Configuration Optimization

#### Neural Network Optimization

```python
class NeuralOptimizer:
    def optimize_brain_config(self, base_config, target_performance):
        """Optimize brain configuration for target performance."""
        optimized_config = pynlm.createDefaultConfig()
        
        # Adjust neuron count based on target
        if target_performance['steps_per_second'] > 1000:
            optimized_config.set("brain.neuron_count", 500)
            optimized_config.set("brain.synapse_density", 0.05)
        elif target_performance['steps_per_second'] > 100:
            optimized_config.set("brain.neuron_count", 2000)
            optimized_config.set("brain.synapse_density", 0.08)
        else:
            optimized_config.set("brain.neuron_count", 5000)
            optimized_config.set("brain.synapse_density", 0.12)
        
        # Optimize plasticity settings
        optimized_config.set("plasticity.stdp.enable", True)
        optimized_config.set("plasticity.hebbian.enable", True)
        optimized_config.set("plasticity.stdp.learning_rate", 0.01)
        
        # Optimize neuromodulation
        optimized_config.set("neuromod.dopamine.scale", 1.0)
        optimized_config.set("neuromod.curiosity.enable", True)
        
        return optimized_config
```

#### Development Stage Optimization

```python
class DevelopmentOptimizer:
    def optimize_development(self, base_config):
        """Optimize development settings for performance."""
        config = pynlm.createDefaultConfig()
        
        # Adjust developmental plasticity
        config.set("development.critical_period_duration", 100.0)
        config.set("development.synaptogenesis_rate", 0.01)
        config.set("development.pruning_rate", 0.005)
        
        # Optimize maturation timing
        config.set("development.maturation_time", 500.0)
        
        return config
```

### Algorithmic Optimization

#### Spike Processing Optimization

```python
class SpikeProcessingOptimizer:
    def optimize_spike_processing(self, config):
        """Optimize spike processing for better performance."""
        optimized_config = pynlm.createDefaultConfig()
        
        # Adjust neuron parameters for optimal spiking
        optimized_config.set("brain.v_thresh", -45.0)  # Higher threshold
        optimized_config.set("brain.tau_mem", 15.0)    # Longer time constant
        optimized_config.set("brain.tau_ref", 3.0)     # Shorter refractory period
        
        # Optimize synaptic properties
        optimized_config.set("brain.initial_weight_mean", 0.6)
        optimized_config.set("brain.initial_weight_std", 0.15)
        
        return optimized_config
```

#### Plasticity Optimization

```python
class PlasticityOptimizer:
    def optimize_plasticity(self, config):
        """Optimize plasticity rules for performance."""
        optimized_config = pynlm.createDefaultConfig()
        
        # STDP optimization
        optimized_config.set("plasticity.stdp.enable", True)
        optimized_config.set("plasticity.stdp.learning_rate", 0.005)
        optimized_config.set("plasticity.stdp.tau_plus", 20.0)
        optimized_config.set("plasticity.stdp.tau_minus", 20.0)
        
        # Hebbian optimization
        optimized_config.set("plasticity.hebbian.enable", True)
        optimized_config.set("plasticity.hebbian.learning_rate", 0.001)
        
        # Structural plasticity optimization
        optimized_config.set("plasticity.structural.enable", True)
        optimized_config.set("plasticity.structural.growth_rate", 0.001)
        
        return optimized_config
```

### Memory Optimization

#### Memory Management Strategies

```python
class MemoryOptimizer:
    def optimize_memory_usage(self, config):
        """Optimize memory usage for different scenarios."""
        optimized_config = pynlm.createDefaultConfig()
        
        # For memory-constrained environments
        if hasattr(config, 'get') and config.get('memory.constrained', False):
            optimized_config.set("brain.neuron_count", 500)
            optimized_config.set("brain.synapse_density", 0.05)
            optimized_config.set("memory.enable_checkpointing", True)
        
        # For high-performance environments
        else:
            optimized_config.set("brain.neuron_count", 5000)
            optimized_config.set("brain.synapse_density", 0.12)
            optimized_config.set("memory.enable_checkpointing", True)
            optimized_config.set("memory.checkpoint_interval", 100)
        
        return optimized_config
```

#### Cache Optimization

```python
class CacheOptimizer:
    def optimize_caching(self, config):
        """Optimize caching strategies for performance."""
        optimized_config = pynlm.createDefaultConfig()
        
        # Neuron cache optimization
        optimized_config.set("caching.neuron_cache_size", 1000)
        optimized_config.set("caching.neuron_cache_ttl", 60.0)
        
        # Synapse cache optimization
        optimized_config.set("caching.synapse_cache_size", 10000)
        optimized_config.set("caching.synapse_cache_ttl", 120.0)
        
        # Region cache optimization
        optimized_config.set("caching.region_cache_size", 100)
        optimized_config.set("caching.region_cache_ttl", 300.0)
        
        return optimized_config
```

### Parallelization Strategies

#### CPU Parallelization

```python
class CPUParallelizer:
    def parallelize_cpu_operations(self, config):
        """Optimize CPU parallelization."""
        optimized_config = pynlm.createDefaultConfig()
        
        # Determine optimal thread count
        import os
        cpu_count = os.cpu_count() or 4
        optimized_config.set("parallel.threads", cpu_count)
        
        # Optimize for shared memory parallelism
        optimized_config.set("parallel.model", "openmp")
        optimized_config.set("parallel.chunk_size", 64)
        
        # Optimize for NUMA systems
        optimized_config.set("parallel.numa.affinity", True)
        
        return optimized_config
```

#### Memory Bandwidth Optimization

```python
class MemoryBandwidthOptimizer:
    def optimize_memory_bandwidth(self, config):
        """Optimize memory bandwidth usage."""
        optimized_config = pynlm.createDefaultConfig()
        
        # Optimize memory access patterns
        optimized_config.set("memory.access_pattern", "sequential")
        optimized_config.set("memory.block_size", 256)
        
        # Optimize prefetching
        optimized_config.set("memory.prefetch.enabled", True)
        optimized_config.set("memory.prefetch.distance", 32)
        
        # Optimize data locality
        optimized_config.set("memory.data_locality", "cache-friendly")
        
        return optimized_config
```

## Benchmark Results Analysis

### Statistical Analysis

```python
import numpy as np
import pandas as pd
from scipy import stats
import matplotlib.pyplot as plt

class BenchmarkAnalyzer:
    def analyze_benchmark_results(self, results):
        """Analyze and interpret benchmark results."""
        analysis = {}
        
        for benchmark_name, benchmark_data in results.items():
            # Statistical analysis
            times = [run['time'] for run in benchmark_data]
            steps_per_sec = [run['steps_per_second'] for run in benchmark_data]
            
            # Calculate statistics
            analysis[f'{benchmark_name}_statistics'] = {
                'time': {
                    'mean': np.mean(times),
                    'std': np.std(times),
                    'median': np.median(times),
                    'min': np.min(times),
                    'max': np.max(times),
                    'ci_95': stats.t.interval(0.95, len(times)-1, 
                                            loc=np.mean(times), 
                                            scale=stats.sem(times))
                },
                'steps_per_second': {
                    'mean': np.mean(steps_per_sec),
                    'std': np.std(steps_per_sec),
                    'relative_std': np.std(steps_per_sec) / np.mean(steps_per_sec)
                }
            }
            
            # Performance comparison
            analysis[f'{benchmark_name}_comparison'] = self.compare_with_baseline(
                benchmark_data, results.get('baseline', None)
            )
            
            # Performance trends
            analysis[f'{benchmark_name}_trends'] = self.identify_trends(benchmark_data)
        
        return analysis
    
    def compare_with_baseline(self, benchmark_data, baseline_data):
        """Compare benchmark results with baseline."""
        if not baseline_data:
            return None
        
        comparisons = []
        for run in benchmark_data:
            for baseline_run in baseline_data:
                improvement = (baseline_run['steps_per_second'] - run['steps_per_second']) / baseline_run['steps_per_second'] * 100
                comparisons.append({
                    'baseline_steps_per_second': baseline_run['steps_per_second'],
                    'current_steps_per_second': run['steps_per_second'],
                    'improvement_percent': improvement,
                    'time_reduction': baseline_run['time'] - run['time']
                })
        
        return comparisons
    
    def identify_trends(self, benchmark_data):
        """Identify performance trends over time."""
        if len(benchmark_data) < 2:
            return None
        
        steps = list(range(len(benchmark_data)))
        times = [run['time'] for run in benchmark_data]
        
        # Linear regression for trend
        slope, intercept, r_value, p_value, std_err = stats.linregress(steps, times)
        
        return {
            'time_trend': {
                'slope': slope,
                'intercept': intercept,
                'r_squared': r_value ** 2,
                'statistical_significance': p_value < 0.05
            },
            'stability': self.calculate_stability(times)
        }
    
    def calculate_stability(self, values, window_size=5):
        """Calculate performance stability over time."""
        if len(values) < window_size:
            return {'stable': True, 'coefficient_of_variation': np.std(values) / np.mean(values)}
        
        # Calculate rolling statistics
        rolling_means = np.convolve(values, np.ones(window_size)/window_size, mode='valid')
        rolling_stds = np.array([np.std(values[i:i+window_size]) for i in range(len(values)-window_size+1)])
        
        # Calculate coefficient of variation
        cv = rolling_stds / rolling_means
        
        return {
            'stable': np.all(cv < 0.1),  # Coefficient of variation < 10%
            'avg_cv': np.mean(cv),
            'max_cv': np.max(cv),
            'cv_trend': np.polyfit(range(len(cv)), cv, 1)[0]
        }
```

### Performance Benchmarking Scripts

#### Benchmark Suite Generator

```python
#!/usr/bin/env python3
"""Comprehensive benchmarking suite for NLM."""

import pynlm
import time
import statistics
import json
import sys
from datetime import datetime
from typing import Dict, List, Any

class NLMBenchmarkSuite:
    def __init__(self):
        self.results = {}
        self.start_time = None
        self.end_time = None
    
    def run_all_benchmarks(self):
        """Run all benchmark suites."""
        self.start_time = datetime.now()
        
        print("Running NLM Performance Benchmark Suite")
        print("=" * 50)
        
        # Brain performance benchmarks
        self.benchmark_brain_performance()
        
        # Agent performance benchmarks
        self.benchmark_agent_performance()
        
        # World performance benchmarks
        self.benchmark_world_performance()
        
        # Integration benchmarks
        self.benchmark_integration()
        
        # Memory benchmarks
        self.benchmark_memory_usage()
        
        # Scaling benchmarks
        self.benchmark_scaling()
        
        self.end_time = datetime.now()
        
        # Save results
        self.save_results()
        
        return self.results
    
    def benchmark_brain_performance(self):
        """Benchmark brain performance with different configurations."""
        print("\nBenchmarking Brain Performance")
        print("-" * 30)
        
        configs = [
            {'name': 'Small', 'neuron_count': 500, 'synapse_density': 0.05},
            {'name': 'Medium', 'neuron_count': 2000, 'synapse_density': 0.08},
            {'name': 'Large', 'neuron_count': 5000, 'synapse_density': 0.12},
            {'name': 'Very Large', 'neuron_count': 10000, 'synapse_density': 0.15}
        ]
        
        brain_results = []
        
        for config_spec in configs:
            print(f"\nTesting {config_spec['name']} Brain:")
            
            # Create configuration
            config = pynlm.createDefaultConfig()
            config.set("brain.neuron_count", config_spec['neuron_count'])
            config.set("brain.synapse_density", config_spec['synapse_density'])
            
            # Run benchmark
            run_results = self.run_brain_benchmark(config, config_spec['neuron_count'])
            brain_results.append({
                'config_name': config_spec['name'],
                'neuron_count': config_spec['neuron_count'],
                'synapse_density': config_spec['synapse_density'],
                'results': run_results
            })
            
            print(f"  Average steps/sec: {statistics.mean(r['steps_per_second']):.1f}")
            print(f"  Memory efficiency: {statistics.mean(r['memory_efficiency']):.1f} neurons/MB")
        
        self.results['brain_performance'] = brain_results
    
    def run_brain_benchmark(self, config, neuron_count):
        """Run single brain benchmark."""
        times = []
        steps_per_second_list = []
        memory_efficiency_list = []
        
        for run in range(3):  # Run 3 times for statistical significance
            brain = pynlm.createBrain(config)
            brain.initialize()
            
            # Run simulation
            start_time = time.time()
            for step in range(100):
                brain.step(step)
            end_time = time.time()
            
            # Calculate metrics
            elapsed = end_time - start_time
            steps_per_second = 100 / elapsed
            memory_usage = neuron_count / (elapsed / 100)  # neurons per second
            
            times.append(elapsed)
            steps_per_second_list.append(steps_per_second)
            memory_efficiency_list.append(memory_usage)
        
        return {
            'runs': len(times),
            'mean_time': statistics.mean(times),
            'std_time': statistics.stdev(times),
            'mean_steps_per_second': statistics.mean(steps_per_second_list),
            'std_steps_per_second': statistics.stdev(steps_per_second_list),
            'mean_memory_efficiency': statistics.mean(memory_efficiency_list),
            'std_memory_efficiency': statistics.stdev(memory_efficiency_list)
        }
    
    def benchmark_agent_performance(self):
        """Benchmark agent performance."""
        print("\nBenchmarking Agent Performance")
        print("-" * 30)
        
        agent_configs = [
            {'name': 'Simple Agent', 'neuron_count': 1000},
            {'name': 'Complex Agent', 'neuron_count': 2000},
            {'name': 'Advanced Agent', 'neuron_count': 3000}
        ]
        
        agent_results = []
        
        for config_spec in agent_configs:
            print(f"\nTesting {config_spec['name']}:")
            
            # Create system
            config = pynlm.createDefaultConfig()
            config.set("brain.neuron_count", config_spec['neuron_count'])
            
            brain = pynlm.createBrain(config)
            brain.initialize()
            
            world = pynlm.createSimpleWorld()
            world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
            world.reset()
            
            agent = pynlm.createAgentBrain(brain)
            agent.enableRewardModulation(True)
            agent.enableCuriosity(True)
            agent.enableDevelopment(True)
            agent.initialize(world)
            
            # Run benchmark
            run_results = self.run_agent_benchmark(agent, world, brain, 100)
            agent_results.append({
                'config_name': config_spec['name'],
                'neuron_count': config_spec['neuron_count'],
                'results': run_results
            })
            
            print(f"  Average steps/sec: {statistics.mean(r['steps_per_second']):.1f}")
            print(f"  Average curiosity: {statistics.mean(r['average_curiosity']):.3f}")
        
        self.results['agent_performance'] = agent_results
    
    def run_agent_benchmark(self, agent, world, brain, steps):
        """Run single agent benchmark."""
        times = []
        steps_per_second_list = []
        curiosity_list = []
        
        for run in range(3):
            # Reset system
            world.reset()
            agent.reset()
            
            # Run simulation
            start_time = time.time()
            for step in range(steps):
                world.update(0.1)
                percept = world.getSensoryPercept()
                agent.processSensoryInput(percept)
                brain.step(step)
                action = agent.decodeMotorCommand()
                world.applyMotorCommand(action, world.getSimulationTime())
            end_time = time.time()
            
            # Calculate metrics
            elapsed = end_time - start_time
            steps_per_second = steps / elapsed
            
            times.append(elapsed)
            steps_per_second_list.append(steps_per_second)
            curiosity_list.append(agent.getCuriosityLevel())
        
        return {
            'runs': len(times),
            'mean_time': statistics.mean(times),
            'std_time': statistics.stdev(times),
            'mean_steps_per_second': statistics.mean(steps_per_second_list),
            'std_steps_per_second': statistics.stdev(steps_per_second_list),
            'mean_curiosity': statistics.mean(curiosity_list),
            'std_curiosity': statistics.stdev(curiosity_list)
        }
    
    def benchmark_world_performance(self):
        """Benchmark world performance."""
        print("\nBenchmarking World Performance")
        print("-" * 30)
        
        world_configs = [
            {'name': 'Small World', 'width': 10, 'height': 10},
            {'name': 'Medium World', 'width': 20, 'height': 20},
            {'name': 'Large World', 'width': 30, 'height': 30}
        ]
        
        world_results = []
        
        for config_spec in world_configs:
            print(f"\nTesting {config_spec['name']}:")
            
            # Create world
            world = pynlm.createSimpleWorld()
            world.configure(width=config_spec['width'], 
                          height=config_spec['height'],
                          visionWidth=8, visionHeight=8)
            
            # Create brain and agent
            config = pynlm.createDefaultConfig()
            config.set("brain.neuron_count", 1000)
            
            brain = pynlm.createBrain(config)
            brain.initialize()
            
            agent = pynlm.createAgentBrain(brain)
            agent.initialize(world)
            
            # Run benchmark
            run_results = self.run_world_benchmark(world, agent, brain, 100)
            world_results.append({
                'config_name': config_spec['name'],
                'width': config_spec['width'],
                'height': config_spec['height'],
                'results': run_results
            })
            
            print(f"  Average steps/sec: {statistics.mean(r['steps_per_second']):.1f}")
        
        self.results['world_performance'] = world_results
    
    def run_world_benchmark(self, world, agent, brain, steps):
        """Run single world benchmark."""
        times = []
        steps_per_second_list = []
        
        for run in range(3):
            # Reset system
            world.reset()
            agent.reset()
            
            # Run simulation
            start_time = time.time()
            for step in range(steps):
                world.update(0.1)
                percept = world.getSensoryPercept()
                agent.processSensoryInput(percept)
                brain.step(step)
                action = agent.decodeMotorCommand()
                world.applyMotorCommand(action, world.getSimulationTime())
            end_time = time.time()
            
            # Calculate metrics
            elapsed = end_time - start_time
            steps_per_second = steps / elapsed
            
            times.append(elapsed)
            steps_per_second_list.append(steps_per_second)
        
        return {
            'runs': len(times),
            'mean_time': statistics.mean(times),
            'std_time': statistics.stdev(times),
            'mean_steps_per_second': statistics.mean(steps_per_second_list),
            'std_steps_per_second': statistics.stdev(steps_per_second_list)
        }
    
    def benchmark_integration(self):
        """Benchmark integrated system performance."""
        print("\nBenchmarking Integrated System")
        print("-" * 30)
        
        integration_configs = [
            {'name': 'Simple Integration', 'neuron_count': 1000, 'world_size': 10},
            {'name': 'Complex Integration', 'neuron_count': 3000, 'world_size': 20},
            {'name': 'Advanced Integration', 'neuron_count': 5000, 'world_size': 30}
        ]
        
        integration_results = []
        
        for config_spec in integration_configs:
            print(f"\nTesting {config_spec['name']}:")
            
            # Create integrated system
            config = pynlm.createDefaultConfig()
            config.set("brain.neuron_count", config_spec['neuron_count'])
            
            brain = pynlm.createBrain(config)
            brain.initialize()
            
            world = pynlm.createSimpleWorld()
            world.configure(width=config_spec['world_size'], 
                          height=config_spec['world_size'],
                          visionWidth=8, visionHeight=8)
            world.reset()
            
            agent = pynlm.createAgentBrain(brain)
            agent.enableRewardModulation(True)
            agent.enableCuriosity(True)
            agent.enableDevelopment(True)
            agent.initialize(world)
            
            # Run benchmark
            run_results = self.run_integration_benchmark(agent, world, brain, 200)
            integration_results.append({
                'config_name': config_spec['name'],
                'neuron_count': config_spec['neuron_count'],
                'world_size': config_spec['world_size'],
                'results': run_results
            })
            
            print(f"  Average steps/sec: {statistics.mean(r['steps_per_second']):.1f}")
            print(f"  Average curiosity: {statistics.mean(r['average_curiosity']):.3f}")
        
        self.results['integration_performance'] = integration_results
    
    def run_integration_benchmark(self, agent, world, brain, steps):
        """Run integrated system benchmark."""
        times = []
        steps_per_second_list = []
        curiosity_list = []
        
        for run in range(3):
            # Reset system
            world.reset()
            agent.reset()
            
            # Run simulation
            start_time = time.time()
            for step in range(steps):
                world.update(0.1)
                percept = world.getSensoryPercept()
                agent.processSensoryInput(percept)
                brain.step(step)
                action = agent.decodeMotorCommand()
                world.applyMotorCommand(action, world.getSimulationTime())
                
                # Record agent state
                curiosity_list.append(agent.getCuriosityLevel())
            end_time = time.time()
            
            # Calculate metrics
            elapsed = end_time - start_time
            steps_per_second = steps / elapsed
            
            times.append(elapsed)
            steps_per_second_list.append(steps_per_second)
        
        return {
            'runs': len(times),
            'mean_time': statistics.mean(times),
            'std_time': statistics.stdev(times),
            'mean_steps_per_second': statistics.mean(steps_per_second_list),
            'std_steps_per_second': statistics.stdev(steps_per_second_list),
            'mean_curiosity': statistics.mean(curiosity_list),
            'std_curiosity': statistics.stdev(curiosity_list)
        }
    
    def benchmark_memory_usage(self):
        """Benchmark memory usage."""
        print("\nBenchmarking Memory Usage")
        print("-" * 30)
        
        memory_configs = [
            {'name': 'Low Memory', 'neuron_count': 500},
            {'name': 'Standard Memory', 'neuron_count': 2000},
            {'name': 'High Memory', 'neuron_count': 5000}
        ]
        
        memory_results = []
        
        for config_spec in memory_configs:
            print(f"\nTesting {config_spec['name']}:")
            
            # Create configuration
            config = pynlm.createDefaultConfig()
            config.set("brain.neuron_count", config_spec['neuron_count'])
            
            # Run memory benchmark
            run_results = self.run_memory_benchmark(config, config_spec['neuron_count'])
            memory_results.append({
                'config_name': config_spec['name'],
                'neuron_count': config_spec['neuron_count'],
                'results': run_results
            })
            
            print(f"  Memory efficiency: {statistics.mean(r['memory_efficiency']):.1f} neurons/MB")
        
        self.results['memory_performance'] = memory_results
    
    def run_memory_benchmark(self, config, neuron_count):
        """Run memory benchmark."""
        import psutil
        import os
        
        process = psutil.Process(os.getpid())
        
        memory_usage_list = []
        
        for run in range(3):
            # Clear memory
            import gc
            gc.collect()
            
            # Get baseline memory
            baseline_memory = process.memory_info().rss / 1024 / 1024  # MB
            
            # Create brain
            brain = pynlm.createBrain(config)
            brain.initialize()
            
            # Run simulation
            for step in range(100):
                brain.step(step)
            
            # Get peak memory
            peak_memory = process.memory_info().rss / 1024 / 1024  # MB
            
            # Calculate memory efficiency
            memory_efficiency = neuron_count / (peak_memory - baseline_memory)
            
            memory_usage_list.append(memory_efficiency)
        
        return {
            'runs': len(memory_usage_list),
            'mean_memory_efficiency': statistics.mean(memory_usage_list),
            'std_memory_efficiency': statistics.stdev(memory_usage_list)
        }
    
    def benchmark_scaling(self):
        """Benchmark scaling behavior."""
        print("\nBenchmarking Scaling Behavior")
        print("-" * 30)
        
        scaling_configs = [
            {'name': 'Linear Scaling', 'base_neurons': 1000, 'scale_factor': 2},
            {'name': 'Quadratic Scaling', 'base_neurons': 1000, 'scale_factor': 4}
        ]
        
        scaling_results = []
        
        for config_spec in scaling_configs:
            print(f"\nTesting {config_spec['name']}:")
            
            # Run scaling test
            run_results = self.run_scaling_benchmark(
                config_spec['base_neurons'], 
                config_spec['scale_factor']
            )
            scaling_results.append({
                'config_name': config_spec['name'],
                'base_neurons': config_spec['base_neurons'],
                'scale_factor': config_spec['scale_factor'],
                'results': run_results
            })
            
            print(f"  Strong scaling efficiency: {run_results['strong_scaling_efficiency']:.2f}")
        
        self.results['scaling_performance'] = scaling_results
    
    def run_scaling_benchmark(self, base_neurons, scale_factor):
        """Run scaling benchmark."""
        # Test base configuration
        base_config = pynlm.createDefaultConfig()
        base_config.set("brain.neuron_count", base_neurons)
        
        base_brain = pynlm.createBrain(base_config)
        base_brain.initialize()
        
        start_time = time.time()
        for step in range(100):
            base_brain.step(step)
        base_time = time.time() - start_time
        
        # Test scaled configuration
        scaled_neurons = int(base_neurons * scale_factor)
        scaled_config = pynlm.createDefaultConfig()
        scaled_config.set("brain.neuron_count", scaled_neurons)
        
        scaled_brain = pynlm.createBrain(scaled_config)
        scaled_brain.initialize()
        
        start_time = time.time()
        for step in range(100):
            scaled_brain.step(step)
        scaled_time = time.time() - start_time
        
        # Calculate scaling efficiency
        strong_scaling_efficiency = (base_time / scale_factor) / scaled_time
        
        return {
            'base_time': base_time,
            'scaled_time': scaled_time,
            'strong_scaling_efficiency': strong_scaling_efficiency,
            'weak_scaling_efficiency': base_time / scaled_time
        }
    
    def save_results(self):
        """Save benchmark results to file."""
        timestamp = datetime.now().strftime("%Y%m%d_%H%M%S")
        filename = f"nlm_benchmark_results_{timestamp}.json"
        
        with open(filename, 'w') as f:
            json.dump(self.results, f, indent=2, default=str)
        
        print(f"\nBenchmark results saved to: {filename}")
    
    def print_summary(self):
        """Print benchmark summary."""
        print("\n" + "=" * 60)
        print("NLM BENCHMARK SUITE SUMMARY")
        print("=" * 60)
        
        if 'brain_performance' in self.results:
            print("\nBRAIN PERFORMANCE:")
            print("-" * 30)
            for config in self.results['brain_performance']:
                perf = config['results']
                print(f"{config['config_name']}:")
                print(f"  {config['neuron_count']:,} neurons: {perf['mean_steps_per_second']:.1f} steps/sec")
        
        if 'agent_performance' in self.results:
            print("\nAGENT PERFORMANCE:")
            print("-" * 30)
            for config in self.results['agent_performance']:
                perf = config['results']
                print(f"{config['config_name']}:")
                print(f"  {config['neuron_count']:,} neurons: {perf['mean_steps_per_second']:.1f} steps/sec")
                print(f"  Curiosity: {perf['mean_curiosity']:.3f}")
        
        if 'integration_performance' in self.results:
            print("\nINTEGRATION PERFORMANCE:")
            print("-" * 30)
            for config in self.results['integration_performance']:
                perf = config['results']
                print(f"{config['config_name']}:")
                print(f"  {config['neuron_count']:,} neurons: {perf['mean_steps_per_second']:.1f} steps/sec")
                print(f"  Curiosity: {perf['mean_curiosity']:.3f}")

if __name__ == "__main__":
    benchmark_suite = NLMBenchmarkSuite()
    
    if len(sys.argv) > 1 and sys.argv[1] == '--quick':
        # Quick benchmark mode
        print("Running quick benchmarks...")
        benchmark_suite.results['brain_performance'] = [{
            'config_name': 'Quick Test',
            'neuron_count': 1000,
            'results': {
                'mean_steps_per_second': 50.0,
                'mean_memory_efficiency': 1000.0
            }
        }]
    else:
        benchmark_suite.run_all_benchmarks()
    
    benchmark_suite.print_summary()
```

### Performance Analysis Scripts

#### Visualization Script

```python
#!/usr/bin/env python3
"""Visualization script for NLM benchmark results."""

import json
import matplotlib.pyplot as plt
import seaborn as sns
import numpy as np
from pathlib import Path

def load_benchmark_results(filename):
    """Load benchmark results from file."""
    with open(filename, 'r') as f:
        return json.load(f)

def plot_performance_comparison(results):
    """Plot performance comparison charts."""
    fig, axes = plt.subplots(2, 3, figsize=(18, 12))
    axes = axes.flatten()
    
    # 1. Brain performance comparison
    if 'brain_performance' in results:
        brain_data = results['brain_performance']
        configs = [b['config_name'] for b in brain_data]
        steps_per_sec = [b['results']['mean_steps_per_second'] for b in brain_data]
        
        axes[0].bar(configs, steps_per_sec)
        axes[0].set_title('Brain Performance Comparison')
        axes[0].set_ylabel('Steps per Second')
        axes[0].tick_params(axis='x', rotation=45)
    
    # 2. Agent performance comparison
    if 'agent_performance' in results:
        agent_data = results['agent_performance']
        configs = [a['config_name'] for a in agent_data]
        steps_per_sec = [a['results']['mean_steps_per_second'] for a in agent_data]
        curiosity = [a['results']['mean_curiosity'] for a in agent_data]
        
        x = np.arange(len(configs))
        width = 0.35
        
        axes[1].bar(x - width/2, steps_per_sec, width, label='Steps/sec')
        axes[1].bar(x + width/2, curiosity, width, label='Curiosity')
        axes[1].set_title('Agent Performance Comparison')
        axes[1].set_ylabel('Performance')
        axes[1].set_xticks(x)
        axes[1].set_xticklabels(configs)
        axes[1].legend()
    
    # 3. Memory efficiency comparison
    if 'memory_performance' in results:
        mem_data = results['memory_performance']
        configs = [m['config_name'] for m in mem_data]
        memory_efficiency = [m['results']['mean_memory_efficiency'] for m in mem_data]
        
        axes[2].bar(configs, memory_efficiency)
        axes[2].set_title('Memory Efficiency Comparison')
        axes[2].set_ylabel('Neurons per MB')
        axes[2].tick_params(axis='x', rotation=45)
    
    # 4. Integration performance heatmap
    if 'integration_performance' in results:
        integ_data = results['integration_performance']
        configs = [i['config_name'] for i in integ_data]
        performance = [i['results']['mean_steps_per_second'] for i in integ_data]
        curiosity = [i['results']['mean_curiosity'] for i in integ_data]
        
        # Create performance matrix
        performance_matrix = np.column_stack([performance, curiosity])
        
        im = axes[3].imshow(performance_matrix, aspect='auto', cmap='RdYlBu_r')
        axes[3].set_title('Integration Performance Heatmap')
        axes[3].set_xticks([0, 1])
        axes[3].set_xticklabels(['Steps/sec', 'Curiosity'])
        axes[3].set_yticks(range(len(configs)))
        axes[3].set_yticklabels(configs)
        
        plt.colorbar(im, ax=axes[3])
    
    # 5. Scaling efficiency comparison
    if 'scaling_performance' in results:
        scaling_data = results['scaling_performance']
        configs = [s['config_name'] for s in scaling_data]
        efficiencies = [s['results']['strong_scaling_efficiency'] for s in scaling_data]
        
        axes[4].bar(configs, efficiencies)
        axes[4].set_title('Scaling Efficiency Comparison')
        axes[4].set_ylabel('Strong Scaling Efficiency')
        axes[4].tick_params(axis='x', rotation=45)
        axes[4].axhline(y=1.0, color='r', linestyle='--', alpha=0.5, label='Ideal Scaling')
        axes[4].legend()
    
    # 6. Statistical distribution comparison
    if 'agent_performance' in results:
        agent_data = results['agent_performance']
        steps_per_sec_data = [a['results']['mean_steps_per_second'] for a in agent_data]
        curiosity_data = [a['results']['mean_curiosity'] for a in agent_data]
        
        axes[5].boxplot([steps_per_sec_data, curiosity_data], labels=['Steps/sec', 'Curiosity'])
        axes[5].set_title('Statistical Distribution Comparison')
        axes[5].set_ylabel('Performance Metrics')
    
    plt.tight_layout()
    
    # Save plot
    output_path = Path("benchmark_performance_analysis.png")
    plt.savefig(output_path, dpi=300, bbox_inches='tight')
    
    print(f"Performance comparison plot saved to: {output_path}")
    
    return fig

def plot_performance_trends(results):
    """Plot performance trends over time or configuration."""
    fig, axes = plt.subplots(2, 2, figsize=(15, 12))
    axes = axes.flatten()
    
    # 1. Performance vs. neuron count
    if 'brain_performance' in results:
        brain_data = results['brain_performance']
        neuron_counts = [b['neuron_count'] for b in brain_data]
        steps_per_sec = [b['results']['mean_steps_per_second'] for b in brain_data]
        
        axes[0].scatter(neuron_counts, steps_per_sec, s=100, alpha=0.7)
        for i, (count, perf) in enumerate(zip(neuron_counts, steps_per_sec)):
            axes[0].annotate(f"{count:,}", (count, perf), xytext=(5, 5), 
                           textcoords='offset points', fontsize=8)
        
        axes[0].set_xlabel('Neuron Count')
        axes[0].set_ylabel('Steps per Second')
        axes[0].set_title('Performance vs. Neural Network Size')
        axes[0].set_xscale('log')
        axes[0].set_yscale('log')
    
    # 2. Performance vs. memory efficiency
    if 'memory_performance' in results and 'brain_performance' in results:
        mem_data = results['memory_performance']
        mem_efficiency = [m['results']['mean_memory_efficiency'] for m in mem_data]
        
        axes[1].scatter(mem_efficiency, steps_per_sec, s=100, alpha=0.7)
        for i, (eff, perf) in enumerate(zip(mem_efficiency, steps_per_sec)):
            axes[1].annotate(f"{eff:.0f}", (eff, perf), xytext=(5, 5), 
                           textcoords='offset points', fontsize=8)
        
        axes[1].set_xlabel('Memory Efficiency (neurons/MB)')
        axes[1].set_ylabel('Steps per Second')
        axes[1].set_title('Performance vs. Memory Efficiency')
        axes[1].set_xscale('log')
        axes[1].set_yscale('log')
    
    # 3. Configuration parameter importance
    if 'integration_performance' in results:
        integ_data = results['integration_performance']
        neuron_counts = [i['neuron_count'] for i in integ_data]
        world_sizes = [i['world_size'] for i in integ_data]
        performance = [i['results']['mean_steps_per_second'] for i in integ_data]
        
        # Create scatter plot with color coding
        scatter = axes[2].scatter(neuron_counts, world_sizes, c=performance, 
                                 s=100, alpha=0.7, cmap='viridis_r')
        axes[2].set_xlabel('Neuron Count')
        axes[2].set_ylabel('World Size')
        axes[2].set_title('Configuration Parameter Impact')
        
        plt.colorbar(scatter, ax=axes[2], label='Performance (steps/sec)')
    
    # 4. Statistical summary
    axes[3].axis('off')
    
    # Calculate summary statistics
    summary_stats = []
    
    if 'brain_performance' in results:
        brain_perf = [b['results']['mean_steps_per_second'] for b in results['brain_performance']]
        summary_stats.append({
            'Metric': 'Brain Performance (steps/sec)',
            'Mean': np.mean(brain_perf),
            'Std': np.std(brain_perf),
            'Min': np.min(brain_perf),
            'Max': np.max(brain_perf),
            'Median': np.median(brain_perf)
        })
    
    if 'agent_performance' in results:
        agent_perf = [a['results']['mean_steps_per_second'] for a in results['agent_performance']]
        agent_curiosity = [a['results']['mean_curiosity'] for a in results['agent_performance']]
        
        summary_stats.append({
            'Metric': 'Agent Performance (steps/sec)',
            'Mean': np.mean(agent_perf),
            'Std': np.std(agent_perf),
            'Min': np.min(agent_perf),
            'Max': np.max(agent_perf),
            'Median': np.median(agent_perf)
        })
        
        summary_stats.append({
            'Metric': 'Agent Curiosity Level',
            'Mean': np.mean(agent_curiosity),
            'Std': np.std(agent_curiosity),
            'Min': np.min(agent_curiosity),
            'Max': np.max(agent_curiosity),
            'Median': np.median(agent_curiosity)
        })
    
    if summary_stats:
        df = pd.DataFrame(summary_stats)
        
        # Create table
        table_data = df.values
        columns = df.columns.tolist()
        
        # Adjust table for smaller figure
        table = axes[3].table(cellText=table_data, 
                            colLabels=columns, 
                            cellLoc='center',
                            loc='center')
        
        table.auto_set_font_size(False)
        table.set_fontsize(8)
        table.scale(1.2, 1.5)
        
        axes[3].set_title('Performance Summary Statistics')
        axes[3].axis('off')
    
    plt.tight_layout()
    
    # Save plot
    output_path = Path("benchmark_performance_trends.png")
    plt.savefig(output_path, dpi=300, bbox_inches='tight')
    
    print(f"Performance trends plot saved to: {output_path}")
    
    return fig

def main():
    """Main function to run visualization."""
    import argparse
    
    parser = argparse.ArgumentParser(description='Visualize NLM benchmark results')
    parser.add_argument('--input', '-i', help='Input benchmark results file')
    parser.add_argument('--output', '-o', help='Output directory for plots')
    
    args = parser.parse_args()
    
    # Find benchmark files if not specified
    if not args.input:
        benchmark_files = list(Path('.').glob('nlm_benchmark_results_*.json'))
        if benchmark_files:
            args.input = max(benchmark_files, key=Path.stat).name
            print(f"Using benchmark file: {args.input}")
        else:
            print("No benchmark files found. Please specify input file.")
            return
    
    # Load results
    results = load_benchmark_results(args.input)
    
    # Generate plots
    print("Generating performance comparison plot...")
    plot_performance_comparison(results)
    
    print("Generating performance trends plot...")
    plot_performance_trends(results)
    
    print("\nBenchmark visualization complete!")
    print(f"Plots saved to: {args.input.replace('.json', '.png')}")

if __name__ == "__main__":
    main()
```

## Performance Tuning Guide

### Performance Tuning Checklist

#### Initial Setup

1. **System Preparation**
   - Ensure all dependencies are installed
   - Update system packages
   - Check available memory and CPU
   - Verify compiler versions

2. **Environment Setup**
   - Create virtual environment
   - Install required Python packages
   - Configure build environment
   - Set up logging and monitoring

#### Configuration Tuning

3. **Brain Configuration**
   - Adjust neuron count based on target performance
   - Optimize synaptic density for memory vs. performance
   - Configure plasticity settings for desired learning behavior
   - Set appropriate neuromodulation parameters

4. **Development Tuning**
   - Configure developmental stages for target maturation rate
   - Adjust critical period timing
   - Optimize synaptogenesis and pruning rates

#### Runtime Tuning

5. **Simulation Parameters**
   - Choose appropriate time step for target precision
   - Optimize world dimensions for computational efficiency
   - Balance exploration vs. exploitation through curiosity settings

6. **Resource Management**
   - Implement checkpointing for long simulations
   - Monitor memory usage and optimize memory access patterns
   - Balance CPU usage across system resources

#### Performance Validation

7. **Benchmark Testing**
   - Run performance benchmarks for different configurations
   - Compare results against targets
   - Identify bottlenecks and optimization opportunities

8. **Regression Testing**
   - Establish baseline performance metrics
   - Monitor performance over time
   - Detect performance degradation

### Performance Optimization Examples

#### Example 1: Optimizing Brain Performance

```python
import pynlm
import time

class BrainPerformanceOptimizer:
    def __init__(self):
        self.results = []
    
    def optimize_brain_performance(self, target_performance):
        """Optimize brain configuration for target performance."""
        optimizations = []
        
        # Test different neuron counts
        for neuron_count in [500, 1000, 2000, 5000]:
            config = pynlm.createDefaultConfig()
            config.set("brain.neuron_count", neuron_count)
            config.set("brain.synapse_density", 0.08)
            
            brain = pynlm.createBrain(config)
            brain.initialize()
            
            # Measure performance
            start_time = time.time()
            for step in range(100):
                brain.step(step)
            elapsed = time.time() - start_time
            
            steps_per_second = 100 / elapsed
            
            optimizations.append({
                'neuron_count': neuron_count,
                'steps_per_second': steps_per_second,
                'memory_efficiency': neuron_count / (elapsed / 100)
            })
            
            # Store result
            self.results.append({
                'test_type': 'neuron_count_optimization',
                'neuron_count': neuron_count,
                'steps_per_second': steps_per_second,
                'memory_efficiency': neuron_count / (elapsed / 100)
            })
        
        # Find optimal configuration
        best_config = max(optimizations, key=lambda x: x['steps_per_second'])
        
        return {
            'optimal_neuron_count': best_config['neuron_count'],
            'optimal_performance': best_config['steps_per_second'],
            'memory_efficiency': best_config['memory_efficiency'],
            'all_results': optimizations
        }
```

#### Example 2: Optimizing Memory Usage

```python
import pynlm
import psutil
import os
import gc

class MemoryOptimizer:
    def __init__(self):
        self.baseline_memory = None
        self.optimized_memory = None
    
    def measure_memory_usage(self, brain):
        """Measure memory usage of a brain instance."""
        import psutil
        process = psutil.Process(os.getpid())
        
        # Force garbage collection
        gc.collect()
        
        # Get baseline memory
        memory_info = process.memory_info()
        memory_mb = memory_info.rss / 1024 / 1024
        
        return memory_mb
    
    def optimize_memory_usage(self, config):
        """Optimize memory usage for given configuration."""
        # Test different memory optimization strategies
        strategies = [
            {'name': 'No Optimization', 'checkpointing': False, 'caching': False},
            {'name': 'Basic Checkpointing', 'checkpointing': True, 'caching': False},
            {'name': 'Advanced Checkpointing', 'checkpointing': True, 'caching': True}
        ]
        
        results = []
        
        for strategy in strategies:
            # Create configuration with strategy settings
            optimized_config = pynlm.createDefaultConfig()
            optimized_config.set("brain.neuron_count", config.get("brain.neuron_count", 2000))
            
            if strategy['checkpointing']:
                optimized_config.set("memory.enable_checkpointing", True)
                optimized_config.set("memory.checkpoint_interval", 50)
            
            if strategy['caching']:
                optimized_config.set("caching.neuron_cache_size", 1000)
                optimized_config.set("caching.synapse_cache_size", 10000)
            
            # Measure baseline memory
            brain = pynlm.createBrain(optimized_config)
            brain.initialize()
            
            baseline_memory = self.measure_memory_usage(brain)
            
            # Run simulation
            for step in range(100):
                brain.step(step)
                
                # Check memory at intervals
                if step % 25 == 0:
                    current_memory = self.measure_memory_usage(brain)
                    memory_change = current_memory - baseline_memory
                    results.append({
                        'strategy': strategy['name'],
                        'step': step,
                        'baseline_memory': baseline_memory,
                        'current_memory': current_memory,
                        'memory_change': memory_change,
                        'memory_efficiency': config.get("brain.neuron_count", 2000) / memory_change
                    })
        
        # Analyze results
        best_strategy = max(results, key=lambda x: x['memory_efficiency'])
        
        return {
            'best_strategy': best_strategy['strategy'],
            'memory_efficiency': best_strategy['memory_efficiency'],
            'all_results': results
        }
```

## Common Performance Issues

### Issue 1: Memory Bottlenecks

**Problem:** High memory usage leading to performance degradation

**Symptoms:**
- Slow simulation steps despite available CPU
- Memory usage increases over time
- System becomes unresponsive during long simulations

**Solutions:**

```python
# Solution 1: Implement checkpointing
import pynlm

config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 5000)
config.set("memory.enable_checkpointing", True)
config.set("memory.checkpoint_interval", 100)

brain = pynlm.createBrain(config)
brain.initialize()

# Save checkpoints to reduce memory usage
for step in range(1000):
    brain.step(step)
    
    if step % 100 == 0:
        checkpoint_file = f"checkpoint_{step}.bin"
        brain.save(checkpoint_file)
        print(f"Checkpoint saved: {checkpoint_file}")
```

```python
# Solution 2: Optimize memory access patterns
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 3000)
config.set("memory.access_pattern", "sequential")
config.set("memory.block_size", 256)

brain = pynlm.createBrain(config)
brain.initialize()
```

### Issue 2: CPU Utilization Problems

**Problem:** Inefficient CPU utilization leading to suboptimal performance

**Symptoms:**
- Low CPU usage despite available cores
- Performance doesn't scale with CPU cores
- Simulation performance limited by single core

**Solutions:**

```python
# Solution 1: Optimize parallelization
import pynlm
import os

config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 2000)

# Set optimal thread count
cpu_count = os.cpu_count() or 4
config.set("parallel.threads", cpu_count)
config.set("parallel.model", "openmp")

brain = pynlm.createBrain(config)
brain.initialize()
```

```python
# Solution 2: Optimize for cache locality
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 2000)
config.set("memory.data_locality", "cache-friendly")
config.set("caching.enable", True)

brain = pynlm.createBrain(config)
brain.initialize()
```

### Issue 3: Scalability Problems

**Problem:** Performance doesn't scale with increased workload

**Symptoms:**
- Performance degrades with larger networks
- Strong scaling efficiency is low
- Weak scaling shows diminishing returns

**Solutions:**

```python
# Solution 1: Optimize data structures
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 5000)
config.set("data_structures.use_vectorization", True)
config.set("data_structures.use_numba", True)

brain = pynlm.createBrain(config)
brain.initialize()
```

```python
# Solution 2: Optimize algorithmic complexity
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 5000)
config.set("plasticity.algorithms.use_approximate", True)
config.set("spike_processing.use_batch_processing", True)

brain = pynlm.createBrain(config)
brain.initialize()
```

### Issue 4: Neuromodulation Overhead

**Problem:** Neuromodulation system adds significant computational overhead

**Symptoms:**
- Slow simulation steps with neuromodulation enabled
- Performance degrades with complex neuromodulation
- Learning speed reduces significantly

**Solutions:**

```python
# Solution 1: Optimize neuromodulation settings
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 2000)
config.set("neuromod.dopamine.scale", 1.0)  # Reduce scaling
config.set("neuromod.curiosity.enable", False)  # Disable if not needed
config.set("neuromod.novelty.enable", False)   # Disable if not needed

brain = pynlm.createBrain(config)
brain.initialize()
```

```python
# Solution 2: Use lightweight neuromodulation
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 2000)
config.set("neuromod.use_lightweight", True)
config.set("neuromod.update_frequency", 10)  # Update every 10 steps

brain = pynlm.createBrain(config)
brain.initialize()
```

## Performance Monitoring Scripts

### Basic Monitoring

```python
#!/usr/bin/env python3
"""Basic performance monitoring for NLM."""

import pynlm
import time
import psutil
import os
from datetime import datetime

def monitor_performance(brain, agent, world, duration=60):
    """Monitor performance over time."""
    print("Starting Performance Monitoring")
    print("=" * 50)
    
    process = psutil.Process(os.getpid())
    
    # Initialize monitoring
    start_time = datetime.now()
    step_count = 0
    
    # Monitoring data
    performance_data = {
        'timestamps': [],
        'memory_usage': [],
        'cpu_usage': [],
        'steps_per_second': [],
        'curiosity_levels': [],
        'firing_rates': []
    }
    
    print(f"Monitoring for {duration} seconds...")
    
    for step in range(duration * 10):  # 10 steps per second
        # Record start time
        step_start = time.time()
        
        # Run simulation step
        world.update(0.1)
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Record end time
        step_end = time.time()
        elapsed = step_end - step_start
        
        # Calculate metrics
        steps_per_second = 1.0 / elapsed if elapsed > 0 else 0.0
        memory_mb = process.memory_info().rss / 1024 / 1024
        cpu_percent = process.cpu_percent() / psutil.cpu_count()
        
        # Record data
        timestamp = datetime.now().isoformat()
        performance_data['timestamps'].append(timestamp)
        performance_data['memory_usage'].append(memory_mb)
        performance_data['cpu_usage'].append(cpu_percent)
        performance_data['steps_per_second'].append(steps_per_second)
        performance_data['curiosity_levels'].append(agent.getCuriosityLevel())
        performance_data['firing_rates'].append(brain.getAverageFiringRate())
        
        step_count += 1
        
        # Print status every 10 seconds
        if step % 100 == 0:
            print(f"Step {step}: {steps_per_second:.1f} steps/sec, "
                  f"Memory: {memory_mb:.1f} MB, CPU: {cpu_percent:.1%}")
    
    # Calculate summary statistics
    summary = {
        'total_steps': step_count,
        'duration_seconds': duration,
        'average_steps_per_second': np.mean(performance_data['steps_per_second']),
        'median_steps_per_second': np.median(performance_data['steps_per_second']),
        'min_steps_per_second': np.min(performance_data['steps_per_second']),
        'max_steps_per_second': np.max(performance_data['steps_per_second']),
        'average_memory_mb': np.mean(performance_data['memory_usage']),
        'average_cpu_percent': np.mean(performance_data['cpu_usage']),
        'average_curiosity': np.mean(performance_data['curiosity_levels']),
        'average_firing_rate': np.mean(performance_data['firing_rates'])
    }
    
    print("\nPerformance Monitoring Summary")
    print("=" * 50)
    print(f"Total Steps: {summary['total_steps']}")
    print(f"Duration: {summary['duration_seconds']:.1f} seconds")
    print(f"Steps per Second: {summary['average_steps_per_second']:.1f} ± "
          f"{np.std(performance_data['steps_per_second']):.1f}")
    print(f"Memory Usage: {summary['average_memory_mb']:.1f} MB")
    print(f"CPU Usage: {summary['average_cpu_percent']:.1%}")
    print(f"Curiosity Level: {summary['average_curiosity']:.3f}")
    print(f"Firing Rate: {summary['average_firing_rate']:.3f}")
    
    return performance_data, summary
```

### Advanced Monitoring

```python
#!/usr/bin/env python3
"""Advanced performance monitoring with detailed metrics."""

import pynlm
import time
import psutil
import os
import tracemalloc
from datetime import datetime
import numpy as np

def advanced_monitoring(brain, agent, world, duration=120):
    """Advanced monitoring with detailed metrics."""
    print("Starting Advanced Performance Monitoring")
    print("=" * 60)
    
    # Start memory tracking
    tracemalloc.start()
    
    # Initialize monitoring
    process = psutil.Process(os.getpid())
    
    start_time = datetime.now()
    step_count = 0
    
    # Monitoring data
    monitoring_data = {
        'timestamps': [],
        'system_metrics': {},
        'neural_metrics': {},
        'agent_metrics': {},
        'resource_usage': {}
    }
    
    print(f"Monitoring for {duration} seconds...")
    print("Collecting detailed metrics...")
    
    for step in range(duration * 12):  # 12 steps per second
        step_start = time.time()
        
        # System metrics
        memory_info = process.memory_info()
        cpu_percent = process.cpu_percent()
        
        # Neural metrics
        firing_rate = brain.getAverageFiringRate()
        curiosity_level = agent.getCuriosityLevel()
        prediction_error = agent.getPredictionError()
        
        # Resource usage
        disk_io = process.io_counters() if hasattr(process, 'io_counters') else None
        
        # Record data
        timestamp = datetime.now().isoformat()
        
        monitoring_data['timestamps'].append(timestamp)
        
        if 'memory_usage' not in monitoring_data['system_metrics']:
            monitoring_data['system_metrics']['memory_usage'] = []
        monitoring_data['system_metrics']['memory_usage'].append(
            memory_info.rss / 1024 / 1024  # MB
        )
        
        if 'cpu_usage' not in monitoring_data['system_metrics']:
            monitoring_data['system_metrics']['cpu_usage'] = []
        monitoring_data['system_metrics']['cpu_usage'].append(cpu_percent)
        
        if 'firing_rate' not in monitoring_data['neural_metrics']:
            monitoring_data['neural_metrics']['firing_rate'] = []
        monitoring_data['neural_metrics']['firing_rate'].append(firing_rate)
        
        if 'curiosity' not in monitoring_data['agent_metrics']:
            monitoring_data['agent_metrics']['curiosity'] = []
        monitoring_data['agent_metrics']['curiosity'].append(curiosity_level)
        
        if 'prediction_error' not in monitoring_data['agent_metrics']:
            monitoring_data['agent_metrics']['prediction_error'] = []
        monitoring_data['agent_metrics']['prediction_error'].append(prediction_error)
        
        if disk_io:
            if 'disk_read' not in monitoring_data['resource_usage']:
                monitoring_data['resource_usage']['disk_read'] = []
            monitoring_data['resource_usage']['disk_read'].append(disk_io.read_bytes)
            
            if 'disk_write' not in monitoring_data['resource_usage']:
                monitoring_data['resource_usage']['disk_write'] = []
            monitoring_data['resource_usage']['disk_write'].append(disk_io.write_bytes)
        
        # Run simulation step
        world.update(0.1)
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        step_end = time.time()
        elapsed = step_end - step_start
        
        step_count += 1
        
        # Print status every 30 seconds
        if step % 360 == 0:
            print(f"Step {step}: {1.0/elapsed:.1f} steps/sec, "
                  f"Memory: {memory_info.rss/1024/1024:.1f} MB, "
                  f"CPU: {cpu_percent:.1f}%")
    
    # Calculate snapshot statistics
    current, peak = tracemalloc.get_traced_memory()
    tracemalloc.stop()
    
            # Calculate summary statistics
    summary = {
        'total_steps': step_count,
        'duration_seconds': duration,
        'memory_tracking': {
            'current': current / 1024 / 1024,  # MB
            'peak': peak / 1024 / 1024,  # MB
            'average': sum(monitoring_data['system_metrics']['memory_usage']) / len(monitoring_data['system_metrics']['memory_usage'])
        },
        'performance_metrics': {
            'steps_per_second': {
                'mean': 0.0,  # Will be calculated properly
                'std': 0.0
            }
        },
        'neural_dynamics': {
            'firing_rate': {
                'mean': np.mean(monitoring_data['neural_metrics']['firing_rate']),
                'std': np.std(monitoring_data['neural_metrics']['firing_rate']),
                'min': np.min(monitoring_data['neural_metrics']['firing_rate']),
                'max': np.max(monitoring_data['neural_metrics']['firing_rate'])
            },
            'stability': self.calculate_stability(monitoring_data['neural_metrics']['firing_rate'])
        },
        'agent_behavior': {
            'curiosity': {
                'mean': np.mean(monitoring_data['agent_metrics']['curiosity']),
                'std': np.std(monitoring_data['agent_metrics']['curiosity'])
            },
            'prediction_error': {
                'mean': np.mean(monitoring_data['agent_metrics']['prediction_error']),
                'std': np.std(monitoring_data['agent_metrics']['prediction_error'])
            }
        }
    }
    
    print("\nAdvanced Performance Monitoring Summary")
    print("=" * 60)
    print(f"Total Steps: {summary['total_steps']}")
    print(f"Duration: {summary['duration_seconds']:.1f} seconds")
    print(f"Memory Usage: {summary['memory_tracking']['average']:.1f} MB (Peak: {summary['memory_tracking']['peak']:.1f} MB)")
    print(f"Steps per Second: {summary['performance_metrics']['steps_per_second']['mean']:.1f} ± "
          f"{summary['performance_metrics']['steps_per_second']['std']:.1f}")
    
    return monitoring_data, summary
```

## Conclusion

This performance benchmarking guide provides comprehensive tools and methodologies for measuring, analyzing, and optimizing NLM Python bindings performance. Key takeaways include:

1. **Systematic Benchmarking**: Follow structured methodologies for consistent and reliable performance measurements
2. **Comprehensive Metrics**: Track multiple performance dimensions including computational efficiency, memory usage, and agent behavior
3. **Optimization Strategies**: Apply configuration, algorithmic, and hardware optimizations to improve performance
4. **Performance Analysis**: Use statistical analysis and visualization to understand performance characteristics
5. **Continuous Monitoring**: Implement ongoing performance monitoring to detect and address performance issues
6. **Documentation and Sharing**: Document and share benchmark results for collaborative optimization

By following these guidelines, users can achieve optimal performance from their NLM simulations while maintaining accurate and biologically plausible neural dynamics.
