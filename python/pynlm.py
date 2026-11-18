"""
NLM Python Bindings - Main Module Interface

This module provides the main Python interface for the NLM (Neural Learning Machine)
bindings, including wrapper classes, validation, and error handling.
"""

import sys
from typing import Any, Dict, List, Optional, Union, cast
import warnings

# Import error handling classes
from .nlm_errors import (
    NLMError,
    ConfigurationError,
    MemoryError,
    PlasticityError,
    SimulationError,
    DevelopmentError,
    NeuromodulationError,
    NLMValidator,
    ErrorRecoveryManager,
    SafeNLMOperation,
    get_recovery_manager,
    with_safe_operation,
    handle_nlm_error,
    raise_nlm_error,
    exception_registry
)

# Import the underlying C++ module
try:
    import pynlm as _pynlm
    _cpp_available = True
except ImportError as e:
    _cpp_available = False
    _cpp_error = e

# Global error recovery manager
_recovery_manager = get_recovery_manager()

# Version and metadata
__version__ = "0.1.0"
__pynlm_version__ = "0.1.0"


class NLMConfig:
    """
    Enhanced configuration class for NLM with validation and error handling.
    
    This class provides a Python-friendly interface to the NLM configuration,
    with comprehensive validation and error handling.
    """
    
    def __init__(self, config_dict: Optional[Dict[str, Any]] = None):
        """
        Initialize NLM configuration.
        
        Args:
            config_dict: Initial configuration dictionary
            
        Raises:
            ConfigurationError: If configuration is invalid
        """
        if not _cpp_available:
            raise NLMError("NLM C++ bindings not available: " + str(_cpp_error))
        
        self._config = _pynlm.createDefaultConfig()
        self._config_dict = config_dict or {}
        
        # Validate and apply configuration
        if config_dict:
            self._apply_config(config_dict)
    
    def _apply_config(self, config_dict: Dict[str, Any]) -> None:
        """
        Apply configuration dictionary with validation.
        
        Args:
            config_dict: Configuration dictionary
            
        Raises:
            ConfigurationError: If configuration validation fails
        """
        try:
            NLMValidator.validate_config(config_dict)
        except Exception as e:
            raise ConfigurationError(
                f"Configuration validation failed: {str(e)}",
                filepath=None,
                invalid_values=config_dict
            )
        
        # Apply validated configuration
        for key, value in config_dict.items():
            try:
                self._set_value(key, value)
            except Exception as e:
                raise ConfigurationError(
                    f"Failed to set configuration key '{key}': {str(e)}",
                    invalid_values={key: value}
                )
    
    def _set_value(self, key: str, value: Any) -> None:
        """
        Set a configuration value with type checking.
        
        Args:
            key: Configuration key
            value: Configuration value
        """
        if isinstance(value, bool):
            self._config.set(key, value)
        elif isinstance(value, int):
            self._config.set(key, value)
        elif isinstance(value, float):
            self._config.set(key, value)
        elif isinstance(value, str):
            self._config.set(key, value)
        elif isinstance(value, list):
            # Convert list to ConfigValue (simplified)
            if all(isinstance(x, int) for x in value):
                self._config.set(key, value)
            elif all(isinstance(x, float) for x in value):
                self._config.set(key, value)
            elif all(isinstance(x, str) for x in value):
                self._config.set(key, value)
            else:
                raise ConfigurationError(
                    f"Unsupported list type for configuration key '{key}'"
                )
        else:
            raise ConfigurationError(
                f"Unsupported configuration type for key '{key}': {type(value)}"
            )
    
    def get(self, key: str, default: Any = None) -> Any:
        """
        Get a configuration value.
        
        Args:
            key: Configuration key
            default: Default value if key not found
            
        Returns:
            Configuration value or default
        """
        if not self._config.has(key):
            return default
        
        # Try different types
        if default is not None:
            if isinstance(default, bool):
                val = self._config.get(key)
                return val.value if val else default
            elif isinstance(default, int):
                val = self._config.get(key)
                return val.value if val else default
            elif isinstance(default, float):
                val = self._config.get(key)
                return val.value if val else default
            elif isinstance(default, str):
                val = self._config.get(key)
                return val.value if val else default
        
        # Last resort - return raw value
        return default
    
    def has(self, key: str) -> bool:
        """
        Check if configuration has a key.
        
        Args:
            key: Configuration key
            
        Returns:
            True if key exists, False otherwise
        """
        return self._config.has(key)
    
    def keys(self) -> List[str]:
        """
        Get all configuration keys.
        
        Returns:
            List of configuration keys
        """
        return self._config.getKeys()
    
    def save(self, filepath: str) -> None:
        """
        Save configuration to file.
        
        Args:
            filepath: File path to save configuration
            
        Raises:
            ConfigurationError: If save operation fails
        """
        try:
            if not self._config.saveToFile(filepath):
                raise ConfigurationError(
                    f"Failed to save configuration to {filepath}"
                )
        except Exception as e:
            raise ConfigurationError(
                f"Configuration save failed: {str(e)}",
                filepath=filepath
            )
    
    def __repr__(self) -> str:
        """String representation of configuration."""
        return f"<NLMConfig: {len(self.keys())} keys>"


class NLMBrain:
    """
    Enhanced brain class for NLM with validation and error handling.
    
    This class provides a Python-friendly interface to the NLM brain,
    with comprehensive validation, error handling, and recovery.
    """
    
    def __init__(self, config: Optional[NLMConfig] = None):
        """
        Initialize NLM brain.
        
        Args:
            config: Configuration object
            
        Raises:
            NLMError: If brain initialization fails
        """
        if not _cpp_available:
            raise NLMError("NLM C++ bindings not available: " + str(_cpp_error))
        
        self._brain = None
        self._recovery_manager = _recovery_manager
        
        try:
            cpp_config = config._config if config else _pynlm.createDefaultConfig()
            self._brain = _pynlm.createBrain(cpp_config)
        except Exception as e:
            raise NLMError(f"Brain initialization failed: {str(e)}")
    
    def initialize(self, config: Optional[NLMConfig] = None) -> None:
        """
        Initialize the brain with configuration.
        
        Args:
            config: Configuration object (optional, uses initial config if not provided)
            
        Raises:
            SimulationError: If initialization fails
        """
        if not self._brain:
            raise SimulationError("Cannot initialize uninitialized brain")
        
        try:
            if config:
                # Re-initialize with new config
                self._brain = _pynlm.createBrain(config._config)
            
            if not self._brain.initialize():
                raise SimulationError("Brain initialization failed")
        except SimulationError:
            raise
        except Exception as e:
            raise SimulationError(
                f"Brain initialization error: {str(e)}",
                step_number=0,
                component="initialize"
            )
    
    def step(self, current_step: int, current_time: Optional[float] = None) -> None:
        """
        Perform a simulation step with validation.
        
        Args:
            current_step: Current simulation step
            current_time: Current simulation time (optional)
            
        Raises:
            SimulationError: If step execution fails
        """
        if not self._brain:
            raise SimulationError("Cannot step uninitialized brain")
        
        try:
            NLMValidator.validate_simulation_step(current_step)
            
            if current_time is not None:
                self._brain.step(current_step, current_time)
            else:
                self._brain.step(current_step)
        except (SimulationError, MemoryError, PlasticityError, DevelopmentError, NeuromodulationError):
            raise
        except Exception as e:
            raise SimulationError(
                f"Simulation step {current_step} failed: {str(e)}",
                step_number=current_step,
                component="step"
            )
    
    def receive_sensory_input(self, input_data: Any) -> None:
        """
        Receive sensory input with validation.
        
        Args:
            input_data: Sensory input data
            
        Raises:
            SimulationError: If input processing fails
        """
        if not self._brain:
            raise SimulationError("Cannot receive input for uninitialized brain")
        
        try:
            # Validate input data (basic validation)
            if input_data is None:
                raise SimulationError("Sensory input cannot be None")
            
            # Cast to appropriate C++ type
            # This is a simplified version - actual implementation depends on input type
            self._brain.receiveSensoryInput(input_data)
        except SimulationError:
            raise
        except Exception as e:
            raise SimulationError(
                f"Sensory input processing failed: {str(e)}",
                component="receiveSensoryInput"
            )
    
    def produce_action(self) -> Any:
        """
        Produce motor action with validation.
        
        Returns:
            Motor action object
            
        Raises:
            SimulationError: If action production fails
        """
        if not self._brain:
            raise SimulationError("Cannot produce action for uninitialized brain")
        
        try:
            action = self._brain.produceAction()
            if action is None:
                raise SimulationError("Action production returned None")
            return action
        except SimulationError:
            raise
        except Exception as e:
            raise SimulationError(
                f"Action production failed: {str(e)}",
                component="produceAction"
            )
    
    def reset(self) -> None:
        """
        Reset brain state with validation.
        
        Raises:
            SimulationError: If reset fails
        """
        if not self._brain:
            raise SimulationError("Cannot reset uninitialized brain")
        
        try:
            self._brain.reset()
        except Exception as e:
            raise SimulationError(
                f"Brain reset failed: {str(e)}",
                component="reset"
            )
    
    def save(self, filepath: str) -> None:
        """
        Save brain state to file.
        
        Args:
            filepath: File path to save brain state
            
        Raises:
            SimulationError: If save operation fails
        """
        if not self._brain:
            raise SimulationError("Cannot save uninitialized brain")
        
        try:
            if not self._brain.save(filepath):
                raise SimulationError(f"Failed to save brain state to {filepath}")
        except SimulationError:
            raise
        except Exception as e:
            raise SimulationError(
                f"Brain save failed: {str(e)}",
                details={'filepath': filepath}
            )
    
    def load(self, filepath: str) -> None:
        """
        Load brain state from file.
        
        Args:
            filepath: File path to load brain state from
            
        Raises:
            SimulationError: If load operation fails
        """
        if not self._brain:
            raise SimulationError("Cannot load into uninitialized brain")
        
        try:
            if not self._brain.load(filepath):
                raise SimulationError(f"Failed to load brain state from {filepath}")
        except SimulationError:
            raise
        except Exception as e:
            raise SimulationError(
                f"Brain load failed: {str(e)}",
                details={'filepath': filepath}
            )
    
    def get_region_count(self) -> int:
        """
        Get the number of brain regions.
        
        Returns:
            Number of regions
        """
        if not self._brain:
            return 0
        return self._brain.getRegionCount()
    
    def get_neuron_count(self) -> int:
        """
        Get the total number of neurons.
        
        Returns:
            Total neuron count
        """
        if not self._brain:
            return 0
        return self._brain.getTotalNeuronCount()
    
    def get_synapse_count(self) -> int:
        """
        Get the total number of synapses.
        
        Returns:
            Total synapse count
        """
        if not self._brain:
            return 0
        return self._brain.getTotalSynapseCount()
    
    def get_average_firing_rate(self) -> float:
        """
        Get the average firing rate.
        
        Returns:
            Average firing rate
        """
        if not self._brain:
            return 0.0
        return self._brain.getAverageFiringRate()
    
    def get_developmental_stage(self) -> str:
        """
        Get the current developmental stage.
        
        Returns:
            Current developmental stage
        """
        if not self._brain:
            return "unknown"
        return str(self._brain.getDevelopmentalStage())
    
    def set_developmental_stage(self, stage: str) -> None:
        """
        Set the developmental stage.
        
        Args:
            stage: Developmental stage
            
        Raises:
            DevelopmentError: If stage validation fails
        """
        if not self._brain:
            raise DevelopmentError("Cannot set stage for uninitialized brain")
        
        try:
            NLMValidator.validate_developmental_stage("unknown", stage)
            self._brain.setDevelopmentalStage(stage)
        except DevelopmentError:
            raise
        except Exception as e:
            raise DevelopmentError(
                f"Failed to set developmental stage: {str(e)}",
                current_stage="unknown",
                developmental_stage=stage
            )
    
    def __repr__(self) -> str:
        """String representation of brain."""
        if not self._brain:
            return "<NLMBrain: uninitialized>"
        return f"<NLMBrain: {self.get_neuron_count()} neurons, {self.get_region_count()} regions>"


class NLMWorld:
    """
    Enhanced world class for NLM with validation and error handling.
    
    This class provides a Python-friendly interface to the NLM world,
    with comprehensive validation and error handling.
    """
    
    def __init__(self):
        """
        Initialize NLM world.
        
        Raises:
            NLMError: If world initialization fails
        """
        if not _cpp_available:
            raise NLMError("NLM C++ bindings not available: " + str(_cpp_error))
        
        self._world = None
        
        try:
            self._world = _pynlm.createSimpleWorld()
        except Exception as e:
            raise NLMError(f"World initialization failed: {str(e)}")
    
    def configure(self, width: int, height: int, vision_width: int, 
                  vision_height: int) -> None:
        """
        Configure the world dimensions.
        
        Args:
            width: World width
            height: World height
            vision_width: Vision width
            vision_height: Vision height
            
        Raises:
            SimulationError: If configuration fails
        """
        if not self._world:
            raise SimulationError("Cannot configure uninitialized world")
        
        try:
            NLMValidator.validate_memory_bounds(width * height, 1000000, "world_grid")
            self._world.configure(width, height, vision_width, vision_height)
        except SimulationError:
            raise
        except Exception as e:
            raise SimulationError(
                f"World configuration failed: {str(e)}",
                component="configure"
            )
    
    def reset(self) -> None:
        """
        Reset world state.
        
        Raises:
            SimulationError: If reset fails
        """
        if not self._world:
            raise SimulationError("Cannot reset uninitialized world")
        
        try:
            self._world.reset()
        except Exception as e:
            raise SimulationError(
                f"World reset failed: {str(e)}",
                component="reset"
            )
    
    def update(self, timestep: float) -> None:
        """
        Update world simulation.
        
        Args:
            timestep: Time step for update
            
        Raises:
            SimulationError: If update fails
        """
        if not self._world:
            raise SimulationError("Cannot update uninitialized world")
        
        try:
            NLMValidator.validate_simulation_step(int(timestep * 1000), 1000000)
            self._world.update(timestep)
        except SimulationError:
            raise
        except Exception as e:
            raise SimulationError(
                f"World update failed: {str(e)}",
                component="update",
                simulation_time=timestep
            )
    
    def get_width(self) -> int:
        """Get world width."""
        if not self._world:
            return 0
        return self._world.getWidth()
    
    def get_height(self) -> int:
        """Get world height."""
        if not self._world:
            return 0
        return self._world.getHeight()
    
    def get_max_energy(self) -> float:
        """Get maximum world energy."""
        if not self._world:
            return 0.0
        return self._world.getMaxEnergy()
    
    def get_energy_decay_rate(self) -> float:
        """Get world energy decay rate."""
        if not self._world:
            return 0.0
        return self._world.getEnergyDecayRate()
    
    def set_random_seed(self, seed: int) -> None:
        """
        Set random seed.
        
        Args:
            seed: Random seed
            
        Raises:
            SimulationError: If seed setting fails
        """
        if not self._world:
            raise SimulationError("Cannot set seed for uninitialized world")
        
        try:
            NLMValidator.validate_memory_bounds(seed, 2**31 - 1, "random_seed")
            self._world.setRandomSeed(seed)
        except SimulationError:
            raise
        except Exception as e:
            raise SimulationError(
                f"Failed to set random seed: {str(e)}",
                component="setRandomSeed"
            )
    
    def __repr__(self) -> str:
        """String representation of world."""
        if not self._world:
            return "<NLMWorld: uninitialized>"
        return f"<NLMWorld: {self.get_width()}x{self.get_height()}>"


class NLM:
    """
    Main NLM interface class providing Python-friendly access to all NLM components.
    
    This is the main entry point for NLM users, providing a unified interface
    to configuration, brain, world, and error handling.
    """
    
    def __init__(self, config: Optional[Dict[str, Any]] = None):
        """
        Initialize NLM system.
        
        Args:
            config: Initial configuration dictionary
            
        Raises:
            NLMError: If initialization fails
        """
        self.config = NLMConfig(config) if config else NLMConfig()
        self.brain = NLMBrain(self.config)
        self.world = NLMWorld()
        
        # Initialize world if configured
        if self.config.has('world_width') and self.config.has('world_height'):
            self.world.configure(
                self.config.get('world_width', 100),
                self.config.get('world_height', 100),
                self.config.get('vision_width', 20),
                self.config.get('vision_height', 20)
            )
        
        # Initialize brain
        self.brain.initialize(self.config)
    
    def run_simulation(self, num_steps: int) -> None:
        """
        Run simulation for specified number of steps.
        
        Args:
            num_steps: Number of simulation steps
            
        Raises:
            SimulationError: If simulation fails
        """
        NLMValidator.validate_simulation_step(num_steps)
        
        for step in range(num_steps):
            try:
                self.brain.step(step)
                self.world.update(1.0)  # Assuming 1.0 time step
            except SimulationError:
                raise
            except Exception as e:
                raise SimulationError(
                    f"Simulation step {step} failed during run: {str(e)}",
                    step_number=step
                )
    
    def get_status(self) -> Dict[str, Any]:
        """
        Get system status and statistics.
        
        Returns:
            Status dictionary
        """
        status = {
            'brain': {
                'initialized': self.brain._brain is not None,
                'neuron_count': self.brain.get_neuron_count(),
                'synapse_count': self.brain.get_synapse_count(),
                'region_count': self.brain.get_region_count(),
                'average_firing_rate': self.brain.get_average_firing_rate(),
                'developmental_stage': self.brain.get_developmental_stage()
            },
            'world': {
                'initialized': self.world._world is not None,
                'width': self.world.get_width(),
                'height': self.world.get_height(),
                'max_energy': self.world.get_max_energy(),
                'energy_decay_rate': self.world.get_energy_decay_rate()
            },
            'config': {
                'keys': self.config.keys()
            }
        }
        
        return status
    
    def __repr__(self) -> str:
        """String representation of NLM system."""
        return f"<NLM: {self.get_status()}>"


# Convenience functions and shortcuts

def create_default_config() -> NLMConfig:
    """
    Create a default configuration.
    
    Returns:
        NLMConfig object
    """
    return NLMConfig()


def create_brain(config: Optional[NLMConfig] = None) -> NLMBrain:
    """
    Create a brain with configuration.
    
    Args:
        config: Configuration object
        
    Returns:
        NLMBrain object
    """
    return NLMBrain(config)


def create_world() -> NLMWorld:
    """
    Create a world.
    
    Returns:
        NLMWorld object
    """
    return NLMWorld()


def run_nlm_simulation(config_dict: Optional[Dict[str, Any]] = None, 
                       num_steps: int = 100) -> NLM:
    """
    Run a complete NLM simulation.
    
    Args:
        config_dict: Configuration dictionary
        num_steps: Number of simulation steps
        
    Returns:
        NLM object
        
    Raises:
        NLMError: If simulation fails
    """
    try:
        nlm = NLM(config_dict)
        nlm.run_simulation(num_steps)
        return nlm
    except Exception as e:
        raise NLMError(f"Simulation failed: {str(e)}")


# Error handling utilities

def get_error_recovery_manager() -> ErrorRecoveryManager:
    """Get the global error recovery manager."""
    return _recovery_manager


def safe_execute(func, *args, **kwargs):
    """
    Execute a function safely with error handling.
    
    Args:
        func: Function to execute
        *args: Function arguments
        **kwargs: Function keyword arguments
        
    Returns:
        Function result
        
    Raises:
        NLMError: If function execution fails
    """
    try:
        return func(*args, **kwargs)
    except NLMError:
        raise
    except Exception as e:
        raise_nlm_error(
            'UNKNOWN_ERROR',
            f"Safe execution failed for {func.__name__}: {str(e)}",
            details={'function': func.__name__}
        )


def wrap_with_error_handling(func):
    """
    Decorator to wrap functions with error handling.
    
    Args:
        func: Function to wrap
        
    Returns:
        Wrapped function
    """
    def wrapper(*args, **kwargs):
        return handle_nlm_error(func)(*args, **kwargs)
    return wrapper


# Backward compatibility

# Create alias for the main class
Pynlm = NLM

# Export key classes and functions
__all__ = [
    # Exception classes
    'NLMError',
    'ConfigurationError',
    'MemoryError',
    'PlasticityError',
    'SimulationError',
    'DevelopmentError',
    'NeuromodulationError',
    
    # Main classes
    'NLMConfig',
    'NLMBrain',
    'NLMWorld',
    'NLM',
    'Pynlm',
    
    # Validation
    'NLMValidator',
    
    # Error handling
    'ErrorRecoveryManager',
    'SafeNLMOperation',
    'get_recovery_manager',
    'with_safe_operation',
    'handle_nlm_error',
    'raise_nlm_error',
    
    # Utilities
    'create_default_config',
    'create_brain',
    'create_world',
    'run_nlm_simulation',
    'get_error_recovery_manager',
    'safe_execute',
    'wrap_with_error_handling',
    
    # Constants
    'exception_registry',
    
    # Version
    '__version__',
    '__pynlm_version__'
]