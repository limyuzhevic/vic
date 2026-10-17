"""
NLM (Neural Learning Machine) - Python Bindings
Phase 3: Advanced Features for Expert Users

This package provides advanced-level utilities and tools for neural simulation
with fine-grained control, sophisticated performance analysis, and comprehensive
event tracing capabilities.

Phase 3 introduces expert-level features beyond the basic Phase 2 interfaces,
including:

- Advanced Brain Manipulation Utilities
- Performance Monitoring Tools
- Enhanced Configuration Management
- Advanced Serialization and Persistence
- Visualization and Monitoring

The Phase 3 bindings enable research-grade analysis capabilities and
provide the flexibility and control required for complex neural simulations.
"""

# Import all advanced features from Phase 3
from .nlm import (
    # Advanced Brain Manipulation
    AdvancedBrainManipulator,
    
    # Performance Monitoring
    PerformanceMonitor,
    MetricRecorder,
    TimelineRecorder,
    Profiler,
    MemoryMonitor,
    
    # Enhanced Configuration Management
    ConfigManager,
    
    # Advanced Serialization and Persistence
    SerializationManager,
    
    # Visualization and Monitoring
    VisualizationTools,
    
    # Factory functions
    createAdvancedBrainManipulator,
    createPerformanceMonitor,
    createMetricRecorder,
    createTimelineRecorder,
    createProfiler,
    createMemoryMonitor,
    createConfigManager,
    createSerializationManager,
    createVisualizationTools,
    
    # Constants and utilities
    INVALID_NEURON_ID,
    INVALID_SYNAPSE_ID,
    INVALID_REGION_ID,
    INVALID_POPULATION_ID,
    __version__,
    __phase__,
    EXAMPLE_CODE,
    ERROR_HANDLING,
    PERFORMANCE_TIPS,
    MIGRATION_GUIDE,
)

# Re-export the main brain types for convenience
from .nlm import Brain, Config, AgentBrain, SimpleWorld, NeuronId, SynapseId

# Package metadata
__author__ = "NLM Development Team"
__email__ = "support@nlm.org"
__license__ = "MIT"

# Help text
_HELP_TEXT = '''
Phase 3 Advanced Features for NLM Python Bindings

Available Features:
------------------

1. Advanced Brain Manipulation Utilities
   ------------------------------------------------
   AdvancedBrainManipulator
   - Direct neuron/synapse creation, modification, and deletion
   - Memory system access and control
   - Plasticity rule customization (Hebbian, STDP, reward-modulated)
   - Development stage control
   - Regional connectivity management

   Usage:
       manipulator = AdvancedBrainManipulator(brain)
       neuron = manipulator.createNeuron(region_id, NeuronType.Excitatory)
       synapse = manipulator.createSynapse(neuron1, neuron2, weight=0.5)
       manipulator.setHebbianLearningRate(0.01)

2. Performance Monitoring Tools
   ------------------------------------------------
   PerformanceMonitor
   - Real-time performance tracking with detailed metrics
   - Step timing analysis, spike rate monitoring
   - Stability analysis and performance reporting

   MetricRecorder
   - Custom metric recording with statistical analysis
   - Time-series support and data export
   - Mean, stddev, min, max, median calculations

   TimelineRecorder
   - Event timeline recording with categorization
   - Step tracking with custom metrics
   - Event filtering and querying capabilities

   Profiler
   - Function execution time profiling
   - Memory usage tracking
   - Bottleneck identification
   - Call pattern analysis

   MemoryMonitor
   - Memory usage tracking with detailed breakdown
   - Peak memory and growth rate analysis
   - Component-specific memory monitoring

3. Enhanced Configuration Management
   ------------------------------------------------
   ConfigManager
   - Template-based configuration creation
   - Configuration validation with detailed errors
   - Config inheritance and merging
   - Parameter sensitivity analysis
   - Configuration history and recovery

   Usage:
       config_manager = ConfigManager()
       template_config = Config()
       config_manager.addTemplate("default", "Default configuration", template_config)
       config = config_manager.createFromTemplate("default")

4. Advanced Serialization and Persistence
   ------------------------------------------------
   SerializationManager
   - HDF5 support for large datasets
   - Pickle-compatible serialization
   - Incremental save/restore operations
   - Compressed storage support
   - Checkpoint management for recovery

   Usage:
       serializer = SerializationManager()
       serializer.saveHDF5("simulation.h5", config, datasets)
       serializer.createCheckpoint("milestone1", config, brain)
       serializer.restoreCheckpoint("milestone1")

5. Visualization and Monitoring
   ------------------------------------------------
   VisualizationTools
   - Neural network structure visualization
   - Activity monitoring utilities
   - Real-time plotting capabilities
   - Results visualization frameworks
   - Comparison plotting for experiments

   Usage:
       visualizer = VisualizationTools()
       visualizer.visualizeNetwork("network.png", brain)
       visualizer.createRealTimeMonitor("monitor.html", brain)
       visualizer.createResultsDashboard("dashboard.html", [brain1, brain2])

Factory Functions:
------------------
All advanced features provide factory functions for convenient instantiation:

- createAdvancedBrainManipulator(brain)
- createPerformanceMonitor()
- createMetricRecorder()
- createTimelineRecorder()
- createProfiler()
- createMemoryMonitor()
- createConfigManager()
- createSerializationManager()
- createVisualizationTools()

Error Handling:
--------------
The module provides comprehensive error handling:

- ValidationError: Configuration validation errors
- BrainError: Brain manipulation errors
- ConfigError: Configuration management errors
- WorldError: World simulation errors
- NLMError: Generic NLM errors

Example Usage:
-------------
See nlm.EXAMPLE_CODE for complete working examples of all advanced features.
See nlm.ERROR_HANDLING for error handling examples.
See nlm.PERFORMANCE_TIPS for performance optimization recommendations.
See nlm.MIGRATION_GUIDE for Phase 2 to Phase 3 migration guidance.

For more information:
------------------
- Version: {}
- Phase: {}
- Documentation: Available in the module docstring
- Examples: Run 'print(nlm.EXAMPLE_CODE)'
'''

print(_HELP_TEXT.format(__version__, __phase__))

# Backward compatibility aliases
# These provide compatibility with Phase 2 interfaces if needed
__all__ = [
    'AdvancedBrainManipulator',
    'PerformanceMonitor',
    'MetricRecorder',
    'TimelineRecorder',
    'Profiler',
    'MemoryMonitor',
    'ConfigManager',
    'SerializationManager',
    'VisualizationTools',
    'Brain',
    'Config',
    'AgentBrain',
    'SimpleWorld',
    'NeuronId',
    'SynapseId',
    'createAdvancedBrainManipulator',
    'createPerformanceMonitor',
    'createMetricRecorder',
    'createTimelineRecorder',
    'createProfiler',
    'createMemoryMonitor',
    'createConfigManager',
    'createSerializationManager',
    'createVisualizationTools',
    'INVALID_NEURON_ID',
    'INVALID_SYNAPSE_ID',
    'INVALID_REGION_ID',
    'INVALID_POPULATION_ID',
    '__version__',
    '__phase__',
    'EXAMPLE_CODE',
    'ERROR_HANDLING',
    'PERFORMANCE_TIPS',
    'MIGRATION_GUIDE',
]