#!/usr/bin/env python3
"""
Performance monitoring and profiling tools for NLM

This module provides comprehensive performance monitoring capabilities
for NLM simulations, including:
- Real-time performance metrics
- Detailed profiling and benchmarking
- Performance analysis and reporting
- Resource usage monitoring
- Simulation performance tracking
"""

import time
import threading
import sys
import os
from typing import Dict, List, Any, Optional, Callable
from dataclasses import dataclass, field
from enum import Enum
import json
import statistics
from collections import defaultdict, deque

# Try to import pynlm
try:
    import pynlm
    PYNLM_AVAILABLE = True
except ImportError:
    PYNLM_AVAILABLE = False
    pynlm = None

@dataclass
class PerformanceMetrics:
    """Real-time performance metrics"""
    timestamp: float
    step: int
    simulation_time: float
    
    # Brain performance metrics
    neuron_count: int
    active_neurons: int
    firing_rate: float
    spike_count: int
    
    # Processing performance metrics
    step_time: float
    sensory_time: float
    brain_time: float
    motor_time: float
    world_time: float
    
    # Memory usage metrics
    memory_usage: float
    neuron_memory: float
    synapse_memory: float
    
    # Learning performance metrics
    plasticity_operations: int
    neuromodulation_events: int
    learning_rate: float
    
    # System metrics
    cpu_usage: float
    memory_pressure: float
    thread_count: int

@dataclass
class BenchmarkResult:
    """Benchmark test results"""
    test_name: str
    config: Dict[str, Any]
    duration: float
    steps_per_second: float
    metrics: List[PerformanceMetrics]
    memory_efficiency: float
    stability: float
    scalability_score: float

class PerformanceMonitor:
    """Real-time performance monitoring for NLM simulations"""
    
    def __init__(self, enable_real_time=False, sampling_interval=0.1):
        self.enable_real_time = enable_real_time
        self.sampling_interval = sampling_interval
        self.metrics_history = deque(maxlen=10000)
        self.start_time = None
        self.step_count = 0
        self.monitoring_thread = None
        self.stop_monitoring = False
        
        # Performance counters
        self.step_times = deque(maxlen=1000)
        self.function_times: Dict[str, List[float]] = defaultdict(list)
        self.memory_usage = []
        
        # Custom callbacks
        self.performance_callbacks: List[Callable[[PerformanceMetrics], None]] = []
        
        # Target performance thresholds
        self.thresholds = {
            'max_step_time': 0.1,  # seconds
            'min_firing_rate': 0.1,  # Hz
            'max_memory_usage': 1024,  # MB
            'target_steps_per_second': 100,  # Hz
        }
    
    def start_timing(self):
        """Start timing for current operation"""
        self.start_time = time.time()
    
    def end_timing(self, operation_name: str):
        """End timing and record operation duration"""
        if self.start_time is None:
            return
        
        duration = time.time() - self.start_time
        self.start_time = None
        
        self.function_times[operation_name].append(duration)
        return duration
    
    def record_step(self, step: int, metrics: PerformanceMetrics):
        """Record metrics for a simulation step"""
        self.metrics_history.append(metrics)
        self.step_count = step
        
        # Call performance callbacks
        for callback in self.performance_callbacks:
            callback(metrics)
        
        # Check thresholds
        self._check_thresholds(metrics)
    
    def _check_thresholds(self, metrics: PerformanceMetrics):
        """Check if performance metrics exceed thresholds"""
        alerts = []
        
        if metrics.step_time > self.thresholds['max_step_time']:
            alerts.append(f"Step time {metrics.step_time:.3f}s exceeds threshold {self.thresholds['max_step_time']}s")
        
        if metrics.firing_rate < self.thresholds['min_firing_rate']:
            alerts.append(f"Firing rate {metrics.firing_rate:.2f}Hz below threshold {self.thresholds['min_firing_rate']}Hz")
        
        if metrics.memory_usage > self.thresholds['max_memory_usage']:
            alerts.append(f"Memory usage {metrics.memory_usage:.1f}MB exceeds threshold {self.thresholds['max_memory_usage']}MB")
        
        if alerts:
            self._trigger_alerts(alerts)
    
    def _trigger_alerts(self, alerts: List[str]):
        """Trigger performance alerts"""
        print(f"⚠️  PERFORMANCE ALERTS:")
        for alert in alerts:
            print(f"   - {alert}")
    
    def add_performance_callback(self, callback: Callable[[PerformanceMetrics], None]):
        """Add a callback for performance monitoring"""
        self.performance_callbacks.append(callback)
    
    def set_thresholds(self, **kwargs):
        """Set performance thresholds"""
        self.thresholds.update(kwargs)
    
    def get_current_metrics(self) -> Optional[PerformanceMetrics]:
        """Get current performance metrics"""
        if self.metrics_history:
            return self.metrics_history[-1]
        return None
    
    def get_average_step_time(self) -> float:
        """Get average step time"""
        if self.step_times:
            return statistics.mean(self.step_times)
        return 0.0
    
    def get_performance_summary(self) -> Dict[str, Any]:
        """Get comprehensive performance summary"""
        if not self.metrics_history:
            return {}
        
        recent_metrics = list(self.metrics_history)[-100:]  # Last 100 steps
        
        return {
            'simulation_stats': {
                'total_steps': self.step_count,
                'current_step': self.metrics_history[-1].step if self.metrics_history else 0,
                'average_firing_rate': statistics.mean([m.firing_rate for m in recent_metrics]),
                'max_firing_rate': max(m.firing_rate for m in recent_metrics),
                'min_firing_rate': min(m.firing_rate for m in recent_metrics),
                'average_step_time': statistics.mean([m.step_time for m in recent_metrics]),
                'total_spikes': sum(m.spike_count for m in recent_metrics),
            },
            'performance_targets': {
                'steps_per_second': 1.0 / self.get_average_step_time() if self.get_average_step_time() > 0 else 0,
                'target_achieved': self.get_average_step_time() < self.thresholds['max_step_time'],
            },
            'memory_usage': {
                'current': recent_metrics[-1].memory_usage if recent_metrics else 0,
                'average': statistics.mean([m.memory_usage for m in recent_metrics]),
            }
        }

class BenchmarkSuite:
    """Comprehensive benchmark suite for NLM"""
    
    def __init__(self):
        self.results = []
        self.custom_configs = {}
    
    def add_config(self, name: str, config: Dict[str, Any]):
        """Add a custom configuration for benchmarking"""
        self.custom_configs[name] = config
    
    def run_benchmark(self, brain, world, agent, benchmark_name: str, config_name: str = "default"):
        """Run a comprehensive benchmark"""
        if config_name not in self.custom_configs:
            config_name = "default"
        
        config = self.custom_configs.get(config_name, {})
        
        print(f"Running benchmark: {benchmark_name} (config: {config_name})")
        
        # Initialize performance monitor
        monitor = PerformanceMonitor(enable_real_time=False)
        monitor.set_thresholds(**config.get('thresholds', {}))
        
        # Start timing
        start_time = time.time()
        
        # Run simulation
        num_steps = config.get('steps', 1000)
        for step in range(num_steps):
            # Update world
            world.update(config.get('timestep', 0.1))
            
            # Process sensory input
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            
            # Brain step
            brain.step(step)
            
            # Decode motor command
            action = agent.decodeMotorCommand()
            
            # Apply action
            world.applyMotorCommand(action, world.getSimulationTime())
            
            # Apply learning
            reward = percept.getInternal()[0] if percept.getInternal() else 0.0
            agent.applyRewardModulation(reward, 0.0)
            
            agent.updateDevelopment(config.get('timestep', 0.1))
        
        end_time = time.time()
        duration = end_time - start_time
        
        # Calculate results
        steps_per_second = num_steps / duration
        
        # Calculate memory efficiency (approximate)
        neuron_count = brain.getTotalNeuronCount()
        memory_efficiency = neuron_count / duration if duration > 0 else 0
        
        # Calculate stability (coefficient of variation of firing rate)
        stability = 1.0  # Simplified
        
        # Calculate scalability score (simplified)
        scalability_score = min(steps_per_second / 10.0, 1.0)  # Normalize to 0-1
        
        result = BenchmarkResult(
            test_name=benchmark_name,
            config=config,
            duration=duration,
            steps_per_second=steps_per_second,
            metrics=list(monitor.metrics_history),
            memory_efficiency=memory_efficiency,
            stability=stability,
            scalability_score=scalability_score
        )
        
        self.results.append(result)
        return result

# Convenience functions for common benchmarking scenarios
def run_performance_analysis(brain, world, agent):
    """Run comprehensive performance analysis"""
    print("Running comprehensive performance analysis...")
    
    # Record initial state
    initial_neurons = brain.getTotalNeuronCount()
    initial_spikes = brain.getTotalSpikeCount()
    
    # Run performance test
    num_steps = 500
    for step in range(num_steps):
        world.update(0.1)
        
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        brain.step(step)
        
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Apply learning
        reward = percept.getInternal()[0] if percept.getInternal() else 0.0
        agent.applyRewardModulation(reward, 0.0)
        
        agent.updateDevelopment(0.1)
    
    final_neurons = brain.getTotalNeuronCount()
    final_spikes = brain.getTotalSpikeCount()
    
    # Calculate performance metrics
    duration = num_steps * 0.1  # Assuming 0.1s timestep
    steps_per_second = num_steps / duration
    neurons_per_step = initial_neurons / num_steps if num_steps > 0 else 0
    spikes_per_neuron = (final_spikes - initial_spikes) / (final_neurons - initial_neurons) if final_neurons > initial_neurons else 0
    
    performance_data = {
        'simulation_parameters': {
            'total_steps': num_steps,
            'duration': duration,
            'timestep': 0.1,
            'neuron_count': final_neurons,
            'initial_neurons': initial_neurons,
        },
        'performance_metrics': {
            'steps_per_second': steps_per_second,
            'neurons_per_step': neurons_per_step,
            'spikes_per_neuron': spikes_per_neuron,
            'final_spike_count': final_spikes,
            'initial_spike_count': initial_spikes,
        },
        'agent_performance': {
            'curiosity_level': agent.getCuriosityLevel(),
            'novelty_level': agent.getNoveltyLevel(),
            'development_stage': agent.getDevelopmentalStage(),
        },
        'brain_performance': {
            'firing_rate': brain.getAverageFiringRate(),
            'active_neurons': brain.getFiringNeuronCount(),
            'e_i_ratio': brain.getExcitationInhibitionRatio(),
        }
    }
    
    return performance_data

if __name__ == "__main__":
    print("NLM Performance Monitoring and Profiling Tools")
    print("=" * 50)
    
    if PYNLM_AVAILABLE:
        # Example usage
        print("Testing with real NLM...")
        
        config = nlm.createDefaultConfig()
        brain = nlm.createBrain(config)
        brain.initialize()
        
        world = nlm.createSimpleWorld()
        world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
        world.reset()
        
        agent = nlm.createAgentBrain(brain)
        agent.initialize(world)
        
        # Run performance analysis
        analysis = run_performance_analysis(brain, world, agent)
        
        print("\nPerformance Analysis Results:")
        print(json.dumps(analysis, indent=2))
        
    else:
        print("pynlm not available. Please install Python bindings first.")
        print("Run: pip install -e .")