"""
Direct test of expert module components
This script tests the expert modules without importing through the expert.py wrapper
"""

import sys
import os

# Add expert directory to path
sys.path.insert(0, os.path.join(os.path.dirname(__file__), 'expert'))

print("Testing Expert NLM Components...")

# Test performance module
try:
    from performance import MemoryPool, SIMDProcessor, ParallelProcessor
    print("✓ Performance module imported successfully")
    
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
    
except Exception as e:
    print(f"✗ Performance module test failed: {e}")

# Test simulation module
try:
    from simulation import CustomPlasticityRules, AdvancedNeuromodulation
    print("✓ Simulation module imported successfully")
    
    # Test CustomPlasticityRules
    plasticity_rules = CustomPlasticityRules("stdp")
    new_weight = plasticity_rules.apply_plasticity(1.0, 2.0, 0.5)
    print(f"✓ CustomPlasticityRules new weight: {new_weight}")
    
    # Test AdvancedNeuromodulation
    neuromodulation = AdvancedNeuromodulation({
        'dopamine_strength': 0.8,
        'serotonin_influence': 0.6
    })
    print(f"✓ AdvancedNeuromodulation initialized")
    
except Exception as e:
    print(f"✗ Simulation module test failed: {e}")

# Test configuration module
try:
    from configuration import DynamicConfig
    print("✓ Configuration module imported successfully")
    
    # Test DynamicConfig
    config = DynamicConfig({"test": "value"})
    config.set("brain.simulated_neurons", 1000)
    value = config.get("brain.simulated_neurons")
    print(f"✓ DynamicConfig value: {value}")
    
except Exception as e:
    print(f"✗ Configuration module test failed: {e}")

# Test experiment module
try:
    from experiment import CustomExperimentLoop, CheckpointManager
    print("✓ Experiment module imported successfully")
    
    # Test CustomExperimentLoop
    experiment = CustomExperimentLoop(10, 0.001)
    experiment.add_loop_function(lambda state, iteration, dt: state)
    result = experiment.run()
    print(f"✓ CustomExperimentLoop completed: {result.get('status', 'success')}")
    
    # Test CheckpointManager
    checkpoint_manager = CheckpointManager()
    checkpoint_name = checkpoint_manager.create_checkpoint("test_checkpoint", {"test": "data"})
    print(f"✓ CheckpointManager created checkpoint: {checkpoint_name}")
    
except Exception as e:
    print(f"✗ Experiment module test failed: {e}")

# Test integration module
try:
    from integration import WebSocketAPI, CustomIOFormat
    print("✓ Integration module imported successfully")
    
    # Test WebSocketAPI
    ws_api = WebSocketAPI("localhost", 8080)
    print(f"✓ WebSocketAPI initialized")
    
    # Test CustomIOFormat
    io_api = CustomIOFormat("hdf5")
    print(f"✓ CustomIOFormat initialized")
    
except Exception as e:
    print(f"✗ Integration module test failed: {e}")

# Test visualization module
try:
    from visualization import AdvancedPlotting
    print("✓ Visualization module imported successfully")
    
    # Test AdvancedPlotting
    plotting = AdvancedPlotting()
    print(f"✓ AdvancedPlotting initialized")
    
except Exception as e:
    print(f"✗ Visualization module test failed: {e}")

# Test utils module
try:
    from utils import MemoryProfiler
    print("✓ Utils module imported successfully")
    
    # Test MemoryProfiler
    profiler = MemoryProfiler()
    profiler.start_profiling(1.0)
    profiler.stop_profiling()
    print(f"✓ MemoryProfiler completed")
    
except Exception as e:
    print(f"✗ Utils module test failed: {e}")

print("\n✅ Expert module testing completed!")
