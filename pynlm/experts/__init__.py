"""
NLM Expert Utilities

This module provides advanced utilities for expert users of the NLM neural simulation framework.
It includes performance optimization tools, memory management, and advanced simulation capabilities.

The utilities are organized into specialized modules:
- parallel: Parallel simulation execution for performance
- visualization: Advanced neural network visualization tools
- performance: Performance profiling and optimization
- monitoring: Real-time monitoring and alerting
"""

from .parallel import ParallelSimulationManager
from .visualization import NeuralVisualizer, ActivityPlotter, NetworkGrapher
from .performance import PerformanceProfiler, MemoryProfiler, SpeedProfiler
from .monitoring import RealTimeMonitor, MetricsCollector, AlertManager

__all__ = [
    'ParallelSimulationManager',
    'NeuralVisualizer',
    'ActivityPlotter',
    'NetworkGrapher',
    'PerformanceProfiler',
    'MemoryProfiler',
    'SpeedProfiler',
    'RealTimeMonitor',
    'MetricsCollector',
    'AlertManager'
]
