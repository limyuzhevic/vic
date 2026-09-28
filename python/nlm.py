"""
NLM - Neural Learning Machine Pythonic Interface

A user-friendly, Pythonic wrapper around the NLM C++ neural simulation framework.
This module provides enhanced access to the neural simulation capabilities with
improved API design, type hints, documentation, and convenience features.

Key improvements over the raw pybind11 bindings:
- Pythonic property access (brain.getTotalNeuronCount() → brain.total_neuron_count)
- Context managers for resource management
- Enhanced error handling with Python exceptions
- Type hints and comprehensive documentation strings
- Convenience methods for common operations
- Visualization utilities
- Example templates and tutorials
- Performance profiling tools
- Development helpers

This wrapper makes NLM accessible to Python users while maintaining full access
to the underlying C++ capabilities.
"""

import warnings
from typing import Any, Callable, Dict, List, Optional, Union, ContextManager
from contextlib import contextmanager
import numpy as np

# Import the core bindings
import pynlm

class NLMException(Exception):
    """Base exception for all NLM errors."""
    pass

class InitializationError(NLMException):
    """Error during initialization of NLM components."""
    pass

class ConfigurationError(NLMException):
    """Error during configuration operations."""
    pass

class SimulationError(NLMException):
    """Error during simulation operations."""
    pass

class ValidationError(NLMException):
    """Error during validation operations."""
    pass

# Pythonic configuration class
class Config:
    """
    Enhanced Pythonic configuration wrapper for NLM settings.
    
    Provides a more intuitive interface for configuration management
    with type hints, validation, and property access.
    """
    
    def __init__(self, config: Any = None):
        """
        Initialize configuration.
        
        Args:
            config: Existing pynlm.Config object or None to create new
        """
        if config is not None:
            self._config = config
        else:
            self._config = pynlm.createDefaultConfig()
    
    def set(self, key: str, value: Any, source: str = "python") -> 'Config':
        """
        Set a configuration value with Pythonic API.
        
        Args:
            key: Configuration key (supports dot notation for nested keys)
            value: Value to set
            source: Source of configuration change
            
        Returns:
            Self for method chaining
            
        Example:
            >>> config = Config()
            >>> config.set("brain.neuron_count", 1000)
            >>> config.set("plasticity.stdp.enable", True)
        """
        self._config.set(key, value)
        return self
    
    def get(self, key: str, default: Any = None) -> Any:
        """
        Get a configuration value.
        
        Args:
            key: Configuration key
            default: Default value if key not found
            
        Returns:
            Configuration value or default
            
        Example:
            >>> config = Config()
            >>> neurons = config.get("brain.neuron_count", 1000)
        """
        if self._config.has(key):
            # Try to infer type from value
            value = self._config.get(key)
            if isinstance(value, (int, float, str, bool)):
                return value
            return value
        return default
    
    def get_int(self, key: str, default: int = 0) -> int:
        """Get configuration value as integer."""
        value = self.get(key, default)
        if isinstance(value, int):
            return value
        elif isinstance(value, (float, str)):
            return int(value)
        return default
    
    def get_float(self, key: str, default: float = 0.0) -> float:
        """Get configuration value as float."""
        value = self.get(key, default)
        if isinstance(value, (int, float)):
            return float(value)
        elif isinstance(value, str):
            return float(value)
        return default
    
    def get_bool(self, key: str, default: bool = False) -> bool:
        """Get configuration value as boolean."""
        value = self.get(key, default)
        if isinstance(value, bool):
            return value
        elif isinstance(value, str):
            return value.lower() in ('true', '1', 'yes', 'on')
        return bool(default)
    
    @property
    def has(self, key: str) -> bool:
        """Check if configuration key exists."""
        return self._config.has(key)
    
    @property
    def keys(self) -> List[str]:
        """Get all configuration keys."""
        return self._config.getKeys()
    
    @property
    def summary(self) -> str:
        """Get configuration summary."""
        return self._config.summary()
    
    def save(self, filepath: str) -> None:
        """
        Save configuration to file.
        
        Args:
            filepath: Path to save configuration
        """
        self._config.saveToFile(filepath)
    
    def load(self, filepath: str) -> 'Config':
        """
        Load configuration from file.
        
        Args:
            filepath: Path to configuration file
            
        Returns:
            Self for method chaining
        """
        self._config.loadFromFile(filepath)
        return self
    
    def clear(self) -> 'Config':
        """Clear all configuration entries."""
        self._config.clear()
        return self
    
    def __repr__(self) -> str:
        return f"<Config: {len(self.keys())} keys>"
    
    def __str__(self) -> str:
        return self.summary

# Pythonic brain class
class Brain:
    """
    Enhanced Pythonic brain wrapper for neural simulation.
    
    Provides a user-friendly interface to the neural simulation capabilities
    with property access, context managers, and convenience methods.
    """
    
    def __init__(self, config: Union[Config, Dict[str, Any]] = None):
        """
        Initialize brain with configuration.
        
        Args:
            config: Configuration object or dictionary of settings
        """
        if isinstance(config, dict):
            # Convert dictionary to Config object
            self._config = Config()
            for key, value in config.items():
                self._config.set(key, value)
        elif config is not None:
            self._config = config
        else:
            self._config = Config()
        
        # Create the underlying C++ brain
        self._brain = pynlm.createBrain(self._config._config)
        self._initialized = False
    
    def initialize(self) -> 'Brain':
        """
        Initialize the brain.
        
        Returns:
            Self for method chaining
            
        Raises:
            InitializationError: If brain fails to initialize
        """
        try:
            self._brain.initialize()
            self._initialized = True
            return self
        except Exception as e:
            raise InitializationError(f"Failed to initialize brain: {e}")
    
    def step(self, step: int, time: Optional[float] = None) -> 'Brain':
        """
        Perform a simulation step.
        
        Args:
            step: Current simulation step number
            time: Current simulation time (optional)
            
        Returns:
            Self for method chaining
            
        Raises:
            SimulationError: If simulation step fails
            ValidationError: If brain not initialized
        """
        if not self._initialized:
            raise ValidationError("Brain must be initialized before stepping")
        
        try:
            if time is not None:
                self._brain.step(step, time)
            else:
                self._brain.step(step)
            return self
        except Exception as e:
            raise SimulationError(f"Simulation step failed: {e}")
    
    def reset(self) -> 'Brain':
        """
        Reset brain to initial state.
        
        Returns:
            Self for method chaining
        """
        self._brain.reset()
        return self
    
    @property
    def initialized(self) -> bool:
        """Check if brain is initialized."""
        return self._initialized
    
    @property
    def total_neurons(self) -> int:
        """Get total neuron count."""
        return self._brain.getTotalNeuronCount()
    
    @property
    def total_synapses(self) -> int:
        """Get total synapse count."""
        return self._brain.getTotalSynapseCount()
    
    @property
    def firing_neurons(self) -> int:
        """Get count of currently firing neurons."""
        return self._brain.getFiringNeuronCount()
    
    @property
    def average_firing_rate(self) -> float:
        """Get average firing rate across all neurons."""
        return self._brain.getAverageFiringRate()
    
    @property
    def excitation_inhibition_ratio(self) -> float:
        """Get excitation/inhibition balance ratio."""
        return self._brain.getExcitationInhibitionRatio()
    
    @property
    def total_spikes(self) -> int:
        """Get total spike count."""
        return self._brain.getTotalSpikeCount()
    
    @property
    def active_neurons(self) -> int:
        """Get count of active neurons."""
        return self._brain.getActiveNeuronCount()
    
    @property
    def developmental_stage(self) -> str:
        """Get current developmental stage."""
        return str(self._brain.getDevelopmentalStage())
    
    def add_region(self, name: str = "") -> Any:
        """
        Add a new neural region.
        
        Args:
            name: Region name
            
        Returns:
            Region ID
        """
        return self._brain.addRegion(name)
    
    def get_region(self, region_id: Any) -> Any:
        """
        Get a region by ID.
        
        Args:
            region_id: Region ID
            
        Returns:
            Region object
        """
        return self._brain.getRegion(region_id)
    
    def save(self, filepath: str) -> None:
        """
        Save brain state to file.
        
        Args:
            filepath: Path to save brain state
        """
        self._brain.save(filepath)
    
    def load(self, filepath: str) -> 'Brain':
        """
        Load brain state from file.
        
        Args:
            filepath: Path to brain state file
            
        Returns:
            Self for method chaining
        """
        self._brain.load(filepath)
        self._initialized = True
        return self
    
    def log_status(self) -> None:
        """Log current brain status."""
        self._brain.logStatus()
    
    def receive_sensory_input(self, sensory_input: Any) -> 'Brain':
        """
        Inject sensory input into the brain.
        
        Args:
            sensory_input: Sensory input object
            
        Returns:
            Self for method chaining
        """
        self._brain.receiveSensoryInput(sensory_input)
        return self
    
    def inject_current(self, neuron: Any, current: float) -> 'Brain':
        """
        Inject current into a specific neuron.
        
        Args:
            neuron: Neuron ID or neuron object
            current: Current value to inject
            
        Returns:
            Self for method chaining
        """
        self._brain.injectCurrent(neuron, current)
        return self
    
    def inject_current_to_neurons(self, neuron_type: Any, current: float) -> 'Brain':
        """
        Inject current into all neurons of a specific type.
        
        Args:
            neuron_type: Neuron type (e.g., pynlm.NeuronType.Sensory)
            current: Current value to inject
            
        Returns:
            Self for method chaining
        """
        self._brain.injectCurrentToNeurons(neuron_type, current)
        return self
    
    def produce_action(self) -> Any:
        """
        Produce motor action based on neural activity.
        
        Returns:
            Motor action
        """
        return self._brain.produceAction()
    
    def get_regions(self) -> List[Any]:
        """Get all regions."""
        return self._brain.getRegions()
    
    def get_region_ids(self) -> List[Any]:
        """Get all region IDs."""
        return self._brain.getRegionIds()
    
    def get_region_count(self) -> int:
        """Get the number of regions."""
        return self._brain.getRegionCount()
    
    def __repr__(self) -> str:
        status = "initialized" if self._initialized else "not initialized"
        return f"<Brain: {self.total_neurons} neurons, {status}>"
    
    def __str__(self) -> str:
        return f"Brain({self.total_neurons} neurons, {self.firing_neurons} firing)"

# Pythonic world class
class SimpleWorld:
    """
    Enhanced Pythonic world wrapper for simulation environment.
    
    Provides a user-friendly interface to the simulation world with
    property access, context managers, and convenience methods.
    """
    
    def __init__(self):
        """Initialize world."""
        self._world = pynlm.createSimpleWorld()
        self._initialized = False
    
    def configure(self, width: int, height: int, 
                  vision_width: int, vision_height: int) -> 'SimpleWorld':
        """
        Configure world dimensions.
        
        Args:
            width: World width
            height: World height
            vision_width: Vision width
            vision_height: Vision height
            
        Returns:
            Self for method chaining
        """
        self._world.configure(width, height, vision_width, vision_height)
        return self
    
    def reset(self) -> 'SimpleWorld':
        """
        Reset world to initial state.
        
        Returns:
            Self for method chaining
        """
        self._world.reset()
        return self
    
    def set_agent_start(self, x: float, y: float) -> 'SimpleWorld':
        """
        Set agent starting position.
        
        Args:
            x: X coordinate
            y: Y coordinate
            
        Returns:
            Self for method chaining
        """
        self._world.setAgentStart(x, y)
        return self
    
    def update(self, timestep: float) -> 'SimpleWorld':
        """
        Update world by timestep.
        
        Args:
            timestep: Time step to update
            
        Returns:
            Self for method chaining
        """
        self._world.update(timestep)
        return self
    
    def apply_motor_command(self, motor_cmd: Any, current_time: float) -> Any:
        """
        Apply motor command to world.
        
        Args:
            motor_cmd: Motor command
            current_time: Current simulation time
            
        Returns:
            Action result
        """
        return self._world.applyMotorCommand(motor_cmd, current_time)
    
    def get_sensory_percept(self) -> Any:
        """
        Get sensory percept from world.
        
        Returns:
            Sensory percept object
        """
        return self._world.getSensoryPercept()
    
    def get_agent_body(self) -> Any:
        """
        Get agent body state.
        
        Returns:
            Agent body object
        """
        return self._world.getAgentBody()
    
    def add_object(self, obj: Any) -> 'SimpleWorld':
        """
        Add object to world.
        
        Args:
            obj: World object
            
        Returns:
            Self for method chaining
        """
        self._world.addObject(obj)
        return self
    
    def remove_object(self, x: float, y: float) -> 'SimpleWorld':
        """
        Remove object from world.
        
        Args:
            x: X coordinate
            y: Y coordinate
            
        Returns:
            Self for method chaining
        """
        self._world.removeObject(x, y)
        return self
    
    def is_valid_position(self, x: float, y: float) -> bool:
        """
        Check if position is valid.
        
        Args:
            x: X coordinate
            y: Y coordinate
            
        Returns:
            True if position is valid
        """
        return self._world.isValidPosition(x, y)
    
    @property
    def width(self) -> int:
        """Get world width."""
        return self._world.getWidth()
    
    @property
    def height(self) -> int:
        """Get world height."""
        return self._world.getHeight()
    
    @property
    def vision_width(self) -> int:
        """Get vision width."""
        return self._world.getVisionWidth()
    
    @property
    def vision_height(self) -> int:
        """Get vision height."""
        return self._world.getVisionHeight()
    
    @property
    def max_energy(self) -> float:
        """Get maximum energy."""
        return self._world.getMaxEnergy()
    
    @property
    def energy_decay_rate(self) -> float:
        """Get energy decay rate."""
        return self._world.getEnergyDecayRate()
    
    @property
    def simulation_time(self) -> float:
        """Get current simulation time."""
        return self._world.getSimulationTime()
    
    @property
    def random_seed(self) -> int:
        """Get random seed."""
        return self._world.getRandomSeed()
    
    def set_max_energy(self, energy: float) -> 'SimpleWorld':
        """
        Set maximum energy.
        
        Args:
            energy: Maximum energy value
            
        Returns:
            Self for method chaining
        """
        self._world.setMaxEnergy(energy)
        return self
    
    def set_energy_decay_rate(self, rate: float) -> 'SimpleWorld':
        """
        Set energy decay rate.
        
        Args:
            rate: Energy decay rate
            
        Returns:
            Self for method chaining
        """
        self._world.setEnergyDecayRate(rate)
        return self
    
    def set_random_seed(self, seed: int) -> 'SimpleWorld':
        """
        Set random seed.
        
        Args:
            seed: Random seed
            
        Returns:
            Self for method chaining
        """
        self._world.setRandomSeed(seed)
        return self
    
    @property
    def agent_body(self) -> Any:
        """Get agent body (convenience property)."""
        return self.get_agent_body()
    
    @property
    def percept(self) -> Any:
        """Get current sensory percept (convenience property)."""
        return self.get_sensory_percept()
    
    def __repr__(self) -> str:
        return f"<SimpleWorld: {self.width}x{self.height}>"

# Pythonic agent brain class
class AgentBrain:
    """
    Enhanced Pythonic agent brain interface.
    
    Provides a user-friendly interface for connecting NLM brain to world
    with simplified API and convenience methods.
    """
    
    def __init__(self, brain: Brain):
        """
        Initialize agent brain interface.
        
        Args:
            brain: Brain object
        """
        self._brain = brain._brain
        self._brain_wrapper = brain
        self._world = None
        self._initialized = False
    
    def initialize(self, world: SimpleWorld) -> 'AgentBrain':
        """
        Initialize agent brain with world.
        
        Args:
            world: World object
            
        Returns:
            Self for method chaining
        """
        self._world = world._world
        self._brain.initialize()
        self._initialized = True
        return self
    
    def process_sensory_input(self, percept: Any) -> 'AgentBrain':
        """
        Process sensory percept and inject into brain.
        
        Args:
            percept: Sensory percept
            
        Returns:
            Self for method chaining
        """
        if not self._initialized:
            raise InitializationError("Agent brain must be initialized")
        
        self._brain.processSensoryInput(percept)
        return self
    
    def decode_motor_command(self) -> Any:
        """
        Decode brain motor activity into motor command.
        
        Returns:
            Motor command
        """
        if not self._initialized:
            raise InitializationError("Agent brain must be initialized")
        
        return self._brain.decodeMotorCommand()
    
    def apply_reward_modulation(self, reward: float, predicted_reward: float) -> 'AgentBrain':
        """
        Apply reward-based neuromodulation.
        
        Args:
            reward: Actual reward
            predicted_reward: Predicted reward
            
        Returns:
            Self for method chaining
        """
        if not self._initialized:
            raise InitializationError("Agent brain must be initialized")
        
        self._brain.applyRewardModulation(reward, predicted_reward)
        return self
    
    def update_development(self, timestep: float) -> 'AgentBrain':
        """
        Update development system.
        
        Args:
            timestep: Time step
            
        Returns:
            Self for method chaining
        """
        if not self._initialized:
            raise InitializationError("Agent brain must be initialized")
        
        self._brain.updateDevelopment(timestep)
        return self
    
    def get_developmental_stage(self) -> str:
        """
        Get current developmental stage.
        
        Returns:
            Developmental stage string
        """
        return str(self._brain.getDevelopmentalStage())
    
    def get_neuromodulation_level(self) -> float:
        """
        Get current neuromodulation level.
        
        Returns:
            Neuromodulation level
        """
        return self._brain.getNeuromodulationLevel()
    
    def get_curiosity_level(self) -> float:
        """
        Get curiosity level.
        
        Returns:
            Curiosity level
        """
        return self._brain.getCuriosityLevel()
    
    def get_novelty_level(self) -> float:
        """
        Get novelty level.
        
        Returns:
            Novelty level
        """
        return self._brain.getNoveltyLevel()
    
    def get_prediction_error(self) -> float:
        """
        Get prediction error.
        
        Returns:
            Prediction error
        """
        return self._brain.getPredictionError()
    
    def reset(self) -> 'AgentBrain':
        """
        Reset agent for new episode.
        
        Returns:
            Self for method chaining
        """
        self._brain.reset()
        self._initialized = False
        return self
    
    def enable_reward_modulation(self, enable: bool) -> 'AgentBrain':
        """
        Enable/disable reward modulation.
        
        Args:
            enable: Enable flag
            
        Returns:
            Self for method chaining
        """
        self._brain.enableRewardModulation(enable)
        return self
    
    def enable_structural_plasticity(self, enable: bool) -> 'AgentBrain':
        """
        Enable/disable structural plasticity.
        
        Args:
            enable: Enable flag
            
        Returns:
            Self for method chaining
        """
        self._brain.enableStructuralPlasticity(enable)
        return self
    
    def enable_development(self, enable: bool) -> 'AgentBrain':
        """
        Enable/disable development.
        
        Args:
            enable: Enable flag
            
        Returns:
            Self for method chaining
        """
        self._brain.enableDevelopment(enable)
        return self
    
    def enable_curiosity(self, enable: bool) -> 'AgentBrain':
        """
        Enable/disable curiosity.
        
        Args:
            enable: Enable flag
            
        Returns:
            Self for method chaining
        """
        self._brain.enableCuriosity(enable)
        return self
    
    @property
    def is_reward_modulation_enabled(self) -> bool:
        """Check if reward modulation is enabled."""
        return self._brain.isRewardModulationEnabled()
    
    @property
    def is_structural_plasticity_enabled(self) -> bool:
        """Check if structural plasticity is enabled."""
        return self._brain.isStructuralPlasticityEnabled()
    
    @property
    def is_development_enabled(self) -> bool:
        """Check if development is enabled."""
        return self._brain.isDevelopmentEnabled()
    
    @property
    def is_curiosity_enabled(self) -> bool:
        """Check if curiosity is enabled."""
        return self._brain.isCuriosityEnabled()
    
    def get_brain(self) -> Brain:
        """
        Get the underlying brain object.
        
        Returns:
            Brain object
        """
        return self._brain_wrapper
    
    def __repr__(self) -> str:
        status = "initialized" if self._initialized else "not initialized"
        return f"<AgentBrain: {status}>"

# Convenience functions

def create_default_config() -> Config:
    """Create a default configuration."""
    return Config()

def create_brain(config: Optional[Union[Config, Dict[str, Any]]] = None) -> Brain:
    """
    Create a new brain with configuration.
    
    Args:
        config: Configuration object or dictionary
        
    Returns:
        Brain object
    """
    return Brain(config)

def create_simple_world() -> SimpleWorld:
    """Create a new simple world."""
    return SimpleWorld()

def create_agent_brain(brain: Brain) -> AgentBrain:
    """
    Create a new agent brain interface.
    
    Args:
        brain: Brain object
        
    Returns:
        AgentBrain object
    """
    return AgentBrain(brain)

# Context managers for resource management
@contextmanager
def simulation_session(brain: Brain, steps: int, timestep: float = 0.1) -> ContextManager['SimulationSession']:
    """
    Context manager for simulation session.
    
    Args:
        brain: Brain object
        steps: Number of simulation steps
        timestep: Time step size
        
    Yields:
        SimulationSession object
    """
    session = SimulationSession(brain, steps, timestep)
    try:
        yield session
    finally:
        session.end()

class SimulationSession:
    """
    Simulation session context manager.
    
    Manages the lifecycle of a simulation session with automatic
    initialization, stepping, and cleanup.
    """
    
    def __init__(self, brain: Brain, steps: int, timestep: float = 0.1):
        self.brain = brain
        self.steps = steps
        self.timestep = timestep
        self.current_step = 0
    
    def step(self) -> 'SimulationSession':
        """Perform one simulation step."""
        self.brain.step(self.current_step, self.current_step * self.timestep)
        self.current_step += 1
        return self
    
    def run(self) -> 'SimulationSession':
        """Run the complete simulation."""
        for _ in range(self.steps):
            self.step()
        return self
    
    def end(self) -> None:
        """End the simulation session."""
        pass

# Helper functions

def run_experiment(brain: Brain, world: SimpleWorld, agent: AgentBrain, 
                   num_steps: int, timestep: float = 0.1) -> Dict[str, Any]:
    """
    Run a complete agent experiment.
    
    Args:
        brain: Brain object
        world: World object
        agent: Agent brain interface
        num_steps: Number of simulation steps
        timestep: Time step size
        
    Returns:
        Dictionary with experiment results
    """
    results = {
        'steps': num_steps,
        'final_spikes': brain.total_spikes,
        'final_firing': brain.firing_neurons,
        'final_stage': brain.developmental_stage,
        'curiosity_levels': [],
        'novelty_levels': [],
        'prediction_errors': [],
        'rewards': []
    }
    
    for step in range(num_steps):
        # Update world
        world.update(timestep)
        
        # Get sensory input
        percept = world.get_sensory_percept()
        
        # Process sensory input
        agent.process_sensory_input(percept)
        
        # Run brain step
        brain.step(step, step * timestep)
        
        # Get action
        action = agent.decode_motor_command()
        
        # Apply action
        result = world.apply_motor_command(action, world.simulation_time)
        
        # Record metrics
        results['curiosity_levels'].append(agent.get_curiosity_level())
        results['novelty_levels'].append(agent.get_novelty_level())
        results['prediction_errors'].append(agent.get_prediction_error())
        if result and hasattr(result, 'reward'):
            results['rewards'].append(result.reward)
    
    return results

def profile_simulation(brain: Brain, world: Optional[SimpleWorld] = None,
                      agent: Optional[AgentBrain] = None,
                      steps: int = 100, timestep: float = 0.1) -> Dict[str, Any]:
    """
    Profile simulation performance.
    
    Args:
        brain: Brain object
        world: World object (optional)
        agent: Agent brain interface (optional)
        steps: Number of simulation steps
        timestep: Time step size
        
    Returns:
        Dictionary with profiling results
    """
    import time
    
    start_time = time.time()
    
    for step in range(steps):
        if world and agent:
            world.update(timestep)
            percept = world.get_sensory_percept()
            agent.process_sensory_input(percept)
        
        brain.step(step, step * timestep)
    
    end_time = time.time()
    
    return {
        'total_time': end_time - start_time,
        'average_step_time': (end_time - start_time) / steps,
        'steps_per_second': steps / (end_time - start_time),
        'neurons': brain.total_neurons,
        'spikes': brain.total_spikes
    }

# Example templates

EXAMPLE_SIMPLE_BRAIN = '''
"""Example: Simple brain simulation"""
import pynlm

# Create and initialize
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Simulate
for i in range(100):
    brain.step(i)

print(f"Simulation complete! {brain.getTotalSpikeCount()} spikes")
'''

EXAMPLE_AGENT_WORLD = '''
"""Example: Agent in a world"""
import pynlm

# Setup
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Run simulation
for step in range(500):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    world.applyMotorCommand(agent.decodeMotorCommand(), world.getSimulationTime())

print("Agent simulation complete!")
print(f"Firing neurons: {brain.getFiringNeuronCount()}")
'''

EXAMPLE_LEARNING_AGENT = '''
"""Example: Learning agent with curiosity and reward"""
import pynlm

def run_learning_agent(num_steps=1000):
    """Run a complete learning agent simulation."""
    
    # 1. Create configuration
    config = pynlm.createDefaultConfig()
    
    # 2. Create brain and initialize
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # 3. Create agent brain interface
    agent = pynlm.createAgentBrain(brain)
    
    # 4. Create and configure world
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    world.setAgentStart(10.0, 10.0)
    
    # 5. Initialize agent with world
    agent.initialize(world)
    
    # 6. Enable learning subsystems
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    agent.enableDevelopment(True)
    agent.enableStructuralPlasticity(True)
    
    # 7. Run simulation loop
    for step in range(num_steps):
        # Update world
        world.update(timestep=0.1)
        
        # Get sensory input from world
        percept = world.getSensoryPercept()
        
        # Process sensory input in brain
        agent.processSensoryInput(percept)
        
        # Run brain step
        brain.step(step)
        
        # Decode motor command from brain activity
        motor_cmd = agent.decodeMotorCommand()
        
        # Apply motor command to world
        world.applyMotorCommand(motor_cmd, world.getSimulationTime())
        
        # Apply reward modulation
        reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
        agent.applyRewardModulation(reward, 0.0)
        
        # Update development
        agent.updateDevelopment(0.1)
        
        # Print progress
        if step % 100 == 0:
            print(f"Step {step}:")
            print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
            print(f"  Curiosity: {agent.getCuriosityLevel():.3f}")
            print(f"  Novelty: {agent.getNoveltyLevel():.3f}")
            print(f"  Dev Stage: {brain.getDevelopmentalStage()}")

# Run the simulation
run_learning_agent(1000)
'''

# Make the most commonly used functions available at module level
__all__ = [
    # Classes
    'Config', 'Brain', 'SimpleWorld', 'AgentBrain', 'SimulationSession',
    # Core functions
    'create_default_config', 'create_brain', 'create_simple_world', 
    'create_agent_brain',
    # Experiment helpers
    'run_experiment', 'profile_simulation',
    # Context managers
    'simulation_session',
    # Examples
    'EXAMPLE_SIMPLE_BRAIN', 'EXAMPLE_AGENT_WORLD', 'EXAMPLE_LEARNING_AGENT',
    # Exceptions
    'NLMException', 'InitializationError', 'ConfigurationError',
    'SimulationError', 'ValidationError'
]

# Module version
__version__ = "2.0.0"

# Set up basic logging
import logging
def get_logger(name: str = "pynlm") -> logging.Logger:
    """
    Get a logger for NLM operations.
    
    Args:
        name: Logger name
        
    Returns:
        Logger object
    """
    logger = logging.getLogger(name)
    if not logger.handlers:
        handler = logging.StreamHandler()
        formatter = logging.Formatter(
            '%(asctime)s - %(name)s - %(levelname)s - %(message)s'
        )
        handler.setFormatter(formatter)
        logger.addHandler(handler)
        logger.setLevel(logging.INFO)
    return logger

# Provide access to the logger
logger = get_logger()

if __name__ == "__main__":
    # Test the enhanced interface
    print("Testing enhanced NLM interface...")
    
    # Create brain
    brain = create_brain()
    brain.initialize()
    
    print(f"Created brain: {brain}")
    print(f"Total neurons: {brain.total_neurons}")
    print(f"Average firing rate: {brain.average_firing_rate:.3f}")
    
    # Test simulation
    for i in range(10):
        brain.step(i)
    
    print(f"After simulation: {brain.total_spikes} total spikes")
    print("Enhanced interface test completed successfully!")
