"""Environment and world management for NLM Python API."""

import numpy as np
from typing import Dict, List, Optional, Any, Tuple
from collections import deque
import time
from enum import Enum
from pathlib import Path
import json

from ..core.Config.Config import Config


class EnvironmentType(Enum):
    SIMPLE = "simple"
    COMPLEX = "complex"
    PROCEDURAL = "procedural"
    CUSTOM = "custom"


class WorldSimulator:
    """
    Advanced world simulation for NLM agents.
    
    This class provides a comprehensive world simulation environment with
    dynamic physics, object interactions, and environmental stimuli.
    """
    
    def __init__(self, width: int = 100, height: int = 100,
                 time_step: float = 0.1, physics_enabled: bool = True):
        """
        Initialize world simulator.
        
        Args:
            width: World width
            height: World height
            time_step: Simulation time step
            physics_enabled: Whether to enable physics simulation
        """
        self.width = width
        self.height = height
        self.time_step = time_step
        self.physics_enabled = physics_enabled
        
        # World state
        self.current_time = 0.0
        self.world_objects = []
        self.agents = []
        self.events = deque(maxlen=1000)
        
        # Environmental factors
        self.temperature = 20.0
        self.humidity = 0.5
        self.wind_speed = 0.0
        self.wind_direction = 0.0
        self.lighting = 1.0
        
        # Physics state
        self.gravity = np.array([0.0, -9.8])
        self.friction = 0.1
        self.elasticity = 0.7
        
        # Simulation state
        self.simulation_running = False
        self.frame_count = 0
        
    def configure(self, **kwargs) -> None:
        """
        Configure world simulator.
        
        Args:
            **kwargs: Configuration parameters
        """
        for key, value in kwargs.items():
            if hasattr(self, key):
                setattr(self, key, value)
                
        # Apply specific configurations
        if "gravity" in kwargs:
            self.gravity = np.array(kwargs["gravity"])
            
        if "width" in kwargs:
            self.width = kwargs["width"]
            
        if "height" in kwargs:
            self.height = kwargs["height"]
            
    def reset(self) -> None:
        """Reset world to initial state."""
        self.current_time = 0.0
        self.world_objects = []
        self.agents = []
        self.events.clear()
        self.frame_count = 0
        
    def add_agent(self, agent: Any) -> None:
        """
        Add agent to world.
        
        Args:
            agent: Agent object
        """
        self.agents.append(agent)
        
    def remove_agent(self, agent: Any) -> bool:
        """
        Remove agent from world.
        
        Args:
            agent: Agent object
            
        Returns:
            bool: True if agent was removed
        """
        try:
            self.agents.remove(agent)
            return True
        except ValueError:
            return False
            
    def add_object(self, obj: Any) -> None:
        """
        Add object to world.
        
        Args:
            obj: World object
        """
        self.world_objects.append(obj)
        
    def remove_object(self, obj: Any) -> bool:
        """
        Remove object from world.
        
        Args:
            obj: World object
            
        Returns:
            bool: True if object was removed
        """
        try:
            self.world_objects.remove(obj)
            return True
        except ValueError:
            return False
            
    def update(self, dt: float = None) -> Dict[str, Any]:
        """
        Update world simulation.
        
        Args:
            dt: Time step (if None, uses self.time_step)
            
        Returns:
            Dict[str, Any]: World update results
        """
        if dt is None:
            dt = self.time_step
            
        self.current_time += dt
        self.frame_count += 1
        
        # Update environmental factors
        self._update_environment()
        
        # Update physics if enabled
        if self.physics_enabled:
            self._update_physics()
            
        # Update agents
        agent_updates = []
        for agent in self.agents:
            # In a real implementation, this would call agent's update method
            update_result = self._update_agent(agent, dt)
            agent_updates.append(update_result)
            
        # Check for events
        events = self._check_events()
            
        return {
            "time": self.current_time,
            "frame": self.frame_count,
            "agent_updates": agent_updates,
            "events": events,
            "environmental_factors": {
                "temperature": self.temperature,
                "humidity": self.humidity,
                "wind_speed": self.wind_speed,
                "wind_direction": self.wind_direction,
                "lighting": self.lighting
            }
        }
        
    def _update_environment(self) -> None:
        """Update environmental factors."""
        # Simple environmental dynamics
        # In a real implementation, this would be more complex
        
        # Temperature changes over time
        self.temperature += np.sin(self.current_time * 0.01) * 0.1
        
        # Humidity changes with temperature
        self.humidity = 0.5 + 0.1 * np.sin(self.current_time * 0.005)
        
        # Wind changes
        self.wind_speed = 5.0 + 2.0 * np.sin(self.current_time * 0.02)
        self.wind_direction = np.random.random() * 2 * np.pi
        
        # Lighting (day/night cycle)
        time_of_day = (self.current_time / (24 * 3600)) % 1
        if 6 <= time_of_day <= 18:  # Day time
            self.lighting = 1.0 + 0.2 * np.sin((time_of_day - 6) * np.pi / 6)
        else:  # Night time
            self.lighting = 0.3 + 0.1 * np.sin((time_of_day - 18) * np.pi / 6)
            
    def _update_physics(self) -> None:
        """Update physics simulation."""
        # Update object positions based on physics
        for obj in self.world_objects:
            if hasattr(obj, "position"):
                # Apply gravity
                if hasattr(obj, "velocity"):
                    obj.velocity += self.gravity * self.time_step
                    
                    # Apply friction
                    obj.velocity *= (1 - self.friction)
                    
                    # Update position
                    obj.position += obj.velocity * self.time_step
                    
                    # Check boundaries
                    self._check_boundaries(obj)
                    
                    # Handle collisions
                    self._handle_collisions(obj)
                    
    def _update_agent(self, agent: Any, dt: float) -> Dict[str, Any]:
        """
        Update agent in world.
        
        Args:
            agent: Agent to update
            dt: Time step
            
        Returns:
            Dict[str, Any]: Update results
        """
        # In a real implementation, this would call agent's update method
        return {
            "agent": agent,
            "updated": True,
            "time_step": dt,
            "position": getattr(agent, "position", None)
        }
        
    def _check_events(self) -> List[Dict[str, Any]]:
        """Check for and handle events."""
        events = []
        
        # Check for simple events
        for obj in self.world_objects:
            if hasattr(obj, "trigger_events"):
                obj_events = obj.trigger_events()
                events.extend(obj_events)
                
        return events
        
    def _check_boundaries(self, obj: Any) -> None:
        """Check if object is within world boundaries."""
        if hasattr(obj, "position") and hasattr(obj, "radius"):
            position = obj.position
            radius = obj.radius
            
            # Check left boundary
            if position[0] - radius < 0:
                position[0] = radius
                if hasattr(obj, "velocity"):
                    obj.velocity[0] = -obj.velocity[0] * self.elasticity
                    
            # Check right boundary
            if position[0] + radius > self.width:
                position[0] = self.width - radius
                if hasattr(obj, "velocity"):
                    obj.velocity[0] = -obj.velocity[0] * self.elasticity
                    
            # Check bottom boundary
            if position[1] - radius < 0:
                position[1] = radius
                if hasattr(obj, "velocity"):
                    obj.velocity[1] = -obj.velocity[1] * self.elasticity
                    
            # Check top boundary
            if position[1] + radius > self.height:
                position[1] = self.height - radius
                if hasattr(obj, "velocity"):
                    obj.velocity[1] = -obj.velocity[1] * self.elasticity
                    
    def _handle_collisions(self, obj: Any) -> None:
        """Handle collisions between objects."""
        for other in self.world_objects:
            if obj is other:
                continue
                
            # Check collision
            if hasattr(obj, "position") and hasattr(other, "position"):
                if hasattr(obj, "radius") and hasattr(other, "radius"):
                    distance = np.linalg.norm(obj.position - other.position)
                    radius_sum = obj.radius + other.radius
                    
                    if distance < radius_sum:
                        # Handle collision
                        self._resolve_collision(obj, other, distance, radius_sum)
                        
    def _resolve_collision(self, obj1: Any, obj2: Any, distance: float, radius_sum: float) -> None:
        """
        Resolve collision between two objects.
        
        Args:
            obj1: First object
            obj2: Second object
            distance: Distance between objects
            radius_sum: Sum of radii
        """
        # Calculate overlap
        overlap = radius_sum - distance
        
        # Calculate collision direction
        if distance > 0:
            collision_dir = (obj2.position - obj1.position) / distance
        else:
            collision_dir = np.array([1.0, 0.0])  # Default direction
            
        # Resolve collision (simple impulse-based)
        if hasattr(obj1, "velocity"):
            obj1.velocity += collision_dir * overlap * self.elasticity
            
        if hasattr(obj2, "velocity"):
            obj2.velocity -= collision_dir * overlap * self.elasticity
            
    def get_sensory_percept(self) -> Dict[str, Any]:
        """
        Get sensory percept from world.
        
        Returns:
            Dict[str, Any]: Sensory percept
        """
        # Collect sensory information
        percept = {
            "vision": self._get_vision_percept(),
            "objects": self._get_object_percept(),
            "environmental": self._get_environmental_percept(),
            "agents": self._get_agent_percept()
        }
        
        return percept
        
    def _get_vision_percept(self) -> np.ndarray:
        """Get vision percept."""
        # Simple vision simulation
        vision_size = 21  # 21x21 vision grid
        vision = np.zeros((vision_size, vision_size))
        
        # Add agent position to vision
        for agent in self.agents:
            if hasattr(agent, "position"):
                # Project agent position to vision coordinates
                vx = int((agent.position[0] / self.width) * (vision_size - 1))
                vy = int((agent.position[1] / self.height) * (vision_size - 1))
                
                if 0 <= vx < vision_size and 0 <= vy < vision_size:
                    vision[vy, vx] = 1.0
                    
        return vision
        
    def _get_object_percept(self) -> List[Dict[str, Any]]:
        """Get object percept."""
        object_percept = []
        
        for obj in self.world_objects:
            if hasattr(obj, "type") and hasattr(obj, "position"):
                object_percept.append({
                    "type": obj.type,
                    "position": obj.position.tolist() if hasattr(obj.position, "tolist") else list(obj.position),
                    "radius": obj.radius if hasattr(obj, "radius") else 0.0,
                    "active": obj.active if hasattr(obj, "active") else True
                })
                
        return object_percept
        
    def _get_environmental_percept(self) -> Dict[str, Any]:
        """Get environmental percept."""
        return {
            "temperature": self.temperature,
            "humidity": self.humidity,
            "wind_speed": self.wind_speed,
            "wind_direction": self.wind_direction,
            "lighting": self.lighting,
            "time_of_day": (self.current_time / (24 * 3600)) % 1
        }
        
    def _get_agent_percept(self) -> List[Dict[str, Any]]:
        """Get agent percept."""
        agent_percept = []
        
        for agent in self.agents:
            if hasattr(agent, "position"):
                agent_percept.append({
                    "position": agent.position.tolist() if hasattr(agent.position, "tolist") else list(agent.position),
                    "orientation": getattr(agent, "orientation", 0.0),
                    "velocity": agent.velocity.tolist() if hasattr(agent, "velocity") and hasattr(agent.velocity, "tolist") else list(getattr(agent, "velocity", [0.0, 0.0])),
                    "health": getattr(agent, "health", 1.0)
                })
                
        return agent_percept
        
    def get_world_state(self) -> Dict[str, Any]:
        """
        Get current world state.
        
        Returns:
            Dict[str, Any]: World state
        """
        return {
            "time": self.current_time,
            "frame": self.frame_count,
            "width": self.width,
            "height": self.height,
            "temperature": self.temperature,
            "humidity": self.humidity,
            "wind_speed": self.wind_speed,
            "wind_direction": self.wind_direction,
            "lighting": self.lighting,
            "agent_count": len(self.agents),
            "object_count": len(self.world_objects),
            "gravity": self.gravity.tolist() if hasattr(self.gravity, "tolist") else list(self.gravity)
        }
        
    def save(self, filepath: str) -> None:
        """
        Save world state to file.
        
        Args:
            filepath: Output file path
        """
        filepath = Path(filepath)
        filepath.parent.mkdir(parents=True, exist_ok=True)
        
        state = {
            "width": self.width,
            "height": self.height,
            "time_step": self.time_step,
            "current_time": self.current_time,
            "temperature": self.temperature,
            "humidity": self.humidity,
            "wind_speed": self.wind_speed,
            "wind_direction": self.wind_direction,
            "lighting": self.lighting,
            "gravity": self.gravity.tolist() if hasattr(self.gravity, "tolist") else list(self.gravity),
            "friction": self.friction,
            "elasticity": self.elasticity,
            "frame_count": self.frame_count,
            "simulation_running": self.simulation_running
        }
        
        with open(filepath, 'w') as f:
            json.dump(state, f, indent=2)
            
    def load(self, filepath: str) -> None:
        """
        Load world state from file.
        
        Args:
            filepath: Input file path
        """
        filepath = Path(filepath)
        
        with open(filepath, 'r') as f:
            state = json.load(f)
            
        self.width = state.get("width", self.width)
        self.height = state.get("height", self.height)
        self.time_step = state.get("time_step", self.time_step)
        self.current_time = state.get("current_time", 0.0)
        self.temperature = state.get("temperature", self.temperature)
        self.humidity = state.get("humidity", self.humidity)
        self.wind_speed = state.get("wind_speed", self.wind_speed)
        self.wind_direction = state.get("wind_direction", self.wind_direction)
        self.lighting = state.get("lighting", self.lighting)
        
        # Load physics
        if "gravity" in state:
            self.gravity = np.array(state["gravity"])
        self.friction = state.get("friction", self.friction)
        self.elasticity = state.get("elasticity", self.elasticity)
        
        self.frame_count = state.get("frame_count", 0)
        self.simulation_running = state.get("simulation_running", False)
        
    def is_valid_position(self, x: float, y: float, radius: float = 0.0) -> bool:
        """
        Check if position is valid.
        
        Args:
            x: X position
            y: Y position
            radius: Object radius
            
        Returns:
            bool: True if position is valid
        """
        return (radius <= x <= self.width - radius and
                radius <= y <= self.height - radius)


class EnvironmentFactory:
    """
    Create environments for NLM simulations.
    
    This class provides methods for creating different types of environments
    with predefined properties and characteristics.
    """
    
    @staticmethod
    def create_simple_environment(width: int = 100, height: int = 100,
                                 num_resources: int = 10) -> WorldSimulator:
        """
        Create a simple environment.
        
        Args:
            width: Environment width
            height: Environment height
            num_resources: Number of resources
            
        Returns:
            WorldSimulator: Created environment
        """
        env = WorldSimulator(width, height)
        
        # Add resources
        for i in range(num_resources):
            resource = {
                "type": "resource",
                "position": np.array([
                    np.random.uniform(10, width - 10),
                    np.random.uniform(10, height - 10)
                ]),
                "radius": 1.0,
                "value": np.random.uniform(1.0, 10.0),
                "active": True
            }
            env.add_object(resource)
            
        return env
        
    @staticmethod
    def create_complex_environment(width: int = 200, height: int = 200,
                                  num_hazards: int = 5, num_resources: int = 20) -> WorldSimulator:
        """
        Create a complex environment.
        
        Args:
            width: Environment width
            height: Environment height
            num_hazards: Number of hazards
            num_resources: Number of resources
            
        Returns:
            WorldSimulator: Created environment
        """
        env = WorldSimulator(width, height)
        
        # Add resources
        for i in range(num_resources):
            resource = {
                "type": "resource",
                "position": np.array([
                    np.random.uniform(10, width - 10),
                    np.random.uniform(10, height - 10)
                ]),
                "radius": 1.0 + np.random.uniform(-0.2, 0.2),
                "value": np.random.uniform(1.0, 15.0),
                "active": True
            }
            env.add_object(resource)
            
        # Add hazards
        for i in range(num_hazards):
            hazard = {
                "type": "hazard",
                "position": np.array([
                    np.random.uniform(10, width - 10),
                    np.random.uniform(10, height - 10)
                ]),
                "radius": 2.0 + np.random.uniform(-0.3, 0.3),
                "danger_level": np.random.uniform(5.0, 15.0),
                "active": True
            }
            env.add_object(hazard)
            
        return env
        
    @staticmethod
    def create_procedural_environment(env_type: str = "forest", seed: int = None) -> WorldSimulator:
        """
        Create a procedural environment.
        
        Args:
            env_type: Type of environment ("forest", "desert", "ocean", "urban")
            seed: Random seed
            
        Returns:
            WorldSimulator: Created environment
        """
        if seed is not None:
            np.random.seed(seed)
            
        if env_type == "forest":
            return EnvironmentFactory._create_forest_environment()
        elif env_type == "desert":
            return EnvironmentFactory._create_desert_environment()
        elif env_type == "ocean":
            return EnvironmentFactory._create_ocean_environment()
        elif env_type == "urban":
            return EnvironmentFactory._create_urban_environment()
        else:
            return WorldSimulator(100, 100)
            
    @staticmethod
    def _create_forest_environment() -> WorldSimulator:
        """Create a forest environment."""
        env = WorldSimulator(150, 150)
        
        # Add trees
        for i in range(30):
            tree = {
                "type": "tree",
                "position": np.array([
                    np.random.uniform(10, 140),
                    np.random.uniform(10, 140)
                ]),
                "radius": 3.0 + np.random.uniform(-0.5, 0.5),
                "health": np.random.uniform(0.8, 1.0),
                "active": True
            }
            env.add_object(tree)
            
        # Add resources (berries)
        for i in range(20):
            resource = {
                "type": "berry",
                "position": np.array([
                    np.random.uniform(20, 130),
                    np.random.uniform(20, 130)
                ]),
                "radius": 0.5,
                "value": np.random.uniform(2.0, 8.0),
                "active": True
            }
            env.add_object(resource)
            
        return env
        
    @staticmethod
    def _create_desert_environment() -> WorldSimulator:
        """Create a desert environment."""
        env = WorldSimulator(200, 150)
        
        # Add sand dunes
        for i in range(15):
            dune = {
                "type": "dune",
                "position": np.array([
                    np.random.uniform(20, 180),
                    np.random.uniform(20, 130)
                ]),
                "radius": 5.0 + np.random.uniform(-0.8, 0.8),
                "active": True
            }
            env.add_object(dune)
            
        # Add water oasis
        oasis = {
            "type": "oasis",
            "position": np.array([100.0, 75.0]),
            "radius": 4.0,
            "water_amount": 100.0,
            "active": True
        }
        env.add_object(oasis)
        
        return env
        
    @staticmethod
    def _create_ocean_environment() -> WorldSimulator:
        """Create an ocean environment."""
        env = WorldSimulator(250, 200)
        
        # Add islands
        for i in range(10):
            island = {
                "type": "island",
                "position": np.array([
                    np.random.uniform(30, 220),
                    np.random.uniform(30, 170)
                ]),
                "radius": 6.0 + np.random.uniform(-1.0, 1.0),
                "elevation": np.random.uniform(0.0, 100.0),
                "active": True
            }
            env.add_object(island)
            
        # Add fish schools
        for i in range(25):
            fish = {
                "type": "fish_school",
                "position": np.array([
                    np.random.uniform(40, 210),
                    np.random.uniform(40, 160)
                ]),
                "radius": 8.0 + np.random.uniform(-1.5, 1.5),
                "swarm_size": np.random.randint(10, 50),
                "speed": np.random.uniform(2.0, 8.0),
                "active": True
            }
            env.add_object(fish)
            
        return env
        
    @staticmethod
    def _create_urban_environment() -> WorldSimulator:
        """Create an urban environment."""
        env = WorldSimulator(300, 200)
        
        # Add buildings
        for i in range(20):
            building = {
                "type": "building",
                "position": np.array([
                    np.random.uniform(40, 260),
                    np.random.uniform(40, 160)
                ]),
                "radius": 8.0 + np.random.uniform(-1.5, 1.5),
                "floors": np.random.randint(3, 20),
                "active": True
            }
            env.add_object(building)
            
        # Add roads
        for i in range(10):
            road = {
                "type": "road",
                "position": np.array([
                    np.random.uniform(50, 250),
                    np.random.uniform(50, 150)
                ]),
                "radius": 6.0,
                "traffic_level": np.random.uniform(0.5, 1.5),
                "active": True
            }
            env.add_object(road)
            
        return env
        
    @staticmethod
    def create_custom_environment(width: int = 100, height: int = 100,
                                  objects: List[Dict[str, Any]] = None) -> WorldSimulator:
        """
        Create a custom environment.
        
        Args:
            width: Environment width
            height: Environment height
            objects: List of objects to add
            
        Returns:
            WorldSimulator: Created environment
        """
        env = WorldSimulator(width, height)
        
        if objects:
            for obj_spec in objects:
                obj = obj_spec.copy()
                env.add_object(obj)
                
        return env


class ScenarioManager:
    """
    Manage experimental scenarios for NLM simulations.
    
    This class provides functionality to create, configure, and manage
    experimental scenarios for conducting scientific research.
    """
    
    def __init__(self):
        """Initialize scenario manager."""
        self.scenarios = {}
        self.current_scenario = None
        self.scenario_history = deque(maxlen=100)
        
    def create_scenario(self, name: str, description: str, 
                       parameters: Dict[str, Any], environment_type: str = "simple") -> Dict[str, Any]:
        """
        Create a new scenario.
        
        Args:
            name: Scenario name
            description: Scenario description
            parameters: Scenario parameters
            environment_type: Type of environment
            
        Returns:
            Dict[str, Any]: Created scenario
        """
        scenario = {
            "name": name,
            "description": description,
            "parameters": parameters,
            "environment_type": environment_type,
            "created_time": time.time(),
            "version": "1.0",
            "status": "active",
            "metadata": {}
        }
        
        self.scenarios[name] = scenario
        return scenario
        
    def get_scenario(self, name: str) -> Optional[Dict[str, Any]]:
        """
        Get scenario by name.
        
        Args:
            name: Scenario name
            
        Returns:
            Optional[Dict[str, Any]]: Scenario or None if not found
        """
        return self.scenarios.get(name)
        
    def list_scenarios(self) -> List[str]:
        """List all scenario names."""
        return list(self.scenarios.keys())
        
    def update_scenario(self, name: str, **kwargs) -> bool:
        """
        Update scenario parameters.
        
        Args:
            name: Scenario name
            **kwargs: Parameters to update
            
        Returns:
            bool: True if scenario was updated
        """
        if name in self.scenarios:
            self.scenarios[name]["parameters"].update(kwargs)
            self.scenarios[name]["last_updated"] = time.time()
            return True
            
        return False
        
    def delete_scenario(self, name: str) -> bool:
        """
        Delete scenario.
        
        Args:
            name: Scenario name
            
        Returns:
            bool: True if scenario was deleted
        """
        if name in self.scenarios:
            del self.scenarios[name]
            return True
            
        return False
        
    def load_scenario(self, name: str) -> Dict[str, Any]:
        """
        Load scenario for use.
        
        Args:
            name: Scenario name
            
        Returns:
            Dict[str, Any]: Loaded scenario
            
        Raises:
            KeyError: If scenario not found
        """
        if name not in self.scenarios:
            raise KeyError(f"Scenario not found: {name}")
            
        scenario = self.scenarios[name].copy()
        scenario["status"] = "active"
        scenario["start_time"] = time.time()
        
        self.current_scenario = scenario
        self.scenario_history.append(scenario)
        
        return scenario
        
    def get_current_scenario(self) -> Optional[Dict[str, Any]]:
        """Get current scenario."""
        return self.current_scenario
        
    def end_scenario(self) -> Dict[str, Any]:
        """
        End current scenario.
        
        Returns:
            Dict[str, Any]: Ended scenario
        """
        if self.current_scenario:
            self.current_scenario["status"] = "completed"
            self.current_scenario["end_time"] = time.time()
            
            scenario = self.current_scenario
            self.current_scenario = None
            
            return scenario
            
        return None
        
    def get_scenario_statistics(self) -> Dict[str, Any]:
        """Get scenario management statistics."""
        return {
            "total_scenarios": len(self.scenarios),
            "current_scenario": self.current_scenario["name"] if self.current_scenario else None,
            "scenario_history_count": len(self.scenario_history),
            "active_scenarios": len([s for s in self.scenarios.values() if s["status"] == "active"]),
            "completed_scenarios": len([s for s in self.scenarios.values() if s["status"] == "completed"])
        }
        
    def reset(self) -> None:
        """Reset scenario manager."""
        self.scenarios.clear()
        self.current_scenario = None
        self.scenario_history.clear()


class ReplaySystem:
    """
    Replay recorded experiences.
    
    This class provides functionality to replay recorded experiences,
    analyze them, and use them for various purposes like testing and analysis.
    """
    
    def __init__(self, replay_buffer_size: int = 1000):
        """
        Initialize replay system.
        
        Args:
            replay_buffer_size: Size of replay buffer
        """
        self.replay_buffer = deque(maxlen=replay_buffer_size)
        self.playback_history = deque(maxlen=100)
        self.analysis_cache = {}
        
    def record_experience(self, experience: Dict[str, Any]) -> None:
        """
        Record an experience for replay.
        
        Args:
            experience: Experience to record
        """
        # Add timestamp
        experience["timestamp"] = time.time()
        experience["recorded_time"] = time.time()
        
        # Add to replay buffer
        self.replay_buffer.append(experience)
        
        # Update analysis cache
        self._update_analysis_cache(experience)
        
    def replay_experiences(self, start_index: int = None, end_index: int = None,
                           filter_criteria: Dict[str, Any] = None) -> List[Dict[str, Any]]:
        """
        Replay experiences from buffer.
        
        Args:
            start_index: Starting index (None for most recent)
            end_index: Ending index (None for oldest)
            filter_criteria: Criteria for filtering experiences
            
        Returns:
            List[Dict[str, Any]]: Replayed experiences
        """
        experiences = list(self.replay_buffer)
        
        # Apply filters
        if filter_criteria:
            experiences = self._filter_experiences(experiences, filter_criteria)
            
        # Apply index range
        if start_index is not None:
            experiences = experiences[:start_index]
            
        if end_index is not None:
            experiences = experiences[end_index:]
            
        # Add to playback history
        self.playback_history.append({
            "timestamp": time.time(),
            "experiences_count": len(experiences),
            "filter_criteria": filter_criteria
        })
        
        return experiences
        
    def _filter_experiences(self, experiences: List[Dict[str, Any]],
                            filter_criteria: Dict[str, Any]) -> List[Dict[str, Any]]:
        """Filter experiences based on criteria."""
        filtered = []
        
        for exp in experiences:
            match = True
            
            for key, value in filter_criteria.items():
                if key not in exp:
                    match = False
                    break
                    
                if isinstance(value, (list, tuple)):
                    if exp[key] not in value:
                        match = False
                        break
                else:
                    if exp[key] != value:
                        match = False
                        break
                        
            if match:
                filtered.append(exp)
                
        return filtered
        
    def _update_analysis_cache(self, experience: Dict[str, Any]) -> None:
        """Update analysis cache with new experience."""
        # Extract features for analysis
        features = self._extract_replay_features(experience)
        
        # Update cache
        if "features" not in self.analysis_cache:
            self.analysis_cache["features"] = []
        
        self.analysis_cache["features"].append(features)
        
        # Update statistics
        if "statistics" not in self.analysis_cache:
            self.analysis_cache["statistics"] = {}
            
        for feature, value in features.items():
            if feature not in self.analysis_cache["statistics"]:
                self.analysis_cache["statistics"][feature] = []
            
            self.analysis_cache["statistics"][feature].append(value)
            
    def _extract_replay_features(self, experience: Dict[str, Any]) -> Dict[str, Any]:
        """Extract features from experience for analysis."""
        features = {}
        
        # Basic experience features
        if "timestamp" in experience:
            features["age_seconds"] = time.time() - experience["timestamp"]
            
        if "reward" in experience:
            features["reward"] = experience["reward"]
            
        if "action" in experience:
            action = experience["action"]
            if isinstance(action, (list, tuple, np.ndarray)):
                features["action_magnitude"] = np.linalg.norm(action)
                features["action_dimensions"] = len(action)
            else:
                features["action_value"] = action
                
        if "state" in experience:
            state = experience["state"]
            if isinstance(state, (list, tuple, np.ndarray)):
                features["state_magnitude"] = np.linalg.norm(state)
                features["state_dimensions"] = len(state)
                
        return features
        
    def analyze_replay(self, analysis_type: str, time_window: int = 100,
                      filter_criteria: Dict[str, Any] = None) -> Dict[str, Any]:
        """
        Analyze replayed experiences.
        
        Args:
            analysis_type: Type of analysis ("rewards", "actions", "states", "patterns")
            time_window: Time window for analysis
            filter_criteria: Criteria for filtering experiences
            
        Returns:
            Dict[str, Any]: Analysis results
        """
        if analysis_type not in ["rewards", "actions", "states", "patterns"]:
            return {"error": "Unknown analysis type"}
            
        # Get experiences to analyze
        experiences = self.replay_experiences(filter_criteria=filter_criteria)
        
        if not experiences:
            return {"error": "No experiences to analyze"}
            
        # Filter by time window if needed
        if time_window > 0:
            cutoff_time = time.time() - time_window
            experiences = [
                exp for exp in experiences if exp.get("timestamp", 0) > cutoff_time
            ]
            
        analysis_result = {}
        
        if analysis_type == "rewards":
            rewards = [exp.get("reward", 0) for exp in experiences]
            analysis_result = {
                "count": len(rewards),
                "mean": np.mean(rewards),
                "std": np.std(rewards),
                "min": np.min(rewards),
                "max": np.max(rewards),
                "median": np.median(rewards),
                "percentile_25": np.percentile(rewards, 25),
                "percentile_75": np.percentile(rewards, 75)
            }
            
        elif analysis_type == "actions":
            actions = [exp.get("action") for exp in experiences if "action" in exp]
            if actions:
                # Calculate action statistics
                if isinstance(actions[0], (list, tuple, np.ndarray)):
                    # Convert to numpy array for analysis
                    action_array = np.array(actions)
                    analysis_result = {
                        "count": len(actions),
                        "dimensions": action_array.shape[1],
                        "mean_magnitude": np.mean(np.linalg.norm(action_array, axis=1)),
                        "std_magnitude": np.std(np.linalg.norm(action_array, axis=1)),
                        "most_common_direction": self._find_most_common_direction(action_array)
                    }
                else:
                    # Scalar actions
                    analysis_result = {
                        "count": len(actions),
                        "values": sorted(set(actions)),
                        "mean": np.mean(actions),
                        "std": np.std(actions)
                    }
                    
        elif analysis_type == "states":
            states = [exp.get("state") for exp in experiences if "state" in exp]
            if states:
                state_array = np.array(states)
                analysis_result = {
                    "count": len(states),
                    "dimensions": state_array.shape[1],
                    "mean": np.mean(state_array, axis=0).tolist(),
                    "std": np.std(state_array, axis=0).tolist(),
                    "min": np.min(state_array, axis=0).tolist(),
                    "max": np.max(state_array, axis=0).tolist()
                }
                
        elif analysis_type == "patterns":
            # Look for patterns in experiences
            patterns = self._find_patterns(experiences)
            analysis_result = {
                "patterns_found": len(patterns),
                "pattern_types": list(patterns.keys()),
                "most_common_patterns": self._get_most_common_patterns(patterns)
            }
            
        return analysis_result
        
    def _find_most_common_direction(self, action_array: np.ndarray) -> Dict[str, Any]:
        """Find most common action direction."""
        if len(action_array) == 0:
            return {}
            
        # Normalize actions
        norms = np.linalg.norm(action_array, axis=1, keepdims=True)
        normalized_actions = action_array / (norms + 1e-8)
        
        # Find most common direction (simplified)
        mean_direction = np.mean(normalized_actions, axis=0)
        mean_direction_norm = np.linalg.norm(mean_direction)
        
        if mean_direction_norm > 0:
            mean_direction = mean_direction / mean_direction_norm
            
        return {
            "mean_direction": mean_direction.tolist(),
            "mean_magnitude": np.mean(norms.flatten())
        }
        
    def _find_patterns(self, experiences: List[Dict[str, Any]]) -> Dict[str, Any]:
        """Find patterns in experiences."""
        patterns = {}
        
        # Simple pattern detection
        # In a real implementation, this would be more sophisticated
        
        # Reward patterns
        rewards = [exp.get("reward", 0) for exp in experiences]
        if rewards:
            patterns["reward_patterns"] = {
                "positive_count": sum(1 for r in rewards if r > 0),
                "negative_count": sum(1 for r in rewards if r < 0),
                "zero_count": sum(1 for r in rewards if r == 0),
                "high_reward_count": sum(1 for r in rewards if r > 10),
                "low_reward_count": sum(1 for r in rewards if r < -10)
            }
            
        # Action patterns
        actions = [exp.get("action") for exp in experiences if "action" in exp]
        if actions:
            patterns["action_patterns"] = {
                "unique_actions": len(set(str(a) for a in actions)),
                "action_changes": sum(1 for i in range(1, len(actions)) 
                                      if str(actions[i]) != str(actions[i-1]))
            }
            
        return patterns
        
    def _get_most_common_patterns(self, patterns: Dict[str, Any]) -> Dict[str, Any]:
        """Get most common patterns."""
        most_common = {}
        
        for pattern_type, pattern_data in patterns.items():
            if isinstance(pattern_data, dict):
                # Find key with maximum value
                max_key = max(pattern_data.keys(), key=lambda k: pattern_data[k])
                most_common[pattern_type] = {
                    "pattern": max_key,
                    "count": pattern_data[max_key]
                }
                
        return most_common
        
    def get_replay_statistics(self) -> Dict[str, Any]:
        """Get replay system statistics."""
        return {
            "buffer_size": len(self.replay_buffer),
            "max_buffer_size": self.replay_buffer.maxlen,
            "playback_count": len(self.playback_history),
            "current_buffer_usage": len(self.replay_buffer) / self.replay_buffer.maxlen * 100,
            "analysis_cache_size": len(self.analysis_cache)
        }
        
    def clear_replay_buffer(self) -> None:
        """Clear replay buffer."""
        self.replay_buffer.clear()
        self.analysis_cache.clear()
        
    def reset(self) -> None:
        """Reset replay system."""
        self.clear_replay_buffer()
        self.playback_history.clear()


# Re-export commonly used classes and functions
__all__ = [
    "EnvironmentType",
    "WorldSimulator", "EnvironmentFactory",
    "ScenarioManager", "ReplaySystem"
]