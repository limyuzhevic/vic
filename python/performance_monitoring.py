#!/usr/bin/env python3
"""
Performance Monitoring Tools for NLM (Neural Learning Machine) 

This module provides comprehensive performance monitoring, profiling, 
benchmarking, and analysis capabilities for NLM simulations.
"""

import time
import threading
import psutil
import numpy as np
from datetime import datetime, timedelta
from typing import Dict, List, Any, Optional, Callable, Union
from dataclasses import dataclass, asdict
import json
import csv
import sys
from collections import deque
import warnings
warnings.filterwarnings('ignore')

@dataclass
class PerformanceMetric:
    """Data structure for performance metrics."""
    timestamp: float
    name: str
    value: float
    unit: str
    metadata: Dict[str, Any] = None
    
    def __post_init__(self):
        if self.metadata is None:
            self.metadata = {}

@dataclass
class PerformanceSnapshot:
    """Complete performance snapshot at a point in time."""
    timestamp: float
    system_metrics: Dict[str, Any]
    neural_metrics: Dict[str, Any]
    simulation_metrics: Dict[str, Any]
    resource_metrics: Dict[str, Any]

class SystemMonitor:
    """Real-time system resource monitoring."""
    
    def __init__(self):
        self.metrics_history = deque(maxlen=1000)
        self.alert_thresholds = {}
        self.monitoring = False
        self.monitor_thread = None
        
    def get_system_metrics(self) -> Dict[str, Any]:
        """Collect current system metrics."""
        process = psutil.Process()
        cpu_percent = process.cpu_percent()
        memory_info = process.memory_info()
        disk_io = process.io_counters()
        
        system_metrics = {
            'timestamp': time.time(),
            'cpu_usage': cpu_percent,
            'memory_usage_mb': memory_info.rss / (1024 * 1024),
            'memory_percent': process.memory_percent(),
            'memory_available_mb': psutil.virtual_memory().available / (1024 * 1024),
            'disk_read_mb': disk_io.read_bytes / (1024 * 1024),
            'disk_write_mb': disk_io.write_bytes / (1024 * 1024),
            'threads_count': process.num_threads(),
            'open_files': len(process.open_files()),
            'connections_count': len(process.connections()),
            'system_load': psutil.getloadavg()[0] if hasattr(psutil, 'getloadavg') else 0.0,
            'cpu_count': psutil.cpu_count(),
            'cpu_freq': psutil.cpu_freq().current if psutil.cpu_freq() else 0.0
        }
        
        # Add to history
        self.metrics_history.append({
            'type': 'system',
            'data': system_metrics
        })
        
        return system_metrics
    
    def start_monitoring(self, interval: float = 1.0):
        """Start continuous system monitoring."""
        self.monitoring = True
        self.monitor_thread = threading.Thread(
            target=self._monitor_loop,
            args=(interval,),
            daemon=True
        )
        self.monitor_thread.start()
        
    def _monitor_loop(self, interval: float):
        """Background monitoring loop."""
        while self.monitoring:
            self.get_system_metrics()
            time.sleep(interval)
    
    def stop_monitoring(self):
        """Stop system monitoring."""
        self.monitoring = False
        if self.monitor_thread:
            self.monitor_thread.join(timeout=5.0)

class NeuralMonitor:
    """Monitor neural activity patterns and performance."""
    
    def __init__(self):
        self.metrics_history = deque(maxlen=1000)
        self.activity_patterns = {}
        self.firing_rates = deque(maxlen=100)
        self.spike_times = deque(maxlen=10000)
        self.synaptic_efficiency = 0.0
        self.plasticity_measure = 0.0
        
    def analyze_firing_rate(self, neuron_counts: Dict[str, Any]) -> Dict[str, Any]:
        """Analyze firing rate patterns."""
        total_neurons = neuron_counts.get('total', 0)
        active_neurons = neuron_counts.get('active', 0)
        firing_neurons = neuron_counts.get('firing', 0)
        
        firing_rate = firing_neurons / max(total_neurons, 1)
        activity_pattern = {
            'timestamp': time.time(),
            'total_neurons': total_neurons,
            'active_neurons': active_neurons,
            'firing_neurons': firing_neurons,
            'firing_rate': firing_rate,
            'activity_level': min(active_neurons / max(total_neurons, 1), 1.0)
        }
        
        self.firing_rates.append(firing_rate)
        self.metrics_history.append({
            'type': 'neural_firing',
            'data': activity_pattern
        })
        
        return activity_pattern
    
    def analyze_synaptic_efficiency(self, total_synapses: int, active_synapses: int, 
                                   weight_changes: List[float]) -> Dict[str, Any]:
        """Measure synaptic efficiency."""
        efficiency = active_synapses / max(total_synapses, 1)
        avg_weight_change = np.mean(weight_changes) if weight_changes else 0.0
        stability = 1.0 - min(abs(avg_weight_change), 1.0)
        
        synaptic_metrics = {
            'timestamp': time.time(),
            'total_synapses': total_synapses,
            'active_synapses': active_synapses,
            'efficiency_ratio': efficiency,
            'average_weight_change': avg_weight_change,
            'stability': stability
        }
        
        self.synaptic_efficiency = efficiency
        self.metrics_history.append({
            'type': 'synaptic',
            'data': synaptic_metrics
        })
        
        return synaptic_metrics
    
    def analyze_spike_timing(self, spike_times: List[float]) -> Dict[str, Any]:
        """Analyze spike timing precision."""
        if len(spike_times) < 2:
            return {'coherence': 0.0, 'regularity': 0.0}
        
        intervals = np.diff(spike_times)
        mean_interval = np.mean(intervals)
        std_interval = np.std(intervals)
        
        coherence = max(0, 1.0 - std_interval / mean_interval)
        cv_coefficient = std_interval / mean_interval if mean_interval > 0 else 0.0
        
        timing_metrics = {
            'timestamp': time.time(),
            'total_spikes': len(spike_times),
            'mean_interval': mean_interval,
            'std_interval': std_interval,
            'coherence': coherence,
            'cv_coefficient': cv_coefficient
        }
        
        self.spike_times.extend(spike_times)
        self.metrics_history.append({
            'type': 'spike_timing',
            'data': timing_metrics
        })
        
        return timing_metrics
    
    def analyze_plasticity(self, plasticity_history: List[float]) -> Dict[str, Any]:
        """Monitor plasticity effectiveness."""
        if not plasticity_history:
            return {'rate': 0.0, 'direction': 'stable', 'effectiveness': 0.0}
        
        current_plasticity = plasticity_history[-1]
        avg_plasticity = np.mean(plasticity_history[-100:])
        
        if current_plasticity > avg_plasticity * 1.1:
            direction = 'increasing'
        elif current_plasticity < avg_plasticity * 0.9:
            direction = 'decreasing'
        else:
            direction = 'stable'
        
        # Calculate effectiveness (positive change in relevant measures)
        effectiveness = min(abs(current_plasticity - avg_plasticity) / max(avg_plasticity, 0.001), 1.0)
        
        plasticity_metrics = {
            'timestamp': time.time(),
            'current_plasticity': current_plasticity,
            'average_plasticity': avg_plasticity,
            'direction': direction,
            'effectiveness': effectiveness
        }
        
        self.plasticity_measure = current_plasticity
        self.metrics_history.append({
            'type': 'plasticity',
            'data': plasticity_metrics
        })
        
        return plasticity_metrics

class SimulationMonitor:
    """Monitor simulation performance metrics."""
    
    def __init__(self):
        self.metrics_history = deque(maxlen=1000)
        self.step_times = deque(maxlen=1000)
        self.frame_times = deque(maxlen=1000)
        self.throughput_samples = deque(maxlen=100)
        self.latency_samples = deque(maxlen=100)
        self.total_steps = 0
        
    def measure_step_timing(self, step_time: float) -> Dict[str, Any]:
        """Measure step execution time."""
        self.step_times.append(step_time)
        
        if len(self.step_times) > 0:
            avg_step_time = np.mean(self.step_times)
            std_step_time = np.std(self.step_times)
            min_step_time = np.min(self.step_times)
            max_step_time = np.max(self.step_times)
            
            step_metrics = {
                'timestamp': time.time(),
                'step_time_ms': step_time * 1000,
                'average_step_time_ms': avg_step_time * 1000,
                'std_step_time_ms': std_step_time * 1000,
                'min_step_time_ms': min_step_time * 1000,
                'max_step_time_ms': max_step_time * 1000,
                'steps_per_second': 1.0 / max(step_time, 0.0001)
            }
            
            self.metrics_history.append({
                'type': 'step_timing',
                'data': step_metrics
            })
            
            return step_metrics
        
        return {'timestamp': time.time(), 'step_time_ms': step_time * 1000}
    
    def measure_frame_rate(self, frame_time: float) -> Dict[str, Any]:
        """Measure frame rate for visualization."""
        self.frame_times.append(frame_time)
        
        if len(self.frame_times) > 0:
            fps = 1.0 / frame_time if frame_time > 0 else 0.0
            avg_fps = 1.0 / np.mean(self.frame_times) if len(self.frame_times) > 1 else fps
            
            frame_metrics = {
                'timestamp': time.time(),
                'frame_time_ms': frame_time * 1000,
                'fps': fps,
                'average_fps': avg_fps,
                'frame_time_target_ms': 16.67,  # 60 FPS target
                'frame_time_variance': np.std(self.frame_times) * 1000 if len(self.frame_times) > 1 else 0.0
            }
            
            self.metrics_history.append({
                'type': 'frame_rate',
                'data': frame_metrics
            })
            
            return frame_metrics
        
        return {'timestamp': time.time(), 'frame_time_ms': frame_time * 1000, 'fps': 1.0 / max(frame_time, 0.0001)}
    
    def measure_simulation_speed(self, steps_completed: int, wall_time: float) -> Dict[str, Any]:
        """Measure simulation speed and throughput."""
        self.total_steps += steps_completed
        
        if wall_time > 0:
            steps_per_second = steps_completed / wall_time
            avg_throughput = self.total_steps / time.time() if time.time() > 0 else steps_per_second
            
            speed_metrics = {
                'timestamp': time.time(),
                'steps_completed': steps_completed,
                'wall_time_s': wall_time,
                'steps_per_second': steps_per_second,
                'average_throughput': avg_throughput,
                'time_per_step_ms': wall_time * 1000 / steps_completed if steps_completed > 0 else 0.0
            }
            
            self.throughput_samples.append(steps_per_second)
            self.metrics_history.append({
                'type': 'simulation_speed',
                'data': speed_metrics
            })
            
            return speed_metrics
        
        return {'timestamp': time.time(), 'steps_per_second': 0.0}
    
    def measure_latency(self, latency: float) -> Dict[str, Any]:
        """Measure simulation latency."""
        self.latency_samples.append(latency)
        
        if len(self.latency_samples) > 0:
            avg_latency = np.mean(self.latency_samples)
            max_latency = np.max(self.latency_samples)
            p95_latency = np.percentile(self.latency_samples, 95)
            
            latency_metrics = {
                'timestamp': time.time(),
                'current_latency_ms': latency * 1000,
                'average_latency_ms': avg_latency * 1000,
                'max_latency_ms': max_latency * 1000,
                'p95_latency_ms': p95_latency * 1000,
                'latency_target_ms': 1.0,  # 1ms target
                'latency_variance': np.var(self.latency_samples) * 1000000 if len(self.latency_samples) > 1 else 0.0
            }
            
            self.metrics_history.append({
                'type': 'latency',
                'data': latency_metrics
            })
            
            return latency_metrics
        
        return {'timestamp': time.time(), 'current_latency_ms': latency * 1000}

class AdvancedAnalyzer:
    """Advanced performance analysis tools."""
    
    def __init__(self):
        self.trend_data = {}
        self.bottleneck_detected = False
        self.optimization_suggestions = []
        
    def analyze_bottlenecks(self, system_metrics: Dict[str, Any], neural_metrics: Dict[str, Any],
                           simulation_metrics: Dict[str, Any]) -> Dict[str, Any]:
        """Identify performance bottlenecks."""
        bottlenecks = []
        
        # Check CPU bottlenecks
        cpu_usage = system_metrics.get('cpu_usage', 0)
        if cpu_usage > 80:
            bottlenecks.append({
                'type': 'cpu',
                'severity': 'high' if cpu_usage > 90 else 'medium',
                'description': f'High CPU usage: {cpu_usage:.1f}%'
            })
        
        # Check memory bottlenecks
        memory_usage = system_metrics.get('memory_percent', 0)
        if memory_usage > 80:
            bottlenecks.append({
                'type': 'memory',
                'severity': 'high' if memory_usage > 90 else 'medium',
                'description': f'High memory usage: {memory_usage:.1f}%'
            })
        
        # Check neural network bottlenecks
        firing_rate = neural_metrics.get('firing_rate', 0)
        if firing_rate > 0.5:
            bottlenecks.append({
                'type': 'neural',
                'severity': 'medium',
                'description': f'High firing rate: {firing_rate:.3f} - potential neural overload'
            })
        
        # Check simulation bottlenecks
        step_time = simulation_metrics.get('average_step_time_ms', 0)
        if step_time > 50:
            bottlenecks.append({
                'type': 'simulation',
                'severity': 'high' if step_time > 100 else 'medium',
                'description': f'Slow simulation step: {step_time:.1f}ms'
            })
        
        analysis = {
            'timestamp': time.time(),
            'bottlenecks': bottlenecks,
            'total_bottlenecks': len(bottlenecks),
            'critical_bottlenecks': len([b for b in bottlenecks if b['severity'] == 'high'])
        }
        
        self.bottleneck_detected = len(bottlenecks) > 0
        return analysis
    
    def analyze_resource_allocation(self, system_metrics: Dict[str, Any]) -> Dict[str, Any]:
        """Analyze resource allocation efficiency."""
        cpu_efficiency = 100.0 - system_metrics.get('cpu_usage', 0)
        memory_efficiency = 100.0 - system_metrics.get('memory_percent', 0)
        
        # Calculate combined efficiency score
        efficiency_score = (cpu_efficiency + memory_efficiency) / 2.0
        
        allocation_analysis = {
            'timestamp': time.time(),
            'cpu_efficiency': cpu_efficiency,
            'memory_efficiency': memory_efficiency,
            'overall_efficiency': efficiency_score,
            'resource_status': 'optimal' if efficiency_score > 70 else 'suboptimal',
            'optimization_opportunity': max(0, 100 - efficiency_score)
        }
        
        return allocation_analysis
    
    def analyze_scalability(self, system_metrics_history: List[Dict[str, Any]]) -> Dict[str, Any]:
        """Analyze scalability trends."""
        if len(system_metrics_history) < 10:
            return {'scalability_score': 0.0, 'trend': 'insufficient_data'}
        
        # Extract key metrics
        cpu_usages = [m.get('cpu_usage', 0) for m in system_metrics_history]
        memory_usages = [m.get('memory_percent', 0) for m in system_metrics_history]
        step_times = [m.get('average_step_time_ms', 0) for m in system_metrics_history]
        
        # Calculate trends
        def calculate_trend(values):
            if len(values) < 2:
                return 0.0
            x = np.arange(len(values))
            y = np.array(values)
            slope = np.polyfit(x, y, 1)[0]
            return slope
        
        cpu_trend = calculate_trend(cpu_usages)
        memory_trend = calculate_trend(memory_usages)
        step_time_trend = calculate_trend(step_times)
        
        # Scalability score (negative trends in resource usage and step time = good scalability)
        scalability_score = max(0, 100 - (abs(cpu_trend) + abs(memory_trend) + abs(step_time_trend)))
        
        scalability_analysis = {
            'timestamp': time.time(),
            'cpu_trend': cpu_trend,
            'memory_trend': memory_trend,
            'step_time_trend': step_time_trend,
            'scalability_score': scalability_score,
            'trend_analysis': 'improving' if scalability_score > 70 else 'stable' if scalability_score > 40 else 'degrading'
        }
        
        return scalability_analysis
    
    def calculate_efficiency_ratios(self, neural_metrics: Dict[str, Any], 
                                   simulation_metrics: Dict[str, Any]) -> Dict[str, Any]:
        """Calculate efficiency ratios."""
        # Neural efficiency
        total_neurons = neural_metrics.get('total_neurons', 1)
        active_neurons = neural_metrics.get('active_neurons', 0)
        firing_neurons = neural_metrics.get('firing_neurons', 0)
        
        neural_efficiency = firing_neurons / max(total_neurons, 1)
        
        # Simulation efficiency
        steps_per_second = simulation_metrics.get('steps_per_second', 0)
        time_per_step = simulation_metrics.get('time_per_step_ms', 0)
        
        # Combined efficiency score
        efficiency_score = (neural_efficiency * 100 + steps_per_second + (1.0 / max(time_per_step, 0.001))) / 3.0
        
        efficiency_ratios = {
            'timestamp': time.time(),
            'neural_efficiency': neural_efficiency,
            'simulation_throughput': steps_per_second,
            'simulation_efficiency': 1.0 / max(time_per_step, 0.001),
            'overall_efficiency_score': efficiency_score,
            'efficiency_grade': 'A' if efficiency_score > 80 else 'B' if efficiency_score > 60 else 'C' if efficiency_score > 40 else 'D'
        }
        
        return efficiency_ratios
    
    def generate_performance_indicators(self, metrics: Dict[str, Any]) -> Dict[str, Any]:
        """Generate custom performance indicators."""
        indicators = {}
        
        # CPU indicator
        cpu_usage = metrics.get('cpu_usage', 0)
        indicators['cpu_utilization'] = {
            'value': cpu_usage,
            'status': 'good' if cpu_usage < 50 else 'warning' if cpu_usage < 80 else 'critical',
            'target': '<=80%',
            'trend': 'stable'
        }
        
        # Memory indicator
        memory_usage = metrics.get('memory_percent', 0)
        indicators['memory_utilization'] = {
            'value': memory_usage,
            'status': 'good' if memory_usage < 50 else 'warning' if memory_usage < 80 else 'critical',
            'target': '<=80%',
            'trend': 'stable'
        }
        
        # Performance indicator
        fps = metrics.get('fps', 0)
        indicators['performance_fps'] = {
            'value': fps,
            'status': 'good' if fps > 55 else 'warning' if fps > 30 else 'critical',
            'target': '>=60 FPS',
            'trend': 'stable'
        }
        
        # Neural health indicator
        firing_rate = metrics.get('firing_rate', 0)
        indicators['neural_health'] = {
            'value': firing_rate,
            'status': 'good' if 0.1 < firing_rate < 0.3 else 'warning' if 0.05 < firing_rate < 0.5 else 'critical',
            'target': '0.1-0.3 firing rate',
            'trend': 'stable'
        }
        
        custom_indicators = {
            'timestamp': time.time(),
            'indicators': indicators,
            'overall_status': 'good',
            'alerts_count': sum(1 for ind in indicators.values() if ind['status'] == 'critical')
        }
        
        return custom_indicators

class PerformanceMonitor:
    """Comprehensive performance monitoring and analysis system."""
    
    def __init__(self):
        self.system_monitor = SystemMonitor()
        self.neural_monitor = NeuralMonitor()
        self.simulation_monitor = SimulationMonitor()
        self.advanced_analyzer = AdvancedAnalyzer()
        
        self.metrics_store = {
            'system': deque(maxlen=10000),
            'neural': deque(maxlen=10000),
            'simulation': deque(maxlen=10000)
        }
        
        self.performance_alerts = []
        self.thresholds = {
            'cpu': 90.0,
            'memory': 90.0,
            'fps': 30.0,
            'step_time_ms': 100.0,
            'firing_rate': 0.5
        }
        
        self.is_running = False
        self.monitor_thread = None
        self.collection_interval = 1.0
        
    def start_monitoring(self, interval: float = 1.0):
        """Start comprehensive performance monitoring."""
        self.collection_interval = interval
        self.is_running = True
        
        self.monitor_thread = threading.Thread(
            target=self._monitoring_loop,
            daemon=True
        )
        self.monitor_thread.start()
        
        # Start individual monitors
        self.system_monitor.start_monitoring(interval)
        
        return True
    
    def stop_monitoring(self):
        """Stop performance monitoring."""
        self.is_running = False
        self.system_monitor.stop_monitoring()
        
        if self.monitor_thread:
            self.monitor_thread.join(timeout=5.0)
    
    def _monitoring_loop(self):
        """Main monitoring loop collecting all metrics."""
        while self.is_running:
            try:
                # Collect system metrics
                system_metrics = self.system_monitor.get_system_metrics()
                self.metrics_store['system'].append({
                    'timestamp': time.time(),
                    'metrics': system_metrics
                })
                
                # Check for alerts
                self._check_alerts(system_metrics)
                
                # Simulate neural and simulation metrics collection
                # In a real implementation, these would be collected from actual NLM systems
                neural_metrics = self._collect_neural_metrics()
                simulation_metrics = self._collect_simulation_metrics()
                
                self.metrics_store['neural'].append({
                    'timestamp': time.time(),
                    'metrics': neural_metrics
                })
                
                self.metrics_store['simulation'].append({
                    'timestamp': time.time(),
                    'metrics': simulation_metrics
                })
                
                time.sleep(self.collection_interval)
                
            except Exception as e:
                print(f"Monitoring loop error: {e}")
                time.sleep(self.collection_interval)
    
    def _collect_neural_metrics(self) -> Dict[str, Any]:
        """Collect neural performance metrics from NLM systems."""
        # In a real implementation, this would query actual NLM brain components
        # For demonstration, return simulated metrics
        return {
            'total_neurons': 500,
            'active_neurons': 125,
            'firing_neurons': 75,
            'firing_rate': 0.15,
            'average_firing_rate': 0.12,
            'total_spikes': 1250,
            'excitatory_inhibition_ratio': 1.8,
            'developmental_stage': 'adult',
            'plasticity_rate': 0.05,
            'synaptic_density': 0.3
        }
    
    def _collect_simulation_metrics(self) -> Dict[str, Any]:
        """Collect simulation performance metrics."""
        return {
            'steps_completed': 1000,
            'wall_time_s': 45.0,
            'steps_per_second': 22.2,
            'current_step_time_ms': 45.0,
            'average_step_time_ms': 45.0,
            'min_step_time_ms': 30.0,
            'max_step_time_ms': 80.0,
            'frame_time_ms': 16.7,
            'fps': 59.9,
            'current_latency_ms': 2.5,
            'average_latency_ms': 3.2,
            'max_latency_ms': 8.1,
            'p95_latency_ms': 5.8
        }
    
    def _check_alerts(self, system_metrics: Dict[str, Any]):
        """Check system metrics against thresholds and generate alerts."""
        timestamp = time.time()
        
        # Check CPU threshold
        cpu_usage = system_metrics.get('cpu_usage', 0)
        if cpu_usage >= self.thresholds['cpu']:
            self.performance_alerts.append({
                'timestamp': timestamp,
                'type': 'cpu',
                'severity': 'high' if cpu_usage >= 95 else 'medium',
                'value': cpu_usage,
                'threshold': self.thresholds['cpu'],
                'message': f'High CPU usage: {cpu_usage:.1f}%'
            })
        
        # Check memory threshold
        memory_usage = system_metrics.get('memory_percent', 0)
        if memory_usage >= self.thresholds['memory']:
            self.performance_alerts.append({
                'timestamp': timestamp,
                'type': 'memory',
                'severity': 'high' if memory_usage >= 95 else 'medium',
                'value': memory_usage,
                'threshold': self.thresholds['memory'],
                'message': f'High memory usage: {memory_usage:.1f}%'
            })
    
    def collect_snapshot(self) -> PerformanceSnapshot:
        """Collect a complete performance snapshot."""
        system_metrics = self.system_monitor.get_system_metrics()
        neural_metrics = self._collect_neural_metrics()
        simulation_metrics = self._collect_simulation_metrics()
        resource_metrics = self._collect_resource_metrics()
        
        return PerformanceSnapshot(
            timestamp=time.time(),
            system_metrics=system_metrics,
            neural_metrics=neural_metrics,
            simulation_metrics=simulation_metrics,
            resource_metrics=resource_metrics
        )
    
    def _collect_resource_metrics(self) -> Dict[str, Any]:
        """Collect additional resource metrics."""
        process = psutil.Process()
        disk = psutil.disk_usage('/')
        
        return {
            'disk_usage_percent': disk.percent,
            'disk_free_gb': disk.free / (1024**3),
            'disk_total_gb': disk.total / (1024**3),
            'network_io_bytes': process.net_io_counters().bytes_sent + process.net_io_counters().bytes_recv,
            'process_cpu_time': process.cpu_times(),
            'process_status': process.status(),
            'nice_priority': process.nice()
        }
    
    def analyze_bottlenecks(self) -> Dict[str, Any]:
        """Analyze performance bottlenecks."""
        # Get latest metrics
        system_data = list(self.metrics_store['system'])[-1:] if self.metrics_store['system'] else []
        neural_data = list(self.metrics_store['neural'])[-1:] if self.metrics_store['neural'] else []
        simulation_data = list(self.metrics_store['simulation'])[-1:] if self.metrics_store['simulation'] else []
        
        system_metrics = system_data[0]['metrics'] if system_data else {}
        neural_metrics = neural_data[0]['metrics'] if neural_data else {}
        simulation_metrics = simulation_data[0]['metrics'] if simulation_data else {}
        
        return self.advanced_analyzer.analyze_bottlenecks(system_metrics, neural_metrics, simulation_metrics)
    
    def analyze_resource_allocation(self) -> Dict[str, Any]:
        """Analyze resource allocation efficiency."""
        system_data = list(self.metrics_store['system'])[-1:] if self.metrics_store['system'] else []
        system_metrics = system_data[0]['metrics'] if system_data else {}
        
        return self.advanced_analyzer.analyze_resource_allocation(system_metrics)
    
    def analyze_scalability(self) -> Dict[str, Any]:
        """Analyze scalability trends."""
        system_data = list(self.metrics_store['system'])
        return self.advanced_analyzer.analyze_scalability(system_data)
    
    def calculate_efficiency_ratios(self) -> Dict[str, Any]:
        """Calculate efficiency ratios."""
        neural_data = list(self.metrics_store['neural'])[-1:] if self.metrics_store['neural'] else []
        simulation_data = list(self.metrics_store['simulation'])[-1:] if self.metrics_store['simulation'] else []
        
        neural_metrics = neural_data[0]['metrics'] if neural_data else {}
        simulation_metrics = simulation_data[0]['metrics'] if simulation_data else {}
        
        return self.advanced_analyzer.calculate_efficiency_ratios(neural_metrics, simulation_metrics)
    
    def generate_performance_indicators(self) -> Dict[str, Any]:
        """Generate custom performance indicators."""
        system_data = list(self.metrics_store['system'])[-1:] if self.metrics_store['system'] else []
        neural_data = list(self.metrics_store['neural'])[-1:] if self.metrics_store['neural'] else []
        simulation_data = list(self.metrics_store['simulation'])[-1:] if self.metrics_store['simulation'] else []
        
        system_metrics = system_data[0]['metrics'] if system_data else {}
        neural_metrics = neural_data[0]['metrics'] if neural_data else {}
        simulation_metrics = simulation_data[0]['metrics'] if simulation_data else {}
        
        # Merge metrics for indicators
        all_metrics = {**system_metrics, **neural_metrics, **simulation_metrics}
        
        return self.advanced_analyzer.generate_performance_indicators(all_metrics)
    
    def get_performance_trends(self, hours: float = 1.0) -> Dict[str, Any]:
        """Get performance trends over specified time period."""
        cutoff_time = time.time() - (hours * 3600)
        
        recent_system = [d for d in self.metrics_store['system'] if d['timestamp'] > cutoff_time]
        recent_neural = [d for d in self.metrics_store['neural'] if d['timestamp'] > cutoff_time]
        recent_simulation = [d for d in self.metrics_store['simulation'] if d['timestamp'] > cutoff_time]
        
        def calculate_trends(data_points, metric_key):
            if not data_points:
                return {'trend': 'no_data', 'value': 0.0}
            
            values = [d['metrics'].get(metric_key, 0) for d in data_points]
            if not values:
                return {'trend': 'no_data', 'value': 0.0}
            
            # Calculate linear trend
            x = np.arange(len(values))
            y = np.array(values)
            slope = np.polyfit(x, y, 1)[0]
            
            return {
                'trend': 'increasing' if slope > 0.01 else 'decreasing' if slope < -0.01 else 'stable',
                'slope': slope,
                'current_value': values[-1],
                'average_value': np.mean(values),
                'min_value': np.min(values),
                'max_value': np.max(values)
            }
        
        trends = {
            'system_cpu': calculate_trends(recent_system, 'cpu_usage'),
            'system_memory': calculate_trends(recent_system, 'memory_percent'),
            'neural_firing_rate': calculate_trends(recent_neural, 'firing_rate'),
            'simulation_fps': calculate_trends(recent_simulation, 'fps'),
            'simulation_steps_per_second': calculate_trends(recent_simulation, 'steps_per_second'),
            'simulation_avg_step_time': calculate_trends(recent_simulation, 'average_step_time_ms')
        }
        
        return {
            'timestamp': time.time(),
            'time_period_hours': hours,
            'data_points_collected': len(recent_system) + len(recent_neural) + len(recent_simulation),
            'trends': trends
        }
    
    def export_metrics(self, filepath: str, format_type: str = 'json'):
        """Export metrics to file."""
        data = {
            'timestamp': time.time(),
            'metrics_store': {
                'system': list(self.metrics_store['system']),
                'neural': list(self.metrics_store['neural']),
                'simulation': list(self.metrics_store['simulation'])
            },
            'performance_alerts': self.performance_alerts,
            'thresholds': self.thresholds
        }
        
        if format_type.lower() == 'json':
            with open(filepath, 'w') as f:
                json.dump(data, f, indent=2, default=str)
        elif format_type.lower() == 'csv':
            # Export as CSV for each metric type
            self._export_csv_format(filepath, data)
        
        return True
    
    def _export_csv_format(self, base_filepath: str, data: Dict[str, Any]):
        """Export metrics in CSV format."""
        # Implementation for CSV export
        pass
    
    def import_metrics(self, filepath: str):
        """Import metrics from file."""
        try:
            with open(filepath, 'r') as f:
                data = json.load(f)
            
            # Clear existing data and import
            self.metrics_store = {
                'system': deque(maxlen=10000),
                'neural': deque(maxlen=10000),
                'simulation': deque(maxlen=10000)
            }
            
            # Import data
            for metric_type in ['system', 'neural', 'simulation']:
                if metric_type in data.get('metrics_store', {}):
                    for point in data['metrics_store'][metric_type]:
                        self.metrics_store[metric_type].append(point)
            
            self.performance_alerts = data.get('performance_alerts', [])
            self.thresholds = data.get('thresholds', self.thresholds)
            
            return True
        except Exception as e:
            print(f"Error importing metrics: {e}")
            return False
    
    def get_summary_statistics(self) -> Dict[str, Any]:
        """Get summary statistics of performance metrics."""
        summary = {
            'timestamp': time.time(),
            'total_monitoring_time': len(list(self.metrics_store['system'])) * self.collection_interval if self.metrics_store['system'] else 0,
            'system_metrics_count': len(self.metrics_store['system']),
            'neural_metrics_count': len(self.metrics_store['neural']),
            'simulation_metrics_count': len(self.metrics_store['simulation']),
            'alerts_generated': len(self.performance_alerts)
        }
        
        if self.metrics_store['system']:
            cpu_values = [d['metrics'].get('cpu_usage', 0) for d in self.metrics_store['system']]
            summary['system_cpu_stats'] = {
                'mean': np.mean(cpu_values),
                'median': np.median(cpu_values),
                'std': np.std(cpu_values),
                'min': np.min(cpu_values),
                'max': np.max(cpu_values),
                'p95': np.percentile(cpu_values, 95)
            }
        
        if self.metrics_store['simulation']:
            fps_values = [d['metrics'].get('fps', 0) for d in self.metrics_store['simulation']]
            summary['simulation_fps_stats'] = {
                'mean': np.mean(fps_values),
                'median': np.median(fps_values),
                'std': np.std(fps_values),
                'min': np.min(fps_values),
                'max': np.max(fps_values),
                'p95': np.percentile(fps_values, 95)
            }
        
        return summary

# Additional command classes for performance monitoring

class MonitorCommand:
    """Real-time monitoring commands."""
    
    def __init__(self, performance_monitor: PerformanceMonitor):
        self.monitor = performance_monitor
        
    def execute_monitor(self, duration: int = 60, update_interval: float = 1.0):
        """Real-time system monitoring dashboard."""
        print(f"Starting real-time monitoring for {duration} seconds...")
        
        self.monitor.start_monitoring(update_interval)
        
        end_time = time.time() + duration
        snapshot_count = 0
        
        while time.time() < end_time:
            snapshot = self.monitor.collect_snapshot()
            snapshot_count += 1
            
            print(f"\n[{snapshot_count}] Performance Snapshot - {datetime.fromtimestamp(snapshot.timestamp)}")
            print(f"CPU Usage: {snapshot.system_metrics.get('cpu_usage', 0):.1f}%")
            print(f"Memory Usage: {snapshot.system_metrics.get('memory_percent', 0):.1f}%")
            print(f"FPS: {snapshot.simulation_metrics.get('fps', 0):.1f}")
            print(f"Firing Rate: {snapshot.neural_metrics.get('firing_rate', 0):.3f}")
            print(f"Step Time: {snapshot.simulation_metrics.get('average_step_time_ms', 0):.1f}ms")
            
            time.sleep(update_interval)
        
        self.monitor.stop_monitoring()
        return {
            'duration': duration,
            'snapshots_collected': snapshot_count,
            'monitoring_completed': True
        }
    
    def execute_track(self, metric: str, duration: int = 60):
        """Track specific performance metrics."""
        print(f"Tracking metric '{metric}' for {duration} seconds...")
        
        self.monitor.start_monitoring(1.0)
        
        end_time = time.time() + duration
        metric_values = []
        
        while time.time() < end_time:
            snapshot = self.monitor.collect_snapshot()
            metric_value = self._extract_metric_from_snapshot(snapshot, metric)
            metric_values.append({
                'timestamp': snapshot.timestamp,
                'value': metric_value
            })
            
            time.sleep(1.0)
        
        self.monitor.stop_monitoring()
        
        return {
            'metric': metric,
            'values': metric_values,
            'average': sum(v['value'] for v in metric_values) / len(metric_values) if metric_values else 0,
            'min': min(v['value'] for v in metric_values) if metric_values else 0,
            'max': max(v['value'] for v in metric_values) if metric_values else 0
        }
    
    def _extract_metric_from_snapshot(self, snapshot: PerformanceSnapshot, metric: str) -> float:
        """Extract specific metric from snapshot."""
        metric_map = {
            'cpu': snapshot.system_metrics.get('cpu_usage', 0),
            'memory': snapshot.system_metrics.get('memory_percent', 0),
            'fps': snapshot.simulation_metrics.get('fps', 0),
            'firing_rate': snapshot.neural_metrics.get('firing_rate', 0),
            'step_time': snapshot.simulation_metrics.get('average_step_time_ms', 0),
            'latency': snapshot.simulation_metrics.get('average_latency_ms', 0),
            'throughput': snapshot.simulation_metrics.get('steps_per_second', 0)
        }
        
        return metric_map.get(metric, 0.0)

class AnalysisCommand:
    """Performance analysis commands."""
    
    def __init__(self, performance_monitor: PerformanceMonitor):
        self.monitor = performance_monitor
        
    def execute_analyze_performance(self) -> Dict[str, Any]:
        """Analyze performance bottlenecks."""
        print("Analyzing performance bottlenecks...")
        
        bottleneck_analysis = self.monitor.analyze_bottlenecks()
        resource_analysis = self.monitor.analyze_resource_allocation()
        efficiency_ratios = self.monitor.calculate_efficiency_ratios()
        
        results = {
            'bottleneck_analysis': bottleneck_analysis,
            'resource_allocation_analysis': resource_analysis,
            'efficiency_ratios': efficiency_ratios,
            'recommendations': self._generate_recommendations(bottleneck_analysis, resource_analysis)
        }
        
        print(f"Analysis complete. Found {bottleneck_analysis['total_bottlenecks']} bottlenecks.")
        return results
    
    def execute_collect_metrics(self) -> Dict[str, Any]:
        """Collect and display performance metrics."""
        print("Collecting performance metrics...")
        
        system_data = list(self.monitor.metrics_store['system'])
        neural_data = list(self.monitor.metrics_store['neural'])
        simulation_data = list(self.monitor.metrics_store['simulation'])
        
        metrics_summary = {
            'system_metrics_count': len(system_data),
            'neural_metrics_count': len(neural_data),
            'simulation_metrics_count': len(simulation_data),
            'latest_system_metrics': system_data[-1]['metrics'] if system_data else {},
            'latest_neural_metrics': neural_data[-1]['metrics'] if neural_data else {},
            'latest_simulation_metrics': simulation_data[-1]['metrics'] if simulation_data else {},
            'alerts_generated': len(self.monitor.performance_alerts)
        }
        
        print(f"Collected {metrics_summary['system_metrics_count']} system, {metrics_summary['neural_metrics_count']} neural, {metrics_summary['simulation_metrics_count']} simulation metrics.")
        return metrics_summary
    
    def execute_generate_statistics(self) -> Dict[str, Any]:
        """Generate statistical analysis."""
        print("Generating statistical analysis...")
        
        # Get trends for different time periods
        trends_1h = self.monitor.get_performance_trends(hours=1.0)
        trends_24h = self.monitor.get_performance_trends(hours=24.0)
        
        statistics = {
            'summary_statistics': self.monitor.get_summary_statistics(),
            'trends_1h': trends_1h,
            'trends_24h': trends_24h,
            'performance_indicators': self.monitor.generate_performance_indicators()
        }
        
        print(f"Statistics generated. Collected {statistics['summary_statistics']['total_monitoring_time']:.1f} seconds of data.")
        return statistics
    
    def execute_track_trends(self, metric: str, hours: float = 24.0) -> Dict[str, Any]:
        """Track performance trends over time."""
        print(f"Tracking trends for '{metric}' over {hours} hours...")
        
        trends = self.monitor.get_performance_trends(hours=hours)
        metric_trend = trends['trends'].get(metric)
        
        trend_analysis = {
            'metric': metric,
            'trend_analysis': metric_trend,
            'time_period_hours': hours,
            'data_points': trends['data_points_collected']
        }
        
        return trend_analysis
    
    def _generate_recommendations(self, bottleneck_analysis: Dict[str, Any], 
                                  resource_analysis: Dict[str, Any]) -> List[str]:
        """Generate optimization recommendations."""
        recommendations = []
        
        for bottleneck in bottleneck_analysis['bottlenecks']:
            if bottleneck['type'] == 'cpu':
                recommendations.append("Consider optimizing CPU-intensive operations or adding CPU resources.")
            elif bottleneck['type'] == 'memory':
                recommendations.append("Monitor memory usage and consider memory optimization or additional memory.")
            elif bottleneck['type'] == 'neural':
                recommendations.append("Review neural network parameters and firing rate thresholds.")
            elif bottleneck['type'] == 'simulation':
                recommendations.append("Analyze simulation step timing and optimize critical code paths.")
        
        efficiency_score = resource_analysis.get('overall_efficiency', 0)
        if efficiency_score < 50:
            recommendations.append("Resource allocation efficiency is poor. Consider rebalancing resources.")
        
        if not recommendations:
            recommendations.append("Performance is optimal. Continue monitoring for any degradation.")
        
        return recommendations

class ResourceCommand:
    """Resource monitoring commands."""
    
    def __init__(self, performance_monitor: PerformanceMonitor):
        self.monitor = performance_monitor
        
    def execute_memory_usage(self) -> Dict[str, Any]:
        """Monitor memory usage."""
        print("Monitoring memory usage...")
        
        process = psutil.Process()
        memory_info = process.memory_info()
        memory_percent = process.memory_percent()
        
        memory_metrics = {
            'timestamp': time.time(),
            'memory_usage_mb': memory_info.rss / (1024 * 1024),
            'memory_percent': memory_percent,
            'memory_available_mb': psutil.virtual_memory().available / (1024 * 1024),
            'memory_shared_mb': memory_info.shared / (1024 * 1024) if hasattr(memory_info, 'shared') else 0,
            'memory_text_mb': memory_info.text / (1024 * 1024) if hasattr(memory_info, 'text') else 0,
            'memory_lib_mb': memory_info.lib / (1024 * 1024) if hasattr(memory_info, 'lib') else 0,
            'memory_data_mb': memory_info.data / (1024 * 1024) if hasattr(memory_info, 'data') else 0
        }
        
        return memory_metrics
    
    def execute_cpu_usage(self) -> Dict[str, Any]:
        """Monitor CPU usage."""
        print("Monitoring CPU usage...")
        
        process = psutil.Process()
        cpu_percent = process.cpu_percent()
        
        cpu_metrics = {
            'timestamp': time.time(),
            'cpu_usage_percent': cpu_percent,
            'cpu_count': psutil.cpu_count(),
            'cpu_freq': psutil.cpu_freq().current if psutil.cpu_freq() else 0.0,
            'cpu_temperature': self._get_cpu_temperature(),
            'load_average': psutil.getloadavg()[0] if hasattr(psutil, 'getloadavg') else 0.0
        }
        
        return cpu_metrics
    
    def execute_network_traffic(self) -> Dict[str, Any]:
        """Monitor network traffic."""
        print("Monitoring network traffic...")
        
        process = psutil.Process()
        net_io = process.net_io_counters()
        
        network_metrics = {
            'timestamp': time.time(),
            'bytes_sent': net_io.bytes_sent,
            'bytes_recv': net_io.bytes_recv,
            'packets_sent': net_io.packets_sent,
            'packets_recv': net_io.packets_recv,
            'connections': len(process.connections())
        }
        
        return network_metrics
    
    def execute_disk_usage(self) -> Dict[str, Any]:
        """Monitor disk usage."""
        print("Monitoring disk usage...")
        
        disk = psutil.disk_usage('/')
        
        disk_metrics = {
            'timestamp': time.time(),
            'disk_usage_percent': disk.percent,
            'disk_free_bytes': disk.free,
            'disk_used_bytes': disk.used,
            'disk_total_bytes': disk.total
        }
        
        return disk_metrics
    
    def execute_process_info(self) -> Dict[str, Any]:
        """Monitor process information."""
        print("Monitoring process information...")
        
        process = psutil.Process()
        
        process_metrics = {
            'timestamp': time.time(),
            'process_id': process.pid,
            'process_name': process.name(),
            'process_status': process.status(),
            'process_username': process.username(),
            'parent_process_id': process.ppid(),
            'process_cmdline': process.cmdline(),
            'process_create_time': process.create_time(),
            'process_cpu_percent': process.cpu_percent(),
            'process_memory_percent': process.memory_percent(),
            'process_threads': process.num_threads(),
            'process_open_files': len(process.open_files())
        }
        
        return process_metrics
    
    def _get_cpu_temperature(self) -> float:
        """Get CPU temperature (platform dependent)."""
        try:
            if hasattr(psutil, 'sensors_temperatures'):
                temps = psutil.sensors_temperatures()
                if 'coretemp' in temps:
                    return temps['coretemp'][0].current
                elif 'cpu-thermal' in temps:
                    return temps['cpu-thermal'][0].current
        except:
            pass
        return 0.0

class NeuralCommand:
    """Neural performance monitoring commands."""
    
    def __init__(self):
        self.neural_monitor = NeuralMonitor()
        
    def execute_firing_rate_analysis(self, neuron_counts: Dict[str, Any]) -> Dict[str, Any]:
        """Analyze firing rate patterns."""
        print("Analyzing firing rate patterns...")
        
        neural_metrics = self.neural_monitor.analyze_firing_rate(neuron_counts)
        
        analysis_result = {
            'analysis_type': 'firing_rate',
            'timestamp': neural_metrics['timestamp'],
            'neurons_analyzed': neuron_counts.get('total', 0),
            'firing_rate': neural_metrics['firing_rate'],
            'activity_level': neural_metrics['activity_level'],
            'recommendations': self._analyze_firing_rate_patterns(neural_metrics)
        }
        
        return analysis_result
    
    def execute_synaptic_efficiency(self, total_synapses: int, active_synapses: int,
                                   weight_changes: List[float]) -> Dict[str, Any]:
        """Measure synaptic efficiency."""
        print("Analyzing synaptic efficiency...")
        
        neural_metrics = self.neural_monitor.analyze_synaptic_efficiency(
            total_synapses, active_synapses, weight_changes
        )
        
        efficiency_result = {
            'analysis_type': 'synaptic_efficiency',
            'timestamp': neural_metrics['timestamp'],
            'efficiency_ratio': neural_metrics['efficiency_ratio'],
            'stability': neural_metrics['stability'],
            'recommendations': self._analyze_synaptic_efficiency(neural_metrics)
        }
        
        return efficiency_result
    
    def execute_spike_timing_analysis(self, spike_times: List[float]) -> Dict[str, Any]:
        """Analyze spike timing precision."""
        print("Analyzing spike timing...")
        
        neural_metrics = self.neural_monitor.analyze_spike_timing(spike_times)
        
        timing_result = {
            'analysis_type': 'spike_timing',
            'timestamp': neural_metrics['timestamp'],
            'coherence': neural_metrics['coherence'],
            'cv_coefficient': neural_metrics['cv_coefficient'],
            'regularity': 'regular' if neural_metrics['cv_coefficient'] < 0.1 else 'irregular',
            'recommendations': self._analyze_spike_timing_patterns(neural_metrics)
        }
        
        return timing_result
    
    def execute_neural_plasticity(self, plasticity_history: List[float]) -> Dict[str, Any]:
        """Monitor plasticity effectiveness."""
        print("Analyzing neural plasticity...")
        
        neural_metrics = self.neural_monitor.analyze_plasticity(plasticity_history)
        
        plasticity_result = {
            'analysis_type': 'neural_plasticity',
            'timestamp': neural_metrics['timestamp'],
            'current_plasticity': neural_metrics['current_plasticity'],
            'average_plasticity': neural_metrics['average_plasticity'],
            'direction': neural_metrics['direction'],
            'effectiveness': neural_metrics['effectiveness'],
            'recommendations': self._analyze_plasticity_patterns(neural_metrics)
        }
        
        return plasticity_result
    
    def execute_learning_performance(self, learning_curve: List[float]) -> Dict[str, Any]:
        """Measure learning progress."""
        print("Analyzing learning performance...")
        
        if not learning_curve:
            return {
                'analysis_type': 'learning_performance',
                'timestamp': time.time(),
                'learning_score': 0.0,
                'convergence': 'not_started',
                'recommendations': ['No learning data available']
            }
        
        final_score = learning_curve[-1]
        initial_score = learning_curve[0]
        learning_gain = final_score - initial_score
        
        # Calculate learning rate (slope of learning curve)
        x = np.arange(len(learning_curve))
        y = np.array(learning_curve)
        learning_rate = np.polyfit(x, y, 1)[0]
        
        # Check for convergence
        if len(learning_curve) >= 10:
            recent_std = np.std(learning_curve[-10:])
            convergence = 'converged' if recent_std < 0.01 else 'still_learning'
        else:
            convergence = 'insufficient_data'
        
        learning_result = {
            'analysis_type': 'learning_performance',
            'timestamp': time.time(),
            'learning_score': final_score,
            'learning_gain': learning_gain,
            'learning_rate': learning_rate,
            'convergence': convergence,
            'recommendations': self._analyze_learning_patterns(final_score, learning_gain, learning_rate)
        }
        
        return learning_result
    
    def _analyze_firing_rate_patterns(self, metrics: Dict[str, Any]) -> List[str]:
        """Generate recommendations from firing rate analysis."""
        recommendations = []
        firing_rate = metrics.get('firing_rate', 0)
        activity_level = metrics.get('activity_level', 0)
        
        if firing_rate < 0.05:
            recommendations.append("Low firing rate detected. Consider increasing neural excitability or synaptic input.")
        elif firing_rate > 0.5:
            recommendations.append("High firing rate detected. Risk of neural saturation. Consider reducing input or increasing inhibition.")
        
        if activity_level < 0.1:
            recommendations.append("Low overall activity. Brain may be under-aroused. Consider increasing stimulation.")
        
        if 0.1 <= firing_rate <= 0.3:
            recommendations.append("Optimal firing rate range. Maintain current parameters.")
        
        return recommendations
    
    def _analyze_synaptic_efficiency(self, metrics: Dict[str, Any]) -> List[str]:
        """Generate recommendations from synaptic efficiency analysis."""
        recommendations = []
        efficiency = metrics.get('efficiency_ratio', 0)
        stability = metrics.get('stability', 0)
        
        if efficiency < 0.1:
            recommendations.append("Very low synaptic efficiency. Consider improving synaptic connectivity or reducing noise.")
        elif efficiency > 0.8:
            recommendations.append("High synaptic efficiency. Network is well-optimized.")
        
        if stability < 0.5:
            recommendations.append("Low synaptic stability. Plasticity may be too high. Consider reducing learning rate.")
        
        return recommendations
    
    def _analyze_spike_timing_patterns(self, metrics: Dict[str, Any]) -> List[str]:
        """Generate recommendations from spike timing analysis."""
        recommendations = []
        coherence = metrics.get('coherence', 0)
        cv_coefficient = metrics.get('cv_coefficient', 0)
        
        if coherence < 0.5:
            recommendations.append("Low spike timing coherence. Neural synchronization may be impaired.")
        
        if cv_coefficient > 0.5:
            recommendations.append("High variability in spike timing. Neural processing may be less precise.")
        
        if 0.7 <= coherence <= 1.0 and cv_coefficient < 0.3:
            recommendations.append("Excellent spike timing precision. Neural timing is optimal.")
        
        return recommendations
    
    def _analyze_plasticity_patterns(self, metrics: Dict[str, Any]) -> List[str]:
        """Generate recommendations from plasticity analysis."""
        recommendations = []
        direction = metrics.get('direction', 'stable')
        effectiveness = metrics.get('effectiveness', 0)
        
        if direction == 'increasing' and effectiveness > 0.5:
            recommendations.append("Plasticity is increasing rapidly. Monitor for over-plasticity and consider reducing learning rate.")
        elif direction == 'decreasing':
            recommendations.append("Plasticity is decreasing. Learning may be slowing down. Consider reviewing learning parameters.")
        elif direction == 'stable' and effectiveness > 0.7:
            recommendations.append("Stable and effective plasticity. Network has good adaptive capacity.")
        
        return recommendations
    
    def _analyze_learning_patterns(self, final_score: float, learning_gain: float, learning_rate: float) -> List[str]:
        """Generate recommendations from learning performance analysis."""
        recommendations = []
        
        if learning_gain < 0.1:
            recommendations.append("Low learning gain. Task may be too difficult or learning parameters need adjustment.")
        elif learning_gain > 1.0:
            recommendations.append("Excellent learning gain. Strong adaptive capacity.")
        
        if learning_rate < 0.01:
            recommendations.append("Slow learning rate. Consider increasing learning rate or providing better training signals.")
        elif learning_rate > 0.1:
            recommendations.append("Rapid learning rate. Monitor for over-learning or instability.")
        
        if final_score < 0.5:
            recommendations.append("Low final performance. Consider task difficulty or neural capacity.")
        
        return recommendations

class SimulationCommand:
    """Simulation performance monitoring commands."""
    
    def __init__(self):
        self.simulation_monitor = SimulationMonitor()
        
    def execute_simulation_speed(self, steps_completed: int, wall_time: float) -> Dict[str, Any]:
        """Measure simulation speed."""
        print("Measuring simulation speed...")
        
        neural_metrics = self.simulation_monitor.measure_simulation_speed(steps_completed, wall_time)
        
        speed_result = {
            'measurement_type': 'simulation_speed',
            'timestamp': neural_metrics['timestamp'],
            'steps_per_second': neural_metrics['steps_per_second'],
            'average_throughput': neural_metrics['average_throughput'],
            'time_per_step_ms': neural_metrics['time_per_step_ms'],
            'performance_grade': self._grade_simulation_performance(neural_metrics['time_per_step_ms'])
        }
        
        return speed_result
    
    def execute_step_timing(self, step_time: float) -> Dict[str, Any]:
        """Measure step execution times."""
        print("Measuring step timing...")
        
        neural_metrics = self.simulation_monitor.measure_step_timing(step_time)
        
        timing_result = {
            'measurement_type': 'step_timing',
            'timestamp': neural_metrics['timestamp'],
            'step_time_ms': neural_metrics['step_time_ms'],
            'average_step_time_ms': neural_metrics['average_step_time_ms'],
            'std_step_time_ms': neural_metrics['std_step_time_ms'],
            'min_step_time_ms': neural_metrics['min_step_time_ms'],
            'max_step_time_ms': neural_metrics['max_step_time_ms'],
            'performance_grade': self._grade_step_timing(neural_metrics['average_step_time_ms'])
        }
        
        return timing_result
    
    def execute_frame_rate(self, frame_time: float) -> Dict[str, Any]:
        """Measure frame rate for visualization."""
        print("Measuring frame rate...")
        
        neural_metrics = self.simulation_monitor.measure_frame_rate(frame_time)
        
        frame_result = {
            'measurement_type': 'frame_rate',
            'timestamp': neural_metrics['timestamp'],
            'frame_time_ms': neural_metrics['frame_time_ms'],
            'fps': neural_metrics['fps'],
            'average_fps': neural_metrics['average_fps'],
            'frame_time_target_ms': neural_metrics['frame_time_target_ms'],
            'frame_time_variance': neural_metrics['frame_time_variance'],
            'performance_grade': self._grade_frame_rate(neural_metrics['fps'])
        }
        
        return frame_result
    
    def execute_throughput(self, steps_completed: int, time_period: float) -> Dict[str, Any]:
        """Measure simulation throughput."""
        print("Measuring simulation throughput...")
        
        throughput = steps_completed / max(time_period, 0.0001)
        
        throughput_result = {
            'measurement_type': 'throughput',
            'timestamp': time.time(),
            'steps_completed': steps_completed,
            'time_period_s': time_period,
            'steps_per_second': throughput,
            'performance_grade': self._grade_throughput(throughput)
        }
        
        return throughput_result
    
    def execute_latency(self, latency: float) -> Dict[str, Any]:
        """Measure simulation latency."""
        print("Measuring simulation latency...")
        
        neural_metrics = self.simulation_monitor.measure_latency(latency)
        
        latency_result = {
            'measurement_type': 'latency',
            'timestamp': neural_metrics['timestamp'],
            'current_latency_ms': neural_metrics['current_latency_ms'],
            'average_latency_ms': neural_metrics['average_latency_ms'],
            'max_latency_ms': neural_metrics['max_latency_ms'],
            'p95_latency_ms': neural_metrics['p95_latency_ms'],
            'performance_grade': self._grade_latency(neural_metrics['average_latency_ms'])
        }
        
        return latency_result
    
    def _grade_simulation_performance(self, time_per_step_ms: float) -> str:
        """Grade simulation performance based on step time."""
        if time_per_step_ms < 10:
            return 'A+'
        elif time_per_step_ms < 20:
            return 'A'
        elif time_per_step_ms < 50:
            return 'B'
        elif time_per_step_ms < 100:
            return 'C'
        elif time_per_step_ms < 200:
            return 'D'
        else:
            return 'F'
    
    def _grade_step_timing(self, average_step_time_ms: float) -> str:
        """Grade step timing performance."""
        if average_step_time_ms < 10:
            return 'Excellent'
        elif average_step_time_ms < 20:
            return 'Good'
        elif average_step_time_ms < 50:
            return 'Acceptable'
        elif average_step_time_ms < 100:
            return 'Needs Improvement'
        else:
            return 'Poor'
    
    def _grade_frame_rate(self, fps: float) -> str:
        """Grade frame rate performance."""
        if fps >= 60:
            return 'Excellent'
        elif fps >= 30:
            return 'Good'
        elif fps >= 15:
            return 'Acceptable'
        elif fps >= 10:
            return 'Needs Improvement'
        else:
            return 'Poor'
    
    def _grade_throughput(self, throughput: float) -> str:
        """Grade throughput performance."""
        if throughput >= 100:
            return 'Excellent'
        elif throughput >= 50:
            return 'Good'
        elif throughput >= 10:
            return 'Acceptable'
        elif throughput >= 1:
            return 'Needs Improvement'
        else:
            return 'Poor'
    
    def _grade_latency(self, average_latency_ms: float) -> str:
        """Grade latency performance."""
        if average_latency_ms <= 1:
            return 'Excellent'
        elif average_latency_ms <= 5:
            return 'Good'
        elif average_latency_ms <= 10:
            return 'Acceptable'
        elif average_latency_ms <= 50:
            return 'Needs Improvement'
        else:
            return 'Poor'

class VisualizationCommand:
    """Visualization and reporting commands."""
    
    def __init__(self, performance_monitor: PerformanceMonitor):
        self.monitor = performance_monitor
        
    def execute_generate_reports(self, format: str = 'json') -> Dict[str, Any]:
        """Generate performance reports."""
        print(f"Generating performance reports in {format} format...")
        
        summary = self.monitor.get_summary_statistics()
        
        report_data = {
            'timestamp': time.time(),
            'report_type': 'performance_summary',
            'format': format,
            'summary_statistics': summary,
            'performance_indicators': self.monitor.generate_performance_indicators(),
            'recommendations': self._generate_report_recommendations(summary)
        }
        
        filename = f"performance_report_{datetime.now().strftime('%Y%m%d_%H%M%S')}.{format}"
        
        if format.lower() == 'json':
            with open(filename, 'w') as f:
                json.dump(report_data, f, indent=2, default=str)
        
        print(f"Report generated: {filename}")
        return report_data
    
    def execute_create_dashboards(self) -> Dict[str, Any]:
        """Create real-time dashboards."""
        print("Creating real-time dashboards...")
        
        dashboard_data = {
            'timestamp': time.time(),
            'dashboard_type': 'performance_dashboard',
            'charts': self._create_dashboard_charts(),
            'metrics': self._create_dashboard_metrics(),
            'alerts': self.monitor.performance_alerts,
            'refresh_interval_s': 5
        }
        
        dashboard_file = f"performance_dashboard_{datetime.now().strftime('%Y%m%d_%H%M%S')}.json"
        
        with open(dashboard_file, 'w') as f:
            json.dump(dashboard_data, f, indent=2, default=str)
        
        print(f"Dashboard created: {dashboard_file}")
        return dashboard_data
    
    def execute_export_metrics(self, filepath: str, format_type: str = 'json') -> Dict[str, Any]:
        """Export metrics for analysis."""
        print(f"Exporting metrics to {filepath}...")
        
        success = self.monitor.export_metrics(filepath, format_type)
        
        export_result = {
            'timestamp': time.time(),
            'file_path': filepath,
            'format': format_type,
            'status': 'success' if success else 'failed'
        }
        
        return export_result
    
    def execute_visualize_trends(self, metric: str, hours: float = 24.0) -> Dict[str, Any]:
        """Visualize performance trends."""
        print(f"Visualizing trends for '{metric}'...")
        
        trends = self.monitor.get_performance_trends(hours=hours)
        
        visualization_data = {
            'timestamp': time.time(),
            'metric': metric,
            'time_period_hours': hours,
            'trends': trends,
            'chart_data': self._prepare_chart_data(trends, metric)
        }
        
        return visualization_data
    
    def execute_create_heatmaps(self, metric: str, hours: float = 24.0) -> Dict[str, Any]:
        """Create performance heatmaps."""
        print(f"Creating heatmaps for '{metric}'...")
        
        trends = self.monitor.get_performance_trends(hours=hours)
        
        heatmap_data = {
            'timestamp': time.time(),
            'metric': metric,
            'time_period_hours': hours,
            'heatmap_data': self._create_heatmap_data(trends, metric)
        }
        
        return heatmap_data
    
    def _generate_report_recommendations(self, summary: Dict[str, Any]) -> List[str]:
        """Generate recommendations for reports."""
        recommendations = []
        
        if summary.get('alerts_generated', 0) > 0:
            recommendations.append("Review generated performance alerts for system issues.")
        
        if summary.get('total_monitoring_time', 0) < 3600:
            recommendations.append("Extend monitoring duration for more comprehensive analysis.")
        
        if summary.get('system_metrics_count', 0) < 100:
            recommendations.append("Increase sampling frequency for better statistical significance.")
        
        return recommendations
    
    def _create_dashboard_charts(self) -> Dict[str, Any]:
        """Create dashboard chart configurations."""
        return {
            'cpu_usage': {
                'type': 'line',
                'title': 'CPU Usage Over Time',
                'x_axis': 'Time',
                'y_axis': 'CPU %',
                'data_source': 'system_metrics'
            },
            'memory_usage': {
                'type': 'line',
                'title': 'Memory Usage Over Time',
                'x_axis': 'Time',
                'y_axis': 'Memory %',
                'data_source': 'system_metrics'
            },
            'simulation_fps': {
                'type': 'line',
                'title': 'Simulation FPS Over Time',
                'x_axis': 'Time',
                'y_axis': 'FPS',
                'data_source': 'simulation_metrics'
            },
            'neural_firing_rate': {
                'type': 'line',
                'title': 'Neural Firing Rate Over Time',
                'x_axis': 'Time',
                'y_axis': 'Firing Rate',
                'data_source': 'neural_metrics'
            }
        }
    
    def _create_dashboard_metrics(self) -> Dict[str, Any]:
        """Create dashboard metric displays."""
        return {
            'current_cpu': {'value': '0%', 'status': 'good', 'trend': 'stable'},
            'current_memory': {'value': '0%', 'status': 'good', 'trend': 'stable'},
            'current_fps': {'value': '0', 'status': 'good', 'trend': 'stable'},
            'current_firing_rate': {'value': '0.00', 'status': 'good', 'trend': 'stable'},
            'active_alerts': {'value': '0', 'status': 'good'},
            'monitoring_status': {'value': 'running', 'status': 'good'}
        }
    
    def _prepare_chart_data(self, trends: Dict[str, Any], metric: str) -> Dict[str, Any]:
        """Prepare chart data for visualization."""
        trend_analysis = trends['trends'].get(metric, {})
        
        return {
            'labels': [f"Point {i}" for i in range(10)],
            'values': [trend_analysis.get('current_value', 0)] * 10,
            'trend': trend_analysis.get('trend', 'stable'),
            'slope': trend_analysis.get('slope', 0.0)
        }
    
    def _create_heatmap_data(self, trends: Dict[str, Any], metric: str) -> Dict[str, Any]:
        """Create heatmap data for visualization."""
        return {
            'heatmap_grid': [[0.0 for _ in range(24)] for _ in range(7)],
            'metric': metric,
            'time_range': '24h'
        }

class AlertCommand:
    """Alerting and notification commands."""
    
    def __init__(self, performance_monitor: PerformanceMonitor):
        self.monitor = performance_monitor
        
    def execute_performance_alerts(self, enable: bool = True) -> Dict[str, Any]:
        """Set up performance alerts."""
        print("Setting up performance alerts...")
        
        self.monitor.thresholds.update({
            'cpu': 90.0,
            'memory': 90.0,
            'fps': 30.0,
            'step_time_ms': 100.0,
            'firing_rate': 0.5
        })
        
        alert_config = {
            'timestamp': time.time(),
            'alerts_enabled': enable,
            'thresholds': self.monitor.thresholds,
            'current_alerts': len(self.monitor.performance_alerts)
        }
        
        return alert_config
    
    def execute_notification_system(self) -> Dict[str, Any]:
        """Notification system configuration."""
        print("Configuring notification system...")
        
        notification_config = {
            'timestamp': time.time(),
            'notification_channels': ['email', 'slack', 'webhook'],
            'alert_priorities': ['low', 'medium', 'high', 'critical'],
            'notification_format': 'json',
            'rate_limit': '1_minute'
        }
        
        return notification_config
    
    def execute_auto_alerts(self, enable: bool = True) -> Dict[str, Any]:
        """Automatic performance alerting."""
        print("Configuring automatic performance alerts...")
        
        auto_alert_config = {
            'timestamp': time.time(),
            'auto_alerts_enabled': enable,
            'alert_rules': [
                {'condition': 'cpu_usage > 90%', 'severity': 'high'},
                {'condition': 'memory_usage > 90%', 'severity': 'high'},
                {'condition': 'fps < 30', 'severity': 'medium'},
                {'condition': 'firing_rate > 0.5', 'severity': 'low'}
            ],
            'cooldown_period': 60
        }
        
        return auto_alert_config
    
    def execute_thresholds(self, thresholds: Dict[str, Any]) -> Dict[str, Any]:
        """Configure performance thresholds."""
        print("Configuring performance thresholds...")
        
        self.monitor.thresholds.update(thresholds)
        
        thresholds_config = {
            'timestamp': time.time(),
            'updated_thresholds': self.monitor.thresholds,
            'thresholds_count': len(self.monitor.thresholds)
        }
        
        return thresholds_config
    
    def execute_escalation(self, escalation_policy: str = 'standard') -> Dict[str, Any]:
        """Alert escalation policies."""
        print("Configuring alert escalation policies...")
        
        escalation_policies = {
            'standard': {
                'level_1': 'email',
                'level_2': 'slack',
                'level_3': 'sms',
                'level_4': 'phone'
            },
            'immediate': {
                'level_1': 'email',
                'level_2': 'slack',
                'level_3': 'sms',
                'level_4': 'phone',
                'level_5': 'webhook'
            },
            'basic': {
                'level_1': 'email',
                'level_2': 'slack'
            }
        }
        
        escalation_config = {
            'timestamp': time.time(),
            'selected_policy': escalation_policy,
            'escalation_levels': escalation_policies.get(escalation_policy, escalation_policies['standard']),
            'alert_count': len(self.monitor.performance_alerts)
        }
        
        return escalation_config

# Main function to integrate with unified CLI

def main_performance_monitoring():
    """Main function for performance monitoring integration."""
    print("Starting NLM Performance Monitoring System...")
    
    # Initialize performance monitor
    performance_monitor = PerformanceMonitor()
    
    # Initialize command handlers
    monitor_cmd = MonitorCommand(performance_monitor)
    analysis_cmd = AnalysisCommand(performance_monitor)
    resource_cmd = ResourceCommand(performance_monitor)
    neural_cmd = NeuralCommand()
    simulation_cmd = SimulationCommand()
    visualization_cmd = VisualizationCommand(performance_monitor)
    alert_cmd = AlertCommand(performance_monitor)
    
    # Example usage
    try:
        # Start monitoring
        performance_monitor.start_monitoring(1.0)
        
        # Collect some sample metrics
        time.sleep(3)
        
        # Get snapshot
        snapshot = performance_monitor.collect_snapshot()
        print(f"\nPerformance Snapshot:")
        print(f"CPU Usage: {snapshot.system_metrics.get('cpu_usage', 0):.1f}%")
        print(f"Memory Usage: {snapshot.system_metrics.get('memory_percent', 0):.1f}%")
        print(f"FPS: {snapshot.simulation_metrics.get('fps', 0):.1f}")
        print(f"Firing Rate: {snapshot.neural_metrics.get('firing_rate', 0):.3f}")
        
        # Analyze performance
        bottleneck_analysis = performance_monitor.analyze_bottlenecks()
        print(f"\nBottleneck Analysis: Found {bottleneck_analysis['total_bottlenecks']} potential bottlenecks.")
        
        # Generate performance indicators
        indicators = performance_monitor.generate_performance_indicators()
        print(f"Performance Indicators: {indicators['overall_status']} status with {indicators['alerts_count']} alerts.")
        
        # Export metrics
        export_file = "performance_metrics_" + datetime.now().strftime('%Y%m%d_%H%M%S') + ".json"
        success = performance_monitor.export_metrics(export_file)
        print(f"Metrics exported to: {export_file}")
        
        # Stop monitoring
        performance_monitor.stop_monitoring()
        
        print("\nPerformance monitoring completed successfully!")
        return True
        
    except Exception as e:
        print(f"Performance monitoring error: {e}")
        performance_monitor.stop_monitoring()
        return False

if __name__ == "__main__":
    success = main_performance_monitoring()
    sys.exit(0 if success else 1)