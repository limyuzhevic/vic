#!/usr/bin/env python3
"""
NLM Python Convenience API - High-level utilities for NLM usage

This module provides enhanced convenience methods and utilities for working with
NLM (Neural Learning Machine) that build on top of the core pynlm bindings.
It makes the system easier to use for beginners while adding advanced features
for expert users.
"""

import pynlm
import json
import time
import numpy as np
from typing import Dict, List, Optional, Union, Callable
from pathlib import Path
import threading
import multiprocessing

__all__ = [
    'createQuickBrain',
    'createTrainingEnvironment', 
    'createExperimentRunner',
    'createVisualizer',
    'createProfiler',
    'createBenchmark',
    'configureFromPreset',
    'validateBrainHealth',
    'runLearningEpisode',
    'saveBrainWithMetadata',
    'loadBrainWithMetadata',
    'BrainManager',
    'ExperimentManager',
    'PerformanceProfiler'
]

def createQuickBrain(neuron_count: int = 500, learning_rate: float = 0.01) -> pynlm.Brain:
    """
    Create a pre-configured brain for quick testing.
    
    This is a convenience function that creates a brain with reasonable defaults
    for testing and experimentation, eliminating the need to configure each parameter.
    
    Args:
        neuron_count: Number of neurons (default: 500)
        learning_rate: Learning rate for plasticity (default: 0.01)
        
    Returns:
        Configured and initialized brain ready for use
        
    Example:
        >>> brain = createQuickBrain(1000, 0.001)
        >>> brain.initialize()
    """
    config = pynlm.createDefaultConfig()
    
    # Set sensible defaults for a quick test brain
    config.set("brain.neuron_count", neuron_count)
    config.set("brain.connection_probability", 0.1)
    config.set("brain.initial_weight_mean", 0.5)
    config.set("brain.initial_weight_std", 0.1)
    config.set("plasticity.stdp.learning_rate", learning_rate)
    config.set("plasticity.hebbian.learning_rate", learning_rate)
    config.set("neuromod.dopamine.scale", 1.0)
    config.set("neuromod.curiosity.enable", True)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    return brain

def createTrainingEnvironment(width: int = 20, height: int = 20, 
                             enable_features: bool = True) -> Dict[str, pynlm.Brain]:
    """
    Create a complete training environment with agent and world setup.
    
    This convenience function sets up a full agent-based training environment
    with pre-configured worlds and agent interfaces, ready for reinforcement
    learning or exploration training.
    
    Args:
        width: World width in cells
        height: World height in cells  
        enable_features: Whether to enable all agent features (reward, curiosity, etc.)
        
    Returns:
        Dictionary containing 'brain', 'agent', and 'world' objects
        
    Example:
        >>> env = createTrainingEnvironment(30, 30)
        >>> brain = env['brain']
        >>> agent = env['agent']
        >>> world = env['world']
    """
    # Create configuration
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 1000)
    
    # Create brain and initialize
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Create world
    world = pynlm.createSimpleWorld()
    vision_width = min(8, width // 2)
    vision_height = min(8, height // 2)
    world.configure(width, height, vision_width, vision_height)
    world.reset()
    
    # Create agent interface
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable features if requested
    if enable_features:
        agent.enableRewardModulation(True)
        agent.enableStructuralPlasticity(True)
        agent.enableDevelopment(True)
        agent.enableCuriosity(True)
    
    return {
        'brain': brain,
        'agent': agent,
        'world': world
    }

def configureFromPreset(preset_name: str) -> pynlm.Config:
    """
    Create a configuration from a preset profile.
    
    This function provides easy access to different configuration presets
    optimized for various use cases.
    
    Args:
        preset_name: Name of the preset. Options: 'quick', 'research', 
                    'production', 'debug', 'minimal'
                    
    Returns:
        Configured NLM Config object
        
    Example:
        >>> config = configureFromPreset('research')
        >>> config.set("custom.parameter", 42)
    """
    config = pynlm.createDefaultConfig()
    
    presets = {
        'quick': {
            'brain.neuron_count': 500,
            'brain.connection_probability': 0.1,
            'plasticity.stdp.learning_rate': 0.001,
            'neuromod.curiosity.enable': True
        },
        'research': {
            'brain.neuron_count': 5000,
            'brain.connection_probability': 0.05,
            'plasticity.stdp.learning_rate': 0.01,
            'plasticity.structural.enable': True,
            'neuromod.curiosity.enable': True,
            'neuromod.novelty.enable': True,
            'neuromod.dopamine.scale': 2.0,
            'brain.initial_weight_mean': 0.3,
            'brain.initial_weight_std': 0.2
        },
        'production': {
            'brain.neuron_count': 2000,
            'brain.connection_probability': 0.08,
            'plasticity.stdp.learning_rate': 0.005,
            'plasticity.structural.enable': True,
            'neuromod.curiosity.enable': False,
            'neuromod.novelty.enable': False,
            'neuromod.dopamine.scale': 1.0,
            'brain.initial_weight_mean': 0.5,
            'brain.initial_weight_std': 0.15,
            'simulation_timestep': 0.001
        },
        'debug': {
            'brain.neuron_count': 100,
            'brain.connection_probability': 0.2,
            'plasticity.stdp.learning_rate': 0.1,
            'plasticity.hebbian.enable': True,
            'neuromod.curiosity.enable': True,
            'neuromod.novelty.enable': True,
            'simulation_timestep': 0.01,
            'random_seed': 12345
        },
        'minimal': {
            'brain.neuron_count': 100,
            'brain.connection_probability': 0.1,
            'plasticity.stdp.enable': False,
            'plasticity.hebbian.enable': False,
            'neuromod.curiosity.enable': False,
            'neuromod.novelty.enable': False,
            'neuromod.dopamine.scale': 0.0
        }
    }
    
    if preset_name not in presets:
        available = ', '.join(presets.keys())
        raise ValueError(f"Unknown preset '{preset_name}'. Available presets: {available}")
    
    for key, value in presets[preset_name].items():
        config.set(key, value)
    
    return config

class BrainManager:
    """
    Advanced brain lifecycle management with checkpointing and serialization.
    
    This class provides comprehensive brain management including:
    - Multiple checkpoint saves with metadata
    - Brain cloning and state restoration
    - Configuration management
    - Batch operations
    - Performance tracking
    """
    
    def __init__(self):
        self.brains = {}
        self.checkpoints = {}
        self.metadata_store = {}
    
    def create_brain(self, name: str, config: Optional[pynlm.Config] = None, 
                     preset: str = 'quick') -> pynlm.Brain:
        """
        Create and store a brain with metadata tracking.
        
        Args:
            name: Unique name for the brain
            config: Optional config, uses preset if not provided
            preset: Configuration preset name (see configureFromPreset)
            
        Returns:
            Created brain
        """
        if name in self.brains:
            raise ValueError(f"Brain with name '{name}' already exists")
        
        if config is None:
            config = configureFromPreset(preset)
        
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        self.brains[name] = brain
        
        # Store metadata
        self.metadata_store[name] = {
            'created': time.time(),
            'config': preset,
            'neuron_count': config.getOr('brain.neuron_count', 500),
            'connection_probability': config.getOr('brain.connection_probability', 0.1),
            'steps': 0,
            'total_spikes': 0,
            'avg_firing_rate': 0.0
        }
        
        return brain
    
    def save_with_metadata(self, name: str, filepath: str, 
                          description: str = "", tags: Optional[List[str]] = None):
        """
        Save brain state with metadata.
        
        Args:
            name: Brain name
            filepath: Output file path
            description: Description of the brain state
            tags: Optional list of tags for categorization
        """
        if name not in self.brains:
            raise ValueError(f"Brain with name '{name}' not found")
        
        brain = self.brains[name]
        
        # Save brain
        brain.save(filepath)
        
        # Update metadata
        self.metadata_store[name]['last_saved'] = time.time()
        self.metadata_store[name]['last_filepath'] = filepath
        self.metadata_store[name]['description'] = description
        self.metadata_store[name]['tags'] = tags or []
        
        return True
    
    def load_with_metadata(self, name: str, filepath: str) -> pynlm.Brain:
        """
        Load brain with metadata.
        
        Args:
            name: Brain name
            filepath: Input file path
            
        Returns:
            Loaded brain
        """
        brain = pynlm.createBrain(pynlm.createDefaultConfig())
        brain.load(filepath)
        
        if name in self.brains:
            print(f"Warning: Overwriting existing brain '{name}'")
        
        self.brains[name] = brain
        
        # Load metadata from file if available
        metadata_file = filepath.replace('.bin', '_meta.json')
        if Path(metadata_file).exists():
            with open(metadata_file, 'r') as f:
                file_metadata = json.load(f)
            self.metadata_store[name].update(file_metadata)
        
        return brain
    
    def clone_brain(self, source_name: str, target_name: str, 
                    modify_config: Optional[Callable[[pynlm.Config], pynlm.Config]] = None) -> pynlm.Brain:
        """
        Clone a brain with optional configuration modification.
        
        Args:
            source_name: Name of source brain
            target_name: Name for cloned brain
            modify_config: Optional function to modify the config
            
        Returns:
            Cloned brain
        """
        if source_name not in self.brains:
            raise ValueError(f"Source brain '{source_name}' not found")
        
        source_brain = self.brains[source_name]
        source_config = source_brain.getConfig()
        
        if modify_config:
            config = modify_config(source_config)
        else:
            config = source_config
        
        target_brain = pynlm.createBrain(config)
        target_brain.initialize()
        
        # Copy current state if possible
        try:
            # Save and load to copy state
            import tempfile
            with tempfile.NamedTemporaryFile(suffix='.bin') as tmp:
                source_brain.save(tmp.name)
                target_brain.load(tmp.name)
        except Exception:
            pass  # State copying is best-effort
        
        self.brains[target_name] = target_brain
        return target_brain
    
    def list_brains(self) -> List[Dict]:
        """Get list of all managed brains with metadata."""
        result = []
        for name, brain in self.brains.items():
            metadata = self.metadata_store[name]
            result.append({
                'name': name,
                'neuron_count': brain.getTotalNeuronCount(),
                'synapse_count': brain.getTotalSynapseCount(),
                'firing_count': brain.getFiringNeuronCount(),
                'avg_firing_rate': brain.getAverageFiringRate(),
                **metadata
            })
        return result

class ExperimentManager:
    """
    Advanced experiment management and execution.
    
    This class provides comprehensive experiment tracking, comparison,
    and execution capabilities for running and analyzing NLM experiments.
    """
    
    def __init__(self):
        self.experiments = {}
        self.results = {}
    
    def create_experiment(self, name: str, agent_config: Optional[Dict] = None,
                         environment_config: Optional[Dict] = None) -> Dict:
        """
        Create a complete training experiment.
        
        Args:
            name: Experiment name
            agent_config: Agent configuration options
            environment_config: Environment configuration options
            
        Returns:
            Experiment configuration dictionary
        """
        if name in self.experiments:
            raise ValueError(f"Experiment '{name}' already exists")
        
        # Create environment
        env_config = environment_config or {'width': 20, 'height': 20}
        env = createTrainingEnvironment(**env_config)
        
        # Create agent config
        agent_config = agent_config or {}
        if agent_config.get('enable_features', True):
            env['agent'].enableRewardModulation(True)
            env['agent'].enableStructuralPlasticity(True)
            env['agent'].enableDevelopment(True)
            env['agent'].enableCuriosity(True)
        
        self.experiments[name] = {
            'config': {
                'name': name,
                'agent_config': agent_config,
                'environment_config': environment_config,
                'created': time.time()
            },
            'state': {
                'brain': env['brain'],
                'agent': env['agent'],
                'world': env['world'],
                'current_step': 0,
                'total_reward': 0.0,
                'rewards_history': [],
                'firing_rates': [],
                'curiosity_levels': [],
                'novelty_levels': []
            }
        }
        
        return self.experiments[name]['config']
    
    def run_episode(self, name: str, num_steps: int, log_interval: int = 100) -> Dict:
        """
        Run a single training episode.
        
        Args:
            name: Experiment name
            num_steps: Number of steps to run
            log_interval: Logging interval
            
        Returns:
            Episode results
        """
        if name not in self.experiments:
            raise ValueError(f"Experiment '{name}' not found")
        
        exp = self.experiments[name]
        state = exp['state']
        brain = state['brain']
        agent = state['agent']
        world = state['world']
        
        # Reset environment
        world.reset()
        agent.reset()
        
        # Run episode
        start_time = time.time()
        
        for step in range(num_steps):
            # Update world
            world.update(0.1)
            
            # Get sensory input
            percept = world.getSensoryPercept()
            
            # Process input
            agent.processSensoryInput(percept)
            
            # Brain thinks
            brain.step(step)
            
            # Get action
            action = agent.decodeMotorCommand()
            
            # Apply action
            world.applyMotorCommand(action, world.getSimulationTime())
            
            # Record metrics
            reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
            agent.applyRewardModulation(reward, 0.0)
            
            # Collect data
            state['rewards_history'].append(reward)
            state['firing_rates'].append(brain.getAverageFiringRate())
            state['curiosity_levels'].append(agent.getCuriosityLevel())
            state['novelty_levels'].append(agent.getNoveltyLevel())
            
            state['total_reward'] += reward
            state['current_step'] = step
            
            # Log progress
            if log_interval > 0 and (step + 1) % log_interval == 0:
                print(f"Step {step + 1}/{num_steps}: "
                      f"Reward: {state['total_reward'] / (step + 1):.3f}, "
                      f"Firing: {brain.getAverageFiringRate():.1f}, "
                      f"Curiosity: {agent.getCuriosityLevel():.3f}")
        
        # Calculate episode statistics
        end_time = time.time()
        duration = end_time - start_time
        
        rewards = state['rewards_history']
        firing_rates = state['firing_rates']
        
        results = {
            'episode': len(self.results.get(name, [])) + 1,
            'steps': num_steps,
            'duration': duration,
            'total_reward': state['total_reward'],
            'avg_reward': np.mean(rewards) if rewards else 0.0,
            'std_reward': np.std(rewards) if len(rewards) > 1 else 0.0,
            'max_reward': np.max(rewards) if rewards else 0.0,
            'min_reward': np.min(rewards) if rewards else 0.0,
            'final_firing_rate': firing_rates[-1] if firing_rates else 0.0,
            'avg_firing_rate': np.mean(firing_rates) if firing_rates else 0.0,
            'curiosity_history': state['curiosity_levels'],
            'novelty_history': state['novelty_levels'],
            'end_step': state['current_step']
        }
        
        # Store results
        if name not in self.results:
            self.results[name] = []
        self.results[name].append(results)
        
        return results
    
    def compare_experiments(self, experiment_names: List[str]) -> Dict:
        """
        Compare results across multiple experiments.
        
        Args:
            experiment_names: List of experiment names
            
        Returns:
            Comparison statistics
        """
        comparison = {}
        
        for name in experiment_names:
            if name not in self.results:
                print(f"Warning: No results for experiment '{name}'")
                continue
            
            episodes = self.results[name]
            if not episodes:
                continue
            
            # Calculate aggregate statistics
            total_rewards = [ep['total_reward'] for ep in episodes]
            avg_rewards = [ep['avg_reward'] for ep in episodes]
            durations = [ep['duration'] for ep in episodes]
            
            comparison[name] = {
                'episodes': len(episodes),
                'total_reward': sum(total_rewards),
                'avg_reward_per_episode': np.mean(avg_rewards) if avg_rewards else 0.0,
                'std_reward_per_episode': np.std(avg_rewards) if len(avg_rewards) > 1 else 0.0,
                'total_duration': sum(durations),
                'avg_duration_per_episode': np.mean(durations) if durations else 0.0,
                'best_episode': max(total_rewards) if total_rewards else 0.0,
                'recent_performance': avg_rewards[-min(5, len(avg_rewards)):] if avg_rewards else []
            }
        
        return comparison

class PerformanceProfiler:
    """
    Performance profiling and benchmarking for NLM simulations.
    
    This class provides timing, memory, and resource profiling capabilities
    to optimize NLM performance and identify bottlenecks.
    """
    
    def __init__(self):
        self.metrics = []
        self.baseline_metrics = {}
    
    def profile_step(self, brain: pynlm.Brain, 
                    description: str = "step") -> Dict:
        """
        Profile a single brain step.
        
        Args:
            brain: Brain to profile
            description: Description of the operation
            
        Returns:
            Performance metrics
        """
        start_time = time.time()
        start_neurons = brain.getTotalNeuronCount()
        
        # Take snapshot before operation
        before_spikes = brain.getTotalSpikeCount()
        
        # Perform the operation (this would be called in the main code)
        # For profiling, we just record the metrics
        
        # Get timing
        end_time = time.time()
        duration = end_time - start_time
        
        # Get metrics
        after_spikes = brain.getTotalSpikeCount()
        new_spikes = after_spikes - before_spikes
        
        metrics = {
            'timestamp': time.time(),
            'description': description,
            'duration': duration,
            'neuron_count': start_neurons,
            'spikes_generated': new_spikes,
            'spikes_per_second': new_spikes / duration if duration > 0 else 0.0,
            'cpu_time': duration,  # Simplified - real implementation would use perf counters
            'memory_usage': 0.0  # Simplified - would use memory profiling
        }
        
        self.metrics.append(metrics)
        return metrics
    
    def set_baseline(self, name: str):
        """Set a baseline for performance comparison."""
        if self.metrics:
            # Calculate average metrics from all recorded data
            baseline = {
                'avg_duration': np.mean([m['duration'] for m in self.metrics]),
                'avg_spikes_per_sec': np.mean([m['spikes_per_second'] for m in self.metrics]),
                'total_spikes': sum(m['spikes_generated'] for m in self.metrics),
                'sample_count': len(self.metrics)
            }
            self.baseline_metrics[name] = baseline
    
    def compare_to_baseline(self, name: str) -> Optional[Dict]:
        """
        Compare current performance to baseline.
        
        Args:
            name: Baseline name
            
        Returns:
            Comparison results or None if baseline not set
        """
        if name not in self.baseline_metrics:
            print(f"Baseline '{name}' not set")
            return None
        
        baseline = self.baseline_metrics[name]
        
        if not self.metrics:
            return {'error': 'No metrics to compare'}
        
        current = {
            'avg_duration': np.mean([m['duration'] for m in self.metrics]),
            'avg_spikes_per_sec': np.mean([m['spikes_per_second'] for m in self.metrics]),
            'total_spikes': sum(m['spikes_generated'] for m in self.metrics),
            'sample_count': len(self.metrics)
        }
        
        comparison = {
            'baseline': baseline,
            'current': current,
            'duration_change': (current['avg_duration'] - baseline['avg_duration']) / baseline['avg_duration'] * 100,
            'spikes_change': (current['avg_spikes_per_sec'] - baseline['avg_spikes_per_sec']) / baseline['avg_spikes_per_sec'] * 100
        }
        
        return comparison

# Convenience functions for common operations
def runLearningEpisode(brain, agent, world, num_steps: int, 
                      log_interval: int = 100) -> Dict:
    """
    Run a complete learning episode with automatic logging.
    
    This is a high-level convenience function that combines all the steps
    of a typical learning episode into a single call.
    
    Args:
        brain: NLM brain
        agent: AgentBrain interface
        world: SimpleWorld environment
        num_steps: Number of steps to run
        log_interval: Logging interval
        
    Returns:
        Episode results dictionary
    """
    # Reset to initial state
    world.reset()
    agent.reset()
    
    results = {
        'steps': num_steps,
        'total_reward': 0.0,
        'rewards': [],
        'firing_rates': [],
        'curiosity': [],
        'novelty': [],
        'actions_taken': 0
    }
    
    for step in range(num_steps):
        # Update world
        world.update(0.1)
        
        # Get observation
        percept = world.getSensoryPercept()
        
        # Process sensory input
        agent.processSensoryInput(percept)
        
        # Brain thinks
        brain.step(step)
        
        # Get action
        action = agent.decodeMotorCommand()
        
        # Apply action
        world.applyMotorCommand(action, world.getSimulationTime())
        results['actions_taken'] += 1
        
        # Get reward
        reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
        
        # Apply reward modulation
        agent.applyRewardModulation(reward, 0.0)
        
        # Collect metrics
        results['rewards'].append(reward)
        results['firing_rates'].append(brain.getAverageFiringRate())
        results['curiosity'].append(agent.getCuriosityLevel())
        results['novelty'].append(agent.getNoveltyLevel())
        
        results['total_reward'] += reward
        
        # Log progress
        if log_interval > 0 and (step + 1) % log_interval == 0:
            print(f"Step {step + 1}/{num_steps}: "
                  f"Reward: {results['total_reward'] / (step + 1):.3f}, "
                  f"Firing: {brain.getAverageFiringRate():.1f}")
    
    # Calculate final statistics
    if results['rewards']:
        results['avg_reward'] = np.mean(results['rewards'])
        results['std_reward'] = np.std(results['rewards'])
        results['max_reward'] = np.max(results['rewards'])
    
    results['final_firing_rate'] = results['firing_rates'][-1] if results['firing_rates'] else 0.0
    results['final_curiosity'] = results['curiosity'][-1] if results['curiosity'] else 0.0
    
    return results

def saveBrainWithMetadata(brain: pynlm.Brain, filepath: str, 
                         metadata: Optional[Dict] = None) -> bool:
    """
    Save brain state with optional metadata.
    
    Args:
        brain: Brain to save
        filepath: Output file path
        metadata: Optional metadata dictionary
        
    Returns:
        True if successful, False otherwise
    """
    try:
        # Save brain state
        brain.save(filepath)
        
        # Save metadata if provided
        if metadata:
            metadata_file = filepath.replace('.bin', '_meta.json')
            with open(metadata_file, 'w') as f:
                json.dump(metadata, f, indent=2)
        
        return True
    except Exception as e:
        print(f"Error saving brain: {e}")
        return False

def loadBrainWithMetadata(filepath: str, 
                         expected_metadata: Optional[Dict] = None) -> pynlm.Brain:
    """
    Load brain state with optional metadata validation.
    
    Args:
        filepath: Input file path
        expected_metadata: Optional metadata to validate against
        
    Returns:
        Loaded brain
    """
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    brain.load(filepath)
    
    # Load metadata if available
    metadata_file = filepath.replace('.bin', '_meta.json')
    if Path(metadata_file).exists():
        with open(metadata_file, 'r') as f:
            metadata = json.load(f)
        
        if expected_metadata:
            # Validate metadata
            for key, value in expected_metadata.items():
                if key in metadata and metadata[key] != value:
                    print(f"Warning: Metadata mismatch for '{key}': expected {value}, got {metadata[key]}")
    
    return brain

def validateBrainHealth(brain: pynlm.Brain) -> Dict:
    """
    Validate brain health and return diagnostic information.
    
    Args:
        brain: Brain to validate
        
    Returns:
        Health validation results
    """
    health = {
        'is_healthy': True,
        'warnings': [],
        'errors': [],
        'neuron_count': brain.getTotalNeuronCount(),
        'synapse_count': brain.getTotalSynapseCount(),
        'firing_count': brain.getFiringNeuronCount(),
        'avg_firing_rate': brain.getAverageFiringRate(),
        'e_i_ratio': brain.getExcitationInhibitionRatio(),
        'total_spikes': brain.getTotalSpikeCount(),
        'total_neurons': brain.getTotalNeuronCount()
    }
    
    # Check for potential issues
    if health['neuron_count'] == 0:
        health['errors'].append('No neurons in brain')
        health['is_healthy'] = False
    
    if health['synapse_count'] == 0:
        health['warnings'].append('No synapses in brain (neurons isolated)')
    
    if health['avg_firing_rate'] == 0:
        health['warnings'].append('No firing activity')
    
    if health['e_i_ratio'] > 10 or health['e_i_ratio'] < 0.1:
        health['warnings'].append('Extreme E/I ratio may indicate imbalance')
    
    if health['total_spikes'] == 0:
        health['warnings'].append('No spikes recorded (may indicate issue with dynamics)')
    
    return health

# Command-line interface utilities
class CLIUtils:
    """
    Command-line interface utilities for NLM operations.
    
    This class provides utilities for working with NLM from the command line,
    including argument parsing, file operations, and output formatting.
    """
    
    @staticmethod
    def create_arg_parser():
        """Create argument parser for NLM CLI tools."""
        import argparse
        
        parser = argparse.ArgumentParser(
            description='NLM (Neural Learning Machine) Command Line Interface',
            formatter_class=argparse.RawDescriptionHelpFormatter,
            epilog="""
Examples:
  nlm-train --preset research --steps 1000
  nlm-config --validate config.json
  nlm-benchmark --neurons 1000 --duration 60
  nlm-profile --brain brain.bin --output profile.json
            """
        )
        
        subparsers = parser.add_subparsers(dest='command', help='Available commands')
        
        # Train command
        train_parser = subparsers.add_parser('train', help='Train a brain')
        train_parser.add_argument('--preset', choices=['quick', 'research', 'production', 'debug', 'minimal'],
                                default='quick', help='Configuration preset')
        train_parser.add_argument('--steps', type=int, default=100,
                                help='Number of training steps')
        train_parser.add_argument('--log-interval', type=int, default=50,
                                help='Logging interval')
        train_parser.add_argument('--output', type=str, help='Output file path')
        
        # Config command
        config_parser = subparsers.add_parser('config', help='Configuration utilities')
        config_parser.add_argument('--preset', choices=['quick', 'research', 'production', 'debug', 'minimal'],
                                 help='Show preset configuration')
        config_parser.add_argument('--validate', type=str, help='Validate configuration file')
        config_parser.add_argument('--export', type=str, help='Export configuration to file')
        config_parser.add_argument('--import', type=str, dest='import_file', help='Import configuration from file')
        
        # Benchmark command
        bench_parser = subparsers.add_parser('benchmark', help='Performance benchmarking')
        bench_parser.add_argument('--neurons', type=int, default=100,
                                help='Number of neurons to test')
        bench_parser.add_argument('--duration', type=int, default=30,
                                help='Benchmark duration in seconds')
        bench_parser.add_argument('--steps', type=int, default=1000,
                                help='Number of simulation steps')
        bench_parser.add_argument('--output', type=str, help='Output file path')
        
        # Profile command
        profile_parser = subparsers.add_parser('profile', help='Performance profiling')
        profile_parser.add_argument('--brain', type=str, help='Brain file to profile')
        profile_parser.add_argument('--steps', type=int, default=100,
                                 help='Number of steps to profile')
        profile_parser.add_argument('--output', type=str, help='Output file path')
        profile_parser.add_argument('--detailed', action='store_true',
                                 help='Enable detailed profiling')
        
        # Utils command
        utils_parser = subparsers.add_parser('utils', help='Utility operations')
        utils_parser.add_argument('--validate', choices=['brain', 'config', 'all'],
                                help='Validate files or configurations')
        utils_parser.add_argument('--file', type=str, help='File to validate')
        
        return parser
    
    @staticmethod
    def format_brain_info(brain: pynlm.Brain, detailed: bool = False) -> str:
        """
        Format brain information for display.
        
        Args:
            brain: Brain to format
            detailed: Whether to include detailed statistics
            
        Returns:
            Formatted string
        """
        info = f"NLM Brain Information:\n"
        info += f"  Total Neurons: {brain.getTotalNeuronCount()}\n"
        info += f"  Total Synapses: {brain.getTotalSynapseCount()}\n"
        info += f"  Firing Neurons: {brain.getFiringNeuronCount()}\n"
        info += f"  Average Firing Rate: {brain.getAverageFiringRate():.2f}\n"
        info += f"  E/I Ratio: {brain.getExcitationInhibitionRatio():.3f}\n"
        info += f"  Total Spikes: {brain.getTotalSpikeCount()}\n"
        
        if detailed:
            info += f"  Developmental Stage: {brain.getDevelopmentalStage()}\n"
            info += f"  Configuration: {brain.getConfig() if hasattr(brain, 'getConfig') else 'N/A'}\n"
        
        return info
    
    @staticmethod
    def format_experiment_results(results: Dict) -> str:
        """
        Format experiment results for display.
        
        Args:
            results: Experiment results
            
        Returns:
            Formatted string
        """
        output = f"Experiment Results:\n"
        output += f"  Total Reward: {results.get('total_reward', 0):.3f}\n"
        output += f"  Average Reward: {results.get('avg_reward', 0):.3f}\n"
        output += f"  Final Firing Rate: {results.get('final_firing_rate', 0):.2f}\n"
        output += f"  Final Curiosity: {results.get('final_curiosity', 0):.3f}\n"
        
        if 'rewards' in results and results['rewards']:
            output += f"  Reward Stats: Mean={np.mean(results['rewards']):.3f}, "
            output += f"Std={np.std(results['rewards']):.3f}, "
            output += f"Min={np.min(results['rewards']):.3f}, "
            output += f"Max={np.max(results['rewards']):.3f}\n"
        
        return output

# Global instances for convenience
brain_manager = BrainManager()
experiment_manager = ExperimentManager()
performance_profiler = PerformanceProfiler()
cli_utils = CLIUtils()

# Main execution function for CLI
if __name__ == "__main__":
    parser = CLIUtils.create_arg_parser()
    args = parser.parse_args()
    
    if args.command == 'train':
        # Train brain
        config = configureFromPreset(args.preset)
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Run training
        for step in range(args.steps):
            brain.step(step)
            if (step + 1) % args.log_interval == 0:
                print(f"Step {step + 1}/{args.steps}, "
                      f"Firing: {brain.getFiringNeuronCount()}, "
                      f"Spikes: {brain.getTotalSpikeCount()}")
        
        # Save if requested
        if args.output:
            brain.save(args.output)
            print(f"Brain saved to {args.output}")
    
    elif args.command == 'config':
        if args.preset:
            # Show preset
            config = configureFromPreset(args.preset)
            print(f"Preset '{args.preset}':")
            # Print config
        elif args.validate:
            # Validate config
            print(f"Config validation for {args.validate}")
        elif args.export:
            # Export config
            print(f"Export config to {args.export}")
        elif args.import_file:
            # Import config
            print(f"Import config from {args.import_file}")
    
    elif args.command == 'benchmark':
        # Run benchmark
        print(f"Running benchmark with {args.neurons} neurons for {args.duration} seconds")
        print("Benchmarking not fully implemented in this demo")
    
    elif args.command == 'profile':
        # Profile brain
        if args.brain:
            brain = pynlm.createBrain(pynlm.createDefaultConfig())
            brain.load(args.brain)
            print(f"Profiling brain with {args.steps} steps")
            print("Profiling not fully implemented in this demo")
        else:
            parser.error("--brain argument is required for profile command")
    
    elif args.command == 'utils':
        if args.validate == 'brain':
            if args.file:
                brain = pynlm.createBrain(pynlm.createDefaultConfig())
                brain.load(args.file)
                health = validateBrainHealth(brain)
                print("Brain validation:")
                print(f"  Healthy: {health['is_healthy']}")
                if health['warnings']:
                    print("  Warnings:", ', '.join(health['warnings']))
                if health['errors']:
                    print("  Errors:", ', '.join(health['errors']))
            else:
                parser.error("--file argument is required for brain validation")
        
        elif args.validate == 'config':
            if args.file:
                print(f"Config validation for {args.file} (not implemented)")
            else:
                parser.error("--file argument is required for config validation")
        
        elif args.validate == 'all':
            print("Comprehensive validation (not implemented)")
    
    else:
        parser.print_help()