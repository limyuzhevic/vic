    return 0.0

# Configuration migration helper
def get_migration_guide():
    """
    Get migration guide for upgrading from simple config system.
    
    Returns:
        Migration guide text
    """
    return """
NLM Configuration System Migration Guide
=========================================

Upgrading from Simple Config System to Enhanced Config System

Changes Summary:
---------------
1. API Changes:
   - Old: config.get("key", default)
   - New: config.get("key", default)
   - Old: config.set("key", value)
   - New: config.set("key", value, source=ConfigSource.Runtime)
   - Old: config.has("key")
   - New: "key" in config or config.has("key")

2. New Features:
   - Configuration builder pattern (fluent API)
   - Template system with variable substitution
   - Advanced validation with schema support
   - Batch operations for performance
   - Configuration history tracking
   - Export/import utilities
   - Configuration presets for common use cases
   - Comparison and merging utilities
   - Type inference for automatic configuration
   - Category-based organization

Migration Steps:
---------------

Step 1: Basic Configuration
===========================

# Old way (simple config)
config = pynlm.createDefaultConfig()
config.set("brain.neuronCount", 1000)

# New way (enhanced config)
config = NLMConfig()
config['brain.neuronCount'] = 1000

# Or using builder pattern
config = NLMConfig().createBuilder() \
    .set("brain.neuronCount", 1000) \
    .build()

Step 2: Validation
===================

# Old way (basic validation)
def validate(config):
    return config.has("brain.neuronCount") and config["brain.neuronCount"] > 0

# New way (advanced validation)
config = NLMConfig()
config.enableValidation(True)
config.addValidator("brain.neuronCount", lambda x: x > 0, "Neuron count must be positive")

Step 3: Presets and Templates
=============================

# Load preset
config.loadPreset("research")

# Create template
template = ConfigurationTemplate("my_template", {"brain.neuronCount": 1000})
rendered = template.render({"SPEED": 1.0})

Step 4: Batch Operations
========================

# Old way (loop)
keys = ["a", "b", "c"]
for key in keys:
    config.set(key, values[keys.index(key)])

# New way (batch)
config.batchSet(keys, values)

Step 5: Export/Import
===================

# Export
config.exportToFile("config.json")

# Import
new_config = NLMConfig()
new_config.importFromFile("config.json")

Step 6: Comparison and Merging
=============================

# Compare
diff = config1.compare(config2)

# Merge
merged = config1.merge(config2)

Step 7: History Tracking
========================

# Enable history
config.enableHistory(True)

# Get history
changes = config.getHistory()

Step 8: Category Support
========================

# Set category
config.setCategory("brain.neuronCount", "Brain")

# Get by category
brain_configs = config.getConfigByCategory()

Migration Commands:
------------------

# 1. Replace old config with new config
python -c "from pynlm_enhanced import NLMConfig; config = NLMConfig()"

# 2. Convert existing config
python -c "
from pynlm_enhanced import NLMConfig
import json

# Load old config
with open('old_config.json') as f:
    old_data = json.load(f)

# Create new config
new_config = NLMConfig()
for key, value in old_data.items():
    new_config[key] = value

# Save as new format
new_config.exportToFile('new_config.json')
"

# 3. Update tests
python -m pytest tests/ -v  # Run all tests with new config system

Troubleshooting:
---------------

Q: My old code is using pynlm.Config directly.
A: Replace with NLMConfig() wrapper:

# Old
from pynlm import Config
config = Config()

# New
from pynlm_enhanced import NLMConfig
config = NLMConfig()

Q: I'm getting type errors with the enhanced API.
A: The enhanced API uses the same ConfigValue types as the original:
   - int, int64_t, double, bool for simple types
   - std::string for strings
   - std::vector<int>, std::vector<double>, std::vector<std::string> for lists

Q: My templates are not working.
A: Templates use ${variable} syntax for substitution:

template = {"speed": "${SPEED}"}
rendered = template.render({"SPEED": "1.0"})

Q: I need to access the original Core Config.
A: You can get the underlying core config:

enhanced_config = NLMConfig()
core_config = enhanced_config.getCoreConfig()

For more detailed migration examples, see:
- examples_advanced.py
- docs/migration_guide.md
"""


# Configuration comparison utilities
def compare_configs(config1, config2):
    """
    Compare two configurations and return differences.
    
    Args:
        config1: First configuration (NLMConfig or dict)
        config2: Second configuration (NLMConfig or dict)
        
    Returns:
        Dictionary with differences
    """
    if isinstance(config1, NLMConfig) and isinstance(config2, NLMConfig):
        return config1.compare(config2)
    
    # Convert dicts to NLMConfig
    config1_obj = NLMConfig() if isinstance(config1, dict) else config1
    config2_obj = NLMConfig() if isinstance(config2, dict) else config2
    
    if isinstance(config1, dict):
        for key, value in config1.items():
            config1_obj.set(key, value)
    
    if isinstance(config2, dict):
        for key, value in config2.items():
            config2_obj.set(key, value)
    
    return config1_obj.compare(config2_obj)


def merge_configs(*configs, strategy="replace"):
    """
    Merge multiple configurations.
    
    Args:
        *configs: Configuration objects to merge
        strategy: Merge strategy (replace, merge, union)
        
    Returns:
        Merged configuration
    """
    if not configs:
        return NLMConfig()
    
    result = configs[0].clone() if hasattr(configs[0], 'clone') else configs[0]
    for config in configs[1:]:
        if hasattr(config, 'merge') and hasattr(config, 'clone'):
            result = result.merge(config, strategy)
        else:
            # Manual merge for dicts
            if hasattr(result, '__setitem__'):
                for key, value in config.items():
                    result[key] = value
    
    return result


def export_config(config, filepath, format="json"):
    """
    Export configuration to file.
    
    Args:
        config: Configuration to export (NLMConfig or dict)
        filepath: Output file path
        format: Export format (json, yaml, ini)
    """
    if isinstance(config, NLMConfig):
        return config.export(filepath, format)
    
    # Convert dict to NLMConfig
    temp_config = NLMConfig()
    for key, value in config.items():
        temp_config.set(key, value)
    
    return temp_config.export(filepath, format)


def import_config(filepath, format="json"):
    """
    Import configuration from file.
    
    Args:
        filepath: Input file path
        format: Import format (json, yaml, ini)
        
    Returns:
        Imported configuration
    """
    config = NLMConfig()
    return config.import_config(filepath, format)


def auto_detect_config_type(value):
    """
    Automatically infer configuration type from value.
    
    Args:
        value: Value to infer type from
        
    Returns:
        Inferred type string
    """
    if isinstance(value, bool):
        return "bool"
    elif isinstance(value, int):
        return "int"
    elif isinstance(value, float):
        return "float"
    elif isinstance(value, str):
        return "string"
    elif isinstance(value, list):
        if all(isinstance(x, int) for x in value):
            return "int_list"
        elif all(isinstance(x, float) for x in value):
            return "float_list"
        elif all(isinstance(x, str) for x in value):
            return "string_list"
        else:
            return "mixed_list"
    else:
        return "unknown"


def create_validation_function(*validators):
    """
    Create a composite validation function.
    
    Args:
        *validators: Validation functions
        
    Returns:
        Composite validation function
    """
    def composite(value):
        return all(validator(value) for validator in validators)
    return composite


# Example configuration factories
def create_minimal_config():
    """Create a minimal configuration example."""
    return NLMConfig()


def create_standard_config():
    """Create a standard configuration example."""
    config = NLMConfig()
    config.setupStandard()
    return config


def create_high_performance_config():
    """Create a high-performance configuration example."""
    config = NLMConfig()
    config.setupHighPerformance()
    return config


def create_research_config():
    """Create a research configuration example."""
    config = NLMConfig()
    config.setupResearch()
    return config


# Error handling
class EnhancedRuntimeError(Exception):
    """Base exception for enhanced configuration errors."""
    pass


class EnhancedInvalidArgumentError(EnhancedRuntimeError):
    """Exception for invalid configuration arguments."""
    pass


class EnhancedOutOfRangeError(EnhancedRuntimeError):
    """Exception for out-of-range configuration values."""
    pass


# Factory functions for common use cases
def create_brain_config(neuron_count=1000, simulation_speed=1.0, 
                       enable_plasticity=True, enable_development=False):
    """
    Create a brain configuration.
    
    Args:
        neuron_count: Number of neurons
        simulation_speed: Simulation speed multiplier
        enable_plasticity: Enable synaptic plasticity
        enable_development: Enable developmental processes
        
    Returns:
        Configuration with brain settings
    """
    config = NLMConfig()
    config.set("brain.neuronCount", neuron_count)
    config.set("brain.simulationSpeed", simulation_speed)
    config.set("brain.enablePlasticity", enable_plasticity)
    config.set("brain.enableDevelopment", enable_development)
    return config


def create_sensory_config(vision_enabled=True, audio_enabled=False,
                         vision_width=640, vision_height=480):
    """
    Create a sensory configuration.
    
    Args:
        vision_enabled: Enable vision sensory input
        audio_enabled: Enable audio sensory input
        vision_width: Vision width in pixels
        vision_height: Vision height in pixels
        
    Returns:
        Configuration with sensory settings
    """
    config = NLMConfig()
    config.set("sensory.vision.enabled", vision_enabled)
    config.set("sensory.audio.enabled", audio_enabled)
    config.set("sensory.vision.width", vision_width)
    config.set("sensory.vision.height", vision_height)
    return config


def create_plasticity_config(learning_rate=0.01, max_connections=1000,
                            enable_structural_plasticity=False):
    """
    Create a plasticity configuration.
    
    Args:
        learning_rate: Synaptic learning rate
        max_connections: Maximum number of connections
        enable_structural_plasticity: Enable structural plasticity
        
    Returns:
        Configuration with plasticity settings
    """
    config = NLMConfig()
    config.set("plasticity.learningRate", learning_rate)
    config.set("plasticity.maxConnections", max_connections)
    config.set("plasticity.enableStructuralPlasticity", enable_structural_plasticity)
    return config


def create_development_config(enabled=True, critical_period_length=100,
                            maturation_rate=1.0):
    """
    Create a development configuration.
    
    Args:
        enabled: Enable development processes
        critical_period_length: Length of critical period
        maturation_rate: Rate of maturation
        
    Returns:
        Configuration with development settings
    """
    config = NLMConfig()
    config.set("development.enabled", enabled)
    config.set("development.criticalPeriodLength", critical_period_length)
    config.set("development.maturationRate", maturation_rate)
    return config


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
    
    # Example configurations
    'create_minimal_config',
    'create_standard_config',
    'create_high_performance_config',
    'create_research_config',
    
    # Factory functions
    'create_brain_config',
    'create_sensory_config',
    'create_plasticity_config',
    'create_development_config',
    
    # Error handling
    'EnhancedRuntimeError',
    'EnhancedInvalidArgumentError',
    'EnhancedOutOfRangeError',
    
    # Migration
    'get_migration_guide'
]
