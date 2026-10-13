"""
Performance monitoring module for NLM neural simulation framework.

This module provides comprehensive performance monitoring and optimization tools
for tracking and analyzing NLM simulation performance.
"""

import time
import psutil
import os
from typing import Dict, List, Any, Optional
import json

from .enhanced_api import NLMError

class PerformanceMonitor:
    """
    Primary performance monitoring class that tracks and analyzes NLM simulation performance.
    
    Tracks core simulation metrics, resource usage, and provides performance analysis
    and optimization recommendations.
    """
    
    def __init__(self, brain=None):
        """Initialize performance monitor with optional brain reference"""
        self.brain = brain
        self.metrics = {
            'steps': 0,
            'start_time': time.time(),
            'neuron_count': 0,
            'synapse_count': 0,
            'total_spikes': 0,
            'max_firing_rate': 0.0,
            'memory_usage': 0,
            'cpu_usage': 0.0,
            'step_times': [],
            'step_count': 0,
        }
        
        if self.brain:
            self.metrics['neuron_count'] = self.brain.getTotalNeuronCount()
            self.metrics['synapse_count'] = self.brain.getTotalSynapseCount()
    
    def record_step(self):
        """Record a simulation step for performance tracking"""
        if not self.brain:
            raise NLMError("Brain instance required for performance monitoring")
        
        step_start = time.time()
        self.metrics['steps'] += 1
        self.metrics['total_spikes'] += self.brain.getFiringNeuronCount()
        
        # Calculate and update firing rate
        current_firing_rate = self.brain.getAverageFiringRate()
        self.metrics['max_firing_rate'] = max(
            self.metrics['max_firing_rate'],
            current_firing_rate
        )
        
        # Record step time for profiling
        step_time = time.time() - step_start
        self.metrics['step_times'].append(step_time)
        
        # Update resource usage
        self._update_resource_usage()
    
    def _update_resource_usage(self):
        """Update current resource usage metrics"""
        process = psutil.Process(os.getpid())
        
        # Memory usage in bytes
        memory_info = process.memory_info()
        self.metrics['memory_usage'] = memory_info.rss
        
        # CPU usage percentage
        self.metrics['cpu_usage'] = process.cpu_percent(interval=None)
    
    def get_performance_stats(self) -> Dict[str, Any]:
        """
        Get comprehensive performance statistics.
        
        Returns:
            Dictionary containing performance metrics and statistics
        """
        elapsed_time = time.time() - self.metrics['start_time']
        
        if elapsed_time > 0:
            steps_per_second = self.metrics['steps'] / elapsed_time
            spikes_per_second = self.metrics['total_spikes'] / elapsed_time
        else:
            steps_per_second = 0.0
            spikes_per_second = 0.0
        
        avg_step_time = 0.0
        if self.metrics['step_times']:
            avg_step_time = sum(self.metrics['step_times']) / len(self.metrics['step_times'])
        
        return {
            'simulation_summary': {
                'steps_completed': self.metrics['steps'],
                'elapsed_time_seconds': elapsed_time,
                'steps_per_second': steps_per_second,
                'spikes_per_second': spikes_per_second,
                'average_step_time_ms': avg_step_time * 1000,
            },
            'neural_performance': {
                'neurons': self.metrics['neuron_count'],
                'synapses': self.metrics['synapse_count'],
                'total_spikes': self.metrics['total_spikes'],
                'max_firing_rate': self.metrics['max_firing_rate'],
                'spike_efficiency': (self.metrics['total_spikes'] / 
                                 max(self.metrics['neuron_count'], 1)),
            },
            'resource_usage': {
                'memory_usage_mb': self.metrics['memory_usage'] / (1024 * 1024),
                'cpu_usage_percent': self.metrics['cpu_usage'],
                'peak_memory_mb': self.metrics['memory_usage'] / (1024 * 1024),  # Should track peak
            },
            'performance_indicators': {
                'simulation_speed_rating': self._get_speed_rating(steps_per_second),
                'neural_efficiency_rating': self._get_efficiency_rating(steps_per_second),
                'resource_efficiency_rating': self._get_resource_efficiency_rating(),
            }
        }
    
    def _get_speed_rating(self, sps: float) -> str:
        """Get speed rating based on steps per second"""
        if sps > 1000:
            return "Excellent"
        elif sps > 500:
            return "Good"
        elif sps > 100:
            return "Acceptable"
        elif sps > 10:
            return "Slow"
        else:
            return "Needs Optimization"
    
    def _get_efficiency_rating(self, sps: float) -> str:
        """Get neural efficiency rating"""
        if sps > 500 and self.metrics['max_firing_rate'] > 10:
            return "High"
        elif sps > 100:
            return "Moderate"
        else:
            return "Low"
    
    def _get_resource_efficiency_rating(self) -> str:
        """Get resource efficiency rating"""
        memory_mb = self.metrics['memory_usage'] / (1024 * 1024)
        neuron_count = self.metrics['neuron_count']
        
        if neuron_count > 0:
            memory_per_neuron = memory_mb / neuron_count
            if memory_per_neuron < 0.01:  # Less than 10KB per neuron
                return "Efficient"
            elif memory_per_neuron < 0.1:  # Less than 100KB per neuron
                return "Good"
            elif memory_per_neuron < 1.0:  # Less than 1MB per neuron
                return "Acceptable"
            else:
                return "Inefficient"
        
        return "Unknown"
    
    def get_efficiency_metrics(self) -> Dict[str, Any]:
        """
        Get efficiency metrics and optimization recommendations.
        
        Returns:
            Dictionary containing efficiency analysis and recommendations
        """
        stats = self.get_performance_stats()
        simulation = stats['simulation_summary']
        neural = stats['neural_performance']
        resources = stats['resource_usage']
        
        recommendations = []
        
        # Analyze step times
        if self.metrics['step_times']:
            avg_step_time = sum(self.metrics['step_times']) / len(self.metrics['step_times'])
            if avg_step_time > 0.1:  # Step taking more than 100ms
                recommendations.append(
                    f"Step time ({avg_step_time*1000:.2f}ms) is high. "
                    "Consider optimizing neural computations or reducing network size."
                )
        
        # Analyze memory efficiency
        memory_mb = resources['memory_usage_mb']
        neuron_count = neural['neurons']
        if neuron_count > 0:
            memory_per_neuron = memory_mb / neuron_count
            if memory_per_neuron > 0.1:  # More than 100KB per neuron
                recommendations.append(
                    f"Memory usage ({memory_per_neuron:.2f}MB/neuron) is high. "
                    "Consider using memory pools or reducing object overhead."
                )
        
        # Analyze firing rate efficiency
        firing_rate = neural['max_firing_rate']
        if firing_rate > 50:  # Very high firing rate
            recommendations.append(
                f"Firing rate ({firing_rate:.2f}Hz) is very high. "
                "Consider adjusting neuron parameters for more efficient dynamics."
            )
        
        # Speed optimization
        sps = simulation['steps_per_second']
        if sps < 100:
            recommendations.append(
                f"Simulation speed ({sps:.2f} SPS) is slow. "
                "Consider parallel processing or algorithmic optimizations."
            )
        
        # CPU optimization
        cpu_usage = resources['cpu_usage_percent']
        if cpu_usage > 80:
            recommendations.append(
                f"CPU usage ({cpu_usage:.1f}%) is high. "
                "Consider reducing computational complexity or enabling parallel processing."
            )
        
        return {
            'summary': {
                'overall_performance': self._calculate_overall_performance(stats),
                'optimization_priority': self._get_optimization_priority(stats),
                'recommendations_count': len(recommendations),
            },
            'detailed_metrics': stats,
            'recommendations': recommendations,
            'optimization_areas': self._identify_optimization_areas(stats),
        }
    
    def _calculate_overall_performance(self, stats: Dict[str, Any]) -> str:
        """Calculate overall performance rating"""
        simulation = stats['simulation_summary']
        sps = simulation['steps_per_second']
        
        if sps > 1000 and stats['neural_performance']['spike_efficiency'] > 10:
            return "Excellent"
        elif sps > 500:
            return "Good"
        elif sps > 100:
            return "Acceptable"
        else:
            return "Needs Optimization"
    
    def _get_optimization_priority(self, stats: Dict[str, Any]) -> str:
        """Get optimization priority based on performance metrics"""
        simulation = stats['simulation_summary']
        resources = stats['resource_usage']
        
        issues = []
        
        if simulation['steps_per_second'] < 100:
            issues.append("slow_simulation")
        
        if resources['cpu_usage_percent'] > 80:
            issues.append("high_cpu_usage")
        
        if stats['neural_performance']['spike_efficiency'] < 1:
            issues.append("low_neural_efficiency")
        
        if not issues:
            return "Maintenance"
        elif "slow_simulation" in issues and "high_cpu_usage" in issues:
            return "High"
        elif len(issues) >= 2:
            return "Medium"
        else:
            return "Low"
    
    def _identify_optimization_areas(self, stats: Dict[str, Any]) -> List[str]:
        """Identify specific areas for optimization"""
        areas = []
        
        simulation = stats['simulation_summary']
        if simulation['steps_per_second'] < 200:
            areas.append("simulation_speed")
        
        resources = stats['resource_usage']
        if resources['cpu_usage_percent'] > 60:
            areas.append("cpu_utilization")
        
        neural = stats['neural_performance']
        if neural['spike_efficiency'] < 5:
            areas.append("neural_efficiency")
        
        return areas
    
    def export_metrics(self, filepath: str):
        """
        Export performance metrics to file.
        
        Args:
            filepath: Path to output file (JSON format)
        """
        try:
            metrics = {
                'timestamp': time.time(),
                'performance_stats': self.get_performance_stats(),
                'efficiency_metrics': self.get_efficiency_metrics(),
                'configuration': self._get_configuration_info(),
            }
            
            with open(filepath, 'w') as f:
                json.dump(metrics, f, indent=2, default=str)
            
            print(f"Performance metrics exported to {filepath}")
            
        except Exception as e:
            raise NLMError(f"Error exporting metrics: {e}")
    
    def _get_configuration_info(self) -> Dict[str, Any]:
        """Get basic configuration information"""
        return {
            'neuron_count': self.metrics['neuron_count'],
            'synapse_count': self.metrics['synapse_count'],
            'simulation_time': time.time() - self.metrics['start_time'],
        }
