"""Python module initialization for Enhanced NLM API.

This module provides both the original low-level bindings and the enhanced API
for NLM (Neural Learning Machine). The enhanced API is available through the
`enhanced` submodule and provides:

- Pythonic interfaces with context managers, iterators, and properties
- Advanced configuration and brain management tools
- Visualization and analysis utilities
- Comprehensive error handling and validation
- Extensive examples and documentation

The enhanced API is designed to be more intuitive and easier to use for
researchers and developers while maintaining full backward compatibility.

Usage examples:

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

# Import the original bindings from the C++ module
from . import pynlm

# Import all original classes and functions
from .pynlm import (
    createBrain, createSimpleWorld, createAgentBrain, createDefaultConfig,
    Config, Brain, AgentBrain, SimpleWorld,
    NeuronType, SynapseType, DevelopmentalStage, FiringState,
    ActionType, MotorCommand, WorldObjectType, Vision, Audio,
    InternalSignals, SensoryInput, Action, WorldObject, AgentBody,
    ActionResult, SensoryPercept, NeuronId, SynapseId, RegionId, PopulationId,
)

# Import enhanced API submodule
try:
    from .enhanced import (
        EnhancedBrain,
        EnhancedConfig,
        EnhancedAgent,
        EnhancedWorld,
        BrainAnalysis,
        VisualizationTools,
        ErrorHandler,
        create_enhanced_brain,
        create_enhanced_config,
        create_enhanced_agent,
        create_enhanced_world,
        create_training_session,
        create_analysis_session,
        ConfigBuilder,
        BrainManager,
        VisualizationExporter,
        ValidationError,
        NeuralNetworkError,
        tensor,
        neuron_id,
        synapse_id,
        region_id,
        population_id,
        validate_config,
    )
    
    # Create enhanced submodule reference
    enhanced = __import__('pynlm.enhanced', fromlist=[''])
    
except ImportError as e:
    # If enhanced API is not available, create placeholders
    print(f"Warning: Enhanced API not available: {e}")
    
    # Define placeholder classes
    class EnhancedBrain:
        pass
    
    class EnhancedConfig:
        pass
    
    class EnhancedAgent:
        pass
    
    class EnhancedWorld:
        pass
    
    class BrainAnalysis:
        pass
    
    class VisualizationTools:
        pass
    
    class ErrorHandler:
        pass
    
    # Define placeholder factory functions
    def create_enhanced_brain(config=None, name=None):
        raise ImportError("Enhanced API not available")
    
    def create_enhanced_config(config_file=None):
        raise ImportError("Enhanced API not available")
    
    def create_enhanced_agent(brain, name=None):
        raise ImportError("Enhanced API not available")
    
    def create_enhanced_world():
        raise ImportError("Enhanced API not available")
    
    def create_training_session(brain, steps, enable_learning):
        raise ImportError("Enhanced API not available")
    
    def create_analysis_session(brain):
        raise ImportError("Enhanced API not available")
    
    def ConfigBuilder():
        raise ImportError("Enhanced API not available")
    
    def BrainManager(config=None):
        raise ImportError("Enhanced API not available")
    
    def VisualizationExporter():
        raise ImportError("Enhanced API not available")
    
    # Define placeholder error classes
    class ValidationError(Exception):
        pass
    
    class NeuralNetworkError(Exception):
        pass
    
    # Define placeholder type aliases
    tensor = list
    neuron_id = object
    synapse_id = object
    region_id = object
    population_id = object
    
    # Define placeholder validation function
    def validate_config(config_dict):
        return config_dict

# Version information
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
    
    # Enhanced API
    'EnhancedBrain', 'EnhancedConfig', 'EnhancedAgent', 'EnhancedWorld',
    'BrainAnalysis', 'VisualizationTools', 'ErrorHandler',
    'create_enhanced_brain', 'create_enhanced_config', 'create_enhanced_agent',
    'create_enhanced_world', 'create_training_session', 'create_analysis_session',
    'ConfigBuilder', 'BrainManager', 'VisualizationExporter',
    'ValidationError', 'NeuralNetworkError',
    'tensor', 'neuron_id', 'synapse_id', 'region_id', 'population_id',
    'validate_config',
    
    # Submodules
    'enhanced',
]

# Module docstring with examples
__doc__ = f"""NLM (Neural Learning Machine) Python Bindings

Version: {__version__}

This module provides Python bindings for the NLM neural simulation framework.

Key features:
- Original low-level API (backward compatible)
- Enhanced Pythonic API with context managers, iterators, and analysis
- Configuration builders and validation
- Visualization and analysis tools
- Comprehensive error handling with helpful messages

Quick start:
    import pynlm
    from pynlm.enhanced import create_enhanced_brain, training_context
    
    brain = create_enhanced_brain(name="MyBrain")
    brain.initialize()
    
    with training_context(brain, steps=100) as session:
        session.run(100)
    
    analysis = brain.analyze()
    print(f"Stability: {{analysis.get_stability():.2f}}")

The enhanced API is available through the `enhanced` submodule and provides
additional functionality including:

- Pythonic interfaces with context managers and iterators
- Advanced configuration and brain management tools
- Visualization and analysis utilities
- Comprehensive error handling and validation
- Extensive examples and documentation

See the enhanced module for complete documentation and examples.
"""

# Help and documentation functions

def get_help():
    """Get help information about the NLM Python bindings."""
    return """
NLM Python Bindings - Help

Available modules:
- pynlm - Original low-level bindings
- pynlm.enhanced - Enhanced Pythonic API

For original API usage:
    import pynlm
    brain = pynlm.createBrain(pynlm.createDefaultConfig())

For enhanced API usage:
    import pynlm
    from pynlm.enhanced import create_enhanced_brain, training_context
    
    brain = create_enhanced_brain(name="MyBrain")
    brain.initialize()
    
    with training_context(brain, steps=100) as session:
        session.run(100)
    
    analysis = brain.analyze()

For more information, see the enhanced module documentation.
"""

# Installation information
def print_installation_info():
    """Print installation information."""
    print("Enhanced NLM API Installation")
    print("=" * 40)
    print("To use the enhanced API:")
    print("1. Ensure pynlm is installed with dev dependencies")
    print("2. Import from pynlm.enhanced")
    print("3. Use the enhanced factory functions")
    print("\nFor visualization:")
    print("pip install matplotlib numpy pandas")

# Check if running as a script
if __name__ == "__main__":
    print_installation_info()
    print(get_help())
