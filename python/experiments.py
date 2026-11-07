# Experiment management API for NLM
# This file provides a high-level interface for managing and running NLM experiments

import time
import json
import logging
from typing import Dict, List, Any, Optional, Callable, Union
from dataclasses import dataclass, field
import numpy as np
from pathlib import Path

from . import pynlm

@dataclass
class ExperimentConfig:
    """Configuration for NLM experiments"""
    name: str
    description: str = ""
    neuron_count: int = 500
    region_count: int = 1
    simulation_steps: int = 1000
    timestep: float = 0.001
    random_seed: int = 42
    enable_checkpoints: bool = False
    checkpoint_interval: int = 100
    enable_visualization: bool = False
    visualization_interval: int = 100
    custom_params: Dict[str, Any] = field(default_factory=dict)

@dataclass
class ExperimentResults:
    """Results from an NLM experiment"""
    experiment_name: str
    config: ExperimentConfig
    total_reward: float = 0.0
    average_firing_rate: float = 0.0
    total_spikes: int = 0
    final_brain_state: Optional[str] = None
    performance_metrics: Dict[str, float] = field(default_factory=dict)
    time_series_data: Dict[str, List[float]] = field(default_factory=dict)
    neural_network_stats: List[Dict[str, Any]] = field(default_factory=list)
    success: bool = True
    error_message: Optional[str] = None

class NLMSimulationRunner:
    """
    High-level simulation runner for NLM experiments
    """
    
    def __init__(self, config: ExperimentConfig):
        """Initialize simulation runner with experiment configuration"""
        self.config = config
        self.results = ExperimentResults(config.name, config)
        self.world = pynlm.createSimpleWorld()
        self.agent_brain = None
        self.brain = None
        self.step_history = []
        self.logger = logging.getLogger(f"NLMExperiment.{config.name}")
    
    def setup(self) -> bool:
        """Setup experiment environment"""
        try:
            # Create configuration
            nlm_config = pynlm.createDefaultConfig()
            nlm_config.set("random_seed", self.config.random_seed)
            nlm_config.set("neuron_count", self.config.neuron_count)
            nlm_config.set("region_count", self.config.region_count)
            nlm_config.set("simulation_timestep", self.config.timestep)
            
            # Apply custom parameters
            for key, value in self.config.custom_params.items():
                nlm_config.set(key, value)
            
            # Create brain
            self.brain = pynlm.createBrain(nlm_config)
            self.agent_brain = pynlm.createAgentBrain(self.brain)
            
            # Initialize world
            self.world.configure(
                width=100, height=100,
                vision_width=10, vision_height=10
            )
            self.world.setAgentStart(50, 50)
            self.world.setRandomSeed(self.config.random_seed)
            
            # Initialize agent brain with world
            self.agent_brain.initialize(self.world)
            
            self.logger.info(f"Experiment {self.config.name} setup complete")
            return True
            
        except Exception as e:
            self.logger.error(f"Failed to setup experiment: {str(e)}")
            self.results.success = False
            self.results.error_message = str(e)
            return False
    
    def run_step(self, step: int) -> Dict[str, Any]:
        """Run a single simulation step"""
        step_start = time.time()
        
        try:
            # Get sensory input
            percept = self.world.getSensoryPercept()
            
            # Process through agent brain
            self.agent_brain.processSensoryInput(percept)
            
            # Update world
            self.world.update(self.config.timestep)
            
            # Step brain
            self.brain.step(step, step * self.config.timestep)
            
            # Get action
            action = self.brain.produceAction()
            
            # Calculate step metrics
            spike_count = self.brain.getTotalSpikeCount()
            active_neurons = self.brain.getActiveNeuronCount()
            firing_neurons = self.brain.getFiringNeuronCount()
            average_firing_rate = self.brain.getAverageFiringRate()
            excitation_inhibition_ratio = self.brain.getExcitationInhibitionRatio()
            
            step_duration = time.time() - step_start
            
            # Create step result
            step_result = {
                'step': step,
                'time': step * self.config.timestep,
                toitureate': action,
                'spike_count': spike_count,
                'active_neurons': active_neurons,
                'firing_neurons': firing_neurons,
                'average_firing_rate': average_firing_rate,
                'excitation_inhibition_ratio': excitation_inhibition_ratio,
                'step_duration': step_duration,
            }
            
            self.step_history.append(step_result)
            
            # Update results
            self.results.total_reward += step_result.get('reward', 0.0)
            self.results.total_spikes = spike_count
            self.results.average_firing_rate = average_firing_rate
            
            # Record time series data
            if 'reward' in step_result:
                self.results.time_series_data.setdefault('reward', []).append(step_result['reward'])
            if 'spike_count' in step_result:
                self.results.time_series_data.setdefault('spike_count', []).append(step_result['spike_count'])
            if 'average_firing_rate' in step_result:
                self.results.time_series_data.setdefault('firing_rate', []).append(step_result['average_firing_rate'])
            
            # Record neural network stats
            neural_stats = {
                'step': step,
                'total_neurons': self.brain.getTotalNeuronCount(),
                'total_synapses': self.brain.getTotalSynapseCount(),
                'active_neurons': active_neurons,
                'firing_neurons': firing_neurons,
                'average_firing_rate': average_firing_rate,
                'excitation_inhibition_ratio': excitation_inhibition_ratio,
                'total_spikes': spike_count,
            }
            self.results.neural_network_stats.append(neural_stats)
            
            return step_result
            
        except Exception as e:
            self.logger.error(f"Step {step} failed: {str(e)}")
            step_result = {
                'step': step,
                'error': str(e),
                'success': False
            }
            self.step_history.append(step_result)
            self.results.success = False
            self.results.error_message = str(e)
            return step_result
    
    def run_experiment(self, callback: Optional[Callable[[Dict[str, Any]], None]] = None) -> ExperimentResults:
        """Run the entire experiment"""
        self.logger.info(f"Starting experiment {self.config.name} with {self.config.simulation_steps} steps")
        
        if not self.setup():
            return self.results
        
        try:
            for step in range(self.config.simulation_steps):
                step_result = self.run_step(step)
                
                if callback:
                    callback(step_result)
                
                # Save checkpoint if enabled
                if self.config.enable_checkpoints and (step + 1) % self.config.checkpoint_interval == 0:
                    checkpoint_file = f"{self.config.name}_checkpoint_{step + 1}.nml"
                    if self.brain.save(checkpoint_file):
                        self.logger.info(f"Checkpoint saved: {checkpoint_file}")
                        self.results.final_brain_state = checkpoint_file
                
                # Enable visualization if enabled
                if self.config.enable_visualization and (step + 1) % self.config.visualization_interval == 0:
                    self._visualize_step(step)
            
            # Save final brain state
            final_brain_file = f"{self.config.name}_final.nml"
            if self.brain.save(final_brain_file):
                self.results.final_brain_state = final_brain_file
            
            # Calculate performance metrics
            self._calculate_performance_metrics()
            
            self.logger.info(f"Experiment {self.config.name} completed successfully")
            return self.results
            
        except Exception as e:
            self.logger.error(f"Experiment failed: {str(e)}")
            self.results.success = False
            self.results.error_message = str(e)
            return self.results
    
    def _visualize_step(self, step: int) -> None:
        """Visualize current step"""
        # Placeholder for visualization
        pass
    
    def _calculate_performance_metrics(self) -> None:
        """Calculate performance metrics from experiment results"""
        if not self.step_history:
            return
        
        time_series = self.results.time_series_data
        
        # Calculate basic metrics
        if 'reward' in time_series:
            rewards = time_series['reward']
            self.results.performance_metrics['total_reward'] = sum(rewards)
            self.results.performance_metrics['mean_reward'] = np.mean(rewards)
            self.results.performance_metrics['std_reward'] = np.std(rewards)
            self.results.performance_metrics['max_reward'] = np.max(rewards)
            self.results.performance_metrics['min_reward']扣除'] = np.min(rewards)
        
        if 'spike_count' in time_series:
            spike_counts = time_series['spike_count']
            self.results.performance_metrics['mean_spike_count'] = np.mean(spike_counts)
            self.results.performance_metrics['max_spike_count'] = np.max(spike_counts)
            self.results.performance_metrics['min_spike_count'] = np.min(spike_counts)
        
        if 'firing_rate' in time_series:
            firing_rates = time_series['firing_rate']
            self.results.performance_metrics['mean_firing_rate'] = np.mean(firing_rates)
            self.results.performance_metrics['std_firing_rate'] = np.std(firing_rates)
        
        # Calculate time taken
        if self.step_history:
            first_step = self.step_history[0]
            last_step = self.step_history[-1]
            total_time = last_step['time'] - first_step['time']
            
            self.results.performance_metrics['total_time'] = total_time
            self.results.performance_metrics['steps_per_second'] = len(self.step_history) / total_time
    
    def save_results(self, filepath: str) -> bool:
        """Save experiment results to file"""
        try:
            results_dict = {
                'experiment_name': self.results.experiment_name,
                'config': {
                    'name': self.config.name,
                    'description': self.config.description,
                    'neuron_count': self.config.neuron_count,
                    'region_count': self.config.region_count,
                    'simulation_steps': self.config.simulation_steps,
                    'timestep': self.config.timestep,
                    'random_seed': self.config.random_seed,
                },
                'total_reward': self.results.total_reward,
                'average_firing_rate': self.results.average_firing_rate,
                'total_spikes': self.results.total_spikes,
                'final_brain_state': self.results.final_brain_state,
                'performance_metrics': self.results.performance_metrics,
                'time_series_data': self.results.time_series_data,
                'neural_network_stats': self.results.neural_network_stats,
                'success': self.results.success,
                'error_message': self.results.error_message,
            }
            
            with open(filepath, 'w') as f:
                json.dump(results_dict, f, indent=2, default=str)
            
            self.logger.info(f"Results saved to {filepath}")
            return True
            
        except Exception as e:
            self.logger.error(f"Failed to save results: {str(e)}")
            return False
    
    def load_results(self, filepath: str) -> bool:
        """Load experiment results from file"""
        try:
            with open(filepath, 'r') as f:
                results_dict = json.load(f)
            
            self.results = ExperimentResults(
                experiment_name=results_dict['experiment_name'],
                config=ExperimentConfig(
                    name=results_dict['config']['name'],
                    description=results_dict['config']['description'],
                    neuron_count=results_dict['config']['neuron_count'],
                    region_count=results_dict['config']['region_count'],
                    simulation_steps=results_dict['config']['simulation_steps'],
                    timestep=results_dict['config']['timestep'],
                    random_seed=results_dict['config']['random_seed'],
                ),
                total_reward=results_dict['total_reward'],
                average_firing_rate=results_dict['average_firing_rate'],
                total_spikes=results_dict['total_spikes'],
                final_brain_state=results_dict['final_brain_state'],
                performance_metrics=results_dict['performance_metrics'],
                time_series_data=results_dict['time_series_data'],
                neural_network_stats=results_dict['neural_network_stats'],
                success=results_dict['success'],
                error_message=results_dict.get('error_message'),
            )
            
            self.logger.info(f"Results loaded from {filepath}")
            return True
            
        except Exception as e:
            self.logger.error(f"Failed to load results: {str(e)}")
            return False

class MultiSeedExperimentRunner:
    """
    Run experiments with multiple random seeds for statistical analysis
    """
    
    def __init__(self, base_config: ExperimentConfig):
        """Initialize multi-seed experiment runner"""
        self.base_config = base_config
        self.runners = []
        self.results = []
        self.logger = logging.getLogger("MultiSeedExperiment")
    
    def run_multiple_seeds(self, num_seeds: int) -> List[ExperimentResults]:
        """Run experiment with multiple random seeds"""
        self.logger.info(f"Running {num_seeds} experiments with different random seeds")
        
        for seed in range(num_seeds):
            self.logger.info(f"Running seed {seed + 1}/{num_seeds}")
            
            # Create seed-specific config
            seed_config = ExperimentConfig(
                name=f"{self.base_config.name}_seed_{seed}",
                description=self.base_config.description,
                neuron_count=self.base_config.neuron_count,
                region_count=self.base_config.region_count,
                simulation_steps=self.base_config.simulation_steps,
                timestep=self.base_config.timestep,
                random_seed=self.base_config.random_seed + seed,
                enable_checkpoints=self.base_config.enable_checkpoints,
                checkpoint_interval=self.base_config.checkpoint_interval,
                enable_visualization=self.base_config.enable_visualization,
                visualization_interval=self.base_config.visualization_interval,
                custom_params=self.base_config.custom_params.copy(),
            )
            
            # Create and run runner
            runner = NLMSimulationRunner(seed_config)
            result = runner.run_experiment()
            self.runners.append(runner)
            self.results.append(result)
        
        self.logger.info(f"All {num_seeds} experiments completed")
        return self.results
    
    def analyze_results(self) -> Dict[str, Any]:
        """Analyze results from multiple seeds"""
        if not self.results:
            return {}
        
        analysis = {
            'num_experiments': len(self.results),
            'successful_experiments': sum(1 for r in self.results if r.success),
            'failed_experiments': sum(1 for r in self.results if not r.success),
            'reward_statistics': {},
            'performance_metrics': {},
        }
        
        # Analyze reward statistics
        all_rewards = [r.total_reward for r in self.results if r.success]
        if all_rewards:
            analysis['reward_statistics'] = {
                'mean': np.mean(all_rewards),
                'std': np.std(all_rewards),
                'min': np.min(all_rewards),
                'max': np.max(all_rewards),
                'median': np.median(all_rewards),
            }
        
        # Analyze performance metrics
        for metric in ['average_firing_rate', 'total_spikes']:
            metric_values = [getattr(r, metric) for r in self.results if r.success and hasattr(r, metric)]
            if metric_values:
                analysis['performance_metrics'][metric] = {
                    'mean': np.mean(metric_values),
                    'std': np.std(metric_values),
                    'min': np.min(metric_values),
                    'max': np.max(metric_values),
                }
        
        return analysis

# Export key classes and functions
__all__ = [
    'ExperimentConfig',
    'ExperimentResults',
    'NLMSimulationRunner',
    'MultiSeedExperimentRunner',
]

# Example usage functions
def example_single_experiment():
    """Example of single experiment usage"""
    print("=== Single Experiment Example ===")
    
    # Create experiment config
    config = ExperimentConfig(
        name="test_experiment",
        description="Test experiment with default parameters",
        neuron_count=100,
        simulation_steps=100,
        random_seed=42,
    )
    
    # Create and run experiment
    runner = NLMSimulationRunner(config)
    results = runner.run_experiment()
    
    print(f"Experiment completed: {results.success}")
    print(f"Total reward: {results.total_reward}")
    print(f"Average firing rate: {results.average_firing_rate:.2f} Hz")
    print(f"Total spikes: {results.total_spikes}")
    
    # Save results
    runner.save_results("experiment_results.json")
    
    print("Results saved to experiment_results.json")

def example_multi_seed_experiment():
    """Example of multi-seed experiment usage"""
    print("=== Multi-Seed Experiment Example ===")
    
    # Create base config
    base_config = ExperimentConfig(
        name="multi_seed_test",
        description="Multi-seed experiment for statistical analysis",
        neuron_count=100,
        simulation_steps=50,
        random_seed=42,
    )
    
    # Create and run multi-seed experiment
    runner = MultiSeedExperimentRunner(base_config)
    results = runner.run_multiple_seeds(5)
    
    # Analyze results
    analysis = runner.analyze_results()
    
    print(f"Ran {analysis['num_experiments']} experiments")
    print(f"Successful: {analysis['successful_experiments']}")
    print(f"Failed: {analysis['failed_experiments']}")
    
    if 'reward_statistics' in analysis:
        reward_stats = analysis['reward_statistics']
        print(f"Reward mean: {reward_stats['mean']:.2f}")
        print(f"Reward std: {reward_stats['std']:.2f}")
    
    print("Multi-seed experiment completed!")

# Run examples if this module is executed directly
if __name__ == '__main__':
    example_single_experiment()
    example_multi_seed_experiment()