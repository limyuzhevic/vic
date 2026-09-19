"""
NLM Python Module

A Python binding for the NLM C++ neural simulation framework.
Provides classes for Brain, Config, AgentBrain, SimpleWorld, SensoryInput, and Action.
Includes comprehensive neural utilities for advanced neural manipulation and debugging.
"""

from .config import (
    ConfigManager,
    ConfigProfile,
    ConfigGroup,
    ValidationLevel,
    ConfigSource,
    ConfigEntry,
    ConfigChange,
    ConfigSchema,
    ConfigMonitor,
    create_default_config,
    create_test_config,
    create_development_config,
)

from .utilities import (
    NeuralGroup,
    BrainState,
    NeuralUtils,
    NeuralManipulationMode,
    PerformanceMetrics,
    DebugEvent,
    DebugEventType,
    VisualizationData,
    ConnectivityGraph,
    NeuralSimulation,
    SimulationResult,
    PerformanceMonitor,
    create_neural_utils,
    export_neural_data,
    import_neural_data,
    create_neural_simulation,
    monitor_neural_performance,
)

__version__ = "0.1.0"

__all__ = [
    # Configuration classes
    "ConfigManager",
    "ConfigProfile",
    "ConfigGroup",
    "ValidationLevel",
    "ConfigSource",
    "ConfigEntry",
    "ConfigChange",
    "ConfigSchema",
    "ConfigMonitor",
    "create_default_config",
    "create_test_config",
    "create_development_config",
    # Neural utilities classes
    "NeuralGroup",
    "BrainState",
    "NeuralUtils",
    "NeuralManipulationMode",
    "PerformanceMetrics",
    "DebugEvent",
    "DebugEventType",
    "VisualizationData",
    "ConnectivityGraph",
    "NeuralSimulation",
    "SimulationResult",
    "PerformanceMonitor",
    # Factory functions
    "create_neural_utils",
    "export_neural_data",
    "import_neural_data",
    "create_neural_simulation",
    "monitor_neural_performance",
    # Version
    "__version__",
]