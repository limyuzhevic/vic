# Core NLM Python module
# Provides a Pythonic interface to the NLM neural simulation framework

__all__ = [
    # Core classes
    'Brain', 'Config', 'createDefaultConfig',
    'SimpleWorld', 'AgentBrain', 'Agent',
    'Vision', 'Audio', 'InternalSignals', 'SensoryInput',
    'Action', 'ActionType', 'ActionResult',
    'WorldObject', 'WorldObjectType',
    
    # Enumerations
    'NeuronType', 'SynapseType', 'DevelopmentalStage', 'FiringState',
    'MotorCommand', 'WorldObjectType',
    
    # Unique identifiers
    'NeuronId', 'SynapseId', 'RegionId', 'PopulationId',
    'INVALID_NEURON_ID', 'INVALID_SYNAPSE_ID', 'INVALID_REGION_ID', 'INVALID_POPULATION_ID',
    
    # Error handling
    'NLMRuntimeError', 'NLMConfigurationError', 'NLMSimulationError', 'NLMValidationError',
    'NLMLogger', 'ErrorHandler', 'ProgressTracker',
    
    # Enhanced utility functions
    'create_simple_brain', 'create_agent_environment',
    'run_simulation_episode', 'get_brain_summary',
    'benchmark_simulation', 'validate_configuration',
    'create_brain_from_preset', 'run_parallel_simulations',
    'analyze_simulation_results', 'export_results_to_json',
    'import_results_from_json', 'validate_simulation_params',
    'get_performance_rating', 'format_simulation_time',
    'generate_simulation_report',
    
    # Command line interface
    'setup_cli', 'ExecutionMode', 'SimulationPreset',
    
    # Error handling utilities
    'get_logger', 'get_error_handler', 'safe_simulation_step',
    'validate_and_log_config', 'NLMErrorContext',
]

from .brain import Brain
from .config import Config, createDefaultConfig
from .world import SimpleWorld
from .agent import AgentBrain, Agent
from .sensory import Vision, Audio, InternalSignals, SensoryInput
from .motor import Action
from .errors import NLMRuntimeError, NLMError

# Convenience functions
from .utils import (
    create_simple_brain, create_agent_environment,
    run_simulation_episode, get_brain_summary,
    benchmark_simulation, validate_configuration
)

__version__ = "0.1.0"
__author__ = "NLM Authors"
