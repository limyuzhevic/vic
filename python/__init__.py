# NLM (Neural Learning Machine) - Enhanced Python Bindings
# An experimental artificial developmental brain

__version__ = "2.0.0"
__author__ = "NLM Team"
__description__ = "Enhanced Neural Learning Machine Python bindings"
__license__ = "MIT"
__copyright__ = "Copyright 2024-2026 NLM Team"

# Core types
from .types import (
    NeuronId,
    SynapseId,
    RegionId,
    PopulationId,
    NeuronType,
    SynapseType,
    DevelopmentalStage,
    FiringState,
    ActionType,
    MotorCommand,
    WorldObjectType,
)

# Core classes
from .brain import Brain
from .agent import AgentBrain, AgentBody
from .world import SimpleWorld, WorldObject
from .sensory import Vision, Audio, InternalSignals, SensoryInput
from .motor import Action, ActionResult
from .config import Config
from .environment import Environment
from .experiments import Experiment, ExperimentRunner

# Expert module
from .expert import (
    MemoryPool, SIMDProcessor, ParallelProcessor,
    CustomAllocator, ZeroCopyInterface, DirectMemoryAccess,
    CustomPlasticityRules, AdvancedNeuromodulation,
    CustomConnectivityPatterns, NeuralPopulationManager,
    NeuralSignalProcessor, PatternRecognizer, StatisticalAnalyzer,
    MLIntegration, SignalFilter, FeatureExtractor,
    CustomNeuronModel, SynapticDynamics,
    RegionConfiguration, NetworkTopology,
    DynamicConfig, ConfigInheritance, ConfigValidator,
    ConfigOptimizer, ConfigSnapshot,
    CustomExperimentLoop, ParallelExperimentRunner,
    CheckpointManager, ResultAnalyzer, ExperimentMonitor,
    CXXExtensionAPI, CustomIOFormat, SensorIntegration,
    WebSocketAPI, ExternalResourceManager,
    AdvancedPlotting, RealTimeVisualizer,
    VisualizationPipeline, DataExporter,
    MemoryProfiler, PerformanceProfiler,
    BenchmarkRunner, ProfilingSession
)

# Factory functions
from .factories import (
    create_default_config,
    create_minimal_config,
    create_high_performance_config,
    create_grid_world_config,
    create_maze_world_config,
    create_brain,
    create_simple_world,
    create_agent_brain,
    create_simulation_controller,
    create_performance_monitor,
    create_batch_processor,
    create_experiment_manager,
)

# Visualization helpers
from .visualization import generate_neuron_activity_plot, generate_connection_density_plot

# Exception types
from .exceptions import NLMError, ConfigurationError, SimulationError, ValidationError

# Main module exports
__all__ = [
    # Version info
    '__version__', '__author__', '__description__', '__license__', '__copyright__',
    
    # Core types
    'NeuronId', 'SynapseId', 'RegionId', 'PopulationId',
    'NeuronType', 'SynapseType', 'DevelopmentalStage', 'FiringState',
    'ActionType', 'MotorCommand', 'WorldObjectType',
    
    # Core classes
    'Brain', 'AgentBrain', 'AgentBody', 'SimpleWorld', 'WorldObject',
    'Vision', 'Audio', 'InternalSignals', 'SensoryInput',
    'Action', 'ActionResult', 'Config', 'Environment',
    'Experiment', 'ExperimentRunner',
    
    # Expert APIs
    'MemoryPool', 'SIMDProcessor', 'ParallelProcessor',
    'CustomAllocator', 'ZeroCopyInterface', 'DirectMemoryAccess',
    'CustomPlasticityRules', 'AdvancedNeuromodulation',
    'CustomConnectivityPatterns', 'NeuralPopulationManager',
    'NeuralSignalProcessor', 'PatternRecognizer', 'StatisticalAnalyzer',
    'MLIntegration', 'SignalFilter', 'FeatureExtractor',
    'CustomNeuronModel', 'SynapticDynamics',
    'RegionConfiguration', 'NetworkTopology',
    'DynamicConfig', 'ConfigInheritance', 'ConfigValidator',
    'ConfigOptimizer', 'ConfigSnapshot',
    'CustomExperimentLoop', 'ParallelExperimentRunner',
    'CheckpointManager', 'ResultAnalyzer', 'ExperimentMonitor',
    'CXXExtensionAPI', 'CustomIOFormat', 'SensorIntegration',
    'WebSocketAPI', 'ExternalResourceManager',
    'AdvancedPlotting', 'RealTimeVisualizer',
    'VisualizationPipeline', 'DataExporter',
    'MemoryProfiler', 'PerformanceProfiler',
    'BenchmarkRunner', 'ProfilingSession',
]

# Expert module
from .expert import (
    MemoryPool, SIMDProcessor, ParallelProcessor,
    CustomAllocator, ZeroCopyInterface, DirectMemoryAccess,
    CustomPlasticityRules, AdvancedNeuromodulation,
    CustomConnectivityPatterns, NeuralPopulationManager,
    NeuralSignalProcessor, PatternRecognizer, StatisticalAnalyzer,
    MLIntegration, SignalFilter, FeatureExtractor,
    CustomNeuronModel, SynapticDynamics,
    RegionConfiguration, NetworkTopology,
    DynamicConfig, ConfigInheritance, ConfigValidator,
    ConfigOptimizer, ConfigSnapshot,
    CustomExperimentLoop, ParallelExperimentRunner,
    CheckpointManager, ResultAnalyzer, ExperimentMonitor,
    CXXExtensionAPI, CustomIOFormat, SensorIntegration,
    WebSocketAPI, ExternalResourceManager,
    AdvancedPlotting, RealTimeVisualizer,
    VisualizationPipeline, DataExporter,
    MemoryProfiler, PerformanceProfiler,
    BenchmarkRunner, ProfilingSession
)

# Expert examples
expert_examples = """
Expert NLM Usage Examples:

# 1. Performance optimization with memory pools
from nlm import expert
pool = expert.MemoryPool(1024, 8192)  # Custom memory pool
processor = expert.SIMDProcessor()  # SIMD operations
parallel = expert.ParallelProcessor(4)  # 4 threads

# 2. Advanced neural models
neuron_model = expert.CustomNeuronModel("biophysical_lif")
plasticity_rules = expert.CustomPlasticityRules()
neuromodulation = expert.AdvancedNeuromodulation({
    'dopamine_strength': 0.8,
    'serotonin_influence': 0.6
})

# 3. Real-time visualization pipeline
pipeline = expert.VisualizationPipeline()
real_time_viz = expert.RealTimeVisualizer(pipeline)

# 4. Advanced configuration management
config = expert.DynamicConfig()
config.load_from_json("expert_config.json")
config.validate()

# 5. Parallel experiment execution
experiment = expert.CustomExperimentLoop(100, 0.001)
runner = expert.ParallelExperimentRunner([experiment], 8)
runner.run()

# 6. Custom I/O and integration
io_api = expert.CustomIOFormat("hdf5")
ws_api = expert.WebSocketAPI("ws://localhost:8080")
sensor = expert.SensorIntegration("neurosensor")
"""
