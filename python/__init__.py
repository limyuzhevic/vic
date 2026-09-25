# NLM - Enhanced Python Package for Neural Learning Machine

This is the main package module that exposes the core NLM API and utilities.

The NLM package provides:
- Core NLM functionality via pynlm bindings
- High-level simulation utilities
- Command-line interface
- Configuration presets
- Monitoring and analysis tools
- Enhanced error handling
"""

import sys

# Core imports - these are the main pynlm bindings
import pynlm

# Import submodules
sys.path.insert(0, str(sys.path[0] + '/python'))

# Core API (exposed from pynlm)
from .core import *

# Simulation classes
from .simulation import *

# Configuration utilities
from .config import *

# Monitoring and analysis
from .monitoring import *

# CLI module (import but don't expose directly at package level)
from .nlm_cli import main as cli_main

# Package metadata
__version__ = "0.1.0"
__author__ = "NLM Authors"
__description__ = "Enhanced Python utilities for NLM neural simulations"
__license__ = "MIT"

# Public API - what users will access
__all__ = [
    # Core NLM API (from pynlm bindings)
    "createDefaultConfig",
    "createBrain",
    "createSimpleWorld",
    "createAgentBrain",
    
    # Core types and enums
    "NeuronId", "SynapseId", "RegionId", "PopulationId",
    "NeuronType", "SynapseType", "DevelopmentalStage", "FiringState",
    "ActionType", "MotorCommand", "WorldObjectType",
    
    # Core classes
    "Config", "Brain", "AgentBrain", "SimpleWorld",
    "SensoryInput", "Vision", "Audio", "InternalSignals",
    "Action", "WorldObject", "AgentBody", "ActionResult",
    "SensoryPercept",
    
    # Error types
    "NLMError", "ConfigError", "BrainError", "WorldError",
    
    # Convenience functions
    "create_standard_config",
    
    # Quick access functions
    "quick_brain_simulation",
    "quick_agent_simulation",
    
    # Example functions
    "example_simple_simulation",
    "example_agent_simulation",
    "example_config_presets",
    "example_performance_benchmark",
    
    # High-level utilities
    "SimulationPhase",
    "SimulationStats",
    "Simulation",
    "ConfigPresets",
    "Monitoring",
    
    # Configuration utilities
    "get_config_for_research",
    "get_config_for_artificial",
    "get_config_for_biomimetic",
    "validate_config_briefly",
    "create_custom_config",
    
    # Monitoring utilities
    "quick_brain_analysis",
    "quick_performance_analysis",
    "export_results_for_research",
    
    # CLI function (for direct access)
    "cli_main",
]

# Package-level convenience functions
def create_standard_config():
    """
    Create a standard configuration with typical values for common use cases.
    
    This is a convenience function that creates a pre-configured NLM configuration
    with reasonable defaults for general simulation purposes.
    
    Returns:
        Config: A configured NLM configuration object.
        
    Example:
        >>> config = create_standard_config()
        >>> brain = createBrain(config)
        >>> brain.initialize()
    """
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", "1000")
    config.set("brain.synapse_density", "0.1")
    config.set("plasticity.stdp.enable", "true")
    config.set("plasticity.hebbian.enable", "true")
    config.set("neuromod.dopamine.scale", "1.0")
    config.set("neuromod.curiosity.enable", "true")
    config.set("neuromod.novelty.enable", "true")
    return config


def example_simple_simulation():
    """
    Example function demonstrating simple NLM usage.
    
    This function shows the basic pattern for creating and running
    a simple neural simulation without world interaction.
    
    Example:
        >>> example_simple_simulation()
        Running simple simulation...
        Simulation completed successfully!
    """
    print("Running simple simulation...")
    try:
        config = create_standard_config()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        for i in range(100):
            brain.step(i)
            
        print("Simulation completed successfully!")
        return brain.get_statistics()
    except Exception as e:
        print(f"Simulation failed: {e}")
        raise


def example_agent_simulation():
    """
    Example function demonstrating complete agent simulation.
    
    This function shows the complete pattern for creating an agent
    that interacts with a world, receives sensory input, makes decisions,
    and learns from experiences.
    
    Example:
        >>> example_agent_simulation()
        Running agent simulation...
        Simulation completed successfully!
    """
    print("Running agent simulation...")
    try:
        config = create_standard_config()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        agent = pynlm.createAgentBrain(brain)
        world = pynlm.createSimpleWorld()
        world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
        world.reset()
        agent.initialize(world)
        
        agent.enableRewardModulation(True)
        agent.enableCuriosity(True)
        
        for step in range(100):
            world.update(0.1)
            agent.processSensoryInput(world.getSensoryPercept())
            brain.step(step)
            
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            
            agent.applyRewardModulation(0.1, 0.0)
        
        print("Simulation completed successfully!")
        return {
            'brain_stats': brain.get_statistics(),
            'agent_status': agent.get_agent_status(),
            'world_state': world.get_world_state(),
        }
    except Exception as e:
        print(f"Simulation failed: {e}")
        raise


def example_config_presets():
    """
    Example function showing configuration presets.
    
    This function demonstrates how to use different configuration
    presets for various simulation scenarios.
    
    Example:
        >>> example_config_presets()
        Available configuration presets:
        1. default
        2. learning_agent
        3. exploratory_agent
        4. development_agent
    """
    from .config import ConfigPresets
    
    presets = ConfigPresets.get_all_presets()
    print("Available configuration presets:")
    
    for name, config in presets.items():
        print(f"\n{name}:")
        try:
            config.validate()
            print(f"  ✓ Valid configuration")
            print(f"  ✓ Neurons: {config.get('brain.neuron_count', 'default')}")
            print(f"  ✓ Synapse density: {config.get('brain.synapse_density', 'default')}")
        except Exception as e:
            print(f"  ✗ Invalid: {e}")


def example_performance_benchmark():
    """
    Example function demonstrating performance benchmarking.
    
    This function shows how to benchmark the performance of
    NLM simulations to optimize for speed and efficiency.
    
    Example:
        >>> example_performance_benchmark()
        Running performance benchmark...
        Benchmark completed!
    """
    print("Running performance benchmark...")
    try:
        config = create_standard_config()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        results = brain.benchmark_performance(500)
        print("Benchmark completed!")
        print(f"  Speed: {results['steps_per_second']:.1f} steps/second")
        print(f"  Average time per step: {results['average_time_per_step_ms']:.3f} ms")
        
        return results
    except Exception as e:
        print(f"Benchmark failed: {e}")
        raise


def quick_brain_simulation(steps=100, neurons=1000):
    """
    Quick brain simulation for testing and demonstration.
    
    This is a convenience function for rapid prototyping and testing
    of brain simulation functionality without the complexity of
    full agent/world setup.
    
    Args:
        steps: Number of simulation steps to run (default: 100).
        neurons: Number of neurons in the brain (default: 1000).
        
    Returns:
        Dictionary with simulation results including statistics
        about neuron activity, firing rates, and other metrics.
        
    Example:
        >>> results = quick_brain_simulation(steps=500, neurons=2000)
        >>> print(f"Average firing rate: {results['average_firing_rate']:.3f}")
    """
    config = create_standard_config()
    config.set("brain.neuron_count", str(neurons))
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    for step in range(steps):
        brain.step(step)
    
    return brain.get_statistics()


def quick_agent_simulation(steps=100, enable_learning=True):
    """
    Quick agent simulation for testing and demonstration.
    
    This is a convenience function for rapid prototyping and testing
    of complete agent simulations with world interaction and learning.
    
    Args:
        steps: Number of simulation steps to run (default: 100).
        enable_learning: Whether to enable learning features (default: True).
        
    Returns:
        Dictionary with comprehensive simulation results including
        brain statistics, agent status, and world state.
        
    Example:
        >>> results = quick_agent_simulation(steps=200, enable_learning=True)
        >>> print(f"Final curiosity level: {results['agent_status']['curiosity_level']:.3f}")
    """
    config = create_standard_config()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    agent = pynlm.createAgentBrain(brain)
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    agent.initialize(world)
    
    if enable_learning:
        agent.enableRewardModulation(True)
        agent.enableCuriosity(True)
    
    for step in range(steps):
        world.update(0.1)
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(step)
        
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        if enable_learning:
            agent.applyRewardModulation(0.1, 0.0)
    
    return {
        'brain_stats': brain.get_statistics(),
        'agent_status': agent.get_agent_status(),
        'world_state': world.get_world_state(),
    }


# Package entry point
if __name__ == "__main__":
    print(f"NLM Package v{__version__}")
    print(f"Author: {__author__}")
    print(f"Description: {__description__}")
    print("\nFor more information, visit:")
    print("  https://github.com/nlm-project/nlm")
    
    # Run CLI if requested
    if "--cli" in sys.argv:
        cli_main()