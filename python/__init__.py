"""
NLM (Neural Learning Machine) Python Bindings
=============================================

This is the main package for NLM, providing Python bindings to the C++ neural
simulation framework. The package includes:

- Core neural simulation classes (Brain, Config, etc.)
- Sensory and motor systems
- Simulation world management
- Progress tracking utilities
- Experiment management
- Data analysis tools
- Visualization utilities

The NLM framework implements real spiking neural computation with integrated
memory, prediction, cognition, and neuromodulation systems.

Key Features:
-------------
- Event-driven neural simulation with precise timing
- Multi-region brain architecture
- Integrated neuromodulation (dopamine, curiosity, novelty)
- Structural and functional plasticity
- Developmental systems
- Memory systems (working, episodic, associative)
- Prediction and planning systems
- Attention and concept formation
- Full sensory-motor loop

Installation:
-------------
The package requires scikit-build-core and pybind11 for building.

Usage:
------
Import the core classes to start building neural simulations:

.. code-block:: python

    import pynlm
    import pynlm.utils as nlm_utils
    
    # Create configuration
    config = pynlm.Config()
    config.set("learning_rate", 0.01)
    config.set("num_neurons", 1000)
    
    # Create brain and world
    brain = pynlm.Brain(config)
    world = pynlm.SimpleWorld()
    
    # Initialize
    brain.initialize()
    
    # Run simulation
    for step in range(100):
        brain.step(step)
        world.update(0.1)
    
    # Visualize results
    nlm_utils.print_brain_summary(brain)

Advanced Usage:
---------------
Use the utils module for experiment management and advanced functionality:

.. code-block:: python

    # Create an experiment
    experiment = nlm_utils.Experiment(config, "my_experiment")
    
    # Define progress callback
    def progress_callback(progress, message):
        print(f"Progress: {progress:.1%}: {message}")
    
    # Run experiment with progress tracking
    experiment.run(world, progress_callback, steps=1000)
    
    # Save results
    experiment.save("experiment_results.nlm")

API Documentation:
------------------
See the API documentation for detailed class and function references.

License:
--------
MIT License. See LICENSE file for details.

Author:
-------
NLM Authors
"""

# Version information
__version__ = "0.2.0"
__version_name__ = "NLM Python Bindings v0.2.0"
__author__ = "NLM Authors"
__license__ = "MIT"

# Import pynlm module
import sys
import os

# Core imports
from .pynlm import (
    # Configuration
    Config,
    
    # Types
    NeuronId, SynapseId, RegionId, PopulationId,
    NeuronType, SynapseType, DevelopmentalStage, FiringState,
    ActionType, MotorCommand, WorldObjectType,
    
    # Sensory Systems
    SensoryInput, Vision, Audio, InternalSignals, SensoryPercept,
    
    # Motor Systems
    Action, ActionResult,
    
    # Core Components
    Brain, SimpleWorld, AgentBrain, AgentBody,
    
    # Neuromodulation
    Neuromodulator, Dopamine, Acetylcholine, Norepinephrine, Serotonin,
    
    # Exceptions
    NLMException, NLMConfigError, NLMBrainError, NLMWorldError,
    NLMSimulationError, NLMMemoryError,
    
    # Progress Tracking
    ProgressTracker,
    
    # Factory Functions
    createDefaultConfig, createBrain, createSimpleWorld, createAgentBrain,
    createProgressTracker, runSimulationWithProgress, saveBrainState, loadBrainState,
    
    # Constants (backward compatibility)
    INVALID_NEURON_ID, INVALID_SYNAPSE_ID, INVALID_REGION_ID, INVALID_POPULATION_ID
)

# Utility imports
from .utils import (
    Experiment,
    DataHandler,
    Visualization,
    create_default_config,
    run_quick_test
)

# Re-export commonly used items
__all__ = [
    # Core Classes
    'Config', 'Brain', 'SimpleWorld', 'AgentBrain', 'AgentBody',
    'Vision', 'Audio', 'InternalSignals', 'SensoryPercept',
    'Action', 'ActionResult', 'WorldObject',
    
    # Types
    'NeuronId', 'SynapseId', 'RegionId', 'PopulationId',
    'NeuronType', 'SynapseType', 'DevelopmentalStage', 'FiringState',
    'ActionType', 'MotorCommand', 'WorldObjectType',
    
    # Neuromodulation
    'Neuromodulator', 'Dopamine', 'Acetylcholine', 'Norepinephrine', 'Serotonin',
    
    # Exceptions
    'NLMException', 'NLMConfigError', 'NLMBrainError', 'NLMWorldError',
    'NLMSimulationError', 'NLMMemoryError',
    
    # Progress Tracking
    'ProgressTracker',
    
    # Factory Functions
    'createDefaultConfig', 'createBrain', 'createSimpleWorld', 'createAgentBrain',
    'createProgressTracker', 'runSimulationWithProgress', 'saveBrainState', 'loadBrainState',
    
    # Constants
    'INVALID_NEURON_ID', 'INVALID_SYNAPSE_ID', 'INVALID_REGION_ID', 'INVALID_POPULATION_ID',
    
    # Utilities
    'Experiment', 'DataHandler', 'Visualization',
    'create_default_config', 'run_quick_test',
    
    # Version info
    '__version__', '__version_name__', '__author__', '__license__'
]

# Set up module globals
if 'pybind11' in sys.modules:
    # Set up exception translations
    import pybind11
    
    # Make sure exceptions are properly translated
    def _translate_exception(exc_type, exc_value, exc_traceback):
        # Custom exception translation logic
        pass
    
    # Register exception translator
    # pybind11::register_exception_translator(_translate_exception)

# Convenience shortcuts
brain = {
    'create_default_config': createDefaultConfig,
    'create_brain': createBrain,
    'create_simple_world': createSimpleWorld,
    'create_agent_brain': createAgentBrain
}

utils = {
    'Experiment': Experiment,
    'DataHandler': DataHandler,
    'Visualization': Visualization,
    'create_default_config': create_default_config,
    'run_quick_test': run_quick_test
}

# Print version info on import if requested
if 'PYNLM_VERBOSE' in os.environ:
    print(f"NLM Python Bindings {__version__}")
    print(f"Author: {__author__}")
    print(f"License: {__license__}")
    print(f"Built with pybind11 version: {pybind11.__version__}")
    print("")
    
    # List available modules
    print("Available modules:")
    print("  - Core: Config, Brain, SimpleWorld, AgentBrain, etc.")
    print("  - Sensory: Vision, Audio, InternalSignals, SensoryPercept")
    print("  - Motor: Action, ActionResult")
    print("  - Neuromodulation: Neuromodulator, Dopamine, etc.")
    print("  - Utilities: Experiment, DataHandler, Visualization")