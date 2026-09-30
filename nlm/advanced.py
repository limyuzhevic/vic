#!/usr/bin/env python3
"""
Advanced features for NLM Python API

This module provides advanced features for the NLM (Neural Learning Machine) Python API,
including experiment management, advanced visualization, custom environments,
and sophisticated analysis tools.

Usage:
    import nlm
    from nlm.advanced import AdvancedExperiment, CustomEnvironment
    
    # Create an advanced experiment
    experiment = AdvancedExperiment("my_experiment")
    experiment.run()
"""

import json
import yaml
import numpy as np
import matplotlib.pyplot as plt
from typing import Dict, List, Any, Optional, Callable, Union
from datetime import datetime
import time
import os

# Import NLM core classes
from .pynlm import (
    Config, 
    Brain, 
    SimpleWorld, 
    AgentBrain, 
    Action, 
    SensoryPercept,
    Vision,
    Audio,
    InternalSignals,
    ActionType,
    NeuronType,
    DevelopmentalStage,
    NeuronId,
    SynapseId,
    RegionId,
    PopulationId
)

class AdvancedExperiment:
    """Advanced experiment framework for NLM simulations.
    
    This class provides comprehensive experiment management capabilities,
    including parameter sweeps, multi-environment testing, and detailed
    results analysis.
    
    Attributes:
        name (str): Name of the experiment
        config (dict): Experiment configuration
        results (dict): Store experiment results
    """
    
    def __init__(self, name: str, config: Optional[Dict] = None):
        """Initialize an advanced experiment.
        
        Args:
            name (str): Name of the experiment
            config (dict, optional): Configuration for the experiment
        """
        self.name = name
        self.config = config or {}
        self.results = {}
        self.start_time = None
        self.end_time = None
        self.experiments = []
        
        # Validate configuration
        self._validate_config()
    
    def _validate_config(self):
        """Validate experiment configuration."""
        required_keys = ['description']
        for key in required_keys:
            if key not in self.config:
                self.config[key] = ""
    
    def run_parameter_sweep(self, param_grid: Dict[str, List], brain_factory: Callable) -> Dict:
        """Run a parameter sweep experiment.
        
        Args:
            param_grid (dict): Dictionary of parameter names to lists of values
            brain_factory (callable): Function that creates a brain with given parameters
            
        Returns:
            dict: Results from parameter sweep
        """
        print(f"Running parameter sweep for experiment: {self.name}")
        print(f"Parameter grid: {param_grid}")
        
        results = []
        
        # Generate all parameter combinations
        param_combinations = self._generate_parameter_combinations(param_grid)
        
        for i, params in enumerate(param_combinations):
            print(f"Running experiment {i+1}/{len(param_combinations)}: {params}")
            
            try:
                # Create brain with specific parameters
                brain = brain_factory(params)
                
                # Run experiment
                experiment_result = self._run_single_experiment(brain, params)
                
                # Store results
                results.append({
                    'parameters': params,
                    'result': experiment_result,
                    'success': True
                })
                
            except Exception as e:
                print(f"Experiment failed with parameters {params}: {e}")
                results.append({
                    'parameters': params,
                    'result': None,
                    'success': False,
                    'error': str(e)
                })
        
        self.results['parameter_sweep'] = results
        return results
    
    def _generate_parameter_combinations(self, param_grid: Dict[str, List]) -> List[Dict]:
        """Generate all combinations of parameter values."""
        from itertools import product
        
        keys = list(param_grid.keys())
        values = list(param_grid.values())
        
        combinations = []
        for combination in product(*values):
            combinations.append(dict(zip(keys, combination)))
        
        return combinations
    
    def _run_single_experiment(self, brain: Brain, params: Dict) -> Dict:
        """Run a single experiment with given brain and parameters."""
        # Create world
        world = SimpleWorld()
        world.configure(
            params.get('world_width', 100),
            params.get('world_height', 100),
            params.get('vision_width', 10),
            params.get('vision_height', 10)
        )
        
        # Create agent brain
        agent_brain = AgentBrain(brain)
        agent_brain.initialize(world)
        
        # Run simulation
        num_steps = params.get('num_steps', 100)
        actions = []
        
        for step in range(num_steps):
            # Get sensory percept
            percept = world.getSensoryPercept()
            
            # Process input
            agent_brain.processSensoryInput(percept)
            
            # Produce action
            action = agent_brain.decodeMotorCommand()
            actions.append(action)
            
            # Apply action to world
            world.applyMotorCommand(action, world.getSimulationTime())
            
            # Update world
            world.update(0.01)
        
        # Calculate statistics
        stats = self._calculate_experiment_stats(actions, brain)
        
        return {
            'actions': actions,
            'statistics': stats,
            'final_state': brain.logStatus()
        }
    
    def _calculate_experiment_stats(self, actions: List[Action], brain: Brain) -> Dict:
        """Calculate statistics for experiment results."""
        # Analyze actions
        action_types = {}
        for action in actions:
            action_type = action.getType()
            action_types[str(action_type)] = action_types.get(str(action_type), 0) + 1
        
        # Get brain statistics
        return {
            'total_actions': len(actions),
            'action_distribution': action_types,
            'total_neurons': brain.getTotalNeuronCount(),
            'total_synapses': brain.getTotalSynapseCount(),
            'active_neurons': brain.getActiveNeuronCount(),
            'firing_neurons': brain.getFiringNeuronCount(),
            'average_firing_rate': brain.getAverageFiringRate(),
            'excitation_inhibition_ratio': brain.getExcitationInhibitionRatio(),
            'total_spikes': brain.getTotalSpikeCount(),
            'developmental_stage': brain.getDevelopmentalStage()
        }
    
    def run_multi_environment(self, environments: List[Dict], brain_factory: Callable) -> Dict:
        """Run experiment across multiple different environments.
        
        Args:
            environments (list): List of environment configurations
            brain_factory (callable): Function that creates a brain for each environment
            
        Returns:
            dict: Results from multi-environment experiment
        """
        print(f"Running multi-environment experiment: {self.name}")
        print(f"Testing {len(environments)} environments")
        
        results = []
        
        for i, env_config in enumerate(environments):
            print(f"Running environment {i+1}/{len(environments)}")
            
            try:
                # Create brain for this environment
                brain = brain_factory(env_config)
                
                # Create and configure world for this environment
                world = SimpleWorld()
                world.configure(
                    env_config.get('width', 100),
                    env_config.get('height', 100),
                    env_config.get('vision_width', 10),
                    env_config.get('vision_height', 10)
                )
                
                # Set agent start position
                world.setAgentStart(
                    env_config.get('start_x', 50),
                    env_config.get('start_y', 50)
                )
                
                # Create agent brain
                agent_brain = AgentBrain(brain)
                agent_brain.initialize(world)
                
                # Run simulation
                num_steps = env_config.get('steps', 100)
                env_results = []
                
                for step in range(num_steps):
                    # Get sensory percept
                    percept = world.getSensoryPercept()
                    
                    # Process input
                    agent_brain.processSensoryInput(percept)
                    
                    # Produce action
                    action = agent_brain.decodeMotorCommand()
                    env_results.append(action)
                    
                    # Apply action to world
                    world.applyMotorCommand(action, world.getSimulationTime())
                    
                    # Update world
                    world.update(0.01)
                
                # Calculate environment statistics
                env_stats = self._calculate_environment_stats(env_results, brain)
                
                results.append({
                    'environment_config': env_config,
                    'results': env_results,
                    'statistics': env_stats,
                    'success': True
                })
                
            except Exception as e:
                print(f"Environment experiment failed: {e}")
                results.append({
                    'environment_config': env_config,
                    'results': None,
                    'statistics': None,
                    'success': False,
                    'error': str(e)
                })
        
        self.results['multi_environment'] = results
        return results
    
    def _calculate_environment_stats(self, actions: List[Action], brain: Brain) -> Dict:
        """Calculate statistics for environment results."""
        # Calculate action efficiency
        successful_actions = sum(1 for action in actions 
                                if action.getType() != ActionType.Wait)
        
        efficiency = successful_actions / len(actions) if actions else 0
        
        return {
            'total_actions': len(actions),
            'successful_actions': successful_actions,
            'efficiency': efficiency,
            'brain_stats': {
                'total_neurons': brain.getTotalNeuronCount(),
                'total_synapses': brain.getTotalSynapseCount(),
                'active_neurons': brain.getActiveNeuronCount(),
                'firing_neurons': brain.getFiringNeuronCount(),
                'average_firing_rate': brain.getAverageFiringRate(),
                'total_spikes': brain.getTotalSpikeCount(),
                'developmental_stage': brain.getDevelopmentalStage()
            }
        }
    
    def run_custom_agent(self, agent_factory: Callable, num_episodes: int = 100) -> Dict:
        """Run custom agent implementation.
        
        Args:
            agent_factory (callable): Function that creates a custom agent
            num_episodes (int): Number of episodes to run
            
        Returns:
            dict: Results from custom agent experiment
        """
        print(f"Running custom agent experiment: {self.name}")
        print(f"Running {num_episodes} episodes")
        
        episodes = []
        
        for episode in range(num_episodes):
            try:
                # Create custom agent
                agent = agent_factory(episode)
                
                # Run episode
                episode_result = self._run_episode(agent)
                
                episodes.append({
                    'episode': episode,
                    'result': episode_result,
                    'success': True
                })
                
                # Periodic checkpoint
                if episode % 10 == 0:
                    self._save_checkpoint(episode)
                
            except Exception as e:
                print(f"Episode {episode} failed: {e}")
                episodes.append({
                    'episode': episode,
                    'result': None,
                    'success': False,
                    'error': str(e)
                })
        
        self.results['custom_agent'] = episodes
        return episodes
    
    def _run_episode(self, agent) -> Dict:
        """Run a single episode with the given agent."""
        # This is a simplified example
        # In practice, you would implement custom episode logic
        
        # For now, use the basic agent brain interface
        # You would replace this with your custom agent logic
        
        # Example: Get agent's brain and world
        brain = agent.getBrain()
        world = agent.getWorld()
        
        # Create agent brain
        agent_brain = AgentBrain(brain)
        agent_brain.initialize(world)
        
        # Run episode
        num_steps = self.config.get('episode_steps', 100)
        actions = []
        
        for step in range(num_steps):
            # Get sensory percept
            percept = world.getSensoryPercept()
            
            # Process input
            agent_brain.processSensoryInput(percept)
            
            # Produce action
            action = agent_brain.decodeMotorCommand()
            actions.append(action)
            
            # Apply action to world
            world.applyMotorCommand(action, world.getSimulationTime())
            
            # Update world
            world.update(0.01)
        
        return {
            'actions': actions,
            'final_state': brain.logStatus()
        }
    
    def _save_checkpoint(self, episode: int):
        """Save checkpoint for experiment recovery."""
        checkpoint_file = f"checkpoint_{self.name}_{episode}.json"
        
        # Save current results to checkpoint
        checkpoint_data = {
            'experiment_name': self.name,
            'episode': episode,
            'timestamp': datetime.now().isoformat(),
            'results': self.results
        }
        
        with open(checkpoint_file, 'w') as f:
            json.dump(checkpoint_data, f, indent=2)
    
    def save_results(self, filepath: str):
        """Save experiment results to file.
        
        Args:
            filepath (str): Path to save results file
        """
        results_file = {
            'experiment_name': self.name,
            'config': self.config,
            'results': self.results,
            'start_time': self.start_time,
            'end_time': self.end_time,
            'timestamp': datetime.now().isoformat()
        }
        
        # Create directory if it doesn't exist
        os.makedirs(os.path.dirname(filepath) if os.path.dirname(filepath) else '.', exist_ok=True)
        
        # Save as JSON
        with open(filepath, 'w') as f:
            json.dump(results_file, f, indent=2, default=str)
        
        print(f"Results saved to: {filepath}")
    
    def load_results(self, filepath: str):
        """Load experiment results from file.
        
        Args:
            filepath (str): Path to results file
        """
        with open(filepath, 'r') as f:
            results_file = json.load(f)
        
        self.name = results_file.get('experiment_name', self.name)
        self.config = results_file.get('config', self.config)
        self.results = results_file.get('results', {})
        self.start_time = results_file.get('start_time')
        self.end_time = results_file.get('end_time')
        
        print(f"Results loaded from: {filepath}")
    
    def analyze_results(self) -> Dict:
        """Analyze experiment results and generate statistics."""
        analysis = {
            'experiment_name': self.name,
            'analysis_timestamp': datetime.now().isoformat(),
            'summary': {},
            'detailed_analysis': {}
        }
        
        # Analyze parameter sweep results
        if 'parameter_sweep' in self.results:
            analysis['detailed_analysis']['parameter_sweep'] = self._analyze_parameter_sweep()
        
        # Analyze multi-environment results
        if 'multi_environment' in self.results:
            analysis['detailed_analysis']['multi_environment'] = self._analyze_multi_environment()
        
        # Analyze custom agent results
        if 'custom_agent' in self.results:
            analysis['detailed_analysis']['custom_agent'] = self._analyze_custom_agent()
        
        return analysis
    
    def _analyze_parameter_sweep(self) -> Dict:
        """Analyze parameter sweep results."""
        sweep_results = self.results['parameter_sweep']
        
        successful_experiments = [r for r in sweep_results if r['success']]
        
        if not successful_experiments:
            return {'error': 'No successful experiments'}
        
        analysis = {
            'total_experiments': len(sweep_results),
            'successful_experiments': len(successful_experiments),
            'success_rate': len(successful_experiments) / len(sweep_results) if sweep_results else 0,
            'best_parameters': None,
            'parameter_importance': {}
        }
        
        # Find best experiment based on fitness
        best_experiment = None
        best_fitness = float('-inf')
        
        for experiment in successful_experiments:
            # Calculate fitness from experiment results
            experiment_result = experiment['result']
            if experiment_result:
                # Example fitness calculation
                stats = experiment_result['statistics']
                fitness = stats.get('efficiency', 0) * stats.get('average_firing_rate', 0)
                
                if fitness > best_fitness:
                    best_fitness = fitness
                    best_experiment = experiment
        
        if best_experiment:
            analysis['best_parameters'] = best_experiment['parameters']
            analysis['best_fitness'] = best_fitness
        
        return analysis
    
    def _analyze_multi_environment(self) -> Dict:
        """Analyze multi-environment results."""
        env_results = self.results['multi_environment']
        
        successful_envs = [r for r in env_results if r['success']]
        
        if not successful_envs:
            return {'error': 'No successful environment experiments'}
        
        analysis = {
            'total_environments': len(env_results),
            'successful_environments': len(successful_envs),
            'success_rate': len(successful_envs) / len(env_results) if env_results else 0,
            'environment_performance': {}
        }
        
        for env_result in successful_envs:
            env_config = env_result['environment_config']
            env_stats = env_result['statistics']
            
            env_name = f"{env_config.get('width', 100)}x{env_config.get('height', 100)}"
            analysis['environment_performance'][env_name] = {
                'efficiency': env_stats['efficiency'],
                'total_actions': env_stats['total_actions'],
                'brain_stats': env_stats['brain_stats']
            }
        
        return analysis
    
    def _analyze_custom_agent(self) -> Dict:
        """Analyze custom agent results."""
        agent_results = self.results['custom_agent']
        
        successful_episodes = [r for r in agent_results if r['success']]
        
        if not successful_episodes:
            return {'error': 'No successful agent episodes'}
        
        analysis = {
            'total_episodes': len(agent_results),
            'successful_episodes': len(successful_episodes),
            'success_rate': len(successful_episodes) / len(agent_results) if agent_results else 0,
            'episode_stats': []
        }
        
        for episode_result in successful_episodes:
            episode_data = episode_result['result']
            if episode_data:
                analysis['episode_stats'].append({
                    'episode': episode_result['episode'],
                    'action_count': len(episode_data['actions']),
                    'final_state': episode_data['final_state']
                })
        
        return analysis
    
    def plot_results(self, output_dir: str = "."):
        """Generate plots from experiment results.
        
        Args:
            output_dir (str): Directory to save plots
        """
        os.makedirs(output_dir, exist_ok=True)
        
        # Create plots for different result types
        self._plot_parameter_sweep(output_dir)
        self._plot_multi_environment(output_dir)
        self._plot_custom_agent(output_dir)
    
    def _plot_parameter_sweep(self, output_dir: str):
        """Plot parameter sweep results."""
        if 'parameter_sweep' not in self.results:
            return
        
        sweep_results = self.results['parameter_sweep']
        successful_experiments = [r for r in sweep_results if r['success']]
        
        if not successful_experiments:
            return
        
        # Create parameter vs fitness plot
        import matplotlib.pyplot as plt
        
        plt.figure(figsize=(12, 8))
        
        # Example: Plot parameter 1 vs fitness
        param_keys = list(successful_experiments[0]['parameters'].keys())
        if len(param_keys) > 0:
            param_key = param_keys[0]
            
            param_values = []
            fitness_values = []
            
            for experiment in successful_experiments:
                params = experiment['parameters']
                result = experiment['result']
                
                if result:
                    param_values.append(params.get(param_key))
                    stats = result['statistics']
                    fitness = stats.get('efficiency', 0) * stats.get('average_firing_rate', 0)
                    fitness_values.append(fitness)
            
            if param_values and fitness_values:
                plt.scatter(param_values, fitness_values, alpha=0.7)
                plt.xlabel(param_key)
                plt.ylabel('Fitness')
                plt.title(f'{self.name}: {param_key} vs Fitness')
                plt.grid(True, alpha=0.3)
                
                plot_path = os.path.join(output_dir, f'{self.name}_parameter_sweep_{param_key}.png')
                plt.savefig(plot_path, dpi=150, bbox_inches='tight')
                plt.close()
    
    def _plot_multi_environment(self, output_dir: str):
        """Plot multi-environment results."""
        if 'multi_environment' not in self.results:
            return
        
        env_results = self.results['multi_environment']
        successful_envs = [r for r in env_results if r['success']]
        
        if not successful_envs:
            return
        
        import matplotlib.pyplot as plt
        
        plt.figure(figsize=(10, 6))
        
        env_names = []
        efficiencies = []
        
        for env_result in successful_envs:
            env_config = env_result['environment_config']
            env_stats = env_result['statistics']
            
            env_name = f"{env_config.get('width', 100)}x{env_config.get('height', 100)}"
            env_names.append(env_name)
            efficiencies.append(env_stats['efficiency'])
        
        plt.bar(env_names, efficiencies, alpha=0.7)
        plt.xlabel('Environment')
        plt.ylabel('Efficiency')
        plt.title(f'{self.name}: Environment Performance')
        plt.grid(True, alpha=0.3)
        plt.xticks(rotation=45)
        
        plot_path = os.path.join(output_dir, f'{self.name}_multi_environment.png')
        plt.savefig(plot_path, dpi=150, bbox_inches='tight')
        plt.close()
    
    def _plot_custom_agent(self, output_dir: str):
        """Plot custom agent results."""
        if 'custom_agent' not in self.results:
            return
        
        agent_results = self.results['custom_agent']
        successful_episodes = [r for r in agent_results if r['success']]
        
        if not successful_episodes:
            return
        
        import matplotlib.pyplot as plt
        
        plt.figure(figsize=(12, 5))
        
        # Create subplots
        fig, (ax1, ax2) = plt.subplots(1, 2, figsize=(15, 5))
        
        episodes = []
        action_counts = []
        
        for episode_result in successful_episodes:
            episodes.append(episode_result['episode'])
            episode_data = episode_result['result']
            if episode_data:
                action_counts.append(len(episode_data['actions']))
        
        # Plot action counts per episode
        ax1.plot(episodes, action_counts, 'b-', alpha=0.7, marker='o')
        ax1.set_xlabel('Episode')
        ax1.set_ylabel('Number of Actions')
        ax1.set_title(f'{self.name}: Actions per Episode')
        ax1.grid(True, alpha=0.3)
        
        # Plot success rate over time
        episode_success = []
        window_size = 10
        
        for i in range(0, len(episodes), window_size):
            window_episodes = episodes[i:i+window_size]
            window_success = [r for r in agent_results if r['success'] and r['episode'] in window_episodes]
            success_rate = len(window_success) / len(window_episodes) if window_episodes else 0
            episode_success.append((i, success_rate))
        
        ax2.plot([e[0] for e in episode_success], [e[1] for e in episode_success], 'g-', alpha=0.7, marker='s')
        ax2.set_xlabel('Episode')
        ax2.set_ylabel('Success Rate')
        ax2.set_title(f'{self.name}: Success Rate (moving average)')
        ax2.grid(True, alpha=0.3)
        
        plt.suptitle(f'{self.name} - Custom Agent Analysis')
        plt.tight_layout()
        
        plot_path = os.path.join(output_dir, f'{self.name}_custom_agent.png')
        plt.savefig(plot_path, dpi=150, bbox_inches='tight')
        plt.close()
    
    def run(self):
        """Run the experiment."""
        self.start_time = datetime.now()
        
        print(f"Starting experiment: {self.name}")
        print(f"Start time: {self.start_time}")
        
        # Run different experiment types based on configuration
        if 'parameter_grid' in self.config:
            self.run_parameter_sweep(self.config['parameter_grid'], self.config.get('brain_factory'))
        
        if 'environments' in self.config:
            self.run_multi_environment(self.config['environments'], self.config.get('brain_factory'))
        
        if 'agent_factory' in self.config:
            num_episodes = self.config.get('num_episodes', 100)
            self.run_custom_agent(self.config['agent_factory'], num_episodes)
        
        self.end_time = datetime.now()
        
        print(f"Experiment completed: {self.name}")
        print(f"End time: {self.end_time}")
        print(f"Duration: {self.end_time - self.start_time}")
        
        return self.analyze_results()

class CustomEnvironment:
    """Custom environment for NLM experiments.
    
    This class provides tools for creating custom environments with
    custom rules, dynamics, and interaction patterns.
    
    Attributes:
        name (str): Name of the custom environment
        world (SimpleWorld): Underlying NLM world
        rules (list): Environment rules
        dynamics (dict): Environment dynamics parameters
    """
    
    def __init__(self, name: str, world_config: Optional[Dict] = None):
        """Initialize a custom environment.
        
        Args:
            name (str): Name of the custom environment
            world_config (dict, optional): World configuration
        """
        self.name = name
        self.world = SimpleWorld()
        self.rules = []
        self.dynamics = {}
        
        if world_config:
            self.configure(world_config)
        else:
            # Default configuration
            self.configure({
                'width': 100,
                'height': 100,
                'vision_width': 10,
                'vision_height': 10,
                'max_energy': 100.0,
                'energy_decay_rate': 0.01
            })
    
    def configure(self, config: Dict):
        """Configure the custom environment.
        
        Args:
            config (dict): Environment configuration
        """
        self.world.configure(
            config.get('width', 100),
            config.get('height', 100),
            config.get('vision_width', 10),
            config.get('vision_height', 10)
        )
        
        self.world.setMaxEnergy(config.get('max_energy', 100.0))
        self.world.setEnergyDecayRate(config.get('energy_decay_rate', 0.01))
        
        self.dynamics.update(config.get('dynamics', {}))
        
        print(f"Custom environment '{self.name}' configured")
    
    def add_rule(self, rule: Callable, description: str = ""):
        """Add an environment rule.
        
        Args:
            rule (callable): Function that takes (world, agent) and returns bool
            description (str): Description of the rule
        """
        self.rules.append({
            'function': rule,
            'description': description
        })
    
    def add_dynamic(self, name: str, params: Dict):
        """Add dynamic behavior to environment.
        
        Args:
            name (str): Name of dynamic behavior
            params (dict): Parameters for dynamic behavior
        """
        self.dynamics[name] = params
    
    def check_rules(self, world, agent):
        """Check all environment rules.
        
        Args:
            world (SimpleWorld): The world
            agent: The agent
            
        Returns:
            list: List of rule violations
        """
        violations = []
        
        for rule in self.rules:
            try:
                if not rule['function'](world, agent):
                    violations.append(rule['description'])
            except Exception as e:
                violations.append(f"Rule error: {e}")
        
        return violations
    
    def apply_dynamics(self, world, agent, timestep: float):
        """Apply environment dynamics.
        
        Args:
            world (SimpleWorld): The world
            agent: The agent
            timestep (float): Time step
        """
        for name, params in self.dynamics.items():
            if name == 'resource_decay':
                self._apply_resource_decay(world, agent, params, timestep)
            elif name == 'hazard_generation':
                self._apply_hazard_generation(world, agent, params, timestep)
            elif name == 'temperature_effect':
                self._apply_temperature_effect(world, agent, params, timestep)
            elif name == 'custom_dynamics':
                self._apply_custom_dynamics(world, agent, params, timestep)
    
    def _apply_resource_decay(self, world, agent, params: Dict, timestep: float):
        """Apply resource decay dynamics."""
        # Example: Gradually decrease resources
        decay_rate = params.get('decay_rate', 0.01)
        # Implementation depends on your resource system
    
    def _apply_hazard_generation(self, world, agent, params: Dict, timestep: float):
        """Apply hazard generation dynamics."""
        # Example: Generate hazards randomly
        hazard_rate = params.get('hazard_rate', 0.01)
        # Implementation depends on your hazard system
    
    def _apply_temperature_effect(self, world, agent, params: Dict, timestep: float):
        """Apply temperature effect dynamics."""
        # Example: Temperature affects agent behavior
        base_temp = params.get('base_temp', 20.0)
        # Implementation depends on your temperature system
    
    def _apply_custom_dynamics(self, world, agent, params: Dict, timestep: float):
        """Apply custom dynamics."""
        # User-defined dynamics function
        if 'function' in params:
            params['function'](world, agent, timestep)
    
    def step(self, agent, action, timestep: float = 0.01):
        """Step the environment with given action.
        
        Args:
            agent: The agent
            action (Action): Action to take
            timestep (float): Time step
            
        Returns:
            dict: Environment step result
        """
        # Apply action
        self.world.applyMotorCommand(action, self.world.getSimulationTime())
        
        # Check rules
        violations = self.check_rules(self.world, agent)
        
        # Apply dynamics
        self.apply_dynamics(self.world, agent, timestep)
        
        # Update world
        self.world.update(timestep)
        
        # Return step result
        return {
            'success': True,
            'violations': violations,
            'world_state': self.world.getAgentBody(),
            'percept': self.world.getSensoryPercept()
        }
    
    def reset(self):
        """Reset the environment."""
        self.world.reset()
        
        # Reset dynamic elements
        for name, params in self.dynamics.items():
            if name == 'resource_decay':
                # Reset resource system
                pass
            elif name == 'hazard_generation':
                # Reset hazard system
                pass
            elif name == 'temperature_effect':
                # Reset temperature system
                pass
            elif name == 'custom_dynamics':
                if 'reset_function' in params:
                    params['reset_function']()
    
    def get_world(self):
        """Get the underlying world."""
        return self.world
    
    def __repr__(self):
        return f"CustomEnvironment(name='{self.name}', width={self.world.getWidth()}, height={self.world.getHeight()})"