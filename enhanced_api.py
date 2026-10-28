# Improved Python API for NLM

## High-Level Convenience Classes

This module provides enhanced Python interfaces for common NLM use cases,
making the system more accessible while maintaining full control for advanced users.

```python
"""Enhanced NLM Python API with convenience classes and utilities."""

import time
import numpy as np
from typing import Callable, Optional, List, Dict, Any
import pynlm

class NLMEnvironment:
    """
    High-level interface for common NLM use cases with automatic setup
    and management of brain, world, and agent components.
    """
    
    def __init__(self, config_type: str = 'standard'):
        """
        Initialize NLM environment with preset configuration.
        
        Args:
            config_type: Preset configuration ('standard', 'exploration', 
                'exploitation', 'balanced')
        """
        self.config = pynlm.createDefaultConfig()
        self._apply_preset_config(config_type)
        
        self.brain = pynlm.createBrain(self.config)
        self.brain.initialize()
        
        self.world = pynlm.createSimpleWorld()
        self._configure_world_for_agent()
        
        self.agent = pynlm.createAgentBrain(self.brain)
        self.agent.initialize(self.world)
        
        # Enable commonly used subsystems
        self.enable_feature('reward_modulation', True)
        self.enable_feature('curiosity', True)
        self.enable_feature('development', True)
        
        # Statistics tracking
        self.stats = {
            'steps': 0,
            'total_spikes': 0,
            'total_neurons': 0,
            'episode_reward': 0.0,
            'max_firing_rate': 0.0,
            'action_distribution': {},
            'novelty_history': [],
            'curiosity_history': []
        }
        
        # Episode tracking
        self.episode_start_time = 0.0
        self.episode_actions: List[Any] = []
        self.episode_percepts: List[Any] = []
        
    def _apply_preset_config(self, config_type: str):
        """Apply preset configuration based on type."""
        presets = {
            'standard': {
                'brain.neuron_count': 1000,
                'brain.synapse_density': 0.1,
                'neuromod.curiosity.enable': True,
                'neuromod.novelty.enable': True,
                'neuromod.dopamine.scale': 1.0,
            },
            'exploration': {
                'brain.neuron_count': 2000,
                'brain.synapse_density': 0.15,
                'neuromod.curiosity.enable': True,
                'neuromod.curiosity.level': 0.8,
                'neuromod.novelty.enable': True,
            },
            'exploitation': {
                'brain.neuron_count': 500,
                'brain.synapse_density': 0.08,
                'neuromod.curiosity.enable': False,
                'neuromod.novelty.enable': False,
                'plasticity.stdp.enable': True,
                'plasticity.hebbian.enable': True,
            },
            'balanced': {
                'brain.neuron_count': 1500,
                'brain.synapse_density': 0.12,
                'neuromod.curiosity.enable': True,
                'neuromod.novelty.enable': True,
                'plasticity.structural.enable': True,
                'development.enabled': True,
            }
        }
        
        if config_type in presets:
            for key, value in presets[config_type].items():
                self.config.set(key, value)
    
    def _configure_world_for_agent(self):
        """Configure world settings optimal for agent interaction."""
        self.world.configure(
            width=20,
            height=20,
            visionWidth=8,
            visionHeight=8
        )
        self.world.setAgentStart(10.0, 10.0)
        self.world.setRandomSeed(42)
    
    def enable_feature(self, feature: str, enable: bool = True):
        """Enable or disable specific brain subsystems."""
        feature_map = {
            'reward_modulation': self.agent.enableRewardModulation,
            'structural_plasticity': self.agent.enableStructuralPlasticity,
            'development': self.agent.enableDevelopment,
            'curiosity': self.agent.enableCuriosity,
            'novelty': self.agent.enableNovelty,
        }
        
        if feature in feature_map:
            feature_map[feature](enable)
        else:
            raise ValueError(f"Unknown feature: {feature}")
    
    def run_episode(self, num_steps: int, reward_fn: Optional[Callable] = None,
                   render_callback: Optional[Callable] = None) -> Dict[str, Any]:
        """
        Run a complete simulation episode with automatic state management.
        
        Args:
            num_steps: Number of simulation steps to run
            reward_fn: Optional function to compute reward from world state
            render_callback: Optional function for visualization
            
        Returns:
            Dictionary containing episode statistics
        """
        # Reset episode state
        self.world.reset()
        self.agent.reset()
        self.episode_start_time = self.world.getSimulationTime()
        self.episode_actions.clear()
        self.episode_percepts.clear()
        
        # Initialize statistics
        episode_reward = 0.0
        max_firing_rate = 0.0
        action_counts: Dict[Any, int] = {}
        
        for step in range(num_steps):
            # Update world
            self.world.update(0.1)
            
            # Get perception and store
            percept = self.world.getSensoryPercept()
            self.episode_percepts.append(percept)
            
            # Process sensory input
            self.agent.processSensoryInput(percept)
            
            # Brain step with automatic time tracking
            self.brain.step(step, self.world.getSimulationTime())
            
            # Decode action and execute
            action = self.agent.decodeMotorCommand()
            self.episode_actions.append(action)
            
            # Update action statistics
            action_key = str(action)
            action_counts[action_key] = action_counts.get(action_key, 0) + 1
            
            # Apply action to world
            self.world.applyMotorCommand(action, self.world.getSimulationTime())
            
            # Compute reward if callback provided
            if reward_fn:
                reward = reward_fn(self.world, action, percept)
                self.agent.applyRewardModulation(reward, 0.0)
                episode_reward += reward
            else:
                # Default reward based on novelty
                novelty = self.agent.getNoveltyLevel()
                reward = novelty * 0.1
                self.agent.applyRewardModulation(reward, 0.0)
                episode_reward += reward
            
            # Update development
            self.agent.updateDevelopment(0.1)
            
            # Record statistics
            firing_rate = self.brain.getAverageFiringRate()
            max_firing_rate = max(max_firing_rate, firing_rate)
            
            # Store history
            self.stats['novelty_history'].append(self.agent.getNoveltyLevel())
            self.stats['curiosity_history'].append(self.agent.getCuriosityLevel())
            
            # Optional rendering
            if render_callback:
                render_callback(self, step)
            
            # Print progress for long episodes
            if step % 100 == 0:
                print(f"Step {step}/{num_steps}, "
                      f"Firing: {firing_rate:.2f}, "
                      f"Curiosity: {self.agent.getCuriosityLevel():.2f}, "
                      f"Novelty: {self.agent.getNoveltyLevel():.2f}")
        
        # Update final statistics
        self.stats['steps'] = num_steps
        self.stats['total_spikes'] = self.brain.getTotalSpikeCount()
        self.stats['total_neurons'] = self.brain.getTotalNeuronCount()
        self.stats['episode_reward'] = episode_reward
        self.stats['max_firing_rate'] = max_firing_rate
        self.stats['action_distribution'] = action_counts
        
        return self._get_episode_summary()
    
    def _get_episode_summary(self) -> Dict[str, Any]:
        """Generate comprehensive episode summary."""
        return {
            'total_reward': self.stats['episode_reward'],
            'avg_firing_rate': (self.stats['total_spikes'] / 
                              max(1, self.stats['total_neurons'] * self.stats['steps'])),
            'max_firing_rate': self.stats['max_firing_rate'],
            'action_distribution': self.stats['action_distribution'],
            'final_curiosity': self.agent.getCuriosityLevel(),
            'final_novelty': self.agent.getNoveltyLevel(),
            'final_development': self.agent.getDevelopmentalStage(),
            'episode_length': self.stats['steps'],
            'total_spikes': self.stats['total_spikes'],
            'total_neurons': self.stats['total_neurons'],
        }
    
    def save_brain(self, filepath: str):
        """Save brain state for later loading."""
        self.brain.save(filepath)
    
    def load_brain(self, filepath: str):
        """Load brain state from file."""
        self.brain.load(filepath)
    
    def get_brain_stats(self) -> Dict[str, Any]:
        """Get comprehensive brain statistics."""
        return {
            'total_neurons': self.brain.getTotalNeuronCount(),
            'total_synapses': self.brain.getTotalSynapseCount(),
            'firing_neurons': self.brain.getFiringNeuronCount(),
            'average_firing_rate': self.brain.getAverageFiringRate(),
            'excitatory_inhibition_ratio': 
                self.brain.getExcitationInhibitionRatio(),
            'total_spikes': self.brain.getTotalSpikeCount(),
            'developmental_stage': self.brain.getDevelopmentalStage(),
            'region_count': self.brain.getRegionCount(),
        }
    
    def train(self, episodes: int, validation_interval: int = 100,
              save_interval: int = 1000, checkpoint_dir: str = 'checkpoints'):
        """
        Train agent with automatic checkpointing and validation.
        
        Args:
            episodes: Number of training episodes
            validation_interval: Validate performance every N episodes
            save_interval: Save checkpoint every N episodes  
            checkpoint_dir: Directory for checkpoints
        """
        import os
        os.makedirs(checkpoint_dir, exist_ok=True)
        
        best_reward = float('-inf')
        best_episode = 0
        
        for episode in range(episodes):
            # Run episode
            summary = self.run_episode(1000)
            
            # Print progress
            print(f"Episode {episode + 1}/{episodes}: "
                  f"Reward: {summary['total_reward']:.2f}, "
                  f"Firing: {summary['avg_firing_rate']:.4f}")
            
            # Save best model
            if summary['total_reward'] > best_reward:
                best_reward = summary['total_reward']
                best_episode = episode + 1
                self.save_brain(f"{checkpoint_dir}/best_brain_{best_episode}.bin")
                print(f"  New best reward! Saved model.")
            
            # Save periodic checkpoint
            if (episode + 1) % save_interval == 0:
                self.save_brain(f"{checkpoint_dir}/checkpoint_{episode + 1}.bin")
                print(f"  Saved checkpoint.")
            
            # Validate periodically
            if (episode + 1) % validation_interval == 0:
                self._validate_performance(episode + 1)
        
        print(f"Training complete. Best reward: {best_reward:.2f} "
              f"in episode {best_episode}")
        
        return {
            'best_reward': best_reward,
            'best_episode': best_episode,
            'final_stats': self._get_episode_summary()
        }
    
    def _validate_performance(self, episode: int):
        """Validate current performance against baseline."""
        # Run quick validation episode with reduced settings
        temp_config = pynlm.createDefaultConfig()
        temp_brain = pynlm.createBrain(temp_config)
        temp_brain.initialize()
        
        temp_agent = pynlm.createAgentBrain(temp_brain)
        temp_world = pynlm.createSimpleWorld()
        temp_world.configure(width=10, height=10, visionWidth=4, visionHeight=4)
        temp_world.reset()
        temp_agent.initialize(temp_world)
        
        # Run validation
        for step in range(200):
            temp_world.update(0.1)
            percept = temp_world.getSensoryPercept()
            temp_agent.processSensoryInput(percept)
            temp_brain.step(step)
            action = temp_agent.decodeMotorCommand()
            temp_world.applyMotorCommand(action, temp_world.getSimulationTime())
        
        validation_reward = 0.0
        print(f"  Validation episode {episode}: "
              f"Validation completed successfully")
class NLMDataProcessor:
    """
    Utilities for processing NLM data and preparing inputs/outputs
    for machine learning applications.
    """
    
    @staticmethod
    def extract_features(percept: Any, action: Any) -> np.ndarray:
        """
        Extract features from percept and action for ML training.
        
        Args:
            percept: Sensory percept from world
            action: Motor command from agent
            
        Returns:
            Feature vector for training
        """
        # Extract vision data
        vision = percept.getVision()
        vision_flat = np.array(vision).flatten()
        
        # Extract other sensory data
        touch = percept.getTouch()
        internal = percept.getInternal()
        proprioception = percept.getProprioception()
        
        # Convert action to categorical encoding
        action_vec = NLMDataProcessor._encode_action(action)
        
        # Combine all features
        features = np.concatenate([
            vision_flat,
            np.array(touch),
            np.array(internal),
            np.array(proprioception),
            action_vec
        ])
        
        return features
    
    @staticmethod
    def _encode_action(action: Any) -> np.ndarray:
        """One-hot encode action for feature extraction."""
        action_types = [
            pynlm.MotorCommand.MoveForward,
            pynlm.MotorCommand.MoveBackward,
            pynlm.MotorCommand.TurnLeft,
            pynlm.MotorCommand.TurnRight,
            pynlm.MotorCommand.Look,
            pynlm.MotorCommand.Interact,
            pynlm.MotorCommand.Eat,
            pynlm.MotorCommand.Drink,
            pynlm.MotorCommand.Rest,
            pynlm.MotorCommand.Wait,
            pynlm.MotorCommand.TurnUp,
            pynlm.MotorCommand.TurnDown,
            pynlm.MotorCommand.Custom
        ]
        
        encoding = np.zeros(len(action_types))
        for i, action_type in enumerate(action_types):
            if action == action_type:
                encoding[i] = 1.0
                break
        
        return encoding
    
    @staticmethod
    def prepare_training_data(experiences: List[Dict]) -> np.ndarray:
        """
        Prepare experiences for machine learning training.
        
        Args:
            experiences: List of experience dictionaries with 'percept', 'action', 
                'reward', 'next_percept'
                
        Returns:
            Training data array
        """
        X = []
        y = []
        
        for exp in experiences:
            # Extract features from current state
            features = NLMDataProcessor.extract_features(
                exp['percept'], exp['action'])
            
            # Create target (next state features)
            target_features = NLMDataProcessor.extract_features(
                exp['next_percept'], exp['next_action'])
            
            # Calculate reward change as target
            reward_change = exp['reward'] - exp.get('prev_reward', 0)
            
            X.append(features)
            y.append([reward_change, *target_features])
        
        return np.array(X), np.array(y)
    
    @staticmethod
    def compute_reward_shaping(reward: float, novelty: float, 
                              curiosity: float, prediction_error: float) -> float:
        """
        Apply reward shaping using multiple signals for better learning.
        
        Args:
            reward: Original reward from environment
            novelty: Novelty detection signal
            curiosity: Curiosity level
            prediction_error: Prediction error signal
            
        Returns:
            Shaped reward
        """
        # Combine signals with tunable weights
        shaped_reward = (reward * 0.5 + 
                        novelty * 0.2 + 
                        curiosity * 0.15 + 
                        prediction_error * 0.15)
        
        # Ensure non-negative
        return max(0.0, shaped_reward)
class NLMVisualizer:
    """
    Enhanced visualization tools for NLM simulations.
    """
    
    @staticmethod
    def plot_brain_activity(brain_stats: Dict, output_file: str = None):
        """
        Generate comprehensive brain activity plot.
        
        Args:
            brain_stats: Statistics from brain.get_stats()
            output_file: Optional file to save plot
        """
        try:
            import matplotlib.pyplot as plt
            import seaborn as sns
            
            fig, axes = plt.subplots(2, 3, figsize=(15, 10))
            fig.suptitle('NLM Brain Activity Analysis', fontsize=16)
            
            # Plot 1: Neuron counts
            axes[0, 0].pie([
                brain_stats['firing_neurons'],
                brain_stats['total_neurons'] - brain_stats['firing_neurons']
            ], labels=['Firing', 'Inactive'], autopct='%1.1f%%')
            axes[0, 0].set_title('Neuron Distribution')
            
            # Plot 2: Firing rate over time (if available)
            if 'firing_history' in brain_stats:
                axes[0, 1].plot(brain_stats['firing_history'])
                axes[0, 1].set_title('Firing Rate Over Time')
                axes[0, 1].set_ylabel('Firing Rate')
            
            # Plot 3: Synapse distribution
            axes[0, 2].hist([brain_stats['total_synapses']] * 10, bins=20)
            axes[0, 2].set_title('Synapse Count Distribution')
            axes[0, 2].set_xlabel('Synapse Count')
            axes[0, 2].set_ylabel('Frequency')
            
            # Plot 4: E/I ratio
            axes[1, 0].bar(['Excitatory/Inhibitory'], 
                           [brain_stats['excitatory_inhibition_ratio']])
            axes[1, 0].set_title('Excitatory/Inhibitory Ratio')
            axes[1, 0].set_ylabel('Ratio')
            axes[1, 0].set_ylim(0, 10)
            
            # Plot 5: Spike count histogram
            if 'spike_history' in brain_stats:
                axes[1, 1].hist(brain_stats['spike_history'], bins=20, alpha=0.7)
                axes[1, 1].set_title('Spike Count Distribution')
                axes[1, 1].set_xlabel('Spike Count')
                axes[1, 1].set_ylabel('Frequency')
            
            # Plot 6: Developmental stage
            stage_names = ['Initial', 'Critical', 'Maturation', 'Adult', 'Aging']
            stage_values = [0, 0, 0, 0, 0]
            current_stage = brain_stats['developmental_stage']
            stage_idx = min(int(current_stage), len(stage_names) - 1)
            stage_values[stage_idx] = 1
            
            axes[1, 2].bar(stage_names, stage_values)
            axes[1, 2].set_title('Developmental Stage')
            axes[1, 2].set_ylabel('Active')
            axes[1, 2].set_ylim(0, 1)
            
            plt.tight_layout()
            
            if output_file:
                plt.savefig(output_file, dpi=300, bbox_inches='tight')
            
            plt.show()
            plt.close()
            
        except ImportError:
            print("Matplotlib not available. Install with: pip install matplotlib seaborn")
    
    @staticmethod
    def create_3d_neural_view(brain, max_neurons: int = 1000, 
                             output_file: str = None):
        """
        Create 3D visualization of neural network structure.
        
        Args:
            brain: NLM brain object
            max_neurons: Maximum number of neurons to display
            output_file: Optional file to save visualization
        """
        try:
            import matplotlib.pyplot as plt
            from mpl_toolkits.mplot3d import Axes3D
            
            fig = plt.figure(figsize=(12, 10))
            ax = fig.add_subplot(111, projection='3d')
            
            # Get neuron populations
            regions = brain.getRegions()
            neuron_count = 0
            
            for region in regions:
                populations = region.getPopulations()
                
                for pop in populations:
                    neurons = pop.getNeurons()
                    neuron_type = pop.getNeuronType()
                    
                    # Generate random position in 3D space
                    x = np.random.uniform(-1, 1, len(neurons))
                    y = np.random.uniform(-1, 1, len(neurons))
                    z = np.random.uniform(-1, 1, len(neurons))
                    
                    # Color by neuron type
                    if neuron_type == pynlm.NeuronType.Excitatory:
                        color = 'red'
                    elif neuron_type == pynlm.NeuronType.Inhibitory:
                        color = 'blue'
                    elif neuron_type == pynlm.NeuronType.Sensory:
                        color = 'green'
                    elif neuron_type == pynlm.NeuronType.Motor:
                        color = 'orange'
                    else:
                        color = 'gray'
                    
                    # Plot neurons
                    ax.scatter(x, y, z, c=color, alpha=0.6, s=10)
                    
                    neuron_count += len(neurons)
                    if neuron_count >= max_neurons:
                        break
                
                if neuron_count >= max_neurons:
                    break
            
            ax.set_title('3D Neural Network View')
            ax.set_xlabel('X')
            ax.set_ylabel('Y')
            ax.set_zlabel('Z')
            ax.set_xlim(-1, 1)
            ax.set_ylim(-1, 1)
            ax.set_zlim(-1, 1)
            
            if output_file:
                plt.savefig(output_file, dpi=300, bbox_inches='tight')
            
            plt.show()
            plt.close()
            
        except ImportError as e:
            print(f"3D visualization requires: {e}")
class NLMExperiment:
    """
    Experiment framework for systematic NLM testing and parameter tuning.
    """
    
    def __init__(self, config: Any = None):
        """
        Initialize experiment framework.
        
        Args:
            config: Optional initial configuration
        """
        self.config = config if config is not None else pynlm.createDefaultConfig()
        self.results = []
        self.baseline_performance = {}
    
    def run_parameter_sweep(self, param_space: Dict[str, List],
                           num_episodes: int = 100,
                           metric: str = 'total_reward') -> Dict[str, Any]:
        """
        Run parameter sweep over specified parameter space.
        
        Args:
            param_space: Dictionary mapping parameter names to lists of values
            num_episodes: Number of episodes per parameter set
            metric: Performance metric to optimize
            
        Returns:
            Dictionary containing results for each parameter combination
        """
        import itertools
        
        param_names = list(param_space.keys())
        param_values = list(param_space.values())
        
        results = []
        total_combinations = 1
        for values in param_values:
            total_combinations *= len(values)
        
        print(f"Running parameter sweep over {total_combinations} combinations")
        
        for i, combination in enumerate(itertools.product(*param_values)):
            param_dict = dict(zip(param_names, combination))
            
            print(f"Running combination {i + 1}/{total_combinations}: {param_dict}")
            
            # Apply parameters to config
            temp_config = pynlm.createDefaultConfig()
            for key, value in param_dict.items():
                temp_config.set(key, value)
            
            # Run experiment
            env = NLMEnvironment(temp_config)
            episode = 1
            
            while episode <= num_episodes:
                summary = env.run_episode(1000)
                
                # Collect results
                result = {
                    'parameters': param_dict.copy(),
                    'episode': episode,
                    'reward': summary['total_reward'],
                    'firing_rate': summary['avg_firing_rate'],
                    'max_firing_rate': summary['max_firing_rate'],
                    'final_curiosity': summary['final_curiosity'],
                    'final_novelty': summary['final_novelty'],
                    'final_development': summary['final_development'],
                }
                
                results.append(result)
                
                # Early stopping if performance plateaus
                if episode > 10:
                    recent_rewards = [r['reward'] for r in results[-10:]]
                    if len(set(recent_rewards)) < 3:  # Little variation
                        break
                
                episode += 1
        
        # Find best parameters
        if metric in results[0]:
            best_result = max(results, key=lambda x: x[metric])
            
            return {
                'results': results,
                'best_parameters': best_result['parameters'],
                'best_value': best_result[metric],
                'best_result': best_result,
                'total_combinations': total_combinations,
                'successful_runs': len(results),
            }
        else:
            raise ValueError(f"Metric {metric} not found in results")
    
    def run_ablation_study(self, feature_set: List[str],
                          baseline_config: Any = None) -> Dict[str, Any]:
        """
        Run ablation study removing different features from the system.
        
        Args:
            feature_set: List of features to test (e.g., ['curiosity', 'novelty', 
                'development', 'reward_modulation'])
            baseline_config: Optional baseline configuration
                
        Returns:
            Dictionary containing results for each ablation
        """
        if baseline_config is None:
            baseline_config = self.config
        
        results = []
        
        # Test baseline (all features)
        print("Running baseline (all features)...")
        env = NLMEnvironment(baseline_config)
        baseline_summary = env.run_episode(1000)
        
        baseline_result = {
            'features': feature_set.copy(),
            'removed_features': [],
            'performance': baseline_summary['total_reward'],
            'firing_rate': baseline_summary['avg_firing_rate'],
        }
        results.append(baseline_result)
        
        # Test each ablation
        for feature in feature_set:
            print(f"Running ablation (removing {feature})...")
            
            # Create config without the feature
            temp_config = baseline_config
            ablation_env = NLMEnvironment(temp_config)
            
            # Disable the feature
            if feature == 'curiosity':
                ablation_env.enable_feature('curiosity', False)
            elif feature == 'novelty':
                ablation_env.enable_feature('reward_modulation', False)
            elif feature == 'development':
                ablation_env.enable_feature('development', False)
            elif feature == 'reward_modulation':
                ablation_env.enable_feature('reward_modulation', False)
            elif feature == 'structural_plasticity':
                ablation_env.enable_feature('structural_plasticity', False)
            
            # Run ablation test
            ablation_summary = ablation_env.run_episode(1000)
            
            ablation_result = {
                'features': feature_set.copy(),
                'removed_features': [feature],
                'performance': ablation_summary['total_reward'],
                'firing_rate': ablation_summary['avg_firing_rate'],
                'curiosity_effect': (ablation_summary.get('final_curiosity', 0) -
                                   baseline_summary.get('final_curiosity', 0)),
                'novelty_effect': (ablation_summary.get('final_novelty', 0) -
                                 baseline_summary.get('final_novelty', 0)),
            }
            
            results.append(ablation_result)
        
        return {
            'results': results,
            'baseline_performance': baseline_summary['total_reward'],
            'baseline_firing_rate': baseline_summary['avg_firing_rate'],
        }
    
    def save_results(self, filepath: str):
        """Save experiment results to file."""
        import json
        
        # Convert non-serializable objects
        serializable_results = []
        for result in self.results:
            serializable_result = {}
            for key, value in result.items():
                if hasattr(value, '__dict__'):
                    serializable_result[key] = value.__dict__
                else:
                    serializable_result[key] = value
            serializable_results.append(serializable_result)
        
        with open(filepath, 'w') as f:
            json.dump({
                'results': serializable_results,
                'baseline_performance': self.baseline_performance
            }, f, indent=2)
    
    def load_results(self, filepath: str):
        """Load experiment results from file."""
        import json
        
        with open(filepath, 'r') as f:
            data = json.load(f)
        
        self.results = data['results']
        self.baseline_performance = data['baseline_performance']
# Convenience factory functions for common use cases

def create_explorer_agent(config_type: str = 'exploration') -> NLMEnvironment:
    """Create an agent optimized for exploration and discovery."""
    return NLMEnvironment(config_type)

def create_exploiter_agent(config_type: str = 'exploitation') -> NLMEnvironment:
    """Create an agent optimized for exploitation and efficiency."""
    return NLMEnvironment(config_type)

def create_balanced_agent(config_type: str = 'balanced') -> NLMEnvironment:
    """Create a balanced agent with mixed capabilities."""
    return NLMEnvironment(config_type)

def create_minimal_brain(config: Any = None) -> pynlm.Brain:
    """Create a minimal brain with just essential components."""
    if config is None:
        config = pynlm.createDefaultConfig()
    
    # Set minimal configuration
    config.set('brain.neuron_count', 100)
    config.set('brain.synapse_density', 0.05)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    return brain

def create_training_environment(pretrained_brain: str = None) -> NLMEnvironment:
    """Create environment for training with optional pretrained brain."""
    env = NLMEnvironment('balanced')
    
    if pretrained_brain:
        env.load_brain(pretrained_brain)
    
    return env
# Export all public classes and functions
__all__ = [
    'NLMEnvironment',
    'NLMDataProcessor', 
    'NLMVisualizer',
    'NLMExperiment',
    'create_explorer_agent',
    'create_exploiter_agent',
    'create_balanced_agent',
    'create_minimal_brain',
    'create_training_environment'
]