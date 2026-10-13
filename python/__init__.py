"""
NLM — 熙然

An Experimental Artificial Developmental Brain

This is the main entry point for the NLM Python package.
It provides a Python interface to the NLM neural simulation framework.

Key Features:
- Enhanced configuration management
- Python-native brain simulation (when C++ bindings unavailable)
- Comprehensive logging and error handling
- Visualization and monitoring tools
- Simulation context managers
- Development and experimentation tools

Usage Examples:

Basic Usage:
    import nlm
    
    # Create a brain with custom configuration
    config = nlm.Config()
    config.set("neuron_count", 1000)
    config.set("environment_name", "GridWorld")
    
    brain = nlm.create_brain(config)
    brain.initialize()
    
    # Run simulation
    for step in range(100):
        brain.step(step)

Context Manager Usage:
    with nlm.simulation_context(steps=50) as brain:
        for step in range(50):
            brain.step(step)

Visualization:
    with nlm.simulation_context(steps=100) as brain:
        visualizer = nlm.create_visualizer(brain)
        for step in range(100):
            brain.step(step)
            # Get visualization data
            data = visualizer.get_current_state()

Error Handling:
    try:
        with nlm.simulation_context(steps=100) as brain:
            for step in range(100):
                brain.step(step)
    except nlm.SimulationError as e:
        print(f"Simulation failed: {e}")

Configuration Management:
    # Get global configuration
    config = nlm.get_config()
    config.set("log_level", "DEBUG")
    
    # Load from file
    config.load_from_file("configs/my_config.cfg")
    
    # Save to file
    config.save_to_file("configs/saved_config.cfg")

Installation:
    The NLM Python package can be installed with pip:
    pip install .
    
    For development, install in editable mode:
    pip install -e .

Requirements:
    - Python 3.8+
    - numpy (optional, for advanced features)
    - pybind11 (for C++ bindings)
"""

# Version information
__version__ = "0.1.0"
__author__ = "NLM Project"
__description__ = "Neural Learning Machine - Python bindings for brain simulation"

# Import core modules and make them available at package level
try:
    from .nlm import (
        Config,
        ConfigSource,
        NLMConfigError,
        Brain,
        NeuronType,
        SynapseType,
        DevelopmentalStage,
        SensoryInput,
        Vision,
        Audio,
        InternalSignals,
        Action,
        SimpleWorld,
        AgentBrain,
        Neuromodulator,
        Dopamine,
        WorkingMemory,
        EpisodicMemory,
        PredictionSystem,
        NeuralPlanner,
        ConceptFormation,
        AttentionalSelection,
        DevelopmentSystem,
        STDP,
        Hebbian,
        SpikeSystem,
        VisualizationInterface,
        Logger,
        LogLevel,
        get_config,
        set_config_value,
        get_config_value,
        get_logger,
        create_brain,
        create_world,
        create_agent_brain,
        create_visualizer,
        simulation_context,
        handle_simulation_error,
        SimulationError,
    )
    
    # Make symbols available at package level
    __all__ = [
        'Config',
        'ConfigSource',
        'NLMConfigError',
        'Brain',
        'NeuronType',
        'SynapseType',
        'DevelopmentalStage',
        'SensoryInput',
        'Vision',
        'Audio',
        'InternalSignals',
        'Action',
        'SimpleWorld',
        'AgentBrain',
        'Neuromodulator',
        'Dopamine',
        'WorkingMemory',
        'EpisodicMemory',
        'PredictionSystem',
        'NeuralPlanner',
        'ConceptFormation',
        'AttentionalSelection',
        'DevelopmentSystem',
        'STDP',
        'Hebbian',
        'SpikeSystem',
        'VisualizationInterface',
        'Logger',
        'LogLevel',
        'get_config',
        'set_config_value',
        'get_config_value',
        'get_logger',
        'create_brain',
        'create_world',
        'create_agent_brain',
        'create_visualizer',
        'simulation_context',
        'handle_simulation_error',
        'SimulationError',
        'get_config_value',
    ]
    
except ImportError as e:
    print(f"Warning: Some modules could not be imported: {e}")
    # Define placeholder classes for better error messages
    class ImportError(Exception):
        pass
    
    # Create placeholder classes
    class Config:
        def __init__(self):
            raise ImportError("NLM Python bindings not available. Please install the NLM C++ extension.")
    
    class Brain:
        def __init__(self, config):
            raise ImportError("NLM Python bindings not available. Please install the NLM C++ extension.")
    
    class SimulationError(Exception):
        pass
    
    # Other placeholders...
    # (For brevity, only showing key ones)

# Package metadata
__package__ = "nlm"
__license__ = "MIT"
__copyright__ = "Copyright 2024 NLM Project"
__maintainer__ = "NLM Project"
__email__ = "contact@nlm-project.org"