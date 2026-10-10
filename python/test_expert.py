"""Simple test of expert module without numpy dependency"""

import sys
import os

# Add expert directory to path
sys.path.insert(0, os.path.join(os.path.dirname(__file__), 'expert'))

# Import modules that don't require numpy
from performance import (
    MemoryPool, SIMDProcessor, ParallelProcessor, 
    CustomAllocator, ZeroCopyInterface, DirectMemoryAccess
)

from simulation import (
    CustomPlasticityRules, AdvancedNeuromodulation, 
    CustomConnectivityPatterns, NeuralPopulationManager
)

from configuration import (
    DynamicConfig, ConfigInheritance, ConfigValidator,
    ConfigOptimizer, ConfigSnapshot
)

from experiment import (
    CustomExperimentLoop, ParallelExperimentRunner,
    CheckpointManager, ResultAnalyzer, ExperimentMonitor
)

from integration import (
    CXXExtensionAPI, CustomIOFormat, SensorIntegration,
    WebSocketAPI, ExternalResourceManager
)

from visualization import (
    AdvancedPlotting, RealTimeVisualizer, 
    VisualizationPipeline, DataExporter
)

from utils import (
    MemoryProfiler, PerformanceProfiler, 
    BenchmarkRunner, ProfilingSession
)

# Test basic functionality
def test_expert_functionality():
    print("Testing Expert NLM Module...")
    
    # Test MemoryPool
    pool = MemoryPool(100, 1000)
    block = pool.allocate(10)
    print(f"✓ MemoryPool allocated: {block}")
    
    # Test SIMDProcessor
    processor = SIMDProcessor()
    result = processor.process_vector([1.0, 2.0, 3.0, 4.0], lambda x: [v * 2 for v in x])
    print(f"✓ SIMDProcessor result: {result}")
    
    # Test ParallelProcessor
    parallel = ParallelProcessor(2)
    results = parallel.distribute_work(lambda chunk: [x * 2 for x in chunk], [[1, 2], [3, 4], [5, 6]])
    print(f"✓ ParallelProcessor results: {results}")
    
    # Test CustomPlasticityRules
    plasticity_rules = CustomPlasticityRules("stdp")
    new_weight = plasticity_rules.apply_plasticity(1.0, 2.0, 0.5)
    print(f"✓ CustomPlasticityRules new weight: {new_weight}")
    
    # Test DynamicConfig
    config = DynamicConfig({"test": "value"})
    config.set("brain.simulated_neurons", 1000)
    value = config.get("brain.simulated_neurons")
    print(f"✓ DynamicConfig value: {value}")
    
    # Test CustomExperimentLoop
    experiment = CustomExperimentLoop(10, 0.001)
    experiment.add_loop_function(lambda state, iteration, dt: state)
    result = experiment.run()
    print(f"✓ CustomExperimentLoop completed: {result.get('status', 'success')}")
    
    # Test CheckpointManager
    checkpoint_manager = CheckpointManager()
    checkpoint_name = checkpoint_manager.create_checkpoint("test_checkpoint", {"test": "data"})
    print(f"✓ CheckpointManager created checkpoint: {checkpoint_name}")
    
    # Test WebSocketAPI
    ws_api = WebSocketAPI("localhost", 8080)
    print(f"✓ WebSocketAPI initialized")
    
    # Test AdvancedPlotting
    plotting = AdvancedPlotting()
    print(f"✓ AdvancedPlotting initialized")
    
    # Test MemoryProfiler
    profiler = MemoryProfiler()
    profiler.start_profiling(1.0)
    profiler.stop_profiling()
    print(f"✓ MemoryProfiler completed")
    
    print("\n✅ All expert module tests passed!")
    return True

if __name__ == "__main__":
    test_expert_functionality()
