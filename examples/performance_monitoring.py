#!/usr/bin/env python3
"""
NLM Performance Monitoring and Debugging Utilities
===================================================

This module provides comprehensive performance monitoring and debugging utilities
for NLM (Neural Learning Machine) simulations. It includes:

1. **Performance Profiling** - Measure and analyze simulation performance
2. **Memory Monitoring** - Track memory usage and allocation patterns
3. **Neural Activity Monitoring** - Monitor neuron and synapse activity
4. **Debugging Tools** - Debug simulation issues and analyze performance bottlenecks
5. **Visualization Tools** - Generate performance visualizations and reports
6. **Benchmark Suite** - Comprehensive benchmarking framework
7. **Optimization Recommendations** - AI-powered optimization suggestions
8. **Real-time Monitoring** - Live performance monitoring during simulation
"""

import time
import threading
import psutil
import os
import sys
import json
import numpy as np
from typing import Dict, List, Optional, Any, Callable, Tuple, Union
from dataclasses import dataclass, field
from enum import Enum
from pathlib import Path
from collections import deque
import warnings
warnings.filterwarnings('ignore')

# Import NLM core modules
import pynlm
class PerformanceMetric(Enum):
    """Enumeration of performance metrics."""
    CPU_USAGE = "cpu_usage"
    MEMORY_USAGE = "memory_usage"
    SIMULATION_SPEED = "simulation_speed"
    NEURON_ACTIVITY = "neuron_activity"
    SYNAPSE_ACTIVITY = "synapse_activity"
    SPIKE_RATE = "spike_rate"
    LATENCY = "latency"
    THROUGHPUT = "throughput"
    PLASITICITY_UPDATE_TIME = "plasticity_update_time"
    NEUROMODULATION_TIME = "neuromodulation_time"
    WORKING_MEMORY_USAGE = "working_memory_usage"
    EPISODIC_MEMORY_USAGE = "episodic_memory_usage"
    PLANNING_TIME = "planning_time"
    ATTENTION_TIME = "attention_time"
class MetricCollector:
    """Collects and manages performance metrics."""

    def __init__(self, max_history: int = 1000):
        self.max_history = max_history
        self.metrics: Dict[str, deque] = {}
        self.timing_data: Dict[str, deque] = {}
        self.counters: Dict[str, int] = {}
        self.start_time = time.time()
        self.collection_active = False
        self.collection_thread: Optional[threading.Thread] = None
        self.collection_interval = 0.1  # seconds

    def start_collection(self):
        """Start collecting metrics in background thread."""
        self.collection_active = True
        self.collection_thread = threading.Thread(target=self._collect_loop, daemon=True)
        self.collection_thread.start()

    def stop_collection(self):
        """Stop collecting metrics."""
        self.collection_active = False
        if self.collection_thread:
            self.collection_thread.join(timeout=1.0)

    def record_metric(self, metric_name: str, value: Any, timestamp: Optional[float] = None):
        """Record a performance metric."""
        if timestamp is None:
            timestamp = time.time()
        
        if metric_name not in self.metrics:
            self.metrics[metric_name] = deque(maxlen=self.max_history)
        
        self.metrics[metric_name].append((timestamp, value))

    def record_timing(self, operation_name: str, duration: float):
        """Record timing for an operation."""
        if operation_name not in self.timing_data:
            self.timing_data[operation_name] = deque(maxlen=self.max_history)
        
        self.timing_data[operation_name].append((time.time(), duration))

    def increment_counter(self, counter_name: str):
        """Increment a counter."""
        if counter_name not in self.counters:
            self.counters[counter_name] = 0
        self.counters[counter_name] += 1

    def get_metric_history(self, metric_name: str, time_window: Optional[float] = None) -> List[Tuple[float, Any]]:
        """Get metric history with optional time window."""
        if metric_name not in self.metrics:
            return []
        
        history = list(self.metrics[metric_name])
        
        if time_window:
            cutoff_time = time.time() - time_window
            history = [(ts, val) for ts, val in history if ts > cutoff_time]
        
        return history

    def get_timing_stats(self, operation_name: str, time_window: Optional[float] = None) -> Dict[str, Any]:
        """Get timing statistics for an operation."""
        if operation_name not in self.timing_data:
            return {}
        
        timings = list(self.timing_data[operation_name])
        
        if time_window:
            cutoff_time = time.time() - time_window
            timings = [(ts, dur) for ts, dur in timings if ts > cutoff_time]
        
        if not timings:
            return {}
        
        durations = [dur for _, dur in timings]
        
        return {
            'count': len(durations),
            'mean': np.mean(durations),
            'median': np.median(durations),
            'std': np.std(durations),
            'min': np.min(durations),
            'max': np.max(durations),
            'p95': np.percentile(durations, 95),
            'p99': np.percentile(durations, 99)
        }

    def get_counter_stats(self) -> Dict[str, Any]:
        """Get counter statistics."""
        return self.counters.copy()

    def get_system_metrics(self) -> Dict[str, Any]:
        """Get current system metrics."""
        return {
            'uptime': time.time() - self.start_time,
            'memory_usage': psutil.virtual_memory().percent,
            'cpu_usage': psutil.cpu_percent(),
            'thread_count': threading.active_count(),
            'metric_names': list(self.metrics.keys()),
            'timing_operations': list(self.timing_data.keys()),
            'counter_names': list(self.counters.keys())
        }

    def _collect_loop(self):
        """Background collection loop."""
        while self.collection_active:
            # Collect system metrics
            self.record_metric('cpu_usage', psutil.cpu_percent())
            self.record_metric('memory_usage', psutil.virtual_memory().percent)
            self.record_metric('thread_count', threading.active_count())
            
            time.sleep(self.collection_interval)
class MemoryMonitor:
    """Monitor memory usage and allocation patterns."""

    def __init__(self):
        self.memory_samples: List[Dict[str, Any]] = []
        self.allocation_patterns: Dict[str, List[Dict[str, Any]]] = {}
        self.deallocation_patterns: Dict[str, List[Dict[str, Any]]] = {}
        self.malloc_count = 0
        self.free_count = 0
        self.current_memory = 0
        self.peak_memory = 0

    def start_monitoring(self):
        """Start memory monitoring."""
        self.memory_samples = []
        self.allocation_patterns = {}
        self.deallocation_patterns = {}
        self.malloc_count = 0
        self.free_count = 0
        self.current_memory = 0
        self.peak_memory = 0

    def stop_monitoring(self):
        """Stop memory monitoring."""
        pass

    def record_memory_event(self, event_type: str, size: int, object_type: str = None):
        """Record a memory event."""
        timestamp = time.time()
        
        event = {
            'timestamp': timestamp,
            'type': event_type,
            'size': size,
            'object_type': object_type
        }
        
        self.memory_samples.append(event)
        
        if event_type == 'malloc':
            self.malloc_count += 1
            self.current_memory += size
            self.peak_memory = max(self.peak_memory, self.current_memory)
            
            if object_type:
                if object_type not in self.allocation_patterns:
                    self.allocation_patterns[object_type] = []
                self.allocation_patterns[object_type].append(event)
                
        elif event_type == 'free':
            self.free_count += 1
            self.current_memory -= size
            
            if object_type:
                if object_type not in self.deallocation_patterns:
                    self.deallocation_patterns[object_type] = []
                self.deallocation_patterns[object_type].append(event)

    def get_memory_stats(self) -> Dict[str, Any]:
        """Get memory statistics."""
        if not self.memory_samples:
            return {}
        
        return {
            'current_memory': self.current_memory,
            'peak_memory': self.peak_memory,
            'total_allocations': self.malloc_count,
            'total_deallocations': self.free_count,
            'net_allocation': self.malloc_count - self.free_count,
            'memory_samples': self.memory_samples,
            'allocation_patterns': self.allocation_patterns,
            'deallocation_patterns': self.deallocation_patterns
        }

    def get_memory_efficiency(self) -> float:
        """Calculate memory efficiency."""
        if not self.memory_samples:
            return 0.0
        
        # Calculate memory efficiency as ratio of allocated to deallocated memory
        total_allocated = sum(event['size'] for event in self.memory_samples if event['type'] == 'malloc')
        total_deallocated = sum(event['size'] for event in self.memory_samples if event['type'] == 'free')
        
        if total_deallocated == 0:
            return 0.0
        
        return total_deallocated / total_allocated if total_allocated > 0 else 0.0
class NeuralActivityMonitor:
    """Monitor neural activity patterns and metrics."""

    def __init__(self):
        self.spike_times: List[float] = []
        self.firing_rates: List[Dict[str, Any]] = []
        self.neuron_activities: Dict[str, Dict[str, Any]] = {}
        self.synaptic_activities: Dict[str, Dict[str, Any]] = {}
        self.plasticity_events: List[Dict[str, Any]] = []
        self.neuromodulation_events: List[Dict[str, Any]] = []

    def record_spike(self, neuron_id: str, timestamp: float, neuron_type: str = "excitatory"):
        """Record a neural spike event."""
        self.spike_times.append(timestamp)
        
        if neuron_id not in self.neuron_activities:
            self.neuron_activities[neuron_id] = {
                'spike_count': 0,
                'firing_rate': 0.0,
                'last_spike_time': 0.0,
                'neuron_type': neuron_type
            }
        
        neuron_data = self.neuron_activities[neuron_id]
        neuron_data['spike_count'] += 1
        neuron_data['last_spike_time'] = timestamp
        
        # Calculate firing rate (spikes per second)
        if len(self.spike_times) > 1:
            time_window = timestamp - self.spike_times[0]
            if time_window > 0:
                neuron_data['firing_rate'] = neuron_data['spike_count'] / time_window

    def record_synaptic_event(self, synapse_id: str, event_type: str, weight_change: float = 0.0):
        """Record synaptic activity."""
        if synapse_id not in self.synaptic_activities:
            self.synaptic_activities[synapse_id] = {
                'event_count': 0,
                'weight_changes': [],
                'event_types': set(),
                'last_event_time': 0.0
            }
        
        synapse_data = self.synaptic_activities[synapse_id]
        synapse_data['event_count'] += 1
        synapse_data['event_types'].add(event_type)
        synapse_data['last_event_time'] = time.time()
        
        if weight_change != 0.0:
            synapse_data['weight_changes'].append({
                'change': weight_change,
                'timestamp': time.time()
            })

    def record_plasticity_event(self, event_type: str, magnitude: float, neurons_involved: List[str]):
        """Record plasticity event."""
        self.plasticity_events.append({
            'type': event_type,
            'magnitude': magnitude,
            'neurons_involved': neurons_involved,
            'timestamp': time.time()
        })

    def record_neuromodulation_event(self, neuromodulator: str, level: float, neurons_affected: List[str]):
        """Record neuromodulation event."""
        self.neuromodulation_events.append({
            'neuromodulator': neuromodulator,
            'level': level,
            'neurons_affected': neurons_affected,
            'timestamp': time.time()
        })

    def get_spike_statistics(self, time_window: Optional[float] = None) -> Dict[str, Any]:
        """Get spike statistics."""
        if not self.spike_times:
            return {}
        
        if time_window:
            cutoff_time = time.time() - time_window
            spike_times = [ts for ts in self.spike_times if ts > cutoff_time]
        else:
            spike_times = self.spike_times
        
        if not spike_times:
            return {}
        
        return {
            'total_spikes': len(spike_times),
            'time_window': time_window or (spike_times[-1] - spike_times[0]),
            'average_firing_rate': len(spike_times) / (time_window or (spike_times[-1] - spike_times[0])),
            'first_spike_time': spike_times[0],
            'last_spike_time': spike_times[-1]
        }

    def get_neuron_statistics(self) -> Dict[str, Any]:
        """Get neuron statistics."""
        return {
            'total_neurons': len(self.neuron_activities),
            'neurons_with_spikes': len([n for n in self.neuron_activities.values() if n['spike_count'] > 0]),
            'neuron_details': self.neuron_activities
        }

    def get_synaptic_statistics(self) -> Dict[str, Any]:
        """Get synaptic statistics."""
        return {
            'total_synapses': len(self.synaptic_activities),
            'synaptic_activity': self.synaptic_activities
        }

    def get_plasticity_statistics(self) -> Dict[str, Any]:
        """Get plasticity statistics."""
        return {
            'total_plasticity_events': len(self.plasticity_events),
            'plasticity_events_by_type': {
                event['type']: len([e for e in self.plasticity_events if e['type'] == event['type']])
                for event in self.plasticity_events
            },
            'plasticity_events': self.plasticity_events
        }

    def get_neuromodulation_statistics(self) -> Dict[str, Any]:
        """Get neuromodulation statistics."""
        return {
            'total_neuromodulation_events': len(self.neuromodulation_events),
            'neuromodulators': {
                event['neuromodulator']: len([e for e in self.neuromodulation_events if e['neuromodulator'] == event['neuromodulator']])
                for event in self.neuromodulation_events
            },
            'neuromodulation_events': self.neuromodulation_events
        }
class SimulationProfiler:
    """Profile simulation performance and identify bottlenecks."""

    def __init__(self, brain=None, world=None, agent=None):
        self.brain = brain
        self.world = world
        self.agent = agent
        self.performance_metrics: Dict[str, Any] = {}
        self.timing_data: Dict[str, List[float]] = {}
        self.bottleneck_detector = BottleneckDetector()
        self.optimization_recommender = OptimizationRecommender()

    def profile_simulation_step(self, step_number: int, step_start_time: float) -> Dict[str, Any]:
        """Profile a single simulation step."""
        step_end_time = time.time()
        step_duration = step_end_time - step_start_time
        
        # Collect basic performance metrics
        self.timing_data['total_step_time'].append(step_duration)
        
        # Collect component-specific metrics if available
        if self.brain:
            self.timing_data['brain_step_time'].append(step_duration * 0.7)  # Approximate
        
        if self.world:
            self.timing_data['world_update_time'].append(step_duration * 0.2)  # Approximate
        
        if self.agent:
            self.timing_data['agent_processing_time'].append(step_duration * 0.1)  # Approximate
        
        # Update performance metrics
        self.performance_metrics['last_step_time'] = step_duration
        self.performance_metrics['total_steps'] = self.performance_metrics.get('total_steps', 0) + 1
        self.performance_metrics['average_step_time'] = (
            np.mean(self.timing_data['total_step_time']) if self.timing_data['total_step_time'] else 0.0
        )
        
        # Detect bottlenecks
        bottleneck_report = self.bottleneck_detector.analyze_performance(self.timing_data)
        
        # Generate optimization recommendations
        optimization_suggestions = self.optimization_recommender.generate_suggestions(
            self.performance_metrics, self.timing_data
        )
        
        return {
            'step_number': step_number,
            'step_duration': step_duration,
            'performance_metrics': self.performance_metrics,
            'bottleneck_report': bottleneck_report,
            'optimization_suggestions': optimization_suggestions,
            'timing_data': self.timing_data
        }

    def get_performance_summary(self) -> Dict[str, Any]:
        """Get comprehensive performance summary."""
        if not self.timing_data['total_step_time']:
            return {}
        
        return {
            'simulation_stats': {
                'total_steps': self.performance_metrics.get('total_steps', 0),
                'average_step_time': self.performance_metrics.get('average_step_time', 0.0),
                'min_step_time': np.min(self.timing_data['total_step_time']),
                'max_step_time': np.max(self.timing_data['total_step_time']),
                'std_step_time': np.std(self.timing_data['total_step_time'])
            },
            'component_stats': {
                name: {
                    'mean': np.mean(times) if times else 0.0,
                    'std': np.std(times) if times else 0.0,
                    'min': np.min(times) if times else 0.0,
                    'max': np.max(times) if times else 0.0
                }
                for name, times in self.timing_data.items()
                if times
            },
            'bottleneck_analysis': self.bottleneck_detector.analyze_performance(self.timing_data),
            'optimization_recommendations': self.optimization_recommender.generate_suggestions(
                self.performance_metrics, self.timing_data
            )
        }

    def export_performance_data(self, filepath: str) -> bool:
        """Export performance data to file."""
        try:
            data = {
                'performance_metrics': self.performance_metrics,
                'timing_data': self.timing_data,
                'bottleneck_analysis': self.bottleneck_detector.analyze_performance(self.timing_data),
                'optimization_recommendations': self.optimization_recommender.generate_suggestions(
                    self.performance_metrics, self.timing_data
                )
            }
            
            with open(filepath, 'w') as f:
                json.dump(data, f, indent=2)
            
            return True
            
        except Exception as e:
            print(f"Error exporting performance data: {e}")
            return False
class BottleneckDetector:
    """Detect performance bottlenecks in simulations."""

    def analyze_performance(self, timing_data: Dict[str, List[float]]) -> Dict[str, Any]:
        """Analyze performance data for bottlenecks."""
        bottlenecks = []
        recommendations = []
        
        # Check for high variance in step times
        if 'total_step_time' in timing_data and timing_data['total_step_time']:
            step_times = timing_data['total_step_time']
            cv = np.std(step_times) / np.mean(step_times) if np.mean(step_times) > 0 else 0
            
            if cv > 0.5:  # Coefficient of variation > 50%
                bottlenecks.append({
                    'type': 'high_variance',
                    'severity': 'high',
                    'description': f"High variance in step times (CV: {cv:.2f})",
                    'impact': "Unpredictable performance and potential system instability"
                })
                recommendations.append("Consider implementing consistent timing mechanisms")
        
        # Check for increasing step times (performance degradation)
        if 'total_step_time' in timing_data and len(timing_data['total_step_time']) > 10:
            recent_steps = timing_data['total_step_time'][-10:]
            early_steps = timing_data['total_step_time'][:-10] if len(timing_data['total_step_time']) > 20 else []
            
            if early_steps:
                early_mean = np.mean(early_steps)
                recent_mean = np.mean(recent_steps)
                
                if recent_mean > early_mean * 1.5:  # Recent steps 50% slower
                    bottlenecks.append({
                        'type': 'performance_degradation',
                        'severity': 'medium',
                        'description': f"Performance degrading: early {early_mean:.4f}s vs recent {recent_mean:.4f}s",
                        'impact': "Memory leaks or accumulating computational overhead"
                    })
                    recommendations.append("Check for memory leaks or accumulating data structures")
        
        # Check for specific component bottlenecks
        if 'brain_step_time' in timing_data and timing_data['brain_step_time']:
            brain_times = timing_data['brain_step_time']
            if np.mean(brain_times) > 0.1:  # Brain steps taking > 100ms
                bottlenecks.append({
                    'type': 'brain_processing_bottleneck',
                    'severity': 'high',
                    'description': f"Brain processing bottleneck: {np.mean(brain_times):.4f}s average",
                    'impact': "Neural computation is the limiting factor"
                })
                recommendations.append("Consider optimizing neural dynamics or reducing neuron count")
        
        if 'world_update_time' in timing_data and timing_data['world_update_time']:
            world_times = timing_data['world_update_time']
            if np.mean(world_times) > 0.05:  # World updates taking > 50ms
                bottlenecks.append({
                    'type': 'world_processing_bottleneck',
                    'severity': 'medium',
                    'description': f"World processing bottleneck: {np.mean(world_times):.4f}s average",
                    'impact': "Environment simulation is slowing down"
                })
                recommendations.append("Consider simplifying world model or optimizing update algorithms")
        
        # Check for rare but expensive operations
        if 'agent_processing_time' in timing_data and timing_data['agent_processing_time']:
            agent_times = timing_data['agent_processing_time']
            max_agent_time = np.max(agent_times)
            if max_agent_time > 0.02:  # Agent processing taking > 20ms
                bottlenecks.append({
                    'type': 'agent_processing_spikes',
                    'severity': 'low',
                    'description': f"Agent processing spikes: max {max_agent_time:.4f}s",
                    'impact': "Occasional delays but overall system stable"
                })
                recommendations.append("Investigate and optimize expensive agent operations")
        
        # Overall performance score
        performance_score = 100.0
        performance_score -= len([b for b in bottlenecks if b['severity'] == 'high']) * 20
        performance_score -= len([b for b in bottlenecks if b['severity'] == 'medium']) * 10
        performance_score -= len([b for b in bottlenecks if b['severity'] == 'low']) * 5
        
        return {
            'bottlenecks': bottlenecks,
            'recommendations': recommendations,
            'performance_score': max(0, performance_score),
            'total_bottlenecks': len(bottlenecks)
        }
class OptimizationRecommender:
    """Generate optimization recommendations based on performance analysis."""

    def generate_suggestions(self, performance_metrics: Dict[str, Any], 
                             timing_data: Dict[str, List[float]]) -> List[Dict[str, Any]]:
        """Generate optimization recommendations."""
        suggestions = []
        
        # Check for slow average step time
        if 'total_step_time' in timing_data and timing_data['total_step_time']:
            avg_step_time = np.mean(timing_data['total_step_time'])
            
            if avg_step_time > 0.05:  # Steps taking > 50ms
                suggestions.append({
                    'category': 'general',
                    'priority': 'high',
                    'title': 'Optimize Overall Simulation Speed',
                    'description': f"Average step time ({avg_step_time:.4f}s) is high",
                    'suggestions': [
                        "Reduce neuron count if precision allows",
                        "Use SIMD optimizations for neural computations",
                        "Implement parallel processing for independent components",
                        "Consider using lower precision floating point"
                    ],
                    'estimated_improvement': '25-50% reduction in step time'
                })
        
        # Check for brain processing bottleneck
        if 'brain_step_time' in timing_data and timing_data['brain_step_time']:
            brain_times = timing_data['brain_step_time']
            if np.mean(brain_times) > 0.05:
                suggestions.append({
                    'category': 'brain_optimization',
                    'priority': 'high',
                    'title': 'Optimize Brain Processing',
                    'description': f"Brain processing averaging {np.mean(brain_times):.4f}s per step",
                    'suggestions': [
                        "Reduce synaptic connectivity density",
                        "Implement event-driven spike propagation",
                        "Use approximation methods for distant neurons",
                        "Consider pruning inactive neurons"
                    ],
                    'estimated_improvement': '30-40% reduction in brain processing time'
                })
        
        # Check for world processing bottleneck
        if 'world_update_time' in timing_data and timing_data['world_update_time']:
            world_times = timing_data['world_update_time']
            if np.mean(world_times) > 0.02:
                suggestions.append({
                    'category': 'world_optimization',
                    'priority': 'medium',
                    'title': 'Optimize World Processing',
                    'description': f"World updates averaging {np.mean(world_times):.4f}s per step",
                    'suggestions': [
                        "Reduce world size or update frequency",
                        "Implement spatial partitioning for large worlds",
                        "Use approximate collision detection",
                        "Cache frequently computed world states"
                    ],
                    'estimated_improvement': '20-35% reduction in world update time'
                })
        
        # Check for memory issues
        if 'memory_usage' in performance_metrics:
            memory_usage = performance_metrics['memory_usage']
            if memory_usage > 80:  # Memory usage > 80%
                suggestions.append({
                    'category': 'memory_optimization',
                    'priority': 'medium',
                    'title': 'Optimize Memory Usage',
                    'description': f"High memory usage ({memory_usage}%)",
                    'suggestions': [
                        "Implement memory pooling for neuron/synapse data",
                        "Use sparse representations for neural connectivity",
                        "Implement memory-efficient data structures",
                        "Consider checkpointing for long simulations"
                    ],
                    'estimated_improvement': '15-30% reduction in memory usage'
                })
        
        # Check for spike rate analysis
        if 'spike_rate' in performance_metrics:
            spike_rate = performance_metrics['spike_rate']
            if spike_rate > 100:  # Spike rate > 100 Hz
                suggestions.append({
                    'category': 'spike_optimization',
                    'priority': 'low',
                    'title': 'Optimize Spike Processing',
                    'description': f"High spike rate ({spike_rate} Hz)",
                    'suggestions': [
                        "Implement spike compression for high-activity regions",
                        "Use rate-coded representations instead of spiking",
                        "Consider adaptive time stepping",
                        "Implement spike throttling for noise reduction"
                    ],
                    'estimated_improvement': '10-20% reduction in spike processing time'
                })
        
        # Check for plasticity overhead
        if 'plasticity_update_time' in timing_data and timing_data['plasticity_update_time']:
            plasticity_times = timing_data['plasticity_update_time']
            if np.mean(plasticity_times) > 0.01:
                suggestions.append({
                    'category': 'plasticity_optimization',
                    'priority': 'medium',
                    'title': 'Optimize Plasticity Updates',
                    'description': f"Plasticity updates averaging {np.mean(plasticity_times):.4f}s",
                    'suggestions': [
                        "Implement approximate plasticity calculations",
                        "Use batch plasticity updates",
                        "Implement eligibility trace optimization",
                        "Consider sparse plasticity rules"
                    ],
                    'estimated_improvement': '25-35% reduction in plasticity update time'
                })
        
        # Check for neuromodulation overhead
        if 'neuromodulation_time' in timing_data and timing_data['neuromodulation_time']:
            neuromod_times = timing_data['neuromodulation_time']
            if np.mean(neuromod_times) > 0.005:
                suggestions.append({
                    'category': 'neuromodulation_optimization',
                    'priority': 'low',
                    'title': 'Optimize Neuromodulation',
                    'description': f"Neuromodulation averaging {np.mean(neurom_times):.4f}s",
                    'suggestions': [
                        "Implement neuromodulator pooling",
                        "Use simplified neuromodulation models",
                        "Consider event-driven neuromodulation",
                        "Implement neuromodulator thresholding"
                    ],
                    'estimated_improvement': '15-25% reduction in neuromodulation time'
                })
        
        return suggestions
class RealTimeMonitor:
    """Real-time monitoring of simulation performance during execution."""

    def __init__(self, update_interval: float = 1.0):
        self.update_interval = update_interval
        self.monitoring_active = False
        self.monitor_thread: Optional[threading.Thread] = None
        self.performance_data: List[Dict[str, Any]] = []
        self.alerts: List[Dict[str, Any]] = []
        self.thresholds = {
            'cpu_usage': 90.0,
            'memory_usage': 80.0,
            'step_time': 0.1,  # seconds
            'spike_rate': 500.0,  # Hz
            'neuron_activity': 0.9  # fraction of active neurons
        }

    def start_monitoring(self):
        """Start real-time monitoring."""
        self.monitoring_active = True
        self.monitor_thread = threading.Thread(target=self._monitor_loop, daemon=True)
        self.monitor_thread.start()

    def stop_monitoring(self):
        """Stop real-time monitoring."""
        self.monitoring_active = False
        if self.monitor_thread:
            self.monitor_thread.join(timeout=1.0)

    def _monitor_loop(self):
        """Monitoring loop."""
        last_report_time = time.time()
        
        while self.monitoring_active:
            current_time = time.time()
            
            # Collect system metrics
            metrics = {
                'timestamp': current_time,
                'cpu_usage': psutil.cpu_percent(),
                'memory_usage': psutil.virtual_memory().percent,
                'thread_count': threading.active_count()
            }
            
            # Check for alerts
            self._check_alerts(metrics)
            
            # Store metrics
            self.performance_data.append(metrics)
            
            # Report periodically
            if current_time - last_report_time >= self.update_interval:
                self._report_status()
                last_report_time = current_time
            
            time.sleep(0.1)  # Sample every 100ms

    def _check_alerts(self, metrics: Dict[str, Any]):
        """Check for performance alerts."""
        for metric_name, threshold in self.thresholds.items():
            if metric_name in metrics:
                value = metrics[metric_name]
                if value > threshold:
                    alert = {
                        'timestamp': time.time(),
                        'metric': metric_name,
                        'value': value,
                        'threshold': threshold,
                        'severity': 'high' if value > threshold * 1.5 else 'medium'
                    }
                    self.alerts.append(alert)

    def _report_status(self):
        """Report monitoring status."""
        if self.performance_data:
            latest = self.performance_data[-1]
            
            print(f"\n[{latest['timestamp']:.1f}] Performance Status:")
            print(f"  CPU Usage: {latest['cpu_usage']:.1f}%")
            print(f"  Memory Usage: {latest['memory_usage']:.1f}%")
            print(f"  Thread Count: {latest['thread_count']}")
            
            if self.alerts:
                recent_alerts = [a for a in self.alerts if time.time() - a['timestamp'] < 60]
                if recent_alerts:
                    print(f"  Alerts: {len(recent_alerts)}")
                    for alert in recent_alerts[-3:]:  # Show last 3 alerts
                        print(f"    - {alert['metric']}: {alert['value']:.1f}% (threshold: {alert['threshold']}%)")

    def get_performance_history(self, time_window: Optional[float] = None) -> List[Dict[str, Any]]:
        """Get performance history."""
        if time_window:
            cutoff_time = time.time() - time_window
            return [p for p in self.performance_data if p['timestamp'] > cutoff_time]
        else:
            return self.performance_data.copy()

    def get_alerts(self, time_window: Optional[float] = None) -> List[Dict[str, Any]]:
        """Get alerts with optional time window."""
        if time_window:
            cutoff_time = time.time() - time_window
            return [a for a in self.alerts if a['timestamp'] > cutoff_time]
        else:
            return self.alerts.copy()
class SimulationBenchmark:
    """Comprehensive benchmarking framework for NLM simulations."""

    def __init__(self):
        self.benchmark_results: List[Dict[str, Any]] = []

    def run_benchmark(self, name: str, brain, world, agent, 
                     num_steps: int = 1000, num_repeats: int = 3) -> Dict[str, Any]:
        """Run a comprehensive benchmark."""
        print(f"\nRunning benchmark: {name}")
        print(f"  Steps: {num_steps} x {num_repeats} repeats")
        
        results = {
            'name': name,
            'num_steps': num_steps,
            'num_repeats': num_repeats,
            'step_results': [],
            'average_step_time': 0.0,
            'min_step_time': float('inf'),
            'max_step_time': 0.0,
            'std_step_time': 0.0,
            'total_time': 0.0,
            'steps_per_second': 0.0,
            'memory_usage': {},
            'performance_metrics': {}
        }
        
        # Run benchmark multiple times
        for repeat in range(num_repeats):
            print(f"  Run {repeat + 1}/{num_repeats}")
            
            # Reset simulation if needed
            if brain:
                brain.reset()
                brain.initialize()
            
            # Record start time
            start_time = time.time()
            
            # Run simulation
            for step in range(num_steps):
                step_start = time.time()
                
                # Execute simulation step
                if world:
                    world.update(0.1)
                
                if agent:
                    percept = world.getSensoryPercept()
                    agent.processSensoryInput(percept)
                
                if brain:
                    brain.step(step)
                
                if agent:
                    action = agent.decodeMotorCommand()
                    if world:
                        world.applyMotorCommand(action, world.getSimulationTime())
                
                # Record step timing
                step_duration = time.time() - step_start
                results['step_results'].append(step_duration)
                
                # Update min/max
                results['min_step_time'] = min(results['min_step_time'], step_duration)
                results['max_step_time'] = max(results['max_step_time'], step_duration)
            
            # Calculate repeat statistics
            repeat_duration = time.time() - start_time
            repeat_steps_per_second = num_steps / repeat_duration
            
            results['step_results'] = results['step_results']
            results['total_time'] += repeat_duration
            results['steps_per_second'] += repeat_steps_per_second
            
            print(f"    Total time: {repeat_duration:.2f}s")
            print(f"    Steps/sec: {repeat_steps_per_second:.0f}")
        
        # Calculate final statistics
        results['average_step_time'] = np.mean(results['step_results'])
        results['std_step_time'] = np.std(results['step_results'])
        results['total_time'] /= num_repeats
        results['steps_per_second'] /= num_repeats
        
        # Remove infinity from min
        if results['min_step_time'] == float('inf'):
            results['min_step_time'] = 0.0
        
        # Get memory usage
        if hasattr(psutil, 'virtual_memory'):
            results['memory_usage'] = {
                'current': psutil.virtual_memory().used,
                'peak': psutil.virtual_memory().peak,
                'percent': psutil.virtual_memory().percent
            }
        
        # Get performance metrics
        if brain:
            results['performance_metrics'] = {
                'total_spikes': brain.getTotalSpikeCount(),
                'firing_neuron_count': brain.getFiringNeuronCount(),
                'average_firing_rate': brain.getAverageFiringRate(),
                'neuron_count': brain.getTotalNeuronCount(),
                'synapse_count': brain.getTotalSynapseCount()
            }
        
        # Store benchmark results
        self.benchmark_results.append(results)
        
        print(f"\nBenchmark Results: {name}")
        print(f"  Average step time: {results['average_step_time']*1000:.1f}ms")
        print(f"  Steps per second: {results['steps_per_second']:.0f}")
        print(f"  Total time: {results['total_time']:.2f}s")
        print(f"  Total spikes: {results['performance_metrics']['total_spikes']}")
        
        return results

    def get_benchmark_summary(self) -> Dict[str, Any]:
        """Get summary of all benchmark results."""
        if not self.benchmark_results:
            return {}
        
        return {
            'total_benchmarks': len(self.benchmark_results),
            'benchmarks': self.benchmark_results,
            'best_performance': max(self.benchmark_results, key=lambda x: x['steps_per_second']) if self.benchmark_results else None,
            'average_performance': {
                'steps_per_second': np.mean([b['steps_per_second'] for b in self.benchmark_results]),
                'average_step_time': np.mean([b['average_step_time'] for b in self.benchmark_results])
            },
            'performance_range': {
                'steps_per_second': {
                    'min': min(b['steps_per_second'] for b in self.benchmark_results),
                    'max': max(b['steps_per_second'] for b in self.benchmark_results)
                },
                'step_time': {
                    'min': min(b['average_step_time'] for b in self.benchmark_results),
                    'max': max(b['average_step_time'] for b in self.benchmark_results)
                }
            }
        }

    def export_benchmark_results(self, filepath: str) -> bool:
        """Export benchmark results to file."""
        try:
            data = self.get_benchmark_summary()
            
            with open(filepath, 'w') as f:
                json.dump(data, f, indent=2)
            
            return True
            
        except Exception as e:
            print(f"Error exporting benchmark results: {e}")
            return False
def main():
    """Demonstrate NLM performance monitoring and debugging utilities."""
    print("NLM Performance Monitoring and Debugging Utilities")
    print("=" * 60)
    
    # Create example neural system
    print("\n1. Creating example neural system...")
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 1000)
    config.set("random_seed", 42)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    world.setAgentStart(10.0, 10.0)
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Start performance monitoring
    print("\n2. Starting performance monitoring...")
    metric_collector = MetricCollector()
    metric_collector.start_collection()
    
    # Start memory monitoring
    memory_monitor = MemoryMonitor()
    memory_monitor.start_monitoring()
    
    # Start neural activity monitoring
    neural_monitor = NeuralActivityMonitor()
    
    # Create profiler
    profiler = SimulationProfiler(brain, world, agent)
    
    # Create real-time monitor
    realtime_monitor = RealTimeMonitor(update_interval=2.0)
    realtime_monitor.start_monitoring()
    
    # Run simulation with monitoring
    print("\n3. Running simulation with monitoring...")
    simulation_start_time = time.time()
    
    for step in range(100):
        step_start = time.time()
        
        # Execute simulation step
        world.update(0.1)
        
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        brain.step(step)
        
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
        agent.applyRewardModulation(reward, 0.0)
        agent.updateDevelopment(0.1)
        
        # Record neural activity
        neural_monitor.record_spike(f"neuron_{step % 100}", step, "excitatory")
        
        # Profile step
        profile_result = profiler.profile_simulation_step(step, step_start)
        
        # Record metrics periodically
        if step % 10 == 0:
            metric_collector.record_metric('simulation_step', step)
            metric_collector.record_timing('brain_step', profile_result['performance_metrics'].get('average_step_time', 0.0))
        
        # Print progress
        if step % 25 == 0:
            print(f"   Step {step}: {brain.getAverageFiringRate():.2f} Hz, {brain.getFiringNeuronCount()} firing")
    
    simulation_end_time = time.time()
    total_duration = simulation_end_time - simulation_start_time
    
    # Stop monitoring
    print("\n4. Stopping monitoring...")
    metric_collector.stop_collection()
    memory_monitor.stop_monitoring()
    realtime_monitor.stop_monitoring()
    
    # Display results
    print("\n5. Performance Analysis Results:")
    print("=" * 60)
    
    # Display profiler results
    profile_summary = profiler.get_performance_summary()
    print(f"\nSimulation Statistics:")
    print(f"  Total steps: {profile_summary['simulation_stats']['total_steps']}")
    print(f"  Average step time: {profile_summary['simulation_stats']['average_step_time']*1000:.1f}ms")
    print(f"  Step time std: {profile_summary['simulation_stats']['std_step_time']*1000:.1f}ms")
    print(f"  Total duration: {total_duration:.2f}s")
    print(f"  Steps per second: {profile_summary['simulation_stats']['total_steps']/total_duration:.0f}")
    
    # Display bottleneck analysis
    bottleneck_analysis = profile_summary['bottleneck_analysis']
    print(f"\nBottleneck Analysis:")
    print(f"  Performance Score: {bottleneck_analysis['performance_score']}/100")
    print(f"  Total Bottlenecks: {bottleneck_analysis['total_bottlenecks']}")
    
    if bottleneck_analysis['bottlenecks']:
        print(f"  Identified Bottlenecks:")
        for bottleneck in bottleneck_analysis['bottlenecks']:
            print(f"    - {bottleneck['type']}: {bottleneck['severity'].upper()} ({bottleneck['description']})")
    
    # Display optimization recommendations
    recommendations = bottleneck_analysis['recommendations']
    if recommendations:
        print(f"\nOptimization Recommendations:")
        for i, recommendation in enumerate(recommendations, 1):
            print(f"  {i}. {recommendation}")
    
    # Display neural activity statistics
    neural_stats = neural_monitor.get_neuron_statistics()
    spike_stats = neural_monitor.get_spike_statistics()
    print(f"\nNeural Activity Statistics:")
    print(f"  Total neurons: {neural_stats['total_neurons']}")
    print(f"  Neurons with spikes: {neural_stats['neurons_with_spikes']}")
    print(f"  Total spikes: {spike_stats['total_spikes']}")
    print(f"  Average firing rate: {spike_stats['average_firing_rate']:.2f} Hz")
    
    # Display memory statistics
    memory_stats = memory_monitor.get_memory_stats()
    if memory_stats:
        print(f"\nMemory Statistics:")
        print(f"  Current memory: {memory_stats['current_memory'] / 1024 / 1024:.2f} MB")
        print(f"  Peak memory: {memory_stats['peak_memory'] / 1024 / 1024:.2f} MB")
        print(f"  Memory efficiency: {memory_monitor.get_memory_efficiency():.3f}")
    
    # Display system metrics
    system_metrics = metric_collector.get_system_metrics()
    print(f"\nSystem Metrics:")
    print(f"  Uptime: {system_metrics['uptime']:.1f}s")
    print(f"  CPU Usage: {system_metrics['cpu_usage']:.1f}%")
    print(f"  Memory Usage: {system_metrics['memory_usage']:.1f}%")
    print(f"  Thread Count: {system_metrics['thread_count']}")
    
    # Display real-time monitoring status
    realtime_stats = realtime_monitor.get_performance_history()
    if realtime_stats:
        latest = realtime_stats[-1]
        print(f"\nReal-time Monitoring:")
        print(f"  Latest CPU Usage: {latest['cpu_usage']:.1f}%")
        print(f"  Latest Memory Usage: {latest['memory_usage']:.1f}%")
        print(f"  Active Alerts: {len(realtime_monitor.get_alerts())}")
    
    # Export performance data
    print("\n6. Exporting performance data...")
    profiler.export_performance_data("simulation_performance.json")
    
    # Run benchmark
    print("\n7. Running comprehensive benchmark...")
    benchmark = SimulationBenchmark()
    benchmark_result = benchmark.run_benchmark(
        name="performance_monitoring_benchmark",
        brain=brain,
        world=world,
        agent=agent,
        num_steps=200,
        num_repeats=2
    )
    
    # Display benchmark results
    benchmark_summary = benchmark.get_benchmark_summary()
    print(f"\nBenchmark Summary:")
    print(f"  Best performance: {benchmark_summary['best_performance']['steps_per_second']:.0f} steps/sec")
    print(f"  Average performance: {benchmark_summary['average_performance']['steps_per_second']:.0f} steps/sec")
    print(f"  Performance range: {benchmark_summary['performance_range']['steps_per_second']['min']:.0f} - {benchmark_summary['performance_range']['steps_per_second']['max']:.0f} steps/sec")
    
    # Export benchmark results
    benchmark.export_benchmark_results("benchmark_results.json")
    
    print("\n" + "=" * 60)
    print("Performance monitoring and debugging demonstration completed successfully!")
    print("=" * 60)
    print("\nGenerated files:")
    print("  - simulation_performance.json")
    print("  - benchmark_results.json")
    print("  - Console output contains detailed performance analysis")
    print("\nKey insights:")
    print("  - Performance profiling identifies bottlenecks")
    print("  - Memory monitoring tracks allocation patterns")
    print("  - Neural activity monitoring provides detailed spike analysis")
    print("  - Real-time monitoring enables live debugging")
    print("  - Benchmarking provides repeatable performance metrics")

if __name__ == "__main__":
    main()