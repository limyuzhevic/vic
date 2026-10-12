#!/usr/bin/env python3

"""
NLM Visualization Module - Brain State Exploration and Analysis

This module provides advanced visualization and analysis tools:
- Brain state visualization and exploration
- Activity analysis and heatmaps
- Plot generation and statistical analysis
- Network topology visualization
"""

__version__ = "1.0.0"
__author__ = "NLM Team"

# Re-export visualization classes for easy access
from nlm.visualization.interface import VisualizationInterface
from nlm.visualization.analysis import BrainStateAnalyzer
from nlm.visualization.plotting import PlotGenerator

__all__ = [
    "VisualizationInterface", "BrainStateAnalyzer", "PlotGenerator",
    "__version__", "__author__"
]