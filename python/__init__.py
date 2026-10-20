__version__ = "0.1.0"

# Import core bindings
import pynlm

# Import high-level API
from .high_level_api import (
    create_brain_with_preset,
    run_learning_experiment,
    train_agent,
    benchmark_performance,
    plot_brain_network,
    plot_activity_raster,
    plot_weight_matrix,
    create_video,
    train_predictive_model,
    optimize_weights,
    run_reinforcement_learning,
    cross_validate,
    monitor_performance,
    measure_compatibility,
    profile_activity,
    export_for_debugging,
    import_from_checkpoint,
    validate_configuration,
    generate_config_template,
    BrainSimulator,
    HighLevelAPIError
)

# Import utilities
from .utils import (
    DataProcessor,
    TimeSeriesGenerator,
    ConfigurationValidator,
    ProgressTracker,
    CheckpointManager,
    BatchProcessor,
    NLMUtilsError,
    get_memory_usage,
    get_cpu_usage,
    time_operation,
    NLMContextManager
)

# Re-export commonly used classes and functions for convenience
__all__ = [
    # Core bindings
    'pynlm',
    
    # High-level API
    'create_brain_with_preset',
    'run_learning_experiment',
    'train_agent',
    'benchmark_performance',
    'plot_brain_network',
    'plot_activity_raster',
    'plot_weight_matrix',
    'create_video',
    'train_predictive_model',
    'optimize_weights',
    'run_reinforcement_learning',
    'cross_validate',
    'monitor_performance',
    'measure_compatibility',
    'profile_activity',
    'export_for_debugging',
    'import_from_checkpoint',
    'validate_configuration',
    'generate_config_template',
    'BrainSimulator',
    'HighLevelAPIError',
    
    # Utilities
    'DataProcessor',
    'TimeSeriesGenerator',
    'ConfigurationValidator',
    'ProgressTracker',
    'CheckpointManager',
    'BatchProcessor',
    'NLMUtilsError',
    'get_memory_usage',
    'get_cpu_usage',
    'time_operation',
    'NLMContextManager',
    
    # Version
    '__version__'
]

# Define Python version requirement
if sys.version_info < (3, 8):
    raise ImportError("pynlm requires Python 3.8 or higher")

# Import sys for version check
import sys
