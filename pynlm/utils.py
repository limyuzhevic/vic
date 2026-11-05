"""
Utility functions and helper classes for NLM Python API
"""

import time
import json
from typing import Optional, List, Dict, Any, Callable
from .types import *
from .core import Brain, Config, AgentBrain, SimpleWorld, NeuralRegion
from .sensory import Vision, Audio, InternalSignals, SensoryPercept
from .motor import Action, ActionResult
from .agent import AgentBody


def create_default_config() -> Config:
    """Create a default configuration for NLM simulation"""
    config = Config()
    
    # Set some sensible defaults for a simple simulation
    config.set_if_not_exists("simulation.steps_per_update", 10)
    config.set_if_not_exists("simulation.max_neurons", 500)
    config.set_if_not_exists("simulation.simulation_time", 1000.0)
    config.set_if_not_exists("world.width", 640)
    config.set_if_not_exists("world.height", 480)
    config.set_if_not_exists("brain.region_count", 5)
    config.set_if_not_exists("plasticity.stdp_enabled", True)
    config.set_if_not_exists("plasticity.hebbian_enabled", True)
    config.set_if_not_exists("neuromodulation.dopamine.enabled", True)
    config.set_if_not_exists("neuromodulation.curiosity.enabled", True)
    config.set_if_not_exists("neuromodulation.novelty.enabled", True)
    config.set_if_not_exists("memory.working.enabled", True)
    config.set_if_not_exists("memory.episodic.enabled", True)
    
    return config


def create_brain(config: Optional[Config] = None) -> Brain:
    """Create a new brain with optional configuration"""
    return Brain(config or create_default_config())


def create_simple_world() -> SimpleWorld:
    """Create a new simple world"""
    return SimpleWorld()


def create_agent_brain(brain: Brain, world) -> AgentBrain:
    """Create a new agent brain interface"""
    return AgentBrain(brain, world)


def run_simulation_step(brain: Brain, world, timestep: SimulationStep, 
                       current_time: Timestamp = None) -> Action:
    """
    Run a single simulation step and return the action
    
    This is a convenience function that handles the common simulation loop:
    1. Get sensory input from world
    2. Process input through agent brain
    3. Step the brain
    4. Return action from brain
    """
    if current_time is None:
        current_time = Timestamp(time.time())
    
    # Get sensory percept from world
    percept = world.get_sensory_percept()
    
    # Process through agent brain (if available)
    # Note: This assumes the world has an agent_brain attribute
    if hasattr(world, 'agent_brain'):
        world.agent_brain.processSensoryInput(percept)
    
    # Step the brain
    brain.step(timestep, current_time)
    
    # Get action from brain
    return brain.produce_action()


def save_brain_checkpoint(brain: Brain, filepath: str, metadata: Dict[str, Any] = None) -> bool:
    """
    Save brain state to file with optional metadata
    
    Args:
        brain: The brain to save
        filepath: File path to save to
        metadata: Optional metadata to include in the save file
    
    Returns:
        True if successful, False otherwise
    """
    try:
        # Add timestamp and metadata to the brain state
        save_data = {
            'timestamp': time.time(),
            'version': __version__,
            'brain_statistics': {
                'total_neurons': brain.get_total_neuron_count(),
                'total_synapses': brain.get_total_synapse_count(),
                'active_neurons': brain.get_active_neuron_count(),
                'firing_neurons': brain.get_firing_neuron_count(),
                'total_spikes': brain.get_total_spike_count(),
                'e_i_ratio': brain.get_excitation_inhibition_ratio(),
                'developmental_stage': brain.get_developmental_stage().name,
            }
        }
        
        if metadata:
            save_data['metadata'] = metadata
        
        # Save the data
        with open(filepath, 'w') as f:
            json.dump(save_data, f, indent=2)
        
        # Also call the brain's save method if available
        brain.save(filepath)
        
        return True
    except Exception as e:
        print(f"Error saving brain checkpoint: {e}")
        return False


def load_brain_checkpoint(brain: Brain, filepath: str) -> bool:
    """
    Load brain state from file
    
    Args:
        brain: The brain object to load into
        filepath: File path to load from
    
    Returns:
        True if successful, False otherwise
    """
    try:
        # Load the data
        with open(filepath, 'r') as f:
            save_data = json.load(f)
        
        # Load the brain state (this would need to be implemented with proper C++ bindings)
        brain.load(filepath)
        
        return True
    except Exception as e:
        print(f"Error loading brain checkpoint: {e}")
        return False


def get_brain_statistics(brain: Brain) -> Dict[str, Any]:
    """
    Get comprehensive statistics about the brain
    
    Args:
        brain: The brain to get statistics from
    
    Returns:
        Dictionary containing brain statistics
    """
    return {
        'neuron_count': {
            'total': brain.get_total_neuron_count(),
            'active': brain.get_active_neuron_count(),
            'firing': brain.get_firing_neuron_count(),
            'ratio_active_total': brain.get_active_neuron_count() / max(1, brain.get_total_neuron_count()),
            'ratio_firing_active': brain.get_firing_neuron_count() / max(1, brain.get_active_neuron_count()),
        },
        'synapse_count': {
            'total': brain.get_total_synapse_count(),
        },
        'activity': {
            'total_spikes': brain.get_total_spike_count(),
            'average_firing_rate': brain.get_average_firing_rate(),
            'excitation_inhibition_ratio': brain.get_excitation_inhibition_ratio(),
        },
        'development': {
            'stage': brain.get_developmental_stage().name,
            'stage_index': brain.get_developmental_stage().value,
        },
        'region_info': {
            'count': brain.get_region_count(),
            'ids': [region_id.index() for region_id in brain.get_region_ids()],
        }
    }


def reset_all_systems(brain: Brain, world) -> None:
    """
    Reset all simulation systems to initial state
    
    Args:
        brain: The brain to reset
        world: The world to reset
    """
    brain.reset()
    world.reset()


class SimulationLoop:
    """
    Convenience class for running simulation loops
    
    Provides a high-level interface for running simulation steps,
    tracking performance, and managing episodes.
    """
    
    def __init__(self, brain: Brain, world, agent_brain: Optional[AgentBrain] = None):
        self.brain = brain
        self.world = world
        self.agent_brain = agent_brain
        self.step_count = 0
        self.start_time = time.time()
        self.actions_taken = []
        self.rewards_received = []
    
    def run_step(self, timestep: Optional[SimulationStep] = None) -> Action:
        """
        Run a single simulation step
        
        Args:
            timestep: The simulation step (defaults to step count)
        
        Returns:
            The action produced by the brain
        """
        if timestep is None:
            timestep = SimulationStep(self.step_count)
        
        current_time = Timestamp(time.time())
        
        # Get sensory input
        percept = self.world.get_sensory_percept()
        
        # Process through agent brain if available
        if self.agent_brain:
            self.agent_brain.processSensoryInput(percept)
        
        # Step the brain
        self.brain.step(timestep, current_time)
        
        # Get action
        action = self.brain.produce_action()
        
        # Record action
        self.actions_taken.append(action)
        
        # Update step count
        self.step_count += 1
        
        return action
    
    def run_episode(self, steps: int) -> List[Action]:
        """
        Run an entire episode of simulation
        
        Args:
            steps: Number of steps to run
        
        Returns:
            List of actions taken during the episode
        """
        episode_actions = []
        
        for _ in range(steps):
            action = self.run_step()
            episode_actions.append(action)
        
        return episode_actions
    
    def get_performance_stats(self) -> Dict[str, Any]:
        """
        Get performance statistics for the simulation
        
        Returns:
            Dictionary containing performance statistics
        """
        elapsed = time.time() - self.start_time
        
        return {
            'steps_per_second': self.step_count / max(0.001, elapsed),
            'total_steps': self.step_count,
            'total_time': elapsed,
            'actions_taken': len(self.actions_taken),
            'unique_actions': len(set(str(action) for action in self.actions_taken)),
        }
    
    def get_brain_stats(self) -> Dict[str, Any]:
        """Get brain statistics"""
        return get_brain_statistics(self.brain)


class ActionSequence:
    """
    Utility class for managing sequences of actions
    
    Provides functionality to replay actions, generate sequences,
    and analyze action patterns.
    """
    
    def __init__(self):
        self.actions = []
    
    def add_action(self, action: Action) -> None:
        """Add an action to the sequence"""
        self.actions.append(action)
    
    def replay(self, brain: Brain) -> List[Action]:
        """
        Replay the action sequence
        
        Note: This is a conceptual implementation. In practice,
        action sequences would need to be converted back to neural patterns.
        """
        results = []
        for action in self.actions:
            # This would involve injecting specific neural patterns
            # that correspond to the desired action
            results.append(action)
        return results
    
    def get_action_types(self) -> List[ActionType]:
        """Get all action types in the sequence"""
        return [action.getType() for action in self.actions]
    
    def get_action_names(self) -> List[str]:
        """Get all action names in the sequence"""
        return [action.getName() for action in self.actions]
    
    def get_parameter_summary(self) -> List[List[float]]:
        """Get parameter summary for all actions"""
        return [action.getParameters() for action in self.actions]
    
    def analyze_patterns(self) -> Dict[str, Any]:
        """
        Analyze patterns in the action sequence
        
        Returns:
            Dictionary containing pattern analysis
        """
        if not self.actions:
            return {}
        
        action_types = self.get_action_types()
        action_names = self.get_action_names()
        
        # Count action types
        type_counts = {}
        for action_type in action_types:
            type_counts[action_type.name] = type_counts.get(action_type.name, 0) + 1
        
        # Find most common action
        most_common = max(type_counts.items(), key=lambda x: x[1]) if type_counts else None
        
        return {
            'total_actions': len(self.actions),
            'unique_actions': len(set(action_names)),
            'action_type_counts': type_counts,
            'most_common_action': most_common[0] if most_common else None,
            'most_common_count': most_common[1] if most_common else 0,
            'action_variance': len(set(action_types)),
        }


def parse_action_string(action_str: str) -> Action:
    """
    Parse an action from a human-readable string
    
    Examples:
        "move forward"
        "turn left"
        "look up"
        "interact"
    
    Args:
        action_str: Human-readable action string
    
    Returns:
        Action object
    """
    return parse_action_from_string(action_str)


def format_action(action: Action) -> str:
    """
    Format an action as a human-readable string
    
    Args:
        action: The action to format
    
    Returns:
        Human-readable action string
    """
    params_str = f" with parameters {action.getParameters()}" if action.getParameters() else ""
    return f"{action.getName()}{params_str}"


def wait_for_input(prompt: str = "Press Enter to continue...") -> None:
    """Wait for user input (useful for interactive scripts)"""
    input(prompt)


def format_duration(seconds: float) -> str:
    """Format duration in seconds to human-readable string"""
    if seconds < 1:
        return f"{seconds:.3f}s"
    elif seconds < 60:
        return f"{seconds:.1f}s"
    elif seconds < 3600:
        minutes = seconds / 60
        return f"{minutes:.1f}m"
    else:
        hours = seconds / 3600
        return f"{hours:.1f}h"


def benchmark_function(func: Callable, *args, **kwargs) -> Dict[str, Any]:
    """
    Benchmark a function and return performance metrics
    
    Args:
        func: Function to benchmark
        *args: Arguments to pass to function
        **kwargs: Keyword arguments to pass to function
    
    Returns:
        Dictionary containing benchmark results
    """
    start_time = time.time()
    result = func(*args, **kwargs)
    end_time = time.time()
    
    duration = end_time - start_time
    
    return {
        'result': result,
        'duration': duration,
        'duration_formatted': format_duration(duration),
        'success': True
    }


# Default error classes
class NLMError(Exception):
    """Base exception for NLM-related errors"""
    pass

class ConfigurationError(NLMError):
    """Error in configuration"""
    pass

class BrainError(NLMError):
    """Error in brain simulation"""
    pass

class WorldError(NLMError):
    """Error in world simulation"""
    pass

# Version information
__version__ = "0.1.0"

# Make commonly used types available at package level
__all__ = [
    # Core classes
    'Brain', 'Config', 'AgentBrain', 'SimpleWorld',
    # Sensory classes
    'Vision', 'Audio', 'InternalSignals', 'SensoryPercept',
    # Motor classes
    'Action', 'ActionResult',
    # Agent classes
    'AgentBody',
    # Type definitions
    'NeuronId', 'SynapseId', 'RegionId', 'PopulationId',
    'NeuronType', 'SynapseType', 'DevelopmentalStage',
    'FiringState', 'ActionType', 'MotorCommand', 'WorldObjectType',
    'SimulationStep', 'Timestamp', 'MembranePotential', 'Delay',
    'PlasticityFlags', 'Reward',
    # Helper functions
    'create_default_config', 'create_brain', 'create_simple_world',
    'create_agent_brain', 'run_simulation_step',
    'save_brain_checkpoint', 'load_brain_checkpoint',
    'get_brain_statistics', 'reset_all_systems',
    # Classes
    'SimulationLoop', 'ActionSequence',
    # Parsing and formatting
    'parse_action_string', 'format_action',
    # Utilities
    'wait_for_input', 'format_duration', 'benchmark_function',
    # Errors
    'NLMError', 'ConfigurationError', 'BrainError', 'WorldError',
    # Constants
    'INVALID_NEURON_ID', 'INVALID_SYNAPSE_ID', 'INVALID_REGION_ID',
    'INVALID_POPULATION_ID', 'DEFAULT_WORLD_WIDTH', 'DEFAULT_WORLD_HEIGHT',
    'DEFAULT_VISION_WIDTH', 'DEFAULT_VISION_HEIGHT',
    'DEFAULT_MAX_ENERGY', 'DEFAULT_ENERGY_DECAY_RATE', 'DEFAULT_RANDOM_SEED',
    # Version
    '__version__'
]
