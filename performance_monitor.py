#!/usr/bin/env python3
"""
Enhanced Performance Monitoring and Debugging Tools for NLM

This module provides comprehensive performance monitoring and debugging
tools for NLM simulations, designed to integrate seamlessly with the
enhanced Python API.

Author: NLM Performance Team
Version: 1.0.0
License: MIT
"""

import time
import threading
import statistics
import psutil
import os
from typing import Dict, List, Optional, Callable, Any
from dataclasses import dataclass, field
from contextlib import contextmanager
from enum import Enum

class PerformanceMetric(Enum):
    """Performance metrics tracked by NLM."""
    NEURONS_PER_SECOND = "neurons_per_second"
    SPIKES_PER_SECOND = "spikes_per_second"
    REAL_TIME_FACTOR = "real_time_factor"
    STEP_TIME_MS = "step_time_ms"
    MEMORY_USAGE_MB = "memory_usage_mb"
    CPU_USAGE_PERCENT = "cpu_usage_percent"
    SYNAPSE_ACTIVITY = "synapse_activity"
    ENERGY_CONSUMPTION = "energy_consumption"
    LEARNING_RATE = "learning_rate"
    NOVELTY_LEVEL = "novelty_level"

@dataclass
class PerformanceSample:
    """Single performance measurement sample."""
    timestamp: float
    step: int
    neurons_firing: int
    total_spikes: int
    step_time_ms: float
    memory_usage_mb: float
    cpu_usage_percent: float
    synapse_activity: float
    energy_consumption: float
    learning_rate: float
    novelty_level: float
    curiosity_level: float

@dataclass
class PerformanceStats:
    """Aggregated performance statistics."""
    total_steps: int = 0
    total_neuron_spikes: int = 0
    total_external_spikes: int = 0
    avg_step_time_ms: float = 0.0
    min_step_time_ms: float = 0.0
    max_step_time_ms: float = 0.0
    std_step_time_ms: float = 0.0
    avg_memory_usage_mb: float = 0.0
    avg_cpu_usage_percent: float = 0.0
    avg_neurons_per_second: float = 0.0
    avg_spikes_per_second: float = 0.0
    avg_real_time_factor: float = 0.0
    current_developmental_stage: str = "Initial"
    learning_curve: List[float] = field(default_factory=list)
    performance_trend: str = "stable"

class PerformanceMonitor:
    """
    Comprehensive performance monitoring for NLM simulations.
    
    This class provides real-time monitoring of simulation performance,
    memory usage, learning progress, and provides alerts for performance
    degradation or abnormal behavior.
    """
    
    def __init__(self, sampling_interval: float = 0.1):
        """
        Initialize performance monitor.
        
        Args:
            sampling_interval: Time between samples in seconds
        """
        self.sampling_interval = sampling_interval
        self.samples: List[PerformanceSample] = []
        self.stats = PerformanceStats()
        self.monitoring = False
        self.monitor_thread: Optional[threading.Thread] = None
        self.callbacks: List[Callable[[PerformanceSample], None]] = []
        self.alert_thresholds = self._default_alert_thresholds()
        self.process = psutil.Process(os.getpid())
        
    def _default_alert_thresholds(self) -> Dict[str, Any]:
        """Define default alert thresholds for performance anomalies."""
        return {
            'max_step_time_ms': 100.0,  # Alert if step takes > 100ms
            'min_neuron_activity': 10.0,  # Alert if < 10 neurons firing
            'max_memory_growth_mb': 500.0,  # Alert if memory grows > 500MB
            'min_learning_rate': 0.001,  # Alert if learning stalls
            'max_cpu_usage': 90.0,  # Alert if CPU usage > 90%
            'min_spike_frequency': 1.0,  # Alert if too few spikes per second
        }
    
    def add_callback(self, callback: Callable[[PerformanceSample], None]):
        """
        Add callback function to be called on each performance sample.
        
        Args:
            callback: Function that takes a PerformanceSample parameter
        """
        self.callbacks.append(callback)
    
    def set_alert_threshold(self, metric: str, threshold: Any):
        """
        Set alert threshold for performance metrics.
        
        Args:
            metric: Metric name (see PerformanceMetric enum)
            threshold: Threshold value
        """
        self.alert_thresholds[metric] = threshold
    
    def start_monitoring(self, brain, world, agent):
        """
        Start continuous performance monitoring in background thread.
        
        Args:
            brain: NLM Brain object
            world: NLM World object
            agent: NLM AgentBrain object
        """
        if self.monitoring:
            return
            
        self.monitoring = True
        self.monitor_thread = threading.Thread(target=self._monitor_loop, args=(brain, world, agent))
        self.monitor_thread.daemon = True
        self.monitor_thread.start()
    
    def stop_monitoring(self):
        """Stop performance monitoring."""
        self.monitoring = False
        if self.monitor_thread:
            self.monitor_thread.join(timeout=5.0)
            self.monitor_thread = None
    
    def _monitor_loop(self, brain, world, agent):
        """Main monitoring loop running in background thread."""
        step = 0
        last_time = time.time()
        
        while self.monitoring:
            current_time = time.time()
            elapsed = current_time - last_time
            
            if elapsed >= self.sampling_interval:
                sample = self._collect_sample(brain, world, agent, step, current_time)
                self._process_sample(sample)
                step += 1
                last_time = current_time
            
            time.sleep(0.001)  # Small sleep to prevent busy waiting
    
    def _collect_sample(self, brain, world, agent, step: int, timestamp: float) -> PerformanceSample:
        """
        Collect a single performance sample.
        
        Args:
            brain: NLM Brain object
            world: NLM World object
            agent: NLM AgentBrain object
            step: Current simulation step
            timestamp: Current time
            
        Returns:
            PerformanceSample with collected metrics
        """
        # Get current system metrics
        memory_info = self.process.memory_info()
        cpu_percent = self.process.cpu_percent()
        
        # Get brain metrics
        neurons_firing = brain.getFiringNeuronCount()
        total_spikes = brain.getTotalSpikeCount()
        avg_firing_rate = brain.getAverageFiringRate()
        developmental_stage = str(brain.getDevelopmentalStage())
        
        # Get agent metrics
        curiosity_level = agent.getCuriosityLevel()
        novelty_level = agent.getNoveltyLevel()
        neuromodulation = agent.getNeuromodulationLevel()
        
        # Get world metrics
        body = world.getAgentBody()
        energy_consumption = body.energy if body else 0.0
        
        return PerformanceSample(
            timestamp=timestamp,
            step=step,
            neurons_firing=neurons_firing,
            total_spikes=total_spikes,
            step_time_ms=0.0,  # Will be calculated from timing
            memory_usage_mb=memory_info.rss / (1024 * 1024),
            cpu_usage_percent=cpu_percent,
            synapse_activity=self._calculate_synapse_activity(brain),
            energy_consumption=energy_consumption,
            learning_rate=self._calculate_learning_rate(agent),
            novelty_level=novelty_level,
            curiosity_level=curiosity_level
        )
    
    def _process_sample(self, sample: PerformanceSample):
        """
        Process and analyze a performance sample.
        
        Args:
            sample: Performance sample to process
        """
        # Calculate step time from timing
        if len(self.samples) > 0:
            sample.step_time_ms = (sample.timestamp - self.samples[-1].timestamp) / 1000.0 * 1000.0
        
        # Add to samples
        self.samples.append(sample)
        
        # Update statistics
        self._update_stats(sample)
        
        # Check for alerts
        self._check_alerts(sample)
        
        # Call registered callbacks
        for callback in self.callbacks:
            try:
                callback(sample)
            except Exception as e:
                print(f"Callback error: {e}")
        
        # Keep only recent samples (last hour)
        if len(self.samples) > 3600:  # Assuming 10Hz sampling
            self.samples = self.samples[-3600:]
    
    def _update_stats(self, sample: PerformanceSample):
        """Update aggregated performance statistics."""
        self.stats.total_steps += 1
        self.stats.total_neuron_spikes += sample.neurons_firing
        self.stats.total_external_spikes += sample.total_spikes
        
        # Update running statistics
        if self.stats.total_steps == 1:
            self.stats.min_step_time_ms = sample.step_time_ms
            self.stats.max_step_time_ms = sample.step_time_ms
        else:
            self.stats.min_step_time_ms = min(self.stats.min_step_time_ms, sample.step_time_ms)
            self.stats.max_step_time_ms = max(self.stats.max_step_time_ms, sample.step_time_ms)
        
        # Update averages using incremental approach
        self.stats.avg_step_time_ms = (self.stats.avg_step_time_ms * (self.stats.total_steps - 1) + sample.step_time_ms) / self.stats.total_steps
        self.stats.avg_memory_usage_mb = (self.stats.avg_memory_usage_mb * (self.stats.total_steps - 1) + sample.memory_usage_mb) / self.stats.total_steps
        self.stats.avg_cpu_usage_percent = (self.stats.avg_cpu_usage_percent * (self.stats.total_steps - 1) + sample.cpu_usage_percent) / self.stats.total_steps
        
        # Calculate derived metrics
        if self.stats.total_steps > 0:
            elapsed_time = self.samples[-1].timestamp - self.samples[0].timestamp
            if elapsed_time > 0:
                self.stats.avg_neurons_per_second = self.stats.total_neuron_spikes / elapsed_time
                self.stats.avg_spikes_per_second = self.stats.total_external_spikes / elapsed_time
                
                if self.stats.avg_step_time_ms > 0:
                    self.stats.avg_real_time_factor = elapsed_time / (self.stats.avg_step_time_ms * self.stats.total_steps / 1000.0)
        
        # Update learning curve
        learning_rate = sample.learning_rate
        self.stats.learning_curve.append(learning_rate)
        
        # Keep only recent learning curve (last 100 steps)
        if len(self.stats.learning_curve) > 100:
            self.stats.learning_curve = self.stats.learning_curve[-100:]
        
        # Update performance trend
        self._update_performance_trend()
    
    def _update_performance_trend(self):
        """Analyze performance trends from recent data."""
        if len(self.stats.learning_curve) < 10:
            self.stats.performance_trend = "insufficient_data"
            return
        
        recent_learning = self.stats.learning_curve[-10:]
        first_half = recent_learning[:5]
        second_half = recent_learning[5:]
        
        avg_first = sum(first_half) / len(first_half)
        avg_second = sum(second_half) / len(second_half)
        
        if avg_second > avg_first * 1.1:
            self.stats.performance_trend = "improving"
        elif avg_second < avg_first * 0.9:
            self.stats.performance_trend = "degrading"
        else:
            self.stats.performance_trend = "stable"
    
    def _calculate_synapse_activity(self, brain) -> float:
        """
        Calculate synapse activity level.
        
        Args:
            brain: NLM Brain object
            
        Returns:
            Synapse activity metric (0.0 to 1.0)
        """
        total_synapses = brain.getTotalSynapseCount()
        if total_synapses == 0:
            return 0.0
            
        # Estimate based on firing rate and neural activity
        firing_rate = brain.getAverageFiringRate()
        activity = min(firing_rate / 100.0, 1.0)  # Normalize to 0-1
        return activity
    
    def _calculate_learning_rate(self, agent) -> float:
        """
        Calculate learning rate based on neuromodulation and curiosity.
        
        Args:
            agent: NLM AgentBrain object
            
        Returns:
            Learning rate metric (0.0 to 1.0)
        """
        curiosity = agent.getCuriosityLevel()
        novelty = agent.getNoveltyLevel()
        neuromodulation = agent.getNeuromodulationLevel()
        
        # Combine metrics (curiosity and novelty are primary learning drivers)
        learning_rate = (curiosity * 0.4 + novelty * 0.4 + abs(neuromodulation) * 0.2)
        return min(max(learning_rate, 0.0), 1.0)
    
    def _check_alerts(self, sample: PerformanceSample):
        """
        Check for performance alerts and warnings.
        
        Args:
            sample: Performance sample to check
        """
        alerts = []
        
        # Check step time threshold
        if sample.step_time_ms > self.alert_thresholds['max_step_time_ms']:
            alerts.append(f"Step time too high: {sample.step_time_ms:.2f}ms > {self.alert_thresholds['max_step_time_ms']}ms")
        
        # Check neuron activity threshold
        if sample.neurons_firing < self.alert_thresholds['min_neuron_activity']:
            alerts.append(f"Low neuron activity: {sample.neurons_firing} neurons firing < {self.alert_thresholds['min_neuron_activity']}")
        
        # Check CPU usage threshold
        if sample.cpu_usage_percent > self.alert_thresholds['max_cpu_usage']:
            alerts.append(f"High CPU usage: {sample.cpu_usage_percent:.1f}% > {self.alert_thresholds['max_cpu_usage']}%")
        
        # Check learning rate threshold
        if sample.learning_rate < self.alert_thresholds['min_learning_rate']:
            alerts.append(f"Learning rate too low: {sample.learning_rate:.4f} < {self.alert_thresholds['min_learning_rate']}")
        
        # Generate alerts
        for alert in alerts:
            self._generate_alert(alert, sample)
    
    def _generate_alert(self, message: str, sample: PerformanceSample):
        """
        Generate performance alert.
        
        Args:
            message: Alert message
            sample: Current performance sample
        """
        alert_level = self._determine_alert_level(message, sample)
        timestamp = time.strftime("%Y-%m-%d %H:%M:%S", time.localtime(sample.timestamp))
        
        alert_msg = f"[{timestamp}] [{alert_level.upper()}] PERFORMANCE ALERT: {message}"
        alert_msg += f" | Step: {sample.step}, Neurons: {sample.neurons_firing}, Spikes: {sample.total_spikes}"
        
        print(alert_msg)
        
        # Log to file for persistence
        self._log_alert(alert_msg)
    
    def _determine_alert_level(self, message: str, sample: PerformanceSample) -> str:
        """
        Determine alert level based on message and current state.
        
        Args:
            message: Alert message
            sample: Current performance sample
            
        Returns:
            Alert level (INFO, WARNING, ERROR, CRITICAL)
        """
        if "High CPU usage" in message or "Learning rate too low" in message:
            return "WARNING"
        elif "Step time too high" in message or "Low neuron activity" in message:
            return "ERROR"
        else:
            return "INFO"
    
    def _log_alert(self, message: str):
        """
        Log alert message to file.
        
        Args:
            message: Alert message to log
        """
        log_file = "nlm_performance_alerts.log"
        try:
            with open(log_file, 'a') as f:
                f.write(message + '\n')
        except Exception as e:
            print(f"Failed to log alert: {e}")
    
    def export_performance_data(self, filepath: str):
        """
        Export performance data to CSV file.
        
        Args:
            filepath: Path to output CSV file
        """
        if not self.samples:
            print("No performance data to export")
            return
        
        import csv
        
        try:
            with open(filepath, 'w', newline='') as csvfile:
                fieldnames = [
                    'timestamp', 'step', 'neurons_firing', 'total_spikes',
                    'step_time_ms', 'memory_usage_mb', 'cpu_usage_percent',
                    'synapse_activity', 'energy_consumption', 'learning_rate',
                    'novelty_level', 'curiosity_level'
                ]
                
                writer = csv.DictWriter(csvfile, fieldnames=fieldnames)
                writer.writeheader()
                
                for sample in self.samples:
                    writer.writerow({
                        'timestamp': sample.timestamp,
                        'step': sample.step,
                        'neurons_firing': sample.neurons_firing,
                        'total_spikes': sample.total_spikes,
                        'step_time_ms': sample.step_time_ms,
                        'memory_usage_mb': sample.memory_usage_mb,
                        'cpu_usage_percent': sample.cpu_usage_percent,
                        'synapse_activity': sample.synapse_activity,
                        'energy_consumption': sample.energy_consumption,
                        'learning_rate': sample.learning_rate,
                        'novelty_level': sample.novelty_level,
                        'curiosity_level': sample.curiosity_level
                    })
            
            print(f"Performance data exported to {filepath}")
            
        except Exception as e:
            print(f"Failed to export performance data: {e}")
    
    def get_performance_summary(self) -> Dict[str, Any]:
        """
        Get comprehensive performance summary.
        
        Returns:
            Dictionary with performance statistics
        """
        if not self.samples:
            return {'error': 'No performance data available'}
        
        summary = {
            'total_samples': len(self.samples),
            'total_steps': self.stats.total_steps,
            'performance_stats': {
                'step_time': {
                    'mean_ms': self.stats.avg_step_time_ms,
                    'min_ms': self.stats.min_step_time_ms,
                    'max_ms': self.stats.max_step_time_ms,
                    'std_ms': self.stats.std_step_time_ms
                },
                'performance': {
                    'neurons_per_second': self.stats.avg_neurons_per_second,
                    'spikes_per_second': self.stats.avg_spikes_per_second,
                    'real_time_factor': self.stats.avg_real_time_factor
                },
                'resource_usage': {
                    'memory_mb': {
                        'mean': self.stats.avg_memory_usage_mb,
                        'min': min(s.memory_usage_mb for s in self.samples),
                        'max': max(s.memory_usage_mb for s in self.samples)
                    },
                    'cpu_percent': {
                        'mean': self.stats.avg_cpu_usage_percent,
                        'min': min(s.cpu_usage_percent for s in self.samples),
                        'max': max(s.cpu_usage_percent for s in self.samples)
                    }
                },
                'learning': {
                    'learning_rate_trend': self.stats.performance_trend,
                    'current_learning_rate': self.stats.learning_curve[-1] if self.stats.learning_curve else 0.0,
                    'average_learning_rate': sum(self.stats.learning_curve) / len(self.stats.learning_curve) if self.stats.learning_curve else 0.0
                }
            },
            'recent_activity': {
                'last_10_steps': len(self.stats.learning_curve[-10:]) if len(self.stats.learning_curve) >= 10 else len(self.stats.learning_curve),
                'current_step': self.samples[-1].step if self.samples else 0,
                'current_neurons_firing': self.samples[-1].neurons_firing if self.samples else 0
            }
        }
        
        return summary

@contextmanager
def performance_monitoring(brain, world, agent, sampling_interval: float = 0.1):
    """
    Context manager for temporary performance monitoring.
    
    Args:
        brain: NLM Brain object
        world: NLM World object
        agent: NLM AgentBrain object
        sampling_interval: Monitoring sampling interval in seconds
        
    Usage:
        with performance_monitoring(brain, world, agent) as monitor:
            # Your simulation code here
            for step in range(1000):
                world.update(0.1)
                agent.processSensoryInput(world.getSensoryPercept())
                brain.step(step)
                action = agent.decodeMotorCommand()
                world.applyMotorCommand(action, world.getSimulationTime())
        
        # Access performance data
        summary = monitor.get_performance_summary()
        monitor.export_performance_data("simulation_performance.csv")
    """
    monitor = PerformanceMonitor(sampling_interval)
    monitor.start_monitoring(brain, world, agent)
    
    try:
        yield monitor
    finally:
        monitor.stop_monitoring()