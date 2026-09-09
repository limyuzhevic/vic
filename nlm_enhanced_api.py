#!/usr/bin/env python3
"""
Enhanced Python API for NLM (Neural Learning Machine) Bindings

This module provides an improved, user-friendly Python interface to the NLM C++ library
with enhanced usability, batch operations, method chaining, and better error handling.

Key improvements:
- Convenience methods and fluent interface
- Batch operations for performance
- Enhanced error handling with descriptive messages
- Method chaining support
- Better documentation and type hints
"""

import sys
import traceback
from typing import List, Dict, Optional, Union, Any, Callable, TypeVar, overload
from pathlib import Path
from contextlib import contextmanager

# Try to import the underlying C++ bindings
try:
    import pynlm
    PYNLML_AVAILABLE = True
except ImportError as e:
    PYNLML_AVAILABLE = False
    print(f"Warning: pynlm module not available: {e}")

T = TypeVar('T')

class NLMError(Exception):
    """Base exception for NLM Python API errors."""
    pass

class ValidationError(NLMError):
    """Raised when input validation fails."""
    pass

class ConfigurationError(NLMError):
    """Raised when configuration operations fail."""
    pass

class MemoryError(NLMError):
    """Raised when memory-related operations fail."""
    pass

class AgentBrainError(NLMError):
    """Raised when agent brain operations fail."""
    pass

@contextmanager
def catch_nlm_error(operation_name: str):
    """Context manager for catching and enhancing NLM errors."""
    try:
        yield
    except pynlm.RuntimeError as e:
        raise NLMError(f"{operation_name} failed: {str(e)}") from e
    except pynlm.ValueError as e:
        raise ValidationError(f"{operation_name} validation error: {str(e)}") from e
    except pynlm.IndexError as e:
        raise ValidationError(f"{operation_name} index error: {str(e)}") from e
    except Exception as e:
        raise NLMError(f"{operation_name} unexpected error: {str(e)}") from e

class NLMConfig:
    """
    Enhanced configuration class with improved API usability.
    
    Provides a fluent interface and batch operations for configuration management.
    """
    
    def __init__(self):
        if not PYNLML_AVAILABLE:
            raise NLMError("pynlm module is not available")
        self._config = pynlm.createDefaultConfig()
    
    def __repr__(self):
        return f"<NLMConfig: {self._config.summary()}>"
    
    # ========== Configuration Setters (Fluent Interface) ==========
    
    def set_neuron_count(self, count: int) -> 'NLMConfig':
        """Set neuron count and return self for method chaining."""
        with catch_nlm_error("set_neuron_count"):
            self._config.set("brain.neuron_count", count)
        return self
    
    def set_synapse_density(self, density: float) -> 'NLMConfig':
        """Set synapse density and return self for method chaining."""
        with catch_nlm_error("set_synapse_density"):
            self._config.set("brain.synapse_density", density)
        return self
    
    def set_learning_rate(self, rate: float) -> 'NLMConfig':
        """Set learning rate and return self for method chaining."""
        with catch_nlm_error("set_learning_rate"):
            self._config.set("brain.learning_rate", rate)
        return self
    
    def set_stdp_learning_rate(self, rate: float) -> 'NLMConfig':
        """Set STDP learning rate and return self for method chaining."""
        with catch_nlm_error("set_stdp_learning_rate"):
            self._config.set("plasticity.stdp.learning_rate", rate)
        return self
    
    # ========== Batch Configuration Operations ==========
    
    def batch_set(self, config_dict: Dict[str, Any]) -> 'NLMConfig':
        """Set multiple configuration values at once.
        
        Args:
            config_dict: Dictionary of configuration key-value pairs
            
        Returns:
            Self for method chaining
        """
        with catch_nlm_error("batch_set"):
            for key, value in config_dict.items():
                self._config.set(key, value)
        return self
    
    def batch_get(self, keys: List[str]) -> Dict[str, Any]:
        """Get multiple configuration values efficiently.
        
        Args:
            keys: List of configuration keys to retrieve
            
        Returns:
            Dictionary mapping keys to their values (or None if key doesn't exist)
        """
        results = {}
        for key in keys:
            with catch_nlm_error("batch_get"):
                if self._config.has(key):
                    results[key] = self._config.get(key)
                else:
                    results[key] = None
        return results
    
    def validate_required_keys(self, required_keys: List[str]) -> 'NLMConfig':
        """Validate that required configuration keys exist.
        
        Args:
            required_keys: List of required configuration keys
            
        Returns:
            Self for method chaining
            
        Raises:
            ValidationError: If any required key is missing
        """
        missing = [key for key in required_keys if not self._config.has(key)]
        if missing:
            raise ValidationError(f"Missing required configuration keys: {', '.join(missing)}")
        return self
    
    # ========== Configuration Loading ==========
    
    def load_from_file(self, filepath: Union[str, Path]) -> 'NLMConfig':
        """Load configuration from file.
        
        Args:
            filepath: Path to configuration file
            
        Returns:
            Self for method chaining
            
        Raises:
            ConfigurationError: If file loading fails
        """
        with catch_nlm_error("load_from_file"):
            self._config.loadFromFile(str(filepath))
        return self
    
    def load_from_args(self, args: Optional[List[str]] = None) -> 'NLMConfig':
        """Load configuration from command line arguments.
        
        Args:
            args: Command line arguments (defaults to sys.argv)
            
        Returns:
            Self for method chaining
        """
        if args is None:
            args = sys.argv
        with catch_nlm_error("load_from_args"):
            self._config.loadFromArgs(len(args), args)
        return self
    
    def save_to_file(self, filepath: Union[str, Path]) -> 'NLMConfig':
        """Save configuration to file.
        
        Args:
            filepath: Path to save configuration file
            
        Returns:
            Self for method chaining
            
        Raises:
            ConfigurationError: If file saving fails
        """
        with catch_nlm_error("save_to_file"):
            self._config.saveToFile(str(filepath))
        return self
    
    # ========== Configuration Query Methods ==========
    
    def has(self, key: str) -> bool:
        """Check if configuration key exists."""
        return self._config.has(key)
    
    def get(self, key: str, default: Any = None) -> Any:
        """Get configuration value with optional default."""
        with catch_nlm_error("get"):
            if self._config.has(key):
                return self._config.get(key)
            return default
    
    def get_int(self, key: str, default: int = 0) -> int:
        """Get configuration value as integer with default."""
        value = self.get(key)
        return value if isinstance(value, int) else default
    
    def get_float(self, key: str, default: float = 0.0) -> float:
        """Get configuration value as float with default."""
        value = self.get(key)
        return value if isinstance(value, float) else default
    
    def get_bool(self, key: str, default: bool = False) -> bool:
        """Get configuration value as boolean with default."""
        value = self.get(key)
        return value if isinstance(value, bool) else default
    
    def get_string(self, key: str, default: str = "") -> str:
        """Get configuration value as string with default."""
        value = self.get(key)
        return value if isinstance(value, str) else default
    
    def get_keys(self) -> List[str]:
        """Get all configuration keys."""
        return self._config.getKeys()
    
    def get_summary(self) -> str:
        """Get configuration summary string."""
        return self._config.summary()
    
    def clear(self) -> 'NLMConfig':
        """Clear all configuration and return self."""
        with catch_nlm_error("clear"):
            self._config.clear()
        return self
    
    def copy(self) -> 'NLMConfig':
        """Create a copy of this configuration."""
        new_config = NLMConfig()
        for key in self.get_keys():
            new_config._config.set(key, self._config.get(key))
        return new_config
class NLMBrain:
    """
    Enhanced brain class with improved usability, method chaining, and batch operations.
    
    Provides a more Pythonic interface to the underlying C++ brain implementation.
    """
    
    def __init__(self, config: NLMConfig):
        if not PYNLML_AVAILABLE:
            raise NLMError("pynlm module is not available")
        self._brain = pynlm.createBrain(config._config)
        self._brain.initialize()
    
    def __repr__(self):
        return f"<NLMBrain: {self.get_total_neuron_count()} neurons, {self.get_total_synapse_count()} synapses>"
    
    # ========== Initialization (Method Chaining Support) ==========
    
    def initialize(self) -> 'NLMBrain':
        """Initialize the brain and return self for method chaining."""
        with catch_nlm_error("initialize"):
            self._brain.initialize()
        return self
    
    # ========== Simulation Steps (Batch Operations) ==========
    
    def step(self, steps: int = 1) -> 'NLMBrain':
        """Run one or more simulation steps.
        
        Args:
            steps: Number of steps to run (default: 1)
            
        Returns:
            Self for method chaining
        """
        for i in range(steps):
            with catch_nlm_error(f"step_{i}"):
                self._brain.step(i)
        return self
    
    def step_with_time(self, steps: List[tuple]) -> 'NLMBrain':
        """Run simulation steps with timestamps.
        
        Args:
            steps: List of (step_number, time) tuples
            
        Returns:
            Self for method chaining
        """
        for step_num, time_val in steps:
            with catch_nlm_error(f"step_with_time_{step_num}"):
                self._brain.step(step_num, time_val)
        return self
    
    # ========== Batch Neural Operations ==========
    
    def inject_current_batch(self, neuron_type: pynlm.NeuronType, 
                           current: float) -> 'NLMBrain':
        """Inject current into all neurons of a given type.
        
        Args:
            neuron_type: Type of neurons to inject current into
            current: Current value to inject
            
        Returns:
            Self for method chaining
        """
        with catch_nlm_error("inject_current_batch"):
            self._brain.injectCurrentToNeurons(neuron_type, current)
        return self
    
    def inject_current_specific(self, neuron_id: pynlm.NeuronId, 
                               current: float) -> 'NLMBrain':
        """Inject current into a specific neuron.
        
        Args:
            neuron_id: ID of neuron to inject current into
            current: Current value to inject
            
        Returns:
            Self for method chaining
        """
        with catch_nlm_error("inject_current_specific"):
            self._brain.injectCurrent(neuron_id, current)
        return self
    
    # ========== Sensory Input Processing (Batch) ==========
    
    def receive_sensory_input_batch(self, inputs: List[Any]) -> 'NLMBrain':
        """Process multiple sensory inputs in sequence.
        
        Args:
            inputs: List of sensory inputs
            
        Returns:
            Self for method chaining
        """
        for input_data in inputs:
            with catch_nlm_error("receive_sensory_input_batch"):
                self._brain.receiveSensoryInput(input_data)
        return self
    
    # ========== Action Production ==========
    
    def produce_action(self) -> Any:
        """Produce an action based on current brain state."""
        with catch_nlm_error("produce_action"):
            return self._brain.produceAction()
    
    # ========== Statistics and Analytics (Batch Queries) ==========
    
    def get_statistics(self) -> Dict[str, Any]:
        """Get comprehensive statistics about the brain.
        
        Returns:
            Dictionary containing brain statistics
        """
        stats = {}
        
        with catch_nlm_error("get_total_neuron_count"):
            stats['total_neurons'] = self._brain.getTotalNeuronCount()
        
        with catch_nlm_error("get_total_synapse_count"):
            stats['total_synapses'] = self._brain.getTotalSynapseCount()
        
        with catch_nlm_error("get_firing_neuron_count"):
            stats['firing_neurons'] = self._brain.getFiringNeuronCount()
        
        with catch_nlm_error("get_active_neuron_count"):
            stats['active_neurons'] = self._brain.getActiveNeuronCount()
        
        with catch_nlm_error("get_average_firing_rate"):
            stats['average_firing_rate'] = self._brain.getAverageFiringRate()
        
        with catch_nlm_error("get_excitation_inhibition_ratio"):
            stats['excitation_inhibition_ratio'] = self._brain.getExcitationInhibitionRatio()
        
        with catch_nlm_error("get_total_spike_count"):
            stats['total_spikes'] = self._brain.getTotalSpikeCount()
        
        with catch_nlm_error("get_developmental_stage"):
            stats['developmental_stage'] = self._brain.getDevelopmentalStage()
        
        return stats
    
    # ========== Convenience Methods ==========
    
    def get_total_neuron_count(self) -> int:
        """Get total neuron count."""
        return self._brain.getTotalNeuronCount()
    
    def get_total_synapse_count(self) -> int:
        """Get total synapse count."""
        return self._brain.getTotalSynapseCount()
    
    def get_firing_neuron_count(self) -> int:
        """Get count of currently firing neurons."""
        return self._brain.getFiringNeuronCount()
    
    def get_active_neuron_count(self) -> int:
        """Get count of active neurons."""
        return self._brain.getActiveNeuronCount()
    
    def get_average_firing_rate(self) -> float:
        """Get average firing rate across all neurons."""
        return self._brain.getAverageFiringRate()
    
    def get_excitation_inhibition_ratio(self) -> float:
        """Get excitation/inhibition balance ratio."""
        return self._brain.getExcitationInhibitionRatio()
    
    def get_total_spike_count(self) -> int:
        """Get total spike count."""
        return self._brain.getTotalSpikeCount()
    
    def get_developmental_stage(self) -> Any:
        """Get current developmental stage."""
        return self._brain.getDevelopmentalStage()
    
    def get_configuration(self) -> NLMConfig:
        """Get configuration object."""
        # Note: This would need a getConfig method in the C++ bindings
        # For now, return a minimal config
        config = NLMConfig()
        return config
    
    # ========== State Management ==========
    
    def reset(self) -> 'NLMBrain':
        """Reset brain state and return self."""
        with catch_nlm_error("reset"):
            self._brain.reset()
        return self
    
    def save(self, filepath: Union[str, Path]) -> 'NLMBrain':
        """Save brain state to file.
        
        Args:
            filepath: Path to save brain state
            
        Returns:
            Self for method chaining
            
        Raises:
            MemoryError: If save operation fails
        """
        with catch_nlm_error("save"):
            success = self._brain.save(str(filepath))
            if not success:
                raise MemoryError(f"Failed to save brain state to {filepath}")
        return self
    
    def load(self, filepath: Union[str, Path]) -> 'NLMBrain':
        """Load brain state from file.
        
        Args:
            filepath: Path to load brain state from
            
        Returns:
            Self for method chaining
            
        Raises:
            MemoryError: If load operation fails
        """
        with catch_nlm_error("load"):
            success = self._brain.load(str(filepath))
            if not success:
                raise MemoryError(f"Failed to load brain state from {filepath}")
        return self
    
    # ========== Region Management (Enhanced) ==========
    
    def add_region(self, name: str = "") -> Any:
        """Add a new neural region.
        
        Args:
            name: Optional name for the region
            
        Returns:
            Region ID
        """
        with catch_nlm_error("add_region"):
            return self._brain.addRegion(name)
    
    def get_region(self, region_id: Any) -> Optional[Any]:
        """Get a neural region by ID.
        
        Args:
            region_id: Region ID
            
        Returns:
            NeuralRegion object or None if not found
        """
        with catch_nlm_error("get_region"):
            return self._brain.getRegion(region_id)
    
    def get_region_count(self) -> int:
        """Get number of regions."""
        return self._brain.getRegionCount()
    
    def get_region_ids(self) -> List[Any]:
        """Get all region IDs."""
        return self._brain.getRegionIds()
    
    def get_regions(self) -> List[Any]:
        """Get all regions."""
        return self._brain.getRegions()
    
    # ========== Convenience Simulation Patterns ==========
    
    def run_steps(self, num_steps: int, step_callback: Optional[Callable] = None) -> 'NLMBrain':
        """Run simulation with optional step callback.
        
        Args:
            num_steps: Number of steps to run
            step_callback: Optional callback function called after each step
            
        Returns:
            Self for method chaining
        """
        for i in range(num_steps):
            self._brain.step(i)
            if step_callback:
                step_callback(i, self.get_statistics())
        return self
    
    def run_episode(self, num_steps: int, 
                   sensory_inputs: Optional[List[Any]] = None) -> Dict[str, Any]:
        """Run a complete simulation episode.
        
        Args:
            num_steps: Number of steps to run
            sensory_inputs: Optional list of sensory inputs (one per step)
            
        Returns:
            Dictionary with episode statistics and results
        """
        results = {
            'steps_completed': 0,
            'total_spikes': 0,
            'final_stats': None,
            'actions': []
        }
        
        for i in range(num_steps):
            # Process sensory input if provided
            if sensory_inputs and i < len(sensory_inputs):
                self._brain.receiveSensoryInput(sensory_inputs[i])
            
            # Run brain step
            self._brain.step(i)
            
            # Record statistics
            results['steps_completed'] += 1
            results['total_spikes'] += self._brain.getTotalSpikeCount()
            
            # Record action
            action = self._brain.produceAction()
            results['actions'].append(action)
        
        # Get final statistics
        results['final_stats'] = self.get_statistics()
        
        return results
class NLMWorld:
    """
    Enhanced world class with improved usability and convenience methods.
    
    Provides a user-friendly interface for world simulation.
    """
    
    def __init__(self):
        if not PYNLML_AVAILABLE:
            raise NLMError("pynlm module is not available")
        self._world = pynlm.createSimpleWorld()
    
    def __repr__(self):
        return f"<NLMWorld: {self._world.getWidth()}x{self._world.getHeight()}>"
    
    # ========== World Configuration (Method Chaining) ==========
    
    def configure(self, width: int, height: int, 
                  vision_width: int, vision_height: int) -> 'NLMWorld':
        """Configure world dimensions and return self for method chaining."""
        with catch_nlm_error("configure"):
            self._world.configure(width, height, vision_width, vision_height)
        return self
    
    # ========== World Control ==========
    
    def reset(self) -> 'NLMWorld':
        """Reset world to initial state and return self."""
        with catch_nlm_error("reset"):
            self._world.reset()
        return self
    
    def update(self, timestep: float) -> 'NLMWorld':
        """Update world for one timestep and return self."""
        with catch_nlm_error("update"):
            self._world.update(timestep)
        return self
    
    def update_batch(self, timesteps: List[float]) -> 'NLMWorld':
        """Update world for multiple timesteps.
        
        Args:
            timesteps: List of timestep values
            
        Returns:
            Self for method chaining
        """
        for timestep in timesteps:
            self.update(timestep)
        return self
    
    # ========== Agent Interaction ==========
    
    def set_agent_start(self, x: float, y: float) -> 'NLMWorld':
        """Set agent starting position and return self."""
        with catch_nlm_error("set_agent_start"):
            self._world.setAgentStart(x, y)
        return self
    
    def set_agent_start_tuple(self, position: tuple) -> 'NLMWorld':
        """Set agent starting position from tuple and return self."""
        return self.set_agent_start(position[0], position[1])
    
    def apply_motor_command(self, action: Any, current_time: float) -> 'NLMWorld':
        """Apply motor command to world and return self.
        
        Args:
            action: Action to apply
            current_time: Current simulation time
            
        Returns:
            Self for method chaining
        """
        with catch_nlm_error("apply_motor_command"):
            self._world.applyMotorCommand(action, current_time)
        return self
    
    # ========== State Access ==========
    
    def get_sensory_percept(self) -> Any:
        """Get sensory percept from world."""
        with catch_nlm_error("get_sensory_percept"):
            return self._world.getSensoryPercept()
    
    def get_agent_body(self) -> Any:
        """Get agent body state from world."""
        with catch_nlm_error("get_agent_body"):
            return self._world.getAgentBody()
    
    # ========== World Object Management ==========
    
    def add_object(self, obj: Any) -> 'NLMWorld':
        """Add world object and return self.
        
        Args:
            obj: World object to add
            
        Returns:
            Self for method chaining
        """
        with catch_nlm_error("add_object"):
            self._world.addObject(obj)
        return self
    
    def add_object_at(self, x: float, y: float, obj_type: Any, 
                      value: float = 0.0, radius: float = 0.5) -> 'NLMWorld':
        """Add world object at specific position.
        
        Args:
            x: X position
            y: Y position
            obj_type: Type of object
            value: Object value
            radius: Object radius
            
        Returns:
            Self for method chaining
        """
        obj = pynlm.WorldObject(x, y, obj_type, value, radius)
        return self.add_object(obj)
    
    def remove_object(self, x: float, y: float) -> 'NLMWorld':
        """Remove object at position and return self.
        
        Args:
            x: X position
            y: Y position
            
        Returns:
            Self for method chaining
        """
        with catch_nlm_error("remove_object"):
            self._world.removeObject(x, y)
        return self
    
    def is_valid_position(self, x: float, y: float) -> bool:
        """Check if position is valid.
        
        Args:
            x: X position
            y: Y position
            
        Returns:
            True if position is valid, False otherwise
        """
        with catch_nlm_error("is_valid_position"):
            return self._world.isValidPosition(x, y)
    
    # ========== World Properties ==========
    
    def get_width(self) -> int:
        """Get world width."""
        return self._world.getWidth()
    
    def get_height(self) -> int:
        """Get world height."""
        return self._world.getHeight()
    
    def get_simulation_time(self) -> float:
        """Get current simulation time."""
        return self._world.getSimulationTime()
    
    def get_vision_dimensions(self) -> tuple:
        """Get vision dimensions (width, height)."""
        return (self._world.getVisionWidth(), self._world.getVisionHeight())
    
    def get_max_energy(self) -> float:
        """Get max energy."""
        return self._world.getMaxEnergy()
    
    def set_max_energy(self, energy: float) -> 'NLMWorld':
        """Set max energy and return self."""
        with catch_nlm_error("set_max_energy"):
            self._world.setMaxEnergy(energy)
        return self
    
    def get_energy_decay_rate(self) -> float:
        """Get energy decay rate."""
        return self._world.getEnergyDecayRate()
    
    def set_energy_decay_rate(self, rate: float) -> 'NLMWorld':
        """Set energy decay rate and return self."""
        with catch_nlm_error("set_energy_decay_rate"):
            self._world.setEnergyDecayRate(rate)
        return self
    
    def get_random_seed(self) -> int:
        """Get random seed."""
        return self._world.getRandomSeed()
    
    def set_random_seed(self, seed: int) -> 'NLMWorld':
        """Set random seed and return self."""
        with catch_nlm_error("set_random_seed"):
            self._world.setRandomSeed(seed)
        return self
    
    def get_summary(self) -> str:
        """Get world summary string."""
        return str(self._world)
class NLMAgentBrain:
    """
    Enhanced agent brain class with improved usability and convenience methods.
    
    Provides a user-friendly interface for agent-brain interactions with the world.
    """
    
    def __init__(self, brain: NLMBrain):
        if not PYNLML_AVAILABLE:
            raise NLMError("pynlm module is not available")
        self._agent_brain = pynlm.createAgentBrain(brain._brain)
    
    def __repr__(self):
        return f"<NLMAgentBrain: integrated with brain>"
    
    # ========== Initialization ==========
    
    def initialize(self, world: NLMWorld) -> 'NLMAgentBrain':
        """Initialize agent brain with world and return self."""
        with catch_nlm_error("initialize"):
            self._agent_brain.initialize(world._world)
        return self
    
    # ========== Sensory Processing ==========
    
    def process_sensory_input(self, percept: Any) -> 'NLMAgentBrain':
        """Process sensory percept and return self."""
        with catch_nlm_error("process_sensory_input"):
            self._agent_brain.processSensoryInput(percept)
        return self
    
    # ========== Motor Decoding ==========
    
    def decode_motor_command(self) -> Any:
        """Decode brain motor activity into motor command."""
        with catch_nlm_error("decode_motor_command"):
            return self._agent_brain.decodeMotorCommand()
    
    # ========== Neuromodulation ==========
    
    def apply_reward_modulation(self, reward: float, predicted_reward: float) -> 'NLMAgentBrain':
        """Apply reward-based neuromodulation and return self.
        
        Args:
            reward: Actual reward received
            predicted_reward: Predicted reward
            
        Returns:
            Self for method chaining
        """
        with catch_nlm_error("apply_reward_modulation"):
            self._agent_brain.applyRewardModulation(reward, predicted_reward)
        return self
    
    def apply_reward(self, reward: float) -> 'NLMAgentBrain':
        """Apply reward (shortcut for apply_reward_modulation with 0 predicted)."""
        return self.apply_reward_modulation(reward, 0.0)
    
    # ========== Development ==========
    
    def update_development(self, timestep: float) -> 'NLMAgentBrain':
        """Update development system and return self.
        
        Args:
            timestep: Time step for development update
            
        Returns:
            Self for method chaining
        """
        with catch_nlm_error("update_development"):
            self._agent_brain.updateDevelopment(timestep)
        return self
    
    # ========== State Access ==========
    
    def get_developmental_stage(self) -> Any:
        """Get current developmental stage."""
        return self._agent_brain.getDevelopmentalStage()
    
    def get_neuromodulation_level(self) -> float:
        """Get current neuromodulation level (dopamine)."""
        return self._agent_brain.getNeuromodulationLevel()
    
    def get_curiosity_level(self) -> float:
        """Get curiosity level."""
        return self._agent_brain.getCuriosityLevel()
    
    def get_novelty_level(self) -> float:
        """Get novelty level."""
        return self._agent_brain.getNoveltyLevel()
    
    def get_prediction_error(self) -> float:
        """Get prediction error."""
        return self._agent_brain.getPredictionError()
    
    # ========== Subsystem Control ==========
    
    def enable_reward_modulation(self, enable: bool) -> 'NLMAgentBrain':
        """Enable or disable reward modulation and return self."""
        with catch_nlm_error("enable_reward_modulation"):
            self._agent_brain.enableRewardModulation(enable)
        return self
    
    def enable_structural_plasticity(self, enable: bool) -> 'NLMAgentBrain':
        """Enable or disable structural plasticity and return self."""
        with catch_nlm_error("enable_structural_plasticity"):
            self._agent_brain.enableStructuralPlasticity(enable)
        return self
    
    def enable_development(self, enable: bool) -> 'NLMAgentBrain':
        """Enable or disable development and return self."""
        with catch_nlm_error("enable_development"):
            self._agent_brain.enableDevelopment(enable)
        return self
    
    def enable_curiosity(self, enable: bool) -> 'NLMAgentBrain':
        """Enable or disable curiosity and return self."""
        with catch_nlm_error("enable_curiosity"):
            self._agent_brain.enableCuriosity(enable)
        return self
    
    # ========== Convenience Methods ==========
    
    def is_reward_modulation_enabled(self) -> bool:
        """Check if reward modulation is enabled."""
        return self._agent_brain.isRewardModulationEnabled()
    
    def is_structural_plasticity_enabled(self) -> bool:
        """Check if structural plasticity is enabled."""
        return self._agent_brain.isStructuralPlasticityEnabled()
    
    def is_development_enabled(self) -> bool:
        """Check if development is enabled."""
        return self._agent_brain.isDevelopmentEnabled()
    
    def is_curiosity_enabled(self) -> bool:
        """Check if curiosity is enabled."""
        return self._agent_brain.isCuriosityEnabled()
    
    def reset(self) -> 'NLMAgentBrain':
        """Reset agent for new episode and return self."""
        with catch_nlm_error("reset"):
            self._agent_brain.reset()
        return self
    
    def get_brain(self) -> NLMBrain:
        """Get underlying brain object."""
        # Note: This would need a getBrain method in the C++ bindings
        # For now, we can't provide direct access
        raise NotImplementedError("Direct brain access not yet implemented in Python bindings")
# === Convenience Factory Functions ===

def create_enhanced_config(**kwargs) -> NLMConfig:
    """Create an enhanced configuration with optional initial values.
    
    Args:
        **kwargs: Initial configuration values
        
    Returns:
        Configured NLMConfig instance
    """
    config = NLMConfig()
    
    # Map common configuration keys to their proper NLMConfig methods
    config_mapping = {
        'neuron_count': 'set_neuron_count',
        'synapse_density': 'set_synapse_density',
        'learning_rate': 'set_learning_rate',
        'stdp_learning_rate': 'set_stdp_learning_rate',
    }
    
    for key, value in kwargs.items():
        if key in config_mapping:
            method = getattr(config, config_mapping[key])
            method(value)
        else:
            # Set directly as key-value pair
            config._config.set(key, value)
    
    return config

def run_simulation_episode(brain: NLMBrain, world: NLMWorld, 
                           agent: NLMAgentBrain, 
                           num_steps: int = 100,
                           timestep: float = 0.1) -> Dict[str, Any]:
    """Run a complete simulation episode with all components.
    
    Args:
        brain: NLMBrain instance
        world: NLMWorld instance
        agent: NLMAgentBrain instance
        num_steps: Number of steps to run
        timestep: Time step for world updates
        
    Returns:
        Dictionary with simulation results and statistics
    """
    results = {
        'steps_completed': 0,
        'actions_taken': [],
        'statistics_over_time': [],
        'episode_rewards': [],
        'final_brain_state': None
    }
    
    for step in range(num_steps):
        # Update world
        world.update(timestep)
        
        # Get sensory input
        percept = world.get_sensory_percept()
        
        # Process sensory input
        agent.process_sensory_input(percept)
        
        # Run brain step
        brain.step(step)
        
        # Decode action
        action = agent.decode_motor_command()
        
        # Apply action
        world.apply_motor_command(action, world.get_simulation_time())
        
        # Apply reward modulation
        reward = percept.get_internal()[0] if percept.get_internal() else 0.0
        agent.apply_reward(reward)
        
        # Update development
        agent.update_development(timestep)
        
        # Record results
        results['steps_completed'] += 1
        results['actions_taken'].append(action)
        results['statistics_over_time'].append(brain.get_statistics())
        results['episode_rewards'].append(reward)
    
    # Get final state
    results['final_brain_state'] = brain.get_statistics()
    
    return results

# === Error Handling Utilities ===

def safe_operation(operation: Callable, *args, default: Any = None, 
                   operation_name: str = "operation") -> Any:
    """Safely execute an operation with error handling.
    
    Args:
        operation: Operation to execute
        *args: Arguments to pass to operation
        default: Default value to return on error
        operation_name: Name of operation for error messages
        
    Returns:
        Result of operation or default value on error
    """
    try:
        return operation(*args)
    except (ValidationError, ConfigurationError, MemoryError) as e:
        print(f"Error in {operation_name}: {e}")
        return default
    except Exception as e:
        print(f"Unexpected error in {operation_name}: {e}")
        print(f"Traceback: {traceback.format_exc()}")
        return default

# === Documentation and Usage Examples ===

def print_examples():
    """Print usage examples for the enhanced NLM API."""
    examples = """
=== Enhanced NLM API Examples ===

1. Basic Usage with Method Chaining:

config = NLMConfig().set_neuron_count(2000).set_learning_rate(0.001)
brain = NLMBrain(config).initialize()
brain.step(100).step(50)  # Chain steps

2. Batch Configuration:

config = (NLMConfig()
          .batch_set({
              'brain.neuron_count': 3000,
              'brain.synapse_density': 0.15,
              'plasticity.stdp.learning_rate': 0.002
          }))

3. Complete Simulation Episode:

brain = NLMBrain(create_enhanced_config(neuron_count=2000))
world = NLMWorld().configure(50, 50, 8, 8)
agent = NLMAgentBrain(brain).initialize(world)

agent.enable_reward_modulation(True)
agent.enable_curiosity(True)

results = run_simulation_episode(brain, world, agent, 1000)
print(f"Episode completed: {results['steps_completed']} steps")
print(f"Total spikes: {results['final_brain_state']['total_spikes']}")

4. Error Handling:

try:
    brain = NLMBrain(NLMConfig())
    brain.initialize()
    brain.step(10)
except NLMError as e:
    print(f"Simulation error: {e}")

5. Statistics Collection:

stats = brain.get_statistics()
print(f"Firing neurons: {stats['firing_neurons']}")
print(f"Average firing rate: {stats['average_firing_rate']:.2f}")
print(f"E/I ratio: {stats['excitation_inhibition_ratio']:.3f}")
"""
    print(examples)
# === Module Information ===

__version__ = "0.1.0"
__author__ = "NLM Development Team"
__description__ = "Enhanced Python API for NLM (Neural Learning Machine) with improved usability"

if not PYNLML_AVAILABLE:
    print("\n=== WARNING ===")
    print("The underlying pynlm module is not available.")
    print("The enhanced API will not function until pynlm is properly installed.")
    print("You can build pynlm using: pip install -e . from the project root.")
    print("\nTo install pynlm:")
    print("  cd /path/to/nlm")
    print("  pip install -e .")
    print("  # or build with scikit-build-core")
    print("  pip install scikit-build-core pybind11")
    print("  pip install .")

if __name__ == "__main__":
    print_examples()
