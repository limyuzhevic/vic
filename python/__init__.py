"""
pynlm - NLM (Neural Learning Machine) Python Bindings

A Python interface to the NLM C++ neural simulation framework.
Provides classes for Brain, Config, AgentBrain, SimpleWorld, SensoryInput, and Action.

This module provides a high-level API for creating and simulating neural brains
with event-driven spiking dynamics, synaptic plasticity, and world interaction.

Key features:
- Intuitive, Pythonic API design
- Comprehensive error handling with descriptive messages
- Type hints for better IDE support
- Extensive documentation and examples
- Advanced utilities for both beginners and experts
- Seamless integration with Python ecosystem

Examples:
    >>> import pynlm
    >>> # Create a brain with custom configuration
    >>> config = pynlm.createDefaultConfig()
    >>> config.set("brain.neuron_count", 2000)
    >>> config.set("plasticity.stdp.enable", True)
    >>> brain = pynlm.createBrain(config)
    >>> brain.initialize()
    >>> # Run simulation
    >>> for step in range(100):
    ...     brain.step(step)
    >>> print(f"Spikes: {brain.getTotalSpikeCount()}")
"""

# Module metadata
__version__ = "0.1.0"
__author__ = "NLM Authors"
__description__ = "Python bindings for the NLM neural simulation framework"

# Import core classes and functions from the C++ bindings
from pynlm import (
    # Core classes
    Config,
    Brain,
    AgentBrain,
    SimpleWorld,
    SensoryInput,
    Vision,
    Audio,
    InternalSignals,
    Action,
    WorldObject,
    AgentBody,
    ActionResult,
    SensoryPercept,
    
    # Enum types
    NeuronType,
    SynapseType,
    DevelopmentalStage,
    FiringState,
    ActionType,
    MotorCommand,
    WorldObjectType,
    
    # ID types
    NeuronId,
    SynapseId,
    RegionId,
    PopulationId,
    
    # Constants
    INVALID_NEURON_ID,
    INVALID_SYNAPSE_ID,
    INVALID_REGION_ID,
    INVALID_POPULATION_ID,
    
    # Module-level functions
    createDefaultConfig,
    createBrain,
    createSimpleWorld,
    createAgentBrain,
)

# Type aliases for convenience
from typing import List, Dict, Tuple, Optional, Union, Any, Callable

FloatArray = List[float]
IntArray = List[int]
Matrix = List[List[float]]
ConfigDict = Dict[str, Union[int, float, bool, str, List[str]]]
Vector3D = Tuple[float, float, float]
Color = Tuple[float, float, float, float]

# Public API exports
__all__ = [
    # Core classes
    'Config',
    'Brain',
    'AgentBrain',
    'SimpleWorld',
    'SensoryInput',
    'Vision',
    'Audio',
    'InternalSignals',
    'Action',
    'WorldObject',
    'AgentBody',
    'ActionResult',
    'SensoryPercept',
    
    # Enum types
    'NeuronType',
    'SynapseType',
    'DevelopmentalStage',
    'FiringState',
    'ActionType',
    'MotorCommand',
    'WorldObjectType',
    
    # ID types
    'NeuronId',
    'SynapseId',
    'RegionId',
    'PopulationId',
    
    # Constants
    'INVALID_NEURON_ID',
    'INVALID_SYNAPSE_ID',
    'INVALID_REGION_ID',
    'INVALID_POPULATION_ID',
    
    # Type aliases
    'FloatArray',
    'IntArray',
    'Matrix',
    'ConfigDict',
    'Vector3D',
    'Color',
    
    # Module functions
    'createDefaultConfig',
    'createBrain',
    'createSimpleWorld',
    'createAgentBrain',
    'get_version',
    'configure_logging',
    'validate_config',
    'list_available_configs',
    
    # Version info
    '__version__',
    '__author__',
    '__description__',
]

# Global logger
import logging
from logging.handlers import RotatingFileHandler
import sys

def _get_logger():
    """Get or create the module logger."""
    logger = logging.getLogger('pynlm')
    if not logger.handlers:
        # Add console handler
        console_handler = logging.StreamHandler(sys.stdout)
        console_handler.setLevel(logging.INFO)
        formatter = logging.Formatter(
            '%(asctime)s - %(name)s - %(levelname)s - %(message)s'
        )
        console_handler.setFormatter(formatter)
        logger.addHandler(console_handler)
    
    return logger

logger = _get_logger()

def get_version() -> str:
    """Get the NLM Python bindings version.
    
    Returns:
        Version string in semantic versioning format.
        
    Example:
        >>> import pynlm
        >>> version = pynlm.get_version()
        >>> print(f"NLM version: {version}")
    """
    logger.debug(f"Getting NLM Python bindings version: {__version__}")
    return __version__

def configure_logging(
    level: str = 'INFO',
    log_file: Optional[str] = None,
    max_bytes: int = 10 * 1024 * 1024,  # 10MB
    backup_count: int = 5
) -> None:
    """Configure logging for NLM operations.
    
    Args:
        level: Logging level (DEBUG, INFO, WARNING, ERROR, CRITICAL)
        log_file: Optional file to log to (if None, only console logging)
        max_bytes: Maximum size of log file before rotation
        backup_count: Number of backup log files to keep
        
    Raises:
        ValueError: If invalid logging level is provided
        
    Example:
        >>> import pynlm
        >>> # Configure detailed logging
        >>> pynlm.configure_logging('DEBUG', 'nlm_simulation.log')
        >>> # Configure only warnings and errors
        >>> pynlm.configure_logging('WARNING')
    """
    numeric_level = getattr(logging, level.upper(), None)
    if not isinstance(numeric_level, int):
        raise ValueError(f"Invalid logging level: {level}")
    
    # Remove existing handlers
    logger.handlers.clear()
    
    # Add console handler
    console_handler = logging.StreamHandler(sys.stdout)
    console_handler.setLevel(numeric_level)
    console_formatter = logging.Formatter(
        '%(asctime)s - %(name)s - %(levelname)s - %(message)s'
    )
    console_handler.setFormatter(console_formatter)
    logger.addHandler(console_handler)
    
    # Add file handler if specified
    if log_file:
        file_handler = RotatingFileHandler(
            log_file, maxBytes=max_bytes, backupCount=backup_count
        )
        file_handler.setLevel(numeric_level)
        file_formatter = logging.Formatter(
            '%(asctime)s - %(name)s - %(levelname)s - %(message)s'
        )
        file_handler.setFormatter(file_formatter)
        logger.addHandler(file_handler)
    
    logger.setLevel(numeric_level)
    logger.info(f"Logging configured at level: {level}")
    if log_file:
        logger.info(f"Logging to file: {log_file}")

def validate_config(config_dict: ConfigDict) -> bool:
    """Validate a configuration dictionary.
    
    Args:
        config_dict: Configuration dictionary to validate
        
    Returns:
        True if valid
        
    Raises:
        ValidationError: If configuration is invalid
        
    Example:
        >>> import pynlm
        >>> # Validate a configuration
        >>> config = {
        ...     "brain.neuron_count": 1000,
        ...     "brain.synapse_density": 0.1,
        ...     "plasticity.stdp.enable": True
        ... }
        >>> if pynlm.validate_config(config):
        ...     print("Configuration is valid")
    """
    logger.debug(f"Validating configuration: {config_dict}")
    
    if not isinstance(config_dict, dict):
        raise ValidationError("Configuration must be a dictionary")
    
    # Basic validation rules
    allowed_keys = [
        'brain.neuron_count', 'brain.synapse_density', 'brain.connection_probability',
        'brain.initial_weight_mean', 'brain.initial_weight_std',
        'brain.v_thresh', 'brain.v_rest', 'brain.v_reset',
        'brain.tau_mem', 'brain.tau_ref',
        'plasticity.stdp.enable', 'plasticity.stdp.learning_rate',
        'plasticity.stdp.tau_plus', 'plasticity.stdp.tau_minus',
        'plasticity.hebbian.enable',
        'plasticity.structural.enable',
        'neuromod.dopamine.scale',
        'neuromod.curiosity.enable', 'neuromod.novelty.enable'
    ]
    
    # Validate types
    for key, value in config_dict.items():
        if key in ['brain.neuron_count', 'brain.connection_probability']:
            if not isinstance(value, int) or value < 0:
                raise ValidationError(f"Invalid value for {key}: {value}. Must be non-negative integer.")
        elif key in ['brain.synapse_density', 'brain.initial_weight_mean', 'brain.initial_weight_std',
                     'brain.v_thresh', 'brain.v_rest', 'brain.v_reset', 'brain.tau_mem', 'brain.tau_ref',
                     'plasticity.stdp.learning_rate', 'plasticity.stdp.tau_plus', 'plasticity.stdp.tau_minus',
                     'neuromod.dopamine.scale']:
            if not isinstance(value, (int, float)):
                raise ValidationError(f"Invalid value for {key}: {value}. Must be a number.")
            if value < 0:
                raise ValidationError(f"Invalid value for {key}: {value}. Must be non-negative.")
        elif key in ['plasticity.stdp.enable', 'plasticity.hebbian.enable',
                     'plasticity.structural.enable', 'neuromod.curiosity.enable', 'neuromod.novelty.enable']:
            if not isinstance(value, bool):
                raise ValidationError(f"Invalid value for {key}: {value}. Must be boolean.")
        elif key not in allowed_keys:
            logger.warning(f"Unknown configuration key: {key}")
    
    logger.debug("Configuration validation successful")
    return True

def list_available_configs() -> List[str]:
    """List all available configuration options.
    
    Returns:
        List of available configuration option strings
        
    Example:
        >>> import pynlm
        >>> configs = pynlm.list_available_configs()
        >>> for config in configs:
        ...     print(config)
    """
    logger.debug("Listing available configuration options")
    
    available_configs = [
        'brain.neuron_count', 'brain.synapse_density', 'brain.connection_probability',
        'brain.initial_weight_mean', 'brain.initial_weight_std',
        'brain.v_thresh', 'brain.v_rest', 'brain.v_reset',
        'brain.tau_mem', 'brain.tau_ref',
        'plasticity.stdp.enable', 'plasticity.stdp.learning_rate',
        'plasticity.stdp.tau_plus', 'plasticity.stdp.tau_minus',
        'plasticity.hebbian.enable',
        'plasticity.structural.enable',
        'neuromod.dopamine.scale',
        'neuromod.curiosity.enable', 'neuromod.novelty.enable'
    ]
    
    logger.debug(f"Found {len(available_configs)} configuration options")
    return available_configs

def create_simulation_summary(
    brain: Optional[Brain] = None,
    world: Optional[SimpleWorld] = None,
    agent: Optional[AgentBrain] = None,
    steps_completed: int = 0
) -> Dict[str, Any]:
    """Create a comprehensive summary of a simulation state.
    
    Args:
        brain: The brain instance (optional)
        world: The world instance (optional)
        agent: The agent instance (optional)
        steps_completed: Number of simulation steps completed
        
    Returns:
        Dictionary containing simulation summary
        
    Example:
        >>> import pynlm
        >>> # Create simulation components
        >>> config = pynlm.createDefaultConfig()
        >>> brain = pynlm.createBrain(config)
        >>> world = pynlm.createSimpleWorld()
        >>> agent = pynlm.createAgentBrain(brain)
        >>> # Run simulation
        >>> for step in range(100):
        ...     world.update(0.1)
        ...     agent.processSensoryInput(world.getSensoryPercept())
        ...     brain.step(step)
        ...     action = agent.decodeMotorCommand()
        ...     world.applyMotorCommand(action, world.getSimulationTime())
        >>> # Get simulation summary
        >>> summary = pynlm.create_simulation_summary(brain, world, agent, 100)
        >>> print(f"Simulation summary: {summary}")
    """
    logger.debug(f"Creating simulation summary after {steps_completed} steps")
    
    summary = {
        'steps_completed': steps_completed,
        'timestamp': None,  # Could add timestamp if available
    }
    
    # Add brain statistics if brain is provided
    if brain:
        summary['brain'] = {
            'total_neurons': brain.getTotalNeuronCount(),
            'total_synapses': brain.getTotalSynapseCount(),
            'firing_neurons': brain.getFiringNeuronCount(),
            'total_spikes': brain.getTotalSpikeCount(),
            'average_firing_rate': brain.getAverageFiringRate(),
            'excitation_inhibition_ratio': brain.getExcitationInhibitionRatio(),
            'developmental_stage': brain.getDevelopmentalStage(),
        }
    
    # Add world statistics if world is provided
    if world:
        summary['world'] = {
            'width': world.getWidth(),
            'height': world.getHeight(),
            'simulation_time': world.getSimulationTime(),
            'max_energy': world.getMaxEnergy(),
            'agent_position': (world.getAgentBody().x, world.getAgentBody().y),
            'agent_orientation': world.getAgentBody().orientation,
        }
    
    # Add agent statistics if agent is provided
    if agent:
        summary['agent'] = {
            'curiosity_level': agent.getCuriosityLevel(),
            'novelty_level': agent.getNoveltyLevel(),
            'prediction_error': agent.getPredictionError(),
            'neuromodulation_level': agent.getNeuromodulationLevel(),
            'developmental_stage': agent.getDevelopmentalStage(),
        }
    
    logger.debug("Simulation summary created successfully")
    return summary

def run_demo_simulation(steps: int = 100) -> Dict[str, Any]:
    """Run a complete demo simulation with all components.
    
    Args:
        steps: Number of simulation steps to run
        
    Returns:
        Simulation summary
        
    Example:
        >>> import pynlm
        >>> # Run a demo simulation
        >>> summary = pynlm.run_demo_simulation(200)
        >>> print(f"Simulation completed: {summary['steps_completed']} steps")
        >>> print(f"Final spike count: {summary['brain']['total_spikes']}")
    """
    logger.info(f"Running demo simulation with {steps} steps")
    
    try:
        # Create components
        config = pynlm.createDefaultConfig()
        config.set("brain.neuron_count", 1000)
        config.set("plasticity.stdp.enable", True)
        config.set("neuromod.curiosity.enable", True)
        
        brain = pynlm.createBrain(config)
        world = pynlm.createSimpleWorld()
        agent = pynlm.createAgentBrain(brain)
        
        # Configure and initialize
        world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
        world.reset()
        agent.initialize(world)
        brain.initialize()
        
        # Enable learning
        agent.enableRewardModulation(True)
        agent.enableCuriosity(True)
        
        # Run simulation
        for step in range(steps):
            world.update(0.1)
            
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            brain.step(step)
            
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            
            reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
            agent.applyRewardModulation(reward, 0.0)
            
            agent.updateDevelopment(0.1)
            
            if step % 50 == 0:
                logger.info(f"Step {step} completed")
        
        # Create summary
        summary = create_simulation_summary(brain, world, agent, steps)
        logger.info("Demo simulation completed successfully")
        return summary
        
    except Exception as e:
        logger.error(f"Error in demo simulation: {e}")
        raise

# Exception classes for better error handling
class NLMError(Exception):
    """Base exception for all NLM-related errors."""
    pass

class ConfigurationError(NLMError):
    """Raised when there's an error with configuration."""
    pass

class SimulationError(NLMError):
    """Raised when there's an error during simulation."""
    pass

class ValidationError(NLMError):
    """Raised when validation fails."""
    pass

# Make exception classes available in module namespace
import sys

sys.modules[__name__].NLMError = NLMError
sys.modules[__name__].ConfigurationError = ConfigurationError
sys.modules[__name__].SimulationError = SimulationError
sys.modules[__name__].ValidationError = ValidationError