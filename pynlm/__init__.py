"""
pynlm - Comprehensive Neural Language Modeling Utilities

This package provides modular utilities for neural language modeling experiments,
including configuration management, experiment tracking, simulation control,
visualization, analysis, checkpoint management, and advanced debugging capabilities.

Key Features:
- Modular design for easy extension
- Comprehensive error handling and validation
- Performance optimization utilities
- Advanced simulation control
- Comprehensive visualization tools
- Robust checkpoint management
- Advanced logging and debugging capabilities

Usage Example:
    >>> from pynlm import Simulation, Experiment
    >>> from pynlm.utils import load_config, create_preset
    >>> from pynlm.visualization import plot_activity
    >>> 
    >>> config = load_config("config.yaml")
    >>> experiment = Experiment("my_experiment")
    >>> experiment.run_simulation(config)
    >>> 
    >>> # Advanced debugging
    >>> from pynlm.debug import get_brain_state_inspector
    >>> inspector = get_brain_state_inspector()
    >>> snapshot = inspector.create_snapshot(step=10, neurons=neuron_data)
"""

__version__ = "1.0.0"
__author__ = "NLM Team"
__email__ = "nlm@example.com"

# Core imports
from .utils import *
from .experiments import *
from .visualization import *
from .analysis import *
from .simulation import *
from .checkpoint import *

# Debugging and logging modules
from .logging import *
from .debug import *
from .state_inspection import *
from .visualization_debug import *
from .error_reporting import *
from .export_debug import *

# Package information
__all__ = [
    # utils
    'load_config', 'save_config', 'create_preset', 'merge_configs',
    'Timer', 'ProgressBar', 'validate_parameters', 'safe_divide', 'safe_mean', 'safe_std',
    'CheckpointManager', 'format_time', 'format_bytes', 'sanitize_filename',
    'setup_logging', 'get_package_info',
    
    # experiments
    'Experiment', 'ExperimentRunner', 'ParameterSweep', 'BaselineSetup',
    'DataLogger', 'ResultCollector', 'ExperimentTemplate',
    
    # visualization
    'ActivityPlotter', 'SimulationVisualizer', 'ResultGrapher',
    'InteractiveVisualizer', 'PlotStyle', 'ColorMap',
    
    # analysis
    'NeuralPatternAnalyzer', 'PerformanceMetrics', 'StatisticalAnalyzer',
    'ComparativeAnalyzer', 'PatternDetector', 'CorrelationAnalyzer',
    
    # simulation
    'Simulation', 'AgentSimulator', 'WorldManager', 'EpisodeController',
    'RewardManager', 'LearningController', 'SimulationLoop',
    
    # checkpoint
    'CheckpointManager', 'AutoCheckpoint', 'StateRestorer',
    'VersionManager', 'BackupManager',
    
    # logging
    'NLMLogManager', 'JSONFormatter', 'LogContext',
    'get_log_manager', 'log_simulation_step', 'log_brain_region', 'log_agent',
    'get_brain_logger', 'get_memory_logger', 'get_simulation_logger',
    'get_plastic_logger', 'get_visualization_logger', 'get_checkpoint_logger',
    
    # debug
    'BrainStateSnapshot', 'BrainStateInspector', 'SimulationDebugger',
    'ConnectionValidator', 'MemoryMonitor', 'PerformanceProfiler',
    'brain_state_inspector', 'simulation_debugger', 'connection_validator',
    'memory_monitor', 'performance_profiler',
    
    # state inspection
    'BrainStateInfo', 'StateInspector', 'BrainStateSnapshotter',
    'ConnectionInspector', 'MemoryContentInspector', 'NeuralActivityVisualizer',
    'brain_state_snapshotter', 'connection_inspector', 'memory_content_inspector',
    'neural_activity_visualizer',
    
    # visualization debug
    'VisualizationState', 'VisualizationDebugger', 'RenderingDebugger',
    'DataValidator', 'visualization_debugger', 'rendering_debugger', 'data_validator',
    
    # error reporting
    'ErrorSeverity', 'ErrorCategory', 'ErrorContext', 'ErrorDetails',
    'ErrorReporter', 'error_reporter', 'get_error_reporter',
    'report_brain_state_error', 'report_simulation_error', 'report_memory_error',
    
    # export debug
    'DebugReport', 'DebugReportGenerator', 'StateExporter',
    'ConfigurationExporter', 'PerformanceReportGenerator',
    'debug_report_generator', 'state_exporter', 'configuration_exporter',
    'performance_report_generator'
]

# Setup logging for the package
import logging
setup_logging()

# Version check
def check_version():
    """Check if pynlm version is compatible with requirements."""
    required_version = "1.0.0"
    if __version__ != required_version:
        import warnings
        warnings.warn(f"Version mismatch: expected {required_version}, got {__version__}")
        return False
    return True

# Initialize package
check_version()