#!/usr/bin/env python3
# NLM CLI Performance Module
# 
# Performance monitoring and benchmarking tools for NLM advanced CLI

import os
import time
import statistics
import json
import threading
from pathlib import Path
from typing import Dict, List, Any, Optional, Callable
from dataclasses import dataclass
from enum import Enum
import psutil
import matplotlib.pyplot as plt

class PerformanceMetric(Enum):
    CPU_USAGE = "cpu_usage"
    MEMORY_USAGE = "memory_usage"
    SIMULATION_SPEED = "simulation_speed"
    NETWORK_ACTIVITY = "network_activity"
    PLACILITY_ACTIVITY = "plasticity_activity"
    MEMORY_USAGE_RATE = "memory_usage_rate"
    TIME_STEP_DURATION = "time_step_duration"

@dataclass
class PerformanceSample:
    timestamp: float
    cpu_usage: float
    memory_usage: float
    simulation_step: int
    firing_rate: float
    total_spikes: float
    active_neurons: int
    pending_events: int

class NLMPerformanceMonitor:
    def __init__(self):
        self.monitoring = False
        self.samples: List[PerformanceSample] = []
        self.start_time = 0.0
        self.baseline_metrics = {}
        self.thresholds = {}
    
    def start_monitoring(self, interval: float = 1.0) -> bool:
        """Start performance monitoring"""
        print(f"Starting performance monitoring (interval: {interval}s)")
        
        self.monitoring = True
        self.start_time = time.time()
        self.samples = []
        
        # Start monitoring thread
        monitor_thread = threading.Thread(
            target=self._monitor_loop,
            args=(interval,),
            daemon=True
        )
        monitor_thread.start()
        
        print("Performance monitoring started")
        return True
    
    def stop_monitoring(self) -> List[PerformanceSample]:
        """Stop performance monitoring and return samples"""
        print("Stopping performance monitoring")
        self.monitoring = False
        return self.samples
    
    def _monitor_loop(self, interval: float):
        """Background monitoring loop"""
        while self.monitoring:
            sample = self._collect_sample()
            if sample:
                self.samples.append(sample)
            
            time.sleep(interval)
    
    def _collect_sample(self) -> Optional[PerformanceSample]:
        """Collect a single performance sample"""
        try:
            # Get system metrics
            cpu_usage = psutil.cpu_percent(interval=0.1)
            memory = psutil.virtual_memory()
            memory_usage = memory.percent
            
            # Get simulation metrics (would need brain reference in real implementation)
            current_step = len(self.samples) if self.samples else 0
            firing_rate = 1.5  # Placeholder
            total_spikes = 1500  # Placeholder
            active_neurons = 500  # Placeholder
            pending_events = 50  # Placeholder
            
            return PerformanceSample(
                timestamp=time.time() - self.start_time,
                cpu_usage=cpu_usage,
                memory_usage=memory_usage,
                simulation_step=current_step,
                firing_rate=firing_rate,
                total_spikes=total_spikes,
                active_neurons=active_neurons,
                pending_events=pending_events
            )
        except Exception as e:
            print(f"Error collecting performance sample: {e}")
            return None
    
    def set_thresholds(self, thresholds: Dict[PerformanceMetric, float]):
        """Set performance thresholds for alerts"""
        self.thresholds = thresholds
    
    def check_alerts(self, sample: PerformanceSample) -> List[str]:
        """Check if any thresholds are exceeded"""
        alerts = []
        
        for metric, threshold in self.thresholds.items():
            if metric == PerformanceMetric.CPU_USAGE and sample.cpu_usage > threshold:
                alerts.append(f"High CPU usage: {sample.cpu_usage:.1f}% > {threshold}%")
            
            elif metric == PerformanceMetric.MEMORY_USAGE and sample.memory_usage > threshold:
                alerts.append(f"High memory usage: {sample.memory_usage:.1f}% > {threshold}%")
            
            elif metric == PerformanceMetric.SIMULATION_SPEED:
                # Calculate simulation speed (steps per second)
                speed = sample.simulation_step / sample.timestamp if sample.timestamp > 0 else 0
                if speed < threshold:
                    alerts.append(f"Slow simulation speed: {speed:.2f} steps/s < {threshold} steps/s")
        
        return alerts
    
    def calculate_statistics(self, samples: List[PerformanceSample] = None) -> Dict[str, Any]:
        """Calculate performance statistics"""
        if not samples:
            samples = self.samples
        
        if not samples:
            return {}
        
        stats = {
            'sample_count': len(samples),
            'duration': samples[-1].timestamp - samples[0].timestamp if len(samples) > 1 else 0,
            'cpu_usage': {
                'mean': statistics.mean(s.cpu_usage for s in samples),
                'min': min(s.cpu_usage for s in samples),
                'max': max(s.cpu_usage for s in samples),
                'std': statistics.stdev(s.cpu_usage for s in samples) if len(samples) > 1 else 0
            },
            'memory_usage': {
                'mean': statistics.mean(s.memory_usage for s in samples),
                'min': min(s.memory_usage for s in samples),
                'max': max(s.memory_usage for s in samples),
                'std': statistics.stdev(s.memory_usage for s in samples) if len(samples) > 1 else 0
            },
            'simulation_performance': {
                'steps_per_second': len(samples) / max(samples[-1].timestamp - samples[0].timestamp, 1) if len(samples) > 1 else len(samples),
                'average_firing_rate': statistics.mean(s.firing_rate for s in samples),
                'average_spikes_per_second': statistics.mean(s.total_spikes for s in samples) / max(samples[-1].timestamp - samples[0].timestamp, 1) if len(samples) > 1 else 0,
                'average_active_neurons': statistics.mean(s.active_neurons for s in samples)
            }
        }
        
        return stats
    
    def generate_performance_report(self, samples: List[PerformanceSample] = None, 
                                  output_file: str = "performance_report.json") -> bool:
        """Generate performance report"""
        print(f"Generating performance report: {output_file}")
        
        if not samples:
            samples = self.samples
        
        if not samples:
            print("No performance samples to report")
            return False
        
        # Calculate statistics
        stats = self.calculate_statistics(samples)
        
        # Create comprehensive report
        report = {
            'monitoring_session': {
                'start_time': samples[0].timestamp if samples else 0,
                'end_time': samples[-1].timestamp if samples else 0,
                'duration': samples[-1].timestamp - samples[0].timestamp if len(samples) > 1 else 0,
                'sample_count': len(samples)
            },
            'system_performance': stats,
            'alerts': [],  # Would be populated if monitoring was active
            'recommendations': self._generate_recommendations(stats),
            'summary': self._generate_summary(stats)
        }
        
        # Save report
        try:
            with open(output_file, 'w') as f:
                json.dump(report, f, indent=2)
            
            print(f"Performance report saved to {output_file}")
            return True
            
        except Exception as e:
            print(f"Error generating performance report: {e}")
            return False
    
    def _generate_recommendations(self, stats: Dict[str, Any]) -> List[str]:n:
        """Generate performance recommendations"""
        recommendations = []
        
        # Check CPU usage
        avg_cpu = stats.get('cpu_usage', {}).get('mean', 0)
        if avg_cpu > 80:
            recommendations.append("Consider reducing simulation complexity or adding parallel processing")
        
        # Check memory usage
        avg_memory = stats.get('memory_usage', {}).get('mean', 0)
        if avg_memory > 85:
            recommendations.append("Consider reducing simulation size or implementing memory optimization")
        
        # Check simulation speed
        steps_per_second = stats.get('simulation_performance', {}).get('steps_per_second', 0)
        if steps_per_second < 100:
            recommendations.append("Simulation performance may be optimized for faster execution")
        
        if not recommendations:
            recommendations.append("Performance is within acceptable limits")
        
        return recommendations
    
    def _generate_summary(self, stats: Dict[str, Any]) -> Dict[str, Any]:
        """Generate performance summary"""
        return {
            'overall_performance_rating': self._calculate_performance_rating(stats),
            'bottleneck_identified': self._identify_bottleneck(stats),
            'optimization_potential': self._calculate_optimization_potential(stats)
        }
    
    def _calculate_performance_rating(self, stats: Dict[str, Any]) -> str:
        """Calculate overall performance rating"""
        cpu_avg = stats.get('cpu_usage', {}).get('mean', 0)
        memory_avg = stats.get('memory_usage', {}).get('mean', 0)
        steps_per_sec = stats.get('simulation_performance', {}).get('steps_per_second', 0)
        
        # Calculate rating (simplified)
        score = 0
        if cpu_avg < 70:
            score += 3
        elif cpu_avg < 85:
            score += 2
        else:
            score += 1
        
        if memory_avg < 70:
            score += 3
        elif memory_avg < 85:
            score += 2
        else:
            score += 1
        
        if steps_per_sec > 500:
            score += 3
        elif steps_per_sec > 100:
            score += 2
        else:
            score += 1
        
        if score >= 7:
            return "Excellent"
        elif score >= 5:
            return "Good"
        elif score >= 3:
            return "Fair"
        else:
            return "Poor"
    
    def _identify_bottleneck(self, stats: Dict[str, Any]) -> str:
        """Identify performance bottleneck"""
        cpu_avg = stats.get('cpu_usage', {}).get('mean', 0)
        memory_avg = stats.get('memory_usage', {}).get('mean', 0)
        steps_per_sec = stats.get('simulation_performance', {}).get('steps_per_second', 0)
        
        if cpu_avg > 85:
            return "CPU bottleneck"
        elif memory_avg > 85:
            return "Memory bottleneck"
        elif steps_per_sec < 100:
            return "Simulation performance bottleneck"
        else:
            return "No significant bottleneck identified"
    
    def _calculate_optimization_potential(self, stats: Dict[str, Any]) -> float:
        """Calculate optimization potential (0-1)"""
        # Simple optimization potential calculation
        cpu_avg = stats.get('cpu_usage', {}).get('mean', 0)
        memory_avg = stats.get('memory_usage', {}).get('mean', 0)
        
        # Lower averages mean more optimization potential
        cpu_potential = max(0, 1 - cpu_avg / 100)
        memory_potential = max(0, 1 - memory_avg / 100)
        
        return (cpu_potential + memory_potential) / 2

class NLMBenchmarkSuite:
    def __init__(self):
        self.benchmark_results = {}
    
    def run_plasticity_benchmark(self, brain, iterations: int = 5, 
                               step_size: int = 100) -> Dict[str, Any]:
        """Run plasticity benchmark"""
        print(f"Running plasticity benchmark: {iterations} iterations, {step_size} steps each")
        
        results = {
            'benchmark_type': 'plasticity',
            'iterations': iterations,
            'step_size': step_size,
            'results': [],
            'summary': {}
        }
        
        for i in range(iterations):
            print(f"  Iteration {i+1}/{iterations}")
            
            # Record initial state
            initial_weights = self._get_initial_weights(brain)
            initial_spikes = brain.getTotalSpikeCount()
            
            # Run simulation
            start_time = time.time()
            for step in range(step_size):
                brain.step(step)
            end_time = time.time()
            
            # Record final state
            final_weights = self._get_final_weights(brain)
            final_spikes = brain.getTotalSpikeCount()
            
            # Calculate metrics
            time_taken = end_time - start_time
            steps_per_second = step_size / time_taken if time_taken > 0 else 0
            spike_count = final_spikes - initial_spikes
            weight_change = self._calculate_weight_change(initial_weights, final_weights)
            
            iteration_result = {
                'iteration': i + 1,
                'time_taken': time_taken,
                'steps_per_second': steps_per_second,
                'spike_count': spike_count,
                'weight_change': weight_change,
                'learning_efficiency': spike_count / time_taken if time_taken > 0 else 0
            }
            
            results['results'].append(iteration_result)
        
        # Calculate summary
        times = [r['time_taken'] for r in results['results']]
        speeds = [r['steps_per_second'] for r in results['results']]
        efficiencies = [r['learning_efficiency'] for r in results['results']]
        
        results['summary'] = {
            'average_time': statistics.mean(times),
            'average_speed': statistics.mean(speeds),
            'average_efficiency': statistics.mean(efficiencies),
            'time_std': statistics.stdev(times) if len(times) > 1 else 0,
            'speed_std': statistics.stdev(speeds) if len(speeds) > 1 else 0,
            'efficiency_std': statistics.stdev(efficiencies) if len(efficiencies) > 1 else 0,
            'consistency_score': 1 - (statistics.stdev(speeds) / statistics.mean(speeds) if statistics.mean(speeds) > 0 else 0)
        }
        
        self.benchmark_results['plasticity'] = results
        return results
    
    def run_memory_benchmark(self, brain, iterations: int = 5,
                          step_size: int = 200) -> Dict[str, Any]:
        """Run memory benchmark"""
        print(f"Running memory benchmark: {iterations} iterations, {step_size} steps each")
        
        results = {
            'benchmark_type': 'memory',
            'iterations': iterations,
            'step_size': step_size,
            'results': [],
            'summary': {}
        }
        
        for i in range(iterations):
            print(f"  Iteration {i+1}/{iterations}")
            
            # Record initial memory state
            initial_memory = self._get_memory_state(brain)
            
            # Run simulation
            start_time = time.time()
            for step in range(step_size):
                brain.step(step)
            end_time = time.time()
            
            # Record final memory state
            final_memory = self._get_memory_state(brain)
            
            # Calculate metrics
            time_taken = end_time - start_time
            steps_per_second = step_size / time_taken if time_taken > 0 else 0
            memory_change = self._calculate_memory_change(initial_memory, final_memory)
            
            iteration_result = {
                'iteration': i + 1,
                'time_taken': time_taken,
                'steps_per_second': steps_per_second,
                'memory_change': memory_change,
                'memory_efficiency': memory_change / time_taken if time_taken > 0 else 0
            }
            
            results['results'].append(iteration_result)
        
        # Calculate summary
        times = [r['time_taken'] for r in results['results']]
        speeds = [r['steps_per_second'] for r in results['results']]
        
        results['summary'] = {
            'average_time': statistics.mean(times),
            'average_speed': statistics.mean(speeds),
            'time_std': statistics.stdev(times) if len(times) > 1 else 0,
            'speed_std': statistics.stdev(speeds) if len(speeds) > 1 else 0,
            'consistency_score': 1 - (statistics.stdev(speeds) / statistics.mean(speeds) if statistics.mean(speeds) > 0 else 0)
        }
        
        self.benchmark_results['memory'] = results
        return results
    
    def run_simulation_benchmark(self, brain, iterations: int = 3,
                               step_sizes: List[int] = None) -> Dict[str, Any]:
        """Run simulation benchmark with different step sizes"""
        if step_sizes is None:
            step_sizes = [100, 500, 1000, 2000]
        
        print(f"Running simulation benchmark: {iterations} iterations per step size")
        
        results = {
            'benchmark_type': 'simulation',
            'step_sizes': step_sizes,
            'iterations_per_step_size': iterations,
            'results': {},
            'summary': {}
        }
        
        for step_size in step_sizes:
            print(f"  Testing step size: {step_size}")
            step_results = []
            
            for i in range(iterations):
                print(f"    Iteration {i+1}/{iterations}")
                
                # Reset brain for fresh test
                brain.reset()
                brain.initialize()
                
                # Record initial state
                initial_state = self._get_initial_brain_state(brain)
                
                # Run simulation
                start_time = time.time()
                for step in range(step_size):
                    brain.step(step)
                end_time = time.time()
                
                # Record final state
                final_state = self._get_final_brain_state(brain)
                
                # Calculate metrics
                time_taken = end_time - start_time
                steps_per_second = step_size / time_taken if time_taken > 0 else 0
                activity_change = self._calculate_activity_change(initial_state, final_state)
                
                iteration_result = {
                    'iteration': i + 1,
                    'time_taken': time_taken,
                    'steps_per_second': steps_per_second,
                    'activity_change': activity_change,
                    'performance_efficiency': activity_change / time_taken if time_taken > 0 else 0
                }
                
                step_results.append(iteration_result)
            
            results['results'][str(step_size)] = step_results
        
        # Calculate summary across all step sizes
        all_times = []
        all_speeds = []
        for step_size_results in results['results'].values():
            all_times.extend([r['time_taken'] for r in step_size_results])
            all_speeds.extend([r['steps_per_second'] for r in step_size_results])
        
        results['summary'] = {
            'average_time': statistics.mean(all_times),
            'average_speed': statistics.mean(all_speeds),
            'time_std': statistics.stdev(all_times) if len(all_times) > 1 else 0,
            'speed_std': statistics.stdev(all_speeds) if len(all_speeds) > 1 else 0,
            'consistency_score': 1 - (statistics.stdev(all_speeds) / statistics.mean(all_speeds) if statistics.mean(all_speeds) > 0 else 0),
            'performance_scalability': self._calculate_scalability(results['results'])
        }
        
        self.benchmark_results['simulation'] = results
        return results
    
    def _get_initial_weights(self, brain) -> List[float]:
        """Get initial synaptic weights"""
        # Placeholder implementation
        return [0.1, 0.2, 0.15, 0.08, 0.12]
    
    def _get_final_weights(self, brain) -> List[float]:
        """Get final synaptic weights"""
        # Placeholder implementation
        return [0.15, 0.25, 0.18, 0.12, 0.16]
    
    def _calculate_weight_change(self, initial: List[float], final: List[float]) -> float:
        """Calculate weight change between initial and final states"""
        if len(initial) != len(final) or len(initial) == 0:
            return 0.0
        
        total_change = 0.0
        for init, fin in zip(initial, final):
            total_change += abs(fin - init)
        
        return total_change / len(initial)
    
    def _get_memory_state(self, brain) -> Dict[str, Any]:
        """Get current memory state"""
        # Placeholder implementation
        return {
            'working_memory_size': 100,
            'episodic_memory_count': 50,
            'active_traces': 25,
            'total_associations': 200
        }
    
    def _calculate_memory_change(self, initial: Dict[str, Any], final: Dict[str, Any]) -> float:
        """Calculate memory change between initial and final states"""
        change = 0.0
        for key in initial:
            if key in final:
                change += abs(final[key] - initial[key])
        
        return change / max(len(initial), 1)
    
    def _get_initial_brain_state(self, brain) -> Dict[str, Any]:
        """Get initial brain state"""
        # Placeholder implementation
        return {
            'neuron_count': brain.getTotalNeuronCount(),
            'synapse_count': brain.getTotalSynapseCount(),
            'firing_rate': brain.getAverageFiringRate(),
            'total_spikes': brain.getTotalSpikeCount()
        }
    
    def _get_final_brain_state(self, brain) -> Dict[str, Any]:
        """Get final brain state"""
        # Placeholder implementation
        return {
            'neuron_count': brain.getTotalNeuronCount(),
            'synapse_count': brain.getTotalSynapseCount(),
            'firing_rate': brain.getAverageFiringRate(),
            'total_spikes': brain.getTotalSpikeCount()
        }
    
    def _calculate_activity_change(self, initial: Dict[str, Any], final: Dict[str, Any]) -> float:
        """Calculate activity change between initial and final states"""
        change = 0.0
        for key in initial:
            if key in final:
                change += abs(final[key] - initial[key])
        
        return change / max(len(initial), 1)
    
    def _calculate_scalability(self, step_size_results: Dict[str, List[Dict[str, Any]]]) -> Dict[str, float]:
        """Calculate scalability results across different step sizes"""
        scalability = {}
        
        step_sizes = sorted([int(k) for k in step_size_results.keys()])
        
        for i in range(1, len(step_sizes)):
            smaller = step_sizes[i-1]
            larger = step_sizes[i]
            
            smaller_results = step_size_results[str(smaller)]
            larger_results = step_size_results[str(larger)]
            
            smaller_speed = statistics.mean([r['steps_per_second'] for r in smaller_results])
            larger_speed = statistics.mean([r['steps_per_second'] for r in larger_results])
            
            if smaller_speed > 0:
                expected_speedup = larger / smaller
                actual_speedup = larger_speed / smaller_speed
                scalability[f"{smaller} to {larger}"] = actual_speedup / expected_speedup if expected_speedup > 0 else 1.0
        
        return scalability
    
    def save_benchmark_results(self, filepath: str = "benchmark_results.json") -> bool:
        """Save benchmark results to file"""
        try:
            with open(filepath, 'w') as f:
                json.dump(self.benchmark_results, f, indent=2)
            
            print(f"Benchmark results saved to {filepath}")
            return True
            
        except Exception as e:
            print(f"Error saving benchmark results: {e}")
            return False

# Global performance tools instance
_performance_tools = None
_benchmark_suite = None

def get_performance_tools() -> NLMPerformanceMonitor:
    """Get global performance monitor instance"""
    global _performance_tools
    if _performance_tools is None:
        _performance_tools = NLMPerformanceMonitor()
    return _performance_tools

def get_benchmark_suite() -> NLMBenchmarkSuite:
    """Get global benchmark suite instance"""
    global _benchmark_suite
    if _benchmark_suite is None:
        _benchmark_suite = NLMBenchmarkSuite()
    return _benchmark_suite

# Convenience functions

def start_performance_monitor(interval: float = 1.0) -> bool:
    """Convenience function to start performance monitoring"""
    return get_performance_tools().start_monitoring(interval)

def stop_performance_monitor() -> List:
    """Convenience function to stop performance monitoring"""
    return get_performance_tools().stop_monitoring()

def run_plasticity_benchmark(brain, iterations: int = 5, step_size: int = 100):
    """Convenience function to run plasticity benchmark"""
    return get_benchmark_suite().run_plasticity_benchmark(brain, iterations, step_size)

def run_memory_benchmark(brain, iterations: int = 5, step_size: int = 200):
    """Convenience function to run memory benchmark"""
    return get_benchmark_suite().run_memory_benchmark(brain, iterations, step_size)

def run_simulation_benchmark(brain, iterations: int = 3, step_sizes=None):
    """Convenience function to run simulation benchmark"""
    return get_benchmark_suite().run_simulation_benchmark(brain, iterations, step_sizes)

if __name__ == "__main__":
    # Test the performance tools
    print("Testing NLM Performance Tools")
    
    # Create performance monitor
    monitor = NLMPerformanceMonitor()
    
    # Create benchmark suite
    benchmark = NLMBenchmarkSuite()
    
    # Test performance monitoring (would need brain reference in real implementation)
    print("\n--- Testing Performance Monitoring ---")
    # Note: This would require actual brain simulation in real usage
    print("Performance monitoring initialized (requires brain reference for real testing)")
    
    # Test benchmark suite (would need brain reference in real implementation)
    print("\n--- Testing Benchmark Suite ---")
    # Note: This would require actual brain simulation in real usage
    print("Benchmark suite initialized (requires brain reference for real testing)")
    
    print("\nPerformance tools test completed")
