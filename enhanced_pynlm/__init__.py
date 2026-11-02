# enhanced_pynlm/__init__.py
"""
Enhanced NLM Python API Package

This is the enhanced version of the NLM Python bindings, providing
a more Pythonic interface with improved error handling, context managers,
performance monitoring, and advanced features.

Key features:
• Context managers for resource safety
• Better error handling with Python exceptions
• Performance monitoring tools
• Neural recording and playback
• Configuration management with validation
• Advanced agent simulation capabilities
• Analysis and visualization tools

The enhanced API maintains full backward compatibility with the original
pynlm bindings while adding powerful new features for advanced usage.
"""

import time
import threading
from typing import Optional, Callable, Any, Dict, List
from contextlib import contextmanager

# Import original pynlm bindings
import pynlm

# Re-export original API for backward compatibility
from pynlm import (
    createDefaultConfig,
    createBrain,
    createSimpleWorld,
    createAgentBrain,
    Config,
    Brain,
    SimpleWorld,
    AgentBrain,
    NeuronType,
    MotorCommand,
    ActionType,
    WorldObjectType,
    NeuronId,
    SynapseId,
    RegionId,
    PopulationId,
    DevelopmentalStage,
    FiringState,
    SensoryInput,
    Vision,
    Audio,
    InternalSignals,
    Action,
    WorldObject,
    AgentBody,
    ActionResult,
    SensoryPercept,
    INVALID_NEURON_ID,
    INVALID_SYNAPSE_ID,
    INVALID_REGION_ID,
    INVALID_POPULATION_ID
)

# Import enhanced classes
from .enhanced import (
    EnhancedConfig,
    EnhancedBrain,
    EnhancedWorld,
    EnhancedAgent,
    BrainMonitor,
    NeuralRecorder,
    ConfigManager,
    BrainAnalyzer,
    SimulationError,
    ConfigurationError,
    ResourceError
)

# Package metadata
__version__ = "2.0.0"
__enhanced_api_version__ = "2.0.0"
__original_version__ = getattr(pynlm, '__version__', '1.0.0')

# === Global State ===

_global_config = EnhancedConfig()
_global_monitoring_enabled = False
_global_recording_enabled = False

# === Global Functions ===

def get_global_config() -> EnhancedConfig:
    """Get the global configuration."""
    return _global_config

def set_global_config(config: EnhancedConfig):
    """Set the global configuration."""
    global _global_config
    _global_config = config

def enable_global_monitoring(enabled: bool = True):
    """Enable/disable global monitoring."""
    global _global_monitoring_enabled
    _global_monitoring_enabled = enabled

def enable_global_recording(enabled: bool = True):
    """Enable/disable global recording."""
    global _global_recording_enabled
    _global_recording_enabled = enabled

# === Enhanced Session Management ===

@contextmanager
def create_enhanced_session(config=None, monitor=None, record=None):
    """
    Create an enhanced simulation session with automatic resource management.
    
    Args:
        config: Enhanced configuration (uses global if None)
        monitor: Brain monitor for performance tracking
        record: Neural recorder for activity recording
    
    Yields:
        tuple: (EnhancedBrain, monitor_or_None)
    """
    config = config or _global_config
    
    with EnhancedBrain(config) as brain:
        if monitor:
            monitor.start_monitoring()
            yield brain, monitor
        else:
            yield brain, None

def run_experiment(experiment_func: Callable, config=None, iterations: int = 1, **kwargs) -> List[Dict[str, Any]]:
    """
    Run an experiment with the given configuration.
    
    Args:
        experiment_func: Function that takes (brain, iteration) and returns results
        config: Enhanced configuration
        iterations: Number of iterations to run
        **kwargs: Additional parameters for the experiment
    
    Returns:
        list: Results from all iterations
    """
    config = config or _global_config
    results = []
    
    for i in range(iterations):
        with EnhancedBrain(config) as brain:
            result = experiment_func(brain, i, **kwargs)
            results.append(result)
    
    return results

# === Convenience Functions ===

def quick_simulation(neuron_count: int = 100, steps: int = 100) -> EnhancedBrain:
    """
    Quick simulation with reasonable defaults.
    
    Args:
        neuron_count: Number of neurons
        steps: Number of simulation steps
    
    Returns:
        EnhancedBrain: Simulation results
    """
    config = EnhancedConfig()
    config.set("neuron_count", neuron_count)
    config.set("random_seed", 42)
    
    with EnhancedBrain(config) as brain:
        for step in range(steps):
            brain.step(step)
        
        return brain

def agent_simulation(world_config: Optional[Dict[str, Any]] = None,
                    agent_config: Optional[Dict[str, Any]] = None,
                    max_steps: int = 1000) -> tuple:
    """
    Quick agent simulation with world and agent configuration.
    
    Args:
        world_config: World configuration
        agent_config: Agent configuration
        max_steps: Maximum simulation steps
    
    Returns:
        tuple: (EnhancedAgent, stats_dict)
    """
    # Create world
    world = EnhancedWorld()
    
    # Create brain and agent
    brain_config = EnhancedConfig()
    if agent_config:
        for key, value in agent_config.items():
            brain_config.set(key, value)
    
    brain = EnhancedBrain(brain_config)
    agent = EnhancedAgent(brain, world)
    
    # Run episode
    stats = agent.run_episode(max_steps=max_steps)
    
    return agent, stats

# === Enhanced API Classes ===

# Re-export enhanced classes for direct access
EnhancedConfig = EnhancedConfig
EnhancedBrain = EnhancedBrain
EnhancedWorld = EnhancedWorld
EnhancedAgent = EnhancedAgent
BrainMonitor = BrainMonitor
NeuralRecorder = NeuralRecorder
ConfigManager = ConfigManager
BrainAnalyzer = BrainAnalyzer

# Error classes
SimulationError = SimulationError
ConfigurationError = ConfigurationError
ResourceError = ResourceError

# === Module-level Aliases for Backward Compatibility ===

# Original API aliases (for backward compatibility)
createBrain = createBrain
createSimpleWorld = createSimpleWorld
createAgentBrain = createAgentBrain
createDefaultConfig = createDefaultConfig

# Enhanced API aliases
create_enhanced_config = EnhancedConfig
create_enhanced_brain = EnhancedBrain
create_enhanced_world = EnhancedWorld
create_enhanced_agent = EnhancedAgent

# === Comprehensive Documentation ===

def print_api_summary():
    """Print a summary of available API components."""
    print("=== Enhanced NLM Python API v2.0.0 ===\n")
    
    print("Original API (backward compatible):")
    original_components = [
        'createDefaultConfig', 'createBrain', 'createSimpleWorld', 'createAgentBrain',
        'Config', 'Brain', 'SimpleWorld', 'AgentBrain', 'NeuronType', 'MotorCommand',
        'ActionType', 'WorldObjectType', 'NeuronId', 'SynapseId', 'RegionId', 'PopulationId',
        'DevelopmentalStage', 'FiringState', 'SensoryInput', 'Vision', 'Audio',
        'InternalSignals', 'Action', 'WorldObject', 'AgentBody', 'ActionResult',
        'SensoryPercept', 'INVALID_NEURON_ID', 'INVALID_SYNAPSE_ID',
        'INVALID_REGION_ID', 'INVALID_POPULATION_ID'
    ]
    
    for component in original_components:
        print(f"  • {component}")
    
    print(f"\nEnhanced API (new features):")
    enhanced_components = [
        'EnhancedConfig', 'EnhancedBrain', 'EnhancedWorld', 'EnhancedAgent',
        'BrainMonitor', 'NeuralRecorder', 'ConfigManager', 'BrainAnalyzer',
        'SimulationError', 'ConfigurationError', 'ResourceError'
    ]
    
    for component in enhanced_components:
        print(f"  • {component}")
    
    print(f"\nGlobal Functions:")
    global_functions = [
        'create_enhanced_session', 'run_experiment', 'quick_simulation',
        'agent_simulation', 'get_global_config', 'set_global_config'
    ]
    
    for function in global_functions:
        print(f"  • {function}()")
    
    print(f"\nKey Features:")
    features = [
        "✓ Context managers for resource safety",
        "✓ Better error handling with Python exceptions",
        "✓ Performance monitoring tools",
        "✓ Neural recording and playback",
        "✓ Configuration management with validation",
        "✓ Advanced agent simulation capabilities",
        "✓ Analysis and visualization tools",
        "✓ Backward compatibility with original pynlm"
    ]
    
    for feature in features:
        print(f"  {feature}")
    
    print(f"\nEnhanced NLM API loaded successfully!")
    print(f"Version: {__version__}")
    print(f"Original pynlm version: {__original_version__}")

# Print summary when module is imported
if __name__ != "__main__":
    print_api_summary()

# === Example Usage Functions ===

def example_basic_usage():
    """Example of basic enhanced API usage."""
    print("\n=== Basic Usage Example ===\n")
    
    # Create enhanced configuration
    config = EnhancedConfig()
    config.set("neuron_count", 500)
    config.set("development_enabled", True)
    
    # Create enhanced brain with context manager
    with EnhancedBrain(config) as brain:
        print("Running simulation...")
        for step in range(50):
            brain.step(step)
        
        print(f"Results:")
        print(f"  Spike count: {brain.get_total_spike_count()}")
        print(f"  Firing rate: {brain.get_average_firing_rate():.2f} Hz")
        print(f"  Active neurons: {brain.get_active_neuron_count()}")

def example_advanced_usage():
    """Example of advanced enhanced API usage."""
    print("\n=== Advanced Usage Example ===\n")
    
    # Create enhanced setup
    brain = EnhancedBrain()
    world = EnhancedWorld()
    agent = EnhancedAgent(brain, world)
    
    print("Running agent simulation...")
    stats = agent.run_episode(max_steps=100)
    
    print("Episode Results:")
    print(f"  Total steps: {stats['total_steps']}")
    print(f"  Total actions: {stats['total_actions']}")
    print(f"  Success rate: {stats['success_rate']*100:.1f}%")
    print(f"  Total reward: {stats['total_reward']:.2f}")

# === Make module executable ===

if __name__ == "__main__":
    print_api_summary()
    
    # Run examples if requested
    import argparse
    parser = argparse.ArgumentParser(description="Enhanced NLM Python API Examples")
    parser.add_argument("--basic", action="store_true", help="Run basic usage example")
    parser.add_argument("--advanced", action="store_true", help="Run advanced usage example")
    
    args = parser.parse_args()
    
    if args.basic:
        example_basic_usage()
    elif args.advanced:
        example_advanced_usage()
    else:
        print("\nUse --basic or --advanced to run examples")
        print("Or import this module in your own code:")
        print("  import enhanced_pynlm")
        print("  with enhanced_pynlm.create_enhanced_session() as (brain, monitor):")
        print("      # Your simulation code here")

print("Enhanced NLM Python API loaded successfully!")
