"""
Agent module for NLM

This module provides the AgentBrain class for connecting NLM brain to world.
"""

from .brain import Brain
from .world import SimpleWorld
from .sensory import SensoryPercept
from .motor import Action
from .core import LogLevel
from typing import List

class AgentBrain:
    def __init__(self, brain: Brain):
        self._brain = brain
        self._world = None
        self._reward_modulation_enabled = False
        self._structural_plasticity_enabled = False
        self._development_enabled = False
        self._curiosity_enabled = False
        self._development_stage = 0

    def initialize(self, world: SimpleWorld):
        """Initialize agent with world"""
        self._world = world

    def getSensoryInputSize(self) -> int:
        """Get expected sensory input size"""
        if self._world:
            return self._world.getVisionWidth() * self._world.getVisionHeight()
        return 0

    def getMotorOutputSize(self) -> int:
        """Get expected motor output size"""
        return 6  # Movement actions

    def processSensoryInput(self, percept: SensoryPercept):
        """Process sensory percept and inject into brain"""
        try:
            if not percept or not self._world:
                return
            # Extract vision data and inject into brain
            vision = percept.getVision()
            # Simplified injection - in real implementation this would be more complex
            pass
        except Exception:
            pass

    def decodeMotorCommand(self) -> Action:
        """Decode brain motor activity into motor command"""
        try:
            # Simplified motor command production
            action = Action()
            action.setType(0)  # MoveForward
            return action
        except Exception:
            # Return a default action if something goes wrong
            action = Action()
            action.setType(0)  # MoveForward
            return action

    def applyRewardModulation(self, reward: float, predicted_reward: float):
        """Apply reward-based neuromodulation"""
        if self._reward_modulation_enabled:
            # Simplified reward modulation
            pass

    def updateDevelopment(self, timestep: float):
        """Update development system"""
        if self._development_enabled:
            self._development_stage += 1

    def getDevelopmentalStage(self):
        """Get current developmental stage"""
        from .core import DevelopmentalStage
        if self._development_stage < 1:
            return DevelopmentalStage.Initial
        elif self._development_stage < 3:
            return DevelopmentalStage.CriticalPeriod
        elif self._development_stage < 5:
            return DevelopmentalStage.Maturation
        elif self._development_stage < 10:
            return DevelopmentalStage.Adult
        else:
            return DevelopmentalStage.Aging

    def getNeuromodulationLevel(self) -> float:
        return 0.5  # Simplified

    def getCuriosityLevel(self) -> float:
        return 0.3 if self._curiosity_enabled else 0.0

    def getNoveltyLevel(self) -> float:
        return 0.2

    def getPredictionError(self) -> float:
        return 0.0

    def reset(self):
        """Reset agent for new episode"""
        self._development_stage = 0

    def getBrain(self) -> Brain:
        return self._brain

    def enableRewardModulation(self, enable: bool):
        self._reward_modulation_enabled = enable

    def enableStructuralPlasticity(self, enable: bool):
        self._structural_plasticity_enabled = enable

    def enableDevelopment(self, enable: bool):
        self._development_enabled = enable

    def enableCuriosity(self, enable: bool):
        self._curiosity_enabled = enable

    def isRewardModulationEnabled(self) -> bool:
        return self._reward_modulation_enabled

    def isStructuralPlasticityEnabled(self) -> bool:
        return self._structural_plasticity_enabled

    def isDevelopmentEnabled(self) -> bool:
        return self._development_enabled

    def isCuriosityEnabled(self) -> bool:
        return self._curiosity_enabled

    def __repr__(self):
        return f"AgentBrain(reward_modulation={self._reward_modulation_enabled})"

# Convenience function

def createAgentBrain(brain: Brain) -> AgentBrain:
    """Create a new agent brain interface"""
    return AgentBrain(brain)

__all__ = [
    'AgentBrain',
    'createAgentBrain',
]