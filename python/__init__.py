#!/usr/bin/env python3
"""
NLM Visualization and Debugging Interface
Main Python interface for NLM visualization and debugging functionality.

This module provides:
- Enhanced visualization with real-time graphics
- Comprehensive debugging tools for neural simulation
- Memory leak detection and performance profiling
- Event timeline visualization
- Cross-platform debugging utilities
- Python bindings for all visualization features

Usage:
    from nlm_viz import createEnhancedVisualization, createDebugTools
    import pynlm
    
    # Create enhanced visualization
    viz = createEnhancedVisualization(1600, 900, "NLM Neural Network")
    
    # Create debugging tools
    debug = createDebugTools()
    debug.startDebugging()
    
    # Run simulation with visualization
    config = pynlm.createDefaultConfig()
    config.set("neuron_count", 1000)
    config.set("region_count", 4)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    for step in range(1000):
        brain.step(step)
        viz.update()
        viz.render()
        debug.updateDebugDisplay()
        
        if step % 100 == 0:
            viz.addEvent("simulation", f"Completed step {step}", step * 0.01)
            debug.logEvent("simulation", f"Step {step}", step * 0.01)
    
    # Export results
    viz.exportVisualizationData("results/simulation_data.json")
    debug.exportDebugData("results/debug_data.json")
    
    viz.close()
    debug.stopDebugging()
"""

import sys
import os
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

from .enhanced_api import (
    EnhancedVisualization,
    createEnhancedVisualization,
    createDebugTools,
    createMemoryProfiler,
    createPerformanceAnalyzer,
    createNeuralStateInspector,
    createEventTimeline
)

__all__ = [
    'EnhancedVisualization',
    'createEnhancedVisualization',
    'createDebugTools',
    'createMemoryProfiler',
    'createPerformanceAnalyzer',
    'createNeuralStateInspector',
    'createEventTimeline',
    'VisualizationData'
]

print("NLM Visualization Module loaded successfully")
