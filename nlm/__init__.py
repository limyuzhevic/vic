from __future__ import annotations
"""
NLM - Neural Learning Machine Python API

A comprehensive Python interface for the NLM neural simulation framework,
providing high-level abstractions, debugging tools, and advanced features
for working with spiking neural networks.

Main API Modules:
- nlm_core: Core shared functionality and base classes
- nlm_debug: Debugging and performance monitoring
- nlm_advanced_config: Advanced configuration API
- nlm_advanced_commands: Command-line interface and scripting

This module serves as the main package entry point and provides
convenient imports for the most commonly used functionality.
"""

__version__ = "0.1.0"
__author__ = "NLM Project"
__email__ = "contact@nlm-project.org"

# Core functionality
from .nlm_core import (
    # Main classes
    Brain,
    Config,
    AgentBrain,
    SimpleWorld,
    Vision,
    Audio,
    InternalSignals,
    Action,
    WorldObject,
    SensoryPercept,
    AgentBody,
    ActionResult,
    
    # Constants and enumerations
    NeuronId,
    SynapseId,
    RegionId,
    PopulationId,
    NeuronType,
    SynapseType,
    DevelopmentalStage,
    FiringState,
    ActionType,
    MotorCommand,
    WorldObjectType,
    INVALID_NEURON_ID,
    INVALID_SYNAPSE_ID,
    INVALID_REGION_ID,
    INVALID_POPULATION_ID,
    
    # Helper functions
    createConfig,
    createBrain,
    createSimpleWorld,
    createAgentBrain,
    createVision,
    createAudio,
    createInternalSignals,
    createAction,
    createWorldObject,
    createAgentBody,
    createActionResult,
    createSensoryPercept,
    
    # Configuration presets
    getDefaultConfig,
    getDevelopmentConfig,
    getSimulationConfig,
    getResearchConfig,
    getProductionConfig,
)

# Debugging and performance monitoring
from .nlm_debug import (
    # Debug logging
    debug_logger,
    info_logger,
    warning_logger,
    error_logger,
    
    # Memory monitoring
    start_memory_profiling,
    stop_memory_profiling,
    get_memory_usage,
    take_memory_snapshot,
    get_memory_report,
    
    # Performance profiling
    start_performance_profiling,
    stop_performance_profiling,
    profile_function,
    get_performance_stats,
    
    # Debugging utilities
    DebugLevel,
    MemoryProfiler,
    PerformanceProfiler,
    StackTraceAnalyzer,
    MemoryLeakDetector,
    
    # Monitoring
    Monitor,
    PerformanceMonitor,
    MemoryMonitor,
)

# Advanced configuration
from .nlm_advanced_config import (
    # Configuration management
    ConfigManager,
    ConfigValidator,
    ConfigExporter,
    ConfigImporter,
    
    # Advanced configuration
    AdvancedConfig,
    ParameterConfig,
    ConfigTemplate,
    ConfigInheritance,
    
    # Configuration presets
    createConfigPreset,
    getAvailablePresets,
    validateConfigSchema,
    
    # Configuration utilities
    getConfigDiff,
    mergeConfigs,
    exportConfigToJSON,
    importConfigFromJSON,
)

# Advanced commands and scripting
from .nlm_advanced_commands import (
    # Command execution
    Command,
    CommandExecutor,
    executeCommand,
    executeCommands,
    
    # Command types
    CommandType,
    SIMULATION_COMMAND,
    ANALYSIS_COMMAND,
    EXPORT_COMMAND,
    CONFIG_COMMAND,
    VALIDATION_COMMAND,
    MONITORING_COMMAND,
    DEBUG_COMMAND,
    SCRIPT_COMMAND,
    BATCH_COMMAND,
    
    # Scripting
    ScriptingEngine,
    Macro,
    Script,
    executeScript,
    
    # CLI tools
    CLIToolkit,
    CLICommand,
    runCLICommand,
    
    # Batch processing
    BatchProcessor,
    BatchJob,
    executeBatch,
)

# Utility functions
from .nlm_core import (
    # Utility functions
    formatNumber,
    formatTime,
    formatBytes,
    calculateStatistics,
    normalizeValue,
    clampValue,
    
    # Validation
    validateNeuronCount,
    validateSynapseDensity,
    validateConfigParams,
    
    # Error handling
    ConfigurationError,
    SimulationError,
    ValidationError,
    ResourceError,
    NetworkError,
    IntegrationError,
)

# Type aliases for convenience
import typing as t
if t.TYPE_CHECKING:
    from typing import Any, Callable, Dict, List, Optional, Union, Tuple
    
    # Common type aliases
    ConfigDict = Dict[str, Any]
    BrainParameters = Dict[str, Any]
    WorldParameters = Dict[str, Any]
    ActionParameters = Dict[str, Any]
    SensorData = Union[Vision, Audio, InternalSignals]
    
    # Callback types
    BrainCallback = Callable[[Brain], Any]
    StepCallback = Callable[[int, float], Any]
    ActionCallback = Callable[[Action], Any]
    RewardCallback = Callable[[float, float], Any]

# Package information
__all__ = [
    # Main classes
    'Brain',
    'Config',
    'AgentBrain',
    'SimpleWorld',
    'Vision',
    'Audio',
    'InternalSignals',
    'Action',
    'WorldObject',
    'SensoryPercept',
    'AgentBody',
    'ActionResult',
    
    # Constants
    'NeuronId', 'SynapseId', 'RegionId', 'PopulationId',
    'NeuronType', 'SynapseType', 'DevelopmentalStage', 'FiringState',
    'ActionType', 'MotorCommand', 'WorldObjectType',
    'INVALID_NEURON_ID', 'INVALID_SYNAPSE_ID', 'INVALID_REGION_ID', 'INVALID_POPULATION_ID',
    
    # Helper functions
    'createConfig', 'createBrain', 'createSimpleWorld', 'createAgentBrain',
    'createVision', 'createAudio', 'createInternalSignals', 'createAction',
    'createWorldObject', 'createAgentBody', 'createActionResult', 'createSensoryPercept',
    
    # Configuration presets
    'getDefaultConfig', 'getDevelopmentConfig', 'getSimulationConfig',
    'getResearchConfig', 'getProductionConfig',
    
    # Debugging
    'debug_logger', 'info_logger', 'warning_logger', 'error_logger',
    'start_memory_profiling', 'stop_memory_profiling', 'get_memory_usage',
    'take_memory_snapshot', 'get_memory_report',
    'start_performance_profiling', 'stop_performance_profiling', 'profile_function',
    'get_performance_stats',
    'DebugLevel', 'MemoryProfiler', 'PerformanceProfiler', 'StackTraceAnalyzer', 'MemoryLeakDetector',
    'Monitor', 'PerformanceMonitor', 'MemoryMonitor',
    
    # Advanced configuration
    'ConfigManager', 'ConfigValidator', 'ConfigExporter', 'ConfigImporter',
    'AdvancedConfig', 'ParameterConfig', 'ConfigTemplate', 'ConfigInheritance',
    'createConfigPreset', 'getAvailablePresets', 'validateConfigSchema',
    'getConfigDiff', 'mergeConfigs', 'exportConfigToJSON', 'importConfigFromJSON',
    
    # Advanced commands
    'Command', 'CommandExecutor', 'executeCommand', 'executeCommands',
    'CommandType', 'SIMULATION_COMMAND', 'ANALYSIS_COMMAND', 'EXPORT_COMMAND',
    'CONFIG_COMMAND', 'VALIDATION_COMMAND', 'MONITORING_COMMAND', 'DEBUG_COMMAND',
    'SCRIPT_COMMAND', 'BATCH_COMMAND',
    'ScriptingEngine', 'Macro', 'Script', 'executeScript',
    'CLIToolkit', 'CLICommand', 'runCLICommand',
    'BatchProcessor', 'BatchJob', 'executeBatch',
    
    # Utilities
    'formatNumber', 'formatTime', 'formatBytes', 'calculateStatistics',
    'normalizeValue', 'clampValue',
    'validateNeuronCount', 'validateSynapseDensity', 'validateConfigParams',
    'ConfigurationError', 'SimulationError', 'ValidationError', 'ResourceError', 'NetworkError', 'IntegrationError',
    
    # Package info
    '__version__',
    '__author__',
    '__email__',
]

# Convenience imports for common use cases
__init__ = __version__  # Alias for version info

# Set up basic logging configuration
import logging
logging.basicConfig(level=logging.INFO, format='%(asctime)s - %(name)s - %(levelname)s - %(message)s')

def get_logger(name: str = 'nlm') -> logging.Logger:
    """Get a configured logger for NLM operations.
    
    Args:
        name: Logger name (default: 'nlm')
        
    Returns:
        Configured logger instance
    """
    return logging.getLogger(name)

# Initialize default logger
default_logger = get_logger()

# Quick start example function
def quick_start_example() -> str:
    """Return a quick start example for NLM Python API.
    
    Returns:
        String containing quick start example code
    """
    return """
# Quick Start Example - NLM Python API

import nlm

# Create a brain with default configuration
brain = nlm.createBrain(nlm.createDefaultConfig())
brain.initialize()

# Run a simple simulation
for step in range(100):
    brain.step(step)

print(f"Simulation complete! Total spikes: {brain.getTotalSpikeCount()}")
print(f"Neurons firing: {brain.getFiringNeuronCount()}")
"""

# Installation info function
def get_installation_info() -> Dict[str, str]:
    """Get installation information for NLM Python API.
    
    Returns:
        Dictionary containing installation information
    """
    return {
        'version': __version__,
        'author': __author__,
        'email': __email__,
        'python_version': '>=3.8',
        'description': 'Neural Learning Machine Python API',
        'documentation': 'https://nlm.readthedocs.io',
        'repository': 'https://github.com/nlm-project/nlm',
    }

print(f"NLM Python API {__version__} loaded successfully!")
