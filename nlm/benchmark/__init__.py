#!/usr/bin/env python3

"""
NLM Benchmark Module - Performance Testing and Analysis

This module provides advanced benchmarking tools for performance analysis:
- Scaling benchmarks for different neural network sizes
- Performance monitoring with millisecond precision
- Memory usage tracking and analysis
"""

__version__ = "1.0.0"
__author__ = "NLM Team"

# Re-export benchmark classes for easy access
from nlm.benchmark.scaling import ScalingBenchmark
from nlm.benchmark.performance import PerformanceMonitor
from nlm.benchmark.memory import MemoryProfiler

__all__ = [
    "ScalingBenchmark", "PerformanceMonitor", "MemoryProfiler",
    "__version__", "__author__"
]