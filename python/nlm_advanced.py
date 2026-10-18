# NLM Advanced Interface Wrapper
# Provides enhanced Python functionality for experienced users

import pynlm
import numpy as np
import json
import time
import math
from typing import List, Dict, Any, Optional, Callable
from enum import Enum

# Re-export basic bindings for backward compatibility
from pynlm import *

# Advanced NLM Interface Classes
class NLMAdvancedInterface:
    """
    Advanced NLM interface providing enhanced functionality for experienced users.
    
    This interface extends the basic pynlm bindings with:
    - Neural network analysis and visualization
    - Advanced simulation control
    - Performance profiling
    - Custom simulation loops
    - Integration with numpy/scipy
    - Experimental protocol templates
    """
    
    def __init__(self, neuron_count: int = 100):
        """Initialize advanced NLM interface.
        
        Args:
            neuron_count: Number of neurons for the simulation
        """
        # Create configuration
        self.config = createDefaultConfig()
        self.config.set("neuron_count", neuron_count, ConfigSource.Default)
        self.config.set("region_count", 1, ConfigSource.Default)
        
        # Create brain
        self.brain = createBrain(self.config)
        self.brain.initialize()
        
        # Create world
        self.world = createSimpleWorld()
        self.world.initialize(10, 10)
        
        # Create agent
        self.agent = createAgentBrain(self.brain)
        self.agent.initialize(self.world)
        self.agent.enableRewardModulation(True)
        self.agent.enableCuriosity(True)
        
        # Performance tracking
        self.performance_data = {
            'steps': 0,
            'total_reward': 0.0,
            'start_time': None,
            'spike_counts': [],
            'firing_rates': [],
            'custom_metrics': {}
        }
        
    def advanced_simulate(self, steps: int, 
                         custom_sensory_fn: Optional[Callable] = None,
                         enable_developmental_updates: bool = True,
                         reward_fn: Optional[Callable] = None,
                         verbose: bool = False) -> Dict[str, Any]:
        """
        Advanced simulation with custom control and enhanced feedback.
        
        Args:
            steps: Number of simulation steps
            custom_sensory_fn: Custom function for sensory input generation
            enable_developmental_updates: Enable developmental updates
            reward_fn: Custom reward function
            verbose: Enable verbose output
            
        Returns:
            Dictionary with simulation results and metrics
        """
        if self.performance_data['start_time'] is None:
            self.performance_data['start_time'] = time.time()
        
        total_reward = 0.0
        firing_rate_sum = 0.0
        
        for step in range(steps):
            # Custom sensory input generation
            if custom_sensory_fn:
                # Use custom sensory function
                percept = custom_sensory_fn(step)
            else:
                # Default sensory input
                percept = self.world.observe(self.brain.getRegions()[0].get())
            
            # Process sensory input
            self.agent.processSensoryInput(percept)
            
            # Brain step
            self.brain.step(step, step * 0.001)
            
            # Get action
            cmd = self.agent.decodeMotorCommand()
            
            # Apply action
            self.world.applyAction(self.brain.getRegions()[0].get(), cmd)
            
            # Custom reward computation
            if reward_fn:
                reward = reward_fn(step, cmd, self.world)
            else:
                reward = self.world.computeReward(self.brain.getRegions()[0].get())
            
            total_reward += reward
            
            # Apply neuromodulation
            self.agent.applyRewardModulation(reward, 0.0f)
            
            # Update development
            if enable_developmental_updates:
                self.agent.updateDevelopment(0.001)
            
            # Collect metrics
            firing_rate_sum += self.brain.getAverageFiringRate()
            self.performance_data['spike_counts'].append(self.brain.getFiringNeuronCount())
            self.performance_data['firing_rates'].append(self.brain.getAverageFiringRate())
            
            if verbose and step % 100 == 0:
                print(f"Step {step}: Reward={reward:.3f}, Firing={self.brain.getAverageFiringRate():.3f}")
        
        # Calculate metrics
        end_time = time.time()
        duration = end_time - self.performance_data['start_time']
        
        results = {
            'total_reward': total_reward / steps,
            'average_firing_rate': firing_rate_sum / steps,
            'duration': duration,
            'steps_per_second': steps / duration,
            'final_state': self.get_neural_state(),
            'performance_data': self.performance_data.copy()
        }
        
        self.performance_data['steps'] = steps
        self.performance_data['total_reward'] = total_reward / steps
        
        return results
    
    def get_neural_state(self) -> Dict[str, Any]:
        """Get detailed neural state information."""
        state = {}
        
        # Basic statistics
        state['total_neurons'] = self.brain.getTotalNeuronCount()
        state['total_synapses'] = self.brain.getTotalSynapseCount()
        state['active_neurons'] = self.brain.getActiveNeuronCount()
        state['firing_neurons'] = self.brain.getFiringNeuronCount()
        state['excitation_inhibition_ratio'] = self.brain.getExcitationInhibitionRatio()
        
        # Developmental information
        state['developmental_stage'] = self.agent.getDevelopmentalStage()
        state['curiosity_level'] = self.agent.getCuriosityLevel()
        state['dopamine_level'] = self.agent.getNeuromodulationLevel()
        state['novelty_level'] = self.agent.getNoveltyLevel()
        
        # Regional information
        regions = self.brain.getRegions()
        state['region_count'] = len(regions)
        state['regions'] = []
        
        for i, region in enumerate(regions):
            region_info = {
                'id': i,
                'neuron_count': region->getNeuronCount(),
                'synapse_count': region->getSynapseCount(),
                'region_name': region->getName()
            }
            state['regions'].append(region_info)
        
        return state
    
    def analyze_activity_patterns(self, window_size: int = 100) -> Dict[str, Any]:
        """
        Analyze neural activity patterns over time.
        
        Args:
            window_size: Window size for pattern analysis
            
        Returns:
            Dictionary with pattern analysis results
        """
        if len(self.performance_data['spike_counts']) < window_size:
            return {'error': 'Insufficient data for analysis'}
        
        spike_data = self.performance_data['spike_counts']
        firing_data = self.performance_data['firing_rates']
        
        # Calculate statistics
        analysis = {}
        
        # Spike count analysis
        analysis['spike_count_stats'] = {
            'mean': np.mean(spike_data),
            'std': np.std(spike_data),
            'min': np.min(spike_data),
            'max': np.max(spike_data),
            'median': np.median(spike_data),
            'percentiles': np.percentile(spike_data, [25, 50, 75])
        }
        
        # Firing rate analysis
        analysis['firing_rate_stats'] = {
            'mean': np.mean(firing_data),
            'std': np.std(firing_data),
            'min': np.min(firing_data),
            'max': np.max(firing_data),
            'cv': np.std(firing_data) / np.mean(firing_data) if np.mean(firing_data) > 0 else 0
        }
        
        # Pattern detection (simple)
        analysis['pattern_detection'] = {
            'stationary_periods': self._detect_stationary_periods(spike_data),
            'bursts': self._detect_bursts(spike_data),
            'transients': self._detect_transients(firing_data)
        }
        
        return analysis
    
    def save_results(self, filepath: str, results: Dict[str, Any]):
        """Save advanced simulation results to file."""
        # Convert numpy data to JSON-serializable format
        def convert_numpy(obj):
            if isinstance(obj, np.ndarray):
                return obj.tolist()
            elif isinstance(obj, np.floating):
                return float(obj)
            elif isinstance(obj, np.integer):
                return int(obj)
            elif isinstance(obj, dict):
                return {k: convert_numpy(v) for k, v in obj.items()}
            elif isinstance(obj, list):
                return [convert_numpy(item) for item in obj]
            else:
                return obj
        
        # Convert results
        converted_results = convert_numpy(results)
        
        # Add metadata
        converted_results['metadata'] = {
            'timestamp': time.time(),
            'neuron_count': self.brain.getTotalNeuronCount(),
            'simulation_type': 'advanced'
        }
        
        # Write to file
        with open(filepath, 'w') as f:
            json.dump(converted_results, f, indent=2)
        
        print(f"Results saved to {filepath}")
    
    def benchmark_performance(self, steps: int, repetitions: int = 5) -> Dict[str, Any]:
        """
        Benchmark NLM performance.
        
        Args:
            steps: Number of steps per benchmark
            repetitions: Number of repetitions
            
        Returns:
            Dictionary with benchmark results
        """
        times = []
        rewards = []
        firing_rates = []
        
        for rep in range(repetitions):
            # Reset performance data
            self.performance_data = {
                'steps': 0,
                'total_reward': 0.0,
                'start_time': None,
                'spike_counts': [],
                'firing_rates': [],
                'custom_metrics': {}
            }
            
            # Run simulation
            start_time = time.time()
            
            results = self.advanced_simulate(
                steps, 
                custom_sensory_fn=lambda step: self.world.observe(self.brain.getRegions()[0].get()),
                verbose=False
            )
            
            end_time = time.time()
            
            times.append(end_time - start_time)
            rewards.append(results['total_reward'])
            firing_rates.append(results['average_firing_rate'])
        
        benchmark_results = {
            'repetitions': repetitions,
            'steps_per_run': steps,
            'average_time': np.mean(times),
            'time_std': np.std(times),
            'time_min': np.min(times),
            'time_max': np.max(times),
            'steps_per_second_mean': np.mean(steps / np.array(times)),
            'reward_mean': np.mean(rewards),
            'reward_std': np.std(rewards),
            'firing_rate_mean': np.mean(firing_rates),
            'firing_rate_std': np.std(firing_rates)
        }
        
        return benchmark_results
    
    def experimental_protocol(self, protocol_name: str, **kwargs) -> Dict[str, Any]:
        """
        Run predefined experimental protocols.
        
        Args:
            protocol_name: Name of the protocol (e.g., 'adaptation', 'learning', 'social')
            **kwargs: Protocol-specific parameters
            
        Returns:
            Dictionary with protocol results
        """
        protocols = {
            'adaptation': self._protocol_adaptation,
            'learning': self._protocol_learning,
            'novelty_detection': self._protocol_novelty_detection,
            'reward_learning': self._protocol_reward_learning
        }
        
        if protocol_name not in protocols:
            raise ValueError(f"Unknown protocol: {protocol_name}")
        
        return protocols[protocol_name](**kwargs)
    
    def _detect_stationary_periods(self, data: List[float], threshold: float = 2.0) -> List[Dict]:
        """Detect stationary periods in spike count data."""
        periods = []
        n = len(data)
        
        for i in range(n - 100):
            window = data[i:i+100]
            mean_val = np.mean(window)
            std_val = np.std(window)
            
            if std_val / (mean_val + 1e-6) < threshold / np.sqrt(100):
                periods.append({
                    'start': i,
                    'end': i + 100,
                    'mean': mean_val,
                    'std': std_val
                })
        
        return periods
    
    def _detect_bursts(self, data: List[float], threshold: float = 3.0) -> List[Dict]:
        """Detect burst activity in spike count data."""
        bursts = []
        n = len(data)
        
        for i in range(n - 20):
            window = data[i:i+20]
            threshold_val = np.mean(data) + threshold * np.std(data)
            
            if any(val > threshold_val for val in window):
                # Check if this is the start of a burst
                if i == 0 or not any(val > threshold_val for val in data[i-10:i]):
                    # Find burst duration
                    burst_end = i
                    while burst_end < n and any(val > threshold_val for val in data[burst_end:burst_end+20]):
                        burst_end += 1
                    
                    bursts.append({
                        'start': i,
                        'end': burst_end,
                        'duration': burst_end - i
                    })
        
        return bursts
    
    def _detect_transients(self, data: List[float], threshold: float = 2.0) -> List[Dict]:
        """Detect transient activity in firing rate data."""
        transients = []
        n = len(data)
        
        for i in range(1, n-1):
            if abs(data[i] - data[i-1]) > threshold * np.std(data):
                transients.append({
                    'position': i,
                    'value_before': data[i-1],
                    'value_after': data[i],
                    'change': data[i] - data[i-1]
                })
        
        return transients
    
    def _protocol_adaptation(self, steps: int = 1000, change_threshold: float = 0.1) -> Dict[str, Any]:
        """Adaptation protocol - test response to environmental changes."""
        print("Running adaptation protocol...")
        
        # Initial adaptation phase
        results1 = self.advanced_simulate(steps // 2, verbose=False)
        
        # Record initial state
        initial_state = self.get_neural_state()
        initial_reward = results1['total_reward']
        
        # Environmental change simulation
        results2 = self.advanced_simulate(steps // 2, verbose=False)
        
        # Calculate adaptation metrics
        reward_change = results2['total_reward'] - results1['total_reward']
        adaptation_score = 1.0 - min(1.0, abs(reward_change) / change_threshold)
        
        return {
            'protocol': 'adaptation',
            'initial_reward': initial_reward,
            'final_reward': results2['total_reward'],
            'reward_change': reward_change,
            'adaptation_score': adaptation_score,
            'initial_state': initial_state
        }
    
    def _protocol_learning(self, steps: int = 2000, pattern_repeats: int = 5) -> Dict[str, Any]:
        """Learning protocol - test pattern learning."""
        print("Running learning protocol...")
        
        # Generate simple pattern
        pattern = []
        for i in range(pattern_repeats):
            pattern.extend([0.1, 0.2, 0.3, 0.4, 0.5, 0.4, 0.3, 0.2, 0.1])
        
        # Learn pattern
        results = self.advanced_simulate(steps, 
                                        custom_sensory_fn=lambda step: pattern[step % len(pattern)],
                                        verbose=False)
        
        return {
            'protocol': 'learning',
            'pattern_length': len(pattern),
            'pattern_repeats': pattern_repeats,
            'total_reward': results['total_reward'],
            'final_state': self.get_neural_state()
        }
    
    def _protocol_novelty_detection(self, steps: int = 1000) -> Dict[str, Any]:
        """Novelty detection protocol - test response to novel stimuli."""
        print("Running novelty detection protocol...")
        
        results = self.advanced_simulate(steps, verbose=False)
        
        return {
            'protocol': 'novelty_detection',
            'curiosity_level': self.agent.getCuriosityLevel(),
            'novelty_level': self.agent.getNoveltyLevel(),
            'total_reward': results['total_reward'],
            'steps_per_second': results['steps_per_second']
        }
    
    def _protocol_reward_learning(self, steps: int = 1500) -> Dict[str, Any]:
        """Reward learning protocol - test reward-based learning."""
        print("Running reward learning protocol...")
        
        results = self.advanced_simulate(steps, 
                                        reward_fn=lambda step, cmd, world: 1.0 if step % 100 == 0 else 0.0,
                                        verbose=False)
        
        return {
            'protocol': 'reward_learning',
            'dopamine_level': self.agent.getNeuromodulationLevel(),
            'total_reward': results['total_reward'],
            'developmental_stage': self.agent.getDevelopmentalStage()
        }

# Convenience functions for quick setup

def create_advanced_experiment(neurons: int = 100, steps: int = 1000, **kwargs) -> Dict[str, Any]:
    """
    Create an advanced experiment with the given parameters.
    
    This is a convenience function for quick setup of advanced NLM experiments.
    """
    interface = NLMAdvancedInterface(neurons)
    
    results = interface.advanced_simulate(steps, **kwargs)
    results['interface_info'] = str(interface)
    
    return results


def create_benchmark_suite(steps_list: List[int] = None, repetitions: int = 3) -> Dict[str, Any]:
    """
    Create a benchmark suite for performance testing.
    
    Args:
        steps_list: List of step counts for benchmarking
        repetitions: Number of repetitions for each benchmark
        
    Returns:
        Dictionary with benchmark suite results
    """
    if steps_list is None:
        steps_list = [100, 500, 1000, 2000]
    
    benchmark_suite = {}
    
    for steps in steps_list:
        interface = NLMAdvancedInterface(100)
        benchmark_results = interface.benchmark_performance(steps, repetitions)
        benchmark_suite[f'steps_{steps}'] = benchmark_results
    
    return benchmark_suite


def analyze_neural_data(performance_data: Dict[str, Any]) -> Dict[str, Any]:
    """
    Analyze neural data for external analysis.
    
    Args:
        performance_data: Performance data dictionary
        
    Returns:
        Dictionary with analyzed results
    """
    spike_data = performance_data.get('spike_counts', [])
    firing_data = performance_data.get('firing_rates', [])
    
    if not spike_data or not firing_data:
        return {'error': 'No neural data available'}
    
    analysis = {}
    
    # Basic statistics
    analysis['spike_count_statistics'] = {
        'mean': np.mean(spike_data),
        'std': np.std(spike_data),
        'min': np.min(spike_data),
        'max': np.max(spike_data),
        'cv': np.std(spike_data) / np.mean(spike_data) if np.mean(spike_data) > 0 else 0
    }
    
    analysis['firing_rate_statistics'] = {
        'mean': np.mean(firing_data),
        'std': np.std(firing_data),
        'min': np.min(firing_data),
        'max': np.max(firing_data),
        'cv': np.std(firing_data) / np.mean(firing_data) if np.mean(firing_data) > 0 else 0
    }
    
    # Pattern analysis
    analysis['pattern_analysis'] = {
        'total_steps': len(spike_data),
        'stationary_periods': len(performance_data.get('stationary_periods', [])),
        'bursts': len(performance_data.get('bursts', [])),
        'transients': len(performance_data.get('transients', []))
    }
    
    return analysis


# Export key components
__all__ = [
    'NLMAdvancedInterface',
    'create_advanced_experiment',
    'create_benchmark_suite',
    'analyze_neural_data'
]
