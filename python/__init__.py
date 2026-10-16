"""Python module initialization for Enhanced NLM API.

This module provides both the original low-level bindings and the enhanced API
for NLM (Neural Learning Machine).

The enhanced API is available through the `enhanced` submodule and provides:
- Pythonic interfaces with context managers and iterators
- Advanced configuration and brain management tools
- Visualization and analysis utilities
- Comprehensive error handling and validation
- Extensive examples and documentation

Usage:
    import pynlm
    from pynlm.enhanced import create_enhanced_brain, training_context
    
    # Basic usage
    brain = create_enhanced_brain(name="MyBrain")
    brain.initialize()
    
    # Use training context
    with training_context(brain, steps=100) as session:
        session.run(100)
    
    # Analyze results
    analysis = brain.analyze()
    print(f"Stability: {analysis.get_stability():.2f}")
"""

# Import the enhanced API module
from .enhanced_api import (
    # Original API (backward compatibility)
    createBrain, createSimpleWorld, createAgentBrain, createDefaultConfig,
    Config, Brain, AgentBrain, SimpleWorld,
    NeuronType, SynapseType, DevelopmentalStage, FiringState,
    ActionType, MotorCommand, WorldObjectType, Vision, Audio,
    InternalSignals, SensoryInput, Action, WorldObject, AgentBody,
    ActionResult, SensoryPercept, NeuronId, SynapseId, RegionId, PopulationId,
    
    # Enhanced API
    EnhancedBrain, EnhancedConfig, EnhancedAgent, EnhancedWorld,
    BrainAnalysis, VisualizationTools, ErrorHandler,
    create_enhanced_brain, create_enhanced_config, create_enhanced_agent,
    create_enhanced_world, create_training_session, create_analysis_session,
    ConfigBuilder, BrainManager, VisualizationExporter,
)

# Create version attribute
__version__ = "0.1.0-enhanced"

# Module-level constants
__all__ = [
    # Original API
    'createBrain', 'createSimpleWorld', 'createAgentBrain', 'createDefaultConfig',
    'Config', 'Brain', 'AgentBrain', 'SimpleWorld',
    'NeuronType', 'SynapseType', 'DevelopmentalStage', 'FiringState',
    'ActionType', 'MotorCommand', 'WorldObjectType', 'Vision', 'Audio',
    'InternalSignals', 'SensoryInput', 'Action', 'WorldObject', 'AgentBody',
    'ActionResult', 'SensoryPercept', 'NeuronId', 'SynapseId', 'RegionId', 'PopulationId',
    
    # Enhanced API (exported for backward compatibility)
    'EnhancedBrain', 'EnhancedConfig', 'EnhancedAgent', 'EnhancedWorld',
    'BrainAnalysis', 'VisualizationTools', 'ErrorHandler',
    'create_enhanced_brain', 'create_enhanced_config', 'create_enhanced_agent',
    'create_enhanced_world', 'create_training_session', 'create_analysis_session',
    'ConfigBuilder', 'BrainManager', 'VisualizationExporter',
    
    # Submodules
    'enhanced',
]

# Create enhanced submodule
try:
    from . import enhanced
    enhanced.__all__ = [
        'EnhancedBrain', 'EnhancedConfig', 'EnhancedAgent', 'EnhancedWorld',
        'BrainAnalysis', 'VisualizationTools', 'ErrorHandler',
        'create_enhanced_brain', 'create_enhanced_config', 'create_enhanced_agent',
        'create_enhanced_world', 'create_training_session', 'create_analysis_session',
        'ConfigBuilder', 'BrainManager', 'VisualizationExporter',
    ]
except ImportError:
    pass

# Module docstring
__doc__ = """NLM (Neural Learning Machine) Python Bindings

This module provides Python bindings for the NLM neural simulation framework.

Key features:
- Original low-level API (backward compatible)
- Enhanced Pythonic API with context managers, iterators, and analysis
- Configuration builders and validation
- Visualization and analysis tools
- Comprehensive error handling

Quick start:
    import pynlm
    from pynlm.enhanced import create_enhanced_brain, training_context
    
    brain = create_enhanced_brain(name="MyBrain")
    brain.initialize()
    
    with training_context(brain, steps=100) as session:
        session.run(100)
    
    analysis = brain.analyze()
"""
