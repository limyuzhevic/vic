# Agent interface for NLM
import numpy as np
from typing import Optional, List, Any

from .brain import Brain
from .world import SimpleWorld, WorldObject, WorldObjectType
from .sensory import SensoryPercept
from .motor import Action, ActionType

class AgentBody:
    """Agent body state information."""
    
    def __init__(self):
        """Initialize empty agent body."""
        self.x = 0.0
        self.y = 0.0
        self.orientation = 0.0
        self.velocityX = 0.0
        self.velocityY = 0.0
        self.angularVelocity = 0.0
        self.energy = 0.0
        self.health = 0.0
        self.age = 0.0
        self.isMoving = False
        self.isTurning = False
        self.lastActionTime = 0.0
    
    def reset(self) -> None:
        """Reset body state."""
        self.x = 0.0
        self.y = 0.0
        self.orientation = 0.0
        self.velocityX = 0.0
        self.velocityY = 0.0
        self.angularVelocity = 0.0
        self.energy = 0.0
        self.health = 0.0
        self.age = 0.0
        self.isMoving = False
        self.isTurning = False
        self.lastActionTime = 0.0

class ActionResult:
    """Result of applying an action in the world."""
    
    def __init__(self, reward: float = 0.0, success: bool = False, message: str = ""):
        """Create action result."""
        self.reward = reward
        self.success = success
        self.message = message

class Agent:
    """Agent that interacts with the world using brain control."""
    
    def __init__(self, agent_brain, world: SimpleWorld):
        """Initialize agent with brain interface and world."""
        self.agent_brain = agent_brain
        self.world = world
        self.body = AgentBody()
        
        # Subsystem enable flags
        self._reward_modulation_enabled = False
        self._structural_plasticity_enabled = False
        self._development_enabled = False
        self._curiosity_enabled = False
        
        # Internal state
        self._current_step = 0
        self._last_action = None
        self._action_history = []
    
    def initialize(self, world: SimpleWorld) -> None:
        """Initialize agent with world."""
        self.world = world
        self.agent_brain.initialize(world)
        self.body.reset()
        self._current_step = 0
        
        # Synchronize body state with world
        world_body = world.getAgentBody()
        self.body.x = world_body.x
        self.body.y = world_body.y
        self.body.orientation = world_body.orientation
        self.body.velocityX = world_body.velocityX
        self.body.velocityY = world_body.velocityY
        self.body.angularVelocity = world_body.angularVelocity
        self.body.energy = world_body.energy
        self.body.health = world_body.health
        self.body.age = world_body.age
        self.body.isMoving = world_body.isMoving
        self.body.isTurning = world_body.isTurning
        self.body.lastActionTime = world_body.lastActionTime
    
    def processSensoryInput(self, percept: SensoryPercept) -> None:
        """Process sensory percept and inject into brain."""
        self.agent_brain.processSensoryInput(percept)
    
    def decodeMotorCommand(self) -> Action:
        """Decode brain activity into motor command."""
        self._last_action = self.agent_brain.decodeMotorCommand()
        self._action_history.append({
            'step': self._current_step,
            'action': self._last_action,
            'time': self.world.getSimulationTime()
        })
        return self._last_action
    
    def applyMotorCommand(self, action: Action) -> ActionResult:
        """Apply motor command to world."""
        return self.world.applyMotorCommand(action, self.world.getSimulationTime())
    
    def applyRewardModulation(self, reward: float, predicted_reward: float) -> None:
        """Apply reward-based neuromodulation."""
        if self._reward_modulation_enabled:
            self.agent_brain.applyRewardModulation(reward, predicted_reward)
    
    def updateDevelopment(self, timestep: float) -> None:
        """Update development system."""
        if self._development_enabled:
            self.agent_brain.updateDevelopment(timestep)
    
    def getDevelopmentalStage(self):
        """Get current developmental stage."""
        return self.agent_brain.getDevelopmentalStage()
    
    def getNeuromodulationLevel(self) -> float:
        """Get current neuromodulation level."""
        return self.agent_brain.getNeuromodulationLevel()
    
    def getCuriosityLevel(self) -> float:
        """Get curiosity level."""
        return self.agent_brain.getCuriosityLevel()
    
    def getNoveltyLevel(self) -> float:
        """Get novelty level."""
        return self.agent_brain.getNoveltyLevel()
    
    def getPredictionError(self) -> float:
        """Get prediction error."""
        return self.agent_brain.getPredictionError()
    
    # Subsystem control
    def enableRewardModulation(self, enable: bool) -> None:
        """Enable or disable reward modulation."""
        self._reward_modulation_enabled = enable
    
    def enableStructuralPlasticity(self, enable: bool) -> None:
        """Enable or disable structural plasticity."""
        self._structural_plasticity_enabled = enable
    
    def enableDevelopment(self, enable: bool) -> None:
        """Enable or disable development."""
        self._development_enabled = enable
    
    def enableCuriosity(self, enable: bool) -> None:
        """Enable or disable curiosity."""
        self._curiosity_enabled = enable
    
    # Subsystem status
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
    
    def getBrain(self) -> Brain:
        """Get the underlying brain."""
        return self.agent_brain.getBrain()
    
    def reset(self) -> None:
        """Reset agent for new episode."""
        self.agent_brain.reset()
        self.body.reset()
        self._action_history.clear()
    
    def getActionHistory(self) -> List[Dict[str, Any]]:
        """Get history of actions taken."""
        return self._action_history.copy()
    
    def getLastAction(self) -> Optional[Action]:
        """Get last action taken."""
        return self._last_action
    
    def getStats(self) -> Dict[str, Any]:
        """Get comprehensive agent statistics."""
        brain = self.getBrain()
        return {
            'step': self._current_step,
            'action_count': len(self._action_history),
            'brain_neurons': brain.getTotalNeuronCount(),
            'brain_synapses': brain.getTotalSynapseCount(),
            'firing_neurons': brain.getFiringNeuronCount(),
            'avg_firing_rate': brain.getAverageFiringRate(),
            'total_spikes': brain.getTotalSpikeCount(),
            'e_i_ratio': brain.getExcitationInhibitionRatio(),
            'dev_stage': brain.getDevelopmentalStage().name,
            'reward_mod_enabled': self._reward_modulation_enabled,
            'structural_plastic_enabled': self._structural_plasticity_enabled,
            'development_enabled': self._development_enabled,
            'curiosity_enabled': self._curiosity_enabled,
            'neuromodulation_level': self.getNeuromodulationLevel(),
            'curiosity_level': self.getCuriosityLevel(),
            'novelty_level': self.getNoveltyLevel(),
            'prediction_error': self.getPredictionError(),
        }
    
    def __repr__(self) -> str:
        """String representation of agent."""
        return f"<Agent dev_stage={self.getDevelopmentalStage().name} actions={len(self._action_history)}>"
    
    def get_performance_rating(self, brain: Brain, world: SimpleWorld) -> str:
        """Get performance rating based on brain and world state."""
        from .utils import get_performance_rating
        return get_performance_rating(brain, world)
