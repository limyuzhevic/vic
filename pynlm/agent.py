"""
Enhanced Python interface for NLM AgentBrain and related classes
"""

from typing import List, Dict, Any, Optional, Tuple
from enum import Enum

from .core import Brain, NeuronId, SynapseId, RegionId, PopulationId, NeuronType, ActionType
from .exceptions import NLMError

class MotorCommand(Enum):
    """Motor command enumeration"""
    MOVE_FORWARD = "move_forward"
    MOVE_BACKWARD = "move_backward"
    TURN_LEFT = "turn_left"
    TURN_RIGHT = "turn_right"
    LOOK_LEFT = "look_left"
    LOOK_RIGHT = "look_right"
    INTERACT = "interact"
    WAIT = "wait"

class DevelopmentalStage(Enum):
    """Developmental stage enumeration"""
    INITIAL = "initial"
    CRITICAL_PERIOD = "critical_period"
    MATURATION = "maturation"
    ADULT = "adult"
    AGING = "aging"

class Action:
    """Action representation with enhanced Python interface"""
    
    def __init__(self, action_type: ActionType = ActionType.WAIT, 
                 parameters: Optional[List[float]] = None):
        self.action_type = action_type
        self.parameters = parameters or []
        
    def get_type(self) -> ActionType:
        """Get action type"""
        return self.action_type
        
    def set_type(self, action_type: ActionType) -> None:
        """Set action type"""
        self.action_type = action_type
        
    def get_parameters(self) -> List[float]:
        """Get action parameters"""
        return self.parameters
        
    def set_parameters(self, parameters: List[float]) -> None:
        """Set action parameters"""
        self.parameters = parameters
        
    def get_name(self) -> str:
        """Get action name"""
        return str(self.action_type).split('.')[-1]
        
    def get_description(self) -> str:
        """Get action description"""
        descriptions = {
            ActionType.MOVE_FORWARD: "Move forward in the direction agent is facing",
            ActionType.MOVE_BACKWARD: "Move backward",
            ActionType.TURN_LEFT: "Turn left (typically 90 degrees)",
            ActionType.TURN_RIGHT: "Turn right (typically 90 degrees)",
            ActionType.LOOK: "Look in current direction",
            ActionType.LOOK_UP: "Look upward",
            ActionType.LOOK_DOWN: "Look downward",
            ActionType.INTERACT: "Interact with nearby object",
            ActionType.EAT: "Eat available food resource",
            ActionType.DRINK: "Drink water resource",
            ActionType.REST: "Rest and recover energy",
            ActionType.WAIT: "Do nothing this step",
            ActionType.CUSTOM: "Custom action (requires parameters)"
        }
        return descriptions.get(self.action_type, "Unknown action")
        
    def clone(self) -> 'Action':
        """Create a copy of this action"""
        return Action(self.action_type, self.parameters.copy())
        
    def __repr__(self):
        return f"<Action: {self.get_name()}>"
        
    def __eq__(self, other):
        if not isinstance(other, Action):
            return False
        return (self.action_type == other.action_type and 
                self.parameters == other.parameters)
        
    def __hash__(self):
        return hash((self.action_type, tuple(self.parameters)))

class WorldObject:
    """World object representation"""
    
    def __init__(self, x: float = 0.0, y: float = 0.0, 
                 obj_type: str = "empty", value: float = 0.0, 
                 radius: float = 0.5, active: bool = True):
        self.x = x
        self.y = y
        self.type = obj_type
        self.value = value
        self.radius = radius
        self.active = active
        
    def get_position(self) -> Tuple[float, float]:
        """Get object position"""
        return (self.x, self.y)
        
    def set_position(self, x: float, y: float) -> None:
        """Set object position"""
        self.x = x
        self.y = y
        
    def get_type(self) -> str:
        """Get object type"""
        return self.type
        
    def set_type(self, obj_type: str) -> None:
        """Set object type"""
        self.type = obj_type
        
    def get_value(self) -> float:
        """Get object value"""
        return self.value
        
    def set_value(self, value: float) -> None:
        """Set object value"""
        self.value = value
        
    def get_radius(self) -> float:
        """Get object radius"""
        return self.radius
        
    def set_radius(self, radius: float) -> None:
        """Set object radius"""
        self.radius = radius
        
    def is_active(self) -> bool:
        """Check if object is active"""
        return self.active
        
    def set_active(self, active: bool) -> None:
        """Set object active state"""
        self.active = active
        
    def distance_to(self, other: 'WorldObject') -> float:
        """Calculate distance to another object"""
        dx = self.x - other.x
        dy = self.y - other.y
        return (dx * dx + dy * dy) ** 0.5
        
    def is_colliding(self, other: 'WorldObject') -> bool:
        """Check if colliding with another object"""
        distance = self.distance_to(other)
        return distance < (self.radius + other.radius)
        
    def __repr__(self):
        return f"<WorldObject: {self.type} at ({self.x:.1f}, {self.y:.1f}), radius={self.radius:.1f}>"

class AgentBody:
    """Agent body state"""
    
    def __init__(self):
        self.x = 0.0
        self.y = 0.0
        self.orientation = 0.0
        self.velocity_x = 0.0
        self.velocity_y = 0.0
        self.angular_velocity = 0.0
        self.energy = 100.0
        self.health = 1.0
        self.age = 0.0
        self.is_moving = False
        self.is_turning = False
        self.last_action_time = 0.0
        
    def reset(self) -> None:
        """Reset body state"""
        self.x = 0.0
        self.y = 0.0
        self.orientation = 0.0
        self.velocity_x = 0.0
        self.velocity_y = 0.0
        self.angular_velocity = 0.0
        self.is_moving = False
        self.is_turning = False
        
    def __repr__(self):
        return f"<AgentBody: pos=({self.x:.1f}, {self.y:.1f}), energy={self.energy:.1f}>"

class ActionResult:
    """Result from applying a motor command in the world"""
    
    def __init__(self, reward: float = 0.0, success: bool = False, 
                 message: str = ""):
        self.reward = reward
        self.success = success
        self.message = message
        
    def get_reward(self) -> float:
        """Get reward value"""
        return self.reward
        
    def set_reward(self, reward: float) -> None:
        """Set reward value"""
        self.reward = reward
        
    def was_successful(self) -> bool:
        """Check if action was successful"""
        return self.success
        
    def set_successful(self, success: bool) -> None:
        """Set success status"""
        self.success = success
        
    def get_message(self) -> str:
        """Get result message"""
        return self.message
        
    def set_message(self, message: str) -> None:
        """Set result message"""
        self.message = message
        
    def __repr__(self):
        return f"<ActionResult: reward={self.reward:.2f}, success={self.success}, message='{self.message}'>"

class SensoryPercept:
    """Sensory percept data from the world"""
    
    def __init__(self):
        self.vision = []  # List of vision values
        self.touch = []   # List of touch values
        self.internal = []  # List of internal signals
        self.proprioception = []  # List of proprioception values
        self.audio = []    # List of audio values
        self.timestamp = 0.0
        
    def get_vision(self) -> List[float]:
        """Get vision data"""
        return self.vision
        
    def set_vision(self, vision: List[float]) -> None:
        """Set vision data"""
        self.vision = vision
        
    def get_vision_width(self) -> int:
        """Get vision width"""
        return int(len(self.vision) ** 0.5) if self.vision else 0
        
    def get_vision_height(self) -> int:
        """Get vision height"""
        return self.get_vision_width()
        
    def get_touch(self) -> List[float]:
        """Get touch data"""
        return self.touch
        
    def set_touch(self, touch: List[float]) -> None:
        """Set touch data"""
        self.touch = touch
        
    def get_internal(self) -> List[float]:
        """Get internal signals"""
        return self.internal
        
    def set_internal(self, internal: List[float]) -> None:
        """Set internal signals"""
        self.internal = internal
        
    def get_proprioception(self) -> List[float]:
        """Get proprioception data"""
        return self.proprioception
        
    def set_proprioception(self, proprioception: List[float]) -> None:
        """Set proprioception data"""
        self.proprioception = proprioception
        
    def get_audio(self) -> List[float]:
        """Get audio data"""
        return self.audio
        
    def set_audio(self, audio: List[float]) -> None:
        """Set audio data"""
        self.audio = audio
        
    def get_all_signals(self) -> Dict[str, List[float]]:
        """Get all sensory signals"""
        return {
            'vision': self.vision,
            'touch': self.touch,
            'internal': self.internal,
            'proprioception': self.proprioception,
            'audio': self.audio
        }
        
    def get_timestamp(self) -> float:
        """Get timestamp"""
        return self.timestamp
        
    def set_timestamp(self, timestamp: float) -> None:
        """Set timestamp"""
        self.timestamp = timestamp
        
    def __repr__(self):
        return f"<SensoryPercept: vision={len(self.vision)}, touch={len(self.touch)}>"

class AgentBrain:
    """Enhanced Python interface for AgentBrain"""
    
    def __init__(self, brain: Brain):
        self._brain_ptr = brain
        self._brain = brain  # For convenience
        
        # Initialize agent state
        self._dopamine_level = 0.0
        self._novelty_level = 0.0
        self._curiosity_level = 0.0
        self._prediction_error = 0.0
        self._expected_reward = 0.0
        self._developmental_age = 0.0
        self._plasticity_modifier = 1.0
        
        # Configuration flags
        self._reward_modulation_enabled = True
        self._structural_plasticity_enabled = True
        self._development_enabled = True
        self._curiosity_enabled = True
        
        # Previous vision for novelty detection
        self._previous_vision = []
        self._sensory_novelty_decay = 0.99
        
        # Motor neuron groups (will be populated when brain is initialized)
        self._motor_forward = []
        self._motor_backward = []
        self._motor_turn_left = []
        self._motor_turn_right = []
        self._motor_interact = []
        self._motor_wait = []
        
        # Sensory neuron groups
        self._sensory_vision = []
        self._sensory_touch = []
        self._sensory_internal = []
        self._sensory_proprioception = []
        
    def initialize(self, world) -> None:
        """Initialize the agent brain with world interface"""
        # Set up initial vision storage
        if hasattr(world, 'get_vision_width') and hasattr(world, 'get_vision_height'):
            self._previous_vision = [0.0] * (world.get_vision_width() * world.get_vision_height())
            
    def get_sensory_input_size(self) -> int:
        """Get expected sensory input size"""
        # Vision (16x16) + touch (8) + internal (4) + proprioception (6)
        return 256 + 8 + 4 + 6
        
    def get_motor_output_size(self) -> int:
        """Get expected motor output size"""
        # One motor neuron per action
        return 6
        
    def process_sensory_input(self, percept: SensoryPercept) -> None:
        """Process sensory percept and inject into brain"""
        if not self._brain:
            raise NLMError("Brain not initialized")
        if not percept:
            raise NLMError("Sensory percept cannot be None")
            
        # Update novelty based on current vision
        if percept.vision:
            novelty = self._calculate_novelty(percept.vision)
            self._novelty_level = novelty * 2.0  # Scale factor
            self._curiosity_level = novelty * 2.0 + abs(self._prediction_error) * 0.5
            self._curiosity_level = max(0.0, min(1.0, self._curiosity_level))
            
        # Inject sensory currents into brain
        # Vision input
        for i, vision_val in enumerate(percept.vision):
            if i < len(self._sensory_vision_) and self._sensory_vision_[i]:
                current = vision_val * 5.0
                # Inject current into sensory neuron
                # This would call the C++ method: sensory_neuron->injectCurrent(current)
                
        # Touch input
        for i, touch_val in enumerate(percept.touch):
            if i < len(self._sensory_touch_) and self._sensory_touch_[i]:
                current = touch_val * 8.0
                # Inject current into touch neuron
                
        # Internal signals
        for i, internal_val in enumerate(percept.internal):
            if i < len(self._sensory_internal_) and self._sensory_internal_[i]:
                current = (internal_val * 2.0 - 1.0) * 5.0
                # Inject current into internal neuron
                
        # Proprioception
        for i, proprio_val in enumerate(percept.proprioception):
            if i < len(self._sensory_proprioception_) and self._sensory_proprioception_[i]:
                current = (proprio_val * 2.0 - 1.0) * 3.0
                # Inject current into proprioception neuron
                
    def decode_motor_command(self) -> MotorCommand:
        """Decode brain motor activity into motor command"""
        if not self._brain:
            return MotorCommand.WAIT
            
        # Calculate activity of motor neuron groups
        motor_groups = {
            'forward': self._motor_forward_,
            'backward': self._motor_backward_,
            'left': self._motor_turn_left_,
            'right': self._motor_turn_right_,
            'interact': self._motor_interact_,
            'wait': self._motor_wait_
        }
        
        activity_scores = {}
        for action, neurons in motor_groups.items():
            if not neurons:
                activity_scores[action] = 0.0
                continue
                
            # Calculate activity based on neuron membrane potential deviation
            activity = 0.0
            for neuron in neurons:
                if neuron:
                    activity += abs(neuron->getState().membranePotential - neuron->getState().restingPotential)
            activity_scores[action] = activity / len(neurons)
            
        # Find maximum activity
        best_action = 'wait'
        best_activity = activity_scores.get('wait', 0.0)
        
        for action, activity in activity_scores.items():
            if activity > best_activity:
                best_activity = activity
                best_action = action
                
        # Apply curiosity-based exploration
        if self._curiosity_enabled and self._curiosity_level > 0.3:
            best_action = self._select_with_curiosity(best_action)
            
        # Convert action name to MotorCommand
        action_to_command = {
            'forward': MotorCommand.MOVE_FORWARD,
            'backward': MotorCommand.MOVE_BACKWARD,
            'left': MotorCommand.TURN_LEFT,
            'right': MotorCommand.TURN_RIGHT,
            'interact': MotorCommand.INTERACT,
            'wait': MotorCommand.WAIT
        }
        
        return action_to_command.get(best_action, MotorCommand.WAIT)
        
    def apply_reward_modulation(self, reward: float, predicted_reward: float) -> None:
        """Apply reward-based neuromodulation"""
        if not self._brain or not self._reward_modulation_enabled:
            return
            
        # Calculate prediction error
        self._prediction_error = reward - predicted_reward
        
        # Update expected reward (exponential moving average)
        self._expected_reward = 0.95 * self._expected_reward + 0.05 * reward
        
        # Dopamine-like signal (based on prediction error)
        self._dopamine_level = self._prediction_error
        self._dopamine_level = max(-1.0, min(1.0, self._dopamine_level))  # Clamp
        
        # Apply to all synapses with eligibility traces
        # This would call C++ methods to apply neuromodulation
        # For now, just record the values
        
        # Modulate plasticity based on dopamine
        # Positive dopamine increases plasticity, negative decreases
        self._plasticity_modifier = 0.5 + 0.5 * self._dopamine_level
        self._plasticity_modifier = max(0.1, min(2.0, self._plasticity_modifier))
        
    def update_development(self, timestep: float) -> None:
        """Update development system"""
        if not self._brain or not self._development_enabled:
            return
            
        self._developmental_age += timestep
        
        # Simple developmental stages based on age
        if self._developmental_age < 60.0:  # Initial stage
            self._plasticity_modifier = 1.0
            # Set developmental stage in brain
        elif self._developmental_age < 300.0:  # Critical period
            self._plasticity_modifier = 0.8
        elif self._developmental_age < 900.0:  # Maturation
            self._plasticity_modifier = 0.5
        else:  # Adult
            self._plasticity_modifier = 0.2  # Adult - more stable
            
        # Structural plasticity changes with development
        if self._structural_plasticity_enabled:
            # This would call C++ methods to update structural plasticity
            pass
        
    def get_developmental_stage(self) -> str:
        """Get current developmental stage"""
        if not self._brain:
            return "initial"
        # This would call the C++ method: brain->getDevelopmentalStage()
        return self._brain.get_developmental_stage()
        
    def get_neuromodulation_level(self) -> float:
        """Get current neuromodulation level"""
        return self._dopamine_level
        
    def get_curiosity_level(self) -> float:
        """Get curiosity level"""
        return self._curiosity_level
        
    def get_novelty_level(self) -> float:
        """Get novelty level"""
        return self._novelty_level
        
    def get_prediction_error(self) -> float:
        """Get prediction error"""
        return self._prediction_error
        
    def reset(self) -> None:
        """Reset agent for new episode"""
        self._dopamine_level = 0.0
        self._novelty_level = 0.0
        self._curiosity_level = 0.0
        self._prediction_error = 0.0
        self._expected_reward = 0.0
        self._developmental_age = 0.0
        self._plasticity_modifier = 1.0
        
        # Clear previous vision
        self._previous_vision = [0.0] * len(self._previous_vision)
        
    # Advanced motor control methods
    def get_motor_neurons(self, action_type: str) -> List:
        """Get motor neurons for specific action type"""
        if action_type == "forward":
            return self._motor_forward_
        elif action_type == "backward":
            return self._motor_backward_
        elif action_type == "turn_left":
            return self._motor_turn_left_
        elif action_type == "turn_right":
            return self._motor_turn_right_
        elif action_type == "interact":
            return self._motor_interact_
        elif action_type == "wait":
            return self._motor_wait_
        else:
            return []
        
    def get_sensory_neurons(self, modality: str) -> List:
        """Get sensory neurons for specific modality"""
        if modality == "vision":
            return self._sensory_vision_
        elif modality == "touch":
            return self._sensory_touch_
        elif modality == "internal":
            return self._sensory_internal_
        elif modality == "proprioception":
            return self._sensory_proprioception_
        else:
            return []
        
    def set_novelty_decay(self, decay_rate: float) -> None:
        """Set novelty decay rate"""
        self._sensory_novelty_decay = decay_rate
        
    def get_novelty_decay(self) -> float:
        """Get novelty decay rate"""
        return self._sensory_novelty_decay
        
    def _calculate_novelty(self, vision: List[float]) -> float:
        """Calculate novelty based on vision difference"""
        if not vision or not self._previous_vision:
            return 0.0
            
        total_diff = 0.0
        for i, vision_val in enumerate(vision):
            if i < len(self._previous_vision):
                diff = abs(vision_val - self._previous_vision[i])
                total_diff += diff
                
        # Normalize
        novelty = total_diff / max(len(vision), 1)
        
        # Apply decay
        novelty *= self._sensory_novelty_decay
        
        # Store for next time
        self._previous_vision = vision.copy()
        
        return novelty
        
    def _select_with_curiosity(self, default_action: str) -> str:
        """Select action based on curiosity/exploration"""
        # Exploration: occasionally choose random action when curiosity is high
        if self._curiosity_level > 0.5:
            # Higher curiosity = more exploration
            import random
            explore_chance = self._curiosity_level * 0.3  # Up to 30% random
            
            if random.random() < explore_chance:
                # Random motor command
                actions = ['forward', 'backward', 'turn_left', 'turn_right', 'interact', 'wait']
                return random.choice(actions)
                
        return default_action
        
    def get_brain(self) -> Brain:
        """Get the underlying brain"""
        return self._brain
        
    # Configuration methods
    def enable_reward_modulation(self, enable: bool) -> None:
        """Enable/disable reward modulation"""
        self._reward_modulation_enabled = enable
        
    def enable_structural_plasticity(self, enable: bool) -> None:
        """Enable/disable structural plasticity"""
        self._structural_plasticity_enabled = enable
        
    def enable_development(self, enable: bool) -> None:
        """Enable/disable development"""
        self._development_enabled = enable
        
    def enable_curiosity(self, enable: bool) -> None:
        """Enable/disable curiosity"""
        self._curiosity_enabled = enable
        
    def is_reward_modulation_enabled(self) -> bool:
        """Check if reward modulation is enabled"""
        return self._reward_modulation_enabled
        
    def is_structural_plasticity_enabled(self) -> bool:
        """Check if structural plasticity is enabled"""
        return self._structural_plasticity_enabled
        
    def is_development_enabled(self) -> bool:
        """Check if development is enabled"""
        return self._development_enabled
        
    def is_curiosity_enabled(self) -> bool:
        """Check if curiosity is enabled"""
        return self._curiosity_enabled
        
    def __repr__(self):
        return f"<AgentBrain: dopamine={self._dopamine_level:.2f}, curiosity={self._curiosity_level:.2f}>"
