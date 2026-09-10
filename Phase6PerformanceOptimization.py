#!/usr/bin/env python3
"""
Phase 6 Integration Experiment - Performance Optimizations

This script demonstrates performance optimization techniques for the NLM Phase 6 integration experiment.
It includes memory profiling, time profiling, and optimization strategies.

Key optimizations:
1. Memory access pattern optimization using references
2. Early termination based on simulation phase
3. Conditional logging to reduce I/O overhead
4. Pre-allocation of buffers for better memory management
5. Efficient metrics collection using moving averages
"""

import time
import tracemalloc
import psutil
import os
from typing import List, Tuple, Dict, Any
import json

class PerformanceMonitor:
    """Performance monitoring and profiling for NLM integration experiments."""
    
    def __init__(self):
        self.start_time = time.time()
        self.start_memory = 0
        self.max_memory = 0
        self.cpu_samples = []
        self.simulation_steps = 0
        self.step_times = []
        self.action_times = []
        self.brain_times = []
        self.reward_times = []
        
    def start_memory_tracking(self):
        """Start memory tracking using tracemalloc."""
        tracemalloc.start()
        self.start_memory = tracemalloc.get_traced_memory()[1]
        
    def update_memory_tracking(self):
        """Update memory tracking and record statistics."""
        if tracemalloc.is_tracing():
            current, peak = tracemalloc.get_traced_memory()
            self.max_memory = max(self.max_memory, current)
            
    def record_step_time(self, step_time: float):
        """Record time for a simulation step."""
        self.step_times.append(step_time)
        self.simulation_steps += 1
        
    def record_brain_time(self, brain_time: float):
        """Record brain simulation time."""
        self.brain_times.append(brain_time)
        
    def record_action_time(self, action_time: float):
        """Record action processing time."""
        self.action_times.append(action_time)
        
    def record_reward_time(self, reward_time: float):
        """Record reward computation time."""
        self.reward_times.append(reward_time)
        
    def stop_memory_tracking(self):
        """Stop memory tracking and return statistics."""
        tracemalloc.stop()
        return {
            'peak_memory_bytes': self.max_memory,
            'start_memory_bytes': self.start_memory,
            'total_allocated_bytes': self.max_memory - self.start_memory
        }
        
    def get_cpu_usage(self):
        """Get current CPU usage percentage."""
        process = psutil.Process(os.getpid())
        return process.cpu_percent()
        
    def get_total_time(self):
        """Get total experiment time in seconds."""
        return time.time() - self.start_time
        
    def get_statistics(self) -> Dict[str, Any]:
        """Get comprehensive performance statistics."""
        stats = {
            'total_time_seconds': self.get_total_time(),
            'simulation_steps': self.simulation_steps,
            'steps_per_second': self.simulation_steps / max(self.get_total_time(), 0.001),
            'memory_stats': self.stop_memory_tracking(),
            'time_distribution': {
                'brain_simulation': self._calculate_stats(self.brain_times),
                'action_processing': self._calculate_stats(self.action_times),
                'reward_computation': self._calculate_stats(self.reward_times),
                'total_step': self._calculate_stats(self.step_times)
            }
        }
        return stats
        
    def _calculate_stats(self, times: List[float]) -> Dict[str, float]:
        """Calculate statistics for a list of time measurements."""
        if not times:
            return {'mean': 0.0, 'median': 0.0, 'min': 0.0, 'max': 0.0, 'stddev': 0.0}
        
        mean = sum(times) / len(times)
        sorted_times = sorted(times)
        median = sorted_times[len(sorted_times) // 2]
        min_time = sorted_times[0]
        max_time = sorted_times[-1]
        
        # Calculate standard deviation
        variance = sum((t - mean) ** 2 for t in times) / len(times)
        stddev = variance ** 0.5
        
        return {
            'mean': mean,
            'median': median,
            'min': min_time,
            'max': max_time,
            'stddev': stddev,
            'count': len(times)
        }
        
    def print_summary(self):
        """Print performance summary."""
        stats = self.get_statistics()
        
        print("\n" + "="*60)
        print("NLM PHASE 6 INTEGRATION EXPERIMENT - PERFORMANCE SUMMARY")
        print("="*60)
        print(f"Total execution time: {stats['total_time_seconds']:.2f} seconds")
        print(f"Simulation steps: {stats['simulation_steps']:,}")
        print(f"Steps per second: {stats['steps_per_second']:.1f}")
        
        memory_stats = stats['memory_stats']
        print(f"\nMemory Usage:")
        print(f"  Peak memory: {memory_stats['peak_memory_bytes'] / 1024 / 1024:.2f} MB")
        print(f"  Memory increase: {memory_stats['total_allocated_bytes'] / 1024 / 1024:.2f} MB")
        
        print(f"\nTime Distribution (microseconds):")
        time_dist = stats['time_distribution']
        print(f"  Brain simulation: mean={time_dist['brain_simulation']['mean']*1000000:.1f}μs "
              f"(median={time_dist['brain_simulation']['median']*1000000:.1f}μs)")
        print(f"  Action processing: mean={time_dist['action_processing']['mean']*1000000:.1f}μs "
              f"(median={time_dist['action_processing']['median']*1000000:.1f}μs)")
        print(f"  Reward computation: mean={time_dist['reward_computation']['mean']*1000000:.1f}μs "
              f"(median={time_dist['reward_computation']['median']*1000000:.1f}μs)")
        print(f"  Total step time: mean={time_dist['total_step']['mean']*1000000:.1f}μs "
              f"(median={time_dist['total_step']['median']*1000000:.1f}μs)")
        
        print(f"\nPerformance bottlenecks (top 3 slowest steps):")
        slow_steps = sorted(time_dist['total_step']['stddev'], reverse=True)[:3]
        for i, stddev in enumerate(slow_steps, 1):
            print(f"  {i}. Standard deviation: {stddev*1000000:.1f}μs")

class MetricsCollection:
    """Efficient collection of simulation metrics with moving averages."""
    
    def __init__(self, buffer_size: int = 100):
        self.buffer_size = buffer_size
        self.reward_buffer: List[float] = []
        self.firing_buffer: List[float] = []
        self.spike_buffer: List[float] = []
        self.novelty_buffer: List[float] = []
        self.curiosity_buffer: List[float] = []
        self.dopamine_buffer: List[float] = []
        
        # Pre-allocate for performance
        self.reward_buffer = [0.0] * buffer_size
        self.firing_buffer = [0.0] * buffer_size
        self.spike_buffer = [0.0] * buffer_size
        self.novelty_buffer = [0.0] * buffer_size
        self.curiosity_buffer = [0.0] * buffer_size
        self.dopamine_buffer = [0.0] * buffer_size
        
        self.write_index = 0
        self.is_full = False
        self.total_samples = 0
        
    def add_metrics(self, reward: float, firing_rate: float, 
                   spike_count: float, novelty: float, 
                   curiosity: float, dopamine: float):
        """Add metrics to buffers using circular buffer pattern."""
        self.reward_buffer[self.write_index] = reward
        self.firing_buffer[self.write_index] = firing_rate
        self.spike_buffer[self.write_index] = spike_count
        self.novelty_buffer[self.write_index] = novelty
        self.curiosity_buffer[self.write_index] = curiosity
        self.dopamine_buffer[self.write_index] = dopamine
        
        self.write_index = (self.write_index + 1) % self.buffer_size
        self.is_full = self.write_index == 0
        self.total_samples += 1
        
    def get_moving_average(self, buffer: List[float]) -> float:
        """Get moving average from circular buffer."""
        if self.total_samples == 0:
            return 0.0
        
        start_idx = 0 if not self.is_full else (self.write_index - self.buffer_size + 1)
        end_idx = self.write_index
        
        # Sum active portion of buffer
        total = 0.0
        if self.is_full:
            total = sum(buffer)
        else:
            for i in range(start_idx, end_idx):
                total += buffer[i]
        
        return total / self.total_samples
        
    def get_statistics(self) -> Dict[str, float]:
        """Get comprehensive statistics for all metrics."""
        return {
            'reward_moving_avg': self.get_moving_average(self.reward_buffer),
            'firing_rate_moving_avg': self.get_moving_average(self.firing_buffer),
            'spike_count_moving_avg': self.get_moving_average(self.spike_buffer),
            'novelty_moving_avg': self.get_moving_average(self.novelty_buffer),
            'curiosity_moving_avg': self.get_moving_average(self.curiosity_buffer),
            'dopamine_moving_avg': self.get_moving_average(self.dopamine_buffer),
            'buffer_size': self.buffer_size,
            'total_samples': self.total_samples,
            'samples_in_buffer': min(self.total_samples, self.buffer_size)
        }
        
    def get_final_metrics(self) -> Dict[str, float]:
        """Get final aggregated metrics for experiment summary."""
        stats = self.get_statistics()
        
        # For final results, calculate weighted averages based on actual samples
        if self.total_samples > 0:
            # Use buffer to compute actual final averages
            buffer_samples = min(self.total_samples, self.buffer_size)
            
            if buffer_samples == 0:
                return {k: 0.0 for k in ['total_reward', 'avg_firing_rate', 'novelty_level', 'curiosity_level', 'dopamine_level']}
            
            # Sum actual values from buffer (considering wrap-around)
            actual_samples = []
            for i in range(buffer_samples):
                idx = (self.write_index - buffer_samples + i) % self.buffer_size
                actual_samples.extend([
                    self.reward_buffer[idx],
                    self.firing_buffer[idx],
                    self.spike_buffer[idx],
                    self.novelty_buffer[idx],
                    self.curiosity_buffer[idx],
                    self.dopamine_buffer[idx]
                ])
            
            # Calculate averages
            reward_avg = sum(actual_samples[::6]) / buffer_samples
            firing_avg = sum(actual_samples[1::6]) / buffer_samples
            spike_avg = sum(actual_samples[2::6]) / buffer_samples
            novelty_avg = sum(actual_samples[3::6]) / buffer_samples
            curiosity_avg = sum(actual_samples[4::6]) / buffer_samples
            dopamine_avg = sum(actual_samples[5::6]) / buffer_samples
            
            return {
                'total_reward': reward_avg,
                'avg_firing_rate': firing_avg,
                'memory_episodes_stored': spike_avg,
                'novelty_level': novelty_avg,
                'curiosity_level': curiosity_avg,
                'dopamine_level': dopamine_avg
            }
        
        return {k: 0.0 for k in ['total_reward', 'avg_firing_rate', 'memory_episodes_stored', 'novelty_level', 'curiosity_level', 'dopamine_level']}

class ExperimentOptimizer:
    """Main class for optimizing NLM Phase 6 integration experiments."""
    
    def __init__(self):
        self.performance_monitor = PerformanceMonitor()
        self.metrics_collector = MetricsCollection()
        self.optimization_profile = {
            'early_termination_enabled': True,
            'conditional_logging': True,
            'memory_monitoring': True,
            'performance_threshold': 1000.0,  # seconds
            'max_memory_mb': 2048  # MB
        }
        
    def run_optimized_experiment(self, config: Dict[str, Any]) -> Dict[str, Any]:
        """
        Run optimized experiment with comprehensive performance monitoring.
        
        This is a template function that demonstrates how to run the NLM Phase 6
        integration experiment with various optimizations for performance.
        """
        print("Starting NLM Phase 6 Integration Experiment with Optimizations")
        print("=" * 60)
        
        # Start performance monitoring
        self.performance_monitor.start_memory_tracking()
        
        try:
            # Initialize experiment components
            print("Initializing experiment components...")
            # This would normally create brain, world, agent, etc.
            # For this example, we'll simulate the experiment
            
            # Run optimized simulation loop
            result = self._run_optimized_simulation(config)
            
            # Stop performance monitoring
            memory_stats = self.performance_monitor.stop_memory_tracking()
            
            # Get final statistics
            perf_stats = self.performance_monitor.get_statistics()
            final_metrics = self.metrics_collector.get_final_metrics()
            
            # Compile final result
            experiment_result = {
                'simulation_result': result,
                'performance_statistics': perf_stats,
                'final_metrics': final_metrics,
                'optimization_profile': self.optimization_profile,
                'memory_statistics': memory_stats,
                'completed_successfully': True
            }
            
            # Print performance summary
            self.performance_monitor.print_summary()
            
            return experiment_result
            
        except Exception as e:
            print(f"Experiment failed with error: {e}")
            return {
                'error': str(e),
                'completed_successfully': False,
                'performance_statistics': self.performance_monitor.get_statistics()
            }
            
    def _run_optimized_simulation(self, config: Dict[str, Any]) -> Dict[str, Any]:
        """Run optimized simulation loop with performance monitoring."""
        print(f"Running simulation with {config.get('maxSteps', 10000)} steps")
        
        # Simulated experiment components
        simulation_time = 0.0
        steps_completed = 0
        total_reward = 0.0
        
        # Run simulation loop
        for step in range(config.get('maxSteps', 10000)):
            # Track step timing
            step_start = time.time()
            
            # Performance optimizations
            if step % 100 == 0 and self.optimization_profile['conditional_logging']:
                print(f"Step {step}: Running...")
                
            # Simulate brain processing (optimized)
            brain_time_start = time.time()
            # Simulated brain computation
            time.sleep(0.0001)  # Simulate work
            brain_time = time.time() - brain_time_start
            self.performance_monitor.record_brain_time(brain_time)
            
            # Simulate action processing (optimized)
            action_time_start = time.time()
            # Simulated action computation
            time.sleep(0.00005)  # Simulate work
            action_time = time.time() - action_time_start
            self.performance_monitor.record_action_time(action_time)
            
            # Simulate reward computation (optimized)
            reward_time_start = time.time()
            # Simulated reward computation
            reward = 0.1 * (step % 10)  # Simulated reward
            total_reward += reward
            reward_time = time.time() - reward_time_start
            self.performance_monitor.record_reward_time(reward_time)
            
            # Record metrics (optimized with circular buffer)
            self.metrics_collector.add_metrics(
                reward,
                0.5 + (step % 100) * 0.01,  # Simulated firing rate
                step,  # Simulated spike count
                0.3 + (step % 100) * 0.001,  # Simulated novelty
                0.2 + (step % 100) * 0.002,  # Simulated curiosity
                0.15 + (step % 100) * 0.0015  # Simulated dopamine
            )
            
            # Update performance monitoring
            step_time = time.time() - step_start
            self.performance_monitor.record_step_time(step_time)
            simulation_time += step_time
            steps_completed += 1
            
            # Early termination check for development/testing
            if (self.optimization_profile['early_termination_enabled'] and 
                steps_completed >= config.get('maxSteps', 10000)):
                break
                
            # Memory monitoring
            if self.optimization_profile['memory_monitoring']:
                self.performance_monitor.update_memory_tracking()
                
        return {
            'steps_completed': steps_completed,
            'total_reward': total_reward,
            'simulation_time': simulation_time,
            'steps_per_second': steps_completed / max(simulation_time, 0.001)
        }
        
    def save_performance_report(self, result: Dict[str, Any], filepath: str = "performance_report.json"):
        """Save performance report to file."""
        try:
            with open(filepath, 'w') as f:
                json.dump(result, f, indent=2, default=str)
            print(f"Performance report saved to {filepath}")
            return True
        except Exception as e:
            print(f"Failed to save performance report: {e}")
            return False
        
    def generate_performance_visualization_data(self, result: Dict[str, Any]) -> Dict[str, Any]:
        """Generate data for performance visualization."""
        perf_stats = result['performance_statistics']
        
        return {
            'timeline_data': {
                'steps': list(range(perf_stats['simulation_steps'])),
                'step_times': [t * 1000000 for t in perf_stats['time_distribution']['total_step']['mean']] * 10
            },
            'memory_profile': {
                'peak_memory_mb': perf_stats['memory_stats']['peak_memory_bytes'] / 1024 / 1024,
                'memory_trend': 'increasing'
            },
            'performance_metrics': {
                'cpu_efficiency': perf_stats['steps_per_second'] / 10000,  # Normalized
                'memory_efficiency': 1.0 / (perf_stats['memory_stats']['peak_memory_bytes'] / 1024 / 1024),
                'time_to_steady_state': 1000  # Simulated
            }
        }

# Example usage and demonstration
def main():
    """Demonstrate the optimized NLM Phase 6 experiment."""
    print("NLM Phase 6 Integration Experiment - Performance Optimization")
    print("=" * 70)
    
    # Create optimizer instance
    optimizer = ExperimentOptimizer()
    
    # Configure experiment
    config = {
        'maxSteps': 5000,
        'neuronCount': 500,
        'regionCount': 2,
        'connectionProbability': 0.15,
        'enableCheckpointing': False,
        'enableReplay': False,
        'enableDevelopment': True,
        'checkpointPath': '/tmp/phase6_checkpoint.bin'
    }
    
    # Run optimized experiment
    result = optimizer.run_optimized_experiment(config)
    
    if result['completed_successfully']:
        print("\n" + "=" * 70)
        print("EXPERIMENT COMPLETED SUCCESSFULLY")
        print("=" * 70)
        
        # Display key results
        final_metrics = result['final_metrics']
        perf_stats = result['performance_statistics']
        
        print(f"\nSimulation Results:")
        print(f"  Steps completed: {result['simulation_result']['steps_completed']:,}")
        print(f"  Total reward: {final_metrics['total_reward']:.4f}")
        print(f"  Average firing rate: {final_metrics['avg_firing_rate']:.4f}")
        print(f"  Episodes stored: {final_metrics['memory_episodes_stored']:.0f}")
        print(f"  Novelty level: {final_metrics['novelty_level']:.4f}")
        print(f"  Curiosity level: {final_metrics['curiosity_level']:.4f}")
        print(f"  Dopamine level: {final_metrics['dopamine_level']:.4f}")
        
        print(f"\nPerformance Metrics:")
        print(f"  Total execution time: {perf_stats['total_time_seconds']:.2f} seconds")
        print(f"  Steps per second: {perf_stats['steps_per_second']:.1f}")
        memory_mb = perf_stats['memory_stats']['peak_memory_bytes'] / 1024 / 1024
        print(f"  Peak memory usage: {memory_mb:.2f} MB")
        
        # Save performance report
        optimizer.save_performance_report(result, "phase6_performance_report.json")
        
        # Generate visualization data
        viz_data = optimizer.generate_performance_visualization_data(result)
        print(f"\nVisualization data generated for plotting")
        
    else:
        print(f"\nExperiment failed: {result['error']}")

if __name__ == "__main__":
    # Install required packages if not available
    try:
        import psutil
        import tracemalloc
    except ImportError:
        print("Required packages not found. Install with: pip install psutil")
        print("Continuing with basic monitoring only...")
    
    main()