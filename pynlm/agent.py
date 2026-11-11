# AgentBrain module - Enhanced Python interface for agent-brain integration
"""
Enhanced Python interface for the AgentBrain class that connects the NLM brain to the world.
"""

from typing import List, Optional, Any, Dict
import numpy as np

# Try to import C++ bindings
try:
    from ._pynlm import createAgentBrain as _createAgentBrain
    CPLUSPLUS_AVAILABLE = True
except ImportError:
    CPLUSPLUS_AVAILABLE = False

class AgentBrain:
    """Enhanced Python interface for AgentBrain class.
    
    This class provides a more Pythonic interface for controlling the interaction
    between the NLM brain and the external world, including sensory processing,
    action decoding, and reward modulation.
    """
    
    def __init__(self, brain):
        """Initialize agent brain with NLM brain.
        
        Args:
            brain: NLM Brain instance to control.
        """
        self._brain = brain
        
        if CPLUSPLUS_AVAILABLE and _createAgentBrain is not None:
            self._agent_brain = _createAgentBrain(brain)
        else:
            # Create mock agent brain for testing
            self._agent_brain = MockAgentBrain(brain)
    
    def initialize(self, world):
        """Initialize agent brain with world.
        
        Args:
            world: World instance to interact with.
        """
        return self._agent_brain.initialize(world)
    
    def get_sensory_input_size(self):
        """Get expected sensory input size.
        
        Returns:
            Expected size of sensory input.
        """
        return self._agent_brain.getSensoryInputSize()
    
    def get_motor_output_size(self):
        """Get expected motor output size.
        
        Returns:
            Expected size of motor output.
        """
        return self._agent_brain.getMotorOutputSize()
    
    def process_sensory_input(self, percept):
        """Process sensory percept and inject into brain.
        
        Args:
            percept: Sensory percept data.
        """
        return self._agent_brain.processSensoryInput(percept)
    
    def decode_motor_command(self):
        """Decode brain motor activity into motor command.
        
        Returns:
            Motor command string.
        """
        return self._agent_brain.decodeMotorCommand()
    
    def apply_reward_modulation(self, reward, predicted_reward=None):
        """Apply reward-based neuromodulation.
        
        Args:
            reward: Actual reward received.
            predicted_reward: Expected reward (optional).
        """
        return self._agent_brain.applyRewardModulation(reward, predicted_reward)
    
    def update_development(self, timestep):
        """Update development system.
        
        Args:
            timestep: Current timestep.
        """
        return self._agent_brain.updateDevelopment(timestep)
    
    def get_developmental_stage(self):
        """Get current developmental stage.
        
        Returns:
            Current developmental stage.
        """
        return self._agent_brain.getDevelopmentalStage()
    
    def get_neuromodulation_level(self):
        """Get current neuromodulation level.
        
        Returns:
            Current neuromodulation level.
        """
        return self._agent_brain.getNeuromodulationLevel()
    
    def get_curiosity_level(self):
        """Get curiosity level.
        
        Returns:
            Current curiosity level.
        """
        return self._agent_brain.getCuriosityLevel()
    
    def get_novelty_level(self):
        """Get novelty level.
        
        Returns:
            Current novelty level.
        """
        return self._agent_brain.getNoveltyLevel()
    
    def get_prediction_error(self):
        """Get prediction error.
        
        Returns:
            Current prediction error.
        """
        return self._agent_brain.getPredictionError()
    
    def reset(self):
        """Reset agent for new episode."""
        return self._agent_brain.reset()
    
    def get_brain(self):
        """Get the underlying brain.
        
        Returns:
            Brain instance.
        """
        return self._brain
    
    # Convenience methods for common operations
    
    def act(self, percept):
        """Process sensory input and produce action (one complete step).
        
        Args:
            percept: Sensory percept data.
            
        Returns:
            Action produced by the agent.
        """
        self.process_sensory_input(percept)
        return self.decode_motor_command()
    
    def get_agent_status(self):
        """Get comprehensive agent status.
        
        Returns:
            Dictionary with agent status information.
        """
        status = {
            'developmental_stage': self.get_developmental_stage(),
            'neuromodulation_level': self.get_neuromodulation_level(),
            'curiosity_level': self.get_curiosity_level(),
            'novelty_level': self.get_novelty_level(),
            'prediction_error': self.get_prediction_error(),
            'brain_total_neurons': self._brain.getTotalNeuronCount(),
            'brain_total_spikes': self._brain.getTotalSpikeCount(),
            'brain_active_count': self._brain.getActiveNeuronCount(),
        }
        return status
    
    def enable_systems(self, reward_modulation=True, structural_plasticity=False,
                      development=False, curiosity=False):
        """Enable/disable various agent systems.
        
        Args:
            reward_modulation: Enable reward modulation.
            structural_plasticity: Enable structural plasticity.
            development: Enable development updates.
            curiosity: Enable curiosity system.
        """
        self._agent_brain.enableRewardModulation(reward_modulation)
        self._agent_brain.enableStructuralPlasticity(structural_plasticity)
        self._agent_brain.enableDevelopment(development)
        self._agent_brain.enableCuriosity(curiosity)
    
    def get_enabled_systems(self):
        """Get status of enabled systems.
        
        Returns:
            Dictionary with enabled system status.
        """
        return {
            'reward_modulation': self._agent_brain.isRewardModulationEnabled(),
            'structural_plasticity': self._agent_brain.isStructuralPlasticityEnabled(),
            'development': self._agent_brain.isDevelopmentEnabled(),
            'curiosity': self._agent_brain.isCuriosityEnabled(),
        }
    
    def __repr__(self):
        """String representation of the agent brain."""
        return f"<AgentBrain: {self._brain.getTotalNeuronCount()} neurons, stage={self.get_developmental_stage()}>"


# Mock agent brain class for testing when C++ bindings are not available
class MockAgentBrain:
    def __init__(self, brain):
        self.brain = brain
        self.development_stage = "Adult"
        self.neuromodulation_level = 0.5
        self.curiosity_level = 0.3
        self.novelty_level = 0.2
        self.prediction_error = 0.1
        
    def initialize(self, world):
        return True
        
    def getSensoryInputSize(self):
        return 64 * 64 * 3
        
    def getMotorOutputSize(self):
        return 20
        
    def processSensoryInput(self, percept):
        pass
        
    def decodeMotorCommand(self):
        return "MoveForward"
        
    def applyRewardModulation(self, reward, predicted_reward=None):
        self.neuromodulation_level = min(1.0, self.neuromodulation_level + reward * 0.1)
        
    def updateDevelopment(self, timestep):
        pass
        
    def getDevelopmentalStage(self):
        return self.development_stage
        
    def getNeuromodulationLevel(self):
        return self.neuromodulation_level
        
    def getCuriosityLevel(self):
        return self.curiosity_level
        
    def getNoveltyLevel(self):
        return self.novelty_level
        
    def getPredictionError(self):
        return self.prediction_error
        
    def reset(self):
        self.neuromodulation_level = 0.5
        self.prediction_error = 0.0
        
    def enableRewardModulation(self, enable):
        pass
        
    def enableStructuralPlasticity(self, enable):
        pass
        
    def enableDevelopment(self, enable):
        pass
        
    def enableCuriosity(self, enable):
        pass
        
    def isRewardModulationEnabled(self):
        return True
        
    def isStructuralPlasticityEnabled(self):
        return False
        
    def isDevelopmentEnabled(self):
        return False
        
    def isCuriosityEnabled(self):
        return False


# Re-export for convenience
from . import _pynlm
AgentBrain.__base__ = _pynlm.AgentBrain