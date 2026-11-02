#!/usr/bin/env python3
"""
NLM Performance Profiler - Advanced performance monitoring and profiling tool
"""

import sys
import argparse
import time
import json
import os
from pathlib import Path
import pynlm

class PerformanceProfiler:
    def __init__(self, config):
        self.config = config
        self.brain = pynlm.createBrain(config)
        self.brain.initialize()
        
    def profile_step(self, step, time_step=0.001):
        """Profile a single simulation step"""
        start_time = time.perf_counter()
        
        self.brain.step(step, step * time_step)
        
        end_time = time.perf_counter()
        elapsed = end_time - start_time
        
        return elapsed, self.brain.getTotalSpikeCount()
    
    def profile_simulation(self, steps, time_step=0.001):
        """Profile a complete simulation"""
        print(f"=== Profiling Simulation: {steps} steps ===")
        
        total_time = 0.0
        total_spikes = 0
        step_times = []
        
        for i in range(steps):
            step_time, spikes_this_step = self.profile_step(i, time_step)
            step_times.append(step_time)
            total_time += step_time
            total_spikes += spikes_this_step
            
            if (i + 1) % 100 == 0:
                print(f"  Completed {i + 1}/{steps} steps")
        
        # Calculate statistics
        avg_step_time = total_time / steps if steps > 0 else 0
        min_step_time = min(step_times) if step_times else 0
        max_step_time = max(step_times) if step_times else 0
        std_dev = calculate_stddev(step_times) if len(step_times) > 1 else 0
        
        stats = {
            "total_steps": steps,
            "total_simulation_time": total_time,
            "steps_per_second": steps / total_time if total_time > 0 else 0,
            "avg_step_time_ms": avg_step_time * 1000,
            "min_step_time_ms": min_step_time * 1000,
            "max_step_time_ms": max_step_time * 1000,
            "std_dev_step_time_ms": std_dev * 1000,
            "total_spikes": total_spikes,
            "avg_spikes_per_step": total_spikes / steps if steps > 0 else 0,
            "avg_spikes_per_second": total_spikes / total_time if total_time > 0 else 0,
            "step_time_distribution": {
                "p50": calculate_percentile(step_times, 50),
                "p90": calculate_percentile(step_times, 90),
                "p95": calculate_percentile(step_times, 95),
                "p99": calculate_percentile(step_times, 99)
            }
        }
        
        return stats
    
    def profile_memory_usage(self):
        """Profile memory usage (simplified)"""
        print("=== Memory Usage Profile ===")
        
        # Get brain statistics
        neuron_count = self.brain.getTotalNeuronCount()
        synapse_count = self.brain.getTotalSynapseCount()
        spike_count = self.brain.getTotalSpikeCount()
        
        # Estimate memory usage (rough approximation)
        # Each neuron: ~100 bytes, each synapse: ~80 bytes, spike data: ~24 bytes
        neuron_memory = neuron_count * 100
        synapse_memory = synapse_count * 80
        spike_memory = spike_count * 24
        
        total_estimated = neuron_memory + synapse_memory + spike_memory
        
        print(f"Estimated memory usage:")
        print(f"  - Neurons: {neuron_count} * 100 bytes = {neuron_memory / 1024:.1f} KB")
        print(f"  - Synapses: {synapse_count} * 80 bytes = {synapse_memory / 1024:.1f} KB")
        print(f"  - Spikes: {spike_count} * 24 bytes = {spike_memory / 1024:.1f} KB")
        print(f"  - Total estimated: {total_estimated / 1024:.1f} KB")
        
        return {
            "neuron_count": neuron_count,
            "synapse_count": synapse_count,
            "spike_count": spike_count,
            "estimated_memory_bytes": total_estimated,
            "estimated_memory_kb": total_estimated / 1024
        }
    
    def profile_heat_map(self, steps, time_step=0.001):
        """Create a performance heat map showing spikes over time"""
        print(f"=== Creating Spike Heat Map ({steps} steps) ===")
        
        # Reset brain
        self.brain.reset()
        self.brain.initialize()
        
        # Collect spike data per step
        spikes_per_step = []
        for i in range(steps):
            self.brain.step(i, i * time_step)
            spikes_this_step = self.brain.getTotalSpikeCount() - sum(spikes_per_step) if spikes_per_step else self.brain.getTotalSpikeCount()
            spikes_per_step.append(spikes_this_step)
        
        # Find statistics
        max_spikes = max(spikes_per_step)
        avg_spikes = sum(spikes_per_step) / len(spikes_per_step)
        
        # Create heat map data (simplified)
        heat_map = {
            "steps": steps,
            "spikes_per_step": spikes_per_step,
            "max_spikes": max_spikes,
            "avg_spikes": avg_spikes,
            "heat_intensity": normalize_for_heat_map(spikes_per_step)
        }
        
        print(f"Spike statistics:")
        print(f"  - Max spikes in single step: {max_spikes}")
        print(f"  - Average spikes per step: {avg_spikes:.1f}")
        print(f"  - Peak activity step: {spikes_per_step.index(max_spikes)}")
        
        return heat_map
    
    def compare_scenarios(self, scenarios):
        """Compare performance across different scenarios"""
        print("=== Scenario Comparison ===")
        
        results = {}
        
        for name, config_params in scenarios.items():
            print(f"Running scenario: {name}")
            
            config = pynlm.createDefaultConfig()
            for key, value in config_params.items():
                set_config_value(config, key, value)
            
            brain = pynlm.createBrain(config)
            brain.initialize()
            
            # Run simulation
            steps = config_params.get('steps', 100)
            for i in range(steps):
                brain.step(i)
            
            scenario_result = {
                'neurons': brain.getTotalNeuronCount(),
                'synapses': brain.getTotalSynapseCount(),
                'total_spikes': brain.getTotalSpikeCount(),
                'firing_neurons': brain.getFiringNeuronCount(),
                'avg_firing_rate': brain.getAverageFiringRate()
            }
            
            results[name] = scenario_result
            print(f"  - Completed: {scenario_result}")
        
        return results

def calculate_stddev(values):
    """Calculate standard deviation"""
    if len(values) < 2:
        return 0.0
    mean = sum(values) / len(values)
    variance = sum((x - mean) ** 2 for x in values) / (len(values) - 1)
    return variance ** 0.5

def calculate_percentile(values, percentile):
    """Calculate percentile value"""
    if not values:
        return 0.0
    sorted_values = sorted(values)
    index = (percentile / 100) * (len(sorted_values) - 1)
    lower = int(index)
    upper = lower + 1 if lower + 1 < len(sorted_values) else lower
    weight = index - lower
    return sorted_values[lower] * (1 - weight) + sorted_values[upper] * weight

def normalize_for_heat_map(spikes_per_step):
    """Normalize spike data for heat map visualization"""
    if not spikes_per_step:
        return []
    
    max_spikes = max(spikes_per_step)
    if max_spikes == 0:
        return [0.0] * len(spikes_per_step)
    
    return [spike_count / max_spikes for spike_count in spikes_per_step]

def set_config_value(config, key, value):
    """Set configuration value based on type"""
    if isinstance(value, bool):
        config.set(key, value)
    elif isinstance(value, int):
        config.set(key, value)
    elif isinstance(value, float):
        config.set(key, value)
    elif isinstance(value, str):
        config.set(key, value)
    elif isinstance(value, list):
        if all(isinstance(x, int) for x in value):
            config.set(key, value)
        elif all(isinstance(x, float) for x in value):
            config.set(key, value)
        elif all(isinstance(x, str) for x in value):
            config.set(key, value)
        else:
            print(f"Warning: Cannot set config key '{key}' with mixed type list")
    else:
        print(f"Warning: Unsupported type for config key '{key}': {type(value)}")

def main():
    parser = argparse.ArgumentParser(
        description="NLM Performance Profiler - Advanced performance monitoring",
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="""
Examples:
  %(prog)s profile --steps 1000
  %(prog)s profile --steps 500 --time-step 0.001
  %(prog)s profile --brain-file brain.bin
  %(prog)s compare --scenarios config1.json config2.json
  %(prog)s heat-map --steps 1000
  %(prog)s memory
        """
    )
    
    subparsers = parser.add_subparsers(dest='command', help='Available commands')
    
    # Profile command
    profile_parser = subparsers.add_parser('profile', help='Profile simulation performance')
    profile_parser.add_argument('--steps', type=int, default=100, help='Number of simulation steps (default: 100)')
    profile_parser.add_argument('--time-step', type=float, default=0.001, help='Time step duration (default: 0.001)')
    profile_parser.add_argument('--output', '-o', help='Output file for profile results (JSON format)')
    profile_parser.add_argument('--brain-file', help='Load existing brain for profiling')
    
    # Memory command
    memory_parser = subparsers.add_parser('memory', help='Profile memory usage')
    
    # Heat map command
    heatmap_parser = subparsers.add_parser('heat-map', help='Create spike heat map')
    heatmap_parser.add_argument('--steps', type=int, default=100, help='Number of simulation steps (default: 100)')
    heatmap_parser.add_argument('--output', '-o', help='Output file for heat map data (JSON format)')
    
    # Compare command
    compare_parser = subparsers.add_parser('compare', help='Compare scenarios')
    compare_parser.add_argument('--scenarios', nargs='+', help='Scenario configuration files (JSON)')
    compare_parser.add_argument('--output', '-o', help='Output file for comparison results (JSON format)')
    
    args = parser.parse_args()
    
    if not args.command:
        parser.print_help()
        return 1
    
    try:
        if args.command == 'profile':
            if args.brain_file:
                # Load existing brain for profiling
                config = pynlm.createDefaultConfig()
                brain = pynlm.createBrain(config)
                if not brain.load(args.brain_file):
                    print(f"Failed to load brain from {args.brain_file}")
                    return 1
                
                profiler = PerformanceProfiler(config)
                profiler.brain = brain  # Replace with loaded brain
                
                stats = profiler.profile_simulation(args.steps, args.time_step)
            else:
                # Create new brain for profiling
                config = pynlm.createDefaultConfig()
                profiler = PerformanceProfiler(config)
                stats = profiler.profile_simulation(args.steps, args.time_step)
            
            if args.output:
                with open(args.output, 'w') as f:
                    json.dump(stats, f, indent=2)
                print(f"Profile results saved to: {args.output}")
            
            print("\n=== Profile Results ===")
            print(f"Total simulation time: {stats['total_simulation_time']:.3f} seconds")
            print(f"Steps per second: {stats['steps_per_second']:.1f}")
            print(f"Average step time: {stats['avg_step_time_ms']:.2f} ms")
            print(f"Step time range: {stats['min_step_time_ms']:.2f} - {stats['max_step_time_ms']:.2f} ms")
            print(f"Total spikes generated: {stats['total_spikes']}")
            
            return 0
            
        elif args.command == 'memory':
            config = pynlm.createDefaultConfig()
            profiler = PerformanceProfiler(config)
            stats = profiler.profile_memory_usage()
            return 0
            
        elif args.command == 'heat-map':
            config = pynlm.createDefaultConfig()
            profiler = PerformanceProfiler(config)
            heatmap_data = profiler.profile_heat_map(args.steps)
            
            if args.output:
                with open(args.output, 'w') as f:
                    json.dump(heatmap_data, f, indent=2)
                print(f"Heat map data saved to: {args.output}")
            
            print("\nHeat map created successfully")
            return 0
            
        elif args.command == 'compare':
            if not args.scenarios:
                print("Error: --scenarios argument is required for compare command")
                return 1
            
            # Load scenario configurations
            scenarios = {}
            for scenario_file in args.scenarios:
                with open(scenario_file, 'r') as f:
                    scenario_data = json.load(f)
                    if isinstance(scenario_data, dict) and 'name' in scenario_data:
                        scenarios[scenario_data['name']] = scenario_data
                    else:
                        print(f"Warning: Invalid scenario format in {scenario_file}")
            
            if not scenarios:
                print("No valid scenarios found")
                return 1
            
            # Create profiler instance
            config = pynlm.createDefaultConfig()
            profiler = PerformanceProfiler(config)
            
            results = profiler.compare_scenarios(scenarios)
            
            if args.output:
                with open(args.output, 'w') as f:
                    json.dump(results, f, indent=2)
                print(f"Comparison results saved to: {args.output}")
            
            return 0
            
    except KeyboardInterrupt:
        print("\nOperation cancelled by user")
        return 1
    except Exception as e:
        print(f"Error: {e}")
        return 1

if __name__ == "__main__":
    sys.exit(main())
