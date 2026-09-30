#!/usr/bin/env python3
"""
Comprehensive NLM Visualization and Analysis Tools

This module provides advanced visualization and analysis capabilities for the NLM (Neural Learning Machine) system.
It extends the existing basic visualization system with comprehensive tools for:

1. Core Visualization Components:
   - Neural Network Visualization (structure and activity)
   - Spike Train Visualization (time-series)
   - Memory System Visualization (contents and activities)
   - Plasticity Visualization (synaptic weight changes)
   - Development Visualization (developmental stages)
   - Neuromodulation Visualization (neuromodulator levels)

2. Analysis Tools:
   - Statistical Analysis (neural activity and system behavior)
   - Pattern Recognition (automatic pattern detection)
   - Network Analysis (connectivity and topology)
   - Learning Analysis (learning progress and efficiency)
   - Performance Analysis (system benchmarking)
   - Comparative Analysis (experiment comparisons)

3. Interface Options:
   - GUI (interactive visualization with controls)
   - Web-based Visualization (browser-based with real-time updates)
   - Command-line Visualization (text-based for terminal)
   - Export Capabilities (multiple formats)
   - Integration (with existing monitoring systems)

4. Advanced Features:
   - Interactive Visualization (clickable and zoomable)
   - Data Filtering and Selection
   - Annotation and Marking
   - Multi-panel Displays
   - Custom Visualization Plugins

5. Performance Optimization:
   - Efficient Rendering (large-scale optimizations)
   - Data Streaming (large dataset handling)
   - Level of Detail (adaptive quality)
   - Hardware Acceleration (GPU support)

6. Integration with Existing Systems:
   - Monitoring Integration
   - Python Library Integration
   - WebSocket Integration
   - REST API Integration

This system provides comprehensive insight into NLM brain operation,
making it easier for researchers and developers to understand, debug,
and optimize neural systems.
"""

from .visualization_core import (
    NLMVisualizer, VisualizationComponent, VisualizationBackend,
    NeuralNetworkVisualizer, SpikeTrainVisualizer, MemoryVisualizer,
    PlasticityVisualizer, DevelopmentVisualizer, NeuromodulationVisualizer
)

from .analysis_tools import (
    NLMDataAnalyzer, AnalysisTool, StatisticalAnalyzer,
    PatternRecognizer, NetworkAnalyzer, LearningAnalyzer,
    PerformanceAnalyzer, ComparativeAnalyzer
)

from .interface_manager import (
    VisualizationInterface, GUIInterface, WebInterface,
    CLIInterface, ExportManager, IntegrationManager
)

from .advanced_features import (
    InteractiveVisualizer, DataFilter, AnnotationSystem,
    MultiPanelDisplay, PluginSystem, PerformanceOptimizer
)

from .integrations import (
    MonitoringIntegration, PythonIntegration, WebSocketIntegration,
    RESTAPIIntegration
)

from .core_components import (
    RealTimeMonitor, DataStreamer, LevelOfDetailManager,
    HardwareAccelerator, ResourceMonitor
)

__all__ = [
    # Core Visualization Components
    'NLMVisualizer',
    'VisualizationComponent',
    'VisualizationBackend',
    'NeuralNetworkVisualizer',
    'SpikeTrainVisualizer',
    'MemoryVisualizer',
    'PlasticityVisualizer',
    'DevelopmentVisualizer',
    'NeuromodulationVisualizer',
    
    # Analysis Tools
    'NLMDataAnalyzer',
    'AnalysisTool',
    'StatisticalAnalyzer',
    'PatternRecognizer',
    'NetworkAnalyzer',
    'LearningAnalyzer',
    'PerformanceAnalyzer',
    'ComparativeAnalyzer',
    
    # Interface Options
    'VisualizationInterface',
    'GUIInterface',
    'WebInterface',
    'CLIInterface',
    'ExportManager',
    'IntegrationManager',
    
    # Advanced Features
    'InteractiveVisualizer',
    'DataFilter',
    'AnnotationSystem',
    'MultiPanelDisplay',
    'PluginSystem',
    'PerformanceOptimizer',
    
    # Integration with Existing Systems
    'MonitoringIntegration',
    'PythonIntegration',
    'WebSocketIntegration',
    'RESTAPIIntegration',
    
    # Performance Optimization
    'RealTimeMonitor',
    'DataStreamer',
    'LevelOfDetailManager',
    'HardwareAccelerator',
    'ResourceMonitor'
]
