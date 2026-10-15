"""
NLM Python API Enhancements

This package contains enhanced Python modules for NLM (Neural Learning Machine)
with improved convenience methods, advanced features, command-line tools,
configuration management, error handling, benchmarking, profiling,
visualization, and debugging capabilities.
"""

__version__ = "1.0.0"
__author__ = "NLM Project"
__description__ = "Enhanced Python API for NLM with advanced features"

from .convenience_api import (
    createQuickBrain,
    createTrainingEnvironment,
    configureFromPreset,
    BrainManager,
    ExperimentManager,
    PerformanceProfiler,
    runLearningEpisode,
    validateBrainHealth
)

from .advanced_features import (
    TransferLearner,
    AdvancedVisualizer,
    ModelRegistry,
    TrainingFramework,
    PerformanceOptimizer,
    MultiAgentSystem,
    createAdvancedExperiment,
    createNeuralArchitectureSearch,
    createMetaLearningSystem
)

from .nlm_cli import main
from .config_management import (
    ConfigValidator,
    ConfigMigrator,
    ConfigTemplateManager,
    AutoConfigGenerator,
    createConfigValidator,
    createConfigMigrator,
    createTemplateManager,
    createAutoGenerator
)

from .error_handling import (
    ErrorHandler,
    MemoryManager,
    SafeExecutor,
    ResourceMonitor,
    createErrorHandler,
    createMemoryManager,
    createSafeExecutor,
    createResourceMonitor
)

from .benchmarking import (
    BenchmarkSuite,
    Profiler,
    PerformanceAnalyzer,
    StressTester,
    createBenchmarkSuite,
    createProfiler,
    createPerformanceAnalyzer,
    createStressTester
)

from .visualization import (
    VisualizationManager,
    Debugger,
    NeuralVisualizer,
    MemoryDebugger,
    PerformanceDebugger,
    TimelineDebugger,
    createVisualizationManager,
    createDebugger,
    createNeuralVisualizer,
    createMemoryDebugger,
    createPerformanceDebugger
)

__all__ = [
    # Convenience API
    'createQuickBrain',
    'createTrainingEnvironment',
    'configureFromPreset',
    'BrainManager',
    'ExperimentManager',
    'PerformanceProfiler',
    'runLearningEpisode',
    'validateBrainHealth',
    
    # Advanced Features
    'TransferLearner',
    'AdvancedVisualizer',
    'ModelRegistry',
    'TrainingFramework',
    'PerformanceOptimizer',
    'MultiAgentSystem',
    'createAdvancedExperiment',
    'createNeuralArchitectureSearch',
    'createMetaLearningSystem',
    
    # Command Line Interface
    'main',
    
    # Configuration Management
    'ConfigValidator',
    'ConfigMigrator',
    'ConfigTemplateManager',
    'AutoConfigGenerator',
    'createConfigValidator',
    'createConfigMigrator',
    'createTemplateManager',
    'createAutoGenerator',
    
    # Error Handling and Memory Management
    'ErrorHandler',
    'MemoryManager',
    'SafeExecutor',
    'ResourceMonitor',
    'createErrorHandler',
    'createMemoryManager',
    'createSafeExecutor',
    'createResourceMonitor',
    
    # Benchmarking and Profiling
    'BenchmarkSuite',
    'Profiler',
    'PerformanceAnalyzer',
    'StressTester',
    'createBenchmarkSuite',
    'createProfiler',
    'createPerformanceAnalyzer',
    'createStressTester',
    
    # Visualization and Debugging
    'VisualizationManager',
    'Debugger',
    'NeuralVisualizer',
    'MemoryDebugger',
    'PerformanceDebugger',
    'TimelineDebugger',
    'createVisualizationManager',
    'createDebugger',
    'createNeuralVisualizer',
    'createMemoryDebugger',
    'createPerformanceDebugger',
    
    # Package metadata
    '__version__',
    '__author__',
    '__description__'
]