# pynlm/core.py
# 
# This is the core module for NLM Python bindings.
# It provides the interface between Python and C++ NLM bindings.
# 
# This module handles:
# 1. Import and validation of C++ bindings
# 2. Type conversion between Python and C++
# 3. Error handling and exception conversion
# 4. Documentation and type hints
# 5. Basic validation and helper functions
#
# The actual implementations are in C++ (python/bindings.cpp) and
# exposed through pybind11. This layer provides Python-specific
# functionality and a cleaner interface.

import sys
from typing import Any, Dict, List, Optional, Union, Callable, TypeVar

# Try to import C++ bindings
try:
    # Import from the C++ bindings
    from .cpp_bindings import (
        # Core classes
        Config as CConfig,
        Brain as CBrain,
        AgentBrain as CAgentBrain,
        SimpleWorld as CSimpleWorld,
        
        # Sensory classes
        Vision as CVision,
        Audio as CAudio,
        InternalSignals as CInternalSignals,
        SensoryInput as CSensoryInput,
        
        # Motor classes
        Action as CAction,
        MotorCommand as CMotorCommand,
        ActionResult as CActionResult,
        
        # Other classes
        SensoryPercept as CSensoryPercept,
        WorldObject as CWorldObject,
        AgentBody as CAgentBody,
        
        # Enums
        NeuronType as CNeuronType,
        SynapseType as CSynapseType,
        ActionType as CActionType,
        WorldObjectType as CWorldObjectType,
        DevelopmentalStage as CDevelopmentalStage,
        FiringState as CFiringState,
        
        # Type definitions
        NeuronId as CNeuronId,
        SynapseId as CSynapseId,
        RegionId as CRegionId,
        PopulationId as CPopulationId,
        SimulationStep as CSimulationStep,
        Timestamp as CTimestamp,
        TimestepDuration as CTimestepDuration,
        MembranePotential as CMembranePotential,
        SynapticWeight as CSynapticWeight,
        
        # Error types
        RuntimeError as CRuntimeError,
        
        # Factory functions
        createDefaultConfig as CcreateDefaultConfig,
        createBrain as CcreateBrain,
        createSimpleWorld as CcreateSimpleWorld,
        createAgentBrain as CcreateAgentBrain,
    )
    
    CPLUSPLUS_AVAILABLE = True
    
except ImportError as e:
    CPLUSPLUS_AVAILABLE = False
    print(f"Warning: C++ NLM bindings not available: {e}", file=sys.stderr)
    
    # Define dummy classes for when C++ bindings are not available
    CConfig = None
    CBrain = None
    CAgentBrain = None
    CSimpleWorld = None
    CVision = None
    CAudio = None
    CInternalSignals = None
    CSensoryInput = None
    CAction = None
    CMotorCommand = None
    CActionResult = None
    CSensoryPercept = None
    CWorldObject = None
    CAgentBody = None
    CNeuronType = None
    CSynapseType = None
    CActionType = None
    CWorldObjectType = None
    CDevelopmentalStage = None
    CFiringState = None
    CNeuronId = None
    CSynapseId = None
    CRegionId = None
    CPopulationId = None
    CSimulationStep = None
    CTimestamp = None
    CTimestepDuration = None
    CMembranePotential = None
    CSynapticWeight = None
    CRuntimeError = None
    CcreateDefaultConfig = None
    CcreateBrain = None
    CcreateSimpleWorld = None
    CcreateAgentBrain = None

# Type variables for generic functions
T = TypeVar('T')
K = TypeVar('K')

# Python exception classes that wrap C++ exceptions
class NLMError(CRuntimeError if CPLUSPLUS_AVAILABLE else Exception):
    """Base exception class for all NLM errors."""
    
    def __init__(self, message: str, original_exception: Optional[Exception] = None):
        super().__init__(message)
        self.message = message
        self.original_exception = original_exception
    
    def __str__(self) -> str:
        return self.message

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

class ValidationError(NLMError):
    """Error related to input validation."""
    pass

# Type conversion functions
def to_python_config(c_config: Any) -> 'Config':
    """Convert C++ Config to Python Config wrapper."""
    if c_config is None:
        return None
    return Config(c_config)

def to_python_brain(c_brain: Any) -> 'Brain':
    """Convert C++ Brain to Python Brain wrapper."""
    if c_brain is None:
        return None
    return Brain(c_brain)

def to_python_agent(c_agent: Any) -> 'AgentBrain':
    """Convert C++ AgentBrain to Python AgentBrain wrapper."""
    if c_agent is None:
        return None
    return AgentBrain(c_agent)

def to_python_world(c_world: Any) -> 'SimpleWorld':
    """Convert C++ SimpleWorld to Python SimpleWorld wrapper."""
    if c_world is None:
        return None
    return SimpleWorld(c_world)

# Validation and utility functions
def validate_neuron_count(count: int) -> bool:
    """Validate neuron count for simulation."""
    return isinstance(count, int) and 1 <= count <= 1000000

def validate_timestep(timestep: float) -> bool:
    """Validate simulation timestep."""
    return isinstance(timestep, (int, float)) and 0 < timestep <= 1.0

def validate_region_count(count: int) -> bool:
    """Validate region count."""
    return isinstance(count, int) and 1 <= count <= 1000

def validate_connection_probability(prob: float) -> bool:
    """Validate connection probability."""
    return isinstance(prob, (int, float)) and 0 <= prob <= 1

# Helper functions for common operations
def create_simulation_config(
    neuron_count: int = 1000,
    region_count: int = 1,
    connection_probability: float = 0.1,
    random_seed: int = 42,
    simulation_timestep: float = 0.001,
) -> 'Config':
    """
    Create a simulation configuration with common parameters.
    
    Args:
        neuron_count: Number of neurons in simulation
        region_count: Number of brain regions
        connection_probability: Connection probability between neurons
        random_seed: Random seed for reproducibility
        simulation_timestep: Time step for simulation
        
    Returns:
        Configured Config object
        
    Example:
        >>> config = create_simulation_config(
        ...     neuron_count=500,
        ...     region_count=2,
        ...     connection_probability=0.2
        ... )
    """
    if not CPLUSPLUS_AVAILABLE:
        raise NLMError("C++ bindings not available")
    
    config = CcreateDefaultConfig()
    
    # Validate parameters
    if not validate_neuron_count(neuron_count):
        raise ValidationError(f"Invalid neuron count: {neuron_count}")
    if not validate_region_count(region_count):
        raise ValidationError(f"Invalid region count: {region_count}")
    if not validate_connection_probability(connection_probability):
        raise ValidationError(f"Invalid connection probability: {connection_probability}")
    if not validate_timestep(simulation_timestep):
        raise ValidationError(f"Invalid timestep: {simulation_timestep}")
    
    # Set configuration values
    config.set("brain.neuron_count", neuron_count)
    config.set("brain.region_count", region_count)
    config.set("brain.connection_probability", connection_probability)
    config.set("random_seed", random_seed)
    config.set("simulation_timestep", simulation_timestep)
    
    return Config(config)

def run_benchmark_simulation(
    config: 'Config',
    num_steps: int = 1000,
    warmup_steps: int = 100,
) -> Dict[str, Any]:
    """
    Run a benchmark simulation with warmup phase.
    
    Args:
        config: Simulation configuration
        num_steps: Number of simulation steps
        warmup_steps: Number of warmup steps
        
    Returns:
        Dictionary containing benchmark results
        
    Example:
        >>> config = create_simulation_config(neuron_count=100)
        >>> results = run_benchmark_simulation(config, num_steps=100)
        >>> print(f"Firing rate: {results['firing_rate']:.2f}")
    """
    if not CPLUSPLUS_AVAILABLE:
        raise NLMError("C++ bindings not available")
    
    try:
        # Create brain and world
        brain = CcreateBrain(config)
        world = CcreateSimpleWorld()
        agent = CcreateAgentBrain(brain)
        
        # Initialize
        world.configure(20, 20, 8, 8)
        world.reset()
        agent.initialize(world)
        
        # Warmup phase
        for step in range(warmup_steps):
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            brain.step(step)
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            agent.applyRewardModulation(0.0, 0.0)
        
        # Benchmark phase
        spikes_per_step = []
        for step in range(num_steps):
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            brain.step(step)
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            agent.applyRewardModulation(0.0, 0.0)
            
            spikes_per_step.append(brain.getFiringNeuronCount())
        
        # Calculate statistics
        avg_spikes = sum(spikes_per_step) / len(spikes_per_step) if spikes_per_step else 0
        max_spikes = max(spikes_per_step) if spikes_per_step else 0
        min_spikes = min(spikes_per_step) if spikes_per_step else 0
        
        return {
            'average_firing_rate': avg_spikes,
            'max_firing_rate': max_spikes,
            'min_firing_rate': min_spikes,
            'standard_deviation': (
                sum((x - avg_spikes) ** 2 for x in spikes_per_step) / len(spikes_per_step) ** 0.5
                if spikes_per_step else 0
            ),
            'neurons_created': brain.getTotalNeuronCount(),
            'synapses_created': brain.getTotalSynapseCount(),
            'steps_executed': num_steps,
            'warmup_steps': warmup_steps,
            'success': True
        }
        
    except Exception as e:
        return {
            'success': False,
            'error_message': str(e),
            'error_type': type(e).__name__
        }

# Memory management utilities
def get_memory_usage(brain: 'Brain') -> Dict[str, Any]:
    """
    Get memory usage information for a brain simulation.
    
    Args:
        brain: Brain instance
        
    Returns:
        Dictionary containing memory usage statistics
    """
    if not CPLUSPLUS_AVAILABLE:
        return {'success': False, 'error_message': 'C++ bindings not available'}
    
    try:
        return {
            'neuron_count': brain.getTotalNeuronCount(),
            'synapse_count': brain.getTotalSynapseCount(),
            'region_count': brain.getRegionCount(),
            'working_memory_traces': (
                brain.getWorkingMemory().getActiveTraces() 
                if brain.getWorkingMemory() else 0
            ),
            'episodic_memory_episodes': (
                brain.getEpisodicMemory().getEpisodeCount()
                if brain.getEpisodicMemory() else 0
            ),
            'cognition_system_count': sum([
                1 for system in [
                    brain.getPlanner(),
                    brain.getConceptFormation(),
                    brain.getAttention(),
                    brain.getDevelopmentSystem(),
                    brain.getPredictionSystem(),
                    brain.getDopamine(),
                    brain.getCuriosity(),
                    brain.getNovelty(),
                    brain.getPredictionErrorSignal()
                ] if system is not None
            ])
        }
    except Exception as e:
        return {
            'success': False,
            'error_message': str(e)
        }

# Performance monitoring
def monitor_performance(
    brain: 'Brain',
    duration_steps: int = 100,
    collect_interval: int = 10,
) -> Dict[str, Any]:
    """
    Monitor performance metrics over time.
    
    Args:
        brain: Brain instance
        duration_steps: Number of steps to monitor
        collect_interval: Interval for collecting metrics
        
    Returns:
        Dictionary containing performance metrics
    """
    if not CPLUSPLUS_AVAILABLE:
        return {'success': False, 'error_message': 'C++ bindings not available'}
    
    try:
        metrics = {
            'spike_counts': [],
            'firing_rates': [],
            'e_i_ratios': [],
            'timestamps': [],
            'collect_interval': collect_interval,
            'total_steps': duration_steps
        }
        
        for step in range(duration_steps):
            if step % collect_interval == 0:
                metrics['spike_counts'].append(brain.getTotalSpikeCount())
                metrics['firing_rates'].append(brain.getAverageFiringRate())
                metrics['e_i_ratios'].append(brain.getExcitationInhibitionRatio())
                metrics['timestamps'].append(step)
        
        return metrics
        
    except Exception as e:
        return {
            'success': False,
            'error_message': str(e)
        }

# Export all public interfaces
__all__ = [
    # Exception classes
    'NLMError',
    'ConfigurationError',
    'SimulationError',
    'AgentError',
    'MemoryError',
    'ValidationError',
    
    # Type conversion functions
    'to_python_config',
    'to_python_brain',
    'to_python_agent',
    'to_python_world',
    
    # Validation functions
    'validate_neuron_count',
    'validate_timestep',
    'validate_region_count',
    'validate_connection_probability',
    
    # Helper functions
    'create_simulation_config',
    'run_benchmark_simulation',
    'get_memory_usage',
    'monitor_performance',
    
    # Package metadata
    '__version__',
    'CPLUSPLUS_AVAILABLE',
]

# Package initialization
if __name__ == "__main__":
    print("pynlm.core module - Core interface for NLM Python bindings")
    print(f"C++ bindings available: {CPLUSPLUS_AVAILABLE}")
    print(f"Package version: {__version__}")
