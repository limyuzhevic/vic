"""
Performance profiler for NLM neural simulation framework.

This module provides detailed performance profiling capabilities for identifying
performance bottlenecks and optimization opportunities in NLM simulations.
"""

import time
import threading
import sys
from typing import Dict, List, Any, Optional, Callable
import json

from .enhanced_api import NLMError

class ProfilingResults:
    """Container for profiling results and analysis"""
    
    def __init__(self):
        self.start_time = 0.0
        self.end_time = 0.0
        self.step_times: List[float] = []
        self.neuron_times: List[float] = []
        self.synapse_times: List[float] = []
        self.memory_snapshots: List[Dict[str, Any]] = []
        self.cpu_snapshots: List[Dict[str, Any]] = []
        self.custom_metrics: Dict[str, Any] = {}
    
    def to_dict(self) -> Dict[str, Any]:
        """Convert profiling results to dictionary"""
        return {
            'start_time': self.start_time,
            'end_time': self.end_time,
            'duration': self.end_time - self.start_time,
            'step_times': self.step_times,
            'neuron_times': self.neuron_times,
            'synapse_times': self.synapse_times,
            'memory_snapshots': self.memory_snapshots,
            'cpu_snapshots': self.cpu_snapshots,
            'custom_metrics': self.custom_metrics,
        }

class PerformanceProfiler:
    """
    Detailed performance profiler for NLM simulations.
    
    Provides comprehensive profiling capabilities including step-by-step timing,
    neural computation profiling, resource monitoring, and bottleneck identification.
    """
    
    def __init__(self, brain=None):
        """Initialize performance profiler with optional brain reference"""
        self.brain = brain
        self.results = ProfilingResults()
        self.profiling = False
        self.profiler_thread = None
        self.step_callbacks: List[Callable] = []
        self.neuron_callbacks: List[Callable] = []
        self.custom_timers: Dict[str, List[float]] = {}
        
        # Profiling configuration
        self.config = {
            'enable_step_profiling': True,
            'enable_neuron_profiling': True,
            'enable_memory_profiling': True,
            'enable_cpu_profiling': True,
            'record_interval': 0.1,  # seconds
            'max_data_points': 10000,
        }
    
    def configure(self, **kwargs):
        """Configure profiler settings"""
        for key, value in kwargs.items():
            if key in self.config:
                self.config[key] = value
            else:
                raise NLMError(f"Unknown profiler configuration option: {key}")
        return self
    
    def start_profiling(self, duration: float = 0.0):
        """
        Start performance profiling.
        
        Args:
            duration: Maximum profiling duration in seconds (0 = unlimited)
        """
        if self.profiling:
            raise NLMError("Profiler is already running")
        
        self.profiling = True
        self.results.start_time = time.time()
        self.results.end_time = (self.results.start_time + duration) if duration > 0 else 0.0
        
        print(f"Performance profiling started (duration: {duration}s)")
        return self
    
    def stop_profiling(self):
        """Stop performance profiling"""
        if not self.profiling:
            return
        
        self.profiling = False
        self.results.end_time = time.time()
        
        print(f"Performance profiling stopped (duration: {self.results.end_time - self.results.start_time:.2f}s)")
        return self
    
    def add_step_callback(self, callback: Callable):
        """Add callback function to be called after each step"""
        self.step_callbacks.append(callback)
        return self
    
    def add_neuron_callback(self, callback: Callable):
        """Add callback function to be called for each neuron"""
        self.neuron_callbacks.append(callback)
        return self
    
    def start_custom_timer(self, name: str):
        """Start custom timer for named operation"""
        self.custom_timers[name] = []
    
    def stop_custom_timer(self, name: str) -> float:
        """Stop custom timer and return elapsed time in seconds"""
        if name not in self.custom_timers:
            raise NLMError(f"Custom timer '{name}' not found")
        
        elapsed = time.time() - self.custom_timers[name][-1]
        self.custom_timers[name].append(elapsed)
        
        # Remove the timer after stopping if it's a single-use timer
        if len(self.custom_timers[name]) == 2:  # Start and stop
            del self.custom_timers[name]
        
        return elapsed
    
    def profile_step(self):
        """Profile a single simulation step"""
        if not self.profiling:
            return
        
        step_start = time.time()
        
        try:
            # Profile step execution
            if self.config['enable_step_profiling']:
                # Record step time
                self.results.step_times.append(time.time() - step_start)
            
            # Call step callbacks
            for callback in self.step_callbacks:
                try:
                    callback()
                except Exception as e:
                    print(f"Warning: Step callback error: {e}")
            
            # Record resource usage
            if self.config['enable_memory_profiling']:
                self._record_memory_snapshot()
            
            if self.config['enable_cpu_profiling']:
                self._record_cpu_snapshot()
            
            # Check duration limit
            if self.results.end_time > 0 and time.time() >= self.results.end_time:
                self.stop_profiling()
                
        except Exception as e:
            print(f"Error in step profiling: {e}")
    
    def profile_neuron(self, neuron_id: int, neuron_type: str = "unknown"):
        """Profile individual neuron computation"""
        if not self.profiling:
            return
        
        start_time = time.time()
        
        try:
            # Call neuron callbacks
            for callback in self.neuron_callbacks:
                try:
                    callback(neuron_id, neuron_type)
                except Exception as e:
                    print(f"Warning: Neuron callback error: {e}")
            
            if self.config['enable_neuron_profiling']:
                self.results.neuron_times.append(time.time() - start_time)
                
        except Exception as e:
            print(f"Error in neuron profiling: {e}")
    
    def profile_synapse(self, synapse_id: int, source_id: int, target_id: int):
        """Profile synapse computation"""
        if not self.profiling:
            return
        
        start_time = time.time()
        
        try:
            if self.config['enable_neuron_profiling']:
                self.results.synapse_times.append(time.time() - start_time)
                
        except Exception as e:
            print(f"Error in synapse profiling: {e}")
    
    def _record_memory_snapshot(self):
        """Record current memory usage snapshot"""
        try:
            import psutil
            process = psutil.Process()
            memory_info = process.memory_info()
            
            snapshot = {
                'timestamp': time.time(),
                'rss_mb': memory_info.rss / (1024 * 1024),
                'vms_mb': memory_info.vms / (1024 * 1024),
                'shared_mb': memory_info.shared / (1024 * 1024),
                'text_mb': getattr(memory_info, 'text', 0) / (1024 * 1024),
                'data_mb': getattr(memory_info, 'data', 0) / (1024 * 1024),
                'dirty_mb': getattr(memory_info, 'dirty', 0) / (1024 * 1024),
            }
            
            self.results.memory_snapshots.append(snapshot)
            
            # Limit snapshot count
            max_snapshots = self.config['max_data_points']
            if len(self.results.memory_snapshots) > max_snapshots:
                self.results.memory_snapshots = self.results.memory_snapshots[-max_snapshots:]
                
        except ImportError:
            print("Warning: psutil not available for memory profiling")
        except Exception as e:
            print(f"Warning: Memory snapshot error: {e}")
    
    def _record_cpu_snapshot(self):
        """Record current CPU usage snapshot"""
        try:
            import psutil
            process = psutil.Process()
            
            snapshot = {
                'timestamp': time.time(),
                'cpu_percent': process.cpu_percent(interval=None),
                'cpu_times_user': process.cpu_times().user,
                'cpu_times_system': process.cpu_times().system,
                'num_threads': process.num_threads(),
                'context_switches': process.num_ctx_switches(),
            }
            
            self.results.cpu_snapshots.append(snapshot)
            
            # Limit snapshot count
            max_snapshots = self.config['max_data_points']
            if len(self.results.cpu_snapshots) > max_snapshots:
                self.results.cpu_snapshots = self.results.cpu_snapshots[-max_snapshots:]
                
        except ImportError:
            print("Warning: psutil not available for CPU profiling")
        except Exception as e:
            print(f"Warning: CPU snapshot error: {e}")
    
    def get_profiling_results(self) -> Dict[str, Any]:
        """
        Get comprehensive profiling results.
        
        Returns:
            Dictionary containing detailed profiling data and analysis
        """
        if not self.results.step_times:
            return {
                'error': 'No profiling data available. Start profiling first.',
                'profiling_active': self.profiling,
            }
        
        # Calculate statistics
        step_times = self.results.step_times
        neuron_times = self.results.neuron_times
        synapse_times = self.results.synapse_times
        
        def calculate_stats(times: List[float]) -> Dict[str, float]:
            if not times:
                return {'count': 0, 'mean': 0.0, 'median': 0.0, 'min': 0.0, 'max': 0.0,
                       'std': 0.0, 'percentile_50': 0.0, 'percentile_95': 0.0, 'percentile_99': 0.0}
            
            sorted_times = sorted(times)
            n = len(sorted_times)
            
            return {
                'count': n,
                'mean': sum(sorted_times) / n,
                'median': sorted_times[n // 2],
                'min': sorted_times[0],
                'max': sorted_times[-1],
                'std': (sum((x - sum(sorted_times) / n) ** 2 for x in sorted_times) / n) ** 0.5
                      if n > 1 else 0.0,
                'percentile_50': sorted_times[n // 2],
                'percentile_95': sorted_times[int(n * 0.95)],
                'percentile_99': sorted_times[int(n * 0.99)],
            }
        
        step_stats = calculate_stats(step_times)
        neuron_stats = calculate_stats(neuron_times)
        synapse_stats = calculate_stats(synapse_times)
        
        # Custom timer statistics
        custom_timer_stats = {}
        for name, times in self.custom_timers.items():
            if len(times) >= 2:
                custom_timer_stats[name] = {
                    'count': len(times) // 2,
                    'mean': sum(times[::2]) / len(times[::2]) if len(times) >= 2 else 0.0,
                    'min': min(times[::2]),
                    'max': max(times[::2]),
                }
        
        # Memory usage trends
        memory_stats = self._calculate_memory_stats()
        
        return {
            'profiling_summary': {
                'duration': self.results.end_time - self.results.start_time,
                'start_time': self.results.start_time,
                'end_time': self.results.end_time,
                'steps_profiled': len(step_times),
                'neurons_profiled': len(neuron_times),
                'synapses_profiled': len(synapse_times),
                'profiling_active': self.profiling,
            },
            'performance_statistics': {
                'step_computation': step_stats,
                'neuron_computation': neuron_stats,
                'synapse_computation': synapse_stats,
                'custom_timers': custom_timer_stats,
            },
            'resource_usage': {
                'memory_statistics': memory_stats,
                'cpu_statistics': self._calculate_cpu_stats(),
            },
            'bottleneck_analysis': self._identify_bottlenecks(),
            'optimization_recommendations': self._generate_optimization_recommendations(),
        }
    
    def _calculate_memory_stats(self) -> Dict[str, Any]:
        """Calculate memory usage statistics"""
        if not self.results.memory_snapshots:
            return {}
        
        rss_values = [s['rss_mb'] for s in self.results.memory_snapshots]
        
        return {
            'total_samples': len(self.results.memory_snapshots),
            'avg_rss_mb': sum(rss_values) / len(rss_values),
            'min_rss_mb': min(rss_values),
            'max_rss_mb': max(rss_values),
            'rss_growth_mb': max(rss_values) - min(rss_values),
            'memory_trend': self._calculate_trend(rss_values),
        }
    
    def _calculate_cpu_stats(self) -> Dict[str, Any]:
        """Calculate CPU usage statistics"""
        if not self.results.cpu_snapshots:
            return {}
        
        cpu_values = [s['cpu_percent'] for s in self.results.cpu_snapshots]
        
        return {
            'total_samples': len(self.results.cpu_snapshots),
            'avg_cpu_percent': sum(cpu_values) / len(cpu_values),
            'max_cpu_percent': max(cpu_values),
            'min_cpu_percent': min(cpu_values),
            'cpu_utilization_trend': self._calculate_trend(cpu_values),
        }
    
    def _calculate_trend(self, values: List[float]) -> str:
        """Calculate trend of a time series"""
        if len(values) < 2:
            return "stable"
        
        # Simple linear trend detection
        n = len(values)
        x = list(range(n))
        y = values
        
        mean_x = sum(x) / n
        mean_y = sum(y) / n
        
        numerator = sum((x[i] - mean_x) * (y[i] - mean_y) for i in range(n))
        denominator = sum((x[i] - mean_x) ** 2 for i in range(n))
        
        if denominator == 0:
            return "stable"
        
        slope = numerator / denominator
        
        if abs(slope) < 0.01:
            return "stable"
        elif slope > 0:
            return "increasing"
        else:
            return "decreasing"
    
    def _identify_bottlenecks(self) -> List[Dict[str, Any]]:
        """Identify performance bottlenecks"""
        bottlenecks = []
        
        # Check step computation
        if self.results.step_times:
            avg_step_time = sum(self.results.step_times) / len(self.results.step_times)
            if avg_step_time > 0.1:  # Step taking more than 100ms
                bottlenecks.append({
                    'type': 'step_computation',
                    'severity': 'high',
                    'description': f'Average step time ({avg_step_time*1000:.2f}ms) is high',
                    'recommendation': 'Optimize neural computation or reduce network size',
                })
        
        # Check neuron computation
        if self.results.neuron_times:
            avg_neuron_time = sum(self.results.neuron_times) / len(self.results.neuron_times)
            if avg_neuron_time > 0.001:  # Neuron taking more than 1ms
                bottlenecks.append({
                    'type': 'neuron_computation',
                    'severity': 'medium',
                    'description': f'Average neuron computation ({avg_neuron_time*1000:.2f}ms) is high',
                    'recommendation': 'Optimize neuron dynamics or use vectorization',
                })
        
        # Check synapse computation
        if self.results.synapse_times:
            avg_synapse_time = sum(self.results.synapse_times) / len(self.results.synapse_times)
            if avg_synapse_time > 0.0001:  # Synapse taking more than 0.1ms
                bottlenecks.append({
                    'type': 'synapse_computation',
                    'severity': 'low',
                    'description': f'Average synapse computation ({avg_synapse_time*1000:.3f}ms) is high',
                    'recommendation': 'Optimize synaptic operations or reduce connectivity',
                })
        
        # Check memory usage
        if self.results.memory_snapshots:
            rss_growth = self._calculate_memory_stats().get('rss_growth_mb', 0)
            if rss_growth > 100:  # Memory growth more than 100MB
                bottlenecks.append({
                    'type': 'memory_usage',
                    'severity': 'high',
                    'description': f'Memory usage growth ({rss_growth:.1f}MB) suggests potential memory leak',
                    'recommendation': 'Check for memory leaks and optimize memory usage',
                })
        
        return bottlenecks
    
    def _generate_optimization_recommendations(self) -> List[str]:
        """Generate optimization recommendations based on profiling data"""
        recommendations = []
        
        # Step optimization
        if self.results.step_times:
            step_times = self.results.step_times
            p95_time = sorted(step_times)[int(len(step_times) * 0.95)]
            
            if p95_time > 0.05:  # 95th percentile step time > 50ms
                recommendations.append(
                    f"Step times are highly variable. Consider optimizing critical path operations."
                )
        
        # Memory optimization
        if self.results.memory_snapshots:
            memory_trend = self._calculate_memory_stats().get('memory_trend', '')
            if memory_trend == 'increasing':
                recommendations.append(
                    "Memory usage is increasing over time. Consider implementing memory pools."
                )
        
        # CPU optimization
        if self.results.cpu_snapshots:
            cpu_trend = self._calculate_cpu_stats().get('cpu_utilization_trend', '')
            if cpu_trend == 'increasing':
                recommendations.append(
                    "CPU utilization is increasing. Consider reducing computational complexity."
                )
        
        # Custom timer optimization
        for name, times in self.custom_timers.items():
            if len(times) >= 4:  # At least 2 start/stop pairs
                avg_time = sum(times[::2]) / len(times[::2])
                if avg_time > 0.1:  # Average operation time > 100ms
                    recommendations.append(
                        f"Custom timer '{name}' shows high average operation time ({avg_time*1000:.1f}ms)."
                    )
        
        if not recommendations:
            recommendations.append("Performance looks good. Consider further optimization for specific use cases.")
        
        return recommendations
    
    def identify_bottlenecks(self) -> List[str]:
        """
        Identify performance bottlenecks (legacy method for compatibility).
        
        Returns:
            List of bottleneck descriptions
        """
        results = self.get_profiling_results()
        return [f"{b['description']} ({b['severity']} severity)" for b in results['bottleneck_analysis']]
