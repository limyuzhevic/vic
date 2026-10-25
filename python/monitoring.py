"""Monitoring and statistics for NLM simulations"""

import time
import statistics
from typing import List, Dict, Any, Optional
from dataclasses import dataclass
from enum import Enum
from collections import deque
import math
class MonitoringLevel(Enum):
    """Monitoring levels."""
    BASIC = "basic"
    DETAILED = "detailed"
    COMPREHENSIVE = "comprehensive"
class StatisticsCollector:
    """Collect and analyze statistics from NLM simulations."""
    
    def __init__(self, level: MonitoringLevel = MonitoringLevel.BASIC):
        self.level = level
        self.step_data: List[Dict[str, Any]] = []
        self.neural_activity: Dict[int, List[float]] = {}
        self.spike_history: List[Dict[str, Any]] = []
        self.region_stats: Dict[str, Dict[str, Any]] = {}
        
        # Rolling averages for performance monitoring
        self.recent_activity = deque(maxlen=1000)
        self.recent_spikes = deque(maxlen=1000)
    
    def record_step(self, step: int, metrics: Dict[str, Any], 
                   region_data: Optional[Dict[str, Any]] = None):
        """Record step data."""
        step_record = {
            'step': step,
            'timestamp': time.time(),
            'metrics': metrics.copy(),
            'region_data': region_data or {}
        }
        
        self.step_data.append(step_record)
        
        # Record neural activity
        if 'active_neurons' in metrics:
            self.neural_activity[step] = [
                metrics['active_neurons'],
                metrics.get('firing_neurons', 0),
                metrics.get('average_firing_rate', 0.0),
                metrics.get('total_spikes', 0)
            ]
        
        # Record spike history
        if 'total_spikes' in metrics:
            spike_record = {
                'step': step,
                'total_spikes': metrics['total_spikes'],
                'spikes_this_step': metrics['total_spikes'] - (
                    self.step_data[-2]['metrics'].get('total_spikes', 0) if len(self.step_data) > 1 else 0
                ),
                'firing_rate': metrics.get('average_firing_rate', 0.0)
            }
            self.spike_history.append(spike_record)
        
        # Update recent activity
        self.recent_activity.append({
            'step': step,
            'active_neurons': metrics.get('active_neurons', 0),
            'firing_rate': metrics.get('average_firing_rate', 0.0),
            'timestamp': time.time()
        })
        
        # Record region-specific stats if provided
        if region_data:
            for region_id, region_metrics in region_data.items():
                if region_id not in self.region_stats:
                    self.region_stats[region_id] = {
                        'neuron_counts': [],
                        'synapse_counts': [],
                        'firing_rates': [],
                        'average_weights': []
                    }
                
                self.region_stats[region_id]['neuron_counts'].append(
                    region_metrics.get('neuron_count', 0)
                )
                self.region_stats[region_id]['synapse_counts'].append(
                    region_metrics.get('synapse_count', 0)
                )
                self.region_stats[region_id]['firing_rates'].append(
                    region_metrics.get('firing_rate', 0.0)
                )
                self.region_stats[region_id]['average_weights'].append(
                    region_metrics.get('average_weight', 0.0)
                )
    
    def get_summary_statistics(self) -> Dict[str, Any]:
        """Get summary statistics across all recorded steps."""
        if not self.step_data:
            return {}
        
        all_metrics = [record['metrics'] for record in self.step_data]
        
        # Basic statistics
        steps = len(self.step_data)
        total_spikes = sum(m.get('total_spikes', 0) for m in all_metrics)
        
        stats = {
            'total_steps': steps,
            'total_spikes': total_spikes,
            'average_spikes_per_step': total_spikes / steps if steps > 0 else 0,
            'min_spikes': min(m.get('total_spikes', 0) for m in all_metrics),
            'max_spikes': max(m.get('total_spikes', 0) for m in all_metrics),
            'std_spikes': statistics.stdev(
                [m.get('total_spikes', 0) for m in all_metrics]
            ) if len(all_metrics) > 1 else 0.0
        }
        
        # Neural activity statistics
        if self.neural_activity:
            active_neurons = [activity[0] for activity in self.neural_activity.values()]
            firing_neurons = [activity[1] for activity in self.neural_activity.values()]
            firing_rates = [activity[2] for activity in self.neural_activity.values()]
            
            stats['neural_activity'] = {
                'average_active_neurons': statistics.mean(active_neurons) if active_neurons else 0,
                'average_firing_neurons': statistics.mean(firing_neurons) if firing_neurons else 0,
                'average_firing_rate': statistics.mean(firing_rates) if firing_rates else 0,
                'min_firing_rate': min(firing_rates) if firing_rates else 0,
                'max_firing_rate': max(firing_rates) if firing_rates else 0,
                'firing_rate_std': statistics.stdev(firing_rates) if len(firing_rates) > 1 else 0.0
            }
        
        # Spike statistics
        if self.spike_history:
            spikes_per_step = [spike['spikes_this_step'] for spike in self.spike_history]
            spike_firing_rates = [spike['firing_rate'] for spike in self.spike_history]
            
            stats['spike_statistics'] = {
                'average_spikes_per_step': statistics.mean(spikes_per_step) if spikes_per_step else 0,
                'spike_rate_std': statistics.stdev(spikes_per_step) if len(spikes_per_step) > 1 else 0.0,
                'average_firing_rate': statistics.mean(spike_firing_rates) if spike_firing_rates else 0,
                'max_firing_rate': max(spike_firing_rates) if spike_firing_rates else 0
            }
        
        # Region statistics
        if self.region_stats:
            region_summary = {}
            for region_id, stats_data in self.region_stats.items():
                if stats_data['neuron_counts']:
                    region_summary[region_id] = {
                        'neuron_count': {
                            'mean': statistics.mean(stats_data['neuron_counts']),
                            'min': min(stats_data['neuron_counts']),
                            'max': max(stats_data['neuron_counts'])
                        },
                        'synapse_count': {
                            'mean': statistics.mean(stats_data['synapse_counts']),
                            'min': min(stats_data['synapse_counts']),
                            'max': max(stats_data['synapse_counts'])
                        },
                        'firing_rate': {
                            'mean': statistics.mean(stats_data['firing_rates']),
                            'min': min(stats_data['firing_rates']),
                            'max': max(stats_data['firing_rates'])
                        },
                        'average_weight': {
                            'mean': statistics.mean(stats_data['average_weights']),
                            'min': min(stats_data['average_weights']),
                            'max': max(stats_data['average_weights'])
                        }
                    }
            
            stats['region_statistics'] = region_summary
        
        # Performance metrics
        if self.recent_activity:
            recent_times = [record['timestamp'] for record in list(self.recent_activity)[-100:]]
            if len(recent_times) > 1:
                time_diffs = [t2 - t1 for t1, t2 in zip(recent_times[:-1], recent_times[1:])]
                stats['performance'] = {
                    'average_step_interval': statistics.mean(time_diffs) if time_diffs else 0,
                    'min_step_interval': min(time_diffs) if time_diffs else 0,
                    'max_step_interval': max(time_diffs) if time_diffs else 0
                }
        
        return stats
    
    def get_time_series_data(self, metric_name: str, 
                           start_step: Optional[int] = None,
                           end_step: Optional[int] = None) -> List[Any]:
        """Get time series data for a specific metric."""
        if not self.step_data:
            return []
        
        # Filter by step range
        filtered_data = self.step_data
        if start_step is not None:
            filtered_data = [d for d in filtered_data if d['step'] >= start_step]
        
        if end_step is not None:
            filtered_data = [d for d in filtered_data if d['step'] <= end_step]
        
        # Extract metric
        metric_values = []
        for record in filtered_data:
            metrics = record['metrics']
            if metric_name in metrics:
                metric_values.append(metrics[metric_name])
            elif '.' in metric_name:
                # Handle nested metrics (e.g., 'neural_activity.active_neurons')
                keys = metric_name.split('.')
                value = metrics
                for key in keys:
                    if isinstance(value, dict) and key in value:
                        value = value[key]
                    else:
                        value = None
                        break
                metric_values.append(value)
        
        return metric_values
    
    def get_convergence_metrics(self, window_size: int = 100) -> Dict[str, Any]:
        """Get convergence metrics indicating stability."""
        if len(self.step_data) < window_size * 2:
            return {'converged': False, 'reason': 'Insufficient data'}
        
        # Get recent data
        recent_steps = self.step_data[-window_size:]
        previous_steps = self.step_data[-window_size*2:-window_size]
        
        # Calculate firing rate variance
        recent_firing_rates = [d['metrics'].get('average_firing_rate', 0) 
                              for d in recent_steps]
        previous_firing_rates = [d['metrics'].get('average_firing_rate', 0) 
                                for d in previous_steps]
        
        recent_variance = statistics.variance(recent_firing_rates) if len(recent_firing_rates) > 1 else 0
        previous_variance = statistics.variance(previous_firing_rates) if len(previous_firing_rates) > 1 else 0
        
        # Check for convergence
        converged = (recent_variance < previous_variance and 
                    recent_variance < 0.01 and  # Less than 1% firing rate variance
                    len(recent_firing_rates) > 50)
        
        return {
            'converged': converged,
            'recent_variance': recent_variance,
            'previous_variance': previous_variance,
            'stability_improvement': previous_variance - recent_variance,
            'mean_recent_firing_rate': statistics.mean(recent_firing_rates) if recent_firing_rates else 0,
            'stability_confidence': 'high' if converged else 'low'
        }
    
    def export_data(self, filepath: str, format: str = 'json'):
        """Export recorded data to file."""
        import json
        
        export_data = {
            'step_data': self.step_data,
            'neural_activity': self.neural_activity,
            'spike_history': self.spike_history,
            'region_stats': self.region_stats
        }
        
        if format.lower() == 'json':
            with open(filepath, 'w') as f:
                json.dump(export_data, f, indent=2, default=str)
        
        elif format.lower() == 'csv':
            # Simple CSV export for key metrics
            import csv
            with open(filepath, 'w', newline='') as f:
                writer = csv.writer(f)
                writer.writerow(['step', 'timestamp', 'total_spikes', 
                               'active_neurons', 'firing_rate', 'e_i_ratio'])
                
                for record in self.step_data:
                    metrics = record['metrics']
                    writer.writerow([
                        record['step'],
                        record['timestamp'],
                        metrics.get('total_spikes', 0),
                        metrics.get('active_neurons', 0),
                        metrics.get('average_firing_rate', 0),
                        metrics.get('e_i_ratio', 0)
                    ])
        
        return True
class PerformanceMonitor:
    """Monitor system performance and resource usage."""
    
    def __init__(self):
        self.step_times: List[float] = []
        self.memory_usage: List[float] = []
        self.cpu_usage: List[float] = []
        self.monitoring = False
        self.start_time = 0.0
        self.samples_per_second = 1.0
    
    def start_monitoring(self):
        """Start performance monitoring."""
        import psutil
        
        self.monitoring = True
        self.start_time = time.time()
        self.process = psutil.Process()
    
    def stop_monitoring(self):
        """Stop performance monitoring."""
        self.monitoring = False
    
    def record_step_time(self, step_time: float):
        """Record step execution time."""
        self.step_times.append(step_time)
    
    def _update_system_metrics(self):
        """Update system metrics if monitoring is enabled."""
        if not self.monitoring:
            return
        
        try:
            import psutil
            
            # Record memory usage
            memory_info = self.process.memory_info()
            self.memory_usage.append(memory_info.rss / (1024 * 1024))  # MB
            
            # Record CPU usage
            self.cpu_usage.append(self.process.cpu_percent())
        
        except ImportError:
            # psutil not available, skip system metrics
            pass
    
    def get_performance_report(self) -> Dict[str, Any]:
        """Get performance report."""
        if not self.step_times:
            return {}
        
        total_time = sum(self.step_times)
        
        report = {
            'steps': len(self.step_times),
            'total_time': total_time,
            'average_step_time': total_time / len(self.step_times),
            'min_step_time': min(self.step_times),
            'max_step_time': max(self.step_times),
            'steps_per_second': len(self.step_times) / total_time if total_time > 0 else 0
        }
        
        # Add system metrics if available
        if self.memory_usage:
            report['memory_usage'] = {
                'current_mb': self.memory_usage[-1],
                'average_mb': statistics.mean(self.memory_usage) if self.memory_usage else 0,
                'max_mb': max(self.memory_usage) if self.memory_usage else 0,
                'min_mb': min(self.memory_usage) if self.memory_usage else 0
            }
        
        if self.cpu_usage:
            report['cpu_usage'] = {
                'current_percent': self.cpu_usage[-1],
                'average_percent': statistics.mean(self.cpu_usage) if self.cpu_usage else 0,
                'max_percent': max(self.cpu_usage) if self.cpu_usage else 0
            }
        
        return report
    
    def get_efficiency_metrics(self, brain_stats: Dict[str, Any]) -> Dict[str, Any]:
        """Get efficiency metrics combining performance and neural activity."""
        if not self.step_times:
            return {}
        
        efficiency = {
            'steps_per_second': len(self.step_times) / sum(self.step_times),
            'spikes_per_step': brain_stats.get('total_spikes', 0) / len(self.step_times) if self.step_times else 0,
            'activity_efficiency': (brain_stats.get('active_neurons', 0) / 
                                  brain_stats.get('total_neurons', 1) if brain_stats.get('total_neurons') else 0)
        }
        
        return efficiency
class NeuralAnalyzer:
    """Analyze neural activity patterns and dynamics."""
    
    def __init__(self, sampling_rate: float = 1000.0):
        self.sampling_rate = sampling_rate
        self.neural_data: List[Dict[str, Any]] = []
        self.spike_patterns: List[List[int]] = []
        self.region_dynamics: Dict[str, List[float]] = {}
    
    def add_neural_data(self, step: int, neuron_activity: Dict[str, Any]):
        """Add neural activity data for analysis."""
        data_point = {
            'step': step,
            'timestamp': time.time(),
            'neuron_activity': neuron_activity,
            'firing_rate': neuron_activity.get('average_firing_rate', 0),
            'active_neurons': neuron_activity.get('active_neurons', 0),
            'total_spikes': neuron_activity.get('total_spikes', 0)
        }
        
        self.neural_data.append(data_point)
        
        # Extract spike pattern
        spike_pattern = []
        if 'spikes_this_step' in neuron_activity:
            spike_pattern.append(neuron_activity['spikes_this_step'])
        elif 'total_spikes' in neuron_activity:
            # Calculate spikes this step
            if len(self.neural_data) > 1:
                prev_spikes = self.neural_data[-2]['neuron_activity'].get('total_spikes', 0)
                current_spikes = neuron_activity['total_spikes']
                spike_pattern.append(current_spikes - prev_spikes)
        
        self.spike_patterns.append(spike_pattern)
        
        # Extract region dynamics
        if 'region_data' in neuron_activity:
            for region_id, region_data in neuron_activity['region_data'].items():
                if region_id not in self.region_dynamics:
                    self.region_dynamics[region_id] = []
                
                self.region_dynamics[region_id].append({
                    'firing_rate': region_data.get('firing_rate', 0),
                    'neuron_count': region_data.get('neuron_count', 0),
                    'synapse_count': region_data.get('synapse_count', 0),
                    'activity': region_data.get('activity', 0)
                })
    
    def detect_patterns(self) -> Dict[str, Any]:
        """Detect neural activity patterns."""
        if not self.neural_data:
            return {}
        
        patterns = {}
        
        # Detect bursting patterns
        patterns['burst_detection'] = self._detect_bursts()
        
        # Detect oscillations
        patterns['oscillation_detection'] = self._detect_oscillations()
        
        # Detect synchronization
        patterns['synchronization'] = self._detect_synchronization()
        
        # Detect criticality
        patterns['criticality'] = self._detect_criticality()
        
        # Detect transitions
        patterns['transitions'] = self._detect_transitions()
        
        return patterns
    
    def _detect_bursts(self) -> Dict[str, Any]:
        """Detect burst patterns in neural activity."""
        if not self.spike_patterns:
            return {'bursts_detected': 0, 'burst_rate': 0.0, 'burst_characteristics': {}}
        
        # Simple burst detection: consecutive high spike counts
        burst_threshold = statistics.mean(self.spike_patterns) + statistics.stdev(self.spike_patterns)
        burst_threshold = max(0, burst_threshold)  # Ensure non-negative
        
        bursts = []
        current_burst = []
        
        for i, spike_count in enumerate(self.spike_patterns):
            if spike_count > burst_threshold:
                current_burst.append(i)
            else:
                if len(current_burst) >= 3:  # Minimum burst length
                    bursts.append(current_burst)
                current_burst = []
        
        # Check final burst
        if len(current_burst) >= 3:
            bursts.append(current_burst)
        
        return {
            'bursts_detected': len(bursts),
            'burst_rate': len(bursts) / len(self.spike_patterns) if self.spike_patterns else 0,
            'burst_characteristics': {
                'average_burst_length': statistics.mean([len(burst) for burst in bursts]) if bursts else 0,
                'average_burst_intensity': statistics.mean([
                    statistics.mean([self.spike_patterns[i] for i in burst])
                    for burst in bursts
                ]) if bursts else 0
            }
        }
    
    def _detect_oscillations(self) -> Dict[str, Any]:
        """Detect oscillatory patterns."""
        if not self.neural_data:
            return {'oscillations_detected': 0, 'dominant_frequency': 0.0}
        
        # Extract firing rates over time
        firing_rates = [d['firing_rate'] for d in self.neural_data]
        
        # Simple Fourier transform for frequency analysis
        try:
            import numpy as np
            
            n = len(firing_rates)
            if n > 1:
                # Compute FFT
                fft_result = np.fft.fft(firing_rates - np.mean(firing_rates))
                freqs = np.fft.fftfreq(n, 1.0 / self.sampling_rate)
                
                # Get power spectrum
                power_spectrum = np.abs(fft_result) ** 2
                
                # Find dominant frequency (excluding DC component)
                positive_freqs = freqs[:n//2]
                positive_power = power_spectrum[:n//2]
                
                if len(positive_power) > 0 and np.max(positive_power) > 0:
                    dominant_idx = np.argmax(positive_power)
                    dominant_freq = positive_freqs[dominant_idx]
                    
                    # Check if dominant frequency indicates oscillation
                    is_oscillatory = (abs(dominant_freq) > 0.1 and 
                                    abs(dominant_freq) < self.sampling_rate / 2 and
                                    np.max(positive_power) > np.mean(positive_power) * 3)
                    
                    return {
                        'oscillations_detected': int(is_oscillatory),
                        'dominant_frequency': float(dominant_freq),
                        'power_ratio': float(np.max(positive_power) / np.mean(positive_power) if np.mean(positive_power) > 0 else 1)
                    }
        
        except ImportError:
            # numpy not available
            pass
        
        return {'oscillations_detected': 0, 'dominant_frequency': 0.0}
    
    def _detect_synchronization(self) -> Dict[str, Any]:
        """Detect synchronization between neural populations."""
        if not self.region_dynamics:
            return {'synchronization_detected': False, 'sync_strength': 0.0}
        
        regions = list(self.region_dynamics.keys())
        if len(regions) < 2:
            return {'synchronization_detected': False, 'sync_strength': 0.0}
        
        # Calculate cross-correlation between region activities
        sync_strengths = []
        
        for i in range(len(regions)):
            for j in range(i + 1, len(regions)):
                region1_data = self.region_dynamics[regions[i]]
                region2_data = self.region_dynamics[regions[j]]
                
                if region1_data and region2_data:
                    # Extract firing rates
                    rates1 = [d['firing_rate'] for d in region1_data]
                    rates2 = [d['firing_rate'] for d in region2_data]
                    
                    # Calculate cross-correlation at zero lag
                    if len(rates1) == len(rates2) and len(rates1) > 0:
                        correlation = np.corrcoef(rates1, rates2)[0, 1] if 'np' in globals() else 0.0
                        sync_strengths.append(abs(correlation))
        
        avg_sync_strength = statistics.mean(sync_strengths) if sync_strengths else 0.0
        
        return {
            'synchronization_detected': avg_sync_strength > 0.5,
            'sync_strength': avg_sync_strength,
            'synchronized_regions': len([s for s in sync_strengths if s > 0.5])
        }
    
    def _detect_criticality(self) -> Dict[str, Any]:
        """Detect if system is in critical state."""
        if not self.neural_data:
            return {'critical': False, 'order_parameter': 0.0, 'evidence': {}}
        
        # Extract neural activity metrics
        firing_rates = [d['firing_rate'] for d in self.neural_data]
        active_neurons = [d['active_neurons'] for d in self.neural_data]
        total_spikes = [d['total_spikes'] for d in self.neural_data]
        
        if not firing_rates:
            return {'critical': False, 'order_parameter': 0.0, 'evidence': {}}
        
        # Calculate order parameter (variance/mean ratio)
        firing_rate_mean = statistics.mean(firing_rates)
        firing_rate_var = statistics.variance(firing_rates) if len(firing_rates) > 1 else 0
        
        order_parameter = firing_rate_var / firing_rate_mean if firing_rate_mean > 0 else 0
        
        # Calculate avalanche statistics
        avalanche_stats = self._calculate_avalanche_statistics()
        
        # Criticality criteria
        critical_indicators = {
            'order_parameter': 0.1 <= order_parameter <= 1.0,  # Goldilocks zone
            'avalanche_size_distribution': avalanche_stats['power_law'],
            'avalanche_duration_distribution': avalanche_stats['power_law'],
            'activity_variance': firing_rate_var > firing_rate_mean  # Scale-invariant
        }
        
        # System is critical if multiple indicators are true
        critical_score = sum(critical_indicators.values()) / len(critical_indicators)
        is_critical = critical_score >= 0.6  # 60% of indicators must be true
        
        return {
            'critical': is_critical,
            'order_parameter': order_parameter,
            'evidence': critical_indicators,
            'criticality_score': critical_score
        }
    
    def _detect_transitions(self) -> Dict[str, Any]:n        """Detect state transitions in neural dynamics."""
        if len(self.neural_data) < 10:
            return {'transitions_detected': 0, 'transition_types': {}}
        
        transitions = {'state_changes': 0, 'regime_switches': 0, 'developmental_transitions': 0}
        transition_types = {}
        
        # Detect state changes based on firing rate
        firing_rates = [d['firing_rate'] for d in self.neural_data]
        
        for i in range(1, len(firing_rates)):
            rate_change = abs(firing_rates[i] - firing_rates[i-1])
            
            if rate_change > 0.1 * firing_rates[i-1]:  # 10% change threshold
                transitions['state_changes'] += 1
                
                # Classify transition type
                if firing_rates[i] > firing_rates[i-1]:
                    transition_type = 'activation'
                else:
                    transition_type = 'inactivation'
                
                transition_types[transition_type] = transition_types.get(transition_type, 0) + 1
        
        # Detect developmental transitions (based on stage changes)
        developmental_stages = [d.get('development_stage', 'Initial') for d in self.neural_data]
        unique_stages = set(developmental_stages)
        
        if len(unique_stages) > 1:
            transitions['developmental_transitions'] = len(unique_stages) - 1
            transition_types['developmental'] = transitions['developmental_transitions']
        
        return {
            'transitions_detected': transitions['state_changes'] + transitions['regime_switches'] + transitions['developmental_transitions'],
            'transition_types': transition_types
        }
    
    def _calculate_avalanche_statistics(self) -> Dict[str, Any]:
        """Calculate avalanche statistics for criticality detection."""
        if not self.spike_patterns:
            return {'power_law': False, 'exponent': 0.0, 'size_distribution': [], 'duration_distribution': []}
        
        # This is a simplified avalanche detection
        # In practice, you'd need more sophisticated detection based on spike timing
        
        # For now, return placeholder values
        return {
            'power_law': False,  # Would need proper analysis
            'exponent': 2.0,     # Theoretical critical exponent
            'size_distribution': [],
            'duration_distribution': []
        }
    
    def get_analysis_summary(self) -> Dict[str, Any]:
        """Get comprehensive analysis summary."""
        if not self.neural_data:
            return {'analysis_complete': False}
        
        patterns = self.detect_patterns()
        
        summary = {
            'analysis_complete': True,
            'total_data_points': len(self.neural_data),
            'time_span': (self.neural_data[-1]['timestamp'] - self.neural_data[0]['timestamp']) 
                        if len(self.neural_data) > 1 else 0,
            'activity_range': {
                'min_firing_rate': min(d['firing_rate'] for d in self.neural_data),
                'max_firing_rate': max(d['firing_rate'] for d in self.neural_data),
                'mean_firing_rate': statistics.mean(d['firing_rate'] for d in self.neural_data)
            },
            'patterns': patterns,
            'key_findings': self._extract_key_findings(patterns)
        }
        
        return summary
    
    def _extract_key_findings(self, patterns: Dict[str, Any]) -> List[str]:
        """Extract key findings from pattern analysis."""
        findings = []
        
        if patterns.get('burst_detection', {}).get('bursts_detected', 0) > 0:
            findings.append(f"Detected {patterns['burst_detection']['bursts_detected']} burst patterns")
        
        if patterns.get('oscillation_detection', {}).get('oscillations_detected', 0):
            findings.append(f"Detected oscillatory activity at {patterns['oscillation_detection']['dominant_frequency']:.2f} Hz")
        
        if patterns.get('synchronization', {}).get('synchronization_detected'):
            findings.append(f"Neural populations are synchronized (strength: {patterns['synchronization']['sync_strength']:.3f})")
        
        if patterns.get('criticality', {}).get('critical'):
            findings.append("System appears to be in critical state")
        
        return findings