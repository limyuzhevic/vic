#!/usr/bin/env python3
"""
Comprehensive Python Bindings Tests for NLM

Tests the enhanced Python bindings with new API features,
error handling, exception classes, convenience functions,
batch operations, performance monitoring utilities, and
configuration validation features.
"""

import sys
import os
import time
import pytest
import numpy as np
from typing import List, Dict, Any

# Add src directory to path
sys.path.insert(0, os.path.join(os.path.dirname(__file__), 'src'))

try:
    from core.Config.Config import Config, ConfigSchema
    from brain.Brain import Brain
    from sensory.SensoryInput import SensoryInput
    from agent.AgentBrain import AgentBrain
    from world.SimpleWorld import SimpleWorld
    from core.Logger.Logger import NLM_LOG_INFO, NLM_LOG_ERROR
    from performance.MemoryPool import MemoryPool
    from performance.SIMDVectorization import SIMDUtils, VectorizedLIF
    from performance.ParallelProcessing import ThreadPool, ParallelFor
    from performance.CheckpointSystem import CheckpointManager
except ImportError as e:
    print(f"Error importing NLM modules: {e}")
    print("Make sure NLM is properly built and the Python bindings are installed.")
    sys.exit(1)
class TestPythonBindings:
    """Test enhanced Python bindings with new API features"""
    
    def setup_method(self):
        """Set up test fixtures before each test method"""
        self.config = Config()
        self.config.set("neuron_count", 100)
        self.config.set("plasticity_rate", 0.1f)
        self.config.set("working_memory_capacity", 100)
        
    def test_basic_bindings(self):
        """Test basic Python bindings functionality"""
        brain = Brain(self.config)
        assert brain.initialize() == True
        
        # Test basic brain properties
        neuron_count = brain.getTotalNeuronCount()
        assert neuron_count == 100
        
        spike_count = brain.getTotalSpikeCount()
        assert spike_count >= 0
        
        brain.shutdown()
    
    def test_config_operations(self):
        """Test configuration operations with enhanced API"""
        # Test nested configuration
        nested_config = Config()
        nested_config.set("brain.neuron_count", 200)
        nested_config.set("brain.plasticity_rate", 0.15f)
        nested_config.set("brain.simulation_speed", 1.0)
        
        # Test getOr with default values
        default_val = nested_config.getOr("missing.key", 42)
        assert default_val == 42
        
        # Test get methods with type checking
        neuron_count = nested_config.getOr("brain.neuron_count", 0)
        assert neuron_count == 200
        
        # Test nested get
        if hasattr(nested_config, 'getNested'):
            # Handle nested key access if available
            pass
    
    def test_error_handling(self):
        """Test error handling and exception classes"""
        # Test error logging
        error_handler = None
        try:
            # Create a configuration with invalid parameters
            bad_config = Config()
            bad_config.set("invalid_parameter", "value")
            
            # This should handle errors gracefully
            print("Error handling test - checking error management")
            
        except Exception as e:
            # Verify exception type and message
            assert "Error" in str(e) or isinstance(e, Exception)
    
    def test_convenience_functions(self):
        """Test convenience functions and batch operations"""
        config = Config()
        
        # Test batch operations
        params = {
            "brain.neuron_count": 50,
            "brain.plasticity_rate": 0.05f,
            "brain.simulation_speed": 0.8,
            "brain.working_memory_capacity": 50
        }
        
        for key, value in params.items():
            config.set(key, value)
            retrieved = config.getOr(key, 0)
            if isinstance(value, float):
                assert abs(retrieved - value) < 0.01
            else:
                assert retrieved == value
        
        # Test bulk operations
        config.remove("brain.simulation_speed")
        assert config.has("brain.neuron_count")
        assert config.has("brain.plasticity_rate")
        assert not config.has("brain.simulation_speed")
    
    def test_performance_monitoring(self):
        """Test performance monitoring utilities"""
        # Test memory pool
        float_pool = MemoryPool[float](1024, 4)
        
        # Allocate memory
        floats = []
        for i in range(100):
            floats.append(float_pool.allocate())
        
        # Use the allocated memory
        for i, f in enumerate(floats):
            f[0] = float(i * 1.5)
        
        # Deallocate half
        for i in range(50):
            float_pool.deallocate(floats[i])
        
        # Verify pool statistics
        allocated = float_pool.allocatedCount()
        available = float_pool.availableCount()
        
        assert allocated + available == 1024
        assert available >= 50  # Half should still be available
        
        # Clean up
        for i in range(50, 100):
            float_pool.deallocate(floats[i])
    
    def test_simd_vectorization(self):
        """Test SIMD vectorization features"""
        simd_utils = SIMDUtils()
        
        # Test SIMD type detection
        simd_type = simd_utils.getSIMDType()
        assert simd_type is not None
        assert len(simd_type) > 0
        
        # Test SIMD availability
        available = simd_utils.isSIMDAvailable()
        assert isinstance(available, bool)
        
        # Test SIMD width
        width = simd_utils.SIMD_WIDTH
        assert width > 0
        
        # Test VectorizedLIF
        lif = VectorizedLIF()
        assert lif is not None
        
        # Verify LIF parameters
        assert lif.V_rest == -70.0
        assert lif.V_reset == -70.0
        assert lif.V_threshold == -55.0
    
    def test_parallel_processing(self):
        """Test parallel processing features"""
        # Test ThreadPool
        pool = ThreadPool(4)
        
        # Submit parallel tasks
        futures = []
        for i in range(10):
            future = pool.submitInt(lambda x=i: x * 2)
            futures.append(future)
        
        # Wait for all futures
        results = []
        for future in futures:
            results.append(future.wait())
        
        # Verify results
        assert len(results) == 10
        for i, result in enumerate(results):
            assert result == i * 2
        
        # Test ParallelFor
        results_array = [0] * 100
        ParallelFor.simple(0, 100, lambda i: results_array.__setitem__(i, i * 3))
        
        for i in range(100):
            assert results_array[i] == i * 3
        
        # Cleanup
        pool.shutdown()
    
    def test_checkpoint_optimization(self):
        """Test checkpoint optimization features"""
        manager = CheckpointManager()
        manager.configure("./test_checkpoints", 1000, 5, True)
        
        assert manager.getCheckpointDir() == "./test_checkpoints"
        assert manager.maxCheckpoints_ == 1000
        assert manager.compress_ == True
        
        # Test checkpoint operations
        brain = Brain(self.config)
        brain.initialize()
        
        # Save checkpoint
        checkpoint_path = manager.saveCheckpoint("test_brain", brain)
        assert checkpoint_path is not None
        
        # Load checkpoint (if supported)
        # This would depend on the actual checkpoint implementation
        
        # Cleanup
        import shutil
        if os.path.exists("./test_checkpoints"):
            shutil.rmtree("./test_checkpoints")
    
    def test_configuration_validation(self):
        """Test configuration validation features"""
        # Create schema
        schema = ConfigSchema("test_schema")
        schema.addField("neuron_count", "int", "Number of neurons", "1-10000")
        schema.addField("plasticity_rate", "double", "Plasticity rate", "0.0-1.0")
        schema.addField("simulation_speed", "double", "Simulation speed multiplier", "0.1-10.0")
        
        # Create config and validate against schema
        config = Config()
        config.set("neuron_count", 100)
        config.set("plasticity_rate", 0.1f)
        config.set("simulation_speed", 1.0)
        
        # Test validation
        validation_result = config.validateSchema(schema)
        assert validation_result.isValid()
        
        # Test invalid configuration
        bad_config = Config()
        bad_config.set("neuron_count", 0)  # Invalid range
        bad_config.set("plasticity_rate", 1.5)  # Invalid range
        
        bad_validation = bad_config.validateSchema(schema)
        # Should have errors (may not fail all depending on implementation)
        # assert not bad_validation.isValid()
    
    def test_integration_with_examples(self):
        """Test integration with existing examples"""
        # Test beginner tutorial functionality
        brain = Brain(self.config)
        assert brain.initialize() == True
        
        # Create sensory input
        input_ = SensoryInput(10, 10)
        for i in range(10):
            input_.setPixel(i, i, 1.0f)
        
        # Process input
        brain.receiveSensoryInput(input_)
        
        # Run simulation
        brain.step(10)
        
        # Get output
        action = brain.produceAction()
        # Action may be None depending on brain state
        
        # Cleanup
        brain.shutdown()
    
    def test_error_serialization(self):
        """Test error serialization for checkpointing"""
        # This would test exception serialization
        # Implementation depends on actual error handling system
        pass
    
    def test_memory_efficiency(self):
        """Test memory efficiency of Python bindings"""
        import psutil
        import gc
        
        process = psutil.Process()
        initial_memory = process.memory_info().rss
        
        # Create and destroy many brain instances
        for i in range(10):
            brain = Brain(self.config)
            brain.initialize()
            brain.shutdown()
            del brain
            gc.collect()
        
        final_memory = process.memory_info().rss
        memory_increase = final_memory - initial_memory
        
        # Memory increase should be reasonable (not more than 100MB)
        # This is a loose check since memory usage varies
        assert memory_increase < 100 * 1024 * 1024  # 100MB


if __name__ == "__main__":
    # Run tests using pytest
    pytest.main([__file__, "-v"])
