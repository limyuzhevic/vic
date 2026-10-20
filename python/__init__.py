"""
NLM (Neural Learning Machine) Python Bindings

A Python interface for the NLM C++ neural simulation framework.
Provides classes for Brain, Config, AgentBrain, SimpleWorld, and more.

This module provides a high-level, Pythonic API for creating and simulating
artificial brains with spiking neural networks, memory systems, and learning capabilities.
"""

from .core import *
from .brain import *
from .memory import *
from .agent import *
from .world import *
from .sensory import *
from .neuromodulation import *
from .cognition import *
from .development import *
from .visualization import *
from .experiments import *

__all__ = [
    # Core classes
    'Config',
    'Brain',
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
    
    # Data types
    'NeuronId',
    'SynapseId',
    'RegionId',
    'PopulationId',
    
    # Enumerations
    'NeuronType',
    'SynapseType',
    'DevelopmentalStage',
    'FiringState',
    'ActionType',
    'MotorCommand',
    'WorldObjectType',
    
    # Factory functions
    'createDefaultConfig',
    'createBrain',
    'createSimpleWorld',
    'createAgentBrain',
    
    # Constants
    'INVALID_NEURON_ID',
    'INVALID_SYNAPSE_ID',
    'INVALID_REGION_ID',
    'INVALID_POPULATION_ID',
    
    # Error types
    'NLMError',
    'ConfigurationError',
    'InitializationError',
    'RuntimeError',
    
    # Research/Advanced features
    'getSimulationStats',
    'benchmarkSimulation',
    'profilePerformance',
    
    # Enhanced Visualization Features
    'AnalyticsEngine',
    'DebugLogger',
    'RealTimeVisualizer',
    'InteractivePlotting',
    'ExportManager',
    'ComparisonAnalyzer',
    'EducationalToolkit',
    'LogLevel',
    'VisualizationStatus',
    'is_visualization_feature_available',
    'get_visualization_capabilities',
    'initialize_visualization',
    'shutdown_visualization',
    'get_global_visualizer',
    'get_global_analytics',
    'get_global_debug_logger',
    'get_global_exporter',
    'get_global_comparison_analyzer',
    'get_global_educational_toolkit',
    
    # New Visualization Functions
    'real_time_monitor',
    'live_plot_brain_activity',
    'analyze_brain_statistics',
    'profile_brain_performance',
    'monitor_resource_usage',
    'enable_debug_logging',
    'track_errors',
    'monitor_component_health',
    'create_matplotlib_plots',
    'create_interactive_plots',
    'create_jupyter_notebook',
    'export_to_csv',
    'export_to_json',
    'export_to_image',
    'compare_simulations',
    'compare_performance',
    'compare_configurations',
    'create_tutorial',
    'generate_example',
    'create_learning_mode',
]

# Python version compatibility
import sys
if sys.version_info >= (3, 8):
    from typing import Optional, List, Dict, Any, Union, Tuple
    import numpy as np
else:
    from typing_extensions import Optional, List, Dict, Any, Union, Tuple
    import numpy as np

# Module metadata
__version__ = "0.1.0"
__author__ = "NLM Project"
__license__ = "MIT"
__description__ = "Python bindings for NLM neural simulation framework"

def get_version() -> str:
    """Get the current version of NLM."""
    return __version__

def get_info() -> Dict[str, Any]:
    """
    Get information about the NLM Python bindings.
    
    Returns:
        Dictionary containing version, author, and other module information.
    """
    return {
        'version': __version__,
        'author': __author__,
        'license': __license__,
        'description': __description__,
        'python_version': f"{sys.version_info.major}.{sys.version_info.minor}.{sys.version_info.micro}",
        'numpy_available': True,
    }

def benchmark_simulation(config_dict: Optional[Dict[str, Any]] = None, 
                         num_steps: int = 1000) -> Dict[str, Any]:
    """
    Run a benchmark simulation with specified configuration.
    
    Args:
        config_dict: Optional dictionary of configuration parameters.
        num_steps: Number of simulation steps to run.
        
    Returns:
        Dictionary containing benchmark results including timing,
        performance metrics, and resource usage.
    """
    pass  # Implementation will be in the native modules

def profile_performance(brain, num_steps: int = 1000) -> Dict[str, Any]:
    """
    Profile the performance of a brain simulation.
    
    Args:
        brain: Brain object to profile.
        num_steps: Number of steps to profile.
        
    Returns:
        Dictionary containing performance metrics including timing,
        memory usage, and computational statistics.
    """
    pass  # Implementation will be in the native modules

def get_simulation_stats(brain) -> Dict[str, Any]:
    """
    Get comprehensive statistics about a brain simulation.
    
    Args:
        brain: Brain object to analyze.
        
    Returns:
        Dictionary containing simulation statistics including neuron counts,
        spike statistics, memory usage, and performance metrics.
    """
    pass  # Implementation will be in the native modules