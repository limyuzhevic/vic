"""
NLM Visualization Module

Visualization and data analysis module for the NLM neural simulation framework.
Includes numpy-based visualization functions and data export/import utilities.
"""

from .python_utils import (
    PythonSessionLogger,
    PythonTimer,
    numpy_utils,
    numpy_export,
    numpy_visualization,
)

__all__ = [
    'PythonSessionLogger',
    'PythonTimer',
    'numpy_utils',
    'numpy_export',
    'numpy_visualization',
]

# Visualization utility functions

def create_default_visualization_config():
    """Create a default configuration for visualization"""
    return {
        "theme": "default",
        "width": 800,
        "height": 600,
        "dpi": 100,
        "show_grid": True,
        "show_axes": True,
        "colormap": "viridis",
        "animation": False,
        "animation_speed": 1.0,
    }

def validate_visualization_config(config):
    """Validate visualization configuration"""
    errors = []
    
    # Check required keys
    required_keys = ["width", "height", "dpi"]
    
    for key in required_keys:
        if key not in config:
            errors.append(f"Missing required visualization config: {key}")
    
    # Validate value ranges
    if "width" in config and (config["width"] < 100 or config["width"] > 2000):
        errors.append("width must be between 100 and 2000")
    
    if "height" in config and (config["height"] < 100 or config["height"] > 2000):
        errors.append("height must be between 100 and 2000")
    
    if "dpi" in config and (config["dpi"] < 50 or config["dpi"] > 300):
        errors.append("dpi must be between 50 and 300")
    
    if "animation_speed" in config and config["animation_speed"] <= 0:
        errors.append("animation_speed must be positive")
    
    return errors

# Module initialization
def _init_module():
    """Initialize the visualization module"""
    try:
        logger = PythonSessionLogger()
        logger.logEntry("Visualization module initialized successfully")
    except Exception:
        pass  # Module may not have logger available yet

# Initialize module
_init_module()

# Command-line interface utilities
def export_visualization_data(world, brain, filepath, format="json"):
    """
    Export visualization data to file.
    
    Args:
        world: SimpleWorld object
        brain: Brain object
        filepath: Path to export file
        format: Export format ("json", "csv", "npy")
        
    Returns:
        True if successful, False otherwise
    """
    import os
    
    try:
        # Get visualization data
        viz_data = {
            "timestamp": time.time(),
            "world": {
                "width": world.getWidth(),
                "height": world.getHeight(),
                "agent_x": world.getAgentBody().x,
                "agent_y": world.getAgentBody().y,
                "simulation_time": world.getSimulationTime(),
            },
            "brain": {
                "total_neurons": brain.getStatus().get("total_neurons", 0),
                "total_synapses": brain.getStatus().get("total_synapses", 0),
                "active_neurons": brain.getStatus().get("active_neurons", 0),
                "average_firing_rate": brain.getStatus().get("average_firing_rate", 0.0),
                "excitation_inhibition_ratio": brain.getStatus().get("excitation_inhibition_ratio", 1.0),
            }
        }
        
        # Export based on format
        if format.lower() == "json":
            import json
            with open(filepath, 'w') as f:
                json.dump(viz_data, f, indent=2)
                
        elif format.lower() == "csv":
            import pandas as pd
            df = pd.DataFrame(viz_data)
            df.to_csv(filepath, index=False)
            
        elif format.lower() == "npy":
            import numpy as np
            np.save(filepath, viz_data)
            
        else:
            raise ValueError(f"Unsupported export format: {format}")
        
        return True
        
    except Exception as e:
        print(f"Failed to export visualization data: {e}")
        return False

def import_visualization_data(filepath, format="json"):
    """
    Import visualization data from file.
    
    Args:
        filepath: Path to import file
        format: Import format ("json", "csv", "npy")
        
    Returns:
        Dictionary with imported data, or None if import failed
    """
    import os
    
    try:
        if not os.path.exists(filepath):
            print(f"File not found: {filepath}")
            return None
        
        # Import based on format
        if format.lower() == "json":
            import json
            with open(filepath, 'r') as f:
                data = json.load(f)
                
        elif format.lower() == "csv":
            import pandas as pd
            df = pd.read_csv(filepath)
            data = df.to_dict('records')
            
        elif format.lower() == "npy":
            import numpy as np
            data = np.load(filepath, allow_pickle=True).item()
            
        else:
            raise ValueError(f"Unsupported import format: {format}")
        
        return data
        
    except Exception as e:
        print(f"Failed to import visualization data: {e}")
        return None

__all__.extend([
    'create_default_visualization_config',
    'validate_visualization_config',
    'export_visualization_data',
    'import_visualization_data',
])
