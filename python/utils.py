# NLM Python Utilities

import numpy as np
from typing import Dict, List, Any, Optional, Union
from .exceptions import NLMConfigError, NLMValidationError

def createDefaultConfig():
    """Create a default configuration with sensible defaults"""
    from .bindings import Config
    return Config()

def createSimulationConfig(neuron_count=1000, region_count=4, 
                          connection_probability=0.1, random_seed=42):
    """Create a configuration optimized for simulation"""
    from .bindings import Config
    
    config = Config()
    config.set("neuron_count", neuron_count)
    config.set("region_count", region_count)
    config.set("connection_probability", connection_probability)
    config.set("random_seed", random_seed)
    return config

def createSmallBrainConfig():
    """Create a small configuration for testing and rapid prototyping"""
    from .bindings import Config
    
    config = Config()
    config.set("neuron_count", 100)
    config.set("region_count", 2)
    config.set("connection_probability", 0.15)
    config.set("random_seed", 123)
    return config

def createLargeBrainConfig(neuron_count=10000):
    """Create a large configuration for realistic simulations"""
    from .bindings import Config
    
    config = Config()
    config.set("neuron_count", neuron_count)
    config.set("region_count", 8)
    config.set("connection_probability", 0.05)
    config.set("random_seed", 456)
    return config

def createVisionFromArray(data, width, height, channels=3):
    """Create Vision sensory input from numpy array"""
    from .bindings import Vision
    
    if isinstance(data, np.ndarray):
        data = data.flatten().tolist()
    elif not isinstance(data, list):
        raise TypeError("Data must be a numpy array or list")
    
    if len(data) != width * height * channels:
        raise ValueError(f"Data size {len(data)} doesn't match vision dimensions {width}x{height}x{channels}")
    
    vision = Vision(width, height, channels)
    vision.setData(data)
    return vision

def createActionFromDict(action_dict):
    """Create Action from Python dictionary"""
    from .bindings import Action, ActionType
    
    if not isinstance(action_dict, dict):
        raise TypeError("Action dict must be a Python dictionary")
    
    if "type" not in action_dict:
        raise ValueError("Action dict must contain 'type' key")
    
    action_type = ActionType(action_dict["type"])
    parameters = action_dict.get("parameters", [])
    
    return Action(action_type, parameters)

def createSensoryPerceptFromVision(vision):
    """Create SensoryPercept from Vision input"""
    from .bindings import SensoryPercept
    
    if not hasattr(vision, 'getWidth'):
        raise TypeError("Vision object required")
    
    percept = SensoryPercept()
    percept.setVision(vision)
    return percept

def runControlledSimulation(brain, num_steps, timestep=0.001):
    """Run controlled simulation with time tracking"""
    current_time = 0.0
    
    for i in range(num_steps):
        brain.step(i, current_time)
        current_time += timestep
    
    return brain.getTotalSpikeCount()

def benchmarkBrain(brain, steps):
    """Benchmark brain performance"""
    import time
    
    start_time = time.time()
    for i in range(steps):
        brain.step(i)
    end_time = time.time()
    
    duration = end_time - start_time
    
    return {
        "steps": steps,
        "duration_seconds": duration,
        "steps_per_second": steps / duration,
        "total_spikes": brain.getTotalSpikeCount()
    }

def runStandardExperiment(brain, world, agent, steps):
    """Run a standard experiment with brain, world, and agent"""
    agent.reset()
    world.reset()
    
    total_reward = 0.0
    steps_completed = 0
    
    for i in range(steps):
        world.update(0.1)
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(i)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Simple reward system
        reward = 0.1 if action.getType().name == "MoveForward" else 0.0
        agent.applyRewardModulation(reward, 0.0)
        total_reward += reward
        steps_completed += 1
    
    return {
        "steps_completed": steps_completed,
        "total_reward": total_reward,
        "avg_reward": total_reward / steps_completed if steps_completed > 0 else 0,
        "final_time": world.getSimulationTime()
    }

def compareBrains(brain1, brain2, steps):
    """Compare two brains after running them for same number of steps"""
    comparison = {}
    
    for i in range(steps):
        brain1.step(i)
        brain2.step(i)
    
    comparison["brain1_neuron_count"] = brain1.getTotalNeuronCount()
    comparison["brain2_neuron_count"] = brain2.getTotalNeuronCount()
    comparison["brain1_spike_count"] = brain1.getTotalSpikeCount()
    comparison["brain2_spike_count"] = brain2.getTotalSpikeCount()
    comparison["brain1_firing_rate"] = brain1.getAverageFiringRate()
    comparison["brain2_firing_rate"] = brain2.getAverageFiringRate()
    comparison["brain1_ei_ratio"] = brain1.getExcitationInhibitionRatio()
    comparison["brain2_ei_ratio"] = brain2.getExcitationInhibitionRatio()
    
    return comparison

def createRandomConfig(seed=42):
    """Create a random configuration with specified seed"""
    from .bindings import Config
    
    config = Config()
    config.set("random_seed", seed)
    config.set("neuron_count", 500)
    config.set("region_count", 4)
    config.set("connection_probability", 0.1)
    return config

def validateBrainConfiguration(config):
    """Validate brain configuration and return True if valid"""
    if not hasattr(config, 'getKeys'):
        raise TypeError("Config object required")
    
    keys = config.getKeys()
    errors = []
    
    if "neuron_count" not in keys:
        errors.append("neuron_count is required")
    else:
        neuron_count = config.get("neuron_count")
        if neuron_count <= 0:
            errors.append("neuron_count must be positive")
    
    if "region_count" not in keys:
        errors.append("region_count is required")
    
    if errors:
        raise NLMValidationError("Configuration validation failed: " + "; ".join(errors))
    
    return True

def exportBrainState(brain):
    """Export brain state as Python dictionary"""
    return {
        "neuron_count": brain.getTotalNeuronCount(),
        "synapse_count": brain.getTotalSynapseCount(),
        "firing_count": brain.getFiringNeuronCount(),
        "total_spikes": brain.getTotalSpikeCount(),
        "avg_firing_rate": brain.getAverageFiringRate(),
        "ei_ratio": brain.getExcitationInhibitionRatio(),
        "development_stage": brain.getDevelopmentalStage().name,
        "current_time": brain.getCurrentTime()
    }

def importBrainState(brain, state):
    """Import brain state from dictionary (validation only)"""
    if not isinstance(state, dict):
        raise TypeError("State must be a dictionary")
    
    required_keys = ["neuron_count", "synapse_count", "firing_count", 
                     "total_spikes", "avg_firing_rate", "ei_ratio"]
    
    for key in required_keys:
        if key not in state:
            raise ValueError(f"Missing required key in state: {key}")
    
    # Note: This doesn't actually restore state, just validates format
    return True