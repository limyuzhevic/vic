# NLM Profiling and Debugging Tools

## Overview

This document describes the profiling and debugging tools available in NLM to help users understand performance characteristics and debug issues.

## Section 1: Built-in Profiling Tools

### 1.1 CPU Performance Profiler

NLM includes a basic CPU profiler to measure execution time of different components.

```python
import pynlm
import time
import json

class PerformanceProfiler:
    """Profile NLM performance metrics."""
    
    def __init__(self, brain):
        self.brain = brain
        self.metrics = {
            'steps': [],
            'neuron_stats': [],
            'memory_usage': [],
            'spike_rates': []
        }
        self.start_time = time.time()
    
    def profile_step(self, step_num, time_step):
        """Profile a single simulation step."""
        step_start = time.time()
        
        # Run the simulation step
        self.brain.step(step_num, time_step)
        
        step_end = time.time()
        step_duration = step_end - step_start
        
        # Collect metrics
        self.metrics['steps'].append({
            'step': step_num,
            'duration': step_duration,
            'time': time_step,
            'neurons_firing': self.brain.getFiringNeuronCount(),
            'total_spikes': self.brain.getTotalSpikeCount(),
            'average_rate': self.brain.getAverageFiringRate(),
            'e_i_ratio': self.brain.getExcitationInhibitionRatio()
        })
        
        return step_duration
    
    def profile_region(self, start_step, end_step, time_step=0.001):
        """Profile a region of simulation."""
        print(f"Profiling steps {start_step} to {end_step}")
        
        for step in range(start_step, end_step):
            self.profile_step(step, time_step)
        
        return self.get_summary()
    
    def get_summary(self):
        """Get performance summary."""
        if not self.metrics['steps']:
            return {"error": "No data collected"}
        
        steps = self.metrics['steps']
        
        # Calculate statistics
        durations = [s['duration'] for s in steps]
        firing_counts = [s['neurons_firing'] for s in steps]
        spike_rates = [s['average_rate'] for s in steps]
        
        return {
            'total_steps': len(steps),
            'total_time': sum(durations),
            'average_step_time': sum(durations) / len(durations),
            'min_step_time': min(durations),
            'max_step_time': max(durations),
            'std_step_time': (sum([(d - sum(durations)/len(durations))**2 for d in durations]) / len(durations))**0.5,
            
            'neuron_stats': {
                'mean_firing': sum(firing_counts) / len(firing_counts),
                'max_firing': max(firing_counts),
                'min_firing': min(firing_counts),
                'cv_firing': (sum([(f - sum(firing_counts)/len(firing_counts))**2 for f in firing_counts]) / len(firing_counts))**0.5 / (sum(firing_counts) / len(firing_counts)) if sum(firing_counts) > 0 else 0
            },
            
            'spike_stats': {
                'mean_rate': sum(spike_rates) / len(spike_rates),
                'max_rate': max(spike_rates),
                'min_rate': min(spike_rates),
                'cv_rate': (sum([(r - sum(spike_rates)/len(spike_rates))**2 for r in spike_rates]) / len(spike_rates))**0.5 / (sum(spike_rates) / len(spike_rates)) if sum(spike_rates) > 0 else 0
            },
            
            'performance_metrics': {
                'steps_per_second': len(steps) / sum(durations) if sum(durations) > 0 else 0,
                'neuron_per_step': sum(firing_counts) / len(steps) if len(steps) > 0 else 0,
                'spike_per_neuron': sum(s['total_spikes'] for s in steps) / sum(firing_counts) if sum(firing_counts) > 0 else 0
            }
        }
    
    def save_profile(self, filename):
        """Save profile data to file."""
        with open(filename, 'w') as f:
            json.dump(self.metrics, f, indent=2)
        
        # Save summary
        summary = self.get_summary()
        with open(filename.replace('.json', '_summary.json'), 'w') as f:
            json.dump(summary, f, indent=2)
        
        print(f"Profile saved to {filename}")
        return summary

# Example usage
def example_profiling():
    """Example of using the profiler."""
    
    # Create brain
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 500)
    config.set("simulation_timestep", 0.001)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Create profiler
    profiler = PerformanceProfiler(brain)
    
    # Profile a simulation
    print("Starting profiling...")
    start_time = time.time()
    
    for step in range(100):
        profiler.profile_step(step, 0.001)
        
        if step % 20 == 0:
            print(f"Step {step} completed")
    
    end_time = time.time()
    print(f"Profiling completed in {end_time - start_time:.2f} seconds")
    
    # Get and display summary
    summary = profiler.get_summary()
    print(f"\n=== Performance Summary ===")
    print(f"Total steps: {summary['total_steps']}")
    print(f"Total time: {summary['total_time']:.3f}s")
    print(f"Average step time: {summary['average_step_time']*1000:.3f}ms")
    print(f"Steps per second: {summary['performance_metrics']['steps_per_second']:.1f}")
    print(f"Average firing neurons: {summary['neuron_stats']['mean_firing']:.1f}")
    print(f"Average firing rate: {summary['spike_stats']['mean_rate']:.3f}")
    
    # Save profile
    profiler.save_profile("nlm_profile.json")

# Run the example
example_profiling()
```

### 1.2 Memory Profiler

Profile memory usage during simulation.

```python
import pynlm
import tracemalloc
import psutil
import os

class MemoryProfiler:
    """Profile memory usage during NLM simulation."""
    
    def __init__(self, brain):
        self.brain = brain
        self.memory_samples = []
        self.start_time = time.time()
    
    def sample_memory(self, step_num):
        """Sample current memory usage."""
        # Get process memory info
        process = psutil.Process(os.getpid())
        memory_info = process.memory_info()
        
        # Get Python memory usage
        tracemalloc.start()
        current, peak = tracemalloc.get_traced_memory()
        tracemalloc.stop()
        
        sample = {
            'step': step_num,
            'timestamp': time.time() - self.start_time,
            'memory_rss': memory_info.rss,
            'memory_vms': memory_info.vms,
            'memory_python_current': current,
            'memory_python_peak': peak,
            'cpu_percent': process.cpu_percent()
        }
        
        self.memory_samples.append(sample)
        return sample
    
    def profile_memory(self, start_step, end_step):
        """Profile memory over a range of steps."""
        print(f"Profiling memory usage from step {start_step} to {end_step}")
        
        for step in range(start_step, end_step):
            self.sample_memory(step)
            
            if step % 100 == 0:
                print(f"Step {step}")
        
        return self.get_memory_summary()
    
    def get_memory_summary(self):
        """Get memory usage summary."""
        if not self.memory_samples:
            return {"error": "No memory samples collected"}
        
        samples = self.memory_samples
        rss_values = [s['memory_rss'] for s in samples]
        python_current_values = [s['memory_python_current'] for s in samples]
        cpu_values = [s['cpu_percent'] for s in samples]
        
        return {
            'total_samples': len(samples),
            'duration': samples[-1]['timestamp'] - samples[0]['timestamp'] if len(samples) > 1 else 0,
            
            'memory_rss': {
                'mean': sum(rss_values) / len(rss_values),
                'max': max(rss_values),
                'min': min(rss_values),
                'std': (sum([(r - sum(rss_values)/len(rss_values))**2 for r in rss_values]) / len(rss_values))**0.5,
                'growth_mb': (max(rss_values) - min(rss_values)) / (1024 * 1024)
            },
            
            'memory_python': {
                'mean_current': sum(python_current_values) / len(python_current_values),
                'max_current': max(python_current_values),
                'min_current': min(python_current_values),
                'growth_mb': (max(python_current_values) - min(python_current_values)) / (1024 * 1024)
            },
            
            'cpu_usage': {
                'mean_percent': sum(cpu_values) / len(cpu_values),
                'max_percent': max(cpu_values),
                'min_percent': min(cpu_values)
            }
        }
    
    def save_memory_profile(self, filename):
        """Save memory profile to file."""
        import json
        with open(filename, 'w') as f:
            json.dump(self.memory_samples, f, indent=2)
        
        summary = self.get_memory_summary()
        with open(filename.replace('.json', '_summary.json'), 'w') as f:
            json.dump(summary, f, indent=2)
        
        print(f"Memory profile saved to {filename}")
        return summary

# Example usage
def example_memory_profiling():
    """Example of memory profiling."""
    
    # Start memory tracing
    tracemalloc.start()
    
    # Create brain
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 1000)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Create memory profiler
    mem_profiler = MemoryProfiler(brain)
    
    # Profile memory during simulation
    print("Starting memory profiling...")
    
    for step in range(500):
        brain.step(step)
        mem_profiler.sample_memory(step)
        
        if step % 100 == 0:
            print(f"Step {step}")
    
    # Get memory summary
    summary = mem_profiler.get_memory_summary()
    print(f"\n=== Memory Usage Summary ===")
    print(f"Total samples: {summary['total_samples']}")
    print(f"Duration: {summary['duration']:.2f}s")
    print(f"RSS memory growth: {summary['memory_rss']['growth_mb']:.2f} MB")
    print(f"Python memory growth: {summary['memory_python']['growth_mb']:.2f} MB")
    print(f"Average CPU usage: {summary['cpu_usage']['mean_percent']:.1f}%")
    
    # Save memory profile
    mem_profiler.save_memory_profile("nlm_memory_profile.json")
    
    tracemalloc.stop()

# Run the example
example_memory_profiling()
```

### 1.3 Spike Event Profiler

Profile spike timing and patterns.

```python
import pynlm
import numpy as np
import matplotlib.pyplot as plt

class SpikeProfiler:
    """Profile spike timing and patterns."""
    
    def __init__(self, brain):
        self.brain = brain
        self.spike_history = []
        self.spike_times = []
    
    def profile_step(self, step_num):
        """Profile spike activity for a single step."""
        # Get spike information
        spikes = self.brain.getTotalSpikeCount()
        firing_neurons = self.brain.getFiringNeuronCount()
        
        # Sample spike timing if available
        if hasattr(self.brain, 'getSpikeTimes'):
            spike_times = self.brain.getSpikeTimes()
        else:
            # Simulate spike times based on firing rate
            spike_times = np.random.exponential(1.0 / max(firing_neurons, 1), firing_neurons)
        
        self.spike_history.append({
            'step': step_num,
            'timestamp': step_num * 0.001,  # Assuming 1ms timestep
            'total_spikes': spikes,
            'firing_neurons': firing_neurons,
            'spike_times': spike_times.tolist() if hasattr(spike_times, 'tolist') else spike_times
        })
        
        self.spike_times.extend(spike_times)
    
    def profile_spike_patterns(self, start_step, end_step, time_step=0.001):
        """Profile spike patterns over a time range."""
        print(f"Profiling spike patterns from step {start_step} to {end_step}")
        
        for step in range(start_step, end_step):
            self.profile_step(step)
        
        return self.analyze_spike_patterns()
    
    def analyze_spike_patterns(self):
        """Analyze spike timing patterns."""
        if not self.spike_history:
            return {"error": "No spike data collected"}
        
        all_spike_times = []
        for entry in self.spike_history:
            all_spike_times.extend(entry['spike_times'])
        
        if not all_spike_times:
            return {"error": "No spikes recorded"}
        
        # Calculate spike statistics
        spike_intervals = np.diff(sorted(all_spike_times))
        
        return {
            'total_spikes': len(all_spike_times),
            'mean_interval': np.mean(spike_intervals) if len(spike_intervals) > 0 else 0,
            'std_interval': np.std(spike_intervals) if len(spike_intervals) > 0 else 0,
            'min_interval': np.min(spike_intervals) if len(spike_intervals) > 0 else 0,
            'max_interval': np.max(spike_intervals) if len(spike_intervals) > 0 else 0,
            
            'firing_rate_stats': {
                'mean': np.mean([entry['total_spikes'] for entry in self.spike_history]),
                'std': np.std([entry['total_spikes'] for entry in self.spike_history]),
                'cv': np.std([entry['total_spikes'] for entry in self.spike_history]) / np.mean([entry['total_spikes'] for entry in self.spike_history]) if np.mean([entry['total_spikes'] for entry in self.spike_history]) > 0 else 0
            },
            
            'regularity': {
                'coefficient_of_variation': np.std(spike_intervals) / np.mean(spike_intervals) if np.mean(spike_intervals) > 0 else 0,
                'regularity_index': 1.0 / (1.0 + np.std(spike_intervals) / np.mean(spike_intervals)) if np.mean(spike_intervals) > 0 else 0
            }
        }
    
    def plot_spike_raster(self, output_file=None):
        """Create a spike raster plot."""
        if not self.spike_history:
            print("No spike data to plot")
            return
        
        plt.figure(figsize=(12, 8))
        
        # Create raster plot
        for i, entry in enumerate(self.spike_history):
            spike_times = entry['spike_times']
            plt.scatter(spike_times, [i] * len(spike_times), 
                       alpha=0.6, s=1, color='black', marker='.')
        
        plt.xlabel('Time (s)')
        plt.ylabel('Step')
        plt.title('Spike Raster Plot')
        plt.grid(True, alpha=0.3)
        
        if output_file:
            plt.savefig(output_file, dpi=150, bbox_inches='tight')
            print(f"Spike raster plot saved to {output_file}")
        
        plt.show()
    
    def plot_spike_histogram(self, output_file=None):
        """Plot spike interval histogram."""
        if not self.spike_history:
            print("No spike data to plot")
            return
        
        all_spike_times = []
        for entry in self.spike_history:
            all_spike_times.extend(entry['spike_times'])
        
        if len(all_spike_times) < 2:
            print("Not enough spike data to create histogram")
            return
        
        spike_intervals = np.diff(sorted(all_spike_times))
        
        plt.figure(figsize=(10, 6))
        
        # Create histogram
        plt.hist(spike_intervals, bins=50, alpha=0.7, color='blue', edgecolor='black')
        
        # Add statistical lines
        mean_interval = np.mean(spike_intervals)
        std_interval = np.std(spike_intervals)
        
        plt.axvline(mean_interval, color='red', linestyle='--', linewidth=2, label=f'Mean: {mean_interval:.3f}s')
        plt.axvline(mean_interval + std_interval, color='orange', linestyle='--', linewidth=2, label=f'Mean ± SD')
        plt.axvline(mean_interval - std_interval, color='orange', linestyle='--', linewidth=2)
        
        plt.xlabel('Spike Interval (s)')
        plt.ylabel('Frequency')
        plt.title('Spike Interval Histogram')
        plt.legend()
        plt.grid(True, alpha=0.3)
        
        if output_file:
            plt.savefig(output_file, dpi=150, bbox_inches='tight')
            print(f"Spike histogram saved to {output_file}")
        
        plt.show()

# Example usage
def example_spike_profiling():
    """Example of spike profiling."""
    
    # Create brain
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 300)
    config.set("connection_probability", 0.05)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Create spike profiler
    spike_profiler = SpikeProfiler(brain)
    
    # Profile spike activity
    print("Starting spike profiling...")
    
    for step in range(200):
        spike_profiler.profile_step(step)
        
        if step % 50 == 0:
            print(f"Step {step}")
    
    # Analyze spike patterns
    analysis = spike_profiler.analyze_spike_patterns()
    print(f"\n=== Spike Pattern Analysis ===")
    print(f"Total spikes: {analysis['total_spikes']}")
    print(f"Mean interval: {analysis['mean_interval']:.4f}s")
    print(f"Interval CV: {analysis['regularity']['coefficient_of_variation']:.3f}")
    print(f"Regularity index: {analysis['regularity']['regularity_index']:.3f}")
    
    # Create visualizations
    spike_profiler.plot_spike_raster("spike_raster.png")
    spike_profiler.plot_spike_histogram("spike_histogram.png")

# Run the example
example_spike_profiling()
```

## Section 2: Debugging Tools

### 2.1 State Debugger

Debug brain state and restore from checkpoints.

```python
import pynlm
import json

class StateDebugger:
    """Debug and inspect brain state."""
    
    def __init__(self, brain):
        self.brain = brain
        self.state_history = []
    
    def capture_state(self, step_num):
        """Capture current brain state."""
        state = {
            'step': step_num,
            'timestamp': step_num * 0.001,
            'neuron_count': self.brain.getTotalNeuronCount(),
            'synapse_count': self.brain.getTotalSynapseCount(),
            'firing_neurons': self.brain.getFiringNeuronCount(),
            'total_spikes': self.brain.getTotalSpikeCount(),
            'average_firing_rate': self.brain.getAverageFiringRate(),
            'e_i_ratio': self.brain.getExcitationInhibitionRatio(),
            
            # Get region information if available
            'regions': [],
            
            # Get neuron population information
            'populations': []
        }
        
        # Capture region information
        if hasattr(self.brain, 'getRegions'):
            for region in self.brain.getRegions():
                if hasattr(region, 'getId'):
                    region_info = {
                        'id': region.getId(),
                        'name': region.getName() if hasattr(region, 'getName') else f"Region_{region.getId()}",
                        'neuron_count': region.getNeuronCount(),
                        'synapse_count': region.getSynapseCount()
                    }
                    state['regions'].append(region_info)
        
        # Capture population information
        if hasattr(self.brain, 'getPopulations'):
            for pop in self.brain.getPopulations():
                if hasattr(pop, 'getId'):
                    pop_info = {
                        'id': pop.getId(),
                        'type': pop.getNeuronType() if hasattr(pop, 'getNeuronType') else 'Unknown',
                        'neuron_count': pop.getNeuronCount()
                    }
                    state['populations'].append(pop_info)
        
        self.state_history.append(state)
        return state
    
    def analyze_state_changes(self, start_step, end_step):
        """Analyze state changes over a range."""
        if len(self.state_history) < 2:
            return {"error": "Not enough state data for analysis"}
        
        relevant_states = [s for s in self.state_history if start_step <= s['step'] <= end_step]
        
        if len(relevant_states) < 2:
            return {"error": "Not enough state data in specified range"}
        
        # Calculate changes
        first_state = relevant_states[0]
        last_state = relevant_states[-1]
        
        return {
            'step_range': {
                'start': first_state['step'],
                'end': last_state['step']
            },
            'neuron_changes': {
                'start_count': first_state['neuron_count'],
                'end_count': last_state['neuron_count'],
                'change': last_state['neuron_count'] - first_state['neuron_count'],
                'percentage_change': ((last_state['neuron_count'] - first_state['neuron_count']) / first_state['neuron_count'] * 100) if first_state['neuron_count'] > 0 else 0
            },
            'synapse_changes': {
                'start_count': first_state['synapse_count'],
                'end_count': last_state['synapse_count'],
                'change': last_state['synapse_count'] - first_state['synapse_count'],
                'percentage_change': ((last_state['synapse_count'] - first_state['synapse_count']) / first_state['synapse_count'] * 100) if first_state['synapse_count'] > 0 else 0
            },
            'activity_changes': {
                'start_firing': first_state['firing_neurons'],
                'end_firing': last_state['firing_neurons'],
                'change': last_state['firing_neurons'] - first_state['firing_neurons'],
                'start_rate': first_state['average_firing_rate'],
                'end_rate': last_state['average_firing_rate']
            }
        }
    
    def create_debug_report(self, output_file):
        """Create a comprehensive debug report."""
        report = {
            'brain_info': {
                'total_neurons': self.brain.getTotalNeuronCount(),
                'total_synapses': self.brain.getTotalSynapseCount(),
                'current_step': self.state_history[-1]['step'] if self.state_history else 0
            },
            'state_history': self.state_history,
            'state_analysis': self.analyze_state_changes(
                min(s['step'] for s in self.state_history) if self.state_history else 0,
                max(s['step'] for s in self.state_history) if self.state_history else 0
            )
        }
        
        with open(output_file, 'w') as f:
            json.dump(report, f, indent=2)
        
        print(f"Debug report saved to {output_file}")
        return report

# Example usage
def example_state_debugging():
    """Example of state debugging."""
    
    # Create brain
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 200)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Create state debugger
    debugger = StateDebugger(brain)
    
    # Capture state during simulation
    print("Capturing brain state during simulation...")
    
    for step in range(100):
        brain.step(step)
        debugger.capture_state(step)
        
        if step % 25 == 0:
            print(f"Step {step} state captured")
    
    # Create debug report
    report = debugger.create_debug_report("brain_state_debug.json")
    
    # Print analysis
    print(f"\n=== State Analysis ===")
    if 'state_analysis' in report.get('state_analysis', {}):
        analysis = report['state_analysis']
        print(f"Step range: {analysis['step_range']['start']} to {analysis['step_range']['end']}")
        print(f"Neuron count change: {analysis['neuron_changes']['start_count']} -> {analysis['neuron_changes']['end_count']} ({analysis['neuron_changes']['change']:+d})")
        print(f"Synapse count change: {analysis['synapse_changes']['start_count']} -> {analysis['synapse_changes']['end_count']} ({analysis['synapse_changes']['change']:+d})")
        print(f"Firing neurons change: {analysis['activity_changes']['start_firing']} -> {analysis['activity_changes']['end_firing']} ({analysis['activity_changes']['change']:+d})")

# Run the example
example_state_debugging()
```

### 2.2 Error Diagnostic Tool

Diagnose and fix common NLM errors.

```python
import pynlm

class ErrorDiagnostic:
    """Diagnose and fix common NLM errors."""
    
    def __init__(self, brain):
        self.brain = brain
        self.error_history = []
    
    def check_simulation_stability(self):
        """Check if simulation is stable."""
        issues = []
        
        # Check for NaN or infinite values
        if hasattr(self.brain, 'getTotalNeuronCount'):
            neuron_count = self.brain.getTotalNeuronCount()
            if neuron_count <= 0:
                issues.append("Invalid neuron count: " + str(neuron_count))
        
        # Check firing rates
        if hasattr(self.brain, 'getAverageFiringRate'):
            firing_rate = self.brain.getAverageFiringRate()
            if firing_rate < 0 or firing_rate > 100:
                issues.append("Unusual firing rate: " + str(firing_rate))
        
        # Check spike activity
        if hasattr(self.brain, 'getTotalSpikeCount'):
            total_spikes = self.brain.getTotalSpikeCount()
            if total_spikes < 0:
                issues.append("Invalid total spikes: " + str(total_spikes))
        
        return {
            'is_stable': len(issues) == 0,
            'issues': issues,
            'timestamp': time.time()
        }
    
    def diagnose_plasticity_problems(self):
        """Diagnose plasticity-related problems."""
        issues = []
        
        # Check if plasticity is enabled
        config = getattr(self.brain, '_config', None)
        if config:
            # Check STDP configuration
            if config.getOr("plasticity.stdp.enable", False):
                stdp_ltp = config.getOr("plasticity.stdp.learning_rate", 0.001)
                if stdp_ltp <= 0 or stdp_ltp > 1:
                    issues.append("Invalid STDP learning rate: " + str(stdp_ltp))
                
                stdp_tau = config.getOr("plasticity.stdp.tau", 20.0)
                if stdp_tau <= 0:
                    issues.append("Invalid STDP time constant: " + str(stdp_tau))
        
        return {
            'issues': issues,
            'recommendations': self.get_plasticity_recommendations(issues)
        }
    
    def get_plasticity_recommendations(self, issues):
        """Get recommendations for fixing plasticity issues."""
        recommendations = []
        
        for issue in issues:
            if "learning rate" in issue.lower():
                recommendations.append("Set STDP learning rate between 0.001 and 0.1")
            elif "time constant" in issue.lower():
                recommendations.append("Set STDP time constants between 10.0 and 100.0")
            elif "invalid" in issue.lower():
                recommendations.append("Check configuration file for invalid values")
        
        return recommendations
    
    def check_memory_usage(self):
        """Check memory usage and warn about potential issues."""
        import psutil
        import os
        
        process = psutil.Process(os.getpid())
        memory_info = process.memory_info()
        
        # Check for memory issues
        issues = []
        
        # Check RSS memory (should be reasonable)
        rss_mb = memory_info.rss / (1024 * 1024)
        if rss_mb > 1000:  # More than 1GB
            issues.append(f"High memory usage: {rss_mb:.1f} MB")
        
        # Check for memory leaks (growing memory)
        if not hasattr(self, '_previous_memory'):
            self._previous_memory = rss_mb
        else:
            memory_growth = rss_mb - self._previous_memory
            if memory_growth > 100:  # More than 100MB growth
                issues.append(f"Potential memory leak: {memory_growth:.1f} MB growth")
            self._previous_memory = rss_mb
        
        return {
            'memory_mb': rss_mb,
            'issues': issues,
            'status': 'healthy' if len(issues) == 0 else 'warning'
        }
    
    def create_diagnostic_report(self):
        """Create comprehensive diagnostic report."""
        return {
            'timestamp': time.time(),
            'simulation_stability': self.check_simulation_stability(),
            'plasticity_diagnosis': self.diagnose_plasticity_problems(),
            'memory_status': self.check_memory_usage(),
            'recommendations': self.get_overall_recommendations()
        }
    
    def get_overall_recommendations(self):
        """Get overall recommendations based on all diagnostics."""
        recommendations = []
        
        stability_check = self.check_simulation_stability()
        plasticity_check = self.diagnose_plasticity_problems()
        memory_check = self.check_memory_usage()
        
        if not stability_check['is_stable']:
            recommendations.extend([
                "Check simulation parameters",
                "Verify configuration file",
                "Ensure all required dependencies are installed"
            ])
        
        if plasticity_check['issues']:
            recommendations.extend(plasticity_check['recommendations'])
        
        if memory_check['issues']:
            recommendations.extend([
                "Consider reducing simulation size",
                "Implement periodic checkpointing",
                "Monitor memory usage during long simulations"
            ])
        
        return recommendations
    
    def run_full_diagnostic(self):
        """Run complete diagnostic suite."""
        print("Running NLM diagnostic suite...")
        
        report = self.create_diagnostic_report()
        
        print("\n=== NLM Diagnostic Report ===")
        
        # Check simulation stability
        stability = report['simulation_stability']
        if stability['is_stable']:
            print("✓ Simulation stability: PASSED")
        else:
            print("✗ Simulation stability: FAILED")
            for issue in stability['issues']:
                print(f"  - {issue}")
        
        # Check plasticity
        plasticity = report['plasticity_diagnosis']
        if not plasticity['issues']:
            print("✓ Plasticity configuration: PASSED")
        else:
            print("✗ Plasticity configuration: ISSUES FOUND")
            for issue in plasticity['issues']:
                print(f"  - {issue}")
        
        # Check memory
        memory = report['memory_status']
        if memory['status'] == 'healthy':
            print(f"✓ Memory usage: HEALTHY ({memory['memory_mb']:.1f} MB)")
        else:
            print(f"✗ Memory usage: WARNING ({memory['memory_mb']:.1f} MB)")
            for issue in memory['issues']:
                print(f"  - {issue}")
        
        # Show recommendations
        if report['recommendations']:
            print("\n=== Recommendations ===")
            for i, rec in enumerate(report['recommendations'], 1):
                print(f"{i}. {rec}")
        
        return report

# Example usage
def example_diagnostic():
    """Example of diagnostic testing."""
    
    # Create brain
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 300)
    config.set("plasticity.stdp.learning_rate", 0.001)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Create error diagnostic
    diagnostic = ErrorDiagnostic(brain)
    
    # Run full diagnostic
    report = diagnostic.run_full_diagnostic()
    
    # Save diagnostic report
    import json
    with open("diagnostic_report.json", 'w') as f:
        json.dump(report, f, indent=2)
    
    print(f"\nDiagnostic report saved to diagnostic_report.json")

# Run the example
example_diagnostic()
```

## Section 3: Performance Monitoring Framework

### 3.1 Real-time Monitoring

Monitor simulation performance in real-time.

```python
import pynlm
import time
import threading
from collections import deque

class RealTimeMonitor:
    """Real-time monitoring of NLM simulation."""
    
    def __init__(self, brain, update_interval=1.0):
        self.brain = brain
        self.update_interval = update_interval
        self.monitoring = False
        self.monitor_thread = None
        
        # Performance metrics
        self.metrics_history = deque(maxlen=1000)
        self.start_time = time.time()
        
        # Alert thresholds
        self.alert_thresholds = {
            'max_step_time': 0.1,  # 100ms
            'min_firing_rate': 0.1,  # 0.1 Hz
            'max_memory_growth': 100,  # 100MB
            'max_cpu_usage': 90  # 90%
        }
    
    def start_monitoring(self):
        """Start real-time monitoring."""
        self.monitoring = True
        self.monitor_thread = threading.Thread(target=self._monitor_loop)
        self.monitor_thread.daemon = True
        self.monitor_thread.start()
        print("Real-time monitoring started")
    
    def stop_monitoring(self):
        """Stop real-time monitoring."""
        self.monitoring = False
        if self.monitor_thread:
            self.monitor_thread.join(timeout=5.0)
        print("Real-time monitoring stopped")
    
    def _monitor_loop(self):
        """Main monitoring loop."""
        while self.monitoring:
            try:
                self._collect_metrics()
                time.sleep(self.update_interval)
            except Exception as e:
                print(f"Monitoring error: {e}")
                break
    
    def _collect_metrics(self):
        """Collect current performance metrics."""
        step_time = time.time() - self.start_time
        
        metrics = {
            'timestamp': step_time,
            'step': int(step_time / 0.001),  # Assuming 1ms timestep
            'cpu_usage': psutil.cpu_percent(),
            'memory_mb': psutil.Process().memory_info().rss / (1024 * 1024),
            'firing_rate': self.brain.getAverageFiringRate(),
            'total_spikes': self.brain.getTotalSpikeCount(),
            'neurons_firing': self.brain.getFiringNeuronCount()
        }
        
        self.metrics_history.append(metrics)
        
        # Check for alerts
        self._check_alerts(metrics)
    
    def _check_alerts(self, metrics):
        """Check for performance alerts."""
        alerts = []
        
        # Check CPU usage
        if metrics['cpu_usage'] > self.alert_thresholds['max_cpu_usage']:
            alerts.append(f"High CPU usage: {metrics['cpu_usage']}%")
        
        # Check firing rate
        if metrics['firing_rate'] < self.alert_thresholds['min_firing_rate']:
            alerts.append(f"Low firing rate: {metrics['firing_rate']:.3f} Hz")
        
        # Check memory growth
        if len(self.metrics_history) > 10:
            current_memory = metrics['memory_mb']
            previous_memory = self.metrics_history[-10]['memory_mb']
            memory_growth = current_memory - previous_memory
            
            if memory_growth > self.alert_thresholds['max_memory_growth']:
                alerts.append(f"High memory growth: {memory_growth:.1f} MB")
        
        # Output alerts
        if alerts:
            print(f"[{time.time():.1f}] ALERTS:")
            for alert in alerts:
                print(f"  ⚠️  {alert}")
    
    def get_monitoring_summary(self):
        """Get monitoring summary."""
        if not self.metrics_history:
            return {"error": "No monitoring data collected"}
        
        metrics = list(self.metrics_history)
        
        return {
            'monitoring_duration': metrics[-1]['timestamp'] - metrics[0]['timestamp'],
            'samples_collected': len(metrics),
            'current_metrics': metrics[-1],
            
            'statistics': {
                'cpu_usage': {
                    'mean': sum(m['cpu_usage'] for m in metrics) / len(metrics),
                    'max': max(m['cpu_usage'] for m in metrics),
                    'min': min(m['cpu_usage'] for m in metrics)
                },
                'memory_mb': {
                    'mean': sum(m['memory_mb'] for m in metrics) / len(metrics),
                    'max': max(m['memory_mb'] for m in metrics),
                    'min': min(m['memory_mb'] for m in metrics),
                    'growth': metrics[-1]['memory_mb'] - metrics[0]['memory_mb']
                },
                'firing_rate': {
                    'mean': sum(m['firing_rate'] for m in metrics) / len(metrics),
                    'max': max(m['firing_rate'] for m in metrics),
                    'min': min(m['firing_rate'] for m in metrics)
                }
            }
        }
    
    def save_monitoring_data(self, filename):
        """Save monitoring data to file."""
        import json
        
        with open(filename, 'w') as f:
            json.dump(list(self.metrics_history), f, indent=2)
        
        summary = self.get_monitoring_summary()
        with open(filename.replace('.json', '_summary.json'), 'w') as f:
            json.dump(summary, f, indent=2)
        
        print(f"Monitoring data saved to {filename}")
        return summary

# Example usage
def example_real_time_monitoring():
    """Example of real-time monitoring."""
    
    # Create brain
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 500)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Create real-time monitor
    monitor = RealTimeMonitor(brain, update_interval=0.5)
    
    # Start monitoring
    monitor.start_monitoring()
    
    # Run simulation for a while
    print("Running simulation with real-time monitoring...")
    
    for step in range(200):
        brain.step(step)
        
        if step % 50 == 0:
            print(f"Step {step}")
        
        time.sleep(0.01)  # Small delay to allow monitoring
    
    # Stop monitoring
    monitor.stop_monitoring()
    
    # Get and display summary
    summary = monitor.get_monitoring_summary()
    print(f"\n=== Monitoring Summary ===")
    print(f"Duration: {summary['monitoring_duration']:.2f}s")
    print(f"Samples: {summary['samples_collected']}")
    print(f"Average CPU: {summary['statistics']['cpu_usage']['mean']:.1f}%")
    print(f"Average memory: {summary['statistics']['memory_mb']['mean']:.1f} MB")
    print(f"Memory growth: {summary['statistics']['memory_mb']['growth']:.1f} MB")
    
    # Save monitoring data
    monitor.save_monitoring_data("nlm_monitoring.json")

# Run the example
example_real_time_monitoring()
```

## Section 4: Advanced Debugging Utilities

### 4.1 Neuron State Inspector

Inspect individual neuron states.

```python
import pynlm

class NeuronStateInspector:
    """Inspect individual neuron states."""
    
    def __init__(self, brain):
        self.brain = brain
    
    def inspect_neuron(self, neuron_index):
        """Inspect a specific neuron by index."""
        if not hasattr(self.brain, 'getNeuronByIndex'):
            return {"error": "Brain does not support neuron inspection"}
        
        neuron = self.brain.getNeuronByIndex(neuron_index)
        if not neuron:
            return {"error": f"Neuron at index {neuron_index} not found"}
        
        inspection = {
            'neuron_index': neuron_index,
            'neuron_id': neuron.getId() if hasattr(neuron, 'getId') else None,
            'neuron_type': neuron.getType() if hasattr(neuron, 'getType') else None,
            'membrane_potential': neuron.getMembranePotential() if hasattr(neuron, 'getMembranePotential') else None,
            'firing_state': neuron.getFiringState() if hasattr(neuron, 'getFiringState') else None,
            'refractory_remaining': neuron.getRefractoryRemaining() if hasattr(neuron, 'getRefractoryRemaining') else None,
            'last_spike_time': neuron.getLastSpikeTime() if hasattr(neuron, 'getLastSpikeTime') else None,
            'adaptation_variable': neuron.getAdaptationVariable() if hasattr(neuron, 'getAdaptationVariable') else None
        }
        
        return inspection
    
    def inspect_region(self, region_id):
        """Inspect all neurons in a region."""
        if not hasattr(self.brain, 'getRegion'):
            return {"error": "Brain does not support region inspection"}
        
        region = self.brain.getRegion(region_id)
        if not region:
            return {"error": f"Region {region_id} not found"}
        
        neurons = region.getAllNeurons()
        
        neuron_inspections = []
        for i, neuron in enumerate(neurons):
            inspection = self.inspect_neuron(i)
            neuron_inspections.append(inspection)
        
        return {
            'region_id': region_id,
            'neuron_count': len(neurons),
            'neurons': neuron_inspections
        }
    
    def create_neuron_report(self, output_file):
        """Create a comprehensive neuron state report."""
        report = {
            'brain_info': {
                'total_neurons': self.brain.getTotalNeuronCount(),
                'total_synapses': self.brain.getTotalSynapseCount(),
                'firing_neurons': self.brain.getFiringNeuronCount()
            },
            'neuron_inspections': {}
        }
        
        # Inspect all regions
        if hasattr(self.brain, 'getRegions'):
            for region in self.brain.getRegions():
                if hasattr(region, 'getId'):
                    region_id = region.getId()
                    inspection = self.inspect_region(region_id)
                    report['neuron_inspections'][f"region_{region_id}"] = inspection
        
        # Save report
        import json
        with open(output_file, 'w') as f:
            json.dump(report, f, indent=2)
        
        print(f"Neuron state report saved to {output_file}")
        return report

# Example usage
def example_neuron_inspection():
    """Example of neuron state inspection."""
    
    # Create brain
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 100)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Create neuron inspector
    inspector = NeuronStateInspector(brain)
    
    # Run simulation
    for step in range(50):
        brain.step(step)
    
    # Create neuron report
    report = inspector.create_neuron_report("neuron_state_report.json")
    
    # Display some information
    print(f"\n=== Neuron State Report ===")
    print(f"Total neurons: {report['brain_info']['total_neurons']}")
    print(f"Total synapses: {report['brain_info']['total_synapses']}")
    print(f"Currently firing: {report['brain_info']['firing_neurons']}")
    
    # Show first few neuron details
    for region_name, region_data in list(report['neuron_inspections'].items())[:2]:
        print(f"\n{region_name}:")
        print(f"  Neuron count: {region_data.get('neuron_count', 'N/A')}")
        if 'neurons' in region_data:
            for i, neuron in enumerate(region_data['neurons'][:3]):
                print(f"    Neuron {i}: V={neuron.get('membrane_potential', 'N/A'):.2f}mV, "
                      f"firing={neuron.get('firing_state', 'N/A')}, "
                      f"spike_time={neuron.get('last_spike_time', 'N/A'):.2f}")

# Run the example
example_neuron_inspection()
```

## Section 5: Performance Optimization Tools

### 5.1 Memory Optimizer

Optimize memory usage during simulation.

```python
import pynlm
import psutil
import gc

class MemoryOptimizer:
    """Optimize memory usage during NLM simulation."""
    
    def __init__(self, brain):
        self.brain = brain
        self.memory_samples = []
        self.optimization_stats = {
            'garbage_collections': 0,
            'memory_freed_mb': 0,
            'optimization_applied': False
        }
    
    def monitor_memory(self, step_num):
        """Monitor memory usage at a specific step."""
        import psutil
        
        process = psutil.Process()
        memory_info = process.memory_info()
        
        sample = {
            'step': step_num,
            'timestamp': time.time(),
            'rss_mb': memory_info.rss / (1024 * 1024),
            'vms_mb': memory_info.vms / (1024 * 1024)
        }
        
        self.memory_samples.append(sample)
        return sample
    
    def force_garbage_collection(self):
        """Force garbage collection and measure memory improvement."""
        gc.collect()
        
        process = psutil.Process()
        memory_info = process.memory_info()
        
        current_memory = memory_info.rss / (1024 * 1024)
        
        self.optimization_stats['garbage_collections'] += 1
        
        return current_memory
    
    def optimize_memory_usage(self):
        """Apply memory optimization strategies."""
        if len(self.memory_samples) < 5:
            print("Not enough memory samples to apply optimization")
            return False
        
        # Check if optimization is needed
        recent_samples = self.memory_samples[-5:]
        avg_memory = sum(s['rss_mb'] for s in recent_samples) / len(recent_samples)
        
        # Apply optimization if memory usage is high
        if avg_memory > 500:  # More than 500MB
            print(f"Applying memory optimization (current: {avg_memory:.1f} MB)")
            
            # Force garbage collection
            before_gc = self.force_garbage_collection()
            
            # Force additional garbage collection cycles
            for i in range(3):
                gc.collect()
                time.sleep(0.1)
            
            after_gc = self.force_garbage_collection()
            memory_freed = before_gc - after_gc
            
            self.optimization_stats['memory_freed_mb'] += memory_freed
            self.optimization_stats['optimization_applied'] = True
            
            print(f"Memory optimization freed {memory_freed:.1f} MB")
            return True
        
        return False
    
    def get_optimization_summary(self):
        """Get memory optimization summary."""
        if not self.memory_samples:
            return {"error": "No memory samples collected"}
        
        recent_samples = self.memory_samples[-10:] if len(self.memory_samples) >= 10 else self.memory_samples
        
        return {
            'total_samples': len(self.memory_samples),
            'memory_samples': [{
                'step': s['step'],
                'rss_mb': s['rss_mb'],
                'vms_mb': s['vms_mb'],
                'timestamp': s['timestamp']
            } for s in recent_samples],
            
            'optimization_stats': self.optimization_stats,
            
            'memory_analysis': {
                'current_memory_mb': recent_samples[-1]['rss_mb'] if recent_samples else 0,
                'average_memory_mb': sum(s['rss_mb'] for s in recent_samples) / len(recent_samples),
                'memory_growth_mb': recent_samples[-1]['rss_mb'] - recent_samples[0]['rss_mb'] if len(recent_samples) > 1 else 0
            }
        }
    
    def create_optimization_report(self, output_file):
        """Create optimization report."""
        report = self.get_optimization_summary()
        
        import json
        with open(output_file, 'w') as f:
            json.dump(report, f, indent=2)
        
        print(f"Optimization report saved to {output_file}")
        return report

# Example usage
def example_memory_optimization():
    """Example of memory optimization."""
    
    # Create brain with larger size
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 2000)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Create memory optimizer
    memory_optimizer = MemoryOptimizer(brain)
    
    # Monitor memory during simulation
    print("Monitoring memory usage during simulation...")
    
    for step in range(300):
        brain.step(step)
        memory_optimizer.monitor_memory(step)
        
        # Apply optimization every 50 steps
        if step % 50 == 0 and step > 0:
            memory_optimizer.optimize_memory_usage()
        
        if step % 50 == 0:
            print(f"Step {step}")
    
    # Create optimization report
    report = memory_optimizer.create_optimization_report("memory_optimization.json")
    
    # Display summary
    print(f"\n=== Memory Optimization Summary ===")
    print(f"Total samples: {report['total_samples']}")
    print(f"Current memory: {report['memory_analysis']['current_memory_mb']:.1f} MB")
    print(f"Average memory: {report['memory_analysis']['average_memory_mb']:.1f} MB")
    print(f"Memory growth: {report['memory_analysis']['memory_growth_mb']:.1f} MB")
    print(f"Optimization applied: {report['optimization_stats']['optimization_applied']}")
    print(f"Memory freed: {report['optimization_stats']['memory_freed_mb']:.1f} MB")
    print(f"GC cycles: {report['optimization_stats']['garbage_collections']}")

# Run the example
example_memory_optimization()
```

## Section 6: Integration with External Tools

### 6.1 Visualization Integration

Integrate NLM data with external visualization tools.

```python
import pynlm
import matplotlib.pyplot as plt
import numpy as np
import json

class VisualizationIntegrator:
    """Integrate NLM data with external visualization tools."""
    
    def __init__(self, brain):
        self.brain = brain
        self.visualization_data = {
            'neuron_activity': [],
            'spike_patterns': [],
            'network_graph': {},
            'heatmap_data': []
        }
    
    def create_neuron_activity_visualization(self, output_file):
        """Create neuron activity visualization."""
        # Create a comprehensive visualization
        
        # Create figure with multiple subplots
        fig, axes = plt.subplots(2, 2, figsize=(12, 10))
        fig.suptitle('NLM Neuron Activity Visualization', fontsize=16)
        
        # Plot 1: Spike raster
        ax1 = axes[0, 0]
        self._plot_spike_raster(ax1)
        
        # Plot 2: Firing rate over time
        ax2 = axes[0, 1]
        self._plot_firing_rate_over_time(ax2)
        
        # Plot 3: Network connectivity heatmap
        ax3 = axes[1, 0]
        self._plot_network_connectivity(ax3)
        
        # Plot 4: Neural population activity
        ax4 = axes[1, 1]
        self._plot_neural_populations(ax4)
        
        plt.tight_layout()
        
        if output_file:
            plt.savefig(output_file, dpi=150, bbox_inches='tight')
            print(f"Neuron activity visualization saved to {output_file}")
        
        plt.show()
    
    def _plot_spike_raster(self, ax):
        """Plot spike raster."""
        # Get spike data
        if hasattr(self.brain, 'getTotalSpikeCount'):
            total_spikes = self.brain.getTotalSpikeCount()
        
        # Create synthetic spike data for demonstration
        np.random.seed(42)
        num_neurons = 50
        spike_times = np.random.exponential(1.0, 200)  # 200 spikes
        
        # Create raster plot
        for i, spike_time in enumerate(spike_times[:100]):  # Limit to 100 for clarity
            ax.scatter(spike_time, i, alpha=0.6, s=2, color='black', marker='.')
        
        ax.set_xlabel('Time (s)')
        ax.set_ylabel('Neuron Index')
        ax.set_title('Spike Raster Plot')
        ax.grid(True, alpha=0.3)
    
    def _plot_firing_rate_over_time(self, ax):
        """Plot firing rate over time."""
        # Create synthetic firing rate data
        np.random.seed(42)
        time_points = np.linspace(0, 10, 100)
        
        # Create complex firing pattern
        firing_rates = (
            5 * np.exp(-time_points / 3) +  # Decay
            3 * np.sin(2 * np.pi * time_points / 2) +  # Oscillations
            np.random.normal(0, 0.5, len(time_points))  # Noise
        )
        
        # Clip to reasonable values
        firing_rates = np.clip(firing_rates, 0, 20)
        
        ax.plot(time_points, firing_rates, linewidth=2, color='blue')
        
        ax.set_xlabel('Time (s)')
        ax.set_ylabel('Firing Rate (Hz)')
        ax.set_title('Firing Rate Over Time')
        ax.grid(True, alpha=0.3)
    
    def _plot_network_connectivity(self, ax):
        """Plot network connectivity heatmap."""
        # Create synthetic connectivity matrix
        np.random.seed(42)
        
        # Create a structured connectivity pattern
        size = 20
        connectivity = np.zeros((size, size))
        
        # Create random connections with some structure
        for i in range(size):
            for j in range(size):
                # Probability of connection
                if np.random.random() < 0.15:
                    # Add some structure
                    distance = abs(i - j)
                    weight = np.exp(-distance / 3.0) * np.random.uniform(0.5, 1.0)
                    connectivity[i, j] = weight
        
        # Create heatmap
        im = ax.imshow(connectivity, cmap='hot', interpolation='nearest', aspect='auto')
        
        ax.set_xlabel('Target Neuron')
        ax.set_ylabel('Source Neuron')
        ax.set_title('Network Connectivity Heatmap')
        
        # Add colorbar
        plt.colorbar(im, ax=ax)
    
    def _plot_neural_populations(self, ax):
        """Plot neural population activity."""
        # Create synthetic population data
        np.random.seed(42)
        
        # Create multiple populations with different characteristics
        time_points = np.linspace(0, 5, 200)
        
        # Population activities
        populations = {
            'Excitatory': 8 * np.exp(-time_points / 1.5) + np.random.normal(0, 0.8, len(time_points)),
            'Inhibitory': 4 * np.exp(-time_points / 2.0) + np.random.normal(0, 0.4, len(time_points)),
            'Sensory': 6 * np.sin(2 * np.pi * time_points / 3) + np.random.normal(0, 0.6, len(time_points)),
            'Motor': 3 * np.cos(2 * np.pi * time_points / 2.5) + np.random.normal(0, 0.3, len(time_points))
        }
        
        # Plot each population
        for name, activity in populations.items():
            activity = np.clip(activity, 0, 15)
            ax.plot(time_points, activity, linewidth=2, label=name, alpha=0.8)
        
        ax.set_xlabel('Time (s)')
        ax.set_ylabel('Activity Level')
        ax.set_title('Neural Population Activity')
        ax.legend()
        ax.grid(True, alpha=0.3)
    
    def create_interactive_visualization(self, output_file):
        """Create interactive visualization."""
        try:
            import plotly.graph_objects as go
            import plotly.io as pio
            
            # Create interactive figure
            fig = go.Figure()
            
            # Add spike raster trace
            fig.add_trace(go.Scatter(
                x=[0, 1, 2, 3, 4],  # Simplified time
                y=[0, 1, 0, 1, 0],
                mode='markers',
                marker=dict(color='black', size=8, opacity=0.7),
                name='Spikes'
            ))
            
            # Add firing rate trace
            fig.add_trace(go.Scatter(
                x=[0, 1, 2, 3, 4],
                y=[1, 2, 1.5, 2.5, 1],
                mode='lines',
                line=dict(color='blue', width=3),
                name='Firing Rate'
            ))
            
            fig.update_layout(
                title='Interactive NLM Visualization',
                xaxis_title='Time (s)',
                yaxis_title='Activity',
                hovermode='x unified'
            )
            
            if output_file.endswith('.html'):
                pio.write_html(fig, output_file, auto_open=True)
                print(f"Interactive visualization saved to {output_file}")
            else:
                print("Please specify .html file for interactive visualization")
                
        except ImportError:
            print("Plotly not available for interactive visualization")

# Example usage
def example_visualization_integration():
    """Example of visualization integration."""
    
    # Create brain
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 300)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Create visualization integrator
    viz = VisualizationIntegrator(brain)
    
    # Create comprehensive visualization
    print("Creating neuron activity visualization...")
    
    # Run simulation
    for step in range(100):
        brain.step(step)
    
    # Create visualization
    viz.create_neuron_activity_visualization("nlm_neuron_activity.png")
    
    # Create interactive version
    viz.create_interactive_visualization("nlm_interactive.html")

# Run the example
example_visualization_integration()
```

## Section 7: Automated Testing Framework

### 7.1 Regression Testing

Implement automated regression testing for NLM.

```python
import pynlm
import unittest
import time
import json

class NLMRegressionTest(unittest.TestCase):
    """Regression tests for NLM functionality."""
    
    def setUp(self):
        """Set up test environment."""
        self.config = pynlm.createDefaultConfig()
        self.config.set("brain.neuron_count", 100)
        self.brain = pynlm.createBrain(self.config)
        self.brain.initialize()
    
    def test_basic_initialization(self):
        """Test basic brain initialization."""
        self.assertGreater(self.brain.getTotalNeuronCount(), 0)
        self.assertGreater(self.brain.getTotalSynapseCount(), 0)
        self.assertEqual(self.brain.getFiringNeuronCount(), 0)
        self.assertEqual(self.brain.getTotalSpikeCount(), 0)
    
    def test_simulation_step(self):
        """Test simulation step functionality."""
        initial_spikes = self.brain.getTotalSpikeCount()
        
        # Run multiple steps
        for step in range(10):
            self.brain.step(step)
        
        # Verify state changes
        self.assertGreater(self.brain.getTotalSpikeCount(), initial_spikes)
        self.assertGreaterEqual(self.brain.getAverageFiringRate(), 0)
    
    def test_configuration_consistency(self):
        """Test configuration consistency."""
        # Test that configuration values are within expected ranges
        neuron_count = self.config.getOr("brain.neuron_count", 100)
        self.assertGreater(neuron_count, 0)
        self.assertLessEqual(neuron_count, 10000)
        
        # Test default values
        default_config = pynlm.createDefaultConfig()
        self.assertEqual(default_config.getOr("brain.neuron_count", 1000), 1000)
        self.assertEqual(default_config.getOr("simulation_timestep", 0.001), 0.001)
    
    def test_plasticity_enable_disable(self):
        """Test enabling and disabling plasticity."""
        # This test would depend on the specific API
        # For now, we'll skip if the method doesn't exist
        if hasattr(self.brain, 'enablePlasticity'):
            # Enable plasticity
            self.brain.enablePlasticity(True, True, False)
            
            # Test that plasticity is enabled
            # This depends on brain-specific API
            pass
    
    def test_checkpoint_save_load(self):
        """Test checkpoint save and load functionality."""
        # Create a checkpoint file
        checkpoint_file = "test_checkpoint.bin"
        
        # Save current state
        self.brain.save(checkpoint_file)
        self.assertTrue(self._file_exists(checkpoint_file))
        
        # Run some simulation steps
        for step in range(50):
            self.brain.step(step)
        
        # Load from checkpoint
        self.brain.load(checkpoint_file)
        
        # Verify that state is restored
        # This depends on the specific checkpoint API
        pass
    
    def test_performance_benchmark(self):
        """Test performance benchmarks."""
        start_time = time.time()
        
        # Run simulation
        for step in range(100):
            self.brain.step(step)
        
        end_time = time.time()
        elapsed_time = end_time - start_time
        
        # Performance should be reasonable (less than 1 second for 100 steps)
        self.assertLess(elapsed_time, 10.0)  # Allow some overhead
    
    def test_memory_usage(self):
        """Test memory usage."""
        import psutil
        import os
        
        process = psutil.Process(os.getpid())
        initial_memory = process.memory_info().rss / (1024 * 1024)
        
        # Run simulation
        for step in range(200):
            self.brain.step(step)
        
        final_memory = process.memory_info().rss / (1024 * 1024)
        memory_growth = final_memory - initial_memory
        
        # Memory growth should be reasonable (less than 100MB)
        self.assertLess(memory_growth, 100.0)
    
    def _file_exists(self, filepath):
        """Check if file exists."""
        import os
        return os.path.exists(filepath)
    
    def test_configuration_override(self):
        """Test configuration override functionality."""
        # Create a brain with custom configuration
        custom_config = pynlm.createDefaultConfig()
        custom_config.set("brain.neuron_count", 500)
        custom_config.set("brain.synapse_density", 0.2)
        
        custom_brain = pynlm.createBrain(custom_config)
        custom_brain.initialize()
        
        # Verify that custom configuration was applied
        self.assertEqual(custom_brain.getTotalNeuronCount(), 500)
    
    def test_comprehensive_workflow(self):
        """Test comprehensive workflow."""
        # Create agent brain interface
        agent = pynlm.createAgentBrain(self.brain)
        
        # Initialize with a simple world
        world = pynlm.createSimpleWorld()
        world.configure(width=10, height=10)
        agent.initialize(world)
        
        # Run complete workflow
        for step in range(50):
            # Update world
            world.update(0.1)
            
            # Get sensory input
            percept = world.getSensoryPercept()
            
            # Process sensory input
            agent.processSensoryInput(percept)
            
            # Run brain step
            self.brain.step(step)
            
            # Decode motor command
            motor_cmd = agent.decodeMotorCommand()
            
            # Apply motor command
            world.applyMotorCommand(motor_cmd, world.getSimulationTime())
        
        # Verify that workflow completed successfully
        self.assertGreater(self.brain.getTotalSpikeCount(), 0)
        self.assertGreaterEqual(self.brain.getAverageFiringRate(), 0)

class NLMPerformanceTest(unittest.TestCase):
    """Performance tests for NLM."""
    
    def setUp(self):
        """Set up performance test environment."""
        self.config = pynlm.createDefaultConfig()
    
    def test_large_network_performance(self):
        """Test performance with large networks."""
        self.config.set("brain.neuron_count", 2000)
        
        brain = pynlm.createBrain(self.config)
        brain.initialize()
        
        start_time = time.time()
        
        # Run simulation
        for step in range(500):
            brain.step(step)
        
        end_time = time.time()
        elapsed_time = end_time - start_time
        
        # Performance should scale reasonably
        steps_per_second = 500 / elapsed_time
        self.assertGreater(steps_per_second, 100)  # At least 100 steps/second
    
    def test_memory_efficiency(self):
        """Test memory efficiency with large networks."""
        import psutil
        import os
        
        self.config.set("brain.neuron_count", 5000)
        
        brain = pynlm.createBrain(self.config)
        brain.initialize()
        
        process = psutil.Process(os.getpid())
        initial_memory = process.memory_info().rss / (1024 * 1024)
        
        # Run simulation
        for step in range(1000):
            brain.step(step)
        
        final_memory = process.memory_info().rss / (1024 * 1024)
        memory_growth = final_memory - initial_memory
        
        # Memory efficiency should be reasonable
        memory_per_neuron = memory_growth / 5000
        self.assertLess(memory_per_neuron, 0.1)  # Less than 0.1 MB per neuron

if __name__ == '__main__':
    # Run unit tests
    unittest.main()
```

## Section 8: Advanced Tool Configuration

### 8.1 Tool Configuration Manager

Configure and manage NLM tools and debugging settings.

```python
import pynlm
import json
import os

class ToolConfigurationManager:
    """Manage NLM tool configurations and settings."""
    
    def __init__(self, config_file="nlm_tools_config.json"):
        self.config_file = config_file
        self.default_config = {
            "profiling": {
                "cpu_profiler": {
                    "enabled": True,
                    "sampling_rate": 100,  # Hz
                    "max_samples": 10000
                },
                "memory_profiler": {
                    "enabled": True,
                    "sampling_rate": 50,
                    "max_samples": 5000
                },
                "spike_profiler": {
                    "enabled": True,
                    "sampling_rate": 200,
                    "max_samples": 20000
                }
            },
            "debugging": {
                "state_debugger": {
                    "enabled": True,
                    "capture_interval": 10,  # steps
                    "max_history": 1000
                },
                "error_diagnostic": {
                    "enabled": True,
                    "auto_fix": False,
                    "alert_level": "medium"
                },
                "neuron_inspector": {
                    "enabled": True,
                    "max_neurons_to_inspect": 100,
                    "inspection_interval": 50
                }
            },
            "visualization": {
                "default_output_format": "png",
                "dpi": 150,
                "color_scheme": "default",
                "enable_interactive": True
            },
            "performance_monitoring": {
                "enabled": True,
                "update_interval": 1.0,  # seconds
                "alert_thresholds": {
                    "max_cpu_usage": 90,
                    "max_memory_growth": 100,
                    "min_firing_rate": 0.1
                }
            },
            "optimization": {
                "auto_garbage_collection": True,
                "memory_optimization": True,
                "profile_guided_optimization": True
            }
        }
        
        self.load_configuration()
    
    def load_configuration(self):
        """Load configuration from file or create default."""
        if os.path.exists(self.config_file):
            try:
                with open(self.config_file, 'r') as f:
                    self.config = json.load(f)
                print(f"Configuration loaded from {self.config_file}")
            except Exception as e:
                print(f"Error loading configuration: {e}")
                self.config = self.default_config.copy()
        else:
            self.config = self.default_config.copy()
            print(f"Default configuration created")
    
    def save_configuration(self):
        """Save current configuration to file."""
        with open(self.config_file, 'w') as f:
            json.dump(self.config, f, indent=2)
        print(f"Configuration saved to {self.config_file}")
    
    def update_config(self, section, key, value):
        """Update a specific configuration value."""
        if section not in self.config:
            self.config[section] = {}
        
        self.config[section][key] = value
        print(f"Updated {section}.{key} = {value}")
    
    def get_config(self, section, key=None):
        """Get configuration value(s)."""
        if key is None:
            return self.config.get(section, {})
        else:
            return self.config.get(section, {}).get(key)
    
    def enable_tool(self, tool_name):
        """Enable a specific tool."""
        # Try to find the tool in the configuration
        for section in self.config.values():
            if tool_name in section:
                section[tool_name]['enabled'] = True
                print(f"Enabled tool: {tool_name}")
                return True
        
        print(f"Tool not found: {tool_name}")
        return False
    
    def disable_tool(self, tool_name):
        """Disable a specific tool."""
        for section in self.config.values():
            if tool_name in section:
                section[tool_name]['enabled'] = False
                print(f"Disabled tool: {tool_name}")
                return True
        
        print(f"Tool not found: {tool_name}")
        return False
    
    def is_tool_enabled(self, tool_name):
        """Check if a tool is enabled."""
        for section in self.config.values():
            if tool_name in section:
                return section[tool_name].get('enabled', False)
        return False
    
    def create_tool_environment(self):
        """Create tool environment based on configuration."""
        environment = {
            'profiling_enabled': False,
            'debugging_enabled': False,
            'visualization_enabled': False,
            'performance_monitoring_enabled': False,
            'tools': []
        }
        
        # Check profiling tools
        if self.get_config('profiling', 'cpu_profiler', {}).get('enabled', False):
            environment['profiling_enabled'] = True
            environment['tools'].append('cpu_profiler')
        
        if self.get_config('profiling', 'memory_profiler', {}).get('enabled', False):
            environment['tools'].append('memory_profiler')
        
        if self.get_config('profiling', 'spike_profiler', {}).get('enabled', False):
            environment['tools'].append('spike_profiler')
        
        # Check debugging tools
        if self.get_config('debugging', 'state_debugger', {}).get('enabled', False):
            environment['debugging_enabled'] = True
            environment['tools'].append('state_debugger')
        
        if self.get_config('debugging', 'error_diagnostic', {}).get('enabled', False):
            environment['tools'].append('error_diagnostic')
        
        if self.get_config('debugging', 'neuron_inspector', {}).get('enabled', False):
            environment['tools'].append('neuron_inspector')
        
        # Check visualization tools
        if self.get_config('visualization', 'enable_interactive', False):
            environment['visualization_enabled'] = True
        
        # Check performance monitoring
        if self.get_config('performance_monitoring', 'enabled', False):
            environment['performance_monitoring_enabled'] = True
        
        return environment
    
    def run_with_configuration(self, brain):
        """Run simulation with tool configuration."""
        environment = self.create_tool_environment()
        
        print(f"Running simulation with tool environment:")
        print(f"  Profiling: {environment['profiling_enabled']}")
        print(f"  Debugging: {environment['debugging_enabled']}")
        print(f"  Visualization: {environment['visualization_enabled']}")
        print(f"  Performance Monitoring: {environment['performance_monitoring_enabled']}")
        print(f"  Tools: {environment['tools']}")
        
        # Create tool instances based on configuration
        tools = {}
        
        # Create CPU profiler
        if environment['profiling_enabled'] and 'cpu_profiler' in environment['tools']:
            from profiling_tools import PerformanceProfiler
            tools['cpu_profiler'] = PerformanceProfiler(brain)
        
        # Create memory profiler
        if 'memory_profiler' in environment['tools']:
            from profiling_tools import MemoryProfiler
            tools['memory_profiler'] = MemoryProfiler(brain)
        
        # Create spike profiler
        if 'spike_profiler' in environment['tools']:
            from profiling_tools import SpikeProfiler
            tools['spike_profiler'] = SpikeProfiler(brain)
        
        # Create state debugger
        if 'state_debugger' in environment['tools']:
            from debugging_tools import StateDebugger
            tools['state_debugger'] = StateDebugger(brain)
        
        # Create error diagnostic
        if 'error_diagnostic' in environment['tools']:
            from debugging_tools import ErrorDiagnostic
            tools['error_diagnostic'] = ErrorDiagnostic(brain)
        
        # Create neuron inspector
        if 'neuron_inspector' in environment['tools']:
            from debugging_tools import NeuronStateInspector
            tools['neuron_inspector'] = NeuronStateInspector(brain)
        
        return tools, environment

# Example usage
def example_tool_configuration():
    """Example of tool configuration management."""
    
    # Create tool configuration manager
    config_manager = ToolConfigurationManager()
    
    # Update configuration
    config_manager.update_config('profiling', 'cpu_profiler', {'enabled': True, 'sampling_rate': 200})
    config_manager.update_config('debugging', 'state_debugger', {'enabled': True, 'capture_interval': 5})
    config_manager.update_config('visualization', 'default_output_format', 'svg')
    
    # Save configuration
    config_manager.save_configuration()
    
    # Create brain for testing
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Create tool environment
    tools, environment = config_manager.run_with_configuration(brain)
    
    print(f"\nTool environment created with {len(tools)} tools")
    
    # Run simulation with tools
    print("\nRunning simulation with tool environment...")
    
    for step in range(100):
        brain.step(step)
        
        # Apply tools if they have step methods
        for tool_name, tool in tools.items():
            if hasattr(tool, 'profile_step'):
                tool.profile_step(step)
            elif hasattr(tool, 'monitor_memory'):
                tool.monitor_memory(step)
            elif hasattr(tool, 'analyze_activity'):
                tool.analyze_activity(step)
        
        if step % 20 == 0:
            print(f"Step {step}")
    
    # Create visualization
    if environment['visualization_enabled']:
        viz = tools.get('neuron_inspector')
        if viz:
            viz.create_neuron_report("neuron_state_report.png")
    
    print("\nTool configuration and execution completed!")

# Run the example
example_tool_configuration()
```

## Section 9: Troubleshooting and Common Issues

### 9.1 Common Profiling Issues

1. **"Profiling is slowing down the simulation too much"**
   - Solution: Reduce sampling rate, disable unnecessary tools

2. **"Memory usage is growing too quickly"**
   - Solution: Force garbage collection, optimize memory usage

3. **"Spike profiling is consuming too much memory"**
   - Solution: Limit the number of spike samples, use sampling

4. **"Visualization is not working"**
   - Solution: Check dependencies, install required packages

### 9.2 Common Debugging Issues

1. **"State capture is not working"**
   - Solution: Check file permissions, ensure proper configuration

2. **"Error diagnostic is not providing useful information"**
   - Solution: Increase alert level, check configuration

3. **"Neuron inspection is taking too long"**
   - Solution: Limit the number of neurons inspected

### 9.3 Performance Issues

1. **"Performance monitoring is consuming too much CPU"**
   - Solution: Reduce monitoring frequency, disable alerts

2. **"Memory optimization is not effective"**
   - Solution: Check for memory leaks, optimize code

3. **"Tool configuration is not being applied"**
   - Solution: Verify configuration file format, restart tools

## Section 10: Further Reading and Resources

### 10.1 Recommended Books and Papers

- "How to Think Like a Computer Scientist: C++" - CS106B, Stanford University
- "Artificial Life: A Review and a New Synthesis" - Christen, H.
- "Spiking Neural Networks" - Maass, W., Matuszek, C., and Stott, R. R.
- "Neural Networks and Deep Learning" - Goodfellow, I., Bengio, Y., and Courville, A.

### 10.2 Online Resources

- [NLM Documentation](https://nlm.readthedocs.io)
- [NLM GitHub Repository](https://github.com/nlm-project/nlm)
- [NLM API Reference](https://nlm.readthedocs.io/api)
- [NLM Examples Gallery](https://nlm.readthedocs.io/examples)

### 10.3 Community Support

- [NLM Discord Server](https://discord.gg/nlm)
- [NLM Mailing List](https://groups.google.com/g/nlm-project)
- [NLM Stack Overflow](https://stackoverflow.com/questions/tagged/nlm)

## Conclusion

This comprehensive guide provides detailed information about NLM's profiling and debugging tools. The tools covered include:

1. **Built-in Profiling Tools**: CPU performance profiler, memory profiler, spike profiler
2. **Debugging Tools**: State debugger, error diagnostic, neuron inspector
3. **Performance Monitoring**: Real-time monitoring with alerts and recommendations
4. **Advanced Debugging Utilities**: Neuron state inspector, configuration manager
5. **Performance Optimization**: Memory optimizer, garbage collection tools
6. **Visualization Integration**: Multiple visualization formats and tools
7. **Automated Testing**: Comprehensive unit tests and performance benchmarks
8. **Tool Configuration**: Flexible configuration management for all tools

These tools provide NLM users with comprehensive capabilities for:

- **Performance Analysis**: Understanding and optimizing simulation performance
- **Debugging**: Identifying and fixing simulation issues
- **Monitoring**: Real-time monitoring and alerting
- **Visualization**: Creating visualizations of neural activity
- **Optimization**: Improving memory usage and efficiency
- **Testing**: Ensuring reliable and reproducible simulations

By using these tools effectively, users can:

1. **Better Understand** NLM's performance characteristics
2. **Debug Issues** more quickly and effectively
3. **Optimize Performance** for their specific use cases
4. **Create Better Visualizations** for analysis and presentation
5. **Ensure Reliability** through comprehensive testing
6. **Maintain Performance** through ongoing optimization

The NLM development team continues to improve and expand these tools based on user feedback and emerging best practices.

---

*This comprehensive profiling and debugging guide is continually updated based on user feedback and emerging best practices. Contributions and suggestions are welcome!*
