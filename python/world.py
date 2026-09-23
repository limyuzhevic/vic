#!/usr/bin/env python3
"""
World module for NLM (Neural Learning Machine).

This module provides the SimpleWorld class and related functionality for
creating and managing virtual environments that NLM agents can interact with.
"""

from typing import Dict, Any, Optional, List, Tuple
from dataclasses import dataclass
import math

@dataclass
class WorldObject:
    """
    World object representation.

    This class represents objects in the virtual world that agents can
    perceive, interact with, and potentially learn about.
    """

    id: str
    position: Tuple[float, float]
    size: Tuple[float, float]
    object_type: str = "generic"
    properties: Optional[Dict[str, Any]] = None

    def get_x(self) -> float:
        """Get X coordinate of object."""
        return self.position[0]

    def get_y(self) -> float:
        """Get Y coordinate of object."""
        return self.position[1]

    def get_width(self) -> float:
        """Get object width."""
        return self.size[0]

    def get_height(self) -> float:
        """Get object height."""
        return self.size[1]

    def is_colliding_with(self, other: 'WorldObject') -> bool:
        """Check if this object is colliding with another."""
        return (abs(self.get_x() - other.get_x()) < (self.get_width() + other.get_width()) / 2 and
                abs(self.get_y() - other.get_y()) < (self.get_height() + other.get_height()) / 2)

    def distance_to(self, other: 'WorldObject') -> float:
        """Get distance to another object."""
        dx = self.get_x() - other.get_x()
        dy = self.get_y() - other.get_y()
        return math.sqrt(dx * dx + dy * dy)

    def __str__(self) -> str:
        """String representation."""
        return f"WorldObject({self.id}, {self.object_type} at {self.position})"
class SimpleWorld:
    """
    Simple 2D world for NLM simulation.

    This class represents a virtual environment where NLM agents can
    interact, perceive objects, and take actions. The world manages
    object positions, updates, and sensory information.
    """

    def __init__(self):
        """Initialize an empty world."""
        self.objects: Dict[str, WorldObject] = {}
        self.time = 0.0
        self.width = 100.0
        self.height = 100.0
        self.vision_width = 20.0
        self.vision_height = 20.0

        # Agent state
        self.agent_position = (50.0, 50.0)
        self.agent_direction = 0.0  # radians

    def configure(self, width: float = 100.0, height: float = 100.0,
                  vision_width: float = 20.0, vision_height: float = 20.0) -> None:
        """
        Configure world dimensions and sensor ranges.

        Args:
            width: World width in units
            height: World height in units
            vision_width: Width of agent's vision range
            vision_height: Height of agent's vision range
        """
        self.width = width
        self.height = height
        self.vision_width = vision_width
        self.vision_height = vision_height

        # Clamp agent position to new bounds
        self.agent_position = (
            max(0, min(self.agent_position[0], self.width - 1)),
            max(0, min(self.agent_position[1], self.height - 1))
        )

        print(f"World configured:")
        print(f"  - Dimensions: {self.width} x {self.height}")
        print(f"  - Vision range: {self.vision_width} x {self.vision_height}")
        print(f"  - Agent start position: {self.agent_position}")

    def add_object(self, obj: WorldObject) -> None:
        """
        Add an object to the world.

        Args:
            obj: WorldObject to add
        """
        self.objects[obj.id] = obj
        print(f"Added object: {obj}")

    def remove_object(self, obj_id: str) -> bool:
        """
        Remove an object from the world.

        Args:
            obj_id: ID of object to remove

        Returns:
            True if object was removed, False if not found
        """
        if obj_id in self.objects:
            obj = self.objects.pop(obj_id)
            print(f"Removed object: {obj}")
            return True
        print(f"Object not found: {obj_id}")
        return False

    def reset(self) -> None:
        """Reset the world to initial state."""
        self.time = 0.0
        self.objects.clear()
        self.agent_position = (self.width / 2, self.height / 2)
        self.agent_direction = 0.0
        print("World reset to initial state")

    def update(self, dt: float) -> None:
        """
        Update world state by dt time units.

        Args:
            dt: Time step to advance
        """
        self.time += dt
        print(f"World updated: t = {self.time:.3f}")

        # Update dynamic objects (placeholder for future expansion)
        # For now, just print world status
        if self.objects:
            print(f"  - Objects in world: {len(self.objects)}")
        else:
            print(f"  - No objects in world")

    def get_sensory_percept(self) -> Dict[str, Any]:
        """
        Get agent's sensory perception of the world.

        Returns:
            Dictionary containing sensory information including:
            - vision: List of visible objects
            - audio: Audio information (placeholder)
            - internal: Internal agent state
            - timestamp: Current simulation time
        """
        percept = {
            'vision': self._get_vision_percept(),
            'audio': self._get_audio_percept(),
            'internal': self._get_internal_percept(),
            'timestamp': self.time,
        }
        return percept

    def _get_vision_percept(self) -> List[Dict[str, Any]]:
        """Get visual perception of the world."""
        visible_objects = []

        for obj in self.objects.values():
            # Simple vision: object is visible if within vision range
            distance = self._distance_to_agent(obj)
            if distance <= self.vision_width / 2:
                # Calculate angle
                agent_x, agent_y = self.agent_position
                obj_x, obj_y = obj.position
                angle = math.atan2(obj_y - agent_y, obj_x - agent_x)

                visible_objects.append({
                    'id': obj.id,
                    'type': obj.object_type,
                    'distance': distance,
                    'angle': angle,
                    'size': obj.size,
                })

        return visible_objects

    def _get_audio_percept(self) -> Dict[str, Any]:
        """Get auditory perception of the world."""
        # Placeholder for audio perception
        return {
            'sources': [],
            'volume': 0.0,
            'localization': None,
        }

    def _get_internal_percept(self) -> Dict[str, Any]:
        """Get internal sensory signals."""
        # Placeholder for internal signals
        return {
            'agent_position': self.agent_position,
            'agent_direction': self.agent_direction,
            'time': self.time,
        }

    def _distance_to_agent(self, obj: WorldObject) -> float:
        """Calculate distance from agent to object."""
        agent_x, agent_y = self.agent_position
        obj_x, obj_y = obj.position
        return math.sqrt((obj_x - agent_x) ** 2 + (obj_y - agent_y) ** 2)

    def apply_motor_command(self, action: Dict[str, Any], timestamp: float) -> None:
        """
        Apply a motor command to the world.

        Args:
            action: Action dictionary with action_type and parameters
            timestamp: Time when action was applied
        """
        action_type = action.get('action_type', 'move')

        print(f"Applying action '{action_type}' at t = {timestamp:.3f}")

        if action_type == 'move':
            self._process_movement_action(action)
        elif action_type == 'interact':
            self._process_interaction_action(action)
        elif action_type == 'wait':
            self._process_wait_action(action)
        else:
            print(f"Unknown action type: {action_type}")

    def _process_movement_action(self, action: Dict[str, Any]) -> None:
        """Process movement action."""
        direction = action.get('direction', 0.0)  # radians
        distance = action.get('distance', 1.0)

        dx = distance * math.cos(direction)
        dy = distance * math.sin(direction)

        new_x = self.agent_position[0] + dx
        new_y = self.agent_position[1] + dy

        # Keep agent within bounds
        new_x = max(0, min(new_x, self.width - 1))
        new_y = max(0, min(new_y, self.height - 1))

        old_pos = self.agent_position
        self.agent_position = (new_x, new_y)
        self.agent_direction = direction

        print(f"  - Moved from {old_pos} to {self.agent_position}")

    def _process_interaction_action(self, action: Dict[str, Any]) -> None:
        """Process interaction action."""
        target_type = action.get('target_type', 'object')

        print(f"  - Interacting with {target_type}")
        # Placeholder for interaction logic

    def _process_wait_action(self, action: Dict[str, Any]) -> None:
        """Process wait action."""
        duration = action.get('duration', 1.0)
        print(f"  - Waiting for {duration} time units")

    def get_simulation_time(self) -> float:
        """Get current simulation time."""
        return self.time

    def get_agent_state(self) -> Dict[str, Any]:
        """Get current agent state."""
        return {
            'position': self.agent_position,
            'direction': self.agent_direction,
            'time': self.time,
            'world_size': (self.width, self.height),
        }

    def __str__(self) -> str:
        """String representation."""
        return f"SimpleWorld({self.width}x{self.height}, t={self.time:.2f})"

    def __repr__(self) -> str:
        """Detailed string representation."""
        return (f"SimpleWorld(width={self.width}, height={self.height}, "
                f"vision=({self.vision_width}x{self.vision_height}), "
                f"objects={len(self.objects)})")
def create_world() -> SimpleWorld:
    """
    Create a new simple world.

    Returns:
        New SimpleWorld instance
    """
    return SimpleWorld()
def demo_world_environment() -> None:
    """
    Run a demonstration of basic world environment.

    This function shows how to create and use a simple world,
    demonstrating basic world functionality and agent interaction.
    """
    print("Running world environment demo...")

    # Create world
    world = create_world()

    # Configure world
    world.configure(width=50, height=50, vision_width=15, vision_height=15)

    # Add some objects
    player = WorldObject("player", (25, 25), (2, 2), "agent")
    world.add_object(player)

    treasure = WorldObject("treasure", (40, 40), (1, 1), "treasure")
    world.add_object(treasure)

    obstacle = WorldObject("obstacle", (10, 10), (3, 3), "wall")
    world.add_object(obstacle)

    # Show initial state
    print(f"\nInitial world state: {world}")
    percept = world.get_sensory_percept()
    print(f"Initial percept: {len(percept['vision'])} visible objects")

    # Update world
    world.update(1.0)

    # Apply an action
    action = {'action_type': 'move', 'direction': 0.0, 'distance': 5.0}
    world.apply_motor_command(action, world.get_simulation_time())

    # Get new percept
    percept = world.get_sensory_percept()
    print(f"Percept after move: {len(percept['vision'])} visible objects")

    print("Demo completed successfully!")