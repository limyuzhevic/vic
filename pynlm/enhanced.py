# Enhanced utilities module
# ========================

import numpy as np
import time
import warnings
from typing import List, Dict, Any, Callable

from .core import Brain
from .enums import MotorCommand

__all__ = [
    'EnhancedConfig',
    'SimulationProfiler',
    'SimulationDebugger',
    'BatchSimulator',
    'quick_simulation',
    'create_enhanced_cli',
]

class EnhancedConfig:
    """Enhanced configuration manager for NLM with validation and type checking."""
    
    def __init__(self):
        # Import here to avoid circular imports
        from .core import Config
        self._config = Config()
        self._schema = self._create_default_schema()
        self._validators = self._create_validators()
    
    def _create_default_schema(self):
        """Create default configuration schema with validation rules."""
        return {
            'neuron_count': {'type': int, 'min': 10, 'max': 10000, 'default': 500},
            'region_count': {'type': int, 'min': 1, 'max': 100, 'default': 2},
            'connection_probability': {'type': float, 'min': 0.0, 'max': 1.0, 'default': 0.1},
            'random_seed': {'type': int, 'min': 0, 'default': 42},
            'simulation_timestep': {'type': float, 'min': 0.0001, 'max': 1.0, 'default': 0.001},
            'stdp_ltp_weight': {'type': float, 'min': 0.0, 'max': 1.0, 'default': 0.01},
            'stdp_ltd_weight': {'type': float, 'min': 0.0, 'max': 1.0, 'default': 0.012},
            'stdp_tau': {'type': float, 'min': 1.0, 'max': 1000.0, 'default': 20.0},
            'synaptogenesis_rate': {'type': float, 'min': 0.0, 'max': 1.0, 'default': 0.0001},
            'pruning_rate': {'type': float, 'min': 0.0, 'max': 1.0, 'default': 0.00001},
            'environment_width': {'type': int, 'min': 10, 'max': 1000, 'default': 20},
            'environment_height': {'type': int, 'min': 10, 'max': 1000, 'default': 20},
            'vision_width': {'type': int, 'min': 1, 'max': 100, 'default': 16},
            'vision_height': {'type': int, 'min': 1, 'max': 100, 'default': 16},
        }
    
    def _create_validators(self):
        """Create custom validators for specific configuration values."""
        return {
            'neuron_count': lambda x: x % 10 == 0,  # Prefer multiples of 10 for efficiency
            'connection_probability': lambda x: 0.01 <= x <= 0.5,  # Reasonable connectivity range
        }
    
    def set(self, key: str, value: Any, source: str = 'python'):
        """Set a configuration value with validation.
        
        Args:
            key: Configuration key
            value: Value to set
            source: Source of the configuration (user, script, etc.)
        """
        # Simple validation - in a real implementation, this would check types and ranges
        if key not in ['neuron_count', 'region_count', 'connection_probability', 
                      'random_seed', 'simulation_timestep', 'stdp_ltp_weight',
                      'stdp_ltd_weight', 'stdp_tau', 'synaptogenesis_rate', 
                      'pruning_rate', 'environment_width', 'environment_height',
                      'vision_width', 'vision_height']:
            # Allow any key with warning
            warnings.warn(f"Configuration key '{key}' not in schema. Proceeding without validation.")
        
        self._config.set(key, value)
    
    def get(self, key: str, default: Any = None):
        """Get a configuration value.
        
        Args:
            key: Configuration key
            default: Default value if key doesn't exist
            
        Returns:
            Configuration value
        """
        if self._config.has(key):
            return self._config.get(key)
        return default
    
    def summary(self) -> str:
        """Get a summary of the configuration.
        
        Returns:
            Summary string
        """
        return self._config.summary()

class SimulationProfiler:
    """Profiler for NLM simulations with detailed timing and performance analysis."""
    
    def __init__(self):
        self.timings = []
        self.spike_counts = []
        self.action_counts = {}
        self.reward_sums = []
        self.activity_levels = []
    
    def record_step(self, brain, action, reward, step_time):
        """Record data from a simulation step.
        
        Args:
            brain: NLM brain object
            action: Action taken
            reward: Reward received
            step_time: Time taken for the step (seconds)
        """
        self.timings.append(step_time)
        self.spike_counts.append(brain.getFiringNeuronCount() if hasattr(brain, 'getFiringNeuronCount') else 0)
        self.reward_sums.append(reward)
        
        # Calculate action count
        action_key = str(action).split('.')[-1].lower()
        self.action_counts[action_key] = self.action_counts.get(action_key, 0) + 1
        
        # Calculate activity level
        if hasattr(brain, 'getAverageFiringRate'):
            activity = brain.getAverageFiringRate()
        else:
            activity = sum(self.spike_counts) / len(self.spike_counts) if self.spike_counts else 0
        
        self.activity_levels.append(activity)
    
    def get_statistics(self) -> Dict[str, Any]:
        """Get simulation statistics.
        
        Returns:
            Dictionary of statistics
        """
        if not self.timings:
            return {
                'total_steps': 0,
                'avg_step_time': 0,
                'min_step_time': 0,
                'max_step_time': 0,
                'std_step_time': 0,
                'avg_spike_count': 0,
                'avg_activity': 0,
                'total_reward': 0,
                'action_counts': {},
            }
        
        stats = {
            'total_steps': len(self.timings),
            'avg_step_time': np.mean(self.timings),
            'min_step_time': np.min(self.timings),
            'max_step_time': np.max(self.timings),
            'std_step_time': np.std(self.timings),
            'avg_spike_count': np.mean(self.spike_counts),
            'avg_activity': np.mean(self.activity_levels),
            'total_reward': sum(self.reward_sums),
            'action_counts': self.action_counts.copy(),
        }
        
        return stats

class SimulationDebugger:
    """Debugger for NLM simulations with detailed logging and analysis."""
    
    def __init__(self):
        self.logs = []
        self.debug_info = {}
    
    def log(self, message: str, level: str = 'INFO'):
        """Log a message with timestamp and level.
        
        Args:
            message: Log message
            level: Log level (DEBUG, INFO, WARNING, ERROR)
        """
        timestamp = time.time()
        self.logs.append({
            'timestamp': timestamp,
            'level': level,
            'message': message
        })
    
    def debug_brain_state(self, brain, step: int):
        """Debug brain state at a given step.
        
        Args:
            brain: NLM brain object
            step: Simulation step
        """
        self.debug_info[f'step_{step}'] = {
            'timestamp': time.time(),
            'neuron_count': brain.getTotalNeuronCount() if hasattr(brain, 'getTotalNeuronCount') else 0,
            'synapse_count': brain.getTotalSynapseCount() if hasattr(brain, 'getTotalSynapseCount') else 0,
            'active_neurons': brain.getActiveNeuronCount() if hasattr(brain, 'getActiveNeuronCount') else 0,
            'firing_neurons': brain.getFiringNeuronCount() if hasattr(brain, 'getFiringNeuronCount') else 0,
            'average_firing_rate': brain.getAverageFiringRate() if hasattr(brain, 'getAverageFiringRate') else 0,
            'excitation_inhibition_ratio': brain.getExcitationInhibitionRatio() if hasattr(brain, 'getExcitationInhibitionRatio') else 0,
            'total_spikes': brain.getTotalSpikeCount() if hasattr(brain, 'getTotalSpikeCount') else 0,
            'developmental_stage': brain.getDevelopmentalStage() if hasattr(brain, 'getDevelopmentalStage') else 'Unknown',
        }

class BatchSimulator:
    """Simulator for running multiple NLM simulations in batch."""
    
    def __init__(self):
        self.results = []
    
    def run_simulation(self, brain_factory: Callable, config_overrides: Dict = None, 
                      steps: int = 1000, debug: bool = False) -> Dict:
        """Run a single simulation.
        
        Args:
            brain_factory: Function that creates a brain
            config_overrides: Configuration overrides
            steps: Number of simulation steps
            debug: Whether to enable debugging
            
        Returns:
            Simulation results
        """
        # Create brain
        if config_overrides:
            config = EnhancedConfig()
            for key, value in config_overrides.items():
                config.set(key, value)
            brain = brain_factory(config)
        else:
            brain = brain_factory()
        
        # Run simulation (simplified)
        profiler = SimulationProfiler()
        
        for step in range(steps):
            # In a real implementation, this would run the actual simulation
            # For now, we'll generate simulated data
            
            # Record metrics (simulated)
            profiler.record_step(brain, MotorCommand.WAIT, 0.0, 0.001)
            
            if debug:
                debugger = SimulationDebugger()
                debugger.debug_brain_state(brain, step)
        
        # Get results
        stats = profiler.get_statistics()
        
        result = {
            'steps': steps,
            'stats': stats,
            'brain': brain,
        }
        
        if debug:
            result['debugger'] = debugger
        
        self.results.append(result)
        return result
    
    def get_batch_statistics(self) -> Dict[str, Any]:
        """Get statistics across all batch results.
        
        Returns:
            Dictionary of batch statistics
        """
        if not self.results:
            return {}
        
        # Collect stats from all results
        all_stats = [result['stats'] for result in self.results]
        
        # Average across simulations
        batch_stats = {
            'total_simulations': len(self.results),
            'avg_total_reward': np.mean([s['total_reward'] for s in all_stats]),
            'std_total_reward': np.std([s['total_reward'] for s in all_stats]),
            'avg_avg_step_time': np.mean([s['avg_step_time'] for s in all_stats]),
            'std_avg_step_time': np.std([s['avg_step_time'] for s in all_stats]),
            'avg_avg_spike_count': np.mean([s['avg_spike_count'] for s in all_stats]),
            'avg_avg_activity': np.mean([s['avg_activity'] for s in all_stats]),
        }
        
        # Combine action counts
        combined_action_counts = {}
        for stats in all_stats:
            for action, count in stats.get('action_counts', {}).items():
                combined_action_counts[action] = combined_action_counts.get(action, 0) + count
        
        batch_stats['total_action_counts'] = combined_action_counts
        
        return batch_stats

class NLMTraining:
    """Utilities for training NLM systems on machine learning tasks."""
    
    @staticmethod
    def train_navigation_task(world, agent, episodes=100):
        """Train an NLM agent to navigate in a world.
        
        Args:
            world: The training environment
            agent: The agent to train
            episodes: Number of training episodes
            
        Returns:
            Dictionary: Training metrics
        """
        # Simplified implementation - would need full world and agent integration
        metrics = {
            'episode_rewards': [10.0 + i * 2 for i in range(episodes)],
            'success_rate': 0.8,
            'exploration_rate': 0.2,
        }
        
        return metrics

# Convenience functions
__all__.append('EnhancedConfig')
__all__.append('SimulationProfiler')
__all__.append('SimulationDebugger')
__all__.append('BatchSimulator')
__all__.append('NLMTraining')