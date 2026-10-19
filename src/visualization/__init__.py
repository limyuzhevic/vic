"""
NLM Visualization and Monitoring Package

This package provides comprehensive visualization and monitoring capabilities
for the NLM (Neural Learning Machine) framework. It includes:

- Real-time brain visualization
- Memory system monitoring
- Neuromodulation tracking
- Plasticity analysis
- Development visualization
- Prediction system monitoring
- Performance metrics collection
- Resource usage tracking
- Web-based dashboards
- Data export tools
"""

from .visualization_system import VisualizationSystem
from .real_time_monitor import RealTimeMonitor
from .neural_network_visualizer import NeuralNetworkVisualizer
from .memory_system_visualizer import MemorySystemVisualizer
from .neuromodulation_visualizer import NeuromodulationVisualizer
from .plasticity_visualizer import PlasticityVisualizer
from .development_visualizer import DevelopmentVisualizer
from .prediction_visualizer import PredictionVisualizer
from .visualization_interface import VisualizationInterface

# Version
__version__ = "0.1.0"

# Package metadata
__author__ = "NLM Team"
__description__ = "Comprehensive visualization and monitoring for NLM"
__license__ = "MIT"

# Export key classes
__all__ = [
    "VisualizationSystem",
    "RealTimeMonitor",
    "NeuralNetworkVisualizer",
    "MemorySystemVisualizer",
    "NeuromodulationVisualizer",
    "PlasticityVisualizer",
    "DevelopmentVisualizer",
    "PredictionVisualizer",
    "VisualizationInterface",
    "__version__",
    "__author__",
    "__description__",
    "__license__"
]
