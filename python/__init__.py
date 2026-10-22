"""
NLM (Neural Learning Machine) Python Bindings

A Python binding for the NLM C++ neural simulation framework.

This module provides a Pythonic interface to the NLM neural simulation system,
including enhanced error handling, batch operations, and performance optimizations.

Key Features:
- Enhanced Pythonic API with type safety and error handling
- Context managers for automatic resource management
- Batch and parallel processing capabilities
- Performance benchmarking utilities
- Comprehensive documentation and examples
- Full backwards compatibility with legacy API
"""

import sys
from typing import Dict, Any, List, Optional

# Import enhanced classes
from .core import NeuronId, Config, EnhancedConfig, Brain, EnhancedAgentBrain, SimpleWorld
from .enums import (
    NeuronType, SynapseType, DevelopmentalStage, FiringState,
    ActionType, MotorCommand, WorldObjectType, ConfigSource
)
from .sensorium import Vision, Audio, InternalSignals, SensoryPercept
from .motor import Action, AgentBody, ActionResult, WorldObject
from .world import SimpleWorld as SimpleWorldImpl
from .brain import Brain as BrainImpl, EnhancedBrainContext
from .agent import AgentBrain as AgentBrainImpl, EnhancedAgentBrain as EnhancedAgentBrainImpl
from .utils import Benchmark, batchStep, parallelBatchStep, createBrainContext

# Public API exports
__all__ = [
    # Core classes
    'NeuronId',
    'Config',
    'EnhancedConfig',
    'Brain',
    'EnhancedAgentBrain',
    'SimpleWorld',
    
    # Enums
    'NeuronType',
    'SynapseType',
    'DevelopmentalStage',
    'FiringState',
    'ActionType',
    'MotorCommand',
    'WorldObjectType',
    'ConfigSource',
    
    # Sensory systems
    'Vision',
    'Audio',
    'InternalSignals',
    'SensoryPercept',
    
    # Motor systems
    'Action',
    'AgentBody',
    'ActionResult',
    'WorldObject',
    
    # Utilities
    'Benchmark',
    'batchStep',
    'parallelBatchStep',
    'createBrainContext',
    
    # Metadata
    '__version__',
    '__author__',
    '__license__',
    '__description__',
    '__all__',
]

# Version and metadata
__version__ = "0.1.0"
__author__ = "NLM Project Team"
__license__ = "MIT"
__description__ = "Python bindings for NLM (Neural Learning Machine) neural simulation framework"

# Error classes for user-friendly exception handling
class NLMError(Exception):
    """Base exception for NLM errors"""
    pass

class ConfigurationError(NLMError):
    """Exception for configuration-related errors"""
    pass

class BrainError(NLMError):
    """Exception for brain-related errors"""
    pass

# Helper functions for common operations
def create_default_config(use_enhanced: bool = True):
    """
    Create a default configuration.
    
    Args:
        use_enhanced: If True, create an EnhancedConfig (Pythonic API).
                     If False, create a legacy Config.
    
    Returns:
        Configured configuration object.
    """
    if use_enhanced:
        from .core import EnhancedConfig
        return EnhancedConfig()
    else:
        from .core import Config
        return Config()


def create_brain(config=None, use_enhanced: bool = True):
    """
    Create a brain with configuration.
    
    Args:
        config: Configuration object. If None, creates default config.
        use_enhanced: If True, use enhanced brain API.
    
    Returns:
        Brain object.
    """
    if config is None:
        config = create_default_config(use_enhanced)
    
    from .brain import EnhancedBrain
    return EnhancedBrain(config)


def create_simple_world():
    """
    Create a simple world for simulation.
    
    Returns:
        SimpleWorld object.
    """
    from .world import SimpleWorld
    return SimpleWorld()


def create_agent_brain(brain, use_enhanced: bool = True):
    """
    Create an agent brain interface.
    
    Args:
        brain: Brain object to connect.
        use_enhanced: If True, use enhanced agent brain API.
    
    Returns:
        AgentBrain object.
    """
    if use_enhanced:
        from .agent import EnhancedAgentBrain
        return EnhancedAgentBrain(brain)
    else:
        from .agent import AgentBrain
        return AgentBrain(brain)

# Convenience functions for common simulation patterns
def run_simple_simulation(num_steps: int = 100, seed: Optional[int] = None):
    """
    Run a simple simulation with default settings.
    
    Args:
        num_steps: Number of simulation steps.
        seed: Random seed for reproducibility.
    
    Returns:
        Dictionary with simulation results.
    """
    config = create_default_config()
    if seed is not None:
        config.setValue("random_seed", seed)
    
    brain = create_brain(config)
    brain.initialize()
    
    results = {
        'steps': list(range(num_steps)),
        'total_spikes': [],
        'firing_neurons': [],
        'firing_rates': [],
        'developmental_stages': [],
        'neuron_counts': [],
    }
    
    for step in range(num_steps):
        brain.step(step)
        
        results['total_spikes'].append(brain.getTotalSpikeCount())
        results['firing_neurons'].append(brain.getFiringNeuronCount())
        results['firing_rates'].append(brain.getAverageFiringRate())
        results['developmental_stages'].append(brain.getDevelopmentalStage())
        results['neuron_counts'].append(brain.getTotalNeuronCount())
    
    return results


def run_agent_world_simulation(
    world_size: int = 20,
    num_steps: int = 500,
    enable_learning: bool = True,
    seed: Optional[int] = None
):
    """
    Run a complete agent-world simulation.
    
    Args:
        world_size: Size of the square world.
        num_steps: Number of simulation steps.
        enable_learning: Whether to enable learning subsystems.
        seed: Random seed for reproducibility.
    
    Returns:
        Dictionary with simulation results and statistics.
    """
    # Create components
    config = create_default_config()
    if seed is not None:
        config.setValue("random_seed", seed)
    
    brain = create_brain(config)
    world = create_simple_world()
    agent = create_agent_brain(brain)
    
    # Configure world
    world.configure(
        width=world_size,
        height=world_size,
        visionWidth=8,
        visionHeight=8
    )
    
    # Initialize components
    brain.initialize()
    world.reset()
    agent.initialize(world)
    
    # Enable learning if requested
    if enable_learning:
        agent.enableAllSubsystems(True)
    
    # Run simulation
    results = {
        'steps': list(range(num_steps)),
        'agent_actions': [],
        'reward_values': [],
        'curiosity_levels': [],
        'novelty_levels': [],
        'developmental_stages': [],
        'total_spikes': [],
        'firing_neurons': [],
        'agent_body_states': [],
    }
    
    for step in range(num_steps):
        # Update world
        world.update(timestep=0.1)
        
        # Get sensory input
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        # Run brain step
        brain.step(step)
        
        # Get motor command
        motor_cmd = agent.decodeMotorCommand()
        results['agent_actions'].append(motor_cmd)
        
        # Apply action to world
        result = world.applyMotorCommand(motor_cmd, world.getSimulationTime())
        results['reward_values'].append(result.reward)
        
        # Collect statistics
        if hasattr(agent, 'getCuriosityLevel'):
            results['curiosity_levels'].append(agent.getCuriosityLevel())
        if hasattr(agent, 'getNoveltyLevel'):
            results['novelty_levels'].append(agent.getNoveltyLevel())
        
        results['developmental_stages'].append(brain.getDevelopmentalStage())
        results['total_spikes'].append(brain.getTotalSpikeCount())
        results['firing_neurons'].append(brain.getFiringNeuronCount())
        results['agent_body_states'].append(world.getAgentBody())
    
    # Calculate summary statistics
    summary = {
        'total_reward': sum(results['reward_values']),
        'average_curiosity': (sum(results['curiosity_levels']) / len(results['curiosity_levels'])
                            if results['curiosity_levels'] else 0),
        'final_developmental_stage': results['developmental_stages'][-1] if results['developmental_stages'] else None,
        'total_spikes': results['total_spikes'][-1] if results['total_spikes'] else 0,
        'final_firing_rate': results['firing_rates'][-1] if results['firing_rates'] else 0,
    }
    
    return {
        'results': results,
        'summary': summary,
        'config': config,
        'brain': brain,
        'world': world,
        'agent': agent,
    }

# Performance benchmarking utilities
def benchmark_simulation(brain_factory, benchmark_name: str = "Simulation", num_iterations: int = 10):
    """
    Benchmark a brain simulation.
    
    Args:
        brain_factory: Function that creates and returns a brain.
        benchmark_name: Name for the benchmark.
        num_iterations: Number of iterations to run.
    
    Returns:
        Dictionary with benchmark results.
    """
    def benchmark_func():
        brain = brain_factory()
        brain.initialize()
        for i in range(100):  # 100 simulation steps
            brain.step(i)
    
    start_time = time.time()
    for _ in range(num_iterations):
        benchmark_func()
    end_time = time.time()
    
    total_time = end_time - start_time
    avg_time = total_time / num_iterations
    
    return {
        'benchmark_name': benchmark_name,
        'total_time': total_time,
        'average_time': avg_time,
        'iterations': num_iterations,
        'time_per_iteration_ms': (avg_time * 1000),
    }

# Legacy compatibility functions
def legacy_example():
    """
    Example showing how to use the legacy API for backwards compatibility.
    
    Returns:
        Example code as a string.
    """
    return '''
# Legacy API example (for backwards compatibility)
import pynlm

# Create components using legacy API
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Run simulation
for step in range(100):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    world.applyMotorCommand(agent.decodeMotorCommand(), world.getSimulationTime())
'''

# Enhanced API example
def enhanced_example():
    """
    Example showing how to use the enhanced API.
    
    Returns:
        Example code as a string.
    """
    return '''
# Enhanced API example
import pynlm

# Create components using enhanced API
config = pynlm.createDefaultConfig(use_enhanced=True)
brain = pynlm.createBrain(config, use_enhanced=True)
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
world.reset()

agent = pynlm.createAgentBrain(brain, use_enhanced=True)
agent.initialize(world)
agent.enableAllSubsystems(True)

# Use context manager for automatic resource management
with pynlm.createBrainContext(brain):
    # Run simulation
    for step in range(100):
        world.update(0.1)
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(step)
        world.applyMotorCommand(agent.decodeMotorCommand(), world.getSimulationTime())

# Use batch operations for parallel processing
brains = [pynlm.createBrain(pynlm.createDefaultConfig()) for _ in range(5)]
for b in brains:
    b.initialize()

# Use benchmarking for performance analysis
from pynlm import Benchmark
import time

def benchmark_simulation():
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    brain.initialize()
    for i in range(100):
        brain.step(i)

result = Benchmark.timeFunction(benchmark_simulation, "Brain Simulation", 10)
Benchmark.printBenchmark(result)
'''

# Module initialization and validation
if __name__ == "__main__":
    print(f"NLM Python Bindings v{__version__}")
    print(__description__)
    print("Author:", __author__)
    print("License:", __license__)
    print("\nTo use NLM with Python, import the module:")
    print("  import pynlm")
    print("\nFor help, see documentation and examples in the repository.")
