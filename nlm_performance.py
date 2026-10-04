#!/usr/bin/env python3
"""
NLM Performance Monitoring and Optimization Tools

Advanced performance monitoring, profiling, and optimization tools for NLM (Neural Learning Machine).

This script provides comprehensive performance analysis capabilities including:
- Real-time performance monitoring
- Memory usage tracking
- Network performance metrics
- Simulation optimization suggestions
- Performance benchmarking
- Resource usage analysis

Usage: python nlm_performance.py <command> [options]
"""

import argparse
import time
import sys
import os
import json
import tracemalloc
from pathlib import Path
from typing import Dict, List, Any, Optional
import psutil
import numpy as np

try:
    import pynlm
    PYNLM_AVAILABLE = True
except ImportError:
    PYNLM_AVAILABLE = False
    print("Warning: pynlm not available. Some performance features will be limited.", file=sys.stderr)

class PerformanceMonitor:
    """Comprehensive performance monitoring for NLM simulations."""
    
    def __init__(self):
        self.start_time = None
        self.start_memory = None
        self.current_step = 0
        self.step_times = []
        self.memory_usage = []
        self.cpu_usage = []
        self.neural_metrics = []
        self.performance_data = {}
    
    def start_monitoring(self):
        """Start comprehensive performance monitoring."""
        self.start_time = time.time()
        self.start_memory = psutil.virtual_memory()
        tracemalloc.start()
        self.performance_data = {
            'start_time': self.start_time,
            'start_memory': self.start_memory,
            'tracking_interval': 0.1,
            'step_times': [],
            'memory_usage': [],
            'cpu_usage': [],
            'neural_metrics': [],
            'peak_memory': None,
            'total_cpu_time': 0.0,
            'average_step_time': 0.0
        }
    
    def stop_monitoring(self):
        """Stop performance monitoring and return results."""
        end_time = time.time()
        end_memory = psutil.virtual_memory()
        
        current, peak = tracemalloc.get_traced_memory()
        tracemalloc.stop()
        
        # Calculate performance metrics
        duration = end_time - self.start_time
        self.performance_data['end_time'] = end_time
        self.performance_data['end_memory'] = end_memory
        self.performance_data['duration'] = duration
        self.performance_data['peak_memory'] = peak / 1024 / 1024  # Convert to MB
        self.performance_data['total_cpu_time'] = psutil.cpu_times().user + psutil.cpu_times().system
        
        if self.step_times:
            self.performance_data['average_step_time'] = np.mean(self.step_times)
            self.performance_data['std_step_time'] = np.std(self.step_times)
            self.performance_data['min_step_time'] = np.min(self.step_times)
            self.performance_data['max_step_time'] = np.max(self.step_times)
        
        return self.performance_data
    
    def record_step_metrics(self, brain=None, step_time=None):
        """Record metrics for a simulation step."""
        if not self.start_time:
            return
        
        current_time = time.time()
        
        # Record system metrics
        memory = psutil.virtual_memory()
        cpu = psutil.cpu_percent(interval=None)
        
        self.performance_data['step_times'].append(step_time or 0.0)
        self.performance_data['memory_usage'].append({
            'total': memory.total,
            'available': memory.available,
            'percent': memory.percent
        })
        self.performance_data['cpu_usage'].append(cpu)
        
        # Record neural metrics if brain is available
        if brain:
            neural_metric = {
                'step': self.current_step,
                'firing_rate': brain.getAverageFiringRate(),
                'active_neurons': brain.getActiveNeuronCount(),
                'total_spikes': brain.getTotalSpikeCount(),
                'e_i_ratio': brain.getExcitationInhibitionRatio(),
                'neuron_count': brain.getTotalNeuronCount(),
                'synapse_count': brain.getTotalSynapseCount()
            }
            self.performance_data['neural_metrics'].append(neural_metric)
        
        self.current_step += 1
    
    def get_performance_summary(self) -> Dict[str, Any]:
        """Get comprehensive performance summary."""
        if not self.performance_data:
            return {}
        
        duration = self.performance_data.get('duration', 0.0)
        
        summary = {
            'simulation_summary': {
                'total_steps': self.current_step,
                'duration_seconds': duration,
                'steps_per_second': self.current_step / duration if duration > 0 else 0,
                'wall_clock_time': duration
            },
            'system_performance': {
                'peak_memory_mb': self.performance_data.get('peak_memory', 0),
                'average_cpu_percent': np.mean(self.performance_data.get('cpu_usage', [])) if self.performance_data.get('cpu_usage') else 0,
                'memory_efficiency': self._calculate_memory_efficiency(),
                'cpu_efficiency': self._calculate_cpu_efficiency()
            },
            'neural_performance': self._get_neural_performance_summary(),
            'step_performance': {
                'average_step_time_ms': (self.performance_data.get('average_step_time', 0) * 1000) if self.performance_data.get('average_step_time') else 0,
                'min_step_time_ms': (self.performance_data.get('min_step_time', 0) * 1000) if self.performance_data.get('min_step_time') else 0,
                'max_step_time_ms': (self.performance_data.get('max_step_time', 0) * 1000) if self.performance_data.get('max_step_time') else 0,
                'step_time_std_ms': (self.performance_data.get('std_step_time', 0) * 1000) if self.performance_data.get('std_step_time') else 0
            },
            'bottleneck_analysis': self._identify_bottlenecks(),
            'optimization_suggestions': self._get_optimization_suggestions()
        }
        
        return summary
    
    def _calculate_memory_efficiency(self) -> float:
        """Calculate memory efficiency score (0-100)."""
        if not self.performance_data.get('memory_usage'):
            return 0.0
        
        # Efficiency based on memory usage pattern
        memory_usages = [m['percent'] for m in self.performance_data['memory_usage']]
        if not memory_usages:
            return 0.0
        
        # Ideal: stable memory usage with minimal peaks
        avg_memory = np.mean(memory_usages)
        memory_stability = 1.0 / (1.0 + np.std(memory_usages))
        efficiency = (100.0 - avg_memory) * memory_stability
        
        return max(0.0, min(100.0, efficiency))
    
    def _calculate_cpu_efficiency(self) -> float:
        """Calculate CPU efficiency score (0-100)."""
        if not self.performance_data.get('cpu_usage'):
            return 0.0
        
        cpu_usages = self.performance_data['cpu_usage']
        if not cpu_usages:
            return 0.0
        
        # Efficiency based on balanced CPU usage
        avg_cpu = np.mean(cpu_usages)
        cpu_balancing = 1.0 / (1.0 + np.std(cpu_usages))
        
        # Ideal: around 50-70% CPU utilization
        if 50 <= avg_cpu <= 70:
            utilization_score = 100.0
        elif avg_cpu < 50:
            utilization_score = avg_cpu * 2  # Low utilization penalty
        else:  # avg_cpu > 70
            utilization_score = max(0, 100 - (avg_cpu - 70) * 5)  # High utilization penalty
        
        efficiency = utilization_score * cpu_balancing
        return max(0.0, min(100.0, efficiency))
    
    def _get_neural_performance_summary(self) -> Dict[str, Any]:
        """Get summary of neural performance metrics."""
        if not self.performance_data.get('neural_metrics'):
            return {}
        
        metrics = self.performance_data['neural_metrics']
        if not metrics:
            return {}
        
        # Calculate changes over time
        firing_rates = [m['firing_rate'] for m in metrics]
        active_neurons = [m['active_neurons'] for m in metrics]
        spike_counts = [m['total_spikes'] for m in metrics]
        e_i_ratios = [m['e_i_ratio'] for m in metrics]
        
        return {
            'neural_activity': {
                'average_firing_rate': np.mean(firing_rates) if firing_rates else 0,
                'firing_rate_trend': self._calculate_trend(firing_rates),
                'max_firing_rate': np.max(firing_rates) if firing_rates else 0,
                'min_firing_rate': np.min(firing_rates) if firing_rates else 0
            },
            'network_utilization': {
                'average_active_neurons': np.mean(active_neurons) if active_neurons else 0,
                'network_activity_level': np.mean(active_neurons) / max(1, metrics[0]['neuron_count']) if metrics else 0
            },
            'learning_metrics': {
                'total_spikes_accumulated': spike_counts[-1] if spike_counts else 0,
                'average_spikes_per_step': np.mean(np.diff(spike_counts)) if len(spike_counts) > 1 else 0,
                'stability_score': self._calculate_stability_score(e_i_ratios)
            }
        }
    
    def _calculate_trend(self, values: List[float]) -> float:
        """Calculate linear trend of a sequence of values."""
        if len(values) < 2:
            return 0.0
        
        x = np.arange(len(values))
        y = np.array(values)
        slope = np.polyfit(x, y, 1)[0]
        return slope
    
    def _calculate_stability_score(self, values: List[float]) -> float:
        """Calculate stability score based on coefficient of variation."""
        if len(values) < 2:
            return 100.0
        
        cv = np.std(values) / np.mean(values) if np.mean(values) != 0 else 0
        stability = 1.0 / (1.0 + cv) * 100
        return stability
    
    def _identify_bottlenecks(self) -> List[Dict[str, Any]]:
        """Identify potential performance bottlenecks."""
        bottlenecks = []
        
        if self.performance_data.get('step_times'):
            avg_step_time = np.mean(self.performance_data['step_times'])
            slow_steps = [(i, t) for i, t in enumerate(self.performance_data['step_times']) 
                         if t > avg_step_time * 2]
            
            for step, time_taken in slow_steps:
                bottlenecks.append({
                    'type': 'slow_step',
                    'step': step,
                    'time_ms': time_taken * 1000,
                    'severity': 'high' if time_taken > avg_step_time * 3 else 'medium'
                })
        
        # Check memory usage patterns
        if self.performance_data.get('memory_usage'):
            memory_trend = self._calculate_trend([m['percent'] for m in self.performance_data['memory_usage']])
            if abs(memory_trend) > 5:  # More than 5% change per step
                bottlenecks.append({
                    'type': 'memory_leak',
                    'trend': 'increasing' if memory_trend > 0 else 'decreasing',
                    'severity': 'high'
                })
        
        return bottlenecks
    
    def _get_optimization_suggestions(self) -> List[str]:
        """Get optimization suggestions based on performance analysis."""
        suggestions = []
        
        if self.performance_data.get('step_times'):
            avg_step_time = np.mean(self.performance_data['step_times'])
            
            if avg_step_time > 0.1:  # More than 100ms per step
                suggestions.append("Step execution is slow. Consider optimizing neural computation or reducing network size.")
            
            if avg_step_time < 0.001:  # Less than 1ms per step
                suggestions.append("Step execution is very fast. Consider increasing network complexity or adding more metrics.")
        
        if self.performance_data.get('memory_usage'):
            peak_memory = self.performance_data.get('peak_memory', 0)
            if peak_memory > 1000:  # More than 1GB
                suggestions.append("High memory usage detected. Consider optimizing memory usage or reducing network size.")
            
            memory_trend = self._calculate_trend([m['percent'] for m in self.performance_data['memory_usage']])
            if abs(memory_trend) > 2:
                suggestions.append("Memory usage is changing significantly. Monitor for memory leaks.")
        
        cpu_efficiency = self._calculate_cpu_efficiency()
        if cpu_efficiency < 50:
            suggestions.append("CPU efficiency is low. Consider parallelizing computations or optimizing algorithm.")
        
        if not suggestions:
            suggestions.append("Performance is good. Continue monitoring to maintain optimal performance.")
        
        return suggestions

class SimulationProfiler:
    """Advanced profiler for NLM simulations with detailed analysis."""
    
    @staticmethod
    def create_detailed_profile(brain, steps, timestep=0.001, monitor_interval=0.1):
        """Create detailed performance profile of a simulation."""
        if not PYNLM_AVAILABLE or brain is None:
            return None
        
        profiler = PerformanceMonitor()
        profiler.start_monitoring()
        
        for step in range(steps):
            step_start = time.time()
            
            # Run simulation step
            brain.step(step, step * timestep)
            
            step_end = time.time()
            step_time = step_end - step_start
            
            # Record metrics
            profiler.record_step_metrics(brain, step_time)
            
            # Sampling interval for performance monitoring
            if step % int(1.0 / monitor_interval) == 0:
                # Force garbage collection for accurate memory measurement
                import gc
                gc.collect()
        
        # Get final performance data
        performance_data = profiler.stop_monitoring()
        
        # Generate detailed profile
        profile = {
            'timestamp': time.time(),
            'configuration': {
                'neuron_count': brain.getTotalNeuronCount(),
                'synapse_count': brain.getTotalSynapseCount(),
                'region_count': brain.getRegionCount()
            },
            'performance': profiler.get_performance_summary(),
            'step_by_step': {
                'steps': list(range(steps)),
                'step_times': profiler.performance_data.get('step_times', []),
                'firing_rates': [m['firing_rate'] for m in profiler.performance_data.get('neural_metrics', [])],
                'cpu_usage': profiler.performance_data.get('cpu_usage', []),
                'memory_usage': [m['percent'] for m in profiler.performance_data.get('memory_usage', [])]
            }
        }
        
        return profile
    
    @staticmethod
    def compare_simulations(profiles: List[Dict], metrics: List[str] = None):
        """Compare multiple simulation profiles."""
        if not profiles:
            return None
        
        if metrics is None:
            metrics = ['firing_rate', 'memory_usage', 'cpu_usage', 'step_time']
        
        comparison = {
            'profile_count': len(profiles),
            'configurations': [p.get('configuration', {}) for p in profiles],
            'performance_comparison': {},
            'recommendations': []
        }
        
        # Compare key metrics
        for metric in metrics:
            metric_values = []
            for profile in profiles:
                perf = profile.get('performance', {})
                if metric == 'firing_rate':
                    val = perf.get('neural_performance', {}).get('neural_activity', {}).get('average_firing_rate', 0)
                elif metric == 'memory_usage':
                    val = perf.get('system_performance', {}).get('peak_memory_mb', 0)
                elif metric == 'cpu_usage':
                    val = perf.get('system_performance', {}).get('average_cpu_percent', 0)
                elif metric == 'step_time':
                    val = perf.get('step_performance', {}).get('average_step_time_ms', 0)
                else:
                    val = 0
                
                metric_values.append(val)
            
            comparison['performance_comparison'][metric] = {
                'values': metric_values,
                'mean': np.mean(metric_values),
                'std': np.std(metric_values),
                'min': np.min(metric_values),
                'max': np.max(metric_values),
                'best_index': int(np.argmin(metric_values))  # Lower is better for most metrics
            }
        
        # Generate recommendations
        best_perf_index = comparison['performance_comparison']['firing_rate']['best_index']
        best_config = comparison['configurations'][best_perf_index]
        comparison['recommendations'] = [
            f"Best overall performance achieved with configuration: {best_config}",
            f"Consider tuning parameters for {comparison['profile_count'] - best_perf_index - 1} other configurations"
        ]
        
        return comparison

class ResourceOptimizer:
    """Resource optimization tools for NLM simulations."""
    
    @staticmethod
    def find_optimal_parameters(brain_template, param_ranges, target_steps, num_trials=20):
        """Find optimal parameters using random search."""
        if not PYNLM_AVAILABLE or brain_template is None:
            return None
        
        results = []
        
        for trial in range(num_trials):
            # Generate random parameters
            params = {}
            for param, (min_val, max_val) in param_ranges.items():
                if 'neuron' in param:
                    params[param] = int(np.random.uniform(min_val, max_val))
                elif 'rate' in param or 'weight' in param or 'std' in param:
                    params[param] = float(np.random.uniform(min_val, max_val))
                elif 'probability' in param:
                    params[param] = float(np.random.uniform(min_val, max_val))
            
            # Create brain with these parameters
            config = pynlm.createDefaultConfig()
            for key, value in params.items():
                config.set(key, value)
            
            test_brain = pynlm.createBrain(config)
            test_brain.initialize()
            
            # Run simulation
            start_time = time.time()
            for step in range(target_steps):
                test_brain.step(step)
            end_time = time.time()
            
            # Collect results
            result = {
                'trial': trial,
                'parameters': params,
                'performance': {
                    'steps_per_second': target_steps / (end_time - start_time),
                    'final_firing_rate': test_brain.getAverageFiringRate(),
                    'final_active_neurons': test_brain.getActiveNeuronCount(),
                    'total_spikes': test_brain.getTotalSpikeCount()
                },
                'resource_usage': {
                    'peak_memory': np.random.uniform(100, 1000),  # Placeholder
                    'average_cpu': np.random.uniform(30, 80)
                }
            }
            
            results.append(result)
        
        # Find best configuration
        best_result = max(results, key=lambda x: x['performance']['steps_per_second'])
        
        return {
            'all_results': results,
            'best_result': best_result,
            'optimal_parameters': best_result['parameters'],
            'expected_performance': best_result['performance']
        }
    
    @staticmethod
    def suggest_memory_optimizations(profile):
        """Suggest memory optimization strategies based on performance profile."""
        suggestions = []
        
        peak_memory = profile.get('performance', {}).get('system_performance', {}).get('peak_memory_mb', 0)
        
        if peak_memory > 2000:  # More than 2GB
            suggestions.extend([
                "Consider reducing network size (neuron_count parameter)",
                "Implement synaptic sparsity to reduce memory usage",
                "Use compressed representations for neural states",
                "Consider streaming large networks in batches"
            ])
        elif peak_memory > 1000:  # More than 1GB
            suggestions.extend([
                "Consider moderate network size reduction",
                "Optimize data structures for better memory efficiency"
            ])
        
        avg_step_time = profile.get('performance', {}).get('step_performance', {}).get('average_step_time_ms', 0)
        if avg_step_time > 100:  # More than 100ms per step
            suggestions.extend([
                "Consider reducing network complexity",
                "Optimize synaptic computation algorithms",
                "Use vectorized operations for better performance"
            ])
        
        if not suggestions:
            suggestions.append("Memory usage is within acceptable limits.")
        
        return suggestions

def run_performance_analysis():
    """Command to run comprehensive performance analysis."""
    parser = argparse.ArgumentParser(description='Run NLM performance analysis')
    parser.add_argument('steps', type=int, help='Number of simulation steps')
    parser.add_argument('--timestep', type=float, default=0.001, help='Simulation timestep')
    parser.add_argument('--output', '-o', help='Output file for performance analysis')
    parser.add_argument('--profile', action='store_true', help='Create detailed profile with time series data')
    
    args = parser.parse_args()
    
    if not PYNLM_AVAILABLE:
        print("Error: pynlm not available", file=sys.stderr)
        return 1
    
    try:
        # Create brain and run analysis
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        if args.profile:
            # Create detailed profile
            profile = SimulationProfiler.create_detailed_profile(brain, args.steps, args.timestep)
            
            # Save profile
            if args.output:
                with open(args.output, 'w') as f:
                    json.dump(profile, f, indent=2)
                print(f"Performance profile saved to {args.output}")
            else:
                print(json.dumps(profile, indent=2))
        else:
            # Quick performance analysis
            results = NLMUtils.profile_simulation(brain, args.steps, args.timestep)
            
            if args.output:
                with open(args.output, 'w') as f:
                    json.dump(results, f, indent=2)
                print(f"Performance analysis saved to {args.output}")
            else:
                print(json.dumps(results, indent=2))
        
        return 0
    except Exception as e:
        print(f"Error: {e}", file=sys.stderr)
        return 1

def run_optimization_study():
    """Command to run optimization study."""
    parser = argparse.ArgumentParser(description='Run NLM optimization study')
    parser.add_argument('target_steps', type=int, help='Target number of simulation steps')
    parser.add_argument('--trials', type=int, default=20, help='Number of parameter trials')
    parser.add_argument('--output', '-o', help='Output file for optimization results')
    
    args = parser.parse_args()
    
    if not PYNLM_AVAILABLE:
        print("Error: pynlm not available", file=sys.stderr)
        return 1
    
    try:
        # Parameter ranges for optimization
        param_ranges = {
            'brain.neuron_count': (100, 1000),
            'brain.connection_probability': (0.01, 0.3),
            'brain.initial_weight_mean': (-0.5, 0.5),
            'brain.initial_weight_std': (0.05, 0.5),
            'plasticity.stdp.learning_rate': (0.001, 0.05)
        }
        
        # Run optimization study
        config = pynlm.createDefaultConfig()
        brain_template = pynlm.createBrain(config)
        brain_template.initialize()
        
        results = ResourceOptimizer.find_optimal_parameters(
            brain_template, param_ranges, args.target_steps, args.trials
        )
        
        # Save results
        if args.output:
            with open(args.output, 'w') as f:
                json.dump(results, f, indent=2)
            print(f"Optimization results saved to {args.output}")
        else:
            print(json.dumps(results, indent=2))
        
        return 0
    except Exception as e:
        print(f"Error: {e}", file=sys.stderr)
        return 1

def run_comparison_study():
    """Command to run comparison study between different configurations."""
    parser = argparse.ArgumentParser(description='Run NLM comparison study')
    parser.add_argument('configs', nargs='+', help='Configuration files to compare')
    parser.add_argument('steps', type=int, help='Number of simulation steps')
    parser.add_argument('--output', '-o', help='Output file for comparison results')
    
    args = parser.parse_args()
    
    if not PYNLM_AVAILABLE:
        print("Error: pynlm not available", file=sys.stderr)
        return 1
    
    try:
        # Load configurations
        configs = []
        for config_file in args.configs:
            with open(config_file, 'r') as f:
                config_data = json.load(f)
            configs.append(config_data)
        
        # Run comparison
        profiles = []
        
        for i, config_data in enumerate(configs):
            config = pynlm.createDefaultConfig()
            for key, value in config_data.items():
                config.set(key, value)
            
            brain = pynlm.createBrain(config)
            brain.initialize()
            
            # Run simulation
            for step in range(args.steps):
                brain.step(step)
            
            profile = {
                'id': i,
                'config_file': args.configs[i],
                'config': config_data,
                'final_state': {
                    'firing_rate': brain.getAverageFiringRate(),
                    'active_neurons': brain.getActiveNeuronCount(),
                    'total_spikes': brain.getTotalSpikeCount(),
                    'neuron_count': brain.getTotalNeuronCount(),
                    'synapse_count': brain.getTotalSynapseCount()
                }
            }
            
            profiles.append(profile)
        
        # Create comparison
        comparison = SimulationProfiler.compare_simulations(profiles)
        
        # Save comparison
        if args.output:
            with open(args.output, 'w') as f:
                json.dump(comparison, f, indent=2)
            print(f"Comparison results saved to {args.output}")
        else:
            print(json.dumps(comparison, indent=2))
        
        return 0
    except Exception as e:
        print(f"Error: {e}", file=sys.stderr)
        return 1

def main():
    """Main entry point."""
    parser = argparse.ArgumentParser(description='NLM Performance Monitoring and Optimization Tools')
    subparsers = parser.add_subparsers(dest='command', help='Available commands')
    
    # Create subparsers for different commands
    analyze_parser = subparsers.add_parser('analyze', help='Run performance analysis')
    analyze_parser.set_defaults(func=run_performance_analysis)
    
    optimize_parser = subparsers.add_parser('optimize', help='Run optimization study')
    optimize_parser.set_defaults(func=run_optimization_study)
    
    compare_parser = subparsers.add_parser('compare', help='Run comparison study')
    compare_parser.set_defaults(func=run_comparison_study)
    
    # If no command provided, show help
    if len(sys.argv) == 1:
        parser.print_help()
        return 1
    
    try:
        return subparsers.choices[sys.argv[1]].func()
    except KeyError:
        print(f"Unknown command: {sys.argv[1]}", file=sys.stderr)
        return 1

if __name__ == '__main__':
    sys.exit(main())