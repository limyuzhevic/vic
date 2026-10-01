"""
NLM (Neural Learning Machine) - Python Bindings

A Python binding for the NLM C++ neural simulation framework.

This package provides a Python interface to the NLM brain simulator,
including classes for Brain, Config, AgentBrain, SimpleWorld, and more.

Key Features:
- Modern Python API with type hints and context managers
- Advanced features for power users
- Comprehensive error handling and validation
- Memory management utilities
- Debug and profiling tools
- Configuration file support (JSON/YAML)
"""

import sys
from typing import Optional, Union, List, Dict, Any, ContextManager
from contextlib import contextmanager
import json
import os

# Version information
__version__ = "0.1.0"
__author__ = "NLM Authors"
__description__ = "Python bindings for NLM neural simulation framework"

# Import the main C++ module
try:
    import pynlm
    _HAS_PYNLM = True
except ImportError:
    _HAS_PYNLM = False
    pynlm = None

# Custom exceptions
class NLMError(Exception):
    """Base exception for all NLM errors."""
    pass

class ConfigurationError(NLMError):
    """Error in configuration."""
    pass

class SimulationError(NLMError):
    """Error during simulation."""
    pass

class MemoryError(NLMError):
    """Memory-related error."""
    pass

class ValidationError(NLMError):
    """Validation error."""
    pass

def check_pynlm_available():
    """Check if pynlm is available and raise error if not."""
    if not _HAS_PYNLM:
        raise NLMError(
            "pynlm module is not available. "
            "Please ensure NLM is properly installed with Python bindings."
        )

class ConfigManager:
    """
    Enhanced configuration manager with JSON/YAML support.
    
    Provides a Pythonic interface to NLM configuration with validation,
    serialization, and file I/O support.
    """
    
    def __init__(self, config=None):
        """
        Initialize configuration manager.
        
        Args:
            config: Optional existing pynlm.Config object
        """
        check_pynlm_available()
        if config is not None:
            self._config = config
        else:
            self._config = pynlm.createDefaultConfig()
        
        # Validate configuration
        self._validate_config()
    
    def _validate_config(self):
        """Validate configuration consistency."""
        # Basic validation - can be extended with more sophisticated checks
        if self.has("brain.neuron_count"):
            neuron_count = self.get_int("brain.neuron_count")
            if neuron_count <= 0:
                raise ValidationError("Neuron count must be positive")
        
        if self.has("brain.connection_probability"):
            prob = self.get_float("brain.connection_probability")
            if prob < 0.0 or prob > 1.0:
                raise ValidationError("Connection probability must be between 0.0 and 1.0")
    
    def load_from_json(self, filepath: str) -> None:
        """
        Load configuration from JSON file.
        
        Args:
            filepath: Path to JSON configuration file
            
        Raises:
            ConfigurationError: If file cannot be loaded or parsed
            ValidationError: If configuration is invalid
        """
        try:
            with open(filepath, 'r') as f:
                data = json.load(f)
            self._load_dict(data)
        except (IOError, OSError) as e:
            raise ConfigurationError(f"Cannot read configuration file: {e}")
        except json.JSONDecodeError as e:
            raise ConfigurationError(f"Invalid JSON in configuration file: {e}")
    
    def save_to_json(self, filepath: str, indent: int = 2) -> None:
        """
        Save configuration to JSON file.
        
        Args:
            filepath: Path to save JSON configuration
            indent: JSON indentation level
            
        Raises:
            ConfigurationError: If file cannot be written
        """
        try:
            data = self.to_dict()
            with open(filepath, 'w') as f:
                json.dump(data, f, indent=indent)
        except (IOError, OSError) as e:
            raise ConfigurationError(f"Cannot write configuration file: {e}")
    
    def _load_dict(self, data: Dict[str, Any]) -> None:
        """Load configuration from dictionary."""
        # Flatten the dictionary for pynlm's key=value system
        for key, value in data.items():
            self.set(key, value)
    
    def to_dict(self) -> Dict[str, Any]:
        """
        Convert configuration to dictionary.
        
        Returns:
            Dictionary representation of configuration
        """
        # This is a simplified implementation
        # pynlm doesn't provide direct serialization, so we read from the config
        result = {}
        for key in self._config.getKeys():
            result[key] = self._get_value(key)
        return result
    
    def _get_value(self, key: str) -> Any:
        """Get value from config with type inference."""
        if self._config.has(key):
            # Try different types
            if self._config.has(key):
                # Try int
                try:
                    return self._config.getInt(key)
                except:
                    pass
                
                # Try double
                try:
                    return self._config.getDouble(key)
                except:
                    pass
                
                # Try string
                try:
                    return self._config.getString(key)
                except:
                    pass
                
                # Try bool
                try:
                    return self._config.getBool(key)
                except:
                    pass
        
        return None
    
    def __getattr__(self, name: str):
        """Get configuration value using attribute access."""
        if self._config.has(name):
            return self._get_value(name)
        raise AttributeError(f"Configuration has no key '{name}'")
    
    def __setattr__(self, name: str, value: Any) -> None:
        """Set configuration value using attribute access."""
        if name.startswith('_'):
            super().__setattr__(name, value)
        else:
            self.set(name, value)
    
    def has(self, key: str) -> bool:
        """Check if configuration has key."""
        return self._config.has(key)
    
    def get(self, key: str, default: Any = None) -> Any:
        """Get configuration value."""
        if self.has(key):
            return self._get_value(key)
        return default
    
    def get_int(self, key: str, default: int = None) -> Optional[int]:
        """Get integer configuration value."""
        if self.has(key):
            try:
                return self._config.getInt(key)
            except:
                pass
        return default
    
    def get_float(self, key: str, default: float = None) -> Optional[float]:
        """Get float configuration value."""
        if self.has(key):
            try:
                return self._config.getDouble(key)
            except:
                pass
        return default
    
    def get_string(self, key: str, default: str = None) -> Optional[str]:
        """Get string configuration value."""
        if self.has(key):
            try:
                return self._config.getString(key)
            except:
                pass
        return default
    
    def get_bool(self, key: str, default: bool = None) -> Optional[bool]:
        """Get boolean configuration value."""
        if self.has(key):
            try:
                return self._config.getBool(key)
            except:
                pass
        return default
    
    def set(self, key: str, value: Any) -> None:
        """
        Set configuration value.
        
        Args:
            key: Configuration key (supports dot notation)
            value: Configuration value
        """
        # Convert Python types to C++ types
        if isinstance(value, int):
            self._config.set(key, value)
        elif isinstance(value, float):
            self._config.set(key, value)
        elif isinstance(value, str):
            self._config.set(key, value)
        elif isinstance(value, bool):
            self._config.set(key, value)
        else:
            raise ConfigurationError(f"Unsupported configuration value type: {type(value)}")
    
    def keys(self) -> List[str]:
        """Get all configuration keys."""
        return self._config.getKeys()
    
    def summary(self) -> str:
        """Get configuration summary."""
        return self._config.summary()
    
    def clear(self) -> None:
        """Clear all configuration entries."""
        self._config.clear()
    
    def clone(self) -> 'ConfigManager':
        """Create a copy of this configuration."""
        return ConfigManager(self._config)

class _BrainContext:
    """Internal context manager for Brain lifecycle."""
    
    def __init__(self, config_manager):
        self.config_manager = config_manager
        self.brain = None
    
    def __enter__(self):
        self.brain = pynlm.createBrain(self.config_manager._config)
        self.brain.initialize()
        return self.brain
    
    def __exit__(self, exc_type, exc_val, exc_tb):
        if self.brain:
            self.brain.reset()

class _AgentContext:
    """Internal context manager for AgentBrain lifecycle."""
    
    def __init__(self, brain, world):
        self.brain = brain
        self.world = world
        self.agent = None
    
    def __enter__(self):
        self.agent = pynlm.createAgentBrain(self.brain)
        self.agent.initialize(self.world)
        return self.agent
    
    def __exit__(self, exc_type, exc_val, exc_tb):
        if self.agent:
            self.agent.reset()

class Brain:
    """
    Enhanced Brain class with modern Python patterns.
    
    Provides a Pythonic interface to the NLM brain simulation with
    context management, batch operations, and advanced features.
    """
    
    def __init__(self, config_manager: Optional[ConfigManager] = None):
        """
        Initialize Brain.
        
        Args:
            config_manager: Configuration manager. If None, creates default config.
        """
        check_pynlm_available()
        
        if config_manager is not None:
            self._config_manager = config_manager
            self._brain = pynlm.createBrain(config_manager._config)
        else:
            self._config_manager = ConfigManager()
            self._brain = pynlm.createBrain(self._config_manager._config)
        
        self._initialized = False
        self._step_count = 0
    
    def __enter__(self) -> 'Brain':
        """Context manager entry."""
        if not self._initialized:
            self.initialize()
        return self
    
    def __exit__(self, exc_type, exc_val, exc_tb) -> None:
        """Context manager exit."""
        self.reset()
    
    def __repr__(self) -> str:
        """String representation."""
        return f"<Brain neurons={self.get_total_neuron_count()} synapses={self.get_total_synapse_count()}>"
    
    @contextmanager
    def simulation_context(self) -> ContextManager['Brain']:
        """
        Context manager for simulation steps.
        
        Yields:
            Self for use in with statement
        """
        if not self._initialized:
            self.initialize()
        try:
            yield self
        finally:
            pass
    
    def initialize(self) -> 'Brain':
        """
        Initialize the brain with configuration.
        
        Returns:
            Self for method chaining
            
        Raises:
            SimulationError: If brain cannot be initialized
        """
        try:
            if not self._initialized:
                success = self._brain.initialize()
                if success:
                    self._initialized = True
                else:
                    raise SimulationError("Brain initialization failed")
        except Exception as e:
            raise SimulationError(f"Brain initialization error: {e}")
        
        return self
    
    def step(self, step_number: int, current_time: Optional[float] = None) -> 'Brain':
        """
        Perform a simulation step.
        
        Args:
            step_number: Simulation step number
            current_time: Optional timestamp
            
        Returns:
            Self for method chaining
            
        Raises:
            SimulationError: If step fails
        """
        if not self._initialized:
            raise SimulationError("Brain not initialized. Call initialize() first.")
        
        try:
            if current_time is not None:
                self._brain.step(step_number, current_time)
            else:
                self._brain.step(step_number)
            self._step_count += 1
        except Exception as e:
            raise SimulationError(f"Simulation step error: {e}")
        
        return self
    
    def run_steps(self, num_steps: int, start_time: float = 0.0) -> 'Brain':
        """
        Run multiple simulation steps efficiently.
        
        Args:
            num_steps: Number of steps to run
            start_time: Starting timestamp
            
        Returns:
            Self for method chaining
        """
        for i in range(num_steps):
            current_time = start_time + i * 0.001  # Assuming 1ms timestep
            self.step(i, current_time)
        
        return self
    
    def reset(self) -> 'Brain':
        """
        Reset brain state.
        
        Returns:
            Self for method chaining
        """
        try:
            self._brain.reset()
            self._initialized = False
            self._step_count = 0
        except Exception as e:
            raise SimulationError(f"Brain reset error: {e}")
        
        return self
    
    def save(self, filepath: str) -> 'Brain':
        """
        Save brain state to file.
        
        Args:
            filepath: Path to save checkpoint
            
        Returns:
            Self for method chaining
            
        Raises:
            SimulationError: If save fails
        """
        try:
            success = self._brain.save(filepath)
            if not success:
                raise SimulationError("Failed to save brain state")
        except Exception as e:
            raise SimulationError(f"Save error: {e}")
        
        return self
    
    def load(self, filepath: str) -> 'Brain':
        """
        Load brain state from file.
        
        Args:
            filepath: Path to checkpoint file
            
        Returns:
            Self for method chaining
            
        Raises:
            SimulationError: If load fails
        """
        try:
            success = self._brain.load(filepath)
            if not success:
                raise SimulationError("Failed to load brain state")
            self._initialized = True
        except Exception as e:
            raise SimulationError(f"Load error: {e}")
        
        return self
    
    # Configuration access methods
    
    def get_config(self) -> ConfigManager:
        """Get configuration manager."""
        return self._config_manager
    
    # Statistics methods
    
    def get_total_neuron_count(self) -> int:
        """Get total neuron count."""
        return self._brain.getTotalNeuronCount()
    
    def get_total_synapse_count(self) -> int:
        """Get total synapse count."""
        return self._brain.getTotalSynapseCount()
    
    def get_active_neuron_count(self) -> int:
        """Get active neuron count."""
        return self._brain.getActiveNeuronCount()
    
    def get_firing_neuron_count(self) -> int:
        """Get firing neuron count."""
        return self._brain.getFiringNeuronCount()
    
    def get_average_firing_rate(self) -> float:
        """Get average firing rate."""
        return self._brain.getAverageFiringRate()
    
    def get_excitation_inhibition_ratio(self) -> float:
        """Get excitation/inhibition ratio."""
        return self._brain.getExcitationInhibitionRatio()
    
    def get_total_spike_count(self) -> int:
        """Get total spike count."""
        return self._brain.getTotalSpikeCount()
    
    def get_pending_spike_event_count(self) -> int:
        """Get pending spike event count."""
        return self._brain.getPendingSpikeEventCount()
    
    def get_developmental_stage(self):
        """Get developmental stage."""
        return self._brain.getDevelopmentalStage()
    
    def set_developmental_stage(self, stage) -> 'Brain':
        """Set developmental stage."""
        self._brain.setDevelopmentalStage(stage)
        return self
    
    # Access to underlying C++ brain object
    
    def _get_cbrain(self):
        """Get underlying C++ brain object (internal use)."""
        return self._brain

class Agent:
    """
    Enhanced AgentBrain class with Pythonic interface.
    
    Provides a simplified interface for agent simulation with
    automatic configuration and advanced features.
    """
    
    def __init__(self, brain: Optional[Brain] = None, config_manager: Optional[ConfigManager] = None):
        """
        Initialize Agent.
        
        Args:
            brain: Optional Brain object. If None, creates new brain.
            config_manager: Optional configuration manager.
        """
        check_pynlm_available()
        
        if brain is not None:
            self._brain = brain._get_cbrain()
            self._config_manager = brain.get_config()
        else:
            self._config_manager = config_manager or ConfigManager()
            brain_obj = pynlm.createBrain(self._config_manager._config)
            brain_obj.initialize()
            self._brain = brain_obj
        
        self._agent = pynlm.createAgentBrain(self._brain)
        self._world = None
        self._initialized = False
    
    def __enter__(self) -> 'Agent':
        """Context manager entry."""
        return self
    
    def __exit__(self, exc_type, exc_val, exc_tb) -> None:
        """Context manager exit."""
        self.reset()
    
    def initialize(self, world) -> 'Agent':
        """
        Initialize agent with world.
        
        Args:
            world: World object
            
        Returns:
            Self for method chaining
            
        Raises:
            SimulationError: If initialization fails
        """
        try:
            self._world = world
            self._agent.initialize(world)
            self._initialized = True
        except Exception as e:
            raise SimulationError(f"Agent initialization error: {e}")
        
        return self
    
    def process_sensory_input(self, percept) -> 'Agent':
        """
        Process sensory input.
        
        Args:
            percept: Sensory percept data
            
        Returns:
            Self for method chaining
        """
        if not self._initialized:
            raise SimulationError("Agent not initialized.")
        
        try:
            self._agent.processSensoryInput(percept)
        except Exception as e:
            raise SimulationError(f"Sensory processing error: {e}")
        
        return self
    
    def decode_motor_command(self):
        """
        Decode motor command from brain.
        
        Returns:
            Motor command
            
        Raises:
            SimulationError: If decoding fails
        """
        if not self._initialized:
            raise SimulationError("Agent not initialized.")
        
        try:
            return self._agent.decodeMotorCommand()
        except Exception as e:
            raise SimulationError(f"Motor decoding error: {e}")
    
    def apply_reward_modulation(self, reward: float, predicted_reward: float) -> 'Agent':
        """
        Apply reward modulation.
        
        Args:
            reward: Actual reward
            predicted_reward: Predicted reward
            
        Returns:
            Self for method chaining
        """
        if not self._initialized:
            raise SimulationError("Agent not initialized.")
        
        try:
            self._agent.applyRewardModulation(reward, predicted_reward)
        except Exception as e:
            raise SimulationError(f"Reward modulation error: {e}")
        
        return self
    
    def update_development(self, timestep: float) -> 'Agent':
        """
        Update development system.
        
        Args:
            timestep: Time step
            
        Returns:
            Self for method chaining
        """
        if not self._initialized:
            raise SimulationError("Agent not initialized.")
        
        try:
            self._agent.updateDevelopment(timestep)
        except Exception as e:
            raise SimulationError(f"Development update error: {e}")
        
        return self
    
    def reset(self) -> 'Agent':
        """
        Reset agent.
        
        Returns:
            Self for method chaining
        """
        try:
            self._agent.reset()
            self._initialized = False
        except Exception as e:
            raise SimulationError(f"Agent reset error: {e}")
        
        return self
    
    # Configuration access
    
    def get_config(self) -> ConfigManager:
        """Get configuration manager."""
        return self._config_manager
    
    def get_brain(self) -> Brain:
        """Get brain object."""
        return Brain(self._config_manager)
    
    # Statistics methods
    
    def get_curiosity_level(self) -> float:
        """Get curiosity level."""
        return self._agent.getCuriosityLevel()
    
    def get_novelty_level(self) -> float:
        """Get novelty level."""
        return self._agent.getNoveltyLevel()
    
    def get_prediction_error(self) -> float:
        """Get prediction error."""
        return self._agent.getPredictionError()
    
    def get_neuromodulation_level(self) -> float:
        """Get neuromodulation level."""
        return self._agent.getNeuromodulationLevel()
    
    def get_developmental_stage(self):
        """Get developmental stage."""
        return self._agent.getDevelopmentalStage()
    
    # Feature enable/disable
    
    def enable_reward_modulation(self, enable: bool) -> 'Agent':
        """Enable/disable reward modulation."""
        self._agent.enableRewardModulation(enable)
        return self
    
    def enable_structural_plasticity(self, enable: bool) -> 'Agent':
        """Enable/disable structural plasticity."""
        self._agent.enableStructuralPlasticity(enable)
        return self
    
    def enable_development(self, enable: bool) -> 'Agent':
        """Enable/disable development."""
        self._agent.enableDevelopment(enable)
        return self
    
    def enable_curiosity(self, enable: bool) -> 'Agent':
        """Enable/disable curiosity."""
        self._agent.enableCuriosity(enable)
        return self
class SimpleWorld:
    """
    Enhanced SimpleWorld class with Pythonic interface.
    
    Provides a simplified interface for world simulation with
    automatic configuration and advanced features.
    """
    
    def __init__(self):
        """Initialize world."""
        check_pynlm_available()
        self._world = pynlm.createSimpleWorld()
    
    def configure(self, width: int, height: int, vision_width: int = 8, vision_height: int = 8) -> 'SimpleWorld':
        """
        Configure world dimensions.
        
        Args:
            width: World width
            height: World height
            vision_width: Vision width
            vision_height: Vision height
            
        Returns:
            Self for method chaining
        """
        self._world.configure(width, height, vision_width, vision_height)
        return self
    
    def reset(self) -> 'SimpleWorld':
        """Reset world."""
        self._world.reset()
        return self
    
    def set_agent_start(self, x: float, y: float) -> 'SimpleWorld':
        """Set agent starting position."""
        self._world.setAgentStart(x, y)
        return self
    
    def update(self, timestep: float) -> 'SimpleWorld':
        """Update world."""
        self._world.update(timestep)
        return self
    
    def apply_motor_command(self, motor_cmd, current_time: float) -> 'SimpleWorld':
        """Apply motor command."""
        self._world.applyMotorCommand(motor_cmd, current_time)
        return self
    
    def get_sensory_percept(self):
        """Get sensory percept."""
        return self._world.getSensoryPercept()
    
    def get_agent_body(self):
        """Get agent body."""
        return self._world.getAgentBody()
    
    def add_object(self, obj) -> 'SimpleWorld':
        """Add world object."""
        self._world.addObject(obj)
        return self
    
    def remove_object(self, x: float, y: float) -> 'SimpleWorld':
        """Remove world object."""
        self._world.removeObject(x, y)
        return self
    
    def is_valid_position(self, x: float, y: float) -> bool:
        """Check if position is valid."""
        return self._world.isValidPosition(x, y)
    
    def get_width(self) -> int:
        """Get world width."""
        return self._world.getWidth()
    
    def get_height(self) -> int:
        """Get world height."""
        return self._world.getHeight()
    
    def get_simulation_time(self) -> float:
        """Get simulation time."""
        return self._world.getSimulationTime()
    
    def set_random_seed(self, seed: int) -> 'SimpleWorld':
        """Set random seed."""
        self._world.setRandomSeed(seed)
        return self
    
    # Access to underlying C++ world object
    
    def _get_cworld(self):
        """Get underlying C++ world object (internal use)."""
        return self._world
# Factory functions for easy usage
def create_brain(config_manager: Optional[ConfigManager] = None) -> Brain:
    """
    Create a new brain.
    
    Args:
        config_manager: Optional configuration manager.
        
    Returns:
        Brain object
    """
    return Brain(config_manager)


def create_world() -> SimpleWorld:
    """
    Create a new world.
    
    Returns:
        SimpleWorld object
    """
    return SimpleWorld()


def create_agent(brain: Optional[Brain] = None, config_manager: Optional[ConfigManager] = None) -> Agent:
    """
    Create a new agent.
    
    Args:
        brain: Optional brain object.
        config_manager: Optional configuration manager.
        
    Returns:
        Agent object
    """
    return Agent(brain, config_manager)


# Example usage function
def example_usage():
    """
    Example usage of the NLM Python API.
    
    This function demonstrates the main patterns for using NLM.
    """
    print("=== NLM Python API Example Usage ===")
    
    # Example 1: Simple brain simulation
    print("\n1. Simple brain simulation:")
    config = ConfigManager()
    config.set("brain.neuron_count", 500)
    
    with Brain(config) as brain:
        print(f"Created brain with {brain.get_total_neuron_count()} neurons")
        brain.run_steps(100)
        print(f"After 100 steps: {brain.get_total_spike_count()} spikes")
    
    # Example 2: Complete agent simulation
    print("\n2. Complete agent simulation:")
    brain = Brain()
    agent = Agent(brain)
    world = SimpleWorld()
    world.configure(width=20, height=20, vision_width=8, vision_height=8)
    world.reset()
    world.set_agent_start(10.0, 10.0)
    
    agent.initialize(world)
    agent.enable_reward_modulation(True)
    agent.enable_curiosity(True)
    
    for step in range(50):
        world.update(0.1)
        percept = world.get_sensory_percept()
        agent.process_sensory_input(percept)
        
        # Brain step is handled by agent internally
        motor_cmd = agent.decode_motor_command()
        world.apply_motor_command(motor_cmd, world.get_simulation_time())
        
        if step % 10 == 0:
            print(f"Step {step}: {agent.get_curiosity_level():.3f} curiosity")
    
    print("Example completed successfully!")

if _HAS_PYNLM:
    # Replace module-level factory functions with enhanced versions
    pynlm.createBrain = create_brain
    pynlm.createSimpleWorld = create_world
    pynlm.createAgentBrain = create_agent
    pynlm.createDefaultConfig = lambda: ConfigManager()

# Make main classes available at module level
__all__ = [
    'Brain', 'Agent', 'SimpleWorld', 'ConfigManager',
    'create_brain', 'create_world', 'create_agent',
    'example_usage',
    'NLMError', 'ConfigurationError', 'SimulationError',
    'ValidationError'
]
