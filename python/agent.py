"""
Agent brain interface for NLM - bridge between brain and world.

This module provides the agent brain interface that connects the NLM
brain to the simulated environment, enabling perception, action,
and learning through interaction.
"""

import sys
from typing import List, Dict, Any, Optional

__all__ = [
    'AgentBrain',
    'SensoryPercept',
    'ActionResult',
]


class SensoryPercept:
    """Sensory percept data from the environment.
    
    This class represents the complete sensory input that the agent
    receives from the environment, including vision, audio, internal
    signals, and other sensory modalities.
    
    Attributes:
        vision: Visual sensory data.
        audio: Auditory sensory data.
        internal: Internal state sensory data.
        time: Timestamp when percept was captured.
    """
    
    def __init__(self, vision: List[float] = None, audio: List[float] = None,
                 internal: List[float] = None, time: float = 0.0):
        self._vision = vision or []
        self._audio = audio or []
        self._internal = internal or []
        self._time = time
    
    def getVision(self) -> List[float]:
        """Get visual sensory data.
        
        Returns:
            List[float]: Visual sensory data.
        """
        return self._vision
    
    def setVision(self, vision: List[float]) -> bool:
        """Set visual sensory data.
        
        Args:
            vision: New visual sensory data.
            
        Returns:
            bool: True if vision set successfully.
        """
        self._vision = vision
        return True
    
    def getAudio(self) -> List[float]:
        """Get auditory sensory data.
        
        Returns:
            List[float]: Auditory sensory data.
        """
        return self._audio
    
    def setAudio(self, audio: List[float]) -> bool:
        """Set auditory sensory data.
        
        Args:
            audio: New auditory sensory data.
            
        Returns:
            bool: True if audio set successfully.
        """
        self._audio = audio
        return True
    
    def getInternal(self) -> List[float]:
        """Get internal state sensory data.
        
        Returns:
            List[float]: Internal state sensory data.
        """
        return self._internal
    
    def setInternal(self, internal: List[float]) -> bool:
        """Set internal state sensory data.
        
        Args:
            internal: New internal state sensory data.
            
        Returns:
            bool: True if internal data set successfully.
        """
        self._internal = internal
        return True
    
    def getTime(self) -> float:
        """Get sensory percept timestamp.
        
        Returns:
            float: Timestamp.
        """
        return self._time
    
    def setTime(self, time: float) -> bool:
        """Set sensory percept timestamp.
        
        Args:
            time: New timestamp.
            
        Returns:
            bool: True if timestamp set successfully.
        """
        self._time = time
        return True
    
    def getAllSignals(self) -> Dict[str, List[float]]:
        """Get all sensory signals.
        
        Returns:
            Dict[str, List[float]]: Dictionary of all sensory signals.
        """
        return {
            "vision": self._vision,
            "audio": self._audio,
            "internal": self._internal
        }
    
    def __repr__(self) -> str:
        return f"<SensoryPercept: vision={len(self._vision)} samples, audio={len(self._audio)} samples, internal={len(self._internal)} samples, time={self._time:.1f}>"


class ActionResult:
    """Result of applying an action to the environment.
    
    This class represents the outcome of an action, including any
    rewards received, success status, and descriptive messages.
    
    Attributes:
        reward: Reward received (positive or negative).
        success: Whether action was successful.
        message: Descriptive message about action outcome.
    """
    
    def __init__(self, reward: float = 0.0, success: bool = False,
                 message: str = ""):
        self._reward = reward
        self._success = success
        self._message = message
    
    def getReward(self) -> float:
        """Get action reward.
        
        Returns:
            float: Action reward.
        """
        return self._reward
    
    def setReward(self, reward: float) -> bool:
        """Set action reward.
        
        Args:
            reward: New action reward.
            
        Returns:
            bool: True if reward set successfully.
        """
        self._reward = reward
        return True
    
    def getSuccess(self) -> bool:
        """Get action success status.
        
        Returns:
            bool: Whether action was successful.
        """
        return self._success
    
    def setSuccess(self, success: bool) -> bool:
        """Set action success status.
        
        Args:
            success: New success status.
            
        Returns:
            bool: True if success status set successfully.
        """
        self._success = success
        return True
    
    def getMessage(self) -> str:
        """Get action result message.
        
        Returns:
            str: Action result message.
        """
        return self._message
    
    def setMessage(self, message: str) -> bool:
        """Set action result message.
        
        Args:
            message: New action result message.
            
        Returns:
            bool: True if message set successfully.
        """
        self._message = message
        return True
    
    def __repr__(self) -> str:
        return f"<ActionResult: reward={self._reward:.2f}, success={self._success}, message='{self._message}'>"


class AgentBrain:
    """Agent brain interface connecting NLM brain to world.
    
    The AgentBrain class serves as a bridge between the neural processing
    brain and the simulated environment. It handles perception, action,
    and learning through interaction with the world.
    
    Key features:
    - Sensory input processing
    - Motor output generation
    - Reward-based learning
    - Development system updates
    - Neuromodulation integration
    
    Example:
        >>> from pynlm import createBrain, createSimpleWorld, createAgentBrain
        >>> 
        >>> brain = createBrain()
        >>> brain.initialize()
        >>> 
        >>> world = createSimpleWorld()
        >>> world.configure(width=20, height=20)
        >>> world.reset()
        >>> 
        >>> agent = createAgentBrain(brain)
        >>> agent.initialize(world)
        >>> 
        >>> # Run simulation
        >>> for step in range(100):
        ...     world.update(0.1)
        ...     percept = world.getSensoryPercept()
        ...     agent.processSensoryInput(percept)
        ...     brain.step(step)
        ...     action = agent.decodeMotorCommand()
        ...     result = world.applyMotorCommand(action, world.getSimulationTime())
        ...     agent.applyRewardModulation(result.getReward(), 0.0)
    """
    
    def __init__(self, brain_ptr=None):
        self._agent_brain = brain_ptr
    
    def initialize(self, world_ptr=None) -> bool:
        """Initialize agent brain with world interface.
        
        Args:
            world_ptr: World pointer for environment interface.
            
        Returns:
            bool: True if initialization successful.
        """
        if self._agent_brain:
            return self._agent_brain.initialize(world_ptr)
        return False
    
    def getSensoryInputSize(self) -> int:
        """Get expected sensory input size.
        
        Returns:
            int: Expected sensory input size.
        """
        if self._agent_brain:
            return self._agent_brain.getSensoryInputSize()
        return 0
    
    def getMotorOutputSize(self) -> int:
        """Get expected motor output size.
        
        Returns:
            int: Expected motor output size.
        """
        if self._agent_brain:
            return self._agent_brain.getMotorOutputSize()
        return 0
    
    def processSensoryInput(self, percept: SensoryPercept) -> bool:
        """Process sensory input and inject into brain.
        
        Args:
            percept: Sensory percept to process.
            
        Returns:
            bool: True if input processed successfully.
        """
        if self._agent_brain:
            # Convert percept to appropriate format
            sensory_data = percept.getVision()
            if len(sensory_data) > 0:
                return self._agent_brain.processSensoryInput(sensory_data)
        return False
    
    def decodeMotorCommand(self) -> Dict[str, Any]:
        """Decode brain motor activity into motor command.
        
        Returns:
            Dict[str, Any]: Motor command with type and parameters.
        """
        if self._agent_brain:
            return self._agent_brain.decodeMotorCommand()
        return {"type": "Wait"}
    
    def applyRewardModulation(self, reward: float, predicted_reward: float) -> bool:
        """Apply reward-based neuromodulation.
        
        Args:
            reward: Actual reward received.
            predicted_reward: Predicted reward.
            
        Returns:
            bool: True if reward modulation applied successfully.
        """
        if self._agent_brain:
            return self._agent_brain.applyRewardModulation(reward, predicted_reward)
        return False
    
    def updateDevelopment(self, timestep: float) -> bool:
        """Update development system.
        
        Args:
            timestep: Time step duration.
            
        Returns:
            bool: True if development updated successfully.
        """
        if self._agent_brain:
            return self._agent_brain.updateDevelopment(timestep)
        return False
    
    def getDevelopmentalStage(self) -> str:
        """Get current developmental stage.
        
        Returns:
            str: Current developmental stage.
        """
        if self._agent_brain:
            return self._agent_brain.getDevelopmentalStage()
        return "Initial"
    
    def getNeuromodulationLevel(self) -> float:
        """Get current neuromodulation level.
        
        Returns:
            float: Neuromodulation level (0.0 to 1.0).
        """
        if self._agent_brain:
            return self._agent_brain.getNeuromodulationLevel()
        return 0.0
    
    def getCuriosityLevel(self) -> float:
        """Get curiosity level.
        
        Returns:
            float: Curiosity level (0.0 to 1.0).
        """
        if self._agent_brain:
            return self._agent_brain.getCuriosityLevel()
        return 0.0
    
    def getNoveltyLevel(self) -> float:
        """Get novelty level.
        
        Returns:
            float: Novelty level (0.0 to 1.0).
        """
        if self._agent_brain:
            return self._agent_brain.getNoveltyLevel()
        return 0.0
    
    def getPredictionError(self) -> float:
        """Get prediction error.
        
        Returns:
            float: Prediction error level.
        """
        if self._agent_brain:
            return self._agent_brain.getPredictionError()
        return 0.0
    
    def reset(self) -> None:
        """Reset agent for new episode."""
        if self._agent_brain:
            self._agent_brain.reset()
    
    def getBrain(self) -> 'Brain':
        """Get the underlying brain.
        
        Returns:
            Brain: Underlying brain object.
        """
        # This would need to be implemented based on the C++ bindings
        # For now, return None
        return None
    
    # Enable/Disable features
    def enableRewardModulation(self, enable: bool) -> bool:
        """Enable or disable reward modulation.
        
        Args:
            enable: True to enable, False to disable.
            
        Returns:
            bool: True if enabled status changed successfully.
        """
        if self._agent_brain:
            return self._agent_brain.enableRewardModulation(enable)
        return False
    
    def enableStructuralPlasticity(self, enable: bool) -> bool:
        """Enable or disable structural plasticity.
        
        Args:
            enable: True to enable, False to disable.
            
        Returns:
            bool: True if enabled status changed successfully.
        """
        if self._agent_brain:
            return self._agent_brain.enableStructuralPlasticity(enable)
        return False
    
    def enableDevelopment(self, enable: bool) -> bool:
        """Enable or disable development.
        
        Args:
            enable: True to enable, False to disable.
            
        Returns:
            bool: True if enabled status changed successfully.
        """
        if self._agent_brain:
            return self._agent_brain.enableDevelopment(enable)
        return False
    
    def enableCuriosity(self, enable: bool) -> bool:
        """Enable or disable curiosity.
        
        Args:
            enable: True to enable, False to disable.
            
        Returns:
            bool: True if enabled status changed successfully.
        """
        if self._agent_brain:
            return self._agent_brain.enableCuriosity(enable)
        return False
    
    # Check feature status
    def isRewardModulationEnabled(self) -> bool:
        """Check if reward modulation is enabled.
        
        Returns:
            bool: Whether reward modulation is enabled.
        """
        if self._agent_brain:
            return self._agent_brain.isRewardModulationEnabled()
        return False
    
    def isStructuralPlasticityEnabled(self) -> bool:
        """Check if structural plasticity is enabled.
        
        Returns:
            bool: Whether structural plasticity is enabled.
        """
        if self._agent_brain:
            return self._agent_brain.isStructuralPlasticityEnabled()
        return False
    
    def isDevelopmentEnabled(self) -> bool:
        """Check if development is enabled.
        
        Returns:
            bool: Whether development is enabled.
        """
        if self._agent_brain:
            return self._agent_brain.isDevelopmentEnabled()
        return False
    
    def isCuriosityEnabled(self) -> bool:
        """Check if curiosity is enabled.
        
        Returns:
            bool: Whether curiosity is enabled.
        """
        if self._agent_brain:
            return self._agent_brain.isCuriosityEnabled()
        return False
    
    def __repr__(self) -> str:
        if self._agent_brain:
            return f"<AgentBrain: initialized>"
        return "<AgentBrain: uninitialized>"
