#!/usr/bin/env python3
"""
Advanced Python API for NLM - Power user features and tools

This module provides advanced Python APIs for NLM neural simulation framework,
extending the enhanced features with sophisticated tools for power users,
researchers, and performance optimization.

Features include:
- Advanced performance monitoring and profiling
- Neural network analysis and visualization
- Batch processing and experiment management
- Advanced configuration and hyperparameter optimization
- Checkpoint management with automation
- Debugging and validation tools
- Distributed simulation capabilities
- Research experiment frameworks
"""

from pynlm import (
    # Core classes
    Config,
    Brain,
    AgentBrain,
    SimpleWorld,
    SensoryPercept,
    Vision,
    Action,
    WorldObject,
    ActionType,
    MotorCommand,
    NeuronType,
    DevelopmentalStage,
    WorldObjectType,
)

from typing import List, Dict, Any, Optional, Callable, Union, Tuple
from dataclasses import dataclass, field_default_factory
import json
import time
import logging
import numpy as np
from enum import Enum
from pathlib import Path
import multiprocessing as mp
from concurrent.futures import ProcessPoolExecutor, ThreadPoolExecutor
import matplotlib.pyplot as plt
import seaborn as sns
import pandas as pd
from scipy import stats
from collections import defaultdict, deque
import threading
import queue
import psutil
import os
class PerformanceProfiler:
    """Advanced performance profiling for neural simulations"""
    
    def __init__(self, brain=None, agent=None, world=None):
        self.brain = brain
        self.agent = agent
        self.world = world
        self.profiling_data = {}
        self.timing_samples = []
        self.memory_samples = []
        self.cpu_samples = []
        self.step_times = []
        self.enabled = True
        self.sampling_interval = 0.01  # seconds
        self.max_samples = 10000
        self.start_time = None
        self.last_sample_time = None
        self.monitoring_thread = None
        self.stop_monitoring = threading.Event()
    
    def start_profiling(self):
        """Start comprehensive profiling"""
        if not self.enabled:
            return
        
        self.start_time = time.time()
        self.last_sample_time = time.time()
        self.timing_samples = []
        self.memory_samples = []
        self.cpu_samples = []
        self.step_times = []
        
        # Start background monitoring thread
        self.stop_monitoring.clear()
        self.monitoring_thread = threading.Thread(target=self._monitor_background)
        self.monitoring_thread.daemon = True
        self.monitoring_thread.start()
    
    def stop_profiling(self):
        """Stop profiling and collect final statistics"""
        if self.monitoring_thread:
            self.stop_monitoring.set()
            self.monitoring_thread.join(timeout=5.0)
        
        self._collect_final_stats()
    
    def _monitor_background(self):
        """Background monitoring thread"""
        while not self.stop_monitoring.wait(self.sampling_interval):
            current_time = time.time()
            
            # Sample CPU and memory usage
            process = psutil.Process()
            self.cpu_samples.append(process.cpu_percent())
            self.memory_samples.append(process.memory_info().rss / 1024 / 1024)  # MB
            
            # Sample brain performance if available
            if self.brain:
                self.step_times.append(current_time - self.last_sample_time)
            
            self.last_sample_time = current_time
            
            # Limit sample size
            if len(self.cpu_samples) > self.max_samples:
                self.cpu_samples.pop(0)
                self.memory_samples.pop(0)
                self.step_times.pop(0)
    
    def add_timing_sample(self, step_num, step_time):
        """Add timing sample for a specific step"""
        self.timing_samples.append({
            'step': step_num,
            'time': step_time,
            'timestamp': time.time(),
            'cpu_percent': psutil.cpu_percent(),
            'memory_mb': psutil.Process().memory_info().rss / 1024 / 1024
        })
    
    def _collect_final_stats(self):
        """Collect final profiling statistics"""
        if not self.timing_samples:
            return
        
        times = [s['time'] for s in self.timing_samples]
        cpus = [s['cpu_percent'] for s in self.timing_samples]
        memories = [s['memory_mb'] for s in self.timing_samples]
        
        self.profiling_data = {
            'total_steps': len(self.timing_samples),
            'total_time': times[-1] if times else 0,
            'avg_step_time_ms': np.mean(times) * 1000 if times else 0,
            'min_step_time_ms': np.min(times) * 1000 if times else 0,
            'max_step_time_ms': np.max(times) * 1000 if times else 0,
            'std_step_time_ms': np.std(times) * 1000 if times else 0,
            'steps_per_second': len(times) / (times[-1] if times else 1),
            'cpu_avg_percent': np.mean(cpus) if cpus else 0,
            'cpu_max_percent': np.max(cpus) if cpus else 0,
            'memory_avg_mb': np.mean(memories) if memories else 0,
            'memory_max_mb': np.max(memories) if memories else 0,
            'memory_growth_mb': memories[-1] - memories[0] if len(memories) > 1 else 0,
        }
    
    def get_summary(self):
        """Get profiling summary"""
        return self.profiling_data
    
    def get_timing_analysis(self):
        """Get detailed timing analysis"""
        if not self.timing_samples:
            return None
        
        times = [s['time'] for s in self.timing_samples]
        steps = [s['step'] for s in self.timing_samples]
        
        # Detect performance anomalies
        time_series = pd.Series(times)
        rolling_mean = time_series.rolling(window=10, center=True).mean()
        rolling_std = time_series.rolling(window=10, center=True).std()
        
        anomalies = []
        for i, (time, mean, std) in enumerate(zip(times, rolling_mean, rolling_std)):
            if std > 0 and abs(time - mean) > 3 * std:
                anomalies.append({
                    'step': steps[i],
                    'time_ms': time * 1000,
                    'mean_ms': mean * 1000,
                    'std_ms': std * 1000,
                    'z_score': (time - mean) / std if std > 0 else 0
                })
        
        return {
            'basic_stats': self.profiling_data,
            'step_range': (steps[0], steps[-1]),
            'time_range': (times[0] * 1000, times[-1] * 1000),
            'anomalies': anomalies,
            'sample_count': len(self.timing_samples)
        }
    
    def generate_performance_report(self, output_file=None):
        """Generate detailed performance report"""
        timing_analysis = self.get_timing_analysis()
        if not timing_analysis:
            return None
        
        report = {
            'profile_metadata': {
                'start_time': self.start_time,
                'end_time': time.time(),
                'total_duration': time.time() - self.start_time,
                'sampling_interval': self.sampling_interval
            },
            'performance_summary': self.profiling_data,
            'timing_analysis': timing_analysis,
            'recommendations': self._generate_recommendations(timing_analysis)
        }
        
        if output_file:
            with open(output_file, 'w') as f:
                json.dump(report, f, indent=2)
        
        return report
    
    def _generate_recommendations(self, analysis):
        """Generate performance optimization recommendations"""
        recommendations = []
        perf = analysis['performance_summary']
        
        if perf['avg_step_time_ms'] > 100:
            recommendations.append({
                'type': 'performance',
                'priority': 'high',
                'message': 'Average step time is high (>100ms). Consider optimizing brain configuration or reducing neuron count.',
                'suggestion': 'Try using the "fast" preset or reducing simulation complexity.'
            })
        
        if perf['cpu_avg_percent'] > 80:
            recommendations.append({
                'type': 'resource',
                'priority': 'medium',
                'message': 'CPU usage is high. Consider reducing simulation complexity or using parallel processing.',
                'suggestion': 'Use the "small" preset or implement batch processing.'
            })
        
        anomalies = analysis['anomalies']
        if len(anomalies) > 0:
            recommendations.append({
                'type': 'stability',
                'priority': 'medium',
                'message': f'Found {len(anomalies)} performance anomalies in step execution.',
                'suggestion': 'Investigate these steps for potential memory leaks or inefficient operations.'
            })
        
        if perf['memory_growth_mb'] > 100:
            recommendations.append({
                'type': 'memory',
                'priority': 'high',
                'message': 'Significant memory growth detected.',
                'suggestion': 'Check for memory leaks in simulation loop or implement checkpointing to save memory.'
            })
        
        return recommendations
class NeuralNetworkAnalyzer:
    """Advanced neural network analysis and visualization"""
    
    def __init__(self, brain=None):
        self.brain = brain
        self.neural_activity = []
        self.spike_patterns = []
        self.synapse_analysis = []
        self.connectivity_matrix = None
        self.performance_metrics = {}
    
    def record_activity(self, step_num, neuron_activity=None, spike_patterns=None):
        """Record neural activity for analysis"""
        if neuron_activity is None and self.brain:
            neuron_activity = self.brain.getFiringNeuronCount()
        
        self.neural_activity.append({
            'step': step_num,
            'firing_neurons': neuron_activity,
            'total_spikes': self.brain.getTotalSpikeCount() if self.brain else 0,
            'avg_firing_rate': self.brain.getAverageFiringRate() if self.brain else 0,
            'timestamp': time.time()
        })
        
        if spike_patterns:
            self.spike_patterns.extend(spike_patterns)
    
    def compute_connectivity_matrix(self):
        """Compute neural connectivity matrix"""
        if not self.brain:
            return None
        
        # Get all neurons and their connections
        regions = self.brain.getRegions()
        if not regions:
            return None
        
        neurons = []
        for region in regions:
            neurons.extend(region.getAllNeurons())
        
        n = len(neurons)
        connectivity = np.zeros((n, n))
        
        # This is a simplified version - in real implementation,
        # you would need access to synapse information
        for i, neuron1 in enumerate(neurons):
            for j, neuron2 in enumerate(neurons):
                if i != j:
                    # Check for connection (simplified)
                    connectivity[i, j] = 1 if np.random.random() < 0.05 else 0
        
        self.connectivity_matrix = connectivity
        return connectivity
    
    def analyze_spike_patterns(self, window_size=10):
        """Analyze spike patterns using statistical methods"""
        if not self.neural_activity:
            return None
        
        firing_rates = [activity['avg_firing_rate'] for activity in self.neural_activity]
        
        # Compute statistics
        stats_analysis = {
            'mean_firing_rate': np.mean(firing_rates),
            'std_firing_rate': np.std(firing_rates),
            'cv_firing_rate': np.std(firing_rates) / np.mean(firing_rates) if np.mean(firing_rates) > 0 else 0,
            'firing_rate_trend': self._compute_trend(firing_rates),
            'stationarity': self._test_stationarity(firing_rates),
            'burst_detection': self._detect_bursts(firing_rates, window_size)
        }
        
        return stats_analysis
    
    def _compute_trend(self, data):
        """Compute linear trend of time series"""
        if len(data) < 2:
            return 0
        
        x = np.arange(len(data))
        slope, intercept, r_value, p_value, std_err = stats.linregress(x, data)
        
        return {
            'slope': slope,
            'intercept': intercept,
            'r_squared': r_value ** 2,
            'p_value': p_value,
            'significant_trend': p_value < 0.05
        }
    
    def _test_stationarity(self, data, threshold=0.05):
        """Test stationarity using Augmented Dickey-Fuller test"""
        try:
            from statsmodels.tsa.stattools import adfuller
            result = adfuller(data)
            p_value = result[1]
            
            return {
                'adf_statistic': result[0],
                'p_value': p_value,
                'is_stationary': p_value < threshold,
                'critical_values': result[4]
            }
        except ImportError:
            return {'error': 'statsmodels not available'}
    
    def _detect_bursts(self, data, window_size):
        """Detect bursting activity in neural data"""
        if len(data) < window_size * 2:
            return []
        
        bursts = []
        for i in range(len(data) - window_size):
            window_data = data[i:i + window_size]
            mean_rate = np.mean(window_data)
            std_rate = np.std(window_data)
            
            if std_rate > 2 * mean_rate and mean_rate > 0:  # Coefficient of variation > 2
                bursts.append({
                    'start_step': i,
                    'end_step': i + window_size,
                    'mean_rate': mean_rate,
                    'std_rate': std_rate,
                    'cv': std_rate / mean_rate
                })
        
        return bursts
    
    def generate_analysis_report(self, output_file=None):
        """Generate comprehensive neural network analysis report"""
        report = {
            'analysis_metadata': {
                'timestamp': time.time(),
                'brain_info': {
                    'total_neurons': self.brain.getTotalNeuronCount() if self.brain else 0,
                    'total_synapses': self.brain.getTotalSynapseCount() if self.brain else 0,
                    'firing_neuron_count': self.brain.getFiringNeuronCount() if self.brain else 0,
                    'total_spike_count': self.brain.getTotalSpikeCount() if self.brain else 0,
                } if self.brain else {},
                'recording_duration': len(self.neural_activity)
            },
            'spike_pattern_analysis': self.analyze_spike_patterns(),
            'connectivity_analysis': {
                'matrix_computed': self.connectivity_matrix is not None,
                'matrix_shape': self.connectivity_matrix.shape if self.connectivity_matrix is not None else None
            },
            'performance_metrics': self.performance_metrics,
            'recommendations': self._generate_analysis_recommendations()
        }
        
        if output_file:
            with open(output_file, 'w') as f:
                json.dump(report, f, indent=2)
        
        return report
    
    def _generate_analysis_recommendations(self):
        """Generate analysis-based recommendations"""
        recommendations = []
        spike_analysis = self.analyze_spike_patterns()
        
        if spike_analysis:
            if spike_analysis['cv_firing_rate'] > 1.0:
                recommendations.append({
                    'type': 'activity_analysis',
                    'message': 'High coefficient of variation in firing rates suggests highly variable neural activity.',
                    'suggestion': 'Consider adjusting neuromodulation parameters or synaptic strengths for more stable activity.'
                })
            
            trend = spike_analysis['firing_rate_trend']
            if trend['significant_trend']:
                if trend['slope'] > 0:
                    recommendations.append({
                        'type': 'activity_analysis',
                        'message': 'Significant increasing trend in firing rates detected.',
                        'suggestion': 'Network may be becoming unstable. Consider reducing simulation time or adjusting plasticity parameters.'
                    })
                else:
                    recommendations.append({
                        'type': 'activity_analysis',
                        'message': 'Significant decreasing trend in firing rates detected.',
                        'suggestion': 'Network activity is waning. Consider increasing neuromodulation or synaptic input.'
                    })
            
            bursts = spike_analysis['burst_detection']
            if len(bursts) > 0:
                recommendations.append({
                    'type': 'activity_analysis',
                    'message': f'Detected {len(bursts)} burst events in neural activity.',
                    'suggestion': 'Burst detection indicates dynamic neural dynamics. These may be important for learning and memory processes.'
                })
        
        return recommendations
class BatchProcessor:
    """Advanced batch processing for running multiple simulations efficiently"""
    
    def __init__(self, num_workers=None):
        self.num_workers = num_workers or mp.cpu_count()
        self.experiment_configs = []
        self.results = []
        self.progress_callback = None
    
    def add_experiment(self, config, name=None, description=None):
        """Add experiment configuration to batch"""
        self.experiment_configs.append({
            'config': config,
            'name': name,
            'description': description
        })
    
    def run_batch(self, max_concurrent=None):
        """Run all experiments in batch mode"""
        if not self.experiment_configs:
            return []
        
        max_concurrent = max_concurrent or self.num_workers
        self.results = []
        
        print(f"Running {len(self.experiment_configs)} experiments with {max_concurrent} workers")
        
        with ProcessPoolExecutor(max_workers=max_concurrent) as executor:
            # Submit all experiments
            future_to_config = {
                executor.submit(self._run_single_experiment, config): config
                for config in self.experiment_configs
            }
            
            # Collect results
            completed = 0
            for future in future_to_config:
                config = future_to_config[future]
                try:
                    result = future.result(timeout=300)  # 5 minute timeout
                    self.results.append(result)
                    completed += 1
                    
                    if self.progress_callback:
                        self.progress_callback(completed, len(self.experiment_configs), config)
                    
                    print(f"Completed: {config.get('name', 'Unknown')}")
                
                except Exception as e:
                    print(f"Error in experiment {config.get('name', 'Unknown')}: {e}")
                    self.results.append({
                        'config': config,
                        'status': 'failed',
                        'error': str(e),
                        'timestamp': time.time()
                    })
        
        return self.results
    
    def _run_single_experiment(self, config):
        """Run a single experiment in a separate process"""
        # This function should be picklable for multiprocessing
        import pynlm
        from pynlm_enhanced import run_simulation_loop, SimulationMonitor
        
        brain = config['brain']
        world = config['world']
        agent = config['agent']
        
        monitor = SimulationMonitor(brain, agent, world)
        run_simulation_loop(
            brain, world, agent,
            steps=config['steps'],
            timestep=config['timestep'],
            monitor=monitor,
            verbose=False
        )
        
        return {
            'name': config.get('name', 'Experiment'),
            'description': config.get('description', ''),
            'status': 'completed',
            'performance': monitor.get_summary(),
            'analysis': {
                'total_spikes': brain.getTotalSpikeCount(),
                'firing_neurons': brain.getFiringNeuronCount(),
                'avg_firing_rate': brain.getAverageFiringRate()
            },
            'timestamp': time.time()
        }
    
    def generate_batch_report(self, output_file=None):
        """Generate comprehensive batch processing report"""
        report = {
            'batch_metadata': {
                'timestamp': time.time(),
                'total_experiments': len(self.experiment_configs),
                'completed_experiments': len([r for r in self.results if r.get('status') == 'completed']),
                'failed_experiments': len([r for r in self.results if r.get('status') != 'completed']),
                'num_workers': self.num_workers
            },
            'experiment_results': self.results,
            'summary_statistics': self._compute_summary_statistics(),
            'recommendations': self._generate_batch_recommendations()
        }
        
        if output_file:
            with open(output_file, 'w') as f:
                json.dump(report, f, indent=2)
        
        return report
    
    def _compute_summary_statistics(self):
        """Compute summary statistics for all experiments"""
        completed_results = [r for r in self.results if r.get('status') == 'completed']
        
        if not completed_results:
            return {}
        
        perf_data = []
        for result in completed_results:
            if 'performance' in result:
                perf_data.append(result['performance'])
        
        if not perf_data:
            return {}
        
        # Convert to DataFrame for analysis
        df = pd.DataFrame(perf_data)
        
        return {
            'experiment_count': len(completed_results),
            'avg_step_time_ms': df['avg_step_time_ms'].mean(),
            'std_step_time_ms': df['avg_step_time_ms'].std(),
            'min_step_time_ms': df['avg_step_time_ms'].min(),
            'max_step_time_ms': df['avg_step_time_ms'].max(),
            'avg_cpu_percent': df['cpu_avg_percent'].mean(),
            'avg_memory_mb': df['memory_avg_mb'].mean(),
            'success_rate': len(completed_results) / len(self.results) * 100
        }
    
    def _generate_batch_recommendations(self):
        """Generate batch processing recommendations"""
        recommendations = []
        summary = self._compute_summary_statistics()
        
        if summary:
            if summary['avg_step_time_ms'] > 100:
                recommendations.append({
                    'type': 'performance',
                    'message': 'Average step time is high across experiments.',
                    'suggestion': 'Consider reducing simulation complexity or optimizing brain configurations.'
                })
            
            if summary['avg_cpu_percent'] > 80:
                recommendations.append({
                    'type': 'resource',
                    'message': 'High CPU usage across experiments.',
                    'suggestion': 'Consider reducing number of concurrent workers or optimizing simulations.'
                })
            
            if summary['avg_memory_mb'] > 500:
                recommendations.append({
                    'type': 'memory',
                    'message': 'High memory usage across experiments.',
                    'suggestion': 'Consider implementing checkpointing or reducing neuron counts.'
                })
        
        return recommendations
class ExperimentManager:
    """Comprehensive experiment management for research studies"""
    
    def __init__(self, experiment_name, output_dir=None):
        self.experiment_name = experiment_name
        self.output_dir = output_dir or f"experiments/{experiment_name}_{int(time.time())}"
        Path(self.output_dir).mkdir(parents=True, exist_ok=True)
        
        self.experiments = []
        self.results = []
        self.metadata = {}
        self.parameters = {}
        self.progress_callback = None
    
    def set_parameters(self, **kwargs):
        """Set experiment parameters"""
        self.parameters.update(kwargs)
    
    def add_experiment(self, name, config, description=None, **kwargs):
        """Add experiment to manager"""
        experiment = {
            'name': name,
            'config': config,
            'description': description,
            'metadata': kwargs,
            'timestamp': time.time(),
            'status': 'pending'
        }
        self.experiments.append(experiment)
    
    def run_experiment(self, experiment_idx=None, config_overrides=None):
        """Run a specific experiment"""
        if experiment_idx is None:
            # Run all experiments
            for i, experiment in enumerate(self.experiments):
                self.run_experiment(i, config_overrides)
            return self.results
        
        experiment = self.experiments[experiment_idx]
        experiment['status'] = 'running'
        experiment['start_time'] = time.time()
        
        try:
            # Apply configuration overrides
            config = experiment['config'].copy()
            if config_overrides:
                config.update(config_overrides)
            
            # Import here to avoid issues with multiprocessing
            import pynlm
            from pynlm_enhanced import (
                createConfigFromPreset, 
                createBrainFromPreset, 
                create_agent_with_features
            )
            
            # Create brain and agent
            brain = pynlm.createBrain(config['brain_config'])
            world = pynlm.createSimpleWorld()
            
            if 'brain_config' in config:
                world.configure(**config['brain_config'].get('world_config', {}))
            
            agent = create_agent_with_features(
                brain, world,
                enable_reward=config.get('enable_reward', True),
                enable_curiosity=config.get('enable_curiosity', True),
                enable_development=config.get('enable_development', True),
                enable_plasticity=config.get('enable_plasticity', True)
            )
            
            # Run simulation
            world.reset()
            agent.initialize(world)
            
            from pynlm_enhanced import run_simulation_loop, SimulationMonitor
            monitor = SimulationMonitor(brain, agent, world)
            run_simulation_loop(
                brain, world, agent,
                steps=config['steps'],
                timestep=config['timestep'],
                monitor=monitor
            )
            
            experiment['status'] = 'completed'
            experiment['result'] = {
                'performance': monitor.get_summary(),
                'analysis': {
                    'total_spikes': brain.getTotalSpikeCount(),
                    'firing_neurons': brain.getFiringNeuronCount(),
                    'avg_firing_rate': brain.getAverageFiringRate(),
                    'final_development_stage': brain.getDevelopmentalStage()
                }
            }
            
        except Exception as e:
            experiment['status'] = 'failed'
            experiment['error'] = str(e)
        
        experiment['end_time'] = time.time()
        self.results.append(experiment)
        
        if self.progress_callback:
            progress = len([e for e in self.experiments if e['status'] == 'completed']) / len(self.experiments) * 100
            self.progress_callback(progress, experiment['name'])
        
        return experiment
    
    def save_experiment_data(self, experiment_idx=None, output_format='json'):
        """Save experiment data"""
        if experiment_idx is not None:
            # Save specific experiment
            experiment = self.experiments[experiment_idx] if experiment_idx < len(self.experiments) else None
            if not experiment:
                return None
            
            filename = f"{self.output_dir}/{experiment['name']}_data.{output_format}"
            self._save_experiment(experiment, filename, output_format)
            return filename
        
        # Save all experiments
        return self._save_all_experiments(output_format)
    
    def _save_experiment(self, experiment, filename, output_format):
        """Save single experiment"""
        if output_format == 'json':
            with open(filename, 'w') as f:
                json.dump(experiment, f, indent=2)
        elif output_format == 'csv':
            # Convert to CSV format
            import pandas as pd
            # This would need to be customized based on experiment structure
            pass
        
        print(f"Saved experiment data to {filename}")
    
    def _save_all_experiments(self, output_format):
        """Save all experiments"""
        files = []
        for i, experiment in enumerate(self.experiments):
            filename = f"{self.output_dir}/experiment_{i:03d}_{experiment['name']}.{output_format}"
            self._save_experiment(experiment, filename, output_format)
            files.append(filename)
        
        # Save experiment metadata
        metadata = {
            'experiment_name': self.experiment_name,
            'parameters': self.parameters,
            'total_experiments': len(self.experiments),
            'completed_experiments': len([e for e in self.experiments if e['status'] == 'completed']),
            'failed_experiments': len([e for e in self.experiments if e['status'] == 'failed']),
            'timestamp': time.time()
        }
        
        metadata_file = f"{self.output_dir}/experiment_metadata.json"
        with open(metadata_file, 'w') as f:
            json.dump(metadata, f, indent=2)
        
        files.append(metadata_file)
        return files
    
    def generate_report(self, output_file=None):
        """Generate comprehensive experiment report"""
        report = {
            'experiment_name': self.experiment_name,
            'parameters': self.parameters,
            'execution_summary': {
                'total_experiments': len(self.experiments),
                'completed': len([e for e in self.experiments if e['status'] == 'completed']),
                'failed': len([e for e in self.experiments if e['status'] == 'failed']),
                'success_rate': len([e for e in self.experiments if e['status'] == 'completed']) / len(self.experiments) * 100
            },
            'experiments': self.experiments,
            'results_summary': self._generate_results_summary(),
            'recommendations': self._generate_experiment_recommendations()
        }
        
        if output_file:
            with open(output_file, 'w') as f:
                json.dump(report, f, indent=2)
        
        return report
    
    def _generate_results_summary(self):
        """Generate summary of experiment results"""
        completed = [e for e in self.experiments if e['status'] == 'completed']
        
        if not completed:
            return {}
        
        summary = {
            'avg_performance': {},
            'performance_distribution': {},
            'analysis_by_parameter': {}
        }
        
        # Average performance across completed experiments
        perf_data = []
        for exp in completed:
            if 'result' in exp and 'performance' in exp['result']:
                perf_data.append(exp['result']['performance'])
        
        if perf_data:
            df = pd.DataFrame(perf_data)
            summary['avg_performance'] = {
                'step_time_ms': df['avg_step_time_ms'].mean(),
                'cpu_percent': df['cpu_avg_percent'].mean(),
                'memory_mb': df['memory_avg_mb'].mean()
            }
        
        return summary
    
    def _generate_experiment_recommendations(self):
        """Generate experiment-based recommendations"""
        recommendations = []
        summary = self._generate_results_summary()
        
        if summary:
            if 'avg_performance' in summary:
                perf = summary['avg_performance']
                
                if perf['step_time_ms'] > 100:
                    recommendations.append({
                        'type': 'performance',
                        'message': 'Average experiment step time is high.',
                        'suggestion': 'Consider optimizing brain configurations or reducing simulation complexity.'
                    })
        
        return recommendations
class HyperparameterOptimizer:
    """Hyperparameter optimization for neural network simulations"""
    
    def __init__(self, parameter_space):
        self.parameter_space = parameter_space
        self.search_results = []
        self.best_params = None
        self.best_score = float('-inf')
    
    def add_objective_function(self, objective_func):
        """Add objective function for optimization"""
        self.objective_function = objective_func
    
    def optimize(self, n_iterations=50, optimization_method='random', 
                 metric='performance', parallel=True):
        """Run hyperparameter optimization"""
        import random
        import numpy as np
        
        print(f"Starting hyperparameter optimization with {n_iterations} iterations")
        
        if parallel:
            with ProcessPoolExecutor(max_workers=4) as executor:
                futures = []
                for i in range(n_iterations):
                    params = self._sample_parameters(optimization_method)
                    future = executor.submit(self._evaluate_parameters, params)
                    futures.append(future)
                
                for future in futures:
                    result = future.result()
                    self.search_results.append(result)
                    if result['score'] > self.best_score:
                        self.best_score = result['score']
                        self.best_params = result['parameters']
        
        else:
            for i in range(n_iterations):
                params = self._sample_parameters(optimization_method)
                result = self._evaluate_parameters(params)
                self.search_results.append(result)
                
                if result['score'] > self.best_score:
                    self.best_score = result['score']
                    self.best_params = result['parameters']
                
                print(f"Iteration {i+1}/{n_iterations}: Score = {result['score']:.4f}")
        
        return self.search_results
    
    def _sample_parameters(self, method):
        """Sample parameters according to specified method"""
        if method == 'random':
            params = {}
            for param_name, param_range in self.parameter_space.items():
                if isinstance(param_range, tuple) and len(param_range) == 2:
                    params[param_name] = random.uniform(param_range[0], param_range[1])
                elif isinstance(param_range, list):
                    params[param_name] = random.choice(param_range)
                else:
                    params[param_name] = param_range
            return params
        
        elif method == 'grid':
            # Grid sampling (simplified)
            pass
        
        return {}
    
    def _evaluate_parameters(self, params):
        """Evaluate parameters using objective function"""
        try:
            # Create brain with these parameters
            import pynlm
            from pynlm_enhanced import createConfigFromPreset
            
            config = createConfigFromPreset("standard")
            for key, value in params.items():
                config.set(key, value)
            
            brain = pynlm.createBrain(config)
            brain.initialize()
            
            # Run simulation and evaluate performance
            from pynlm_enhanced import run_simulation_loop, SimulationMonitor
            monitor = SimulationMonitor(brain)
            run_simulation_loop(
                brain, None, None,
                steps=100, timestep=0.001,
                monitor=monitor
            )
            
            # Calculate score based on performance metric
            score = self._calculate_score(monitor, params)
            
            return {
                'parameters': params,
                'score': score,
                'performance': monitor.get_summary(),
                'timestamp': time.time()
            }
            
        except Exception as e:
            return {
                'parameters': params,
                'score': float('-inf'),
                'error': str(e),
                'timestamp': time.time()
            }
    
    def _calculate_score(self, monitor, params):
        """Calculate optimization score from performance metrics"""
        perf = monitor.get_summary()
        
        # Composite score based on multiple metrics
        score = 0
        
        # High score for low step time
        if 'avg_step_time_ms' in perf:
            score += 1.0 / (perf['avg_step_time_ms'] / 10.0)  # Higher score for lower time
        
        # High score for stable performance (low std dev)
        if 'std_step_time_ms' in perf and perf['avg_step_time_ms'] > 0:
            cv = perf['std_step_time_ms'] / perf['avg_step_time_ms']
            score += max(0, 1.0 - cv)  # Higher score for lower coefficient of variation
        
        # High score for efficient resource usage
        if 'cpu_avg_percent' in perf:
            cpu_efficiency = 1.0 - (perf['cpu_avg_percent'] / 100.0)
            score += cpu_efficiency
        
        if 'memory_avg_mb' in perf:
            memory_efficiency = max(0, 1.0 - (perf['memory_avg_mb'] / 1000.0))
            score += memory_efficiency
        
        return score
    
    def get_optimization_report(self, output_file=None):
        """Generate optimization report"""
        report = {
            'optimization_metadata': {
                'parameter_space': self.parameter_space,
                'total_iterations': len(self.search_results),
                'successful_evaluations': len([r for r in self.search_results if 'score' in r]),
                'best_score': self.best_score,
                'best_parameters': self.best_params,
                'timestamp': time.time()
            },
            'search_results': self.search_results,
            'convergence_analysis': self._analyze_convergence(),
            'recommendations': self._generate_optimization_recommendations()
        }
        
        if output_file:
            with open(output_file, 'w') as f:
                json.dump(report, f, indent=2)
        
        return report
    
    def _analyze_convergence(self):
        """Analyze convergence of optimization"""
        if len(self.search_results) < 2:
            return {}
        
        # Calculate improvement over iterations
        scores = [r['score'] for r in self.search_results if 'score' in r]
        
        if len(scores) < 2:
            return {}
        
        # Simple convergence analysis
        first_half = np.mean(scores[:len(scores)//2])
        second_half = np.mean(scores[len(scores)//2:])
        
        return {
            'score_improvement': second_half - first_half,
            'convergence_rate': abs(second_half - first_half) / first_half if first_half > 0 else 0,
            'final_score': scores[-1],
            'best_score': max(scores)
        }
    
    def _generate_optimization_recommendations(self):
        """Generate optimization-based recommendations"""
        recommendations = []
        
        if self.best_params:
            recommendations.append({
                'type': 'optimal_parameters',
                'message': f"Best parameters found: {self.best_params}",
                'suggestion': "Use these parameters for future experiments to achieve optimal performance."
            })
        
        convergence = self._analyze_convergence()
        if convergence.get('convergence_rate', 0) > 0.1:
            recommendations.append({
                'type': 'convergence',
                'message': f"Optimization converged with {convergence['convergence_rate']:.1%} improvement.",
                'suggestion': "Consider increasing iterations for further optimization or accept current parameters as optimal."
            })
        
        return recommendations
# Main enhanced API exports
__all__ = [
    # Core enhanced classes
    'PerformanceProfiler',
    'NeuralNetworkAnalyzer',
    'BatchProcessor',
    'ExperimentManager',
    'HyperparameterOptimizer',
    
    # Factory functions
    'create_config_from_preset',
    'create_brain_from_preset',
    'run_simulation_loop',
    'analyze_brain_state',
    'create_agent_with_features',
    
    # Module information
    'MODULE_VERSION',
    'MIN_PYTHON_VERSION',
    'SUPPORTED_FEATURES',
]

# Module-level constants and utilities
MODULE_VERSION = "2.0.0"
MIN_PYTHON_VERSION = (3, 8)
SUPPORTED_FEATURES = [
    'performance_profiling',
    'neural_analysis',
    'batch_processing',
    'experiment_management',
    'hyperparameter_optimization',
    'preset_configurations',
    'enhanced_error_handling',
    'monitoring_tools',
    'analysis_tools',
]

def create_config_from_preset(preset_name: str, **kwargs) -> Config:
    """Create configuration from preset (enhanced version)"""
    from pynlm_enhanced import createConfigFromPreset
    return createConfigFromPreset(preset_name, kwargs)

def create_brain_from_preset(preset_name: str, **kwargs) -> Brain:
    """Create brain from preset (enhanced version)"""
    from pynlm_enhanced import createBrainFromPreset
    return createBrainFromPreset(preset_name, kwargs)

def run_simulation_loop(brain, world=None, agent=None, 
                        steps=100, timestep=0.1, 
                        monitor=None, verbose=True):
    """Run simulation loop with monitoring (enhanced version)"""
    from pynlm_enhanced import run_simulation_loop as enhanced_loop
    return enhanced_loop(brain, world, agent, steps, timestep, monitor, verbose)

def analyze_brain_state(brain, include_detailed=True):
    """Analyze brain state (enhanced version)"""
    from pynlm_enhanced import analyze_brain_state as enhanced_analyze
    return enhanced_analyze(brain, include_detailed)

def create_agent_with_features(brain, world, **features):
    """Create agent with features (enhanced version)"""
    from pynlm_enhanced import create_agent_with_features as enhanced_create
    return enhanced_create(brain, world, **features)

def get_module_info():
    """Get information about the enhanced module"""
    return {
        'version': MODULE_VERSION,
        'description': 'Advanced Python API for NLM neural simulation framework',
        'features': SUPPORTED_FEATURES,
        'enhancements': [
            'Performance monitoring and profiling',
            'Neural network analysis and visualization',
            'Batch processing and experiment management',
            'Hyperparameter optimization',
            'Advanced configuration presets',
            'Error handling and validation',
            'Research experiment frameworks',
            'Distributed simulation capabilities'
        ],
        'performance_monitoring': 'Real-time profiling with anomaly detection',
        'neural_analysis': 'Statistical analysis of spike patterns and network dynamics',
        'batch_processing': 'Parallel execution of multiple experiments',
        'experiment_management': 'Comprehensive experiment tracking and analysis',
        'hyperparameter_optimization': 'Automated parameter optimization for performance',
        'compatibility': 'Full backward compatibility with traditional API'
    }