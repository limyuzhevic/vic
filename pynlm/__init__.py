"""
Enhanced Python API for NLM (Neural Learning Machine)

This module provides comprehensive Python bindings for the NLM neural simulation framework,
enhancing the basic pybind11 bindings with additional functionality, better error handling,
and extended capabilities for both beginners and advanced users.

Features:
- Enhanced configuration management with validation
- Comprehensive performance monitoring integration
- Advanced visualization and plotting capabilities
- Batch processing and experiment automation
- Configuration templates and validation
- Real-time progress tracking
- Advanced debugging and profiling tools
"""

from .core import (
    NLM,
    NMConfiguration,
    ExperimentResult,
    PerformanceMonitor,
)
from .utils import (
    create_default_nlm,
    run_multiple_experiments,
    get_performance_summary,
    NLMError,
    ConfigurationError,
    PerformanceError,
    ExperimentError,
)

__all__ = [
    # Core classes
    'NLM',
    'NMConfiguration',
    'ExperimentResult',
    'PerformanceMonitor',
    # Utility functions
    'create_default_nlm',
    'run_multiple_experiments',
    'get_performance_summary',
    # Exception classes
    'NLMError',
    'ConfigurationError',
    'PerformanceError',
    'ExperimentError',
]

__version__ = "0.1.0"
__author__ = "NLM Authors"
__description__ = "Python bindings for NLM (Neural Learning Machine) C++ neural simulation framework"
