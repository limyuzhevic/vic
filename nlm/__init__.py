"""
NLM Python Bindings Package

This package provides Python bindings for the NLM (Neural Learning Machine) neural simulation framework.
It includes high-level APIs for creating brains, running simulations, and analyzing results.

The package is organized around the following main components:

- **Core API**: Basic functionality for brain creation and simulation
- **Convenience Functions**: High-level functions for common use cases
- **Example Scripts**: Demonstrations of advanced usage patterns
- **Utilities**: Helper functions for research and analysis
"""

import sys
import os

# Add the build directory to the path so we can import the C++ extension
build_dir = os.path.join(os.path.dirname(__file__), "build")
if os.path.exists(build_dir):
    sys.path.insert(0, build_dir)

# Import core bindings
try:
    from .bindings import *
    has_bindings = True
except ImportError:
    has_bindings = False
    print("Warning: NLM C++ bindings not found. Please build the project first.")
    print("Run: mkdir build && cd build && cmake .. && make pynlm")

# Import convenience functions
from . import examples
from . import utils
from . import research

# Re-export important classes and functions for backward compatibility
__all__ = [
    # Core classes
    "Brain",
    "Config", 
    "Action",
    "SensoryPercept",
    "SimpleWorld",
    "AgentBrain",
    "AgentBody",
    "WorldObject",
    "ActionResult",
    "Vision",
    "Audio",
    "InternalSignals",
    
    # Convenience functions
    "create_default_config",
    "create_demo_config", 
    "createBrain",
    "createSimpleWorld",
    "createAgentBrain",
    "create_agent",
    "run_simulation",
    "save_brain_state",
    "load_brain_state",
    "validate_config",
    "profile_brain",
    
    # Constants and enums
    "NEURON_TYPE_EXCITATORY",
    "NEURON_TYPE_INHIBITORY",
    "NEURON_TYPE_MODULATORY",
    "NEURON_TYPE_SENSORY",
    "NEURON_TYPE_MOTOR",
    "NEURON_TYPE_INTERNAL",
    
    "ACTION_TYPE_MOVE_FORWARD",
    "ACTION_TYPE_MOVE_BACKWARD",
    "ACTION_TYPE_TURN_LEFT",
    "ACTION_TYPE_TURN_RIGHT",
    "ACTION_TYPE_LOOK",
    "ACTION_TYPE_INTERACT",
    "ACTION_TYPE_REST",
    "ACTION_TYPE_WAIT",
    
    "WORLD_OBJECT_EMPTY",
    "WORLD_OBJECT_RESOURCE",
    "WORLD_OBJECT_HAZARD",
    "WORLD_OBJECT_WALL",
    "WORLD_OBJECT_MARKER",
    
    "DEVELOPMENTAL_STAGE_INITIAL",
    "DEVELOPMENTAL_STAGE_CRITICAL_PERIOD",
    "DEVELOPMENTAL_STAGE_MATURATION",
    "DEVELOPMENTAL_STAGE_ADULT",
    "DEVELOPMENTAL_STAGE_AGING",
    
    # Invalid IDs
    "INVALID_NEURON_ID",
    "INVALID_SYNAPSE_ID",
    "INVALID_REGION_ID",
    "INVALID_POPULATION_ID",
    
    # Package information
    "__version__",
    "__author__",
    
    # Submodules
    "examples",
    "utils",
    "research",
]

# Package version
__version__ = "0.1.0"
__author__ = "NLM Research Team"

# Check if we have the C++ bindings
if has_bindings:
    print(f"NLM Python bindings v{__version__} loaded successfully")
else:
    print(f"NLM Python bindings v{__version__} - C++ bindings not available")
    print("Please build the project using: mkdir build && cd build && cmake .. && make pynlm")

# Default configuration factory function
def create_default_config():
    """
    Create a default configuration for NLM.
    
    Returns:
        Config: A new configuration object with sensible defaults.
    """
    if not has_bindings:
        raise RuntimeError("NLM C++ bindings not available. Please build the project first.")
    
    return Config()

def create_demo_config():
    """
    Create a demo configuration with values suitable for testing and demonstrations.
    
    Returns:
        Config: A demo configuration with commonly used values.
    """
    if not has_bindings:
        raise RuntimeError("NLM C++ bindings not available. Please build the project first.")
    
    config = Config()
    
    # Set demo values
    config.set("brain.neuron_count", 1000)
    config.set("brain.simulation_steps", 1000)
    config.set("world.width", 100.0)
    config.set("world.height", 100.0)
    config.set("world.vision_width", 10)
    config.set("world.vision_height", 10)
    config.set("random_seed", 42)
    config.set("simulation_timestep", 0.001)
    
    return config

def create_simple_demo_brain(config=None):
    """
    Create a simple demo brain with default configuration.
    
    Args:
        config (Config, optional): Configuration to use. If None, creates demo config.
        
    Returns:
        Brain: A demo brain instance.
    """
    if not has_bindings:
        raise RuntimeError("NLM C++ bindings not available. Please build the project first.")
    
    if config is None:
        config = create_demo_config()
    
    brain = createBrain(config)
    brain.initialize()
    
    # Add default regions
    brain.addRegion("sensory")
    brain.addRegion("motor")
    brain.addRegion("association")
    
    return brain

def create_simple_demo_world():
    """
    Create a simple demo world with default settings.
    
    Returns:
        SimpleWorld: A demo world instance.
    """
    if not has_bindings:
        raise RuntimeError("NLM C++ bindings not available. Please build the project first.")
    
    world = createSimpleWorld()
    world.configure(100, 100, 10, 10)
    world.setAgentStart(50.0, 50.0)
    world.setRandomSeed(42)
    
    return world
# Error handling utilities
def check_bindings_available():
    """
    Check if the NLM C++ bindings are available.
    
    Returns:
        bool: True if bindings are available, False otherwise.
    """
    return has_bindings

# Diagnostic function
def diagnostic():
    """
    Run diagnostic checks and report system status.
    
    Returns:
        dict: Dictionary containing diagnostic information.
    """
    import platform
    import numpy as np
    
    diagnostics = {
        "python_version": sys.version,
        "platform": platform.platform(),
        "has_bindings": has_bindings,
        "numpy_version": np.__version__,
        "package_version": __version__,
        "package_location": __file__,
    }
    
    if has_bindings:
        try:
            # Try to create a simple brain to test functionality
            config = create_default_config()
            brain = createBrain(config)
            brain.initialize()
            diagnostics["bindings_test"] = "SUCCESS"
            
            # Get basic info
            stats = brain.get_statistics()
            diagnostics["brain_info"] = {
                "neuron_count": stats.get("total_neurons", 0),
                "region_count": stats.get("region_count", 0),
            }
            
        except Exception as e:
            diagnostics["bindings_test"] = f"FAILED: {str(e)}"
    
    return diagnostics

# Quick start helper
def quick_start():
    """
    Display quick start information and example code.
    
    This function prints helpful information for new users.
    """
    print("=== NLM Python Bindings Quick Start ===\n")
    print("1. Basic Usage:")
    print("```python")
    print("import nlm")
    print("brain = nlm.createBrain(nlm.create_default_config())")
    print("brain.initialize()")
    print("world = nlm.create_simple_demo_world()")
    print("nlm.run_simulation(brain, world, 1000)")
    print("```\n")
    
    print("2. Create Action:")
    print("```python")
    print("action = nlm.Action(nlm.ACTION_TYPE_MOVE_FORWARD)")
    print("action.add_param(1.0)  # Distance")
    print("action_dict = action.to_dict()")
    print("```\n")
    
    print("3. Configuration:")
    print("```python")
    print("config = nlm.create_default_config()")
    print("config.set_int('brain.neuron_count', 2000)")
    print("neuron_count = config.get_int('brain.neuron_count', 1000)")
    print("```\n")
    
    print("4. Sensory Data:")
    print("```python")
    print("percept = world.getSensoryPercept()")
    print("percept_dict = percept.to_dict()")
    print("vision_data = percept_dict['vision']")
    print("```\n")
    
    print("For more information, visit:")
    print("  - Documentation: https://nlm.readthedocs.io")
    print("  - Examples: https://github.com/nlm-project/nlm/tree/main/examples")
    print("  - GitHub: https://github.com/nlm-project/nlm")
    print("\n=== End Quick Start ===")

# Print quick start info when package is first imported
if os.environ.get('NLM_SHOW_QUICK_START', '').lower() == 'true':
    quick_start()
