#!/usr/bin/env python3
"""
NLM Performance Monitoring and Debugging Tools

This provides comprehensive performance monitoring, profiling, and debugging capabilities
for the NLM neural simulation framework.
"""

import time
import json
import sys
import os
import threading
from datetime import datetime, timedelta
from typing import Dict, List, Any, Optional, Callable
from enum import Enum
import psutil
import numpy as np
from dataclasses import dataclass, asdict
import logging

class LogLevel(Enum):
    DEBUG = "DEBUG"
    INFO = "INFO"
    WARNING = "WARNING"
    ERROR = "ERROR"
    CRITICAL = "CRITICAL"

@dataclass
class PerformanceMetric:
    """Base class for performance metrics"""
    name: str
    value: float
    timestamp: datetime
    unit: str = ""
    metadata: Dict[str, Any] = None
    
    def __post_init__(self):
        if self.metadata is None:
            self.metadata = {}

@dataclass
class SimulationMetrics:
    """Metrics collected during neural simulation"""
    step: int = 0
    elapsed_time: float = 0.0
    neurons_active: int = 0
    neurons_firing: int = 0
    total_spikes: int = 0
    average_firing_rate: float = 0.0
    e_i_ratio: float = 0.0
    memory_usage_mb: float = 0.0
    cpu_usage_percent: float = 0.0
    simulation_speed_hz: float = 0.0

@dataclass
class MemoryStats:
    """Memory usage statistics"""
    timestamp: datetime
    total_memory_mb: float
    available_memory_mb: float
    used_memory_mb: float
    memory_percent: float
    neural_network_memory_mb: float
    working_memory_mb: float
    episodic_memory_mb: float

@dataclass
class NeuronStats:
    """Individual neuron statistics"""
    neuron_id: int
    neuron_type: str
    membrane_potential: float
    firing_rate: float
    activity_level: float
    last_spike_time: float
    idle_time: float

class PerformanceMonitor:
    """Comprehensive performance monitoring system for NLM"""
    
    def __init__(self, log_level: LogLevel = LogLevel.INFO):
        self.log_level = log_level
        self.metrics_history: List[PerformanceMetric] = []
        self.simulation_metrics: List[SimulationMetrics] = []
        self.memory_stats: List[MemoryStats] = []
        self.neuron_stats: List[NeuronStats] = []
        self.monitoring_active = False
        self.monitoring_thread = None
        self.measurement_interval = 0.1  # seconds
        self.max_history_size = 10000
        self.profiler = Profiler()
        self.timeline = Timeline()
        
        # Setup logging
        self.setup_logging()
    
    def setup_logging(self):
        """Setup logging configuration"""
        log_format = '%(asctime)s - %(name)s - %(levelname)s - %(message)s'
        logging.basicConfig(
            level=self.log_level.value,
            format=log_format,
            handlers=[
                logging.FileHandler('nlm_performance.log'),
                logging.StreamHandler(sys.stdout)
            ]
        )
        self.logger = logging.getLogger('NLM.PerformanceMonitor')
    
    def start_monitoring(self):
        """Start background monitoring thread"""
        if self.monitoring_active:
            return
        
        self.monitoring_active = True
        self.monitoring_thread = threading.Thread(target=self._monitoring_loop, daemon=True)
        self.monitoring_thread.start()
        self.logger.info("Performance monitoring started")
    
    def stop_monitoring(self):
        """Stop background monitoring thread"""
        self.monitoring_active = False
        if self.monitoring_thread:
            self.monitoring_thread.join(timeout=1.0)
        self.logger.info("Performance monitoring stopped")
    
    def _monitoring_loop(self):
        """Main monitoring loop"""
        while self.monitoring_active:
            try:
                self._collect_metrics()
                time.sleep(self.measurement_interval)
            except Exception as e:
                self.logger.error(f"Error in monitoring loop: {e}")
    
    def _collect_metrics(self):
        """Collect current performance metrics"""
        timestamp = datetime.now()
        
        # Collect system metrics
        process = psutil.Process(os.getpid())
        
        # Memory stats
        mem_info = process.memory_info()
        mem_stats = MemoryStats(
            timestamp=timestamp,
            total_memory_mb=mem_info.rss / (1024 * 1024),
            available_memory_mb=psutil.virtual_memory().available / (1024 * 1024),
            used_memory_mb=mem_info.rss / (1024 * 1024),
            memory_percent=process.memory_percent(),
            neural_network_memory_mb=0.0,  # Would need integration
            working_memory_mb=0.0,
            episodic_memory_mb=0.0
        )
        
        # CPU stats
        cpu_percent = process.cpu_percent()
        
        self.memory_stats.append(mem_stats)
        self.metrics_history.append(PerformanceMetric(
            name="cpu_usage",
            value=cpu_percent,
            timestamp=timestamp,
            unit="%"
        ))
        
        # Trim history if too large
        if len(self.metrics_history) > self.max_history_size:
            self.metrics_history = self.metrics_history[-self.max_history_size:]
        
        if len(self.memory_stats) > self.max_history_size:
            self.memory_stats = self.memory_stats[-self.max_history_size:]
    
    def start_profiling(self, name: str, description: str = ""):
        """Start profiling a specific operation"""
        return self.profiler.start_profile(name, description)
    
    def stop_profiling(self, profile_id: str) -> Dict[str, Any]:
        """Stop profiling and return results"""
        return self.profiler.stop_profile(profile_id)
    
    def add_simulation_step_metrics(self, step: int, brain_stats: Dict[str, Any]):
        """Add metrics from a simulation step"""
        timestamp = datetime.now()
        
        sim_metrics = SimulationMetrics(
            step=step,
            elapsed_time=time.time(),
            neurons_active=brain_stats.get('active_neurons', 0),
            neurons_firing=brain_stats.get('firing_neurons', 0),
            total_spikes=brain_stats.get('total_spikes', 0),
            average_firing_rate=brain_stats.get('avg_firing_rate', 0.0),
            e_i_ratio=brain_stats.get('e_i_ratio', 0.0),
            memory_usage_mb=brain_stats.get('memory_usage_mb', 0.0),
            cpu_usage_percent=brain_stats.get('cpu_usage_percent', 0.0),
            simulation_speed_hz=brain_stats.get('sim_speed_hz', 0.0)
        )
        
        self.simulation_metrics.append(sim_metrics)
        
        # Add to timeline
        self.timeline.add_event(
            f"Step {step}",
            timestamp,
            {
                'step': step,
                'neurons_active': sim_metrics.neurons_active,
                'neurons_firing': sim_metrics.neurons_firing,
                'total_spikes': sim_metrics.total_spikes,
                'avg_firing_rate': sim_metrics.average_firing_rate
            }
        )
        
        # Log performance
        self.logger.debug(f"Simulation step {step} completed: "
                         f"{sim_metrics.neurons_firing} firing neurons, "
                         f"{sim_metrics.total_spikes} total spikes")
    
    def add_neuron_stats(self, neuron_id: int, neuron_type: str, 
                        membrane_potential: float, firing_rate: float,
                        activity_level: float, last_spike_time: float,
                        idle_time: float):
        """Add statistics for a single neuron"""
        neuron_stat = NeuronStats(
            neuron_id=neuron_id,
            neuron_type=neuron_type,
            membrane_potential=membrane_potential,
            firing_rate=firing_rate,
            activity_level=activity_level,
            last_spike_time=last_spike_time,
            idle_time=idle_time
        )
        
        self.neuron_stats.append(neuron_stat)
        
        # Log if activity is high
        if activity_level > 0.8:
            self.logger.warning(f"High activity neuron {neuron_id}: {activity_level:.2f}")
    
    def get_performance_report(self) -> Dict[str, Any]:
        """Generate comprehensive performance report"""
        report = {
            'summary': {
                'total_steps': len(self.simulation_metrics),
                'total_monitoring_samples': len(self.metrics_history),
                'monitoring_active': self.monitoring_active,
                'peak_cpu_usage': max(m.value for m in self.metrics_history if m.name == 'cpu_usage') if self.metrics_history else 0,
                'average_cpu_usage': sum(m.value for m in self.metrics_history if m.name == 'cpu_usage') / len([m for m in self.metrics_history if m.name == 'cpu_usage']) if self.metrics_history else 0,
            },
            'simulation': {
                'steps_completed': len(self.simulation_metrics),
                'average_neurons_active': np.mean([m.neurons_active for m in self.simulation_metrics]) if self.simulation_metrics else 0,
                'average_neurons_firing': np.mean([m.neurons_firing for m in self.simulation_metrics]) if self.simulation_metrics else 0,
                'total_spikes': sum(m.total_spikes for m in self.simulation_metrics),
                'average_firing_rate': np.mean([m.average_firing_rate for m in self.simulation_metrics]) if self.simulation_metrics else 0,
                'average_e_i_ratio': np.mean([m.e_i_ratio for m in self.simulation_metrics]) if self.simulation_metrics else 0,
            },
            'memory': {
                'peak_memory_usage_mb': max(m.used_memory_mb for m in self.memory_stats) if self.memory_stats else 0,
                'average_memory_usage_mb': sum(m.used_memory_mb for m in self.memory_stats) / len(self.memory_stats) if self.memory_stats else 0,
                'memory_trend': 'increasing' if len(self.memory_stats) > 10 and 
                                     self.memory_stats[-1].used_memory_mb > self.memory_stats[0].used_memory_mb else 'stable'
            },
            'neuron_analysis': {
                'total_neurons_monitored': len(self.neuron_stats),
                'high_activity_neurons': len([n for n in self.neuron_stats if n.activity_level > 0.8]),
                'average_neuron_activity': np.mean([n.activity_level for n in self.neuron_stats]) if self.neuron_stats else 0,
            },
            'profiling': self.profiler.get_summary(),
            'timeline': self.timeline.get_summary(),
            'recent_events': self._get_recent_events()
        }
        
        return report
    
    def export_metrics(self, filepath: str):
        """Export metrics to JSON file"""
        data = {
            'metrics_history': [asdict(m) for m in self.metrics_history],
            'simulation_metrics': [asdict(m) for m in self.simulation_metrics],
            'memory_stats': [asdict(m) for m in self.memory_stats],
            'neuron_stats': [asdict(m) for m in self.neuron_stats],
            'profiling': self.profiler.export_data(),
            'timeline': self.timeline.export_data(),
            'export_time': datetime.now().isoformat()
        }
        
        with open(filepath, 'w') as f:
            json.dump(data, f, indent=2, default=str)
        
        self.logger.info(f"Metrics exported to {filepath}")
    
    def import_metrics(self, filepath: str):
        """Import metrics from JSON file"""
        with open(filepath, 'r') as f:
            data = json.load(f)
        
        self.metrics_history = [PerformanceMetric(**m) for m in data['metrics_history']]
        self.simulation_metrics = [SimulationMetrics(**m) for m in data['simulation_metrics']]
        self.memory_stats = [MemoryStats(**m) for m in data['memory_stats']]
        self.neuron_stats = [NeuronStats(**m) for m in data['neuron_stats']]
        self.profiler.import_data(data['profiling'])
        self.timeline.import_data(data['timeline'])
        
        self.logger.info(f"Metrics imported from {filepath}")
    
    def generate_performance_alerts(self) -> List[Dict[str, Any]]:
        """Generate performance alerts based on thresholds"""
        alerts = []
        
        if self.simulation_metrics:
            # High CPU usage alert
            avg_cpu = sum(m.cpu_usage_percent for m in self.simulation_metrics) / len(self.simulation_metrics)
            if avg_cpu > 80:
                alerts.append({
                    'type': 'performance',
                    'severity': 'warning',
                    'message': f'High CPU usage detected: {avg_cpu:.1f}%',
                    'timestamp': datetime.now().isoformat()
                })
            
            # Memory usage alert
            avg_memory = sum(m.memory_usage_mb for m in self.simulation_metrics) / len(self.simulation_metrics)
            if avg_memory > 512:  # 512 MB threshold
                alerts.append({
                    'type': 'memory',
                    'severity': 'warning',
                    'message': f'High memory usage detected: {avg_memory:.1f} MB',
                    'timestamp': datetime.now().isoformat()
                })
            
            # Low firing rate alert
            avg_firing_rate = sum(m.average_firing_rate for m in self.simulation_metrics) / len(self.simulation_metrics)
            if avg_firing_rate < 0.01:
                alerts.append({
                    'type': 'performance',
                    'severity': 'info',
                    'message': f'Low firing rate detected: {avg_firing_rate:.4f} Hz',
                    'timestamp': datetime.now().isoformat()
                })
        
        return alerts
    
    def _get_recent_events(self, limit: int = 10) -> List[Dict[str, Any]]:
        """Get recent events from timeline"""
        return self.timeline.get_recent_events(limit)

class Profiler:
    """Function and operation profiler"""
    
    def __init__(self):
        self.profiles: Dict[str, Dict[str, Any]] = {}
        self.active_profiles: Dict[str, Dict[str, Any]] = {}
    
    def start_profile(self, name: str, description: str = "") -> str:
        """Start profiling an operation"""
        profile_id = f"{name}_{datetime.now().timestamp()}"
        
        self.active_profiles[profile_id] = {
            'name': name,
            'description': description,
            'start_time': time.time(),
            'start_timestamp': datetime.now()
        }
        
        return profile_id
    
    def stop_profile(self, profile_id: str) -> Dict[str, Any]:
        """Stop profiling and return results"""
        if profile_id not in self.active_profiles:
            raise ValueError(f"Profile {profile_id} not found or already stopped")
        
        profile_data = self.active_profiles.pop(profile_id)
        end_time = time.time()
        
        duration = end_time - profile_data['start_time']
        
        result = {
            'name': profile_data['name'],
            'description': profile_data['description'],
            'start_time': profile_data['start_time'],
            'end_time': end_time,
            'duration_seconds': duration,
            'start_timestamp': profile_data['start_timestamp'],
            'end_timestamp': datetime.now()
        }
        
        self.profiles[profile_id] = result
        return result
    
    def get_summary(self) -> Dict[str, Any]:
        """Get profiling summary"""
        if not self.profiles:
            return {}
        
        total_time = sum(p['duration_seconds'] for p in self.profiles.values())
        
        return {
            'total_profiles': len(self.profiles),
            'total_time_seconds': total_time,
            'average_duration': total_time / len(self.profiles),
            'profiles': self.profiles
        }
    
    def export_data(self) -> Dict[str, Any]:
        """Export profiling data"""
        return {
            'profiles': self.profiles,
            'active_profiles': self.active_profiles
        }
    
    def import_data(self, data: Dict[str, Any]):
        """Import profiling data"""
        self.profiles = data.get('profiles', {})
        self.active_profiles = data.get('active_profiles', {})

class Timeline:
    """Timeline and event tracking system"""
    
    def __init__(self):
        self.events: List[Dict[str, Any]] = []
        self.markers: List[Dict[str, Any]] = []
    
    def add_event(self, name: str, timestamp: datetime, data: Dict[str, Any] = None):
        """Add an event to the timeline"""
        event = {
            'name': name,
            'timestamp': timestamp,
            'data': data or {}
        }
        
        self.events.append(event)
    
    def add_marker(self, name: str, timestamp: datetime, description: str = ""):
        """Add a timeline marker"""
        marker = {
            'name': name,
            'timestamp': timestamp,
            'description': description
        }
        
        self.markers.append(marker)
    
    def get_events(self, start_time: datetime = None, end_time: datetime = None) -> List[Dict[str, Any]]:
        """Get events within time range"""
        if not start_time and not end_time:
            return self.events
        
        filtered = []
        for event in self.events:
            if start_time and event['timestamp'] < start_time:
                continue
            if end_time and event['timestamp'] > end_time:
                continue
            filtered.append(event)
        
        return filtered
    
    def get_recent_events(self, limit: int = 10) -> List[Dict[str, Any]]:
        """Get recent events"""
        return self.events[-limit:]
    
    def get_summary(self) -> Dict[str, Any]:
        """Get timeline summary"""
        if not self.events:
            return {}
        
        timestamps = [e['timestamp'] for e in self.events]
        
        return {
            'total_events': len(self.events),
            'total_markers': len(self.markers),
            'first_event': self.events[0]['timestamp'],
            'last_event': self.events[-1]['timestamp'],
            'time_span_seconds': (self.events[-1]['timestamp'] - self.events[0]['timestamp']).total_seconds()
        }
    
    def export_data(self) -> Dict[str, Any]:
        """Export timeline data"""
        return {
            'events': self.events,
            'markers': self.markers
        }
    
    def import_data(self, data: Dict[str, Any]):
        """Import timeline data"""
        self.events = data.get('events', [])
        self.markers = data.get('markers', [])

class NLMDebugger:
    """Main debugger interface for NLM"""
    
    def __init__(self):
        self.performance_monitor = PerformanceMonitor()
        self.breakpoints: List[int] = []
        self.watch_variables: Dict[str, Any] = {}
        self.log_level = logging.INFO
        
        # Setup logging
        logging.basicConfig(
            level=self.log_level,
            format='%(asctime)s - %(name)s - %(levelname)s - %(message)s'
        )
        self.logger = logging.getLogger('NLM.Debugger')
    
    def start_debug_session(self):
        """Start debugging session"""
        self.performance_monitor.start_monitoring()
        self.logger.info("Debug session started")
    
    def stop_debug_session(self):
        """Stop debugging session"""
        self.performance_monitor.stop_monitoring()
        self.logger.info("Debug session stopped")
    
    def add_breakpoint(self, step: int):
        """Add a breakpoint at simulation step"""
        self.breakpoints.append(step)
        self.logger.debug(f"Breakpoint added at step {step}")
    
    def remove_breakpoint(self, step: int):
        """Remove a breakpoint"""
        if step in self.breakpoints:
            self.breakpoints.remove(step)
            self.logger.debug(f"Breakpoint removed at step {step}")
    
    def add_watch_variable(self, name: str, value: Any):
        """Add a watch variable"""
        self.watch_variables[name] = value
        self.logger.debug(f"Watch variable added: {name} = {value}")
    
    def get_performance_report(self) -> Dict[str, Any]:
        """Get performance report"""
        return self.performance_monitor.get_performance_report()
    
    def export_data(self, filepath: str):
        """Export debugging data"""
        data = {
            'performance_report': self.get_performance_report(),
            'breakpoints': self.breakpoints,
            'watch_variables': self.watch_variables,
            'export_time': datetime.now().isoformat()
        }
        
        with open(filepath, 'w') as f:
            json.dump(data, f, indent=2, default=str)
        
        self.logger.info(f"Debug data exported to {filepath}")
    
    def import_data(self, filepath: str):
        """Import debugging data"""
        with open(filepath, 'r') as f:
            data = json.load(f)
        
        self.breakpoints = data.get('breakpoints', [])
        self.watch_variables = data.get('watch_variables', {})
        
        self.logger.info(f"Debug data imported from {filepath}")

if __name__ == "__main__":
    # Command line interface for debugging tools
    import argparse
    
    parser = argparse.ArgumentParser(description="NLM Performance Monitoring and Debugging Tools")
    parser.add_argument('--log-level', choices=['DEBUG', 'INFO', 'WARNING', 'ERROR', 'CRITICAL'],
                       default='INFO', help='Logging level')
    parser.add_argument('--monitor', action='store_true', help='Start performance monitoring')
    parser.add_argument('--export', help='Export data to file')
    parser.add_argument('--import', dest='import_file', help='Import data from file')
    parser.add_argument('--report', action='store_true', help='Generate performance report')
    parser.add_argument('--alerts', action='store_true', help='Generate performance alerts')
    
    args = parser.parse_args()
    
    # Setup log level
    log_level_map = {
        'DEBUG': logging.DEBUG,
        'INFO': logging.INFO,
        'WARNING': logging.WARNING,
        'ERROR': logging.ERROR,
        'CRITICAL': logging.CRITICAL
    }
    
    # Create debugger
    debugger = NLMDebugger()
    debugger.log_level = log_level_map[args.log_level]
    
    if args.monitor:
        debugger.start_debug_session()
    
    if args.import_file:
        debugger.import_data(args.import_file)
    
    if args.report:
        report = debugger.get_performance_report()
        print("Performance Report:")
        print(json.dumps(report, indent=2, default=str))
    
    if args.alerts:
        alerts = debugger.performance_monitor.generate_performance_alerts()
        print("Performance Alerts:")
        for alert in alerts:
            print(f"{alert['severity'].upper()}: {alert['message']} ({alert['timestamp']})")
    
    if args.export:
        debugger.export_data(args.export)
    
    if not any([args.monitor, args.import_file, args.report, args.alerts, args.export]):
        parser.print_help()
    
    debugger.stop_debug_session()