"""
World module - World and environment functionality for NLM
"""

from typing import List, Dict, Any, Optional, Tuple
import math
import time

from .pynlm import (
    SimpleWorld as WorldCore, Environment as EnvironmentCore, Observation as ObservationCore,
    Action as ActionCore, Sensor as SensorCore, Terrain as TerrainCore, Location as LocationCore,
    NeuronId, SynapseId, Timestamp, TimestepDuration, SimulationStep, PlasticityFlags,
    ActionType, ObservationData
)

__all__ = [
    'SimpleWorld', 'Environment', 'Observation', 'Sensor',
    'Terrain', 'Location',
    'NeuronId', 'SynapseId', 'Timestamp', 'TimestepDuration', 'SimulationStep',
    'ActionType', 'ObservationData',
]

class Sensor:
    """Sensor for detecting environment features"""
    
    def __init__(self, range: float = 10.0, resolution: float = 0.1):
        """Initialize sensor
        
        Args:
            range: Sensor detection range
            resolution: Sensor resolution
        """
        self.range = range
        self.resolution = resolution
        self.reading = ObservationData()
        self.timestamp = time.time()
    
    def scan(self, position: Tuple[float, float], world_size: Tuple[float, float] = (100.0, 100.0)):
        """Scan environment
        
        Args:
            position: Sensor position (x, y)
            world_size: World size (width, height)
        """
        # Create observation based on position
        data = ObservationData()
        
        # Add features based on position
        x, y = position
        
        # Normalize position to range [-1, 1]
        norm_x = 2 * x / world_size[0] - 1
        norm_y = 2 * y / world_size[1] - 1
        
        # Create observation data
        data.set_data([norm_x, norm_y, math.sin(x * 0.1), math.cos(y * 0.1), 
                      math.sqrt(x * x + y * y) / math.sqrt(world_size[0] ** 2 + world_size[1] ** 2)])
        
        self.reading = data
        self.timestamp = time.time()
    
    def get_reading(self) -> ObservationData:
        """Get sensor reading
        
        Returns:
            Sensor reading
        """
        return self.reading
    
    def get_feature(self, index: int) -> float:
        """Get specific feature
        
        Args:
            index: Feature index
            
        Returns:
            Feature value
        """
        data = self.reading.getData()
        if index < len(data):
            return data[index]
        return 0.0

class Location:
    """Location in world space"""
    
    def __init__(self, x: float = 0.0, y: float = 0.0, z: float = 0.0):
        """Initialize location
        
        Args:
            x: X coordinate
            y: Y coordinate
            z: Z coordinate
        """
        self.x = x
        self.y = y
        self.z = z
    
    def distance_to(self, other: 'Location') -> float:
        """Calculate distance to another location
        
        Args:
            other: Other location
            
        Returns:
            Distance
        """
        dx = self.x - other.x
        dy = self.y - other.y
        dz = self.z - other.z
        return math.sqrt(dx * dx + dy * dy + dz * dz)
    
    def distance_to_point(self, x: float, y: float, z: float = 0.0) -> float:
        """Calculate distance to point
        
        Args:
            x: X coordinate
            y: Y coordinate
            z: Z coordinate
            
        Returns:
            Distance
        """
        dx = self.x - x
        dy = self.y - y
        dz = self.z - z
        return math.sqrt(dx * dx + dy * dy + dz * dz)
    
    def add(self, other: 'Location') -> 'Location':
        """Add another location
        
        Args:
            other: Other location
            
        Returns:
            New location
        """
        return Location(self.x + other.x, self.y + other.y, self.z + other.z)
    
    def __repr__(self):
        return f"Location({self.x:.2f}, {self.y:.2f}, {self.z:.2f})"

class Terrain:
    """Terrain with properties"""
    
    def __init__(self, width: float, height: float):
        """Initialize terrain
        
        Args:
            width: Terrain width
            height: Terrain height
        """
        self.width = width
        self.height = height
        self.height_map = []
        self.generate_height_map()
    
    def generate_height_map(self):
        """Generate height map"""
        # Simple terrain generation
        self.height_map = []
        for y in range(int(self.height)):
            row = []
            for x in range(int(self.width)):
                # Simple terrain function
                height = (
                    math.sin(x * 0.1) * math.cos(y * 0.1) +
                    math.sin(x * 0.05) * math.cos(y * 0.05) * 0.5 +
                    math.sin((x + y) * 0.02) * 0.3
                ) * 10 + 5
                row.append(height)
            self.height_map.append(row)
    
    def get_height(self, x: float, y: float) -> float:
        """Get height at position
        
        Args:
            x: X coordinate
            y: Y coordinate
            
        Returns:
            Height
        """
        if x < 0 or x >= self.width or y < 0 or y >= self.height:
            return 0.0
        
        ix = int(x)
        iy = int(y)
        
        if ix >= len(self.height_map[0]) or iy >= len(self.height_map):
            return 0.0
        
        return self.height_map[iy][ix]
    
    def is_passable(self, x: float, y: float, agent_radius: float = 0.0) -> bool:
        """Check if position is passable
        
        Args:
            x: X coordinate
            y: Y coordinate
            agent_radius: Agent radius
            
        Returns:
            True if passable
        """
        height = self.get_height(x, y)
        return height > 0.0

class Environment:
    """Enhanced environment with physics and interactions"""
    
    def __init__(self, name: str = "DefaultEnvironment", width: float = 100.0, height: float = 100.0):
        """Initialize environment
        
        Args:
            name: Environment name
            width: Environment width
            height: Environment height
        """
        self.name = name
        self.width = width
        self.height = height
        self.terrain = Terrain(width, height)
        self.objects = []
        self.agents = []
        self.timestamp = 0.0
        self.max_objects = 100
        self.max_agents = 10
    
    def add_object(self, obj: Dict[str, Any]):
        """Add object to environment
        
        Args:
            obj: Object definition
        """
        if len(self.objects) < self.max_objects:
            self.objects.append(obj)
    
    def remove_object(self, obj_id: str):
        """Remove object from environment
        
        Args:
            obj_id: Object identifier
        """
        self.objects = [obj for obj in self.objects if obj.get('id') != obj_id]
    
    def add_agent(self, agent: Dict[str, Any]):
        """Add agent to environment
        
        Args:
            agent: Agent definition
        """
        if len(self.agents) < self.max_agents:
            self.agents.append(agent)
    
    def remove_agent(self, agent_id: str):
        """Remove agent from environment
        
        Args:
            agent_id: Agent identifier
        """
        self.agents = [agent for agent in self.agents if agent.get('id') != agent_id]
    
    def update(self, dt: float):
        """Update environment
        
        Args:
            dt: Time step
        """
        self.timestamp += dt
        
        # Update objects
        for obj in self.objects:
            self._update_object(obj, dt)
        
        # Update agents
        for agent in self.agents:
            self._update_agent(agent, dt)
    
    def _update_object(self, obj: Dict[str, Any], dt: float):
        """Update object
        
        Args:
            obj: Object to update
            dt: Time step
        """
        # Update object properties
        if 'position' in obj and 'velocity' in obj:
            obj['position']['x'] += obj['velocity']['vx'] * dt
            obj['position']['y'] += obj['velocity']['vy'] * dt
            
            # Apply gravity
            if 'gravity' in obj:
                obj['velocity']['vy'] -= obj['gravity'] * dt
            
            # Check bounds
            self._check_bounds(obj)
    
    def _update_agent(self, agent: Dict[str, Any], dt: float):
        """Update agent
        
        Args:
            agent: Agent to update
            dt: Time step
        """
        # Update agent properties
        if 'position' in agent:
            self._check_bounds(agent)
    
    def _check_bounds(self, obj: Dict[str, Any]):
        """Check object bounds
        
        Args:
            obj: Object to check
        """
        if 'position' in obj:
            pos = obj['position']
            if pos['x'] < 0:
                pos['x'] = 0
                if 'velocity' in obj:
                    obj['velocity']['vx'] = abs(obj['velocity']['vx'])
            elif pos['x'] > self.width:
                pos['x'] = self.width
                if 'velocity' in obj:
                    obj['velocity']['vx'] = -abs(obj['velocity']['vx'])
            
            if pos['y'] < 0:
                pos['y'] = 0
                if 'velocity' in obj:
                    obj['velocity']['vy'] = abs(obj['velocity']['vy'])
            elif pos['y'] > self.height:
                pos['y'] = self.height
                if 'velocity' in obj:
                    obj['velocity']['vy'] = -abs(obj['velocity']['vy'])
    
    def get_observation(self, position: Tuple[float, float], range: float) -> ObservationData:
        """Get observation at position
        
        Args:
            position: Position (x, y)
            range: Observation range
            
        Returns:
            Observation
        """
        data = ObservationData()
        
        # Collect features from environment
        features = []
        
        # Position features
        x, y = position
        features.extend([x, y])
        
        # Terrain features
        height = self.terrain.get_height(x, y)
        features.append(height)
        
        # Nearby objects
        nearby_objects = []
        for obj in self.objects:
            if 'position' in obj:
                obj_pos = obj['position']
                dist = ((x - obj_pos['x']) ** 2 + (y - obj_pos['y']) ** 2) ** 0.5
                if dist <= range:
                    nearby_objects.append({
                        'id': obj.get('id', 'unknown'),
                        'type': obj.get('type', 'unknown'),
                        'distance': dist,
                        'height': obj_pos.get('z', 0.0),
                        'velocity': obj.get('velocity', {'vx': 0, 'vy': 0})
                    })
        
        # Add object features
        features.extend([len(nearby_objects), sum(obj['distance'] for obj in nearby_objects) / len(nearby_objects) if nearby_objects else 0])
        
        # Agent features
        nearby_agents = []
        for agent in self.agents:
            if 'position' in agent:
                agent_pos = agent['position']
                dist = ((x - agent_pos['x']) ** 2 + (y - agent_pos['y']) ** 2) ** 0.5
                if dist <= range:
                    nearby_agents.append({
                        'id': agent.get('id', 'unknown'),
                        'distance': dist
                    })
        
        # Add agent features
        features.extend([len(nearby_agents), sum(agent['distance'] for agent in nearby_agents) / len(nearby_agents) if nearby_agents else 0])
        
        # Temporal features
        features.extend([math.sin(self.timestamp * 0.1), math.cos(self.timestamp * 0.1)])
        
        data.set_data(features)
        return data
    
    def reset(self):
        """Reset environment"""
        self.objects.clear()
        self.agents.clear()
        self.timestamp = 0.0

class SimpleWorld:
    """Simple world with basic functionality"""
    
    def __init__(self, name: str = "SimpleWorld", width: float = 100.0, height: float = 100.0):
        """Initialize simple world
        
        Args:
            name: World name
            width: World width
            height: World height
        """
        self.name = name
        self.width = width
        self.height = height
        self.environment = Environment(name, width, height)
        self.timestamp = 0.0
    
    def add_object(self, obj: Dict[str, Any]):
        """Add object to world
        
        Args:
            obj: Object to add
        """
        self.environment.add_object(obj)
    
    def remove_object(self, obj_id: str):
        """Remove object from world
        
        Args:
            obj_id: Object identifier
        """
        self.environment.remove_object(obj_id)
    
    def add_agent(self, agent: Dict[str, Any]):
        """Add agent to world
        
        Args:
            agent: Agent to add
        """
        self.environment.add_agent(agent)
    
    def remove_agent(self, agent_id: str):
        """Remove agent from world
        
        Args:
            agent_id: Agent identifier
        """
        self.environment.remove_agent(agent_id)
    
    def get_observation(self, position: Tuple[float, float], range: float) -> ObservationData:
        """Get observation at position
        
        Args:
            position: Position (x, y)
            range: Observation range
            
        Returns:
            Observation
        """
        return self.environment.get_observation(position, range)
    
    def reset(self):
        """Reset world"""
        self.environment.reset()
    
    def update(self, dt: float):
        """Update world
        
        Args:
            dt: Time step
        """
        self.environment.update(dt)
        self.timestamp += dt
    
    def get_name(self) -> str:
        """Get world name
        
        Returns:
            World name
        """
        return self.name
    
    def get_width(self) -> float:
        """Get world width
        
        Returns:
            World width
        """
        return self.width
    
    def get_height(self) -> float:
        """Get world height
        
        Returns:
            World height
        """
        return self.height

# Convenience functions
def create_world(name: str = "SimpleWorld", width: float = 100.0, height: float = 100.0) -> SimpleWorld:
    """Create world
    
    Args:
        name: World name
        width: World width
        height: World height
        
    Returns:
        SimpleWorld instance
    """
    return SimpleWorld(name, width, height)

class WorldUtils:
    """Utility functions for world operations"""
    
    @staticmethod
    def create_location(x: float = 0.0, y: float = 0.0, z: float = 0.0) -> Location:
        """Create location
        
        Args:
            x: X coordinate
            y: Y coordinate
            z: Z coordinate
            
        Returns:
            Location instance
        """
        return Location(x, y, z)
    
    @staticmethod
    def create_sensor(range: float = 10.0, resolution: float = 0.1) -> Sensor:
        """Create sensor
        
        Args:
            range: Sensor range
            resolution: Sensor resolution
            
        Returns:
            Sensor instance
        """
        return Sensor(range, resolution)
    
    @staticmethod
    def create_terrain(width: float, height: float) -> Terrain:
        """Create terrain
        
        Args:
            width: Terrain width
            height: Terrain height
            
        Returns:
            Terrain instance
        """
        return Terrain(width, height)
    
    @staticmethod
    def create_environment(name: str = "DefaultEnvironment", width: float = 100.0, height: float = 100.0) -> Environment:
        """Create environment
        
        Args:
            name: Environment name
            width: Environment width
            height: Environment height
            
        Returns:
            Environment instance
        """
        return Environment(name, width, height)
    
    @staticmethod
    def create_simple_world(name: str = "SimpleWorld", width: float = 100.0, height: float = 100.0) -> SimpleWorld:
        """Create simple world
        
        Args:
            name: World name
            width: World width
            height: World height
            
        Returns:
            SimpleWorld instance
        """
        return SimpleWorld(name, width, height)
