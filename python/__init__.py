"""
Enhanced Python API for NLM (Neural Learning Machine)

This module provides an enhanced, Pythonic interface to the NLM C++ neural simulation framework.
It builds upon the basic bindings with additional features for better usability, visualization,
and advanced simulation capabilities.

Key Features:
- Enhanced error handling with detailed feedback
- Real-time visualization and plotting capabilities
- Advanced simulation control (pause/resume/step)
- Model export/import with multiple formats
- Memory management utilities
- Advanced querying and analysis tools
- Context managers for safe resource handling
- Comprehensive documentation and examples

For the complete documentation and examples, see the enhanced_api.md file.
"""

from .enhanced_api import (
    # Core enhanced classes
    BrainStats,
    SimulationControls,
    ModelManager,
    VisualizationInterface,
    MemoryManager,
    QueryBuilder,
    SimulationContext,
    BrainContext,
    
    # Custom exceptions
    NLMError,
    ConfigurationError,
    SimulationError,
    VisualizationError,
    MemoryError,
    ValidationError,
    
    # Factory functions
    create_enhanced_brain,
    create_enhanced_world,
    create_enhanced_agent,
    
    # Simulation utilities
    run_basic_simulation,
    run_agent_simulation,
    run_enhanced_simulation,
    
    # Export/import utilities
    export_model_to_json,
    import_model_from_json,
    
    # Utility functions
    validate_inputs,
    get_version_info,
    create_error_handler,
    
    # Backward compatibility aliases (from original bindings)
    createBrain,
    createSimpleWorld,
    createAgentBrain,
    createDefaultConfig,
    INVALID_NEURON_ID,
    INVALID_SYNAPSE_ID,
    INVALID_REGION_ID,
    INVALID_POPULATION_ID,
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
)

# Version information
__version__ = "2.0.0"
__author__ = "NLM Project"
__email__ = "contact@nlm-project.org"
__license__ = "MIT"

# Enhanced API capabilities
API_FEATURES = {
    "error_handling": True,
    "visualization": True,
    "simulation_control": True,
    "model_management": True,
    "memory_management": True,
    "advanced_querying": True,
    "context_management": True,
    "backward_compatibility": True
}

# Initialize the enhanced module
def _enhanced_init():
    """Initialize the enhanced API module."""
    import logging
    logging.basicConfig(level=logging.INFO)
    
    # Log module load with version info
    logging.info(f"Enhanced NLM Python API v{__version__} loaded successfully")
    logging.info(f"API Features: {list(API_FEATURES.keys())}")

# Call initialization
_enhanced_init()
