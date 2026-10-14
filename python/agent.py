# Agent interface wrappers

from typing import Optional, Dict, Any, List, Tuple
from enum import Enum
import time
from dataclasses import dataclass

from .exceptions import NLMErrors
from .world import SensoryPercept, ActionResult
from .brain import BrainSession

class ActionType(Enum):
    """Action types."""
    MOVE_FORWARD = 0
    MOVE_BACKWARD = 1
    MOVE_LEFT = 2
    MOVE_RIGHT = 3
    TURN_LEFT = 4
    TURN_RIGHT = 5
    LOOK = 6
    LOOK_UP = 7
    LOOK_DOWN = 8
    INTERACT = 9
    EAT = 10
    DRINK = 11
    REST = 12
    WAIT = 13
    CUSTOM = 14

class MotorCommand(Enum):
    """Motor command types."""
    MOVE_FORWARD = 0
    MOVE_BACKWARD = 1
    TURN_LEFT = 2
    TURN_RIGHT = 3
    LOOK_LEFT = 4
    LOOK_RIGHT = 5
    INTERACT = 6
    WAIT = 7

@dataclass
class Action:
    """Action representation."""
    action_type: ActionType
    parameters: List[float]
    
    def __post_init__(self):
        if self.action_type == ActionType.CUSTOM and not self.parameters:
            raise NLMErrors("Custom actions require parameters")
            
    def get_name(self) -> str:
        """Get human-readable action name."""
        return self.action_type.name.replace('_', ' ').title()
        
    def clone(self) -> 'Action':
        """Clone action."""
        return Action(self.action_type, self.parameters.copy())

@dataclass
class ActionResult:
    """Action result from world."""
    success: bool
    reward: float
    message: str = ""
    metadata: Dict[str, Any] = None
    
    def __post_init__(self):
        if self.metadata is None:
            self.metadata = {}

class AgentBrain:
    """Enhanced agent brain interface with Pythonic API."""
    
    def __init__(self, brain_session: BrainSession):
        self.brain_session = brain_session
        self._agent = None
        self._is_initialized = False
        self._reward_modulation_enabled = False
        self._curiosity_enabled = False
        self._structural_plasticity_enabled = False
        self._development_enabled = False
        self._neuromodulation_level = 0.0
        self._curiosity_level = 0.0
        self._novelty_level = 0.0
        self._prediction_error = 0.0
        self._expected_reward = 0.0
        self._previous_vision: Optional[List[float]] = None
        self._sensory_novelty_decay = 0.1
        
    def initialize(self, world):
        """Initialize agent with world."""
        if self._is_initialized:
            raise NLMErrors("Agent already initialized")
            
        try:
            from .bindings import AgentBrain as CPPAgentBrain
            self._agent = CPPAgentBrain(self.brain_session.brain)
            self._agent.initialize(world)
            self._is_initialized = True
        except Exception as e:
            raise NLMErrors(f"Failed to initialize agent: {e}")
            
    def process_sensory_input(self, percept: SensoryPercept):
        """Process sensory percept and inject into brain."""
        if not self._is_initialized:
            raise NLMErrors("Agent not initialized. Call initialize() first")
            
        if not isinstance(percept, SensoryPercept):
            raise NLMErrors(f"Expected SensoryPercept, got {type(percept)}")
            
        # Detect novelty
        if self._curiosity_enabled and self._previous_vision is not None:
            current_vision = percept.vision.get_flat_data()
            self._novelty_level = self._compute_novelty(current_vision)
            self._previous_vision = current_vision
            
        self._agent.process_sensory_input(percept)
        
    def decode_motor_command(self) -> MotorCommand:
        """Decode brain motor activity into motor command."""
        if not self._is_initialized:
            raise NLMErrors("Agent not initialized. Call initialize() first")
            
        return self._agent.decode_motor_command()
        
    def apply_reward_modulation(self, reward: float, predicted_reward: float = 0.0):
        """Apply reward-based neuromodulation."""
        if not self._is_initialized:
            raise NLMErrors("Agent not initialized. Call initialize() first")
            
        self._agent.apply_reward_modulation(reward, predicted_reward)
        self._expected_reward = predicted_reward
        
    def update_development(self, timestep: float):
        """Update development system."""
        if not self._is_initialized:
            raise NLMErrors("Agent not initialized. Call initialize() first")
            
        self._agent.update_development(timestep)
        
    def get_developmental_stage(self):
        """Get current developmental stage."""
        if not self._is_initialized:
            raise NLMErrors("Agent not initialized. Call initialize() first")
            
        return self._agent.get_developmental_stage()
        
    def get_neuromodulation_level(self) -> float:
        """Get current neuromodulation level."""
        if not self._is_initialized:
            raise NLMErrors("Agent not initialized. Call initialize() first")
            
        return self._agent.get_neuromodulation_level()
        
    def get_curiosity_level(self) -> float:
        """Get curiosity level."""
        if not self._is_initialized:
            raise NLMErrors("Agent not initialized. Call initialize() first")
            
        return self._agent.get_curiosity_level()
        
    def get_novelty_level(self) -> float:
        """Get novelty level."""
        if not self._is_initialized:
            raise NLMErrors("Agent not initialized. Call initialize() first")
            
        return self._agent.get_novelty_level()
        
    def get_prediction_error(self) -> float:
        """Get prediction error."""
        if not self._is_initialized:
            raise NLMErrors("Agent not initialized. Call initialize() first")
            
        return self._agent.get_prediction_error()
        
    def get_brain(self):
        """Get underlying brain."""
        return self.brain_session.brain
        
    def reset(self):
        """Reset agent for new episode."""
        if self._is_initialized:
            self._agent.reset()
            self._reset_neuromodulation_state()
            
    def enable_reward_modulation(self, enable: bool):
        """Enable/disable reward modulation."""
        self._reward_modulation_enabled = enable
        
    def enable_curiosity(self, enable: bool):
        """Enable/disable curiosity."""
        self._curiosity_enabled = enable
        
    def enable_structural_plasticity(self, enable: bool):
        """Enable/disable structural plasticity."""
        self._structural_plasticity_enabled = enable
        
    def enable_development(self, enable: bool):
        """Enable/disable development."""
        self._development_enabled = enable
        
    def is_reward_modulation_enabled(self) -> bool:
        """Check if reward modulation is enabled."""
        return self._reward_modulation_enabled
        
    def is_curiosity_enabled(self) -> bool:
        """Check if curiosity is enabled."""
        return self._curiosity_enabled
        
    def is_structural_plasticity_enabled(self) -> bool:
        """Check if structural plasticity is enabled."""
        return self._structural_plasticity_enabled
        
    def is_development_enabled(self) -> bool:
        """Check if development is enabled."""
        return self._development_enabled
        
    def get_sensory_input_size(self) -> int:
        """Get expected sensory input size."""
        if not self._is_initialized:
            raise NLMErrors("Agent not initialized. Call initialize() first")
            
        return self._agent.get_sensory_input_size()
        
    def get_motor_output_size(self) -> int:
        """Get expected motor output size."""
        if not self._is_initialized:
            raise NLMErrors("Agent not initialized. Call initialize() first")
            
        return self._agent.get_motor_output_size()
        
    def _compute_novelty(self, current_vision: List[float]) -> float:
        """Compute novelty of current vision."""
        if self._previous_vision is None:
            return 0.0
            
        if len(current_vision) != len(self._previous_vision):
            return 0.0
            
        # Compute mean squared difference
        squared_diff = 0.0
        for i in range(len(current_vision)):
            diff = current_vision[i] - self._previous_vision[i]
            squared_diff += diff * diff
            
        mse = squared_diff / len(current_vision)
        
        # Decay novelty over time
        novelty = min(1.0, mse * 10.0) * (1.0 - self._sensory_novelty_decay)
        
        return novelty
        
    def _reset_neuromodulation_state(self):
        """Reset neuromodulation state."""
        self._neuromodulation_level = 0.0
        self._curiosity_level = 0.0
        self._novelty_level = 0.0
        self._prediction_error = 0.0
        self._expected_reward = 0.0
        self._previous_vision = None
        
    @property
    def agent(self):
        """Get underlying C++ agent (advanced usage)."""
        if not self._is_initialized:
            raise NLMErrors("Agent not initialized")
        return self._agent

# Convenience functions

def create_agent_brain(brain_session: BrainSession) -> AgentBrain:
    """Create an agent brain interface."""
    return AgentBrain(brain_session)
        

def run_agent_simulation(world, agent_brain, num_steps: int) -> List[Dict[str, Any]]:
    """Run an agent simulation and collect statistics."""
    statistics = []
    
    for step in range(num_steps):
        # Process sensory input
        percept = world.get_sensory_percept()
        agent_brain.process_sensory_input(percept)
        
        # Get motor command
        motor_cmd = agent_brain.decode_motor_command()
        
        # Apply action to world
        result = world.apply_motor_command(motor_cmd, step * 0.1)
        
        # Apply reward modulation
        agent_brain.apply_reward_modulation(result.reward)
        
        # Update development
        agent_brain.update_development(0.1)
        
        # Collect statistics
        stats = {
            'step': step,
            'action': motor_cmd.name,
            'reward': result.reward,
            'success': result.success,
            'dopamine': agent_brain.get_neuromodulation_level(),
            'curiosity': agent_brain.get_curiosity_level(),
            'novelty': agent_brain.get_novelty_level(),
            'prediction_error': agent_brain.get_prediction_error(),
        }
        statistics.append(stats)
        
    return statistics