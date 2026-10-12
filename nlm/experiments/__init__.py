#!/usr/bin/env python3

"""
NLM Experiments Module - Experiment Management and Comparison

This module provides comprehensive experiment management:
- Experiment tracking and management
- Experiment comparison with statistical analysis
- Experiment analysis and insights
- Batch experiment execution
"""

__version__ = "1.0.0"
__author__ = "NLM Team"

# Re-export experiment classes for easy access
from nlm.experiments.manager import ExperimentManager
from nlm.experiments.comparison import ExperimentComparator
from nlm.experiments.analysis import ExperimentAnalyzer

__all__ = [
    "ExperimentManager", "ExperimentComparator", "ExperimentAnalyzer",
    "__version__", "__author__"
]