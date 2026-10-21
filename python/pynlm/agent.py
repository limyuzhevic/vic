"""
Python bindings for NLM agent brain class.

This module provides Python bindings for the NLM agent brain class,
which serves as an interface between the neural brain and the world.
"""

from .core import Brain, SensoryPercept, Action, DevelopmentalStage, ActionType
from .world import SimpleWorld
class AgentBrain:
    """
    Agent brain interface connecting NLM brain to world.
    
    This class provides the interface for processing sensory input
    from the world and producing motor actions based on neural activity.
    """
    
    def __init__(self, brain: Brain):
        """
        Initialize with brain.
        
        Args:
            brain: The brain to interface with
        """
        self._brain = brain
        self._world = None
        self._initialized = False
        
        # Enable/Disable features
        self._reward_modulation_enabled = False
        self._structural_plasticity_enabled = False
        self._development_enabled = False
        self._curiosity_enabled = False
        
        # Neuromodulation levels
        self._neuromodulation_level = 0.0
        self._curiosity_level = 0.0
        self._novelty_level = 0.0
        self._prediction_error = 0.0
    
    def initialize(self, world: SimpleWorld):
        """
        Initialize with world.
        
        Args:
            world: The world to interact with
        """
        self._world = world
        self._initialized = True
        self._brain.initialize()
    
    def getSensoryInputSize(self) -> int:
        """Get expected sensory input size."""
        return 100  # Placeholder
    
    def getMotorOutputSize(self) -> int:
        """Get expected motor output size."""
        return 50  # Placeholder
    
    def processSensoryInput(self, percept: SensoryPercept):
        """
        Process sensory percept and inject into brain.
        
        Args:
            percept: Sensory percept data
        """
        if not self._initialized:
            raise RuntimeError("AgentBrain must be initialized before processing input")
        
        # Process vision
        vision = percept.getVision()
        if vision.getWidth() > 0 and vision.getHeight() > 0:
            # Convert vision to neural input
            data = vision.getData()
            if data:
                self._brain.receiveSensoryInput(data)
        
        # Process audio
        audio = percept.getAudio()
        if audio.getNumSamples() > 0:
            data = audio.getData()
            if data:
                self._brain.receiveSensoryInput(data)
        
        # Process internal signals
        internal = percept.getInternal()
        if internal.getDimensions()[0] > 0:
            signals = internal.getData()
            if signals:
                self._brain.receiveSensoryInput(signals)
    
    def decodeMotorCommand(self) -> Action:
        """
        Decode brain motor activity into motor command.
        
        Returns:
            Action object representing the motor command
        """
        if not self._initialized:
            raise RuntimeError("AgentBrain must be initialized before decoding commands")
        
        # Use brain to produce action
        return self._brain.produceAction()
    
    def applyRewardModulation(self, reward: float, predicted_reward: float):
        """
        Apply reward-based neuromodulation.
        
        Args:
            reward: Actual reward received
            predicted_reward: Predicted reward
        """
        if not self._reward_modulation_enabled:
            return
        
        # Calculate reward prediction error
        error = reward - predicted_reward
        self._prediction_error = error
        
        # Update neuromodulation level based on error
        learning_rate = 0.01
        self._neuromodulation_level += learning_rate * abs(error)
        
        # Clamp to reasonable range
        self._neuromodulation_level = max(0.0, min(1.0, self._neuromodulation_level))
    
    def updateDevelopment(self, timestep: float):
        """
        Update development system.
        
        Args:
            timestep: Time step for development update
        """
        if not self._development_enabled:
            return
        
        # Update developmental stage based on time
        current_stage = self._brain.getDevelopmentalStage()
        if current_stage == DevelopmentalStage.Initial and timestep > 100:
            self._brain.setDevelopmentalStage(DevelopmentalStage.CriticalPeriod)
        elif current_stage == DevelopmentalStage.CriticalPeriod and timestep > 1000:
            self._brain.setDevelopmentalStage(DevelopmentalStage.Maturation)
        elif current_stage == DevelopmentalStage.Maturation and timestep > 10000:
            self._brain.setDevelopmentalStage(DevelopmentalStage.Adult)
    
    def getDevelopmentalStage(self) -> DevelopmentalStage:
        """Get current developmental stage."""
        return self._brain.getDevelopmentalStage()
    
    def getNeuromodulationLevel(self) -> float:
        """Get current neuromodulation level."""
        return self._neuromodulation_level
    
    def getCuriosityLevel(self) -> float:
        """Get curiosity level."""
        return self._curiosity_level
    
    def getNoveltyLevel(self) -> float:
        """Get novelty level."""
        return self._novelty_level
    
    def getPredictionError(self) -> float:
        """Get prediction error."""
        return self._prediction_error
    
    def reset(self):
        """Reset agent for new episode."""
        self._initialized = False
        self._world = None
        self._neuromodulation_level = 0.0
        self._curiosity_level = 0.0
        self._novelty_level = 0.0
        self._prediction_error = 0.0
        self._brain.reset()
    
    def getBrain(self) -> Brain:
        """Get the underlying brain."""
        return self._brain
    
    def enableRewardModulation(self, enable: bool):
        """
        Enable/disable reward modulation.
        
        Args:
            enable: Whether to enable reward modulation
        """
        self._reward_modulation_enabled = enable
    
    def enableStructuralPlasticity(self, enable: bool):
        """
        Enable/disable structural plasticity.
n        
        Args:
            enable: Whether to enable structural plasticity
        """
        self._structural_plasticity_enabled = enable
    
    def enableDevelopment(self, enable: bool):
        """
        Enable/disable development.
        
        Args:
            enable: Whether to enable development
        """
        self._development_enabled = enable
    
    def enableCuriosity(self, enable: bool):
        """
        Enable/disable curiosity.
        
        Args:
            enable: Whether to enable curiosity
        """
        self._curiosity_enabled = enable
    
    def isRewardModulationEnabled(self) -> bool:
        """Check if reward modulation is enabled."""
        return self._reward_modulation_enabled
    
    def isStructuralPlasticityEnabled(self) -> bool:
        """Check if structural plasticity is enabled."""
        return self._structural_plasticity_enabled
    
    def isDevelopmentEnabled(self) -> bool:
        """Check if development is enabled."""
        return self._development_enabled
    
    def isCuriosityEnabled(self) -> bool:
        """Check if curiosity is enabled."""
        return self._curiosity_enabled
