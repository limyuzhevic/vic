# NLM Python Bindings Wrapper

"""
Python wrapper for NLM C++ neural simulation framework.

This module provides enhanced Python interfaces to the NLM C++ bindings,
with better error handling, documentation, and Pythonic APIs.
"""

import ctypes
import sys
import os
from typing import Optional, List, Dict, Any, Union

# Try to import the core bindings
try:
    # This will import the pybind11 module that should have been built
    import pynlm_core
    _PYNLM_AVAILABLE = True
except ImportError:
    _PYNLM_AVAILABLE = False
    print("Warning: NLM C++ bindings not available. Please build the project first.")
    print("Run: cd python && cmake . && make")

# Re-export everything from the core bindings
if _PYNLM_AVAILABLE:
    from .pynlm_core import (
        # Core types
        NeuronId,
        SynapseId,
        RegionId,
        PopulationId,
        
        # Enumerations
        NeuronType,
        SynapseType,
        DevelopmentalStage,
        ActionType,
        WorldObjectType,
        
        # Main classes
        Config,
        Brain,
        AgentBrain,
        SimpleWorld,
        SensoryInput,
        Action,
        WorldObject,
        Observation,
        AgentBody,
        SensoryPercept,
        
        # Factory functions
        createDefaultConfig,
        createBrain,
        createAgentBrain,
        createSimpleWorld,
    )
else:
    # Provide placeholder classes when bindings are not available
    class NeuronId:
        def __init__(self, value=0):
            self.value = value
        
        def index(self):
            return self.value
    
    class SynapseId:
        def __init__(self, value=0):
            self.value = value
    
    class RegionId:
        def __init__(self, value=0):
            self.value = value
    
    class PopulationId:
        def __init__(self, value=0):
            self.value = value
    
    # Enumerations as simple classes
    class NeuronType:
        Excitatory = 0
        Inhibitory = 1
        Modulatory = 2
        Sensory = 3
        Motor = 4
        Internal = 5
    
    class SynapseType:
        Excitatory = 0
        Inhibitory = 1
        Modulatory = 2
        Electrical = 3
        GapJunction = 4
    
    class DevelopmentalStage:
        Initial = 0
        CriticalPeriod = 1
        Maturation = 2
        Adult = 3
        Aging = 4
    
    class ActionType:
        MoveForward = 0
        TurnLeft = 1
        TurnRight = 2
        Interact = 3
        Eat = 4
        Wait = 5
    
    class WorldObjectType:
        Empty = 0
        Resource = 1
        Hazard = 2
        Wall = 3
        Marker = 4
    
    # Placeholder classes for when bindings are not available
    class Config:
        def __init__(self):
            self._config = None
    
    class Brain:
        def __init__(self, config=None):
            pass
        
        def initialize(self):
            pass
        
        def step(self, step_number, time=0.0):
            pass
    
    class AgentBrain:
        def __init__(self, brain):
            self.brain = brain
        
        def initialize(self, world):
            pass
        
        def processSensoryInput(self, percept):
            pass
        
        def decodeMotorCommand(self):
            return Action()
    
    class SimpleWorld:
        def __init__(self):
            pass
        
        def configure(self, **kwargs):
            pass
        
        def reset(self):
            pass
        
        def update(self, timestep):
            pass
        
        def getSensoryPercept(self):
            return SensoryPercept()
    
    class SensoryInput:
        pass
    
    class Action:
        pass
    
    class WorldObject:
        pass
    
    class Observation:
        pass
    
    class AgentBody:
        pass
    
    class SensoryPercept:
        pass
    
    # Factory functions
    def createDefaultConfig():
        return Config()
    
    def createBrain(config=None):
        return Brain(config)
    
    def createAgentBrain(brain):
        return AgentBrain(brain)
    
    def createSimpleWorld():
        return SimpleWorld()

# Enhanced convenience functions

def run_minimal_example():
    """Run a minimal NLM simulation example."""
    print("Running minimal NLM example...")
    
    # Create brain
    brain = createBrain()
    brain.initialize()
    
    # Run simulation
    for i in range(10):
        brain.step(i)
    
    print(f"Simulation complete! Total spikes: {brain.getTotalSpikeCount()}")
    print("Neurons: {brain.getTotalNeuronCount()}")
    print("Synapses: {brain.getTotalSynapseCount()}")

def run_agent_example():
    """Run a complete agent simulation example."""
    print("Running agent example...")
    
    # Setup
    config = createDefaultConfig()
    brain = createBrain(config)
    brain.initialize()
    
    world = createSimpleWorld()
    world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
    world.reset()
    
    agent = createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable learning features
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    agent.enableDevelopment(True)
    
    # Run simulation
    for step in range(100):
        world.update(0.1)
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(step)
        world.applyMotorCommand(agent.decodeMotorCommand(), world.getSimulationTime())
        
        # Apply reward
        reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
        agent.applyRewardModulation(reward, 0.0)
        
        agent.updateDevelopment(0.1)
    
    print(f"Agent simulation complete!")
    print(f"Firing neurons: {brain.getFiringNeuronCount()}")
    print(f"Curiosity: {agent.getCuriosityLevel():.3f}")
    print(f"Novelty: {agent.getNoveltyLevel():.3f}")
    print(f"Dev Stage: {brain.getDevelopmentalStage()}")

def benchmark_brain(config=None, steps=1000):
    """Benchmark brain performance with given configuration."""
    print(f"Benchmarking brain for {steps} steps...")
    
    if config is None:
        config = createDefaultConfig()
    
    brain = createBrain(config)
    brain.initialize()
    
    import time
    start_time = time.time()
    
    for step in range(steps):
        brain.step(step)
    
    end_time = time.time()
    elapsed = end_time - start_time
    
    print(f"Benchmark results:")
    print(f"  Total steps: {steps}")
    print(f"  Time elapsed: {elapsed:.3f}s")
    print(f"  Steps per second: {steps/elapsed:.1f}")
    print(f"  Total spikes: {brain.getTotalSpikeCount()}")
    print(f"  Average firing rate: {brain.getAverageFiringRate():.3f} Hz")
    print(f"  Excitatory/inhibitory ratio: {brain.getExcitationInhibitionRatio():.3f}")
    
    return {
        'steps': steps,
        'time_elapsed': elapsed,
        'steps_per_second': steps/elapsed,
        'total_spikes': brain.getTotalSpikeCount(),
        'average_firing_rate': brain.getAverageFiringRate(),
        'e_i_ratio': brain.getExcitationInhibitionRatio(),
    }

# Configuration utilities

def get_default_configuration():
    """Get recommended default configuration for common use cases."""
    config = createDefaultConfig()
    
    # Set up small brain for testing
    config.set("brain.neuron_count", 1000)
    config.set("brain.synapse_density", 0.1)
    config.set("brain.v_thresh", -50.0)
    config.set("brain.v_rest", -70.0)
    config.set("brain.tau_mem", 20.0)
    
    # Set up learning parameters
    config.set("plasticity.stdp.enable", True)
    config.set("plasticity.hebbian.enable", True)
    config.set("plasticity.stdp.learning_rate", 0.001)
    
    # Set up neuromodulation
    config.set("neuromod.dopamine.scale", 1.0)
    config.set("neuromod.curiosity.enable", True)
    config.set("neuromod.novelty.enable", True)
    
    return config

def validate_configuration(config):
    """Validate configuration and return any issues."""
    issues = []
    
    # Check for required configuration keys
    required_keys = [
        "brain.neuron_count",
        "brain.synapse_density",
        "brain.v_thresh",
        "brain.v_rest",
        "brain.tau_mem",
    ]
    
    for key in required_keys:
        if not config.has(key):
            issues.append(f"Missing required configuration: {key}")
    
    # Check for reasonable values
    if config.has("brain.neuron_count"):
        neuron_count = config.getOr("brain.neuron_count", 0)
        if neuron_count < 100:
            issues.append("Neuron count seems too low (< 100)")
        elif neuron_count > 100000:
            issues.append("Neuron count seems very high (> 100,000)")
    
    return issues

# System information

def get_system_info():
    """Get system information about the NLM installation."""
    info = {
        'version': __version__,
        'bindings_available': _PYNLM_AVAILABLE,
        'platform': sys.platform,
        'python_version': sys.version,
    }
    
    if _PYNLM_AVAILABLE:
        try:
            info['build_info'] = {
                'total_neuron_count': 0,  # Would need to get from actual binding
                'total_synapse_count': 0,
            }
        except:
            pass
    
    return info

# Progress tracking

class NLMProgressTracker:
    """Track progress of long-running NLM operations."""
    
    def __init__(self, total_steps: int):
        self.total_steps = total_steps
        self.current_step = 0
        self.start_time = time.time()
    
    def step(self, step: int = None):
        """Record progress for a step."""
        if step is not None:
            self.current_step = step
        else:
            self.current_step += 1
        
        progress = (self.current_step / self.total_steps) * 100
        elapsed = time.time() - self.start_time
        
        if self.current_step % 10 == 0 or self.current_step == self.total_steps:
            print(f"Progress: {self.current_step}/{self.total_steps} ({progress:.1f}%) "
                  f"- {elapsed:.2f}s elapsed")
    
    def finish(self):
        """Report completion."""
        elapsed = time.time() - self.start_time
        print(f"Complete! Processed {self.current_step} steps in {elapsed:.2f}s "
              f"({self.current_step/elapsed:.1f} steps/sec)")

# Example scripts (embedded for convenience)

EXAMPLES = {
    'minimal': '''
import pynlm

# Create and initialize
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Simulate
for i in range(100):
    brain.step(i)

print("Minimal example complete!")
print(f"Neurons: {brain.getTotalNeuronCount()}")
print(f"Total spikes: {brain.getTotalSpikeCount()}")
''',

    'agent': '''
import pynlm

# Setup
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable learning
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableDevelopment(True)

# Run simulation
for step in range(100):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    world.applyMotorCommand(agent.decodeMotorCommand(), world.getSimulationTime())
    
    # Apply reward
    reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
    agent.applyRewardModulation(reward, 0.0)
    
    agent.updateDevelopment(0.1)

print("Agent simulation complete!")
''',

    'benchmark': '''
import pynlm

# Get recommended configuration
config = pynlm.get_default_configuration()

# Run benchmark
results = pynlm.benchmark_brain(config, steps=1000)
print("Benchmark complete!")
print(f"Performance: {results['steps_per_second']:.1f} steps/second")
''',
}

def print_example(name: str):
    """Print a specific example script."""
    if name in EXAMPLES:
        print(f"\n=== {name.upper()} EXAMPLE ===")
        print(EXAMPLES[name])
        return True
    else:
        print(f"Example '{name}' not found")
        print(f"Available examples: {list(EXAMPLES.keys())}")
        return False

# For backward compatibility
if not _PYNLM_AVAILABLE:
    print("WARNING: NLM C++ bindings not available. Using stub implementations.")
    print("Please build the project with: cd python && cmake . && make")