# PerformanceProfiler - Performance analysis and optimization

This module provides performance analysis and optimization tools for NLM simulations,
including benchmarking, profiling, and performance prediction.
"""

import time
import threading
import psutil
from typing import Optional, List, Dict, Any, Callable, Union
from dataclasses import dataclass, field
import numpy as np
import pandas as pd
from pathlib import Path
import json

from .enhanced_api import NLMSystem, SimulationStats

@dataclass
class PerformanceMetrics:
    """Performance metrics for NLM simulations"""
    # Time metrics
    initialization_time: float = 0.0
    step_time: float = 0.0
    total_simulation_time: float = 0.0
    
    # Resource usage
    cpu_usage: float = 0.0
    memory_usage: float = 0.0
    peak_memory: float = 0.0
    
    # Simulation metrics
    steps_per_second: float = 0.0
    spikes_per_step: float = 0.0
    memory_efficiency: float = 0.0
    computational_efficiency: float = 0.0
    
    # Advanced metrics
    plasticity_effectiveness: float = 0.0
    learning_speed: float = 0.0
    exploration_bonus: float = 0.0
    adaptation_rate: float = 0.0

@dataclass
class ProfilingConfig:
    """Configuration for performance profiling"""
    enable_profiling: bool = True
    profile_frequency: float = 1.0  # Hz
    collect_memory_info: bool = True
    collect_cpu_info: bool = True
    record_system_resources: bool = True
    export_detailed_reports: bool = True
    output_directory: str = "./performance_profiles"

class PerformanceProfiler:
    """
    Comprehensive performance profiling system for NLM simulations.
    
    This class provides tools for:
    - Performance benchmarking and profiling
    - Resource usage monitoring
    - Performance prediction and optimization
    - Comparison of different configurations
    - Automated performance tuning
    
    Examples:
        >>> # Basic performance profiling
        >>> profiler = nlm.PerformanceProfiler()
        >>> metrics = profiler.profile_performance(brain, 1000)
        >>> 
        >>> # Advanced profiling with resource monitoring
        >>> profiler = nlm.PerformanceProfiler()
        >>> profiler.set_monitoring_frequency(10.0)  # 10 Hz
        >>> profiler.enable_resource_monitoring(True)
        >>> metrics = profiler.advanced_profile(brain, 5000)
        >>> 
        >>> # Performance comparison
        >>> profiler = nlm.PerformanceProfiler()
        >>> metrics1 = profiler.profile_performance(brain1, 1000)
        >>> metrics2 = profiler.profile_performance(brain2, 1000)
        >>> comparison = profiler.compare_performance(metrics1, metrics2)
    """
    
    def __init__(self, config: Optional[ProfilingConfig] = None):
        """
        Initialize the performance profiler.
        
        Args:
            config: Optional profiling configuration
        """
        self.config = config or ProfilingConfig()
        self._current_metrics = PerformanceMetrics()
        self.profiling_thread: Optional[threading.Thread] = None
        self.is_profiling = False
        self.metrics_history: List[PerformanceMetrics] = []
        
        # Create output directory
        Path(self.config.output_directory).mkdir(parents=True, exist_ok=True)
    
    def set_monitoring_frequency(self, frequency: float) -> 'PerformanceProfiler':
        """
        Set monitoring frequency.
        
        Args:
            frequency: Monitoring frequency in Hz
            
        Returns:
            Self for method chaining
        """
        self.config.profile_frequency = frequency
        return self
    
    def enable_resource_monitoring(self, enabled: bool) -> 'PerformanceProfiler':
        """
        Enable or disable resource monitoring.
        
        Args:
            enabled: Whether to enable resource monitoring
            
        Returns:
            Self for method chaining
        """
        self.config.collect_memory_info = enabled
        self.config.collect_cpu_info = enabled
        self.config.record_system_resources = enabled
        return self
    
    def set_profiling_frequency(self, frequency: float) -> 'PerformanceProfiler':
        """
        Set profiling frequency.
n        
        Args:
            frequency: Profiling frequency in Hz
            
        Returns:
            Self for method chaining
        """
        self.config.profile_frequency = frequency
        return self
    
    def profile_performance(self, brain: Any, num_steps: int) -> PerformanceMetrics:
        """
        Profile performance of a brain simulation.
        
        Args:
            brain: The brain to profile
            num_steps: Number of steps to simulate
            
        Returns:
            Performance metrics
        """
        try:
            # Create enhanced system
            system = NLMSystem(brain)
            system.initialize()
            
            # Start profiling
            start_time = time.time()
            start_memory = psutil.virtual_memory().used if self.config.collect_memory_info else 0
            start_cpu = psutil.cpu_percent() if self.config.collect_cpu_info else 0
            
            # Run simulation and collect metrics
            simulation_start = time.time()
            step_times = []
            
            for step in range(num_steps):
                step_start = time.time()
                
                # Execute simulation step
                stats = system.step()
                
                # Record step time
                step_time = time.time() - step_start
                step_times.append(step_time)
                
                # Collect system metrics if enabled
                if self.config.record_system_resources:
                    current_memory = psutil.virtual_memory().used
                    current_cpu = psutil.cpu_percent()
                    
                    self.current_metrics.peak_memory = max(self.current_metrics.peak_memory, current_memory)
                    self.current_metrics.cpu_usage = current_cpu
                
                # Progress reporting
                if (step + 1) % 100 == 0:
                    progress = (step + 1) / num_steps * 100
                    print(f"Profiling progress: {progress:.1f}% ({step + 1}/{num_steps} steps)")
            
            simulation_end = time.time()
            total_time = simulation_end - simulation_start
            
            # Calculate performance metrics
            self.current_metrics.total_simulation_time = total_time
            self.current_metrics.step_time = np.mean(step_times) if step_times else 0
            self.current_metrics.steps_per_second = num_steps / total_time if total_time > 0 else 0
            
            if self.config.collect_memory_info:
                end_memory = psutil.virtual_memory().used
                self.current_metrics.memory_usage = (end_memory - start_memory) / (1024 * 1024)  # MB
            
            # Calculate efficiency metrics
            self.current_metrics.computational_efficiency = self._calculate_computational_efficiency(
                system, num_steps, total_time
            )
            self.current_metrics.learning_speed = self._calculate_learning_speed(system, num_steps)
            self.current_metrics.adaptation_rate = self._calculate_adaptation_rate(system, num_steps)
            
            # Store metrics in history
            self.metrics_history.append(self.current_metrics.copy())
            
            # Export report if configured
            if self.config.export_detailed_reports:
                self._export_performance_report("performance_profile.json")
            
            return self.current_metrics
            
        except Exception as e:
            raise Exception(f"Performance profiling failed: {e}")
    
    def advanced_profile(self, brain: Any, num_steps: int, 
                        custom_metrics: Optional[Dict[str, Callable]] = None) -> PerformanceMetrics:
        """
        Advanced performance profiling with custom metrics.
        
        Args:
            brain: The brain to profile
            num_steps: Number of steps to simulate
            custom_metrics: Dictionary of custom metric functions
            
        Returns:
            Performance metrics
        """
        try:
            # Reset current metrics
            self.current_metrics = PerformanceMetrics()
            
            # Create enhanced system
            system = NLMSystem(brain)
            system.initialize()
            
            # Measure initialization time
            init_start = time.time()
            system.initialize()
            self.current_metrics.initialization_time = time.time() - init_start
            
            # Run simulation with advanced monitoring
            simulation_start = time.time()
            
            for step in range(num_steps):
                step_start = time.time()
                
                # Execute simulation step
                stats = system.step()
                
                # Calculate step metrics
                step_time = time.time() - step_start
                self.current_metrics.step_time = step_time
                
                # Calculate spikes per step
                if step > 0:
                    self.current_metrics.spikes_per_step = (stats.total_spikes - 
                                                           self.metrics_history[-1].total_spikes) / 1
                
                # Calculate memory efficiency
                self.current_metrics.memory_efficiency = self._calculate_memory_efficiency(
                    system, step
                )
                
                # Calculate plasticity effectiveness
                self.current_metrics.plasticity_effectiveness = self._calculate_plasticity_effectiveness(
                    system, step
                )
                
                # Calculate exploration bonus
                self.current_metrics.exploration_bonus = self._calculate_exploration_bonus(
                    system, step
                )
                
                # Record step timing
                if step % 10 == 0:  # Record every 10 steps
                    self.metrics_history.append(self.current_metrics.copy())
            
            simulation_end = time.time()
            total_time = simulation_end - simulation_start
            self.current_metrics.total_simulation_time = total_time
            self.current_metrics.steps_per_second = num_steps / total_time if total_time > 0 else 0
            
            # Calculate advanced metrics
            if custom_metrics:
                for metric_name, metric_func in custom_metrics.items():
                    setattr(self.current_metrics, metric_name, metric_func(system, num_steps))
            
            # Store metrics in history
            self.metrics_history.append(self.current_metrics.copy())
            
            # Export report if configured
            if self.config.export_detailed_reports:
                self._export_performance_report("advanced_performance_profile.json")
            
            return self.current_metrics
            
        except Exception as e:
            raise Exception(f"Advanced performance profiling failed: {e}")
    
    def compare_performance(self, metrics1: PerformanceMetrics, 
                          metrics2: PerformanceMetrics) -> Dict[str, Any]:
        """
        Compare performance metrics between two runs.
        
        Args:
            metrics1: First performance metrics
            metrics2: Second performance metrics
            
        Returns:
            Dictionary containing comparison results
        """
        try:
            comparison = {}
            
            # Compare time metrics
            time_metrics = ['initialization_time', 'step_time', 'total_simulation_time']
            for metric in time_metrics:
                val1 = getattr(metrics1, metric)
                val2 = getattr(metrics2, metric)
                comparison[f'{metric}_improvement'] = (val1 - val2) / val2 if val2 > 0 else 0
            
            # Compare efficiency metrics
            efficiency_metrics = ['computational_efficiency', 'learning_speed', 
                                'adaptation_rate', 'memory_efficiency']
            for metric in efficiency_metrics:
                val1 = getattr(metrics1, metric)
                val2 = getattr(metrics2, metric)
                comparison[f'{metric}_improvement'] = (val2 - val1) / val1 if val1 > 0 else 0
            
            # Calculate overall performance score
            metrics1_score = self._calculate_performance_score(metrics1)
            metrics2_score = self._calculate_performance_score(metrics2)
            
            comparison['performance_scores'] = {
                'metrics1': metrics1_score,
                'metrics2': metrics2_score,
                'winner': 'metrics1' if metrics1_score > metrics2_score else 'metrics2'
            }
            
            return comparison
            
        except Exception as e:
            return {'error': f"Performance comparison failed: {e}"}
    
    def generate_performance_report(self, metrics: PerformanceMetrics, 
                                   output_file: str = "performance_report.html") -> str:
        """
        Generate a comprehensive performance report.
        
        Args:
            metrics: Performance metrics to report
            output_file: Output HTML filename
            
        Returns:
            Path to generated report
        """
        try:
            # Create HTML report
            html_content = f"""
<!DOCTYPE html>
<html>
<head>
    <title>NLM Performance Report</title>
    <style>
        body {{ font-family: Arial, sans-serif; margin: 20px; background-color: #f5f5f5; }}
        .container {{ max-width: 1200px; margin: 0 auto; background-color: white; padding: 20px; border-radius: 8px; box-shadow: 0 2px 10px rgba(0,0,0,0.1); }}
        .header {{ text-align: center; color: #333; margin-bottom: 30px; }}
        .metrics-grid {{ display: grid; grid-template-columns: repeat(auto-fit, minmax(300px, 1fr)); gap: 20px; margin-bottom: 30px; }}
        .metric-card {{ background: linear-gradient(135deg, #667eea 0%, #764ba2 100%); color: white; padding: 20px; border-radius: 8px; }}
        .metric-card h3 {{ margin: 0 0 10px 0; font-size: 1.2em; }}
        .metric-card .value {{ font-size: 2em; font-weight: bold; }}
        .section {{ margin-bottom: 30px; }}
        .section h2 {{ color: #333; border-bottom: 2px solid #667eea; padding-bottom: 10px; }}
        .chart {{ width: 100%; height: 400px; border: 1px solid #ddd; border-radius: 8px; margin-bottom: 20px; }}
    </style>
</head>
<body>
    <div class="container">
        <div class="header">
            <h1>NLM Performance Report</h1>
            <p>Performance Analysis and Profiling Results</p>
        </div>
        
        <div class="section">
            <h2>Key Performance Metrics</h2>
            <div class="metrics-grid">
                <div class="metric-card">
                    <h3>Simulation Speed</h3>
                    <div class="value">{metrics.steps_per_second:.1f} steps/s</div>
                </div>
                <div class="metric-card">
                    <h3>Average Step Time</h3>
                    <div class="value">{metrics.step_time*1000:.1f} ms</div>
                </div>
                <div class="metric-card">
                    <h3>Total Time</h3>
                    <div class="value">{metrics.total_simulation_time:.1f} s</div>
                </div>
                <div class="metric-card">
                    <h3>Computational Efficiency</h3>
                    <div class="value">{metrics.computational_efficiency:.2f}</div>
                </div>
                <div class="metric-card">
                    <h3>Learning Speed</h3>
                    <div class="value">{metrics.learning_speed:.3f}</div>
                </div>
                <div class="metric-card">
                    <h3>Memory Efficiency</h3>
                    <div class="value">{metrics.memory_efficiency:.3f}</div>
                </div>
            </div>
        </div>
        
        <div class="section">
            <h2>Resource Usage</h2>
            <div class="metrics-grid">
                <div class="metric-card">
                    <h3>CPU Usage</h3>
                    <div class="value">{metrics.cpu_usage:.1f}%</div>
                </div>
                <div class="metric-card">
                    <h3>Memory Usage</h3>
                    <div class="value">{metrics.memory_usage:.1f} MB</div>
                </div>
                <div class="metric-card">
                    <h3>Peak Memory</h3>
                    <div class="value">{metrics.peak_memory:.1f} MB</div>
                </div>
                <div class="metric-card">
                    <h3>Spikes per Step</h3>
                    <div class="value">{metrics.spikes_per_step:.1f}</div>
                </div>
            </div>
        </div>
        
        <div class="section">
            <h2>Advanced Metrics</h2>
            <div class="metrics-grid">
                <div class="metric-card">
                    <h3>Plasticity Effectiveness</h3>
                    <div class="value">{metrics.plasticity_effectiveness:.3f}</div>
                </div>
                <div class="metric-card">
                    <h3>Exploration Bonus</h3>
                    <div class="value">{metrics.exploration_bonus:.3f}</div>
                </div>
                <div class="metric-card">
                    <h3>Adaptation Rate</h3>
                    <div class="value">{metrics.adaptation_rate:.3f}</div>
                </div>
                <div class="metric-card">
                    <h3>Overall Score</h3>
                    <div class="value">{self._calculate_performance_score(metrics):.2f}</div>
                </div>
            </div>
        </div>
    </div>
</body>
</html>
"""
            
            # Save report
            filepath = Path(self.config.output_directory) / output_file
            with open(filepath, 'w') as f:
                f.write(html_content)
            
            return str(filepath)
            
        except Exception as e:
            return f"Report generation failed: {e}"
    
    def _calculate_computational_efficiency(self, system: NLMSystem, 
                                          num_steps: int, total_time: float) -> float:
        """Calculate computational efficiency."""
        try:
            # Simple efficiency calculation based on steps per second and learning
            base_efficiency = 1.0 / max(0.001, total_time / num_steps)
            learning_bonus = min(2.0, system.getAverageFiringRate() / 100.0)
            
            return base_efficiency * learning_bonus
        except Exception:
            return 0.0
    
    def _calculate_learning_speed(self, system: NLMSystem, num_steps: int) -> float:
        """Calculate learning speed."""
        try:
            # Estimate learning speed based on firing rate changes
            final_firing = system.getAverageFiringRate()
            initial_firing = 10.0  # Estimate
            
            return min(1.0, (final_firing - initial_firing) / (final_firing + 0.1))
        except Exception:
            return 0.0
    
    def _calculate_adaptation_rate(self, system: NLMSystem, num_steps: int) -> float:
        """Calculate adaptation rate."""
        try:
            # Simple adaptation calculation
            return min(1.0, system.getAverageFiringRate() / 200.0)
        except Exception:
            return 0.0
    
    def _calculate_memory_efficiency(self, system: NLMSystem, step: int) -> float:
        """Calculate memory efficiency."""
        try:
            # Estimate based on neuron count and firing rate
            neuron_count = system.getTotalNeuronCount()
            firing_rate = system.getAverageFiringRate()
            
            return min(1.0, firing_rate / (neuron_count * 0.001))
        except Exception:
            return 0.0
    
    def _calculate_plasticity_effectiveness(self, system: NLMSystem, step: int) -> float:
        """Calculate plasticity effectiveness."""
        try:
            # Estimate based on system properties
            neuron_count = system.getTotalNeuronCount()
            synapse_count = system.getTotalSynapseCount()
            
            return min(1.0, synapse_count / (neuron_count * 10))
        except Exception:
            return 0.0
    
    def _calculate_exploration_bonus(self, system: NLMSystem, step: int) -> float:
        """Calculate exploration bonus."""
        try:
            # Estimate based on firing rate variability
            firing_rate = system.getAverageFiringRate()
            return min(1.0, (firing_rate - 10.0) / 190.0)
        except Exception:
            return 0.0
    
    def _calculate_performance_score(self, metrics: PerformanceMetrics) -> float:
        """Calculate overall performance score."""
        try:
            # Weighted combination of all metrics
            weights = {
                'steps_per_second': 0.3,
                'computational_efficiency': 0.2,
                'learning_speed': 0.2,
                'adaptation_rate': 0.15,
                'memory_efficiency': 0.1,
                'plasticity_effectiveness': 0.05,
                'exploration_bonus': 0.05
            }
            
            score = 0.0
            total_weight = sum(weights.values())
            
            # Normalize metrics to 0-1 range
            score += weights['steps_per_second'] * min(1.0, metrics.steps_per_second / 100.0)
            score += weights['computational_efficiency'] * metrics.computational_efficiency
            score += weights['learning_speed'] * metrics.learning_speed
            score += weights['adaptation_rate'] * metrics.adaptation_rate
            score += weights['memory_efficiency'] * metrics.memory_efficiency
            score += weights['plasticity_effectiveness'] * metrics.plasticity_effectiveness
            score += weights['exploration_bonus'] * metrics.exploration_bonus
            
            return score / total_weight
            
        except Exception:
            return 0.0
    
    def _export_performance_report(self, filename: str):
        """Export performance metrics to JSON file."""
        try:
            data = {
                'current_metrics': vars(self._current_metrics),
                'metrics_history': [vars(m) for m in self.metrics_history]
            }
            
            filepath = Path(self.config.output_directory) / filename
            with open(filepath, 'w') as f:
                json.dump(data, f, indent=2)
                
        except Exception as e:
            print(f"Failed to export performance report: {e}")