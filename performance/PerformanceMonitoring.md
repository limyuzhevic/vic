# NLM Performance Monitoring and Profiling
# Advanced performance analysis tools for NLM simulations

## Overview

Performance monitoring and profiling are essential for understanding, optimizing, and validating NLM brain simulations. This document provides comprehensive tools for monitoring simulation performance, identifying bottlenecks, analyzing resource usage, and generating performance reports.

## Performance Monitoring Overview

NLM performance monitoring encompasses several key areas:

1. **Real-time Monitoring**: Track simulation metrics during execution
2. **Performance Analysis**: Analyze collected data to identify trends and issues
3. **Bottleneck Detection**: Identify performance bottlenecks and optimization opportunities
4. **Resource Usage**: Monitor CPU, memory, and I/O usage
5. **Comparison Analysis**: Compare performance across different configurations
6. **Profiling Tools**: Detailed profiling of specific operations and components

## Performance Monitoring System Architecture

The NLM performance monitoring system is designed to be modular and extensible:

```
+-----------------------------------+
| PerformanceMonitor (Main Class)    |
+-----------------------------------+
         |
+-----------------------------------+
| StepMonitor                       |
+-----------------------------------+
         |
+-----------------------------------+
| BrainMonitor                      |
+-----------------------------------+
         |
+-----------------------------------+
| MemoryMonitor                     |
+-----------------------------------+
         |
+-----------------------------------+
| CPUMonitor                        |
+-----------------------------------+
         |
+-----------------------------------+
| IOMonitor                         |
+-----------------------------------+
```

## Core Performance Monitoring Classes

### 1. PerformanceMonitor

```python
import time
import psutil
import pandas as pd
import matplotlib.pyplot as plt
from typing import Dict, List, Any, Optional, Callable
from datetime import datetime
from dataclasses import dataclass
from enum import Enum
import json
import threading
from concurrent.futures import ThreadPoolExecutor
import numpy as np

class PerformanceMetric(Enum):
    STEP_TIME = "step_time"
    SPIKE_COUNT = "spike_count"
    FIRING_RATE = "firing_rate"
    NEURON_COUNT = "neuron_count"
    SYNAPSE_COUNT = "synapse_count"
    CPU_USAGE = "cpu_usage"
    MEMORY_USAGE = "memory_usage"
    I_O_BANDWIDTH = "io_bandwidth"
    PLASTICITY_RATE = "plasticity_rate"
    DEVELOPMENT_PROGRESS = "development_progress"
    LEARNING_EFFICIENCY = "learning_efficiency"

@dataclass
class PerformanceDataPoint:
    """Represents a single data point in the performance log"""
    timestamp: datetime
    step: int
    simulation_time: float
    metrics: Dict[PerformanceMetric, float]
    brain_state: Dict[str, Any]
    thread_id: int
    process_id: int

class PerformanceMonitor:
    """Main performance monitoring class for NLM simulations"""
    
    def __init__(self, 
                 sampling_rate: int = 10,
                 enable_cpu_monitoring: bool = True,
                 enable_memory_monitoring: bool = True,
                 enable_io_monitoring: bool = False,
                 max_history_points: int = 10000,
                 alert_thresholds: Optional[Dict[PerformanceMetric, float]] = None):
        """
        Initialize performance monitor
        
        Args:
            sampling_rate: Number of steps between performance measurements
            enable_cpu_monitoring: Enable CPU usage monitoring
            enable_memory_monitoring: Enable memory usage monitoring
            enable_io_monitoring: Enable I/O usage monitoring
            max_history_points: Maximum number of data points to store
            alert_thresholds: Alert thresholds for performance metrics
        """
        self.sampling_rate = sampling_rate
        self.enable_cpu_monitoring = enable_cpu_monitoring
        self.enable_memory_monitoring = enable_memory_monitoring
        self.enable_io_monitoring = enable_io_monitoring
        self.max_history_points = max_history_points
        
        self.performance_data: List[PerformanceDataPoint] = []
        self.monitoring_active = False
        self.current_step = 0
        self.simulation_start_time = None
        self.thread_id = threading.current_thread().ident
        self.process_id = psutil.Process().pid
        
        # Initialize alert thresholds
        self.alert_thresholds = alert_thresholds or {}
        self.alert_callbacks: List[Callable[[PerformanceMetric, float, float], None]] = []
        
        # System resources
        self.cpu_samples: List[float] = []
        self.memory_samples: List[float] = []
        self.io_samples: List[float] = []
        
    def start_monitoring(self, brain):
        """Start performance monitoring for a simulation
        
        Args:
            brain: NLM Brain object to monitor
        """
        if self.monitoring_active:
            raise RuntimeError("Performance monitoring already active")
        
        self.monitoring_active = True
        self.current_step = 0
        self.simulation_start_time = datetime.now()
        self.performance_data = []
        
        # Reset system samples
        self.cpu_samples = []
        self.memory_samples = []
        self.io_samples = []
        
        print(f"Performance monitoring started for simulation")
        print(f"Monitoring will sample every {self.sampling_rate} steps")
    
    def stop_monitoring(self):
        """Stop performance monitoring"""
        if not self.monitoring_active:
            return
        
        self.monitoring_active = False
        
        # Generate final analysis
        self._finalize_data()
        
        print(f"Performance monitoring stopped")
        print(f"Collected {len(self.performance_data)} performance data points")
    
    def record_step_performance(self, brain, step_time):
        """Record performance metrics for current step
        
        Args:
            brain: NLM Brain object
            step_time: Time taken for current step (seconds)
        """
        self.current_step += 1
        
        # Collect system metrics
        cpu_usage = self._get_cpu_usage()
        memory_usage = self._get_memory_usage()
        io_bandwidth = self._get_io_bandwidth() if self.enable_io_monitoring else 0.0
        
        # Store system samples
        self.cpu_samples.append(cpu_usage)
        self.memory_samples.append(memory_usage)
        self.io_samples.append(io_bandwidth)
        
        # Get brain state metrics
        brain_metrics = self._get_brain_metrics(brain)
        
        # Create performance data point
        data_point = PerformanceDataPoint(
            timestamp=datetime.now(),
            step=self.current_step,
            simulation_time=self.current_step * 0.001,  # Approximate
            metrics={
                PerformanceMetric.STEP_TIME: step_time,
                PerformanceMetric.CPU_USAGE: cpu_usage,
                PerformanceMetric.MEMORY_USAGE: memory_usage,
                PerformanceMetric.I_O_BANDWIDTH: io_bandwidth,
                **brain_metrics
            },
            brain_state={
                'neurons': brain.getTotalNeuronCount(),
                'synapses': brain.getTotalSynapseCount(),
                'spikes': brain.getTotalSpikeCount(),
                'firing_rate': brain.getAverageFiringRate(),
                'e_i_ratio': brain.getExcitationInhibitionRatio(),
                'development_stage': brain.getDevelopmentalStage(),
                'active_neurons': brain.getActiveNeuronCount(),
                'firing_neurons': brain.getFiringNeuronCount()
            },
            thread_id=self.thread_id,
            process_id=self.process_id
        )
        
        # Add to performance data
        self.performance_data.append(data_point)
        
        # Check alerts
        self._check_alerts(brain_metrics)
        
        # Clean up old data if exceeding maximum
        if len(self.performance_data) > self.max_history_points:
            self.performance_data = self.performance_data[-self.max_history_points:]
    
    def _get_cpu_usage(self) -> float:
        """Get current CPU usage percentage"""
        return psutil.cpu_percent(interval=0.01)
    
    def _get_memory_usage(self) -> float:
        """Get current memory usage percentage"""
        process = psutil.Process(self.process_id)
        return process.memory_percent()
    
    def _get_io_bandwidth(self) -> float:
        """Get current I/O bandwidth usage (MB/s)"""
        try:
            net_io = psutil.net_io_counters()
            return (net_io.bytes_sent + net_io.bytes_recv) / (1024 * 1024)  # MB/s
        except:
            return 0.0
    
    def _get_brain_metrics(self, brain) -> Dict[PerformanceMetric, float]:
        """Get brain-specific performance metrics"""
        metrics = {}
        
        # Basic brain metrics
        metrics[PerformanceMetric.NEURON_COUNT] = brain.getTotalNeuronCount()
        metrics[PerformanceMetric.SYNAPSE_COUNT] = brain.getTotalSynapseCount()
        metrics[PerformanceMetric.SPIKE_COUNT] = brain.getTotalSpikeCount()
        metrics[PerformanceMetric.FIRING_RATE] = brain.getAverageFiringRate()
        metrics[PerformanceMetric.EXCITATION_INHIBITION_RATIO] = brain.getExcitationInhibitionRatio()
        
        # Plasticity metrics
        stdp = brain.getSTDP()
        if stdp:
            metrics[PerformanceMetric.PLASTICITY_RATE] = stdp.getLearningRate()
        
        # Development metrics
        development = brain.getDevelopmentSystem()
        if development:
            stage = brain.getDevelopmentalStage()
            metrics[PerformanceMetric.DEVELOPMENT_PROGRESS] = self._get_development_stage_value(stage)
            metrics[PerformanceMetric.LEARNING_EFFICIENCY] = development.getLearningEfficiency()
        
        return metrics
    
    def _get_development_stage_value(self, stage) -> float:
        """Convert developmental stage to numeric value"""
        stage_values = {
            'Initial': 0.0,
            'CriticalPeriod': 0.3,
            'Maturation': 0.6,
            'Adult': 0.8,
            'Aging': 1.0
        }
        return stage_values.get(str(stage), 0.0)
    
    def _check_alerts(self, brain_metrics: Dict[PerformanceMetric, float]):
        """Check for performance alerts"""
        for metric, threshold in self.alert_thresholds.items():
            if metric in brain_metrics:
                value = brain_metrics[metric]
                if value > threshold:
                    self._trigger_alert(metric, value, threshold)
    
    def _trigger_alert(self, metric: PerformanceMetric, value: float, threshold: float):
        """Trigger an alert for a performance metric"""
        alert_message = f"Performance Alert: {metric.value} = {value:.2f} (threshold: {threshold:.2f})"
        print(f"⚠️  {alert_message}")
        
        # Call alert callbacks
        for callback in self.alert_callbacks:
            callback(metric, value, threshold)
    
    def add_alert_callback(self, callback: Callable[[PerformanceMetric, float, float], None]):
        """Add a callback function for performance alerts"""
        self.alert_callbacks.append(callback)
    
    def _finalize_data(self):
        """Finalize performance data collection"""
        # Calculate summary statistics
        if self.performance_data:
            print(f"\nPerformance Summary:")
            print(f"  Total steps simulated: {self.current_step}")
            print(f"  Total duration: {datetime.now() - self.simulation_start_time}")
            print(f"  Average step time: {self._get_average_step_time():.4f} seconds")
            print(f"  Total spikes: {self.performance_data[-1].brain_state['spikes']}")
            print(f"  Average firing rate: {self._get_average_firing_rate():.2f}")
    
    def _get_average_step_time(self) -> float:
        """Calculate average step time from performance data"""
        if not self.performance_data:
            return 0.0
        
        step_times = [dp.metrics[PerformanceMetric.STEP_TIME] for dp in self.performance_data]
        return sum(step_times) / len(step_times)
    
    def _get_average_firing_rate(self) -> float:
        """Calculate average firing rate from performance data"""
        if not self.performance_data:
            return 0.0
        
        firing_rates = [dp.metrics[PerformanceMetric.FIRING_RATE] for dp in self.performance_data]
        return sum(firing_rates) / len(firing_rates)
    
    def analyze_performance(self) -> Dict[str, Any]:
        """Analyze collected performance data"""
        if not self.performance_data:
            return {'error': 'No performance data collected'}
        
        # Convert to pandas DataFrame for analysis
        df = self._to_dataframe()
        
        analysis = {
            'total_steps': len(self.performance_data),
            'total_spikes': self.performance_data[-1].brain_state['spikes'] - self.performance_data[0].brain_state['spikes'],
            'duration': (self.performance_data[-1].timestamp - self.performance_data[0].timestamp).total_seconds(),
            'performance_metrics': self._analyze_metrics(df),
            'brain_dynamics': self._analyze_brain_dynamics(df),
            'system_resources': self._analyze_system_resources(df),
            'bottlenecks': self._identify_bottlenecks(df),
            'recommendations': self._generate_recommendations(df)
        }
        
        return analysis
    
    def _to_dataframe(self) -> pd.DataFrame:
        """Convert performance data to pandas DataFrame"""
        data = []
        for dp in self.performance_data:
            row = {
                'timestamp': dp.timestamp,
                'step': dp.step,
                'simulation_time': dp.simulation_time,
                'step_time': dp.metrics[PerformanceMetric.STEP_TIME],
                'cpu_usage': dp.metrics.get(PerformanceMetric.CPU_USAGE, 0),
                'memory_usage': dp.metrics.get(PerformanceMetric.MEMORY_USAGE, 0),
                'neuron_count': dp.brain_state['neurons'],
                'synapse_count': dp.brain_state['synapses'],
                'spike_count': dp.brain_state['spikes'],
                'firing_rate': dp.brain_state['firing_rate'],
                'e_i_ratio': dp.brain_state['e_i_ratio'],
                'development_stage': dp.brain_state['development_stage'],
                'active_neurons': dp.brain_state['active_neurons'],
                'firing_neurons': dp.brain_state['firing_neurons']
            }
            data.append(row)
        
        return pd.DataFrame(data)
    
    def _analyze_metrics(self, df: pd.DataFrame) -> Dict[str, Any]:
        """Analyze performance metrics"""
        return {
            'step_time': {
                'mean': df['step_time'].mean(),
                'median': df['step_time'].median(),
                'std': df['step_time'].std(),
                'min': df['step_time'].min(),
                'max': df['step_time'].max(),
                'trend': self._calculate_trend(df['step_time'])
            },
            'firing_rate': {
                'mean': df['firing_rate'].mean(),
                'median': df['firing_rate'].median(),
                'std': df['firing_rate'].std(),
                'min': df['firing_rate'].min(),
                'max': df['firing_rate'].max(),
                'trend': self._calculate_trend(df['firing_rate'])
            },
            'neuron_growth': {
                'rate': (df['neuron_count'].iloc[-1] - df['neuron_count'].iloc[0]) / len(df),
                'efficiency': df['spike_count'].iloc[-1] / max(df['neuron_count'].iloc[-1], 1)
            }
        }
    
    def _analyze_brain_dynamics(self, df: pd.DataFrame) -> Dict[str, Any]:
        """Analyze brain dynamics and learning"""
        return {
            'plasticity_effectiveness': self._calculate_plasticity_effectiveness(df),
            'development_progress': self._calculate_development_progress(df),
            'stability': self._calculate_stability(df),
            'learning_efficiency': self._calculate_learning_efficiency(df)
        }
    
    def _analyze_system_resources(self, df: pd.DataFrame) -> Dict[str, Any]:
        """Analyze system resource usage"""
        return {
            'cpu_usage': {
                'mean': df['cpu_usage'].mean(),
                'peak': df['cpu_usage'].max(),
                'utilization_efficiency': 100.0 * df['cpu_usage'].mean() / 100.0
            },
            'memory_usage': {
                'mean': df['memory_usage'].mean(),
                'peak': df['memory_usage'].max(),
                'efficiency': 1.0 - (df['memory_usage'].std() / df['memory_usage'].mean())
            }
        }
    
    def _identify_bottlenecks(self, df: pd.DataFrame) -> List[Dict[str, Any]]:
        """Identify performance bottlenecks"""
        bottlenecks = []
        
        # Step time bottlenecks
        step_time_q75 = df['step_time'].quantile(0.75)
        slow_steps = df[df['step_time'] > step_time_q75]
        if len(slow_steps) > len(df) * 0.1:  # More than 10% of steps are slow
            bottlenecks.append({
                'type': 'step_time_bottleneck',
                'severity': 'high',
                'description': f"25% of steps take longer than {step_time_q75:.4f}s",
                'affected_steps': len(slow_steps)
            })
        
        # Memory bottlenecks
        memory_q75 = df['memory_usage'].quantile(0.75)
        memory_bursts = df[df['memory_usage'] > memory_q75 * 2]
        if len(memory_bursts) > 0:
            bottlenecks.append({
                'type': 'memory_bottleneck',
                'severity': 'medium',
                'description': f"Memory usage peaks at {df['memory_usage'].max():.1f}%",
                'affected_steps': len(memory_bursts)
            })
        
        # Firing rate bottlenecks
        firing_rate_q90 = df['firing_rate'].quantile(0.9)
        high_firing_steps = df[df['firing_rate'] > firing_rate_q90]
        if len(high_firing_steps) > len(df) * 0.05:  # More than 5% have high firing rate
            bottlenecks.append({
                'type': 'high_activity_bottleneck',
                'severity': 'low',
                'description': f"{len(high_firing_steps)} steps have firing rate > {firing_rate_q90:.1f}",
                'affected_steps': len(high_firing_steps)
            })
        
        return bottlenecks
    
    def _generate_recommendations(self, df: pd.DataFrame) -> List[str]:
        """Generate performance improvement recommendations"""
        recommendations = []
        
        # Check for common issues
        avg_step_time = df['step_time'].mean()
        if avg_step_time > 0.1:  # More than 100ms per step
            recommendations.append(
                f"Consider reducing neuron count or improving algorithm efficiency - "
                f"current average step time: {avg_step_time:.4f}s"
            )
        
        if df['cpu_usage'].mean() > 80:
            recommendations.append(
                "High CPU usage detected. Consider optimizing computation or "
                "running on more powerful hardware."
            )
        
        if df['memory_usage'].max() > 90:
            recommendations.append(
                "High memory usage detected. Consider reducing memory footprint "
                "or increasing available system memory."
            )
        
        firing_rate_trend = self._calculate_trend(df['firing_rate'])
        if firing_rate_trend > 0.01:  # Increasing trend
            recommendations.append(
                "Firing rate is increasing over time, which may indicate "
                "unstable dynamics. Consider adjusting learning parameters."
            )
        
        if not recommendations:
            recommendations.append(
                "Performance looks good! Continue monitoring for optimal results."
            )
        
        return recommendations
    
    def _calculate_trend(self, series: pd.Series) -> float:
        """Calculate linear trend of a time series"""
        if len(series) < 2:
            return 0.0
        
        x = np.arange(len(series))
        y = series.values
        
        # Linear regression
        slope = np.polyfit(x, y, 1)[0]
        return slope / series.mean() if series.mean() > 0 else 0.0
    
    def _calculate_plasticity_effectiveness(self, df: pd.DataFrame) -> float:
        """Calculate plasticity effectiveness based on weight changes"""
        # This would require access to brain internal state
        # For now, use a proxy based on firing rate changes
        if len(df) < 10:
            return 0.0
        
        # Calculate coefficient of variation of firing rates
        cv = df['firing_rate'].std() / df['firing_rate'].mean()
        return min(cv, 1.0)  # Cap at 1.0
    
    def _calculate_development_progress(self, df: pd.DataFrame) -> float:
        """Calculate development progress over time"""
        if len(df) < 2:
            return 0.0
        
        # Track development stage progression
        stage_values = []
        for _, row in df.iterrows():
            stage = row['development_stage']
            stage_values.append(self._get_development_stage_value(stage))
        
        return (stage_values[-1] - stage_values[0]) / (len(stage_values) - 1) if len(stage_values) > 1 else 0.0
    
    def _calculate_stability(self, df: pd.DataFrame) -> float:
        """Calculate system stability based on metric volatility"""
        # Lower volatility indicates better stability
        firing_rate_volatility = df['firing_rate'].std() / df['firing_rate'].mean()
        e_i_ratio_volatility = df['e_i_ratio'].std() / df['e_i_ratio'].mean()
        
        stability = 1.0 / (1.0 + firing_rate_volatility + e_i_ratio_volatility)
        return max(0.0, min(stability, 1.0))
    
    def _calculate_learning_efficiency(self, df: pd.DataFrame) -> float:
        """Calculate learning efficiency based on performance improvements"""
        if len(df) < 10:
            return 0.0
        
        # Calculate learning based on spike generation efficiency
        early_spikes = df['spike_count'].iloc[:len(df)//4].sum()
        late_spikes = df['spike_count'].iloc[-len(df)//4:].sum()
        
        learning_efficiency = (late_spikes - early_spikes) / early_spikes if early_spikes > 0 else 0.0
        return min(learning_efficiency, 1.0)
    
    def export_performance_data(self, filepath: str, format: str = 'csv'):
        """Export performance data to file"""
        if not self.performance_data:
            raise ValueError("No performance data to export")
        
        df = self._to_dataframe()
        
        if format.lower() == 'csv':
            df.to_csv(filepath, index=False)
        elif format.lower() == 'json':
            df.to_json(filepath, orient='records')
        elif format.lower() == 'excel':
            df.to_excel(filepath, index=False)
        else:
            raise ValueError(f"Unsupported format: {format}")
        
        print(f"Performance data exported to: {filepath}")
    
    def generate_performance_report(self, output_file: Optional[str] = None) -> str:
        """Generate a comprehensive performance report"""
        if not self.performance_data:
            return "No performance data available"
        
        analysis = self.analyze_performance()
        
        report = f"NLM Performance Report\n"
        report += f"Generated at: {datetime.now().isoformat()}\n"
        report += f"Total steps: {analysis['total_steps']}\n"
        report += f"Total spikes: {analysis['total_spikes']}\n"
        report += f"Duration: {analysis['duration']:.2f} seconds\n\n"
        
        report += f"Performance Metrics:\n"
        for metric, stats in analysis['performance_metrics'].items():
            report += f"  {metric}:\n"
            for stat, value in stats.items():
                if isinstance(value, float):
                    report += f"    {stat}: {value:.4f}\n"
                else:
                    report += f"    {stat}: {value}\n"
        
        report += f"\nBrain Dynamics:\n"
        for metric, value in analysis['brain_dynamics'].items():
            report += f"  {metric}: {value:.4f}\n"
        
        report += f"\nSystem Resources:\n"
        for category, metrics in analysis['system_resources'].items():
            report += f"  {category}:\n"
            for metric, value in metrics.items():
                if isinstance(value, float):
                    report += f"    {metric}: {value:.4f}\n"
                else:
                    report += f"    {metric}: {value}\n"
        
        report += f"\nPerformance Bottlenecks:\n"
        for bottleneck in analysis['bottlenecks']:
            report += f"  - [{bottleneck['severity'].upper()}] {bottleneck['description']}\n"
        
        report += f"\nRecommendations:\n"
        for i, recommendation in enumerate(analysis['recommendations'], 1):
            report += f"  {i}. {recommendation}\n"
        
        if output_file:
            with open(output_file, 'w') as f:
                f.write(report)
            print(f"Performance report saved to: {output_file}")
        
        return report
```

### 2. BrainProfiler

```python
class BrainProfiler:
    """Detailed profiler for brain operations and components"""
    
    def __init__(self, brain):
        self.brain = brain
        self.profiling_data = {
            'neuron_operations': [],
            'synapse_operations': [],
            'plasticity_events': [],
            'memory_operations': [],
            'development_events': []
        }
        self.enable_detailed_profiling = True
    
    def profile_step(self, step_id: int, start_time: float, end_time: float):
        """Profile a single simulation step"""
        step_profile = {
            'step_id': step_id,
            'start_time': start_time,
            'end_time': end_time,
            'duration': end_time - start_time,
            'neuron_count': self.brain.getTotalNeuronCount(),
            'synapse_count': self.brain.getTotalSynapseCount(),
            'spike_count': self.brain.getTotalSpikeCount(),
            'firing_rate': self.brain.getAverageFiringRate(),
            'e_i_ratio': self.brain.getExcitationInhibitionRatio()
        }
        
        self.profiling_data['neuron_operations'].append({
            'step': step_id,
            'operation': 'update_states',
            'duration': end_time - start_time * 0.3,
            'neurons_processed': self.brain.getTotalNeuronCount()
        })
        
        self.profiling_data['synapse_operations'].append({
            'step': step_id,
            'operation': 'propagate_spikes',
            'duration': end_time - start_time * 0.4,
            'synapses_activated': self.brain.getTotalSynapseCount() * 0.1  # Estimate
        })
        
        return step_profile
    
    def profile_plasticity(self, step_id: int, plasticity_type: str):
        """Profile plasticity events"""
        event = {
            'step': step_id,
            'type': plasticity_type,
            'timestamp': datetime.now(),
            'brain_state': {
                'neuron_count': self.brain.getTotalNeuronCount(),
                'synapse_count': self.brain.getTotalSynapseCount(),
                'learning_rate': self._get_current_learning_rate()
            }
        }
        
        self.profiling_data['plasticity_events'].append(event)
    
    def profile_memory(self, step_id: int, memory_type: str):
        """Profile memory operations"""
        event = {
            'step': step_id,
            'type': memory_type,
            'timestamp': datetime.now(),
            'brain_state': {
                'working_memory': len(self.brain.getWorkingMemory().getActiveMemories()),
                'episodic_memory': len(self.brain.getEpisodicMemory().getEpisodes()),
                'total_spikes': self.brain.getTotalSpikeCount()
            }
        }
        
        self.profiling_data['memory_operations'].append(event)
    
    def profile_development(self, step_id: int):
        """Profile development events"""
        event = {
            'step': step_id,
            'type': 'development',
            'timestamp': datetime.now(),
            'brain_state': {
                'development_stage': self.brain.getDevelopmentalStage(),
                'age': self.brain.getDevelopmentSystem().getDevelopmentalAge(),
                'neuron_count': self.brain.getTotalNeuronCount(),
                'plasticity_efficiency': self._get_plasticity_efficiency()
            }
        }
        
        self.profiling_data['development_events'].append(event)
    
    def _get_current_learning_rate(self) -> float:
        """Get current learning rate from brain"""
        # This would require accessing brain internal state
        return 0.001  # Placeholder
    
    def _get_plasticity_efficiency(self) -> float:
        """Get current plasticity efficiency"""
        # This would require accessing brain internal state
        return 0.5  # Placeholder
    
    def analyze_profiling_data(self) -> Dict[str, Any]:
        """Analyze profiling data and generate insights"""
        analysis = {}
        
        # Analyze neuron operations
        if self.profiling_data['neuron_operations']:
            analysis['neuron_operations'] = {
                'total_steps': len(self.profiling_data['neuron_operations']),
                'average_duration': np.mean([op['duration'] for op in self.profiling_data['neuron_operations']]),
                'total_neurons_processed': sum(op['neurons_processed'] for op in self.profiling_data['neuron_operations']),
                'efficiency': self.profiling_data['neuron_operations'][-1]['neurons_processed'] / self.profiling_data['neuron_operations'][0]['duration']
            }
        
        # Analyze synapse operations
        if self.profiling_data['synapse_operations']:
            analysis['synapse_operations'] = {
                'total_steps': len(self.profiling_data['synapse_operations']),
                'average_duration': np.mean([op['duration'] for op in self.profiling_data['synapse_operations']]),
                'total_synapses_activated': sum(op['synapses_activated'] for op in self.profiling_data['synapse_operations']),
                'activation_rate': sum(op['synapses_activated'] for op in self.profiling_data['synapse_operations']) / (len(self.profiling_data['synapse_operations']) * self.brain.getTotalSynapseCount())
            }
        
        # Analyze plasticity events
        if self.profiling_data['plasticity_events']:
            analysis['plasticity_events'] = {
                'total_events': len(self.profiling_data['plasticity_events']),
                'event_types': self._analyze_event_types(self.profiling_data['plasticity_events']),
                'average_learning_rate': np.mean([event['brain_state']['learning_rate'] for event in self.profiling_data['plasticity_events']])
            }
        
        return analysis
    
    def _analyze_event_types(self, events: List[Dict]) -> Dict[str, int]:
        """Analyze types of plasticity events"""
        type_counts = {}
        for event in events:
            event_type = event['type']
            type_counts[event_type] = type_counts.get(event_type, 0) + 1
        return type_counts
    
    def export_profiling_data(self, filepath: str, format: str = 'json'):
        """Export profiling data to file"""
        with open(filepath, 'w') as f:
            if format.lower() == 'json':
                json.dump(self.profiling_data, f, indent=2, default=str)
            else:
                raise ValueError(f"Unsupported format: {format}")
        
        print(f"Profiling data exported to: {filepath}")
```

### 3. ResourceMonitor

```python
class ResourceMonitor:
    """Monitor system resources during simulation"""
    
    def __init__(self, sampling_interval: float = 1.0):
        self.sampling_interval = sampling_interval
        self.resource_data = []
        self.monitoring = False
        self.thread = None
        
    def start_monitoring(self):
        """Start monitoring system resources"""
        if self.monitoring:
            return
        
        self.monitoring = True
        self.thread = threading.Thread(target=self._monitoring_loop)
        self.thread.daemon = True
        self.thread.start()
        
        print("Resource monitoring started")
    
    def stop_monitoring(self):
        """Stop monitoring system resources"""
        self.monitoring = False
        if self.thread:
            self.thread.join(timeout=5.0)
        print("Resource monitoring stopped")
    
    def _monitoring_loop(self):
        """Main monitoring loop"""
        while self.monitoring:
            timestamp = datetime.now()
            
            # Get system metrics
            cpu_percent = psutil.cpu_percent(interval=0)
            memory_info = psutil.virtual_memory()
            disk_info = psutil.disk_usage('/')
            
            # Get process metrics
            process = psutil.Process()
            process_memory = process.memory_info()
            process_cpu = process.cpu_percent()
            
            # Network metrics
            network_io = psutil.net_io_counters()
            
            resource_sample = {
                'timestamp': timestamp,
                'system': {
                    'cpu_percent': cpu_percent,
                    'memory_percent': memory_info.percent,
                    'memory_total': memory_info.total,
                    'memory_available': memory_info.available,
                    'disk_percent': disk_info.percent,
                    'disk_total': disk_info.total,
                    'disk_free': disk_info.free
                },
                'process': {
                    'cpu_percent': process_cpu,
                    'memory_rss': process_memory.rss,
                    'memory_vms': process_memory.vms,
                    'memory_shared': process_memory.shared,
                    'num_threads': process.num_threads(),
                    'num_fds': process.num_fds()
                },
                'network': {
                    'bytes_sent': network_io.bytes_sent,
                    'bytes_recv': network_io.bytes_recv,
                    'packets_sent': network_io.packets_sent,
                    'packets_recv': network_io.packets_recv,
                    'connections': len(psutil.net_connections())
                }
            }
            
            self.resource_data.append(resource_sample)
            time.sleep(self.sampling_interval)
    
    def analyze_resource_usage(self) -> Dict[str, Any]:
        """Analyze resource usage data"""
        if not self.resource_data:
            return {}
        
        df = pd.DataFrame(self.resource_data)
        
        analysis = {
            'system_resources': {
                'cpu': {
                    'average': df['system'].apply(lambda x: x['cpu_percent']).mean(),
                    'peak': df['system'].apply(lambda x: x['cpu_percent']).max(),
                    'utilization_efficiency': 100.0 * df['system'].apply(lambda x: x['cpu_percent']).mean() / 100.0
                },
                'memory': {
                    'average': df['system'].apply(lambda x: x['memory_percent']).mean(),
                    'peak': df['system'].apply(lambda x: x['memory_percent']).max(),
                    'efficiency': 1.0 - (df['system'].apply(lambda x: x['memory_percent']).std() / df['system'].apply(lambda x: x['memory_percent']).mean())
                },
                'disk': {
                    'average': df['system'].apply(lambda x: x['disk_percent']).mean(),
                    'peak': df['system'].apply(lambda x: x['disk_percent']).max()
                }
            },
            'process_resources': {
                'cpu': {
                    'average': df['process'].apply(lambda x: x['cpu_percent']).mean(),
                    'peak': df['process'].apply(lambda x: x['cpu_percent']).max()
                },
                'memory': {
                    'rss_average': df['process'].apply(lambda x: x['memory_rss']).mean() / (1024 * 1024),  # MB
                    'vms_average': df['process'].apply(lambda x: x['memory_vms']).mean() / (1024 * 1024),  # MB
                    'peak_rss': df['process'].apply(lambda x: x['memory_rss']).max() / (1024 * 1024)  # MB
                },
                'threads': {
                    'average': df['process'].apply(lambda x: x['num_threads']).mean(),
                    'peak': df['process'].apply(lambda x: x['num_threads']).max()
                }
            },
            'network_resources': {
                'bandwidth': {
                    'average_send': df['network'].apply(lambda x: x['bytes_sent']).mean(),
                    'average_recv': df['network'].apply(lambda x: x['bytes_recv']).mean(),
                    'peak_send': df['network'].apply(lambda x: x['bytes_sent']).max(),
                    'peak_recv': df['network'].apply(lambda x: x['bytes_recv']).max()
                }
            }
        }
        
        return analysis
    
    def detect_resource_issues(self) -> List[Dict[str, Any]]:
        """Detect resource-related issues"""
        issues = []
        
        if not self.resource_data:
            return issues
        
        df = pd.DataFrame(self.resource_data)
        
        # Check for CPU bottlenecks
        avg_cpu = df['system'].apply(lambda x: x['cpu_percent']).mean()
        if avg_cpu > 80:
            issues.append({
                'type': 'cpu_bottleneck',
                'severity': 'high' if avg_cpu > 90 else 'medium',
                'description': f"Average CPU usage: {avg_cpu:.1f}%",
                'recommendation': "Consider reducing computational complexity or using more powerful hardware"
            })
        
        # Check for memory issues
        max_memory = df['system'].apply(lambda x: x['memory_percent']).max()
        if max_memory > 90:
            issues.append({
                'type': 'memory_bottleneck',
                'severity': 'high',
                'description': f"Peak memory usage: {max_memory:.1f}%",
                'recommendation': "Consider reducing memory footprint or increasing available system memory"
            })
        
        # Check for disk issues
        max_disk = df['system'].apply(lambda x: x['disk_percent']).max()
        if max_disk > 80:
            issues.append({
                'type': 'disk_bottleneck',
                'severity': 'medium',
                'description': f"Peak disk usage: {max_disk:.1f}%",
                'recommendation': "Consider cleaning up disk space or using faster storage"
            })
        
        return issues
    
    def export_resource_data(self, filepath: str, format: str = 'csv'):
        """Export resource data to file"""
        df = pd.DataFrame(self.resource_data)
        
        if format.lower() == 'csv':
            df.to_csv(filepath, index=False)
        elif format.lower() == 'json':
            df.to_json(filepath, orient='records')
        else:
            raise ValueError(f"Unsupported format: {format}")
        
        print(f"Resource data exported to: {filepath}")
```

## Advanced Performance Analysis

### 1. Comparative Analysis

```python
class ComparativeAnalyzer:
    """Compare performance across different simulations or configurations"""
    
    @staticmethod
    def compare_solutions(solutions: List[Dict[str, Any]]) -> Dict[str, Any]:
        """Compare multiple simulation solutions"""
        comparison = {
            'configurations': [],
            'performance_ranking': [],
            'statistical_significance': {}
        }
        
        # Extract performance metrics from each solution
        for i, solution in enumerate(solutions):
            config_analysis = {
                'solution_id': i,
                'configuration': solution.get('config', {}),
                'performance_metrics': solution.get('performance', {}),
                'resource_usage': solution.get('resource_usage', {}),
                'efficiency_score': solution.get('efficiency_score', 0.0)
            }
            comparison['configurations'].append(config_analysis)
        
        # Rank solutions by efficiency score
        comparison['performance_ranking'] = sorted(
            comparison['configurations'],
            key=lambda x: x['efficiency_score'],
            reverse=True
        )
        
        # Statistical significance analysis
        if len(comparison['configurations']) > 1:
            comparison['statistical_significance'] = ComparativeAnalyzer._calculate_significance(
                comparison['configurations']
            )
        
        return comparison
    
    @staticmethod
    def _calculate_significance(configurations: List[Dict[str, Any]]) -> Dict[str, Any]:
        """Calculate statistical significance of performance differences"""
        # Implementation would use statistical tests (t-test, ANOVA, etc.)
        return {
            'p_value': 0.0,
            'confidence_interval': (0.0, 0.0),
            'effect_size': 0.0,
            'statistical_power': 0.0
        }
```

### 2. Optimization Recommendations

```python
class OptimizationRecommender:
    """Generate optimization recommendations based on performance analysis"""
    
    def __init__(self, performance_monitor: PerformanceMonitor):
        self.performance_monitor = performance_monitor
        self.optimization_strategies = {
            'neuron_optimization': self._optimize_neuron_parameters,
            'plasticity_optimization': self._optimize_plasticity_parameters,
            'memory_optimization': self._optimize_memory_parameters,
            'system_optimization': self._optimize_system_resources,
            'development_optimization': self._optimize_development_parameters
        }
    
    def generate_recommendations(self) -> List[Dict[str, Any]]:
        """Generate optimization recommendations"""
        recommendations = []
        
        analysis = self.performance_monitor.analyze_performance()
        
        # Check for each optimization category
        for category, optimizer in self.optimization_strategies.items():
            category_recommendations = optimizer(analysis)
            recommendations.extend(category_recommendations)
        
        return recommendations
    
    def _optimize_neuron_parameters(self, analysis: Dict[str, Any]) -> List[Dict[str, Any]]:
        """Optimize neuron-related parameters"""
        recommendations = []
        
        metrics = analysis.get('performance_metrics', {})
        step_time_stats = metrics.get('step_time', {})
        avg_step_time = step_time_stats.get('mean', 0.0)
        
        if avg_step_time > 0.1:  # More than 100ms per step
            recommendations.append({
                'category': 'neuron_optimization',
                'priority': 'high',
                'title': 'Reduce Neuron Count or Improve Algorithm',
                'description': f'Average step time ({avg_step_time:.4f}s) exceeds optimal range',
                'implementation': [
                    'Consider reducing brain.neuron_count parameter',
                    'Optimize neural computation algorithms',
                    'Implement more efficient spike propagation',
                    'Use sparse connectivity patterns'
                ],
                'expected_improvement': '50-80% reduction in step time'
            })
        
        return recommendations
    
    def _optimize_plasticity_parameters(self, analysis: Dict[str, Any]) -> List[Dict[str, Any]]:
        """Optimize plasticity-related parameters"""
        recommendations = []
        
        brain_dynamics = analysis.get('brain_dynamics', {})
        plasticity_effectiveness = brain_dynamics.get('plasticity_effectiveness', 0.0)
        
        if plasticity_effectiveness < 0.5:
            recommendations.append({
                'category': 'plasticity_optimization',
                'priority': 'medium',
                'title': 'Enhance Plasticity Mechanisms',
                'description': f'Plasticity effectiveness is low ({plasticity_effectiveness:.2f})',
                'implementation': [
                    'Increase plasticity.stdp.learning_rate',
                    'Enable Hebbian learning if not already active',
                    'Optimize STDP temporal windows',
                    'Add structural plasticity for long-term changes'
                ],
                'expected_improvement': '20-40% improvement in learning efficiency'
            })
        
        return recommendations
    
    def _optimize_memory_parameters(self, analysis: Dict[str, Any]) -> List[Dict[str, Any]]:
        """Optimize memory-related parameters"""
        recommendations = []
        
        # Check for memory issues
        resource_issues = self._detect_resource_issues(analysis)
        memory_issues = [issue for issue in resource_issues if issue['type'] == 'memory_bottleneck']
        
        if memory_issues:
            recommendations.append({
                'category': 'memory_optimization',
                'priority': 'high',
                'title': 'Optimize Memory Usage',
                'description': 'Memory bottlenecks detected during simulation',
                'implementation': [
                    'Reduce memory.working_memory.capacity if not needed',
                    'Optimize memory.episodic_memory.consolidation_rate',
                    'Implement memory pooling techniques',
                    'Use more efficient data structures'
                ],
                'expected_improvement': '30-60% reduction in memory usage'
            })
        
        return recommendations
    
    def _optimize_system_resources(self, analysis: Dict[str, Any]) -> List[Dict[str, Any]]:
        """Optimize system resource usage"""
        recommendations = []
        
        resource_issues = self._detect_resource_issues(analysis)
        
        # Check for CPU issues
        cpu_issues = [issue for issue in resource_issues if issue['type'] == 'cpu_bottleneck']
        if cpu_issues:
            recommendations.append({
                'category': 'system_optimization',
                'priority': 'high',
                'title': 'Optimize CPU Usage',
                'description': 'CPU bottlenecks detected during simulation',
                'implementation': [
                    'Reduce computational complexity',
                    'Optimize neural algorithm efficiency',
                    'Use vectorized operations where possible',
                    'Consider parallel processing for large networks'
                ],
                'expected_improvement': '20-40% reduction in CPU usage'
            })
        
        # Check for disk issues
        disk_issues = [issue for issue in resource_issues if issue['type'] == 'disk_bottleneck']
        if disk_issues:
            recommendations.append({
                'category': 'system_optimization',
                'priority': 'medium',
                'title': 'Optimize Disk Usage',
                'description': 'Disk I/O bottlenecks detected',
                'implementation': [
                    'Implement disk caching mechanisms',
                    'Reduce unnecessary file operations',
                    'Use in-memory storage for frequently accessed data',
                    'Schedule simulations during low I/O periods'
                ],
                'expected_improvement': '15-30% reduction in disk usage'
            })
        
        return recommendations
    
    def _optimize_development_parameters(self, analysis: Dict[str, Any]) -> List[Dict[str, Any]]:
        """Optimize development-related parameters"""
        recommendations = []
        
        brain_dynamics = analysis.get('brain_dynamics', {})
        development_progress = brain_dynamics.get('development_progress', 0.0)
        
        if development_progress < 0.3:
            recommendations.append({
                'category': 'development_optimization',
                'priority': 'low',
                'title': 'Enhance Development System',
                'description': f'Development progress is slow ({development_progress:.2f})',
                'implementation': [
                    'Increase development.system.plastic_window',
                    'Enhance developmental age-related plasticity',
                    'Optimize developmental stage transitions',
                    'Add more developmental factors'
                ],
                'expected_improvement': '10-25% acceleration of development'
            })
        
        return recommendations
    
    def _detect_resource_issues(self, analysis: Dict[str, Any]) -> List[Dict[str, Any]]:
        """Detect resource-related issues from analysis"""
        # This would typically use the resource monitor's detection methods
        # For now, return empty list as resource issues would be detected elsewhere
        return []
```

## Performance Testing Utilities

### 1. Load Testing

```python
class LoadTester:
    """Load testing for NLM simulations"""
    
    def __init__(self, config_generator, brain_factory):
        self.config_generator = config_generator
        self.brain_factory = brain_factory
        self.results = []
    
    def run_load_test(self, num_solutions: int, steps_per_solution: int, 
                     concurrent_workers: int = 1) -> Dict[str, Any]:
        """
        Run load test with multiple concurrent simulations
        
        Args:
            num_solutions: Number of different configurations to test
            steps_per_solution: Number of steps per simulation
            concurrent_workers: Number of concurrent workers
        
        Returns:
            Load test results
        """
        import time
        from concurrent.futures import ThreadPoolExecutor, as_completed
        
        # Generate test configurations
        configs = [self.config_generator.generate_config(i) 
                  for i in range(num_solutions)]
        
        # Run tests
        start_time = time.time()
        
        with ThreadPoolExecutor(max_workers=concurrent_workers) as executor:
            futures = []
            
            for i, config in enumerate(configs):
                future = executor.submit(
                    self._run_single_test,
                    config, steps_per_solution, i
                )
                futures.append(future)
            
            # Collect results
            for future in as_completed(futures):
                self.results.append(future.result())
        
        end_time = time.time()
        
        # Analyze results
        analysis = self._analyze_load_test_results()
        
        return {
            'total_duration': end_time - start_time,
            'configurations_tested': num_solutions,
            'steps_per_configuration': steps_per_solution,
            'concurrent_workers': concurrent_workers,
            'results': self.results,
            'analysis': analysis
        }
    
    def _run_single_test(self, config, steps, test_id: int) -> Dict[str, Any]:
        """Run a single load test simulation"""
        start_time = time.time()
        
        brain = self.brain_factory.create_brain(config)
        brain.initialize()
        
        for step in range(steps):
            brain.step(step)
        
        end_time = time.time()
        
        return {
            'test_id': test_id,
            'config': config,
            'duration': end_time - start_time,
            'steps_completed': steps,
            'performance': self._calculate_performance(brain, steps),
            'resource_usage': self._measure_resource_usage(brain)
        }
    
    def _calculate_performance(self, brain, steps: int) -> Dict[str, Any]:
        """Calculate performance metrics"""
        return {
            'spikes_per_second': brain.getTotalSpikeCount() / steps,
            'average_firing_rate': brain.getAverageFiringRate(),
            'e_i_ratio': brain.getExcitationInhibitionRatio(),
            'neurons_per_second': brain.getTotalNeuronCount() / (steps / 1000),  # neurons per 1000 steps
            'efficiency_score': self._calculate_efficiency_score(brain, steps)
        }
    
    def _calculate_efficiency_score(self, brain, steps: int) -> float:
        """Calculate efficiency score for the simulation"""
        firing_rate = brain.getAverageFiringRate()
        e_i_ratio = brain.getExcitationInhibitionRatio()
        spike_count = brain.getTotalSpikeCount()
        
        # Normalize factors
        firing_rate_score = min(firing_rate / 50.0, 1.0)  # Normalize to 50 Hz
        e_i_ratio_score = 1.0 - abs(e_i_ratio - 1.0)  # Target E/I ratio of 1.0
        spike_efficiency = min(spike_count / (steps * 0.5), 1.0)  # Target 0.5 spikes/step
        
        return (firing_rate_score + e_i_ratio_score + spike_efficiency) / 3.0
    
    def _measure_resource_usage(self, brain) -> Dict[str, Any]:
        """Measure resource usage during simulation"""
        # This would use system monitoring tools
        return {
            'cpu_usage': 0.0,  # Placeholder
            'memory_usage': brain.getTotalNeuronCount() * 100 / (1024 * 1024),  # Approximate
            'step_time': 0.0  # Placeholder
        }
    
    def _analyze_load_test_results(self) -> Dict[str, Any]:
        """Analyze load test results"""
        if not self.results:
            return {}
        
        analysis = {
            'total_simulations': len(self.results),
            'average_duration': np.mean([r['duration'] for r in self.results]),
            'duration_std': np.std([r['duration'] for r in self.results]),
            'average_performance': self._average_performance(),
            'performance_variance': self._calculate_performance_variance(),
            'bottlenecks': self._identify_load_bottlenecks(),
            'recommendations': self._generate_load_recommendations()
        }
        
        return analysis
    
    def _average_performance(self) -> Dict[str, Any]:
        """Calculate average performance across all tests"""
        performances = [r['performance'] for r in self.results]
        
        return {
            'spikes_per_second': np.mean([p['spikes_per_second'] for p in performances]),
            'average_firing_rate': np.mean([p['average_firing_rate'] for p in performances]),
            'e_i_ratio': np.mean([p['e_i_ratio'] for p in performances]),
            'efficiency_score': np.mean([p['efficiency_score'] for p in performances])
        }
    
    def _calculate_performance_variance(self) -> Dict[str, Any]:
        """Calculate variance in performance metrics"""
        performances = [r['performance'] for r in self.results]
        
        return {
            'spikes_per_second_var': np.var([p['spikes_per_second'] for p in performances]),
            'firing_rate_var': np.var([p['average_firing_rate'] for p in performances]),
            'e_i_ratio_var': np.var([p['e_i_ratio'] for p in performances]),
            'efficiency_score_var': np.var([p['efficiency_score'] for p in performances])
        }
    
    def _identify_load_bottlenecks(self) -> List[Dict[str, Any]]:
        """Identify bottlenecks from load test results"""
        bottlenecks = []
        
        # Check for performance variability
        performances = [r['performance'] for r in self.results]
        efficiency_scores = [p['efficiency_score'] for p in performances]
        
        if max(efficiency_scores) - min(efficiency_scores) > 0.5:
            bottlenecks.append({
                'type': 'performance_variability',
                'severity': 'medium',
                'description': 'High variability in efficiency scores across configurations',
                'recommendation': 'Standardize configuration parameters'
            })
        
        # Check for long durations
        durations = [r['duration'] for r in self.results]
        if max(durations) > min(durations) * 3:
            bottlenecks.append({
                'type': 'duration_variability',
                'severity': 'low',
                'description': 'High variability in simulation durations',
                'recommendation': 'Investigate causes of timing variability'
            })
        
        return bottlenecks
    
    def _generate_load_recommendations(self) -> List[str]:
        """Generate load testing recommendations"""
        recommendations = []
        
        performances = [r['performance'] for r in self.results]
        avg_efficiency = np.mean([p['efficiency_score'] for p in performances])
        
        if avg_efficiency < 0.5:
            recommendations.append(
                "Overall efficiency is low. Consider optimizing neural algorithms "
                "and reducing computational complexity."
            )
        
        if len(self.results) > 10:
            recommendations.append(
                "Large number of concurrent simulations tested. Ensure hardware "
                "resources are adequate for production use."
            )
        
        return recommendations
```

## Performance Monitoring Integration

### 1. Integration with NLMCLI

```python
# Example integration of performance monitoring with NLMCLI
class PerformanceAwareNLMCLI(NLMCLI):
    """Enhanced CLI with performance monitoring capabilities"""
    
    def __init__(self, argc, argv):
        super().__init__(argc, argv)
        self.performance_monitor = PerformanceMonitor()
        self.brain_profiler = BrainProfiler(None)  # Will be set later
        
    def initializeBrain(self):
        """Initialize brain with performance monitoring"""
        super().initializeBrain()
        
        # Set up brain profiler
        self.brain_profiler = BrainProfiler(self.brain)
        
        # Configure performance monitoring based on CLI options
        if self.options.mode == 'benchmark':
            self.performance_monitor.sampling_rate = 1
            self.performance_monitor.alert_thresholds = {
                PerformanceMetric.CPU_USAGE: 80.0,
                PerformanceMetric.MEMORY_USAGE: 90.0
            }
        elif self.options.mode == 'debug':
            self.performance_monitor.sampling_rate = 10
            self.performance_monitor.enable_cpu_monitoring = True
            self.performance_monitor.enable_memory_monitoring = True
        
        # Start performance monitoring
        self.performance_monitor.start_monitoring(self.brain)
    
    def runStandardMode(self):
        """Override standard mode to include performance monitoring"""
        import time
        
        for step in range(self.options.steps):
            step_start = time.time()
            
            # Execute simulation step
            self.brain.step(step)
            
            # Profile the step
            step_time = time.time() - step_start
            self.brain_profiler.profile_step(step, step_start, step_start + step_time)
            
            # Record performance metrics
            self.performance_monitor.record_step_performance(self.brain, step_time)
            
            # Check for plasticity events
            if step % 100 == 0:
                self.brain_profiler.profile_plasticity(step, 'stdp')
            
            # Check for development events
            if step % 500 == 0:
                self.brain_profiler.profile_development(step)
            
            # Check for memory operations
            if step % 200 == 0:
                self.brain_profiler.profile_memory(step, 'working_memory')
            
            # Log progress periodically
            if step % 100 == 0 and not self.options.quiet:
                self.logStepProgress(step)
            
            # Check for early termination
            if self.checkTerminationCondition(step):
                break
        
        # Generate performance report
        if not self.options.quiet:
            print(self.performance_monitor.generate_performance_report())
    
    def runBenchmarkMode(self):
        """Override benchmark mode with enhanced profiling"""
        # Perform warm-up
        for step in range(100):
            self.brain.step(step)
        
        # Run benchmark with detailed profiling
        benchmark_start = time.time()
        
        for step in range(self.options.steps):
            step_start = time.time()
            
            self.brain.step(step)
            
            step_time = time.time() - step_start
            self.performance_monitor.record_step_performance(self.brain, step_time)
            self.brain_profiler.profile_step(step, step_start, step_start + step_time)
        
        benchmark_end = time.time()
        
        # Generate comprehensive benchmark report
        benchmark_analysis = self._analyze_benchmark_performance(
            benchmark_start, benchmark_end
        )
        
        self._output_benchmark_results(benchmark_analysis)
    
    def runDebugMode(self):
        """Override debug mode with comprehensive monitoring"""
        for step in range(self.options.steps):
            step_start = time.time()
            
            # Execute step with detailed profiling
            self.brain.step(step)
            
            step_time = time.time() - step_start
            self.performance_monitor.record_step_performance(self.brain, step_time)
            self.brain_profiler.profile_step(step, step_start, step_start + step_time)
            self.brain_profiler.profile_plasticity(step, 'stdp')
            self.brain_profiler.profile_memory(step, 'working_memory')
            
            # Log detailed information
            if step % 50 == 0:
                self._log_debug_step(step, step_time)
            
            # Check for termination
            if self.checkTerminationCondition(step):
                break
        
        # Generate detailed debug report
        self._generate_debug_report()
    
    def _analyze_benchmark_performance(self, start_time: float, end_time: float) -> Dict[str, Any]:
        """Analyze benchmark performance"""
        duration = end_time - start_time
        
        performance_analysis = {
            'throughput': {
                'steps_per_second': self.options.steps / duration,
                'spikes_per_second': self.brain.getTotalSpikeCount() / duration
            },
            'efficiency': {
                'cpu_efficiency': self.performance_monitor.analyze_performance(),
                'brain_efficiency': self.brain_profiler.analyze_profiling_data()
            },
            'scaling': self._calculate_scaling_analysis(),
            'bottlenecks': self.performance_monitor.detect_bottlenecks(),
            'recommendations': self.performance_monitor.generate_recommendations()
        }
        
        return performance_analysis
    
    def _output_benchmark_results(self, analysis: Dict[str, Any]):
        """Output benchmark results"""
        print("=== BENCHMARK RESULTS ===")
        print(f"Total duration: {analysis['throughput']['steps_per_second']:.2f} seconds")
        print(f"Steps per second: {analysis['throughput']['steps_per_second']:.2f}")
        print(f"Spikes per second: {analysis['throughput']['spikes_per_second']:.2f}")
        
        print("\n=== EFFICIENCY ANALYSIS ===")
        cpu_eff = analysis['efficiency']['cpu_efficiency']
        brain_eff = analysis['efficiency']['brain_efficiency']
        
        print(f"CPU Efficiency: {cpu_eff.get('step_time', {}).get('mean', 0):.4f}s/step")
        print(f"Firing Rate Efficiency: {cpu_eff.get('firing_rate', {}).get('mean', 0):.2f} Hz")
        
        print("\n=== BOTTLENECKS ===")
        for bottleneck in analysis['bottlenecks']:
            print(f"  - {bottleneck['description']}")
        
        print("\n=== RECOMMENDATIONS ===")
        for recommendation in analysis['recommendations']:
            print(f"  - {recommendation}")
        
        # Save results to file if requested
        if self.options.output_file:
            self._save_benchmark_results(analysis)
    
    def _generate_debug_report(self):
        """Generate comprehensive debug report"""
        performance_report = self.performance_monitor.generate_performance_report()
        profiling_report = self.brain_profiler.generate_profiling_report()
        
        debug_report = f"DEBUG REPORT\n"
        debug_report += f"=" * 50 + "\n"
        debug_report += f"Performance Analysis:\n"
        debug_report += performance_report + "\n"
        debug_report += f"Profiling Analysis:\n"
        debug_report += profiling_report + "\n"
        
        if self.options.output_file:
            with open(self.options.output_file, 'w') as f:
                f.write(debug_report)
        
        print(debug_report)
    
    def _save_benchmark_results(self, analysis: Dict[str, Any]):
        """Save benchmark results to file"""
        import json
        
        results = {
            'configuration': self.options,
            'performance_analysis': analysis,
            'timestamp': datetime.now().isoformat()
        }
        
        with open(self.options.output_file, 'w') as f:
            json.dump(results, f, indent=2, default=str)
```

### 2. Performance Monitoring API

```python
# Performance monitoring API for user access
def create_performance_monitor(sampling_rate: int = 10,
                               enable_cpu_monitoring: bool = True,
                               enable_memory_monitoring: bool = True) -> PerformanceMonitor:
    """
    Create a performance monitor for NLM simulations
    
    Args:
        sampling_rate: Number of steps between performance measurements
        enable_cpu_monitoring: Enable CPU usage monitoring
        enable_memory_monitoring: Enable memory usage monitoring
    
    Returns:
        PerformanceMonitor instance
    """
    return PerformanceMonitor(
        sampling_rate=sampling_rate,
        enable_cpu_monitoring=enable_cpu_monitoring,
        enable_memory_monitoring=enable_memory_monitoring
    )

def profile_simulation(brain, steps: int, mode: str = 'standard') -> Dict[str, Any]:
    """
    Profile an NLM simulation with automatic performance monitoring
    
    Args:
        brain: NLM Brain object
        steps: Number of simulation steps
        mode: Profiling mode ('standard', 'debug', 'benchmark')
    
    Returns:
        Dictionary containing profiling results and analysis
    """
    monitor = create_performance_monitor()
    profiler = BrainProfiler(brain)
    
    # Start monitoring
    monitor.start_monitoring(brain)
    
    # Run simulation
    for step in range(steps):
        step_start = time.time()
        
        brain.step(step)
        
        step_time = time.time() - step_start
        monitor.record_step_performance(brain, step_time)
        profiler.profile_step(step, step_start, step_start + step_time)
        
        # Additional profiling based on mode
        if mode == 'debug':
            profiler.profile_plasticity(step, 'stdp')
            profiler.profile_memory(step, 'working_memory')
        
        if step % 100 == 0:
            print(f"Step {step}/{steps}: {brain.getFiringNeuronCount()} firing neurons")
    
    # Stop monitoring
    monitor.stop_monitoring()
    
    # Generate reports
    performance_analysis = monitor.analyze_performance()
    profiling_analysis = profiler.analyze_profiling_data()
    
    return {
        'performance_analysis': performance_analysis,
        'profiling_analysis': profiling_analysis,
        'brain_stats': {
            'total_spikes': brain.getTotalSpikeCount(),
            'average_firing_rate': brain.getAverageFiringRate(),
            'e_i_ratio': brain.getExcitationInhibitionRatio(),
            'neurons': brain.getTotalNeuronCount(),
            'synapses': brain.getTotalSynapseCount()
        },
        'simulation_summary': {
            'steps_completed': steps,
            'duration': time.time() - start_time,
            'throughput': steps / (time.time() - start_time)
        }
    }

class PerformanceAnalysis:
    """Utility class for performance analysis and visualization"""
    
    @staticmethod
    def plot_performance_metrics(performance_data: pd.DataFrame, metrics: List[str] = None):
        """Plot performance metrics"""
        if metrics is None:
            metrics = ['step_time', 'firing_rate', 'cpu_usage', 'memory_usage']
        
        fig, axes = plt.subplots(len(metrics), 1, figsize=(12, 4 * len(metrics)))
        
        if len(metrics) == 1:
            axes = [axes]
        
        for i, metric in enumerate(metrics):
            if metric in performance_data.columns:
                axes[i].plot(performance_data['timestamp'], performance_data[metric])
                axes[i].set_title(f"{metric.replace('_', ' ').title()} over Time")
                axes[i].set_xlabel("Time")
                axes[i].set_ylabel(metric.replace('_', ' '))
                axes[i].grid(True)
        
        plt.tight_layout()
        return fig
    
    @staticmethod
    def generate_performance_summary(performance_data: pd.DataFrame) -> str:
        """Generate a text summary of performance metrics"""
        if performance_data.empty:
            return "No performance data available"
        
        summary = f"Performance Summary\n"
        summary += f"{'=' * 50}\n"
        summary += f"Total steps: {len(performance_data)}\n"
        summary += f"Time range: {performance_data['timestamp'].min()} to {performance_data['timestamp'].max()}\n"
        summary += f"Average step time: {performance_data['step_time'].mean():.4f}s\n"
        summary += f"Average firing rate: {performance_data['firing_rate'].mean():.2f} Hz\n"
        summary += f"Firing rate trend: {PerformanceAnalysis._calculate_trend(performance_data['firing_rate']):.4f}\n"
        summary += f"CPU usage: {performance_data['cpu_usage'].mean():.1f}%\n"
        summary += f"Memory usage: {performance_data['memory_usage'].mean():.1f}%\n"
        
        return summary
    
    @staticmethod
    def _calculate_trend(series: pd.Series) -> float:
        """Calculate linear trend of a time series"""
        if len(series) < 2:
            return 0.0
        
        x = np.arange(len(series))
        y = series.values
        
        slope = np.polyfit(x, y, 1)[0]
        return slope / series.mean() if series.mean() > 0 else 0.0
```

## Conclusion

The NLM performance monitoring and profiling system provides comprehensive tools for:

1. **Real-time Monitoring**: Track simulation metrics during execution
2. **Performance Analysis**: Analyze collected data to identify trends and issues
3. **Bottleneck Detection**: Identify performance bottlenecks and optimization opportunities
4. **Resource Usage**: Monitor CPU, memory, and I/O usage
5. **Comparison Analysis**: Compare performance across different configurations
6. **Profiling Tools**: Detailed profiling of specific operations and components
7. **Load Testing**: Test system performance under various load conditions
8. **Optimization Recommendations**: Generate actionable optimization recommendations

This system enables NLM users to:

- **Understand Performance**: Gain insights into simulation performance and behavior
- **Identify Bottlenecks**: Detect and address performance bottlenecks
- **Optimize Configurations**: Find optimal configuration parameters
- **Validate Results**: Ensure simulation correctness and consistency
- **Benchmark Performance**: Compare performance across different implementations
- **Monitor Production**: Monitor performance in production environments
- **Generate Reports**: Create comprehensive performance reports for analysis

The performance monitoring system is designed to be flexible, extensible, and non-intrusive, allowing users to gather performance insights without significantly impacting simulation performance.

---

**Next Steps for Performance Monitoring:**

1. **Integration**: Integrate performance monitoring into the NLMCLI and other command-line tools
2. **Visualization**: Add interactive visualization capabilities
3. **Alerting**: Implement real-time alerting for performance issues
4. **Comparative Analysis**: Enhance comparative analysis tools
5. **Machine Learning**: Integrate machine learning for performance prediction
6. **Automated Optimization**: Implement automated performance optimization
7. **Documentation**: Add comprehensive documentation and examples

The performance monitoring system provides a foundation for understanding, optimizing, and validating NLM simulations, making it an essential tool for researchers, developers, and users of the NLM framework.
