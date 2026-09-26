"""
Enhanced NLM Python API Module

This is the main entry point for the Enhanced NLM Python API package.
It provides enhanced Python interfaces for the NLM (Neural Learning Machine) neural simulation framework.
"""

__version__ = "1.0.0"
__author__ = "NLM Development Team"
__email__ = "nlm@example.com"
__license__ = "MIT"

# Import core classes from the enhanced bindings
from nlmenhanced import (
    # Core classes from enhanced bindings
    Brain,
    Config,
    AgentBrain,
    SimpleWorld,
    SensoryInput,
    Vision,
    Audio,
    InternalSignals,
    Action,
    WorldObject,
    AgentBody,
    ActionResult,
    SensoryPercept,
    
    # Enhanced exception classes
    NLMError,
    ConfigurationError,
    InitializationError,
    SimulationError,
    VisualizationError,
    
    # Context managers
    BrainContext,
    ExperimentContext,
    
    # Visualization and analysis
    NLMVisualizer,
    
    # Experiment management
    ExperimentManager,
    ExperimentResult,
    
    # Enumerations
    Profile,
    VisualizationType,
    ExportFormat,
    
    # Data classes
    NeuronMetrics,
    SimulationResult,
    ExperimentConfig,
    
    # Utility functions
    create_brain,
    run_simulation,
    create_experiment,
    create_default_brain,
    run_quick_simulation,
    initialize,
    get_available_features,
)

# Define package metadata
__all__ = [
    # Core classes from enhanced bindings
    'Brain',
    'Config',
    'AgentBrain',
    'SimpleWorld',
    'SensoryInput',
    'Vision',
    'Audio',
    'InternalSignals',
    'Action',
    'WorldObject',
    'AgentBody',
    'ActionResult',
    'SensoryPercept',
    
    # Enhanced exception classes
    'NLMError',
    'ConfigurationError',
    'InitializationError',
    'SimulationError',
    'VisualizationError',
    
    # Context managers
    'BrainContext',
    'ExperimentContext',
    
    # Visualization and analysis
    'NLMVisualizer',
    
    # Experiment management
    'ExperimentManager',
    'ExperimentResult',
    
    # Enumerations
    'Profile',
    'VisualizationType',
    'ExportFormat',
    
    # Data classes
    'NeuronMetrics',
    'SimulationResult',
    'ExperimentConfig',
    
    # Utility functions
    'create_brain',
    'run_simulation',
    'create_experiment',
    'create_default_brain',
    'run_quick_simulation',
    'initialize',
    'get_available_features',
    
    # Package metadata
    '__version__',
    '__author__',
    '__email__',
    '__license__',
]

# Initialize the package
import sys
import os

# Check if we have the necessary dependencies
required_deps = ['numpy', 'pandas', 'scipy']
missing_deps = []

for dep in required_deps:
    try:
        __import__(dep)
    except ImportError:
        missing_deps.append(dep)

if missing_deps:
    print(f"Warning: Missing required dependencies: {missing_deps}")
    print("Please install them with: pip install 'nlmenhanced[{missing_deps[0]}]'")
    print("Or install all dependencies with: pip install 'nlmenhanced[full]'")

# Try to import the enhanced bindings
try:
    from nlmenhanced.bindings import (
        Brain,
        Config,
        AgentBrain,
        SimpleWorld,
        SensoryInput,
        Vision,
        Audio,
        InternalSignals,
        Action,
        WorldObject,
        AgentBody,
        ActionResult,
        SensoryPercept,
    )
except ImportError as e:
    # If the bindings module is not available, we'll use a fallback
    # This allows the package to be imported even if the C++ bindings are not built
    print(f"Warning: Could not import enhanced bindings: {e}")
    print("The enhanced Python API may not work properly without the C++ bindings.")

# Module-level function for initialization
def init():
    """
    Initialize the Enhanced NLM Python API module.
    
    This function performs any necessary initialization when the module
    is imported. It checks for dependencies and sets up the environment.
    
    Returns:
        dict: A dictionary with initialization status information
    """
    status = {
        'version': __version__,
        'initialized': True,
        'dependencies': [],
        'features': get_available_features(),
        'status': 'ready'
    }
    
    return status

# Check if running in interactive mode
if __name__ == "__main__":
    # Print module information
    print(f"Enhanced NLM Python API v{__version__}")
    print(f"Author: {__author__}")
    print(f"License: {__license__}")
    print()
    print("Available features:")
    for feature in get_available_features():
        print(f"  • {feature}")
    print()
    print("To get help on specific functions, use: help(function_name)")
    print("Example: help(create_brain)")
    print()
    print("For documentation and examples, see the README.md file.")
    print("For support, visit the NLM documentation website.")

# Set up module-level aliases for backward compatibility
# These provide compatibility with the original pynlm API
__all__ = __all__ + [
    # Backward compatibility aliases
    'createDefaultConfig',
    'createQuickConfig',
    'createBrain',
    'createSimpleWorld',
    'createAgentBrain',
    'runQuickSimulation',
    'analyzeResults',
]

# Define the fallback functions for backward compatibility
# These will be overridden if the enhanced bindings are available
def createDefaultConfig():
    """Backward compatibility function."""
    from nlmenhanced import BrainConfigurator
    return BrainConfigurator.createDefaultConfig()
def createQuickConfig(profile="balanced"):
    """Backward compatibility function."""
    from nlmenhanced import BrainConfigurator
    return BrainConfigurator.createQuickStartConfig(profile)
def createBrain(config):
    """Backward compatibility function."""
    from nlmenhanced import BrainConfigurator
    BrainConfigurator.validateConfig(config)
    from nlmenhanced import createBrain as create_brain_func
    return create_brain_func(config)
def createSimpleWorld():
    """Backward compatibility function."""
    from nlmenhanced import SimpleWorld
    return SimpleWorld()
def createAgentBrain(brain):
    """Backward compatibility function."""
    from nlmenhanced import AgentBrain
    return AgentBrain(brain)
def runQuickSimulation(brain=None, steps=100):
    """Backward compatibility function."""
    from nlmenhanced import Profile
    if brain is None:
        brain = create_brain(nlm.Profile.BALANCED)
    from nlmenhanced import run_simulation
    return run_simulation(brain, steps, Profile.BALANCED)
def analyzeResults(results):
    """Backward compatibility function."""
    from nlmenhanced import SimulationResult
    if isinstance(results, SimulationResult):
        return results.calculate_statistics()
    return {}