"""
Enhanced NLM Python Bindings

This module provides enhanced Python bindings for the NLM (Neural Learning Machine) 
C++ neural simulation framework. The bindings include Pythonic interfaces with:

- Context managers for resource management
- Fluent API and property-style access
- Configuration builders
- Simulation runners
- Visualization helpers
- Enhanced error handling

Module Structure:
---------------
pynlm: Main module containing all NLM bindings
pynlm.PythonBrain: Enhanced Brain interface with context managers
pynlm.PythonWorld: Enhanced SimpleWorld interface with context managers  
pynlm.PythonAgentBrain: Enhanced AgentBrain interface with context managers
pynlm.ConfigBuilder: Configuration builder for common setups
pynlm.SimulationRunner: Helper for running complete simulations
pynlm.VisualizationHelper: Visualization and data analysis helpers
pynlm.NLMException: Base exception class
pynlm.ConfigurationError: Configuration-specific exceptions
pynlm.SimulationError: Simulation-specific exceptions
pynlm.InitializationError: Initialization-specific exceptions

Usage Examples:
--------------
See the examples/ directory for comprehensive usage examples.

Quick Start:
------------
import pynlm

# Create a brain with enhanced interface
with pynlm.PythonBrain(pynlm.createBrain(pynlm.createDefaultConfig())) as brain:
    brain.initialize()
    brain.step(0)
    print(f"Neurons: {brain.getNeuronCount()}")

# Or use the basic factory functions (backward compatible)
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

Version Information:
-------------------
pynlm.__version__ = "0.1.0"
pynlm.__api_version__ = "enhanced"

Key Features:
-------------
✓ Context managers for automatic resource management
✓ Fluent API for common operations
✓ Property-style access to brain state
✓ Configuration builders for common scenarios
✓ Simulation runner for complete experiment setup
✓ Visualization helpers for data analysis
✓ Enhanced error handling with custom exceptions
✓ Type checking and validation utilities
✓ Unicode string support and encoding helpers
✓ Backward compatibility with original API
"""
