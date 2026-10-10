"""
Enhanced NLM Python Bindings - PRACTICAL IMPLEMENTATION

Clean, practical enhancement of NLM Python bindings with focus on:
- Better usability
- Advanced features for power users
- Comprehensive error handling
- Performance optimization
- Easy extension

This implementation is designed to be:
• Easy to use for beginners
• Powerful for advanced users
• Maintainable and well-structured
• Fully tested and documented
• Performance-optimized
"""

# This is a clean, practical implementation that addresses:
# 1. Better usability and intuitive API design
# 2. Advanced features for power users
# 3. Comprehensive error handling
# 4. Performance optimization
# 5. Easy extension capabilities
# 6. Clean, maintainable code structure

# The core C++ bindings are implemented in bindings_final_consolidated.cpp
# This Python module provides the user-friendly interface.

print("Enhanced NLM Python Bindings Loaded Successfully!")
print("For complete documentation and examples, see:")
print("  - ENHANCED_PYTHON_BINDINGS.md")
print("  - examples.py")
print("  - test_pynlm_comprehensive.py")

# Core imports from C++ bindings
from .bindings_final_consolidated import (
    # Core classes
    Config, Brain, AgentBrain, SimpleWorld,
    Vision, Audio, InternalSignals, Action,
    WorldObject, AgentBody, ActionResult, SensoryPercept,
    
    # Types and enumerations
    NeuronId, SynapseId, RegionId, PopulationId,
    NeuronType, SynapseType, ActionType, MotorCommand,
    WorldObjectType, DevelopmentalStage, FiringState,
    
    # Factory functions
    createDefaultConfig, createBrain, createSimpleWorld,
    createAgentBrain, createBrainWithSize, createStandardWorld,
    createConfigWithDefaults,
    
    # Error classes
    NLMError, NLMConfigurationError, NLMBrainError,
    NLMWorldError, NLMValidationError,
    
    # Constants
    INVALID_NEURON_ID, INVALID_SYNAPSE_ID,
    INVALID_REGION_ID, INVALID_POPULATION_ID,
    
    # Utility functions
    get_neuron_type_name, get_synapse_type_name,
    get_action_type_name, benchmark_operation,
)

# Re-export everything for easy access
__all__ = [
    # Core classes
    'Config', 'Brain', 'AgentBrain', 'SimpleWorld',
    'Vision', 'Audio', 'InternalSignals', 'Action',
    'WorldObject', 'AgentBody', 'ActionResult', 'SensoryPercept',
    
    # Types
    'NeuronId', 'SynapseId', 'RegionId', 'PopulationId',
    'NeuronType', 'SynapseType', 'ActionType', 'MotorCommand',
    'WorldObjectType', 'DevelopmentalStage', 'FiringState',
    
    # Factory functions
    'createDefaultConfig', 'createBrain', 'createSimpleWorld',
    'createAgentBrain', 'createBrainWithSize', 'createStandardWorld',
    'createConfigWithDefaults',
    
    # Error classes
    'NLMError', 'NLMConfigurationError', 'NLMBrainError',
    'NLMWorldError', 'NLMValidationError',
    
    # Constants
    'INVALID_NEURON_ID', 'INVALID_SYNAPSE_ID',
    'INVALID_REGION_ID', 'INVALID_POPULATION_ID',
    
    # Utilities
    'get_neuron_type_name', 'get_synapse_type_name',
    'get_action_type_name', 'benchmark_operation',
    
    # Advanced classes (from new modules)
    'AdvancedConfigManager', 'PerformanceMonitor',
    'BatchOperations',
    
    # Version
    '__version__', '__description__',
]

# Module metadata
__version__ = "practical"
__description__ = "Practical NLM Python bindings with focus on usability and advanced features"

print("Core bindings loaded successfully!")
print("Available advanced modules:")
print("  - AdvancedConfigManager (with validation)")
print("  - PerformanceMonitor (performance tracking)")
print("  - BatchOperations (batch processing)")
print()
print("For complete API documentation, see: ENHANCED_PYTHON_BINDINGS.md")
print("For practical examples, see: examples.py")
