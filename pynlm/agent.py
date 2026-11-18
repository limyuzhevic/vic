'''AgentBrain class for pynlm'''

from __future__ import annotations

from typing import Dict, List, Optional, Any, Union
import time

from .core.types import NeuronId, RegionId, SimulationStep, Timestamp
from .core.enums import (
    NeuronType, SynapseType, DevelopmentalStage, FiringState,
    ActionType, MotorCommand
)
from .brain import Brain
from .world import SimpleWorld
from .agent.AgentBody import AgentBody
from .agent.SensoryPercept import SensoryPercept
from .agent.SensorySignals import (
    Vision, Audio, InternalSignals, SensoryInput
)
from .motor.Action import Action
from .motor.MotorCommand import MotorCommand
from .motor.WorldObject import WorldObject
from .motor.ActionResult import ActionResult


class AgentBrain:
    """Agent brain interface connecting NLM brain to world.
    
    The AgentBrain class serves as the bridge between the internal neural
    computation (Brain) and the external environment (World). It processes
    sensory input from the world, updates the brain, and converts neural
    activity into motor commands for the agent.
    """
    
    def __init__(self, brain: Brain):
        """Initialize an AgentBrain instance.
        
        Args:
            brain: The NLM brain that will be controlled by this agent.
        """
        self.brain: Brain = brain
        self.world: Optional[SimpleWorld] = None
        self.body: Optional[AgentBody] = None
        
        # Agent state
        self.initialized: bool = False
        self.development_enabled: bool = False
        self.reward_modulation_enabled: bool = False
        self.structural_plasticity_enabled: bool = False
        self.curiosity_enabled: bool = False
        
        # Communication state
        self.last_percept: Optional[SensoryPercept] = None
        self.last_action: Optional[Action] = None
        self.last_reward: float = 0.0
        self.last_predicted_reward: float = 0.0
        
        # Learning state
        self.prediction_error: float = 0.0
        self.neuromodulation_level: float = 0.0
        self.curiosity_level: float = 0.0
        self.novelty_level: float = 0.0
        
        # Statistics
        self.action_count: int = 0
        self.step_count: SimulationStep = 0
        
        # Configuration
        self.sensory_input_size: int = 0
        self.motor_output_size: int = 0
        
        # Callbacks
        self.callbacks: Dict[str, Any] = {}
    
    def initialize(self, world: SimpleWorld):
        """Initialize the agent with a world.
        
        Args:
            world: The world to initialize the agent in.
        """
        self.world = world
        self.body = world.getAgentBody()
        
        # Initialize from brain configuration if available
        if self.brain:
            self.development_enabled = self.brain.development_enabled
            self.reward_modulation_enabled = self.brain.reward_modulation_enabled
            self.structural_plasticity_enabled = self.brain.structural_plasticity_enabled
            self.curiosity_enabled = self.brain.curiosity_enabled
        
        # Update developmental stage from brain
        if self.brain:
            self.updateDevelopment(0)
        
        self.initialized = True
        self.callbacks.get('on_initialize', lambda: None)()
    
    def getSensoryInputSize(self) -> int:
        """Get expected sensory input size.
        
        Returns:
            Expected size of sensory input vector.
        """
        if self.sensory_input_size == 0:
            # Calculate based on available sensory modalities
            size = 0
            
            # Vision
            if self.world and self.world.getSensoryPercept():
                vp = self.world.getSensoryPercept()
                size += vp.getVisionWidth() * vp.getVisionHeight() * 3  # RGB
            
            # Audio
            size += 128  # Audio samples
            
            # Internal signals
            size += 16  # Internal signal channels
            
            self.sensory_input_size = size
        
        return self.sensory_input_size
    
    def getMotorOutputSize(self) -> int:
        """Get expected motor output size.
        
        Returns:
            Expected size of motor output vector.
        """
        if self.motor_output_size == 0:
            # Calculate based on available motor commands
            size = 0
            
            # Basic motor commands
            size += 8  # Movement commands
            size += 2  # Look commands
            size += 1  # Interaction
            
            # Continuous controls
            size += 6  # Speed, rotation, etc.
            
            self.motor_output_size = size
        
        return self.motor_output_size
    
    def processSensoryInput(self, percept: SensoryPercept):
        """Process sensory percept and inject into brain.
        
        Args:
            percept: Sensory percept from the world.
        """
        self.last_percept = percept
        self.step_count += 1
        
        # Convert percept to neural input
        if self.brain:
            # Convert different sensory modalities to neural signals
            self._convert_vision_to_neural_input(percept)
            self._convert_audio_to_neural_input(percept)
            self._convert_internal_to_neural_input(percept)
            
            # Update developmental state
            self.updateDevelopment(1)
            
            # Send processed input to brain
            self.brain.receiveSensoryInput(percept)
        
        self.callbacks.get('on_sensory_processed', lambda: None)()
    
    def decodeMotorCommand(self) -> Union[Action, MotorCommand]:
        """Decode brain motor activity into motor command.
        
        Returns:
            Action or MotorCommand decoded from brain activity.
        """
        if not self.brain:
            return Action(ActionType.WAIT)
        
        # Get brain action
        brain_action = self.brain.produceAction()
        
        # Convert to appropriate action type
        if 'type' in brain_action and brain_action['type'] in ['MOVE_FORWARD', 'MOVE_BACKWARD',
                                                              'MOVE_LEFT', 'MOVE_RIGHT',
                                                              'TURN_LEFT', 'TURN_RIGHT',
                                                              'LOOK', 'INTERACT', 'EAT',
                                                              'DRINK', 'REST', 'WAIT',
                                                              'CUSTOM']:
            # Create Action from brain output
            action_type_str = brain_action['type']
            try:
                action_type = ActionType[action_type_str]
            except (KeyError, AttributeError):
                action_type = ActionType.WAIT
            
            action = Action(action_type)
            self.last_action = action
            
            # Apply reward modulation if enabled
            if self.reward_modulation_enabled and self.last_reward > 0:
                action = self._apply_reward_modulation(action)
            
            return action
        
        # Default to MotorCommand
        # This would be determined by brain output in a real implementation
        return MotorCommand.WAIT
    
    def applyMotorCommand(self):
        """Apply decoded motor command to the world."""
        if not self.world or not self.body:
            return
        
        # Get motor command
        motor_cmd = self.decodeMotorCommand()
        
        # Apply to world
        self.world.applyMotorCommand(motor_cmd, self.step_count)
        
        # Update last action
        self.last_action = motor_cmd if isinstance(motor_cmd, Action) else Action(ActionType.CUSTOM)
        
        self.action_count += 1
        self.callbacks.get('on_action_applied', lambda: None)()
    
    def applyRewardModulation(self, reward: float, predicted_reward: float):
        """Apply reward-based neuromodulation.
        
        Args:
            reward: Actual reward received.
            predicted_reward: Predicted reward from brain.
        """
        self.last_reward = reward
        self.last_predicted_reward = predicted_reward
        
        # Calculate prediction error
        self.prediction_error = abs(reward - predicted_reward)
        
        # Update neuromodulation level
        if self.reward_modulation_enabled:
            self.neuromodulation_level = min(1.0, self.prediction_error * 0.1)
        
        # Apply to brain
        if self.brain:
            # In a real implementation, this would modulate synaptic weights
            pass
        
        self.callbacks.get('on_reward', lambda: None)()
    
    def updateDevelopment(self, timestep: SimulationStep):
        """Update development system.
        
        Args:
            timestep: Time step for development update.
        """
        if not self.brain or not self.development_enabled:
            return
        
        # Update developmental stage
        current_stage = self.brain.getDevelopmentalStage()
        
        if current_stage == DevelopmentalStage.INITIAL:
            # Critical period development
            if self.step_count > 100:
                self.brain.setDevelopmentalStage(DevelopmentalStage.CRITICAL_PERIOD)
        
        elif current_stage == DevelopmentalStage.CRITICAL_PERIOD:
            # Maturation
            if self.step_count > 500:
                self.brain.setDevelopmentalStage(DevelopmentalStage.MATURATION)
        
        elif current_stage == DevelopmentalStage.MATURATION:
            # Adult development
            if self.step_count > 1000:
                self.brain.setDevelopmentalStage(DevelopmentalStage.ADULT)
        
        elif current_stage == DevelopmentalStage.ADULT:
            # Aging (if enabled)
            pass
        
        # Update curiosity level
        if self.curiosity_enabled:
            # Curioisty increases with exploration
            self.curiosity_level = min(1.0, self.action_count * 0.001)
            
            # Novelty based on prediction error
            self.novelty_level = min(1.0, self.prediction_error * 2.0)
        
        self.callbacks.get('on_development', lambda: None)()
    
    def getDevelopmentalStage(self) -> DevelopmentalStage:
        """Get current developmental stage.
        
        Returns:
            Current developmental stage.
        """
        return self.brain.getDevelopmentalStage() if self.brain else DevelopmentalStage.ADULT
    
    def getNeuromodulationLevel(self) -> float:
        """Get current neuromodulation level.
        
        Returns:
            Current neuromodulation level.
        """
        return self.neuromodulation_level
    
    def getCuriosityLevel(self) -> float:
        """Get curiosity level.
        
        Returns:
            Current curiosity level.
        """
        return self.curiosity_level
    
    def getNoveltyLevel(self) -> float:
        """Get novelty level.
        
        Returns:
            Current novelty level.
        """
        return self.novelty_level
    
    def getPredictionError(self) -> float:
        """Get prediction error.
        
        Returns:
            Current prediction error.
        """
        return self.prediction_error
    
    def reset(self):
        """Reset agent for new episode."""
        self.step_count = 0
        self.action_count = 0
        self.last_percept = None
        self.last_action = None
        self.last_reward = 0.0
        self.last_predicted_reward = 0.0
        self.prediction_error = 0.0
        self.neuromodulation_level = 0.0
        self.curiosity_level = 0.0
        self.novelty_level = 0.0
        
        if self.brain:
            self.brain.reset()
        
        if self.world:
            self.world.reset()
        
        self.callbacks.get('on_reset', lambda: None)()
    
    def getNeuromodulationLevel(self) -> float:
        """Get current neuromodulation level.
        
        Returns:
            Current neuromodulation level.
        """
        return self.neuromodulation_level
    
    def getCuriosityLevel(self) -> float:
        """Get curiosity level.
        
        Returns:
            Current curiosity level.
        """
        return self.curiosity_level
    
    def getNoveltyLevel(self) -> float:
        """Get novelty level.
        
        Returns:
            Current novelty level.
        """
        return self.novelty_level
    
    def getPredictionError(self) -> float:
        """Get prediction error.
        
        Returns:
            Current prediction error.
        """
        return self.prediction_error
    
    def enableStructuralPlasticity(self, enable: bool):
        """Enable or disable structural plasticity.
        
        Args:
            enable: Whether to enable structural plasticity.
        """
        self.structural_plasticity_enabled = enable
        self.callbacks.get('on_plasticity_changed', lambda: None)()
    
    def enableDevelopment(self, enable: bool):
        """Enable or disable development.
        
        Args:
            enable: Whether to enable development.
        """
        self.development_enabled = enable
        self.callbacks.get('on_development_changed', lambda: None)()
    
    def enableCuriosity(self, enable: bool):
        """Enable or disable curiosity.
        
        Args:
            enable: Whether to enable curiosity.
        """
        self.curiosity_enabled = enable
        self.callbacks.get('on_curiosity_changed', lambda: None)()
    
    # Getter methods for subsystem status (for backward compatibility with tests)
    def isRewardModulationEnabled(self) -> bool:
        """Check if reward modulation is enabled.
        
        Returns:
            True if reward modulation is enabled, False otherwise.
        """
        return self.reward_modulation_enabled
    
    def isStructuralPlasticityEnabled(self) -> bool:
        """Check if structural plasticity is enabled.
        
        Returns:
            True if structural plasticity is enabled, False otherwise.
        """
        return self.structural_plasticity_enabled
    
    def isDevelopmentEnabled(self) -> bool:
        """Check if development is enabled.
        
        Returns:
            True if development is enabled, False otherwise.
        """
        return self.development_enabled
    
    def isCuriosityEnabled(self) -> bool:
        """Check if curiosity is enabled.
        
        Returns:
            True if curiosity is enabled, False otherwise.
        """
        return self.curiosity_enabled
    
    def addCallback(self, event: str, callback):
        """Add a callback for a specific event.
        
        Args:
            event: Event name to listen for.
            callback: Function to call when event occurs.
        """
        self.callbacks[event] = callback
    
    def _convert_vision_to_neural_input(self, percept: SensoryPercept):
        """Convert vision percept to neural input signals.
        
        Args:
            percept: Sensory percept containing vision data.
        """
        # Implementation would convert vision data to neural signals
        # For now, it's a placeholder
        pass
    
    def _convert_audio_to_neural_input(self, percept: SensoryPercept):
        """Convert audio percept to neural input signals.
        
        Args:
            percept: Sensory percept containing audio data.
        """
        # Implementation would convert audio data to neural signals
        # For now, it's a placeholder
        pass
    
    def _convert_internal_to_neural_input(self, percept: SensoryPercept):
        """Convert internal signals to neural input signals.
        
        Args:
            percept: Sensory percept containing internal signals.
        """
        # Implementation would convert internal signals to neural signals
        # For now, it's a placeholder
        pass
    
    def _apply_reward_modulation(self, action: Action) -> Action:
        """Apply reward modulation to an action.
        
        Args:
            action: Action to modulate.
            
        Returns:
            Modulated action.
        """
        # Implementation would modulate action based on reward
        # For now, it's a placeholder
        return action
    
    def __repr__(self) -> str:
        """Get string representation of the agent brain.
        
        Returns:
            String representation of the agent brain.
        """
        return f"<AgentBrain (steps: {self.step_count}, actions: {self.action_count}, " \
               f"stage: {self.getDevelopmentalStage().name})>"


__all__ = ['AgentBrain']