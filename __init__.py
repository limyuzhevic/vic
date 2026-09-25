"""NLM Python Package - Enhanced Python Interface for Neural Learning Machine

This package provides a comprehensive Python interface for the NLM (Neural Learning Machine)
neural simulation framework. It offers both direct access to the C++ bindings and additional
Python utilities for enhanced usability, documentation, and convenience functions.

The package is designed to be:
- **Pythonic**: Follows Python best practices and conventions
- **Well-documented**: Includes comprehensive docstrings and examples
- **Type-aware**: Provides type hints for better IDE support
- **Robust**: Includes extensive error handling and validation
- **Flexible**: Offers multiple ways to accomplish common tasks
- **Educational**: Includes examples and tutorials for learning

Installation:
    pip install pynlm

Basic Usage:
    import pynlm
    
    # Create a simple brain
    brain = pynlm.create_brain_with_default_config()
    brain.initialize()
    
    # Run simulation
    for i in range(100):
        brain.step(i)
    
    # Get statistics
    stats = pynlm.run_brain_simulation(brain, 100)
    print(f"Firing rate: {stats['average_firing_rate']:.2f} Hz")

Advanced Usage:
    # Create complete agent setup
    brain, world, agent = pynlm.create_complete_agent(width=30, height=30)
    
    # Enable learning subsystems
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    agent.enableDevelopment(True)
    
    # Run episode
    episode_stats = pynlm.run_agent_episode(agent, world, 1000)
    
    # Validate setup
    validation = pynlm.validate_agent_setup(brain, world, agent)
    if not validation['is_valid']:
        print("Issues found:", validation['issues'])
    
    # Print summary
    summary = pynlm.print_simulation_summary(brain, world, agent)
    print(summary)

Module Structure:
    pynlm.py           - Main Python module with utilities and enhanced API
    pynlm_core.py      - Core C++ bindings (if available)
    examples/          - Example scripts and demonstrations
    docs/              - Documentation and tutorials
    tests/             - Test suite

Author: NLM Project Authors
License: MIT
Version: 0.1.0
"""

import sys
from typing import Optional

# Check Python version
if sys.version_info < (3, 8):
    raise ImportError("NLM Python package requires Python 3.8 or later")

# Import core functionality from pynlm.py
try:
    from python.nlm import (
        # Version and metadata
        __version__,
        __author__,
        __license__,
        
        # Constants
        DEFAULT_NEURON_COUNT,
        DEFAULT_SYNAPSE_DENSITY,
        DEFAULT_V_THRESH,
        DEFAULT_V_REST,
        DEFAULT_MAX_ENERGY,
        
        # Type aliases
        NeuronId,
        SynapseId,
        RegionId,
        PopulationId,
        
        # Exception classes
        NLMError,
        ConfigurationError,
        SimulationError,
        ValidationError,
        
        # Core functionality
        create_identity_config,
        validate_config,
        create_brain_from_config,
        create_brain_with_default_config,
        run_brain_simulation,
        compare_brain_states,
        create_simulation_environment,
        create_agent_brain,
        create_complete_agent,
        run_agent_episode,
        run_batch_experiments,
        
        # Utilities
        print_simulation_summary,
        validate_agent_setup
    )
    
    # Import core module if available
    try:
        import pynlm_core as core
        
        # Core C++ bindings
        createDefaultConfig = core.createDefaultConfig
        createBrain = core.createBrain
        createSimpleWorld = core.createSimpleWorld
        createAgentBrain = core.createAgentBrain
        
    except ImportError:
        # Define stubs when core module is not available
        def createDefaultConfig():
            raise RuntimeError("pynlm_core not available")
        
        def createBrain(config):
            raise RuntimeError("pynlm_core not available")
        
        def createSimpleWorld():
            raise RuntimeError("pynlm_core not available")
        
        def createAgentBrain(brain):
            raise RuntimeError("pynlm_core not available")
        
    # Export core functions
    __all__ = [
        # Version and metadata
        '__version__',
        '__author__',
        '__license__',
        
        # Constants
        'DEFAULT_NEURON_COUNT',
        'DEFAULT_SYNAPSE_DENSITY',
        'DEFAULT_V_THRESH',
        'DEFAULT_V_REST',
        'DEFAULT_MAX_ENERGY',
        
        # Type aliases
        'NeuronId',
        'SynapseId',
        'RegionId',
        'PopulationId',
        
        # Exception classes
        'NLMError',
        'ConfigurationError',
        'SimulationError',
        'ValidationError',
        
        # Core functionality
        'create_identity_config',
        'validate_config',
        'create_brain_from_config',
        'create_brain_with_default_config',
        'run_brain_simulation',
        'compare_brain_states',
        'create_simulation_environment',
        'create_agent_brain',
        'create_complete_agent',
        'run_agent_episode',
        'run_batch_experiments',
        
        # Utilities
        'print_simulation_summary',
        'validate_agent_setup',
        
        # Core C++ bindings
        'createDefaultConfig',
        'createBrain',
        'createSimpleWorld',
        'createAgentBrain'
    ]
    
except ImportError as e:
    raise ImportError(
        f"Failed to import NLM Python package: {e}. "
        "Please ensure NLM is properly installed with Python bindings."
    ) from e

# Package metadata
__package__ = "pynlm"
__description__ = "Python bindings for NLM (Neural Learning Machine) neural simulation framework"

# Module initialization
def _check_installation():
    """Check if NLM is properly installed and provide helpful error messages."""
    try:
        # Try to create a simple brain to verify installation
        import pynlm_core
        print("NLM core module loaded successfully.")
        return True
    except ImportError:
        print("Warning: NLM core module not found.")
        print("Please install NLM with Python bindings using:")
        print("  pip install pynlm")
        print("Or build from source:")
        print("  git clone https://github.com/nlm-project/nlm.git")
        print("  cd nlm")
        print("  pip install -e .")
        return False

# Auto-check installation when module is imported
if __name__ != "__main__":
    _check_installation()

# Provide helpful usage information
if __name__ == "__main__":
    print("NLM Python Package v" + __version__)
    print("=" * 50)
    print("Usage examples:")
    print()
    print("1. Basic brain simulation:")
    print("   import pynlm")
    print("   brain = pynlm.create_brain_with_default_config()")
    print("   brain.initialize()")
    print("   for i in range(100): brain.step(i)")
    print()
    print("2. Complete agent setup:")
    print("   brain, world, agent = pynlm.create_complete_agent(width=30, height=30)")
    print("   agent.enableRewardModulation(True)")
    print("   agent.enableCuriosity(True)")
    print("   episode_stats = pynlm.run_agent_episode(agent, world, 1000)")
    print()
    print("3. Run experiments:")
    print("   experiments = pynlm.run_batch_experiments(5, 500)")
    print()
    print("For more detailed documentation, visit:")
    print("  https://nlm.readthedocs.io")
