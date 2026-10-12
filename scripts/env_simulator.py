#!/usr/bin/env python3
"""
Environment Simulator

Advanced environment simulation tools for multi-agent systems and dynamic worlds.

This module provides comprehensive tools for creating and managing custom world
environments with support for dynamic events, multi-agent interactions, and
environment parameter tuning.

Features:
- Custom world environments
- Dynamic environments with events
- Multi-agent environments
- Environment parameter tuning
- Environment performance metrics

Usage:
    python scripts/env_simulator.py --create --type grid --width 20 --height 20
    python scripts/env_simulator.py --simulate --config world.json --steps 1000
    python scripts/env_simulator.py --tune --metric efficiency --iterations 50
"""

import sys
from pathlib import Path
import json
import copy
from typing import Dict, List, Any, Optional, Union, Tuple, Set
import numpy as np
from enum import Enum
import random
import warnings
import math

# Add parent directory to path for imports
sys.path.insert(0, str(Path(__file__).parent.parent.parent))

from nlm.models import BaseNeuralModel
from nlm.memory import WorkingMemory
from nlm.neuromodulation import NeuromodulationSystem


class EnvironmentType(Enum):
    """Types of simulated environments."""
    GRID = "grid"
    CONTINUOUS = "continuous"
    SPATIAL = "spatial"
    SOCIAL = "social"
    RESOURCE = "resource"
    MULTI_AGENT = "multi_agent"
    DYNAMIC = "dynamic"
    MIXED = "mixed"


class EventType(Enum):
    """Types of environmental events."""
    OBJECT_SPAWN = "object_spawn"
    OBJECT_REMOVAL = "object_removal"
    AGENT_SPAWN = "agent_spawn"
    AGENT_DEATH = "agent_death"
    WEATHER_CHANGE = "weather_change"
    RESOURCE_UPDATE = "resource_update"
    PREDATOR_SPAWN = "predator_spawn"
    AREA_RESTRICTION = "area_restriction"
    TEMPERATURE_CHANGE = "temperature_change"
    PHENOMENA = "phenomena"


class CellType(Enum):
    """Types of cells in grid environments."""
    EMPTY = "empty"
    WALL = "wall"
    AGENT = "agent"
    FOOD = "food"
    WATER = "water"
    SHELTER = "shelter"
    RESOURCE = "resource"
    OBSTACLE = "obstacle"
    PREDATOR = "predator"
    TRAPPER = "trapper"


@dataclass
class Position:
    """Represents a position in 2D or 3D space."""
    x: float
    y: float
    z: Optional[float] = None
    
    def distance_to(self, other: 'Position', metric: str = "euclidean") -> float:
        """Calculate distance to another position."""
        dx = self.x - other.x
        dy = self.y - other.y
        if self.z is not None and other.z is not None:
            if metric == "euclidean":
                return math.sqrt(dx**2 + dy**2 + (self.z - other.z)**2)
            elif metric == "manhattan":
                return abs(dx) + abs(dy) + abs(self.z - other.z)
        else:
            if metric == "euclidean":
                return math.sqrt(dx**2 + dy**2)
            elif metric == "manhattan":
                return abs(dx) + abs(dy)
        return 0.0


@dataclass
class GameObject:
    """Base class for objects in the environment."""
    id: str
    name: str
    position: Position
    type: str
    properties: Dict[str, Any] = field(default_factory=dict)
    metadata: Dict[str, Any] = field(default_factory=dict)


@dataclass
class Agent:
    """Represents an agent in the environment."""
    id: str
    name: str
    position: Position
    agent_type: str
    health: float = 100.0
    energy: float = 100.0
    memory: WorkingMemory
    neuromodulation: NeuromodulationSystem
    capabilities: List[str] = field(default_factory=list)
    properties: Dict[str, Any] = field(default_factory=dict)
    metadata: Dict[str, Any] = field(default_factory=dict)


@dataclass
class EnvironmentEvent:
    """Represents an environmental event."""
    id: str
    type: EventType
    timestamp: float
    position: Optional[Position] = None
    parameters: Dict[str, Any] = field(default_factory=dict)
    affected_agents: List[str] = field(default_factory=list)
    description: str = ""
    duration: Optional[float] = None
    metadata: Dict[str, Any] = field(default_factory=dict)


class WorldEnvironment:
    """Advanced environment simulator for multi-agent systems."""

    def __init__(self, config: Optional[Dict[str, Any]] = None):
        """Initialize the environment simulator.
        
        Args:
            config: Environment configuration dictionary
        """
        self.config = config or self._get_default_config()
        self.agents = {}
        self.objects = {}
        self.events = []
        self.time_step = 0
        self.global_variables = {}
        self.weather = {}
        self.resources = {}
        self.boundaries = {}
        self.cell_map = {}  # For grid environments
        self.observation_grid = {}
        self.performance_metrics = {}
        
        # Initialize environment based on type
        self._initialize_environment()
        self._setup_dynamic_systems()

    def _get_default_config(self) -> Dict[str, Any]:
        """Get default environment configuration."""
        return {
            "type": EnvironmentType.GRID,
            "width": 100,
            "height": 100,
            "depth": 10,
            "cell_size": 1.0,
            "time_step_duration": 1.0,
            "random_seed": 42,
            "weather_enabled": True,
            "event_enabled": True,
            "agent_max_health": 100.0,
            "agent_max_energy": 100.0,
            "resource_regeneration_rate": 0.01,
            "dynamic_boundaries": False,
            "noise_level": 0.1,
            "physics_enabled": True,
            "gravity": 9.8,
            "friction": 0.5,
            "perception_range": 10.0,
            "communication_range": 5.0,
            "environment_difficulty": "medium"
        }

    def _initialize_environment(self):
        """Initialize environment based on type."""
        env_type = self.config["type"]
        
        if env_type == EnvironmentType.GRID:
            self._initialize_grid_environment()
        elif env_type == EnvironmentType.CONTINUOUS:
            self._initialize_continuous_environment()
        elif env_type == EnvironmentType.SPATIAL:
            self._initialize_spatial_environment()
        elif env_type == EnvironmentType.SOCIAL:
            self._initialize_social_environment()
        elif env_type == EnvironmentType.RESOURCE:
            self._initialize_resource_environment()
        elif env_type == EnvironmentType.MULTI_AGENT:
            self._initialize_multi_agent_environment()
        elif env_type == EnvironmentType.DYNAMIC:
            self._initialize_dynamic_environment()
        else:
            self._initialize_grid_environment()
        
        # Set random seed
        random.seed(self.config.get("random_seed", 42))
        np.random.seed(self.config.get("random_seed", 42))

    def _initialize_grid_environment(self):
        """Initialize a grid-based environment."""
        width = self.config["width"]
        height = self.config["height"]
        cell_size = self.config["cell_size"]
        
        # Initialize cell map
        self.cell_map = {}
        for x in range(int(width / cell_size)):
            for y in range(int(height / cell_size)):
                self.cell_map[(x, y)] = CellType.EMPTY
        
        # Initialize boundaries
        self.boundaries = {
            "x_min": 0,
            "x_max": width,
            "y_min": 0,
            "y_max": height
        }
        
        # Set up initial environment objects
        self._setup_environment_objects()

    def _initialize_continuous_environment(self):
        """Initialize a continuous environment."""
        self.boundaries = {
            "x_min": -100,
            "x_max": 100,
            "y_min": -100,
            "y_max": 100,
            "z_min": -50,
            "z_max": 50
        }
        
        self._setup_environment_objects()

    def _initialize_spatial_environment(self):
        """Initialize a 3D spatial environment."""
        width = self.config["width"]
        height = self.config["height"]
        depth = self.config["depth"]
        
        self.boundaries = {
            "x_min": 0,
            "x_max": width,
            "y_min": 0,
            "y_max": height,
            "z_min": 0,
            "z_max": depth
        }
        
        self._setup_environment_objects()

    def _initialize_social_environment(self):
        """Initialize a social environment with focus on interactions."""
        self.boundaries = {
            "x_min": 0,
            "x_max": 50,
            "y_min": 0,
            "y_max": 50
        }
        
        self.config["social_rules"] = {
            "cooperation_benefit": 0.8,
            "competition_cost": 0.3,
            "territory_size": 5,
            "communication_efficiency": 0.7
        }
        
        self._setup_environment_objects()

    def _initialize_resource_environment(self):
        """Initialize a resource-focused environment."""
        self.boundaries = {
            "x_min": 0,
            "x_max": 80,
            "y_min": 0,
            "y_max": 80
        }
        
        self.config["resource_types"] = {
            "food": {"spawn_rate": 0.02, "regeneration_rate": 0.01, "value": 10},
            "water": {"spawn_rate": 0.03, "regeneration_rate": 0.02, "value": 5},
            "energy": {"spawn_rate": 0.01, "regeneration_rate": 0.005, "value": 20},
            "building_materials": {"spawn_rate": 0.005, "regeneration_rate": 0.001, "value": 50}
        }
        
        self._setup_environment_objects()

    def _initialize_multi_agent_environment(self):
        """Initialize a multi-agent focused environment."""
        self.boundaries = {
            "x_min": 0,
            "x_max": 100,
            "y_min": 0,
            "y_max": 100
        }
        
        self.config["agent_spawn_rates"] = {
            "explorer": 0.3,
            "collector": 0.2,
            "defender": 0.2,
            "coordinator": 0.15,
            "specialist": 0.15
        }
        
        self._setup_environment_objects()

    def _initialize_dynamic_environment(self):
        """Initialize a dynamic environment with changing conditions."""
        self.boundaries = {
            "x_min": 0,
            "x_max": 100,
            "y_min": 0,
            "y_max": 100
        }
        
        self.config["dynamic_patterns"] = {
            "seasonal_cycles": True,
            "weather_patterns": True,
            "population_dynamics": True,
            "resource_cycling": True
        }
        
        self._setup_environment_objects()

    def _setup_environment_objects(self):
        """Set up initial environment objects."""
        # Place walls/boundaries
        self._place_environment_boundaries()
        
        # Place resources
        self._place_resources()
        
        # Place shelters
        self._place_shelters()
        
        # Place obstacles
        self._place_obstacles()

    def _place_environment_boundaries(self):
        """Place environment boundaries."""
        x_min, x_max = self.boundaries["x_min"], self.boundaries["x_max"]
        y_min, y_max = self.boundaries["y_min"], self.boundaries["y_max"]
        cell_size = self.config["cell_size"]
        
        # Create walls around the perimeter
        for x in range(int(x_min / cell_size), int(x_max / cell_size)):
            for y in range(int(y_min / cell_size), int(y_min / cell_size) + 5):
                self.cell_map[(x, y)] = CellType.WALL
            for y in range(int(y_max / cell_size) - 4, int(y_max / cell_size)):
                self.cell_map[(x, y)] = CellType.WALL
        
        for y in range(int(y_min / cell_size), int(y_max / cell_size)):
            for x in range(int(x_min / cell_size), int(x_min / cell_size) + 5):
                self.cell_map[(x, y)] = CellType.WALL
            for x in range(int(x_max / cell_size) - 4, int(x_max / cell_size)):
                self.cell_map[(x, y)] = CellType.WALL

    def _place_resources(self):
        """Place initial resources in the environment."""
        resource_types = self.config.get("resource_types", {})
        
        for resource_type, params in resource_types.items():
            spawn_rate = params.get("spawn_rate", 0.01)
            for _ in range(int(self.config["width"] * self.config["height"] * spawn_rate)):
                x = random.uniform(self.boundaries["x_min"], self.boundaries["x_max"])
                y = random.uniform(self.boundaries["y_min"], self.boundaries["y_max"])
                
                resource = GameObject(
                    id=f"{resource_type}_{len(self.objects)}",
                    name=f"{resource_type.title()} {len(self.objects)}",
                    position=Position(x, y),
                    type=resource_type,
                    properties={
                        "value": params.get("value", 10),
                        "quantity": params.get("initial_quantity", 1),
                        "regeneration_rate": params.get("regeneration_rate", 0.01)
                    }
                )
                
                self.objects[resource.id] = resource

    def _place_shelters(self):
        """Place shelters in the environment."""
        shelter_count = int(self.config["width"] * self.config["height"] * 0.02)
        
        for _ in range(shelter_count):
            x = random.uniform(self.boundaries["x_min"], self.boundaries["x_max"])
            y = random.uniform(self.boundaries["y_min"], self.boundaries["y_max"])
            
            shelter = GameObject(
                id=f"shelter_{len(self.objects)}",
                name=f"Shelter {len(self.objects)}",
                position=Position(x, y),
                type="shelter",
                properties={
                    "capacity": 10,
                    "protection_level": 0.8,
                    "resource_bonus": 1.5
                }
            )
            
            self.objects[shelter.id] = shelter

    def _place_obstacles(self):
        """Place obstacles in the environment."""
        obstacle_count = int(self.config["width"] * self.config["height"] * 0.05)
        
        for _ in range(obstacle_count):
            x = random.uniform(self.boundaries["x_min"], self.boundaries["x_max"])
            y = random.uniform(self.boundaries["y_min"], self.boundaries["y_max"])
            
            obstacle = GameObject(
                id=f"obstacle_{len(self.objects)}",
                name=f"Obstacle {len(self.objects)}",
                position=Position(x, y),
                type="obstacle",
                properties={
                    "size": random.uniform(1.0, 3.0),
                    "passable": False,
                    "difficulty": random.uniform(0.5, 0.9)
                }
            )
            
            self.objects[obstacle.id] = obstacle

    def _setup_dynamic_systems(self):
        """Set up dynamic systems and weather."""
        if self.config.get("weather_enabled", True):
            self._setup_weather_system()
        
        if self.config.get("event_enabled", True):
            self._setup_event_system()

    def _setup_weather_system(self):
        """Set up weather system."""
        self.weather = {
            "temperature": random.uniform(15, 35),
            "humidity": random.uniform(30, 80),
            "wind_speed": random.uniform(0, 10),
            "precipitation": 0.0,
            "visibility": random.uniform(5, 20),
            "pressure": random.uniform(980, 1040),
            "season": random.choice(["spring", "summer", "autumn", "winter"]),
            "weather_type": random.choice(["sunny", "cloudy", "rainy", "stormy"])
        }

    def _setup_event_system(self):
        """Set up event system for dynamic events."""
        self.events = []
        self.event_cooldown = {}

    def add_agent(self, agent: Agent) -> None:
        """Add an agent to the environment.
        
        Args:
            agent: Agent to add
        """
        self.agents[agent.id] = agent
        
        # Update cell map
        if self.config["type"] == EnvironmentType.GRID:
            cell_x = int(agent.position.x / self.config["cell_size"])
            cell_y = int(agent.position.y / self.config["cell_size"])
            self.cell_map[(cell_x, cell_y)] = CellType.AGENT

    def remove_agent(self, agent_id: str) -> bool:
        """Remove an agent from the environment.
        
        Args:
            agent_id: ID of agent to remove
            
        Returns:
            True if agent was removed, False if not found
        """
        if agent_id in self.agents:
            # Update cell map
            if self.config["type"] == EnvironmentType.GRID:
                agent = self.agents[agent_id]
                cell_x = int(agent.position.x / self.config["cell_size"])
                cell_y = int(agent.position.y / self.config["cell_size"])
                self.cell_map[(cell_x, cell_y)] = CellType.EMPTY
            
            del self.agents[agent_id]
            return True
        return False

    def add_object(self, obj: GameObject) -> None:
        """Add an object to the environment.
        
        Args:
            obj: Object to add
        """
        self.objects[obj.id] = obj

    def remove_object(self, object_id: str) -> bool:
        """Remove an object from the environment.
        
        Args:
            object_id: ID of object to remove
            
        Returns:
            True if object was removed, False if not found
        """
        if object_id in self.objects:
            del self.objects[object_id]
            return True
        return False

    def get_cell_at(self, position: Position) -> CellType:
        """Get cell type at position.
        
        Args:
            position: Position to check
            
        Returns:
            Cell type at position
        """
        if self.config["type"] == EnvironmentType.GRID:
            cell_x = int(position.x / self.config["cell_size"])
            cell_y = int(position.y / self.config["cell_size"])
            return self.cell_map.get((cell_x, cell_y), CellType.EMPTY)
        else:
            return CellType.EMPTY

    def set_cell_at(self, position: Position, cell_type: CellType) -> None:
        """Set cell type at position.
        
        Args:
            position: Position to set
            cell_type: Cell type to set
        """
        if self.config["type"] == EnvironmentType.GRID:
            cell_x = int(position.x / self.config["cell_size"])
            cell_y = int(position.y / self.config["cell_size"])
            self.cell_map[(cell_x, cell_y)] = cell_type

    def update_agent_position(self, agent_id: str, new_position: Position) -> bool:
        """Update agent position in the environment.
        
        Args:
            agent_id: ID of agent
            new_position: New position
            
        Returns:
            True if position was updated, False if invalid
        """
        if agent_id not in self.agents:
            return False
        
        # Check if position is within bounds
        if not self._is_position_valid(new_position):
            return False
        
        old_agent = self.agents[agent_id]
        old_position = old_agent.position
        
        # Update position
        old_agent.position = new_position
        
        # Update cell map
        if self.config["type"] == EnvironmentType.GRID:
            old_cell_x = int(old_position.x / self.config["cell_size"])
            old_cell_y = int(old_position.y / self.config["cell_size"])
            self.cell_map[(old_cell_x, old_cell_y)] = CellType.EMPTY
            
            new_cell_x = int(new_position.x / self.config["cell_size"])
            new_cell_y = int(new_position.y / self.config["cell_size"])
            self.cell_map[(new_cell_x, new_cell_y)] = CellType.AGENT
        
        return True

    def _is_position_valid(self, position: Position) -> bool:
        """Check if position is valid.
        
        Args:
            position: Position to check
            
        Returns:
            True if position is valid, False otherwise
        """
        # Check if within boundaries
        if (position.x < self.boundaries["x_min"] or 
            position.x > self.boundaries["x_max"] or
            position.y < self.boundaries["y_min"] or 
            position.y > self.boundaries["y_max"]):
            return False
        
        # Check if cell is passable (for grid environments)
        if self.config["type"] == EnvironmentType.GRID:
            cell_type = self.get_cell_at(position)
            if cell_type == CellType.WALL or cell_type == CellType.OBSTACLE:
                return False
        
        return True

    def simulate_step(self, actions: Dict[str, Any]) -> Dict[str, Any]:
        """Simulate one step of the environment.
        
        Args:
            actions: Dictionary of agent actions (agent_id: action)
            
        Returns:
            Simulation results
        """
        step_results = {
            "time_step": self.time_step,
            "events": [],
            "agent_states": {},
            "object_changes": [],
            "environment_changes": {}
        }
        
        # Process weather updates
        if self.config.get("weather_enabled", True):
            weather_changes = self._update_weather()
            step_results["environment_changes"]["weather"] = weather_changes
        
        # Process events
        if self.config.get("event_enabled", True):
            events = self._process_events()
            step_results["events"].extend(events)
        
        # Process agent actions
        for agent_id, action in actions.items():
            if agent_id in self.agents:
                agent_result = self._process_agent_action(self.agents[agent_id], action)
                step_results["agent_states"][agent_id] = agent_result
        
        # Update environment objects
        object_changes = self._update_environment_objects()
        step_results["object_changes"] = object_changes
        
        # Increment time step
        self.time_step += 1
        
        # Update observation grid
        self._update_observation_grid()
        
        return step_results

    def _update_weather(self) -> Dict[str, Any]:
        """Update weather conditions."""
        changes = {}
        
        # Update temperature
        temp_change = random.uniform(-2, 2)
        self.weather["temperature"] += temp_change
        changes["temperature"] = {"old": self.weather["temperature"] - temp_change, "new": self.weather["temperature"]}
        
        # Update humidity
        humidity_change = random.uniform(-5, 5)
        self.weather["humidity"] += humidity_change
        self.weather["humidity"] = max(0, min(100, self.weather["humidity"]))
        changes["humidity"] = {"old": self.weather["humidity"] - humidity_change, "new": self.weather["humidity"]}
        
        # Update wind speed
        wind_change = random.uniform(-1, 1)
        self.weather["wind_speed"] += wind_change
        self.weather["wind_speed"] = max(0, self.weather["wind_speed"])
        changes["wind_speed"] = {"old": self.weather["wind_speed"] - wind_change, "new": self.weather["wind_speed"]}
        
        # Chance of precipitation
        if random.random() < 0.1:
            precipitation = random.uniform(0, 10)
            self.weather["precipitation"] = precipitation
            changes["precipitation"] = {"old": 0, "new": precipitation}
        
        return changes

    def _process_events(self) -> List[EnvironmentEvent]:
        """Process environmental events."""
        events = []
        
        # Spawn random events
        if random.random() < 0.05:  # 5% chance of event
            event_type = random.choice(list(EventType))
            event = self._create_random_event(event_type)
            events.append(event)
            self.events.append(event)
        
        # Clean up old events
        self.events = [event for event in self.events if event.duration is None or event.duration > 0]
        for event in self.events:
            if event.duration is not None:
                event.duration -= 1.0
        
        return events

    def _create_random_event(self, event_type: EventType) -> EnvironmentEvent:
        """Create a random event of the specified type."""
        event_id = f"event_{len(self.events)}_{int(time.time())}"
        
        # Generate random event parameters
        if event_type == EventType.OBJECT_SPAWN:
            position = Position(
                random.uniform(self.boundaries["x_min"], self.boundaries["x_max"]),
                random.uniform(self.boundaries["y_min"], self.boundaries["y_max"])
            )
            event = EnvironmentEvent(
                id=event_id,
                type=event_type,
                timestamp=self.time_step,
                position=position,
                parameters={"object_type": random.choice(["food", "water", "shelter"]), "quantity": random.randint(1, 5)},
                affected_agents=[],
                description=f"Spawned {event_type.value} at ({position.x:.1f}, {position.y:.1f})"
            )
        elif event_type == EventType.AGENT_SPAWN:
            position = Position(
                random.uniform(self.boundaries["x_min"], self.boundaries["x_max"]),
                random.uniform(self.boundaries["y_min"], self.boundaries["y_max"])
            )
            agent_type = random.choice(list(self.config["agent_spawn_rates"].keys()))
            event = EnvironmentEvent(
                id=event_id,
                type=event_type,
                timestamp=self.time_step,
                position=position,
                parameters={"agent_type": agent_type},
                affected_agents=[],
                description=f"Spawned {agent_type} agent at ({position.x:.1f}, {position.y:.1f})"
            )
        elif event_type == EventType.WEATHER_CHANGE:
            event = EnvironmentEvent(
                id=event_id,
                type=event_type,
                timestamp=self.time_step,
                position=None,
                parameters={"weather_type": random.choice(["sunny", "cloudy", "rainy", "stormy"])},
                affected_agents=[],
                description=f"Weather changed to {event.parameters['weather_type']}",
                duration=100.0
            )
        else:
            # Generic event
            event = EnvironmentEvent(
                id=event_id,
                type=event_type,
                timestamp=self.time_step,
                position=None,
                parameters={},
                affected_agents=[],
                description=f"{event_type.value} occurred"
            )
        
        return event

    def _process_agent_action(self, agent: Agent, action: Dict[str, Any]) -> Dict[str, Any]:
        """Process an agent's action.
        
        Args:
            agent: Agent performing action
            action: Action dictionary
            
        Returns:
            Action result
        """
        result = {
            "agent_id": agent.id,
            "action": action,
            "success": False,
            "new_position": agent.position,
            "energy_change": 0.0,
            "health_change": 0.0,
            "observations": [],
            "interactions": []
        }
        
        # Parse action
        action_type = action.get("type", "move")
        action_params = action.get("parameters", {})
        
        # Execute action
        if action_type == "move":
            result = self._execute_move_action(agent, action_params, result)
        elif action_type == "collect":
            result = self._execute_collect_action(agent, action_params, result)
        elif action_type == "interact":
            result = self._execute_interact_action(agent, action_params, result)
        elif action_type == "wait":
            result = self._execute_wait_action(agent, action_params, result)
        else:
            # Default move action
            result = self._execute_move_action(agent, action_params, result)
        
        return result

    def _execute_move_action(self, agent: Agent, params: Dict[str, Any], result: Dict[str, Any]) -> Dict[str, Any]:
        """Execute move action."""
        # Calculate target position
        dx = params.get("dx", 0.0)
        dy = params.get("dy", 0.0)
        
        new_x = agent.position.x + dx
        new_y = agent.position.y + dy
        
        new_position = Position(new_x, new_y)
        
        # Check if position is valid
        if self._is_position_valid(new_position):
            # Update agent position
            self.update_agent_position(agent.id, new_position)
            result["success"] = True
            result["new_position"] = new_position
            
            # Calculate energy cost
            distance = agent.position.distance_to(new_position)
            energy_cost = distance * 0.1  # Energy cost proportional to distance
            
            agent.energy = max(0, agent.energy - energy_cost)
            result["energy_change"] = -energy_cost
            
            # Check for interactions
            interactions = self._check_interactions(agent.id, new_position)
            result["interactions"] = interactions
        
        return result

    def _execute_collect_action(self, agent: Agent, params: Dict[str, Any], result: Dict[str, Any]) -> Dict[str, Any]:
        """Execute collect action."""
        # Collect nearby resources
        objects_to_collect = self._get_nearby_objects(agent.id, radius=params.get("radius", 2.0))
        
        for obj_id in objects_to_collect:
            if obj_id in self.objects:
                obj = self.objects[obj_id]
                # Add to agent's memory/properties
                if "collected_objects" not in agent.properties:
                    agent.properties["collected_objects"] = []
                
                agent.properties["collected_objects"].append(obj_id)
                
                # Remove object from environment
                self.remove_object(obj_id)
                
                # Update energy based on object value
                obj_value = obj.properties.get("value", 10)
                agent.energy = min(agent.config["agent_max_energy"], agent.energy + obj_value * 0.1)
                
                result["interactions"].append({
                    "type": "collect",
                    "object_id": obj_id,
                    "object_type": obj.type,
                    "value": obj_value
                })
        
        result["success"] = len(objects_to_collect) > 0
        return result

    def _execute_interact_action(self, agent: Agent, params: Dict[str, Any], result: Dict[str, Any]) -> Dict[str, Any]:
        """Execute interact action."""
        # Look for nearby agents or objects to interact with
        interaction_radius = params.get("radius", 3.0)
        nearby_agents = self._get_nearby_agents(agent.id, interaction_radius)
        nearby_objects = self._get_nearby_objects(agent.id, interaction_radius)
        
        interactions = []
        
        # Interact with nearby agents (social interaction)
        for nearby_agent_id in nearby_agents:
            if nearby_agent_id in self.agents:
                nearby_agent = self.agents[nearby_agent_id]
                
                # Simple cooperation/competition logic
                if random.random() < 0.5:  # 50% chance of cooperation
                    # Cooperation: share resources/energy
                    shared_energy = min(agent.energy * 0.3, nearby_agent.energy * 0.3)
                    agent.energy += shared_energy * 0.5
                    nearby_agent.energy += shared_energy * 0.5
                    
                    interactions.append({
                        "type": "cooperation",
                        "partner_id": nearby_agent_id,
                        "energy_shared": shared_energy
                    })
                else:
                    # Competition: fight for resources
                    energy_loss = min(agent.energy * 0.2, nearby_agent.energy * 0.2)
                    agent.energy -= energy_loss
                    nearby_agent.energy -= energy_loss
                    
                    interactions.append({
                        "type": "competition",
                        "partner_id": nearby_agent_id,
                        "energy_lost": energy_loss
                    })
        
        # Interact with nearby objects
        for obj_id in nearby_objects:
            if obj_id in self.objects:
                obj = self.objects[obj_id]
                interactions.append({
                    "type": "object_interaction",
                    "object_id": obj_id,
                    "object_type": obj.type
                })
        
        result["success"] = len(interactions) > 0
        result["interactions"] = interactions
        return result

    def _execute_wait_action(self, agent: Agent, params: Dict[str, Any], result: Dict[str, Any]) -> Dict[str, Any]:n        """Execute wait action."""
        # Agent does nothing for this step
        result["success"] = True
        
        # Regenerate some energy
        energy_regen = 1.0
        agent.energy = min(agent.config["agent_max_energy"], agent.energy + energy_regen)
        result["energy_change"] = energy_regen
        
        return result

    def _check_interactions(self, agent_id: str, position: Position) -> List[Dict[str, Any]]:
        """Check for interactions at a position."""
        interactions = []
        
        # Check for nearby agents
        nearby_agents = self._get_nearby_agents(agent_id, radius=1.0)
        for nearby_agent_id in nearby_agents:
            if nearby_agent_id != agent_id:
                interactions.append({
                    "type": "agent_proximity",
                    "agent_id": nearby_agent_id
                })
        
        # Check for nearby objects
        nearby_objects = self._get_nearby_objects(agent_id, radius=1.0)
        for obj_id in nearby_objects:
            if obj_id in self.objects:
                obj = self.objects[obj_id]
                interactions.append({
                    "type": "object_proximity",
                    "object_id": obj_id,
                    "object_type": obj.type
                })
        
        return interactions

    def _get_nearby_agents(self, agent_id: str, radius: float) -> List[str]:
        """Get IDs of agents within radius.
        
        Args:
            agent_id: ID of reference agent
            radius: Search radius
            
        Returns:
            List of agent IDs within radius
        """
        if agent_id not in self.agents:
            return []
        
        reference_agent = self.agents[agent_id]
        nearby_agents = []
        
        for other_id, other_agent in self.agents.items():
            if other_id != agent_id:
                distance = reference_agent.position.distance_to(other_agent.position)
                if distance <= radius:
                    nearby_agents.append(other_id)
        
        return nearby_agents

    def _get_nearby_objects(self, agent_id: str, radius: float) -> List[str]:
        """Get IDs of objects within radius.
        
        Args:
            agent_id: ID of reference agent
            radius: Search radius
            
        Returns:
            List of object IDs within radius
        """
        if agent_id not in self.agents:
            return []
        
        reference_agent = self.agents[agent_id]
        nearby_objects = []
        
        for obj_id, obj in self.objects.items():
            distance = reference_agent.position.distance_to(obj.position)
            if distance <= radius:
                nearby_objects.append(obj_id)
        
        return nearby_objects

    def _update_environment_objects(self) -> List[Dict[str, Any]]:
        """Update environment objects (resources, etc.)."""
        changes = []
        
        # Update resources
        for obj_id, obj in list(self.objects.items()):
            if obj.type in ["food", "water", "energy"]:
                # Regenerate resources
                regeneration_rate = obj.properties.get("regeneration_rate", 0.01)
                if random.random() < regeneration_rate:
                    # Increase quantity slightly
                    current_quantity = obj.properties.get("quantity", 1)
                    obj.properties["quantity"] = min(current_quantity + 1, 10)
                    changes.append({
                        "type": "resource_regeneration",
                        "object_id": obj_id,
                        "quantity_change": 1
                    })
        
        # Update weather
        if self.config.get("weather_enabled", True):
            # Occasionally change weather type
            if random.random() < 0.01:  # 1% chance per step
                new_weather_type = random.choice(["sunny", "cloudy", "rainy", "stormy"])
                if new_weather_type != self.weather["weather_type"]:
                    changes.append({
                        "type": "weather_change",
                        "old_weather": self.weather["weather_type"],
                        "new_weather": new_weather_type
                    })
                    self.weather["weather_type"] = new_weather_type
        
        return changes

    def _update_observation_grid(self):
        """Update observation grid for agent perception."""
        self.observation_grid = {}
        
        perception_range = self.config.get("perception_range", 10.0)
        
        for agent_id, agent in self.agents.items():
            # Create observation for each agent
            agent_observations = {
                "position": {"x": agent.position.x, "y": agent.position.y},
                "nearby_agents": [],
                "nearby_objects": [],
                "weather": self.weather
            }
            
            # Add nearby agents
            nearby_agents = self._get_nearby_agents(agent_id, perception_range)
            for nearby_agent_id in nearby_agents:
                if nearby_agent_id in self.agents:
                    nearby_agent = self.agents[nearby_agent_id]
                    agent_observations["nearby_agents"].append({
                        "id": nearby_agent.id,
                        "type": nearby_agent.agent_type,
                        "position": {"x": nearby_agent.position.x, "y": nearby_agent.position.y},
                        "health": nearby_agent.health,
                        "energy": nearby_agent.energy
                    })
            
            # Add nearby objects
            nearby_objects = self._get_nearby_objects(agent_id, perception_range)
            for obj_id in nearby_objects:
                if obj_id in self.objects:
                    obj = self.objects[obj_id]
                    agent_observations["nearby_objects"].append({
                        "id": obj.id,
                        "type": obj.type,
                        "position": {"x": obj.position.x, "y": obj.position.y},
                        "properties": obj.properties
                    })
            
            self.observation_grid[agent_id] = agent_observations

    def get_agent_observation(self, agent_id: str) -> Dict[str, Any]:
        """Get observation for a specific agent.
        
        Args:
            agent_id: ID of agent
            
        Returns:
            Agent observation
        """
        return self.observation_grid.get(agent_id, {})

    def get_environment_state(self) -> Dict[str, Any]:
        """Get current environment state.
        
        Returns:
            Environment state
        """
        return {
            "time_step": self.time_step,
            "agents": {agent_id: {
                "id": agent.id,
                "name": agent.name,
                "type": agent.agent_type,
                "position": {"x": agent.position.x, "y": agent.position.y},
                "health": agent.health,
                "energy": agent.energy,
                "capabilities": agent.capabilities,
                "properties": agent.properties
            } for agent_id, agent in self.agents.items()},
            "objects": {obj_id: {
                "id": obj.id,
                "name": obj.name,
                "type": obj.type,
                "position": {"x": obj.position.x, "y": obj.position.y},
                "properties": obj.properties
            } for obj_id, obj in self.objects.items()},
            "weather": self.weather,
            "events": [{k: v for k, v in event.__dict__.items() if not k.startswith('_')} for event in self.events],
            "boundaries": self.boundaries,
            "cell_map": {k: v.value for k, v in self.cell_map.items()},
            "performance_metrics": self.performance_metrics
        }

    def set_environment_state(self, state: Dict[str, Any]) -> None:
        """Set environment state from dictionary.
        
        Args:
            state: Environment state dictionary
        """
        self.time_step = state.get("time_step", 0)
        
        # Clear existing state
        self.agents.clear()
        self.objects.clear()
        self.events.clear()
        self.cell_map.clear()
        
        # Restore agents
        for agent_id, agent_data in state.get("agents", {}).items():
            agent = Agent(
                id=agent_data["id"],
                name=agent_data["name"],
                position=Position(
                    agent_data["position"]["x"],
                    agent_data["position"]["y"]
                ),
                agent_type=agent_data["type"],
                health=agent_data.get("health", 100.0),
                energy=agent_data.get("energy", 100.0),
                memory=agent_data.get("memory", None),
                neuromodulation=agent_data.get("neuromodulation", None),
                capabilities=agent_data.get("capabilities", []),
                properties=agent_data.get("properties", {}),
                metadata=agent_data.get("metadata", {})
            )
            self.agents[agent_id] = agent
        
        # Restore objects
        for obj_id, obj_data in state.get("objects", {}).items():
            obj = GameObject(
                id=obj_data["id"],
                name=obj_data["name"],
                position=Position(
                    obj_data["position"]["x"],
                    obj_data["position"]["y"]
                ),
                type=obj_data["type"],
                properties=obj_data.get("properties", {}),
                metadata=obj_data.get("metadata", {})
            )
            self.objects[obj_id] = obj
        
        # Restore weather
        self.weather = state.get("weather", {})
        
        # Restore cell map
        cell_map_data = state.get("cell_map", {})
        for pos_str, cell_type_str in cell_map_data.items():
            pos = eval(pos_str)  # Simple conversion from string
            self.cell_map[pos] = CellType(cell_type_str)

    def calculate_performance_metrics(self) -> Dict[str, float]:
        """Calculate environment performance metrics.
        
        Returns:
            Performance metrics
        """
        metrics = {}
        
        if not self.agents:
            return metrics
        
        # Agent statistics
        metrics["agent_count"] = len(self.agents)
        metrics["avg_agent_health"] = np.mean([agent.health for agent in self.agents.values()])
        metrics["avg_agent_energy"] = np.mean([agent.energy for agent in self.agents.values()])
        
        # Object statistics
        metrics["object_count"] = len(self.objects)
        metrics["resource_count"] = len([obj for obj in self.objects.values() if obj.type in ["food", "water", "energy"]])
        
        # Survival rate
        dead_agents = sum(1 for agent in self.agents.values() if agent.health <= 0)
        metrics["survival_rate"] = (len(self.agents) - dead_agents) / len(self.agents) if self.agents else 0
        
        # Resource availability
        total_resources = sum(obj.properties.get("quantity", 0) for obj in self.objects.values())
        metrics["resource_availability"] = total_resources / max(1, len([obj for obj in self.objects.values() if obj.type in ["food", "water", "energy"]]))
        
        # Weather stability
        weather_stability = 0.1  # Base stability
        if "precipitation" in self.weather:
            weather_stability -= self.weather["precipitation"] * 0.01
        metrics["weather_stability"] = max(0, weather_stability)
        
        # Environmental complexity
        metrics["environmental_complexity"] = len(self.events) / (self.time_step + 1)
        
        # Store metrics
        self.performance_metrics = metrics
        
        return metrics

    def export_environment(self, file_path: str) -> None:
        """Export environment to file.
        
        Args:
            file_path: Path to export file
        """
        state = self.get_environment_state()
        
        with open(file_path, 'w') as f:
            json.dump(state, f, indent=2, default=str)

    def import_environment(self, file_path: str) -> None:
        """Import environment from file.
        
        Args:
            file_path: Path to import file
            
        Raises:
            FileNotFoundError: If file does not exist
            ValueError: If file format is invalid
        """
        if not Path(file_path).exists():
            raise FileNotFoundError(f"Environment file not found: {file_path}")
        
        with open(file_path, 'r') as f:
            state = json.load(f)
        
        self.set_environment_state(state)


def main():
    """Example usage of WorldEnvironment."""
    import argparse
    
    parser = argparse.ArgumentParser(description="World Environment Simulator")
    parser.add_argument("--create", action="store_true", help="Create a new environment")
    parser.add_argument("--type", dest="env_type", choices=[t.value for t in EnvironmentType],
                       help="Environment type")
    parser.add_argument("--width", type=int, default=100, help="Environment width")
    parser.add_argument("--height", type=int, default=100, help="Environment height")
    parser.add_argument("--depth", type=int, default=10, help="Environment depth (3D)")
    parser.add_argument("--steps", type=int, default=100, help="Number of simulation steps")
    parser.add_argument("--config", help="Environment configuration file")
    parser.add_argument("--import", dest="import_file", help="Import environment from file")
    parser.add_argument("--export", dest="export_file", help="Export environment to file")
    parser.add_argument("--metrics", action="store_true", help="Calculate and display performance metrics")
    parser.add_argument("--verbose", action="store_true", help="Enable verbose output")
    
    args = parser.parse_args()
    
    if args.import_file:
        print(f"Importing environment from {args.import_file}...")
        env = WorldEnvironment()
        env.import_environment(args.import_file)
        print(f"✓ Environment imported successfully")
        print(f"Time step: {env.time_step}")
        print(f"Number of agents: {len(env.agents)}")
        print(f"Number of objects: {len(env.objects)}")
    
    elif args.create and args.env_type:
        print(f"Creating {args.env_type} environment...")
        
        config = {
            "type": args.env_type,
            "width": args.width,
            "height": args.height,
n            "depth": args.depth
        }
        
        env = WorldEnvironment(config)
        print(f"✓ Environment created successfully")
        print(f"Environment type: {env.config['type']}")
        print(f"Boundaries: {env.boundaries}")
        print(f"Cell map size: {len(env.cell_map)}")
        
        # Export if requested
        if args.export_file:
            env.export_environment(args.export_file)
            print(f"\n✓ Environment exported to {args.export_file}")
    
    else:
        print("Please specify either --create with --type or --import to load an existing environment")
        return


if __name__ == "__main__":
    main()
