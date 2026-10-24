"""
Agent system classes and interfaces
"""

from typing import Optional, List, Dict, Any
from .core import Brain
from .motor import Action, MotorCommand
from .sensory import SensoryPercept
from .world import AgentBody
from .types import DevelopmentalStage
class AgentBrain:
    """Agent brain interface connecting NLM brain to world"""
    
    def __init__(self, brain: Brain):
        self.brain = brain
        self.body = AgentBody()
        self.reward_modulation_enabled = False
        self.structural_plasticity_enabled = False
        self.development_enabled = False
        self.curiosity_enabled = False
    
    def initialize(self, world=None) -> bool:
        """Initialize with world"""
        self.body.reset()
        return True
    
    def getSensoryInputSize(self) -> int:
        """Get expected sensory input size"""
        return 1  # Simplified - actual size depends on vision system
    
    def getMotorOutputSize(self) -> int:
        """Get expected motor output size"""
        return 1  # Simplified
    
    def processSensoryInput(self, percept: SensoryPercept):
        """Process sensory percept and inject into brain"""
        if percept:
            # Convert percept to brain input
            strength = percept.get_total_signal_strength()["total_strength"]
            self.brain.injectCurrentToNeurons(
                nlm.NeuroBrain().injectCurrentToNeurons(0.1)
            )
    
    def decodeMotorCommand(self) -> Action:
        """Decode brain motor activity into motor command"""
        action = Action()
        
        # Simple heuristic based on brain state
        if self.brain.getFiringNeuronCount() > 10:
            action.setType(ActionType.MoveForward)
        else:
            action.setType(ActionType.Wait)
        
        return action
    
    def applyRewardModulation(self, reward: float, predicted_reward: float):
        """Apply reward-based neuromodulation"""
        if self.reward_modulation_enabled:
            # Apply reward signal to brain
            pass
    
    def updateDevelopment(self, timestep: float):
        """Update development system"""
        if self.development_enabled:
            # Update developmental stage
            current_stage = self.brain.getDevelopmentalStage()
            if current_stage == DevelopmentalStage.Initial:
                self.brain.setDevelopmentalStage(DevelopmentalStage.CriticalPeriod)
    
    def getDevelopmentalStage(self) -> DevelopmentalStage:
        """Get current developmental stage"""
        return self.brain.getDevelopmentalStage()
    
    def getNeuromodulationLevel(self) -> float:
        """Get current neuromodulation level"""
        return 0.5  # Simplified
    
    def getCuriosityLevel(self) -> float:
        """Get curiosity level"""
        return 0.3  # Simplified
    
    def getNoveltyLevel(self) -> float:
        """Get novelty level"""
        return 0.2  # Simplified
    
    def getPredictionError(self) -> float:
        """Get prediction error"""
        return 0.1  # Simplified
    
    def reset(self):
        """Reset agent for new episode"""
        self.body.reset()
        self.brain.reset()
        self.brain.initialize()
    
    def getBrain(self) -> Brain:
        """Get the underlying brain"""
        return self.brain
    
    def enableRewardModulation(self, enable: bool):
        """Enable/disable reward modulation"""
        self.reward_modulation_enabled = enable
    
    def enableStructuralPlasticity(self, enable: bool):
        """Enable/disable structural plasticity"""
        self.structural_plasticity_enabled = enable
    
    def enableDevelopment(self, enable: bool):
        """Enable/disable development"""
        self.development_enabled = enable
    
    def enableCuriosity(self, enable: bool):
        """Enable/disable curiosity"""
        self.curiosity_enabled = enable
    
    def isRewardModulationEnabled(self) -> bool:
        """Check if reward modulation is enabled"""
        return self.reward_modulation_enabled
    
    def isStructuralPlasticityEnabled(self) -> bool:
        """Check if structural plasticity is enabled"""
        return self.structural_plasticity_enabled
    
    def isDevelopmentEnabled(self) -> bool:
        """Check if development is enabled"""
        return self.development_enabled
    
    def isCuriosityEnabled(self) -> bool:
        """Check if curiosity is enabled"""
        return self.curiosity_enabled
