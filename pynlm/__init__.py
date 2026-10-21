"""NLM Python Package - Neural Learning Machine

This module provides Python bindings for the NLM (Neural Learning Machine) project,
an experimental artificial developmental brain system implemented in C++.

The NLM implements spiking neural networks with real biological-inspired dynamics:
- Leaky Integrate-and-Fire (LIF) neurons
- Event-driven spike propagation
- STDP and Hebbian plasticity
- Neuromodulation (dopamine, curiosity, novelty)
- Memory systems (working, episodic, semantic, procedural)
- Developmental stages and structural plasticity
- Prediction and planning systems

Usage:
    import pynlm
    
    # Create and configure a brain
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 1000)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Run simulation
    for step in range(1000):
        brain.step(step)
        
    # Save/load brain state
    brain.save("checkpoint.bin")
    brain.load("checkpoint.bin")
    
    # Create world and agent
    world = pynlm.createSimpleWorld()
    agent = pynlm.createAgentBrain(brain)
"""

from .core import (
    Config, Neuron, Synapse, STDP, RandomGenerator,
    createDefaultConfig, createBrain
)
from .environment import SimpleWorld
from .agent import AgentBrain
from .experiments import Phase6Config, Phase6IntegratedExperiment, Phase6IntegrationResult
from .logging import Logger

__version__ = "0.1.0"
__author__ = "NLM Research Team"
__email__ = "research@nlm.org"

__all__ = [
    "Config", "Neuron", "Synapse", "STDP", "RandomGenerator",
    "createDefaultConfig", "createBrain", "BrainFactory",
    "SimpleWorld", "AgentBrain",
    "Phase6Config", "Phase6IntegratedExperiment", "Phase6IntegrationResult",
    "Logger", "NeuronType", "SynapseType", "DevelopmentalStage",
    "NLMException",
    "__version__", "__author__", "__email__"
]

# Global configuration
_default_config = None


def getDefaultConfig():
    """Get or create the default configuration."""
    global _default_config
    if _default_config is None:
        _default_config = createDefaultConfig()
    return _default_config


def resetConfig():
    """Reset the default configuration."""
    global _default_config
    _default_config = None


def getVersion():
    """Get NLM Python bindings version."""
    return __version__


def info():
    """Print NLM system information."""
    print(f"NLM Python Bindings v{__version__}")
    print(f"Author: {__author__}")
    print(f"License: MIT")
    print(f"Platform: Linux x86_64")
    print(f"Python: {getVersion()}")
    print("\nKey features:")
    print("  - Real LIF neuron dynamics")
    print("  - Event-driven spike propagation")
    print("  - STDP and Hebbian plasticity")
    print("  - Neuromodulation (dopamine, curiosity, novelty)")
    print("  - Multiple memory systems")
    print("  - Developmental stages")
    print("  - Prediction and planning")
    print("  - Structural plasticity")
    print("  - Checkpoint/restore functionality")


def help_function(category="general"):
    """Display help information for different categories."""
    categories = {
        "general": "General usage and getting started",
        "config": "Configuration management",
        "brain": "Brain creation and management",
        "world": "World and environment simulation",
        "agent": "Agent brain interface",
        "experiments": "Phase 6 integration experiments",
        "advanced": "Advanced features and API usage"
    }
    
    if category not in categories:
        print(f"Available categories: {', '.join(categories.keys())}")
        return
    
    print(f"\n=== {category.upper()} HELP ===\n")
    
    help_texts = {
        "general": """
General Usage:
------------
import pynlm

# Create a brain with default configuration
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Run simulation
for step in range(1000):
    brain.step(step)
    
# Access brain properties
print(f"Neurons: {brain.getTotalNeuronCount()}")
print(f"Firing rate: {brain.getAverageFiringRate():.2f} Hz")
""",
        "config": """
Configuration Management:
------------------------
config = pynlm.Config()

# Set values
config.set("brain.neuron_count", 1000)
config.set("brain.synapse_density", 0.1)
config.set("plasticity.stdp.enable", True)

# Get values
neuron_count = config.get("brain.neuron_count")
default_count = config.getOr("brain.neuron_count", 1000)

# Load/save to files
config.loadFromFile("config.json")
config.saveToFile("config.json")

# Available configuration keys
keys = config.getKeys()
""",
        "brain": """
Brain Operations:
----------------
brain = pynlm.createBrain(config)

# Initialization
brain.initialize()
brain.reset()

# Simulation steps
brain.step(step_number)
brain.step(step_number, timestamp)

# Input/output
brain.receiveSensoryInput(sensory_input)
action = brain.produceAction()

# Statistics
brain.getTotalNeuronCount()
brain.getTotalSynapseCount()
brain.getFiringNeuronCount()
brain.getAverageFiringRate()

# State management
brain.save("checkpoint.bin")
brain.load("checkpoint.bin")

# Regions
region_id = brain.addRegion("cortex")
region = brain.getRegion(region_id)
""",
        "world": """
World Simulation:
-----------------
world = pynlm.createSimpleWorld()

# Configuration
world.configure(
    width=20, height=20,
    visionWidth=8, visionHeight=8
)

# Control
world.reset()
world.update(timestep=0.1)

# Agent interaction
world.setAgentStart(10.0, 10.0)
world.applyMotorCommand(motor_cmd, time)

# State access
percept = world.getSensoryPercept()
body = world.getAgentBody()

# World objects
world.addObject(pynlm.WorldObject(x, y, pynlm.WorldObjectType.Resource, value=1.0))
world.removeObject(x, y)
""",
        "agent": """
Agent Brain Interface:
---------------------
agent = pynlm.createAgentBrain(brain)

# Initialization
agent.initialize(world)

# Sensory processing
percept = world.getSensoryPercept()
agent.processSensoryInput(percept)

# Motor decoding
action = agent.decodeMotorCommand()

# Neuromodulation
agent.applyRewardModulation(reward, predicted_reward)

# Development
agent.updateDevelopment(timestep)
agent.getDevelopmentalStage()

# Subsystem control
agent.enableRewardModulation(True)
agent.enableStructuralPlasticity(True)
agent.enableDevelopment(True)
agent.enableCuriosity(True)

# Statistics
agent.getCuriosityLevel()
agent.getNoveltyLevel()
agent.getPredictionError()
agent.getNeuromodulationLevel()
""",
        "experiments": """
Phase 6 Integration Experiments:
--------------------------------
# Run complete integration verification
experiment = pynlm.Phase6IntegratedExperiment()
result = experiment.run(config)

# Check integration status
print(f"Working Memory: {'CONNECTED' if result.memoryWorkingMemoryIntegrated else 'DISCONNECTED'}")
print(f"Episodic Memory: {'CONNECTED' if result.memoryEpisodicMemoryIntegrated else 'DISCONNECTED'}")
print(f"Neuromodulation: {'CONNECTED' if result.neuromodulationIntegrated else 'DISCONNECTED'}")
print(f"Prediction: {'CONNECTED' if result.predictionIntegrated else 'DISCONNECTED'}")
print(f"Development: {'CONNECTED' if result.developmentIntegrated else 'DISCONNECTED'}")
""",
        "advanced": """
Advanced Features:
-----------------
# Custom configuration
config = pynlm.Config()
config.set("random_seed", 42)
config.set("simulation_timestep", 0.001)

# Multiple regions
brain.addRegion("sensory")
brain.addRegion("cortex")
brain.addRegion("motor")

# Advanced plasticity
brain.enablePlasticity(hebbian=True, stdp=True, rewardModulated=True)

# Complex simulation loops
for step in range(10000):
    if step % 100 == 0:
        agent.processSensoryInput(world.getSensoryPercept())
    
    brain.step(step, step * 0.001)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    # Reward and development
    reward = world.computeReward(brain.getRegion(1))
    agent.applyRewardModulation(reward, 0.0)
    agent.updateDevelopment(0.001)
"""
    }
    
    print(help_texts.get(category, "Help not available for this category."))
    print(f"\nCategory: {categories[category]}")


# Exceptions
class NLMException(Exception):
    """Base exception for NLM errors."""
    pass

class ConfigurationError(NLMException):
    """Configuration-related errors."""
    pass

class SimulationError(NLMException):
    """Simulation-related errors."""
    pass

class IntegrationError(NLMException):
    """Integration-related errors."""
    pass

# Performance monitoring
import time
_performance_data = []


def measure_performance(func_name, func, *args, **kwargs):
    """Measure and record performance of a function."""
    start_time = time.time()
    result = func(*args, **kwargs)
    end_time = time.time()
    
    execution_time = end_time - start_time
    _performance_data.append({
        'function': func_name,
        'time': execution_time,
        'timestamp': start_time
    })
    
    return result


def getPerformanceStats():
    """Get performance statistics."""
    if not _performance_data:
        return "No performance data available"
    
    import statistics
    
    times = [data['time'] for data in _performance_data]
    stats = {
        'total_calls': len(_performance_data),
        'mean_time': statistics.mean(times),
        'median_time': statistics.median(times),
        'stdev_time': statistics.stdev(times) if len(times) > 1 else 0,
        'min_time': min(times),
        'max_time': max(times),
        'total_time': sum(times)
    }
    
    return stats


def clearPerformanceData():
    """Clear performance data."""
    global _performance_data
    _performance_data = []