"""
Core types and utilities for NLM Python bindings.
"""

import ctypes
import struct
from typing import Dict, List, Any, Optional, Union
import json

# Core type definitions
try:
    from pynlm.bindings import NeuronId, SynapseId, RegionId, PopulationId
    from pynlm.bindings import NeuronType, SynapseType, DevelopmentalStage
    from pynlm.bindings import FiringState, ActionType, MotorCommand, WorldObjectType
    from pynlm.bindings import SimulationStep, Timestamp
    from pynlm.bindings import INVALID_NEURON_ID, INVALID_SYNAPSE_ID
    from pynlm.bindings import INVALID_REGION_ID, INVALID_POPULATION_ID
    
    BINDINGS_AVAILABLE = True
except ImportError:
    BINDINGS_AVAILABLE = False
    print("Warning: C++ bindings not available. Some functionality will be limited.")


def validate_config(config_dict: Dict[str, Any]) -> Dict[str, Any]:
    """Validate configuration dictionary.
    
    Args:
        config_dict: Configuration dictionary to validate
    
    Returns:
        Validated configuration dictionary
    
    Raises:
        ValueError: If configuration is invalid
    """
    valid_sections = {
        'brain', 'plasticity', 'neuromod', 'memory', 'cognition',
        'sensory', 'motor', 'prediction', 'development'
    }
    
    for key in config_dict:
        section = key.split('.')[0] if '.' in key else key
        if section not in valid_sections:
            raise ValueError(f"Invalid configuration section: {section}")
    
    return config_dict


def configure_default():
    """Create default configuration."""
    from pynlm.bindings import createDefaultConfig
    config = createDefaultConfig()
    
    # Apply sensible defaults
    default_params = {
        'brain.neuron_count': 1000,
        'brain.synapse_density': 0.1,
        'brain.connection_probability': 0.05,
        'brain.initial_weight_mean': 0.5,
        'brain.initial_weight_std': 0.1,
        'brain.v_thresh': -50.0,
        'brain.v_rest': -70.0,
        'brain.v_reset': -75.0,
        'brain.tau_mem': 20.0,
        'brain.tau_ref': 2.0,
        'plasticity.stdp.enable': True,
        'plasticity.stdp.learning_rate': 0.001,
        'plasticity.stdp.tau_plus': 20.0,
        'plasticity.stdp.tau_minus': 20.0,
        'plasticity.hebbian.enable': True,
        'plasticity.structural.enable': True,
        'neuromod.dopamine.scale': 1.0,
        'neuromod.curiosity.enable': True,
        'neuromod.novelty.enable': True,
    }
    
    for key, value in default_params.items():
        config.set(key, value)
    
    return config


def format_number(value: float, precision: int = 3) -> str:
    """Format a number with appropriate precision.
    
    Args:
        value: Number to format
        precision: Number of decimal places
    
    Returns:
        Formatted number string
    """
    if abs(value) < 0.001:
        return f"{value:.2e}"
    elif abs(value) >= 1000:
        return f"{value:.1e}"
    else:
        return f"{value:.{precision}f}"


def calculate_statistics(values: List[float]) -> Dict[str, float]:
    """Calculate basic statistics for a list of values.
    
    Args:
        values: List of numeric values
    
    Returns:
        Dictionary with statistics
    """
    if not values:
        return {'count': 0, 'mean': 0.0, 'std': 0.0, 'min': 0.0, 'max': 0.0}
    
    mean = sum(values) / len(values)
    variance = sum((x - mean) ** 2 for x in values) / len(values)
    std = variance ** 0.5
    
    return {
        'count': len(values),
        'mean': mean,
        'std': std,
        'min': min(values),
        'max': max(values),
        'range': max(values) - min(values),
    }


def generate_report(brain=None, world=None, agent=None, steps=0):
    """Generate a summary report of simulation state.
    
    Args:
        brain: Brain object (optional)
        world: World object (optional)
        agent: AgentBrain object (optional)
        steps: Number of simulation steps
    
    Returns:
        Formatted report string
    """
    lines = ["NLM Simulation Report", "=" * 50]
    
    if brain:
        lines.append(f"Brain Statistics:")
        lines.append(f"  Neurons: {brain.getTotalNeuronCount()}")
        lines.append(f"  Synapses: {brain.getTotalSynapseCount()}")
        lines.append(f"  Active: {brain.getActiveNeuronCount()}")
        lines.append(f"  Firing: {brain.getFiringNeuronCount()}")
        lines.append(f"  Average Rate: {brain.getAverageFiringRate():.2f} Hz")
        lines.append(f"  E/I Ratio: {brain.getExcitationInhibitionRatio():.3f}")
        lines.append(f"  Total Spikes: {brain.getTotalSpikeCount()}")
        lines.append(f"  Development Stage: {brain.getDevelopmentalStage()}")
        lines.append("")
    
    if agent:
        lines.append(f"Agent Statistics:")
        lines.append(f"  Neuromodulation Level: {agent.getNeuromodulationLevel():.3f}")
        lines.append(f"  Curiosity Level: {agent.getCuriosityLevel():.3f}")
        lines.append(f"  Novelty Level: {agent.getNoveltyLevel():.3f}")
        lines.append(f"  Prediction Error: {agent.getPredictionError():.3f}")
        lines.append(f"  Development Stage: {agent.getDevelopmentalStage()}")
        lines.append("")
    
    if world:
        lines.append(f"World Statistics:")
        lines.append(f"  Width: {world.getWidth()}")
        lines.append(f"  Height: {world.getHeight()}")
        lines.append(f"  Time: {world.getSimulationTime():.3f}")
        body = world.getAgentBody()
        lines.append(f"  Agent Position: ({body.x:.1f}, {body.y:.1f})")
        lines.append(f"  Agent Energy: {body.energy:.1f}")
        lines.append("")
    
    if steps:
        lines.append(f"Simulation:")
        lines.append(f"  Steps: {steps}")
        lines.append("")
    
    return "\n".join(lines)


def export_data(brain, world, agent, steps=0, filepath="nlm_data.json"):
    """Export simulation data to JSON file.
    
    Args:
        brain: Brain object
        world: World object
        agent: AgentBrain object
        steps: Number of steps to record
        filepath: Output file path
    """
    data = {
        'config': {},
        'brain_stats': {},
        'world_stats': {},
        'agent_stats': {},
        'steps': steps,
    }
    
    # Try to get config data
    try:
        config = brain.getConfig()
        data['config'] = {
            'neuron_count': config.get('brain.neuron_count', 0),
            'synapse_density': config.get('brain.synapse_density', 0.0),
            'learning_rate': config.get('plasticity.stdp.learning_rate', 0.0),
        }
    except:
        pass
    
    # Brain stats
    data['brain_stats'] = {
        'total_neurons': brain.getTotalNeuronCount(),
        'total_synapses': brain.getTotalSynapseCount(),
        'active_neurons': brain.getActiveNeuronCount(),
        'firing_neurons': brain.getFiringNeuronCount(),
        'average_firing_rate': brain.getAverageFiringRate(),
        'excitation_inhibition_ratio': brain.getExcitationInhibitionRatio(),
        'total_spikes': brain.getTotalSpikeCount(),
        'developmental_stage': str(brain.getDevelopmentalStage()),
    }
    
    # World stats
    if world:
        body = world.getAgentBody()
        data['world_stats'] = {
            'width': world.getWidth(),
            'height': world.getHeight(),
            'simulation_time': world.getSimulationTime(),
            'agent_x': body.x,
            'agent_y': body.y,
            'agent_orientation': body.orientation,
            'agent_energy': body.energy,
            'agent_health': body.health,
        }
    
    # Agent stats
    if agent:
        data['agent_stats'] = {
            'neuromodulation_level': agent.getNeuromodulationLevel(),
            'curiosity_level': agent.getCuriosityLevel(),
            'novelty_level': agent.getNoveltyLevel(),
            'prediction_error': agent.getPredictionError(),
            'developmental_stage': str(agent.getDevelopmentalStage()),
        }
    
    # Write to file
    with open(filepath, 'w') as f:
        json.dump(data, f, indent=2)


class NLMError(Exception):
    """Base exception for NLM errors."""
    pass


class NLMConfigurationError(NLMError):
    """Error in configuration."""
    pass


class NLMInitializationError(NLMError):
    """Error during initialization."""
    pass


class NLMExecutionError(NLMError):
    """Error during execution."""
    pass
