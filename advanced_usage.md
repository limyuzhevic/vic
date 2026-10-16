# NLM Advanced Usage Guide

This guide covers advanced usage patterns, best practices, and comprehensive examples for the NLM (Neural Learning Machine) Python bindings. It builds upon the basic usage examples with advanced features, performance optimization, and real-world use cases.

## Table of Contents

1. [Advanced Brain Configuration](#advanced-brain-configuration)
2. [Event-Driven Simulation Monitoring](#event-driven-simulation-monitoring)
3. [Performance Profiling and Optimization](#performance-profiling-and-optimization)
4. [Visualization and Analysis](#visualization-and-analysis)
5. [Memory and State Management](#memory-and-state-management)
6. [Advanced Agent Behaviors](#advanced-agent-behaviors)
7. [Production Deployment Patterns](#production-deployment-patterns)
8. [Troubleshooting and Best Practices](#troubleshooting-and-best-practices)
9. [Performance Optimization Techniques](#performance-optimization-techniques)
10. [API Reference - Advanced Functions](#api-reference-advanced-functions)

---

# Advanced Brain Configuration

## Custom Brain Configuration

The advanced configuration system allows for type-safe parameter setting and validation:

```python
import pynlm
import json

def create_advanced_brain():
    """Create a brain with advanced configuration."""
    config = pynlm.createDefaultConfig()
    
    # Set configuration using type-safe methods
    config.setInt("brain.neuron_count", 5000)          # 5000 neurons
    config.setDouble("brain.synapse_density", 0.15)    # 15% connectivity
    config.setDouble("brain.initial_weight_mean", 0.6)
    config.setDouble("brain.initial_weight_std", 0.15)
    config.setDouble("brain.v_thresh", -45.0)         # -45 mV threshold
    config.setDouble("brain.v_rest", -65.0)           # -65 mV resting potential
    config.setDouble("brain.v_reset", -70.0)          # -70 mV reset potential
    config.setDouble("brain.tau_mem", 25.0)           # 25 ms membrane time constant
    config.setDouble("brain.tau_ref", 3.0)            # 3 ms refractory period
    
    # Configure plasticity rules
    config.setBool("plasticity.stdp.enable", True)
    config.setDouble("plasticity.stdp.learning_rate", 0.002)
    config.setDouble("plasticity.stdp.tau_plus", 20.0)
    config.setDouble("plasticity.stdp.tau_minus", 20.0)
    
    config.setBool("plasticity.hebbian.enable", True)
    config.setDouble("plasticity.hebbian.learning_rate", 0.001)
    
    config.setBool("plasticity.structural.enable", True)
    config.setDouble("plasticity.structural.synaptogenesis_rate", 0.0002)
    config.setDouble("plasticity.structural.pruning_rate", 0.00002)
    
    # Configure neuromodulation
    config.setDouble("neuromod.dopamine.scale", 1.5)
    config.setBool("neuromod.curiosity.enable", True)
    config.setDouble("neuromod.curiosity.exploration_rate", 0.3)
    config.setBool("neuromod.novelty.enable", True)
    config.setDouble("neuromod.novelty.threshold", 0.7)
    
    # Configure development
    config.setBool("development.enable", True)
    config.setDouble("development.critical_period_length", 100.0)
    config.setDouble("development.plasticity_modifier.initial", 1.0)
    config.setDouble("development.plasticity_modifier.adult", 0.3)
    
    return config
```

## Configuration Templates

```python
# Template 1: High-performance sparse network
performance_config = pynlm.createDefaultConfig()
performance_config.setInt("brain.neuron_count", 2000)
performance_config.setDouble("brain.synapse_density", 0.05)  # Sparse connections
performance_config.setBool("plasticity.stdp.enable", True)
performance_config.setDouble("plasticity.stdp.learning_rate", 0.001)

# Template 2: Developmental brain
developmental_config = pynlm.createDefaultConfig()
developmental_config.setInt("brain.neuron_count", 1000)
developmental_config.setBool("development.enable", True)
developmental_config.setDouble("development.critical_period_length", 200.0)
developmental_config.setBool("neuromod.curiosity.enable", True)
developmental_config.setBool("neuromod.novelty.enable", True)

# Template 3: Memory-focused network
memory_config = pynlm.createDefaultConfig()
memory_config.setInt("brain.neuron_count", 3000)
memory_config.setBool("neuromod.curiosity.enable", False)  # Focus on memory
memory_config.setBool("neuromod.novelty.enable", False)
memory_config.setDouble("memory.working_memory.capacity", 100.0)
memory_config.setDouble("memory.episodic_memory.capacity", 50.0)
```

---

# Event-Driven Simulation Monitoring

## Custom Event Handler Implementation

```python
import pynlm

class SimulationLogger:
    """Event handler that logs simulation events."""
    
    def __init__(self, log_file="simulation.log"):
        self.log_file = log_file
        self.event_count = 0
        self.start_time = 0.0
        self.spike_events = []
        self.plasticity_events = []
        self.action_events = []
    
    def onStep(self, step):
        """Called when simulation step completes."""
        self.event_count += 1
        with open(self.log_file, 'a') as f:
            f.write(f"Step {step} completed\n")
    
    def onNeuronSpike(self, neuron, region):
        """Called when a neuron spikes."""
        self.spike_events.append((neuron, region))
        with open(self.log_file, 'a') as f:
            f.write(f"Neuron {neuron} spiked in region {region}\n")
    
    def onPlasticityUpdate(self, pre_neuron, post_neuron, weight_change):
        """Called when synaptic plasticity occurs."""
        self.plasticity_events.append((pre_neuron, post_neuron, weight_change))
        with open(self.log_file, 'a') as f:
            f.write(f"Plasticity: {pre_neuron} -> {post_neuron}, Δw={weight_change:.6f}\n")
    
    def onActionProduced(self, action):
        """Called when brain produces an action."""
        self.action_events.append(action)
        with open(self.log_file, 'a') as f:
            f.write(f"Action produced: {action}\n")
    
    def onAgentUpdate(self, agent_state):
        """Called when agent state updates."""
        with open(self.log_file, 'a') as f:
            f.write(f"Agent state: {agent_state}\n")
    
    def get_statistics(self):
        """Get simulation statistics."""
        return {
            'total_events': self.event_count,
            'spike_events': len(self.spike_events),
            'plasticity_events': len(self.plasticity_events),
            'action_events': len(self.action_events),
            'avg_spikes_per_step': len(self.spike_events) / max(self.event_count, 1),
            'avg_plasticity_per_step': len(self.plasticity_events) / max(self.event_count, 1)
        }
class PerformanceMonitor:
    """Monitor simulation performance and detect anomalies."""
    
    def __init__(self, spike_threshold=100, plasticity_threshold=50):
        self.spike_threshold = spike_threshold
        self.plasticity_threshold = plasticity_threshold
        self.step_count = 0
        self.spike_count = 0
        self.plasticity_count = 0
        self.performance_issues = []
    
    def onStep(self, step):
        """Monitor step performance."""
        self.step_count += 1
        
        # Check for performance issues
        if self.spike_count > self.spike_threshold:
            self.performance_issues.append({
                'step': step,
                'type': 'high_activity',
                'spike_count': self.spike_count,
                'message': f"High spike activity: {self.spike_count} spikes in step {step}"
            })
        
        if self.plasticity_count > self.plasticity_threshold:
            self.performance_issues.append({
                'step': step,
                'type': 'high_plasticity',
                'plasticity_count': self.plasticity_count,
                'message': f"High plasticity: {self.plasticity_count} events in step {step}"
            })
        
        # Reset counters for next step
        self.spike_count = 0
        self.plasticity_count = 0
    
    def onNeuronSpike(self, neuron, region):
        """Count spike events."""
        self.spike_count += 1
    
    def onPlasticityUpdate(self, pre_neuron, post_neuron, weight_change):
        """Count plasticity events."""
        self.plasticity_count += 1
    
    def get_performance_report(self):
        """Get performance report."""
        if self.step_count == 0:
            return {'message': 'No simulation steps completed'}
        
        avg_spikes_per_step = self.spike_count / self.step_count
        avg_plasticity_per_step = self.plasticity_count / self.step_count
        
        return {
            'total_steps': self.step_count,
            'avg_spikes_per_step': avg_spikes_per_step,
            'avg_plasticity_per_step': avg_plasticity_per_step,
            'performance_issues': self.performance_issues,
            'issue_count': len(self.performance_issues)
        }
# Integration with NLM simulation
import pynlm

def run_monitored_simulation(config_file="config.json"):
    """Run simulation with comprehensive monitoring."""
    
    # Create brain and monitor
    config = pynlm.createDefaultConfig()
    
    # Load advanced configuration if file exists
    import os
    if os.path.exists(config_file):
        import json
        with open(config_file, 'r') as f:
            config_data = json.load(f)
        
        for key, value in config_data.items():
            if isinstance(value, bool):
                config.setBool(key, value)
            elif isinstance(value, int):
                config.setInt(key, value)
            elif isinstance(value, float):
                config.setDouble(key, value)
            elif isinstance(value, str):
                config.setString(key, value)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Create monitors
    logger = SimulationLogger("advanced_simulation.log")
    monitor = PerformanceMonitor()
    
    # Create agent interface for world interaction
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    world.setAgentStart(10.0, 10.0)
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable advanced features
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    agent.enableDevelopment(True)
    
    # Run simulation with monitoring
    for step in range(1000):
        world.update(0.1)
        
        # Process sensory input
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        # Step the brain
        brain.step(step)
        
        # Decode action
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Apply reward modulation
        reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
        agent.applyRewardModulation(reward, 0.0)
        
        # Update development
        agent.updateDevelopment(0.1)
        
        # Print progress periodically
        if step % 100 == 0:
            stats = monitor.get_performance_report()
            print(f"Step {step}: {brain.getFiringNeuronCount()} firing neurons, "
                  f"{agent.getCuriosityLevel():.3f} curiosity, {agent.getNoveltyLevel():.3f} novelty")
    
    # Get final statistics
    logger_stats = logger.get_statistics()
    monitor_stats = monitor.get_performance_report()
    
    print("\n=== Simulation Complete ===")
    print(f"Logger Statistics: {logger_stats}")
    print(f"Performance Statistics: {monitor_stats}")
    
    return brain, world, agent, logger, monitor
# Run the advanced simulation
if __name__ == "__main__":
    brain, world, agent, logger, monitor = run_monitored_simulation()
```

---

# Performance Profiling and Optimization

## Performance Profiling

```python
import pynlm
import time
import json
import os

class ComprehensiveProfiler:
    """Comprehensive performance profiler for NLM simulations."""
    
    def __init__(self, output_file="profiler_results.json"):
        self.output_file = output_file
        self.profilers = {}
        self.benchmarks = {}
        self.timers = {}
        self.memory_usage = []
        self.cpu_usage = []
    
    def add_profiler(self, name, profiler):
        """Add a performance profiler."""
        self.profilers[name] = profiler
    
    def add_benchmark(self, name, benchmark):
        """Add a benchmark test."""
        self.benchmarks[name] = benchmark
    
    def start_timer(self, name):
        """Start a timer."""
        self.timers[name] = time.time()
    
    def stop_timer(self, name):
        """Stop a timer and record duration."""
        if name in self.timers:
            duration = time.time() - self.timers[name]
            del self.timers[name]
            return duration
        return None
    
    def record_memory_usage(self):
        """Record current memory usage."""
        try:
            import psutil
            process = psutil.Process()
            memory_info = process.memory_info()
            self.memory_usage.append(memory_info.rss / 1024 / 1024)  # MB
            self.cpu_usage.append(process.cpu_percent())
        except ImportError:
            print("psutil not available for memory monitoring")
    
    def run_comprehensive_benchmark(self, config, num_steps=1000):
        """Run comprehensive benchmarks."""
        results = {}
        
        # Benchmark brain initialization
        start_time = time.time()
        brain = pynlm.createBrain(config)
        brain.initialize()
        init_time = time.time() - start_time
        results['initialization'] = {
            'time': init_time,
            'neurons': brain.getTotalNeuronCount(),
            'synapses': brain.getTotalSynapseCount()
        }
        
        # Benchmark simulation steps
        profiler = pynlm.PerformanceProfiler()
        profiler.start()
        
        for step in range(num_steps):
            brain.step(step)
            profiler.record_sample(brain.getFiringNeuronCount())
        
        profiler.stop()
        results['simulation'] = {
            'profiler_stats': profiler.get_statistics(),
            'total_steps': num_steps,
            'avg_firing_rate': profiler.get_samples_per_second()
        }
        
        # Record memory usage
        self.record_memory_usage()
        results['memory'] = {
            'current_usage_mb': self.memory_usage[-1] if self.memory_usage else 0,
            'peak_usage_mb': max(self.memory_usage) if self.memory_usage else 0
        }
        
        return results
    
    def export_results(self, results):
        """Export profiling results to file."""
        # Combine all results
        full_results = {
            'profilers': {name: prof.get_statistics() for name, prof in self.profilers.items()},
            'benchmarks': {name: bench.getResults() for name, bench in self.benchmarks.items()},
            'timers': self.timers,
            'memory_usage': self.memory_usage,
            'cpu_usage': self.cpu_usage,
            'simulation_results': results
        }
        
        with open(self.output_file, 'w') as f:
            json.dump(full_results, f, indent=2)
        
        print(f"Profiling results exported to {self.output_file}")
    
    def generate_report(self, results):
        """Generate human-readable performance report."""
        report = []
        
        report.append("=== NLM Performance Report ===")
        report.append(f"Initialization time: {results.get('initialization', {}).get('time', 0):.3f} seconds")
        
        sim_results = results.get('simulation', {})
        profiler_stats = sim_results.get('profiler_stats', {})
        report.append(f"Simulation: {sim_results.get('total_steps', 0)} steps")
        report.append(f"Average firing rate: {profiler_stats.get('samples_per_second', 0):.1f} Hz")
        
        memory_results = results.get('memory', {})
        report.append(f"Memory usage: {memory_results.get('current_usage_mb', 0):.1f} MB (peak: {memory_results.get('peak_usage_mb', 0):.1f} MB)")
        
        # Add profiler-specific statistics
        for name, prof in self.profilers.items():
            stats = prof.get_statistics()
            report.append(f"\n{name} Profiler:")
            report.append(f"  Duration: {stats.get('duration', 0):.3f} seconds")
            report.append(f"  Samples: {stats.get('samples', 0)}")
            report.append(f"  Average: {stats.get('average', 0):.2f}")
        
        return "\n".join(report)

# Example usage
if __name__ == "__main__":
    # Create profiler
    profiler = ComprehensiveProfiler("advanced_profiler.json")
    
    # Add NLM-specific profilers
    brain_profiler = pynlm.PerformanceProfiler()
    benchmark = pynlm.Benchmark()
    
    profiler.add_profiler("brain", brain_profiler)
    profiler.add_benchmark("initialization", benchmark)
    
    # Create configuration
    config = pynlm.createDefaultConfig()
    config.setInt("brain.neuron_count", 1000)
    config.setDouble("brain.synapse_density", 0.1)
    
    # Run comprehensive benchmark
    start_time = time.time()
    results = profiler.run_comprehensive_benchmark(config, num_steps=1000)
    total_time = time.time() - start_time
    
    results['total_time'] = total_time
    profiler.export_results(results)
    
    # Generate and print report
    report = profiler.generate_report(results)
    print(report)
```

---

# Visualization and Analysis

## Brain Visualization and Analysis

```python
import pynlm
import matplotlib.pyplot as plt
import numpy as np
from typing import List, Tuple

class BrainAnalyzer:
    """Comprehensive brain analysis and visualization tools."""
    
    def __init__(self, visualizer=None):
        self.visualizer = visualizer
        self.spike_history = []
        self.weight_history = []
        self.neuron_activity = {}
        self.network_metrics = {}
    
    def record_spike_events(self, spikes: List[Tuple[int, int]], brain):
        """Record spike events for analysis."""
        self.spike_history.extend(spikes)
        
        # Update neuron activity statistics
        for neuron_id, region_id in spikes:
            if neuron_id not in self.neuron_activity:
                self.neuron_activity[neuron_id] = {
                    'region': region_id,
                    'spike_count': 0,
                    'last_spike_time': 0,
                    'firing_rate': 0.0
                }
            self.neuron_activity[neuron_id]['spike_count'] += 1
        
        # Calculate firing rates
        self._calculate_firing_rates(brain)
    
    def record_plasticity_events(self, plasticity_events: List[Tuple[int, int, float]]):
        """Record plasticity events for analysis."""
        self.weight_history.extend(plasticity_events)
    
    def _calculate_firing_rates(self, brain):
        """Calculate firing rates for all neurons."""
        total_steps = max(1, len(self.spike_history))
        
        for neuron_id, activity in self.neuron_activity.items():
            activity['firing_rate'] = activity['spike_count'] / total_steps
    
    def analyze_network_topology(self, brain):
        """Analyze network topology and connectivity."""
        regions = brain.getRegions()
        
        topology = {
            'region_count': len(regions),
            'total_neurons': brain.getTotalNeuronCount(),
            'total_synapses': brain.getTotalSynapseCount(),
            'synapse_density': brain.getTotalSynapseCount() / max(brain.getTotalNeuronCount(), 1),
            'region_neuron_counts': {},
            'region_synapse_counts': {},
            'connectivity_metrics': {}
        }
        
        # Analyze each region
        for region in regions:
            region_id = region.getId()
            neurons = region.getAllNeurons()
            
            topology['region_neuron_counts'][region_id.value] = len(neurons)
            topology['region_synapse_counts'][region_id.value] = len(region.getSynapses())
        
        # Calculate connectivity metrics
        total_region_synapses = sum(topology['region_synapse_counts'].values())
        topology['connectivity_metrics']['average_synapses_per_region'] = (
            total_region_synapses / max(len(topology['region_synapse_counts']), 1)
        )
        
        self.network_metrics = topology
        return topology
    
    def analyze_plasticity_patterns(self):
        """Analyze plasticity patterns and learning dynamics."""
        if not self.weight_history:
            return None
        
        # Extract weight changes
        weight_changes = [event[2] for event in self.weight_history]
        
        # Calculate plasticity statistics
        analysis = {
            'total_plasticity_events': len(self.weight_history),
            'average_weight_change': np.mean(weight_changes),
            'max_weight_change': np.max(weight_changes),
            'min_weight_change': np.min(weight_changes),
            'std_weight_change': np.std(weight_changes),
            'potentiation_count': sum(1 for change in weight_changes if change > 0.01),
            'depression_count': sum(1 for change in weight_changes if change < -0.01),
            'stable_count': sum(1 for change in weight_changes if abs(change) < 0.01),
            'learning_efficiency': len([c for c in weight_changes if abs(c) > 0.001]) / max(len(weight_changes), 1)
        }
        
        return analysis
    
    def generate_spike_raster(self, save_path=None):
        """Generate spike raster plot."""
        if not self.spike_history:
            print("No spike data available for raster plot")
            return None
        
        # Organize spikes by time
        spike_times = [event[0] for event in self.spike_history]
        neuron_ids = [event[1] for event in self.spike_history]
        
        plt.figure(figsize=(12, 8))
        
        # Create raster plot
        for i, (t, n) in enumerate(zip(spike_times, neuron_ids)):
            plt.plot(t, n, 'k.', markersize=1, alpha=0.6)
        
        plt.xlabel('Time (steps)')
        plt.ylabel('Neuron ID')
        plt.title('Spike Raster Plot')
        plt.grid(True, alpha=0.3)
        
        if save_path:
            plt.savefig(save_path, dpi=150, bbox_inches='tight')
        
        plt.show()
        return plt.gcf()
    
    def plot_neuron_activity_distribution(self, save_path=None):
        """Plot neuron activity distribution."""
        if not self.neuron_activity:
            print("No neuron activity data available")
            return None
        
        firing_rates = [activity['firing_rate'] for activity in self.neuron_activity.values()]
        
        plt.figure(figsize=(10, 6))
        
        # Plot histogram of firing rates
        plt.hist(firing_rates, bins=50, alpha=0.7, edgecolor='black')
        plt.xlabel('Firing Rate (spikes/step)')
        plt.ylabel('Number of Neurons')
        plt.title('Neuron Activity Distribution')
        plt.grid(True, alpha=0.3)
        
        if save_path:
            plt.savefig(save_path, dpi=150, bbox_inches='tight')
        
        plt.show()
        return plt.gcf()
    
    def create_comprehensive_dashboard(self, brain, save_path=None):
        """Create comprehensive analysis dashboard."""
        fig, axes = plt.subplots(2, 3, figsize=(18, 12))
        fig.suptitle('NLM Brain Analysis Dashboard', fontsize=16)
        
        # 1. Spike raster plot
        if self.spike_history:
            spike_times = [event[0] for event in self.spike_history]
            neuron_ids = [event[1] for event in self.spike_history]
            
            axes[0, 0].scatter(spike_times[::10], neuron_ids[::10], s=1, alpha=0.5)
            axes[0, 0].set_xlabel('Time (steps)')
            axes[0, 0].set_ylabel('Neuron ID')
            axes[0, 0].set_title('Spike Raster (downsampled)')
            axes[0, 0].grid(True, alpha=0.3)
        
        # 2. Neuron activity distribution
        if self.neuron_activity:
            firing_rates = [activity['firing_rate'] for activity in self.neuron_activity.values()]
            axes[0, 1].hist(firing_rates, bins=30, alpha=0.7, edgecolor='black')
            axes[0, 1].set_xlabel('Firing Rate')
            axes[0, 1].set_ylabel('Count')
            axes[0, 1].set_title('Activity Distribution')
            axes[0, 1].grid(True, alpha=0.3)
        
        # 3. Network topology
        if self.network_metrics:
            topology = self.network_metrics
            regions = list(topology['region_neuron_counts'].keys())
            neuron_counts = list(topology['region_neuron_counts'].values())
            
            axes[0, 2].bar(regions, neuron_counts, alpha=0.7)
            axes[0, 2].set_xlabel('Region ID')
            axes[0, 2].set_ylabel('Neuron Count')
            axes[0, 2].set_title('Region Neuron Distribution')
            axes[0, 2].grid(True, alpha=0.3)
        
        # 4. Plasticity analysis
        if self.weight_history:
            weight_changes = [event[2] for event in self.weight_history]
            axes[1, 0].hist(weight_changes, bins=50, alpha=0.7, edgecolor='black')
            axes[1, 0].set_xlabel('Weight Change')
            axes[1, 0].set_ylabel('Count')
            axes[1, 0].set_title('Plasticity Distribution')
            axes[1, 0].grid(True, alpha=0.3)
        
        # 5. Brain statistics
        axes[1, 1].axis('off')
        axes[1, 1].text(0.1, 0.9, 'Brain Statistics:', fontsize=14, fontweight='bold')
        
        if brain:
            stats_text = f"Total Neurons: {brain.getTotalNeuronCount()}\n"
            stats_text += f"Total Synapses: {brain.getTotalSynapseCount()}\n"
            stats_text += f"Firing Rate: {brain.getAverageFiringRate():.4f}\n"
            stats_text += f"E/I Ratio: {brain.getExcitationInhibitionRatio():.2f}\n"
            stats_text += f"Total Spikes: {brain.getTotalSpikeCount()}\n"
            stats_text += f"Active Neurons: {brain.getActiveNeuronCount()}\n"
            
            axes[1, 1].text(0.1, 0.8, stats_text, fontsize=12, fontfamily='monospace')
        
        # 6. Time series visualization
        if self.spike_history:
            spike_times = [event[0] for event in self.spike_history]
            axes[1, 2].plot(spike_times[::100], 'k.', markersize=1, alpha=0.5)
            axes[1, 2].set_xlabel('Time (steps)')
            axes[1, 2].set_ylabel('Spike Events')
            axes[1, 2].set_title('Spike Time Series')
            axes[1, 2].grid(True, alpha=0.3)
        
        plt.tight_layout()
        
        if save_path:
            plt.savefig(save_path, dpi=150, bbox_inches='tight')
        
        plt.show()
        return fig

# Advanced analysis example
if __name__ == "__main__":
    # Initialize visualizer
    visualizer = pynlm.BrainVisualizer()
    
    # Create brain
    config = pynlm.createDefaultConfig()
    config.setInt("brain.neuron_count", 500)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Initialize analyzer
    analyzer = BrainAnalyzer(visualizer)
    
    # Run simulation and collect data
    for step in range(500):
        brain.step(step)
        
        # Simulate spike and plasticity events (in real usage, these come from brain)
        # For demonstration, we'll simulate some events
        if step % 10 == 0:
            # Simulate neuron spikes
            spike_events = [(step, i % brain.getTotalNeuronCount()) for i in range(5)]
            analyzer.record_spike_events(spike_events, brain)
        
        if step % 20 == 0:
            # Simulate plasticity events
            plasticity_events = [(i, (i + 1) % brain.getTotalNeuronCount(), (i % 10) * 0.01 - 0.005) 
                               for i in range(3)]
            analyzer.record_plasticity_events(plasticity_events)
    
    # Analyze network topology
    topology = analyzer.analyze_network_topology(brain)
    print("=== Network Topology Analysis ===")
    print(f"Regions: {topology['region_count']}")
    print(f"Total Neurons: {topology['total_neurons']}")
    print(f"Total Synapses: {topology['total_synapses']}")
    print(f"Synapse Density: {topology['synapse_density']:.4f}")
    
    # Analyze plasticity patterns
    plasticity_analysis = analyzer.analyze_plasticity_patterns()
    if plasticity_analysis:
        print("\n=== Plasticity Analysis ===")
        print(f"Total Plasticity Events: {plasticity_analysis['total_plasticity_events']}")
        print(f"Average Weight Change: {plasticity_analysis['average_weight_change']:.6f}")
        print(f"Learning Efficiency: {plasticity_analysis['learning_efficiency']:.2%}")
    
    # Generate visualizations
    print("\nGenerating visualizations...")
    
    # Create spike raster plot
    analyzer.generate_spike_raster("spike_raster.png")
    
    # Plot neuron activity distribution
    analyzer.plot_neuron_activity_distribution("activity_distribution.png")
    
    # Create comprehensive dashboard
    analyzer.create_comprehensive_dashboard(brain, "analysis_dashboard.png")
    
    print("Analysis complete! Visualizations saved.")
```

---

# Memory and State Management

## Advanced State Management

```python
import pynlm
import json
import pickle
import hashlib
from typing import Dict, Any, Optional
import os

class StateManager:
    """Advanced state management for NLM simulations."""
    
    def __init__(self, state_directory="states", backup_enabled=True):
        self.state_directory = state_directory
        self.backup_enabled = backup_enabled
        self.state_hashes = {}
        
        # Create state directory if it doesn't exist
        os.makedirs(state_directory, exist_ok=True)
    
    def save_brain_state(self, brain, filepath, metadata: Optional[Dict[str, Any]] = None):
        """Save brain state with validation and metadata."""
        try:
            # Validate inputs
            if not brain:
                raise ValueError("Brain cannot be None")
            
            if not filepath:
                raise ValueError("Filepath cannot be empty")
            
            # Ensure filepath is within state directory
            full_path = os.path.join(self.state_directory, filepath)
            
            # Create backup if enabled
            if self.backup_enabled:
                self._create_backup(full_path)
            
            # Save state
            if not brain.save(full_path):
                raise RuntimeError(f"Failed to save brain state to {full_path}")
            
            # Compute and store hash
            state_hash = self._compute_file_hash(full_path)
            
            # Create metadata
            if metadata is None:
                metadata = {}
            
            metadata.update({
                'timestamp': str(__import__('datetime').datetime.now()),
                'neuron_count': brain.getTotalNeuronCount(),
                'synapse_count': brain.getTotalSynapseCount(),
                'state_hash': state_hash,
                'version': '1.0'
            })
            
            # Save metadata
            metadata_file = full_path + '.meta'
            with open(metadata_file, 'w') as f:
                json.dump(metadata, f, indent=2)
            
            # Update state hash map
            self.state_hashes[filepath] = state_hash
            
            return True, metadata
            
        except Exception as e:
            raise RuntimeError(f"Error saving brain state: {e}")
    
    def load_brain_state(self, brain, filepath):
        """Load brain state with validation."""
        try:
            # Validate inputs
            if not brain:
                raise ValueError("Brain cannot be None")
            
            if not filepath:
                raise ValueError("Filepath cannot be empty")
            
            # Check if file exists
            full_path = os.path.join(self.state_directory, filepath)
            if not os.path.exists(full_path):
                raise FileNotFoundError(f"State file not found: {full_path}")
            
            # Check metadata
            metadata_file = full_path + '.meta'
            if os.path.exists(metadata_file):
                with open(metadata_file, 'r') as f:
                    metadata = json.load(f)
            else:
                metadata = {}
            
            # Load state
            if not brain.load(full_path):
                raise RuntimeError(f"Failed to load brain state from {full_path}")
            
            # Verify state integrity
            current_hash = self._compute_file_hash(full_path)
            if filepath in self.state_hashes and self.state_hashes[filepath] != current_hash:
                print(f"Warning: State integrity check failed for {filepath}")
            
            return True, metadata
            
        except Exception as e:
            raise RuntimeError(f"Error loading brain state: {e}")
    
    def _create_backup(self, filepath):
        """Create backup of state file."""
        try:
            if os.path.exists(filepath):
                backup_path = filepath + '.backup'
                import shutil
                shutil.copy2(filepath, backup_path)
                print(f"Created backup at {backup_path}")
        except Exception as e:
            print(f"Warning: Failed to create backup: {e}")
    
    def _compute_file_hash(self, filepath):
        """Compute SHA256 hash of file."""
        import hashlib
        sha256_hash = hashlib.sha256()
        
        with open(filepath, "rb") as f:
            for byte_block in iter(lambda: f.read(4096), b""):
                sha256_hash.update(byte_block)
        
        return sha256_hash.hexdigest()
    
    def compare_states(self, brain, filepath1, filepath2):
        """Compare two brain states."""
        try:
            # Load both states
            temp_brain1 = pynlm.createBrain(pynlm.createDefaultConfig())
            temp_brain2 = pynlm.createBrain(pynlm.createDefaultConfig())
            
            self.load_brain_state(temp_brain1, filepath1)
            self.load_brain_state(temp_brain2, filepath2)
            
            # Compare statistics
            comparison = {
                'file1_neuron_count': temp_brain1.getTotalNeuronCount(),
                'file1_synapse_count': temp_brain1.getTotalSynapseCount(),
                'file1_total_spikes': temp_brain1.getTotalSpikeCount(),
                'file2_neuron_count': temp_brain2.getTotalNeuronCount(),
                'file2_synapse_count': temp_brain2.getTotalSynapseCount(),
                'file2_total_spikes': temp_brain2.getTotalSpikeCount(),
                'neuron_count_match': temp_brain1.getTotalNeuronCount() == temp_brain2.getTotalNeuronCount(),
                'synapse_count_match': temp_brain1.getTotalSynapseCount() == temp_brain2.getTotalSynapseCount(),
                'spike_count_match': temp_brain1.getTotalSpikeCount() == temp_brain2.getTotalSpikeCount()
            }
            
            return comparison
            
        except Exception as e:
            raise RuntimeError(f"Error comparing states: {e}")
    
    def list_saved_states(self):
        """List all saved states in directory."""
        try:
            states = []
            
            for filename in os.listdir(self.state_directory):
                if filename.endswith('.bin') or filename.endswith('.meta'):
                    filepath = os.path.join(self.state_directory, filename)
                    stat_info = os.stat(filepath)
                    
                    state_info = {
                        'filename': filename,
                        'full_path': filepath,
                        'size_bytes': stat_info.st_size,
                        'created_time': stat_info.st_ctime,
                        'modified_time': stat_info.st_mtime
                    }
                    
                    # Add metadata if available
                    if filename.endswith('.bin') and os.path.exists(filename + '.meta'):
                        with open(filename + '.meta', 'r') as f:
                            metadata = json.load(f)
                        state_info.update(metadata)
                    
                    states.append(state_info)
            
            return sorted(states, key=lambda x: x['created_time'], reverse=True)
            
        except Exception as e:
            raise RuntimeError(f"Error listing saved states: {e}")
    
    def cleanup_old_states(self, days_threshold=30):
        """Clean up old state files."""
        try:
            import time
            current_time = time.time()
            files_deleted = []
            
            for filename in os.listdir(self.state_directory):
                if filename.endswith('.bin') or filename.endswith('.meta'):
                    filepath = os.path.join(self.state_directory, filename)
                    file_age = current_time - os.path.getctime(filepath)
                    
                    if file_age > (days_threshold * 24 * 60 * 60):
                        os.remove(filepath)
                        files_deleted.append(filepath)
            
            print(f"Cleaned up {len(files_deleted)} old state files")
            return files_deleted
            
        except Exception as e:
            raise RuntimeError(f"Error cleaning up old states: {e}")
class PersistentSimulation:
    """Run persistent simulations with state management."""
    
    def __init__(self, state_manager, config):
        self.state_manager = state_manager
        self.config = config
        self.brain = None
        self.world = None
        self.agent = None
    
    def start_session(self, session_name, auto_save=True, save_interval=100):
        """Start a new simulation session."""
        try:
            # Create brain and world
            self.brain = pynlm.createBrain(self.config)
            self.brain.initialize()
            
            self.world = pynlm.createSimpleWorld()
            self.world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
            self.world.reset()
            self.world.setAgentStart(10.0, 10.0)
            
            self.agent = pynlm.createAgentBrain(self.brain)
            self.agent.initialize(self.world)
            
            # Enable advanced features
            self.agent.enableRewardModulation(True)
            self.agent.enableCuriosity(True)
            self.agent.enableDevelopment(True)
            
            # Save initial state
            session_state = {
                'session_name': session_name,
                'brain_config': self.config,
                'initial_neuron_count': self.brain.getTotalNeuronCount(),
                'initial_synapse_count': self.brain.getTotalSynapseCount()
            }
            
            if auto_save:
                self.state_manager.save_brain_state(self.brain, f"{session_name}_initial.bin", session_state)
            
            return True, session_state
            
        except Exception as e:
            raise RuntimeError(f"Error starting simulation session: {e}")
    
    def run_episode(self, episode_length=500, auto_save=True, save_interval=100):
        """Run a single simulation episode."""
        if not self.brain or not self.agent or not self.world:
            raise RuntimeError("Simulation not initialized. Call start_session() first.")
        
        try:
            # Track episode statistics
            episode_stats = {
                'steps': episode_length,
                'initial_neuron_count': self.brain.getTotalNeuronCount(),
                'initial_synapse_count': self.brain.getTotalSynapseCount(),
                'final_neuron_count': 0,
                'final_synapse_count': 0,
                'total_spikes': 0,
                'max_firing_rate': 0.0,
                'curiosity_levels': [],
                'novelty_levels': []
            }
            
            # Run episode
            for step in range(episode_length):
                # Update world
                self.world.update(0.1)
                
                # Process sensory input
                percept = self.world.getSensoryPercept()
                self.agent.processSensoryInput(percept)
                
                # Step brain
                self.brain.step(step)
                
                # Decode action
                action = self.agent.decodeMotorCommand()
                self.world.applyMotorCommand(action, self.world.getSimulationTime())
                
                # Apply reward modulation
                reward = self.world.getSensoryPercept().getInternal()[0] \
                    if self.world.getSensoryPercept().getInternal() else 0.0
                self.agent.applyRewardModulation(reward, 0.0)
                
                # Update development
                self.agent.updateDevelopment(0.1)
                
                # Collect statistics
                episode_stats['curiosity_levels'].append(self.agent.getCuriosityLevel())
                episode_stats['novelty_levels'].append(self.agent.getNoveltyLevel())
                
                # Auto-save periodically
                if auto_save and save_interval > 0 and (step + 1) % save_interval == 0:
                    checkpoint_file = f"checkpoint_step_{step + 1}.bin"
                    self.state_manager.save_brain_state(self.brain, checkpoint_file)
            
            # Update final statistics
            episode_stats['final_neuron_count'] = self.brain.getTotalNeuronCount()
            episode_stats['final_synapse_count'] = self.brain.getTotalSynapseCount()
            episode_stats['total_spikes'] = self.brain.getTotalSpikeCount()
            episode_stats['max_firing_rate'] = self.brain.getAverageFiringRate()
            
            return True, episode_stats
            
        except Exception as e:
            raise RuntimeError(f"Error running simulation episode: {e}")
    
    def resume_session(self, session_name):
        """Resume a previous simulation session."""
        try:
            # Try to load saved state
            state_file = f"{session_name}_initial.bin"
            
            # Create new brain
            self.brain = pynlm.createBrain(self.config)
            
            # Load saved state
            metadata = self.state_manager.load_brain_state(self.brain, state_file)
            
            # Restore other components
            self.world = pynlm.createSimpleWorld()
            self.world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
            
            self.agent = pynlm.createAgentBrain(self.brain)
            self.agent.initialize(self.world)
            
            return True, metadata
            
        except Exception as e:
            raise RuntimeError(f"Error resuming simulation session: {e}")
    
    def get_session_summary(self, session_name):
        """Get summary of a saved session."""
        try:
            state_file = f"{session_name}_initial.bin"
            states = self.state_manager.list_saved_states()
            
            # Find the session state
            session_state = None
            for state in states:
                if state['filename'] == state_file or state.get('session_name') == session_name:
                    session_state = state
                    break
            
            if not session_state:
                return {'error': f"Session '{session_name}' not found"}
            
            summary = {
                'session_name': session_name,
                'initial_state': session_state,
                'status': 'loaded'
            }
            
            return summary
            
        except Exception as e:
            return {'error': f"Error getting session summary: {e}"}

# Advanced state management example
if __name__ == "__main__":
    # Initialize state manager
    state_manager = StateManager("nlm_states", backup_enabled=True)
    
    # Create configuration
    config = pynlm.createDefaultConfig()
    config.setInt("brain.neuron_count", 1000)
    config.setDouble("brain.synapse_density", 0.1)
    
    # Create persistent simulation
    sim = PersistentSimulation(state_manager, config)
    
    # Start new session
    print("=== Starting New Session ===")
    success, session_info = sim.start_session("learning_session", auto_save=True)
    
    if success:
        print(f"Session started: {session_info['session_name']}")
        print(f"Initial brain: {session_info['initial_neuron_count']} neurons, {session_info['initial_synapse_count']} synapses")
        
        # Run episodes
        for episode in range(3):
            print(f"\n=== Running Episode {episode + 1} ===")
            
            success, episode_stats = sim.run_episode(episode_length=200, auto_save=True, save_interval=50)
            
            if success:
                print(f"Episode completed: {episode_stats['steps']} steps")
                print(f"  Final neurons: {episode_stats['final_neuron_count']}")
                print(f"  Total spikes: {episode_stats['total_spikes']}")
                print(f"  Max firing rate: {episode_stats['max_firing_rate']:.4f}")
                
                # Save episode summary
                episode_summary = {
                    'episode': episode + 1,
                    'statistics': episode_stats,
                    'curiosity_avg': sum(episode_stats['curiosity_levels']) / len(episode_stats['curiosity_levels']),
                    'novelty_avg': sum(episode_stats['novelty_levels']) / len(episode_stats['novelty_levels'])
                }
                
                state_manager.save_brain_state(sim.brain, f"episode_{episode + 1}_summary.bin", episode_summary)
    
    # Resume previous session if available
    print("\n=== Testing Session Resume ===")
    summary = sim.get_session_summary("learning_session")
    if 'error' not in summary:
        print(f"Session found: {summary['session_name']}")
        print("Session management successful!")
    else:
        print(f"Session error: {summary['error']}")
    
    # List all saved states
    print("\n=== Saved States ===")
    all_states = state_manager.list_saved_states()
    for i, state in enumerate(all_states[-5:], 1):  # Show last 5 states
        print(f"{i}. {state['filename']} ({state['size_bytes']:,} bytes)")
        if 'neuron_count' in state:
            print(f"   Neurons: {state['neuron_count']}, Synapses: {state.get('synapse_count', 0)}")
    
    print("\nAdvanced state management complete!")
```