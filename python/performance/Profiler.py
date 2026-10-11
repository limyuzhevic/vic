#!/usr/bin/env python3
"""
NLM Performance Profiler
Performance monitoring for NLM simulations
"""

import pynlm
import time
import threading
import psutil
import statistics
from typing import Dict, List, Optional, Callable, Any
from dataclasses import dataclass, field
from enum import Enum
import logging
import json
from pathlib import Path
from collections import deque
import sys

# Configure logging
logging.basicConfig(level=logging.INFO)
logger = logging.getLogger(__name__)

class ProfileMetric:
    """Single performance metric"""
    def __init__(self, name: str, value: float, unit: str = "", timestamp: float = None):
        self.name = name
        self.value = value
        self.unit = unit
        self.timestamp = timestamp or time.time()
    
    def to_dict(self):
        return {
            'name': self.name,
            'value': self.value,
            'unit': self.unit,
            'timestamp': self.timestamp
        }

class PerformanceSnapshot:
    """Performance snapshot at a point in time"""
    def __init__(self, timestamp: float):
        self.timestamp = timestamp
        self.memory_info = psutil.virtual_memory()
        self.cpu_percent = psutil.cpu_percent(interval=None)
        self.disk_io = psutil.disk_io_counters() if hasattr(psutil, 'disk_io_counters') else None
        self.process = psutil.Process()
        self.metrics: Dict[str, ProfileMetric] = {}
    
    def add_metric(self, name: str, value: float, unit: str = ""):
        """Add a performance metric to this snapshot"""
        self.metrics[name] = ProfileMetric(name, value, unit, self.timestamp)
    
    def to_dict(self):
        return {
            'timestamp': self.timestamp,
            'memory_info': {
                'total': self.memory_info.total,
                'available': self.memory_info.available,
                'percent': self.memory_info.percent,
                'used': self.memory_info.used
            },
            'cpu_percent': self.cpu_percent,
            'process_info': {
                'pid': self.process.pid,
                'memory_info': self.process.memory_info()._asdict(),
                'cpu_times': self.process.cpu_times(),
                'num_threads': self.process.num_threads(),
                'num_fds': self.process.num_fds() if hasattr(self.process, 'num_fds') else 0
            },
            'metrics': {name: metric.to_dict() for name, metric in self.metrics.items()}
        }

class PerformanceProfile:
    """Performance profile for an experiment run"""
    def __init__(self, name: str):
        self.name = name
        self.snapshots: List[PerformanceSnapshot] = []
        self.sample_interval = 0.1  # seconds
        self.is_running = False
        self.start_time = None
        self.end_time = None
        self.duration = 0.0
        self.peak_memory_mb = 0.0
        self.avg_cpu_percent = 0.0
        self.step_times: List[float] = []
        self.neuron_counts: List[int] = []
        self.spike_counts: List[int] = []
    
    def start(self):
        """Start performance profiling"""
        self.is_running = True
        self.start_time = time.time()
        self._sampling_thread = threading.Thread(target=self._sampling_loop)
        self._sampling_thread.daemon = True
        self._sampling_thread.start()
        logger.info(f"Started performance profiling: {self.name}")
    
    def stop(self):
        """Stop performance profiling"""
        self.is_running = False
        if hasattr(self, '_sampling_thread'):
            self._sampling_thread.join(timeout=1.0)
        self.end_time = time.time()
        self.duration = self.end_time - self.start_time
        logger.info(f"Stopped performance profiling: {self.name}")
    
    def _sampling_loop(self):
        """Background thread for periodic sampling"""
        while self.is_running:
            snapshot = PerformanceSnapshot(time.time())
            
            # Add system metrics
            snapshot.add_metric('memory_usage_mb', snapshot.memory_info.used / (1024 * 1024), 'MB')
            snapshot.add_metric('cpu_percent', snapshot.cpu_percent, '%')
            
            # Update peak memory
            current_memory_mb = snapshot.memory_info.used / (1024 * 1024)
            if current_memory_mb > self.peak_memory_mb:
                self.peak_memory_mb = current_memory_mb
            
            # Add to snapshots
            self.snapshots.append(snapshot)
            
            time.sleep(self.sample_interval)
    
    def add_step_metrics(self, step: int, brain, neurons: int = 0, spikes: int = 0, step_time: float = 0.0):
        """
        Add metrics for a simulation step.
        
        Args:
            step: Step number
            brain: Brain object (for getting statistics)
            neurons: Number of neurons in brain
            spikes: Number of spikes in this step
            step_time: Time taken for this step (seconds)
        """
        snapshot = PerformanceSnapshot(time.time())
        
        # Add simulation-specific metrics
        snapshot.add_metric('step_number', step, '')
        snapshot.add_metric('neuron_count', neurons, 'neurons')
        snapshot.add_metric('spike_count', spikes, 'spikes')
        snapshot.add_metric('step_time_ms', step_time * 1000, 'ms')
        
        if brain:
            snapshot.add_metric('active_neurons', brain.active_neuron_count(), 'neurons')
            snapshot.add_metric('firing_neurons', brain.firing_neuron_count(), 'neurons')
            snapshot.add_metric('total_spikes', brain.total_spike_count(), 'spikes')
            snapshot.add_metric('average_firing_rate', brain.average_firing_rate(), 'Hz')
        
        self.snapshots.append(snapshot)
        self.step_times.append(step_time)
        self.neuron_counts.append(neurons)
        self.spike_counts.append(spikes)
        
        # Update CPU average
        total_cpu = sum(s.cpu_percent for s in self.snapshots[-10:] if hasattr(s, 'cpu_percent'))
        self.avg_cpu_percent = total_cpu / len(self.snapshots[-10:]) if self.snapshots else 0.0
    
    def get_summary(self) -> Dict[str, Any]:
        """Get performance profile summary"""
        if not self.snapshots:
            return {}
        
        # Calculate statistics
        memory_values = [s.metrics.get('memory_usage_mb', ProfileMetric('', 0)).value 
                        for s in self.snapshots if 'memory_usage_mb' in s.metrics]
        step_time_values = self.step_times
        
        summary = {
            'name': self.name,
            'duration': self.duration,
            'start_time': self.start_time,
            'end_time': self.end_time,
            'sample_count': len(self.snapshots),
            'sample_interval': self.sample_interval,
            'peak_memory_mb': self.peak_memory_mb,
            'avg_cpu_percent': self.avg_cpu_percent,
            'total_steps': len(self.step_times) if self.step_times else 0,
            'avg_step_time_ms': statistics.mean(step_time_values) * 1000 if step_time_values else 0,
            'min_step_time_ms': min(step_time_values) * 1000 if step_time_values else 0,
            'max_step_time_ms': max(step_time_values) * 1000 if step_time_values else 0,
            'stddev_step_time_ms': statistics.stdev(step_time_values) * 1000 if len(step_time_values) > 1 else 0,
            'neuron_growth': self._calculate_neuron_growth() if self.neuron_counts else 0,
            'spike_growth': self._calculate_spike_growth() if self.spike_counts else 0,
        }
        
        return summary
    
    def _calculate_neuron_growth(self) -> float:
        """Calculate neuron count growth rate"""
        if not self.neuron_counts:
            return 0.0
        
        if len(self.neuron_counts) < 2:
            return self.neuron_counts[0] if self.neuron_counts else 0.0
        
        initial = self.neuron_counts[0]
        final = self.neuron_counts[-1]
        return (final - initial) / initial if initial > 0 else 0.0
    
    def _calculate_spike_growth(self) -> float:
        """Calculate spike count growth rate"""
        if not self.spike_counts:
            return 0.0
        
        if len(self.spike_counts) < 2:
            return self.spike_counts[0] if self.spike_counts else 0.0
        
        initial = self.spike_counts[0]
        final = self.spike_counts[-1]
        return (final - initial) / initial if initial > 0 else 0.0
    
    def export_profile(self, filepath: str):
        """
        Export performance profile to JSON file.
        
        Args:
            filepath: Output file path
        """
        profile_data = {
            'name': self.name,
            'start_time': self.start_time,
            'end_time': self.end_time,
            'duration': self.duration,
            'sample_interval': self.sample_interval,
            'peak_memory_mb': self.peak_memory_mb,
            'avg_cpu_percent': self.avg_cpu_percent,
            'snapshots': [snapshot.to_dict() for snapshot in self.snapshots],
            'step_times': self.step_times,
            'neuron_counts': self.neuron_counts,
            'spike_counts': self.spike_counts,
            'summary': self.get_summary()
        }
        
        with open(filepath, 'w') as f:
            json.dump(profile_data, f, indent=2, default=str)
        
        logger.info(f"Exported performance profile to {filepath}")
    
    def import_profile(self, filepath: str):
        """
        Import performance profile from JSON file.
        
        Args:
            filepath: Input file path
        """
        with open(filepath, 'r') as f:
            profile_data = json.load(f)
        
        self.name = profile_data['name']
        self.start_time = profile_data['start_time']
        self.end_time = profile_data['end_time']
        self.duration = profile_data['duration']
        self.sample_interval = profile_data['sample_interval']
        self.peak_memory_mb = profile_data['peak_memory_mb']
        self.avg_cpu_percent = profile_data['avg_cpu_percent']
        self.step_times = profile_data['step_times']
        self.neuron_counts = profile_data['neuron_counts']
        self.spike_counts = profile_data['spike_counts']
        
        # Convert snapshots from dict to objects (simplified)
        self.snapshots = []
        for snapshot_data in profile_data['snapshots']:
            snapshot = PerformanceSnapshot(snapshot_data['timestamp'])
            snapshot.metrics = {}
            for metric_name, metric_data in snapshot_data['metrics'].items():
                snapshot.metrics[metric_name] = ProfileMetric(
                    metric_data['name'], metric_data['value'], metric_data['unit'], metric_data['timestamp']
                )
            self.snapshots.append(snapshot)
        
        logger.info(f"Imported performance profile from {filepath}")

class PerformanceProfiler:
    """
    Performance profiler for NLM simulations.
    
    Provides comprehensive performance monitoring including:
    - System resource monitoring (CPU, memory, disk I/O)
    - Simulation step profiling
    - Performance metrics collection and analysis
    - Performance comparison and benchmarking
    - Automated performance reporting
    """
    
    def __init__(self):
        self.profiles: Dict[str, PerformanceProfile] = {}
        self.is_profiling = False
        self._monitor_thread = None
    
    def start_profiling(self, profile_name: str):
        """
        Start performance profiling for a new profile.
        
        Args:
            profile_name: Name of the performance profile
        """
        if profile_name in self.profiles:
            raise ValueError(f"Profile '{profile_name}' already exists")
        
        self.profiles[profile_name] = PerformanceProfile(profile_name)
        self.profiles[profile_name].start()
        self.is_profiling = True
        
        # Start system monitoring thread
        self._monitor_thread = threading.Thread(target=self._system_monitor_loop)
        self._monitor_thread.daemon = True
        self._monitor_thread.start()
        
        logger.info(f"Started profiling: {profile_name}")
    
    def stop_profiling(self, profile_name: str):
        """
        Stop performance profiling for a profile.
        
        Args:
            profile_name: Name of the performance profile
        """
        if profile_name not in self.profiles:
            raise ValueError(f"Profile '{profile_name}' not found")
        
        self.profiles[profile_name].stop()
        self.is_profiling = len([p for p in self.profiles.values() if p.is_running]) > 0
        
        logger.info(f"Stopped profiling: {profile_name}")
    
    def _system_monitor_loop(self):
        """Background thread for continuous system monitoring"""
        while self.is_profiling:
            time.sleep(1.0)
            # Update system-wide statistics if needed
    
    def add_step_metrics(self, profile_name: str, step: int, brain=None, neurons: int = 0, 
                        spikes: int = 0, step_time: float = 0.0):
        """
        Add metrics for a simulation step.
        
        Args:
            profile_name: Name of the performance profile
            step: Step number
            brain: Brain object for statistics
            neurons: Number of neurons
            spikes: Number of spikes
            step_time: Step execution time
        """
        if profile_name not in self.profiles:
            raise ValueError(f"Profile '{profile_name}' not found")
        
        self.profiles[profile_name].add_step_metrics(step, brain, neurons, spikes, step_time)
    
    def get_profile(self, profile_name: str) -> PerformanceProfile:
        """
        Get a performance profile by name.
        
        Args:
            profile_name: Name of the performance profile
            
        Returns:
            PerformanceProfile object
        """
        if profile_name not in self.profiles:
            raise ValueError(f"Profile '{profile_name}' not found")
        return self.profiles[profile_name]
    
    def get_profile_summary(self, profile_name: str) -> Dict[str, Any]:
        """
        Get summary of a performance profile.
        
        Args:
            profile_name: Name of the performance profile
            
        Returns:
            Dictionary with profile summary
        """
        if profile_name not in self.profiles:
            raise ValueError(f"Profile '{profile_name}' not found")
        return self.profiles[profile_name].get_summary()
    
    def list_profiles(self) -> List[str]:
        """
        List all performance profiles.
        
        Returns:
            List of profile names
        """
        return list(self.profiles.keys())
    
    def export_profile(self, profile_name: str, filepath: str):
        """
        Export a performance profile to file.
        
        Args:
            profile_name: Name of the performance profile
            filepath: Output file path
        """
        if profile_name not in self.profiles:
            raise ValueError(f"Profile '{profile_name}' not found")
        
        self.profiles[profile_name].export_profile(filepath)
    
    def import_profile(self, filepath: str):
        """
        Import a performance profile from file.
        
        Args:
            filepath: Input file path
        """
        with open(filepath, 'r') as f:
            profile_data = json.load(f)
        
        profile_name = profile_data['name']
        if profile_name in self.profiles:
            raise ValueError(f"Profile '{profile_name}' already exists")
        
        self.profiles[profile_name] = PerformanceProfile(profile_name)
        self.profiles[profile_name].import_profile(filepath)
    
    def compare_profiles(self, profile_names: List[str]) -> Dict[str, Any]:
        """
        Compare multiple performance profiles.
        
        Args:
            profile_names: List of profile names to compare
            
        Returns:
            Dictionary with comparison results
        """
        comparison = {
            'profiles': {},
            'metrics': {},
            'rankings': {}
        }
        
        # Get profile summaries
        for profile_name in profile_names:
            if profile_name in self.profiles:
                comparison['profiles'][profile_name] = self.profiles[profile_name].get_summary()
        
        # Compare key metrics
        if comparison['profiles']:
            metric_names = ['duration', 'peak_memory_mb', 'avg_cpu_percent', 
                           'avg_step_time_ms', 'total_steps']
            
            for metric in metric_names:
                comparison['metrics'][metric] = {}
                for profile_name, profile_data in comparison['profiles'].items():
                    comparison['metrics'][metric][profile_name] = profile_data.get(metric, 0)
            
            # Create rankings
            for metric in metric_names:
                if metric in comparison['metrics']:
                    # Sort by value (lower is better for duration and memory/time, higher is better for steps)
                    reverse = metric not in ['duration', 'peak_memory_mb', 'avg_step_time_ms']
                    sorted_items = sorted(comparison['metrics'][metric].items(), 
                                        key=lambda x: x[1], reverse=reverse)
                    comparison['rankings'][metric] = {
                        profile: rank + 1 for rank, (profile, _) in enumerate(sorted_items)
                    }
        
        return comparison
    
    def benchmark_brain(self, brain_config, experiment_name: str, max_steps: int = 1000) -> PerformanceProfile:
        """
        Benchmark a brain configuration.
        
        Args:
            brain_config: Brain configuration
            experiment_name: Name of the experiment
            max_steps: Number of simulation steps
            
        Returns:
            Performance profile
        """
        # Create brain
        config = pynlm.createDefaultConfig()
        if isinstance(brain_config, dict):
            config = pynlm.create_config_from_dict(brain_config)
        
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Start profiling
        self.start_profiling(experiment_name)
        
        try:
            # Run simulation
            for step in range(max_steps):
                brain.step(step)
                
                # Add metrics
                self.add_step_metrics(experiment_name, step, brain)
                
                # Stop profiling if we hit a stopping condition
                if step % 100 == 0:
                    # Log progress
                    logger.info(f"Step {step}/{max_steps}")
            
            # Get profile
            profile = self.get_profile(experiment_name)
            
            # Stop profiling
            self.stop_profiling(experiment_name)
            
            return profile
            
        except Exception as e:
            logger.error(f"Benchmark failed: {e}")
            self.stop_profiling(experiment_name)
            raise
    
    def cleanup(self):
        """Clean up all profiles and resources"""
        for profile_name, profile in self.profiles.items():
            if profile.is_running:
                profile.stop()
        
        self.profiles.clear()
        self.is_profiling = False
        
        logger.info("Performance profiler cleaned up")


if __name__ == "__main__":
    # Example usage of PerformanceProfiler
    profiler = PerformanceProfiler()
    
    # Create default brain config
    brain_config = {
        'brain': {
            'regions': 1,
            'neurons_per_region': 100,
            'neuron_type': 'Internal'
        }
    }
    
    # Benchmark brain
    print("Starting brain benchmark...")
    profile = profiler.benchmark_brain(brain_config, "brain_benchmark", max_steps=500)
    
    # Get summary
    summary = profile.get_summary()
    print(f"Benchmark completed!")
    print(f"Duration: {summary['duration']:.2f}s")
    print(f"Peak memory: {summary['peak_memory_mb']:.2f} MB")
    print(f"Average CPU: {summary['avg_cpu_percent']:.2f}%")
    print(f"Average step time: {summary['avg_step_time_ms']:.2f} ms")
    
    # Clean up
    profiler.cleanup()
