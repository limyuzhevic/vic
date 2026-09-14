# NLM Agent Python Module

"""
Agent module for NLM Python interface
"""

from . import __init__ as nlm_init
from typing import List, Optional, Union, Any
from pathlib import Path

# Re-export from main module
Brain = nlm_init.Brain
Config = nlm_init.Config
createBrain = nlm_init.createBrain
createDefaultConfig = nlm_init.createDefaultConfig
SensoryPercept = nlm_init.SensoryPercept
Vision = nlm_init.Vision
Audio = nlm_init.Audio
AgentBody = nlm_init.AgentBody
ActionResult = nlm_init.ActionResult
Action = nlm_init.Action
ActionType = nlm_init.ActionType
MotorCommand = nlm_init.MotorCommand
WorldObject = nlm_init.WorldObject
WorldObjectType = nlm_init.WorldObjectType

# AgentBrain interface (to be implemented)
class AgentBrain:
    """
    Agent brain interface connecting NLM brain to world
    
    This class provides a bridge between the NLM brain and the external
    world, handling sensory input processing and motor command decoding.
    """
    
    def __init__(self, brain: Brain):
        """Create a new agent brain interface"""
        self._brain = brain
        # Initialize C++ agent brain (to be implemented)
        self._cpp_agent = None
    
    def initialize(self, world: Any) -> None:
        """Initialize agent with world"""
        # To be implemented
        pass
    
    def getSensoryInputSize(self) -> int:
        """Get expected sensory input size"""
        # To be implemented
        return 0
    
    def getMotorOutputSize(self) -> int:
        """Get expected motor output size"""
        # To be implemented
        return 0
    
    def processSensoryInput(self, percept: SensoryPercept) -> None:
        """Process sensory percept and inject into brain"""
        # Pass to brain
        self._brain.receiveSensoryInput(percept)
    
    def decodeMotorCommand(self) -> Action:
        """Decode brain motor activity into motor command"""
        # For now, return a default action
        # TODO: Implement actual motor decoding
        return Action(ActionType.Wait)
    
    def applyRewardModulation(self, reward: float, predictedReward: float) -> None:
        """Apply reward-based neuromodulation"""
        # To be implemented
        pass
    
    def updateDevelopment(self, timestep: float) -> None:
        """Update development system"""
        # To be implemented
        pass
    
    def getDevelopmentalStage(self) -> Any:
        """Get current developmental stage"""
        return self._brain.getDevelopmentalStage()
    
    def getNeuromodulationLevel(self) -> float:
        """Get current neuromodulation level"""
        # To be implemented
        return 0.0
    
    def getCuriosityLevel(self) -> float:
        """Get curiosity level"""
        # To be implemented
        return 0.0
    
    def getNoveltyLevel(self) -> float:
        """Get novelty level"""
        # To be implemented
        return 0.0
    
    def getPredictionError(self) -> float:
        """Get prediction error"""
        # To be implemented
        return 0.0
    
    def reset(self) -> None:
        """Reset agent for new episode"""
        self._brain.reset()
    
    def getBrain(self) -> Brain:
        """Get the underlying brain"""
        return self._brain
    
    # Subsystem enable/disable
    def enableRewardModulation(self, enable: bool) -> None:
        """Enable/disable reward modulation"""
        # To be implemented
        pass
    
    def enableStructuralPlasticity(self, enable: bool) -> None:
        """Enable/disable structural plasticity"""
        # To be implemented
        pass
    
    def enableDevelopment(self, enable: bool) -> None:
        """Enable/disable development"""
        # To be implemented
        pass
    
    def enableCuriosity(self, enable: bool) -> None:
        """Enable/disable curiosity"""
        # To be implemented
        pass
    
    def isRewardModulationEnabled(self) -> bool:
        """Check if reward modulation is enabled"""
        # To be implemented
        return False
    
    def isStructuralPlasticityEnabled(self) -> bool:
        """Check if structural plasticity is enabled"""
        # To be implemented
        return False
    
    def isDevelopmentEnabled(self) -> bool:
        """Check if development is enabled"""
        # To be implemented
        return False
    
    def isCuriosityEnabled(self) -> bool:
        """Check if curiosity is enabled"""
        # To be implemented
        return False


def createAgentBrain(brain: Brain) -> AgentBrain:
    """Create a new agent brain interface"""
    return AgentBrain(brain)


# Agent utilities
def createReadyAgent(config: Optional[Config] = None, world: Optional[Any] = None) -> tuple:
    """
    Create a ready-to-use agent with brain and world
    
    This is a convenience function that creates all the necessary components
    for a complete agent setup.
    
    Args:
        config: Optional configuration
        world: Optional world object
        
    Returns:
        Tuple of (agent, brain, world)
    """
    # Create brain
    brain = createBrain(config) if config else createBrain()
    brain.initialize()
    
    # Create agent
    agent = createAgentBrain(brain)
    
    # Initialize world if provided
    if world:
        agent.initialize(world)
    
    return agent, brain, world


def runCompleteEpisode(agent: AgentBrain, world: Any, numSteps: int) -> dict:
    """
    Run a complete agent episode
    
    This function runs a complete simulation episode with the given
    agent and world, collecting metrics and returning results.
    
    Args:
        agent: Agent brain interface
        world: World object
        numSteps: Number of steps to run
        
    Returns:
        Dictionary of results including spikes, rewards, etc.
    """
    results = {
        "spikes": [],
        "firing_rates": [],
        "rewards": [],
        "curiosity_levels": [],
        "novelty_levels": [],
        "actions": [],
        "steps": list(range(numSteps))
    }
    
    for step in range(numSteps):
        # Update world
        world.update(0.1)
        
        # Get sensory input
        percept = world.getSensoryPercept()
        
        # Process sensory input
        agent.processSensoryInput(percept)
        
        # Run brain step
        agent.getBrain().step(step)
        
        # Get and apply action
        action = agent.decodeMotorCommand()
        results["actions"].append(action)
        
        # Apply action to world
        result = world.applyMotorCommand(action, world.getSimulationTime())
        
        # Collect metrics
        brain = agent.getBrain()
        results["spikes"].append(brain.getTotalSpikeCount())
        results["firing_rates"].append(brain.getAverageFiringRate())
        
        # Get neuromodulation levels
        results["curiosity_levels"].append(agent.getCuriosityLevel())
        results["novelty_levels"].append(agent.getNoveltyLevel())
        
        # Get reward from action result
        if result:
            results["rewards"].append(result.reward)
        else:
            results["rewards"].append(0.0)
        
        # Apply reward modulation
        reward = results["rewards"][-1] if results["rewards"] else 0.0
        predicted_reward = 0.0  # TODO: Implement prediction
        agent.applyRewardModulation(reward, predicted_reward)
        
        # Update development
        agent.updateDevelopment(0.1)
    
    return results


# Agent factory and presets
AGENT_PRESETS = {
    "explorer": {
        "curiosity_enable": True,
        "novelty_enable": True,
        "structural_plasticity_enable": False,
        "development_enable": True,
        "reward_modulation_enable": True,
    },
    "builder": {
        "curiosity_enable": False,
        "novelty_enable": False,
        "structural_plasticity_enable": True,
        "development_enable": True,
        "reward_modulation_enable": True,
    },
    "learner": {
        "curiosity_enable": True,
        "novelty_enable": True,
        "structural_plasticity_enable": True,
        "development_enable": True,
        "reward_modulation_enable": True,
    },
    "survivor": {
        "curiosity_enable": False,
        "novelty_enable": False,
        "structural_plasticity_enable": False,
        "development_enable": False,
        "reward_modulation_enable": True,
    }
}


def createAgentWithPreset(preset: str, config: Optional[Config] = None) -> tuple:
    """
    Create an agent with a predefined preset behavior
    
    Args:
        preset: One of "explorer", "builder", "learner", "survivor"
        config: Optional configuration
        
    Returns:
        Tuple of (agent, brain)
    """
    if preset not in AGENT_PRESETS:
        raise ValueError(f"Unknown preset: {preset}")
    
    # Create brain
    brain = createBrain(config) if config else createBrain()
    brain.initialize()
    
    # Create agent
    agent = createAgentBrain(brain)
    
    # Apply preset
    preset_config = AGENT_PRESETS[preset]
    agent.enableRewardModulation(preset_config["reward_modulation_enable"])
    agent.enableStructuralPlasticity(preset_config["structural_plasticity_enable"])
    agent.enableDevelopment(preset_config["development_enable"])
    agent.enableCuriosity(preset_config["curiosity_enable"])
    
    return agent, brain


__all__ = [
    "AgentBrain", "createAgentBrain",
    "createReadyAgent", "runCompleteEpisode",
    "createAgentWithPreset", "AGENT_PRESETS"
]
