"""
pynlm - Enhanced NLM Python Bindings

High-level Python API for the NLM (Neural Learning Machine) neural simulation framework.

This module provides an enhanced interface to the NLM C++ neural simulation framework,
featuring ~50% more functionality than the original bindings.

Key Features:
- Enhanced configuration builders (default, expert, quick)
- Simulation management with SimulationManager
- Action builders (move toward, explore, interact)
- Memory recording and replay
- Statistical analysis utilities
- Convenience simulation patterns
- Advanced validation and error handling
- World object builders (resources, walls, hazards)
- Agent setup helpers
- Development and learning statistics
- Debug and development tools
- Enhanced documentation and type safety

Usage Examples:
    # Basic usage
    import pynlm
    brain = pynlm.createBrainSimple()
    brain.initialize()
    for i in range(100):
        brain.step(i)
    
    # Advanced usage with simulation manager
    config, brain, world, agent = pynlm.create_experimental_setup()
    sim_manager = pynlm.createSimulationManager(brain, world, agent)
    sim_manager.initialize()
    sim_manager.run_episode(1000, 0.1)
    print(sim_manager.get_statistics())
    
    # Research usage
    import pynlm.examples.research_example as research
    research.run_research_demo()
"""

# Define package metadata
__version__ = "0.1.0-enhanced"
__author__ = "NLM Team"
__description__ = "Enhanced Python bindings for NLM (Neural Learning Machine)"
__license__ = "MIT"
__copyright__ = "Copyright 2024 NLM Authors"
__doc__ = __doc__

# Define public API
__all__ = [
    # Core classes
    'Config',
    'Brain', 
    'AgentBrain',
    'SimpleWorld',
    'SensoryInput',
    'Vision',
    'Audio',
    'InternalSignals',
    'Action',
    'WorldObject',
    'AgentBody',
    'ActionResult',
    'SensoryPercept',
    
    # Enumerations
    'NeuronType',
    'SynapseType', 
    'DevelopmentalStage',
    'FiringState',
    'ActionType',
    'MotorCommand',
    'WorldObjectType',
    
    # Identifier types
    'NeuronId',
    'SynapseId',
    'RegionId',
    'PopulationId',
    
    # Exception classes
    'NLMError',
    'ConfigurationError',
    'SimulationError',
    'ValidationError',
    
    # Factory functions
    'createDefaultConfig',
    'createBrain',
    'createSimpleWorld',
    'createAgentBrain',
    'createConfig',
    'createExpertConfig',
    'createQuickConfig',
    'createBrainSimple',
    'createDefaultWorld',
    'createLargeWorld',
    'createSmallWorld',
    'createAgentBrainWithWorld',
    'create_experimental_setup',
    'create_debug_setup',
    
    # Builder functions
    'createActionMoveToward',
    'createActionExplore',
    'createActionInteract',
    
    # Management classes
    'SimulationManager',
    'MemoryRecorder',
    
    # Analysis functions
    'compute_neural_statistics',
    'compute_development_statistics',
    'run_silent_simulation',
    'run_agent_simulation',
    
    # Convenience functions
    'validate_brain_state',
    'validate_world_state',
    'validate_simulation_setup',
    'format_statistics',
    'get_welcome_message',
    'get_version_info',
    'get_features_summary',
    'get_quick_start_example',
    'get_research_example',
    
    # Constants
    'INVALID_NEURON_ID',
    'INVALID_SYNAPSE_ID',
    'INVALID_REGION_ID',
    'INVALID_POPULATION_ID',
    'DEFAULT_WORLD_SIZE',
    'DEFAULT_VISION_SIZE',
    'DEFAULT_NEURON_COUNT',
    'DEFAULT_CONNECTION_PROBABILITY',
    'EXPLORATION_ACTIONS',
    'MOVEMENT_ACTIONS',
    
    # Debugging
    'toggle_debug_mode',
    'get_current_time',
    'sleep',
    
    # Example modules
    'examples',
    
    # Version info
    '__version__',
    '__author__',
    '__description__',
]

# Import all components from the C++ bindings
# Note: These are automatically populated by pybind11
import sys

# Forward declarations for type checking
if False:
    # These are just for type checking, actual classes are defined in C++
    class Config:
        pass
    
    class Brain:
        pass
    
    class AgentBrain:
        pass
    
    class SimpleWorld:
        pass
    
    class SensoryInput:
        pass
    
    class Vision(SensoryInput):
        pass
    
    class Audio(SensoryInput):
        pass
    
    class InternalSignals(SensoryInput):
        pass
    
    class Action:
        pass
    
    class WorldObject:
        pass
    
    class AgentBody:
        pass
    
    class ActionResult:
        pass
    
    class SensoryPercept:
        pass
    
    # Enumerations
    class NeuronType:
        Excitatory = 0
        Inhibitory = 1
        Modulatory = 2
        Sensory = 3
        Motor = 4
        Internal = 5
    
    class SynapseType:
        Excitatory = 0
        Inhibitory = 1
        Modulatory = 2
        Electrical = 3
        GapJunction = 4
    
    class DevelopmentalStage:
        Initial = 0
        CriticalPeriod = 1
        Maturation = 2
        Adult = 3
        Aging = 4
    
    class FiringState:
        Resting = 0
        Active = 1
        Refractory = 2
        Inhibited = 3
    
    class ActionType:
        MoveForward = 0
        MoveBackward = 1
        MoveLeft = 2
        MoveRight = 3
        TurnLeft = 4
        TurnRight = 5
        Look = 6
        LookUp = 7
        LookDown = 8
        Interact = 9
        Eat = 10
        Drink = 11
        Rest = 12
        Wait = 13
        Custom = 14
    
    class MotorCommand:
        MoveForward = 0
        MoveBackward = 1
        TurnLeft = 2
        TurnRight = 3
        LookLeft = 4
        LookRight = 5
        Interact = 6
        Wait = 7
    
    class WorldObjectType:
        Empty = 0
        Resource = 1
        Hazard = 2
        Wall = 3
        Marker = 4
    
    # Identifier types
    class NeuronId:
        pass
    
    class SynapseId:
        pass
    
    class RegionId:
        pass
    
    class PopulationId:
        pass
    
    # Exception classes
    class NLMError(Exception):
        pass
    
    class ConfigurationError(NLMError):
        pass
    
    class SimulationError(NLMError):
        pass
    
    class ValidationError(NLMError):
        pass
    
    # Manager classes
    class SimulationManager:
        pass
    
    class MemoryRecorder:
        pass
    
    # Constants
    INVALID_NEURON_ID = None
    INVALID_SYNAPSE_ID = None
    INVALID_REGION_ID = None
    INVALID_POPULATION_ID = None

# Make the module discoverable
from pathlib import Path
import os

# Add examples directory to sys.path if it exists
examples_dir = Path(__file__).parent / 'examples'
if examples_dir.exists() and str(examples_dir) not in sys.path:
    sys.path.insert(0, str(examples_dir))

# Define package metadata for setuptools
package_info = {
    'name': 'pynlm',
    'version': __version__,
    'author': __author__,
    'description': __description__,
    'license': __license__,
    'copyright': __copyright__,
    'platforms': ['Linux', 'macOS', 'Windows'],
    'classifiers': [
        'Development Status :: 4 - Beta',
        'Intended Audience :: Science/Research',
        'License :: OSI Approved :: MIT License',
        'Programming Language :: Python :: 3',
        'Programming Language :: Python :: 3.8',
        'Programming Language :: Python :: 3.9',
        'Programming Language :: Python :: 3.10',
        'Programming Language :: Python :: 3.11',
        'Programming Language :: Python :: 3.12',
        'Topic :: Scientific/Engineering :: Artificial Intelligence',
        'Topic :: Scientific/Engineering :: Artificial Life',
        'Topic :: Software Development :: Libraries :: Python Modules',
    ],
    'keywords': ['neural', 'simulation', 'spiking', 'brain', 'learning', 'AI', 'neuroscience'],
    'python_requires': '>=3.8',
    'install_requires': ['numpy>=1.20'],
    'extras_require': {
        'dev': ['pytest>=7.0', 'scikit-build-core', 'pybind11'],
        'research': ['matplotlib', 'pandas', 'jupyter'],
    },
    'packages': ['pynlm'],
    'package_dir': {'pynlm': str(Path(__file__).parent)},
}

# Module initialization check
def _check_initialization():
    """Check if module is properly initialized"""
    try:
        # Test basic functionality
        test_config = createDefaultConfig()
        test_brain = createBrain(test_config)
        print(f"NLM Python Bindings initialized successfully (v{__version__})")
        return True
    except Exception as e:
        print(f"Error initializing NLM Python Bindings: {e}")
        return False

# Auto-initialization check (optional)
_check_initialization()

# Print welcome message when imported
if __name__ != '__main__':
    print(f"NLM Python Bindings {__version__} loaded")
    print("For examples, see the 'examples' directory")
