'''NLM Python Package

This package provides Python bindings for the NLM (Neural Learning Machine) 
neural simulation framework. It offers a complete API for creating, simulating, 
and controlling artificial brains that learn from experience.

The package includes both the core C++ bindings and additional Python utilities
for enhanced usability and documentation.

Usage:
    import pynlm

    # Create a basic brain
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Run simulation
    for i in range(100):
        brain.step(i)
    
    print(f"Total neurons: {brain.getTotalNeuronCount()}")
    print(f"Total spikes: {brain.getTotalSpikeCount()}")
'''

import sys
from typing import Tuple, Optional, List, Dict, Any
import warnings

# Try to import the core C++ module
try:
    import pynlm_core as _pynlm_core
    _HAS_PYNLM = True
except ImportError:
    _HAS_PYNLM = False
    warnings.warn(
        "pynlm_core not found. Please ensure NLM is properly installed. "
        "Only basic functionality will be available.",
        ImportWarning
    )

# Version information
__version__ = "0.1.0"
__author__ = "NLM Project Authors"
__license__ = "MIT"

# Module-level constants
DEFAULT_NEURON_COUNT = 1000
DEFAULT_SYNAPSE_DENSITY = 0.1
DEFAULT_V_THRESH = -50.0
DEFAULT_V_REST = -70.0
DEFAULT_MAX_ENERGY = 100.0

# Type aliases for better documentation
NeuronId = Any
SynapseId = Any
RegionId = Any
PopulationId = Any

# Exception classes
class NLMError(Exception):
    """Base exception for all NLM errors."""
    pass

class ConfigurationError(NLMError):
    """Error in configuration."""
    pass

class SimulationError(NLMError):
    """Error during simulation."""
    pass

class ValidationError(NLMError):
    """Validation error in input parameters."""
    pass

# Configuration utilities
def create_identity_config() -> Any:
    """
    Create a configuration with standard NLM parameters.
    
    Returns:
        Config: A configuration object with typical NLM settings.
        
    Example:
        >>> config = pynlm.create_identity_config()
        >>> config.setFloat("brain.neuron_count", 2000)
    """
    if not _HAS_PYNLM:
        raise RuntimeError("pynlm_core not available")
    return _pynlm_core.createDefaultConfig()

def validate_config(config: Any) -> bool:
    """
    Validate a configuration object.
    
    Args:
        config: Configuration object to validate.
        
    Returns:
        bool: True if configuration is valid, False otherwise.
        
    Example:
        >>> config = pynlm.create_identity_config()
        >>> is_valid = pynlm.validate_config(config)
        >>> print(f"Config valid: {is_valid}")
    """
    if not _HAS_PYNLM:
        return False
    try:
        # Try to access configuration properties
        keys = config.getKeys()
        return len(keys) > 0
    except Exception:
        return False

# Brain utilities
def create_brain_from_config(config: Any) -> Any:
    """
    Create a brain from a configuration.
    
    Args:
        config: Configuration object.
        
    Returns:
        Brain: A brain object.
        
    Raises:
        ConfigurationError: If configuration is invalid.
        
    Example:
        >>> config = pynlm.create_identity_config()
        >>> brain = pynlm.create_brain_from_config(config)
        >>> brain.initialize()
    """
    if not _HAS_PYNLM:
        raise RuntimeError("pynlm_core not available")
    if not config:
        raise ConfigurationError("Configuration cannot be None")
    return _pynlm_core.createBrain(config)

def create_brain_with_default_config(neuron_count: int = DEFAULT_NEURON_COUNT) -> Any:
    """
    Create a brain with default configuration.
    
    Args:
        neuron_count: Number of neurons for the brain.
        
    Returns:
        Brain: A brain object with default configuration.
        
    Example:
        >>> brain = pynlm.create_brain_with_default_config(2000)
        >>> brain.initialize()
    """
    if not _HAS_PYNLM:
        raise RuntimeError("pynlm_core not available")
    
    config = _pynlm_core.createDefaultConfig()
    config.setInt("brain.neuron_count", neuron_count)
    return _pynlm_core.createBrain(config)

def run_brain_simulation(brain: Any, num_steps: int) -> Dict[str, Any]:
    """
    Run a complete brain simulation and return statistics.
    
    Args:
        brain: Brain object to simulate.
        num_steps: Number of simulation steps to run.
        
    Returns:
        Dict: Dictionary containing simulation statistics.
        
    Example:
        >>> brain = pynlm.create_brain_with_default_config()
        >>> brain.initialize()
        >>> stats = pynlm.run_brain_simulation(brain, 1000)
        >>> print(f"Firing rate: {stats['firing_rate']:.2f} Hz")
    """
    if not _HAS_PYNLM:
        raise RuntimeError("pynlm_core not available")
    if not brain:
        raise SimulationError("Brain cannot be None")
    
    brain.initialize()
    
    for i in range(num_steps):
        brain.step(i)
    
    # Collect statistics
    stats = {
        'total_neurons': brain.getTotalNeuronCount(),
        'total_synapses': brain.getTotalSynapseCount(),
        'firing_neurons': brain.getFiringNeuronCount(),
        'active_neurons': brain.getActiveNeuronCount(),
        'total_spikes': brain.getTotalSpikeCount(),
        'average_firing_rate': brain.getAverageFiringRate(),
        'excitation_inhibition_ratio': brain.getExcitationInhibitionRatio(),
        'developmental_stage': brain.getDevelopmentalStage(),
        'is_healthy': brain.isHealthy() if hasattr(brain, 'isHealthy') else False,
        'simulation_steps': num_steps
    }
    
    return stats

def compare_brain_states(brain1: Any, brain2: Any) -> Dict[str, Any]:
    """
    Compare two brain states and return differences.
    
    Args:
        brain1: First brain object.
        brain2: Second brain object.
        
    Returns:
        Dict: Dictionary containing comparison results.
        
    Example:
        >>> brain1 = pynlm.create_brain_with_default_config()
        >>> brain2 = pynlm.create_brain_with_default_config()
        >>> comparison = pynlm.compare_brain_states(brain1, brain2)
        >>> print(f"Spike difference: {comparison['spike_difference']}")
    """
    if not _HAS_PYNLM:
        raise RuntimeError("pynlm_core not available")
    
    comparison = {
        'neuron_count_match': brain1.getTotalNeuronCount() == brain2.getTotalNeuronCount(),
        'synapse_count_match': brain1.getTotalSynapseCount() == brain2.getTotalSynapseCount(),
        'firing_neuron_count_match': brain1.getFiringNeuronCount() == brain2.getFiringNeuronCount(),
        'active_neuron_count_match': brain1.getActiveNeuronCount() == brain2.getActiveNeuronCount(),
        'total_spike_count_match': brain1.getTotalSpikeCount() == brain2.getTotalSpikeCount(),
        'average_firing_rate_match': brain1.getAverageFiringRate() == brain2.getAverageFiringRate(),
        'excitation_inhibition_ratio_match': brain1.getExcitationInhibitionRatio() == brain2.getExcitationInhibitionRatio(),
        'developmental_stage_match': brain1.getDevelopmentalStage() == brain2.getDevelopmentalStage(),
        'statistical_similarity': None
    }
    
    # Calculate statistical similarity score (0-1, where 1 means identical)
    total_stats = 0
    matching_stats = 0
    
    if comparison['neuron_count_match']:
        matching_stats += 1
    total_stats += 1
    
    if comparison['synapse_count_match']:
        matching_stats += 1
    total_stats += 1
    
    if comparison['firing_neuron_count_match']:
        matching_stats += 1
    total_stats += 1
    
    if comparison['active_neuron_count_match']:
        matching_stats += 1
    total_stats += 1
    
    if comparison['total_spike_count_match']:
        matching_stats += 1
    total_stats += 1
    
    if comparison['average_firing_rate_match']:
        matching_stats += 1
    total_stats += 1
    
    if comparison['excitation_inhibition_ratio_match']:
        matching_stats += 1
    total_stats += 1
    
    if comparison['developmental_stage_match']:
        matching_stats += 1
    total_stats += 1
    
    if total_stats > 0:
        comparison['statistical_similarity'] = matching_stats / total_stats
    
    return comparison

# World utilities
def create_simulation_environment(width: int = 20, height: int = 20,
                                  energy_decay: float = 0.01,
                                  max_energy: float = DEFAULT_MAX_ENERGY) -> Any:
    """
    Create a simulated environment for agent testing.
    
    Args:
        width: Width of the environment.
        height: Height of the environment.
        energy_decay: Energy decay rate per timestep.
        max_energy: Maximum energy for agents.
        
    Returns:
        SimpleWorld: A world object for simulation.
        
    Example:
        >>> world = pynlm.create_simulation_environment(width=50, height=50)
        >>> world.configure(width=50, height=50, visionWidth=8, visionHeight=8)
        >>> world.reset()
    """
    if not _HAS_PYNLM:
        raise RuntimeError("pynlm_core not available")
    
    world = _pynlm_core.createSimpleWorld()
    world.configure(width, height, 8, 8)
    world.setMaxEnergy(max_energy)
    world.setEnergyDecayRate(energy_decay)
    world.reset()
    return world

def create_agent_brain(brain: Any, world: Any) -> Any:
    """
    Create an agent brain interface for brain-world interaction.
    
    Args:
        brain: Brain object to connect to world.
        world: World object to connect to brain.
        
    Returns:
        AgentBrain: Agent brain interface object.
        
    Example:
        >>> brain = pynlm.create_brain_with_default_config()
        >>> world = pynlm.create_simulation_environment()
        >>> agent = pynlm.create_agent_brain(brain, world)
        >>> agent.initialize(world)
    """
    if not _HAS_PYNLM:
        raise RuntimeError("pynlm_core not available")
    
    agent = _pynlm_core.createAgentBrain(brain)
    agent.initialize(world)
    return agent

# Complete agent setup
def create_complete_agent(width: int = 20, height: int = 20,
                         neuron_count: int = DEFAULT_NEURON_COUNT) -> Tuple[Any, Any, Any]:
    """
    Create a complete agent setup with brain, world, and agent interface.
    
    Args:
        width: Width of the simulation world.
        height: Height of the simulation world.
        neuron_count: Number of neurons for the brain.
        
    Returns:
        Tuple: (brain, world, agent) where:
            - brain: Brain object
            - world: World object
            - agent: Agent brain interface object
            
    Example:
        >>> brain, world, agent = pynlm.create_complete_agent(width=30, height=30)
        >>> agent.enableRewardModulation(True)
        >>> agent.enableCuriosity(True)
    """
    if not _HAS_PYNLM:
        raise RuntimeError("pynlm_core not available")
    
    # Create brain
    config = _pynlm_core.createDefaultConfig()
    config.setInt("brain.neuron_count", neuron_count)
    brain = _pynlm_core.createBrain(config)
    brain.initialize()
    
    # Create world
    world = _pynlm_core.createSimpleWorld()
    world.configure(width, height, 8, 8)
    world.reset()
    
    # Create agent
    agent = _pynlm_core.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable common subsystems
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    agent.enableDevelopment(True)
    
    return brain, world, agent

# Simulation utilities
def run_agent_episode(agent: Any, world: Any, max_steps: int) -> Dict[str, Any]:
    """
    Run a complete agent episode in the world.
    
    Args:
        agent: Agent brain interface.
        world: World object.
        max_steps: Maximum number of steps for the episode.
        
    Returns:
        Dict: Dictionary containing episode statistics.
        
    Example:
        >>> brain, world, agent = pynlm.create_complete_agent()
        >>> episode_stats = pynlm.run_agent_episode(agent, world, 1000)
        >>> print(f"Episode finished in {episode_stats['steps']} steps")
        >>> print(f"Final curiosity: {episode_stats['final_curiosity']:.3f}")
    """
    if not _HAS_PYNLM:
        raise RuntimeError("pynlm_core not available")
    
    agent.reset()
    world.reset()
    
    episode_stats = {
        'steps': 0,
        'total_reward': 0.0,
        'max_curiosity': 0.0,
        'max_novelty': 0.0,
        'final_curiosity': 0.0,
        'final_novelty': 0.0,
        'final_prediction_error': 0.0,
        'neural_activity': [],
        'brain_states': []
    }
    
    for step in range(max_steps):
        # Get sensory input
        percept = world.getSensoryPercept()
        
        # Process sensory input
        agent.processSensoryInput(percept)
        
        # Get brain step
        agent.getBrain().step(step)
        
        # Get action from brain
        action = agent.decodeMotorCommand()
        
        # Apply action to world
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Get reward from world
        # Note: This is a simplified reward calculation
        reward = 0.0
        if percept.getInternal():
            reward = percept.getInternal()[0] if len(percept.getInternal()) > 0 else 0.0
        
        # Apply reward modulation
        agent.applyRewardModulation(reward, 0.0)
        
        # Update development
        agent.updateDevelopment(0.1)
        
        # Collect statistics
        episode_stats['steps'] += 1
        episode_stats['total_reward'] += reward
        episode_stats['max_curiosity'] = max(episode_stats['max_curiosity'], agent.getCuriosityLevel())
        episode_stats['max_novelty'] = max(episode_stats['max_novelty'], agent.getNoveltyLevel())
        episode_stats['final_curiosity'] = agent.getCuriosityLevel()
        episode_stats['final_novelty'] = agent.getNoveltyLevel()
        episode_stats['final_prediction_error'] = agent.getPredictionError()
        
        episode_stats['neural_activity'].append({
            'firing_neurons': agent.getBrain().getFiringNeuronCount(),
            'active_neurons': agent.getBrain().getActiveNeuronCount(),
            'total_spikes': agent.getBrain().getTotalSpikeCount(),
            'curiosity': agent.getCuriosityLevel(),
            'novelty': agent.getNoveltyLevel(),
            'prediction_error': agent.getPredictionError()
        })
        
        episode_stats['brain_states'].append({
            'developmental_stage': agent.getBrain().getDevelopmentalStage(),
            'excitation_inhibition_ratio': agent.getBrain().getExcitationInhibitionRatio()
        })
    
    return episode_stats

def run_batch_experiments(num_experiments: int, steps_per_experiment: int) -> List[Dict[str, Any]]:
    """
    Run multiple agent experiments and collect results.
    
    Args:
        num_experiments: Number of experiments to run.
        steps_per_experiment: Number of steps per experiment.
        
    Returns:
        List[Dict]: List of experiment results.
        
    Example:
        >>> experiments = pynlm.run_batch_experiments(5, 500)
        >>> for i, exp in enumerate(experiments):
        ...     print(f"Experiment {i+1}: {exp['steps']} steps, "
        ...           f"curiosity: {exp['final_curiosity']:.3f}")
    """
    experiments = []
    
    for exp_num in range(num_experiments):
        print(f"Running experiment {exp_num + 1}/{num_experiments}...")
        
        brain, world, agent = create_complete_agent()
        agent.enableRewardModulation(True)
        agent.enableCuriosity(True)
        agent.enableDevelopment(True)
        
        experiment_result = run_agent_episode(agent, world, steps_per_experiment)
        experiment_result['experiment_id'] = exp_num + 1
        experiment_result['configuration'] = {
            'neurons': brain.getTotalNeuronCount(),
            'synapses': brain.getTotalSynapseCount(),
            'world_width': world.getWidth(),
            'world_height': world.getHeight()
        }
        
        experiments.append(experiment_result)
        
        # Clean up for next experiment
        del agent
        del world
        del brain
    
    return experiments

# Utility functions
def print_simulation_summary(brain: Any, world: Any, agent: Any) -> str:
    """
    Print a comprehensive summary of a simulation.
    
    Args:
        brain: Brain object.
        world: World object.
        agent: Agent brain interface.
        
    Returns:
        str: Formatted summary string.
        
    Example:
        >>> brain, world, agent = pynlm.create_complete_agent()
        >>> summary = pynlm.print_simulation_summary(brain, world, agent)
        >>> print(summary)
    """
    summary = "NLM Simulation Summary\n"
    summary += "=" * 50 + "\n"
    summary += f"Brain Statistics:\n"
    summary += f"  Total Neurons: {brain.getTotalNeuronCount()}\n"
    summary += f"  Total Synapses: {brain.getTotalSynapseCount()}\n"
    summary += f"  Firing Neurons: {brain.getFiringNeuronCount()}\n"
    summary += f"  Active Neurons: {brain.getActiveNeuronCount()}\n"
    summary += f"  Total Spikes: {brain.getTotalSpikeCount()}\n"
    summary += f"  Average Firing Rate: {brain.getAverageFiringRate():.2f} Hz\n"
    summary += f"  Excitation/Inhibition Ratio: {brain.getExcitationInhibitionRatio():.3f}\n"
    summary += f"  Developmental Stage: {brain.getDevelopmentalStage()}\n"
    summary += f"\nWorld Statistics:\n"
    summary += f"  Width: {world.getWidth()}, Height: {world.getHeight()}\n"
    summary += f"  Agent Position: ({world.getAgentBody().x:.1f}, {world.getAgentBody().y:.1f})\n"
    summary += f"  Agent Energy: {world.getAgentBody().energy:.1f}/{world.getMaxEnergy()}\n"
    summary += f"  Simulation Time: {world.getSimulationTime():.1f}\n"
    summary += f"\nAgent Statistics:\n"
    summary += f"  Curiosity Level: {agent.getCuriosityLevel():.3f}\n"
    summary += f"  Novelty Level: {agent.getNoveltyLevel():.3f}\n"
    summary += f"  Prediction Error: {agent.getPredictionError():.3f}\n"
    summary += f"  Neuromodulation Level: {agent.getNeuromodulationLevel():.3f}\n"
    summary += f"  Reward Modulation Enabled: {agent.isRewardModulationEnabled()}\n"
    summary += f"  Curiosity Enabled: {agent.isCuriosityEnabled()}\n"
    summary += f"  Development Enabled: {agent.isDevelopmentEnabled()}\n"
    
    return summary

def validate_agent_setup(brain: Any, world: Any, agent: Any) -> Dict[str, Any]:
    """
    Validate a complete agent setup for potential issues.
    
    Args:
        brain: Brain object.
        world: World object.
        agent: Agent brain interface.
        
    Returns:
        Dict: Dictionary containing validation results and any issues found.
        
    Example:
        >>> brain, world, agent = pynlm.create_complete_agent()
        >>> validation = pynlm.validate_agent_setup(brain, world, agent)
        >>> if not validation['is_valid']:
        ...     for issue in validation['issues']:
        ...         print(f"Issue: {issue}")
    """
    issues = []
    warnings_list = []
    
    # Check brain health
    if not brain.isHealthy() if hasattr(brain, 'isHealthy') else True:
        issues.append("Brain is not in a healthy state")
    
    # Check world configuration
    if world.getWidth() <= 0 or world.getHeight() <= 0:
        issues.append("World dimensions must be positive")
    
    # Check agent initialization
    try:
        percept = world.getSensoryPercept()
        if not percept:
            warnings_list.append("Sensory percept is empty")
    except Exception as e:
        issues.append(f"Agent initialization failed: {str(e)}")
    
    # Check developmental stage
    dev_stage = brain.getDevelopmentalStage()
    if dev_stage.name == "Initial":
        warnings_list.append("Brain is still in initial developmental stage - may need time to mature")
    
    # Check neural activity levels
    firing_rate = brain.getAverageFiringRate()
    if firing_rate < 0.1:
        warnings_list.append("Very low neural activity - brain may need input to activate")
    elif firing_rate > 100:
        warnings_list.append("Very high neural activity - may indicate instability")
    
    is_valid = len(issues) == 0
    
    return {
        'is_valid': is_valid,
        'issues': issues,
        'warnings': warnings_list,
        'brain_neuron_count': brain.getTotalNeuronCount(),
        'brain_synapse_count': brain.getTotalSynapseCount(),
        'world_dimensions': (world.getWidth(), world.getHeight()),
        'agent_curiosity': agent.getCuriosityLevel(),
        'agent_novelty': agent.getNoveltyLevel()
    }

# Module metadata
__all__ = [
    # Version and metadata
    '__version__',
    '__author__',
    '__license__',
    
    # Constants
    'DEFAULT_NEURON_COUNT',
    'DEFAULT_SYNAPSE_DENSITY',
    'DEFAULT_V_THRESH',
    'DEFAULT_V_REST',
    'DEFAULT_MAX_ENERGY',
    
    # Type aliases
    'NeuronId',
    'SynapseId',
    'RegionId',
    'PopulationId',
    
    # Exception classes
    'NLMError',
    'ConfigurationError',
    'SimulationError',
    'ValidationError',
    
    # Core functionality
    'create_identity_config',
    'validate_config',
    'create_brain_from_config',
    'create_brain_with_default_config',
    'run_brain_simulation',
    'compare_brain_states',
    'create_simulation_environment',
    'create_agent_brain',
    'create_complete_agent',
    'run_agent_episode',
    'run_batch_experiments',
    
    # Utilities
    'print_simulation_summary',
    'validate_agent_setup'
]

# Set up module
if _HAS_PYNLM:
    # Import core module functions into main namespace
    import pynlm_core as core
    
    # Create aliases for core functions
    createDefaultConfig = core.createDefaultConfig
    createBrain = core.createBrain
    createSimpleWorld = core.createSimpleWorld
    createAgentBrain = core.createAgentBrain
    
    # Add them to __all__
    __all__.extend(['createDefaultConfig', 'createBrain', 'createSimpleWorld', 'createAgentBrain'])
else:
    # Define stubs when core module is not available
    def createDefaultConfig():
        raise RuntimeError("pynlm_core not available")
    
    def createBrain(config):
        raise RuntimeError("pynlm_core not available")
    
    def createSimpleWorld():
        raise RuntimeError("pynlm_core not available")
    
    def createAgentBrain(brain):
        raise RuntimeError("pynlm_core not available")
    
    # Add stubs to __all__
    __all__.extend(['createDefaultConfig', 'createBrain', 'createSimpleWorld', 'createAgentBrain'])

# Module initialization
if __name__ == "__main__":
    print(f"NLM Python Package v{__version__}")
    if _HAS_PYNLM:
        print("Core module loaded successfully.")
    else:
        print("WARNING: Core module not available. Limited functionality.")
