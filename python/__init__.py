"""
Enhanced NLM Python API

This module provides an enhanced, high-level Python interface to the NLM (Neural Learning Machine)
neural simulation framework. It builds upon the core pybind11 bindings to offer a more intuitive
and powerful experience for users of all skill levels.

Key Features:
- High-level convenience functions for common use cases
- Advanced simulation modes and presets
- Enhanced error handling and debugging tools
- Performance optimizations for Python users
- Comprehensive configuration management
- Advanced learning algorithms
- Visualization and analysis tools
- Real-time monitoring and statistics

Examples and documentation are provided throughout the module.
"""

import sys
import time
import threading
from typing import Optional, List, Dict, Any, Union, Callable
from dataclasses import dataclass, field
from enum import Enum
import warnings

# Core NLM bindings
import pynlm as nlm_core

__version__ = "enhanced-1.0.0"
__all__ = [
    'NLMSystem',
    'SimulationMode',
    'AgentBuilder',
    'ExperimentManager',
    'VisualizationManager',
    'PerformanceProfiler',
    'NeuralNetworkDesigner',
    'AdvancedConfigurator',
    'createDefaultConfig',
    'createBrain',
    'runSimulation',
    'benchmarkPerformance',
    'analyzeActivity',
]
