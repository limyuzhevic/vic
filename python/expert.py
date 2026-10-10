"""Expert NLM API Module
# Advanced features for expert users requiring deeper control, performance optimization, and custom capabilities

# Import all expert modules directly from subdirectories
import sys
import os

# Add expert directory to path
sys.path.insert(0, os.path.join(os.path.dirname(__file__), 'expert'))

# Import all expert modules using absolute imports
from expert.performance import (
    MemoryPool, SIMDProcessor, ParallelProcessor, 
    CustomAllocator, ZeroCopyInterface, DirectMemoryAccess
)

from expert.simulation import (
    CustomPlasticityRules, AdvancedNeuromodulation, 
    CustomConnectivityPatterns, NeuralPopulationManager
)

from expert.analysis import (
    NeuralSignalProcessor, PatternRecognizer, StatisticalAnalyzer,
    MLIntegration, SignalFilter, FeatureExtractor
)

from expert.architecture import (
    CustomNeuronModel, SynapticDynamics, 
    RegionConfiguration, NetworkTopology
)

from expert.configuration import (
    DynamicConfig, ConfigInheritance, ConfigValidator,
    ConfigOptimizer, ConfigSnapshot
)

from expert.experiment import (
    CustomExperimentLoop, ParallelExperimentRunner,
    CheckpointManager, ResultAnalyzer, ExperimentMonitor
)

from expert.integration import (
    CXXExtensionAPI, CustomIOFormat, SensorIntegration,
    WebSocketAPI, ExternalResourceManager
)

from expert.visualization import (
    AdvancedPlotting, RealTimeVisualizer, 
    VisualizationPipeline, DataExporter
)

from expert.utils import (
    MemoryProfiler, PerformanceProfiler, 
    BenchmarkRunner, ProfilingSession
)

# Main expert module exports
__all__ = [
    # Performance APIs
    'MemoryPool', 'SIMDProcessor', 'ParallelProcessor', 
    'CustomAllocator', 'ZeroCopyInterface', 'DirectMemoryAccess',
    
    # Advanced Simulation Features
    'CustomPlasticityRules', 'AdvancedNeuromodulation',
    'CustomConnectivityPatterns', 'NeuralPopulationManager',
    
    # Data Analysis APIs
    'NeuralSignalProcessor', 'PatternRecognizer', 'StatisticalAnalyzer',
    'MLIntegration', 'SignalFilter', 'FeatureExtractor',
    
    # Custom Brain Architecture APIs
    'CustomNeuronModel', 'SynapticDynamics', 
    'RegionConfiguration', 'NetworkTopology',
    
    # Advanced Configuration Systems
    'DynamicConfig', 'ConfigInheritance', 'ConfigValidator',
    'ConfigOptimizer', 'ConfigSnapshot',
    
    # Experiment Control APIs
    'CustomExperimentLoop', 'ParallelExperimentRunner',
    'CheckpointManager', 'ResultAnalyzer', 'ExperimentMonitor',
    
    # Integration APIs
    'CXXExtensionAPI', 'CustomIOFormat', 'SensorIntegration',
    'WebSocketAPI', 'ExternalResourceManager',
    
    # Enhanced Visualization APIs
    'AdvancedPlotting', 'RealTimeVisualizer', 
    'VisualizationPipeline', 'DataExporter',
    
    # Expert utility functions
    'MemoryProfiler', 'PerformanceProfiler', 
    'BenchmarkRunner', 'ProfilingSession',
]

# Expert examples
expert_examples = """Expert NLM Usage Examples:

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
