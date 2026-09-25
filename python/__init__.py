"""
pynlm - Python bindings for NLM (Neural Learning Machine)

A Python binding for the NLM C++ neural simulation framework.
Provides high-level APIs for creating and controlling neural brains.
"""

__version__ = "0.1.0"

from typing import Optional, Dict, List, Any, Union

# Core imports
try:
    from .core import Config, create_default_config
    from .brain import Brain, create_brain
    from .agent import AgentBrain, create_agent_brain
    from .world import SimpleWorld, create_simple_world
    from .sensory import Vision, Audio, InternalSignals, SensoryInput
    from .motor import Action, MotorCommand
    from .visualization import VisualizationInterface
    from .experiments import Experiment, ExperimentRunner
    
    # Enums
    from .types import NeuronType, SynapseType, DevelopmentalStage
    from .enums import ActionType, MotorCommand as MotorCommandEnum
    
    # Utility functions
    from .utils import run_simulation, save_checkpoint, load_checkpoint
    from .visualization import visualize_brain, create_animation
    
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
        'VisualizationInterface',
        'Experiment',
        'ExperimentRunner',
        
        # Enums
        'NeuronType',
        'SynapseType',
        'DevelopmentalStage',
        'ActionType',
        'MotorCommand',
        
        # Enum classes
        'WorldObjectType',
        'FiringState',
        
        # Factory functions
        'create_default_config',
        'create_brain',
        'create_agent_brain',
        'create_simple_world',
        
        # Utilities
        'run_simulation',
        'save_checkpoint',
        'load_checkpoint',
        'visualize_brain',
        'create_animation',
        
        # Version
        '__version__',
    ]
    
except ImportError as e:
    # Fallback to direct imports if the modular structure doesn't exist yet
    # This maintains backward compatibility while allowing gradual migration
    
    # Import all the core bindings directly
    from .bindings import (
        # Core types
        NeuronId, SynapseId, RegionId, PopulationId,
        NeuronType, SynapseType, DevelopmentalStage,
        FiringState, ActionType, MotorCommand,
        WorldObjectType,
        
        # Classes
        Config, SensoryInput, Vision, Audio, InternalSignals,
        Action, WorldObject, AgentBody, ActionResult,
        SensoryPercept, SimpleWorld, Brain, AgentBrain,
        
        # Factory functions
        createDefaultConfig, createBrain, createSimpleWorld,
        createAgentBrain,
        
        # Constants
        INVALID_NEURON_ID, INVALID_SYNAPSE_ID, INVALID_REGION_ID,
        INVALID_POPULATION_ID,
    )
    
    # Create aliases for backward compatibility
    create_default_config = createDefaultConfig
    create_brain = createBrain
    create_simple_world = createSimpleWorld
    create_agent_brain = createAgentBrain
    
    # Re-export everything
    __all__ = [
        # Types
        'NeuronId', 'SynapseId', 'RegionId', 'PopulationId',
        'NeuronType', 'SynapseType', 'DevelopmentalStage',
        'FiringState', 'ActionType', 'MotorCommand',
        'WorldObjectType',
        
        # Classes
        'Config', 'SensoryInput', 'Vision', 'Audio', 'InternalSignals',
        'Action', 'WorldObject', 'AgentBody', 'ActionResult',
        'SensoryPercept', 'SimpleWorld', 'Brain', 'AgentBrain',
        
        # Factory functions
        'create_default_config',
        'create_brain',
        'create_simple_world',
        'create_agent_brain',
        
        # Constants
        'INVALID_NEURON_ID', 'INVALID_SYNAPSE_ID',
        'INVALID_REGION_ID', 'INVALID_POPULATION_ID',
        
        # Version
        '__version__',
    ]

# Convenience API
def run_simulation_with_callback(
    brain,
    world=None,
    agent=None,
    num_steps: int = 1000,
    timestep: float = 0.1,
    callback=None,
    save_frequency: int = 100,
    checkpoint_file: Optional[str] = None
) -> Dict[str, Any]:
    """
    Run a simulation with optional progress callback and checkpoint support.
    
    Args:
        brain: The brain object to simulate
        world: Optional world object for environment interaction
        agent: Optional agent object for sensorimotor loop
        num_steps: Number of simulation steps to run
        timestep: Time between steps (default: 0.1)
        callback: Optional callback function called every N steps
        save_frequency: How often to save checkpoints
        checkpoint_file: Optional checkpoint file path
        
    Returns:
        Dictionary with simulation statistics
    """
    stats = {
        'steps_completed': 0,
        'total_spikes': 0,
        'firing_neurons': 0,
        'memory_usage': [],
        'performance_metrics': []
    }
    
    try:
        for step in range(num_steps):
            # Update world if provided
            if world:
                world.update(timestep)
                
                # Get sensory input if agent is present
                if agent:
                    percept = world.getSensoryPercept()
                    agent.processSensoryInput(percept)
            
            # Run brain step
            brain.step(step)
            
            # Get action if agent is present
            if agent:
                action = agent.decodeMotorCommand()
                if world:
                    world.applyMotorCommand(action, world.getSimulationTime())
            
            # Collect statistics
            stats['total_spikes'] += brain.getTotalSpikeCount()
            stats['firing_neurons'] += brain.getFiringNeuronCount()
            
            # Call callback if provided
            if callback and step % save_frequency == 0:
                callback(step, stats)
            
            # Save checkpoint if requested
            if checkpoint_file and step % save_frequency == 0:
                save_checkpoint(brain, f"{checkpoint_file}_step_{step}.bin")
        
        return stats
        
    except Exception as e:
        raise RuntimeError(f"Simulation failed at step {step}: {str(e)}")


def create_development_agent(
    config=None,
    world_size: tuple = (20, 20),
    enable_learning: bool = True,
    enable_development: bool = True,
    enable_curiosity: bool = True,
    enable_reward_modulation: bool = True,
    enable_structural_plasticity: bool = True
):
    """
    Create a fully configured agent brain with all subsystems enabled.
    
    This is a convenience function for creating a ready-to-use agent
    for developmental learning experiments.
    
    Args:
        config: Configuration object (creates default if None)
        world_size: Tuple of (width, height) for the world
        enable_learning: Enable reward-based learning
        enable_development: Enable developmental stages
        enable_curiosity: Enable curiosity-driven exploration
        enable_reward_modulation: Enable reward signals
        enable_structural_plasticity: Enable synaptic growth/rewiring
        
    Returns:
        Tuple of (brain, agent, world)
    """
    if config is None:
        config = create_default_config()
    
    # Create brain
    brain = create_brain(config)
    brain.initialize()
    
    # Create world
    world = create_simple_world()
    vision_width, vision_height = world_size
    world.configure(
        width=world_size[0],
        height=world_size[1],
        visionWidth=vision_width,
        visionHeight=vision_height
    )
    world.reset()
    
    # Create agent
    agent = create_agent_brain(brain)
    agent.initialize(world)
    
    # Configure subsystems
    if enable_reward_modulation:
        agent.enable_reward_modulation(True)
    
    if enable_structural_plasticity:
        agent.enable_structural_plasticity(True)
    
    if enable_development:
        agent.enable_development(True)
    
    if enable_curiosity:
        agent.enable_curiosity(True)
    
    return brain, agent, world


def quick_test_simulation(steps: int = 100) -> Dict[str, Any]:
    """
    Quick test to verify the pynlm installation and basic functionality.
    
    Args:
        steps: Number of simulation steps to run
        
    Returns:
        Dictionary with test results
    """
    import time
    
    start_time = time.time()
    
    # Create a minimal setup
    config = create_default_config()
    brain = create_brain(config)
    brain.initialize()
    
    # Run simulation
    for step in range(steps):
        brain.step(step)
    
    end_time = time.time()
    
    return {
        'success': True,
        'total_steps': steps,
        'total_neurons': brain.getTotalNeuronCount(),
        'total_synapses': brain.getTotalSynapseCount(),
        'total_spikes': brain.getTotalSpikeCount(),
        'firing_neurons': brain.getFiringNeuronCount(),
        'execution_time': end_time - start_time,
        'spikes_per_step': brain.getTotalSpikeCount() / steps if steps > 0 else 0
    }


# Global configuration
DEFAULT_CONFIG = {
    'brain': {
        'neuron_count': 1000,
        'synapse_density': 0.1,
        'learning_rate': 0.001,
        'v_thresh': -50.0,
        'v_rest': -70.0
    },
    'plasticity': {
        'stdp': {'enable': True, 'learning_rate': 0.001},
        'hebbian': {'enable': True},
        'structural': {'enable': True}
    },
    'neuromod': {
        'dopamine': {'scale': 1.0},
        'curiosity': {'enable': True},
        'novelty': {'enable': True}
    }
}

# Version check
REQUIRED_VERSION = "0.1.0"

if __version__ != REQUIRED_VERSION:
    import warnings
    warnings.warn(
        f"You are using pynlm version {__version__}, "
        f"but version {REQUIRED_VERSION} is required.",
        RuntimeWarning
    )
