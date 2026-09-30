#!/usr/bin/env python3
"""
Advanced Python utilities and features for NLM.

This module provides high-level utilities and advanced features for the NLM Python API,
including convenience functions, simulation tools, analysis utilities, and more.
"""

import sys
import tempfile
import os
import time
import math
from typing import List, Dict, Any, Optional, Callable, Union
import json

# Import pynlm
import pynlm
class SimulationManager:
    """
    High-level simulation manager for NLM.
    
    Provides advanced simulation control, checkpoint management,
    and experiment tracking capabilities.
    """
    
    def __init__(self, brain: pynlm.Brain, world: Optional[pynlm.SimpleWorld] = None, 
                 agent: Optional[pynlm.AgentBrain] = None):
        """
        Initialize simulation manager.
        
        Args:
            brain: The NLM brain
            world: Optional world for agent simulation
            agent: Optional agent brain interface
        """
        self.brain = brain
        self.world = world
        self.agent = agent
        self.config = brain.getConfig()
        self.sim_history: List[Dict[str, Any]] = []
        self.is_running = False
        self.start_time = 0.0
        
    def run_simulation(self, num_steps: int, timestep: float = 0.1, 
                      checkpoint_interval: int = 100) -> Dict[str, Any]:
        """
        Run a complete simulation with checkpointing and metrics collection.
        
        Args:
            num_steps: Number of simulation steps to run
            timestep: Time step for world updates
            checkpoint_interval: How often to save checkpoints
            
        Returns:
            Dictionary with simulation metrics
        """
        self.is_running = True
        self.start_time = time.time()
        self.sim_history.clear()
        
        print(f"Starting simulation: {num_steps} steps with dt={timestep}")
        
        # Initial state
        initial_spikes = self.brain.getTotalSpikeCount()
        initial_firing = self.brain.getFiringNeuronCount()
        initial_stage = self.brain.getDevelopmentalStage()
        
        try:
            for step in range(num_steps):
                # Update world if present
                if self.world:
                    self.world.update(timestep)
                    
                    if self.agent:
                        # Get sensory input
                        percept = self.world.getSensoryPercept()
                        
                        # Process sensory input
                        self.agent.processSensoryInput(percept)
                        
                        # Apply any reward modulation
                        reward = 0.0
                        if self.world.getSensoryPercept().getInternal():
                            reward = self.world.getSensoryPercept().getInternal()[0]
                        if reward != 0.0:
                            self.agent.applyRewardModulation(reward, 0.0)
                
                # Run brain step
                self.brain.step(step, step * timestep)
                
                # Get action if agent is present
                action = None
                if self.agent:
                    action = self.agent.decodeMotorCommand()
                    if self.world and action:
                        self.world.applyMotorCommand(action, step * timestep)
                
                # Update development
                if self.agent:
                    self.agent.updateDevelopment(timestep)
                
                # Record metrics every step
                sim_state = {
                    'step': step,
                    'time': step * timestep,
                    'total_spikes': self.brain.getTotalSpikeCount(),
                    'firing_neurons': self.brain.getFiringNeuronCount(),
                    'average_firing_rate': self.brain.getAverageFiringRate(),
                    'excitatory_inhibition_ratio': self.brain.getExcitationInhibitionRatio(),
                    'developmental_stage': self.brain.getDevelopmentalStage(),
                    'neuromodulation_level': self.agent.getNeuromodulationLevel() if self.agent else 0.0,
                    'curiosity_level': self.agent.getCuriosityLevel() if self.agent else 0.0,
                    'action': action.getName() if action else "None",
                    'reward': reward if 'reward' in locals() else 0.0
                }
                self.sim_history.append(sim_state)
                
                # Save checkpoint periodically
                if checkpoint_interval > 0 and step > 0 and step % checkpoint_interval == 0:
                    checkpoint_file = f"checkpoint_step_{step}.bin"
                    self.brain.save(checkpoint_file)
                    print(f"  Saved checkpoint: {checkpoint_file}")
                
                # Print progress every 10%
                if step % max(1, num_steps // 10) == 0:
                    print(f"  Progress: {step}/{num_steps} steps ({100.0 * step / num_steps:.1f}%)")
            
            end_time = time.time()
            elapsed = end_time - self.start_time
            
            print(f"Simulation completed in {elapsed:.2f} seconds ({elapsed/num_steps:.4f}s per step)")
            
            # Calculate final metrics
            final_spikes = self.brain.getTotalSpikeCount()
            final_firing = self.brain.getFiringNeuronCount()
            spike_increase = final_spikes - initial_spikes
            firing_increase = final_firing - initial_firing
            
            metrics = {
                'total_steps': num_steps,
                'elapsed_time': elapsed,
                'steps_per_second': num_steps / elapsed,
                'spike_increase': spike_increase,
                'firing_neuron_increase': firing_increase,
                'final_spike_rate': self.brain.getTotalSpikeCount(),
                'final_firing_rate': self.brain.getFiringNeuronCount(),
                'final_development_stage': self.brain.getDevelopmentalStage(),
                'final_neuromodulation': self.agent.getNeuromodulationLevel() if self.agent else 0.0,
                'max_firing_observed': max(s['firing_neurons'] for s in self.sim_history),
                'max_spike_increase': max(s['total_spikes'] - initial_spikes for s in self.sim_history),
                'action_summary': self._analyze_actions(),
                'reward_summary': self._analyze_rewards()
            }
            
            return metrics
            
        except Exception as e:
            print(f"Simulation error: {e}")
            raise
        finally:
            self.is_running = False
    
    def _analyze_actions(self) -> Dict[str, Any]:
        """Analyze action patterns."""
        if not self.sim_history:
            return {}
        
        actions = [s['action'] for s in self.sim_history]
        action_counts = {}
        for action in actions:
            action_counts[action] = action_counts.get(action, 0) + 1
        
        most_common_action = max(action_counts.items(), key=lambda x: x[1]) if action_counts else ("None", 0)
        
        return {
            'total_actions': len(actions),
            'unique_actions': len(set(actions)),
            'most_common_action': most_common_action[0],
            'most_common_count': most_common_action[1],
            'action_distribution': action_counts
        }
    
    def _analyze_rewards(self) -> Dict[str, Any]:
        """Analyze reward patterns."""
        if not self.sim_history:
            return {}
        
        rewards = [s['reward'] for s in self.sim_history if s['reward'] != 0.0]
        
        if not rewards:
            return {'total_rewards': 0, 'reward_events': []}
        
        positive_rewards = [r for r in rewards if r > 0]
        negative_rewards = [r for r in rewards if r < 0]
        
        return {
            'total_rewards': len(rewards),
            'positive_rewards': len(positive_rewards),
            'negative_rewards': len(negative_rewards),
            'max_positive_reward': max(positive_rewards) if positive_rewards else 0.0,
            'min_negative_reward': min(negative_rewards) if negative_rewards else 0.0,
            'total_reward_sum': sum(rewards)
        }
class ExperimentRunner:
    """
    High-level experiment runner for NLM.
    
    Provides tools for running multiple experiments, parameter sweeps,
    and comparative analysis.
    """
    
    def __init__(self, config: pynlm.Config):
        """
        Initialize experiment runner.
        
        Args:
            config: Base configuration template
        """
        self.base_config = config
        self.experiments = []
        
    def add_experiment(self, name: str, **kwargs):
        """
        Add an experiment with specific parameters.
        
        Args:
            name: Unique experiment name
            **kwargs: Configuration parameters to modify
        """
        experiment = {
            'name': name,
            'config': self._create_config(kwargs),
            'results': None
        }
        self.experiments.append(experiment)
        
    def _create_config(self, overrides: Dict[str, Any]) -> pynlm.Config:
        """Create a new config from base with parameter overrides."""
        config = pynlm.createDefaultConfig()
        
        # Apply base config values
        for key in self.base_config.getKeys():
            if key in self.base_config.getKeys():
                value = self.base_config.get(key)
                if value:
                    config.set(key, value)
        
        # Apply overrides
        for key, value in overrides.items():
            config.set(key, value)
        
        return config
    
    def run_all(self, world: Optional[pynlm.SimpleWorld] = None, 
                agent: bool = True) -> Dict[str, Any]:
        """
        Run all experiments and collect comparative results.
        
        Args:
            world: Shared world for experiments (optional)
            agent: Whether to create agent interfaces
            
        Returns:
            Dictionary with comparative results
        """
        results = {}
        
        for exp in self.experiments:
            print(f"\nRunning experiment: {exp['name']}")
            
            config = exp['config']
            brain = pynlm.createBrain(config)
            brain.initialize()
            
            current_agent = None
            if agent:
                current_agent = pynlm.createAgentBrain(brain)
                if world:
                    current_agent.initialize(world)
            
            # Run experiment
            exp_manager = SimulationManager(brain, world, current_agent)
            exp_results = exp_manager.run_simulation(
                num_steps=config.getOr("simulation.steps", 1000),
                timestep=config.getOr("simulation.timestep", 0.01)
            )
            
            exp['results'] = exp_results
            results[exp['name']] = exp_results
            
            print(f"  Completed: {exp_results['total_steps']} steps in {exp_results['elapsed_time']:.2f}s")
        
        return self._compare_experiments(results)
    
    def _compare_experiments(self, results: Dict[str, Any]) -> Dict[str, Any]:
        """Compare results across all experiments."""
        if len(results) < 2:
            return {'comparison': 'single_experiment', 'results': results}
        
        comparison = {
            'experiments': list(results.keys()),
            'comparisons': {}
        }
        
        # Compare key metrics
        metrics_to_compare = ['final_spike_rate', 'final_firing_rate', 
                             'steps_per_second', 'max_firing_observed']
        
        for metric in metrics_to_compare:
            comparison['comparisons'][metric] = {}
            for exp_name, exp_results in results.items():
                if metric in exp_results:
                    comparison['comparisons'][metric][exp_name] = exp_results[metric]
        
        # Find best/worst for each metric
        comparison['best_performers'] = {}
        for metric in metrics_to_compare:
            if metric in comparison['comparisons']:
                best_exp = max(comparison['comparisons'][metric].items(), 
                             key=lambda x: x[1] if x[1] is not None else 0)
                worst_exp = min(comparison['comparisons'][metric].items(),
                              key=lambda x: x[1] if x[1] is not None else 0)
                
                comparison['best_performers'][metric] = {
                    'best': best_exp,
                    'worst': worst_exp
                }
        
        return comparison
class ConfigOptimizer:
    """
    Configuration optimization utilities.
    
    Provides tools for finding optimal configuration parameters
    and automating parameter sweeps.
    """
    
    @staticmethod
    def parameter_sweep(base_config: pynlm.Config, parameter_ranges: Dict[str, List],
                       objective_function: Callable = None, num_trials: int = 10) -> Dict[str, Any]:
        """
        Perform parameter sweep optimization.
        
        Args:
            base_config: Base configuration
            parameter_ranges: Dictionary of parameter names to value ranges
            objective_function: Function to optimize (if None, uses spike rate)
            num_trials: Number of trials to run
            
        Returns:
            Dictionary with optimization results
        """
        if objective_function is None:
            def objective_function(sim_manager, brain):
                """Default objective: maximize spike production."""
                return brain.getTotalSpikeCount()
        
        trials = []
        best_result = None
        best_value = float('-inf')
        
        for trial in range(num_trials):
            print(f"\nTrial {trial + 1}/{num_trials}")
            
            # Generate random parameters within ranges
            config = pynlm.createDefaultConfig()
            
            for param_name, value_range in parameter_ranges.items():
                import random
                value = random.choice(value_range)
                config.set(param_name, value)
            
            # Run simulation
            brain = pynlm.createBrain(config)
            brain.initialize()
            
            world = pynlm.createSimpleWorld()
            world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
            world.reset()
            
            agent = pynlm.createAgentBrain(brain)
            agent.initialize(world)
            
            sim_manager = SimulationManager(brain, world, agent)
            sim_results = sim_manager.run_simulation(num_steps=500)
            
            # Evaluate objective
            objective_value = objective_function(sim_manager, brain)
            
            trial_result = {
                'trial': trial,
                'parameters': {param: value for param, value in zip(parameter_ranges.keys(), 
                                                                     [random.choice(v) for v in parameter_ranges.values()])},
                'objectives': {
                    'final_spikes': sim_results['final_spike_rate'],
                    'firing_rate': sim_results['final_firing_rate'],
                    'steps_per_second': sim_results['steps_per_second'],
                    'max_firing': sim_results['max_firing_observed']
                },
                'objective_value': objective_value,
                'simulation_results': sim_results
            }
            
            trials.append(trial_result)
            
            if objective_value > best_value:
                best_value = objective_value
                best_result = trial_result
                
                print(f"  New best: {objective_value:.2f}")
        
        return {
            'trials': trials,
            'best_trial': best_result,
            'best_objective_value': best_value,
            'parameter_ranges': parameter_ranges,
            'num_trials': num_trials
        }
    
    @staticmethod
    def grid_search(base_config: pynlm.Config, parameter_grids: Dict[str, List],
                   world_size: tuple = (20, 20), vision_size: tuple = (8, 8),
                   num_steps: int = 1000) -> Dict[str, Any]:
        """
        Perform systematic grid search over parameter space.
        
        Args:
            base_config: Base configuration
            parameter_grids: Dictionary of parameter names to value grids
            world_size: World dimensions
            vision_size: Vision dimensions
            num_steps: Number of simulation steps
            
        Returns:
            Dictionary with grid search results
        """
        from itertools import product
        
        # Generate all parameter combinations
        param_names = list(parameter_grids.keys())
        param_values = list(parameter_grids.values())
        
        total_combinations = 1
        for values in param_values:
            total_combinations *= len(values)
        
        print(f"Grid search: {total_combinations} combinations")
        
        if total_combinations > 100:
            print("Warning: Large number of combinations. Use subset or parameter_sweep instead.")
        
        results = []
        combination_count = 0
        
        for combination in product(*param_values):
            # Create parameter dict
            params = {param_names[i]: combination[i] for i in range(len(param_names))}
            
            # Run simulation
            config = pynlm.createDefaultConfig()
            
            # Apply base config
            for key in base_config.getKeys():
                value = base_config.get(key)
                if value:
                    config.set(key, value)
            
            # Apply combination parameters
            for key, value in params.items():
                config.set(key, value)
            
            brain = pynlm.createBrain(config)
            brain.initialize()
            
            world = pynlm.createSimpleWorld()
            world.configure(width=world_size[0], height=world_size[1], 
                           visionWidth=vision_size[0], visionHeight=vision_size[1])
            world.reset()
            
            agent = pynlm.createAgentBrain(brain)
            agent.initialize(world)
            
            sim_manager = SimulationManager(brain, world, agent)
            sim_results = sim_manager.run_simulation(num_steps=num_steps)
            
            result = {
                'parameters': params,
                'objectives': {
                    'final_spikes': sim_results['final_spike_rate'],
                    'firing_rate': sim_results['final_firing_rate'],
                    'steps_per_second': sim_results['steps_per_second'],
                    'max_firing': sim_results['max_firing_observed']
                },
                'simulation_results': sim_results
            }
            
            results.append(result)
            combination_count += 1
            
            if combination_count % 10 == 0 or combination_count == total_combinations:
                print(f"  Completed {combination_count}/{total_combinations} combinations")
        
        return {
            'results': results,
            'total_combinations': total_combinations,
            'parameter_grids': parameter_grids
        }
class DiagnosticTools:
    """
    Diagnostic and debugging tools for NLM.
    
    Provides tools for analyzing brain state, debugging issues,
    and monitoring simulation health.
    """
    
    @staticmethod
    def analyze_brain_state(brain: pynlm.Brain) -> Dict[str, Any]:
        """
        Analyze current brain state for potential issues.
        
        Args:
            brain: The brain to analyze
            
        Returns:
            Dictionary with diagnostic information
        """
        diagnostics = {
            'health_status': 'OK',
            'warnings': [],
            'metrics': {}
        }
        
        # Check basic metrics
        metrics = {
            'neuron_count': brain.getTotalNeuronCount(),
            'synapse_count': brain.getTotalSynapseCount(),
            'active_neurons': brain.getFiringNeuronCount(),
            'total_spikes': brain.getTotalSpikeCount(),
            'average_firing_rate': brain.getAverageFiringRate(),
            'excitatory_inhibition_ratio': brain.getExcitationInhibitionRatio(),
            'developmental_stage': brain.getDevelopmentalStage()
        }
        
        # Check for potential issues
        if metrics['neuron_count'] == 0:
            diagnostics['health_status'] = 'ERROR'
            diagnostics['warnings'].append('No neurons in brain')
        
        if metrics['synapse_count'] == 0:
            diagnostics['warnings'].append('No synapses in brain')
        
        if metrics['average_firing_rate'] == 0:
            diagnostics['warnings'].append('No firing activity detected')
        
        if metrics['excitatory_inhibition_ratio'] == 0:
            diagnostics['warnings'].append('E/I ratio is zero')
        
        if metrics['developmental_stage'] == 0:  # Initial stage
            diagnostics['warnings'].append('Brain is still in initial developmental stage')
        
        # Check for potential instability
        if metrics['active_neurons'] / metrics['neuron_count'] > 0.9:
            diagnostics['warnings'].append('Very high neuron activity - potential instability')
        
        diagnostics['metrics'] = metrics
        return diagnostics
    
    @staticmethod
    def monitor_simulation(sim_manager: SimulationManager, monitor_interval: int = 100) -> Dict[str, Any]:
        """
        Monitor simulation for anomalies and issues.
        
        Args:
            sim_manager: Simulation manager to monitor
            monitor_interval: How often to check for issues
            
        Returns:
            Dictionary with monitoring results
        """
        monitoring = {
            'intervals_checked': 0,
            'warnings_detected': [],
            'anomalies': [],
            'health_score': 100
        }
        
        if not sim_manager.sim_history:
            return monitoring
        
        # Check recent activity
        recent_history = sim_manager.sim_history[-monitor_interval:]
        
        for i, state in enumerate(recent_history):
            step = state['step']
            
            # Check for sudden spikes in activity
            if i > 0:
                prev_state = recent_history[i-1]
                spike_change = state['total_spikes'] - prev_state['total_spikes']
                
                if spike_change > 100:  # Sudden increase in spikes
                    monitoring['anomalies'].append({
                        'type': 'sudden_spike_increase',
                        'step': step,
                        'spike_change': spike_change,
                        'severity': 'high' if spike_change > 500 else 'medium' if spike_change > 200 else 'low'
                    })
                
                # Check for activity collapse
                firing_change = state['firing_neurons'] - prev_state['firing_neurons']
                
                if firing_change < -50:  # Sudden decrease in firing neurons
                    monitoring['anomalies'].append({
                        'type': 'activity_collapse',
                        'step': step,
                        'firing_change': firing_change,
                        'severity': 'high' if firing_change < -200 else 'medium' if firing_change < -100 else 'low'
                    })
        
        # Calculate health score based on stability
        if len(recent_history) >= 2:
            spike_stability = 1.0 / (1.0 + abs(recent_history[-1]['total_spikes'] - recent_history[0]['total_spikes']))
            firing_stability = 1.0 / (1.0 + abs(recent_history[-1]['firing_neurons'] - recent_history[0]['firing_neurons']))
            
            monitoring['health_score'] = (spike_stability + firing_stability) * 100 / 2
        
        monitoring['intervals_checked'] = len(recent_history)
        
        return monitoring
    
    @staticmethod
    def debug_sensory_processing(world: pynlm.SimpleWorld, agent: pynlm.AgentBrain, 
                                 brain: pynlm.Brain, num_steps: int = 10) -> Dict[str, Any]:
        """
        Debug sensory processing pipeline.
        
        Args:
            world: The world
            agent: The agent brain interface
            brain: The underlying brain
            num_steps: Number of steps to debug
            
        Returns:
            Dictionary with debug information
        """
        debug_info = {
            'steps_processed': 0,
            'sensory_analysis': [],
            'brain_response': [],
            'action_analysis': [],
            'issues': []
        }
        
        world.update(0.1)
        
        for step in range(num_steps):
            # Get sensory input
            percept = world.getSensoryPercept()
            vision = percept.getVision()
            
            # Analyze sensory data
            sensory_data = {
                'step': step,
                'vision_width': vision.getWidth(),
                'vision_height': vision.getHeight(),
                'vision_channels': vision.getChannels(),
                'vision_data_range': (min(vision.getData()), max(vision.getData())) if vision.getData() else (0, 0)
            }
            
            # Process sensory input
            agent.processSensoryInput(percept)
            
            # Get brain response
            brain_state = {
                'step': step,
                'total_spikes': brain.getTotalSpikeCount(),
                'firing_neurons': brain.getFiringNeuronCount(),
                'average_firing_rate': brain.getAverageFiringRate(),
                'action_required': True
            }
            
            # Get action
            action = agent.decodeMotorCommand()
            
            action_data = {
                'step': step,
                'action_type': action.getType().name if hasattr(action.getType(), 'name') else str(action.getType()),
                'action_name': action.getName(),
                'action_parameters': action.getParameters()
            }
            
            debug_info['sensory_analysis'].append(sensory_data)
            debug_info['brain_response'].append(brain_state)
            debug_info['action_analysis'].append(action_data)
            
            # Debug checks
            if not vision.getData():
                debug_info['issues'].append({
                    'step': step,
                    'issue': 'Empty vision data',
                    'severity': 'low'
                })
            
            if brain_state['firing_neurons'] == 0:
                debug_info['issues'].append({
                    'step': step,
                    'issue': 'No firing neurons after sensory input',
                    'severity': 'medium'
                })
            
            # Update world
            world.update(0.1)
        
        debug_info['steps_processed'] = num_steps
        return debug_info
class VisualizationTools:
    """
    Visualization and plotting utilities for NLM.
    
    Provides tools for creating visualizations of simulation data,
    brain states, and analysis results.
    """
    
    @staticmethod
    def create_activity_plots(sim_manager: SimulationManager, 
                             output_prefix: str = "nlm_plot") -> List[str]:
        """
        Create activity plots from simulation data.
        
        Args:
            sim_manager: Simulation manager with history
            output_prefix: Prefix for output files
            
        Returns:
            List of generated plot files
        """
        try:
            import matplotlib.pyplot as plt
            import matplotlib.dates as mdates
            from datetime import datetime
            
            plots_generated = []
            
            if not sim_manager.sim_history:
                print("No simulation history to plot")
                return plots_generated
            
            # Extract data
            steps = [s['step'] for s in sim_manager.sim_history]
            times = [s['time'] for s in sim_manager.sim_history]
            spike_counts = [s['total_spikes'] for s in sim_manager.sim_history]
            firing_neurons = [s['firing_neurons'] for s in sim_manager.sim_history]
            firing_rates = [s['average_firing_rate'] for s in sim_manager.sim_history]
            neuromod_levels = [s['neuromodulation_level'] for s in sim_manager.sim_history]
            curiosity_levels = [s['curiosity_level'] for s in sim_manager.sim_history]
            
            # Create figure with subplots
            fig, axes = plt.subplots(3, 2, figsize=(15, 12))
            fig.suptitle('NLM Simulation Analysis', fontsize=16, fontweight='bold')
            
            # Total spikes over time
            axes[0, 0].plot(times, spike_counts, 'r-', linewidth=2, label='Total Spikes')
            axes[0, 0].set_xlabel('Time (s)')
            axes[0, 0].set_ylabel('Spike Count', color='r')
            axes[0, 0].set_title('Spike Production Over Time')
            axes[0, 0].legend()
            axes[0, 0].grid(True, alpha=0.3)
            
            # Firing neurons over time
            axes[0, 1].plot(times, firing_neurons, 'g-', linewidth=2, label='Firing Neurons')
            axes[0, 1].set_xlabel('Time (s)')
            axes[0, 1].set_ylabel('Firing Neurons', color='g')
            axes[0, 1].set_title('Active Neurons Over Time')
            axes[0, 1].legend()
            axes[0, 1].grid(True, alpha=0.3)
            
            # Firing rate over time
            axes[1, 0].plot(times, firing_rates, 'b-', linewidth=2, label='Firing Rate')
            axes[1, 0].set_xlabel('Time (s)')
            axes[1, 0].set_ylabel('Firing Rate (Hz)', color='b')
            axes[1, 0].set_title('Average Firing Rate Over Time')
            axes[1, 0].legend()
            axes[1, 0].grid(True, alpha=0.3)
            
            # Neuromodulation levels
            axes[1, 1].plot(times, neuromod_levels, 'm-', linewidth=2, label='Neuromodulation')
            axes[1, 1].set_xlabel('Time (s)')
            axes[1, 1].set_ylabel('Neuromodulation Level', color='m')
            axes[1, 1].set_title('Neuromodulation Levels Over Time')
            axes[1, 1].legend()
            axes[1, 1].grid(True, alpha=0.3)
            
            # Curiosity levels
            axes[2, 0].plot(times, curiosity_levels, 'c-', linewidth=2, label='Curiosity')
            axes[2, 0].set_xlabel('Time (s)')
            axes[2, 0].set_ylabel('Curiosity Level', color='c')
            axes[2, 0].set_title('Curiosity Levels Over Time')
            axes[2, 0].legend()
            axes[2, 0].grid(True, alpha=0.3)
            
            # Action summary (if available)
            if sim_manager.sim_history and 'action' in sim_manager.sim_history[0]:
                actions = [s['action'] for s in sim_manager.sim_history if s['action'] != 'None']
                if actions:
                    # Count action occurrences
                    action_counts = {}
                    for action in actions:
                        action_counts[action] = action_counts.get(action, 0) + 1
                    
                    actions_list = list(action_counts.keys())
                    counts = list(action_counts.values())
                    
                    axes[2, 1].bar(range(len(actions_list)), counts, alpha=0.7)
                    axes[2, 1].set_xlabel('Actions')
                    axes[2, 1].set_ylabel('Frequency')
                    axes[2, 1].set_title('Action Frequency Distribution')
                    axes[2, 1].set_xticks(range(len(actions_list)))
                    axes[2, 1].set_xticklabels(actions_list, rotation=45)
                else:
                    axes[2, 1].text(0.5, 0.5, 'No Actions Recorded', 
                                  ha='center', va='center', transform=axes[2, 1].transAxes)
                    axes[2, 1].set_title('Action Summary')
            else:
                axes[2, 1].text(0.5, 0.5, 'Action Data Not Available', 
                              ha='center', va='center', transform=axes[2, 1].transAxes)
                axes[2, 1].set_title('Action Summary')
            
            plt.tight_layout()
            
            # Save plots
            plot_file = f"{output_prefix}_analysis.png"
            plt.savefig(plot_file, dpi=150, bbox_inches='tight')
            plots_generated.append(plot_file)
            
            # Save individual metrics as CSV
            csv_file = f"{output_prefix}_metrics.csv"
            import pandas as pd
            
            df_data = {
                'step': steps,
                'time': times,
                'total_spikes': spike_counts,
                'firing_neurons': firing_neurons,
                'firing_rate': firing_rates,
                'neuromodulation': neuromod_levels,
                'curiosity': curiosity_levels
            }
            
            df = pd.DataFrame(df_data)
            df.to_csv(csv_file, index=False)
            plots_generated.append(csv_file)
            
            plt.close()
            
            print(f"Generated plots: {plots_generated}")
            return plots_generated
            
        except ImportError:
            print("Matplotlib not available. Install with: pip install matplotlib pandas")
            return []
    
    @staticmethod
    def create_brain_state_plot(brain: pynlm.Brain, output_file: str = "brain_state.png") -> str:
        """
        Create a visualization of current brain state.
        
        Args:
            brain: The brain to visualize
            output_file: Output filename
            
        Returns:
            Output filename
        """
        try:
            import matplotlib.pyplot as plt
            import numpy as np
            
            # Create figure
            fig, axes = plt.subplots(2, 2, figsize=(12, 10))
            fig.suptitle('NLM Brain State Analysis', fontsize=16, fontweight='bold')
            
            # Basic metrics
            metrics = {
                'Neurons': brain.getTotalNeuronCount(),
                'Synapses': brain.getTotalSynapseCount(),
                'Active Neurons': brain.getFiringNeuronCount(),
                'Total Spikes': brain.getTotalSpikeCount(),
                'E/I Ratio': brain.getExcitationInhibitionRatio(),
                'Avg Firing Rate': brain.getAverageFiringRate(),
                'Development Stage': brain.getDevelopmentalStage()
            }
            
            # Create metrics bar chart
            axes[0, 0].bar(range(len(metrics)), list(metrics.values()), alpha=0.7)
            axes[0, 0].set_xticks(range(len(metrics)))
            axes[0, 0].set_xticklabels(list(metrics.keys()), rotation=45)
            axes[0, 0].set_title('Brain Metrics')
            axes[0, 0].set_ylabel('Value')
            axes[0, 0].grid(True, alpha=0.3)
            
            # Developmental stage indicator
            stage_colors = ['red', 'orange', 'yellow', 'green', 'blue']
            stage = brain.getDevelopmentalStage()
            stage_names = ['Initial', 'Critical Period', 'Maturation', 'Adult', 'Aging']
            
            axes[0, 1].bar(stage_names, [0] * len(stage_names), color='lightgray', alpha=0.5)
            axes[0, 1].bar(stage_names[stage], [1], color=stage_colors[stage], alpha=0.8)
            axes[0, 1].set_title(f'Developmental Stage: {stage_names[stage]}')
            axes[0, 1].set_ylabel('Current Stage')
            axes[0, 1].set_yticks([0, 1])
            axes[0, 1].grid(True, alpha=0.3)
            
            # Brain statistics
            axes[1, 0].axis('tight')
            axes[1, 0].axis('off')
            
            stats_text = f"\n".join([
                f"Total Neurons: {brain.getTotalNeuronCount():,}",
                f"Total Synapses: {brain.getTotalSynapseCount():,}",
                f"Active Neurons: {brain.getFiringNeuronCount()}",
                f"Activity Percentage: {brain.getFiringNeuronCount()/max(1,brain.getTotalNeuronCount())*100:.1f}%",
                f"Total Spikes: {brain.getTotalSpikeCount():,}",
                f"Average Firing Rate: {brain.getAverageFiringRate():.2f} Hz",
                f"E/I Balance: {brain.getExcitationInhibitionRatio():.3f}",
                f"Development: {brain.getDevelopmentalStage()}"
            ])
            
            axes[1, 0].text(0.1, 0.5, stats_text, fontsize=12, 
                          verticalalignment='center', fontfamily='monospace')
            axes[1, 0].set_title('Detailed Statistics')
            
            # Performance indicator
            axes[1, 1].bar(['Firing Efficiency', 'Synapse Density', 'Development Progress'], 
                          [min(brain.getAverageFiringRate()/10, 1.0), 
                           min(brain.getTotalSynapseCount()/brain.getTotalNeuronCount()/10, 1.0),
                           min(brain.getDevelopmentalStage()/4, 1.0)],
                          color=['skyblue', 'lightgreen', 'lightcoral'], alpha=0.7)
            axes[1, 1].set_title('Performance Indicators')
            axes[1, 1].set_ylabel('Normalized Score (0-1)')
            axes[1, 1].set_ylim(0, 1)
            axes[1, 1].grid(True, alpha=0.3)
            
            plt.tight_layout()
            plt.savefig(output_file, dpi=150, bbox_inches='tight')
            plt.close()
            
            print(f"Brain state plot saved to: {output_file}")
            return output_file
            
        except ImportError:
            print("Matplotlib not available. Install with: pip install matplotlib")
            return ""
class ExportTools:
    """
    Export and import utilities for NLM data.
    
    Provides tools for exporting simulation results,
    saving brain states, and importing data.
    """
    
    @staticmethod
    def export_simulation_data(sim_manager: SimulationManager, format: str = "json") -> str:
        """
        Export simulation data to file.
        
        Args:
            sim_manager: Simulation manager with history
            format: Export format (json, csv, or pickle)
            
        Returns:
            Output filename
        """
        if not sim_manager.sim_history:
            raise ValueError("No simulation history to export")
        
        if format == "json":
            return ExportTools._export_json(sim_manager.sim_history)
        elif format == "csv":
            return ExportTools._export_csv(sim_manager.sim_history)
        elif format == "pickle":
            return ExportTools._export_pickle(sim_manager.sim_history)
        else:
            raise ValueError(f"Unsupported format: {format}")
    
    @staticmethod
    def _export_json(history: List[Dict[str, Any]]) -> str:
        """Export simulation history as JSON."""
        # Create a JSON-serializable version
        export_data = {
            'timestamp': time.time(),
            'steps': [],
            'metrics_summary': {
                'total_steps': len(history),
                'max_spikes': max(h['total_spikes'] for h in history),
                'max_firing': max(h['firing_neurons'] for h in history),
                'avg_firing_rate': sum(h['average_firing_rate'] for h in history) / len(history)
            }
        }
        
        for step_data in history:
            # Convert numpy arrays to lists for JSON serialization
            json_step = {}
            for key, value in step_data.items():
                if isinstance(value, (list, tuple)):
                    json_step[key] = list(value)
                elif isinstance(value, dict):
                    json_step[key] = value
                else:
                    json_step[key] = value
            
            export_data['steps'].append(json_step)
        
        filename = f"simulation_export_{int(time.time())}.json"
        
        with open(filename, 'w') as f:
            json.dump(export_data, f, indent=2, default=str)
        
        print(f"Exported simulation data to JSON: {filename}")
        return filename
    
    @staticmethod
    def _export_csv(history: List[Dict[str, Any]]) -> str:
        """Export simulation history as CSV."""
        try:
            import pandas as pd
            
            # Convert to DataFrame
            df = pd.DataFrame(history)
            
            filename = f"simulation_export_{int(time.time())}.csv"
            df.to_csv(filename, index=False)
            
            print(f"Exported simulation data to CSV: {filename}")
            return filename
            
        except ImportError:
            print("Pandas not available. Install with: pip install pandas")
            return ""
    
    @staticmethod
    def _export_pickle(history: List[Dict[str, Any]]) -> str:
        """Export simulation history as pickle."""
        try:
            import pickle
            
            filename = f"simulation_export_{int(time.time())}.pkl"
            
            with open(filename, 'wb') as f:
                pickle.dump(history, f)
            
            print(f"Exported simulation data to pickle: {filename}")
            return filename
            
        except ImportError:
            print("Pickle module not available")
            return ""
    
    @staticmethod
    def import_simulation_data(filename: str) -> List[Dict[str, Any]]:
        """
        Import simulation data from file.
        
        Args:
            filename: Input filename
            
        Returns:
            Imported simulation history
        """
        if filename.endswith('.json'):
            return ExportTools._import_json(filename)
        elif filename.endswith('.csv'):
            return ExportTools._import_csv(filename)
        elif filename.endswith('.pkl'):
            return ExportTools._import_pickle(filename)
        else:
            raise ValueError(f"Unsupported file format: {filename}")
    
    @staticmethod
    def _import_json(filename: str) -> List[Dict[str, Any]]:
        """Import simulation history from JSON."""
        with open(filename, 'r') as f:
            data = json.load(f)
        
        return data['steps']
    
    @staticmethod
    def _import_csv(filename: str) -> List[Dict[str, Any]]:
        """Import simulation history from CSV."""
        try:
            import pandas as pd
            
            df = pd.read_csv(filename)
            return df.to_dict('records')
            
        except ImportError:
            print("Pandas not available. Install with: pip install pandas")
            return []
    
    @staticmethod
    def _import_pickle(filename: str) -> List[Dict[str, Any]]:
        """Import simulation history from pickle."""
        try:
            import pickle
            
            with open(filename, 'rb') as f:
                data = pickle.load(f)
            
            return data
            
        except ImportError:
            print("Pickle module not available")
            return []
class AdvancedAPI:
    """
    Advanced NLM Python API with high-level convenience functions.
    
    This class provides advanced functionality for experienced users,
    including experiment management, optimization, and advanced analysis.
    """
    
    def __init__(self):
        """Initialize advanced API."""
        self.experiment_runner = None
        self.diagnostic_tools = DiagnosticTools()
        self.export_tools = ExportTools()
        self.simulation_manager_class = SimulationManager
        
    def create_experiment(self, config_template: pynlm.Config, name: str, **kwargs):
        """
        Create a new experiment with parameter overrides.
        
        Args:
            config_template: Base configuration template
            name: Experiment name
            **kwargs: Parameter overrides
            
        Returns:
            Experiment dictionary
        """
        if self.experiment_runner is None:
            self.experiment_runner = ExperimentRunner(config_template)
        
        self.experiment_runner.add_experiment(name, **kwargs)
        return {'name': name, 'config_template': config_template}
    
    def run_experiment(self, experiment_name: str, world_size: tuple = (20, 20),
                      vision_size: tuple = (8, 8), num_steps: int = 1000) -> Dict[str, Any]:
        """
        Run a specific experiment.
        
        Args:
            experiment_name: Name of experiment to run
            world_size: World dimensions
            vision_size: Vision dimensions
            num_steps: Number of simulation steps
            
        Returns:
            Experiment results
        """
        if self.experiment_runner is None:
            raise ValueError("No experiments defined. Use create_experiment() first.")
        
        for exp in self.experiment_runner.experiments:
            if exp['name'] == experiment_name:
                # Create world
                world = pynlm.createSimpleWorld()
                world.configure(width=world_size[0], height=world_size[1],
                               visionWidth=vision_size[0], visionHeight=vision_size[1])
                world.reset()
                
                # Run experiment
                return self.experiment_runner.run_all(world=world, agent=True)
        
        raise ValueError(f"Experiment not found: {experiment_name}")
    
    def optimize_parameters(self, base_config: pynlm.Config, parameter_ranges: Dict[str, List],
                           objective_function: Callable = None, num_trials: int = 10) -> Dict[str, Any]:
        """
        Optimize configuration parameters.
        
        Args:
            base_config: Base configuration
            parameter_ranges: Parameter ranges for optimization
            objective_function: Function to optimize
            num_trials: Number of optimization trials
            
        Returns:
            Optimization results
        """
        return ConfigOptimizer.parameter_sweep(
            base_config, parameter_ranges, objective_function, num_trials
        )
    
    def grid_search(self, base_config: pynlm.Config, parameter_grids: Dict[str, List],
                   world_size: tuple = (20, 20), vision_size: tuple = (8, 8),
                   num_steps: int = 1000) -> Dict[str, Any]:
        """
        Perform grid search over parameter space.
        
        Args:
            base_config: Base configuration
            parameter_grids: Parameter grids for search
            world_size: World dimensions
            vision_size: Vision dimensions
            num_steps: Number of simulation steps
            
        Returns:
            Grid search results
        """
        return ConfigOptimizer.grid_search(
            base_config, parameter_grids, world_size, vision_size, num_steps
        )
    
    def analyze_simulation(self, sim_manager: SimulationManager) -> Dict[str, Any]:
        """
        Perform comprehensive analysis of simulation.
        
        Args:
            sim_manager: Simulation manager with history
            
        Returns:
            Analysis results
        """
        analysis = {
            'basic_metrics': {
                'total_steps': len(sim_manager.sim_history),
                'time_elapsed': sim_manager.sim_history[-1]['time'] if sim_manager.sim_history else 0,
                'spike_production': sum(h['total_spikes'] for h in sim_manager.sim_history),
                'activity_pattern': self._analyze_activity_pattern(sim_manager.sim_history)
            },
            'neuromodulation_analysis': self._analyze_neuromodulation(sim_manager.sim_history),
            'action_analysis': self._analyze_actions(sim_manager.sim_history),
            'development_analysis': self._analyze_development(sim_manager.sim_history)
        }
        
        return analysis
    
    def _analyze_activity_pattern(self, history: List[Dict[str, Any]]) -> Dict[str, Any]:
        """Analyze activity patterns over time."""
        if not history:
            return {}
        
        spikes = [h['total_spikes'] for h in history]
        firing = [h['firing_neurons'] for h in history]
        
        return {
            'spike_trend': 'increasing' if spikes[-1] > spikes[0] else 'decreasing' if spikes[-1] < spikes[0] else 'stable',
            'firing_trend': 'increasing' if firing[-1] > firing[0] else 'decreasing' if firing[-1] < firing[0] else 'stable',
            'max_spike_rate': max(spikes) / history[-1]['time'] if history[-1]['time'] > 0 else 0,
            'cv_spikes': np.std(spikes) / np.mean(spikes) if np.mean(spikes) > 0 else 0,
            'cv_firing': np.std(firing) / np.mean(firing) if np.mean(firing) > 0 else 0
        }
    
    def _analyze_neuromodulation(self, history: List[Dict[str, Any]]) -> Dict[str, Any]:
        """Analyze neuromodulation patterns."""
        if not history:
            return {}
        
        neuromod = [h.get('neuromodulation_level', 0) for h in history]
        curiosity = [h.get('curiosity_level', 0) for h in history]
        
        return {
            'neuromodulation_mean': np.mean(neuromod),
            'neuromodulation_std': np.std(neuromod),
            'curiosity_mean': np.mean(curiosity),
            'curiosity_std': np.std(curiosity),
            'neuromodulation_curiosity_correlation': np.corrcoef(neuromod, curiosity)[0, 1] if len(neuromod) > 1 else 0
        }
    
    def _analyze_actions(self, history: List[Dict[str, Any]]) -> Dict[str, Any]:
        """Analyze action patterns."""
        if not history:
            return {}
        
        actions = [h.get('action', 'None') for h in history if h.get('action') != 'None']
        
        if not actions:
            return {'total_actions': 0, 'action_types': []}
        
        action_counts = {}
        for action in actions:
            action_counts[action] = action_counts.get(action, 0) + 1
        
        return {
            'total_actions': len(actions),
            'unique_actions': len(set(actions)),
            'most_common_action': max(action_counts.items(), key=lambda x: x[1]) if action_counts else ('None', 0),
            'action_distribution': action_counts
        }
    
    def _analyze_development(self, history: List[Dict[str, Any]]) -> Dict[str, Any]:
        """Analyze development patterns."""
        if not history:
            return {}
        
        stages = [h.get('developmental_stage', 0) for h in history]
        
        return {
            'stage_transitions': len(set(stages)),
            'final_stage': stages[-1],
            'development_time': stages[-1] / max(1, len(history)),
            'stage_stability': len(set(stages)) / len(stages) if stages else 0
        }
# Global advanced API instance
advanced = AdvancedAPI()

# High-level convenience functions
def create_default_simulation(brain_config: Optional[pynlm.Config] = None,
                            world_size: tuple = (20, 20),
                            vision_size: tuple = (8, 8),
                            enable_agent: bool = True) -> Dict[str, Any]:
    """
    Create a default simulation with recommended settings.
    
    Args:
        brain_config: Brain configuration (if None, uses default)
        world_size: World dimensions (width, height)
        vision_size: Vision dimensions (width, height)
        enable_agent: Whether to create agent interface
        
    Returns:
        Dictionary with simulation components
    """
    if brain_config is None:
        brain_config = pynlm.createDefaultConfig()
    
    # Create components
    brain = pynlm.createBrain(brain_config)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(width=world_size[0], height=world_size[1],
                   visionWidth=vision_size[0], visionHeight=vision_size[1])
    world.reset()
    
    result = {
        'brain': brain,
        'world': world,
        'agent': None,
        'simulation_manager': SimulationManager(brain, world)
    }
    
    if enable_agent:
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        result['agent'] = agent
        result['simulation_manager'] = SimulationManager(brain, world, agent)
    
    print("Created default simulation with:")
    print(f"  - {brain.getTotalNeuronCount()} neurons")
    print(f"  - {brain.getTotalSynapseCount()} synapses")
    print(f"  - World: {world_size[0]}x{world_size[1]}")
    print(f"  - Vision: {vision_size[0]}x{vision_size[1]}")
    print(f"  - Agent: {'Enabled' if enable_agent else 'Disabled'}")
    
    return result
def run_quick_test(steps: int = 100, timestep: float = 0.1) -> Dict[str, Any]:
    """
    Run a quick test simulation to verify everything works.
    
    Args:
        steps: Number of simulation steps
        timestep: Time step size
        
    Returns:
        Simulation results
    """
    print("Running quick test simulation...")
    
    # Create simulation
    sim_result = create_default_simulation(world_size=(10, 10), vision_size=(4, 4))
    
    # Run simulation
    sim_manager = sim_result['simulation_manager']
    metrics = sim_manager.run_simulation(steps, timestep)
    
    print(f"Quick test completed: {metrics['total_steps']} steps in {metrics['elapsed_time']:.2f}s")
    print(f"Final spike rate: {metrics['final_spike_rate']}")
    print(f"Final firing rate: {metrics['final_firing_rate']}")
    
    return metrics
def analyze_brain_state(brain: pynlm.Brain) -> Dict[str, Any]:
    """
    Analyze brain state using advanced diagnostic tools.
    
    Args:
        brain: Brain to analyze
        
    Returns:
        Diagnostic analysis
    """
    return DiagnosticTools.analyze_brain_state(brain)
def debug_simulation(world: pynlm.SimpleWorld, agent: pynlm.AgentBrain, 
                    brain: pynlm.Brain, steps: int = 10) -> Dict[str, Any]:
    """
    Debug simulation for troubleshooting.
    
    Args:
        world: Simulation world
        agent: Agent brain interface
        brain: Underlying brain
        steps: Number of steps to debug
        
    Returns:
        Debug information
    """
    return DiagnosticTools.debug_sensory_processing(world, agent, brain, steps)
# Add advanced API to pynlm module
pynlm.SimulationManager = SimulationManager
pynlm.ExperimentRunner = ExperimentRunner
pynlm.ConfigOptimizer = ConfigOptimizer
pynlm.DiagnosticTools = DiagnosticTools
pynlm.VisualizationTools = VisualizationTools
pynlm.ExportTools = ExportTools
pynlm.advanced = advanced

# Add convenience functions to pynlm module
pynlm.create_default_simulation = create_default_simulation
pynlm.run_quick_test = run_quick_test
pynlm.analyze_brain_state = analyze_brain_state
pynlm.debug_simulation = debug_simulation

# Mark the advanced API module
__all__ = [
    'SimulationManager',
    'ExperimentRunner', 
    'ConfigOptimizer',
    'DiagnosticTools',
    'VisualizationTools',
    'ExportTools',
    'AdvancedAPI',
    'advanced',
    'create_default_simulation',
    'run_quick_test',
    'analyze_brain_state',
    'debug_simulation'
]

if __name__ == '__main__':
    print("NLM Advanced Python API Module")
    print("=" * 40)
    print("This module provides advanced simulation management,")
    print("experiment running, parameter optimization, and analysis")
    print("tools for the NLM Python bindings.")
    print()
    print("Key features:")
    print("  - SimulationManager: High-level simulation control")
    print("  - ExperimentRunner: Multiple experiment management")
    print("  - ConfigOptimizer: Parameter optimization and sweeps")
    print("  - DiagnosticTools: Brain state analysis and debugging")
    print("  - VisualizationTools: Plotting and visualization")
    print("  - ExportTools: Data export and import")
    print()
    print("Usage examples:")
    print("  from pynlm import advanced")
    print("  sim = advanced.create_default_simulation()")
    print("  results = sim['simulation_manager'].run_simulation(1000)")
    print()
    print("For documentation, see the module docstring or documentation.")