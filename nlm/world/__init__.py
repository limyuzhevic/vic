"""
NLM World Module

World simulation module for the NLM neural simulation framework.
Includes world models, object management, and agent interaction systems.
"""

from .simple_world import SimpleWorld
from .world_object import WorldObject
from .agent_body import AgentBody
from .action_result import ActionResult

__all__ = [
    'SimpleWorld',
    'WorldObject',
    'AgentBody',
    'ActionResult',
]

# Python utilities for world module
from .python_utils import (
    PythonSessionLogger,
    PythonTimer,
    numpy_utils,
    numpy_export,
    numpy_visualization,
)

# Re-export utilities
__all__.extend([
    'PythonSessionLogger',
    'PythonTimer',
    'numpy_utils',
    'numpy_export',
    'numpy_visualization',
])

# World utility functions

def create_default_world_config():
    """Create a default configuration for world simulation"""
    from nlm.core import Config
    
    config = Config()
    config.set("world_width", 100.0)
    config.set("world_height", 100.0)
    config.set("vision_width", 84)
    config.set("vision_height", 84)
    config.set("max_energy", 100.0)
    config.set("energy_decay_rate", 0.01)
    config.set("simulation_time_step", 0.1)
    
    return config

def validate_world_config(config):
    """Validate world configuration"""
    from nlm.core import Config
    
    if not isinstance(config, Config):
        raise TypeError("config must be a Config instance")
    
    errors = []
    
    # Check required keys
    required_keys = ["world_width", "world_height", "vision_width", "vision_height"]
    
    for key in required_keys:
        if not config.has(key):
            errors.append(f"Missing required configuration: {key}")
    
    # Validate value ranges
    if config.has("world_width"):
        width = config.get_float("world_width")
        if width is not None and (width < 1.0 or width > 1000.0):
            errors.append("world_width must be between 1.0 and 1000.0")
    
    if config.has("world_height"):
        height = config.get_float("world_height")
        if height is not None and (height < 1.0 or height > 1000.0):
            errors.append("world_height must be between 1.0 and 1000.0")
    
    return errors

# Module initialization
def _init_module():
    """Initialize the world module"""
    try:
        logger = PythonSessionLogger()
        logger.logEntry("World module initialized successfully")
    except Exception:
        pass  # Module may not have logger available yet

# Initialize module
_init_module()

# Command-line interface utilities
def export_world_state(world, filepath):
    """
    Export world state to file.
    
    Args:
        world: SimpleWorld object to export
        filepath: Path to export file
    """
    import json
    import os
    
    try:
        # Create export data
        export_data = {
            "world_state": {
                "width": world.getWidth(),
                "height": world.getHeight(),
                "agent_x": world.getAgentBody().x,
                "agent_y": world.getAgentBody().y,
                "agent_orientation": world.getAgentBody().orientation,
                "simulation_time": world.getSimulationTime(),
                "max_energy": world.getMaxEnergy(),
                "energy_decay_rate": world.getEnergyDecayRate(),
                "objects": [],
            }
        }
        
        # Add world objects
        for i in range(10):  # Example - should iterate over actual objects
            obj = world.getObject(i)  # This would need to be implemented
            if obj:
                export_data["world_state"]["objects"].append({
                    "x": obj.x,
                    "y": obj.y,
                    "type": obj.type.value if hasattr(obj.type, 'value') else str(obj.type),
                    "radius": obj.radius,
                    "value": obj.value,
                    "active": obj.active,
                })
        
        # Save to JSON file
        with open(filepath, 'w') as f:
            json.dump(export_data, f, indent=2)
        
        return True
        
    except Exception as e:
        print(f"Failed to export world: {e}")
        return False

def import_world_state(filepath):
    """
    Import world state from file.
    
    Args:
        filepath: Path to import file
        
    Returns:
        SimpleWorld object or None if import failed
    """
    import json
    import os
    
    try:
        if not os.path.exists(filepath):
            print(f"File not found: {filepath}")
            return None
        
        # Load from JSON file
        with open(filepath, 'r') as f:
            data = json.load(f)
        
        # Create world configuration
        from nlm.core import Config
        
        world_config = Config()
        
        # Set configuration from import data
        if "world_state" in data:
            state_data = data["world_state"]
            world_config.set("world_width", state_data.get("width", 100.0))
            world_config.set("world_height", state_data.get("height", 100.0))
            world_config.set("vision_width", 84)
            world_config.set("vision_height", 84)
            world_config.set("max_energy", state_data.get("max_energy", 100.0))
            world_config.set("energy_decay_rate", state_data.get("energy_decay_rate", 0.01))
        
        # Create world (placeholder)
        # world = SimpleWorld(world_config)
        world = None
        
        return world
        
    except Exception as e:
        print(f"Failed to import world: {e}")
        return None

__all__.extend([
    'create_default_world_config',
    'validate_world_config',
    'export_world_state',
    'import_world_state',
])
