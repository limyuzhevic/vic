"""
NLM (Neural Learning Machine) - Python Interface
=================================================

An experimental artificial developmental brain system with neural simulation
capabilities, learning, memory, and cognition.

This module provides a user-friendly Python interface to the NLM C++ neural
simulation framework, making it accessible for experimentation and research.
"""

import sys
import numpy as np
from typing import List, Dict, Optional, Union, Any
import time

# Import the underlying C++ extension
try:
    from pynlm import (
        Config,
        Brain,
        SimpleWorld,
        AgentBrain,
        SensoryPercept,
        AgentBody,
        ActionResult,
        Vision,
        Audio,
        InternalSignals,
        Action,
        WorldObject,
        WorldObjectType,
        MotorCommand,
        NeuronType,
        DevelopmentalStage,
        ActionType,
        INVALID_NEURON_ID,
        INVALID_SYNAPSE_ID,
        INVALID_REGION_ID,
        INVALID_POPULATION_ID,
        createDefaultConfig,
        createBrain,
        createSimpleWorld,
        createAgentBrain
    )
    _CPP_MODULE_AVAILABLE = True
except ImportError:
    _CPP_MODULE_AVAILABLE = False
    # Create dummy classes for testing/documentation purposes
    Config = SimpleWorld = AgentBrain = object
    def createDefaultConfig(): return None
    def createBrain(config): return None
    def createSimpleWorld(): return None
    def createAgentBrain(brain): return None

# Global module metadata
__version__ = "0.1.0"
__author__ = "NLM Project"
__license__ = "MIT"

# Public API exports
__all__ = [
    'Config',
    'Brain',
    'SimpleWorld',
    'AgentBrain',
    'SensoryPercept',
    'AgentBody',
    'ActionResult',
    'Vision',
    'Audio',
    'InternalSignals',
    'Action',
    'WorldObject',
    'WorldObjectType',
    'MotorCommand',
    'NeuronType',
    'DevelopmentalStage',
    'ActionType',
    'INVALID_NEURON_ID',
    'INVALID_SYNAPSE_ID',
    'INVALID_REGION_ID',
    'INVALID_POPULATION_ID',
    'createDefaultConfig',
    'createBrain',
    'createSimpleWorld',
    'createAgentBrain',
    'createDefaultConfig',
    'DEFAULT_CONFIG',
    'BrainBuilder',
    'AgentBuilder',
    'SimulationRunner',
    'WorldBuilder',
    'run_example_simulation',
    'demo_neural_network'
]


# Configuration utilities
DEFAULT_CONFIG = {
    "brain": {
        "neuron_count": 1000,
        "synapse_density": 0.1,
        "connection_probability": 0.05,
        "initial_weight_mean": 0.5,
        "initial_weight_std": 0.1,
        "v_thresh": -50.0,
        "v_rest": -70.0,
        "v_reset": -75.0,
        "tau_mem": 20.0,
        "tau_ref": 2.0
    },
    "plasticity": {
        "stdp": {
            "enable": True,
            "learning_rate": 0.001,
            "tau_plus": 20.0,
            "tau_minus": 20.0
        },
        "hebbian": {
            "enable": True
        },
        "structural": {
            "enable": True
        }
    },
    "neuromod": {
        "dopamine": {"scale": 1.0},
        "curiosity": {"enable": True},
        "novelty": {"enable": True}
    }
}


def create_default_config() -> Config:
    """
    Create a default configuration for a new brain.
    
    Returns:
        Config: A default configuration object
        
    Example:
        >>> config = create_default_config()
        >>> brain = create_brain(config)
    """
    return createDefaultConfig()


def create_brain(config: Optional[Config] = None) -> Brain:
    """
    Create a new brain instance with the given configuration.
    
    Args:
        config: Optional configuration. If None, a default config is used.
        
    Returns:
        Brain: A new brain instance
        
    Example:
        >>> config = create_default_config()
        >>> brain = create_brain(config)
    """
    if config is None:
        config = create_default_config()
    return createBrain(config)


def create_simple_world() -> SimpleWorld:
    """
    Create a simple 2D world for NLM simulation.
    
    Returns:
        SimpleWorld: A new world instance
        
    Example:
        >>> world = create_simple_world()
    """
    return createSimpleWorld()


def create_agent_brain(brain: Brain) -> AgentBrain:
    """
    Create an agent brain interface that connects a brain to a world.
    
    Args:
        brain: The brain instance to connect
        
    Returns:
        AgentBrain: An agent interface for the brain
        
    Example:
        >>> brain = create_brain()
        >>> agent = create_agent_brain(brain)
    """
    return createAgentBrain(brain)


def get_version() -> str:
    """
    Get the version of the NLM Python module.
    
    Returns:
        str: Version string
    """
    return __version__


def check_cpp_module() -> bool:
    """
    Check if the underlying C++ module is available.
    
    Returns:
        bool: True if C++ module is available
    """
    return _CPP_MODULE_AVAILABLE


class ConfigBuilder:
    """
    Builder class for creating configuration objects with a fluent interface.
    
    Provides a convenient way to configure brains with sensible defaults
    and easy validation.
    """
    
    def __init__(self):
        self.config = create_default_config()
    
    def set_neuron_count(self, count: int) -> 'ConfigBuilder':
        """Set the number of neurons in the brain."""
        self.config.set("brain.neuron_count", count)
        return self
    
    def set_synapse_density(self, density: float) -> 'ConfigBuilder':
        """Set the synapse density (connection probability)."""
        self.config.set("brain.synapse_density", density)
        return self
    
    def set_connection_probability(self, probability: float) -> 'ConfigBuilder':
        """Set the probability of connections between neurons."""
        self.config.set("brain.connection_probability", probability)
        return self
    
    def enable_stdp(self, enable: bool = True, learning_rate: float = 0.001, 
                   tau_plus: float = 20.0, tau_minus: float = 20.0) -> 'ConfigBuilder':
        """Enable/disable STDP plasticity and set parameters."""
        self.config.set("plasticity.stdp.enable", enable)
        if enable:
            self.config.set("plasticity.stdp.learning_rate", learning_rate)
            self.config.set("plasticity.stdp.tau_plus", tau_plus)
            self.config.set("plasticity.stdp.tau_minus", tau_minus)
        return self
    
    def enable_hebbian(self, enable: bool = True) -> 'ConfigBuilder':
        """Enable/disable Hebbian plasticity."""
        self.config.set("plasticity.hebbian.enable", enable)
        return self
    
    def enable_structural_plasticity(self, enable: bool = True) -> 'ConfigBuilder':
        """Enable/disable structural plasticity."""
        self.config.set("plasticity.structural.enable", enable)
        return self
    
    def enable_curiosity(self, enable: bool = True) -> 'ConfigBuilder':
        """Enable/disable curiosity-driven exploration."""
        self.config.set("neuromod.curiosity.enable", enable)
        return self
    
    def enable_novelty_detection(self, enable: bool = True) -> 'ConfigBuilder':
        """Enable/disable novelty detection."""
        self.config.set("neuromod.novelty.enable", enable)
        return self
    
    def set_random_seed(self, seed: int) -> 'ConfigBuilder':
        """Set the random seed for reproducible simulations."""
        self.config.set("random_seed", seed)
        return self
    
    def build(self) -> Config:
        """Build the configuration object."""
        return self.config


class BrainBuilder:
    """
    Builder class for creating and configuring brain instances.
    
    Provides a fluent interface for creating brains with specific properties
    and initializations.
    """
    
    def __init__(self, config: Optional[Config] = None):
        self.config = config or create_default_config()
        self.brain = None
        self.initialized = False
    
    def with_config(self, config: Config) -> 'BrainBuilder':
        """Set the configuration for the brain."""
        self.config = config
        return self
    
    def with_neuron_count(self, count: int) -> 'BrainBuilder':
        """Set the number of neurons."""
        self.config.set("brain.neuron_count", count)
        return self
    
    def with_plasticity(self, stdp_enabled: bool = True, hebbian_enabled: bool = True,
                        structural_enabled: bool = True) -> 'BrainBuilder':
        """Configure plasticity settings."""
        self.config.set("plasticity.stdp.enable", stdp_enabled)
        self.config.set("plasticity.hebbian.enable", hebbian_enabled)
        self.config.set("plasticity.structural.enable", structural_enabled)
        return self
    
    def build(self) -> 'BrainBuilder':
        """Create the brain instance."""
        self.brain = create_brain(self.config)
        return self
    
    def initialize(self) -> 'BrainBuilder':
        """Initialize the brain."""
        if self.brain is None:
            self.build()
        self.brain.initialize()
        self.initialized = True
        return self
    
    def get(self) -> Brain:
        """Get the brain instance."""
        if self.brain is None:
            self.build()
        if not self.initialized:
            self.initialize()
        return self.brain


class AgentBuilder:
    """
    Builder class for creating agent instances with world integration.
    
    Provides a convenient way to set up complete agent-world systems
    with all necessary components.
    """
    
    def __init__(self, brain: Optional[Brain] = None):
        self.brain = brain or BrainBuilder().build().get()
        self.world = None
        self.agent = None
        self.world_configured = False
    
    def with_brain(self, brain: Brain) -> 'AgentBuilder':
        """Set the brain instance."""
        self.brain = brain
        return self
    
    def with_world(self, world: SimpleWorld) -> 'AgentBuilder':
        """Set the world instance."""
        self.world = world
        return self
    
    def configure_world(self, width: int = 20, height: int = 20,
                       vision_width: int = 8, vision_height: int = 8) -> 'AgentBuilder':
        """Configure the world dimensions and vision."""
        if self.world is None:
            self.world = create_simple_world()
        self.world.configure(width, height, vision_width, vision_height)
        self.world_configured = True
        return self
    
    def with_start_position(self, x: float, y: float) -> 'AgentBuilder':
        """Set the agent's starting position in the world."""
        if self.world is not None:
            self.world.setAgentStart(x, y)
        return self
    
    def build(self) -> 'AgentBuilder':
        """Create the agent instance."""
        if self.brain is None:
            self.with_brain(BrainBuilder().build().get())
        if self.world is not None and self.world_configured:
            self.agent = create_agent_brain(self.brain)
            self.agent.initialize(self.world)
        return self
    
    def enable_learning(self, reward_modulation: bool = True, curiosity: bool = True,
                       structural_plasticity: bool = True, development: bool = True) -> 'AgentBuilder':
        """Enable various learning subsystems."""
        if self.agent is not None:
            self.agent.enableRewardModulation(reward_modulation)
            self.agent.enableCuriosity(curiosity)
            self.agent.enableStructuralPlasticity(structural_plasticity)
            self.agent.enableDevelopment(development)
        return self
    
    def get(self) -> AgentBrain:
        """Get the agent instance."""
        if self.agent is None:
            self.build()
        return self.agent


class SimulationRunner:
    """
    Runner for executing simulation loops with configurable logging and monitoring.
    
    Provides a high-level interface for running complete agent simulations
    with optional statistics collection and logging.
    """
    
    def __init__(self, agent: Optional[AgentBrain] = None, world: Optional[SimpleWorld] = None):
        self.agent = agent
        self.world = world
        self.step_count = 0
        self.stats_history = []
    
    def setup(self, agent: AgentBrain, world: SimpleWorld) -> 'SimulationRunner':
        """Set up the simulation with agent and world."""
        self.agent = agent
        self.world = world
        self.step_count = 0
        self.stats_history = []
        return self
    
    def run_episode(self, num_steps: int, timestep: float = 0.1,
                   log_interval: int = 100) -> Dict[str, Any]:
        """
        Run a complete simulation episode.
        
        Args:
            num_steps: Number of simulation steps to run
            timestep: Time step duration
            log_interval: How often to log statistics
            
        Returns:
            Dictionary with final statistics
        """
        if not self.agent or not self.world:
            raise RuntimeError("Agent and world must be set up before running simulation")
        
        self.step_count = 0
        self.stats_history = []
        
        for step in range(num_steps):
            # Update world
            self.world.update(timestep)
            
            # Get sensory input
            percept = self.world.getSensoryPercept()
            
            # Process sensory input in brain
            self.agent.processSensoryInput(percept)
            
            # Run brain step
            self.agent.getBrain().step(step)
            
            # Decode motor command
            motor_cmd = self.agent.decodeMotorCommand()
            
            # Apply motor command to world
            self.world.applyMotorCommand(motor_cmd, self.world.getSimulationTime())
            
            # Get statistics periodically
            if log_interval > 0 and step % log_interval == 0:
                stats = self._collect_stats(step)
                self.stats_history.append(stats)
        
        return self._collect_stats(num_steps)
    
    def _collect_stats(self, step: int) -> Dict[str, Any]:
        """Collect statistics at a given step."""
        if not self.agent or not self.world:
            return {}
        
        brain = self.agent.getBrain()
        body = self.world.getAgentBody()
        
        return {
            'step': step,
            'firing_neurons': brain.getFiringNeuronCount(),
            'total_spikes': brain.getTotalSpikeCount(),
            'avg_firing_rate': brain.getAverageFiringRate(),
            'curiosity': self.agent.getCuriosityLevel(),
            'novelty': self.agent.getNoveltyLevel(),
            'prediction_error': self.agent.getPredictionError(),
            'neuromodulation': self.agent.getNeuromodulationLevel(),
            'position': (body.x, body.y),
            'orientation': body.orientation,
            'energy': body.energy,
            'health': body.health
        }
    
    def get_statistics_history(self) -> List[Dict[str, Any]]:
        """Get the history of statistics collected during simulation."""
        return self.stats_history.copy()
    
    def get_final_stats(self) -> Dict[str, Any]:
        """Get the final statistics from the last simulation."""
        if not self.stats_history:
            return {}
        return self.stats_history[-1] if self.stats_history else {}


class WorldBuilder:
    """
    Builder class for creating and configuring world instances.
    
    Provides a fluent interface for creating worlds with specific properties
    and initial objects.
    """
    
    def __init__(self):
        self.world = create_simple_world()
    
    def configure(self, width: int = 20, height: int = 20,
                  vision_width: int = 8, vision_height: int = 8) -> 'WorldBuilder':
        """Configure world dimensions and vision."""
        self.world.configure(width, height, vision_width, vision_height)
        return self
    
    def add_resource(self, x: float, y: float, value: float = 1.0) -> 'WorldBuilder':
        """Add a resource object to the world."""
        obj = WorldObject(x, y, WorldObjectType.Resource, value)
        self.world.addObject(obj)
        return self
    
    def add_hazard(self, x: float, y: float, value: float = -1.0) -> 'WorldBuilder':
        """Add a hazard object to the world."""
        obj = WorldObject(x, y, WorldObjectType.Hazard, value)
        self.world.addObject(obj)
        return self
    
    def add_wall(self, x: float, y: float, radius: float = 0.5) -> 'WorldBuilder':
        """Add a wall object to the world."""
        obj = WorldObject(x, y, WorldObjectType.Wall, 0.0, radius)
        self.world.addObject(obj)
        return self
    
    def with_agent_start(self, x: float, y: float) -> 'WorldBuilder':
        """Set the agent's starting position."""
        self.world.setAgentStart(x, y)
        return self
    
    def build(self) -> SimpleWorld:
        """Build the world instance."""
        return self.world


def run_example_simulation() -> Dict[str, Any]:
    """
    Run a complete example simulation with all components.
    
    This is a convenience function that demonstrates the full integration
    of all NLM components.
    
    Returns:
        Dictionary with simulation results and statistics
    """
    # Create agent with builder pattern
    agent_builder = (AgentBuilder()
                    .configure_world(width=15, height=15, vision_width=6, vision_height=6)
                    .with_start_position(7.5, 7.5)
                    .enable_learning(reward_modulation=True, curiosity=True, 
                                    structural_plasticity=True, development=True))
    
    agent = agent_builder.get()
    
    # Run simulation
    runner = SimulationRunner(agent, agent.world if hasattr(agent, 'world') else None)
    stats = runner.run_episode(500, timestep=0.1, log_interval=100)
    
    return {
        'simulation_stats': stats,
        'agent_builder': agent_builder,
        'runner': runner,
        'success': True
    }


def demo_neural_network() -> Dict[str, Any]:
    """
    Run a demonstration of neural network learning capabilities.
    
    This creates a complete agent-world system and runs a learning episode,
    demonstrating the full NLM capabilities.
    
    Returns:
        Dictionary with demonstration results
    """
    # Create brain with specific configuration
    brain = (BrainBuilder()
             .with_neuron_count(500)
             .with_plasticity(stdp_enabled=True, hebbian_enabled=True)
             .build().get())
    
    # Create agent with world
    agent = (AgentBuilder(brain)
             .configure_world(10, 10, 5, 5)
             .with_start_position(5.0, 5.0)
             .enable_learning())
    
    # Run learning simulation
    if hasattr(agent, 'world'):
        runner = SimulationRunner(agent, agent.world)
        results = runner.run_episode(300, timestep=0.2, log_interval=50)
    else:
        # Fallback if world is not accessible
        results = {'steps_completed': 300, 'message': 'World not accessible in demo'}
    
    return {
        'brain': brain,
        'agent': agent,
        'results': results,
        'demo_completed': True
    }


def print_welcome_message():
    """Print a welcome message and basic usage information."""
    print("=" * 60)
    print("NLM (Neural Learning Machine) - Python Interface")
    print("=" * 60)
    print()
    print("Welcome to NLM! You're now ready to use the neural simulation framework.")
    print()
    print("Basic usage:")
    print("  1. Create a brain: brain = create_brain()")
    print("  2. Create a world: world = create_simple_world()")
    print("  3. Create an agent: agent = create_agent_brain(brain)")
    print("  4. Initialize: agent.initialize(world)")
    print("  5. Run simulation: agent.processSensoryInput(world.getSensoryPercept())")
    print()
    print("For more examples, see the documentation and demos.")
    print("=" * 60)


# Initialize module-level variables
if _CPP_MODULE_AVAILABLE:
    try:
        # Test if we can actually use the C++ module
        test_config = create_default_config()
        test_brain = create_brain()
        test_brain.initialize()
        _CPP_MODULE_AVAILABLE = True
        print_welcome_message()
    except Exception as e:
        _CPP_MODULE_AVAILABLE = False
        print(f"Warning: C++ module initialization failed: {e}")
        print("Falling back to stub implementation.")
else:
    print("C++ module not available. Using stub implementation.")
    print("To use the full functionality, install NLM with: pip install -e .")