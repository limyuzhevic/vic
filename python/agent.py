"""
NLM AgentBrain implementation.

This module provides the AgentBrain class that serves as an interface
connecting the NLM brain to the world. The AgentBrain wraps the neural
brain with an agent interface that processes sensory inputs, produces
motor outputs, and manages learning and development processes.
"""

from typing import List, Any, Optional, Union

from .brain import Brain, EnhancedBrain

# Import C++ bindings
from .bindings import (
    AgentBrain as _AgentBrain,
    Brain as _Brain,
)
class EnhancedAgentBrain:
    """Enhanced AgentBrain class with better error handling.
    
    This class provides a Pythonic interface to the NLM agent brain,
    making it easier to work with the agent interface through properties
    and methods that follow Python conventions.
    
    The enhanced agent brain provides:
    - Better error handling with custom exceptions
    - Convenience methods for common operations
    - Learning and development management
    - Neuromodulation system control
    - Simulation statistics and monitoring
    
    Example:
        >>> config = EnhancedConfig()
        >>> brain = EnhancedBrain(config)
        >>> agent = EnhancedAgentBrain(brain)
        >>> agent.initialize(world)
        >>> 
        >>> # Process sensory input
        >>> action = agent.process_sensory_input(percept)
        >>> 
        >>> # Enable learning systems
        >>> agent.enable_reward_modulation(True)
        >>> agent.enable_curiosity(True)
        >>> agent.enable_development(True)
    """
    
    def __init__(self, brain: Union[Brain, EnhancedBrain, Any]):
        """Initialize agent brain with brain instance.
        
        Args:
            brain: Brain or EnhancedBrain instance to wrap.
            
        Raises:
            TypeError: If brain is not a valid brain object.
            SimulationError: If brain initialization fails.
        """
        try:
            if isinstance(brain, Brain):
                self._brain = brain
            elif isinstance(brain, EnhancedBrain):
                self._brain = brain
            else:
                # Try to convert to EnhancedBrain
                self._brain = EnhancedBrain(brain)
            
            self._agent = _AgentBrain(self._brain._brain if hasattr(self._brain, '_brain') else brain)
            self._initialized = False
            
        except Exception as e:
            raise SimulationError(f"Failed to create agent brain: {e}")
    
    def initialize(self, world: Any) -> bool:
        """Initialize with world.
        
        Args:
            world: World instance to connect with.
            
        Returns:
            True if successful, False otherwise.
            
        Raises:
            SimulationError: If initialization fails.
        """
        try:
            self._initialized = self._agent.initialize(world)
            return self._initialized
        except Exception as e:
            raise SimulationError(f"Failed to initialize agent brain: {e}")
    
    def getSensoryInputSize(self) -> int:
        """Get expected sensory input size.
        
        Returns:
            Expected size of sensory input.
            
        Raises:
            SimulationError: If input size query fails.
        """
        try:
            return self._agent.getSensoryInputSize()
        except Exception as e:
            raise SimulationError(f"Failed to get sensory input size: {e}")
    
    def getMotorOutputSize(self) -> int:
        """Get expected motor output size.
        
        Returns:
            Expected size of motor output.
            
        Raises:
            SimulationError: If output size query fails.
        """
        try:
            return self._agent.getMotorOutputSize()
        except Exception as e:
            raise SimulationError(f"Failed to get motor output size: {e}")
    
    def processSensoryInput(self, percept: Any) -> Any:
        """Process sensory percept and inject into brain.
        
        Args:
            percept: Sensory percept to process.
            
        Returns:
            Processed action based on brain activity.
            
        Raises:
            SimulationError: If input processing fails.
        """
        if not self._initialized:
            raise SimulationError("Agent brain must be initialized before processing input")
        
        try:
            self._agent.processSensoryInput(percept)
            return self.decodeMotorCommand()
        except Exception as e:
            raise SimulationError(f"Failed to process sensory input: {e}")
    
    def decodeMotorCommand(self) -> Any:
        """Decode brain motor activity into motor command.
        
        Returns:
            Motor command based on brain activity.
            
        Raises:
            SimulationError: If command decoding fails.
        """
        if not self._initialized:
            raise SimulationError("Agent brain must be initialized before decoding command")
        
        try:
            return self._agent.decodeMotorCommand()
        except Exception as e:
            raise SimulationError(f"Failed to decode motor command: {e}")
    
    def applyRewardModulation(self, reward: float, predicted_reward: float) -> None:
        """Apply reward-based neuromodulation.
        
        Args:
            reward: Actual reward received.
            predicted_reward: Predicted reward value.
            
        Raises:
            SimulationError: If reward modulation fails.
        """
        if not self._initialized:
            raise SimulationError("Agent brain must be initialized before applying reward modulation")
        
        try:
            self._agent.applyRewardModulation(reward, predicted_reward)
        except Exception as e:
            raise SimulationError(f"Failed to apply reward modulation: {e}")
    
    def updateDevelopment(self, timestep: float) -> None:
        """Update development system.
        
        Args:
            timestep: Time step for development update.
            
        Raises:
            SimulationError: If development update fails.
        """
        if not self._initialized:
            raise SimulationError("Agent brain must be initialized before updating development")
        
        try:
            self._agent.updateDevelopment(timestep)
        except Exception as e:
            raise SimulationError(f"Failed to update development: {e}")
    
    def getDevelopmentalStage(self) -> Any:
        """Get current developmental stage.
        
        Returns:
            Current developmental stage.
            
        Raises:
            SimulationError: If stage query fails.
        """
        try:
            return self._agent.getDevelopmentalStage()
        except Exception as e:
            raise SimulationError(f"Failed to get developmental stage: {e}")
    
    def getNeuromodulationLevel(self) -> float:
        """Get current neuromodulation level.
        
        Returns:
            Current neuromodulation level.
            
        Raises:
            SimulationError: If neuromodulation level query fails.
        """
        try:
            return self._agent.getNeuromodulationLevel()
        except Exception as e:
            raise SimulationError(f"Failed to get neuromodulation level: {e}")
    
    def getCuriosityLevel(self) -> float:
        """Get curiosity level.
        
        Returns:
            Current curiosity level.
            
        Raises:
            SimulationError: If curiosity level query fails.
        """
        try:
            return self._agent.getCuriosityLevel()
        except Exception as e:
            raise SimulationError(f"Failed to get curiosity level: {e}")
    
    def getNoveltyLevel(self) -> float:
        """Get novelty level.
        
        Returns:
            Current novelty level.
            
        Raises:
            SimulationError: If novelty level query fails.
        """
        try:
            return self._agent.getNoveltyLevel()
        except Exception as e:
            raise SimulationError(f"Failed to get novelty level: {e}")
    
    def getPredictionError(self) -> float:
        """Get prediction error.
        
        Returns:
            Current prediction error.
            
        Raises:
            SimulationError: If prediction error query fails.
        """
        try:
            return self._agent.getPredictionError()
        except Exception as e:
            raise SimulationError(f"Failed to get prediction error: {e}")
    
    def reset(self) -> None:
        """Reset agent for new episode.
        
        Raises:
            SimulationError: If reset fails.
        """
        try:
            self._agent.reset()
            self._initialized = False
        except Exception as e:
            raise SimulationError(f"Failed to reset agent brain: {e}")
    
    def getBrain(self) -> Union[Brain, EnhancedBrain]:
        """Get the underlying brain.
        
        Returns:
            Brain or EnhancedBrain instance.
            
        Raises:
            SimulationError: If brain retrieval fails.
        """
        try:
            brain_obj = self._agent.getBrain()
            # Return the same type as was used to create the agent
            if isinstance(self._brain, EnhancedBrain):
                return self._brain
            else:
                return self._brain
        except Exception as e:
            raise SimulationError(f"Failed to get agent brain: {e}")
    
    # Learning system control methods
    
    def enableRewardModulation(self, enable: bool) -> None:
        """Enable or disable reward modulation.
        
        Args:
            enable: True to enable, False to disable.
            
        Raises:
            SimulationError: If enable/disable operation fails.
        """
        try:
            self._agent.enableRewardModulation(enable)
        except Exception as e:
            raise SimulationError(f"Failed to {'enable' if enable else 'disable'} reward modulation: {e}")
    
    def enableStructuralPlasticity(self, enable: bool) -> None:
        """Enable or disable structural plasticity.
        
        Args:
            enable: True to enable, False to disable.
            
        Raises:
            SimulationError: If enable/disable operation fails.
        """
        try:
            self._agent.enableStructuralPlasticity(enable)
        except Exception as e:
            raise SimulationError(f"Failed to {'enable' if enable else 'disable'} structural plasticity: {e}")
    
    def enableDevelopment(self, enable: bool) -> None:
        """Enable or disable development.
        
        Args:
            enable: True to enable, False to disable.
            
        Raises:
            SimulationError: If enable/disable operation fails.
        """
        try:
            self._agent.enableDevelopment(enable)
        except Exception as e:
            raise SimulationError(f"Failed to {'enable' if enable else 'disable'} development: {e}")
    
    def enableCuriosity(self, enable: bool) -> None:
        """Enable or disable curiosity.
        
        Args:
            enable: True to enable, False to disable.
            
        Raises:
            SimulationError: If enable/disable operation fails.
        """
        try:
            self._agent.enableCuriosity(enable)
        except Exception as e:
            raise SimulationError(f"Failed to {'enable' if enable else 'disable'} curiosity: {e}")
    
    # Learning system status methods
    
    def isRewardModulationEnabled(self) -> bool:
        """Check if reward modulation is enabled.
        
        Returns:
            True if reward modulation is enabled, False otherwise.
            
        Raises:
            SimulationError: If status query fails.
        """
        try:
            return self._agent.isRewardModulationEnabled()
        except Exception as e:
            raise SimulationError(f"Failed to check reward modulation status: {e}")
    
    def isStructuralPlasticityEnabled(self) -> bool:
        """Check if structural plasticity is enabled.
        
        Returns:
            True if structural plasticity is enabled, False otherwise.
            
        Raises:
            SimulationError: If status query fails.
        """
        try:
            return self._agent.isStructuralPlasticityEnabled()
        except Exception as e:
            raise SimulationError(f"Failed to check structural plasticity status: {e}")
    
    def isDevelopmentEnabled(self) -> bool:
        """Check if development is enabled.
        
        Returns:
            True if development is enabled, False otherwise.
            
        Raises:
            SimulationError: If status query fails.
        """
        try:
            return self._agent.isDevelopmentEnabled()
        except Exception as e:
            raise SimulationError(f"Failed to check development status: {e}")
    
    def isCuriosityEnabled(self) -> bool:
        """Check if curiosity is enabled.
        
        Returns:
            True if curiosity is enabled, False otherwise.
            
        Raises:
            SimulationError: If status query fails.
        """
        try:
            return self._agent.isCuriosityEnabled()
        except Exception as e:
            raise SimulationError(f"Failed to check curiosity status: {e}")
    
    # Pythonic properties for easier access
    
    @property
    def neuromodulation_level(self) -> float:
        """Current neuromodulation level."""
        return self.getNeuromodulationLevel()
    
    @property
    def curiosity_level(self) -> float:
        """Current curiosity level."""
        return self.getCuriosityLevel()
    
    @property
    def novelty_level(self) -> float:
        """Current novelty level."""
        return self.getNoveltyLevel()
    
    @property
    def prediction_error(self) -> float:
        """Current prediction error."""
        return self.getPredictionError()
    
    @property
    def developmental_stage(self) -> Any:
        """Current developmental stage."""
        return self.getDevelopmentalStage()
    
    @property
    def initialized(self) -> bool:
        """Whether the agent brain has been initialized."""
        return self._initialized
    
    @property
    def brain(self) -> Union[Brain, EnhancedBrain]:
        """Get the underlying brain."""
        return self.getBrain()
    
    def __repr__(self) -> str:
        return f"EnhancedAgentBrain(initialized={self.initialized})"
class AgentBrain(EnhancedAgentBrain):
    """AgentBrain class for NLM agent interface.
    
    This class serves as an interface connecting the NLM brain to the world.
    The AgentBrain wraps the neural brain with an agent interface that
    processes sensory inputs, produces motor outputs, and manages learning
    and development processes.
    
    The class provides the complete agent interface for the NLM framework,
    including working memory, episodic memory, prediction systems, cognition,
    neuromodulation, and development systems.
    
    Example:
        >>> config = Config()
        >>> brain = Brain(config)
        >>> agent = AgentBrain(brain)
        >>> agent.initialize(world)
        >>> 
        >>> # Process sensory input and get action
        >>> action = agent.processSensoryInput(percept)
        >>> 
        >>> # Access learning systems
        >>> agent.enable_reward_modulation(True)
        >>> agent.enable_curiosity(True)
    """
    
    def __init__(self, brain: Union[Brain, EnhancedBrain]):
        """Initialize agent brain with brain instance.
        
        Args:
            brain: Brain or EnhancedBrain instance to wrap.
        """
        super().__init__(brain)
    
    # The enhanced agent brain provides all the functionality through inheritance
    # This class is kept for backward compatibility and clarity


# Type aliases for better documentation
SimulationStep = int
Timestamp = float

__all__ = ["AgentBrain", "EnhancedAgentBrain", "SimulationStep", "Timestamp"]
