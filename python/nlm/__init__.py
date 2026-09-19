"""
Enhanced NLM Configuration System - Python Bindings
====================================================

This module provides advanced Python bindings for the NLM configuration system
with comprehensive enhancements including:

- Configuration builder pattern
- Template system
- Validation and schema support
- Batch operations
- Configuration history tracking
- Export/import utilities
- Configuration presets
- Comparison and merging utilities
- Type inference
- Category-based organization
- Comprehensive error handling

Quick Usage:
------------

from pynlm_enhanced import NLMConfig, ConfigurationBuilder

# Create enhanced configuration
config = NLMConfig()

# Set values
config['brain.neuronCount'] = 1000
config['brain.simulationSpeed'] = 1.5

# Use builder pattern
config = (NLMConfig()
    .createBuilder()
    .set("brain.neuronCount", 1000)
    .set("brain.simulationSpeed", 1.5)
    .set("plasticity.enabled", True)
    .build())

# Load presets
config.loadPreset("research")

# Validate
config.enableValidation(True)
config.addValidator("brain.neuronCount", lambda x: x > 0)

# Export/Import
config.exportToFile("config.json")
new_config = NLMConfig()
new_config.importFromFile("config.json")

# Compare and merge
diff = config.compare(new_config)
merged = config.merge(new_config)

# History tracking
config.enableHistory(True)
change_history = config.getHistory()

For more detailed examples and documentation, see:
- examples_advanced.py - Comprehensive examples
- docs/ - Additional documentation
"""

# Core classes
from .nlm.config import (
    NLMConfig,
    ConfigurationBuilder,
    ConfigurationTemplate,
    ConfigRegistry
)

# Core utilities
from .nlm.utilities import (
    ValidationSchema,
    HistoryEntry,
    PresetData,
    ValidationRule,
    ConfigCategory,
    ConfigSource
)

# Specialized modules
from .nlm import builders
from .nlm import schema
from .nlm import migration
from .nlm import templates
from .nlm import registry

# Global configuration registry
from .nlm.registry import (
    register_config,
    get_config,
    list_configs,
    register_preset,
    get_preset,
    list_presets,
    register_template,
    get_template,
    list_templates
)

# Validation helpers
from .nlm.schema import (
    validate_int_range,
    validate_positive,
    validate_string_length,
    create_validation_function
)

# Comparison and merging utilities
from .nlm.utilities import (
    merge_configs,
    diff_configs,
    export_config,
    import_config
)

# Type inference
from .nlm.utilities import (
    auto_detect_config_type
)

# Example configurations
from .nlm.config import (
    example_minimal_config,
    example_standard_config,
    example_high_performance_config,
    example_research_config
)

# Error handling
from .nlm.config import (
    EnhancedRuntimeError,
    EnhancedInvalidArgumentError,
    EnhancedOutOfRangeError
)

# Module metadata
__version__ = "0.1.0"
__author__ = "NLM Project"
__license__ = "MIT"
__copyright__ = "Copyright 2024 NLM Project"

# Backward compatibility aliases
from .nlm.config import EnhancedConfig as LegacyEnhancedConfig
from .nlm.config import ConfigBuilder as LegacyConfigBuilder
from .nlm.config import ConfigTemplate as LegacyConfigTemplate

# Export all public symbols
__all__ = [
    # Core classes
    'NLMConfig',
    'ConfigurationBuilder',
    'ConfigurationTemplate',
    'ConfigRegistry',
    
    # Utilities
    'ValidationSchema',
    'HistoryEntry',
    'PresetData',
    'ValidationRule',
    'ConfigCategory',
    'ConfigSource',
    
    # Registry
    'register_config',
    'get_config',
    'list_configs',
    'register_preset',
    'get_preset',
    'list_presets',
    'register_template',
    'get_template',
    'list_templates',
    
    # Validation
    'validate_int_range',
    'validate_positive',
    'validate_string_length',
    'create_validation_function',
    
    # Comparison
    'merge_configs',
    'diff_configs',
    'export_config',
    'import_config',
    
    # Type inference
    'auto_detect_config_type',
    
    # Examples
    'example_minimal_config',
    'example_standard_config',
    'example_high_performance_config',
    'example_research_config',
    
    # Error handling
    'EnhancedRuntimeError',
    'EnhancedInvalidArgumentError',
    'EnhancedOutOfRangeError',
    
    # Backward compatibility
    'EnhancedConfig',
    'ConfigBuilder',
    'ConfigTemplate'
]
