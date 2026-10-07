# Agent implementation for NLM Python API

"""
Agent brain interface connecting NLM brain to world

Provides a Pythonic interface to the NLM agent system,
with type hints, better error handling, and convenient methods.
"""

from typing import List, Any, Optional, Union, Dict
from pathlib import Path

# Import the C++ binding and type definitions
from pynlm import Brain, SimpleWorld
from pynlm import AgentBrain as CppAgentBrain
from pynlm import SensoryPercept
from pynlm.types import SimulationStep, Timestamp, Vision, Audio, InternalSignals, SensoryInput
from pynlm.neuromodulation import Dopamine, Curiosity, Novelty, PredictionError
from pynlm.development import DevelopmentalStage

class NLMError(Exception):
    """Base exception for NLM Python API errors"""
    pass

class AgentError(NLMError):
    """Raised when agent operations fail"""
    pass

class AgentBrain:
    """
    NLM Agent Brain Interface
    
    Connects NLM brain to the world, handling sensory transduction
    and motor decoding. The agent acts as the interface between
    the neural brain and the external environment.
    
    The agent processes sensory input from the world and decodes
    brain activity into motor commands that can be executed in the world.
    
    Examples:
        >>> config = Config()
        >>> brain = Brain(config)
        >>> brain.initialize()
        >>> 
        >>> world = SimpleWorld()
        >>> world.configure(width=20, height=20)
        >>> world.reset()
        >>> 
        >>> agent = AgentBrain(brain)
        >>> agent.initialize(world)
        >>> 
        >>> # Run agent in world
        >>> for step in range(100):
        ...     world.update(0.1)  # Update world
        ...     percept = world.getSensoryPercept()  # What does agent see?
        ...     agent.processSensoryInput(percept)  # Brain sees it
        ...     brain.step(step)  # Brain thinks
        ...     action = agent.decodeMotorCommand()  # Brain decides action
        ...     world.applyMotorCommand(action, world.getSimulationTime())  # Do action
    """
    
    def __init__(self, brain: Brain):
        """
        Initialize agent with brain.
        
        Args:
            brain: NLM brain instance
            
        Raises:
            AgentError: If agent cannot be created
            
        Examples:
            >>> brain = Brain()
            >>> agent = AgentBrain(brain)
        """
        if not isinstance(brain, Brain):
            raise AgentError("Brain must be a Brain instance")
        
        try:
            self._cpp_agent = CppAgentBrain(brain._cpp_brain)
            self._brain = brain
            self._world = None
            self._initialized = False
            
            # Initialize feature flags
            self._reward_modulation_enabled = False
            self._structural_plasticity_enabled = False
            self._development_enabled = False
            self._curiosity_enabled = False
            
        except Exception as e:
            raise AgentError(f"Failed to create agent: {e}")
    
    def initialize(self, world: SimpleWorld) -> None:
        """
        Initialize agent with world.
        
        Args:
            world: NLM world instance
            
        Raises:
            AgentError: If agent cannot be initialized
            
        Examples:
            >>> agent = AgentBrain(brain)
            >>> agent.initialize(world)
        """
        if not isinstance(world, SimpleWorld):
            raise AgentError("World must be a SimpleWorld instance")
        
        try:
            self._cpp_agent.initialize(world._cpp_world)
            self._world = world
            self._initialized = True
        except Exception as e:
            raise AgentError(f"Failed to initialize agent: {e}")
    
    def get_sensory_input_size(self) -> int:
        """
        Get expected sensory input size.
        
        Returns:
            Expected size of sensory input
            
        Raises:
            AgentError: If sensory input size cannot be determined
            RuntimeError: If agent is not initialized
            
        Examples:
            >>> agent = AgentBrain(brain)
            >>> agent.initialize(world)
            >>> size = agent.getSensoryInputSize()
        """
        if not self._initialized:
            raise RuntimeError("Agent is not initialized. Call initialize() first.")
        
        try:
            return self._cpp_agent.getSensoryInputSize()
        except Exception as e:
            raise AgentError(f"Failed to get sensory input size: {e}")
    
    def get_motor_output_size(self) -> int:
        """
        Get expected motor output size.
        
        Returns:
            Expected size of motor output
            
        Raises:
            AgentError: If motor output size cannot be determined
            RuntimeError: If agent is not initialized
            
        Examples:
            >>> agent = AgentBrain(brain)
            >>> agent.initialize(world)
            >>> size = agent.getMotorOutputSize()
        """
        if not self._initialized:
            raise RuntimeError("Agent is not initialized. Call initialize() first.")
        
        try:
            return self._cpp_agent.getMotorOutputSize()
        except Exception as e:
            raise AgentError(f"Failed to get motor output size: {e}")
    
    def process_sensory_input(self, percept: SensoryPercept) -> None:
        """
        Process sensory percept and inject into brain.
        
        Args:
            percept: Sensory percept to process
            
        Raises:
            AgentError: If sensory input cannot be processed
            RuntimeError: If agent is not initialized
            
        Examples:
            >>> agent = AgentBrain(brain)
            >>> agent.initialize(world)
            >>> percept = world.getSensoryPercept()
            >>> agent.processSensoryInput(percept)
        """
        if not self._initialized:
            raise RuntimeError("Agent is not initialized. Call initialize() first.")
        
        if not isinstance(percept, SensoryPercept):
            raise AgentError("Percept must be a SensoryPercept instance")
        
        try:
            self._cpp_agent.processSensoryInput(percept._cpp_percept)
        except Exception as e:
            raise AgentError(f"Failed to process sensory input: {e}")
    
    def decode_motor_command(self) -> Any:
        """
        Decode brain motor activity into motor command.
        
        Returns:
            Motor command object
            
        Raises:
            AgentError: If motor command cannot be decoded
            RuntimeError: If agent is not initialized
            
        Examples:
            >>> agent = AgentBrain(brain)
            >>> agent.initialize(world)
            >>> action = agent.decodeMotorCommand()
            >>> print(f"Action: {action.getType()}")
        """
        if not self._initialized:
            raise RuntimeError("Agent is not initialized. Call initialize() first.")
        
        try:
            return self._cpp_agent.decodeMotorCommand()
        except Exception as e:
            raise AgentError(f"Failed to decode motor command: {e}")
    
    def apply_reward_modulation(self, reward: float, predicted_reward: float) -> None:
        """
        Apply reward-based neuromodulation.
        
        Args:
            reward: Actual reward received
            predicted_reward: Predicted reward
            
        Raises:
            AgentError: If reward modulation cannot be applied
            RuntimeError: If agent is not initialized
            
        Examples:
            >>> agent = AgentBrain(brain)
            >>> agent.initialize(world)
            >>> agent.applyRewardModulation(1.0, 0.5)
        """
        if not self._initialized:
            raise RuntimeError("Agent is not initialized. Call initialize() first.")
        
        if not self._reward_modulation_enabled:
            return  # Silently skip if not enabled
        
        try:
            self._cpp_agent.applyRewardModulation(reward, predicted_reward)
        except Exception as e:
            raise AgentError(f"Failed to apply reward modulation: {e}")
    
    def update_development(self, timestep: float) -> None:
        """
        Update development system.
        
        Args:
            timestep: Time step for development update
            
        Raises:
            AgentError: If development cannot be updated
            RuntimeError: If agent is not initialized
            
        Examples:
            >>> agent = AgentBrain(brain)
            >>> agent.initialize(world)
            >>> agent.updateDevelopment(0.1)
        """
        if not self._initialized:
            raise RuntimeError("Agent is not initialized. Call initialize() first.")
        
        if not self._development_enabled:
            return  # Silently skip if not enabled
        
        try:
            self._cpp_agent.updateDevelopment(timestep)
        except Exception as e:
            raise AgentError(f"Failed to update development: {e}")
    
    def get_developmental_stage(self):
        """
        Get current developmental stage.
        
        Returns:
            Developmental stage
            
        Raises:
            AgentError: If developmental stage cannot be retrieved
            RuntimeError: If agent is not initialized
            
        Examples:
            >>> agent = AgentBrain(brain)
            >>> agent.initialize(world)
            >>> stage = agent.getDevelopmentalStage()
        """
        if not self._initialized:
            raise RuntimeError("Agent is not initialized. Call initialize() first.")
        
        try:
            return self._cpp_agent.getDevelopmentalStage()
        except Exception as e:
            raise AgentError(f"Failed to get developmental stage: {e}")
    
    def get_neuromodulation_level(self) -> float:
        """
        Get current neuromodulation level.
        
        Returns:
            Neuromodulation level
            
        Raises:
            AgentError: If neuromodulation level cannot be retrieved
            RuntimeError: If agent is not initialized
            
        Examples:
            >>> agent = AgentBrain(brain)
            >>> agent.initialize(world)
            >>> level = agent.getNeuromodulationLevel()
        """
        if not self._initialized:
            raise RuntimeError("Agent is not initialized. Call initialize() first.")
        
        try:
            return self._cpp_agent.getNeuromodulationLevel()
        except Exception as e:
            raise AgentError(f"Failed to get neuromodulation level: {e}")
    
    def get_curiosity_level(self) -> float:
        """
        Get curiosity level.
        
        Returns:
            Curiosity level
        
        Raises:
            AgentError: If curiosity level cannot be retrieved
            RuntimeError: If agent is not initialized
        """
        if not self._initialized:
            raise RuntimeError("Agent is not initialized. Call initialize() first.")
        
        try:
            return self._cpp_agent.getCuriosityLevel()
        except Exception as e:
            raise AgentError(f"Failed to get curiosity level: {e}")
    
    def get_novelty_level(self) -> float:
        """
        Get novelty level.
        
        Returns:
            Novelty level
        
        Raises:
            AgentError: If novelty level cannot be retrieved
            RuntimeError: If agent is not initialized
        """
        if not self._initialized:
            raise RuntimeError("Agent is not initialized. Call initialize() first.")
        
        try:
            return self._cpp_agent.getNoveltyLevel()
        except Exception as e:
            raise AgentError(f"Failed to get novelty level: {e}")
    
    def get_prediction_error(self) -> float:
        """
        Get prediction error.
        
        Returns:
            Prediction error
        
        Raises:
            AgentError: If prediction error cannot be retrieved
            RuntimeError: If agent is not initialized
        """
        if not self._initialized:
            raise RuntimeError("Agent is not initialized. Call initialize() first.")
        
        try:
            return self._cpp_agent.getPredictionError()
        except Exception as e:
            raise AgentError(f"Failed to get prediction error: {e}")
    
    def reset(self) -> None:
        """
        Reset agent for new episode.
        
        Raises:
            AgentError: If agent cannot be reset
            RuntimeError: If agent is not initialized
        """
        if not self._initialized:
            raise RuntimeError("Agent is not initialized. Call initialize() first.")
        
        try:
            self._cpp_agent.reset()
        except Exception as e:
            raise AgentError(f"Failed to reset agent: {e}")
    
    def get_brain(self) -> Brain:
        """
        Get underlying brain.
        
        Returns:
            Brain instance
            
        Raises:
            AgentError: If brain cannot be retrieved
        """
        try:
            return self._brain
        except Exception as e:
            raise AgentError(f"Failed to get brain: {e}")
    
    # Configuration methods
    def enable_reward_modulation(self, enable: bool) -> None:
        """
        Enable or disable reward modulation.
        
        Args:
            enable: Whether to enable reward modulation
            
        Examples:
            >>> agent = AgentBrain(brain)
            >>> agent.initialize(world)
            >>> agent.enableRewardModulation(True)  # Enable learning from rewards
        """
        self._reward_modulation_enabled = enable
        try:
            self._cpp_agent.enableRewardModulation(enable)
        except Exception as e:
            raise AgentError(f"Failed to enable reward modulation: {e}")
    
    def enable_structural_plasticity(self, enable: bool) -> None:
        """
        Enable or disable structural plasticity.
        
        Args:
            enable: Whether to enable structural plasticity
            
        Examples:
            >>> agent = AgentBrain(brain)
            >>> agent.initialize(world)
            >>> agent.enableStructuralPlasticity(True)  # Enable growing new connections
        """
        self._structural_plasticity_enabled = enable
        try:
            self._cpp_agent.enableStructuralPlasticity(enable)
        except Exception as e:
            raise AgentError(f"Failed to enable structural plasticity: {e}")
    
    def enable_development(self, enable: bool) -> None:
        """
        Enable or disable development.
        
        Args:
            enable: Whether to enable development
            
        Examples:
            >>> agent = AgentBrain(brain)
            >>> agent.initialize(world)
            >>> agent.enableDevelopment(True)  # Enable brain maturation
        """
        self._development_enabled = enable
        try:
            self._cpp_agent.enableDevelopment(enable)
        except Exception as e:
            raise AgentError(f"Failed to enable development: {e}")
    
    def enable_curiosity(self, enable: bool) -> None:
        """
        Enable or disable curiosity.
        
        Args:
            enable: Whether to enable curiosity
            
        Examples:
            >>> agent = AgentBrain(brain)
            >>> agent.initialize(world)
            >>> agent.enableCuriosity(True)  # Enable exploration behavior
        """
        self._curiosity_enabled = enable
        try:
            self._cpp_agent.enableCuriosity(enable)
        except Exception as e:
            raise AgentError(f"Failed to enable curiosity: {e}")
    
    # Status checking methods
    def is_reward_modulation_enabled(self) -> bool:
        """
        Check if reward modulation is enabled.
        
        Returns:
            True if reward modulation is enabled
            
        Examples:
            >>> agent = AgentBrain(brain)
            >>> agent.initialize(world)
            >>> if agent.isRewardModulationEnabled():
            ...     print("Learning from rewards")
        """
        try:
            return self._cpp_agent.isRewardModulationEnabled()
        except Exception as e:
            raise AgentError(f"Failed to check reward modulation status: {e}")
    
    def is_structural_plasticity_enabled(self) -> bool:
        """
        Check if structural plasticity is enabled.
        
        Returns:
            True if structural plasticity is enabled
            
        Examples:
            >>> agent = AgentBrain(brain)
            >>> agent.initialize(world)
            >>> if agent.isStructuralPlasticityEnabled():
            ...     print("Can grow new connections")
        """
        try:
            return self._cpp_agent.isStructuralPlasticityEnabled()
        except Exception as e:
            raise AgentError(f"Failed to check structural plasticity status: {e}")
    
    def is_development_enabled(self) -> bool:
        """
        Check if development is enabled.
        
        Returns:
            True if development is enabled
            
        Examples:
            >>> agent = AgentBrain(brain)
            >>> agent.initialize(world)
            >>> if agent.isDevelopmentEnabled():
            ...     print("Brain will mature over time")
        """
        try:
            return self._cpp_agent.isDevelopmentEnabled()
        except Exception as e:
            raise AgentError(f"Failed to check development status: {e}")
    
    def is_curiosity_enabled(self) -> bool:
        """
        Check if curiosity is enabled.
        
        Returns:
            True if curiosity is enabled
            
        Examples:
            >>> agent = AgentBrain(brain)
            >>> agent.initialize(world)
            >>> if agent.isCuriosityEnabled():
            ...     print("Agent will explore new things")
        """
        try:
            return self._cpp_agent.isCuriosityEnabled()
        except Exception as e:
            raise AgentError(f"Failed to check curiosity status: {e}")
    
    def __repr__(self) -> str:
        """String representation of agent"""
        if self._initialized:
            return f"<AgentBrain connected to brain>"
        else:
            return "<AgentBrain not initialized>"
    
    def __str__(self) -> str:
        """String representation of agent"""
        return self.__repr__()

# Factory function for agent creation
def create_agent_brain(brain: Brain) -> AgentBrain:
    """
    Create an agent brain interface connecting NLM brain to world.
    
    Args:
        brain: NLM brain instance
        
    Returns:
        New AgentBrain instance
        
    Examples:
        >>> brain = Brain()
        >>> brain.initialize()
        >>> agent = createAgentBrain(brain)
    """
    return AgentBrain(brain)