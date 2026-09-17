"""
Agent interface classes for NLM neural simulation.
Provides enhanced agent brain interface with learning feature control.
"""

from typing import Dict, List, Optional, Any, Union
import time
class LearningFeature:
    """Learning feature enumeration."""
    
    REWARD_MODULATION = "reward_modulation"
    STRUCTURAL_PLASTICITY = "structural_plasticity"
    DEVELOPMENT = "development"
    CURIOUSITY = "curiosity"
    NOVELTY = "novelty"
    PREDICTION_ERROR = "prediction_error"
    ALL = [REWARD_MODULATION, STRUCTURAL_PLASTICITY, DEVELOPMENT, CURIOUSITY, NOVELTY, PREDICTION_ERROR]
    
    @classmethod
    def get_feature_display_name(cls, feature: str) -> str:
        """Get human-readable display name for feature."""
        names = {
            cls.REWARD_MODULATION: "Reward Modulation",
            cls.STRUCTURAL_PLASTICITY: "Structural Plasticity",
            cls.DEVELOPMENT: "Development",
            cls.CURIOUSITY: "Curiosity",
            cls.NOVELTY: "Novelty Detection",
            cls.PREDICTION_ERROR: "Prediction Error"
        }
        return names.get(feature, feature.replace("_", " ").title())
    
    @classmethod
    def get_feature_color(cls, feature: str) -> str:
        """Get color code for feature visualization."""
        colors = {
            cls.REWARD_MODULATION: "#FF6B6B",
            cls.STRUCTURAL_PLASTICITY: "#4ECDC4",
            cls.DEVELOPMENT: "#45B7D1",
            cls.CURIOUSITY: "#FFA07A",
            cls.NOVELTY: "#98D8C8",
            cls.PREDICTION_ERROR: "#FDCB6E"
        }
        return colors.get(feature, "#95A5A6")
class AgentBrain:
    """Enhanced agent brain interface connecting NLM brain to world.
    
    This class provides a Pythonic interface for the agent brain system,
    including context management, learning feature control, and episode simulation.
    """
    
    def __init__(self, brain=None):
        """Initialize agent brain with optional brain instance.
        
        Args:
            brain: Brain instance to connect to
        """
        self._brain = brain
        self._world = None
        self._is_initialized = False
        self._learning_features = set()
        self._development_stage = "initial"
        self._curiosity_level = 0.0
        self._novelty_level = 0.0
        self._prediction_error = 0.0
        self._neuromodulation_level = 0.0
        self._last_updated = time.time()
        
        # Initialize with brain if provided
        if brain is not None:
            self._brain = brain
    
    def initialize(self, world) -> None:
        """Initialize with world.
        
        Args:
            world: World instance to connect to
        """
        if world is None:
            raise ValueError("World cannot be None")
        
        self._world = world
        self._is_initialized = True
        self._last_updated = time.time()
    
    def getSensoryInputSize(self) -> int:
        """Get expected sensory input size.
        
        Returns:
            Expected sensory input size
        """
        if self._world is None:
            return 0
        
        # Default sensory input size for vision + internal signals
        return 8 * 8 + 4  # 64 vision pixels + 4 internal signals
    
    def getMotorOutputSize(self) -> int:
        """Get expected motor output size.
        
        Returns:
            Expected motor output size
        """
        # Default motor output size
        return 8  # 8 possible actions
    
    def processSensoryInput(self, percept) -> None:
        """Process sensory percept and inject into brain.
        
        Args:
            percept: Sensory percept to process
        """
        if not self._is_initialized:
            raise RuntimeError("AgentBrain must be initialized before processing sensory input")
        
        if percept is None:
            raise ValueError("Percept cannot be None")
        
        # Extract sensory data from percept
        if hasattr(percept, 'getVision'):
            vision_data = percept.getVision()
            # Inject vision data into brain
            if self._brain is not None:
                self._brain.receiveSensoryInput(vision_data)
        
        # Extract internal signals
        if hasattr(percept, 'getInternal'):
            internal_signals = percept.getInternal()
            if self._brain is not None:
                for signal in internal_signals:
                    # Inject internal signals
                    pass
        
        self._last_updated = time.time()
    
    def decodeMotorCommand(self) -> Dict[str, Any]:
        """Decode brain motor activity into motor command.
        
        Returns:
            Motor command dictionary
        """
        if self._brain is None:
            return {"type": "wait", "parameters": {}}
        
        # Simple motor decoding based on brain activity
        firing_neurons = self._brain.getFiringNeuronCount()
        total_neurons = self._brain.getTotalNeuronCount()
        
        if firing_neurons == 0:
            return {"type": "wait", "parameters": {"confidence": 0.0}}
        
        # Determine action based on firing patterns
        firing_rate = firing_neurons / max(1, total_neurons)
        
        if firing_rate > 0.3:
            action_type = "move_forward"
        elif firing_rate > 0.15:
            action_type = "turn_left"
        elif firing_rate > 0.05:
            action_type = "turn_right"
        else:
            action_type = "wait"
        
        return {
            "type": action_type,
            "parameters": {
                "confidence": firing_rate,
                "step": self._brain.getTotalSpikeCount()
            }
        }
    
    def applyRewardModulation(self, reward: float, predicted_reward: float) -> None:
        """Apply reward-based neuromodulation.
        
        Args:
            reward: Actual reward received
            predicted_reward: Predicted reward
        """
        if self._brain is None:
            return
        
        # Calculate prediction error
        prediction_error = abs(reward - predicted_reward)
        self._prediction_error = prediction_error
        
        # Apply neuromodulation based on reward
        self._neuromodulation_level = min(1.0, self._neuromodulation_level + reward * 0.1)
        
        # Update curiosity based on prediction error
        if prediction_error > 0.5:
            self._curiosity_level = min(1.0, self._curiosity_level + prediction_error * 0.2)
        
        # Update development stage
        self._update_development()
        
        self._last_updated = time.time()
    
    def updateDevelopment(self, timestep: float) -> None:
        """Update development system.
        
        Args:
            timestep: Time step for development update
        """
        self._update_development()
        self._last_updated = time.time()
    
    def getDevelopmentalStage(self) -> str:
        """Get current developmental stage.
        
        Returns:
            Current developmental stage
        """
        if not self._is_initialized:
            return "initial"
        
        return self._development_stage
    
    def getNeuromodulationLevel(self) -> float:
        """Get current neuromodulation level.
        
        Returns:
            Current neuromodulation level
        """
        return self._neuromodulation_level
    
    def getCuriosityLevel(self) -> float:
        """Get curiosity level.
        
        Returns:
            Current curiosity level
        """
        return self._curiosity_level
    
    def getNoveltyLevel(self) -> float:
        """Get novelty level.
        
        Returns:
            Current novelty level
        """
        return self._novelty_level
    
    def getPredictionError(self) -> float:
        """Get prediction error.
        
        Returns:
            Current prediction error
        """
        return self._prediction_error
    
    def reset(self) -> None:
        """Reset agent for new episode."""
        self._world = None
        self._is_initialized = False
        self._learning_features.clear()
        self._development_stage = "initial"
        self._curiosity_level = 0.0
        self._novelty_level = 0.0
        self._prediction_error = 0.0
        self._neuromodulation_level = 0.0
        self._last_updated = time.time()
    
    def getBrain(self):
        """Get the underlying brain.
        
        Returns:
            Underlying brain instance
        """
        return self._brain
    
    # Learning feature control methods
    def enableRewardModulation(self, enable: bool) -> None:
        """Enable or disable reward modulation.
        
        Args:
            enable: True to enable, False to disable
        """
        if enable:
            self._learning_features.add("reward_modulation")
        else:
            self._learning_features.discard("reward_modulation")
    
    def enableStructuralPlasticity(self, enable: bool) -> None:
        """Enable or disable structural plasticity.
        
        Args:
            enable: True to enable, False to disable
        """
        if enable:
            self._learning_features.add("structural_plasticity")
        else:
            self._learning_features.discard("structural_plasticity")
    
    def enableDevelopment(self, enable: bool) -> None:
        """Enable or disable development.
        
        Args:
            enable: True to enable, False to disable
        """
        if enable:
            self._learning_features.add("development")
        else:
            self._learning_features.discard("development")
    
    def enableCuriosity(self, enable: bool) -> None:
        """Enable or disable curiosity.
        
        Args:
            enable: True to enable, False to disable
        """
        if enable:
            self._learning_features.add("curiosity")
        else:
            self._learning_features.discard("curiosity")
    
    def enableLearning(self, features: List[str] = None) -> None:
        """Enable learning features.
        
        Args:
            features: List of features to enable (default: all)
        """
        if features is None:
            # Enable all features
            self._learning_features.update(LearningFeature.ALL)
        else:
            # Enable specified features
            for feature in features:
                if feature in LearningFeature.ALL:
                    self._learning_features.add(feature)
    
    def disableLearning(self) -> None:
        """Disable all learning features."""
        self._learning_features.clear()
    
    def isRewardModulationEnabled(self) -> bool:
        """Check if reward modulation is enabled.
        
        Returns:
            True if reward modulation enabled, False otherwise
        """
        return "reward_modulation" in self._learning_features
    
    def isStructuralPlasticityEnabled(self) -> bool:
        """Check if structural plasticity is enabled.
        
        Returns:
            True if structural plasticity enabled, False otherwise
        """
        return "structural_plasticity" in self._learning_features
    
    def isDevelopmentEnabled(self) -> bool:
        """Check if development is enabled.
        
        Returns:
            True if development enabled, False otherwise
        """
        return "development" in self._learning_features
    
    def isCuriosityEnabled(self) -> bool:
        """Check if curiosity is enabled.
        
        Returns:
            True if curiosity enabled, False otherwise
        """
        return "curiosity" in self._learning_features
    
    def getLearningStatus(self) -> Dict[str, bool]:
        """Get learning feature status.
        
        Returns:
            Dictionary with learning feature status
        """
        return {
            "reward_modulation": self.isRewardModulationEnabled(),
            "structural_plasticity": self.isStructuralPlasticityEnabled(),
            "development": self.isDevelopmentEnabled(),
            "curiosity": self.isCuriosityEnabled(),
            "novelty": "novelty" in self._learning_features,
            "prediction_error": "prediction_error" in self._learning_features
        }
    
    def configure_learning(self, reward: bool = True, structural: bool = True, 
                          development: bool = True, curiosity: bool = True) -> None:
        """Configure learning features.
        
        Args:
            reward: Enable reward modulation
            structural: Enable structural plasticity
            development: Enable development
            curiosity: Enable curiosity
        """
        self.enableRewardModulation(reward)
        self.enableStructuralPlasticity(structural)
        self.enableDevelopment(development)
        self.enableCuriosity(curiosity)
    
    def runEpisode(self, world, steps: int) -> "SimulationStats":
        """Run episode simulation.
        
        Args:
            world: World instance to simulate in
            steps: Number of steps to run
            
        Returns:
            Episode statistics
        """
        if world is None:
            raise ValueError("World cannot be None")
        
        return run_episode(self, world, steps)
    
    # Internal methods
    def _update_development(self) -> None:
        """Update development stage based on learning features."""
        if not self._is_initialized:
            return
        
        # Simple development stage progression based on learning feature activation
        active_features = len(self._learning_features)
        
        if active_features == 0:
            self._development_stage = "initial"
        elif active_features <= 2:
            self._development_stage = "critical_period"
        elif active_features <= 4:
            self._development_stage = "maturation"
        else:
            self._development_stage = "adult"
    
    def __repr__(self) -> str:
        return f"<AgentBrain: world={'connected' if self._world else 'disconnected'}, features={len(self._learning_features)}, stage={self._development_stage}>"

    def __str__(self) -> str:
        return self.__repr__()
# Global functions for backward compatibility
def createAgentBrain(brain) -> AgentBrain:
    """Create a new agent brain interface.
    
    Args:
        brain: Brain instance to connect to
        
    Returns:
        AgentBrain instance
    """
    return AgentBrain(brain)

# Import needed classes
from .core import run_episode, SimulationStats

__all__ = [
    "AgentBrain",
    "LearningFeature",
    "createAgentBrain"
]
