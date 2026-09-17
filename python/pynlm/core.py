"""
Core classes for pynlm.

Implements the main Brain and Config classes with full type hints,
Pythonic APIs, and comprehensive error handling.
"""

import time
import json
import pickle
from typing import Any, Dict, List, Optional, Union, Iterator, Callable
from contextlib import contextmanager
from dataclasses import dataclass, field
from enum import Enum

from .exceptions import (
    ConfigurationError, SimulationError, MemoryError, 
    ValidationError, raise_config_error, raise_simulation_error
)


class SimulationStage(Enum):
    """Brain development stages."""
    INITIAL = "initial"
    CRITICAL_PERIOD = "critical_period"
    MATURATION = "maturation"
    ADULT = "adult"
    AGING = "aging"


@dataclass
class NeuronMetrics:
    """Metrics for neuron activity."""
    firing_rate: float = 0.0
    spike_count: int = 0
    membrane_potential: float = 0.0
    refractory_until: float = 0.0
    age: float = 0.0


@dataclass
class Config:
    """Enhanced configuration management for NLM.
    
    Provides Pythonic configuration API with dot notation access,
    validation, and type safety.
    
    Examples:
        >>> config = Config({"brain.neuron_count": 1000, "plasticity.stdp.enable": True})
        >>> config.brain.neuron_count
        1000
        >>> config["plasticity.stdp.enable"]
        True
    """
    
    # Default configuration values
    brain_neuron_count: int = 1000
    brain_region_count: int = 10
    brain_connection_count: int = 10000
    brain_global_connectivity: float = 0.1
    brain_noise_level: float = 0.01
    
    # Learning system settings
    plasticity_stdp_enable: bool = True
    plasticity_stdp_learning_rate: float = 0.01
    plasticity_hebbian_enable: bool = True
    plasticity_homeostatic_enable: bool = True
    
    # Neuromodulation settings
    neuromod_dopamine_scale: float = 1.0
    neuromod_acetylcholine_scale: float = 0.5
    neuromod_serotonin_scale: float = 0.3
    neuromod_gaba_scale: float = 0.7
    neuromod_norepinephrine_scale: float = 0.6
    
    # Development settings
    development_critical_period: float = 1.0
    development_maturation_rate: float = 0.1
    development_aging_rate: float = 0.01
    
    # Agent settings
    agent_reward_modulation_enable: bool = True
    agent_structural_plasticity_enable: bool = True
    agent_development_enable: bool = True
    agent_curiosity_enable: bool = True
    
    # Simulation settings
    simulation_steps: int = 1000
    simulation_timestep: float = 0.1
    simulation_save_interval: int = 100
    
    # Performance monitoring
    profiler_enable: bool = True
    profiler_log_interval: int = 100
    
    # Default values
    _default_values: Dict[str, Any] = field(default_factory=dict)
    
    def __post_init__(self):
        """Initialize default values after dataclass init."""
        self._default_values = self.__dict__.copy()
    
    def __getitem__(self, key: str) -> Any:
        """Get configuration value using dot notation or key path."""
        if "." in key:
            # Handle nested key paths like "brain.neuron_count"
            parts = key.split(".")
            value = self
            for part in parts:
                if hasattr(value, part):
                    value = getattr(value, part)
                else:
                    # If not found as attribute, try _default_values
                    if key in self._default_values:
                        return self._default_values[key]
                    raise KeyError(f"Configuration key '{key}' not found")
            return value
        else:
            # Direct attribute access
            if hasattr(self, key):
                return getattr(self, key)
            raise KeyError(f"Configuration key '{key}' not found")
    
    def __setitem__(self, key: str, value: Any) -> None:
        """Set configuration value using dot notation or key path."""
        if "." in key:
            parts = key.split(".")
            parent = self
            for part in parts[:-1]:
                if hasattr(parent, part):
                    parent = getattr(parent, part)
                else:
                    # Try to create nested structure
                    setattr(parent, part, Config())
                    parent = getattr(parent, part)
            setattr(parent, parts[-1], value)
        else:
            if hasattr(self, key):
                setattr(self, key, value)
            else:
                self._default_values[key] = value
    
    def get(self, key: str, default: Any = None) -> Any:
        """Get configuration value with default."""
        try:
            return self[key]
        except KeyError:
            return default
    
    def has(self, key: str) -> bool:
        """Check if configuration key exists."""
        try:
            self[key]
            return True
        except KeyError:
            return False
    
    def get_keys(self) -> List[str]:
        """Get all configuration keys."""
        keys = []
        for attr_name in dir(self):
            if not attr_name.startswith("_") and not attr_name == "_default_values":
                try:
                    keys.append(attr_name)
                except:
                    pass
        return keys
    
    def summary(self) -> str:
        """Get configuration summary."""
        lines = ["Configuration Summary:"]
        for key in sorted(self.get_keys()):
            try:
                value = self[key]
                lines.append(f"  {key}: {value}")
            except:
                pass
        return "\n".join(lines)
    
    def save_to_file(self, filepath: str) -> None:
        """Save configuration to file."""
        with open(filepath, 'w') as f:
            json.dump(self.__dict__, f, indent=2)
    
    def load_from_file(self, filepath: str) -> bool:
        """Load configuration from file."""
        try:
            with open(filepath, 'r') as f:
                data = json.load(f)
            # Update configuration from loaded data
            for key, value in data.items():
                if hasattr(self, key):
                    setattr(self, key, value)
            return True
        except Exception as e:
            raise_config_error(f"Failed to load configuration: {str(e)}", key="filepath")
    
    def clear(self) -> None:
        """Reset configuration to defaults."""
        for key, value in self._default_values.items():
            setattr(self, key, value)
    
    def validate(self) -> List[str]:
        """Validate configuration and return list of errors."""
        errors = []
        
        # Validate numeric ranges
        if self.brain_neuron_count <= 0:
            errors.append("brain_neuron_count must be positive")
        if self.brain_region_count <= 0:
            errors.append("brain_region_count must be positive")
        if self.brain_connection_count < 0:
            errors.append("brain_connection_count cannot be negative")
        if not 0 <= self.brain_global_connectivity <= 1:
            errors.append("brain_global_connectivity must be between 0 and 1")
        if not 0 <= self.brain_noise_level <= 1:
            errors.append("brain_noise_level must be between 0 and 1")
        
        # Validate boolean flags
        for field_name in dir(self):
            if field_name.startswith("brain_") and field_name.endswith("_enable"):
                if not isinstance(getattr(self, field_name), bool):
                    errors.append(f"{field_name} must be boolean")
        
        # Validate scale factors
        for field_name in dir(self):
            if field_name.startswith("plasticity_") and field_name.endswith("_scale"):
                value = getattr(self, field_name)
                if not 0 <= value <= 1:
                    errors.append(f"{field_name} must be between 0 and 1")
            elif field_name.startswith("neuromod_") and field_name.endswith("_scale"):
                value = getattr(self, field_name)
                if not 0 <= value <= 2:
                    errors.append(f"{field_name} must be between 0 and 2")
        
        return errors


def createDefaultConfig() -> Config:
    """Create a default configuration instance."""
    return Config()


class Brain:
    """Central neural simulation brain class with Pythonic API.
    
    Provides the core neural simulation functionality with type hints,
    error handling, performance monitoring, and context manager support.
    
    Examples:
        >>> config = createDefaultConfig()
        >>> brain = Brain(config)
        >>> brain.initialize()
        >>> brain.step(0)
        >>> brain.run_batch(1000, save_interval=100)
    """
    
    def __init__(self, config: Config):
        """Initialize brain with configuration.
        
        Args:
            config: Configuration instance
            
        Raises:
            ConfigurationError: If configuration is invalid
        """
        if not isinstance(config, Config):
            raise ConfigurationError("Config must be a Config instance", key="type")
        
        self._config = config
        self._validate_config()
        
        # Brain state
        self._step_count = 0
        self._neuron_count = config.brain_neuron_count
        self._region_count = config.brain_region_count
        self._synapse_count = config.brain_connection_count
        
        # Performance tracking
        self._spike_count = 0
        self._active_neuron_count = 0
        self._start_time = time.time()
        
        # Development
        self._development_stage = SimulationStage.INITIAL
        self._development_time = 0.0
        
        # Error handling
        self._error_count = 0
        self._last_error = None
    
    def _validate_config(self) -> None:
        """Validate configuration and raise ConfigurationError if invalid."""
        errors = self._config.validate()
        if errors:
            raise ConfigurationError(
                f"Invalid configuration: {'; '.join(errors)}",
                key="validation_errors"
            )
    
    def initialize(self) -> None:
        """Initialize the brain with configuration."""
        try:
            self._step_count = 0
            self._spike_count = 0
            self._active_neuron_count = 0
            self._start_time = time.time()
            self._development_stage = SimulationStage.INITIAL
            self._development_time = 0.0
            self._error_count = 0
            self._last_error = None
            
            # Start development process
            self._development_time = 0.0
            
        except Exception as e:
            raise_simulation_error(f"Failed to initialize brain: {str(e)}")
    
    def step(self, step_number: int, current_time: Optional[float] = None) -> Dict[str, Any]:
        """Perform a simulation step.
        
        Args:
            step_number: Current step number
            current_time: Current simulation time (defaults to step_number * timestep)
            
        Returns:
            Dictionary with step statistics
            
        Example:
            >>> stats = brain.step(0)
            >>> print(stats['spike_count'])
        """
        if current_time is None:
            current_time = step_number * self._config.simulation_timestep
        
        try:
            # Update development stage
            self._update_development(current_time)
            
            # Simulate brain activity
            self._spike_count += self._neuron_count // 100  # Simplified spike calculation
            self._active_neuron_count = min(self._neuron_count, self._active_neuron_count + 10)
            
            # Update metrics
            self._step_count = step_number
            
            return self._get_step_stats(step_number, current_time)
            
        except Exception as e:
            self._error_count += 1
            self._last_error = str(e)
            raise_simulation_error(
                f"Error during step {step_number}: {str(e)}",
                step=step_number,
                component="step"
            )
    
    def _update_development(self, current_time: float) -> None:
        """Update brain development stage based on time."""
        self._development_time = current_time
        
        if self._development_time < self._config.development_critical_period:
            self._development_stage = SimulationStage.CRITICAL_PERIOD
        elif self._development_time < self._config.development_critical_period + 100:
            self._development_stage = SimulationStage.MATURATION
        elif self._development_time < 1000:
            self._development_stage = SimulationStage.ADULT
        else:
            self._development_stage = SimulationStage.AGING
    
    def _get_step_stats(self, step: int, current_time: float) -> Dict[str, Any]:
        """Get statistics for current step."""
        return {
            "step": step,
            "time": current_time,
            "spike_count": self._spike_count,
            "active_neuron_count": self._active_neuron_count,
            "firing_rate": self._spike_count / (step + 1) if step >= 0 else 0,
            "development_stage": self._development_stage.value,
            "error_count": self._error_count,
        }
    
    def run_batch(self, steps: int = 1000, save_interval: Optional[int] = None) -> Dict[str, Any]:
        """Run a batch of brain steps efficiently.
        
        Args:
            steps: Number of steps to simulate
            save_interval: Save brain state every this many steps (None = never)
            
        Returns:
            Dictionary with batch execution statistics
            
        Example:
            >>> stats = brain.run_batch(steps=1000, save_interval=100)
            >>> print(f"Executed {stats['steps']} steps in {stats['elapsed_time']:.2f}s")
        """
        if save_interval is None:
            save_interval = self._config.simulation_save_interval
        
        try:
            start_time = time.time()
            stats = {
                "steps": steps,
                "start_time": start_time,
                "elapsed_time": 0.0,
                "steps_per_second": 0.0,
                "spike_count": self._spike_count,
                "average_firing_rate": 0.0,
            }
            
            for i in range(steps):
                result = self.step(i)
                if save_interval and (i + 1) % save_interval == 0:
                    self.save(f"brain_step_{i+1}.pkl")
            
            end_time = time.time()
            stats["end_time"] = end_time
            stats["elapsed_time"] = end_time - start_time
            stats["steps_per_second"] = steps / stats["elapsed_time"] if stats["elapsed_time"] > 0 else 0
            stats["spike_count"] = self._spike_count
            stats["average_firing_rate"] = self._spike_count / steps if steps > 0 else 0
            
            return stats
            
        except Exception as e:
            raise_simulation_error(f"Error during batch execution: {str(e)}", component="batch")
    
    def save(self, filepath: str) -> bool:
        """Save brain state to file.
        
        Args:
            filepath: Path to save file
            
        Returns:
            True if successful, False otherwise
            
        Example:
            >>> brain.save("brain_state.pkl")
        """
        try:
            with open(filepath, 'wb') as f:
                pickle.dump({
                    'config': self._config.__dict__,
                    'step_count': self._step_count,
                    'spike_count': self._spike_count,
                    'active_neuron_count': self._active_neuron_count,
                    'development_stage': self._development_stage,
                    'development_time': self._development_time,
                    'start_time': self._start_time,
                    'error_count': self._error_count,
                    'last_error': self._last_error,
                }, f)
            return True
        except Exception as e:
            raise_simulation_error(f"Failed to save brain: {str(e)}", component="save")
    
    def load(self, filepath: str) -> bool:
        """Load brain state from file.
        
        Args:
            filepath: Path to load file
            
        Returns:
            True if successful, False otherwise
            
        Example:
            >>> brain.load("brain_state.pkl")
        """
        try:
            with open(filepath, 'rb') as f:
                data = pickle.load(f)
            
            # Restore brain state
            for key, value in data.items():
                if hasattr(self, key):
                    setattr(self, key, value)
            
            return True
        except Exception as e:
            raise_simulation_error(f"Failed to load brain: {str(e)}", component="load")
    
    # Getter methods
    def getTotalNeuronCount(self) -> int:
        """Get total neuron count across all regions."""
        return self._neuron_count
    
    def getTotalSynapseCount(self) -> int:
        """Get total synapse count across all regions."""
        return self._synapse_count
    
    def getActiveNeuronCount(self) -> int:
        """Get count of active neurons."""
        return self._active_neuron_count
    
    def getFiringNeuronCount(self) -> int:
        """Get count of currently firing neurons."""
        # Simplified calculation based on firing rate
        return min(self._neuron_count, self._spike_count // 10)
    
    def getAverageFiringRate(self) -> float:
        """Get average firing rate across all neurons."""
        if self._step_count <= 0:
            return 0.0
        return self._spike_count / self._step_count
    
    def getTotalSpikeCount(self) -> int:
        """Get total spike count."""
        return self._spike_count
    
    def getExcitationInhibitionRatio(self) -> float:
        """Get excitation/inhibition balance ratio."""
        # Simplified calculation
        return 1.0
    
    def getDevelopmentalStage(self) -> str:
        """Get current developmental stage."""
        return self._development_stage.value
    
    def setDevelopmentalStage(self, stage: Union[str, SimulationStage]) -> None:
        """Set developmental stage."""
        if isinstance(stage, str):
            try:
                self._development_stage = SimulationStage(stage)
            except ValueError:
                raise ValidationError(f"Invalid developmental stage: {stage}", 
                                    field="stage", value=stage)
        else:
            self._development_stage = stage
    
    def getConfig(self) -> Config:
        """Get the configuration."""
        return self._config
    
    def getStepCount(self) -> int:
        """Get current step count."""
        return self._step_count
    
    def getSimulationTime(self) -> float:
        """Get current simulation time."""
        return self._step_count * self._config.simulation_timestep
    
    def reset(self) -> None:
        """Reset brain state."""
        self.initialize()
    
    def logStatus(self) -> str:
        """Get brain status as string."""
        return f"Brain at step {self._step_count} (stage: {self._development_stage.value})"
    
    # Context manager support
    def __enter__(self):
        """Context manager entry."""
        self.initialize()
        return self
    
    def __exit__(self, exc_type, exc_val, exc_tb):
        """Context manager exit - cleanup."""
        self.reset()
    
    def __repr__(self) -> str:
        """String representation."""
        return f"<Brain step={self._step_count}, neurons={self._neuron_count}>"
