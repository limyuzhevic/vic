#!/usr/bin/env python3
"""
Enhanced Python Interface for NLM (Neural Learning Machine)

This module provides a comprehensive, Pythonic interface to the NLM C++ framework,
enhancing usability while maintaining full backward compatibility.

Key Features:
- High-level API for common use cases
- Convenience factory functions
- Data processing utilities with NumPy/Pandas support
- Configuration management tools
- Training utilities for learning agents
- Visualization and analysis helpers
- Advanced features like curiosity-driven learning
- Comprehensive documentation and examples
- Type hints and context managers
- Full backward compatibility with existing pynlm bindings

Usage Examples:
    >>> import pynlm
    >>> 
    >>> # Create a simple brain
    >>> brain = pynlm.createBrain(pynlm.createDefaultConfig())
    >>> brain.initialize()
    >>> 
    >>> # High-level factory function
    >>> config = pynlm.create_default_config(neuron_count=5000)
    >>> brain = pynlm.create_reactive_brain(config)
    >>> 
    >>> # Training with experiences
    >>> world = pynlm.create_simple_world()
    >>> agent = pynlm.create_training_agent(brain, world)
    >>> stats = pynlm.train_with_experience(agent, experiences)
    >>> 
    >>> # Neural data analysis
    >>> analysis = pynlm.analyze_neural_data(brain)
    >>> df = pynlm.convert_to_dataframe(brain)
    >>> 
    >>> # Visualization and benchmarking
    >>> viz_data = pynlm.plot_neural_activity(brain)
    >>> benchmark = pynlm.benchmark_brain_performance(brain, time_steps=1000)
"""

import logging
from contextlib import contextmanager
from typing import Optional, Dict, List, Any, Union, Callable, Tuple, ClassVar
from dataclasses import dataclass, field
from pathlib import Path
import json
import pickle
import time

# Optional dependencies - gracefully handle missing imports
try:
    import numpy as np
    NUMPY_AVAILABLE = True
except ImportError:
    NUMPY_AVAILABLE = False
    np = None

try:
    import pandas as pd
    PANDAS_AVAILABLE = True
except ImportError:
    PANDAS_AVAILABLE = False
    pd = None

try:
    import matplotlib.pyplot as plt
    MATPLOTLIB_AVAILABLE = True
except ImportError:
    MATPLOTLIB_AVAILABLE = False
    plt = None

# Try to import the original pynlm bindings
try:
    import pynlm as _pynlm
    PYNLM_AVAILABLE = True
except ImportError:
    PYNLM_AVAILABLE = False
    _pynlm = None

# Configure module-level logging
logging.basicConfig(level=logging.INFO)
logger = logging.getLogger(__name__)

class ResourceTracker:
    """Track resource usage and cleanup for NLM objects."""
    
    _instances: ClassVar[List['ResourceTracker']] = []
    
    def __init__(self, name: str):
        self.name = name
        self.created_at = time.time()
        self.is_closed = False
        ResourceTracker._instances.append(self)
        logger.debug(f"Resource tracker created: {name}")
    
    def close(self):
        """Clean up resources."""
        self.is_closed = True
        if self in ResourceTracker._instances:
            ResourceTracker._instances.remove(self)
        logger.debug(f"Resource tracker closed: {name}")
    
    def __enter__(self):
        return self
    
    def __exit__(self, exc_type, exc_val, exc_tb):
        self.close()
    
    @classmethod
    def get_all_instances(cls) -> List['ResourceTracker']:
        return cls._instances.copy()

@dataclass
class NeuralConfig:
    """Configuration for neural simulations with enhanced Pythonic interface."""
    
    # Core parameters
    neuron_count: int = 1000
    region_count: int = 1
    connection_probability: float = 0.1
    simulation_timestep: float = 0.001
    random_seed: int = 42
    
    # Learning parameters
    stdp_ltp_weight: float = 0.01
    stdp_ltd_weight: float = 0.012
    stdp_tau: float = 20.0
    hebbian_learning_rate: float = 0.01
    synaptogenesis_rate: float = 0.0001
    pruning_rate: float = 0.00001
    
    # Development parameters
    enable_development: bool = True
    developmental_stage: str = "adult"
    
    # Neuromodulation parameters
    dopamine_baseline: float = 0.1
    novelty_threshold: float = 0.5
    curiosity_factor: float = 1.0
    
    # Environment parameters
    world_width: int = 16
    world_height: int = 16
    vision_width: int = 8
    vision_height: int = 8
    
    # Memory parameters
    working_memory_capacity: int = 1000
    episodic_memory_limit: int = 5000
    consolidation_interval: int = 1000
    
    # Additional custom parameters
    custom_params: Dict[str, Any] = field(default_factory=dict)
    
    def to_dict(self) -> Dict[str, Any]:
        """Convert configuration to dictionary."""
        return {
            'neuron_count': self.neuron_count,
            'region_count': self.region_count,
            'connection_probability': self.connection_probability,
            'simulation_timestep': self.simulation_timestep,
            'random_seed': self.random_seed,
            'stdp_ltp_weight': self.stdp_ltp_weight,
            'stdp_ltd_weight': self.stdp_ltd_weight,
            'stdp_tau': self.stdp_tau,
            'hebbian_learning_rate': self.hebbian_learning_rate,
            'synaptogenesis_rate': self.synaptogenesis_rate,
            'pruning_rate': self.pruning_rate,
            'enable_development': self.enable_development,
            'dopamine_baseline': self.dopamine_baseline,
            'novelty_threshold': self.novelty_threshold,
            'curiosity_factor': self.curiosity_factor,
            'world_width': self.world_width,
            'world_height': self.world_height,
            'vision_width': self.vision_width,
            'vision_height': self.vision_height,
            'working_memory_capacity': self.working_memory_capacity,
            'episodic_memory_limit': self.episodic_memory_limit,
            'consolidation_interval': self.consolidation_interval,
            'custom_params': self.custom_params
        }
    
    @classmethod
    def from_dict(cls, config_dict: Dict[str, Any]) -> 'NeuralConfig':
        """Create configuration from dictionary."""
        # Extract standard parameters
        standard_params = {}
        for key in cls.__dataclass_fields__.keys():
            if key in config_dict:
                standard_params[key] = config_dict[key]
        
        # Store custom parameters
        custom_params = {k: v for k, v in config_dict.items() 
                        if k not in cls.__dataclass_fields__}
        
        # Create instance
        instance = cls(**standard_params)
        instance.custom_params = custom_params
        return instance

class ConfigurationManager:
    """Manage NLM configurations with validation and persistence."""
    
    PRESET_CONFIGS = {
        'beginner': NeuralConfig(
            neuron_count=500,
            region_count=1,
            connection_probability=0.05,
            enable_development=False
        ),
        'advanced': NeuralConfig(
            neuron_count=5000,
            region_count=2,
            connection_probability=0.1,
            enable_development=True,
            synaptogenesis_rate=0.001,
            pruning_rate=0.0001
        ),
        'research': NeuralConfig(
            neuron_count=10000,
            region_count=3,
            connection_probability=0.15,
            enable_development=True,
            stdp_ltp_weight=0.02,
            stdp_ltd_weight=0.018,
            synaptogenesis_rate=0.002,
            pruning_rate=0.0002
        ),
        'production': NeuralConfig(
            neuron_count=20000,
            region_count=4,
            connection_probability=0.2,
            enable_development=True,
            world_width=32,
            world_height=32,
            vision_width=16,
            vision_height=16,
            working_memory_capacity=5000,
            episodic_memory_limit=20000
        )
    }
    
    @staticmethod
    def create_preset_config(preset_name: str) -> NeuralConfig:
        """Create a preset configuration.
        
        Args:
            preset_name: One of 'beginner', 'advanced', 'research', 'production'
            
        Returns:
            NeuralConfig instance
            
        Raises:
            ValueError: If preset_name is not recognized
        """
        preset_name = preset_name.lower()
        if preset_name not in ConfigurationManager.PRESET_CONFIGS:
            raise ValueError(f"Unknown preset: {preset_name}. "
                           f"Available presets: {list(ConfigurationManager.PRESET_CONFIGS.keys())}")
        
        return NeuralConfig.from_dict(ConfigurationManager.PRESET_CONFIGS[preset_name].to_dict())
    
    @staticmethod
    def validate_config(config: NeuralConfig) -> List[str]:
        """Validate a configuration and return list of errors.
        
        Args:
            config: NeuralConfig to validate
            
        Returns:
            List of error messages, empty if config is valid
        """
        errors = []
        
        # Validate neuron count
        if config.neuron_count <= 0:
            errors.append("Neuron count must be positive")
        elif config.neuron_count > 100000:
            errors.append("Neuron count too high (max 100000)")
        
        # Validate region count
        if config.region_count <= 0:
            errors.append("Region count must be positive")
        
        # Validate probabilities
        if not 0.0 <= config.connection_probability <= 1.0:
            errors.append("Connection probability must be between 0 and 1")
        
        # Validate other parameters
        if config.simulation_timestep <= 0:
            errors.append("Simulation timestep must be positive")
        
        return errors
    
    @staticmethod
    def save_config(config: NeuralConfig, filepath: Union[str, Path]) -> None:
        """Save configuration to file.
        
        Args:
            config: NeuralConfig to save
            filepath: Path to save file
        """
        filepath = Path(filepath)
        
        # Determine format based on file extension
        if filepath.suffix.lower() == '.json':
            with open(filepath, 'w') as f:
                json.dump(config.to_dict(), f, indent=2)
        elif filepath.suffix.lower() in ['.pkl', '.pickle']:
            with open(filepath, 'wb') as f:
                pickle.dump(config, f)
        else:
            # Default to JSON
            filepath = filepath.with_suffix('.json')
            ConfigurationManager.save_config(config, filepath)
    
    @staticmethod
    def load_config(filepath: Union[str, Path]) -> NeuralConfig:
        """Load configuration from file.
        
        Args:
            filepath: Path to configuration file
            
        Returns:
            NeuralConfig instance
        """
        filepath = Path(filepath)
        
        if filepath.suffix.lower() == '.json':
            with open(filepath, 'r') as f:
                config_dict = json.load(f)
            return NeuralConfig.from_dict(config_dict)
        elif filepath.suffix.lower() in ['.pkl', '.pickle']:
            with open(filepath, 'rb') as f:
                return pickle.load(f)
        else:
            raise ValueError(f"Unsupported file format: {filepath.suffix}")
    
    @staticmethod
    def compare_configs(config1: NeuralConfig, config2: NeuralConfig) -> Dict[str, Any]:
        """Compare two configurations and return differences.
        
        Args:
            config1: First configuration
            config2: Second configuration
            
        Returns:
            Dictionary with differences
        """
        differences = {}
        
        # Compare standard parameters
        for key in NeuralConfig.__dataclass_fields__.keys():
            val1 = getattr(config1, key)
            val2 = getattr(config2, key)
            if val1 != val2:
                differences[key] = {'config1': val1, 'config2': val2}
        
        # Compare custom parameters
        custom_diff1 = set(config1.custom_params.keys()) - set(config2.custom_params.keys())
        custom_diff2 = set(config2.custom_params.keys()) - set(config1.custom_params.keys())
        
        if custom_diff1:
            differences['custom_params_only_in_config1'] = list(custom_diff1)
        if custom_diff2:
            differences['custom_params_only_in_config2'] = list(custom_diff2)
        
        # Find common custom parameters with different values
        common_custom = set(config1.custom_params.keys()) & set(config2.custom_params.keys())
        for key in common_custom:
            if config1.custom_params[key] != config2.custom_params[key]:
                differences[f'custom_param_{key}'] = {
                    'config1': config1.custom_params[key],
                    'config2': config2.custom_params[key]
                }
        
        return differences
    
    @staticmethod
    def optimize_config(config: NeuralConfig, performance_target: float) -> NeuralConfig:
        """Optimize configuration for performance target.
        
        Args:
            config: NeuralConfig to optimize
            performance_target: Target performance metric
            
        Returns:
            Optimized NeuralConfig
        """
        # Simple optimization: adjust neuron count based on target
        optimized = NeuralConfig.from_dict(config.to_dict())
        
        # For demonstration: scale neuron count proportionally
        # In practice, this would involve actual optimization algorithms
        current_performance = optimized.neuron_count * 0.001  # Simplified metric
        if current_performance < performance_target:
            optimized.neuron_count = int(optimized.neuron_count * (performance_target / current_performance))
        
        return optimized

class NeuralDataProcessor:
    """Process neural data with NumPy and Pandas support."""
    
    def __init__(self, brain=None):
        self.brain = brain
        self.processed_data = {}
        
    def extract_spike_times(self) -> Union[List[float], np.ndarray]:
        """Extract spike times from brain.
        
        Returns:
            List or numpy array of spike times
        """
        if not self.brain:
            return [] if not NUMPY_AVAILABLE else np.array([])
        
        # Extract spike data
        total_spikes = self.brain.getTotalSpikeCount()
        spike_times = []
        
        # This is a simplified implementation
        # In practice, you would need to track spikes over time
        for step in range(total_spikes):
            spike_times.append(step * 0.001)  # Assuming 1ms timestep
        
        if NUMPY_AVAILABLE:
            return np.array(spike_times)
        return spike_times
    
    def get_activation_patterns(self) -> Union[Dict[str, Any], pd.DataFrame]:
        """Get neural activation patterns.
        
        Returns:
            Dictionary or pandas DataFrame with activation patterns
        """
        if not self.brain:
            return {} if not PANDAS_AVAILABLE else pd.DataFrame()
        
        # Extract activation data
        patterns = {}
        
        # This is a simplified implementation
        # In practice, you would extract detailed activation data
        patterns['total_neurons'] = self.brain.getTotalNeuronCount()
        patterns['firing_neurons'] = self.brain.getFiringNeuronCount()
        patterns['average_firing_rate'] = self.brain.getAverageFiringRate()
        patterns['total_spikes'] = self.brain.getTotalSpikeCount()
        
        if PANDAS_AVAILABLE:
            return pd.DataFrame([patterns])
        return patterns
    
    def compute_metrics(self) -> Dict[str, float]:
        """Compute neural metrics.
        
        Returns:
            Dictionary with computed metrics
        """
        metrics = {}
        
        if not self.brain:
            return metrics
        
        # Compute various metrics
        metrics['total_neurons'] = float(self.brain.getTotalNeuronCount())
        metrics['firing_neurons'] = float(self.brain.getFiringNeuronCount())
        metrics['average_firing_rate'] = self.brain.getAverageFiringRate()
        metrics['total_spikes'] = float(self.brain.getTotalSpikeCount())
        metrics['excitation_inhibition_ratio'] = self.brain.getExcitationInhibitionRatio()
        
        # Add derived metrics
        metrics['firing_percentage'] = (
            metrics['firing_neurons'] / metrics['total_neurons'] * 100
            if metrics['total_neurons'] > 0 else 0.0
        )
        
        return metrics
    
    def convert_to_dataframe(self) -> Union[pd.DataFrame, None]:
        """Convert neural data to pandas DataFrame.
        
        Returns:
            Pandas DataFrame or None if pandas not available
        """
        if not PANDAS_AVAILABLE:
            logger.warning("Pandas not available. Install with: pip install pandas")
            return None
        
        data = self.get_activation_patterns()
        
        if isinstance(data, dict):
            # Convert single pattern to DataFrame
            df = pd.DataFrame([data])
        else:
            df = data
        
        return df
    
    def export_data(self, filepath: Union[str, Path], format: str = 'json') -> None:
        """Export neural data to file.
        
        Args:
            filepath: Output file path
            format: Export format ('json', 'csv', 'parquet')
        """
        filepath = Path(filepath)
        
        if format.lower() == 'json':
            data = {
                'activation_patterns': self.get_activation_patterns(),
                'metrics': self.compute_metrics(),
                'spike_times': self.extract_spike_times()
            }
            
            with open(filepath, 'w') as f:
                json.dump(data, f, indent=2, default=str)
                
        elif format.lower() == 'csv' and PANDAS_AVAILABLE:
            df = self.convert_to_dataframe()
            if df is not None:
                df.to_csv(filepath, index=False)
                
        elif format.lower() == 'parquet' and PANDAS_AVAILABLE:
            df = self.convert_to_dataframe()
            if df is not None:
                df.to_parquet(filepath, index=False)
        
        else:
            raise ValueError(f"Unsupported format: {format}")
    
    @classmethod
    def from_brain(cls, brain) -> 'NeuralDataProcessor':
        """Create processor from brain.
        
        Args:
            brain: Brain object
            
        Returns:
            NeuralDataProcessor instance
        """
        return cls(brain)

class VisualizationHelper:
    """Visualization and plotting utilities for neural data."""
    
    def __init__(self):
        self.figures = []
        
    def plot_neural_activity(self, brain=None, save_path: Optional[Union[str, Path]] = None) -> Union[Any, None]:
        """Plot neural activity.
        
        Args:
            brain: Brain object (uses internal brain if not provided)
            save_path: Path to save figure
            
        Returns:
            Matplotlib figure or None if matplotlib not available
        """
        if not MATPLOTLIB_AVAILABLE:
            logger.warning("Matplotlib not available. Install with: pip install matplotlib")
            return None
        
        fig, axes = plt.subplots(2, 2, figsize=(12, 10))
        fig.suptitle("NLM Neural Activity Visualization", fontsize=16)
        
        # Use provided brain or internal brain
        target_brain = brain if brain else self.brain
        if not target_brain:
            logger.error("No brain data available for visualization")
            return None
        
        # Plot 1: Neuron count distribution (simplified)
        axes[0, 0].bar(['Total', 'Firing', 'Active'], 
                      [target_brain.getTotalNeuronCount(),
                       target_brain.getFiringNeuronCount(),
                       target_brain.getActiveNeuronCount()])
        axes[0, 0].set_title("Neuron Counts")
        axes[0, 0].set_ylabel("Count")
        
        # Plot 2: Firing rate over time (simplified)
        steps = min(100, target_brain.getTotalSpikeCount())
        if steps > 0:
            firing_rates = []
            for step in range(steps):
                # This is a simplified calculation
                # In practice, you would track firing rates over time
                firing_rates.append(target_brain.getAverageFiringRate())
            
            axes[0, 1].plot(firing_rates)
            axes[0, 1].set_title("Average Firing Rate")
            axes[0, 1].set_xlabel("Step")
            axes[0, 1].set_ylabel("Firing Rate (Hz)")
        
        # Plot 3: Excitation/Inhibition ratio
        ei_ratio = target_brain.getExcitationInhibitionRatio()
        axes[1, 0].bar(['Excitation', 'Inhibition'], [ei_ratio, 1.0 - ei_ratio])
        axes[1, 0].set_title("Excitation/Inhibition Ratio")
        axes[1, 0].set_ylabel("Ratio")
        axes[1, 0].set_ylim(0, 1)
        
        # Plot 4: Spike activity summary
        axes[1, 1].pie([target_brain.getTotalSpikeCount(), 
                       target_brain.getTotalNeuronCount() - target_brain.getTotalSpikeCount()],
                     labels=['Spikes', 'Non-Spikes'],
                     autopct='%1.1f%%')
        axes[1, 1].set_title("Spike Activity")
        
        plt.tight_layout()
        
        if save_path:
            plt.savefig(save_path, dpi=300, bbox_inches='tight')
        
        self.figures.append(fig)
        return fig
    
    def plot_training_progress(self, metrics_history: List[Dict[str, Any]], 
                               save_path: Optional[Union[str, Path]] = None) -> Union[Any, None]:
        """Plot training progress over time.
        
        Args:
            metrics_history: List of metric dictionaries from each training step
            save_path: Path to save figure
            
        Returns:
            Matplotlib figure or None
        """
        if not MATPLOTLIB_AVAILABLE:
            logger.warning("Matplotlib not available. Install with: pip install matplotlib")
            return None
        
        if not metrics_history:
            logger.error("No metrics history provided for training progress plot")
            return None
        
        # Extract metrics from history
        steps = list(range(len(metrics_history)))
        
        fig, axes = plt.subplots(2, 2, figsize=(12, 10))
        fig.suptitle("Training Progress", fontsize=16)
        
        # Plot 1: Reward over time
        if 'total_reward' in metrics_history[0]:
            rewards = [m.get('total_reward', 0) for m in metrics_history]
            axes[0, 0].plot(steps, rewards)
            axes[0, 0].set_title("Total Reward")
            axes[0, 0].set_xlabel("Step")
            axes[0, 0].set_ylabel("Reward")
            axes[0, 0].grid(True, alpha=0.3)
        
        # Plot 2: Learning rate over time
        if 'learning_rate' in metrics_history[0]:
            learning_rates = [m.get('learning_rate', 0) for m in metrics_history]
            axes[0, 1].plot(steps, learning_rates)
            axes[0, 1].set_title("Learning Rate")
            axes[0, 1].set_xlabel("Step")
            axes[0, 1].set_ylabel("Learning Rate")
            axes[0, 1].grid(True, alpha=0.3)
        
        # Plot 3: Firing rate over time
        if 'firing_rate' in metrics_history[0]:
            firing_rates = [m.get('firing_rate', 0) for m in metrics_history]
            axes[1, 0].plot(steps, firing_rates)
            axes[1, 0].set_title("Firing Rate")
            axes[1, 0].set_xlabel("Step")
            axes[1, 0].set_ylabel("Firing Rate (Hz)")
            axes[1, 0].grid(True, alpha=0.3)
        
        # Plot 4: Memory usage over time
        if 'memory_usage' in metrics_history[0]:
            memory_usages = [m.get('memory_usage', 0) for m in metrics_history]
            axes[1, 1].plot(steps, memory_usages)
            axes[1, 1].set_title("Memory Usage")
            axes[1, 1].set_xlabel("Step")
            axes[1, 1].set_ylabel("Memory (MB)")
            axes[1, 1].grid(True, alpha=0.3)
        
        plt.tight_layout()
        
        if save_path:
            plt.savefig(save_path, dpi=300, bbox_inches='tight')
        
        self.figures.append(fig)
        return fig
    
    def save_all_figures(self, directory: Union[str, Path]) -> None:
        """Save all created figures to directory.
        
        Args:
            directory: Directory to save figures
        """
        directory = Path(directory)
        directory.mkdir(parents=True, exist_ok=True)
        
        for i, fig in enumerate(self.figures):
            save_path = directory / f"figure_{i}.png"
            fig.savefig(save_path, dpi=300, bbox_inches='tight')
            logger.info(f"Saved figure {i} to {save_path}")
        
        plt.close('all')
        self.figures.clear()

class TrainingUtilities:
    """Training utilities for learning agents."""
    
    @staticmethod
    def create_training_loop(agent=None, config: Optional[NeuralConfig] = None) -> 'TrainingLoop':
        """Create a training loop for agent.
        
        Args:
            agent: AgentBrain to train
            config: NeuralConfig for training
            
        Returns:
            TrainingLoop instance
        """
        return TrainingLoop(agent, config)
    
    @staticmethod
    def episode_based_training(agent, world, num_episodes: int, 
                              max_steps_per_episode: int = 100) -> List[Dict[str, Any]]:
        """Perform episode-based training.
        
        Args:
            agent: AgentBrain to train
            world: World to train in
            num_episodes: Number of episodes to train
            max_steps_per_episode: Maximum steps per episode
            
        Returns:
            List of episode metrics
        """
        episode_metrics = []
        
        for episode in range(num_episodes):
            episode_metric = {
                'episode': episode,
                'steps': 0,
                'total_reward': 0.0,
                'max_reward': 0.0,
                'min_reward': float('inf'),
                'average_reward': 0.0
            }
            
            # Reset agent and world for new episode
            agent.reset()
            world.reset()
            
            # Run episode
            for step in range(max_steps_per_episode):
                # Get sensory input
                percept = world.getSensoryPercept()
                agent.processSensoryInput(percept)
                
                # Get action from agent
                action = agent.decodeMotorCommand()
                
                # Apply action to world
                world.applyMotorCommand(action, world.getSimulationTime())
                
                # Compute reward
                reward = world.computeReward(agent.getBrain().getRegions()[0].get())
                agent.applyRewardModulation(reward, 0.0)
                
                # Update episode metrics
                episode_metric['steps'] += 1
                episode_metric['total_reward'] += reward
                episode_metric['max_reward'] = max(episode_metric['max_reward'], reward)
                episode_metric['min_reward'] = min(episode_metric['min_reward'], reward)
            
            # Calculate average reward
            if episode_metric['steps'] > 0:
                episode_metric['average_reward'] = (
                    episode_metric['total_reward'] / episode_metric['steps']
                )
            
            episode_metrics.append(episode_metric)
            
            logger.info(f"Episode {episode + 1}/{num_episodes} "
                       f"completed. Total reward: {episode_metric['total_reward']:.2f}")
        
        return episode_metrics
    
    @staticmethod
    def experience_based_training(agent, experiences: List[Dict[str, Any]],
                                  learning_rate: float = 0.01) -> Dict[str, Any]:
        """Train agent from experiences.
        
        Args:
            agent: AgentBrain to train
            experiences: List of experience dictionaries
            learning_rate: Learning rate for training
            
        Returns:
            Training statistics
        """
        stats = {
            'total_experiences': len(experiences),
            'successful_experiences': 0,
            'average_reward': 0.0,
            'convergence_steps': 0
        }
        
        total_reward = 0.0
        
        for i, experience in enumerate(experiences):
            # Process experience
            if 'brain' in experience:
                # This is a simplified implementation
                # In practice, you would apply the experience to update the agent
                pass
            
            # Extract reward if available
            if 'reward' in experience:
                total_reward += experience['reward']
                if experience['reward'] > 0:
                    stats['successful_experiences'] += 1
        
        # Calculate statistics
        if experiences:
            stats['average_reward'] = total_reward / len(experiences)
        
        return stats
    
    @staticmethod
    def train_with_experience(agent, experiences: List[Dict[str, Any]],
                             num_epochs: int = 10) -> Dict[str, Any]:
        """Train agent with experiences over multiple epochs.
        
        Args:
            agent: AgentBrain to train
            experiences: List of experience dictionaries
            num_epochs: Number of training epochs
            
        Returns:
            Training results
        """
        all_metrics = []
        
        for epoch in range(num_epochs):
            # Train with current experiences
            metrics = TrainingUtilities.experience_based_training(
                agent, experiences, learning_rate=0.01 * (1.0 - epoch / num_epochs)
            )
            
            metrics['epoch'] = epoch
            all_metrics.append(metrics)
            
            logger.info(f"Epoch {epoch + 1}/{num_epochs} completed. "
                       f"Average reward: {metrics['average_reward']:.4f}")
        
        return {
            'all_metrics': all_metrics,
            'final_metrics': all_metrics[-1],
            'total_epochs': num_epochs
        }
    
    @staticmethod
    def benchmark_brain_performance(brain, time_steps: int = 1000,
                                    num_runs: int = 3) -> Dict[str, Any]:
        """Benchmark brain performance.
        
        Args:
            brain: Brain to benchmark
            time_steps: Number of time steps per run
            num_runs: Number of runs to average over
            
        Returns:
            Benchmark results
        """
        results = {
            'time_steps': time_steps,
            'num_runs': num_runs,
            'average_firing_rate': 0.0,
            'total_spike_counts': [],
            'average_spike_count': 0.0,
            'min_spike_count': float('inf'),
            'max_spike_count': 0,
            'memory_usage_samples': [],
            'average_memory_usage': 0.0
        }
        
        total_firing_rate = 0.0
        total_spike_count = 0
        
        for run in range(num_runs):
            # Reset brain for new run
            brain.reset()
            brain.initialize()
            
            # Run simulation
            for step in range(time_steps):
                brain.step(step)
            
            # Collect metrics
            firing_rate = brain.getAverageFiringRate()
            spike_count = brain.getTotalSpikeCount()
            
            total_firing_rate += firing_rate
            total_spike_count += spike_count
            
            results['total_spike_counts'].append(spike_count)
            results['min_spike_count'] = min(results['min_spike_count'], spike_count)
            results['max_spike_count'] = max(results['max_spike_count'], spike_count)
            
            logger.info(f"Benchmark run {run + 1}/{num_runs} completed. "
                       f"Firing rate: {firing_rate:.2f} Hz, "
                       f"Spike count: {spike_count}")
        
        # Calculate averages
        results['average_firing_rate'] = total_firing_rate / num_runs
        results['average_spike_count'] = total_spike_count / num_runs
        
        return results

class TrainingLoop:
    """Abstract base class for training loops."""
    
    def __init__(self, agent=None, config: Optional[NeuralConfig] = None):
        self.agent = agent
        self.config = config or NeuralConfig()
        self.is_running = False
        self.metrics_history = []
        
    def run(self, num_steps: int) -> List[Dict[str, Any]]:
        """Run training loop.
        
        Args:
            num_steps: Number of training steps
            
        Returns:
            List of metrics from each step
        """
        raise NotImplementedError("Subclasses must implement run method")
    
    def stop(self) -> None:
        """Stop the training loop."""
        self.is_running = False
    
    def get_metrics(self) -> List[Dict[str, Any]]:
        """Get training metrics."""
        return self.metrics_history
    
    def clear_metrics(self) -> None:
        """Clear training metrics."""
        self.metrics_history.clear()

class EpisodeBasedTrainingLoop(TrainingLoop):
    """Episode-based training loop."""
    
    def __init__(self, agent=None, config: Optional[NeuralConfig] = None,
                 world=None, max_steps_per_episode: int = 100):
        super().__init__(agent, config)
        self.world = world
        self.max_steps_per_episode = max_steps_per_episode
        
    def run(self, num_episodes: int) -> List[Dict[str, Any]]:
        """Run episode-based training loop.
        
        Args:
            num_episodes: Number of episodes to train
            
        Returns:
            List of episode metrics
        """
        if not self.agent or not self.world:
            raise ValueError("Agent and world must be set for episode-based training")
        
        self.is_running = True
        
        for episode in range(num_episodes):
            if not self.is_running:
                break
            
            # Reset for new episode
            self.agent.reset()
            self.world.reset()
            
            # Run episode
            episode_metrics = self._run_episode(episode)
            self.metrics_history.append(episode_metrics)
            
            logger.info(f"Episode {episode + 1}/{num_episodes} completed. "
                       f"Reward: {episode_metrics['total_reward']:.2f}")
        
        self.is_running = False
        return self.metrics_history
    
    def _run_episode(self, episode: int) -> Dict[str, Any]:
        """Run a single episode.
        
        Args:
            episode: Episode number
            
        Returns:
            Episode metrics
        """
        episode_metric = {
            'episode': episode,
            'steps': 0,
            'total_reward': 0.0,
            'max_reward': 0.0,
            'min_reward': float('inf'),
            'average_reward': 0.0,
            'convergence_step': None
        }
        
        for step in range(self.max_steps_per_episode):
            if not self.is_running:
                break
            
            # Get sensory input
            percept = self.world.getSensoryPercept()
            self.agent.processSensoryInput(percept)
            
            # Get action from agent
            action = self.agent.decodeMotorCommand()
            
            # Apply action to world
            self.world.applyMotorCommand(action, self.world.getSimulationTime())
            
            # Compute reward
            reward = self.world.computeReward(self.agent.getBrain().getRegions()[0].get())
            self.agent.applyRewardModulation(reward, 0.0)
            
            # Update episode metrics
            episode_metric['steps'] += 1
            episode_metric['total_reward'] += reward
            episode_metric['max_reward'] = max(episode_metric['max_reward'], reward)
            episode_metric['min_reward'] = min(episode_metric['min_reward'], reward)
        
        # Calculate average reward
        if episode_metric['steps'] > 0:
            episode_metric['average_reward'] = (
                episode_metric['total_reward'] / episode_metric['steps']
            )
        
        return episode_metric

class CuriositySystem:
    """Curiosity-driven learning system."""
    
    def __init__(self, agent=None, config: Optional[NeuralConfig] = None):
        self.agent = agent
        self.config = config or NeuralConfig()
        self.curiosity_history = []
        
    def update_curiosity(self, novelty: float, prediction_error: float) -> float:
        """Update curiosity level based on novelty and prediction error.
        
        Args:
            novelty: Novelty level (0-1)
            prediction_error: Prediction error signal
            
        Returns:
            Updated curiosity level
        """
        # Calculate curiosity based on novelty and prediction error
        curiosity = novelty * self.config.curiosity_factor + \
                   abs(prediction_error) * 0.5
        
        # Clamp to reasonable range
        curiosity = max(0.0, min(1.0, curiosity))
        
        self.curiosity_history.append({
            'novelty': novelty,
            'prediction_error': prediction_error,
            'curiosity': curiosity
        })
        
        return curiosity
    
    def explore(self) -> Any:
        """Generate exploration action based on curiosity.
        
        Returns:
            Exploration action or None
        """
        if not self.agent:
            return None
        
        # High curiosity = more exploration
        current_curiosity = self.get_current_curiosity()
        
        if current_curiosity > 0.7:
            # Very exploratory: random action
            import random
            return random.choice(list(self.agent.getBrain().getRegions()[0].get().getPopulations()))
        elif current_curiosity > 0.4:
            # Moderately exploratory: slightly biased towards new actions
            # This is a simplified implementation
            pass
        
        return None
    
    def get_current_curiosity(self) -> float:
        """Get current curiosity level."""
        if self.curiosity_history:
            return self.curiosity_history[-1]['curiosity']
        return 0.0
    
    def get_curiosity_statistics(self) -> Dict[str, float]:
        """Get curiosity statistics.
        
        Returns:
            Dictionary with curiosity statistics
        """
        if not self.curiosity_history:
            return {}
        
        curiosities = [entry['curiosity'] for entry in self.curiosity_history]
        
        return {
            'mean': sum(curiosities) / len(curiosities),
            'max': max(curiosities),
            'min': min(curiosities),
            'average': sum(curiosities) / len(curiosities)
        }

# Backward compatibility functions
if PYNLM_AVAILABLE:
    def createBrain(config) -> Any:
        """Create a brain (backward compatibility)."""
        return _pynlm.createBrain(config)
    
    def createSimpleWorld() -> Any:
        """Create a simple world (backward compatibility)."""
        return _pynlm.createSimpleWorld()
    
    def createAgentBrain(brain) -> Any:
        """Create an agent brain (backward compatibility)."""
        return _pynlm.createAgentBrain(brain)
    
    def createDefaultConfig() -> Any:
        """Create a default configuration (backward compatibility)."""
        return _pynlm.createDefaultConfig()
else:
    # Define stub functions if pynlm not available
    def createBrain(config) -> Any:
        """Create a brain (backward compatibility)."""
        raise ImportError("pynlm C++ bindings not available. "
                         "Install the NLM C++ framework or use an alternative implementation.")
    
    def createSimpleWorld() -> Any:
        """Create a simple world (backward compatibility)."""
        raise ImportError("pynlm C++ bindings not available. "
                         "Install the NLM C++ framework or use an alternative implementation.")
    
    def createAgentBrain(brain) -> Any:
        """Create an agent brain (backward compatibility)."""
        raise ImportError("pynlm C++ bindings not available. "
                         "Install the NLM C++ framework or use an alternative implementation.")
    
    def createDefaultConfig() -> Any:
        """Create a default configuration (backward compatibility)."""
        raise ImportError("pynlm C++ bindings not available. "
                         "Install the NLM C++ framework or use an alternative implementation.")

# High-level factory functions
def create_default_config(neuron_count: int = 1000, **kwargs) -> NeuralConfig:
    """Create a default configuration.
    
    Args:
        neuron_count: Number of neurons
        **kwargs: Additional configuration parameters
        
    Returns:
        NeuralConfig instance
    """
    config = NeuralConfig(neuron_count=neuron_count)
    
    # Apply additional parameters
    for key, value in kwargs.items():
        if hasattr(config, key):
            setattr(config, key, value)
        else:
            config.custom_params[key] = value
    
    return config


def create_reactive_brain(config: NeuralConfig = None) -> Any:
    """Create a reactive brain with given configuration.
    
    Args:
        config: NeuralConfig (uses default if None)
        
    Returns:
        Brain object
    """
    if config is None:
        config = create_default_config()
    
    if PYNLM_AVAILABLE:
        return _pynlm.createBrain(config)
    else:
        raise ImportError("pynlm C++ bindings not available")


def create_training_agent(brain, world) -> Any:
    """Create a training agent.
    
    Args:
        brain: Brain object
        world: World object
        
    Returns:
        AgentBrain object
    """
    if PYNLM_AVAILABLE:
        agent = _pynlm.createAgentBrain(brain)
        agent.initialize(world)
        return agent
    else:
        raise ImportError("pynlm C++ bindings not available")


def create_preset_configs() -> Dict[str, NeuralConfig]:
    """Create preset configuration presets.
    
    Returns:
        Dictionary of preset configurations
    """
    return ConfigurationManager.PRESET_CONFIGS


def analyze_neural_data(brain, processor: Optional[NeuralDataProcessor] = None) -> Dict[str, Any]:
    """Analyze neural data from brain.
    
    Args:
        brain: Brain object
        processor: NeuralDataProcessor (creates new one if None)
        
    Returns:
        Analysis results
    """
    if processor is None:
        processor = NeuralDataProcessor.from_brain(brain)
    
    return {
        'activation_patterns': processor.get_activation_patterns(),
        'metrics': processor.compute_metrics(),
        'spike_times': processor.extract_spike_times(),
        'summary': {
            'total_neurons': processor.compute_metrics().get('total_neurons', 0),
            'firing_percentage': processor.compute_metrics().get('firing_percentage', 0),
            'average_firing_rate': processor.compute_metrics().get('average_firing_rate', 0),
            'total_spikes': processor.compute_metrics().get('total_spikes', 0)
        }
    }


def convert_to_dataframe(brain, processor: Optional[NeuralDataProcessor] = None) -> Union[pd.DataFrame, None]:
    """Convert brain data to pandas DataFrame.
    
    Args:
        brain: Brain object
        processor: NeuralDataProcessor (creates new one if None)
        
    Returns:
        Pandas DataFrame or None if pandas not available
    """
    if processor is None:
        processor = NeuralDataProcessor.from_brain(brain)
    
    return processor.convert_to_dataframe()


def plot_neural_activity(brain, helper: Optional[VisualizationHelper] = None, 
                        save_path: Optional[Union[str, Path]] = None) -> Union[Any, None]:
    """Plot neural activity.
    
    Args:
        brain: Brain object
        helper: VisualizationHelper (creates new one if None)
        save_path: Path to save figure
        
    Returns:
        Matplotlib figure or None
    """
    if helper is None:
        helper = VisualizationHelper()
    
    return helper.plot_neural_activity(brain, save_path)


def train_with_experience(agent, experiences: List[Dict[str, Any]], 
                          num_epochs: int = 10) -> Dict[str, Any]:
    """Train agent with experiences.
    
    Args:
        agent: AgentBrain to train
        experiences: List of experience dictionaries
        num_epochs: Number of training epochs
        
    Returns:
        Training results
    """
    return TrainingUtilities.train_with_experience(agent, experiences, num_epochs)


def train_episode_based(agent, world, num_episodes: int, 
                       max_steps_per_episode: int = 100) -> List[Dict[str, Any]]:
    """Perform episode-based training.
    
    Args:
        agent: AgentBrain to train
        world: World to train in
        num_episodes: Number of episodes to train
        max_steps_per_episode: Maximum steps per episode
        
    Returns:
        List of episode metrics
    """
    return TrainingUtilities.episode_based_training(
        agent, world, num_episodes, max_steps_per_episode
    )


def benchmark_brain_performance(brain, time_steps: int = 1000, 
                                num_runs: int = 3) -> Dict[str, Any]:
    """Benchmark brain performance.
    
    Args:
        brain: Brain to benchmark
        time_steps: Number of time steps per run
        num_runs: Number of runs to average over
        
    Returns:
        Benchmark results
    """
    return TrainingUtilities.benchmark_brain_performance(brain, time_steps, num_runs)


def export_config(config: NeuralConfig, filepath: Union[str, Path]) -> None:
    """Export configuration to file.
    
    Args:
        config: NeuralConfig to export
        filepath: Path to export file
    """
    ConfigurationManager.save_config(config, filepath)


def import_config(filepath: Union[str, Path]) -> NeuralConfig:
    """Import configuration from file.
    
    Args:
        filepath: Path to configuration file
        
    Returns:
        NeuralConfig instance
    """
    return ConfigurationManager.load_config(filepath)


def validate_config(config: NeuralConfig) -> List[str]:
    """Validate a configuration.
    
    Args:
        config: NeuralConfig to validate
        
    Returns:
        List of error messages, empty if config is valid
    """
    return ConfigurationManager.validate_config(config)


def compare_configs(config1: NeuralConfig, config2: NeuralConfig) -> Dict[str, Any]:
    """Compare two configurations.
    
    Args:
        config1: First configuration
        config2: Second configuration
        
    Returns:
        Dictionary with differences
    """
    return ConfigurationManager.compare_configs(config1, config2)


def optimize_config(config: NeuralConfig, performance_target: float) -> NeuralConfig:
    """Optimize configuration for performance target.
    
    Args:
        config: NeuralConfig to optimize
        performance_target: Target performance metric
        
    Returns:
        Optimized NeuralConfig
    """
    return ConfigurationManager.optimize_config(config, performance_target)


@contextmanager
def resource_manager(name: str) -> ResourceTracker:
    """Context manager for resource tracking.
    
    Args:
        name: Resource name
        
    Returns:
        ResourceTracker instance
    """
    tracker = ResourceTracker(name)
    try:
        yield tracker
    finally:
        tracker.close()


# Get all resource trackers
def get_resource_trackers() -> List[ResourceTracker]:
    """Get all active resource trackers."""
    return ResourceTracker.get_all_instances()


# Clean up all resources
def cleanup_resources() -> None:
    """Clean up all resources."""
    for tracker in ResourceTracker.get_all_instances():
        tracker.close()


# Module metadata
__version__ = "2.0.0"
__author__ = "NLM Development Team"
__description__ = "Enhanced Python Interface for NLM (Neural Learning Machine)"
__all__ = [
    # Core classes
    'NeuralConfig',
    'ConfigurationManager',
    'NeuralDataProcessor',
    'VisualizationHelper',
    'TrainingUtilities',
    'TrainingLoop',
    'EpisodeBasedTrainingLoop',
    'CuriositySystem',
    
    # Factory functions
    'create_default_config',
    'create_reactive_brain',
    'create_training_agent',
    'create_preset_configs',
    
    # Analysis functions
    'analyze_neural_data',
    'convert_to_dataframe',
    'plot_neural_activity',
    'train_with_experience',
    'train_episode_based',
    'benchmark_brain_performance',
    
    # Configuration management
    'export_config',
    'import_config',
    'validate_config',
    'compare_configs',
    'optimize_config',
    
    # Resource management
    'resource_manager',
    'get_resource_trackers',
    'cleanup_resources',
    
    # Backward compatibility
    'createBrain',
    'createSimpleWorld',
    'createAgentBrain',
    'createDefaultConfig'
]

if __name__ == '__main__':
    # Example usage when module is run directly
    print("Enhanced NLM Python Interface")
    print("============================")
    print(f"Version: {__version__}")
    print(f"Author: {__author__}")
    print(f"Description: {__description__}")
    print()
    print("For more information, visit: https://github.com/nlm-project/nlm")
    print("To create an enhanced brain:")
    print("  import pynlm")
    print("  config = pynlm.create_default_config(neuron_count=5000)")
    print("  brain = pynlm.create_reactive_brain(config)")
    print("  brain.initialize()")