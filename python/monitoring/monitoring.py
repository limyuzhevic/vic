"""NLM Monitoring Module

This module provides monitoring and analysis tools for the NLM framework.
"""

import time
import psutil
from typing import Dict, Any, Optional
import logging

logger = logging.getLogger(__name__)

class PerformanceMonitor:
    """Performance monitoring and statistics collection.
    
    Tracks and analyzes performance metrics for NLM operations.
    """
    
    def __init__(self):
        """Initialize performance monitor."""
        self.metrics = {}
        self.start_time = time.time()
        
    def record_metric(self, name: str, value: float, unit: str = "ms"):
        """Record a performance metric.
        
        Args:
            name: Metric name
            value: Metric value
            unit: Unit of measurement
        """
        if name not in self.metrics:
            self.metrics[name] = {
                'values': [],
                'unit': unit,
                'count': 0,
                'min': float('inf'),
                'max': float('-inf'),
                'sum': 0.0,
                'sum_squared': 0.0
            }
            
        metric = self.metrics[name]
        metric['values'].append(value)
        metric['count'] += 1
        metric['min'] = min(metric['min'], value)
        metric['max'] = max(metric['max'], value)
        metric['sum'] += value
        metric['sum_squared'] += value * value
        
    def get_metric(self, name: str):
        """Get metric statistics.
        
        Args:
            name: Metric name
            
        Returns:
            Dictionary with metric statistics
        """
        if name not in self.metrics:
            return None
            
        metric = self.metrics[name]
        count = metric['count']
        
        if count == 0:
            return None
            
        mean = metric['sum'] / count
        variance = (metric['sum_squared'] / count) - (mean * mean)
        stddev = variance ** 0.5 if variance > 0 else 0.0
        
        return {
            'name': name,
            'unit': metric['unit'],
            'count': count,
            'min': metric['min'],
            'max': metric['max'],
            'mean': mean,
            'stddev': stddev,
            'latest': metric['values'][-1]
        }
        
    def get_all_metrics(self):
        """Get all recorded metrics.
        
        Returns:
            Dictionary with all metrics
        """
        return {name: self.get_metric(name) for name in self.metrics}
        
    def get_metric_names(self):
        """Get names of all recorded metrics.
        
        Returns:
            List of metric names
        """
        return list(self.metrics.keys())
        
    def clear(self):
        """Clear all metrics."""
        self.metrics.clear()
        
    def export_metrics(self):
        """Export metrics to dictionary.
        
        Returns:
            Dictionary with all metrics
        """
        return self.get_all_metrics()
        
    def import_metrics(self, metrics):
        """Import metrics from dictionary.
        
        Args:
            metrics: Dictionary with metrics
        """
        self.metrics.clear()
        for name, metric in metrics.items():
            if 'values' in metric:
                self.metrics[name] = metric
        
    def get_uptime(self):
        """Get monitor uptime.
        
        Returns:
            Uptime in seconds
        """
        return time.time() - self.start_time

class Profiler:
    """CPU profiler for performance analysis.
    
    Profiles execution time of functions and operations.
    """
    
    def __init__(self):
        """Initialize profiler."""
        self.profiles = {}
        
    def start(self, name: str):
        """Start profiling a function.
        
        Args:
            name: Function name
        """
        if name not in self.profiles:
            self.profiles[name] = {
                'start_time': None,
                'end_time': None,
                'times': [],
                'count': 0
            }
            
        profile = self.profiles[name]
        profile['start_time'] = time.time()
        profile['count'] += 1
        
    def stop(self, name: str):
        """Stop profiling a function.
        
        Args:
            name: Function name
            
        Returns:
            Profile data for the function
        """
        if name not in self.profiles:
            return None
            
        profile = self.profiles[name]
        if profile['start_time'] is None:
            return None
            
        end_time = time.time()
        duration = end_time - profile['start_time']
        
        profile['end_time'] = end_time
        profile['times'].append(duration)
        
        return self.get_profile_data(name)
        
    def get_profile_data(self, name: str):
        """Get profile data for a function.
        
        Args:
            name: Function name
            
        Returns:
            Profile data dictionary
        """
        if name not in self.profiles:
            return None
            
        profile = self.profiles[name]
        times = profile['times']
        
        if not times:
            return None
            
        count = len(times)
        total_time = sum(times)
        avg_time = total_time / count
        min_time = min(times)
        max_time = max(times)
        
        return {
            'name': name,
            'count': count,
            'total_time': total_time,
            'average_time': avg_time,
            'min_time': min_time,
            'max_time': max_time,
            'times': times
        }
        
    def get_all_profiles(self):
        """Get all profile data.
        
        Returns:
            Dictionary with all profiles
        """
        return {name: self.get_profile_data(name) for name in self.profiles}
        
    def clear(self):
        """Clear all profile data."""
        self.profiles.clear()
        
    def get_total_time(self):
        """Get total profiling time.
        
        Returns:
            Total time in seconds
        """
        total = 0.0
        for profile in self.profiles.values():
            total += sum(profile['times'])
        return total
        
    def get_average_time(self):
        """Get average profiling time per function.
        
        Returns:
            Average time in seconds
        """
        total_time = self.get_total_time()
        total_calls = sum(profile['count'] for profile in self.profiles.values())
        
        if total_calls == 0:
            return 0.0
            
        return total_time / total_calls
        
    def get_call_count(self):
        """Get total number of profiled calls.
        
        Returns:
            Total call count
        """
        return sum(profile['count'] for profile in self.profiles.values())
        
    def export_profile(self):
        """Export profile data to dictionary.
        
        Returns:
            Dictionary with all profile data
        """
        return self.get_all_profiles()
        
    def import_profile(self, profile_data):
        """Import profile data from dictionary.
        
        Args:
            profile_data: Dictionary with profile data
        """
        self.profiles.clear()
        for name, profile in profile_data.items():
            if 'times' in profile:
                self.profiles[name] = {
                    'start_time': None,
                    'end_time': None,
                    'times': profile['times'],
                    'count': profile['count']
                }

class SimulationLogger:
    """Logger for simulation events and errors."""
    
    def __init__(self, level=logging.INFO):
        """Initialize simulation logger.
        
        Args:
            level: Logging level
        """
        self.logger = logging.getLogger('nlm.simulation')
        self.logger.setLevel(level)
        
        if not self.logger.handlers:
            handler = logging.StreamHandler()
            formatter = logging.Formatter(
                '%(asctime)s - %(name)s - %(levelname)s - %(message)s'
            )
            handler.setFormatter(formatter)
            self.logger.addHandler(handler)
            
        self.log_file = None
        self.metrics_log = []
        
    def log_step(self, step, stats):
        """Log simulation step.
        
        Args:
            step: Step number
            stats: Simulation statistics
        """
        message = f"Step {step}: "
        if stats:
            message += f"neurons={stats.get('total_neurons', 'N/A')}, "
            message += f"spikes={stats.get('total_spikes', 'N/A')}, "
            message += f"time={stats.get('simulation_time', 'N/A'):.2f}s"
        else:
            message += "completed"
            
        self.logger.info(message)
        self.metrics_log.append({
            'timestamp': time.time(),
            'step': step,
            'stats': stats
        })
        
    def log_error(self, step, error, stats):
        """Log simulation error.
        
        Args:
            step: Step number
            error: Error object
            stats: Simulation statistics
        """
        self.logger.error(f"Step {step}: {error}")
        
    def log_simulation_start(self, config):
        """Log simulation start.
        
        Args:
            config: Configuration
        """
        self.logger.info(f"Starting simulation with config: {config}")
        
    def log_simulation_end(self, stats):
        """Log simulation end.
        
        Args:
            stats: Final statistics
        """
        duration = stats.get('simulation_time', 0.0) if stats else 0.0
        self.logger.info(f"Simulation completed: {stats.get('steps_completed', 0)} steps "
                        f"in {duration:.2f}s")
        
    def export_metrics(self):
        """Export logged metrics.
        
        Returns:
            List of metric dictionaries
        """
        return self.metrics_log.copy()
        
    def import_metrics(self, metrics):
        """Import logged metrics.
        
        Args:
            metrics: List of metric dictionaries
        """
        self.metrics_log = metrics.copy()
        
    def save_to_file(self, filepath):
        """Save logged metrics to file.
        
        Args:
            filepath: File path
        """
        import json
        with open(filepath, 'w') as f:
            json.dump(self.metrics_log, f, indent=2)
            
    def load_from_file(self, filepath):
        """Load logged metrics from file.
        
        Args:
            filepath: File path
        """
        import json
        with open(filepath, 'r') as f:
            self.metrics_log = json.load(f)

# Global instances for convenience
performance_monitor = PerformanceMonitor()
profiler = Profiler()
simulation_logger = SimulationLogger()