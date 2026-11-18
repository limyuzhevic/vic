'''Helper utilities and advanced features for pynlm'''

from __future__ import annotations

from typing import Dict, List, Optional, Any, Union, Callable, Type
import time
import json
import math

from .core.types import (
    NeuronId, SynapseId, RegionId, PopulationId, SimulationStep, Timestamp
)
from .core.enums import (
    NeuronType, SynapseType, DevelopmentalStage, FiringState,
    ActionType, MotorCommand, WorldObjectType
)
from .brain import Brain, NeuralRegion
from .world import SimpleWorld
from .agent import AgentBrain
from .config import Config
from .sensory import Vision, Audio, InternalSignals, SensoryInput
from .motor import Action, WorldObject, AgentBody, ActionResult, SensoryPercept
from .visualization import VisualizationUtils


class PynlmException(Exception):
    """Base exception for pynlm errors."""
    pass


class ConfigurationError(PynlmException):
    """Error in configuration."""
    pass


class SimulationError(PynlmException):
    """Error in simulation."""
    pass


class VisualizationError(PynlmException):
    """Error in visualization."""
    pass


def createDefaultConfig() -> Config:
    """Create a default configuration.
    
    Returns:
        Default configuration object.
    """
    config = Config()
    
    # Set default values
    config._entries = {
        'simulation/timestep': {
            'value': 0.1,
            'source': ConfigSource.DEFAULT,
            'description': 'Simulation timestep in seconds',
        },
        'simulation/delay': {
            'value': 10,
            'source': ConfigSource.DEFAULT,
            'description': 'Synaptic delay in simulation steps',
        },
        'neuron/threshold': {
            'value': 1.0,
            'source': ConfigSource.DEFAULT,
            'description': 'Neuron firing threshold',
        },
        'neuron/refractory_period': {
            'value': 0.002,
            'source': ConfigSource.DEFAULT,
            'description': 'Neuron refractory period in seconds',
        },
        'neuron/firing_period': {
            'value': 0.02,
            'source': ConfigSource.DEFAULT,
            'description': 'Neuron firing period in seconds',
        },
        'plasticity/enabled': {
            'value': True,
            'source': ConfigSource.DEFAULT,
            'description': 'Whether plasticity is enabled',
        },
        'development/enabled': {
            'value': False,
            'source': ConfigSource.DEFAULT,
            'description': 'Whether development is enabled',
        },
        'reward_modulation/enabled': {
            'value': False,
            'source': ConfigSource.DEFAULT,
            'description': 'Whether reward modulation is enabled',
        },
        'curiosity/enabled': {
            'value': False,
            'source': ConfigSource.DEFAULT,
            'description': 'Whether curiosity is enabled',
        },
        'structural_plasticity/enabled': {
            'value': False,
            'source': ConfigSource.DEFAULT,
            'description': 'Whether structural plasticity is enabled',
        },
        'development/initial_stage': {
            'value': 'Adult',
            'source': ConfigSource.DEFAULT,
            'description': 'Initial developmental stage',
        },
        'world/default_width': {
            'value': 20.0,
            'source': ConfigSource.DEFAULT,
            'description': 'Default world width',
        },
        'world/default_height': {
            'value': 15.0,
            'source': ConfigSource.DEFAULT,
            'description': 'Default world height',
        },
        'world/default_max_energy': {
            'value': 100.0,
            'source': ConfigSource.DEFAULT,
            'description': 'Default maximum energy',
        },
        'world/default_energy_decay_rate': {
            'value': 0.01,
            'source': ConfigSource.DEFAULT,
            'description': 'Default energy decay rate',
        },
    }
    
    return config


def createBrain(config: Optional[Config] = None) -> Brain:
    """Create a new brain with configuration.
    
    Args:
        config: Configuration object. If None, a default configuration
               is created.
               
    Returns:
        Brain instance.
    """
    if config is None:
        config = createDefaultConfig()
    
    return Brain(config)


def createSimpleWorld() -> SimpleWorld:
    """Create a new simple world.
    
    Returns:
        SimpleWorld instance.
    """
    world = SimpleWorld()
    
    # Configure with default dimensions
    world.configure(
        width=20.0,
        height=15.0,
        vision_width=10,
        vision_height=10
    )
    
    return world


def createAgentBrain(brain: Brain) -> AgentBrain:
    """Create a new agent brain interface.
    
    Args:
        brain: The brain to interface with.
        
    Returns:
        AgentBrain instance.
    """
    return AgentBrain(brain)


def run_simulation(brain: Brain, world: SimpleWorld, 
                  agent_brain: Optional[AgentBrain] = None,
                  steps: int = 100) -> Dict[str, Any]:
    """Run a simulation for a specified number of steps.
    
    Args:
        brain: Brain instance to simulate.
        world: World instance to simulate.
        agent_brain: Optional AgentBrain instance.
        steps: Number of simulation steps.
        
    Returns:
        Dictionary of simulation results.
    """
    results = {
        'steps': [],
        'spike_counts': [],
        'active_neuron_counts': [],
        'average_firing_rates': [],
        'agent_positions': [],
        'agent_energies': [],
        'action_counts': [],
    }
    
    # Initialize agent if provided
    if agent_brain and world:
        agent_brain.initialize(world)
    
    for step in range(steps):
        # Run brain step
        brain.step(step)
        
        # Update world
        world.update(0.1)  # Assuming 0.1 timestep
        
        # Process agent if present
        if agent_brain and world:
            # Process sensory input
            if world.getSensoryPercept():
                agent_brain.processSensoryInput(world.getSensoryPercept())
            
            # Apply motor command
            agent_brain.applyMotorCommand()
        
        # Record results
        results['steps'].append(step)
        results['spike_counts'].append(brain.getTotalSpikeCount())
        results['active_neuron_counts'].append(brain.getActiveNeuronCount())
        results['average_firing_rates'].append(brain.getAverageFiringRate())
        
        if world.agent:
            results['agent_positions'].append((world.agent.x, world.agent.y))
            results['agent_energies'].append(world.agent.energy)
        else:
            results['agent_positions'].append(None)
            results['agent_energies'].append(None)
        
        if agent_brain:
            results['action_counts'].append(agent_brain.action_count)
        else:
            results['action_counts'].append(0)
    
    return results


def run_agent_simulation(brain: Brain, world: SimpleWorld, 
                         steps: int = 100) -> Dict[str, Any]:
    """Run a simulation with an agent.
    
    Args:
        brain: Brain instance to simulate.
        world: World instance to simulate.
        steps: Number of simulation steps.
        
    Returns:
        Dictionary of simulation results.
    """
    agent_brain = createAgentBrain(brain)
    return run_simulation(brain, world, agent_brain, steps)


def benchmark_brain(brain: Brain, world: SimpleWorld, 
                    agent_brain: Optional[AgentBrain] = None,
                    steps: int = 100) -> Dict[str, float]:
    """Benchmark brain performance.
    
    Args:
        brain: Brain instance to benchmark.
        world: World instance to benchmark.
        agent_brain: Optional AgentBrain instance.
        steps: Number of steps to run.
        
    Returns:
        Dictionary of performance metrics.
    """
    start_time = time.time()
    
    # Run simulation
    results = run_simulation(brain, world, agent_brain, steps)
    
    end_time = time.time()
    elapsed_time = end_time - start_time
    
    # Calculate metrics
    metrics = {
        'total_time': elapsed_time,
        'time_per_step': elapsed_time / steps if steps > 0 else 0,
        'steps_per_second': steps / elapsed_time if elapsed_time > 0 else 0,
        'total_spikes': sum(results['spike_counts']),
        'max_active_neurons': max(results['active_neuron_counts']) if results['active_neuron_counts'] else 0,
        'average_firing_rate': sum(results['average_firing_rates']) / len(results['average_firing_rates']) if results['average_firing_rates'] else 0,
        'final_energy': results['agent_energies'][-1] if results['agent_energies'] and results['agent_energies'][-1] is not None else 0,
        'final_actions': results['action_counts'][-1] if results['action_counts'] else 0,
    }
    
    return metrics


def save_results(results: Dict[str, Any], filepath: str):
    """Save simulation results to a file.
    
    Args:
        results: Results dictionary to save.
        filepath: Path to save the file.
    """
    with open(filepath, 'w') as f:
        json.dump(results, f, indent=2)


def load_results(filepath: str) -> Dict[str, Any]:
    """Load simulation results from a file.
    
    Args:
        filepath: Path to load the file from.
        
    Returns:
        Loaded results dictionary.
    """
    with open(filepath, 'r') as f:
        return json.load(f)


def create_visualization(utils: VisualizationUtils, brain: Brain, 
                         world: SimpleWorld, agent_brain: Optional[AgentBrain] = None
                         ) -> str:
    """Create visualization files.
    
    Args:
        utils: VisualizationUtils instance.
        brain: Brain instance to visualize.
        world: World instance to visualize.
        agent_brain: Optional AgentBrain instance to visualize.
        
    Returns:
        Path to created visualization file.
    """
    # Create HTML visualization
    html_file = "visualization.html"
    utils.create_html_visualization(brain, world, agent_brain, html_file)
    
    # Create behavior plot
    if agent_brain:
        behavior_file = "behavior.png"
        utils.create_behavior_plot(agent_brain, behavior_file)
    
    # Create neural network plot
    network_file = "neural_network.png"
    utils.create_neural_network_plot(brain, network_file)
    
    return html_file


def validate_configuration(config: Config) -> List[str]:
    """Validate configuration settings.
    
    Args:
        config: Configuration to validate.
        
    Returns:
        List of validation errors (empty if valid).
    """
    errors = []
    
    # Check required keys
    required_keys = ['simulation/timestep', 'neuron/threshold']
    for key in required_keys:
        if not config.has(key):
            errors.append(f"Missing required configuration key: {key}")
    
    # Check value ranges
    if config.has('simulation/timestep') and config.getOr('simulation/timestep', 0.1) <= 0:
        errors.append("simulation/timestep must be positive")
    
    if config.has('neuron/threshold') and config.getOr('neuron/threshold', 1.0) <= 0:
        errors.append("neuron/threshold must be positive")
    
    return errors


def get_default_configuration_template() -> Dict[str, Any]:
    """Get a template for default configuration.
    
    Returns:
        Dictionary template for configuration.
    """
    return {
        'simulation/timestep': 0.1,
        'simulation/delay': 10,
        'neuron/threshold': 1.0,
        'neuron/refractory_period': 0.002,
        'neuron/firing_period': 0.02,
        'plasticity/enabled': True,
        'development/enabled': False,
        'reward_modulation/enabled': False,
        'curiosity/enabled': False,
        'structural_plasticity/enabled': False,
        'development/initial_stage': 'Adult',
        'world/default_width': 20.0,
        'world/default_height': 15.0,
        'world/default_max_energy': 100.0,
        'world/default_energy_decay_rate': 0.01,
    }


def create_preset_config(name: str) -> Config:
    """Create a preset configuration.
    
    Args:
        name: Preset configuration name.
        
    Returns:
        Preset configuration.
        
    Raises:
        ConfigurationError: If preset name is not recognized.
    """
    config = Config()
    
    presets = {
        'simple': {
            'simulation/timestep': 0.1,
            'neuron/threshold': 1.0,
            'plasticity/enabled': True,
            'development/enabled': False,
        },
        'complex': {
            'simulation/timestep': 0.01,
            'neuron/threshold': 0.5,
            'neuron/refractory_period': 0.001,
            'neuron/firing_period': 0.01,
            'plasticity/enabled': True,
            'development/enabled': True,
            'reward_modulation/enabled': True,
            'curiosity/enabled': True,
            'structural_plasticity/enabled': True,
            'development/initial_stage': 'Initial',
        },
        'fast': {
            'simulation/timestep': 0.5,
            'neuron/threshold': 2.0,
            'neuron/refractory_period': 0.01,
            'neuron/firing_period': 0.05,
            'plasticity/enabled': False,
            'development/enabled': False,
        },
    }
    
    if name.lower() not in presets:
        raise ConfigurationError(f"Unknown preset configuration: {name}")
    
    for key, value in presets[name.lower()].items():
        config._entries[key] = {
            'value': value,
            'source': ConfigSource.DEFAULT,
            'description': f"Preset configuration for {name}: {key}",
        }
    
    return config


def export_to_json(obj: Any, filepath: str):
    """Export object to JSON file.
    
    Args:
        obj: Object to export.
        filepath: Path to save the JSON file.
    """
    # Convert object to JSON-serializable format
    if hasattr(obj, '__dict__'):
        data = obj.__dict__
    else:
        data = obj
    
    with open(filepath, 'w') as f:
        json.dump(data, f, indent=2)


def import_from_json(filepath: str) -> Dict[str, Any]:
    """Import object from JSON file.
    
    Args:
        filepath: Path to load the JSON file from.
        
    Returns:
        Imported data.
    """
    with open(filepath, 'r') as f:
        return json.load(f)


def calculate_neural_density(brain: Brain) -> float:
    """Calculate neural density of the brain.
    
    Args:
        brain: Brain instance to analyze.
        
    Returns:
        Neural density (neurons per region).
    """
    if brain.getRegionCount() == 0:
        return 0.0
    
    return brain.getTotalNeuronCount() / brain.getRegionCount()


def calculate_connectivity(brain: Brain) -> float:
    """Calculate brain connectivity ratio.
    
    Args:
        brain: Brain instance to analyze.
        
    Returns:
        Connectivity ratio (synapses per neuron).
    """
    neurons = brain.getTotalNeuronCount()
    if neurons == 0:
        return 0.0
    
    return brain.getTotalSynapseCount() / neurons


def analyze_simulation_results(results: Dict[str, Any]) -> Dict[str, Any]:
    """Analyze simulation results.
    
    Args:
        results: Simulation results dictionary.
        
    Returns:
        Analysis results.
    """
    analysis = {}
    
    # Analyze time series
    if 'steps' in results and 'spike_counts' in results:
        analysis['spike_analysis'] = {
            'max_spikes': max(results['spike_counts']),
            'avg_spikes_per_step': sum(results['spike_counts']) / len(results['spike_counts']) if results['spike_counts'] else 0,
            'spike_variance': sum((x - sum(results['spike_counts']) / len(results['spike_counts']) if results['spike_counts'] else 0) ** 2 
                                 for x in results['spike_counts']) / len(results['spike_counts']) if results['spike_counts'] else 0,
        }
    
    if 'active_neuron_counts' in results:
        analysis['activity_analysis'] = {
            'max_active_neurons': max(results['active_neuron_counts']),
            'avg_active_neurons': sum(results['active_neuron_counts']) / len(results['active_neuron_counts']) if results['active_neuron_counts'] else 0,
        }
    
    if 'average_firing_rates' in results:
        analysis['firing_analysis'] = {
            'max_firing_rate': max(results['average_firing_rates']),
            'avg_firing_rate': sum(results['average_firing_rates']) / len(results['average_firing_rates']) if results['average_firing_rates'] else 0,
        }
    
    if 'agent_energies' in results and results['agent_energies'] and results['agent_energies'][-1] is not None:
        analysis['energy_analysis'] = {
            'final_energy': results['agent_energies'][-1],
            'min_energy': min(results['agent_energies']),
            'max_energy': max(results['agent_energies']),
        }
    
    if 'action_counts' in results:
        analysis['behavior_analysis'] = {
            'total_actions': sum(results['action_counts']),
            'final_action_count': results['action_counts'][-1] if results['action_counts'] else 0,
        }
    
    return analysis


def create_advanced_features_demo(brain: Brain, world: SimpleWorld,
                                   agent_brain: Optional[AgentBrain] = None
                                   ) -> Dict[str, Any]:
    """Create demonstration of advanced features.
    
    Args:
        brain: Brain instance.
        world: World instance.
        agent_brain: Optional AgentBrain instance.
        
    Returns:
        Demonstration results.
    """
    demo_results = {
        'network_info': {
            'neuron_density': calculate_neural_density(brain),
            'connectivity_ratio': calculate_connectivity(brain),
        },
        'behavior_demo': {},
        'visualization_demo': {},
    }
    
    if agent_brain:
        demo_results['behavior_demo'] = {
            'developmental_stage': agent_brain.getDevelopmentalStage().name,
            'neuromodulation_level': agent_brain.getNeuromodulationLevel(),
            'curiosity_level': agent_brain.getCuriosityLevel(),
            'novelty_level': agent_brain.getNoveltyLevel(),
            'prediction_error': agent_brain.getPredictionError(),
        }
    
    # Create visualizations
    utils = VisualizationUtils()
    
    if agent_brain:
        demo_results['visualization_demo']['behavior'] = utils.create_behavior_plot(agent_brain)
    
    demo_results['visualization_demo']['network'] = utils.create_neural_network_plot(brain)
    demo_results['visualization_demo']['simulation'] = utils.create_simulation_animation(brain, world, 50)
    
    return demo_results


__all__ = [
    # Exceptions
    'PynlmException', 'ConfigurationError', 'SimulationError', 'VisualizationError',
    # Factory functions
    'createDefaultConfig', 'createBrain', 'createSimpleWorld', 'createAgentBrain',
    # Simulation functions
    'run_simulation', 'run_agent_simulation', 'benchmark_brain',
    # File operations
    'save_results', 'load_results', 'export_to_json', 'import_from_json',
    # Configuration utilities
    'validate_configuration', 'get_default_configuration_template', 'create_preset_config',
    # Analysis functions
    'calculate_neural_density', 'calculate_connectivity', 'analyze_simulation_results',
    # Advanced features
    'create_advanced_features_demo',
    # Visualization
    'create_visualization',
    # Constants
    'VisualizationUtils',
]