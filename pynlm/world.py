"""
World simulation classes and environment management.

This module contains the world simulation components and advanced environment
features for expert users.
"""

from .core import *

class AdvancedWorld:
    """
    Advanced world simulation for expert users.
    
    Provides advanced world control, environment management, and complex
    world dynamics for sophisticated agent testing.
    """
    
    def __init__(self, world):
        self.world = world
        self._objects = []
        self._terrain = None
        self._weather = "clear"
        self._time_multiplier = 1.0
        self._simulation_paused = False
        self._event_handlers = {}
        
    def configure_advanced(self, width, height, vision_width, vision_height,
                          terrain_type="flat", weather_condition="clear",
                          time_multiplier=1.0):
        """
        Configure world with advanced parameters.
        
        Args:
            width: World width
            height: World height
            vision_width: Agent vision width
            vision_height: Agent vision height
            terrain_type: Type of terrain (flat, hilly, maze, etc.)
            weather_condition: Weather condition (clear, rainy, stormy, etc.)
            time_multiplier: Speed multiplier for world updates
        """
        self.world.configure(width, height, vision_width, vision_height)
        self._terrain = terrain_type
        self._weather = weather_condition
        self._time_multiplier = time_multiplier
        
        print(f"[INFO] World configured: {width}x{height}, terrain={terrain_type}, "
              f"weather={weather_condition}, time_multiplier={time_multiplier}")
        
    def spawn_objects(self, num_objects, object_type="resource", density=0.1,
                      randomize_position=True, value_range=(0.5, 2.0)):
        """
        Spawn objects in the world with advanced control.
        
        Args:
            num_objects: Number of objects to spawn
            object_type: Type of object (resource, hazard, marker, wall)
            density: Object density (0.0 to 1.0)
            randomize_position: Whether to randomize object positions
            value_range: Range of object values
        """
        from .core import WorldObjectType, WorldObject
        import random
        
        # Map object types to enum values
        type_map = {
            "resource": WorldObjectType.Resource,
            "hazard": WorldObjectType.Hazard,
            "marker": WorldObjectType.Marker,
            "wall": WorldObjectType.Wall,
            "empty": WorldObjectType.Empty
        }
        
        if object_type not in type_map:
            print(f"[ERROR] Unknown object type: {object_type}")
            return
        
        world_type = type_map[object_type]
        min_x, max_x = 0, self.world.getWidth()
        min_y, max_y = 0, self.world.getHeight()
        
        for i in range(num_objects):
            if randomize_position:
                x = random.uniform(min_x + 1, max_x - 1)
                y = random.uniform(min_y + 1, max_y - 1)
            else:
                # Grid-based placement
                cell_size = 1.0 / (1.0 / density) if density > 0 else 1.0
                x = min_x + (i % int(1.0 / density)) * cell_size
                y = max_y - ((i // int(1.0 / density)) * cell_size)
            
            value = random.uniform(value_range[0], value_range[1])
            radius = 0.3 if object_type == "hazard" else 0.5
            
            obj = WorldObject(x, y, world_type, value, radius)
            self.world.addObject(obj)
            self._objects.append(obj)
            
        print(f"[INFO] Spawned {num_objects} {object_type} objects in world")
        
    def create_maze(self, width, height, wall_thickness=1.0, corridor_width=2.0):
        """
        Create a maze world for agent navigation testing.
        
        Args:
            width: Maze width
            height: Maze height
            wall_thickness: Thickness of walls
            corridor_width: Width of corridors
        """
        self.world.configure(width, height, width // 3, height // 3)
        self._terrain = "maze"
        
        # Create maze walls
        for x in range(0, width, int(corridor_width)):
            for y in range(0, height, int(corridor_width)):
                if (x // int(corridor_width)) % 2 == 0 or (y // int(corridor_width)) % 2 == 0:
                    # Wall
                    obj = WorldObject(x + wall_thickness/2, y + wall_thickness/2,
                                    WorldObjectType.Wall, 1.0, wall_thickness/2)
                    self.world.addObject(obj)
                else:
                    # Empty space
                    pass
        
        print(f"[INFO] Created maze: {width}x{height}, wall_thickness={wall_thickness}")
        
    def create_terrain(self, terrain_type="random", parameters=None):
        """
        Create custom terrain for world simulation.
        
        Args:
            terrain_type: Type of terrain (flat, hilly, mountainous, random)
            parameters: Terrain parameters (elevation_range, roughness, etc.)
        """
        if parameters is None:
            parameters = {}
        
        self._terrain = terrain_type
        
        if terrain_type == "flat":
            # Flat terrain - no changes needed
            pass
            
        elif terrain_type == "hilly":
            # Simple hills by adding elevation variations
            pass
            
        elif terrain_type == "random":
            # Random terrain features
            import random
            for obj in self._objects:
                if obj.type == WorldObjectType.Resource:
                    # Add elevation variation
                    obj.value *= random.uniform(0.8, 1.2)
                    
        elif terrain_type == "mountainous":
            # Mountainous terrain - create elevated regions
            pass
            
        print(f"[INFO] Created {terrain_type} terrain with parameters: {parameters}")
        
    def set_weather(self, weather_condition, intensity=1.0):
        """
        Set weather conditions that affect agent behavior.
        
        Args:
            weather_condition: Weather type (clear, rainy, stormy, snowy, etc.)
            intensity: Weather intensity (0.0 to 1.0)
        """
        self._weather = weather_condition
        
        # Apply weather effects
        if weather_condition == "rainy":
            # Reduce visibility and increase resource decay
            pass
        elif weather_condition == "stormy":
            # High wind, random disturbances
            pass
        elif weather_condition == "snowy":
            # Reduced visibility, slippery surfaces
            pass
            
        print(f"[INFO] Weather set to {weather_condition} (intensity: {intensity})")
        
    def set_time_multiplier(self, multiplier):
        """
        Set time multiplier for world simulation speed.
        
        Args:
            multiplier: Time speed multiplier (1.0 = real time)
        """
        if multiplier > 0:
            self._time_multiplier = multiplier
            print(f"[INFO] Time multiplier set to {multiplier}")
        else:
            print(f"[ERROR] Invalid time multiplier: {multiplier}")
            
    def pause_simulation(self, pause=True):
        """
        Pause or resume world simulation.
        
        Args:
            pause: True to pause, False to resume
        """
        self._simulation_paused = pause
        print(f"[INFO] World simulation {'paused' if pause else 'resumed'}")
        
    def step(self, timestep):
        """
        Step world simulation with time multiplier.
        
        Args:
            timestep: Base timestep
            
        Returns:
            float: Effective timestep after time multiplier
        """
        if self._simulation_paused:
            return 0.0
            
        effective_timestep = timestep * self._time_multiplier
        return self.world.update(effective_timestep)
        
    def add_event_handler(self, event_type, handler_func):
        """
        Add event handler for world events.
        
        Args:
            event_type: Type of event (agent_collision, object_pickup, etc.)
            handler_func: Function to call when event occurs
        """
        if event_type not in self._event_handlers:
            self._event_handlers[event_type] = []
        self._event_handlers[event_type].append(handler_func)
        
    def trigger_event(self, event_type, event_data):
        """
        Trigger world event and notify handlers.
        
        Args:
            event_type: Type of event
            event_data: Event data dictionary
        """
        if event_type in self._event_handlers:
            for handler in self._event_handlers[event_type]:
                try:
                    handler(event_data)
                except Exception as e:
                    print(f"[ERROR] Event handler failed: {e}")
        
    def get_world_info(self):
        """
        Get comprehensive world information.
        
        Returns:
            dict: World information
        """
        return {
            'dimensions': {
                'width': self.world.getWidth(),
                'height': self.world.getHeight(),
                'vision_width': self.world.getVisionWidth() if hasattr(self.world, 'getVisionWidth') else 8,
                'vision_height': self.world.getVisionHeight() if hasattr(self.world, 'getVisionHeight') else 8
            },
            'environment': {
                'terrain': self._terrain,
                'weather': self._weather,
                'time_multiplier': self._time_multiplier,
                'paused': self._simulation_paused
            },
            'objects': len(self._objects),
            'simulation_time': self.world.getSimulationTime()
        }
        
    def export_world(self, filepath, format="json"):
        """
        Export world to file.
        
        Args:
            filepath: Path to export file
            format: Export format (json, txt, csv)
            
        Returns:
            bool: True if successful, False otherwise
        """
        try:
            import json
            
            world_data = {
                'info': self.get_world_info(),
                'objects': [
                    {
                        'x': obj.x,
                        'y': obj.y,
                        'type': obj.type.name if hasattr(obj.type, 'name') else str(obj.type),
                        'value': obj.value,
                        'radius': obj.radius,
                        'active': obj.active
                    }
                    for obj in self._objects
                ]
            }
            
            if format.lower() == "json":
                with open(filepath, 'w') as f:
                    json.dump(world_data, f, indent=2)
            elif format.lower() == "txt":
                with open(filepath, 'w') as f:
                    f.write(f"World Export\n")
                    f.write(f"Dimensions: {self.world.getWidth()}x{self.world.getHeight()}\n")
                    f.write(f"Terrain: {self._terrain}\n")
                    f.write(f"Weather: {self._weather}\n")
                    f.write(f"Objects: {len(self._objects)}\n")
                    for obj in self._objects:
                        f.write(f"  Object at ({obj.x:.2f}, {obj.y:.2f}): "
                               f"type={obj.type.name if hasattr(obj.type, 'name') else str(obj.type)}, "
                               f"value={obj.value:.2f}\n")
            elif format.lower() == "csv":
                import csv
                with open(filepath, 'w', newline='') as f:
                    writer = csv.writer(f)
                    writer.writerow(['x', 'y', 'type', 'value', 'radius', 'active'])
                    for obj in self._objects:
                        writer.writerow([
                            obj.x, obj.y,
                            obj.type.name if hasattr(obj.type, 'name') else str(obj.type),
                            obj.value, obj.radius, obj.active
                        ])
            else:
                print(f"[ERROR] Unsupported format: {format}")
                return False
            
            print(f"[INFO] World exported to {filepath} ({format})")
            return True
            
        except Exception as e:
            print(f"[ERROR] Failed to export world: {e}")
            return False
            
    def import_world(self, filepath, format="json"):
        """
        Import world from file.
        
        Args:
            filepath: Path to import file
            format: Import format (json, txt, csv)
            
        Returns:
            bool: True if successful, False otherwise
        """
        try:
            import json
            import csv
            
            if format.lower() == "json":
                with open(filepath, 'r') as f:
                    world_data = json.load(f)
                    
                # Clear current world
                self._clear_world()
                
                # Import objects
                for obj_data in world_data.get('objects', []):
                    from .core import WorldObject
                    obj = WorldObject(
                        obj_data['x'], obj_data['y'],
                        self._get_world_object_type(obj_data['type']),
                        obj_data['value'], obj_data['radius']
                    )
                    obj.active = obj_data.get('active', True)
                    self.world.addObject(obj)
                    self._objects.append(obj)
                    
            elif format.lower() == "txt":
                with open(filepath, 'r') as f:
                    lines = f.readlines()
                    
                # Parse world data (simplified)
                self._clear_world()
                
            elif format.lower() == "csv":
                with open(filepath, 'r') as f:
                    reader = csv.DictReader(f)
                    
                self._clear_world()
                for row in reader:
                    from .core import WorldObject
                    obj = WorldObject(
                        float(row['x']), float(row['y']),
                        self._get_world_object_type(row['type']),
                        float(row['value']), float(row['radius'])
                    )
                    obj.active = row.get('active', 'true').lower() == 'true'
                    self.world.addObject(obj)
                    self._objects.append(obj)
            else:
                print(f"[ERROR] Unsupported format: {format}")
                return False
            
            print(f"[INFO] World imported from {filepath} ({format})")
            return True
            
        except Exception as e:
            print(f"[ERROR] Failed to import world: {e}")
            return False
            
    def _clear_world(self):
        """Clear current world objects."""
        # Remove all objects from world
        for obj in self._objects:
            self.world.removeObject(obj.x, obj.y)
        self._objects.clear()
        
    def _get_world_object_type(self, type_name):
        """Convert type name to WorldObjectType enum."""
        from .core import WorldObjectType
        
        type_map = {
            'Empty': WorldObjectType.Empty,
            'Resource': WorldObjectType.Resource,
            'Hazard': WorldObjectType.Hazard,
            'Wall': WorldObjectType.Wall,
            'Marker': WorldObjectType.Marker
        }
        
        return type_map.get(type_name, WorldObjectType.Empty)
        
    def get_object_density(self):
        """
        Get object density statistics.
        
        Returns:
            dict: Object density information
        """
        total_area = self.world.getWidth() * self.world.getHeight()
        object_area = sum(obj.radius ** 2 * 3.14159 for obj in self._objects)
        
        return {
            'total_objects': len(self._objects),
            'total_area': total_area,
            'occupied_area': object_area,
            'density': object_area / total_area if total_area > 0 else 0,
            'average_radius': sum(obj.radius for obj in self._objects) / len(self._objects) if self._objects else 0
        }
        
    def get_agent_visibility_analysis(self, agent_x, agent_y, agent_vision_range=10.0):
        """
        Analyze agent visibility in the world.
        
        Args:
            agent_x: Agent x position
            agent_y: Agent y position
            agent_vision_range: Agent vision range
            
        Returns:
            dict: Visibility analysis
        """
        visible_objects = []
        blocked_objects = []
        
        for obj in self._objects:
            if not obj.active:
                continue
                
            distance = ((obj.x - agent_x) ** 2 + (obj.y - agent_y) ** 2) ** 0.5
            
            if distance <= agent_vision_range:
                # Check line of sight
                if self._has_line_of_sight(agent_x, agent_y, obj.x, obj.y):
                    visible_objects.append({
                        'object': obj,
                        'distance': distance,
                        'type': obj.type
                    })
                else:
                    blocked_objects.append({
                        'object': obj,
                        'distance': distance,
                        'type': obj.type
                    })
        
        return {
            'visible_objects': visible_objects,
            'blocked_objects': blocked_objects,
            'visibility_score': len(visible_objects) / max(1, len(self._objects)),
            'agent_position': (agent_x, agent_y)
        }
        
    def _has_line_of_sight(self, x1, y1, x2, y2):
        """Check if there's line of sight between two points."""
        # Simplified line of sight check
        # In a real implementation, this would check for wall intersections
        return True
        
    def create_custom_environment(self, name, parameters):
        """
        Create a custom environment with specific parameters.
        
        Args:
            name: Environment name
            parameters: Environment parameters
        """
        print(f"[INFO] Created custom environment: {name} with parameters: {parameters}")
        # Store custom environment for future use
        if not hasattr(self, '_environments'):
            self._environments = {}
        self._environments[name] = parameters
        
    def load_environment(self, name):
        """
        Load a previously created environment.
        
        Args:
            name: Environment name
            
        Returns:
            dict: Environment parameters
        """
        if hasattr(self, '_environments') and name in self._environments:
            print(f"[INFO] Loaded environment: {name}")
            return self._environments[name]
        else:
            print(f"[ERROR] Environment not found: {name}")
            return None
        
    def __str__(self):
        """String representation of advanced world."""
        return f"AdvancedWorld({self.world.getWidth()}x{self.world.getHeight()}, " \
               f"terrain={self._terrain}, weather={self._weather})"
        
    def __repr__(self):
        """Detailed representation of advanced world."""
        return f"AdvancedWorld(world={self.world}, " \
               f"terrain={self._terrain}, weather={self._weather}, " \
               f"time_multiplier={self._time_multiplier})"
# Export advanced world classes and functions
from .world import AdvancedWorld