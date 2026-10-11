#!/usr/bin/env python3
"""
Performance Profiling Utilities for NLM

This module provides comprehensive performance profiling capabilities for NLM simulations,
including time profiling, memory profiling, performance analysis, and optimization recommendations.
"""

import pynlm
import time
import sys
import os
import json
from typing import Dict, Any, List, Optional, Callable, Union
from dataclasses import dataclass, field
from collections import defaultdict, deque
import statistics
import threading
from contextlib import contextmanager
import psutil
from datetime import datetime


@dataclass
class PerformanceMetrics:
    """Performance metrics collection for NLM simulations"""
    # Timing metrics
    total_simulation_time: float = 0.0
    brain_initialization_time: float = 0.0
    step_processing_time: float = 0.0
    average_step_time: float = 0.0
    time_per_neuron: float = 0.0
    time_per_synapse: float = 0.0
    
    # Spike processing metrics
    total_spike_processing_time: float = 0.0
    average_spike_time: float = 0.0
    spikes_per_second: float = 0.0
    
    # Memory metrics
    peak_memory_usage: float = 0.0  # MB
    average_memory_usage: float = 0.0
    memory_per_neuron: float = 0.0
    memory_per_synapse: float = 0.0
    
    # Neural computation metrics
    total_neurons_processed: int = 0
    total_synapses_processed: int = 0
    connections_per_second: float = 0.0
    
    # Simulation statistics
    total_steps: int = 0
    total_simulated_time: float = 0.0
    simulation_efficiency: float = 0.0


@dataclass
class TimeProfileData:
    """Time profiling data for specific operations"""
    operation_name: str
    total_time: float
    call_count: int
    average_time: float
    min_time: float
    max_time: float
    time_per_unit: float  # Time per neuron, synapse, etc.
    trend_data: List[float] = field(default_factory=list)


@dataclass
class MemoryProfileData:
    """Memory profiling data for NLM components"""
    component_name: str
    peak_memory: float  # MB
    average_memory: float  # MB
    memory_samples: List[float]
    growth_rate: float


class TimeProfiler:
    """
    Time profiling for NLM operations with detailed statistics and trend analysis.
    
    Provides granular timing information for NLM operations, including:
    - Simulation step timing
    - Brain initialization timing
    - Spike processing timing
    - Custom operation timing
    """
    
    def __init__(self):
        self.time_profiles: Dict[str, TimeProfileData] = {}
        self.start_times: Dict[str, float] = {}
        self.call_counts: Dict[str, int] = defaultdict(int)
        self.total_times: Dict[str, float] = defaultdict(float)
        self.min_times: Dict[str, float] = defaultdict(lambda: float('inf'))
        self.max_times: Dict[str, float] = defaultdict(lambda: 0.0)
        self.operation_data: Dict[str, List[float]] = defaultdict(list)
        
        # Initialize with common NLM operations
        self._initialize_common_operations()
    
    def _initialize_common_operations(self):
        """Initialize time profiles for common NLM operations"""
        operations = [
            'brain_initialization',
            'brain_step',
            'synapse_processing',
            'neuron_update',
            'spike_propagation',
            'plasticity_update',
            'memory_access',
            'sensory_input',
            'motor_output',
            'world_update',
            'agent_processing',
        ]
        
        for op in operations:
            self.time_profiles[op] = TimeProfileData(
                operation_name=op,
                total_time=0.0,
                call_count=0,
                average_time=0.0,
                min_time=float('inf'),
                max_time=0.0,
                time_per_unit=0.0
            )
    
    @contextmanager
    def profile_operation(self, operation_name: str, unit_count: float = 1.0):
        """
        Context manager for profiling a specific operation.
        
        Args:
            operation_name: Name of the operation to profile
            unit_count: Number of units (neurons, synapses, etc.) this operation acts on
        """
        if operation_name not in self.time_profiles:
            self.time_profiles[operation_name] = TimeProfileData(
                operation_name=operation_name,
                total_time=0.0,
                call_count=0,
                average_time=0.0,
                min_time=float('inf'),
                max_time=0.0,
                time_per_unit=0.0
            )
        
        start_time = time.time()
        self.start_times[operation_name] = start_time
        
        yield
        
        end_time = time.time()
        duration = end_time - start_time
        
        # Update statistics
        self.call_counts[operation_name] += 1
        self.total_times[operation_name] += duration
        self.min_times[operation_name] = min(self.min_times[operation_name], duration)
        self.max_times[operation_name] = max(self.max_times[operation_name], duration)
        self.operation_data[operation_name].append(duration)
        
        # Update time profile
        profile = self.time_profiles[operation_name]
        profile.total_time = self.total_times[operation_name]
        profile.call_count = self.call_counts[operation_name]
        profile.average_time = profile.total_time / profile.call_count if profile.call_count > 0 else 0.0
        profile.min_time = self.min_times[operation_name]
        profile.max_time = self.max_times[operation_name]
        profile.time_per_unit = profile.total_time / (profile.call_count * unit_count) if profile.call_count > 0 else 0.0
    
    def profile_step(self, brain: pynlm.Brain, step_number: int, 
                     world: Optional[pynlm.SimpleWorld] = None,
                     agent: Optional[pynlm.AgentBrain] = None) -> Dict[str, Any]:
        """
        Profile a single simulation step with detailed breakdown.
        
        Args:
            brain: The brain being simulated
            step_number: Current step number
            world: Optional world state
            agent: Optional agent state
            
        Returns:
            Dictionary with step profiling results
        """
        step_start_time = time.time()
        
        with self.profile_operation('brain_step', brain.getTotalNeuronCount()):
            with self.profile_operation('synapse_processing', brain.getTotalSynapseCount()):
                # Record state before step
                pre_spikes = brain.getTotalSpikeCount()
                pre_firing_neurons = brain.getFiringNeuronCount()
                
                # Execute step
                brain.step(step_number)
                
                # Record state after step
                post_spikes = brain.getTotalSpikeCount()
                post_firing_neurons = brain.getFiringNeuronCount()
        
        # Profile world and agent if available
        if world:
            with self.profile_operation('world_update', 1):
                world.update(0.1)
        
        if agent:
            with self.profile_operation('agent_processing', 1):
                agent.processSensoryInput(world.getSensoryPercept() if world else None)
        
        step_end_time = time.time()
        step_duration = step_end_time - step_start_time
        
        # Calculate step statistics
        spike_count = post_spikes - pre_spikes
        firing_change = post_firing_neurons - pre_firing_neurons
        
        return {
            'step_number': step_number,
            'step_duration': step_duration,
            'spike_count': spike_count,
            'firing_neurons_before': pre_firing_neurons,
            'firing_neurons_after': post_firing_neurons,
            'firing_change': firing_change,
            'brain_neuron_count': brain.getTotalNeuronCount(),
            'brain_synapse_count': brain.getTotalSynapseCount(),
        }
    
    def get_time_profile_summary(self) -> Dict[str, Any]:
        """Get summary of all time profiles"""
        summary = {}
        
        for op_name, profile in self.time_profiles.items():
            if profile.call_count > 0:
                summary[op_name] = {
                    'total_time': profile.total_time,
                    'call_count': profile.call_count,
                    'average_time': profile.average_time,
                    'min_time': profile.min_time,
                    'max_time': profile.max_time,
                    'time_per_unit': profile.time_per_unit,
                    'percentage_of_total': (profile.total_time / self.get_total_time() * 100) 
                                          if self.get_total_time() > 0 else 0.0,
                }
        
        return summary
    
    def get_total_time(self) -> float:
        """Get total time across all operations"""
        return sum(self.total_times.values())
    
    def get_operations_by_time(self) -> List[Dict[str, Any]]:
        """Get operations sorted by time consumption"""
        operations = []
        for op_name, profile in self.time_profiles.items():
            if profile.call_count > 0:
                operations.append({
                    'name': op_name,
                    'total_time': profile.total_time,
                    'average_time': profile.average_time,
                    'calls': profile.call_count,
                    'percentage': (profile.total_time / self.get_total_time() * 100)
                                  if self.get_total_time() > 0 else 0.0,
                })
        
        return sorted(operations, key=lambda x: x['total_time'], reverse=True)
    
    def get_trend_analysis(self, operation_name: str, window_size: int = 10) -> Dict[str, Any]:
        """
        Analyze time trends for a specific operation.
        
        Args:
            operation_name: Name of operation to analyze
            window_size: Number of recent calls to analyze
            
        Returns:
            Dictionary with trend analysis results
        """
        if operation_name not in self.operation_data:
            return {'error': f'Operation {operation_name} not found'}
        
        data = self.operation_data[operation_name]
        if len(data) < 2:
            return {'error': f'Insufficient data for trend analysis'}
        
        # Use only recent data
        recent_data = data[-window_size:] if len(data) >= window_size else data
        
        # Calculate trend
        x_values = list(range(len(recent_data)))
        y_values = recent_data
        
        if len(y_values) < 2:
            return {'error': 'Insufficient data points for trend analysis'}
        
        # Simple linear regression for trend
        n = len(x_values)
        sum_x = sum(x_values)
        sum_y = sum(y_values)
        sum_xx = sum(x*x for x in x_values)
        sum_xy = sum(x*y for x, y in zip(x_values, y_values))
        
        slope = (n * sum_xy - sum_x * sum_y) / (n * sum_xx - sum_x * sum_x)
        intercept = (sum_y - slope * sum_x) / n
        
        # Calculate R-squared
        y_mean = sum_y / n
        ss_tot = sum((y - y_mean) ** 2 for y in y_values)
        ss_res = sum((y - (slope * x + intercept)) ** 2 for x, y in zip(x_values, y_values))
        
        r_squared = 1 - (ss_res / ss_tot) if ss_tot > 0 else 0.0
        
        return {
            'operation': operation_name,
            'data_points': len(recent_data),
            'current_average': recent_data[-1],
            'overall_average': statistics.mean(data),
            'trend_slope': slope,
            'trend_intercept': intercept,
            'r_squared': r_squared,
            'trend_direction': 'increasing' if slope > 0.001 else 
                              'decreasing' if slope < -0.001 else 'stable',
        }


class MemoryProfiler:
    """
    Memory profiling for NLM components with detailed memory tracking.
    
    Tracks memory usage for NLM components including:
    - Brain memory usage
    - World memory usage
    - Agent memory usage
    - System-wide memory usage
    """
    
    def __init__(self):
        self.memory_profiles: Dict[str, MemoryProfileData] = {}
        self.memory_samples: Dict[str, List[float]] = defaultdict(list)
        self.sample_interval: float = 0.1  # seconds
        self.monitoring_thread: Optional[threading.Thread] = None
        self.monitoring: bool = False
        
        # Initialize with common NLM components
        self._initialize_common_components()
    
    def _initialize_common_components(self):
        """Initialize memory profiles for common NLM components"""
        components = [
            'brain',
            'world',
            'agent',
            'plasticity',
            'memory',
            'prediction',
            'cognition',
        ]
        
        for component in components:
            self.memory_profiles[component] = MemoryProfileData(
                component_name=component,
                peak_memory=0.0,
                average_memory=0.0,
                memory_samples=[],
                growth_rate=0.0
            )
    
    def start_memory_monitoring(self, interval: float = 0.1):
        """Start continuous memory monitoring in background thread"""
        if self.monitoring:
            return
        
        self.sample_interval = interval
        self.monitoring = True
        
        self.monitoring_thread = threading.Thread(target=self._memory_monitor_loop, daemon=True)
        self.monitoring_thread.start()
        print(f"Started memory monitoring with {interval}s interval")
    
    def stop_memory_monitoring(self):
        """Stop memory monitoring"""
        self.monitoring = False
        if self.monitoring_thread:
            self.monitoring_thread.join(timeout=5.0)
            self.monitoring_thread = None
        print("Stopped memory monitoring")
    
    def _memory_monitor_loop(self):
        """Background memory monitoring loop"""
        while self.monitoring:
            self._collect_memory_sample()
            time.sleep(self.sample_interval)
    
    def _collect_memory_sample(self):
        """Collect memory usage sample for all components"""
        process = psutil.Process(os.getpid())
        current_memory = process.memory_info().rss / 1024 / 1024  # MB
        
        for component in self.memory_profiles:
            # Get component-specific memory estimate
            if component == 'brain':
                # Estimate brain memory based on neuron/synapse count
                # This is a rough approximation - could be enhanced
                estimated_memory = (current_memory * 0.7)  # Assume 70% for brain
            elif component == 'world':
                estimated_memory = (current_memory * 0.1)  # Assume 10% for world
            elif component == 'agent':
                estimated_memory = (current_memory * 0.15)  # Assume 15% for agent
            elif component == 'plasticity':
                estimated_memory = (current_memory * 0.05)  # Assume 5% for plasticity
            elif component == 'memory':
                estimated_memory = (current_memory * 0.08)  # Assume 8% for memory
            elif component == 'prediction':
                estimated_memory = (current_memory * 0.02)  # Assume 2% for prediction
            elif component == 'cognition':
                estimated_memory = (current_memory * 0.03)  # Assume 3% for cognition
            else:
                estimated_memory = current_memory * 0.01  # Default 1%
            
            # Store sample
            self.memory_samples[component].append(estimated_memory)
            
            # Update component profile
            profile = self.memory_profiles[component]
            profile.memory_samples.append(estimated_memory)
            profile.peak_memory = max(profile.peak_memory, estimated_memory)
        
        # Calculate component growth rates
        for component, samples in self.memory_samples.items():
            if len(samples) >= 3:
                # Simple linear regression for growth rate
                x_values = list(range(len(samples)))
                y_values = samples
                
                n = len(x_values)
                sum_x = sum(x_values)
                sum_y = sum(y_values)
                sum_xx = sum(x*x for x in x_values)
                sum_xy = sum(x*y for x, y in zip(x_values, y_values))
                
                slope = (n * sum_xy - sum_x * sum_y) / (n * sum_xx - sum_x * sum_x)
                profile = self.memory_profiles[component]
                profile.growth_rate = slope
    
    def profile_memory_usage(self, brain: pynlm.Brain, world: Optional[pynlm.SimpleWorld] = None,
                            agent: Optional[pynlm.AgentBrain] = None) -> Dict[str, Any]:
        """
        Profile memory usage for NLM components.
        
        Args:
            brain: The brain to profile
            world: Optional world state
            agent: Optional agent state
            
        Returns:
            Dictionary with memory profiling results
        """
        process = psutil.Process(os.getpid())
        current_memory = process.memory_info().rss / 1024 / 1024  # MB
        
        # Get component-specific memory estimates
        brain_memory = self._estimate_brain_memory(brain)
        world_memory = self._estimate_world_memory(world) if world else 0.0
        agent_memory = self._estimate_agent_memory(agent) if agent else 0.0
        
        # Update profiles
        self.memory_profiles['brain'].peak_memory = max(
            self.memory_profiles['brain'].peak_memory, brain_memory)
        self.memory_profiles['brain'].memory_samples.append(brain_memory)
        
        if world:
            self.memory_profiles['world'].peak_memory = max(
                self.memory_profiles['world'].peak_memory, world_memory)
            self.memory_profiles['world'].memory_samples.append(world_memory)
        
        if agent:
            self.memory_profiles['agent'].peak_memory = max(
                self.memory_profiles['agent'].peak_memory, agent_memory)
            self.memory_profiles['agent'].memory_samples.append(agent_memory)
        
        # Calculate statistics
        brain_samples = self.memory_profiles['brain'].memory_samples
        if world:
            world_samples = self.memory_profiles['world'].memory_samples
        if agent:
            agent_samples = self.memory_profiles['agent'].memory_samples
        
        return {
            'timestamp': datetime.now().isoformat(),
            'total_memory': current_memory,
            'brain_memory': brain_memory,
            'brain_neuron_count': brain.getTotalNeuronCount(),
            'brain_synapse_count': brain.getTotalSynapseCount(),
            'world_memory': world_memory if world else 0.0,
            'agent_memory': agent_memory if agent else 0.0,
            'memory_per_neuron': brain_memory / max(1, brain.getTotalNeuronCount()),
            'memory_per_synapse': brain_memory / max(1, brain.getTotalSynapseCount()),
            'brain_memory_samples': brain_samples[-10:],  # Last 10 samples
            'world_memory_samples': world_samples[-10:] if world else [],
            'agent_memory_samples': agent_samples[-10:] if agent else [],
        }
    
    def _estimate_brain_memory(self, brain: pynlm.Brain) -> float:
        """Estimate brain memory usage based on neuron/synapse count"""
        # Base memory for brain structure
        base_memory = 10.0  # MB
        
        # Memory per neuron (neuron data, connections, etc.)
        neuron_memory = 0.1  # MB per neuron
        synapse_memory = 0.05  # MB per synapse
        
        # Calculate total memory
        total_memory = (base_memory + 
                       brain.getTotalNeuronCount() * neuron_memory +
                       brain.getTotalSynapseCount() * synapse_memory)
        
        # Add overhead for plasticity rules, memory systems, etc.
        total_memory *= 1.5  # 50% overhead for additional systems
        
        return total_memory
    
    def _estimate_world_memory(self, world: pynlm.SimpleWorld) -> float:
        """Estimate world memory usage"""
        # Base memory for world structure
        base_memory = 5.0  # MB
        
        # Memory for world objects and environment
        object_memory = 0.5  # MB per object
        num_objects = 10  # Estimated
        
        return base_memory + num_objects * object_memory
    
    def _estimate_agent_memory(self, agent: pynlm.AgentBrain) -> float:
        """Estimate agent memory usage"""
        # Base memory for agent structure
        base_memory = 8.0  # MB
        
        # Memory for agent's neural processing
        processing_memory = 0.2  # MB
        
        return base_memory + processing_memory
    
    def get_memory_profile_summary(self) -> Dict[str, Any]:
        """Get summary of all memory profiles"""
        summary = {}
        
        for component_name, profile in self.memory_profiles.items():
            if profile.memory_samples:
                summary[component_name] = {
                    'peak_memory': profile.peak_memory,
                    'average_memory': statistics.mean(profile.memory_samples),
                    'current_memory': profile.memory_samples[-1],
                    'sample_count': len(profile.memory_samples),
                    'growth_rate': profile.growth_rate,
                    'memory_variance': statistics.variance(profile.memory_samples)
                                       if len(profile.memory_samples) > 1 else 0.0,
                }
        
        return summary
    
    def get_memory_usage_over_time(self, component_name: str, 
                                  window_size: int = 60) -> List[Dict[str, Any]]:
        """
        Get memory usage over time for a specific component.
        
        Args:
            component_name: Name of component to analyze
            window_size: Number of recent samples to return
            
        Returns:
            List of memory usage data points
        """
        if component_name not in self.memory_samples:
            return []
        
        samples = self.memory_samples[component_name]
        if not samples:
            return []
        
        # Get recent samples
        recent_samples = samples[-window_size:] if len(samples) >= window_size else samples
        
        return [
            {
                'timestamp': datetime.now().timestamp() - (len(samples) - i) * self.sample_interval,
                'memory_mb': sample,
                'sample_index': len(samples) - len(recent_samples) + i,
            }
            for i, sample in enumerate(recent_samples)
        ]


class PerformanceAnalyzer:
    """
    Advanced performance analysis for NLM simulations.
    
    Provides comprehensive analysis of NLM performance including:
    - Time and memory efficiency analysis
    - Bottleneck identification
    - Optimization recommendations
    - Performance trend analysis
    """
    
    def __init__(self):
        self.time_profiler = TimeProfiler()
        self.memory_profiler = MemoryProfiler()
    
    def analyze_simulation_performance(self, brain: pynlm.Brain,
                                      world: Optional[pynlm.SimpleWorld] = None,
                                      agent: Optional[pynlm.AgentBrain] = None,
                                      steps: int = 100) -> Dict[str, Any]:
        """
        Analyze performance of an NLM simulation.
        
        Args:
            brain: The brain to simulate
            world: Optional world state
            agent: Optional agent state
            steps: Number of simulation steps to run
            
        Returns:
            Comprehensive performance analysis
        """
        # Start memory monitoring
        self.memory_profiler.start_memory_monitoring()
        
        try:
            # Run simulation while profiling
            print(f"Running simulation performance analysis ({steps} steps)...")
            
            step_results = []
            start_time = time.time()
            
            for step in range(steps):
                # Profile step
                result = self.time_profiler.profile_step(brain, step, world, agent)
                step_results.append(result)
                
                if step % 10 == 0:
                    print(f"  Completed step {step}/{steps}")
            
            total_time = time.time() - start_time
            
            # Collect final performance metrics
            memory_data = self.memory_profiler.profile_memory_usage(brain, world, agent)
            
            # Analyze performance
            analysis = self._analyze_step_results(step_results, total_time, memory_data)
            
            # Get time and memory summaries
            time_summary = self.time_profiler.get_time_profile_summary()
            memory_summary = self.memory_profiler.get_memory_profile_summary()
            
            return {
                'simulation_parameters': {
                    'steps': steps,
                    'neuron_count': brain.getTotalNeuronCount(),
                    'synapse_count': brain.getTotalSynapseCount(),
                },
                'performance_metrics': analysis,
                'time_analysis': time_summary,
                'memory_analysis': memory_summary,
                'overall_performance': self._calculate_overall_performance(
                    analysis, memory_data, time_summary, memory_summary),
            }
            
        finally:
            # Stop memory monitoring
            self.memory_profiler.stop_memory_monitoring()
    
    def _analyze_step_results(self, step_results: List[Dict[str, Any]],
                            total_time: float, memory_data: Dict[str, Any]) -> Dict[str, Any]:
        """Analyze step-by-step results"""
        if not step_results:
            return {}
        
        # Calculate step statistics
        step_durations = [r['step_duration'] for r in step_results]
        spike_counts = [r['spike_count'] for r in step_results]
        
        # Calculate efficiency metrics
        average_step_time = statistics.mean(step_durations) if step_durations else 0.0
        min_step_time = min(step_durations) if step_durations else 0.0
        max_step_time = max(step_durations) if step_durations else 0.0
        
        # Calculate spike efficiency
        total_spikes = sum(spike_counts)
        steps_per_spike = len(step_results) / max(1, total_spikes) if total_spikes > 0 else float('inf')
        
        # Calculate performance trends
        trend_analysis = {}
        for operation in self.time_profiler.time_profiles:
            if self.time_profiler.time_profiles[operation].call_count > 0:
                trend = self.time_profiler.get_trend_analysis(operation)
                if 'trend_direction' in trend:
                    trend_analysis[operation] = trend
        
        return {
            'step_analysis': {
                'average_step_time': average_step_time,
                'min_step_time': min_step_time,
                'max_step_time': max_step_time,
                'step_time_variance': statistics.variance(step_durations) 
                                     if len(step_durations) > 1 else 0.0,
                'total_simulation_time': total_time,
                'steps_per_second': len(step_results) / total_time if total_time > 0 else 0.0,
            },
            'spike_analysis': {
                'total_spikes': total_spikes,
                'average_spikes_per_step': statistics.mean(spike_counts) if spike_counts else 0.0,
                'spikes_per_second': total_spikes / total_time if total_time > 0 else 0.0,
                'spike_efficiency': total_spikes / len(step_results) if step_results else 0.0,
            },
            'neural_efficiency': {
                'neurons_per_step': statistics.mean([r['brain_neuron_count'] for r in step_results]),
                'synapses_per_step': statistics.mean([r['brain_synapse_count'] for r in step_results]),
                'time_per_neuron': average_step_time / statistics.mean([r['brain_neuron_count'] for r in step_results]) 
                                 if step_results else 0.0,
                'time_per_synapse': average_step_time / statistics.mean([r['brain_synapse_count'] for r in step_results]) 
                                   if step_results else 0.0,
            },
            'trend_analysis': trend_analysis,
            'bottleneck_analysis': self._identify_bottlenecks(step_results, trend_analysis),
        }
    
    def _calculate_overall_performance(self, step_analysis: Dict[str, Any],
                                     memory_data: Dict[str, Any],
                                     time_summary: Dict[str, Any],
                                     memory_summary: Dict[str, Any]) -> Dict[str, Any]:
        """Calculate overall performance score and grade"""
        # Calculate performance factors
        time_efficiency = self._calculate_time_efficiency(step_analysis)
        memory_efficiency = self._calculate_memory_efficiency(memory_data)
        neural_efficiency = self._calculate_neural_efficiency(step_analysis)
        
        # Calculate overall score (0-100)
        overall_score = (time_efficiency + memory_efficiency + neural_efficiency) / 3
        
        # Determine grade
        if overall_score >= 90:
            grade = 'Excellent'
        elif overall_score >= 80:
            grade = 'Good'
        elif overall_score >= 70:
            grade = 'Average'
        elif overall_score >= 60:
            grade = 'Below Average'
        else:
            grade = 'Needs Improvement'
        
        return {
            'overall_score': overall_score,
            'grade': grade,
            'time_efficiency': time_efficiency,
            'memory_efficiency': memory_efficiency,
            'neural_efficiency': neural_efficiency,
            'recommendations': self._generate_recommendations(
                step_analysis, memory_data, time_summary, memory_summary),
        }
    
    def _calculate_time_efficiency(self, step_analysis: Dict[str, Any]) -> float:
        """Calculate time efficiency score"""
        if 'step_analysis' not in step_analysis:
            return 0.0
        
        step_data = step_analysis['step_analysis']
        
        # Factors affecting time efficiency
        avg_step_time = step_data.get('average_step_time', 0.0)
        steps_per_second = step_data.get('steps_per_second', 0.0)
        
        # Normalize and combine factors (0-100 scale)
        time_score = min(100, (steps_per_second * 10))  # Scale based on steps/sec
        
        return time_score
    
    def _calculate_memory_efficiency(self, memory_data: Dict[str, Any]) -> float:
        """Calculate memory efficiency score"""
        # Memory efficiency based on memory usage per neuron/synapse
        memory_per_neuron = memory_data.get('memory_per_neuron', 0.0)
        memory_per_synapse = memory_data.get('memory_per_synapse', 0.0)
        
        # Target memory usage (adjustable based on typical values)
        target_memory_per_neuron = 0.1  # MB per neuron
        target_memory_per_synapse = 0.05  # MB per synapse
        
        # Calculate scores (higher is better)
        neuron_score = max(0, 100 - (memory_per_neuron / target_memory_per_neuron * 100))
        synapse_score = max(0, 100 - (memory_per_synapse / target_memory_per_synapse * 100))
        
        # Average the two scores
        return (neuron_score + synapse_score) / 2
    
    def _calculate_neural_efficiency(self, step_analysis: Dict[str, Any]) -> float:
        """Calculate neural efficiency score"""
        if 'spike_analysis' not in step_analysis or 'neural_efficiency' not in step_analysis:
            return 0.0
        
        spike_data = step_analysis['spike_analysis']
        neural_data = step_analysis['neural_efficiency']
        
        # Spike efficiency (higher is better)
        spike_efficiency = spike_data.get('spike_efficiency', 0.0) * 100
        
        # Neural density (higher is better)
        neurons_per_step = neural_data.get('neurons_per_step', 0)
        synapses_per_step = neural_data.get('synapses_per_step', 0)
        
        # Scale to 0-100 range
        density_score = min(100, (neurons_per_step + synapses_per_step) / 1000 * 100)
        
        return (spike_efficiency + density_score) / 2
    
    def _identify_bottlenecks(self, step_results: List[Dict[str, Any]],
                            trend_analysis: Dict[str, Any]) -> Dict[str, Any]:
        """Identify performance bottlenecks"""
        bottlenecks = {}
        
        # Check for step time variations
        step_times = [r['step_duration'] for r in step_results]
        if len(step_times) >= 5:
            recent_steps = step_times[-5:]
            if max(recent_steps) / min(recent_steps) > 3.0:  # 3x variation
                bottlenecks['step_time_variance'] = {
                    'issue': 'High step time variance',
                    'severity': 'medium',
                    'recommendation': 'Check for inconsistent spike processing',
                    'affected_percentage': ((max(recent_steps) - min(recent_steps)) / 
                                         min(recent_steps) * 100),
                }
        
        # Check for declining performance trends
        for operation, trend in trend_analysis.items():
            if trend.get('trend_direction') == 'increasing' and trend.get('r_squared', 0) > 0.7:
                bottlenecks[f'{operation}_degradation'] = {
                    'issue': f'Performance degradation in {operation}',
                    'severity': 'high',
                    'recommendation': f'Review {operation} implementation for optimization',
                    'slope': trend.get('slope', 0),
                    'r_squared': trend.get('r_squared', 0),
                }
        
        return bottlenecks
    
    def _generate_recommendations(self, step_analysis: Dict[str, Any],
                                 memory_data: Dict[str, Any],
                                 time_summary: Dict[str, Any],
                                 memory_summary: Dict[str, Any]) -> List[Dict[str, Any]]:
        """Generate performance optimization recommendations"""
        recommendations = []
        
        # Memory optimization recommendations
        if memory_data.get('memory_per_neuron', 0) > 0.2:  # MB per neuron
            recommendations.append({
                'category': 'memory_optimization',
                'priority': 'high',
                'issue': 'High memory usage per neuron',
                'recommendation': 'Consider using more efficient neuron data structures',
                'potential_improvement': '25-50%',
            })
        
        # Time optimization recommendations
        if step_analysis.get('step_analysis', {}).get('step_time_variance', 0) > 0.5:
            recommendations.append({
                'category': 'time_optimization',
                'priority': 'medium',
                'issue': 'High step time variance',
                'recommendation': 'Profile spike processing and plasticity updates',
                'potential_improvement': '15-30%',
            })
        
        # Neural efficiency recommendations
        if step_analysis.get('neural_efficiency', {}).get('time_per_neuron', 0) > 0.01:
            recommendations.append({
                'category': 'neural_efficiency',
                'priority': 'high',
                'issue': 'Low neurons processed per second',
                'recommendation': 'Optimize neural connectivity and spike propagation',
                'potential_improvement': '40-60%',
            })
        
        return recommendations


class NLMPerformanceReporter:
    """
    Comprehensive performance reporting for NLM simulations.
    
    Provides detailed performance reports including:
    - Performance dashboards
    - Comparison reports
    - Historical performance tracking
    - Automated performance alerts
    """
    
    def __init__(self, report_directory: str = "nlm_performance_reports"):
        self.report_directory = Path(report_directory)
        self.report_directory.mkdir(parents=True, exist_ok=True)
        
        # Performance history
        self.performance_history: List[Dict[str, Any]] = []
        
        # Performance benchmarks
        self.benchmarks: Dict[str, Dict[str, Any]] = {}
    
    def create_performance_report(self, simulation_name: str,
                                 performance_data: Dict[str, Any],
                                 brain: pynlm.Brain,
                                 world: Optional[pynlm.SimpleWorld] = None,
                                 agent: Optional[pynlm.AgentBrain] = None) -> str:
        """
        Create a comprehensive performance report.
        
        Args:
            simulation_name: Name of the simulation
            performance_data: Performance data from analyzer
            brain: The brain that was simulated
            world: Optional world state
            agent: Optional agent state
            
        Returns:
            Path to the generated report file
        """
        timestamp = datetime.now().strftime("%Y%m%d_%H%M%S")
        report_filename = f"performance_report_{simulation_name}_{timestamp}.json"
        report_path = self.report_directory / report_filename
        
        # Create report structure
        report = {
            'metadata': {
                'simulation_name': simulation_name,
                'timestamp': datetime.now().isoformat(),
                'brain_neuron_count': brain.getTotalNeuronCount(),
                'brain_synapse_count': brain.getTotalSynapseCount(),
                'world_configured': world is not None,
                'agent_configured': agent is not None,
            },
            'performance_data': performance_data,
            'summary': self._create_performance_summary(performance_data),
            'charts': self._create_chart_data(performance_data),
            'recommendations': performance_data.get('overall_performance', {}).get('recommendations', []),
        }
        
        # Save report
        with open(report_path, 'w') as f:
            json.dump(report, f, indent=2, default=str)
        
        print(f"Performance report saved to {report_path}")
        return str(report_path)
    
    def _create_performance_summary(self, performance_data: Dict[str, Any]) -> Dict[str, Any]:
        """Create a concise performance summary"""
        overall = performance_data.get('overall_performance', {})
        step_analysis = performance_data.get('performance_metrics', {}).get('step_analysis', {})
        spike_analysis = performance_data.get('performance_metrics', {}).get('spike_analysis', {})
        
        return {
            'simulation_efficiency': overall.get('overall_score', 0.0),
            'performance_grade': overall.get('grade', 'N/A'),
            'neural_throughput': {
                'steps_per_second': step_analysis.get('steps_per_second', 0.0),
                'spikes_per_second': spike_analysis.get('spikes_per_second', 0.0),
                'neurons_per_step': step_analysis.get('neurons_per_step', 0.0),
            },
            'resource_efficiency': {
                'memory_usage_mb': performance_data.get('memory_analysis', {}).get('brain_memory', 0.0),
                'memory_per_neuron': performance_data.get('memory_analysis', {}).get('memory_per_neuron', 0.0),
            },
            'bottlenecks_identified': len(performance_data.get('performance_metrics', {}).get('bottleneck_analysis', {})),
            'key_findings': self._extract_key_findings(performance_data),
        }
    
    def _create_chart_data(self, performance_data: Dict[str, Any]) -> Dict[str, Any]:
        """Create data for visualization charts"""
        return {
            'time_trends': self._create_time_trend_data(performance_data),
            'memory_usage': self._create_memory_usage_data(performance_data),
            'performance_distribution': self._create_performance_distribution_data(performance_data),
        }
    
    def _extract_key_findings(self, performance_data: Dict[str, Any]) -> List[str]:
        """Extract key performance findings"""
        findings = []
        
        overall = performance_data.get('overall_performance', {})
        if overall.get('overall_score', 0) < 60:
            findings.append("Overall performance requires significant improvement")
        
        step_analysis = performance_data.get('performance_metrics', {}).get('step_analysis', {})
        if step_analysis.get('step_time_variance', 0) > 0.5:
            findings.append("High variance in step times indicates inconsistent processing")
        
        spike_analysis = performance_data.get('performance_metrics', {}).get('spike_analysis', {})
        if spike_analysis.get('spike_efficiency', 0) < 0.1:
            findings.append("Low spike efficiency - consider optimizing spike processing")
        
        if len(performance_data.get('performance_metrics', {}).get('bottleneck_analysis', {})) > 0:
            findings.append("Performance bottlenecks identified")
        
        return findings if findings else ["Performance within expected range"]
    
    def _create_time_trend_data(self, performance_data: Dict[str, Any]) -> Dict[str, Any]:
        """Create data for time trend visualization"""
        return {
            'labels': [f'Step {i}' for i in range(1, 21)],  # Example
            'step_times': [0.001 * (i + 1) for i in range(20)],  # Example
            'memory_usage': [10 + i * 0.1 for i in range(20)],  # Example
        }
    
    def _create_memory_usage_data(self, performance_data: Dict[str, Any]) -> Dict[str, Any]:
        """Create data for memory usage visualization"""
        return {
            'components': ['Brain', 'World', 'Agent'],
            'memory_mb': [performance_data.get('memory_analysis', {}).get('brain_memory', 0.0),
                          performance_data.get('memory_analysis', {}).get('world_memory', 0.0),
                          performance_data.get('memory_analysis', {}).get('agent_memory', 0.0)],
        }
    
    def _create_performance_distribution_data(self, performance_data: Dict[str, Any]) -> Dict[str, Any]:
        """Create data for performance distribution visualization"""
        return {
            'operation_names': list(performance_data.get('time_analysis', {}).keys()),
            'time_consumption': list(performance_data.get('time_analysis', {}).values()),
            'average_times': [performance_data.get('time_analysis', {}).get(op, {}).get('average_time', 0.0)
                            for op in performance_data.get('time_analysis', {}).keys()],
        }
    
    def add_performance_to_history(self, simulation_name: str,
                                  performance_data: Dict[str, Any]):
        """Add performance data to history for comparison"""
        history_entry = {
            'simulation_name': simulation_name,
            'timestamp': datetime.now().isoformat(),
            'performance_data': performance_data,
            'summary': self._create_performance_summary(performance_data),
        }
        
        self.performance_history.append(history_entry)
        
        # Keep only recent history (last 50 entries)
        if len(self.performance_history) > 50:
            self.performance_history = self.performance_history[-50:]
    
    def get_performance_comparison(self, simulation_name: str = None) -> Dict[str, Any]:
        """
        Get performance comparison between simulations.
        
        Args:
            simulation_name: If provided, compare against this simulation
            
        Returns:
            Performance comparison results
        """
        if not self.performance_history:
            return {'error': 'No performance history available'}
        
        # Filter by simulation name if specified
        history = self.performance_history
        if simulation_name:
            history = [h for h in history if h['simulation_name'] == simulation_name]
        
        if not history:
            return {'error': 'No matching simulations found'}
        
        # Extract performance metrics for comparison
        comparison_data = {}
        for entry in history:
            sim_name = entry['simulation_name']
            comparison_data[sim_name] = {
                'score': entry['summary']['simulation_efficiency'],
                'grade': entry['summary']['performance_grade'],
                'steps_per_second': entry['performance_data']['performance_metrics']['step_analysis']['steps_per_second'],
                'memory_usage': entry['performance_data']['memory_analysis']['brain_memory'],
                'bottlenecks_count': len(entry['performance_data']['performance_metrics']['bottleneck_analysis']),
            }
        
        return {
            'simulations': comparison_data,
            'best_performer': max(comparison_data.items(), key=lambda x: x[1]['score'], default=(None, None)),
            'average_performance': self._calculate_average_performance(comparison_data),
        }
    
    def _calculate_average_performance(self, comparison_data: Dict[str, Any]) -> Dict[str, Any]:
        """Calculate average performance across simulations"""
        if not comparison_data:
            return {}
        
        scores = [data['score'] for data in comparison_data.values()]
        steps_per_second = [data['steps_per_second'] for data in comparison_data.values()]
        memory_usage = [data['memory_usage'] for data in comparison_data.values()]
        
        return {
            'average_score': statistics.mean(scores) if scores else 0,
            'average_steps_per_second': statistics.mean(steps_per_second) if steps_per_second else 0,
            'average_memory_usage': statistics.mean(memory_usage) if memory_usage else 0,
            'simulation_count': len(comparison_data),
        }


# Convenience functions
def run_performance_analysis(brain: pynlm.Brain,
                           world: Optional[pynlm.SimpleWorld] = None,
                           agent: Optional[pynlm.AgentBrain] = None,
                           steps: int = 100) -> Dict[str, Any]:
    """
    Run comprehensive performance analysis on NLM simulation.
    
    Args:
        brain: The brain to analyze
        world: Optional world state
        agent: Optional agent state
        steps: Number of simulation steps to analyze
        
    Returns:
        Comprehensive performance analysis results
    """
    analyzer = PerformanceAnalyzer()
    return analyzer.analyze_simulation_performance(brain, world, agent, steps)


def create_performance_report(simulation_name: str,
                              brain: pynlm.Brain,
                              performance_data: Dict[str, Any],
                              world: Optional[pynlm.SimpleWorld] = None,
                              agent: Optional[pynlm.AgentBrain] = None) -> str:
    """
    Create a performance report for an NLM simulation.
    
    Args:
        simulation_name: Name of the simulation
        brain: The brain that was simulated
        performance_data: Performance data to include in report
        world: Optional world state
        agent: Optional agent state
        
    Returns:
        Path to the generated report file
    """
    reporter = NLMPerformanceReporter()
    return reporter.create_performance_report(simulation_name, performance_data, brain, world, agent)


def run_performance_benchmark(brain_config: pynlm.Config,
                             benchmark_name: str = "default") -> Dict[str, Any]:
    """
    Run a performance benchmark with specified configuration.
    
    Args:
        brain_config: Brain configuration for benchmark
        benchmark_name: Name of the benchmark
        
    Returns:
        Benchmark results
    """
    # Create brain
    brain = pynlm.createBrain(brain_config)
    brain.initialize()
    
    # Create analyzer and run analysis
    analyzer = PerformanceAnalyzer()
    results = analyzer.analyze_simulation_performance(brain, steps=100)
    
    # Add to benchmark history
    reporter = NLMPerformanceReporter()
    reporter.add_performance_to_history(benchmark_name, results)
    
    return results
def run_performance_analysis(brain: pynlm.Brain,
                           world: Optional[pynlm.SimpleWorld] = None,
                           agent: Optional[pynlm.AgentBrain] = None,
                           steps: int = 100) -> Dict[str, Any]:
    """
    Run comprehensive performance analysis on NLM simulation.
    
    Args:
        brain: The brain to analyze
        world: Optional world state
        agent: Optional agent state
        steps: Number of simulation steps to analyze
        
    Returns:
        Comprehensive performance analysis results
    """
    analyzer = PerformanceAnalyzer()
    return analyzer.analyze_simulation_performance(brain, world, agent, steps)


def create_performance_report(simulation_name: str,
                              brain: pynlm.Brain,
                              performance_data: Dict[str, Any],
                              world: Optional[pynlm.SimpleWorld] = None,
                              agent: Optional[pynlm.AgentBrain] = None) -> str:
    """
    Create a performance report for an NLM simulation.
    
    Args:
        simulation_name: Name of the simulation
        brain: The brain that was simulated
        performance_data: Performance data to include in report
        world: Optional world state
        agent: Optional agent state
        
    Returns:
        Path to the generated report file
    """
    reporter = NLMPerformanceReporter()
    return reporter.create_performance_report(simulation_name, performance_data, brain, world, agent)


def run_performance_benchmark(brain_config: pynlm.Config,
                             benchmark_name: str = "default") -> Dict[str, Any]:
    """
    Run a performance benchmark with specified configuration.
    
    Args:
        brain_config: Brain configuration for benchmark
        benchmark_name: Name of the benchmark
        
    Returns:
        Benchmark results
    """
    # Create brain
    brain = pynlm.createBrain(brain_config)
    brain.initialize()
    
    # Create analyzer and run analysis
    analyzer = PerformanceAnalyzer()
    results = analyzer.analyze_simulation_performance(brain, steps=100)
    
    # Add to benchmark history
    reporter = NLMPerformanceReporter()
    reporter.add_performance_to_history(benchmark_name, results)
    
    return results
if __name__ == "__main__":
    print("=== NLM Performance Profiling Utilities ===\\n")
    
    # Example usage
    print("1. Creating example NLM brain...")
    config = pynlm.createDefaultConfig()
    config.set("neuron_count", 1000)
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    print("2. Running performance analysis...")
    performance_results = run_performance_analysis(brain, steps=200)
    
    print("3. Creating performance report...")
    report_path = create_performance_report("performance_example", brain, performance_results)
    
    print("\\nPerformance analysis completed!")
    print(f"Results available in: {report_path}")
    
    # Display key findings
    print("\\nKey Performance Findings:")
    overall = performance_results.get('overall_performance', {})
    print(f"  Overall Performance Score: {overall.get('overall_score', 0):.1f}/100")
    print(f"  Performance Grade: {overall.get('grade', 'N/A')}")
    
    step_analysis = performance_results.get('performance_metrics', {}).get('step_analysis', {})
    spike_analysis = performance_results.get('performance_metrics', {}).get('spike_analysis', {})
    
    print(f"  Steps per Second: {step_analysis.get('steps_per_second', 0):.1f}")
    print(f"  Spikes per Second: {spike_analysis.get('spikes_per_second', 0):.1f}")
    print(f"  Bottlenecks Identified: {len(performance_results.get('performance_metrics', {}).get('bottleneck_analysis', {}))}")