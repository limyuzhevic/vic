"""
Core NLM classes wrapping C++ bindings.

This module provides Pythonic wrappers for the main NLM C++ classes,
including Brain, World, Agent, and other core components.
"""

import time
from typing import Optional, Dict, Any, List, Union
from pathlib import Path

import pybind11
from pybind11 import kwargs_only

from .exceptions import (
    BrainError, SimulationError, ValidationError, ConfigurationError
)
from .types import (
    NeuronId, SynapseId, RegionId, PopulationId,
    NeuronType, SynapseType, DevelopmentalStage, FiringState
)

# Import C++ bindings (these are created by pybind11)
try:
    from ..bindings import (
        createBrain, createDefaultConfig, createSimpleWorld, createAgentBrain,
        Config, Brain, SimpleWorld, AgentBrain, Vision, Audio, Action,
        SensoryPercept, WorldObject, AgentBody, ActionResult, invalid_constants
    )
except ImportError as e:
    raise ImportError(
        f"Failed to import NLM C++ bindings. Make sure NLM is built. Error: {e}"
    )


class NLMConfig:
    """
    Enhanced configuration manager for NLM (alias for pynlm.config.NLMConfig).
    
    This is provided for backward compatibility. Use pynlm.config.NLMConfig
    for new code.
    """
    
    def __init__(self, *args, **kwargs):
        """Initialize configuration (use pynlm.config.NLMConfig instead)."""
        from .config import NLMConfig as ConfigClass
        self._config = ConfigClass(*args, **kwargs)
    
    def __getattr__(self, name):
        return getattr(self._config, name)
    
    def __repr__(self):
        return repr(self._config)
    
    def __str__(self):
        return str(self._config)


class NMLBrain:
    """
    Python wrapper for NLM Brain with enhanced functionality.
    
    Provides Pythonic interface to the NLM brain simulation with
    additional utilities, validation, and convenience methods.
    """
    
    def __init__(self, config=None):
        """
        Initialize NLM brain.
        
        Args:
            config: NLM configuration (optional)
        """
        if config is None:
            config = NLMConfig()
        
        self._config = config
        self._brain = None
        self._is_initialized = False
        
        # Initialize brain if config provided
        if config is not None:
            self.initialize()
    
    def initialize(self) -> None:
        """
        Initialize the brain with current configuration.
        
        Raises:
            BrainError: If brain cannot be initialized
        """
        try:
            # Validate configuration
            from .config import validate_config
            validate_config(self._config)
            
            # Create brain with validated configuration
            self._brain = createBrain(self._config._config)
            
            if not self._brain.initialize():
                raise BrainError("Brain initialization failed")
            
            self._is_initialized = True
            
        except Exception as e:
            if not isinstance(e, BrainError):
                raise BrainError(f"Brain initialization failed: {e}")
            raise
    
    def is_initialized(self) -> bool:
        """Check if brain is initialized."""
        return self._is_initialized and self._brain is not None
    
    def step(self, step: int = 0) -> None:
        """
        Perform one brain simulation step.
        
        Args:
            step: Step number (for logging)
            
        Raises:
            SimulationError: If brain is not initialized or step fails
        """
        if not self.is_initialized():
            raise SimulationError("Brain must be initialized before stepping")
        
        try:
            self._brain.step(step)
        except Exception as e:
            raise SimulationError(f"Simulation step failed: {e}")
    
    def step_with_time(self, step: int, time: float) -> None:
        """
        Perform one brain simulation step with timestamp.
        
        Args:
            step: Step number
            time: Simulation time
            
        Raises:
            SimulationError: If brain is not initialized or step fails
        """
        if not self.is_initialized():
            raise SimulationError("Brain must be initialized before stepping")
        
        try:
            self._brain.step(step, time)
        except Exception as e:
            raise SimulationError(f"Simulation step with time failed: {e}")
    
    def reset(self) -> None:
        """Reset brain to initial state."""
        if not self.is_initialized():
            return
        
        try:
            self._brain.reset()
        except Exception as e:
            raise BrainError(f"Brain reset failed: {e}")
    
    def save(self, filepath: str) -> bool:
        """
        Save brain state to file.
        
        Args:
            filepath: Path to save brain state
            
        Returns:
            True if successful, False otherwise
            
        Raises:
            BrainError: If save operation fails
        """
        if not self.is_initialized():
            raise BrainError("Brain must be initialized before saving")
        
        try:
            return self._brain.save(filepath)
        except Exception as e:
            raise BrainError(f"Brain save failed: {e}")
    
    def load(self, filepath: str) -> bool:
        """
        Load brain state from file.
        
        Args:
            filepath: Path to load brain state from
            
        Returns:
            True if successful, False otherwise
            
        Raises:
            BrainError: If load operation fails
        """
        if not self.is_initialized():
            raise BrainError("Brain must be initialized before loading")
        
        try:
            return self._brain.load(filepath)
        except Exception as e:
            raise BrainError(f"Brain load failed: {e}")
    
    # Statistics and information methods
    def get_total_neuron_count(self) -> int:
        """Get total neuron count."""
        if not self.is_initialized():
            return 0
        return self._brain.getTotalNeuronCount()
    
    def get_total_synapse_count(self) -> int:
        """Get total synapse count."""
        if not self.is_initialized():
            return 0
        return self._brain.getTotalSynapseCount()
    
    def get_total_spike_count(self) -> int:
        """Get total spike count."""
        if not self.is_initialized():
            return 0
        return self._brain.getTotalSpikeCount()
    
    def get_firing_neuron_count(self) -> int:
        """Get number of firing neurons."""
        if not self.is_initialized():
            return 0
        return self._brain.getFiringNeuronCount()
    
    def get_active_neuron_count(self) -> int:
        """Get number of active neurons."""
        if not self.is_initialized():
            return 0
        return self._brain.getActiveNeuronCount()
    
    def get_average_firing_rate(self) -> float:
        """Get average firing rate."""
        if not self.is_initialized():
            return 0.0
        return self._brain.getAverageFiringRate()
    
    def get_excitation_inhibition_ratio(self) -> float:
        """Get excitation/inhibition ratio."""
        if not self.is_initialized():
            return 0.0
        return self._brain.getExcitationInhibitionRatio()
    
    def get_developmental_stage(self) -> DevelopmentalStage:
        """Get current developmental stage."""
        if not self.is_initialized():
            return DevelopmentalStage.Initial
        return self._brain.getDevelopmentalStage()
    
    def set_developmental_stage(self, stage: DevelopmentalStage) -> None:
        """Set developmental stage."""
        if not self.is_initialized():
            raise BrainError("Brain must be initialized to set developmental stage")
        self._brain.setDevelopmentalStage(stage)
    
    # Memory system access
    def get_working_memory(self):
        """Get working memory system."""
        if not self.is_initialized():
            return None
        return self._brain.getWorkingMemory()
    
    def get_episodic_memory(self):
        """Get episodic memory system."""
        if not self.is_initialized():
            return None
        return self._brain.getEpisodicMemory()
    
    def get_associative_memory(self):
        """Get associative memory system."""
        if not self.is_initialized():
            return None
        return self._brain.getAssociativeMemory()
    
    # Neuromodulation system access
    def get_dopamine(self):
        """Get dopamine system."""
        if not self.is_initialized():
            return None
        return self._brain.getDopamine()
    
    def get_curiosity(self):
        """Get curiosity system."""
        if not self.is_initialized():
            return None
        return self._brain.getCuriosity()
    
    def get_novelty(self):
        """Get novelty system."""
        if not self.is_initialized():
            return None
        return self._brain.getNovelty()
    
    def get_prediction_error(self):
        """Get prediction error system."""
        if not self.is_initialized():
            return None
        return self._brain.getPredictionErrorSignal()
    
    # Prediction system access
    def get_prediction_system(self):
        """Get prediction system."""
        if not self.is_initialized():
            return None
        return self._brain.getPredictionSystem()
    
    # Cognition systems access
    def get_planner(self):
        """Get neural planner."""
        if not self.is_initialized():
            return None
        return self._brain.getPlanner()
    
    def get_concept_formation(self):
        """Get concept formation system."""
        if not self.is_initialized():
            return None
        return self._brain.getConceptFormation()
    
    def get_attention(self):
        """Get attention system."""
        if not self.is_initialized():
            return None
        return self._brain.getAttention()
    
    # Development system access
    def get_development_system(self):
        """Get development system."""
        if not self.is_initialized():
            return None
        return self._brain.getDevelopmentSystem()
    
    # Configuration access
    def get_config(self):
        """Get brain configuration."""
        if not self.is_initialized():
            return None
        return self._brain.getConfig()
    
    def get_random_generator(self):
        """Get random generator."""
        if not self.is_initialized():
            return None
        return self._brain.getRandomGenerator()
    
    # Logging and status
    def log_status(self) -> None:
        """Log brain status information."""
        if not self.is_initialized():
            return
        self._brain.logStatus()
    
    def __repr__(self) -> str:
        status = "initialized" if self.is_initialized() else "not initialized"
        return f"NMLBrain(status={status}, neurons={self.get_total_neuron_count()})"
    
    def __str__(self) -> str:
        if not self.is_initialized():
            return "NMLBrain(not initialized)"
        
        return (f"NMLBrain("
                f"neurons={self.get_total_neuron_count()}, "
                f"synapses={self.get_total_synapse_count()}, "
                f"spikes={self.get_total_spike_count()}, "
                f"stage={self.get_developmental_stage()})")
# Alias for backward compatibility
Brain = NMLBrain


class NMLSimpleWorld:
    """
    Python wrapper for NLM SimpleWorld with enhanced functionality.
    
    Provides Pythonic interface to the NLM world simulation with
    additional utilities and convenience methods.
    """
    
    def __init__(self):
        """Initialize NLM world."""
        self._world = createSimpleWorld()
        self._is_initialized = False
    
    def configure(self, width: int, height: int, 
                 vision_width: int = 8, vision_height: int = 8) -> None:
        """
        Configure world dimensions.
        
        Args:
            width: World width
            height: World height
            vision_width: Vision width
            vision_height: Vision height
        """
        self._world.configure(width, height, vision_width, vision_height)
        self._is_initialized = True
    
    def is_initialized(self) -> bool:
        """Check if world is initialized."""
        return self._is_initialized
    
    def reset(self) -> None:
        """Reset world to initial state."""
        if self._is_initialized:
            self._world.reset()
    
    def update(self, timestep: float) -> None:
        """
        Update world state.
        
        Args:
            timestep: Time step to update
        """
        if not self._is_initialized:
            raise SimulationError("World must be configured before updating")
        self._world.update(timestep)
    
    # World information methods
    def get_width(self) -> int:
        """Get world width."""
        return self._world.getWidth()
    
    def get_height(self) -> int:
        """Get world height."""
        return self._world.getHeight()
    
    def get_vision_width(self) -> int:
        """Get vision width."""
        return self._world.getVisionWidth()
    
    def get_vision_height(self) -> int:
        """Get vision height."""
        return self._world.getVisionHeight()
    
    def get_max_energy(self) -> float:
        """Get maximum energy."""
        return self._world.getMaxEnergy()
    
    def get_energy_decay_rate(self) -> float:
        """Get energy decay rate."""
        return self._world.getEnergyDecayRate()
    
    # Agent and sensory methods
    def set_agent_start(self, x: float, y: float) -> None:
        """Set agent starting position."""
        self._world.setAgentStart(x, y)
    
    def get_agent_body(self):
        """Get agent body state."""
        return self._world.getAgentBody()
    
    def get_sensory_percept(self):
        """Get sensory percept from world."""
        return self._world.getSensoryPercept()
    
    def apply_motor_command(self, action, timestep: float) -> None:
        """
        Apply motor command to world.
        
        Args:
            action: Motor action
            timestep: Current time step
        """
        self._world.applyMotorCommand(action, timestep)
    
    def is_valid_position(self, x: float, y: float) -> bool:
        """
        Check if position is valid in world.
        
        Args:
            x: X position
            y: Y position
            
        Returns:
            True if position is valid
        """
        return self._world.isValidPosition(x, y)
    
    def get_simulation_time(self) -> float:
        """Get current simulation time."""
        return self._world.getSimulationTime()
    
    def set_random_seed(self, seed: int) -> None:
        """Set random seed for world."""
        self._world.setRandomSeed(seed)
    
    def get_random_seed(self) -> int:
        """Get random seed for world."""
        return self._world.getRandomSeed()
    
    def __repr__(self) -> str:
        if self._is_initialized:
            return f"NMLSimpleWorld(width={self.get_width()}, height={self.get_height()})"
        return "NMLSimpleWorld(not configured)"


class NMLAgentBrain:
    """
    Python wrapper for NLM AgentBrain with enhanced functionality.
    
    Provides Pythonic interface to the NLM agent brain that connects
    the brain to the world environment.
    """
    
    def __init__(self, brain):
        """
        Initialize agent brain.
        
        Args:
            brain: NMLBrain or C++ Brain object
        """
        if isinstance(brain, NMLBrain):
            self._brain = brain._brain
        else:
            self._brain = brain
        
        self._agent = createAgentBrain(self._brain)
        self._is_initialized = False
    
    def initialize(self, world) -> None:
        """
        Initialize agent brain with world.
        
        Args:
            world: NMLSimpleWorld or C++ SimpleWorld object
        """
        if isinstance(world, NMLSimpleWorld):
            self._world = world._world
        else:
            self._world = world
        
        self._agent.initialize(self._world)
        self._is_initialized = True
    
    def is_initialized(self) -> bool:
        """Check if agent brain is initialized."""
        return self._is_initialized
    
    def get_brain(self):
        """Get underlying brain object."""
        return self._brain
    
    def get_sensory_input_size(self) -> int:
        """Get expected sensory input size."""
        return self._agent.getSensoryInputSize()
    
    def get_motor_output_size(self) -> int:
        """Get expected motor output size."""
        return self._agent.getMotorOutputSize()
    
    def process_sensory_input(self, percept) -> None:
        """
        Process sensory input and inject into brain.
        
        Args:
            percept: Sensory percept data
        """
        if not self._is_initialized:
            raise SimulationError("Agent brain must be initialized")
        
        self._agent.processSensoryInput(percept)
    
    def decode_motor_command(self):
        """
        Decode brain motor activity into motor command.
        
        Returns:
            Motor command
        """
        if not self._is_initialized:
            raise SimulationError("Agent brain must be initialized")
        
        return self._agent.decodeMotorCommand()
    
    def apply_reward_modulation(self, reward: float, predicted_reward: float) -> None:
        """
        Apply reward-based neuromodulation.
        
        Args:
            reward: Actual reward received
            predicted_reward: Predicted reward
        """
        if not self._is_initialized:
            raise SimulationError("Agent brain must be initialized")
        
        self._agent.applyRewardModulation(reward, predicted_reward)
    
    def update_development(self, timestep: float) -> None:
        """
        Update development system.
        
        Args:
            timestep: Time step
        """
        if not self._is_initialized:
            raise SimulationError("Agent brain must be initialized")
        
        self._agent.updateDevelopment(timestep)
    
    def get_developmental_stage(self) -> DevelopmentalStage:
        """Get current developmental stage."""
        return self._agent.getDevelopmentalStage()
    
    def get_neuromodulation_level(self) -> float:
        """Get current neuromodulation level."""
        return self._agent.getNeuromodulationLevel()
    
    def get_curiosity_level(self) -> float:
        """Get curiosity level."""
        return self._agent.getCuriosityLevel()
    
    def get_novelty_level(self) -> float:
        """Get novelty level."""
        return self._agent.getNoveltyLevel()
    
    def get_prediction_error(self) -> float:
        """Get prediction error."""
        return self._agent.getPredictionError()
    
    def reset(self) -> None:
        """Reset agent brain for new episode."""
        self._agent.reset()
        self._is_initialized = False
    
    # Configuration methods
    def enable_reward_modulation(self, enable: bool) -> None:
        """Enable/disable reward modulation."""
        self._agent.enableRewardModulation(enable)
    
    def enable_structural_plasticity(self, enable: bool) -> None:
        """Enable/disable structural plasticity."""
        self._agent.enableStructuralPlasticity(enable)
    
    def enable_development(self, enable: bool) -> None:
        """Enable/disable development."""
        self._agent.enableDevelopment(enable)
    
    def enable_curiosity(self, enable: bool) -> None:
        """Enable/disable curiosity."""
        self._agent.enableCuriosity(enable)
    
    def is_reward_modulation_enabled(self) -> bool:
        """Check if reward modulation is enabled."""
        return self._agent.isRewardModulationEnabled()
    
    def is_structural_plasticity_enabled(self) -> bool:
        """Check if structural plasticity is enabled."""
        return self._agent.isStructuralPlasticityEnabled()
    
    def is_development_enabled(self) -> bool:
        """Check if development is enabled."""
        return self._agent.isDevelopmentEnabled()
    
    def is_curiosity_enabled(self) -> bool:
        """Check if curiosity is enabled."""
        return self._agent.isCuriosityEnabled()
    
    def __repr__(self) -> str:
        return f"NMLAgentBrain(brain={self.get_brain()})"


# Convenience functions
def create_brain(config=None) -> NMLBrain:
    """
    Create NLM brain with optional configuration.
    
    Args:
        config: NLM configuration (optional)
        
    Returns:
        NMLBrain object
    """
    return NMLBrain(config)
def create_agent_brain(brain) -> NMLAgentBrain:
    """
    Create NLM agent brain.
    
    Args:
        brain: NMLBrain or C++ Brain object
        
    Returns:
        NMLAgentBrain object
    """
    return NMLAgentBrain(brain)
def create_simple_world() -> NMLSimpleWorld:
    """
    Create NLM simple world.
    
    Returns:
        NMLSimpleWorld object
    """
    return NMLSimpleWorld()


# Type aliases for convenience
ActionType = Action
MotorCommandType = MotorCommand
WorldObjectType = WorldObject
SensoryPerceptType = SensoryPercept
AgentBodyType = AgentBody

# Global convenience instances
_default_config = None
_default_brain = None
_default_world = None
_default_agent = None

def get_default_config():
    """Get default configuration (creates if needed)."""
    global _default_config
    if _default_config is None:
        _default_config = NLMConfig()
    return _default_config
def get_default_brain():
    """Get default brain (creates if needed)."""
    global _default_brain
    if _default_brain is None:
        _default_brain = create_brain(get_default_config())
        _default_brain.initialize()
    return _default_brain
def get_default_world():
    """Get default world (creates if needed)."""
    global _default_world
    if _default_world is None:
        _default_world = create_simple_world()
        _default_world.configure(100, 100, 16, 16)
    return _default_world
def get_default_agent():
    """Get default agent (creates if needed)."""
    global _default_agent
    if _default_agent is None:
        _default_agent = create_agent_brain(get_default_brain())
        _default_agent.initialize(get_default_world())
    return _default_agent

# Simulation utilities
class Simulation:
    """
    Simulation utilities for running NLM simulations.
    
    Provides helper methods for running simulations with
    common patterns and error handling.
    """
    
    @staticmethod
    def run_steps(brain, world, agent, num_steps: int, 
                 timestep: float = 0.1) -> Dict[str, Any]:
        """
        Run simulation for specified number of steps.
        
        Args:
            brain: NMLBrain object
            world: NMLSimpleWorld object
            agent: NMLAgentBrain object
            num_steps: Number of steps to run
            timestep: Time step size
            
        Returns:
            Dictionary with simulation results
        """
        results = {
            'steps': num_steps,
            'final_spikes': brain.get_total_spike_count(),
            'final_firing': brain.get_firing_neuron_count(),
            'average_firing_rate': brain.get_average_firing_rate(),
            'final_stage': brain.get_developmental_stage().value,
            'final_curiosity': agent.get_curiosity_level(),
            'final_novelty': agent.get_novelty_level(),
            'final_reward': agent.get_neuromodulation_level(),
        }
        
        for step in range(num_steps):
            # Update world
            world.update(timestep)
            
            # Get sensory input
            percept = world.get_sensory_percept()
            
            # Process in agent
            agent.process_sensory_input(percept)
            
            # Brain thinks
            brain.step(step)
            
            # Agent decides action
            action = agent.decode_motor_command()
            
            # Apply action to world
            world.apply_motor_command(action, world.get_simulation_time())
            
            # Apply reward modulation
            reward = 0.0  # Would come from world in real scenario
            agent.apply_reward_modulation(reward, 0.0)
            
            # Update development
            agent.update_development(timestep)
        
        # Update final results
        results.update({
            'final_spikes': brain.get_total_spike_count(),
            'final_firing': brain.get_firing_neuron_count(),
            'average_firing_rate': brain.get_average_firing_rate(),
            'final_stage': brain.get_developmental_stage().value,
            'final_curiosity': agent.get_curiosity_level(),
            'final_novelty': agent.get_novelty_level(),
            'final_reward': agent.get_neuromodulation_level(),
        })
        
        return results
    
    @staticmethod
    def run_brain_only(brain, num_steps: int, timestep: float = 0.1) -> Dict[str, Any]:
        """
        Run brain simulation without world interaction.
        
        Args:
            brain: NMLBrain object
            num_steps: Number of steps to run
            timestep: Time step size
            
        Returns:
            Dictionary with simulation results
        """
        results = {
            'steps': num_steps,
            'final_spikes': brain.get_total_spike_count(),
            'final_firing': brain.get_firing_neuron_count(),
            'average_firing_rate': brain.get_average_firing_rate(),
        }
        
        for step in range(num_steps):
            brain.step(step)
        
        # Update final results
        results.update({
            'final_spikes': brain.get_total_spike_count(),
            'final_firing': brain.get_firing_neuron_count(),
            'average_firing_rate': brain.get_average_firing_rate(),
        })
        
        return results


# Export all main classes
__all__.extend([
    'NMLBrain', 'NMLSimpleWorld', 'NMLAgentBrain',
    'Simulation', 'create_brain', 'create_agent_brain', 
    'create_simple_world', 'get_default_config', 'get_default_brain',
    'get_default_world', 'get_default_agent'
])

# Add type stubs for mypy
if False:
    # This block is only for type checking, not runtime
    from typing import TYPE_CHECKING
    
    if TYPE_CHECKING:
        ConfigType = Any
        BrainType = Any
        SimpleWorldType = Any
        AgentBrainType = Any
        VisionType = Any
        AudioType = Any
        ActionType = Any
        SensoryPerceptType = Any
        WorldObjectType = Any
        AgentBodyType = Any
        ActionResultType = Any
