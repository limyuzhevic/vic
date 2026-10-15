"""
NLM (Neural Learning Machine) Python Interface
===============================================

A comprehensive Python interface for the NLM C++ neural simulation framework.
Provides access to advanced neural systems including memory, prediction, cognition,
and neuromodulation.

Key Features:
- Complete neural system simulation
- Agent-brain integration
- Memory systems (working, episodic, associative)
- Neuromodulation (dopamine, curiosity, novelty)
- Prediction and planning systems
- Development stages
- Pythonic interfaces and error handling
"""

__version__ = "1.0.0"
__author__ = "NLM Project"
__description__ = "Neural Learning Machine Python Bindings"

# Import core classes from the C++ bindings
from pynlm import (
    # Basic types
    NeuronId,
    SynapseId,
    RegionId,
    PopulationId,
    
    # Enumerations
    NeuronType,
    SynapseType,
    DevelopmentalStage,
    FiringState,
    ActionType,
    MotorCommand,
    WorldObjectType,
    
    # Core classes
    Config,
    SensoryInput,
    Vision,
    Audio,
    InternalSignals,
    Action,
    WorldObject,
    AgentBody,
    ActionResult,
    SensoryPercept,
    SimpleWorld,
    
    # Neural systems
    Brain,
    AgentBrain,
)

# Factory functions
from pynlm import (
    createDefaultConfig,
    createBrain,
    createSimpleWorld,
    createAgentBrain,
    create_test_brain,
    create_demo_world,
    brain_context,
)

# Utility functions
from pynlm import (
    version,
    validate_config,
    benchmark,
    collect_memory_stats,
    collect_neural_activity,
    configure_plasticity,
    set_development_stage,
    checkpoint_brain,
    load_checkpoint,
)

# Convenience classes
from pynlm import (
    BrainContextManager,
    Configurable,
)

# Example usage patterns
__all__ = [
    # Core types
    'NeuronId', 'SynapseId', 'RegionId', 'PopulationId',
    'NeuronType', 'SynapseType', 'DevelopmentalStage', 'FiringState',
    'ActionType', 'MotorCommand', 'WorldObjectType',
    
    # Core classes
    'Config', 'SensoryInput', 'Vision', 'Audio', 'InternalSignals',
    'Action', 'WorldObject', 'AgentBody', 'ActionResult', 'SensoryPercept',
    'SimpleWorld', 'Brain', 'AgentBrain',
    
    # Factory functions
    'createDefaultConfig', 'createBrain', 'createSimpleWorld', 'createAgentBrain',
    'create_test_brain', 'create_demo_world', 'brain_context',
    
    # Utility functions
    'version', 'validate_config', 'benchmark', 'collect_memory_stats',
    'collect_neural_activity', 'configure_plasticity', 'set_development_stage',
    'checkpoint_brain', 'load_checkpoint',
    
    # Convenience classes
    'BrainContextManager', 'Configurable',
]

# Example usage

def quick_simulation(neuron_count=100, steps=100):
    """
    Quick simulation example for beginners.
    
    Creates a simple test brain and runs a quick simulation.
    
    Parameters
    ----------
    neuron_count : int, optional
        Number of neurons in the brain (default: 100)
    steps : int, optional
        Number of simulation steps (default: 100)
    
    Returns
    -------
    dict
        Simulation results including total spikes and activity
    """
    config = createDefaultConfig()
    config.set("neuron_count", neuron_count)
    config.set("region_count", 1)
    
    brain = createBrain(config)
    brain.initialize()
    
    for i in range(steps):
        brain.step(i, i * 0.001)
    
    return {
        'total_spikes': brain.getTotalSpikeCount(),
        'active_neurons': brain.getActiveNeuronCount(),
        'firing_rate': brain.getAverageFiringRate(),
        'steps': steps
    }
def create_agent_environment(width=20, height=20, vision_width=10, vision_height=10):
    """
    Create a demo environment for agent testing.
    
    Parameters
    ----------
    width : int, optional
        World width (default: 20)
    height : int, optional
        World height (default: 20)
    vision_width : int, optional
        Vision sensor width (default: 10)
    vision_height : int, optional
        Vision sensor height (default: 10)
    
    Returns
    -------
    SimpleWorld
        Configured world environment
    """
    world = create_demo_world()
    world.configure(width, height, vision_width, vision_height)
    world.setAgentStart(width // 2, height // 2)
    return world
def benchmark_performance(brain, steps):
    """
    Benchmark brain simulation performance.
    
    Parameters
    ----------
    brain : Brain
        The brain to benchmark
    steps : int
        Number of steps to simulate
    
    Returns
    -------
    float
        Average milliseconds per simulation step
    """
    return benchmark(brain, steps)
def get_system_status(brain):
    """
    Get comprehensive system status and statistics.
    
    Parameters
    ----------
    brain : Brain
        The brain to query
    
    Returns
    -------
    dict
        System status including memory, neural activity, and performance metrics
    """
    stats = collect_memory_stats(brain)
    activity = collect_neural_activity(brain)
    
    status = {
        'memory': stats,
        'neural_activity': activity,
        'brain_stats': {
            'total_neurons': brain.getTotalNeuronCount(),
            'total_synapses': brain.getTotalSynapseCount(),
            'region_count': brain.getRegionCount(),
            'excitation_inhibition_ratio': brain.getExcitationInhibitionRatio(),
        }
    }
    
    return status
def save_and_reload(brain, filepath):
    """
    Save brain state to file and reload it.
    
    Parameters
    ----------
    brain : Brain
        The brain to save
    filepath : str
        Path to save/load checkpoint
    
    Returns
    -------
    Brain or None
        Reloaded brain if successful, None otherwise
    """
    if not brain.save(filepath):
        raise RuntimeError(f"Failed to save brain to {filepath}")
    
    config = brain.getConfig()
    reloaded = load_checkpoint(config, filepath)
    if reloaded is None:
        raise RuntimeError(f"Failed to load brain from {filepath}")
    
    return reloaded
# Version information
print(f"NLM Python Interface v{__version__}")
print(f"For documentation and examples, visit: https://github.com/nlm-project/nlm")