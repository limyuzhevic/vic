#!/usr/bin/env python3

"""
NLM Brain Convenience API - High-level Python interface for NLM brain simulation

This module provides a user-friendly Python API for working with the NLM brain,
abstracting away some of the complexity of the low-level C++ interface.

Features:
- High-level factory methods for common brain configurations
- Simplified simulation workflows
- Convenience functions for typical use cases
- Enhanced error handling and validation
- Example usage patterns
"""

import sys
import time
import json
import math
from typing import List, Optional, Dict, Any, Callable
from dataclasses import dataclass
from enum import Enum

try:
    import nlm
    _NLM_AVAILABLE = True
except ImportError:
    _NLM_AVAILABLE = False
    print("Warning: nlm Python bindings not available. Some features will be limited.")

class SimulationMode(Enum):
    """Simulation modes for different use cases"""
    BASIC = "basic"
    TRAINING = "training"
    EXPLORATION = "exploration"
    REINFORCEMENT = "reinforcement"
    DEVELOPMENT = "development"

class BrainConfiguration:
    """Convenience class for brain configuration presets"""
    
    @staticmethod
    def create_default_config() -> Dict[str, Any]:
        """Create default brain configuration"""
        return {
            "neuron_count": 1000,
            "region_count": 2,
            "connection_probability": 0.1,
            "simulation_timestep": 0.001,
            "random_seed": 42,
            "stdp_ltp_weight": 0.01,
            "stdp_ltd_weight": 0.012,
            "stdp_tau": 20.0,
            "synaptogenesis_rate": 0.0001,
            "pruning_rate": 0.00001,
        }
    
    @staticmethod
    def create_training_config() -> Dict[str, Any]:
        """Create configuration optimized for training scenarios"""
        base = BrainConfiguration.create_default_config()
        base.update({
            "neuron_count": 2000,
            "region_count": 3,
            "connection_probability": 0.15,
            "stdp_ltp_weight": 0.02,
            "stdp_ltd_weight": 0.018,
            "replay_interval": 100,
            "consolidation_interval": 1000,
        })
        return base
    
    @staticmethod
    def create_exploration_config() -> Dict[str, Any]:
        """Create configuration optimized for exploration scenarios"""
        base = BrainConfiguration.create_default_config()
        base.update({
            "neuron_count": 500,
            "region_count": 1,
            "connection_probability": 0.05,
            "curiosity_enabled": True,
            "novelty_threshold": 0.5,
            "exploration_rate": 0.3,
        })
        return base
    
    @staticmethod
    def create_development_config() -> Dict[str, Any]:
        """Create configuration for developmental scenarios"""
        base = BrainConfiguration.create_default_config()
        base.update({
            "neuron_count": 1500,
            "region_count": 4,
            "connection_probability": 0.12,
            "developmental_stages": 5,
            "critical_period_length": 1000,
            "plasticity_decay_rate": 0.01,
        })
        return base
    
    @staticmethod
    def create_small_config() -> Dict[str, Any]:
        """Create small, fast configuration for quick testing"""
        return {
            "neuron_count": 100,
            "region_count": 1,
            "connection_probability": 0.2,
            "simulation_timestep": 0.002,
            "random_seed": 123,
        }
    
    @staticmethod
    def create_large_config() -> Dict[str, Any]:
        """Create large-scale configuration for complex scenarios"""
        return {
            "neuron_count": 10000,
            "region_count": 8,
            "connection_probability": 0.08,
            "simulation_timestep": 0.0005,
            "random_seed": 456,
            "stdp_ltp_weight": 0.005,
            "stdp_ltd_weight": 0.006,
            "replay_interval": 500,
            "consolidation_interval": 5000,
        }
class NLMBrain:
    """High-level interface for NLM brain simulation with convenience methods"""
    
    def __init__(self, config_file: Optional[str] = None, 
                 config_dict: Optional[Dict[str, Any]] = None,
                 mode: SimulationMode = SimulationMode.BASIC):
        """Initialize NLM brain with convenience configuration"""
        if not _NLM_AVAILABLE:
            raise RuntimeError("nlm Python bindings not available")
        
        self.mode = mode
        self.config_file = config_file
        self.config_dict = config_dict
        self.brain = None
        self.is_initialized = False
        self.training_data = []
        self.episode_count = 0
        
        # Load or create configuration
        if config_file:
            self._load_config(config_file)
        elif config_dict:
            self._set_config(config_dict)
        else:
            # Use default config based on mode
            self._set_config(BrainConfiguration.create_default_config())
        
        # Create brain
        self._create_brain()
    
    def _load_config(self, config_file: str):
        """Load configuration from file"""
        if not _NLM_AVAILABLE:
            return
        
        try:
            self.config = nlm.createDefaultConfig()
            if self.config.loadFromFile(config_file):
                print(f"Configuration loaded from {config_file}")
            else:
                print(f"Warning: Failed to load config from {config_file}, using defaults")
                self._set_config(BrainConfiguration.create_default_config())
        except Exception as e:
            print(f"Error loading config: {e}, using defaults")
            self._set_config(BrainConfiguration.create_default_config())
    
    def _set_config(self, config_dict: Dict[str, Any]):
        """Set configuration from dictionary"""
        self.config = nlm.createDefaultConfig()
        
        # Apply configuration based on type
        for key, value in config_dict.items():
            if isinstance(value, str):
                self.config.set(key, value, nlm.ConfigSource.Runtime)
            elif isinstance(value, bool):
                self.config.set(key, value, nlm.ConfigSource.Runtime)
            elif isinstance(value, int):
                self.config.set(key, value, nlm.ConfigSource.Runtime)
            elif isinstance(value, float):
                self.config.set(key, value, nlm.ConfigSource.Runtime)
            elif isinstance(value, list):
                # Handle list values (e.g., vector configurations)
                if all(isinstance(x, str) for x in value):
                    self.config.set(key, value, nlm.ConfigSource.Runtime)
                elif all(isinstance(x, int) for x in value):
                    self.config.set(key, value, nlm.ConfigSource.Runtime)
                elif all(isinstance(x, float) for x in value):
                    self.config.set(key, value, nlm.ConfigSource.Runtime)
        
        # Set mode-specific configuration
        self._apply_mode_config()
    
    def _apply_mode_config(self):
        """Apply mode-specific configuration options"""
        mode_configs = {
            SimulationMode.TRAINING: {
                "enable_structural_plasticity": True,
                "enable_reward_modulation": True,
                "enable_development": True,
                "enable_curiosity": True,
            },
            SimulationMode.EXPLORATION: {
                "enable_structural_plasticity": False,
                "enable_reward_modulation": False,
                "enable_development": False,
                "enable_curiosity": True,
                "novelty_threshold": 0.5,
                "exploration_rate": 0.3,
            },
            SimulationMode.REINFORCEMENT: {
                "enable_structural_plasticity": True,
                "enable_reward_modulation": True,
                "enable_development": False,
                "enable_curiosity": False,
                "reward_prediction_window": 50,
            },
            SimulationMode.DEVELOPMENT: {
                "enable_structural_plasticity": True,
                "enable_reward_modulation": False,
                "enable_development": True,
                "enable_curiosity": False,
                "developmental_stages": 5,
            },
        }
        
        for key, value in mode_configs.get(self.mode, {}).items():
            if isinstance(value, bool):
                self.config.set(key, value, nlm.ConfigSource.Runtime)
            elif isinstance(value, (int, float)):
                self.config.set(key, value, nlm.ConfigSource.Runtime)
    
    def _create_brain(self):
        """Create brain from configuration"""
        if not _NLM_AVAILABLE:
            return
        
        try:
            self.brain = nlm.createBrain(self.config)
            print("NLM brain created successfully")
        except Exception as e:
            raise RuntimeError(f"Failed to create brain: {e}")
    
    def initialize(self) -> bool:
        """Initialize the brain with current configuration"""
        if not self.brain:
            raise RuntimeError("Brain not created")
        
        try:
            success = self.brain.initialize()
            self.is_initialized = success
            
            if success:
                print("Brain initialized successfully")
                self._print_config_summary()
            else:
                print("Failed to initialize brain")
            
            return success
        except Exception as e:
            print(f"Error initializing brain: {e}")
            return False
    
    def _print_config_summary(self):
        """Print configuration summary"""
        if not self.brain:
            return
        
        print(f"  Neurons: {self.brain.getTotalNeuronCount()}")
        print(f"  Regions: {self.brain.getRegionCount()}")
        print(f"  Synapses: {self.brain.getTotalSynapseCount()}")
        print(f"  Initial spikes: {self.brain.getTotalSpikeCount()}")
    
    def run_step(self, step: Optional[int] = None) -> Dict[str, Any]:
        """Run a single simulation step"""
        if not self.is_initialized:
            raise RuntimeError("Brain not initialized. Call initialize() first.")
        
        if step is None:
            step = self.get_current_step()
        
        # Record pre-step metrics
        pre_spikes = self.brain.getTotalSpikeCount()
        pre_firing = self.brain.getFiringNeuronCount()
        pre_rate = self.brain.getAverageFiringRate()
        
        # Run step
        self.brain.step(step)
        
        # Record post-step metrics
        post_spikes = self.brain.getTotalSpikeCount()
        post_firing = self.brain.getFiringNeuronCount()
        post_rate = self.brain.getAverageFiringRate()
        
        # Calculate changes
        spike_delta = post_spikes - pre_spikes
        firing_delta = post_firing - pre_firing
        rate_delta = post_rate - pre_rate
        
        # Return metrics
        return {
            "step": step,
            "spike_delta": spike_delta,
            "firing_delta": firing_delta,
            "rate_delta": rate_delta,
            "total_spikes": post_spikes,
            "firing_neurons": post_firing,
            "average_rate": post_rate,
        }
    
    def run_episode(self, max_steps: int, mode: Optional[SimulationMode] = None) -> Dict[str, Any]:
        """Run a complete episode of simulation"""
        if not self.is_initialized:
            raise RuntimeError("Brain not initialized.")
        
        # Use mode-specific max_steps if not provided
        if mode is None:
            mode = self.mode
        
        episode_steps = max_steps
        
        # Mode-specific configuration
        if mode == SimulationMode.EXPLORATION:
            episode_steps = min(max_steps, 500)
        elif mode == SimulationMode.TRAINING:
            episode_steps = min(max_steps, 2000)
        
        print(f"Running episode: {mode.value} ({episode_steps} steps)")
        
        episode_metrics = {
            "mode": mode.value,
            "steps_run": 0,
            "total_spikes": 0,
            "total_reward": 0.0,
            "max_firing": 0,
            "avg_firing_rate": 0.0,
            "start_time": time.time(),
        }
        
        for step in range(episode_steps):
            metrics = self.run_step(step)
            
            episode_metrics["steps_run"] += 1
            episode_metrics["total_spikes"] += metrics["spike_delta"]
            episode_metrics["max_firing"] = max(episode_metrics["max_firing"], metrics["firing_neurons"])
            episode_metrics["avg_firing_rate"] += metrics["average_rate"]
            
            # Mode-specific behavior
            if mode == SimulationMode.EXPLORATION:
                self._apply_exploration_behavior(step)
            elif mode == SimulationMode.TRAINING:
                self._apply_training_behavior(metrics, step)
        
        # Calculate averages
        if episode_metrics["steps_run"] > 0:
            episode_metrics["avg_firing_rate"] /= episode_metrics["steps_run"]
        
        episode_metrics["end_time"] = time.time()
        episode_metrics["duration"] = episode_metrics["end_time"] - episode_metrics["start_time"]
        
        # Store episode data
        self.episode_count += 1
        episode_data = {
            "episode": self.episode_count,
            "mode": mode.value,
            "metrics": episode_metrics,
        }
        self.training_data.append(episode_data)
        
        return episode_metrics
    
    def _apply_exploration_behavior(self, step: int):
        """Apply exploration-specific behavior"""
        # Simple curiosity-driven behavior
        if step % 100 == 0:
            # Occasionally inject curiosity into sensory neurons
            curiosity_level = 0.5 * (1.0 - step / 1000.0)  # Decrease over time
            if curiosity_level > 0.1:
                self.brain.injectCurrentToNeurons(nlm.NeuronType.Sensory, curiosity_level * 10.0)
    
    def _apply_training_behavior(self, metrics: Dict[str, Any], step: int):
        """Apply training-specific behavior"""
        # Simple reward-based training
        if metrics["spike_delta"] > 10:  # High activity = positive signal
            # Apply reward modulation
            reward = metrics["spike_delta"] * 0.01
            predicted = metrics["average_rate"] * 0.5
            self.brain.applyRewardModulation(reward, predicted)
    
    def save_episode_data(self, filepath: str):
        """Save episode data to file"""
        try:
            with open(filepath, 'w') as f:
                json.dump({
                    "config": self.config_dict if self.config_dict else {},
                    "episodes": self.training_data,
                    "episode_count": self.episode_count,
                    "mode": self.mode.value,
                }, f, indent=2)
            print(f"Episode data saved to {filepath}")
        except Exception as e:
            print(f"Error saving episode data: {e}")
    
    def load_episode_data(self, filepath: str):
        """Load episode data from file"""
        try:
            with open(filepath, 'r') as f:
                data = json.load(f)
            
            self.training_data = data.get("episodes", [])
            self.episode_count = data.get("episode_count", 0)
            self.mode = SimulationMode(data.get("mode", "basic"))
            
            if "config" in data:
                self.config_dict = data["config"]
                self._set_config(self.config_dict)
            
            print(f"Episode data loaded from {filepath}")
        except Exception as e:
            print(f"Error loading episode data: {e}")
    
    # Getters and status methods
    
    def get_current_step(self) -> int:
        """Get current simulation step"""
        if self.brain:
            return self.brain.getTotalSpikeCount()
        return 0
    
    def get_neuron_count(self) -> int:
        """Get number of neurons"""
        if self.brain:
            return self.brain.getTotalNeuronCount()
        return 0
    
    def get_region_count(self) -> int:
        """Get number of regions"""
        if self.brain:
            return self.brain.getRegionCount()
        return 0
    
    def get_synapse_count(self) -> int:
        """Get number of synapses"""
        if self.brain:
            return self.brain.getTotalSynapseCount()
        return 0
    
    def get_total_spikes(self) -> int:
        """Get total spike count"""
        if self.brain:
            return self.brain.getTotalSpikeCount()
        return 0
    
    def get_firing_neurons(self) -> int:
        """Get number of currently firing neurons"""
        if self.brain:
            return self.brain.getFiringNeuronCount()
        return 0
    
    def get_average_firing_rate(self) -> float:
        """Get average firing rate"""
        if self.brain:
            return self.brain.getAverageFiringRate()
        return 0.0
    
    def get_excitation_inhibition_ratio(self) -> float:
        """Get excitation/inhibition ratio"""
        if self.brain:
            return self.brain.getExcitationInhibitionRatio()
        return 0.0
    
    def is_config_valid(self) -> bool:
        """Check if configuration is valid"""
        if not self.config:
            return False
        
        # Basic validation
        required_keys = ["neuron_count", "region_count", "connection_probability"]
        for key in required_keys:
            if not self.config.has(key):
                return False
        
        return True
    
    def get_config_summary(self) -> Dict[str, Any]:
        """Get configuration summary"""
        if not self.config:
            return {}
        
        return {
            "mode": self.mode.value,
            "config_file": self.config_file,
            "has_config_dict": self.config_dict is not None,
            "is_valid": self.is_config_valid(),
            "neuron_count": self.get_neuron_count(),
            "region_count": self.get_region_count(),
            "synapse_count": self.get_synapse_count(),
        }
    
    def run_benchmark(self, num_steps: int = 1000) -> Dict[str, Any]:
        """Run benchmark and return performance metrics"""
        if not self.is_initialized:
            raise RuntimeError("Brain not initialized.")
        
        start_time = time.time()
        start_memory = self._get_memory_usage()
        
        for step in range(num_steps):
            self.run_step(step)
        
        end_time = time.time()
        end_memory = self._get_memory_usage()
        
        duration = end_time - start_time
        memory_used = max(start_memory, end_memory) - min(start_memory, end_memory)
        
        return {
            "steps": num_steps,
            "duration": duration,
            "steps_per_second": num_steps / duration,
            "memory_used_mb": memory_used,
            "average_step_time_ms": (duration / num_steps) * 1000,
            "mode": self.mode.value,
        }
    
    def _get_memory_usage(self) -> float:
        """Get memory usage in MB"""
        try:
            import psutil
            process = psutil.Process()
            return process.memory_info().rss / (1024**2)
        except ImportError:
            return 0.0
    
    def __str__(self) -> str:
        """String representation"""
        return f"NLMBrain(mode={self.mode.value}, neurons={self.get_neuron_count()}, initialized={self.is_initialized})"
    
    def __repr__(self) -> str:
        """Detailed string representation"""
        return self.__str__()
    
    def __enter__(self):
        """Context manager entry"""
        if not self.is_initialized:
            self.initialize()
        return self
    
    def __exit__(self, exc_type, exc_val, exc_tb):
        """Context manager exit - cleanup"""
        self.cleanup()
    
    def cleanup(self):
        """Cleanup resources"""
        if self.brain:
            # Any cleanup needed
            pass
        self.is_initialized = False
        print("NLMBrain cleaned up")
# Convenience factory functions
def create_brain_for_task(task_type: str, **kwargs) -> NLMBrain:
    """Create brain for specific task type with convenience factory"""
    
    task_configs = {
        "simple": BrainConfiguration.create_small_config(),
        "complex": BrainConfiguration.create_large_config(),
        "training": BrainConfiguration.create_training_config(),
        "exploration": BrainConfiguration.create_exploration_config(),
        "development": BrainConfiguration.create_development_config(),
        "default": BrainConfiguration.create_default_config(),
    }
    
    if task_type not in task_configs:
        raise ValueError(f"Unknown task type: {task_type}. Available: {list(task_configs.keys())}")
    
    config = task_configs[task_type].copy()
    
    # Apply user overrides
    for key, value in kwargs.items():
        config[key] = value
    
    return NLMBrain(config_dict=config)
def run_simulation_batch(configs: List[Dict[str, Any]], 
                       episodes_per_config: int = 10,
                       steps_per_episode: int = 100) -> List[Dict[str, Any]]:
    """Run simulation batch with multiple configurations"""
    
    results = []
    
    for i, config in enumerate(configs):
        print(f"Running configuration {i+1}/{len(configs)}...")
        
        mode = SimulationMode(config.get("mode", "basic"))
        brain = NLMBrain(config_dict=config, mode=mode)
        
        if brain.initialize():
            batch_results = {
                "config_index": i,
                "config": config,
                "episodes": [],
                "total_performance": 0.0,
            }
            
            for episode in range(episodes_per_config):
                episode_metrics = brain.run_episode(steps_per_episode, mode)
                batch_results["episodes"].append(episode_metrics)
            
            # Calculate average performance
            if batch_results["episodes"]:
                avg_spikes = sum(ep["total_spikes"] for ep in batch_results["episodes"]) / len(batch_results["episodes"])
                batch_results["total_performance"] = avg_spikes
            
            results.append(batch_results)
        else:
            print(f"Failed to initialize brain for configuration {i+1}")
            results.append({
                "config_index": i,
                "config": config,
                "error": "Initialization failed",
                "episodes": [],
                "total_performance": 0.0,
            })
    
    return results
def print_usage_examples():
    """Print usage examples for the convenience API"""
    
    examples = '''
=== NLM Brain Convenience API Examples ===

# Example 1: Simple brain for testing
brain = NLMBrain(config_dict=BrainConfiguration.create_small_config())
brain.initialize()
for step in range(100):
    metrics = brain.run_step(step)
print(f"Step {step}: {metrics['total_spikes']} spikes")

# Example 2: Training brain with exploration
brain = NLMBrain(config_dict=BrainConfiguration.create_training_config(), 
                 mode=SimulationMode.TRAINING)
brain.initialize()
metrics = brain.run_episode(500, SimulationMode.TRAINING)
print(f"Training episode: {metrics['total_spikes']} spikes in {metrics['steps_run']} steps")

# Example 3: Factory function for specific tasks
brain = create_brain_for_task("exploration")
brain.initialize()

# Example 4: Context manager for automatic cleanup
with NLMBrain(config_dict=BrainConfiguration.create_default_config()) as brain:
    brain.initialize()
    for step in range(100):
        brain.run_step(step)
# Brain automatically cleaned up

# Example 5: Batch simulation
configs = [
    BrainConfiguration.create_small_config(),
    BrainConfiguration.create_default_config(),
    BrainConfiguration.create_large_config(),
]
results = run_simulation_batch(configs, episodes_per_config=5, steps_per_episode=200)
for result in results:
    print(f"Config {result['config_index']}: {result['total_performance']:.1f} avg spikes")
'''
    
    print(examples)
# Module-level convenience functions
if _NLM_AVAILABLE:
    # Make convenience functions available at module level
    __all__ = [
        'NLMBrain', 'BrainConfiguration', 'SimulationMode',
        'create_brain_for_task', 'run_simulation_batch', 
        'print_usage_examples'
    ]
else:
    __all__ = [
        'BrainConfiguration', 'SimulationMode',
        'print_usage_examples'
    ]