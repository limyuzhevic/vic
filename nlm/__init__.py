from .config import Config
from .brain import Brain
from .agent import AgentBrain
from .world import SimpleWorld
from .sensory import SensoryInput, Vision, Audio, InternalSignals
from .motor import Action
from .agentbody import AgentBody
from .sensorypercept import SensoryPercept

# Python module version
__version__ = "0.1.0"

# Core constants and types
from .core import *

# Convenience functions
def create_default_config():
    """Create a default configuration for NLM."""
    return Config()

def create_brain(config=None):
    """Create a new brain with configuration."""
    if config is None:
        config = create_default_config()
    return Brain(config)

def create_simple_world():
    """Create a new simple world."""
    return SimpleWorld()

def create_agent_brain(brain):
    """Create an agent brain interface."""
    return AgentBrain(brain)

# Advanced convenience functions
def create_default_brain(neuron_count=1000, timestep=0.001):
    """Create a brain with default settings.
    
    Args:
        neuron_count (int): Number of neurons in the brain
        timestep (float): Simulation timestep in seconds
        
    Returns:
        Brain: Initialized brain ready for simulation
    """
    config = create_default_config()
    config.set("neuron_count", neuron_count)
    config.set("simulation_timestep", timestep)
    brain = create_brain(config)
    brain.initialize()
    return brain

def create_developmental_brain():
    """Create a brain configured for developmental learning.
    
    Returns:
        Brain: Brain with developmental plasticity enabled
    """
    config = create_default_config()
    config.set("development_synaptogenesis_rate", 0.001)
    config.set("development_pruning_rate", 0.0001)
    brain = create_brain(config)
    brain.initialize()
    return brain

def create_experienced_brain(checkpoint_path):
    """Create a brain from a saved checkpoint.
    
    Args:
        checkpoint_path (str): Path to saved brain state
        
    Returns:
        Brain: Brain loaded from checkpoint
    """
    brain = create_default_brain()
    brain.load(checkpoint_path)
    return brain

# Performance utilities
import time

def profile_brain(brain, steps, iterations=5):
    """Profile brain performance over multiple iterations.
    
    Args:
        brain (Brain): Brain to profile
        steps (int): Number of steps per iteration
        iterations (int): Number of iterations to run
        
    Returns:
        dict: Performance metrics including average firing rate,
              total spikes, and average execution time
    """
    times = []
    firing_rates = []
    spike_counts = []
    
    for _ in range(iterations):
        # Reset and initialize brain for each iteration
        brain.reset()
        brain.initialize()
        
        # Run simulation
        start_time = time.time()
        for i in range(steps):
            brain.step(i)
        end_time = time.time()
        
        times.append(end_time - start_time)
        firing_rates.append(brain.getAverageFiringRate())
        spike_counts.append(brain.getTotalSpikeCount())
    
    return {
        'avg_firing_rate': sum(firing_rates) / iterations,
        'total_spikes_per_iteration': sum(spike_counts) / iterations,
        'avg_time_per_iteration': sum(times) / iterations,
        'steps_per_second': sum(steps / t for t in times) / iterations,
        'total_steps': steps * iterations,
        'total_time': sum(times),
    }

# Configuration utilities
import json
import os

def load_config_from_json(filepath):
    """Load configuration from a JSON file.
    
    Args:
        filepath (str): Path to JSON configuration file
        
    Returns:
        Config: Configuration loaded from JSON
    """
    with open(filepath, 'r') as f:
        config_data = json.load(f)
    
    config = create_default_config()
    for key, value in config_data.items():
        config.set(key, value)
    
    return config

def save_config_to_json(config, filepath):
    """Save configuration to a JSON file.
    
    Args:
        config (Config): Configuration to save
        filepath (str): Path where to save JSON file
    """
    config_dict = {}
    for key in config.getKeys():
        config_dict[key] = config.getOr[key]
    
    with open(filepath, 'w') as f:
        json.dump(config_dict, f, indent=2)

def merge_configs(*configs):
    """Merge multiple configurations into one.
    
    Args:
        *configs: Multiple Config objects to merge
        
    Returns:
        Config: Merged configuration
    """
    merged = create_default_config()
    for config in configs:
        for key in config.getKeys():
            merged.set(key, config.getOr[key])
    return merged

# Simulation utilities
def run_agent_simulation(brain, world, agent, steps):
    """Run a complete agent simulation.
    
    Args:
        brain (Brain): Neural brain
        world (SimpleWorld): Environment world
        agent (AgentBrain): Agent interface
        steps (int): Number of simulation steps to run
        
    Returns:
        dict: Simulation results including final position,
              total reward, and brain statistics
    """
    for i in range(steps):
        # Get sensory input from world
        percept = world.getSensoryPercept()
        
        # Process sensory input
        agent.processSensoryInput(percept)
        
        # Brain step
        brain.step(i)
        
        # Get motor command
        motor_cmd = agent.decodeMotorCommand()
        
        # Apply action to world
        world.applyMotorCommand(motor_cmd, world.getSimulationTime())
    
    return {
        'final_position': world.getAgentBody(),
        'total_steps': steps,
        'final_brain_state': brain,
        'avg_firing_rate': brain.getAverageFiringRate(),
        'total_spikes': brain.getTotalSpikeCount(),
    }

# Validation utilities
def validate_config(config):
    """Validate that a configuration has essential settings.
    
    Args:
        config (Config): Configuration to validate
        
    Returns:
        list: List of validation errors (empty if valid)
    """
    errors = []
    
    if not config.has("neuron_count") or config.getOr[int]("neuron_count") <= 0:
        errors.append("neuron_count must be a positive integer")
    
    if not config.has("simulation_timestep") or config.getOr[float]("simulation_timestep") <= 0:
        errors.append("simulation_timestep must be a positive number")
    
    if config.has("region_count") and config.getOr[int]("region_count") <= 0:
        errors.append("region_count must be a positive integer")
    
    if config.has("connection_probability") and \
       (config.getOr[float]("connection_probability") < 0 or config.getOr[float]("connection_probability") > 1):
        errors.append("connection_probability must be between 0 and 1")
    
    return errors

# Debug utilities
def debug_brain(brain, steps=100):
    """Debug brain by logging its state and behavior.
    
    Args:
        brain (Brain): Brain to debug
        steps (int): Number of steps to debug
        
    Returns:
        dict: Debug information including spike statistics,
              neuron counts, and connectivity information
    """
    stats = {
        'steps_run': 0,
        'total_spikes': 0,
        'firing_neurons': 0,
        'average_firing_rate': 0,
        'region_count': brain.getRegionCount(),
        'neuron_counts_by_region': [],
        'connection_counts_by_region': [],
        'developmental_stage': brain.getDevelopmentalStage(),
    }
    
    for i in range(steps):
        brain.step(i)
        
        stats['total_spikes'] += brain.getTotalSpikeCount()
        stats['firing_neurons'] += brain.getFiringNeuronCount()
        
        # Collect per-region statistics
        for region_id in brain.getRegionIds():
            region = brain.getRegion(region_id)
            stats['neuron_counts_by_region'].append(region.getNeuronCount())
            stats['connection_counts_by_region'].append(region.getSynapseCount())
    
    stats['steps_run'] = steps
    stats['total_spikes'] = int(stats['total_spikes'] / steps)
    stats['firing_neurons'] = int(stats['firing_neurons'] / steps)
    stats['average_firing_rate'] = brain.getAverageFiringRate()
    
    return stats
