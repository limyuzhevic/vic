#!/usr/bin/env python3
"""
NLM (Neural Learning Machine) - Python Bindings

This is the main entry point for the NLM Python API. It provides access to
the C++ Neural Learning Machine simulation framework through a Python interface.

The NLM framework implements a brain-inspired spiking neural network simulator
with event-driven dynamics, multiple plasticity mechanisms, and integrated
memory, prediction, and neuromodulation systems.

Key Features:
- Leaky Integrate-and-Fire (LIF) neuron dynamics
- Event-driven spike propagation with synaptic delays
- STDP and Hebbian plasticity rules
- Structural plasticity (synaptogenesis/pruning)
- Working memory, episodic memory, and associative memory
- Neuromodulation (dopamine, curiosity, novelty detection)
- Prediction systems and cognitive mechanisms
- Development and maturation systems
- Checkpoint save/load capabilities
- Replay and consolidation systems

Basic Usage:
    import pynlm

    # Create a brain with default configuration
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()

    # Run a simple simulation
    for step in range(100):
        brain.step(step)
        if step % 10 == 0:
            print(f"Step {step}: {brain.getFiringNeuronCount()} firing neurons")

    # Create a world and agent for more complex simulations
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()

    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)

    # Run a complete agent simulation
    for step in range(500):
        world.update(0.1)
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())

Advanced Usage:
    The package also includes advanced functionality for:

    - Performance monitoring and profiling
    - Batch processing of multiple simulations
    - Advanced analytics and visualization
    - Configuration management
    - Memory system analysis
    - Learning dynamics analysis
    - World scenario creation
    - Reinforcement learning integration

    For advanced usage, see:
    - python/advanced_api_examples.py: Comprehensive examples
    - python/performance_monitor.py: Performance monitoring tools
    - python/ADVANCED_API_GUIDE.md: Detailed documentation

    Prerequisites:
    - Python 3.8 or higher
    - C++ compiler with C++20 support
    - CMake 3.16 or higher
    - pybind11 and scikit-build-core

    Installation:
    pip install .

    The package requires the NLM C++ library to be built first. You can build
    it using:
        mkdir build
        cd build
        cmake ..
        make -j4

    For more information, visit the NLM project documentation.
"""

import sys
import warnings
from typing import Dict, List, Optional, Any

# Import core NLM classes
try:
    # Import the main pybind11 module
    import pybind11
    import numpy as np
    
    # Import main NLM classes
    from .core import Config, Brain, AgentBrain, SimpleWorld
    from .sensory import SensoryInput, Vision, Audio, InternalSignals
    from .motor import Action
    from .agent import AgentBody, SensoryPercept, ActionResult
    from .world import WorldObject
    from .enums import (
        NeuronType, SynapseType, DevelopmentalStage, FiringState,
        ActionType, MotorCommand, WorldObjectType
    )
    
    # Import factory functions
    from .factory import (
        createDefaultConfig,
        createBrain,
        createSimpleWorld,
        createAgentBrain
    )
    
    # Import utility functions
    from .utils import (
        setup_logging,
        validate_config,
        benchmark_simulation,
        analyze_brain_performance
    )
    
    # Import advanced modules (if available)
    try:
        from .advanced import (
            PerformanceMonitor,
            BatchProcessor,
            Analytics,
            Benchmarking
        )
    except ImportError:
        # Advanced modules are optional
        PerformanceMonitor = None
        BatchProcessor = None
        Analytics = None
        Benchmarking = None
    
    # Import example module (if available)
    try:
        from .examples import advanced_api_examples
    except ImportError:
        advanced_api_examples = None
    
    # Define version and package info
    __version__ = "0.1.0"
    __author__ = "NLM Project Authors"
    __email__ = "nlm-project@example.com"
    __license__ = "MIT"
    __url__ = "https://github.com/nlm-project/nlm"
    
    # Define package metadata
    __all__ = [
        # Core classes
        'Config', 'Brain', 'AgentBrain', 'SimpleWorld',
        'SensoryInput', 'Vision', 'Audio', 'InternalSignals',
        'Action', 'AgentBody', 'SensoryPercept', 'ActionResult',
        'WorldObject',
        
        # Enums
        'NeuronType', 'SynapseType', 'DevelopmentalStage', 'FiringState',
        'ActionType', 'MotorCommand', 'WorldObjectType',
        
        # Factory functions
        'createDefaultConfig', 'createBrain', 'createSimpleWorld', 'createAgentBrain',
        
        # Utility functions
        'setup_logging', 'validate_config', 'benchmark_simulation', 'analyze_brain_performance',
        
        # Advanced modules (conditional)
        'PerformanceMonitor', 'BatchProcessor', 'Analytics', 'Benchmarking',
        
        # Examples (conditional)
        'advanced_api_examples',
        
        # Version and metadata
        '__version__', '__author__', '__email__', '__license__', '__url__'
    ]
    
    # Custom warning class for NLM-specific warnings
    class NLMWarning(UserWarning):
        """Base class for NLM-specific warnings."""
        pass
    
    class NLMError(Exception):
        """Base class for NLM-specific errors."""
        pass
    
    # Set up custom warning filters
    warnings.filterwarnings('always', category=NLMWarning)
    
    # Module-level helper functions
    def create_experiment(config_dict=None, brain_config=None):
        """
        Create an experiment setup with optional configuration.
        
        Args:
            config_dict: Dictionary of configuration parameters
            brain_config: Pre-configured Brain object
            
        Returns:
            Tuple of (brain, world, agent) or (brain, config) if brain_config provided
        """
        if brain_config is not None:
            # Use provided brain
            world = createSimpleWorld()
            agent = createAgentBrain(brain_config)
            agent.initialize(world)
            return brain_config, world, agent
        else:
            # Create brain from config
            config = createDefaultConfig()
            if config_dict:
                for key, value in config_dict.items():
                    config.set(key, value)
            
            brain = createBrain(config)
            brain.initialize()
            
            world = createSimpleWorld()
            agent = createAgentBrain(brain)
            agent.initialize(world)
            
            return brain, world, agent
    
    def run_simulation(brain, steps=100, timestep=0.001):
        """
        Run a simple brain simulation.
        
        Args:
            brain: Brain object to simulate
            steps: Number of simulation steps
            timestep: Time step size
            
        Returns:
            Dictionary with simulation statistics
        """
        stats = {
            'total_steps': steps,
            'final_firing_rate': 0.0,
            'total_spikes': 0,
            'active_neurons': 0,
            'average_firing_rate': 0.0
        }
        
        for step in range(steps):
            brain.step(step, step * timestep)
            
            if step == steps - 1:
                stats['final_firing_rate'] = brain.getAverageFiringRate()
                stats['total_spikes'] = brain.getTotalSpikeCount()
                stats['active_neurons'] = brain.getActiveNeuronCount()
                stats['average_firing_rate'] = brain.getAverageFiringRate()
        
        return stats
    
    def validate_setup(brain=None, world=None, agent=None):
        """
        Validate that the NLM setup is properly configured.
        
        Args:
            brain: Brain object to validate
            world: World object to validate
            agent: Agent object to validate
            
        Returns:
            Dictionary with validation results
        """
        results = {
            'brain_valid': False,
            'world_valid': False,
            'agent_valid': False,
            'errors': [],
            'warnings': []
        }
        
        # Validate brain
        if brain is not None:
            try:
                if brain.getTotalNeuronCount() > 0:
                    results['brain_valid'] = True
                else:
                    results['errors'].append("Brain has no neurons")
            except Exception as e:
                results['errors'].append(f"Brain validation error: {e}")
        
        # Validate world
        if world is not None:
            try:
                if world.getWidth() > 0 and world.getHeight() > 0:
                    results['world_valid'] = True
                else:
                    results['warnings'].append("World dimensions may be incorrect")
            except Exception as e:
                results['errors'].append(f"World validation error: {e}")
        
        # Validate agent
        if agent is not None:
            try:
                # Check if agent has required methods
                required_methods = ['processSensoryInput', 'decodeMotorCommand', 'initialize']
                missing_methods = []
                for method in required_methods:
                    if not hasattr(agent, method):
                        missing_methods.append(method)
                
                if not missing_methods:
                    results['agent_valid'] = True
                else:
                    results['errors'].append(f"Agent missing methods: {missing_methods}")
            except Exception as e:
                results['errors'].append(f"Agent validation error: {e}")
        
        return results
    
    # Export module information
    def get_module_info():
        """
        Get information about the NLM Python module.
        
        Returns:
            Dictionary with module information
        """
        return {
            'version': __version__,
            'author': __author__,
            'email': __email__,
            'license': __license__,
            'url': __url__,
            'description': "Python bindings for NLM (Neural Learning Machine) C++ framework",
            'available_classes': [cls for cls in __all__ if not cls.startswith('_')],
            'advanced_features': {
                'performance_monitoring': PerformanceMonitor is not None,
                'batch_processing': BatchProcessor is not None,
                'analytics': Analytics is not None,
                'benchmarking': Benchmarking is not None,
                'examples': advanced_api_examples is not None
            }
        }
    
    # Create module-level singleton for common operations
    _default_config = None
    
    def get_default_config():
        """Get the default configuration singleton."""
        global _default_config
        if _default_config is None:
            _default_config = createDefaultConfig()
        return _default_config
    
    def reset_default_config():
        """Reset the default configuration singleton."""
        global _default_config
        _default_config = None
    
    # Module initialization message
    def _log_initialization():
        """Log module initialization (only in debug mode)."""
        import os
        if os.environ.get('NLM_DEBUG', '').lower() == 'true':
            print(f"NLM Python module v{__version__} initialized successfully")
    
    # Initialize module
    _log_initialization()
    
except ImportError as e:
    # Handle import errors gracefully
    import os
    
    # Create placeholder classes for import errors
    class Config:
        def __init__(self):
            raise ImportError(f"NLM C++ extension not available: {e}. "
                           "Please install the NLM C++ library first.")
        
        def has(self, key):
            return False
        
        def get(self, key):
            return None
        
        def set(self, key, value, source=None):
            pass
        
        def loadFromFile(self, filepath):
            return False
        
        def saveToFile(self, filepath):
            return False
        
        def summary(self):
            return "Config not available"
    
    class Brain:
        def __init__(self, config):
            raise ImportError(f"NLM C++ extension not available: {e}. "
                           "Please install the NLM C++ library first.")
        
        def initialize(self):
            return False
        
        def step(self, step, time=None):
            pass
        
        def getTotalNeuronCount(self):
            return 0
        
        def getAverageFiringRate(self):
            return 0.0
        
        def getTotalSpikeCount(self):
            return 0
    
    class AgentBrain:
        def __init__(self, brain):
            self.brain = brain
        
        def initialize(self, world):
            pass
        
        def processSensoryInput(self, percept):
            pass
        
        def decodeMotorCommand(self):
            return None
        
        def applyRewardModulation(self, reward, predicted):
            pass
    
    class SimpleWorld:
        def __init__(self):
            pass
        
        def configure(self, width, height, visionWidth, visionHeight):
            pass
        
        def update(self, timestep):
            pass
        
        def applyMotorCommand(self, cmd, time):
            return None
        
        def getSensoryPercept(self):
            return None
        
        def getAgentBody(self):
            return None
    
    class SensoryInput:
        pass
    
    class Vision(SensoryInput):
        pass
    
    class Audio(SensoryInput):
        pass
    
    class InternalSignals(SensoryInput):
        pass
    
    class Action:
        def __init__(self, action_type=None):
            self.type = action_type
    
    class AgentBody:
        pass
    
    class SensoryPercept:
        pass
    
    class ActionResult:
        def __init__(self, reward=0.0, success=False, message=""):
            self.reward = reward
            self.success = success
            self.message = message
    
    class WorldObject:
        pass
    
    # Enums
    class NeuronType:
        Excitatory = 1
        Inhibitory = 2
        Modulatory = 3
        Sensory = 4
        Motor = 5
        Internal = 6
    
    class SynapseType:
        Excitatory = 1
        Inhibitory = 2
        Modulatory = 3
        Electrical = 4
        GapJunction = 5
    
    class DevelopmentalStage:
        Initial = 1
        CriticalPeriod = 2
        Maturation = 3
        Adult = 4
        Aging = 5
    
    class FiringState:
        Resting = 1
        Active = 2
        Refractory = 3
        Inhibited = 4
    
    class ActionType:
        MoveForward = 1
        MoveBackward = 2
        MoveLeft = 3
        MoveRight = 4
        TurnLeft = 5
        TurnRight = 6
        Look = 7
        LookUp = 8
        LookDown = 9
        Interact = 10
        Eat = 11
        Drink = 12
        Rest = 13
        Wait = 14
        Custom = 15
    
    class MotorCommand:
        MoveForward = 1
        MoveBackward = 2
        TurnLeft = 3
        TurnRight = 4
        LookLeft = 5
        LookRight = 6
        Interact = 7
        Wait = 8
    
    class WorldObjectType:
        Empty = 1
        Resource = 2
        Hazard = 3
        Wall = 4
        Marker = 5
    
    # Factory functions (create placeholder instances)
    def createDefaultConfig():
        return Config()
    
    def createBrain(config):
        return Brain(config)
    
    def createSimpleWorld():
        return SimpleWorld()
    
    def createAgentBrain(brain):
        return AgentBrain(brain)
    
    # Utility functions
    def setup_logging(level="INFO"):
        pass
    
    def validate_config(config):
        return {'valid': True, 'errors': []}
    
    def benchmark_simulation(brain, steps=100):
        return {'error': 'Benchmarking not available - C++ extension not installed'}
    
    def analyze_brain_performance(brain):
        return {'error': 'Analysis not available - C++ extension not installed'}
    
    # Advanced modules (not available)
    PerformanceMonitor = None
    BatchProcessor = None
    Analytics = None
    Benchmarking = None
    advanced_api_examples = None
    
    # Module metadata
    __version__ = "0.1.0"
    __author__ = "NLM Project Authors"
    __email__ = "nlm-project@example.com"
    __license__ = "MIT"
    __url__ = "https://github.com/nlm-project/nlm"
    
    # Module-level helper functions
    def create_experiment(config_dict=None, brain_config=None):
        if brain_config is not None:
            return brain_config, None, None
        else:
            config = createDefaultConfig()
            if config_dict:
                for key, value in config_dict.items():
                    config.set(key, value)
            
            brain = createBrain(config)
            brain.initialize()
            return brain, None, None
    
    def run_simulation(brain, steps=100, timestep=0.001):
        return {
            'total_steps': steps,
            'final_firing_rate': 0.0,
            'total_spikes': 0,
            'active_neurons': 0,
            'average_firing_rate': 0.0,
            'error': 'Simulation not available - C++ extension not installed'
        }
    
    def validate_setup(brain=None, world=None, agent=None):
        return {
            'brain_valid': False,
            'world_valid': False,
            'agent_valid': False,
            'errors': ['NLM C++ extension not installed'],
            'warnings': []
        }
    
    def get_module_info():
        return {
            'version': __version__,
            'author': __author__,
            'email': __email__,
            'license': __license__,
            'url': __url__,
            'description': "NLM Python bindings not available - C++ extension not installed",
            'available_classes': [],
            'advanced_features': {
                'performance_monitoring': False,
                'batch_processing': False,
                'analytics': False,
                'benchmarking': False,
                'examples': False
            },
            'installation_error': True
        }
    
    # Module-level singletons
    _default_config = None
    
    def get_default_config():
        global _default_config
        if _default_config is None:
            _default_config = createDefaultConfig()
        return _default_config
    
    def reset_default_config():
        global _default_config
        _default_config = None
    
    # Update __all__ for the error case
    __all__ = [
        'Config', 'Brain', 'AgentBrain', 'SimpleWorld',
        'SensoryInput', 'Vision', 'Audio', 'InternalSignals',
        'Action', 'AgentBody', 'SensoryPercept', 'ActionResult',
        'WorldObject',
        'NeuronType', 'SynapseType', 'DevelopmentalStage', 'FiringState',
        'ActionType', 'MotorCommand', 'WorldObjectType',
        'createDefaultConfig', 'createBrain', 'createSimpleWorld', 'createAgentBrain',
        'setup_logging', 'validate_config', 'benchmark_simulation', 'analyze_brain_performance',
        'PerformanceMonitor', 'BatchProcessor', 'Analytics', 'Benchmarking', 'advanced_api_examples',
        '__version__', '__author__', '__email__', '__license__', '__url__',
        'create_experiment', 'run_simulation', 'validate_setup', 'get_module_info',
        'get_default_config', 'reset_default_config'
    ]
    
    # Log initialization error
    warnings.warn(
        "NLM Python bindings are not fully available. "
        "The C++ extension is not installed. "
        "Please install the NLM C++ library first.",
        NLMWarning
    )

# Package metadata for distribution tools
__description__ = "Python bindings for NLM (Neural Learning Machine) C++ neural simulation framework"
__long_description__ = __doc__
__keywords__ = ["neural", "simulation", "spiking", "brain", "learning", "AI", "machine learning"]
__classifiers__ = [
    "Development Status :: 3 - Alpha",
    "Intended Audience :: Science/Research",
    "License :: OSI Approved :: MIT License",
    "Programming Language :: Python :: 3",
    "Programming Language :: Python :: 3.8",
    "Programming Language :: Python :: 3.9",
    "Programming Language :: Python :: 3.10",
    "Programming Language :: Python :: 3.11",
    "Programming Language :: Python :: 3.12",
    "Topic :: Scientific/Engineering :: Artificial Intelligence",
    "Topic :: Software Development :: Libraries :: Python Modules"
]

# Export package information
__package__ = "pynlm"

# Version comparison for compatibility
import sys
if sys.version_info < (3, 8):
    warnings.warn(
        "Python 3.8 or higher is recommended for NLM Python bindings",
        NLMWarning
    )
