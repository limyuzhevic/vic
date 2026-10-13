"""
Neural Learning Machine (NLM) - Complete Python Bindings
=========================================================

This package provides comprehensive Python bindings for the NLM neural simulation framework,
exposing all neural architecture, neuromodulation, memory, cognition, and plasticity systems.

Key Features:
- Complete neural architecture (Regions, Populations, Neurons, Synapses)
- Advanced neuromodulation (Dopamine, Curiosity, Novelty, PredictionError)
- Sophisticated memory systems (WorkingMemory, EpisodicMemory, AssociativeMemory)
- Cognitive functions (Planning, ConceptFormation, Attention)
- Prediction and learning systems
- Plasticity rules (STDP, Hebbian, StructuralPlasticity)
- Development and maturation
- Performance optimization (SpikeSystem, CheckpointManager)
- Advanced visualization and analysis tools
- Network analysis utilities
- Simulation control and debugging tools
- Advanced convenience functions and class abstractions
"""

__version__ = "1.0.0"
__author__ = "Neural Learning Machine Team"
__email__ = "nlm-team@example.com"
__license__ = "Apache-2.0"

# Core classes and modules
from pynlm import (
    # Configuration
    Config,
    
    # Neural Architecture
    NeuralRegion,
    NeuralPopulation,
    Neuron,
    Synapse,
    NeuronId,
    SynapseId,
    RegionId,
    PopulationId,
    
    # Types and Enums
    NeuronType,
    SynapseType,
    DevelopmentalStage,
    FiringState,
    ActionType,
    MotorCommand,
    WorldObjectType,
    
    # Brain Components
    Brain,
    AgentBrain,
    
    # Memory Systems
    NeuralWorkingMemory,
    NeuralEpisodicMemory,
    
    # Neuromodulation
    Neuromodulator,
    Dopamine,
    Curiosity,
    Novelty,
    PredictionError,
    Reward,
    
    # Cognition
    NeuralPlanner,
    ConceptFormation,
    AttentionalSelection,
    
    # Development
    DevelopmentSystem,
    Maturation,
    Pruning,
    Synaptogenesis,
    
    # Plasticity
    PlasticityRule,
    STDP,
    Hebbian,
    StructuralPlasticity,
    
    # Prediction
    PredictionSystem,
    NeuralPrediction,
    
    # Performance
    SpikeSystem,
    CheckpointSystem,
    SynapseLoader,
    
    # World
    SimpleWorld,
    WorldObject,
    AgentBody,
    SensoryPercept,
    
    # Sensory
    SensoryInput,
    Vision,
    Audio,
    InternalSignals,
    
    # Motor
    Action,
    ActionResult,
    
    # Environment
    Observation,
    
    # Spatial and Temporal Processing
    SpatialRepresentation,
    TemporalRelation,
    SelfModel,
    SocialLearning,
    
    # Visualizer
    Visualizer,
)

# Factory functions and utilities
from pynlm import (
    # Factory functions
    createDefaultConfig,
    createBrain,
    createSimpleWorld,
    createAgentBrain,
    createDefaultSensoryInput,
    
    # Network analysis
    analyzeNetworkTopology,
    computeNetworkMetrics,
    findCriticalNeurons,
    calculateClusteringCoefficient,
    computeSmallWorldness,
    
    # Simulation control
    runSimulation,
    runEpisode,
    
    # Debugging tools
    brainProfiler,
    debugSpikeTiming,
    validateBrainState,
    analyzeMemoryUsage,
    
    # Utilities
    showExampleUsage,
    float_array_to_list,
    list_to_float_array,
    setMemoryLimit,
    getMemoryUsage,
)

# Advanced API - High-level convenience functions and class abstractions
from pynlm.advanced_api import (
    # High-level convenience functions
    AdvancedAPI,
    run_simulation,
    run_episode,
    create_brain_from_config,
    create_brain_from_json,
    create_world,
    create_agent,
    analyze_brain_activity,
    visualize_brain_state,
    save_experiment_results,
    load_experiment_results,
    load_config_from_file,
    save_config_to_json,
    
    # High-level class abstractions
    SimpleAgent,
    SimulationRunner,
    BrainProfiler,
    VisualizationHelper,
    ExperimentManager,
    
    # Factory functions for advanced tools
    createBrainProfiler,
    createVisualizationHelper,
    createExperimentManager,
    createNeuronActivityMonitor,
    createNetworkConnectivityAnalyzer,
    createMemorySystemProfiler,
    createNeuromodulationAnalyzer,
    createDevelopmentStageAnalyzer,
)

# Constants
from pynlm import (
    INVALID_NEURON_ID,
    INVALID_SYNAPSE_ID,
    INVALID_REGION_ID,
    INVALID_POPULATION_ID,
    VERSION,
    BUILD_DATE,
    API_LEVEL,
)

# Error classes
from pynlm import (
    RuntimeError,
    InvalidArgument,
)

__all__ = [
    # Core classes
    'Config',
    'NeuralRegion', 'NeuralPopulation', 'Neuron', 'Synapse',
    'Brain', 'AgentBrain',
    'SimpleWorld', 'WorldObject', 'AgentBody', 'SensoryPercept',
    'SensoryInput', 'Vision', 'Audio', 'InternalSignals',
    'Action', 'ActionResult',
    
    # Memory systems
    'NeuralWorkingMemory', 'NeuralEpisodicMemory',
    
    # Neuromodulation
    'Neuromodulator', 'Dopamine', 'Curiosity', 'Novelty', 'PredictionError', 'Reward',
    
    # Cognition
    'NeuralPlanner', 'ConceptFormation', 'AttentionalSelection',
    
    # Development
    'DevelopmentSystem', 'Maturation', 'Pruning', 'Synaptogenesis',
    
    # Plasticity
    'PlasticityRule', 'STDP', 'Hebbian', 'StructuralPlasticity',
    
    # Prediction
    'PredictionSystem', 'NeuralPrediction',
    
    # Performance
    'SpikeSystem', 'CheckpointSystem', 'SynapseLoader',
    
    # Environment
    'Observation',
    
    # Spatial and Temporal Processing
    'SpatialRepresentation', 'TemporalRelation', 'SelfModel', 'SocialLearning',
    
    # Visualizer
    'Visualizer',
    
    # Types and IDs
    'NeuronId', 'SynapseId', 'RegionId', 'PopulationId',
    'NeuronType', 'SynapseType', 'DevelopmentalStage', 'FiringState',
    'ActionType', 'MotorCommand', 'WorldObjectType',
    
    # Factory functions
    'createDefaultConfig', 'createBrain', 'createSimpleWorld', 'createAgentBrain',
    'createDefaultSensoryInput',
    
    # Network analysis
    'analyzeNetworkTopology', 'computeNetworkMetrics', 'findCriticalNeurons',
    'calculateClusteringCoefficient', 'computeSmallWorldness',
    
    # Simulation control
    'runSimulation', 'runEpisode',
    
    # Debugging tools
    'brainProfiler', 'debugSpikeTiming', 'validateBrainState', 'analyzeMemoryUsage',
    
    # Utilities
    'showExampleUsage', 'float_array_to_list', 'list_to_float_array',
    'setMemoryLimit', 'getMemoryUsage',
    
    # Constants
    'INVALID_NEURON_ID', 'INVALID_SYNAPSE_ID', 'INVALID_REGION_ID', 'INVALID_POPULATION_ID',
    'VERSION', 'BUILD_DATE', 'API_LEVEL',
    
    # Advanced API - High-level convenience functions and class abstractions
__all__.extend([
    # Advanced API main class
    'AdvancedAPI',
    
    # High-level convenience functions
    'run_simulation',
    'create_brain_from_config',
    'create_brain_from_json',
    'create_world',
    'create_agent',
    'analyze_brain_activity',
    'visualize_brain_state',
    'save_experiment_results',
    'load_experiment_results',
    'load_config_from_file',
    'save_config_to_json',
    
    # High-level class abstractions
    'SimpleAgent',
    'SimulationRunner',
    'BrainProfiler',
    'VisualizationHelper',
    'ExperimentManager',
    
    # Factory functions for advanced tools
    'createBrainProfiler',
    'createVisualizationHelper',
    'createExperimentManager',
    'createNeuronActivityMonitor',
    'createNetworkConnectivityAnalyzer',
    'createMemorySystemProfiler',
    'createNeuromodulationAnalyzer',
    'createDevelopmentStageAnalyzer',
])
