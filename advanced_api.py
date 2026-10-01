#!/usr/bin/env python3
"""
NLM Advanced Python API - Power User Tools

This module provides advanced functionality for power users and researchers
who need to customize and experiment with the NLM neural simulation framework.

Features:
- Advanced brain configuration and customization
- Debugging and visualization tools
- Experiment control and monitoring
- Batch processing capabilities
- Advanced memory manipulation
- Debugging and profiling tools
- Configuration export/import
- Performance monitoring
- Neural pattern analysis
- Automated experimentation
- Statistical analysis
- Data visualization
"""

import json
import numpy as np
import matplotlib.pyplot as plt
from typing import Dict, List, Optional, Callable, Any
import threading
import time
import pandas as pd
from datetime import datetime

from nlm import (
    Config, Brain, AgentBrain, SimpleWorld, Vision, 
    NeuronType, ActionType, DevelopmentalStage
)


class AdvancedNLMBrain:
    """
    Advanced NLM Brain interface for power users and researchers.
    
    Provides comprehensive control over the NLM brain with advanced features
    for experimentation and research.
    """
    
    def __init__(self, config_file: Optional[str] = None, debug: bool = False):
        """
        Initialize advanced NLM brain.
        
        Args:
            config_file: Optional path to configuration file
            debug: Enable debug mode
        """
        self.config = Config() if config_file is None else Config()
        if config_file:
            self.config.loadFromFile(config_file)
        
        self.brain = Brain(self.config)
        self.debug = debug
        self.history = {
            'steps': [],
            'neuron_activities': [],
            'memory_content': [],
            'prediction_errors': [],
            'action_sequence': [],
            'rewards': []
        }
        self.callbacks = []
        self.experiment_id = f"exp_{datetime.now().strftime('%Y%m%d_%H%M%S')}"
        
        if self.debug:
            print(f"[AdvancedNLM] Initialized brain {self.experiment_id}")
    
    def configure_advanced(self, **kwargs):
        """
        Advanced configuration with parameter validation and documentation.
        
        Args:
            **kwargs: Configuration parameters
        """
        advanced_configs = {
            'brain': {
                'neuron_count': (100, 100000, 1000, "Total number of neurons"),
                'region_count': (1, 10, 4, "Number of brain regions"),
                'connection_probability': (0.0, 1.0, 0.1, "Synapse connection probability"),
                'simulation_timestep': (0.0001, 0.1, 0.001, "Simulation timestep"),
                'v_thresh': (-100, 50, -50, "Neuron threshold potential (mV)"),
                'v_rest': (-100, 50, -70, "Resting potential (mV)"),
                'v_reset': (-100, 50, -75, "Reset potential after spike (mV)"),
            },
            'plasticity': {
                'stdp_enable': (0, 1, 1, "Enable STDP plasticity"),
                'stdp_ltp_weight': (0.0, 1.0, 0.001, "STDP LTP weight"),
                'stdp_ltd_weight': (0.0, 1.0, 0.012, "STDP LTD weight"),
                'stdp_tau': (1, 200, 20, "STDP time constant (ms)"),
                'hebbian_enable': (0, 1, 1, "Enable Hebbian plasticity"),
                'structural_enable': (0, 1, 1, "Enable structural plasticity"),
                'synaptogenesis_rate': (0.0, 0.01, 0.0001, "Synaptogenesis rate"),
                'pruning_rate': (0.0, 0.01, 0.00001, "Pruning rate"),
            },
            'neuromodulation': {
                'dopamine_scale': (0.0, 5.0, 1.0, "Dopamine modulation scale"),
                'curiosity_enable': (0, 1, 1, "Enable curiosity-driven exploration"),
                'novelty_enable': (0, 1, 1, "Enable novelty detection"),
                'prediction_error_enable': (0, 1, 1, "Enable prediction error signals"),
            },
            'development': {
                'stage_progression_rate': (0.0, 1000.0, 60.0, "Stage progression rate (steps)"),
                'plasticity_modifier': (0.0, 1.0, 1.0, "Base plasticity modifier"),
            },
            'memory': {
                'working_memory_capacity': (1, 10000, 100, "Working memory capacity"),
                'working_memory_decay': (0.0, 1.0, 0.01, "Working memory decay rate"),
                'episodic_memory_max': (1, 10000, 1000, "Max episodic memory episodes"),
                'episdodic_memory_replay_interval': (1, 10000, 100, "Replay interval (steps)"),
                'associative_memory_strength': (0.0, 10.0, 1.0, "Associative memory strength"),
            }
        }
        
        for category, params in kwargs.items():
            if category not in advanced_configs:
                print(f"[AdvancedNLM] Warning: Unknown category '{category}'")
                continue
                
            for key, value in params.items():
                if key not in advanced_configs[category]:
                    print(f"[AdvancedNLM] Warning: Unknown parameter '{key}' in category '{category}'")
                    continue
                    
                min_val, max_val, default_val, description = advanced_configs[category][key]
                
                if not isinstance(value, (int, float)):
                    print(f"[AdvancedNLM] Error: Value for '{key}' must be numeric")
                    continue
                    
                if not (min_val <= value <= max_val):
                    print(f"[AdvancedNLM] Error: Value for '{key}' ({value}) outside range [{min_val}, {max_val}]")
                    continue
                    
                config_key = f"{category}.{key}"
                self.config.set(config_key, value)
                if self.debug:
                    print(f"[AdvancedNLM] Set {config_key} = {value}")
    
    def enable_callback(self, event_type: str, callback: Callable[[Dict], None]):
        """
        Enable event callbacks for monitoring.
        
        Args:
            event_type: Type of event ('step', 'memory', 'action', 'error', 'reward')
            callback: Function to call when event occurs
        """
        self.callbacks.append({'type': event_type, 'callback': callback})
        if self.debug:
            print(f"[AdvancedNLM] Registered callback for event type: {event_type}")
    
    def run_advanced_simulation(self, num_steps: int, 
                              batch_size: int = 100,
                              monitor_interval: int = 10,
                              save_interval: int = 100,
                              output_file: Optional[str] = None):
        """
        Run advanced simulation with monitoring and batch processing.
        
        Args:
            num_steps: Total number of simulation steps
            batch_size: Size of processing batches
            monitor_interval: Interval for monitoring
            save_interval: Interval for saving checkpoints
            output_file: Optional output file for results
            
        Returns:
            Dictionary with simulation statistics
        """
        if not self.brain.initialize():
            raise RuntimeError("Failed to initialize brain")
        
        start_time = time.time()
        stats = {
            'total_steps': num_steps,
            'steps_completed': 0,
            'start_time': start_time,
            'batches': 0,
            'memory_usage': [],
            'processing_times': [],
            'errors': []
        }
        
        if self.debug:
            print(f"[AdvancedNLM] Starting simulation with {num_steps} steps")
        
        for step in range(num_steps):
            step_start = time.time()
            
            # Process batch
            batch_end = min(step + batch_size, num_steps)
            for bstep in range(step, batch_end):
                # Record current state
                state = self._capture_state(bstep)
                
                # Process step
                self.brain.step(bstep)
                
                # Record after-step metrics
                self._record_metrics(state, bstep)
                
                # Trigger callbacks
                self._trigger_callbacks(state, bstep)
            
            step_time = time.time() - step_start
            stats['processing_times'].append(step_time)
            stats['steps_completed'] += batch_end - step
            stats['batches'] += 1
            
            # Monitoring
            if step % monitor_interval == 0:
                self._monitor_progress(stats, step, num_steps)
            
            # Save checkpoint
            if step % save_interval == 0 and output_file:
                checkpoint_file = f"{output_file}_step_{step}.json"
                self.save_state(checkpoint_file)
            
            step = batch_end - 1  # Update outer loop step
        
        # Finalize statistics
        end_time = time.time()
        stats['end_time'] = end_time
        stats['total_time'] = end_time - start_time
        stats['avg_processing_time'] = np.mean(stats['processing_times']) if stats['processing_times'] else 0
        stats['steps_per_second'] = stats['total_steps'] / stats['total_time'] if stats['total_time'] > 0 else 0
        
        # Save final results
        if output_file:
            results_file = f"{output_file}_final.json"
            self.save_results(results_file, stats)
        
        if self.debug:
            print(f"[AdvancedNLM] Simulation completed in {stats['total_time']:.2f} seconds")
        
        return stats
    
    def _capture_state(self, step: int) -> Dict:
        """Capture current brain state for analysis."""
        return {
            'step': step,
            'neuron_count': self.brain.getTotalNeuronCount(),
            'synapse_count': self.brain.getTotalSynapseCount(),
            'active_neurons': self.brain.getActiveNeuronCount(),
            'firing_neurons': self.brain.getFiringNeuronCount(),
            'avg_firing_rate': self.brain.getAverageFiringRate(),
            'e_i_ratio': self.brain.getExcitationInhibitionRatio(),
            'total_spikes': self.brain.getTotalSpikeCount(),
            'developmental_stage': self.brain.getDevelopmentalStage(),
            'has_working_memory': self.brain.getWorkingMemory() is not None,
            'has_episodic_memory': self.brain.getEpisodicMemory() is not None,
            'has_prediction_system': self.brain.getPredictionSystem() is not None,
            'has_planner': self.brain.getPlanner() is not None,
        }
    
    def _record_metrics(self, state: Dict, step: int):
        """Record metrics to history."""
        self.history['steps'].append(step)
        
        for key, value in state.items():
            if isinstance(value, (int, float)):
                if key in self.history:
                    self.history[key].append(value)
    
    def _trigger_callbacks(self, state: Dict, step: int):
        """Trigger registered callbacks."""
        for callback_info in self.callbacks:
            event_type = callback_info['type']
            callback = callback_info['callback']
            
            if event_type == 'step':
                callback(state)
    
    def _monitor_progress(self, stats: Dict, step: int, total_steps: int):
        """Monitor and log progress."""
        progress = (step / total_steps) * 100
        elapsed = time.time() - stats['start_time']
        remaining = (elapsed / step) * (total_steps - step) if step > 0 else 0
        
        print(f"[AdvancedNLM] Progress: {progress:.1f}% - Step {step}/{total_steps} - "
              f"Elapsed: {elapsed:.1f}s - Remaining: {remaining:.1f}s - "
              f"Avg Time/Step: {np.mean(stats['processing_times']):.3f}s")
    
    def save_state(self, filepath: str):
        """Save brain state to file."""
        if not self.brain.save(filepath):
            raise RuntimeError(f"Failed to save brain state to {filepath}")
        if self.debug:
            print(f"[AdvancedNLM] State saved to {filepath}")
    
    def load_state(self, filepath: str):
        """Load brain state from file."""
        if not self.brain.load(filepath):
            raise RuntimeError(f"Failed to load brain state from {filepath}")
        if self.debug:
            print(f"[AdvancedNLM] State loaded from {filepath}")
    
    def save_results(self, filepath: str, stats: Dict):
        """Save simulation results to file."""
        results = {
            'experiment_id': self.experiment_id,
            'timestamp': datetime.now().isoformat(),
            'configuration': {},
            'statistics': stats,
            'history': self.history
        }
        
        # Extract configuration keys
        for key in self.config.getKeys():
            results['configuration'][key] = self.config.get(key)
        
        with open(filepath, 'w') as f:
            json.dump(results, f, indent=2)
        
        if self.debug:
            print(f"[AdvancedNLM] Results saved to {filepath}")
    
    def analyze_patterns(self, pattern_type: str, window: Optional[int] = None) -> Dict:
        """
        Analyze neural patterns in the recorded history.
        
        Args:
            pattern_type: Type of pattern to analyze ('firing', 'memory', 'activity')
            window: Optional window size for moving average
            
        Returns:
            Dictionary with analysis results
        """
        analysis = {}
        
        if pattern_type == 'firing':
            if 'firing_neurons' in self.history:
                data = self.history['firing_neurons']
                if window:
                    import pandas as pd
                    series = pd.Series(data)
                    moving_avg = series.rolling(window=window).mean().tolist()
                    analysis['moving_average'] = moving_avg
                analysis['mean'] = np.mean(data)
                analysis['std'] = np.std(data)
                analysis['max'] = np.max(data)
                analysis['min'] = np.min(data)
                
        elif pattern_type == 'memory':
            if 'working_memory' in self.history:
                data = self.history['working_memory']
                analysis['mean'] = np.mean(data)
                analysis['std'] = np.std(data)
                
        elif pattern_type == 'activity':
            if 'avg_firing_rate' in self.history:
                data = self.history['avg_firing_rate']
                analysis['mean'] = np.mean(data)
                analysis['std'] = np.std(data)
        
        return analysis
    
    def visualize_results(self, output_dir: str = "."):
        """
        Generate visualizations of the recorded results.
        
        Args:
            output_dir: Directory for output files
        """
        if not os.path.exists(output_dir):
            os.makedirs(output_dir)
        
        # Create plots for each metric
        fig, axes = plt.subplots(2, 2, figsize=(15, 10))
        
        # Plot 1: Steps vs Firing Rate
        if 'avg_firing_rate' in self.history:
            axes[0, 0].plot(self.history['steps'], self.history['avg_firing_rate'])
            axes[0, 0].set_title('Average Firing Rate Over Time')
            axes[0, 0].set_xlabel('Step')
            axes[0, 0].set_ylabel('Firing Rate')
        
        # Plot 2: Steps vs Working Memory
        if 'working_memory' in self.history:
            axes[0, 1].plot(self.history['steps'], self.history['working_memory'])
            axes[0, 1].set_title('Working Memory Content Over Time')
            axes[0, 1].set_xlabel('Step')
            axes[0, 1].set_ylabel('Memory Activity')
        
        # Plot 3: Memory Usage Histogram
        if 'working_memory' in self.history:
            axes[1, 0].hist(self.history['working_memory'], bins=50)
            axes[1, 0].set_title('Working Memory Usage Distribution')
            axes[1, 0].set_xlabel('Memory Activity')
            axes[1, 0].set_ylabel('Frequency')
        
        # Plot 4: Activity Over Time (log scale)
        if 'avg_firing_rate' in self.history:
            axes[1, 1].semilogy(self.history['steps'], self.history['avg_firing_rate'])
            axes[1, 1].set_title('Firing Rate (Log Scale)')
            axes[1, 1].set_xlabel('Step')
            axes[1, 1].set_ylabel('Firing Rate (log)')
        
        plt.tight_layout()
        plt.savefig(f"{output_dir}/nlm_analysis_{self.experiment_id}.png")
        plt.close()
        
        if self.debug:
            print(f"[AdvancedNLM] Generated visualization: nlm_analysis_{self.experiment_id}.png")


class NLMDebugger:
    """
    Advanced debugging and profiling tools for NLM.
    
    Provides comprehensive debugging capabilities including performance
    profiling, memory tracing, and neural activity visualization.
    """
    
    def __init__(self, brain: AdvancedNLMBrain):
        self.brain = brain
        self.trace_data = []
        self.memory_usage = []
        self.performance_metrics = {
            'step_times': [],
            'neuron_counts': [],
            'synapse_counts': [],
            'memory_usage': []
        }
        self.is_recording = False
    
    def start_trace(self):
        """Start recording performance trace."""
        self.is_recording = True
        self.trace_data = []
        if self.brain.debug:
            print("[NLMDebugger] Started performance tracing")
    
    def stop_trace(self):
        """Stop recording performance trace."""
        self.is_recording = False
        if self.brain.debug:
            print(f"[NLMDebugger] Stopped tracing - recorded {len(self.trace_data)} data points")
    
    def record_step(self, step: int):
        """Record performance data for current step."""
        if not self.is_recording:
            return
        
        state = self.brain._capture_state(step)
        self.trace_data.append(state)
        
        # Record memory usage (approximate)
        import psutil
        process = psutil.Process()
        memory_mb = process.memory_info().rss / 1024 / 1024
        self.memory_usage.append(memory_mb)
        
        # Update performance metrics
        self.performance_metrics['step_times'].append(state.get('step_time', 0))
        self.performance_metrics['neuron_counts'].append(state['neuron_count'])
        self.performance_metrics['synapse_counts'].append(state['synapse_count'])
        self.performance_metrics['memory_usage'].append(memory_mb)
    
    def analyze_performance(self) -> Dict:
        """Analyze performance data and return statistics."""
        if not self.trace_data:
            return {}
        
        analysis = {
            'total_steps': len(self.trace_data),
            'total_time': sum(d.get('step_time', 0) for d in self.trace_data),
            'avg_step_time': np.mean([d.get('step_time', 0) for d in self.trace_data]),
            'min_step_time': np.min([d.get('step_time', 0) for d in self.trace_data]),
            'max_step_time': np.max([d.get('step_time', 0) for d in self.trace_data]),
            'neuron_count_growth': np.polyfit(
                range(len(self.performance_metrics['neuron_counts'])),
                self.performance_metrics['neuron_counts'],
                1
            )[0] if len(self.performance_metrics['neuron_counts']) > 1 else 0,
            'memory_growth_rate': np.polyfit(
                range(len(self.memory_usage)),
                self.memory_usage,
                1
            )[0] if len(self.memory_usage) > 1 else 0,
            'peak_memory_usage': np.max(self.memory_usage) if self.memory_usage else 0,
            'performance_trend': self._determine_performance_trend()
        }
        
        return analysis
    
    def _determine_performance_trend(self) -> str:
        """Determine performance trend based on step times."""
        if len(self.performance_metrics['step_times']) < 3:
            return "insufficient_data"
        
        times = self.performance_metrics['step_times']
        if np.polyfit(range(len(times)), times, 1)[0] > 0.01:
            return "degrading"
        elif np.polyfit(range(len(times)), times, 1)[0] < -0.01:
            return "improving"
        else:
            return "stable"
    
    def generate_report(self, output_file: str) -> Dict:
        """Generate performance analysis report."""
        performance = self.analyze_performance()
        
        report = {
            'experiment_id': self.brain.experiment_id,
            'timestamp': datetime.now().isoformat(),
            'performance_analysis': performance,
            'recommendations': self._generate_recommendations(performance)
        }
        
        with open(output_file, 'w') as f:
            json.dump(report, f, indent=2)
        
        return report
    
    def _generate_recommendations(self, performance: Dict) -> List[str]:
        """Generate performance recommendations."""
        recommendations = []
        
        if performance['performance_trend'] == 'degrading':
            recommendations.append("Performance is degrading. Consider optimizing neural computations.")
        
        if performance['peak_memory_usage'] > 1000:  # MB
            recommendations.append("High memory usage detected. Consider memory optimization.")
        
        if performance['avg_step_time'] > 0.1:  # seconds
            recommendations.append("Slow step processing. Consider hardware acceleration or algorithm optimization.")
        
        if performance['neuron_count_growth'] > 100:
            recommendations.append("Rapid neuron growth. Consider implementing structural plasticity constraints.")
        
        if not recommendations:
            recommendations.append("Performance is within expected ranges. No action required.")
        
        return recommendations


class NLMExperimentRunner:
    """
    Automated experiment runner for NLM research.
    
    Provides capabilities for running systematic experiments, managing
    multiple configurations, and collecting statistical results.
    """
    
    def __init__(self, output_dir: str = "experiments"):
        self.output_dir = output_dir
        self.experiments = []
        self.results = []
        
        if not os.path.exists(output_dir):
            os.makedirs(output_dir)
    
    def create_experiment(self, name: str, config_overrides: Dict = None,
                         simulation_params: Dict = None) -> Dict:
        """
        Create a new experiment configuration.
        
        Args:
            name: Experiment name
            config_overrides: Configuration parameter overrides
            simulation_params: Simulation parameters
            
        Returns:
            Experiment configuration dictionary
        """
        experiment = {
            'name': name,
            'experiment_id': f"exp_{name}_{datetime.now().strftime('%Y%m%d_%H%M%S')}",
            'timestamp': datetime.now().isoformat(),
            'config_overrides': config_overrides or {},
            'simulation_params': simulation_params or {},
            'status': 'pending'
        }
        
        self.experiments.append(experiment)
        
        if self._debug:
            print(f"[NLMExperimentRunner] Created experiment: {name}")
        
        return experiment
    
    def run_experiment(self, experiment_id: str) -> Dict:
        """
        Run a specific experiment.
        
        Args:
            experiment_id: ID of experiment to run
            
        Returns:
            Experiment results
        """
        # Find experiment
        experiment = None
        for exp in self.experiments:
            if exp['experiment_id'] == experiment_id:
                experiment = exp
                break
        
        if not experiment:
            raise ValueError(f"Experiment {experiment_id} not found")
        
        # Create brain
        brain = AdvancedNLMBrain()
        
        # Configure
        brain.configure_advanced(**experiment['config_overrides'])
        
        # Run simulation
        simulation_params = experiment['simulation_params']
        results = brain.run_advanced_simulation(
            num_steps=simulation_params.get('num_steps', 1000),
            batch_size=simulation_params.get('batch_size', 100),
            monitor_interval=simulation_params.get('monitor_interval', 100),
            save_interval=simulation_params.get('save_interval', 500),
            output_file=f"{self.output_dir}/{experiment['experiment_id']}"
        )
        
        # Save results
        output_file = f"{self.output_dir}/{experiment['experiment_id']}_results.json"
        brain.save_results(output_file, results)
        
        # Generate analysis
        analysis = brain.analyze_patterns('firing', window=50)
        analysis_file = f"{self.output_dir}/{experiment['experiment_id']}_analysis.json"
        with open(analysis_file, 'w') as f:
            json.dump(analysis, f, indent=2)
        
        # Update experiment status
        experiment['status'] = 'completed'
        experiment['results_file'] = output_file
        experiment['analysis_file'] = analysis_file
        
        self.results.append({
            'experiment_id': experiment_id,
            'name': experiment['name'],
            'status': 'completed',
            'results_file': output_file,
            'analysis_file': analysis_file,
            'timestamp': datetime.now().isoformat()
        })
        
        if self._debug:
            print(f"[NLMExperimentRunner] Completed experiment: {experiment['name']}")
        
        return results
    
    def run_parameter_sweep(self, base_config: Dict, parameter_grid: Dict) -> List[Dict]:
        """
        Run parameter sweep experiment.
        
        Args:
            base_config: Base configuration
            parameter_grid: Dictionary of parameter names and value lists
            
        Returns:
            List of experiment results
        """
        results = []
        param_combinations = self._generate_param_combinations(parameter_grid)
        
        for i, param_values in enumerate(param_combinations):
            # Create experiment name
            exp_name = f"sweep_{i}"
            
            # Merge base config with parameter values
            config = base_config.copy()
            config.update(param_values)
            
            # Create and run experiment
            experiment = self.create_experiment(
                name=exp_name,
                config_overrides=config
            )
            
            result = self.run_experiment(experiment['experiment_id'])
            results.append(result)
            
            if self._debug:
                print(f"[NLMExperimentRunner] Completed sweep {i+1}/{len(param_combinations)}")
        
        return results
    
    def _generate_param_combinations(self, param_grid: Dict) -> List[Dict]:
        """Generate all combinations of parameter values."""
        param_names = list(param_grid.keys())
        param_values = list(param_grid.values())
        
        combinations = []
        
        def generate_combinations(current_combination, index):
            if index == len(param_names):
                combinations.append(current_combination.copy())
                return
            
            for value in param_values[index]:
                current_combination[param_names[index]] = value
                generate_combinations(current_combination, index + 1)
        
        generate_combinations({}, 0)
        return combinations
    
    def compare_experiments(self, experiment_ids: List[str]) -> Dict:
        """
        Compare multiple experiments.
        
        Args:
            experiment_ids: List of experiment IDs to compare
            
        Returns:
            Comparison results
        """
        comparison = {
            'experiment_ids': experiment_ids,
            'timestamp': datetime.now().isoformat(),
            'comparisons': {}
        }
        
        # Load and compare key metrics
        for exp_id in experiment_ids:
            # Find experiment
            experiment = None
            for exp in self.experiments:
                if exp['experiment_id'] == exp_id:
                    experiment = exp
                    break
            
            if not experiment or 'results_file' not in experiment:
                continue
            
            # Load results
            with open(experiment['results_file'], 'r') as f:
                results = json.load(f)
            
            # Extract key metrics
            comparison['comparisons'][exp_id] = {
                'name': experiment['name'],
                'total_time': results.get('statistics', {}).get('total_time', 0),
                'avg_step_time': results.get('statistics', {}).get('avg_processing_time', 0),
                'steps_per_second': results.get('statistics', {}).get('steps_per_second', 0),
                'neuron_count': results.get('history', {}).get('neuron_count', []),
                'firing_rate': results.get('history', {}).get('avg_firing_rate', []),
                'memory_usage': results.get('history', {}).get('working_memory', [])
            }
        
        return comparison

# Global debug flag
def _debug(msg):
    """Debug print function."""
    print(f"[NLMExperimentRunner] {msg}")

_debug_enabled = False


def enable_debug():
    """Enable debug output."""
    global _debug_enabled
    _debug_enabled = True


# Main execution
if __name__ == "__main__":
    import argparse
    
    parser = argparse.ArgumentParser(description="NLM Advanced Python API")
    parser.add_argument("--debug", action="store_true", help="Enable debug output")
    parser.add_argument("--mode", choices=["advanced", "debug", "experiment"], 
                       default="advanced", help="Operating mode")
    parser.add_argument("--config", help="Configuration file path")
    
    args = parser.parse_args()
    
    if args.debug:
        enable_debug()
    
    if args.mode == "advanced":
        # Advanced mode
        brain = AdvancedNLMBrain(args.config, args.debug)
        brain.configure_advanced(
            brain={'neuron_count': 5000, 'region_count': 4},
            plasticity={'stdp_enable': 1, 'hebbian_enable': 1},
            neuromodulation={'curiosity_enable': 1, 'novelty_enable': 1}
        )
        
        print(f"Starting advanced NLM simulation...")
        results = brain.run_advanced_simulation(5000)
        print(f"Simulation completed in {results['total_time']:.2f} seconds")
        
    elif args.mode == "debug":
        # Debug mode
        brain = AdvancedNLMBrain(args.config, args.debug)
        debugger = NLMDebugger(brain)
        
        brain.configure_advanced(brain={'neuron_count': 2000})
        
        print(f"Starting NLM debugging session...")
        debugger.start_trace()
        
        results = brain.run_advanced_simulation(1000)
        
        debugger.stop_trace()
        report = debugger.generate_report("debug_report.json")
        print(f"Debug report generated: debug_report.json")
        
    elif args.mode == "experiment":
        # Experiment mode
        runner = NLMExperimentRunner()
        
        # Create parameter sweep experiment
        base_config = {
            'brain': {'neuron_count': 1000},
            'plasticity': {'stdp_enable': 1}
        }
        
        param_grid = {
            'plasticity': {'hebbian_enable': [0, 1]},
            'neuromodulation': {'curiosity_enable': [0, 1, 2]}
        }
        
        print(f"Running parameter sweep experiment...")
        results = runner.run_parameter_sweep(base_config, param_grid)
        print(f"Completed {len(results)} parameter combinations")


if __name__ != "__main__":
    # Import for use as module
    import os
    import psutil