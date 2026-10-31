"""
Motor system module for NLM Python bindings.
"""

import time
import math
from typing import Dict, List, Any, Optional, Tuple, Union
from enum import Enum

try:
    from pynlm.bindings import Action, ActionType, MotorCommand, AgentBody, createSimpleWorld
    from pynlm.bindings import createBrain, createDefaultConfig
    BINDINGS_AVAILABLE = True
except ImportError:
    BINDINGS_AVAILABLE = False
    print("Warning: C++ bindings not available. Motor module limited.")


class MovementVector:
    """Represents a 2D movement vector."""
    
    def __init__(self, x: float = 0.0, y: float = 0.0, angle: float = 0.0):
        """
        Initialize movement vector.
        
        Args:
            x: X displacement
            y: Y displacement
            angle: Movement angle in radians
        """
        self.x = x
        self.y = y
        self.angle = angle
    
    def add(self, other: 'MovementVector') -> 'MovementVector':
        """Add another vector to this one."""
        return MovementVector(self.x + other.x, self.y + other.y, self.angle)
    
    def scale(self, factor: float) -> 'MovementVector':
        """Scale this vector by factor."""
        return MovementVector(self.x * factor, self.y * factor, self.angle)
    
    def normalize(self) -> 'MovementVector':
        """Normalize this vector to unit length."""
        length = math.sqrt(self.x ** 2 + self.y ** 2)
        if length > 0:
            return MovementVector(self.x / length, self.y / length, self.angle)
        return MovementVector()
    
    def get_magnitude(self) -> float:
        """Get vector magnitude."""
        return math.sqrt(self.x ** 2 + self.y ** 2)
    
    def get_direction(self) -> float:
        """Get direction in radians."""
        return math.atan2(self.y, self.x)
    
    def rotate(self, angle: float) -> 'MovementVector':
        """Rotate vector by angle."""
        new_angle = self.angle + angle
        length = self.get_magnitude()
        return MovementVector(
            length * math.cos(new_angle),
            length * math.sin(new_angle),
            new_angle
        )


class MotorCommandBuilder:
    """Builder for creating motor commands."""
    
    def __init__(self):
        """Initialize command builder."""
        self._action_type = ActionType.Wait
        self._parameters = {}
        self._urgency = 1.0
        self._priority = 1.0
    
    def set_action(self, action_type: ActionType) -> 'MotorCommandBuilder':
        """Set action type."""
        self._action_type = action_type
        return self
    
    def set_parameters(self, **params) -> 'MotorCommandBuilder':
        """Set action parameters."""
        self._parameters = params
        return self
    
    def set_urgency(self, urgency: float) -> 'MotorCommandBuilder':
        """Set action urgency (0.0 to 1.0)."""
        self._urgency = max(0.0, min(1.0, urgency))
        return self
    
    def set_priority(self, priority: float) -> 'MotorCommandBuilder':
        """Set action priority (0.0 to 1.0)."""
        self._priority = max(0.0, min(1.0, priority))
        return self
    
    def move_forward(self, distance: float = 1.0, urgency: float = None) -> 'MotorCommandBuilder':
        """Set movement forward action."""
        self._action_type = ActionType.MoveForward
        self._parameters['distance'] = distance
        if urgency is not None:
            self._urgency = urgency
        return self
    
    def turn_left(self, angle: float = math.pi / 4, urgency: float = None) -> 'MotorCommandBuilder':
        """Set turn left action."""
        self._action_type = ActionType.TurnLeft
        self._parameters['angle'] = angle
        if urgency is not None:
            self._urgency = urgency
        return self
    
    def turn_right(self, angle: float = math.pi / 4, urgency: float = None) -> 'MotorCommandBuilder':
        """Set turn right action."""
        self._action_type = ActionType.TurnRight
        self._parameters['angle'] = angle
        if urgency is not None:
            self._urgency = urgency
        return self
    
    def look(self, direction_x: float = 0.0, direction_y: float = 0.0) -> 'MotorCommandBuilder':
        """Set look action."""
        self._action_type = ActionType.Look
        self._parameters['direction_x'] = direction_x
        self._parameters['direction_y'] = direction_y
        return self
    
    def interact(self, target_type: str = 'object') -> 'MotorCommandBuilder':
        """Set interaction action."""
        self._action_type = ActionType.Interact
        self._parameters['target_type'] = target_type
        return self
    
    def build(self) -> Action:
        """Build motor command."""
        action = Action(self._action_type, list(self._parameters.values()))
        return action
    
    def build_motor_command(self) -> MotorCommand:
        """Build motor command from action."""
        # Convert to motor command based on action type
        if self._action_type == ActionType.MoveForward:
            distance = self._parameters.get('distance', 1.0)
            return MotorCommand.MotorCommandTypeMoveForward(distance)
        elif self._action_type == ActionType.TurnLeft:
            angle = self._parameters.get('angle', math.pi / 4)
            return MotorCommand.MotorCommandTypeTurnLeft(angle)
        elif self._action_type == ActionType.TurnRight:
            angle = self._parameters.get('angle', math.pi / 4)
            return MotorCommand.MotorCommandTypeTurnRight(angle)
        elif self._action_type == ActionType.Look:
            dx = self._parameters.get('direction_x', 0.0)
            dy = self._parameters.get('direction_y', 0.0)
            return MotorCommand.MotorCommandTypeLook(dx, dy)
        else:
            return MotorCommand.MotorCommandTypeWait()


class MovementPlanner:
    """Plans movement sequences."""
    
    def __init__(self, world_width: int = 20, world_height: int = 20):
        """
        Initialize movement planner.
        
        Args:
            world_width: World width
            world_height: World height
        """
        self.world_width = world_width
        self.world_height = world_height
        self.safety_margin = 1.0
    
    def plan_path(self, start_x: float, start_y: float, 
                  target_x: float, target_y: float) -> List[MovementVector]:
        """
        Plan simple path to target.
        
        Args:
            start_x, start_y: Starting position
            target_x, target_y: Target position
        
        Returns:
            List of movement vectors
        """
        path = []
        
        # Check bounds
        start_x = max(self.safety_margin, min(self.world_width - self.safety_margin, start_x))
        start_y = max(self.safety_margin, min(self.world_height - self.safety_margin, start_y))
        target_x = max(self.safety_margin, min(self.world_width - self.safety_margin, target_x))
        target_y = max(self.safety_margin, min(self.world_height - self.safety_margin, target_y))
        
        # Calculate simple path
        dx = target_x - start_x
        dy = target_y - start_y
        distance = math.sqrt(dx ** 2 + dy ** 2)
        
        if distance > 0:
            # Determine if need to turn first
            if abs(dx) < 0.1:  # Need to turn vertically
                if target_y > start_y:
                    path.append(MovementVector(0, 1).rotate(math.pi / 2))  # Turn right
                else:
                    path.append(MovementVector(0, 1).rotate(-math.pi / 2))  # Turn left
            
            if abs(dy) < 0.1:  # Need to turn horizontally
                if target_x > start_x:
                    path.append(MovementVector(1, 0))  # Move forward
                else:
                    path.append(MovementVector(-1, 0))  # Move backward
            
            # Move forward to target
            direction = MovementVector(dx, dy).normalize()
            steps = int(distance / 0.5)  # 0.5 unit steps
            if steps > 0:
                step_vector = direction.scale(0.5)
                for _ in range(steps):
                    path.append(step_vector)
        
        return path


class MotorSystem:
    """Enhanced motor system with planning and control."""
    
    def __init__(self, agent_body: AgentBody = None):
        """
        Initialize motor system.
        
        Args:
            agent_body: Agent body state
        """
        self.agent_body = agent_body or AgentBody()
        self.command_builder = MotorCommandBuilder()
        self.movement_planner = MovementPlanner()
        
        # Motor control parameters
        self.max_velocity = 2.0
        self.max_angular_velocity = math.pi / 2
        self.acceleration = 0.5
        self.deceleration = 0.3
        self.rotation_speed = 0.1
        
        # Current state
        self.current_velocity_x = 0.0
        self.current_velocity_y = 0.0
        self.current_angular_velocity = 0.0
        self.target_velocity_x = 0.0
        self.target_velocity_y = 0.0
        self.target_angular_velocity = 0.0
    
    def set_target_velocity(self, vx: float, vy: float, angular: float = 0.0):
        """
        Set target velocity.
        
        Args:
            vx, vy: Target linear velocities
            angular: Target angular velocity
        """
        self.target_velocity_x = max(-self.max_velocity, min(self.max_velocity, vx))
        self.target_velocity_y = max(-self.max_velocity, min(self.max_velocity, vy))
        self.target_angular_velocity = max(-self.max_angular_velocity, 
                                           min(self.max_angular_velocity, angular))
    
    def update(self, timestep: float = 0.1):
        """
        Update motor system dynamics.
        
        Args:
            timestep: Time step
        """
        # Smooth velocity changes
        dvx = self.target_velocity_x - self.current_velocity_x
        dvy = self.target_velocity_y - self.current_velocity_y
        d_omega = self.target_angular_velocity - self.current_angular_velocity
        
        self.current_velocity_x += dvx * self.acceleration * timestep
        self.current_velocity_y += dvy * self.acceleration * timestep
        self.current_angular_velocity += d_omega * self.rotation_speed * timestep
        
        # Apply deceleration when no target
        if abs(self.target_velocity_x) < 0.01:
            self.current_velocity_x *= (1 - self.deceleration * timestep)
        if abs(self.target_velocity_y) < 0.01:
            self.current_velocity_y *= (1 - self.deceleration * timestep)
        if abs(self.target_angular_velocity) < 0.01:
            self.current_angular_velocity *= (1 - self.rotation_speed * timestep)
        
        # Update agent body
        self.agent_body.velocityX = self.current_velocity_x
        self.agent_body.velocityY = self.current_velocity_y
        self.agent_body.angularVelocity = self.current_angular_velocity
        
        # Update position
        self.agent_body.x += self.current_velocity_x * timestep
        self.agent_body.y += self.current_velocity_y * timestep
        self.agent_body.orientation += self.current_angular_velocity * timestep
        
        # Keep within bounds
        self._enforce_bounds()
    
    def _enforce_bounds(self):
        """Enforce world bounds."""
        radius = self.agent_body.radius if hasattr(self.agent_body, 'radius') else 0.5
        
        # X bounds
        if self.agent_body.x < radius:
            self.agent_body.x = radius
            self.current_velocity_x = 0
        elif self.agent_body.x > self.movement_planner.world_width - radius:
            self.agent_body.x = self.movement_planner.world_width - radius
            self.current_velocity_x = 0
        
        # Y bounds
        if self.agent_body.y < radius:
            self.agent_body.y = radius
            self.current_velocity_y = 0
        elif self.agent_body.y > self.movement_planner.world_height - radius:
            self.agent_body.y = self.movement_planner.world_height - radius
            self.current_velocity_y = 0
    
    def execute_command(self, action: Action, world, current_time: float):
        """
        Execute motor command.
        
        Args:
            action: Action to execute
            world: World object
            current_time: Current simulation time
        """
        if not action:
            return
        
        action_type = action.getType()
        
        if action_type == ActionType.MoveForward:
            distance = action.getParameters()[0] if action.getParameters() else 1.0
            self.set_target_velocity(
                self.agent_body.velocityX + distance * 0.5,
                self.agent_body.velocityY,
                self.agent_body.angularVelocity
            )
        
        elif action_type == ActionType.MoveBackward:
            distance = action.getParameters()[0] if action.getParameters() else 1.0
            self.set_target_velocity(
                self.agent_body.velocityX - distance * 0.5,
                self.agent_body.velocityY,
                self.agent_body.angularVelocity
            )
        
        elif action_type == ActionType.TurnLeft:
            angle = action.getParameters()[0] if action.getParameters() else math.pi / 4
            self.set_target_velocity(
                self.agent_body.velocityX,
                self.agent_body.velocityY,
                self.agent_body.angularVelocity - angle / 0.1
            )
        
        elif action_type == ActionType.TurnRight:
            angle = action.getParameters()[0] if action.getParameters() else math.pi / 4
            self.set_target_velocity(
                self.agent_body.velocityX,
                self.agent_body.velocityY,
                self.agent_body.angularVelocity + angle / 0.1
            )
        
        elif action_type == ActionType.Look:
            dx = action.getParameters()[0] if len(action.getParameters()) > 0 else 0.0
            dy = action.getParameters()[1] if len(action.getParameters()) > 1 else 0.0
            # Look direction affects next movement
            self.look_direction = (dx, dy)
        
        elif action_type == ActionType.Interact:
            # Trigger interaction with nearby object
            self._handle_interaction(world)
    
    def _handle_interaction(self, world):
        """Handle interaction with nearby objects."""
        # Simple interaction: look for objects within radius
        if hasattr(world, 'getAgentBody'):
            body = world.getAgentBody()
            # Check for collisions with world objects
            # This would need to be implemented based on world API
            pass
    
    def decode_motor_command(self) -> MotorCommand:
        """Decode current motor state into motor command."""
        # Create motor command based on current state
        if abs(self.current_velocity_x) > 0.1 or abs(self.current_velocity_y) > 0.1:
            return MotorCommand.MotorCommandTypeMoveForward(
                math.sqrt(self.current_velocity_x ** 2 + self.current_velocity_y ** 2)
            )
        elif abs(self.current_angular_velocity) > 0.01:
            if self.current_angular_velocity > 0:
                return MotorCommand.MotorCommandTypeTurnRight(
                    abs(self.current_angular_velocity) * 0.1
                )
            else:
                return MotorCommand.MotorCommandTypeTurnLeft(
                    abs(self.current_angular_velocity) * 0.1
                )
        else:
            return MotorCommand.MotorCommandTypeWait()
    
    def plan_movement(self, target_x: float, target_y: float):
        """
        Plan movement to target.
        
        Args:
            target_x, target_y: Target position
        """
        current_x, current_y = self.agent_body.x, self.agent_body.y
        path = self.movement_planner.plan_path(current_x, current_y, target_x, target_y)
        
        # Execute first movement command
        if path:
            first_move = path[0]
            self.set_target_velocity(first_move.x, first_move.y, 0)
        
        return path
    
    def get_state(self) -> Dict[str, Any]:
        """Get current motor system state."""
        return {
            'position': (self.agent_body.x, self.agent_body.y),
            'velocity': (self.current_velocity_x, self.current_velocity_y),
            'angular_velocity': self.current_angular_velocity,
            'orientation': self.agent_body.orientation,
            'max_velocity': self.max_velocity,
            'max_angular_velocity': self.max_angular_velocity,
        }
    
    def __repr__(self) -> str:
        return f"MotorSystem(position=({self.agent_body.x:.1f}, {self.agent_body.y:.1f}), velocity={self.get_state()['velocity']})"


def create_motor_system(agent_body: AgentBody = None) -> MotorSystem:
    """Create motor system."""
    return MotorSystem(agent_body)


def create_movement_builder() -> MotorCommandBuilder:
    """Create movement command builder."""
    return MotorCommandBuilder()


def create_movement_planner(width: int = 20, height: int = 20) -> MovementPlanner:
    """Create movement planner."""
    return MovementPlanner(width, height)
