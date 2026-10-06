# NLM Python package initialization
# Enhanced Python bindings for NLM (Neural Learning Machine)

from .pynlm import (
    Config,
    Brain,
    SimpleWorld,
    AgentBrain,
    Vision,
    Audio,
    InternalSignals,
    Action,
    ActionType,
    MotorCommand,
    WorldObject,
    AgentBody,
    SensoryPercept,
    NeuronType,
    SynapseType,
    DevelopmentalStage,
    FiringState,
    NeuronId,
    SynapseId,
    RegionId,
    PopulationId,
    WorldObjectType,
    INVALID_NEURON_ID,
    INVALID_SYNAPSE_ID,
    INVALID_REGION_ID,
    INVALID_POPULATION_ID,
    create_agent_simulation,
    get_default_config,
    createBrain,
    createSimpleWorld,
    createAgentBrain,
)

# Constants for convenience
NEURON_TYPE_EXCITATORY = NeuronType.Excitatory
NEURON_TYPE_INHIBITORY = NeuronType.Inhibitory
NEURON_TYPE_MODULATORY = NeuronType.Modulatory
NEURON_TYPE_SENSORY = NeuronType.Sensory
NEURON_TYPE_MOTOR = NeuronType.Motor
NEURON_TYPE_INTERNAL = NeuronType.Internal

ACTION_TYPE_MOVE_FORWARD = ActionType.MoveForward
ACTION_TYPE_MOVE_BACKWARD = ActionType.MoveBackward
ACTION_TYPE_MOVE_LEFT = ActionType.MoveLeft
ACTION_TYPE_MOVE_RIGHT = ActionType.MoveRight
ACTION_TYPE_TURN_LEFT = ActionType.TurnLeft
ACTION_TYPE_TURN_RIGHT = ActionType.TurnRight
ACTION_TYPE_LOOK = ActionType.Look
ACTION_TYPE_LOOK_UP = ActionType.LookUp
ACTION_TYPE_LOOK_DOWN = ActionType.LookDown
ACTION_TYPE_INTERACT = ActionType.Interact
ACTION_TYPE_EAT = ActionType.Eat
ACTION_TYPE_DRINK = ActionType.Drink
ACTION_TYPE_REST = ActionType.Rest
ACTION_TYPE_WAIT = ActionType.Wait
ACTION_TYPE_CUSTOM = ActionType.Custom

MOTOR_COMMAND_MOVE_FORWARD = MotorCommand.MoveForward
MOTOR_COMMAND_MOVE_BACKWARD = MotorCommand.MoveBackward
MOTOR_COMMAND_TURN_LEFT = MotorCommand.TurnLeft
MOTOR_COMMAND_TURN_RIGHT = MotorCommand.TurnRight
MOTOR_COMMAND_LOOK_LEFT = MotorCommand.LookLeft
MOTOR_COMMAND_LOOK_RIGHT = MotorCommand.LookRight
MOTOR_COMMAND_INTERACT = MotorCommand.Interact
MOTOR_COMMAND_WAIT = MotorCommand.Wait

# Re-export helper functions
from .helpers import (
    load_config_from_env,
    validate_config,
    create_simulation_step_callback,
    run_episode,
    visualize_brain,
    compare_configs,
    get_debug_config,
    get_performance_config,
    get_creative_config,
    load_config_from_dict,
    save_config_to_dict,
    ConfigValidator,
    export_for_web,
)

# Error handling
from .pynlm.error import getErrorManager, NLMError

__all__ = [
    # Main classes
    'Config',
    'Brain',
    'SimpleWorld',
    'AgentBrain',
    'Vision',
    'Audio',
    'InternalSignals',
    'Action',
    'ActionType',
    'MotorCommand',
    'WorldObject',
    'AgentBody',
    'SensoryPercept',
    'NeuronType',
    'SynapseType',
    'DevelopmentalStage',
    'FiringState',
    'NeuronId',
    'SynapseId',
    'RegionId',
    'PopulationId',
    'WorldObjectType',
    'INVALID_NEURON_ID',
    'INVALID_SYNAPSE_ID',
    'INVALID_REGION_ID',
    'INVALID_POPULATION_ID',
    
    # Convenience constants
    'NEURON_TYPE_EXCITATORY',
    'NEURON_TYPE_INHIBITORY',
    'NEURON_TYPE_MODULATORY',
    'NEURON_TYPE_SENSORY',
    'NEURON_TYPE_MOTOR',
    'NEURON_TYPE_INTERNAL',
    'ACTION_TYPE_MOVE_FORWARD',
    'ACTION_TYPE_MOVE_BACKWARD',
    'ACTION_TYPE_MOVE_LEFT',
    'ACTION_TYPE_MOVE_RIGHT',
    'ACTION_TYPE_TURN_LEFT',
    'ACTION_TYPE_TURN_RIGHT',
    'ACTION_TYPE_LOOK',
    'ACTION_TYPE_LOOK_UP',
    'ACTION_TYPE_LOOK_DOWN',
    'ACTION_TYPE_INTERACT',
    'ACTION_TYPE_EAT',
    'ACTION_TYPE_DRINK',
    'ACTION_TYPE_REST',
    'ACTION_TYPE_WAIT',
    'ACTION_TYPE_CUSTOM',
    'MOTOR_COMMAND_MOVE_FORWARD',
    'MOTOR_COMMAND_MOVE_BACKWARD',
    'MOTOR_COMMAND_TURN_LEFT',
    'MOTOR_COMMAND_TURN_RIGHT',
    'MOTOR_COMMAND_LOOK_LEFT',
    'MOTOR_COMMAND_LOOK_RIGHT',
    'MOTOR_COMMAND_INTERACT',
    'MOTOR_COMMAND_WAIT',
    
    # Functions
    'create_agent_simulation',
    'get_default_config',
    'createBrain',
    'createSimpleWorld',
    'createAgentBrain',
    
    # Helper functions
    'load_config_from_env',
    'validate_config',
    'create_simulation_step_callback',
    'run_episode',
    'visualize_brain',
    'compare_configs',
    'get_debug_config',
    'get_performance_config',
    'get_creative_config',
    'load_config_from_dict',
    'save_config_to_dict',
    'ConfigValidator',
    'export_for_web',
    
    # Error handling
    'getErrorManager',
    'NLMError',
]

__version__ = "0.1.0 (Enhanced)"

# Package metadata
__author__ = "NLM Authors"
__description__ = "Python bindings for NLM (Neural Learning Machine) C++ neural simulation framework"
__license__ = "MIT"

# Environment setup
def setup_environment():
    """
    Setup the NLM Python environment.
    This function performs any necessary environment setup when the package is imported.
    """
    import os
    import warnings
    
    # Set default environment variables if not already set
    if 'NLM_SIMULATION_STEPS' not in os.environ:
        os.environ['NLM_SIMULATION_STEPS'] = '100'
    
    # Check for potential issues
    if os.name == 'nt':  # Windows
        warnings.warn("Running on Windows. Some features may have reduced performance.")
    
    # Log initialization
    import sys
    print(f"NLM Python bindings v{__version__} initialized")
    print(f"Python: {sys.version}")
    print(f"Platform: {os.name}")

# Auto-initialize when package is imported
setup_environment()
