"""
Core NLM functionality.

This module contains the fundamental classes and types for the NLM framework.
"""

import sys
from typing import Dict, Any, Optional, List, Union

# Import native bindings
from ..bindings import (
    # Core types
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
    Brain,
    AgentBrain,
)

# Error types
class NLMError(Exception):
    """Base exception for all NLM errors."""
    pass

class ConfigurationError(NLMError):
    """Error raised for configuration-related issues."""
    pass

class InitializationError(NLMError):
    """Error raised for initialization failures."""
    pass

class RuntimeError(NLMError):
    """Error raised for runtime errors."""
    pass

class ValidationError(NLMError):
    """Error raised for validation failures."""
    pass

# Factory functions with better Pythonic interface
def create_brain(config: Optional[Dict[str, Any]] = None) -> Brain:
    """
    Create a brain with optional configuration.
    
    Args:
        config: Optional dictionary of configuration parameters.
               If None, uses default configuration.
               
    Returns:
        Configured Brain instance.
        
    Raises:
        ConfigurationError: If configuration is invalid.
        InitializationError: If brain creation fails.
    """
    nlm_config = Config()
    
    if config:
        for key, value in config.items():
            nlm_config.set(key, value)
    
    try:
        brain = createBrain(nlm_config)
        brain.initialize()
        return brain
    except Exception as e:
        raise InitializationError(f"Failed to create brain: {e}") from e

def create_simple_world(width: int = 20, height: int = 20,
                        vision_width: int = 8, vision_height: int = 8) -> SimpleWorld:
    """
    Create a simple 2D world with specified dimensions.
    
    Args:
        width: World width in cells.
        height: World height in cells.
        vision_width: Agent's vision width.
        vision_height: Agent's vision height.
        
    Returns:
        Configured SimpleWorld instance.
    """
    world = SimpleWorld()
    world.configure(width, height, vision_width, vision_height)
    return world

def create_agent(brain: Brain, world: SimpleWorld) -> AgentBrain:
    """
    Create an agent brain that connects a brain to a world.
    
    Args:
        brain: Brain instance to control.
        world: World instance to interact with.
        
    Returns:
        Configured AgentBrain instance.
    """
    agent = createAgentBrain(brain)
    agent.initialize(world)
    return agent

def get_version() -> str:
    """Get the current version of NLM."""
    from ..bindings import __version__
    return __version__

def get_info() -> Dict[str, Any]:
    """
    Get information about the NLM library.
    
    Returns:
        Dictionary containing library information.
    """
    info = {
        'version': get_version(),
        'author': 'NLM Project',
        'license': 'MIT',
    }
    return info

# Utility functions for common operations
def run_episode(brain: Brain, world: SimpleWorld, agent: AgentBrain,
                num_steps: int) -> Dict[str, Any]:
    """
    Run a complete simulation episode.
    
    Args:
        brain: Brain instance.
        world: World instance.
        agent: Agent brain instance.
        num_steps: Number of simulation steps to run.
        
    Returns:
        Dictionary containing episode results including final state,
        statistics, and any rewards received.
    """
    results = {
        'final_state': None,
        'statistics': {},
        'rewards': [],
        'success': True,
        'error': None,
    }
    
    try:
        for step in range(num_steps):
            # Update world
            world.update(0.1)
            
            # Get sensory input
            percept = world.getSensoryPercept()
            
            # Process sensory input
            agent.processSensoryInput(percept)
            
            # Brain thinks
            brain.step(step)
            
            # Get action from brain
            action = agent.decodeMotorCommand()
            
            # Apply action to world
            world.applyMotorCommand(action, world.getSimulationTime())
            
            # Record reward if available
            if hasattr(percept, 'getInternal') and percept.getInternal():
                results['rewards'].append(percept.getInternal()[0])
        
        results['final_state'] = {
            'brain': brain,
            'world': world,
            'agent': agent,
        }
        
        results['statistics'] = {
            'final_firing_rate': brain.getAverageFiringRate(),
            'total_spikes': brain.getTotalSpikeCount(),
            'firing_neurons': brain.getFiringNeuronCount(),
            'development_stage': brain.getDevelopmentalStage(),
            'neuromodulation_level': agent.getNeuromodulationLevel(),
        }
        
    except Exception as e:
        results['success'] = False
        results['error'] = str(e)
    
    return results

def benchmark_simulation(config: Optional[Dict[str, Any]] = None,
                         num_steps: int = 1000) -> Dict[str, Any]:
    """
    Run a benchmark simulation and return performance statistics.
    
    Args:
        config: Optional configuration dictionary.
        num_steps: Number of simulation steps.
        
    Returns:
        Dictionary containing benchmark results.
    """
    import time
    import statistics as stats
    
    results = {
        'config': config,
        'num_steps': num_steps,
        'timing': {},
        'performance': {},
        'memory': {},
        'results': None,
    }
    
    try:
        # Create brain
        start_time = time.time()
        brain = create_brain(config)
        init_time = time.time() - start_time
        
        # Run simulation
        step_times = []
        for step in range(num_steps):
            step_start = time.time()
            brain.step(step)
            step_time = time.time() - step_start
            step_times.append(step_time)
        
        total_time = time.time() - start_time
        
        # Calculate statistics
        results['timing'] = {
            'initialization_time': init_time,
            'total_time': total_time,
            'average_step_time': stats.mean(step_times),
            'min_step_time': stats.min(step_times),
            'max_step_time': stats.max(step_times),
            'steps_per_second': num_steps / total_time,
        }
        
        results['performance'] = {
            'neurons_per_second': brain.getTotalNeuronCount() / total_time,
            'synapses_per_second': brain.getTotalSynapseCount() / total_time,
            'spikes_per_second': brain.getTotalSpikeCount() / total_time,
            'average_firing_rate': brain.getAverageFiringRate(),
        }
        
        results['results'] = {
            'brain': brain,
            'final_spike_count': brain.getTotalSpikeCount(),
            'firing_neurons': brain.getFiringNeuronCount(),
        }
        
    except Exception as e:
        results['error'] = str(e)
        results['success'] = False
    
    return results

# Monkey patch common methods to make them more Pythonic
Brain.get_neuron_count = lambda self: self.getTotalNeuronCount()
Brain.get_synapse_count = lambda self: self.getTotalSynapseCount()
Brain.is_initialized = lambda self: True  # Simplified check
Brain.get_current_time = lambda self: self.getTotalSpikeCount()  # Placeholder

AgentBrain.get_sensor_data = lambda self, vision=True, audio=True, internal=True: {
    'vision': self.getVision() if vision else None,
    'audio': self.getAudio() if audio else None,
    'internal': self.getInternal() if internal else None,
}

AgentBrain.get_motor_action_vector = lambda self: [1.0, 0.0, 0.0, 0.0]  # Placeholder
AgentBrain.is_learning = lambda self: True  # Simplified check
AgentBrain.get_learning_rate = lambda self: 0.01  # Default

SimpleWorld.get_objects_by_type = lambda self, obj_type: []  # Placeholder
SimpleWorld.get_nearest_object = lambda self, x, y, radius: None  # Placeholder
SimpleWorld.add_resource = lambda self, x, y, value: None  # Placeholder
SimpleWorld.add_hazard = lambda self, x, y, damage: None  # Placeholder