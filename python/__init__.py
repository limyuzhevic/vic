"""
Python bindings module for NLM (Neural Learning Machine) neural simulation framework.

This module provides Python bindings for the NLM C++ neural simulation framework,
enabling users to create, simulate, and analyze neural networks through Python.

Key Features:
- Create and initialize neural brains with configurable parameters
- Run neural simulations with sensory input and motor output
- Access brain statistics and performance metrics
- Save and load brain states for persistent simulations
- Integrate with world environments for embodied agents

The module provides a clean, high-level interface to the underlying C++ neural
computation engine while maintaining access to advanced functionality through
higher-level APIs.
"""

import sys
from typing import Dict, Any, Optional

# Import core bindings
try:
    from .bindings import pynlm
    PYNLM_AVAILABLE = True
except ImportError as e:
    PYNLM_AVAILABLE = False
    print(f"Warning: pynlm module not available: {e}")
    print("Please install the NLM Python bindings with: pip install -e .")

# Import enhanced API
try:
    from .enhanced_api import (
        NLMConfig,
        EnhancedNLMWorld,
        PerformanceMonitor,
        create_quick_brain,
        create_simple_experiment,
        run_simple_experiment,
        NLMError,
        ConfigurationError,
        PerformanceError,
        EnvironmentError,
    )
    ENHANCED_API_AVAILABLE = True
except ImportError as e:
    ENHANCED_API_AVAILABLE = False
    print(f"Warning: Enhanced API not available: {e}")

# Import performance monitoring modules
try:
    from .performance import (
        PerformanceMonitor as PerformanceProfiler,
        PerformanceProfiler as Profiler,
        ResourceMonitor,
        EfficiencyAnalyzer,
        MetricsExporter,
    )
    PERFORMANCE_AVAILABLE = True
except ImportError as e:
    PERFORMANCE_AVAILABLE = False
    print(f"Warning: Performance modules not available: {e}")

# Core API Functions
def create_default_config():
    """Create a default NLM configuration."""
    if not PYNLM_AVAILABLE:
        raise NLMError("NLM Python bindings not available")
    
    return pynlm.createDefaultConfig()

def create_brain(config=None):
    """Create an NLM brain with the given configuration."""
    if not PYNLM_AVAILABLE:
        raise NLMError("NLM Python bindings not available")
    
    if config is None:
        config = create_default_config()
    
    return pynlm.createBrain(config)

def create_simple_world():
    """Create a simple world environment for simulation."""
    if not PYNLM_AVAILABLE:
        raise NLMError("NLM Python bindings not available")
    
    return pynlm.createSimpleWorld()

def create_agent_brain(brain):
    """Create an agent brain interface for brain-world interaction."""
    if not PYNLM_AVAILABLE:
        raise NLMError("NLM Python bindings not available")
    
    return pynlm.createAgentBrain(brain)

# Convenience Functions for Common Tasks
def run_quick_simulation(neuron_count: int = 500, regions: int = 1, steps: int = 1000):
    """
    Run a quick simulation with basic configuration.
    
    Args:
        neuron_count: Number of neurons in the brain
        regions: Number of brain regions
        steps: Number of simulation steps to run
    
    Returns:
        Tuple of (brain, config)
    """
    config = create_default_config()
    config.set("brain.neuron_count", neuron_count)
    config.set("brain.region_count", regions)
    
    brain = create_brain(config)
    brain.initialize()
    
    for i in range(steps):
        brain.step(i)
    
    return brain, config

def run_experiment_with_agent(world_width: int = 10, world_height: int = 10, 
                             num_steps: int = 1000, neuron_count: int = 1000):
    """
    Run a complete experiment with an agent in a world.
    
    Args:
        world_width: Width of the world environment
        world_height: Height of the world environment
        num_steps: Number of simulation steps
        neuron_count: Number of neurons in the agent brain
    
    Returns:
        Tuple of (world, agent, brain, stats)
    """
    # Create brain and configure
    config = create_default_config()
    config.set("brain.neuron_count", neuron_count)
    
    brain = create_brain(config)
    brain.initialize()
    
    # Create world and agent
    world = create_simple_world()
    world.configure(world_width, world_height, 8, 8)
    world.reset()
    
    agent = create_agent_brain(brain)
    agent.initialize(world)
    
    # Enable subsystems
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    
    # Run experiment
    for step in range(num_steps):
        world.update(0.1)
        
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(step)
        
        motor_cmd = agent.decodeMotorCommand()
        world.applyMotorCommand(motor_cmd, world.getSimulationTime())
        
        reward = 0.0
        if percept.getInternal() and len(percept.getInternal()) > 0:
            reward = percept.getInternal()[0]
        
        agent.applyRewardModulation(reward, 0.0)
        agent.updateDevelopment(0.1)
    
    # Collect statistics
    stats = {
        'total_steps': num_steps,
        'neurons_firing': brain.getFiringNeuronCount(),
        'total_spikes': brain.getTotalSpikeCount(),
        'average_firing_rate': brain.getAverageFiringRate(),
        'e_i_ratio': brain.getExcitationInhibitionRatio(),
        'development_stage': int(agent.getDevelopmentalStage()),
        'neuromodulation_level': agent.getNeuromodulationLevel(),
        'curiosity_level': agent.getCuriosityLevel(),
        'world_time': world.getSimulationTime(),
    }
    
    return world, agent, brain, stats

# Version and Information
__version__ = "2.0.0"
__author__ = "NLM Research Project"
__description__ = "Python bindings for NLM (Neural Learning Machine) neural simulation framework"

# Available modules
__all__ = [
    # Core API
    'create_default_config',
    'create_brain',
    'create_simple_world',
    'create_agent_brain',
    
    # Convenience functions
    'run_quick_simulation',
    'run_experiment_with_agent',
    
    # Enhanced API (if available)
    'NLMConfig',
    'EnhancedNLMWorld',
    'PerformanceMonitor',
    'create_quick_brain',
    'create_simple_experiment',
    'run_simple_experiment',
    
    # Performance modules (if available)
    'PerformanceProfiler',
    'Profiler',
    'ResourceMonitor',
    'EfficiencyAnalyzer',
    'MetricsExporter',
    
    # Exceptions
    'NLMError',
    'ConfigurationError',
    'PerformanceError',
    'EnvironmentError',
    
    # Version info
    '__version__',
    '__author__',
    '__description__',
]

# Module availability checks
def check_availability():
    """Check availability of NLM modules and print status."""
    print("=== NLM Python Bindings Availability Check ===")
    print(f"Core pynlm bindings: {'✓ Available' if PYNLM_AVAILABLE else '✗ Not available'}")
    print(f"Enhanced API: {'✓ Available' if ENHANCED_API_AVAILABLE else '✗ Not available'}")
    print(f"Performance modules: {'✓ Available' if PERFORMANCE_AVAILABLE else '✗ Not available'}")
    
    if not PYNLM_AVAILABLE:
        print("\nTo install NLM Python bindings:")
        print("  pip install -e .")
    
    return PYNLM_AVAILABLE and ENHANCED_API_AVAILABLE

# Initialize on import
if __name__ != "__main__":
    check_availability()
