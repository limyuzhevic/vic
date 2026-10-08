# NLM Python utilities and helper functions

import numpy as np
from typing import Dict, List, Optional, Union, Any, Callable
from pathlib import Path
import json
import logging
from contextlib import contextmanager
import time

# Configure logging
logging.basicConfig(level=logging.INFO)
logger = logging.getLogger(__name__)

# Import NLM bindings
from .bindings import (
    Config, Brain, AgentBrain, SimpleWorld,
    Vision, Audio, InternalSignals, Action, WorldObject,
    AgentBody, ActionResult, SensoryPercept,
    NeuronId, SynapseId, RegionId, PopulationId,
    NeuronType, SynapseType, DevelopmentalStage, FiringState,
    ActionType, MotorCommand, WorldObjectType,
    InvalidArgumentError, ConfigurationError, BrainInitializationError
)

__all__ = [
    'ConfigUtils',
    'BrainUtils',
    'WorldUtils',
    'DataUtils',
    'PlotUtils',
    'SimulationUtils',
    'DevelopmentUtils',
    'NeuromodulationUtils',
    'MemoryUtils',
    'PredictionUtils',
    'create_simulation_config',
    'create_standard_environment',
    'create_default_agent_config',
    'run_benchmark',
    'monitor_performance',
    'validate_configuration',
    'export_brain_state',
    'import_brain_state',
    'create_brain_from_template',
    'create_agent_from_brain',
    'simulate_episode',
    'compare_brains',
    'analyze_neural_dynamics',
]

class ConfigUtils:
    """Utilities for configuration management."""
    
    @staticmethod
    def create_default_config() -> Config:
        """Create a default NLM configuration with optimal settings."""
        config = create_default_config()
        
        # Set sensible defaults for research
        config.set("neuron_count", 1000, ConfigSource::Default)
        config.set("region_count", 4, ConfigSource::Default)
        config.set("connection_probability", 0.1, ConfigSource::Default)
        config.set("plasticity.stdp.learning_rate", 0.01, ConfigSource::Default)
        config.set("plasticity.hebbian.learning_rate", 0.001, ConfigSource::Default)
        config.set("neuromodulation.dopamine.strength", 1.0, ConfigSource::Default)
        config.set("neuromodulation.curiosity.strength", 0.5, ConfigSource::Default)
        config.set("neuromodulation.novelty.strength", 0.3, ConfigSource::Default)
        config.set("development.initial_plasticity", 1.0, ConfigSource::Default)
        config.set("development.maturation_rate", 0.01, ConfigSource::Default)
        
        return config
    
    @staticmethod
    def create_research_config(neuron_count: int = 1000) -> Config:
        """Create a configuration optimized for research experiments."""
        config = create_default_config()
        
        # Research-optimized settings
        config.set("neuron_count", neuron_count, ConfigSource::Default)
        config.set("region_count", min(neuron_count // 100, 10), ConfigSource::Default)
        config.set("connection_probability", 0.15, ConfigSource::Default)
        config.set("plasticity.stdp.learning_rate", 0.02, ConfigSource::Default)
        config.set("plasticity.hebbian.learning_rate", 0.005, ConfigSource::Default)
        config.set("plasticity.structural_plasticity.enabled", True, ConfigSource::Default)
        config.set("neuromodulation.dopamine.strength", 1.5, ConfigSource::Default)
        config.set("neuromodulation.curiosity.strength", 0.8, ConfigSource::Default)
        config.set("neuromodulation.novelty.strength", 0.6, ConfigSource::Default)
        config.set("development.initial_plasticity", 1.2, ConfigSource::Default)
        config.set("development.maturation_rate", 0.02, ConfigSource::Default)
        config.set("development.enable_synaptogenesis", True, ConfigSource::Default)
        config.set("development.enable_pruning", True, ConfigSource::Default)
        
        return config
    
    @staticmethod
    def create_development_config() -> Config:
        """Create a configuration optimized for developmental learning."""
        config = create_default_config()
        
        # Development-focused settings
        config.set("neuron_count", 500, ConfigSource::Default)
        config.set("region_count", 3, ConfigSource::Default)
        config.set("connection_probability", 0.2, ConfigSource::Default)
        config.set("plasticity.stdp.learning_rate", 0.005, ConfigSource::Default)
        config.set("plasticity.hebbian.learning_rate", 0.001, ConfigSource::Default)
        config.set("development.initial_plasticity", 2.0, ConfigSource::Default)
        config.set("development.maturation_rate", 0.005, ConfigSource::Default)
        config.set("development.enable_synaptogenesis", True, ConfigSource::Default)
        config.set("development.enable_pruning", True, ConfigSource::Default)
        config.set("development.critical_period.enabled", True, ConfigSource::Default)
        config.set("development.critical_period.duration", 100, ConfigSource::Default)
        config.set("development.critical_period.sensitivity", 1.5, ConfigSource::Default)
        
        return config
    
    @staticmethod
    def create_validation_config() -> Config:
        """Create a configuration for validation and testing."""
        config = create_default_config()
        
        # Validation settings
        config.set("neuron_count", 100, ConfigSource::Default)
        config.set("region_count", 2, ConfigSource::Default)
        config.set("connection_probability", 0.1, ConfigSource::Default)
        config.set("plasticity.stdp.learning_rate", 0.01, ConfigSource::Default)
        config.set("plasticity.hebbian.learning_rate", 0.001, ConfigSource::Default)
        config.set("neuromodulation.dopamine.strength", 1.0, ConfigSource::Default)
        config.set("neuromodulation.curiosity.strength", 0.5, ConfigSource::Default)
        config.set("neuromodulation.novelty.strength", 0.3, ConfigSource::Default)
        config.set("development.initial_plasticity", 1.0, ConfigSource::Default)
        config.set("development.maturation_rate", 0.01, ConfigSource::Default)
        
        return config
class BrainUtils:
    """Utilities for brain simulation and analysis."""
    
    @staticmethod
    def create_brain_with_config(config: Config) -> Brain:
        """Create and initialize a brain with the given configuration."""
        brain = create_brain(config)
        if not brain.is_initialized():
            raise BrainInitializationError("Failed to initialize brain")
        return brain
    
    @staticmethod
    def create_standard_brain(neuron_count: int = 1000) -> Brain:
        """Create a brain with standard settings."""
        config = ConfigUtils.create_research_config(neuron_count)
        return BrainUtils.create_brain_with_config(config)
    
    @staticmethod
    def get_neuron_statistics(brain: Brain) -> Dict[str, Any]:
        """Get comprehensive neuron statistics."""
        return {
            'total_neurons': brain.get_total_neuron_count(),
            'active_neurons': brain.get_active_neuron_count(),
            'firing_neurons': brain.get_firing_neuron_count(),
            'total_spikes': brain.get_total_spike_count(),
            'e_i_ratio': brain.get_excitation_inhibition_ratio(),
            'average_firing_rate': brain.get_average_firing_rate(),
            'region_count': brain.get_region_count(),
            'initialized': brain.is_initialized()
        }
    
    @staticmethod
    def simulate_step(brain: Brain, step: SimulationStep, time: Timestamp = 0) -> None:
        """Run a single simulation step."""
        brain.run_step(step, time)
    
    @staticmethod
    def run_simulation(brain: Brain, steps: int, time_step: float = 1.0) -> List[Dict[str, Any]]:
        """Run a simulation and collect statistics at each step."""
        statistics = []
        for step in range(steps):
            time = step * time_step
            brain.run_step(step, time)
            statistics.append(BrainUtils.get_neuron_statistics(brain))
        return statistics
class WorldUtils:
    """Utilities for world simulation and environment setup."""
    
    @staticmethod
    def create_standard_environment(width: int = 100, height: int = 100) -> SimpleWorld:
        """Create a standard 2D environment for agent simulation."""
        world = create_simple_world()
        world.configure(width, height, 20, 20)
        return world
    
    @staticmethod
    def create_empty_world(width: int = 100, height: int = 100, vision_width: int = 20, vision_height: int = 20) -> SimpleWorld:
        """Create an empty world with specified dimensions."""
        world = create_simple_world()
        world.configure(width, height, vision_width, vision_height)
        world.reset()
        return world
    
    @staticmethod
    def add_resources(world: SimpleWorld, count: int = 10) -> List[WorldObject>:
        """Add resource objects to the world."""
        resources = []
        import random
        for _ in range(count):
            x = random.uniform(10, world.get_width() - 10)
            y = random.uniform(10, world.get_height() - 10)
            obj = WorldObject(x, y, WorldObjectType::Resource, 1.0)
            world.add_object(obj)
            resources.append(obj)
        return resources
    
    @staticmethod
    def add_hazards(world: SimpleWorld, count: int = 5) -> List[WorldObject]:
        """Add hazard objects to the world."""
        hazards = []
        import random
        for _ in range(count):
            x = random.uniform(5, world.get_width() - 5)
            y = random.uniform(5, world.get_height() - 5)
            obj = WorldObject(x, y, WorldObjectType::Hazard, 1.0)
            world.add_object(obj)
            hazards.append(obj)
        return hazards
class DataUtils:
    """Utilities for data processing and analysis."""
    
    @staticmethod
    def to_numpy(data: Union[List[float], np.ndarray]) -> np.ndarray:
        """Convert data to NumPy array."""
        if isinstance(data, np.ndarray):
            return data
        return np.array(data, dtype=np.float32)
    
    @staticmethod
    def from_numpy(array: np.ndarray) -> List[float]:
        """Convert NumPy array to Python list."""
        return array.flatten().tolist()
    
    @staticmethod
    def normalize(data: List[float]) -> List[float]:
        """Normalize data to [0, 1] range."""
        if not data:
            return data
        min_val = min(data)
        max_val = max(data)
        if max_val == min_val:
            return [0.5 for _ in data]
        return [(x - min_val) / (max_val - min_val) for x in data]
    
    @staticmethod
    def convert_to_dict(data: Dict[str, Any]) -> Dict[str, Any]:
        """Convert data to a dictionary suitable for JSON serialization."""
        def convert_value(value):
            if isinstance(value, np.ndarray):
                return value.tolist()
            elif isinstance(value, list):
                return [convert_value(item) for item in value]
            elif isinstance(value, dict):
                return {k: convert_value(v) for k, v in value.items()}
            else:
                return value
        
        return convert_value(data)
    
    @staticmethod
    def save_to_file(data: Dict[str, Any], filepath: str) -> None:
        """Save data to a JSON file."""
        with open(filepath, 'w') as f:
            json.dump(DataUtils.convert_to_dict(data), f, indent=2)
    
    @staticmethod
    def load_from_file(filepath: str) -> Dict[str, Any]:
        """Load data from a JSON file."""
        with open(filepath, 'r') as f:
            return json.load(f)
class PlotUtils:
    """Utilities for plotting and visualization."""
    
    @staticmethod
    def plot_neuron_statistics(statistics: List[Dict[str, Any]], output_file: Optional[str] = None):
        """Plot neuron statistics over time."""
        try:
            import matplotlib.pyplot as plt
            
            fig, axes = plt.subplots(2, 2, figsize=(12, 10))
            
            # Total neurons
            axes[0, 0].plot([s['total_neurons'] for s in statistics])
            axes[0, 0].set_title('Total Neurons')
            axes[0, 0].set_xlabel('Step')
            axes[0, 0].set_ylabel('Count')
            
            # Active neurons
            axes[0, 1].plot([s['active_neurons'] for s in statistics])
            axes[0, 1].set_title('Active Neurons')
            axes[0, 1].set_xlabel('Step')
            axes[0, 1].set_ylabel('Count')
            
            # Firing rate
            axes[1, 0].plot([s['average_firing_rate'] for s in statistics])
            axes[1, 0].set_title('Average Firing Rate')
            axes[1, 0].set_xlabel('Step')
            axes[1, 0].set_ylabel('Hz')
            
            # E/I ratio
            axes[1, 1].plot([s['e_i_ratio'] for s in statistics])
            axes[1, 1].set_title('Excitation/Inhibition Ratio')
            axes[1, 1].set_xlabel('Step')
            axes[1, 1].set_ylabel('Ratio')
            
            plt.tight_layout()
            
            if output_file:
                plt.savefig(output_file, dpi=150, bbox_inches='tight')
            
            plt.show()
            
        except ImportError:
            logger.warning("Matplotlib not available. Skipping plotting.")
    
    @staticmethod
    def plot_world_view(world: SimpleWorld, output_file: Optional[str] = None):
        """Plot a 2D view of the world."""
        try:
            import matplotlib.pyplot as plt
            
            fig, ax = plt.subplots(figsize=(10, 10))
            
            # Draw world boundaries
            ax.plot([0, world.get_width(), world.get_width(), 0, 0],
                    [0, 0, world.get_height(), world.get_height(), 0], 'k-')
            
            # Draw agent
            agent = world.get_agent_body()
            if agent:
                ax.plot(agent.x, agent.y, 'ro', markersize=10, label='Agent')
            
            # Draw objects
            # Note: SimpleWorld doesn't expose objects directly, so this is a placeholder
            # For a full implementation, you'd need to add object access methods
            
            ax.set_xlim(-5, world.get_width() + 5)
            ax.set_ylim(-5, world.get_height() + 5)
            ax.set_aspect('equal')
            ax.legend()
            ax.set_title('World View')
            
            if output_file:
                plt.savefig(output_file, dpi=150, bbox_inches='tight')
            
            plt.show()
            
        except ImportError:
            logger.warning("Matplotlib not available. Skipping plotting.")
class SimulationUtils:
    """Utilities for running and monitoring simulations."""
    
    @staticmethod
    @contextmanager
    def monitor_performance():
        """Context manager to monitor simulation performance."""
        start_time = time.time()
        try:
            yield
        finally:
            end_time = time.time()
            logger.info(f"Simulation completed in {end_time - start_time:.2f} seconds")
    
    @staticmethod
    def run_benchmark(brain: Brain, world: SimpleWorld, steps: int, iterations: int = 5) -> Dict[str, float]:
        """Run a benchmark and return performance metrics."""
        times = []
        
        for iteration in range(iterations):
            with SimulationUtils.monitor_performance():
                start_time = time.time()
                for step in range(steps):
                    brain.run_step(step, step * 1.0)
                    world.update(step)
                end_time = time.time()
                times.append(end_time - start_time)
        
        return {
            'mean_time': np.mean(times),
            'std_time': np.std(times),
            'min_time': np.min(times),
            'max_time': np.max(times),
            'steps_per_second': steps / np.mean(times)
        }
class DevelopmentUtils:
    """Utilities for developmental learning simulations."""
    
    @staticmethod
    def advance_development(agent: AgentBrain, steps: int, time_step: float = 1.0):
        """Advance agent development over time."""
        for step in range(steps):
            agent.update_development(step * time_step)
    
    @staticmethod
    def evaluate_development(stages: List[Dict[str, Any]]) -> Dict[str, Any]:
        """Evaluate developmental progress."""
        if not stages:
            return {}
        
        return {
            'initial_stage': stages[0].get('developmental_stage', 0),
            'final_stage': stages[-1].get('developmental_stage', 0),
            'stage_progression': [s.get('developmental_stage', 0) for s in stages],
            'development_rate': (stages[-1].get('developmental_stage', 0) - stages[0].get('developmental_stage', 0)) / len(stages)
        }
class NeuromodulationUtils:
    """Utilities for neuromodulation analysis."""
    
    @staticmethod
    def analyze_neuromodulation_levels(agent: AgentBrain, steps: int) -> Dict[str, List[float]]:
        """Analyze neuromodulation levels over time."""
        levels = {
            'dopamine': [],
            'curiosity': [],
            'novelty': [],
            'prediction_error': []
        }
        
        for step in range(steps):
            status = agent.get_agent_status()
            levels['dopamine'].append(status['neuromodulation_level'])
            levels['curiosity'].append(status['curiosity_level'])
            levels['novelty'].append(status['novelty_level'])
            levels['prediction_error'].append(status['prediction_error'])
        
        return levels
class MemoryUtils:
    """Utilities for memory system analysis."""
    
    @staticmethod
    def analyze_memory_systems(brain: Brain) -> Dict[str, Any]:
        """Analyze memory system performance."""
        # This is a placeholder - in a full implementation, you would access memory systems
        return {
            'working_memory_accessible': True,
            'episodic_memory_accessible': True,
            'associative_memory_accessible': True
        }
class PredictionUtils:
    """Utilities for prediction system analysis."""
    
    @staticmethod
    def analyze_prediction_error(agent: AgentBrain, steps: int) -> Dict[str, List[float]]:
        """Analyze prediction error over time."""
        errors = []
        for step in range(steps):
            status = agent.get_agent_status()
            errors.append(status['prediction_error'])
        
        return {
            'prediction_error': errors,
            'mean_error': np.mean(errors),
            'max_error': np.max(errors),
            'min_error': np.min(errors)
        }
# Helper functions
def create_simulation_config(neuron_count: int = 1000, development_mode: str = "research") -> Config:
    """Create a configuration for simulation with specified parameters."""
    config = ConfigUtils.create_default_config()
    
    if development_mode == "research":
        # Already set in create_research_config
        pass
    elif development_mode == "development":
        config = ConfigUtils.create_development_config()
    elif development_mode == "validation":
        config = ConfigUtils.create_validation_config()
    
    return config
def create_standard_environment(width: int = 100, height: int = 100, vision_width: int = 20, vision_height: int = 20) -> SimpleWorld:
    """Create a standard environment for agent simulation."""
    return WorldUtils.create_standard_environment(width, height, vision_width, vision_height)
def create_default_agent_config() -> Config:
    """Create a default agent configuration."""
    return create_default_config()
def run_benchmark(brain: Brain, world: SimpleWorld, steps: int, iterations: int = 5) -> Dict[str, float]:
    """Run a benchmark and return performance metrics."""
    return SimulationUtils.run_benchmark(brain, world, steps, iterations)
def monitor_performance():
    """Monitor simulation performance."""
    return SimulationUtils.monitor_performance()
def validate_configuration(config: Config) -> List[str]:
    """Validate a configuration and return any errors."""
    errors = []
    
    if not config.has("neuron_count"):
        errors.append("Missing neuron_count")
    
    if config.has("neuron_count"):
        neuron_count = config.get_or("neuron_count", 100)
        if neuron_count < 10:
            errors.append("neuron_count must be at least 10")
        elif neuron_count > 10000:
            errors.append("neuron_count must be at most 10000")
    
    return errors
def export_brain_state(brain: Brain, filepath: str) -> None:
    """Export brain state to file."""
    brain.save(filepath)
def import_brain_state(filepath: str) -> Brain:
    """Import brain state from file."""
    config = create_default_config()
    brain = Brain(config)
    
    if not brain.load(filepath):
        raise BrainInitializationError(f"Failed to load brain state from {filepath}")
    
    return brain
def create_brain_from_template(template_name: str, **kwargs) -> Brain:
    """Create a brain from a predefined template."""
    templates = {
        'simple': create_default_config(),
        'research': ConfigUtils.create_research_config(),
        'development': ConfigUtils.create_development_config(),
        'validation': ConfigUtils.create_validation_config()
    }
    
    if template_name not in templates:
        raise InvalidArgumentError(f"Unknown template: {template_name}")
    
    config = templates[template_name]
    
    # Override with provided kwargs
    for key, value in kwargs.items():
        config.set(key, value)
    
    return create_brain(config)
def create_agent_from_brain(brain: Brain) -> AgentBrain:
    """Create an agent brain from a brain."""
    return create_agent_brain(brain)
def simulate_episode(agent: AgentBrain, world: SimpleWorld, steps: int, time_step: float = 1.0) -> Dict[str, Any]:
    """Simulate a complete episode."""
    agent.reset()
    
    episode_data = {
        'steps': [],
        'agent_status': [],
        'world_status': []
    }
    
    for step in range(steps):
        time = step * time_step
        
        # Process sensory input
        percept = world.get_sensory_percept()
        if percept:
            agent.process_sensory_input(*percept)
        
        # Update world
        world.update(step)
        
        # Update agent development
        agent.update_development(step)
        
        # Record data
        episode_data['steps'].append(step)
        episode_data['agent_status'].append(agent.get_agent_status())
        episode_data['world_status'].append(world.get_world_status())
    
    return episode_data
def compare_brains(brain1: Brain, brain2: Brain, steps: int, time_step: float = 1.0) -> Dict[str, Any]:
    """Compare two brains over time."""
    stats1 = BrainUtils.run_simulation(brain1, steps, time_step)
    stats2 = BrainUtils.run_simulation(brain2, steps, time_step)
    
    comparison = {
        'statistics1': stats1,
        'statistics2': stats2,
        'difference': []
    }
    
    for i in range(min(len(stats1), len(stats2))):
        diff = {}
        for key in stats1[i].keys():
            if key in stats2[i]:
                diff[key] = stats2[i][key] - stats1[i][key]
        comparison['difference'].append(diff)
    
    return comparison
def analyze_neural_dynamics(brain: Brain, steps: int, time_step: float = 1.0) -> Dict[str, Any]:
    """Analyze neural dynamics over time."""
    stats = BrainUtils.run_simulation(brain, steps, time_step)
    
    analysis = {
        'total_neurons': [s['total_neurons'] for s in stats],
        'active_neurons': [s['active_neurons'] for s in stats],
        'firing_neurons': [s['firing_neurons'] for s in stats],
        'firing_rate': [s['average_firing_rate'] for s in stats],
        'e_i_ratio': [s['e_i_ratio'] for s in stats],
        'spike_count': [s['spike_count'] for s in stats]
    }
    
    # Calculate statistics
    for key, values in analysis.items():
        analysis[f'{key}_mean'] = np.mean(values)
        analysis[f'{key}_std'] = np.std(values)
        analysis[f'{key}_min'] = np.min(values)
        analysis[f'{key}_max'] = np.max(values)
    
    return analysis

# Re-export commonly used types for convenience
from .bindings import NeuronType, SynapseType, DevelopmentalStage, FiringState
from .bindings import ActionType, MotorCommand, WorldObjectType
from .bindings import INVALID_NEURON_ID, INVALID_SYNAPSE_ID, INVALID_REGION_ID, INVALID_POPULATION_ID

# Export main types
__all__.extend([
    'NeuronType', 'SynapseType', 'DevelopmentalStage', 'FiringState',
    'ActionType', 'MotorCommand', 'WorldObjectType',
    'INVALID_NEURON_ID', 'INVALID_SYNAPSE_ID', 'INVALID_REGION_ID', 'INVALID_POPULATION_ID'
])