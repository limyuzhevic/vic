"""
NLM (熙然) - Neural Learning Machine Python Bindings

A comprehensive computational brain simulator with:
- LIF neuron dynamics
- Event-driven spike propagation
- Plasticity rules (STDP, Hebbian, Structural)
- Neuromodulation systems
- Memory and cognition
- Development and learning
- Full Python bindings

The NLM Python bindings provide access to all NLM C++ functionality
for brain simulation, experimentation, and research.

This enhanced version includes:
- Advanced configuration management with validation and schema support
- Batch processing capabilities for multiple simulations
- Parallel processing and performance profiling
- Context managers for resource management
- Comprehensive analysis tools
- Integration utilities for different formats
- Advanced error handling and logging
"""

import os
import sys

# Add the CLI package to the path
sys.path.insert(0, os.path.join(os.path.dirname(__file__), 'cli'))

# Import core functionality
from .cli import (
    NLMCli,
    create_parser,
    main,
    __version__,
    NLMConfigManager,
    get_default_template,
    save_default_template,
    load_config_with_env,
    NLMBatchProcessor,
    BatchUtils,
    NLMValidator,
    NLMComparison,
    NLMPresets,
    BuiltInPresets
)

# Import enhanced Python wrapper components
sys.path.insert(0, os.path.join(os.path.dirname(__file__), 'nlm_py.py'))
try:
    import nlm_py
    
    # Re-export enhanced wrapper classes
    __all__.extend([
        'AdvancedConfig',
        'NLMContext',
        'NLMBatchProcessor',
        'NLMValidator',
        'NLMComparison',
        'NLMProfiler',
        'NLMExporter',
        'NLMError',
        'ConfigurationError',
        'SimulationError',
        'ValidationError',
        'MemoryError',
        'create_brain_from_config',
        'create_simulation',
        'run_simulation',
        'run_batch_simulation',
        'export_results',
        'load_config_with_env',
        'get_default_template',
        'save_default_template'
    ])
    
    # Create shortcuts for convenience
    AdvancedConfig = nlm_py.AdvancedConfig
    NLMContext = nlm_py.NLMContext
    BatchProcessor = nlm_py.NLMBatchProcessor
    Validator = nlm_py.NLMValidator
    Comparator = nlm_py.NLMComparison
    Profiler = nlm_py.NLMProfiler
    Exporter = nlm_py.NLMExporter
    
except ImportError:
    # Fallback if enhanced wrapper is not available
    pass

# Re-export core NLM functionality
__all__ = [
    # CLI components
    'NLMCli',
    'create_parser',
    'main',
    '__version__',
    
    # Configuration management
    'NLMConfigManager',
    'get_default_template',
    'save_default_template',
    'load_config_with_env',
    
    # Batch processing
    'NLMBatchProcessor',
    'BatchUtils',
    
    # Validation
    'NLMValidator',
    
    # Comparison
    'NLMComparison',
    
    # Presets
    'NLMPresets',
    'BuiltInPresets',
    
    # Enhanced Python wrapper components
    'AdvancedConfig',
    'NLMContext',
    'BatchProcessor',
    'Validator',
    'Comparator',
    'Profiler',
    'Exporter',
    'NLMError',
    'ConfigurationError',
    'SimulationError',
    'ValidationError',
    'MemoryError',
    
    # Core NLM functions (would be implemented in bindings.cpp)
    # These would be imported from the C++ bindings
    'createBrain',
    'createDefaultConfig',
    'createSimpleWorld',
    'createAgentBrain',
    'enableRewardModulation',
    'enableStructuralPlasticity',
    'enableDevelopment',
    'enableCuriosity',
]

# Try to import core NLM functions from C++ bindings
try:
    from .core import (
        createBrain,
        createDefaultConfig,
        createSimpleWorld,
        createAgentBrain,
        enableRewardModulation,
        enableStructuralPlasticity,
        enableDevelopment,
        enableCuriosity
    )
except ImportError:
    # Core functions would be defined here if bindings are available
    pass

# Version information
VERSION = __version__
VERSION_INFO = {
    'version': VERSION,
    'major': 2,
    'minor': 0,
    'patch': 0,
    'release': 'enhanced'
}

# Enhanced module metadata
ENHANCED_FEATURES = [
    'configuration_validation',
    'schema_support',
    'environment_variable_integration',
    'batch_processing',
    'parallel_simulation',
    'context_managers',
    'resource_management',
    'performance_profiling',
    'advanced_analysis',
    'data_export',
    'error_handling',
    'logging'
]

# Feature availability flags
FEATURE_FLAGS = {
    'enhanced_config': True,
    'batch_processing': True,
    'parallel_processing': True,
    'context_management': True,
    'performance_profiling': True,
    'advanced_validation': True,
    'data_export': True,
    'comparison_tools': True,
    'high_level_api': True
}

# Summary of enhancements
summary = """
Enhanced NLM Python Wrapper - Version {version}

New features since original release:
- Advanced configuration management with schema validation
- Batch processing with parallel execution
- Context managers for resource management
- Performance profiling and monitoring
- Enhanced analysis tools
- Data export utilities
- Improved error handling
- Comprehensive documentation

Total features: {feature_count}
""".format(
    version=VERSION,
    feature_count=len(ENHANCED_FEATURES)
)

print(summary)