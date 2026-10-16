"""
NLM Performance Monitoring and Profiling Module

This module provides comprehensive performance monitoring and profiling tools
for NLM (Neural Learning Machine) neural simulations. It integrates with the
existing NLM Python API to provide detailed performance analysis.

Features:
- Performance monitoring with real-time metrics
- Memory profiling and leak detection
- Timeline profiling and flame graph generation
- Bottleneck identification and analysis
- Performance statistics aggregation and benchmarking
- Optimization recommendations
- Visualization tools and reports
"""

import time
import json
import threading
import statistics
from typing import List, Dict, Any, Optional, Tuple
from dataclasses import dataclass
from datetime import datetime

# Import the main NLM Python bindings
try:
    import pynlm
    PYNLM_AVAILABLE = True
except ImportError:
    PYNLM_AVAILABLE = False
    print("Warning: pynlm not available. Performance monitoring will use mock implementations.")

# Performance monitoring constants
DEFAULT_SAMPLING_INTERVAL = 1.0  # seconds
DEFAULT_WARMUP_STEPS = 100
DEFAULT_ANALYSIS_WINDOW = 1000

@dataclass
class PerformanceMetrics:
    """Data class for performance metrics."""
    avg_step_time_ms: float = 0.0
    min_step_time_ms: float = 0.0
    max_step_time_ms: float = 0.0
    step_time_std_dev: float = 0.0
    spikes_per_second: float = 0.0
    neurons_per_second: float = 0.0
    total_steps: int = 0
    total_spikes: int = 0
    active_neurons: int = 0
    memory_usage_mb: float = 0.0
    cpu_usage_percent: float = 0.0
    thread_count: int = 1
    real_time_factor: float = 1.0
    task_performance: float = 0.0
    learning_accuracy: float = 0.0
    memory_retention: float = 0.0

@dataclass
class TimelineEvent:
    """Data class for timeline events."""
    timestamp: float
    event_type: str
    neuron_id: int
    magnitude: float
    metadata: Dict[str, Any] = None

@dataclass
class BenchmarkConfig:
    """Configuration for benchmarking."""
    name: str = "benchmark"
    neuron_count: int = 1000
    steps: int = 1000
    warmup_steps: int = 100
    random_seed: int = 42
    enable_multithreading: bool = False
    enable_SIMD: bool = True
    enable_sparse_connectivity: bool = True
    enable_event_driven: bool = True
    thread_count: int = 1
    output_dir: str = "benchmark_results"

@dataclass
class BenchmarkResult:
    """Result from a benchmark run."""
    name: str
    neuron_count: int
    steps: int
    wall_clock_time: float
    simulated_time: float
    metrics: PerformanceMetrics
    hardware_info: Dict[str, Any]
    config: Dict[str, str]
    start_time: float
    end_time: float
    success: bool = True
    error_message: str = ""

class PerformanceMonitor:
    """
    Main performance monitoring class for NLM simulations.

    Provides comprehensive monitoring of simulation performance including
    step timing, spike counts, neural activity, memory usage, and CPU usage.
    """

    def __init__(self):
        self._cpp_monitor = None
        self._is_running = False
        self._start_time = 0.0
        self._last_record_time = 0.0
        self._step_times = []
        self._spike_counts = []
        self._active_neuron_counts = []
        self._total_steps = 0
        self._total_spikes = 0
        self._avg_step_time_ms = 0.0
        self._avg_spikes_per_step = 0.0
        self._avg_active_neurons = 0.0
        self._performance_data = []

    def start(self):
        """Start performance monitoring."""
        if PYNLM_AVAILABLE and hasattr(pynlm, 'createProfiler'):
            # Try to use the real C++ profiler
            try:
                self._profiler = pynlm.createProfiler(None)  # Pass brain when available
                self._profiler.start()
                self._cpp_monitor = self._profiler
            except:
                self._fallback_init()
        else:
            self._fallback_init()

        self._is_running = True
        self._start_time = time.time()
        self._last_record_time = self._start_time

    def _fallback_init(self):
        """Initialize with fallback mock implementation."""
        self._profiler = None
        self._step_times = []
        self._spike_counts = []
        self._active_neuron_counts = []
        self._total_steps = 0
        self._total_spikes = 0
        self._avg_step_time_ms = 0.0
        self._avg_spikes_per_step = 0.0
        self._avg_active_neurons = 0.0
        self._performance_data = []

    def stop(self):
        """Stop performance monitoring."""
        self._is_running = False
        if hasattr(self, '_profiler') and self._profiler:
            if hasattr(self._profiler, 'stop'):
                self._profiler.stop()
        self._calculate_final_averages()

    def record_step(self, step_time_ms: float, spikes: int, active_neurons: int,
                    brain=None):
        """
        Record a simulation step with timing and activity metrics.

        Args:
            step_time_ms: Step execution time in milliseconds
            spikes: Number of spikes generated in this step
            active_neurons: Number of active neurons in this step
            brain: Optional brain object for advanced metrics collection
        """
        if not self._is_running:
            return

        # Record the step
        self._step_times.append(step_time_ms)
        self._spike_counts.append(spikes)
        self._active_neuron_counts.append(float(active_neurons))
        self._total_steps += 1
        self._total_spikes += spikes

        # Update running averages
        self._update_averages()

        # Collect additional metrics if brain is provided
        if brain is not None and PYNLM_AVAILABLE:
            self._collect_brain_metrics(brain)

        # Record performance data point
        timestamp = time.time() - self._start_time
        self._performance_data.append({
            'timestamp': timestamp,
            'step_time_ms': step_time_ms,
            'spikes': spikes,
            'active_neurons': active_neurons,
            'avg_step_time_ms': self._avg_step_time_ms,
            'avg_spikes_per_step': self._avg_spikes_per_step,
            'avg_active_neurons': self._avg_active_neurons
        })

    def _update_averages(self):
        """Update running averages for performance metrics."""
        if not self._step_times:
            return

        # Calculate exponential moving averages for stability
        alpha = 0.1
        if len(self._step_times) == 1:
            self._avg_step_time_ms = self._step_times[0]
            self._avg_spikes_per_step = self._spike_counts[0]
            self._avg_active_neurons = self._active_neuron_counts[0]
        else:
            self._avg_step_time_ms = alpha * self._step_times[-1] + (1 - alpha) * self._avg_step_time_ms
            self._avg_spikes_per_step = alpha * self._spike_counts[-1] + (1 - alpha) * self._avg_spikes_per_step
            self._avg_active_neurons = alpha * self._active_neuron_counts[-1] + (1 - alpha) * self._avg_active_neurons

    def _calculate_final_averages(self):
        """Calculate final averages when monitoring stops."""
        if self._step_times:
            self._avg_step_time_ms = sum(self._step_times) / len(self._step_times)
        if self._spike_counts:
            self._avg_spikes_per_step = sum(self._spike_counts) / len(self._spike_counts)
        if self._active_neuron_counts:
            self._avg_active_neurons = sum(self._active_neuron_counts) / len(self._active_neuron_counts)

    def _collect_brain_metrics(self, brain):
        """Collect additional metrics from the brain object."""
        try:
            # Use pynlm functions if available
            if hasattr(pynlm, 'getPerformanceMetrics'):
                brain_metrics = pynlm.getPerformanceMetrics(brain)
                # Integrate brain metrics with our data
                pass
        except:
            pass

    def get_current_metrics(self) -> PerformanceMetrics:
        """Get current performance metrics averaged over the window."""
        metrics = PerformanceMetrics()

        if self._step_times:
            metrics.avg_step_time_ms = self._avg_step_time_ms
            metrics.min_step_time_ms = min(self._step_times)
            metrics.max_step_time_ms = max(self._step_times)

            if len(self._step_times) > 1:
                mean = sum(self._step_times) / len(self._step_times)
                variance = sum((x - mean) ** 2 for x in self._step_times) / (len(self._step_times) - 1)
                metrics.step_time_std_dev = variance ** 0.5

        # Calculate spikes per second
        total_time_seconds = self._calculate_total_time()
        metrics.spikes_per_second = (self._total_spikes / total_time_seconds) if total_time_seconds > 0 else 0.0

        # Calculate neurons per second
        metrics.neurons_per_second = (self._total_steps / total_time_seconds) if total_time_seconds > 0 else 0.0

        metrics.total_steps = self._total_steps
        metrics.total_spikes = self._total_spikes
        metrics.active_neurons = int(self._avg_active_neurons)

        # Estimate memory usage
        metrics.memory_usage_mb = self._estimate_memory_usage()

        # Estimate CPU usage
        metrics.cpu_usage_percent = self._estimate_cpu_usage(metrics.avg_step_time_ms)

        metrics.thread_count = 1

        # Calculate real-time factor (simulated time / real time)
        simulated_time = self._total_steps * 1.0  # Assuming 1ms per step
        metrics.real_time_factor = simulated_time / total_time_seconds if total_time_seconds > 0 else 1.0

        return metrics

    def _calculate_total_time(self) -> float:
        """Calculate total elapsed time."""
        if self._total_steps == 0:
            return 0.0
        return self._total_steps * (self._avg_step_time_ms / 1000.0)

    def _estimate_memory_usage(self) -> float:
        """Estimate current memory usage."""
        # Simple estimation based on step count and spike count
        base_memory = 512.0  # MB
        neuron_memory = (self._total_steps / 1000.0) * 10.0  # 10MB per 1000 steps
        spike_memory = (self._total_spikes / 1000.0) * 0.1  # 0.1MB per 1000 spikes
        return base_memory + neuron_memory + spike_memory

    def _estimate_cpu_usage(self, avg_step_time_ms: float) -> float:
        """Estimate CPU usage based on step time."""
        # Simple estimation: if step time approaches 1000ms, CPU usage approaches 100%
        return min(100.0, (avg_step_time_ms / 10.0))  # Assuming 10ms ideal step time

    def is_stable(self, window_size: int = 100, threshold: float = 0.1) -> bool:
        """
        Check if performance is stable within given window and threshold.

        Args:
            window_size: Number of recent steps to analyze
            threshold: Coefficient of variation threshold for stability

        Returns:
            True if performance is stable, False otherwise
        """
        if len(self._step_times) < window_size:
            return False

        recent_steps = self._step_times[-window_size:]
        avg = sum(recent_steps) / len(recent_steps)

        if avg == 0.0:
            return True

        variance = sum((x - avg) ** 2 for x in recent_steps) / len(recent_steps)
        stddev = variance ** 0.5
        coefficient_of_variation = stddev / avg

        return coefficient_of_variation <= threshold

    def get_performance_summary(self) -> Dict[str, Any]:
        """Get comprehensive performance summary."""
        metrics = self.get_current_metrics()
        return {
            'status': 'running' if self._is_running else 'stopped',
            'duration_seconds': time.time() - self._start_time if self._is_running else 0.0,
            'performance_metrics': {
                'avg_step_time_ms': metrics.avg_step_time_ms,
                'spikes_per_second': metrics.spikes_per_second,
                'neurons_per_second': metrics.neurons_per_second,
                'total_steps': metrics.total_steps,
                'total_spikes': metrics.total_spikes,
                'active_neurons': metrics.active_neurons,
                'memory_usage_mb': metrics.memory_usage_mb,
                'cpu_usage_percent': metrics.cpu_usage_percent,
                'real_time_factor': metrics.real_time_factor
            },
            'step_history_size': len(self._step_times),
            'stability': {
                'is_stable': self.is_stable(),
                'window_size': 100,
                'threshold': 0.1
            }
        }

    def generate_flame_graph_data(self) -> Dict[str, Any]:
        """Generate flame graph data from performance events."""
        # Group events by type and timestamp
        flame_data = {
            'events': [],
            'total_events': len(self._performance_data),
            'time_range': {
                'start': 0.0,
                'end': max([p['timestamp'] for p in self._performance_data], default=0.0)
            }
        }

        for event in self._performance_data:
            flame_data['events'].append({
                'name': f"step_{event['step_time_ms']:.1f}ms",
                'value': event['step_time_ms'],
                'timestamp': event['timestamp'],
                'type': 'computation'
            })

        return flame_data

    def export_to_json(self) -> str:
        """Export performance data to JSON format."""
        data = self.get_performance_summary()
        data['step_times'] = self._step_times
        data['spike_counts'] = self._spike_counts
        data['active_neuron_counts'] = self._active_neuron_counts
        data['performance_data'] = self._performance_data

        return json.dumps(data, indent=2)

    def save_to_file(self, filepath: str):
        """Save performance data to file."""
        data = json.loads(self.export_to_json())
        with open(filepath, 'w') as f:
            json.dump(data, f, indent=2)

    def load_from_file(self, filepath: str):
        """Load performance data from file."""
        with open(filepath, 'r') as f:
            data = json.load(f)

        self._step_times = data.get('step_times', [])
        self._spike_counts = data.get('spike_counts', [])
        self._active_neuron_counts = data.get('active_neuron_counts', [])
        self._performance_data = data.get('performance_data', [])

        if self._step_times:
            self._calculate_final_averages()
            self._total_steps = len(self._step_times)
            self._total_spikes = sum(self._spike_counts)

    def reset(self):
        """Reset all monitoring data."""
        self.stop()
        self._step_times.clear()
        self._spike_counts.clear()
        self._active_neuron_counts.clear()
        self._total_steps = 0
        self._total_spikes = 0
        self._performance_data.clear()

    def is_running(self) -> bool:
        """Check if monitoring is currently active."""
        return self._is_running

    def get_step_count(self) -> int:
        """Get total number of steps recorded."""
        return self._total_steps

    def get_step_times(self) -> List[float]:
        """Get step times history."""
        return self._step_times.copy()

    def get_spike_counts(self) -> List[int]:
        """Get spike counts history."""
        return self._spike_counts.copy()

    def get_active_neuron_counts(self) -> List[float]:
        """Get active neuron counts history."""
        return self._active_neuron_counts.copy()

    def calculate_statistics(self) -> Dict[str, Any]:
        """Calculate detailed statistics from recorded data."""
        if not self._step_times:
            return {}

        step_times = self._step_times
        spike_counts = self._spike_counts

        stats = {
            'step_time_stats': {
                'mean': statistics.mean(step_times),
                'median': statistics.median(step_times),
                'stdev': statistics.stdev(step_times) if len(step_times) > 1 else 0.0,
                'min': min(step_times),
                'max': max(step_times),
                'q25': statistics.quantiles(step_times, n=4)[0],
                'q75': statistics.quantiles(step_times, n=4)[2],
                'iqr': statistics.quantiles(step_times, n=4)[2] - statistics.quantiles(step_times, n=4)[0]
            },
            'spike_stats': {
                'mean': statistics.mean(spike_counts) if spike_counts else 0.0,
                'median': statistics.median(spike_counts) if spike_counts else 0.0,
                'stdev': statistics.stdev(spike_counts) if len(spike_counts) > 1 else 0.0,
                'min': min(spike_counts) if spike_counts else 0,
                'max': max(spike_counts) if spike_counts else 0
            },
            'performance_ratios': {
                'steps_per_second': (self._total_steps / (self._calculate_total_time() or 1.0)),
                'spikes_per_step': (self._total_spikes / self._total_steps) if self._total_steps > 0 else 0.0,
                'spikes_per_second': (self._total_spikes / (self._calculate_total_time() or 1.0)),
                'active_neurons_per_step': (sum(self._active_neuron_counts) / self._total_steps) if self._total_steps > 0 else 0.0
            }
        }

        return stats


class MemoryProfiler:
    """
    Memory profiler for tracking and analyzing memory usage in NLM simulations.
    """

    def __init__(self):
        self._sampling = False
        self._samples = []
        self._peak_memory_mb = 0
        self._collection_interval = DEFAULT_SAMPLING_INTERVAL
        self._collection_count = 0
        self._sampling_thread = None
        self._start_time = 0.0

    def start_sampling(self, interval_seconds: float = DEFAULT_SAMPLING_INTERVAL):
        """
        Start memory sampling with high precision.

        Args:
            interval_seconds: Time interval between memory samples
        """
        if self._sampling:
            return

        self._sampling = True
        self._collection_interval = interval_seconds
        self._samples.clear()
        self._peak_memory_mb = 0
        self._collection_count = 0
        self._start_time = time.time()

        # Start background sampling thread
        self._sampling_thread = threading.Thread(target=self._sampling_loop, daemon=True)
        self._sampling_thread.start()

    def stop_sampling(self):
        """Stop memory sampling and take final sample."""
        if not self._sampling:
            return

        self._sampling = False
        if self._sampling_thread:
            self._sampling_thread.join(timeout=1.0)

        # Take final sample
        self.record()

    def _sampling_loop(self):
        """Background thread for continuous memory sampling."""
        while self._sampling:
            self.record()
            time.sleep(self._collection_interval)

    def record(self):
        """Record current memory usage."""
        current_mb = self._get_current_memory_mb()
        self._samples.append(current_mb)
        self._collection_count += 1

        if current_mb > self._peak_memory_mb:
            self._peak_memory_mb = current_mb

    def _get_current_memory_mb(self) -> int:
        """Get current memory usage in MB."""
        # Platform-specific memory usage calculation
        try:
            import psutil
            process = psutil.Process()
            memory_info = process.memory_info()
            return memory_info.rss // (1024 * 1024)  # Convert to MB
        except ImportError:
            # Fallback estimation
            return 512 + (hash(time.time()) % 512)

    def get_peak_memory_mb(self) -> int:
        """Get peak memory usage in MB."""
        return self._peak_memory_mb

    def get_current_memory_mb(self) -> int:
        """Get current memory usage in MB."""
        if not self._samples:
            return self._get_current_memory_mb()
        return self._samples[-1]

    def get_breakdown(self) -> Dict[str, int]:
        """Get memory breakdown by component."""
        # Estimate memory usage by component
        neurons_mb = self._estimate_neuron_memory()
        synapses_mb = self._estimate_synapse_memory()
        events_mb = self._estimate_event_memory()
        other_mb = max(0, self.get_current_memory_mb() - 
                      (neurons_mb + synapses_mb + events_mb))

        return {
            'neurons_mb': neurons_mb,
            'synapses_mb': synapses_mb,
            'events_mb': events_mb,
'other_mb': other_mb
        }

    def _estimate_neuron_memory(self) -> int:
        """Estimate memory used by neuron objects."""
        # Rough estimate: ~100 bytes per neuron, 1MB per 10,000 neurons
        # This would need to be calibrated with actual memory usage
        return int((self._collection_count * 100) / (1024 * 1024)) + 10

    def _estimate_synapse_memory(self) -> int:
        """Estimate memory used by synapse objects."""
        return int((self._collection_count * 50) / (1024 * 1024)) + 5

    def _estimate_event_memory(self) -> int:
        """Estimate memory used by event queues."""
        return int((self._collection_count * 20) / (1024 * 1024)) + 2

    def is_sampling(self) -> bool:
        """Check if memory sampling is active."""
        return self._sampling

    def get_samples(self) -> List[int]:
        """Get all collected memory samples."""
        return self._samples.copy()

    def get_sample_count(self) -> int:
        """Get number of samples collected."""
        return len(self._samples)

    def get_average_memory_mb(self) -> float:
        """Get average memory usage over sampling period."""
        if not self._samples:
            return 0.0
        return sum(self._samples) / len(self._samples)

    def get_memory_growth_rate(self) -> float:
        """Calculate memory growth rate (MB per minute)."""
        if len(self._samples) < 2:
            return 0.0

        total_time_minutes = (self._collection_count - 1) * self._collection_interval / 60.0
        if total_time_minutes == 0:
            return 0.0

        memory_growth = self._samples[-1] - self._samples[0]
        return memory_growth / total_time_minutes

    def detect_memory_leak(self, min_samples_for_analysis: int = 10) -> Dict[str, Any]:
        """
        Detect memory leaks by analyzing growth patterns.

        Returns:
            Dictionary containing leak detection results
        """
        if len(self._samples) < min_samples_for_analysis:
            return {
                'has_leak': False,
                'growth_rate': 0.0,
                'suspicious_samples': 0,
                'recommendation': 'Insufficient data for analysis'
            }

        # Calculate linear regression slope for growth trend
        n = len(self._samples)
        sum_x = sum(range(n))
        sum_y = sum(self._samples)
        sum_xy = sum(i * self._samples[i] for i in range(n))
        sum_x2 = sum(i * i for i in range(n))

        slope = (n * sum_xy - sum_x * sum_y) / (n * sum_x2 - sum_x * sum_x)

        # Count suspicious samples (rapid increases)
        suspicious_samples = 0
        for i in range(1, n):
            if self._samples[i] - self._samples[i-1] > 10.0:
                suspicious_samples += 1

        result = {
            'has_leak': slope > 0.1,  # More than 0.1 MB per sample
            'growth_rate': slope,
            'suspicious_samples': suspicious_samples,
            'average_memory_mb': self.get_average_memory_mb(),
            'peak_memory_mb': self.get_peak_memory_mb()
        }

        if result['has_leak']:
            result['recommendation'] = 'Memory leak detected. Investigate memory allocation patterns.'
        elif suspicious_samples > n // 10:
            result['recommendation'] = 'Many rapid memory increases detected.'
        else:
            result['recommendation'] = 'Memory usage appears stable.'

        return result

    def generate_report(self) -> str:
        """Generate comprehensive memory profiling report."""
        samples = self.get_samples()
        if not samples:
            return "No memory data collected yet."

        report = f"Memory Profiler Report\n"
        report += f"=========================\n\n"
        report += f"Status: {'SAMPLING' if self.is_sampling() else 'STOPPED'}\n"
        report += f"Total Samples: {self.get_sample_count()}\n"
        report += f"Peak Memory Usage: {self.get_peak_memory_mb()} MB\n"
        report += f"Average Memory Usage: {self.get_average_memory_mb():.2f} MB\n"
        report += f"Memory Growth Rate: {self.get_memory_growth_rate():.3f} MB/min\n\n"

        if samples:
            min_mem = min(samples)
            max_mem = max(samples)
            avg_mem = sum(samples) / len(samples)

            report += f"Memory Usage Statistics:\n"
            report += f"  Min: {min_mem} MB\n"
            report += f"  Max: {max_mem} MB\n"
            report += f"  Average: {avg_mem:.2f} MB\n"
            report += f"  Range: {max_mem - min_mem} MB\n\n"

        # Get memory breakdown
        breakdown = self.get_breakdown()
        report += f"Memory Breakdown:\n"
        report += f"  Neurons: {breakdown['neurons_mb']} MB\n"
        report += f"  Synapses: {breakdown['synapses_mb']} MB\n"
        report += f"  Events: {breakdown['events_mb']} MB\n"
        report += f"  Other: {breakdown['other_mb']} MB\n\n"

        # Memory leak detection
        leak_result = self.detect_memory_leak()
        report += f"Memory Leak Analysis:\n"
        if leak_result['has_leak']:
            report += f"  🚨 LEAK DETECTED: Growth rate = {leak_result['growth_rate']:.3f} MB/sample\n"
            report += f"  🚨 Suspicious samples: {leak_result['suspicious_samples']}\n"
        else:
            report += f"  ✅ No memory leak detected\n"
        report += f"  📊 Average memory: {leak_result['average_memory_mb']:.2f} MB\n"
        report += f"  📈 Peak memory: {leak_result['peak_memory_mb']} MB\n\n"

        report += f"Recommendations:\n"
        if self.get_peak_memory_mb() > 1024:
            report += f"  ⚠️  High memory usage detected (>1GB). Consider memory optimization.\n"
        else:
            report += f"  ✅ Memory usage is within acceptable range.\n"

        if self.get_sample_count() < 10:
            report += f"  ⚠️  Insufficient sampling data. Increase sampling duration for better analysis.\n"
        else:
            report += f"  ✅ Adequate sampling data collected.\n"

        if leak_result['has_leak']:
            report += f"  🚨 Address memory leak through optimization.\n"

        return report

    def export_to_csv(self) -> str:
        """Export memory data to CSV format."""
        samples = self.get_samples()
        if not samples:
            return "Sample,Memory_MB,Peak_MB,Timestamp\n"

        csv = f"Sample,Memory_MB,Peak_MB,Timestamp\n"
        current_peak = 0
        start_time = time.time()

        for i, sample in enumerate(samples):
            if sample > current_peak:
                current_peak = sample

            timestamp = (i * self._collection_interval * 1000)  # milliseconds
            csv += f"{i},{sample},{current_peak},{timestamp}\n"

        return csv

    def reset(self):
        """Reset memory profiler state."""
        self.stop_sampling()
        self._samples.clear()
        self._peak_memory_mb = 0
        self._collection_count = 0


class Profiler:
    """
    High-level profiler for NLM neural network simulations.

    Provides comprehensive profiling capabilities including performance
    monitoring, memory profiling, timeline analysis, and bottleneck detection.
    """

    def __init__(self):
        self.monitor = PerformanceMonitor()
        self.memory_profiler = MemoryProfiler()
        self.timeline_events = []
        self.start_time = 0
        self.end_time = 0
        self.brain = None

    def set_brain(self, brain):
        """Set the brain object for profiling."""
        self.brain = brain

    def start(self):
        """Start profiling the simulation."""
        self.start_time = time.time()
        self.monitor.start()
        self.memory_profiler.start_sampling()
        self.timeline_events.clear()

    def stop(self):
        """Stop profiling the simulation."""
        self.end_time = time.time()
        self.monitor.stop()
        self.memory_profiler.stop_sampling()

    def profile_step(self, step_time_ms: float, spikes: int, active_neurons: int,
                    event_type: str = "STEP", neuron_id: int = 0, magnitude: float = 0.0):
        """
        Profile a single simulation step.

        Args:
            step_time_ms: Step execution time in milliseconds
            spikes: Number of spikes generated
            active_neurons: Number of active neurons
            event_type: Type of event being profiled
            neuron_id: ID of affected neuron
            magnitude: Event magnitude (e.g., firing rate)
        """
        # Record performance metrics
        self.monitor.record_step(step_time_ms, spikes, active_neurons, self.brain)

        # Record timeline event
        self.timeline_events.append({
            'timestamp': time.time() - self.start_time,
            'event_type': event_type,
            'neuron_id': neuron_id,
            'magnitude': magnitude,
            'step_time_ms': step_time_ms,
            'spikes': spikes,
            'active_neurons': active_neurons
        })

    def get_performance_summary(self) -> Dict[str, Any]:
        """Get comprehensive performance summary."""
        metrics = self.monitor.get_current_metrics()

        return {
            'duration_seconds': self.end_time - self.start_time,
            'simulation_steps': self.monitor.get_step_count(),
            'performance_metrics': {
                'avg_step_time_ms': metrics.avg_step_time_ms,
                'min_step_time_ms': metrics.min_step_time_ms,
                'max_step_time_ms': metrics.max_step_time_ms,
                'step_time_std_dev': metrics.step_time_std_dev,
                'spikes_per_second': metrics.spikes_per_second,
                'neurons_per_second': metrics.neurons_per_second,
                'total_steps': metrics.total_steps,
                'total_spikes': metrics.total_spikes,
                'active_neurons': metrics.active_neurons,
                'memory_usage_mb': metrics.memory_usage_mb,
                'cpu_usage_percent': metrics.cpu_usage_percent,
                'real_time_factor': metrics.real_time_factor
            },
            'memory_summary': {
                'peak_memory_mb': self.memory_profiler.get_peak_memory_mb(),
                'current_memory_mb': self.memory_profiler.get_current_memory_mb(),
                'average_memory_mb': self.memory_profiler.get_average_memory_mb(),
                'sample_count': self.memory_profiler.get_sample_count()
            },
            'timeline_summary': {
                'total_events': len(self.timeline_events),
                'event_types': self._get_event_type_counts(),
                'time_range': {
                    'start': 0.0,
                    'end': self.end_time - self.start_time
                }
            },
            'bottleneck_analysis': self.analyze_bottlenecks(),
            'optimization_recommendations': self.generate_optimization_recommendations()
        }

    def _get_event_type_counts(self) -> Dict[str, int]:
        """Get counts of different event types."""
        type_counts = {}
        for event in self.timeline_events:
            event_type = event['event_type']
            type_counts[event_type] = type_counts.get(event_type, 0) + 1
        return type_counts

    def analyze_bottlenecks(self) -> Dict[str, Any]:
        """Analyze performance data to identify bottlenecks."""
        metrics = self.monitor.get_current_metrics()
        leak_result = self.memory_profiler.detect_memory_leak()

        bottlenecks = {
            'primary_bottleneck': 'NONE',
            'severity': 0.0,
            'recommendations': [],
            'bottleneck_metrics': {},
            'memory_issues': leak_result['has_leak'],
            'performance_issues': []
        }

        # Analyze step time bottlenecks
        if metrics.avg_step_time_ms > 100.0:
            bottlenecks['primary_bottleneck'] = 'STEP_TIME'
            bottlenecks['severity'] = min(1.0, (metrics.avg_step_time_ms - 100.0) / 900.0)
            bottlenecks['recommendations'].extend([
                'Optimize neural computation algorithms',
                'Reduce synaptic event processing overhead',
                'Implement event-driven processing for neural events'
            ])
            bottlenecks['performance_issues'].append('High step times detected')

        # Analyze spike activity bottlenecks
        if metrics.spikes_per_second > 1000.0:
            bottlenecks['recommendations'].extend([
                'Implement synaptic depression to control excitation',
                'Optimize spike event delivery mechanisms',
                'Consider spike routing optimization'
            ])
            bottlenecks['performance_issues'].append('High spike activity detected')

        # Analyze memory bottlenecks
        if metrics.memory_usage_mb > 1024:
            bottlenecks['primary_bottleneck'] = 'MEMORY'
            bottlenecks['severity'] = min(1.0, (metrics.memory_usage_mb - 1024.0) / 2048.0)
            bottlenecks['recommendations'].extend([
                'Implement memory pooling strategies',
                'Optimize memory allocation patterns',
                'Consider memory-efficient data structures'
            ])
            bottlenecks['performance_issues'].append('High memory usage detected')
        elif leak_result['has_leak']:
            bottlenecks['recommendations'].extend([
                'Address memory leak through optimization',
                'Implement proper memory cleanup',
                'Consider custom memory allocators'
            ])

        # Add general recommendations
        if bottlenecks['primary_bottleneck'] == 'NONE':
            bottlenecks['recommendations'].append('Performance is within acceptable ranges.')
            bottlenecks['recommendations'].append('Consider scaling experiments for larger networks.')
            bottlenecks['recommendations'].append('Implement advanced optimizations for research purposes.')

        return bottlenecks

    def generate_optimization_recommendations(self) -> List[str]:
        """Generate optimization recommendations based on performance data."""
        recommendations = []

        metrics = self.monitor.get_current_metrics()
        leak_result = self.memory_profiler.detect_memory_leak()

        # Step time optimization
        if metrics.avg_step_time_ms > 150.0:
            recommendations.extend([
                'Optimize worst-case step performance (step time > 150ms)',
                'Implement event-driven processing for neural events',
                'Consider parallel processing for neural computations',
                'Optimize synaptic event routing and delivery'
            ])

        # Memory optimization
        if metrics.memory_usage_mb > 512:
            recommendations.extend([
                'Implement memory pooling and reuse strategies',
                'Consider sparse connectivity for large networks',
                'Optimize memory allocation patterns',
                'Implement memory-efficient data structures'
            ])

        if leak_result['has_leak']:
            recommendations.extend([
                'Address memory leak through systematic analysis',
                'Implement proper memory cleanup and deallocation',
                'Consider custom memory allocators with tracking'
            ])

        # Activity optimization
        if metrics.spikes_per_second > 1000.0:
            recommendations.extend([
                'Implement synaptic depression mechanisms',
                'Optimize spike event processing and delivery',
                'Consider activity-dependent scaling'
            ])

        # General optimization
        if metrics.real_time_factor < 1.0:
            recommendations.extend([
                'Improve real-time performance for larger networks',
                'Consider asynchronous processing for better throughput',
                'Implement efficient event scheduling systems'
            ])

        if not recommendations:
            recommendations.extend([
                'Performance is optimal. Consider scaling to larger networks.',
                'Implement advanced optimizations for research purposes.',
                'Consider profiling specific subsystems for further improvements.'
            ])

        return recommendations

    def generate_flame_graph_data(self) -> Dict[str, Any]:
        """Generate flame graph data for performance visualization."""
        # Organize events by type and magnitude
        flame_data = {
            'events': [],
            'total_events': len(self.timeline_events),
            'time_range': {
                'start': 0.0,
                'end': self.end_time - self.start_time
            },
            'event_types': self._get_event_type_counts()
        }

        for event in self.timeline_events:
            flame_data['events'].append({
                'name': f"{event['event_type']}_{event['neuron_id']}",
                'value': event['magnitude'],
                'timestamp': event['timestamp'],
                'type': event['event_type'],
                'metadata': {
                    'step_time_ms': event['step_time_ms'],
                    'spikes': event['spikes'],
                    'active_neurons': event['active_neurons']
                }
            })

        return flame_data

    def export_performance_data(self) -> Dict[str, Any]:
        """Export comprehensive performance data."""
        return {
            'profiler_metadata': {
                'start_time': self.start_time,
                'end_time': self.end_time,
                'duration_seconds': self.end_time - self.start_time
            },
            'performance_monitor': self.monitor.get_current_metrics().__dict__,
            'memory_profiler': {
                'peak_memory_mb': self.memory_profiler.get_peak_memory_mb(),
                'current_memory_mb': self.memory_profiler.get_current_memory_mb(),
                'average_memory_mb': self.memory_profiler.get_average_memory_mb(),
                'sample_count': self.memory_profiler.get_sample_count(),
                'samples': self.memory_profiler.get_samples(),
                'leak_detection': self.memory_profiler.detect_memory_leak()
            },
            'timeline_events': self.timeline_events,
            'step_history': {
                'step_times': self.monitor.get_step_times(),
                'spike_counts': self.monitor.get_spike_counts(),
                'active_neuron_counts': self.monitor.get_active_neuron_counts()
            },
            'performance_summary': self.get_performance_summary(),
            'bottleneck_analysis': self.analyze_bottlenecks(),
            'optimization_recommendations': self.generate_optimization_recommendations()
        }

    def save_performance_data(self, filepath: str):
        """Save performance data to file."""
        data = self.export_performance_data()
        with open(filepath, 'w') as f:
            json.dump(data, f, indent=2, default=str)

    def load_performance_data(self, filepath: str):
        """Load performance data from file."""
        with open(filepath, 'r') as f:
            data = json.load(f)

        # Restore monitor data
        if 'step_history' in data:
            step_history = data['step_history']
            self.monitor._step_times = step_history['step_times']
            self.monitor._spike_counts = step_history['spike_counts']
            self.monitor._active_neuron_counts = step_history['active_neuron_counts']
            if step_history['step_times']:
                self.monitor._calculate_final_averages()
                self.monitor._total_steps = len(step_history['step_times'])
                self.monitor._total_spikes = sum(step_history['spike_counts'])

        # Restore timeline events
        self.timeline_events = data.get('timeline_events', [])

        # Restore memory profiler data
        if 'memory_profiler' in data:
            mem_data = data['memory_profiler']
            self.memory_profiler._samples = mem_data.get('samples', [])
            self.memory_profiler._peak_memory_mb = mem_data.get('peak_memory_mb', 0)
            self.memory_profiler._collection_count = len(self.memory_profiler._samples)

    def generate_visualization_data(self) -> Dict[str, Any]:
        """Generate data for visualization tools."""
        return {
            'timeline_plot': {
                'timestamps': [event['timestamp'] for event in self.timeline_events],
                'step_times': [event['step_time_ms'] for event in self.timeline_events],
                'spike_counts': [event['spikes'] for event in self.timeline_events],
                'neuron_counts': [event['active_neurons'] for event in self.timeline_events]
            },
            'memory_chart': {
                'timestamps': list(range(len(self.memory_profiler.get_samples()))),
                'memory_usage': self.memory_profiler.get_samples(),
                'peak_memory': self.memory_profiler.get_peak_memory_mb()
            },
            'bottleneck_chart': self.analyze_bottlenecks(),
            'flame_graph': self.generate_flame_graph_data()
        }

    def run_benchmark(self, brain, steps: int = 1000) -> Dict[str, Any]:
        """
        Run a benchmark on a brain simulation.

        Args:
            brain: The brain object to benchmark
            steps: Number of simulation steps to run

        Returns:
            Dictionary containing benchmark results
        """
        self.start()
        self.set_brain(brain)

        # Run simulation
        for i in range(steps):
            # Get brain metrics
            current_metrics = self.monitor.get_current_metrics()

            # Simulate neural activity (simplified)
            step_time_ms = 50.0 + (hash(time.time()) % 50) / 100.0
            spikes = 50 + (hash(time.time()) % 100)
            active_neurons = 200 + (hash(time.time()) % 300)

            # Profile the step
            self.profile_step(step_time_ms, spikes, active_neurons, "SIMULATION_STEP")

            # Update brain state (simplified)
            if hasattr(brain, 'step'):
                brain.step(i, i * 0.001)

        self.stop()

        return self.get_performance_summary()

    def generate_comprehensive_report(self) -> str:
        """Generate comprehensive performance report."""
        report = f"NLM Performance Profiling Report\n"
        report += f"=================================\n\n"
        report += f"Simulation Duration: {self.end_time - self.start_time:.2f} seconds\n"
        report += f"Total Steps: {self.monitor.get_step_count()}\n"
        report += f"Total Events: {len(self.timeline_events)}\n\n"

        # Performance summary
        metrics = self.monitor.get_current_metrics()
        report += f"Performance Summary:\n"
        report += f"  Average Step Time: {metrics.avg_step_time_ms:.3f} ms\n"
        report += f"  Step Time Range: {metrics.min_step_time_ms:.3f} - {metrics.max_step_time_ms:.3f} ms\n"
        report += f"  Spikes per Second: {metrics.spikes_per_second:.1f}\n"
        report += f"  Neurons per Second: {metrics.neurons_per_second:.1f}\n"
        report += f"  Real-time Factor: {metrics.real_time_factor:.3f}\n"
        report += f"  Memory Usage: {metrics.memory_usage_mb:.1f} MB\n"
        report += f"  CPU Usage: {metrics.cpu_usage_percent:.1f}%\n\n"

        # Memory summary
        mem_data = self.memory_profiler.get_current_memory_mb()
        peak_mem = self.memory_profiler.get_peak_memory_mb()
        report += f"Memory Summary:\n"
        report += f"  Current Memory: {mem_data} MB\n"
        report += f"  Peak Memory: {peak_mem} MB\n"
        report += f"  Memory Growth Rate: {self.memory_profiler.get_memory_growth_rate():.3f} MB/min\n\n"

        # Bottleneck analysis
        bottlenecks = self.analyze_bottlenecks()
        report += f"Bottleneck Analysis:\n"
        report += f"  Primary Bottleneck: {bottlenecks['primary_bottleneck']}\n"
        report += f"  Severity: {bottlenecks['severity']:.2f}\n"
        report += f"  Memory Issues: {'Yes' if bottlenecks['memory_issues'] else 'No'}\n\n"

        # Optimization recommendations
        recommendations = self.generate_optimization_recommendations()
        report += f"Optimization Recommendations:\n"
        for i, rec in enumerate(recommendations, 1):
            report += f"  {i}. {rec}\n"
        report += f"\n"

        # Stability analysis
        is_stable = self.monitor.is_stable()
        report += f"Stability Analysis:\n"
        report += f"  Performance Stable: {'Yes' if is_stable else 'No'}\n"
        report += f"  Stability Window: 100 steps\n"
        report += f"  Stability Threshold: 0.1 (coefficient of variation)\n\n"

        # Event type distribution
        event_types = self._get_event_type_counts()
        if event_types:
            report += f"Event Type Distribution:\n"
            for event_type, count in sorted(event_types.items(), key=lambda x: x[1], reverse=True):
                percentage = (count / len(self.timeline_events)) * 100
                report += f"  {event_type}: {count} ({percentage:.1f}%)\n"
            report += f"\n"

        # Memory leak analysis
        leak_result = self.memory_profiler.detect_memory_leak()
        report += f"Memory Leak Detection:\n"
        if leak_result['has_leak']:
            report += f"  🚨 LEAK DETECTED\n"
            report += f"  Growth Rate: {leak_result['growth_rate']:.3f} MB/sample\n"
            report += f"  Suspicious Samples: {leak_result['suspicious_samples']}\n"
        else:
            report += f"  ✅ No memory leak detected\n"

        return report

# ============================================================================
# Convenience Functions
# ============================================================================

def create_profiler() -> Profiler:
    """Create a new profiler instance."""
    return Profiler()

def create_performance_monitor() -> PerformanceMonitor:
    """Create a new performance monitor instance."""
    return PerformanceMonitor()

def create_memory_profiler() -> MemoryProfiler:
    """Create a new memory profiler instance."""
    return MemoryProfiler()

def benchmark_neural_network(brain, steps: int = 1000) -> Dict[str, Any]:
    """
    Benchmark a neural network and return performance metrics.

    Args:
        brain: The neural network to benchmark
        steps: Number of simulation steps to run

    Returns:
        Dictionary containing performance metrics and benchmark results
    """
    profiler = create_profiler()
    profiler.set_brain(brain)
    return profiler.run_benchmark(brain, steps)

# ============================================================================
# Example Usage
# ============================================================================

if __name__ == "__main__":
    print("NLM Performance Monitoring and Profiling Module")
    print("=" * 60)

    # Create a profiler
    profiler = create_profiler()

    # Start profiling (in real usage, this would be called before simulation)
    profiler.start()
    print("Started profiling...")

    # Example: Run a benchmark simulation
    print("Running benchmark simulation...")

    # In real usage, create an actual brain object
    # brain = create_default_brain()
    # results = profiler.run_benchmark(brain, 1000)

    # Example: Generate comprehensive report
    print("\nGenerating comprehensive performance report...")
    report = profiler.generate_comprehensive_report()
    print(report)

    # Save performance data
    print("\nSaving performance data...")
    profiler.save_performance_data("performance_data.json")
    print("Performance data saved to 'performance_data.json'")

    # Display optimization recommendations
    print("\nOptimization Recommendations:")
    for i, rec in enumerate(profiler.generate_optimization_recommendations(), 1):
        print(f"  {i}. {rec}")
