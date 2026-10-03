# pynlm/bindings.py
# 
# This file provides the Python interface layer for NLM Python bindings.
# It serves as a convenient wrapper around the C++ pybind11 bindings
# and provides additional Python-specific functionality, documentation,
# and helper functions.
#
# The actual bindings are defined in C++ (python/bindings.cpp) and exposed
# through pybind11. This file provides a cleaner Python interface with
# better documentation, error handling, and convenience methods.
#
# This layer bridges the gap between the low-level C++ bindings and
# high-level Python usability.

import sys
from typing import Optional, List, Dict, Any, Union, Tuple
from enum import IntEnum

# Import the actual C++ bindings
try:
    from pynlm.core import (
        Config,
        Brain,
        AgentBrain,
        SimpleWorld,
        Vision,
        Audio,
        InternalSignals,
        SensoryInput,
        Action,
        MotorCommand,
        ActionResult,
        SensoryPercept,
        WorldObject,
        AgentBody,
        NeuronType,
        SynapseType,
        ActionType,
        WorldObjectType,
        DevelopmentalStage,
        FiringState,
        NeuronId,
        SynapseId,
        RegionId,
        PopulationId,
        SimulationStep,
        Timestamp,
        TimestepDuration,
        MembranePotential,
        SynapticWeight,
        RuntimeError,
        createDefaultConfig,
        createBrain,
        createSimpleWorld,
        createAgentBrain,
    )
    
    CPLUSPLUS_AVAILABLE = True
except ImportError:
    CPLUSPLUS_AVAILABLE = False
    print("Warning: C++ NLM bindings not available. Some functionality may be limited.", file=sys.stderr)

# Package version
__version__ = "0.1.0"

# Type aliases for better Python type hints
OptionalFloat = Optional[float]
OptionalInt = Optional[int]
OptionalStr = Optional[str]
ListFloat = List[float]
DictStrAny = Dict[str, Any]

# Exception classes
class NLMError(RuntimeError):
    """Base exception class for all NLM errors."""
    pass

class ConfigurationError(NLMError):
    """Error related to configuration loading or parsing."""
    pass

class SimulationError(NLMError):
    """Error during neural simulation."""
    pass

class AgentError(NLMError):
    """Error related to agent operations."""
    pass

class MemoryError(NLMError):
    """Error related to memory systems."""
    pass

# Convenience constants and enumerations
class MotorCommands:
    """Convenience class for motor command operations."""
    
    MOVE_FORWARD = MotorCommand.MoveForward
    MOVE_BACKWARD = MotorCommand.MoveBackward
    TURN_LEFT = MotorCommand.TurnLeft
    TURN_RIGHT = MotorCommand.TurnRight
    LOOK_LEFT = MotorCommand.LookLeft
    LOOK_RIGHT = MotorCommand.LookRight
    INTERACT = MotorCommand.Interact
    WAIT = MotorCommand.Wait
    
    @classmethod
    def all_commands(cls) -> List[str]:
        """Get all available motor commands as strings."""
        return [cmd.name for cmd in MotorCommand]
    
    @classmethod
    def from_string(cls, name: str) -> MotorCommand:
        """Get motor command from its string name."""
        try:
            return MotorCommand[name.upper()]
        except KeyError:
            raise ValueError(f"Unknown motor command: {name}")
    
    @classmethod
    def to_string(cls, command: MotorCommand) -> str:
        """Convert motor command to string name."""
        return command.name.lower().replace('_', ' ')

class DevelopmentalStages:
    """Convenience class for developmental stage operations."""
    
    INITIAL = DevelopmentalStage.Initial
    CRITICAL_PERIOD = DevelopmentalStage.CriticalPeriod
    MATURATION = DevelopmentalStage.Maturation
    ADULT = DevelopmentalStage.Adult
    AGING = DevelopmentalStage.Aging
    
    @classmethod
    def get_description(cls, stage: DevelopmentalStage) -> str:
        """Get human-readable description of developmental stage."""
        descriptions = {
            DevelopmentalStage.Initial: "Initial development phase",
            DevelopmentalStage.CriticalPeriod: "Critical period for learning",
            DevelopmentalStage.Maturation: "Developmental maturation",
            DevelopmentalStage.Adult: "Adult stage with stable behavior",
            DevelopmentalStage.Aging: "Age-related decline phase"
        }
        return descriptions.get(stage, "Unknown stage")
    
    @classmethod
    def get_plasticity_modifier(cls, stage: DevelopmentalStage) -> float:
        """Get plasticity modifier for a developmental stage."""
        modifiers = {
            DevelopmentalStage.Initial: 1.0,      # High plasticity
            DevelopmentalStage.CriticalPeriod: 0.8,
            DevelopmentalStage.Maturation: 0.5,
            DevelopmentalStage.Adult: 0.2,      # Low plasticity
            DevelopmentalStage.Aging: 0.1       # Very low plasticity
        }
        return modifiers.get(stage, 1.0)

class NeuronTypes:
    """Convenience class for neuron type operations."""
    
    EXCITATORY = NeuronType.Excitatory
    INHIBITORY = NeuronType.Inhibitory
    SENSORY = NeuronType.Sensory
    MOTOR = NeuronType.Motor
    INTERNAL = NeuronType.Internal
    MODULATORY = NeuronType.Modulatory
    
    @classmethod
    def get_description(cls, neuron_type: NeuronType) -> str:
        """Get description of neuron type."""
        descriptions = {
            NeuronType.Excitatory: "Excitatory neuron (positive effect)",
            NeuronType.Inhibitory: "Inhibitory neuron (negative effect)",
            NeuronType.Sensory: "Sensory neuron (receives input)",
            NeuronType.Motor: "Motor neuron (produces output)",
            NeuronType.Internal: "Internal neuron (cognitive processing)",
            NeuronType.Modulatory: "Modulatory neuron (neuromodulation)"
        }
        return descriptions.get(neuron_type, "Unknown neuron type")

# Configuration helper functions
def create_simulation_config(
    neuron_count: int = 1000,
    region_count: int = 1,
    connection_probability: float = 0.1,
    random_seed: int = 42,
    simulation_timestep: float = 0.001,
    enable_plasticity: bool = True,
    enable_neuromodulation: bool = True,
    enable_development: bool = True,
) -> Config:
    """
    Create a configuration for neural simulation with common parameters.
    
    This is a convenience function that creates a typical configuration
    for NLM simulations, suitable for most use cases.
    
    Args:
        neuron_count: Total number of neurons in the simulation
        region_count: Number of brain regions
        connection_probability: Probability of connections between neurons
        random_seed: Seed for random number generation
        simulation_timestep: Time step for simulation (in seconds)
        enable_plasticity: Whether to enable synaptic plasticity
        enable_neuromodulation: Whether to enable neuromodulation
        enable_development: Whether to enable developmental processes
        
    Returns:
        Configured Config object ready for brain creation
        
    Example:
        >>> config = create_simulation_config(
        ...     neuron_count=500,
        ...     region_count=2,
        ...     enable_neuromodulation=True,
        ...     enable_development=True
        ... )
        >>> brain = createBrain(config)
    """
    config = createDefaultConfig()
    
    # Basic brain configuration
    config.set("brain.neuron_count", neuron_count)
    config.set("brain.region_count", region_count)
    config.set("brain.connection_probability", connection_probability)
    config.set("random_seed", random_seed)
    config.set("simulation_timestep", simulation_timestep)
    
    # Plasticity configuration
    if enable_plasticity:
        config.set("plasticity.stdp.enable", True)
        config.set("plasticity.hebbian.enable", True)
        config.set("plasticity.stdp.learning_rate", 0.01)
        config.set("plasticity.stdp.tau_plus", 20.0)
        config.set("plasticity.stdp.tau_minus", 20.0)
        config.set("plasticity.hebbian.learning_rate", 0.001)
        config.set("plasticity.structural.enable", True)
        config.set("synaptogenesis_rate", 0.0001)
        config.set("pruning_rate", 0.00001)
    
    # Neuromodulation configuration
    if enable_neuromodulation:
        config.set("neuromod.dopamine.scale", 1.0)
        config.set("neuromod.curiosity.enable", True)
        config.set("neuromod.novelty.enable", True)
    
    # Development configuration
    if enable_development:
        config.set("development.enable", True)
        config.set("development.critical_period_duration", 300.0)
        config.set("development.maturation_rate", 0.1)
    
    return config

def load_config_from_file(filepath: str) -> Config:
    """
    Load configuration from a file with error handling.
    
    Args:
        filepath: Path to configuration file
        
    Returns:
        Loaded Config object
        
    Raises:
        ConfigurationError: If file cannot be loaded or parsed
        
    Example:
        >>> try:
        ...     config = load_config_from_file("simulation.cfg")
        ... except ConfigurationError as e:
        ...     print(f"Configuration error: {e}")
    """
    try:
        config = createDefaultConfig()
        if not config.loadFromFile(filepath):
            raise ConfigurationError(f"Failed to load configuration from {filepath}")
        return config
    except Exception as e:
        if isinstance(e, ConfigurationError):
            raise
        raise ConfigurationError(f"Error loading configuration from {filepath}: {e}") from e

def save_config_to_file(config: Config, filepath: str) -> bool:
    """
    Save configuration to a file with error handling.
    
    Args:
        config: Config object to save
        filepath: Path where configuration will be saved
        
    Returns:
        True if save was successful, False otherwise
        
    Raises:
        ConfigurationError: If save operation fails
        
    Example:
        >>> config = createDefaultConfig()
        >>> config.set("brain.neuron_count", 1000)
        >>> if save_config_to_file(config, "my_simulation.cfg"):
        ...     print("Configuration saved successfully")
        ... else:
        ...     print("Failed to save configuration")
    """
    try:
        if not config.saveToFile(filepath):
            raise ConfigurationError(f"Failed to save configuration to {filepath}")
        return True
    except Exception as e:
        if isinstance(e, ConfigurationError):
            raise
        raise ConfigurationError(f"Error saving configuration to {filepath}: {e}") from e

# Simulation helper functions
def run_simple_simulation(
    num_steps: int = 1000,
    config: Optional[Config] = None,
    world: Optional[SimpleWorld] = None,
    agent: Optional[AgentBrain] = None,
) -> Dict[str, Any]:
    """
    Run a simple simulation with basic error handling.
    
    This function provides a convenient way to run a basic simulation
    without having to manage the details of the simulation loop.
    
    Args:
        num_steps: Number of simulation steps to run
        config: Optional configuration (will be created if not provided)
        world: Optional world (will be created if not provided)
        agent: Optional agent (will be created if not provided)
        
    Returns:
        Dictionary containing simulation results and metrics
        
    Raises:
        SimulationError: If simulation fails
        
    Example:
        >>> results = run_simple_simulation(
        ...     num_steps=500,
        ...     config=config
        ... )
        >>> print(f"Final firing rate: {results['average_firing_rate']:.2f}")
        >>> print(f"Total spikes: {results['total_spikes']}")
    """
    if not CPLUSPLUS_AVAILABLE:
        raise SimulationError("C++ NLM bindings not available")
    
    try:
        # Create components if not provided
        if config is None:
            config = create_simulation_config()
        if world is None:
            world = createSimpleWorld()
            world.configure(20, 20, 8, 8)
            world.reset()
        if agent is None:
            brain = createBrain(config)
            brain.initialize()
            agent = createAgentBrain(brain)
            agent.initialize(world)
            agent.enableRewardModulation(True)
            agent.enableCuriosity(True)
            agent.enableDevelopment(True)
        
        # Run simulation
        results = {
            'steps': num_steps,
            'total_reward': 0.0,
            'total_spikes': 0,
            'final_firing_rate': 0.0,
            'average_firing_rate': 0.0,
            'neurons_created': 0,
            'synapses_created': 0,
            'actions_taken': 0,
            'success': True,
            'error_message': None
        }
        
        for step in range(num_steps):
            try:
                # Get sensory input
                percept = world.getSensoryPercept()
                agent.processSensoryInput(percept)
                
                # Brain computation
                brain.step(step)
                
                # Get action
                action = agent.decodeMotorCommand()
                world.applyMotorCommand(action, world.getSimulationTime())
                
                # Get reward (if available)
                # This is simplified - in a real simulation, reward would come from world
                reward = 0.0
                
                # Apply reward modulation
                agent.applyRewardModulation(reward, 0.0)
                
                # Update results
                results['total_reward'] += reward
                results['total_spikes'] += brain.getFiringNeuronCount()
                results['actions_taken'] += 1
                
            except Exception as e:
                results['success'] = False
                results['error_message'] = str(e)
                raise SimulationError(f"Error at step {step}: {e}") from e
        
        # Calculate final statistics
        results['final_firing_rate'] = brain.getAverageFiringRate()
        results['average_firing_rate'] = results['total_spikes'] / num_steps
        results['neurons_created'] = brain.getTotalNeuronCount()
        results['synapses_created'] = brain.getTotalSynapseCount()
        
        return results
        
    except Exception as e:
        if isinstance(e, SimulationError):
            raise
        raise SimulationError(f"Simulation failed: {e}") from e

# Agent helper functions
def create_and_initialize_agent(
    config: Config,
    world: SimpleWorld,
    enable_learning: bool = True,
    enable_exploration: bool = True,
    enable_development: bool = True,
) -> AgentBrain:
    """
    Create and initialize an agent brain with typical settings.
    
    This is a convenience function that creates a brain and agent
    with typical configuration for learning and exploration.
    
    Args:
        config: Configuration for brain creation
        world: World interface for the agent
        enable_learning: Whether to enable reward-based learning
        enable_exploration: Whether to enable curiosity-driven exploration
        enable_development: Whether to enable developmental processes
        
    Returns:
        Initialized AgentBrain instance
        
    Example:
        >>> config = createDefaultConfig()
        >>> config.set("brain.neuron_count", 500)
        >>> world = createSimpleWorld()
        >>> agent = create_and_initialize_agent(config, world)
    """
    try:
        brain = createBrain(config)
        brain.initialize()
        
        agent = createAgentBrain(brain)
        agent.initialize(world)
        
        if enable_learning:
            agent.enableRewardModulation(True)
        
        if enable_exploration:
            agent.enableCuriosity(True)
        
        if enable_development:
            agent.enableDevelopment(True)
            agent.enableStructuralPlasticity(True)
        
        return agent
        
    except Exception as e:
        raise AgentError(f"Failed to create agent: {e}") from e

# Configuration validation functions
def validate_config(config: Config) -> List[str]:
    """
    Validate a configuration for common issues.
    
    Args:
        config: Configuration to validate
        
    Returns:
        List of validation error messages (empty if valid)
        
    Example:
        >>> config = createDefaultConfig()
        >>> config.set("brain.neuron_count", 0)  # Invalid
        >>> errors = validate_config(config)
        >>> if errors:
        ...     for error in errors:
        ...         print(f"Validation error: {error}")
    """
    errors = []
    
    # Check neuron count
    if config.has("brain.neuron_count"):
        neuron_count = config.getOr[int]("brain.neuron_count", 0)
        if neuron_count <= 0:
            errors.append("Neuron count must be positive")
        elif neuron_count > 100000:  # Reasonable upper limit
            errors.append("Neuron count too high (maximum 100000)")
    
    # Check region count
    if config.has("brain.region_count"):
        region_count = config.getOr[int]("brain.region_count", 0)
        if region_count <= 0:
            errors.append("Region count must be positive")
        elif region_count > 1000:
            errors.append("Region count too high (maximum 1000)")
    
    # Check connection probability
    if config.has("brain.connection_probability"):
        prob = config.getOr[float]("brain.connection_probability", -1)
        if prob < 0 or prob > 1:
            errors.append("Connection probability must be between 0 and 1")
    
    # Check timestep
    if config.has("simulation_timestep"):
        timestep = config.getOr[float]("simulation_timestep", 0)
        if timestep <= 0:
            errors.append("Simulation timestep must be positive")
        elif timestep > 1.0:  # Reasonable upper limit
            errors.append("Simulation timestep too large (maximum 1.0)")
    
    return errors

# Package metadata and utilities
__all__ = [
    # Core classes
    'Config',
    'Brain',
    'AgentBrain',
    'SimpleWorld',
    'Vision',
    'Audio',
    'InternalSignals',
    'SensoryInput',
    'Action',
    'MotorCommand',
    'ActionResult',
    'SensoryPercept',
    'WorldObject',
    'AgentBody',
    
    # Enums and types
    'NeuronType',
    'SynapseType',
    'ActionType',
    'WorldObjectType',
    'DevelopmentalStage',
    'FiringState',
    'NeuronId',
    'SynapseId',
    'RegionId',
    'PopulationId',
    'SimulationStep',
    'Timestamp',
    'TimestepDuration',
    'MembranePotential',
    'SynapticWeight',
    
    # Error types
    'NLMError',
    'ConfigurationError',
    'SimulationError',
    'AgentError',
    'MemoryError',
    
    # Convenience classes
    'MotorCommands',
    'DevelopmentalStages',
    'NeuronTypes',
    
    # Functions
    'createDefaultConfig',
    'createBrain',
    'createSimpleWorld',
    'createAgentBrain',
    
    'create_simulation_config',
    'load_config_from_file',
    'save_config_to_file',
    
    'run_simple_simulation',
    'create_and_initialize_agent',
    'validate_config',
    
    # Package metadata
    '__version__',
]

# Package initialization check
if __name__ == "__main__":
    # This is just a module, not a script
    pass
