# Utility functions for NLM
import numpy as np
from typing import Dict, List, Any, Callable
from enum import Enum

from .brain import Brain
from .world import SimpleWorld
from .agent import Agent
from .config import Config, createDefaultConfig
from .enums import DevelopmentalStage
import time

# Progress tracking for long simulations
class SimulationProgress:
    """Track progress of long-running simulations."""
    
    def __init__(self, total_steps: int, update_interval: int = 100):
        """Initialize progress tracker."""
        self.total_steps = total_steps
        self.update_interval = update_interval
        self.current_step = 0
        self.start_time = time.time()
        self.last_update = 0.0
    
    def update(self, step: int) -> None:
        """Update progress."""
        self.current_step = step
        self._update_display()
    
    def _update_display(self) -> None:
        """Update progress display."""
        current_time = time.time()
        if current_time - self.last_update >= 5.0:  # Update every 5 seconds
            progress = (self.current_step / self.total_steps) * 100
            elapsed = current_time - self.start_time
            if self.current_step > 0:
                steps_per_second = self.current_step / elapsed
                remaining = (self.total_steps - self.current_step) / max(steps_per_second, 0.001)
                print(f"Progress: {progress:.1f}% ({self.current_step}/{self.total_steps}) "
                      f"- {steps_per_second:.1f} steps/sec - ETA: {remaining:.1f}s")
            else:
                print(f"Progress: {progress:.1f}% ({self.current_step}/{self.total_steps})")
            self.last_update = current_time
    
    def finish(self) -> None:
        """Mark simulation as finished."""
        self._update_display()
        print("Simulation completed!")

# Configuration helpers
class ConfigPresets:
    """Predefined configuration presets for common scenarios."""
    
    @staticmethod
    def get_minimal_config() -> Config:
        """Get minimal configuration for quick testing."""
        config = createDefaultConfig()
        config.set("brain.neuron_count", 100)
        config.set("brain.synapse_density", 0.05)
        config.set("plasticity.stdp.enable", False)
        config.set("plasticity.hebbian.enable", False)
        return config
    
    @staticmethod
    def get_research_config() -> Config:
        """Get configuration optimized for research."""
        config = createDefaultConfig()
        config.set("brain.neuron_count", 5000)
        config.set("brain.synapse_density", 0.1)
        config.set("plasticity.stdp.enable", True)
        config.set("plasticity.hebbian.enable", True)
        config.set("plasticity.structural.enable", True)
        config.set("neuromod.curiosity.enable", True)
        config.set("neuromod.novelty.enable", True)
        config.set("development.synaptogenesis_rate", 0.01)
        config.set("development.pruning_rate", 0.001)
        return config
    
    @staticmethod
    def get_behavioral_config() -> Config:
        """Get configuration optimized for behavioral experiments."""
        config = createDefaultConfig()
        config.set("brain.neuron_count", 1000)
        config.set("brain.synapse_density", 0.08)
        config.set("plasticity.stdp.enable", True)
        config.set("plasticity.hebbian.enable", True)
        config.set("plasticity.structural.enable", False)
        config.set("neuromod.curiosity.enable", True)
        config.set("neuromod.novelty.enable", False)
        config.set("neuromod.reward.scale", 1.0)
        config.set("reward.discount_factor", 0.95)
        return config

# Validation utilities
class ValidationUtils:
    """Utilities for validating NLM configurations and data."""
    
    @staticmethod
    def validate_config(config: Config) -> List[str]:
        """Validate configuration and return list of warnings."""
        warnings = []
        
        # Check neuron count
        neuron_count = config.getOr[int64_t]("brain.neuron_count", 0)
        if neuron_count <= 0:
            warnings.append("Neuron count must be positive")
        elif neuron_count > 100000:
            warnings.append("Neuron count very large may cause performance issues")
        
        # Check synapse density
        synapse_density = config.getOr[float]("brain.synapse_density", 0.0)
        if synapse_density < 0 or synapse_density > 1:
            warnings.append("Synapse density should be between 0 and 1")
        
        # Check simulation timestep
        timestep = config.getOr[float]("simulation_timestep", 0.0)
        if timestep <= 0 or timestep > 0.1:
            warnings.append("Simulation timestep should be positive and <= 0.1s")
        
        # Check plasticity parameters
        if config.getOr[bool]("plasticity.stdp.enable", False):
            learning_rate = config.getOr[float]("plasticity.stdp.learning_rate", 0.0)
            if learning_rate < 0 or learning_rate > 1:
                warnings.append("STDP learning rate should be between 0 and 1")
        
        # Check memory constraints
        if config.getOr[bool]("neuromod.curiosity.enable", False):
            if config.getOr[float]("neuromod.dopamine.scale", 0.0) <= 0:
                warnings.append("Curiosity with non-positive dopamine scale may not work well")
        
        return warnings
    
    @staticmethod
    def validate_sensory_data(data: np.ndarray, expected_shape: tuple) -> bool:
        """Validate sensory data dimensions."""
        try:
            if data.shape != expected_shape:
                raise ValueError(f"Data shape {data.shape} doesn't match expected {expected_shape}")
            if np.any(np.isnan(data)):
                raise ValueError("Data contains NaN values")
            if np.any(np.isinf(data)):
                raise ValueError("Data contains infinite values")
            if np.any(data < 0):
                raise ValueError("Data contains negative values")
            return True
        except Exception as e:
            print(f"Validation error: {e}")
            return False
    
    @staticmethod
    def validate_action(action: Any) -> bool:
        """Validate action object."""
        try:
            if not hasattr(action, 'getType'):
                raise ValueError("Action must have getType() method")
            if not hasattr(action, 'getParameters'):
                raise ValueError("Action must have getParameters() method")
            return True
        except Exception as e:
            print(f"Action validation error: {e}")
            return False

# Performance monitoring
class PerformanceMonitor:
    """Monitor simulation performance metrics."""
    
    def __init__(self):
        """Initialize performance monitor."""
        self.metrics = {
            'simulation_time': [],
            'steps_per_second': [],
            'memory_usage': [],
            'cpu_usage': [],
        }
        self.start_time = time.time()
        self.step_count = 0
    
    def record_step(self) -> None:
        """Record a simulation step."""
        self.step_count += 1
    
    def get_performance_stats(self) -> Dict[str, Any]:
        """Get performance statistics."""
        if not self.metrics['steps_per_second']:
            return {}
        
        return {
            'avg_steps_per_second': np.mean(self.metrics['steps_per_second']),
            'std_steps_per_second': np.std(self.metrics['steps_per_second']),
            'min_steps_per_second': np.min(self.metrics['steps_per_second']),
            'max_steps_per_second': np.max(self.metrics['steps_per_second']),
            'avg_simulation_time': np.mean(self.metrics['simulation_time']),
        }
    
    def save_stats(self, filepath: str) -> bool:
        """Save performance stats to file."""
        try:
            stats = self.get_performance_stats()
            with open(filepath, 'w') as f:
                import json
                json.dump(stats, f, indent=2)
            return True
        except Exception as e:
            print(f"Error saving performance stats: {e}")
            return False

# Data utilities
class DataUtils:
    """Utilities for data manipulation and analysis."""
    
    @staticmethod
    def smooth_data(data: List[float], window_size: int = 5) -> List[float]:
        """Apply moving average smoothing to data."""
        if len(data) < window_size:
            return data.copy()
        
        smoothed = []
        for i in range(len(data)):
            start = max(0, i - window_size // 2)
            end = min(len(data), i + window_size // 2 + 1)
            smoothed.append(np.mean(data[start:end]))
        
        return smoothed
    
    @staticmethod
    def normalize_data(data: np.ndarray, min_val: float = 0.0, max_val: float = 1.0) -> np.ndarray:
        """Normalize data to specified range."""
        data_min = np.min(data)
        data_max = np.max(data)
        
        if data_max - data_min < 1e-10:
            return np.full_like(data, (min_val + max_val) / 2)
        
        normalized = (data - data_min) / (data_max - data_min)
        return normalized * (max_val - min_val) + min_val
    
    @staticmethod
    def find_peaks(data: np.ndarray, threshold: float = 0.5) -> List[int]:
        """Find peaks in data."""
        peaks = []
        for i in range(1, len(data) - 1):
            if data[i] > data[i-1] and data[i] > data[i+1] and data[i] > threshold:
                peaks.append(i)
        return peaks
    
    @staticmethod
    def calculate_derivative(data: np.ndarray, dt: float = 1.0) -> np.ndarray:
        """Calculate derivative of data."""
        return np.gradient(data, dt)
    
    @staticmethod
    def interpolate_missing_data(data: np.ndarray) -> np.ndarray:
        """Interpolate missing values (NaN or Inf) in data."""
        # Create copy to avoid modifying original
        result = data.copy()
        
        # Find NaN and Inf values
        nan_mask = np.isnan(result)
        inf_mask = np.isinf(result)
        
        if not (np.any(nan_mask) or np.any(inf_mask)):
            return result
        
        # Linear interpolation for missing values
        for i in range(len(result)):
            if nan_mask[i] or inf_mask[i]:
                # Find previous and next valid values
                prev_valid = None
                next_valid = None
                
                for j in range(i - 1, -1, -1):
                    if not (nan_mask[j] or inf_mask[j]):
                        prev_valid = (j, result[j])
                        break
                
                for j in range(i + 1, len(result)):
                    if not (nan_mask[j] or inf_mask[j]):
                        next_valid = (j, result[j])
                        break
                
                # Interpolate if both sides have valid values
                if prev_valid is not None and next_valid is not None:
                    t_prev, v_prev = prev_valid
                    t_next, v_next = next_valid
                    t_current = i
                    
                    # Linear interpolation
                    if t_next != t_prev:
                        weight = (t_current - t_prev) / (t_next - t_prev)
                        result[i] = v_prev + weight * (v_next - v_prev)
                    else:
                        result[i] = v_prev
                elif prev_valid is not None:
                    # Only previous valid value
                    result[i] = prev_valid[1]
                elif next_valid is not None:
                    # Only next valid value
                    result[i] = next_valid[1]
        
        return result

# Randomization utilities
class RandomizationUtils:
    """Utilities for reproducible randomization."""
    
    @staticmethod
    def set_seed(seed: int) -> None:
        """Set random seed for reproducibility."""
        np.random.seed(seed)
    
    @staticmethod
    def get_random_config(config: Config, key: str) -> Any:
        """Get configuration value with randomness for exploration."""
        if not config.has(key):
            return None
        
        value = config.get(key)
        
        # For numeric parameters, apply small random perturbation
        if isinstance(value, (int, float)):
            perturbation = np.random.uniform(-0.1, 0.1) * abs(value)
            return value + perturbation
        elif isinstance(value, bool):
            # Flip boolean with 10% probability
            if np.random.random() < 0.1:
                return not value
        
        return value

# Export/import utilities
class IOUtils:
    """Utilities for file I/O operations."""
    
    @staticmethod
    def save_simulation_results(brain: Brain, world: SimpleWorld, agent: Agent, 
                              filepath: str, metadata: Optional[Dict[str, Any]] = None) -> bool:
        """Save simulation results to file."""
        try:
            import json
            import pickle
            
            results = {
                'metadata': metadata or {},
                'brain_stats': {
                    'neuron_count': brain.getTotalNeuronCount(),
                    'synapse_count': brain.getTotalSynapseCount(),
                    'firing_neuron_count': brain.getFiringNeuronCount(),
                    'avg_firing_rate': brain.getAverageFiringRate(),
                    'total_spikes': brain.getTotalSpikeCount(),
                    'e_i_ratio': brain.getExcitationInhibitionRatio(),
                    'developmental_stage': brain.getDevelopmentalStage().name,
                },
                'agent_stats': agent.getStats(),
                'world_stats': {
                    'width': world.getWidth(),
                    'height': world.getHeight(),
                    'simulation_time': world.getSimulationTime(),
                    'agent_energy': agent.body.energy if hasattr(agent, 'body') else 0,
                    'agent_health': agent.body.health if hasattr(agent, 'body') else 0,
                },
                'timestamp': time.time(),
            }
            
            # Save as JSON
            with open(filepath + '.json', 'w') as f:
                json.dump(results, f, indent=2, default=str)
            
            # Optionally save as pickle for Python objects
            with open(filepath + '.pkl', 'wb') as f:
                pickle.dump(results, f)
            
            return True
        except Exception as e:
            print(f"Error saving simulation results: {e}")
            return False
    
    @staticmethod
    def load_simulation_results(filepath: str) -> Optional[Dict[str, Any]]:
        """Load simulation results from file."""
        try:
            # Try JSON first
            with open(filepath + '.json', 'r') as f:
                import json
                return json.load(f)
        except FileNotFoundError:
            try:
                # Try pickle
                with open(filepath + '.pkl', 'rb') as f:
                    import pickle
                    return pickle.load(f)
            except FileNotFoundError:
                print(f"No simulation results found at {filepath}")
                return None
        except Exception as e:
            print(f"Error loading simulation results: {e}")
            return None

# Exception classes
class NLMRuntimeError(Exception):
    """Base exception for NLM runtime errors."""
    pass

class NLMConfigurationError(NLMRuntimeError):
    """Exception for configuration-related errors."""
    pass

class NLMSimulationError(NLMRuntimeError):
    """Exception for simulation-related errors."""
    pass

class NLMValidationError(NLMRuntimeError):
    """Exception for validation-related errors."""
    pass

# Main utility functions

def create_brain_from_preset(preset_name: str) -> Brain:
    """Create a brain using a predefined preset."""
    presets = ConfigPresets()
    
    preset_map = {
        'minimal': presets.get_minimal_config(),
        'research': presets.get_research_config(),
        'behavioral': presets.get_behavioral_config(),
    }
    
    if preset_name not in preset_map:
        raise ValueError(f"Unknown preset: {preset_name}")
    
    config = preset_map[preset_name]
    brain = Brain(config)
    brain.initialize()
    return brain

def validate_simulation_params(brain_config: Config, world_config: Dict[str, Any]) -> List[str]:
    """Validate simulation parameters and return warnings."""
    validator = ValidationUtils()
    warnings = validator.validate_config(brain_config)
    
    # Validate world config
    if 'width' in world_config:
        if world_config['width'] <= 0:
            warnings.append("World width must be positive")
    
    if 'height' in world_config:
        if world_config['height'] <= 0:
            warnings.append("World height must be positive")
    
    return warnings

def format_simulation_time(seconds: float) -> str:
    """Format simulation time in human-readable format."""
    if seconds < 60:
        return f"{seconds:.2f}s"
    elif seconds < 3600:
        minutes = seconds / 60
        return f"{minutes:.1f}m"
    else:
        hours = seconds / 3600
        return f"{hours:.1f}h"

def generate_simulation_report(brain: Brain, world: SimpleWorld, agent: Agent, 
                              steps_completed: int, start_time: float) -> str:
    """Generate a comprehensive simulation report."""
    end_time = time.time()
    duration = end_time - start_time
    
    report_lines = [
        "NLM Simulation Report",
        "=" * 50,
        f"Simulation Time: {format_simulation_time(duration)}",
        f"Steps Completed: {steps_completed}",
        f"Steps per Second: {steps_completed / max(duration, 0.001):.2f}",
        "",
        "Brain Statistics:",
        f"  Total Neurons: {brain.getTotalNeuronCount()}",
        f"  Total Synapses: {brain.getTotalSynapseCount()}",
        f"  Firing Neurons: {brain.getFiringNeuronCount()}",
        f"  Average Firing Rate: {brain.getAverageFiringRate():.2f} Hz",
        f"  Total Spikes: {brain.getTotalSpikeCount()}",
        f"  E/I Balance: {brain.getExcitationInhibitionRatio():.3f}",
        f"  Developmental Stage: {brain.getDevelopmentalStage().name}",
        "",
        "Agent Statistics:",
    ]
    
    # Add agent stats if available
    if hasattr(agent, 'getStats'):
        stats = agent.getStats()
        for key, value in stats.items():
            if isinstance(value, float):
                report_lines.append(f"  {key}: {value:.3f}")
            else:
                report_lines.append(f"  {key}: {value}")
    
    report_lines.extend([
        "",
        "World Statistics:",
        f"  World Size: {world.getWidth()} x {world.getHeight()}",
        f"  Simulation Time: {world.getSimulationTime():.2f}s",
        f"  Agent Energy: {agent.body.energy:.1f} / {world.getMaxEnergy():.1f}",
        f"  Agent Health: {agent.body.health:.1f}%",
        f"  Agent Age: {agent.body.age:.2f}s",
        "",
        "Performance Rating:",
        "  " + agent.get_performance_rating(brain, world) if hasattr(agent, 'get_performance_rating') else "  N/A",
    ])
    
    return "\n".join(report_lines)

def get_performance_rating(brain: Brain, world: SimpleWorld) -> str:
    """Get performance rating based on brain and world state."""
    # Calculate composite score
    neuron_count = brain.getTotalNeuronCount()
    firing_rate = brain.getAverageFiringRate()
    spike_count = brain.getTotalSpikeCount()
    
    # Normalized metrics (higher is better)
    neuron_score = min(neuron_count / 1000, 1.0)  # Cap at 1000 neurons
    firing_score = min(firing_rate / 10, 1.0)    # Cap at 10 Hz
    spike_score = min(spike_count / 1000, 1.0)   # Cap at 1000 spikes
    
    composite_score = (neuron_score + firing_score + spike_score) / 3
    
    if composite_score > 0.8:
        return "Excellent"
    elif composite_score > 0.6:
        return "Good"
    elif composite_score > 0.4:
        return "Fair"
    else:
        return "Needs Improvement"
