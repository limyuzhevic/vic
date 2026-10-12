#!/usr/bin/env python3

"""
NLM Monitoring Module - Performance Monitoring and Profiling

This module provides real-time performance monitoring:
- Real-time performance monitoring with alerts
- System performance profiling (CPU, memory, I/O)
- Memory allocator profiling and optimization
- Performance dashboards and metrics
"""

__version__ = "1.0.0"
__author__ = "NLM Team"

# Re-export monitoring classes for easy access
from nlm.monitoring.real_time import RealTimeMonitor
from nlm.monitoring.profiler import SystemProfiler
from nlm.monitoring.allocator import MemoryAllocatorProfiler

__all__ = [
    "RealTimeMonitor", "SystemProfiler", "MemoryAllocatorProfiler",
    "__version__", "__author__"
]