#!/usr/bin/env python3
"""
Advanced NLM Python Utilities

This module provides advanced utilities for working with the NLM framework,
including sophisticated simulation tools, analysis functions, and experimentation
support for advanced users.
"""

import numpy as np
import pybind11
from typing import List, Dict, Optional, Callable, Any
import json
import time
import os
from pathlib import Path

import nlm

class NLMExperiment:
    """
    Advanced experiment runner for NLM simulations.
    
    This class provides sophisticated control over NLM experiments, including:
    - Configurable stimulus-response paradigms
    - Automated data collection and analysis
    - Statistical analysis of learning curves
    - Visualization of neural dynamics
    - Protocol scripting for reproducible experiments
    """
    
    def __init__(self, brain: nlm.Brain, world: nlm.SimpleWorld):
        self.brain = brain
        self.world = world
        self.experiment_data = []
        self.protocol_steps = []
        self.current_step = 0
        
    def run_stimulus_response_protocol(self,
                                     stimuli: List[Dict[str, Any]],
                                     response_window: int = 100,
                                     recording_interval: int = 1) -> Dict[str, Any]:
        """
        Run a stimulus-response protocol.
        
        Args:
            stimuli: List of stimulus dictionaries, each containing:
                - 'type': 'vision', 'touch', 'internal', 'proprioception'
                - 'data': sensory data vector
                - 'duration': duration in simulation steps
                - 'timing': when to present the stimulus
            response_window: Number of steps to record after stimulus
            recording_interval: Recording frequency
            
        Returns:
            Dictionary containing neural responses and statistics
        """
        responses = []
        stimulus_events = []
        
        # Schedule stimulus events
        for i, stimulus in enumerate(stimuli):
            stimulus_events.append({
                'step': stimulus.get('timing', i * 100),
                'type': stimulus['type'],
                'data': stimulus['data'],
                'duration': stimulus.get('duration', 20)
            })
        
        # Sort by timing
        stimulus_events.sort(key=lambda x: x['step'])
        
        # Run protocol
        for step in range(self.brain.getTotalSpikeCount(), 
                         self.brain.getTotalSpikeCount() + response_window):
            # Apply scheduled stimuli
            for event in stimulus_events:
                if event['step'] == step:
                    self._apply_stimulus(event)
            
            # Record response
            if step % recording_interval == 0:
                response = self._record_response(step)
                responses.append(response)
            
            # Step the simulation
            self.brain.step(step)
            
            # Update world
            self.world.update(self.brain.getConfig().getOr<double>("simulation_timestep", 0.001))
        
        # Analyze responses
        analysis = self._analyze_responses(responses, stimulus_events)
        return analysis
    
    def run_learning_curve_experiment(self,
                                     task_params: Dict[str, Any],
                                     num_trials: int = 100,
                                     recording_frequency: int = 5) -> Dict[str, Any]:
        """
        Run a learning curve experiment with adaptive difficulty.
        
        Args:
            task_params: Task configuration including:
                - 'task_type': 'reach', 'avoid', 'classify', 'navigate'
                - 'difficulty': initial difficulty level
                - 'reward_function': how to compute rewards
            num_trials: Number of trials to run
            recording_frequency: How often to record data
            
        Returns:
            Dictionary containing learning progress and metrics
            """
        learning_data = []
        performance_metrics = []
        
        for trial in range(num_trials):
            # Adjust difficulty based on performance
            current_difficulty = self._calculate_difficulty(
                performance_metrics, trial)
            
            # Run single trial
            trial_data = self._run_single_learning_trial(
                task_params, current_difficulty, trial)
            
            learning_data.append(trial_data)
            performance_metrics.append(trial_data['performance'])
            
            # Small delay between trials for stabilization
            time.sleep(0.01)
        
        # Analyze learning curve
        analysis = self._analyze_learning_curve(learning_data)
        return analysis
    
    def run_development_experiment(self,
                                  age_stages: List[float],
                                  metrics: List[str] = None) -> Dict[str, Any]:
        """
        Run a development experiment tracking brain changes over age.
        
        Args:
            age_stages: List of developmental ages to test
            metrics: List of metrics to track
            
        Returns:
            Dictionary containing developmental trajectories
            """
        if metrics is None:
            metrics = ['firing_rate', 'synaptic_strength', 
                      'memory_capacity', 'plasticity', 'behavior']
        
        developmental_data = {}
        
        for age in age_stages:
            # Set developmental stage
            self.brain.setDevelopmentalStage(
                self._age_to_stage(age))
            
            # Reset brain for fresh developmental test
            self.brain.reset()
            
            # Run simulation at this age
            age_data = self._run_at_age(age, metrics)
            developmental_data[f"age_{age}"] = age_data
            
            # Small pause between age stages
            time.sleep(0.01)
        
        return developmental_data
    
    def create_replay_protocol(self,
                              replay_episodes: int = 5,
                              replay_interval: int = 100,
                              replay_strength: float = 1.0f) -> Dict[str, Any]:
        """
        Create and run a memory replay protocol.
        
        Args:
            replay_episodes: Number of episodes to replay
            replay_interval: How often to trigger replay
            replay_strength: Strength of replay effects
            
        Returns:
            Dictionary containing replay analysis
            """
        replay_events = []
        replay_data = []
        
        # Trigger replay at specified intervals
        for step in range(0, 1000, replay_interval):
            # Get episodes for replay
            if hasattr(self.brain, 'getEpisodicMemory'):
                episodic_memory = self.brain.getEpisodicMemory()
                if episodic_memory:
                    episodes_to_replay = episodic_memory.getEpisodesForReplay(replay_episodes)
                    
                    for episode in episodes_to_replay:
                        if episode:
                            # Replay the episode
                            episodic_memory.replayEpisode(episode)
                            
                            replay_events.append({
                                'step': step,
                                'episode_id': id(episode),
                                'replay_strength': replay_strength
                            })
            
            # Record replay effects
            replay_snapshot = self._record_replay_state(step)
            replay_data.append(replay_snapshot)
            
            self.brain.step(step)
            self.world.update(0.001)
        
        return {
            'events': replay_events,
            'data': replay_data,
            'total_replays': len(replay_events)
        }
    
    def save_experiment_data(self, filepath: str, format: str = 'json'):
        """Save experiment data to file."""
        if format.lower() == 'json':
            with open(filepath, 'w') as f:
                json.dump(self.experiment_data, f, indent=2)
        elif format.lower() == 'npy':
            np.save(filepath, self.experiment_data)
        else:
            raise ValueError(f"Unsupported format: {format}")
    
    def load_experiment_data(self, filepath: str):
        """Load experiment data from file."""
        if filepath.endswith('.json'):
            with open(filepath, 'r') as f:
                self.experiment_data = json.load(f)
        elif filepath.endswith('.npy'):
            self.experiment_data = np.load(filepath, allow_pickle=True).tolist()
        else:
            raise ValueError(f"Unsupported file format: {filepath}")

class NLMAnalyzer:
    """
    Advanced neural analysis tools for NLM data.
    
    This class provides sophisticated analysis of NLM simulation data,
    including:
    - Neural activity pattern analysis
    - Network topology analysis
    - Learning trajectory analysis
    - Cross-correlation and coherence analysis
    - Statistical testing and hypothesis validation
    """
    
    @staticmethod
    def analyze_neural_dynamics(spike_data: List[Dict[str, Any]], 
                              window_size: int = 100) -> Dict[str, Any]:
        """
        Analyze neural spike dynamics.
        
        Args:
            spike_data: List of spike events with timing and neuron IDs
            window_size: Analysis window size
            
        Returns:
            Dictionary containing dynamic properties
            """
        if not spike_data:
            return {'error': 'No spike data provided'}
        
        # Calculate firing statistics
        firing_rates = NLMAnalyzer._calculate_firing_rates(spike_data, window_size)
        
        # Analyze burst patterns
        burst_analysis = NLMAnalyzer._analyze_burst_patterns(spike_data)
        
        # Calculate synchrony measures
        synchrony = NLMAnalyzer._calculate_synchrony(spike_data)
        
        return {
            'firing_rates': firing_rates,
            'burst_analysis': burst_analysis,
            'synchrony': synchrony,
            'total_spikes': len(spike_data),
            'neurons_active': len(set(event['neuron_id'] for event in spike_data))
        }
    
    @staticmethod
    def analyze_network_topology(neural_regions: List[Any]) -> Dict[str, Any]:
        """
        Analyze network topology and connectivity patterns.
        
        Args:
            neural_regions: List of neural regions
            
        Returns:
            Dictionary containing topological properties
            """
        if not neural_regions:
            return {'error': 'No neural regions provided'}
        
        # Calculate connectivity matrix
        connectivity_matrix = NLMAnalyzer._build_connectivity_matrix(neural_regions)
        
        # Analyze network metrics
        network_metrics = NLMAnalyzer._calculate_network_metrics(connectivity_matrix)
        
        # Identify network modules
        community_structure = NLMAnalyzer._identify_communities(connectivity_matrix)
        
        return {
            'connectivity_matrix': connectivity_matrix,
            'network_metrics': network_metrics,
            'community_structure': community_structure,
            'total_regions': len(neural_regions),
            'total_connections': sum(len(row) for row in connectivity_matrix)
        }
    
    @staticmethod
    def analyze_learning_trajectory(performance_data: List[Dict[str, Any]],
                                   learning_model: str = 'exponential') -> Dict[str, Any]:
        """
        Analyze learning trajectory with model fitting.
        
        Args:
            performance_data: List of performance measurements
            learning_model: Type of learning model ('exponential', 'power_law', 'logarithmic')
            
        Returns:
            Dictionary containing fitted model parameters and goodness of fit
            """
        if not performance_data:
            return {'error': 'No performance data provided'}
        
        # Extract performance values
        trials = [d['trial'] for d in performance_data]
        performance = [d['performance'] for d in performance_data]
        rewards = [d.get('reward', 0) for d in performance_data]
        
        # Fit learning model
        if learning_model == 'exponential':
            params = NLMAnalyzer._fit_exponential_learning(trials, performance)
        elif learning_model == 'power_law':
            params = NLMAnalyzer._fit_power_law_learning(trials, performance)
        elif learning_model == 'logarithmic':
            params = NLMAnalyzer._fit_logarithmic_learning(trials, performance)
        else:
            return {'error': f'Unknown learning model: {learning_model}'}
        
        # Calculate goodness of fit
        goodness_of_fit = NLMAnalyzer._calculate_goodness_of_fit(
            trials, performance, params)
        
        return {
            'model_parameters': params,
            'goodness_of_fit': goodness_of_fit,
            'model_type': learning_model,
            'data_points': len(performance_data)
        }

class NLMSimulationController:
    """
    Advanced simulation controller for NLM experiments.
    
    This class provides fine-grained control over NLM simulations,
    including:
    - Multi-threaded simulation execution
    - Real-time monitoring and debugging
    - Automated checkpointing and recovery
    - Performance optimization and profiling
    - Advanced visualization integration
    """
    
    def __init__(self, brain: nlm.Brain, world: nlm.SimpleWorld):
        self.brain = brain
        self.world = world
        self.simulation_state = 'stopped'
        self.current_step = 0
        self.max_steps = 0
        self.checkpoint_interval = 100
        self.monitoring_enabled = True
        self.performance_stats = {}
        
    def start_simulation(self, max_steps: int, 
                        speed_factor: float = 1.0,
                        enable_monitoring: bool = True):
        """
        Start a simulation with specified parameters.
        
        Args:
            max_steps: Maximum number of simulation steps
            speed_factor: Speed multiplier for visualization
            enable_monitoring: Enable real-time monitoring
        """
        self.max_steps = max_steps
        self.simulation_state = 'running'
        self.monitoring_enabled = enable_monitoring
        self.performance_stats = {
            'start_time': time.time(),
            'steps_completed': 0,
            'spikes_processed': 0,
            'memory_usage': 0,
            'cpu_time': 0
        }
        
        return True
    
    def pause_simulation(self):
        """Pause the current simulation."""
        self.simulation_state = 'paused'
        return True
    
    def resume_simulation(self):
        """Resume a paused simulation."""
        self.simulation_state = 'running'
        return True
    
    def stop_simulation(self):
        """Stop the current simulation."""
        self.simulation_state = 'stopped'
        self.performance_stats['end_time'] = time.time()
        return True
    
    def step_simulation(self, steps: int = 1) -> Dict[str, Any]:
        """
        Advance the simulation by specified steps.
        
        Args:
            steps: Number of steps to advance
            
        Returns:
            Dictionary containing step results and statistics
        """
        if self.simulation_state != 'running':
            return {'error': f'Cannot step simulation in state: {self.simulation_state}'}
        
        step_results = {
            'steps_completed': 0,
            'spikes_generated': 0,
            'actions_produced': 0,
            'memory_updates': 0
        }
        
        start_time = time.time()
        
        for i in range(steps):
            if self.current_step >= self.max_steps:
                break
                
            # Record state before step
            pre_step_stats = self._get_simulation_stats()
            
            # Step the simulation
            self.brain.step(self.current_step)
            self.world.update(0.001)
            
            # Record state after step
            post_step_stats = self._get_simulation_stats()
            
            # Update statistics
            step_results['steps_completed'] += 1
            step_results['spikes_generated'] += (
                post_step_stats['total_spikes'] - pre_step_stats['total_spikes'])
            step_results['actions_produced'] += (
                len(self.world.getAgentBody().getId() if hasattr(self.world, 'getAgentBody') else []))
            
            # Check for checkpoints
            if self.current_step % self.checkpoint_interval == 0:
                self._create_checkpoint()
            
            self.current_step += 1
        
        # Update performance statistics
        end_time = time.time()
        self.performance_stats['cpu_time'] += (end_time - start_time)
        self.performance_stats['steps_completed'] = self.current_step
        
        return step_results

    def _apply_stimulus(self, stimulus_event: Dict[str, Any]):
        """Apply a stimulus to the simulation."""
        stimulus_type = stimulus_event['type']
        stimulus_data = stimulus_event['data']
        
        if stimulus_type == 'vision':
            # Create vision input
            pass  # Implementation would depend on NLM API
        elif stimulus_type == 'touch':
            # Create touch input
            pass
        elif stimulus_type == 'internal':
            # Create internal signals
            pass
        elif stimulus_type == 'proprioception':
            # Create proprioception signals
            pass
    
    def _record_response(self, step: int) -> Dict[str, Any]:
        """Record neural response at current step."""
        return {
            'step': step,
            'timestamp': time.time(),
            'spike_count': self.brain.getTotalSpikeCount(),
            'active_neurons': self.brain.getActiveNeuronCount(),
            'firing_rate': self.brain.getAverageFiringRate(),
            'excitement_inhibition_ratio': self.brain.getExcitationInhibitionRatio()
        }
    
    def _analyze_responses(self, responses: List[Dict[str, Any]], 
                          stimulus_events: List[Dict[str, Any]]) -> Dict[str, Any]:
        """Analyze recorded responses."""
        if not responses:
            return {'error': 'No responses to analyze'}
        
        # Calculate response statistics
        mean_firing_rate = np.mean([r['firing_rate'] for r in responses])
        std_firing_rate = np.std([r['firing_rate'] for r in responses])
        
        # Identify response peaks
        response_peaks = []
        for i, response in enumerate(responses):
            if i > 0 and i < len(responses) - 1:
                if response['firing_rate'] > responses[i-1]['firing_rate'] and \
                   response['firing_rate'] > responses[i+1]['firing_rate']:
                    response_peaks.append(response)
        
        return {
            'mean_firing_rate': mean_firing_rate,
            'std_firing_rate': std_firing_rate,
            'response_peaks': response_peaks,
            'total_responses': len(responses),
            'stimulus_count': len(stimulus_events)
        }

    def _get_simulation_stats(self) -> Dict[str, Any]:
        """Get current simulation statistics."""
        return {
            'total_spikes': self.brain.getTotalSpikeCount(),
            'active_neurons': self.brain.getActiveNeuronCount(),
            'firing_rate': self.brain.getAverageFiringRate(),
            'excitation_inhibition_ratio': self.brain.getExcitationInhibitionRatio()
        }
    
    def _create_checkpoint(self):
        """Create a simulation checkpoint."""
        if hasattr(self.brain, 'save'):
            checkpoint_file = f"checkpoint_{self.current_step}.bin"
            self.brain.save(checkpoint_file)
    
    def _age_to_stage(self, age: float) -> nlm.DevelopmentalStage:
        """Convert age to developmental stage."""
        if age < 60:
            return nlm.DevelopmentalStage.Initial
        elif age < 300:
            return nlm.DevelopmentalStage.CriticalPeriod
        elif age < 900:
            return nlm.DevelopmentalStage.Maturation
        else:
            return nlm.DevelopmentalStage.Adult

    def _calculate_difficulty(self, performance_metrics: List[float], 
                            trial: int) -> float:
        """Calculate current task difficulty based on performance."""
        if not performance_metrics:
            return 0.5
        
        # Adaptive difficulty based on recent performance
        recent_performance = performance_metrics[-min(10, len(performance_metrics)):]
        avg_performance = sum(recent_performance) / len(recent_performance)
        
        # Difficulty is inversely related to performance
        difficulty = max(0.1, min(0.9, 1.0 - avg_performance))
        
        # Add some random variation
        difficulty += (np.random.random() - 0.5) * 0.1
        return np.clip(difficulty, 0.1, 0.9)

    def _run_single_learning_trial(self, task_params: Dict[str, Any],
                                  difficulty: float, trial: int) -> Dict[str, Any]:
        """Run a single learning trial."""
        # Implementation would depend on specific task
        return {
            'trial': trial,
            'difficulty': difficulty,
            'performance': np.random.random(),  # Placeholder
            'reward': np.random.random(),  # Placeholder
            'time_taken': 1.0  # Placeholder
        }
    
    def _run_at_age(self, age: float, metrics: List[str]) -> Dict[str, Any]:
        """Run simulation at a specific developmental age."""
        results = {}
        
        for metric in metrics:
            if metric == 'firing_rate':
                results[metric] = self.brain.getAverageFiringRate()
            elif metric == 'synaptic_strength':
                # Placeholder - would need actual metric calculation
                results[metric] = np.random.random()
            elif metric == 'memory_capacity':
                # Placeholder - would need actual metric calculation
                results[metric] = np.random.random()
            elif metric == 'plasticity':
                # Placeholder - would need actual metric calculation
                results[metric] = np.random.random()
            elif metric == 'behavior':
                # Placeholder - would need actual metric calculation
                results[metric] = np.random.random()
        
        return results
    
    def _record_replay_state(self, step: int) -> Dict[str, Any]:
        """Record state during replay."""
        return {
            'step': step,
            'timestamp': time.time(),
            'replay_active': True,
            'replay_strength': 1.0,
            'memory_usage': 0,  # Placeholder
            'neural_activity': self.brain.getAverageFiringRate()
        }

    # Placeholder methods for advanced statistical analysis
    @staticmethod
    def _calculate_firing_rates(spike_data: List[Dict[str, Any]], window_size: int):
        return []
    
    @staticmethod
    def _analyze_burst_patterns(spike_data: List[Dict[str, Any]]):
        return {}
    
    @staticmethod
    def _calculate_synchrony(spike_data: List[Dict[str, Any]]):
        return 0.0
    
    @staticmethod
    def _build_connectivity_matrix(neural_regions: List[Any]):
        return []
    
    @staticmethod
    def _calculate_network_metrics(matrix: List[List[float]]):
        return {}
    
    @staticmethod
    def _identify_communities(matrix: List[List[float]]):
        return []
    
    @staticmethod
    def _fit_exponential_learning(trials: List[float], performance: List[float]):
        return {}
    
    @staticmethod
    def _fit_power_law_learning(trials: List[float], performance: List[float]):
        return {}
    
    @staticmethod
    def _fit_logarithmic_learning(trials: List[float], performance: List[float]):
        return {}
    
    @staticmethod
    def _calculate_goodness_of_fit(trials: List[float], performance: List[float],
                                   params: Dict[str, Any]):
        return {}


# Advanced convenience functions

def create_advanced_brain(config_dict: Optional[Dict[str, Any]] = None) -> nlm.Brain:
    """
    Create an advanced NLM brain with optimized configuration.
    
    Args:
        config_dict: Optional configuration dictionary
        
    Returns:
        Configured NLM brain instance
    """
    config = nlm.Config()
    
    if config_dict:
        # Apply configuration from dictionary
        for key, value in config_dict.items():
            if isinstance(value, dict):
                # Handle nested configurations
                pass
            else:
                # Set simple key-value pairs
                config[key] = value
    
    # Set advanced optimization parameters
    config['neuron_count'] = config_dict.get('neuron_count', 2000)
    config['region_count'] = config_dict.get('region_count', 3)
    config['connection_probability'] = config_dict.get('connection_probability', 0.15)
    config['simulation_timestep'] = config_dict.get('simulation_timestep', 0.0005)
    config['stdp_ltp_weight'] = config_dict.get('stdp_ltp_weight', 0.02)
    config['stdp_ltd_weight'] = config_dict.get('stdp_ltd_weight', 0.022)
    
    return nlm.Brain(config)


def create_advanced_world(config_dict: Optional[Dict[str, Any]] = None) -> nlm.SimpleWorld:
    """
    Create an advanced NLM world with enhanced features.
    
    Args:
        config_dict: Optional configuration dictionary
        
    Returns:
        Configured SimpleWorld instance
    """
    world = nlm.SimpleWorld()
    
    if config_dict:
        # Apply world configuration
        world.configure(
            width=config_dict.get('world_width', 100),
            height=config_dict.get('world_height', 100),
            vision_width=config_dict.get('vision_width', 32),
            vision_height=config_dict.get('vision_height', 32)
        )
        
        # Set world parameters
        world.setMaxEnergy(config_dict.get('max_energy', 1.0))
        world.setEnergyDecayRate(config_dict.get('energy_decay_rate', 0.01))
        world.setRandomSeed(config_dict.get('random_seed', 42))
    
    return world


def create_experimental_setup(config_dict: Optional[Dict[str, Any]] = None):
    """
    Create a complete experimental setup with brain and world.
    
    Args:
        config_dict: Configuration dictionary
        
    Returns:
        Tuple of (brain, world, experiment)
    """
    if config_dict is None:
        config_dict = {}
    
    brain = create_advanced_brain(config_dict)
    world = create_advanced_world(config_dict)
    
    # Initialize brain with world
    if hasattr(world, 'getAgentBody'):
        agent_brain = nlm.AgentBrain(brain)
        agent_brain.initialize(world)
    else:
        agent_brain = None
    
    # Create experiment controller
    experiment = NLMExperiment(brain, world)
    
    # Initialize brain if not already initialized
    if not brain.getConfig().has('initialized'):
        brain.initialize()
    
    return brain, world, experiment

# Version information
__version__ = "2.0.0"
__author__ = "NLM Project"
__license__ = "MIT"

# Export advanced utilities
__all__ = [
    'NLMExperiment',
    'NLMAnalyzer', 
    'NLMSimulationController',
    'create_advanced_brain',
    'create_advanced_world',
    'create_experimental_setup'
]
