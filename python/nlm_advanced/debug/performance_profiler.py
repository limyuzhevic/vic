"""
PerformanceProfiler class with advanced profiling.

Provides comprehensive performance profiling and monitoring for neural simulations,
including resource usage tracking, simulation progress monitoring, and performance
analysis tools.
"""

import numpy as np
import psutil
import time
import threading
from typing import Dict, List, Optional, Any, Callable
from dataclasses import dataclass, field
from collections import defaultdict, deque

@dataclass
class PerformanceMetrics:
    """Performance metrics snapshot."""
    timestamp: float
    step: int
    cpu_usage: float
    memory_usage_mb: float
    disk_io: float
    network_io: float
    simulation_fps: float
    step_time: float
    custom_metrics: Dict[str, Any] = field(default_factory=dict)
    neural_metrics: Dict[str, Any] = field(default_factory=dict)

@dataclass
class PerformanceProfile:
    """Performance profile for analysis."""
    profile_id: str
    start_time: float
    end_time: Optional[float] = None
    duration: Optional[float] = None
    metrics: List[PerformanceMetrics] = field(default_factory=list)
    custom_metrics: Dict[str, List[float]] = field(default_factory=dict)
    performance_bottlenecks: List[str] = field(default_factory=list)
    optimization_suggestions: List[str] = field(default_factory=list)

class PerformanceProfiler:
    """
    Advanced performance profiling for neural simulations.
    
    Provides comprehensive performance monitoring, profiling, and analysis
    capabilities for neural simulations. Supports resource monitoring,
    performance benchmarking, and optimization recommendations.
    """
    
    def __init__(self, brain):
        self.brain = brain
        self.performance_profiles = {}
        self.current_profile = None
        self.monitoring_active = False
        self.monitor_thread = None
        self.sample_interval = 0.1
        self.performance_history = deque(maxlen=10000)
        self.resource_monitor = ResourceMonitor()
        self.performance_analyzer = PerformanceAnalyzer()
        self.optimization_engine = OptimizationEngine()
        self.bottleneck_detector = BottleneckDetector()
        
        # Performance metrics collection
        self.step_times = deque(maxlen=1000)
        self.fps_history = deque(maxlen=1000)
        self.memory_history = deque(maxlen=1000)
        self.cpu_history = deque(maxlen=1000)
        
        print(f"[PROFILER] Performance profiler initialized for brain with {brain.getTotalNeuronCount() if hasattr(brain, 'getTotalNeuronCount') else 'unknown'} neurons")
    
    def start_profile(self, profile_id: str, interval: float = 0.1) -> str:
        """
        Start performance profiling for a simulation.
        
        Args:
            profile_id: Unique identifier for the performance profile
            interval: Sampling interval in seconds
            
        Returns:
            Profile ID
        """
        if profile_id in self.performance_profiles:
            raise ValueError(f"Profile with ID {profile_id} already exists")
        
        self.current_profile = PerformanceProfile(
            profile_id=profile_id,
            start_time=time.time(),
            duration=None,
            metrics=[]
        )
        
        self.performance_profiles[profile_id] = self.current_profile
        self.sample_interval = interval
        
        # Start resource monitoring
        self.resource_monitor.start_monitoring(interval)
        
        # Start performance monitoring thread
        self.monitoring_active = True
        self.monitor_thread = threading.Thread(target=self._monitoring_loop, daemon=True)
        self.monitor_thread.start()
        
        print(f"[PROFILER] Performance profiling started for {profile_id} with interval {interval}s")
        return profile_id
    
    def stop_profile(self, profile_id: str) -> PerformanceProfile:
        """
        Stop performance profiling for a simulation.
        
        Args:
            profile_id: Profile ID to stop
            
        Returns:
            PerformanceProfile object
        """
        if profile_id not in self.performance_profiles:
            raise ValueError(f"Profile with ID {profile_id} not found")
        
        profile = self.performance_profiles[profile_id]
        profile.end_time = time.time()
        profile.duration = profile.end_time - profile.start_time
        
        # Stop monitoring
        self.monitoring_active = False
        if self.monitor_thread and self.monitor_thread.is_alive():
            self.monitor_thread.join(timeout=5.0)
            self.monitor_thread = None
        
        self.resource_monitor.stop_monitoring()
        
        # Perform performance analysis
        profile = self.performance_analyzer.analyze_profile(profile)
        
        # Detect performance bottlenecks
        profile.performance_bottlenecks = self.bottleneck_detector.detect_bottlenecks(profile)
        
        # Generate optimization suggestions
        profile.optimization_suggestions = self.optimization_engine.generate_suggestions(profile)
        
        self.current_profile = None
        
        print(f"[PROFILER] Performance profiling stopped for {profile_id}")
        return profile
    
    def step_completed(self, step: int, time: float):
        """
        Record completion of a simulation step.
        
        Args:
            step: Step number
            time: Simulation time
        """
        step_start_time = time.time()
        
        # Collect performance metrics
        metrics = self._collect_performance_metrics(step, time)
        
        # Store in performance history
        self.performance_history.append(metrics)
        
        # Store in current profile if active
        if self.current_profile is not None:
            self.current_profile.metrics.append(metrics)
        
        # Store step timing
        step_time = time.time() - step_start_time
        self.step_times.append(step_time)
        
        # Calculate and store FPS
        if step_start_time > self.fps_history[0].timestamp if self.fps_history else True:
            fps = 1.0 / max(step_time, 0.001)
            self.fps_history.append({
                'timestamp': step_start_time,
                'fps': fps
            })
        
        # Store memory usage
        memory_info = psutil.Process().memory_info()
        self.memory_history.append({
            'timestamp': step_start_time,
            'memory_mb': memory_info.rss / 1024 / 1024
        })
        
        # Store CPU usage
        self.cpu_history.append({
            'timestamp': step_start_time,
            'cpu_percent': psutil.cpu_percent()
        })
    
    def add_custom_metric(self, name: str, func: Callable):
        """
        Add a custom metric to collect.
        
        Args:
            name: Metric name
            func: Function that returns the metric value
        """
        if self.current_profile is not None:
            if not hasattr(self.current_profile, 'custom_metrics'):
                self.current_profile.custom_metrics = {}
            self.current_profile.custom_metrics[name] = func
    
    def get_performance_statistics(self, profile_id: str) -> Dict[str, Any]:
        """
        Get performance statistics for a profile.
        
        Args:
            profile_id: Profile ID
            
        Returns:
            Performance statistics dictionary
        """
        if profile_id not in self.performance_profiles:
            raise ValueError(f"Profile with ID {profile_id} not found")
        
        profile = self.performance_profiles[profile_id]
        
        stats = {
            'profile_id': profile_id,
            'duration': profile.duration,
            'total_steps': len(profile.metrics),
            'sampling_rate': len(profile.metrics) / profile.duration if profile.duration and profile.duration > 0 else 0,
            'average_step_time': np.mean([m.step_time for m in profile.metrics]) if profile.metrics else 0,
            'min_step_time': min([m.step_time for m in profile.metrics]) if profile.metrics else 0,
            'max_step_time': max([m.step_time for m in profile.metrics]) if profile.metrics else 0,
            'step_time_std': np.std([m.step_time for m in profile.metrics]) if profile.metrics else 0,
            'average_fps': np.mean([m.simulation_fps for m in profile.metrics]) if profile.metrics else 0,
            'min_fps': min([m.simulation_fps for m in profile.metrics]) if profile.metrics else 0,
            'max_fps': max([m.simulation_fps for m in profile.metrics]) if profile.metrics else 0,
            'average_cpu': np.mean([m.cpu_usage for m in profile.metrics]) if profile.metrics else 0,
            'average_memory': np.mean([m.memory_usage_mb for m in profile.metrics]) if profile.metrics else 0,
            'peak_memory': max([m.memory_usage_mb for m in profile.metrics]) if profile.metrics else 0,
            'bottlenecks_count': len(profile.performance_bottlenecks),
            'optimization_suggestions_count': len(profile.optimization_suggestions)
        }
        
        return stats
    
    def compare_profiles(self, profile_ids: List[str]) -> Dict[str, Any]:
        """
        Compare multiple performance profiles.
        
        Args:
            profile_ids: List of profile IDs to compare
            
        Returns:
            Comparison results
        """
        if len(profile_ids) < 2:
            return {'error': 'At least two profiles required for comparison'}
        
        profiles = [self.performance_profiles[pid] for pid in profile_ids if pid in self.performance_profiles]
        
        if len(profiles) < 2:
            return {'error': 'At least two valid profiles required for comparison'}
        
        comparison = {
            'profiles': {},
            'performance_rankings': {},
            'recommendations': []
        }
        
        # Analyze each profile
        for profile in profiles:
            stats = self.get_performance_statistics(profile.profile_id)
            comparison['profiles'][profile.profile_id] = stats
        
        # Rank profiles by performance
        performance_scores = {}
        for profile_id, stats in comparison['profiles'].items():
            # Calculate performance score (higher is better)
            score = (
                stats['average_fps'] * 0.4 +
                (1.0 / max(stats['average_step_time'], 0.001)) * 0.3 +
                (1.0 / max(stats['peak_memory'], 1.0)) * 0.2 +
                stats['average_cpu'] * 0.1
            )
            performance_scores[profile_id] = score
        
        comparison['performance_rankings'] = dict(sorted(performance_scores.items(), key=lambda x: x[1], reverse=True))
        
        # Generate recommendations
        best_profile = list(comparison['performance_rankings'].keys())[0]
        comparison['recommendations'].append(f"Profile {best_profile} shows the best overall performance")
        
        worst_profile = list(comparison['performance_rankings'].keys())[-1]
        comparison['recommendations'].append(f"Profile {worst_profile} may need optimization")
        
        return comparison
    
    def export_profile(self, profile_id: str, filepath: str):
        """
        Export performance profile to file.
        
        Args:
            profile_id: Profile ID to export
            filepath: Output file path
        """
        if profile_id not in self.performance_profiles:
            raise ValueError(f"Profile with ID {profile_id} not found")
        
        profile = self.performance_profiles[profile_id]
        
        # Convert to serializable format
        export_data = {
            'profile_id': profile.profile_id,
            'start_time': profile.start_time,
            'end_time': profile.end_time,
            'duration': profile.duration,
            'metrics': [
                {
                    'timestamp': m.timestamp,
                    'step': m.step,
                    'cpu_usage': m.cpu_usage,
                    'memory_usage_mb': m.memory_usage_mb,
                    'disk_io': m.disk_io,
                    'network_io': m.network_io,
                    'simulation_fps': m.simulation_fps,
                    'step_time': m.step_time,
                    'custom_metrics': m.custom_metrics,
                    'neural_metrics': m.neural_metrics
                }
                for m in profile.metrics
            ],
            'performance_bottlenecks': profile.performance_bottlenecks,
            'optimization_suggestions': profile.optimization_suggestions
        }
        
        import json
        with open(filepath, 'w') as f:
            json.dump(export_data, f, indent=2, default=str)
        
        print(f"[PROFILER] Performance profile exported to {filepath}")
    
    def import_profile(self, filepath: str) -> str:
        """
        Import performance profile from file.
        
        Args:
            filepath: Input file path
            
        Returns:
            Imported profile ID
        """
        import json
        with open(filepath, 'r') as f:
            import_data = json.load(f)
        
        profile_id = import_data['profile_id']
        
        # Create profile
        profile = PerformanceProfile(
            profile_id=profile_id,
            start_time=import_data['start_time'],
            end_time=import_data.get('end_time'),
            duration=import_data.get('duration'),
            metrics=[]
        )
        
        # Import metrics
        for metric_data in import_data['metrics']:
            metric = PerformanceMetrics(
                timestamp=metric_data['timestamp'],
                step=metric_data['step'],
                cpu_usage=metric_data['cpu_usage'],
                memory_usage_mb=metric_data['memory_usage_mb'],
                disk_io=metric_data['disk_io'],
                network_io=metric_data['network_io'],
                simulation_fps=metric_data['simulation_fps'],
                step_time=metric_data['step_time'],
                custom_metrics=metric_data.get('custom_metrics', {}),
                neural_metrics=metric_data.get('neural_metrics', {})
            )
            profile.metrics.append(metric)
        
        # Import additional data
        profile.performance_bottlenecks = import_data.get('performance_bottlenecks', [])
        profile.optimization_suggestions = import_data.get('optimization_suggestions', [])
        
        # Store profile
        self.performance_profiles[profile_id] = profile
        
        print(f"[PROFILER] Performance profile imported from {filepath}, ID: {profile_id}")
        return profile_id
    
    def _collect_performance_metrics(self, step: int, time: float) -> PerformanceMetrics:
        """Collect performance metrics for current step."""
        # Get neural metrics if available
        neural_metrics = {}
        if hasattr(self.brain, 'getRegions'):
            regions = self.brain.getRegions()
            neural_metrics['regions'] = {
                region_id.value: {
                    'active_neurons': region.getActiveNeuronCount(),
                    'total_neurons': region.getTotalNeuronCount(),
                    'firing_rate': region.getActiveNeuronCount() / max(region.getTotalNeuronCount(), 1)
                }
                for region_id, region in regions.items()
            }
        
        # Get brain-wide metrics
        if hasattr(self.brain, 'getTotalSpikeCount'):
            neural_metrics['total_spikes'] = self.brain.getTotalSpikeCount()
        if hasattr(self.brain, 'getActiveNeuronCount'):
            neural_metrics['active_neurons'] = self.brain.getActiveNeuronCount()
        if hasattr(self.brain, 'getAverageFiringRate'):
            neural_metrics['firing_rate'] = self.brain.getAverageFiringRate()
        
        return PerformanceMetrics(
            timestamp=time.time(),
            step=step,
            cpu_usage=psutil.cpu_percent(),
            memory_usage_mb=psutil.Process().memory_info().rss / 1024 / 1024,
            disk_io=0.0,  # Would need disk I/O monitoring
            network_io=0.0,  # Would need network I/O monitoring
            simulation_fps=1.0 / max(time.time() - (self.step_times[-1] if self.step_times else 0), 0.001),
            step_time=0.0,  # This will be updated in step_completed
            neural_metrics=neural_metrics
        )
    
    def _monitoring_loop(self):
        """Main monitoring loop."""
        while self.monitoring_active:
            time.sleep(self.sample_interval)
            
            # Update current profile metrics
            if self.current_profile is not None:
                metrics = self._collect_performance_metrics(
                    len(self.current_profile.metrics) + 1,
                    time.time()
                )
                metrics.step_time = self.step_times[-1] if self.step_times else 0.001
                self.current_profile.metrics.append(metrics)

class ResourceMonitor:
    """Resource usage monitoring for performance profiling."""
    
    def __init__(self):
        self.monitoring = False
        self.monitor_thread = None
        self.sample_interval = 0.1
        self.process = psutil.Process()
        self.samples = []
    
    def start_monitoring(self, interval: float = 0.1):
        """Start resource monitoring."""
        self.monitoring = True
        self.sample_interval = interval
        self.monitor_thread = threading.Thread(target=self._monitor_loop, daemon=True)
        self.monitor_thread.start()
    
    def stop_monitoring(self):
        """Stop resource monitoring."""
        self.monitoring = False
        if self.monitor_thread:
            self.monitor_thread.join(timeout=5.0)
            self.monitor_thread = None
    
    def _monitor_loop(self):
        """Main monitoring loop."""
        while self.monitoring:
            self._collect_sample()
            time.sleep(self.sample_interval)
    
    def _collect_sample(self):
        """Collect current resource metrics."""
        timestamp = time.time()
        
        # CPU usage
        cpu_percent = psutil.cpu_percent(interval=0.01)
        
        # Memory usage
        memory_info = self.process.memory_info()
        memory_mb = memory_info.rss / 1024 / 1024
        
        # Disk I/O
        current_disk_io = self.process.disk_io_counters()
        disk_read = current_disk_io.read_bytes if current_disk_io else 0
        disk_write = current_disk_io.write_bytes if current_disk_io else 0
        
        # Network I/O
        current_net_io = self.process.net_io_counters()
        net_sent = current_net_io.bytes_sent if current_net_io else 0
        net_recv = current_net_io.bytes_recv if current_net_io else 0
        
        sample = {
            'timestamp': timestamp,
            'cpu_percent': cpu_percent,
            'memory_mb': memory_mb,
            'disk_read_bytes': disk_read,
            'disk_write_bytes': disk_write,
            'net_sent_bytes': net_sent,
            'net_recv_bytes': net_recv
        }
        
        self.samples.append(sample)
        
        # Keep samples manageable
        if len(self.samples) > 10000:
            self.samples = self.samples[-10000:]

class PerformanceAnalyzer:
    """Performance analysis for performance profiling."""
    
    def analyze_profile(self, profile: PerformanceProfile) -> PerformanceProfile:
        """Analyze performance profile and add analysis data."""
        if not profile.metrics:
            return profile
        
        # Add computed metrics to profile metrics
        for i, metric in enumerate(profile.metrics):
            metric.cpu_usage = self._calculate_cpu_usage(i, profile)
            metric.memory_usage_mb = self._calculate_memory_usage(i, profile)
            metric.simulation_fps = self._calculate_fps(i, profile)
        
        return profile
    
    def _calculate_cpu_usage(self, index: int, profile: PerformanceProfile) -> float:
        """Calculate CPU usage for a specific step."""
        if not profile.metrics:
            return 0.0
        
        return 0.0  # Simplified - would use actual resource monitoring data
    
    def _calculate_memory_usage(self, index: int, profile: PerformanceProfile) -> float:
        """Calculate memory usage for a specific step."""
        if not profile.metrics:
            return 0.0
        
        return 0.0  # Simplified - would use actual memory monitoring data
    
    def _calculate_fps(self, index: int, profile: PerformanceProfile) -> float:
        """Calculate FPS for a specific step."""
        if index == 0 or len(profile.metrics) < 2:
            return 1.0
        
        time_diff = profile.metrics[index].timestamp - profile.metrics[index-1].timestamp
        return 1.0 / max(time_diff, 0.001)

class BottleneckDetector:
    """Performance bottleneck detection."""
    
    def detect_bottlenecks(self, profile: PerformanceProfile) -> List[str]:
        """Detect performance bottlenecks in profile."""
        bottlenecks = []
        
        if not profile.metrics:
            return bottlenecks
        
        # Detect high step time bottlenecks
        avg_step_time = np.mean([m.step_time for m in profile.metrics])
        for i, metric in enumerate(profile.metrics):
            if metric.step_time > avg_step_time * 3:  # Step time more than 3x average
                bottlenecks.append(f"Step {metric.step} has high step time: {metric.step_time:.4f}s")
        
        # Detect memory bottlenecks
        avg_memory = np.mean([m.memory_usage_mb for m in profile.metrics])
        for i, metric in enumerate(profile.metrics):
            if metric.memory_usage_mb > avg_memory * 2:  # Memory usage more than 2x average
                bottlenecks.append(f"Step {metric.step} has high memory usage: {metric.memory_usage_mb:.2f}MB")
        
        # Detect CPU bottlenecks
        avg_cpu = np.mean([m.cpu_usage for m in profile.metrics])
        for i, metric in enumerate(profile.metrics):
            if metric.cpu_usage > avg_cpu * 0.8:  # CPU usage high
                bottlenecks.append(f"Step {metric.step} has high CPU usage: {metric.cpu_usage:.1f}%")
        
        return bottlenecks

class OptimizationEngine:
    """Performance optimization suggestions."""
    
    def generate_suggestions(self, profile: PerformanceProfile) -> List[str]:
        """Generate optimization suggestions based on performance profile."""
        suggestions = []
        
        if not profile.metrics:
            return suggestions
        
        # Analyze step times
        avg_step_time = np.mean([m.step_time for m in profile.metrics])
        if avg_step_time > 0.1:  # Step time greater than 100ms
            suggestions.append("Consider reducing simulation step size or optimizing step computation")
        
        # Analyze memory usage
        avg_memory = np.mean([m.memory_usage_mb for m in profile.metrics])
        peak_memory = max([m.memory_usage_mb for m in profile.metrics])
        if peak_memory > avg_memory * 3:
            suggestions.append("Memory usage varies significantly - consider memory management optimizations")
        
        # Analyze CPU usage
        avg_cpu = np.mean([m.cpu_usage for m in profile.metrics])
        if avg_cpu > 80:
            suggestions.append("High CPU usage detected - consider optimizing computation or using parallel processing")
        
        # Analyze consistency
        step_time_std = np.std([m.step_time for m in profile.metrics])
        if step_time_std / avg_step_time > 0.5:  # High variation
            suggestions.append("High variability in step times - consider stabilizing simulation parameters")
        
        if not suggestions:
            suggestions.append("Performance is within acceptable ranges - no major optimizations needed")
        
        return suggestions

# Register performance profiler with pynlm integration
from .. import pynlm
pynlm.register_performance_profiler(PerformanceProfiler)