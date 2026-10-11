#!/usr/bin/env python3
"""
NLM Performance Monitoring and Profiling Tools

Advanced performance monitoring and profiling tools for NLM (Neural Learning Machine) brain simulations.
Provides real-time performance metrics, benchmarking, and optimization analysis.
"""

import sys
import os
import time
import threading
import multiprocessing
import psutil
import numpy as np
from typing import Dict, List, Optional, Any, Callable
from dataclasses import dataclass, field
from enum import Enum
from datetime import datetime
import json
import csv
from collections import defaultdict, deque

# Add the current directory to the path to import our utilities
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

try:
    import pynlm
    PYNLM_AVAILABLE = True
except ImportError:
    PYNLM_AVAILABLE = False
    pynlm = None
class PerformanceMetric:
    """A single performance metric"""
    
    def __init__(self, name: str, value: float, unit: str = "", timestamp: Optional[float] = None):
        self.name = name
        self.value = value
        self.unit = unit
        self.timestamp = timestamp or time.time()
    
    def to_dict(self) -> Dict[str, Any]:
        return {
            'name': self.name,
            'value': self.value,
            'unit': self.unit,
            'timestamp': self.timestamp,
            'formatted': self._format_value()
        }
    
    def _format_value(self) -> str:
        if self.value is None:
            return "N/A"
        if self.unit == "ms":
            return f"{self.value:.3f} ms"
        elif self.unit == "seconds":
            return f"{self.value:.3f} s"
        elif self.unit == "Hz":
            return f"{self.value:.1f} Hz"
        elif self.unit == "MB":
            return f"{self.value:.1f} MB"
        elif self.unit == "percent":
            return f"{self.value:.1f}%"
        else:
            return f"{self.value:.3f}"
class PerformanceProfiler:
    """Comprehensive performance profiler for NLM simulations"""
    
    def __init__(self, brain: Optional[pynlm.Brain] = None):
        if not PYNLM_AVAILABLE:
            raise RuntimeError("NLM not available")
        
        self.brain = brain
        self.metrics_history: List[Dict[str, Any]] = []
        self.timers: Dict[str, Dict[str, Any]] = {}
        self.start_time = time.time()
        self.last_step_time = time.time()
        self.step_times: deque = deque(maxlen=1000)
        self.cpu_samples: deque = deque(maxlen=1000)
        self.memory_samples: deque = deque(maxlen=1000)
        self.thread_samples: deque = deque(maxlen=1000)
    
    def start_timer(self, operation_name: str):
        """Start timing an operation"""
        self.timers[operation_name] = {
            'start': time.time(),
            'samples': self.timers.get(operation_name, {}).get('samples', [])
        }
    
    def stop_timer(self, operation_name: str) -> float:
        """Stop timing an operation and return duration"""
        if operation_name in self.timers:
            duration = time.time() - self.timers[operation_name]['start']
            self.timers[operation_name]['samples'].append(duration)
            return duration
        return 0.0
    
    def record_step_performance(self, step: int):
        """Record performance metrics for a simulation step"""
        current_time = time.time()
        step_duration = current_time - self.last_step_time
        
        # Record step timing
        self.step_times.append(step_duration)
        
        # Collect system metrics
        process = psutil.Process()
        
        # CPU usage
        cpu_percent = process.cpu_percent()
        self.cpu_samples.append(cpu_percent)
        
        # Memory usage
        memory_info = process.memory_info()
        memory_mb = memory_info.rss / 1024 / 1024
        self.memory_samples.append(memory_mb)
        
        # Thread count
        thread_count = process.num_threads()
        self.thread_samples.append(thread_count)
        
        # Brain-specific metrics (if available)
        brain_metrics = {}
        if self.brain:
            brain_metrics = {
                'neuron_count': self.brain.getTotalNeuronCount(),
                'synapse_count': self.brain.getTotalSynapseCount(),
                'active_neurons': self.brain.getActiveNeuronCount(),
                'firing_neurons': self.brain.getFiringNeuronCount(),
                'total_spikes': self.brain.getTotalSpikeCount(),
                'firing_rate': self.brain.getAverageFiringRate(),
                'ei_ratio': self.brain.getExcitationInhibitionRatio()
            }
        
        # Record metrics
        metric_record = {
            'step': step,
            'timestamp': current_time,
            'step_duration': step_duration,
            'cpu_percent': cpu_percent,
            'memory_mb': memory_mb,
            'thread_count': thread_count,
            'brain_metrics': brain_metrics
        }
        
        self.metrics_history.append(metric_record)
        self.last_step_time = current_time
        
        return metric_record
    
    def get_performance_summary(self) -> Dict[str, Any]:
        """Get comprehensive performance summary"""
        if not self.metrics_history:
            return {}
        
        current_metrics = self.metrics_history[-1]
        
        # Calculate statistics
        step_times = list(self.step_times)
        cpu_samples = list(self.cpu_samples)
        memory_samples = list(self.memory_samples)
        
        summary = {
            'current': current_metrics,
            'total_runtime': time.time() - self.start_time,
            'steps_completed': len(self.metrics_history),
            'statistics': {}
        }
        
        if step_times:
            summary['statistics']['step_duration'] = {
                'current': step_times[-1],
                'average': np.mean(step_times),
                'median': np.median(step_times),
                'min': np.min(step_times),
                'max': np.max(step_times),
                'std': np.std(step_times),
                '95th_percentile': np.percentile(step_times, 95) if len(step_times) >= 10 else step_times[-1]
            }
        
        if cpu_samples:
            summary['statistics']['cpu_usage'] = {
                'current': cpu_samples[-1],
                'average': np.mean(cpu_samples),
                'median': np.median(cpu_samples),
                'max': np.max(cpu_samples)
            }
        
        if memory_samples:
            summary['statistics']['memory_usage'] = {
                'current': memory_samples[-1],
                'average': np.mean(memory_samples),
                'max': np.max(memory_samples),
                'min': np.min(memory_samples)
            }
        
        if self.thread_samples:
            summary['statistics']['thread_count'] = {
                'current': self.thread_samples[-1],
                'average': np.mean(self.thread_samples),
                'max': np.max(self.thread_samples)
            }
        
        # Timer statistics
        timer_stats = {}
        for timer_name, timer_data in self.timers.items():
            samples = timer_data['samples']
            if samples:
                timer_stats[timer_name] = {
                    'count': len(samples),
                    'total': sum(samples),
                    'average': np.mean(samples),
                    'min': np.min(samples),
                    'max': np.max(samples),
                    'std': np.std(samples)
                }
        
        summary['statistics']['timers'] = timer_stats
        
        return summary
    
    def save_metrics(self, filepath: str):
        """Save performance metrics to file"""
        with open(filepath, 'w') as f:
            json.dump(self.metrics_history, f, indent=2, default=str)
    
    def load_metrics(self, filepath: str):
        """Load performance metrics from file"""
        with open(filepath, 'r') as f:
            self.metrics_history = json.load(f)
class BenchmarkSuite:
    """Comprehensive benchmarking suite for NLM"""
    
    def __init__(self):
        self.benchmark_results: List[Dict[str, Any]] = []
    
    def benchmark_brain_creation(self, config_dict: Dict[str, Any], 
                                iterations: int = 10) -> Dict[str, Any]:
        """Benchmark brain creation performance"""
        if not PYNLM_AVAILABLE:
            return {"error": "NLM not available"}
        
        times = []
        for i in range(iterations):
            start = time.time()
            config = pynlm.createDefaultConfig()
            for key, value in config_dict.items():
                config.set(key, value)
            brain = pynlm.createBrain(config)
            brain.initialize()
            end = time.time()
            times.append(end - start)
        
        result = {
            'test': 'brain_creation',
            'iterations': iterations,
            'config': config_dict,
            'times': times,
            'statistics': {
                'mean': np.mean(times),
                'median': np.median(times),
                'std': np.std(times),
                'min': np.min(times),
                'max': np.max(times)
            }
        }
        
        self.benchmark_results.append(result)
        return result
    
    def benchmark_step_performance(self, config_dict: Dict[str, Any], 
                                  steps: int = 100, iterations: int = 5) -> Dict[str, Any]:
        """Benchmark step performance"""
        if not PYNLM_AVAILABLE:
            return {"error": "NLM not available"}
        
        all_step_times = []
        
        for i in range(iterations):
            config = pynlm.createDefaultConfig()
            for key, value in config_dict.items():
                config.set(key, value)
            brain = pynlm.createBrain(config)
            brain.initialize()
            
            step_times = []
            for step in range(steps):
                step_start = time.time()
                brain.step(step)
                step_end = time.time()
                step_times.append(step_end - step_start)
            
            all_step_times.append(step_times)
        
        # Flatten all step times
        flat_times = [t for iteration_times in all_step_times for t in iteration_times]
        
        result = {
            'test': 'step_performance',
            'config': config_dict,
            'steps_per_iteration': steps,
            'iterations': iterations,
            'times': flat_times,
            'statistics': {
                'mean': np.mean(flat_times),
                'median': np.median(flat_times),
                'std': np.std(flat_times),
                'min': np.min(flat_times),
                'max': np.max(flat_times),
                '95th_percentile': np.percentile(flat_times, 95) if len(flat_times) >= 10 else flat_times[-1],
                'steps_per_second': len(flat_times) / sum(flat_times) if sum(flat_times) > 0 else 0
            }
        }
        
        self.benchmark_results.append(result)
        return result
    
    def benchmark_memory_usage(self, config_dict: Dict[str, Any], 
                              steps: int = 100) -> Dict[str, Any]:
        """Benchmark memory usage"""
        if not PYNLM_AVAILABLE:
            return {"error": "NLM not available"}
        
        config = pynlm.createDefaultConfig()
        for key, value in config_dict.items():
            config.set(key, value)
        
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Get initial memory
        process = psutil.Process()
        initial_memory = process.memory_info().rss / 1024 / 1024  # MB
        
        # Run steps
        for step in range(steps):
            brain.step(step)
            if step % 10 == 0:
                current_memory = process.memory_info().rss / 1024 / 1024
                time.sleep(0.01)  # Small delay
        
        # Get final memory
        final_memory = process.memory_info().rss / 1024 / 1024
        
        result = {
            'test': 'memory_usage',
            'config': config_dict,
            'steps': steps,
            'initial_memory_mb': initial_memory,
            'final_memory_mb': final_memory,
            'memory_increase_mb': final_memory - initial_memory,
            'memory_increase_percent': ((final_memory - initial_memory) / initial_memory * 100) if initial_memory > 0 else 0
        }
        
        self.benchmark_results.append(result)
        return result
    
    def run_comprehensive_benchmark(self, config_dict: Dict[str, Any]) -> List[Dict[str, Any]]:
        """Run comprehensive benchmark suite"""
        print(f"Running comprehensive benchmark with config: {config_dict}")
        
        results = []
        
        # Brain creation benchmark
        print("  Benchmarking brain creation...")
        creation_result = self.benchmark_brain_creation(config_dict)
        results.append(creation_result)
        print(f"    Mean creation time: {creation_result['statistics']['mean']*1000:.2f} ms")
        
        # Step performance benchmark
        print("  Benchmarking step performance...")
        step_result = self.benchmark_step_performance(config_dict)
        results.append(step_result)
        print(f"    Mean step time: {step_result['statistics']['mean']*1000:.2f} ms")
        print(f"    Steps per second: {step_result['statistics']['steps_per_second']:.1f}")
        
        # Memory usage benchmark
        print("  Benchmarking memory usage...")
        memory_result = self.benchmark_memory_usage(config_dict)
        results.append(memory_result)
        print(f"    Memory increase: {memory_result['memory_increase_mb']:.2f} MB")
        
        return results
    
    def save_results(self, filepath: str):
        """Save benchmark results to file"""
        with open(filepath, 'w') as f:
            json.dump(self.benchmark_results, f, indent=2, default=str)
    
    def load_results(self, filepath: str):
        """Load benchmark results from file"""
        with open(filepath, 'r') as f:
            self.benchmark_results = json.load(f)
class RealTimeMonitor:
    """Real-time performance monitor for NLM simulations"""
    
    def __init__(self, update_interval: float = 1.0):
        self.update_interval = update_interval
        self.is_monitoring = False
        self.monitor_thread = None
        self.performance_data: List[Dict[str, Any]] = []
        self.callbacks: List[Callable] = []
        self.monitoring_process = None
        
        if PYNLM_AVAILABLE:
            import pynlm
            self.monitoring_process = pynlm.createDefaultConfig()
    
    def add_callback(self, callback: Callable):
        """Add a callback function for monitoring updates"""
        self.callbacks.append(callback)
    
    def start_monitoring(self, brain: Optional[pynlm.Brain] = None):
        """Start real-time monitoring"""
        self.is_monitoring = True
        self.monitor_thread = threading.Thread(target=self._monitor_loop, args=(brain,))
        self.monitor_thread.daemon = True
        self.monitor_thread.start()
        print("Started real-time monitoring")
    
    def stop_monitoring(self):
        """Stop real-time monitoring"""
        self.is_monitoring = False
        if self.monitor_thread:
            self.monitor_thread.join(timeout=5.0)
        print("Stopped real-time monitoring")
    
    def _monitor_loop(self, brain: Optional[pynlm.Brain]):
        """Main monitoring loop"""
        while self.is_monitoring:
            try:
                # Collect system metrics
                process = psutil.Process()
                
                metric = {
                    'timestamp': time.time(),
                    'cpu_percent': process.cpu_percent(),
                    'memory_mb': process.memory_info().rss / 1024 / 1024,
                    'thread_count': process.num_threads()
                }
                
                # Add brain metrics if available
                if brain:
                    metric.update({
                        'neuron_count': brain.getTotalNeuronCount(),
                        'firing_rate': brain.getAverageFiringRate(),
                        'active_neurons': brain.getActiveNeuronCount(),
                        'total_spikes': brain.getTotalSpikeCount()
                    })
                
                self.performance_data.append(metric)
                
                # Call callbacks
                for callback in self.callbacks:
                    try:
                        callback(metric)
                    except Exception as e:
                        print(f"Monitoring callback error: {e}")
                
                # Sleep until next update
                time.sleep(self.update_interval)
                
            except Exception as e:
                print(f"Monitoring error: {e}")
                time.sleep(self.update_interval)
    
    def get_recent_data(self, seconds: float = 60.0) -> List[Dict[str, Any]]:
        """Get recent monitoring data"""
        cutoff_time = time.time() - seconds
        return [d for d in self.performance_data if d['timestamp'] >= cutoff_time]
    
    def get_summary(self, seconds: float = 60.0) -> Dict[str, Any]:
        """Get performance summary for recent data"""
        data = self.get_recent_data(seconds)
        if not data:
            return {}
        
        # Calculate statistics
        cpu_values = [d['cpu_percent'] for d in data]
        memory_values = [d.get('memory_mb', 0) for d in data]
        
        summary = {
            'time_window': seconds,
            'data_points': len(data),
            'cpu_usage': {
                'average': np.mean(cpu_values) if cpu_values else 0,
                'max': np.max(cpu_values) if cpu_values else 0,
                'min': np.min(cpu_values) if cpu_values else 0
            },
            'memory_usage': {
                'average': np.mean(memory_values) if memory_values else 0,
                'max': np.max(memory_values) if memory_values else 0,
                'min': np.min(memory_values) if memory_values else 0
            },
            'latest': data[-1]
        }
        
        return summary
class NLMAnalyzer:
    """High-level NLM analysis tools"""
    
    def __init__(self):
        self.profilers: Dict[str, PerformanceProfiler] = {}
        self.benchmarks: BenchmarkSuite = BenchmarkSuite()
    
    def profile_experiment(self, experiment_name: str, brain: pynlm.Brain,
                          steps: int = 100) -> PerformanceProfiler:
        """Profile an experiment and return profiler"""
        profiler = PerformanceProfiler(brain)
        self.profilers[experiment_name] = profiler
        
        print(f"Profiling experiment: {experiment_name}")
        
        for step in range(steps):
            profiler.record_step_performance(step)
            brain.step(step)
            
            if step % 20 == 0:
                print(f"  Step {step}/{steps}")
        
        print(f"Profile complete for {experiment_name}")
        return profiler
    
    def run_benchmark_suite(self, config_dict: Dict[str, Any]) -> List[Dict[str, Any]]:
        """Run complete benchmark suite"""
        return self.benchmarks.run_comprehensive_benchmark(config_dict)
    
    def analyze_performance_trends(self, profiler: PerformanceProfiler, 
                                   window_size: int = 50) -> Dict[str, Any]:
        """Analyze performance trends"""
        if not profiler.metrics_history:
            return {}
        
        # Extract recent metrics
        recent_metrics = profiler.metrics_history[-window_size:]
        
        # Analyze step duration trends
        step_durations = [m['step_duration'] for m in recent_metrics]
        
        # Detect trends (simple linear regression)
        if len(step_durations) > 1:
            x = np.arange(len(step_durations))
            y = np.array(step_durations)
            
            # Calculate trend
            slope = np.polyfit(x, y, 1)[0]
            correlation = np.corrcoef(x, y)[0, 1] if len(x) > 1 else 0
            
            trend_analysis = {
                'step_duration_trend': 'increasing' if slope > 0.001 else 'decreasing' if slope < -0.001 else 'stable',
                'trend_slope': slope,
                'correlation': correlation,
                'average_step_duration': np.mean(step_durations),
                'std_step_duration': np.std(step_durations)
            }
        else:
            trend_analysis = {'error': 'Insufficient data for trend analysis'}
        
        return trend_analysis
    
    def generate_performance_report(self, profiler: PerformanceProfiler) -> Dict[str, Any]:
        """Generate comprehensive performance report"""
        summary = profiler.get_performance_summary()
        trend_analysis = self.analyze_performance_trends(profiler)
        
        report = {
            'generated_at': datetime.now().isoformat(),
            'summary': summary,
            'trend_analysis': trend_analysis,
            'recommendations': self._generate_recommendations(summary, trend_analysis)
        }
        
        return report
    
    def _generate_recommendations(self, summary: Dict[str, Any], 
                                  trend_analysis: Dict[str, Any]) -> List[str]:
        """Generate performance recommendations"""
        recommendations = []
        
        if 'statistics' in summary:
            step_stats = summary['statistics'].get('step_duration', {})
            if step_stats.get('average', 0) > 0.1:  # More than 100ms per step
                recommendations.append(
                    "Step performance is slow (>100ms per step). Consider:
                    - Optimizing brain configuration
                    - Reducing neural population size
                    - Implementing algorithmic optimizations"
                )
            
            cpu_stats = summary['statistics'].get('cpu_usage', {})
            if cpu_stats.get('max', 0) > 80:
                recommendations.append(
                    "High CPU usage (>80%). Consider:
                    - Parallelizing operations
                    - Optimizing code efficiency
                    - Reducing simulation complexity"
                )
            
            memory_stats = summary['statistics'].get('memory_usage', {})
            if memory_stats.get('max', 0) > 1000:  # More than 1GB
                recommendations.append(
                    "High memory usage (>1GB). Consider:
                    - Using memory pools
                    - Reducing neuron/synapse counts
                    - Implementing more efficient data structures"
                )
        
        if 'step_duration_trend' in trend_analysis:
            trend = trend_analysis['step_duration_trend']
            if trend == 'increasing':
                recommendations.append(
                    "Performance degrading over time. Investigate:
                    - Memory leaks
                    - Increasing computational complexity
                    - Resource exhaustion"
                )
        
        if not recommendations:
            recommendations.append("Performance looks good! Consider further optimization if needed.")
        
        return recommendations
class PerformanceReporter:
    """Professional performance reporting"""
    
    def __init__(self, output_dir: str = "reports"):
        self.output_dir = output_dir
        os.makedirs(output_dir, exist_ok=True)
    
    def generate_html_report(self, report_data: Dict[str, Any], 
                           experiment_name: str) -> str:
        """Generate HTML performance report"""
        html = f"""
<!DOCTYPE html>
<html>
<head>
    <title>NLM Performance Report - {experiment_name}</title>
    <style>
        body {{ font-family: Arial, sans-serif; margin: 20px; }}
        .header {{ background-color: #f0f0f0; padding: 20px; border-radius: 5px; }}
        .metric {{ background-color: #f9f9f9; padding: 15px; margin: 10px 0; border-radius: 3px; }}
        .good {{ border-left: 5px solid #4CAF50; }}
        .warning {{ border-left: 5px solid #FF9800; }}
        .bad {{ border-left: 5px solid #f44336; }}
        table {{ border-collapse: collapse; width: 100%; }}
        th, td {{ border: 1px solid #ddd; padding: 8px; text-align: left; }}
        th {{ background-color: #f2f2f2; }}
        .recommendations ul {{ margin: 0; padding-left: 20px; }}
    </style>
</head>
<body>
    <div class="header">
        <h1>NLM Performance Report</h1>
        <h2>{experiment_name}</h2>
        <p>Generated: {datetime.now().strftime('%Y-%m-%d %H:%M:%S')}</p>
    </div>
        """
        
        # Add key metrics
        html += "<div class='metric good'><h3>Key Performance Metrics</h3><table>"
        
        current = report_data.get('summary', {}).get('current', {})
        if current:
            html += f"""
                <tr><td>Steps Completed</td><td>{current.get('step', 'N/A')}</td></tr>
                <tr><td>Average Step Duration</td><td>{current.get('step_duration', 'N/A')}</td></tr>
                <tr><td>CPU Usage</td><td>{current.get('cpu_percent', 'N/A')}%</td></tr>
                <tr><td>Memory Usage</td><td>{current.get('memory_mb', 'N/A')} MB</td></tr>
            """
        
        html += "</table></div>"
        
        # Add trend analysis
        html += """
    <div class='metric warning'><h3>Trend Analysis</h3>
        <p>Trend: <strong>{report_data.get('trend_analysis', {}).get('step_duration_trend', 'N/A')}</strong></p>
        <p>Correlation: {report_data.get('trend_analysis', {}).get('correlation', 'N/A'):.3f}</p>
    </div>
        """
        
        # Add recommendations
        html += """
    <div class='metric'><h3>Recommendations</h3>
        <div class='recommendations'>
            <ul>
        """
        
        for recommendation in report_data.get('recommendations', []):
            html += f"<li>{recommendation}</li>"
        
        html += """
            </ul>
        </div>
    </div>
        """
        
        # Close HTML
        html += """
</body>
</html>
        """
        
        # Save HTML file
        output_path = os.path.join(self.output_dir, f"{experiment_name}_report.html")
        with open(output_path, 'w') as f:
            f.write(html)
        
        return output_path
    
    def generate_csv_report(self, profiler: PerformanceProfiler, 
                           experiment_name: str) -> str:
        """Generate CSV performance report"""
        output_path = os.path.join(self.output_dir, f"{experiment_name}_metrics.csv")
        
        with open(output_path, 'w', newline='') as csvfile:
            fieldnames = ['step', 'timestamp', 'step_duration', 
                         'cpu_percent', 'memory_mb', 'thread_count',
                         'neuron_count', 'firing_rate', 'active_neurons',
                         'total_spikes', 'ei_ratio']
            writer = csv.DictWriter(csvfile, fieldnames=fieldnames)
            writer.writeheader()
            
            for metric in profiler.metrics_history:
                writer.writerow(metric)
        
        return output_path
if __name__ == "__main__":
    print("NLM Performance Monitoring and Profiling Tools")
    print("Version: Advanced Performance Suite")
    print("=" * 60)
    
    if PYNLM_AVAILABLE:
        print("NLM available: Yes")
        print("Available tools:")
        print("• PerformanceProfiler - Detailed performance measurement")
        print("• BenchmarkSuite - Comprehensive benchmarking")
        print("• RealTimeMonitor - Real-time monitoring")
        print("• NLMAnalyzer - High-level performance analysis")
        print("• PerformanceReporter - Professional reporting")
    else:
        print("NLM available: No")
        print("Install pynlm to use performance monitoring tools:")
        print("  pip install pynlm")
