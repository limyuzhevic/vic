"""
NLM Enhanced Python Wrapper

This module provides a comprehensive, Pythonic API for the NLM (Neural Learning Machine)
neural simulation framework. It builds upon the low-level C++ bindings (pynlm) to provide
an enhanced user experience with better error handling, configuration validation,
simulation utilities, and development tools.

Key Features:
- Pythonic API with wrapper classes for all NLM components
- Configuration validation and schema system
- Comprehensive simulation utilities and helpers
- Event system and monitoring tools
- Error handling and logging improvements
- Easy-to-use example scripts and documentation
- Full compatibility with existing C++ bindings

Usage:
    import pynlm
    from python import nlm_enhanced_wrapper as nlm

    # Create brain with configuration
    brain = nlm.create_brain({"neuron_count": 1000})

    # Create world and agent
    world = nlm.create_world()
    agent = nlm.create_agent_brain(brain, world)

    # Run simulation
    results = nlm.run_simulation(brain, world, agent, 100)

Available Components:
    - ConfigValidator: Validates and normalizes configuration parameters
    - SensoryPerceptWrapper: Enhanced sensory perception interface
    - ActionWrapper: Improved action handling with enum support
    - WorldWrapper: Pythonic world interface with utilities
    - AgentWrapper: Agent brain interface with development features
    - BrainWrapper: Enhanced brain interface with monitoring
    - Factory functions: create_brain, create_world, create_agent_brain
    - Simulation utilities: run_simulation, save/load results
    - Event system: add/remove simulation callbacks
    - Error classes: NLMError, ConfigurationError, SimulationError
    - Enums: ActionType, DevelopmentalStage

Example Scripts:
    EXAMPLE_SCRIPTS dict contains runnable examples for:
    - silent_brain: Basic brain operation
    - brain_watching_world: Sensory processing in world
    - complete_agent: Full agent with learning enabled

See documentation directory for comprehensive guides and API references.
"""

__version__ = "1.0.0"
__author__ = "NLM Development Team"
__license__ = "MIT"

# Core classes and functions
from .nlm_enhanced_wrapper import (
    # Error classes
    NLMError,
    ConfigurationError,
    SimulationError,
    
    # Enums
    ActionType,
    DevelopmentalStage,
    
    # Validators and utilities
    ConfigValidator,
    
    # Wrapper classes
    SensoryPerceptWrapper,
    ActionWrapper,
    WorldWrapper,
    AgentWrapper,
    BrainWrapper,
    
    # Factory functions
    create_brain,
    create_world,
    create_agent_brain,
    
    # Simulation utilities
    add_simulation_callback,
    remove_simulation_callback,
    run_simulation,
    save_simulation_results,
    load_simulation_results,
    
    # Examples
    EXAMPLE_SCRIPTS
)

# Re-export pynlm for direct access
from pynlm import *