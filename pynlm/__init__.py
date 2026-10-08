from .bindings import (
    # Core classes
    Config,
    Brain,
    AgentBrain,
    SimpleWorld,
    SensoryInput,
    Vision,
    Audio,
    InternalSignals,
    Action,
    WorldObject,
    AgentBody,
    ActionResult,
    SensoryPercept,
    
    # ID types
    NeuronId,
    SynapseId,
    RegionId,
    PopulationId,
    
    # Enum types
    NeuronType,
    SynapseType,
    DevelopmentalStage,
    FiringState,
    ActionType,
    MotorCommand,
    WorldObjectType,
    
    # Constants
    INVALID_NEURON_ID,
    INVALID_SYNAPSE_ID,
    INVALID_REGION_ID,
    INVALID_POPULATION_ID,
    
    # Exception types
    NLMException,
    InvalidArgumentError,
    ConfigurationError,
    BrainInitializationError,
    
    # Factory functions
    create_default_config,
    create_brain,
    create_simple_world,
    create_agent_brain,
    
    # Utility functions
    run_simulation,
    run_agent_episode,
    random_seed,
    get_version,
    get_system_info,
)

# Package version
__version__ = "0.1.0"
__all__ = [
    'Config', 'Brain', 'AgentBrain', 'SimpleWorld', 'SensoryInput',
    'Vision', 'Audio', 'InternalSignals', 'Action', 'WorldObject',
    'AgentBody', 'ActionResult', 'SensoryPercept',
    'NeuronId', 'SynapseId', 'RegionId', 'PopulationId',
    'NeuronType', 'SynapseType', 'DevelopmentalStage', 'FiringState',
    'ActionType', 'MotorCommand', 'WorldObjectType',
    'INVALID_NEURON_ID', 'INVALID_SYNAPSE_ID', 'INVALID_REGION_ID', 'INVALID_POPULATION_ID',
    'NLMException', 'InvalidArgumentError', 'ConfigurationError', 'BrainInitializationError',
    'create_default_config', 'create_brain', 'create_simple_world', 'create_agent_brain',
    'run_simulation', 'run_agent_episode', 'random_seed', 'get_version', 'get_system_info'
]

# Module metadata
__metadata__ = {
    'name': 'pynlm',
    'version': __version__,
    'description': 'Enhanced Python bindings for NLM (Neural Learning Machine)',
    'author': 'NLM Authors',
    'license': 'MIT',
    'platform': 'any',
    'python_requires': '>=3.8',
    'classifiers': [
        'Development Status :: 3 - Alpha',
        'Intended Audience :: Science/Research',
        'License :: OSI Approved :: MIT License',
        'Programming Language :: Python :: 3',
        'Programming Language :: Python :: 3.8',
        'Programming Language :: Python :: 3.9',
        'Programming Language :: Python :: 3.10',
        'Programming Language :: Python :: 3.11',
        'Programming Language :: Python :: 3.12',
        'Programming Language :: C++',
        'Topic :: Scientific/Engineering :: Artificial Intelligence',
        'Topic :: Scientific/Engineering :: Artificial Neural Networks',
        'Topic :: Software Development :: Libraries :: Python Bindings'
    ],
    'keywords': 'neural simulation spiking brain learning neuroscience'
}