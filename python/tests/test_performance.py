"""
Comprehensive test suite for NLM Python performance tests

This module provides comprehensive tests for performance testing
in the NLM (Neural Learning Machine) Python bindings.

Performance tests measure the speed and efficiency of various NLM operations,
helping identify bottlenecks and optimize the implementation.

Tests cover:
- Performance of context manager operations
- Performance of convenience methods
- Performance of factory methods
- Performance of serialization operations
- Performance of visualization and debugging
- Performance of batch operations
- Performance of integration workflows
- Performance benchmarking
- Performance comparison between different approaches
- Performance edge cases and error conditions
- Performance under different configurations
- Performance with varying data sizes
"""

import pytest
import sys
import os
import time
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

import pynlm as nlm
from pynlm import (
    Config, Brain, AgentBrain, SimpleWorld,
    Vision, Audio, InternalSignals, Action,
    ActionType, NLMError, NLMConfigError, NLMInitializationError,
    NLMResourceError, NLMValidationError, JsonSerializer,
    BinarySerializer, IncrementalSerializer
)

# Performance test constants
PERFORMANCE_TEST_CONFIGS = [
    {"brain.neuron_count": 50, "brain.connection_probability": 0.1},
    {"brain.neuron_count": 100, "brain.connection_probability": 0.2},
    {"brain.neuron_count": 200, "brain.connection_probability": 0.3},
    {"brain.neuron_count": 500, "brain.connection_probability": 0.5},
]

PERFORMANCE_TEST_WORLD_CONFIGS = [
    {"width": 50.0, "height": 50.0, "vision_width": 10, "vision_height": 10},
    {"width": 100.0, "height": 100.0, "vision_width": 20, "vision_height": 20},
]

class TestContextManagerPerformance:
    """Test performance of context manager operations."""
    
    def test_context_creation_performance(self):
        """Test performance of context creation."""
        # Measure time to create many config contexts
        start_time = time.time()
        
        configs = []
        for i in range(100):
            config = nlm.Config()
            config.setDefaults("quick_test")
            config.updateFromDict({
                "brain.neuron_count": (i + 1) * 10,
                "agent.exploration_rate": 0.1 * (i + 1)
            })
            configs.append(config)
        
        end_time = time.time()
        elapsed = end_time - start_time
        
        # Should complete within reasonable time (e.g., 2 seconds)
        assert elapsed < 2.0, f"Context creation too slow: {elapsed:.2f}s"
        
        print(f"✓ Context creation performance test passed ({elapsed:.3f}s for 100 contexts)")
    
    def test_context_usage_performance(self):
        """Test performance of context usage."""
        # Measure time to use many contexts
        start_time = time.time()
        
        for i in range(50):
            with nlm.Config() as config:
                config.setDefaults("quick_test")
                config.updateFromDict({
                    "brain.neuron_count": (i + 1) * 20,
                    "simulation.max_steps": (i + 1) * 50
                })
                
                # Use config
                neuron_count = config.getOr("brain.neuron_count", 0)
                assert neuron_count > 0
        
        end_time = time.time()
        elapsed = end_time - start_time
        
        # Should complete within reasonable time (e.g., 2 seconds)
        assert elapsed < 2.0, f"Context usage too slow: {elapsed:.2f}s"
        
        print(f"✓ Context usage performance test passed ({elapsed:.3f}s for 50 contexts)")
    
    def test_nested_context_performance(self):
        """Test performance of nested context managers."""
        start_time = time.time()
        
        # Create deeply nested contexts
        for i in range(20):
            with nlm.Config() as config, \
                 nlm.SimpleWorld() as world, \
                 nlm.Brain(config) as brain:
                
                config.setDefaults("quick_test")
                world.configure(50.0, 50.0, 10, 10)
                brain.initialize()
        
        end_time = time.time()
        elapsed = end_time - start_time
        
        # Should complete within reasonable time (e.g., 5 seconds)
        assert elapsed < 5.0, f"Nested context performance too slow: {elapsed:.2f}s"
        
        print(f"✓ Nested context performance test passed ({elapsed:.3f}s for 20 nested contexts)")

class TestConvenienceMethodPerformance:
    """Test performance of convenience methods."""
    
    def test_convenience_method_creation_performance(self):
        """Test performance of convenience method creation."""
        start_time = time.time()
        
        # Test setDefaults performance
        for i in range(100):
            config = nlm.Config()
            config.setDefaults("quick_test")
            config.updateFromDict({"test": i})
        
        end_time = time.time()
        elapsed = end_time - start_time
        
        # Should complete within reasonable time (e.g., 2 seconds)
        assert elapsed < 2.0, f"Convenience method creation too slow: {elapsed:.2f}s"
        
        print(f"✓ Convenience method creation performance test passed ({elapsed:.3f}s for 100 operations)")
    
    def test_convenience_method_usage_performance(self):
        """Test performance of convenience method usage."""
        start_time = time.time()
        
        # Test runSimulation performance
        for i in range(20):
            config = nlm.Config()
            config.setDefaults("quick_test")
            
            with nlm.Brain(config) as brain:
                brain.initialize()
                
                # Use convenience methods
                for j in range(10):
                    brain.runSimulation(5)
                    
                    # Get neural stats
                    stats = brain.getNeuralStats()
                    assert isinstance(stats, dict)
        
        end_time = time.time()
        elapsed = end_time - start_time
        
        # Should complete within reasonable time (e.g., 5 seconds)
        assert elapsed < 5.0, f"Convenience method usage too slow: {elapsed:.2f}s"
        
        print(f"✓ Convenience method usage performance test passed ({elapsed:.3f}s for 200 operations)")

class TestFactoryMethodPerformance:
    """Test performance of factory methods."""
    
    def test_factory_method_creation_performance(self):
        """Test performance of factory method creation."""
        start_time = time.time()
        
        # Test createStandardBrain performance
        for i in range(50):
            config = nlm.Config()
            config.setDefaults("default")
            config.updateFromDict({
                "brain.neuron_count": (i + 1) * 20,
                "brain.connection_probability": 0.2
            })
            
            brain = nlm.Brain.createStandardBrain(config)
            assert brain is not None
        
        end_time = time.time()
        elapsed = end_time - start_time
        
        # Should complete within reasonable time (e.g., 2 seconds)
        assert elapsed < 2.0, f"Factory method creation too slow: {elapsed:.2f}s"
        
        print(f"✓ Factory method creation performance test passed ({elapsed:.3f}s for 50 operations)")
    
    def test_factory_method_usage_performance(self):
        """Test performance of factory method usage."""
        start_time = time.time()
        
        # Test factory methods with actual usage
        for i in range(20):
            config = nlm.Config()
            config.setDefaults("quick_test")
            
            # Create brain using factory method
            brain = nlm.Brain.createStandardBrain(config)
            assert brain is not None
            
            with brain:
                brain.initialize()
                
                # Use factory-created brain
                brain.runSimulation(10)
                
                # Get statistics
                stats = brain.getNeuralStats()
                assert isinstance(stats, dict)
        
        end_time = time.time()
        elapsed = end_time - start_time
        
        # Should complete within reasonable time (e.g., 3 seconds)
        assert elapsed < 3.0, f"Factory method usage too slow: {elapsed:.2f}s"
        
        print(f"✓ Factory method usage performance test passed ({elapsed:.3f}s for 20 operations)")

class TestSerializationPerformance:
    """Test performance of serialization operations."""
    
    def test_json_serialization_performance(self):
        """Test performance of JSON serialization."""
        import time
        
        start_time = time.time()
        
        # Create and serialize data
        for i in range(20):
            test_data = {
                "key": f"value_{i}",
                "number": i,
                "list": list(range(i + 1)),
                "dict": {"nested": f"nested_{i}"}
            }
            
            # Serialize using JSON
            JsonSerializer.save_to_file(f"perf_test_{i}.json", test_data)
            
            # Load using JSON
            loaded_data = JsonSerializer.load_from_file(f"perf_test_{i}.json")
            assert loaded_data == test_data
            
            # Clean up
            os.remove(f"perf_test_{i}.json")
        
        end_time = time.time()
        elapsed = end_time - start_time
        
        # Should complete within reasonable time (e.g., 2 seconds)
        assert elapsed < 2.0, f"JSON serialization too slow: {elapsed:.2f}s"
        
        print(f"✓ JSON serialization performance test passed ({elapsed:.3f}s for 20 operations)")
    
    def test_brain_serialization_performance(self):
        """Test performance of brain serialization."""
        import time
        
        start_time = time.time()
        
        # Create and serialize brains
        for i in range(5):
            config = nlm.Config()
            config.setDefaults("quick_test")
            
            brain = nlm.Brain.createStandardBrain(config)
            assert brain is not None
            
            with brain:
                brain.initialize()
                brain.runSimulation(10)
                
                # Serialize brain
                JsonSerializer.serialize_brain(brain, f"perf_brain_{i}.json")
                
                # Clean up
                os.remove(f"perf_brain_{i}.json")
        
        end_time = time.time()
        elapsed = end_time - start_time
        
        # Should complete within reasonable time (e.g., 3 seconds)
        assert elapsed < 3.0, f"Brain serialization too slow: {elapsed:.2f}s"
        
        print(f"✓ Brain serialization performance test passed ({elapsed:.3f}s for 5 operations)")
    
    def test_binary_serialization_performance(self):
        """Test performance of binary serialization."""
        import time
        
        start_time = time.time()
        
        # Create and serialize data using binary format
        for i in range(10):
            test_data = {
                "key": f"value_{i}",
                "number": i,
                "list": list(range(i + 1))
            }
            
            # Serialize using binary format
            BinarySerializer.save_to_file(f"perf_test_{i}.bin", test_data)
            
            # Load using binary format
            loaded_data = BinarySerializer.load_from_file(f"perf_test_{i}.bin")
            assert loaded_data == test_data
            
            # Clean up
            os.remove(f"perf_test_{i}.bin")
        
        end_time = time.time()
        elapsed = end_time - start_time
        
        # Should complete within reasonable time (e.g., 2 seconds)
        assert elapsed < 2.0, f"Binary serialization too slow: {elapsed:.2f}s"
        
        print(f"✓ Binary serialization performance test passed ({elapsed:.3f}s for 10 operations)")

class TestVisualizationPerformance:
    """Test performance of visualization operations."""
    
    def test_neural_stats_performance(self):
        """Test performance of neural stats collection."""
        import time
        
        start_time = time.time()
        
        # Collect neural stats
        for i in range(50):
            config = nlm.Config()
            config.setDefaults("quick_test")
            
            with nlm.Brain(config) as brain:
                brain.initialize()
                brain.runSimulation(5)
                
                # Get neural stats
                stats = brain.getNeuralStats()
                assert isinstance(stats, dict)
        
        end_time = time.time()
        elapsed = end_time - start_time
        
        # Should complete within reasonable time (e.g., 2 seconds)
        assert elapsed < 2.0, f"Neural stats performance too slow: {elapsed:.2f}s"
        
        print(f"✓ Neural stats performance test passed ({elapsed:.3f}s for 50 operations)")
    
    def test_performance_metrics_performance(self):
        """Test performance of performance metrics collection."""
        import time
        
        start_time = time.time()
        
        # Collect performance metrics
        for i in range(20):
            config = nlm.Config()
            config.setDefaults("default")
            
            with nlm.Brain(config) as brain:
                brain.initialize()
                
                with nlm.SimpleWorld() as world:
                    world.configure(50.0, 50.0, 10, 10)
                    
                    with nlm.AgentBrain(brain) as agent:
                        agent.initialize(world)
                        agent.enableCuriosity(True)
                        
                        # Get performance metrics
                        metrics = agent.getPerformanceMetrics()
                        assert isinstance(metrics, dict)
        
        end_time = time.time()
        elapsed = end_time - start_time
        
        # Should complete within reasonable time (e.g., 3 seconds)
        assert elapsed < 3.0, f"Performance metrics performance too slow: {elapsed:.2f}s"
        
        print(f"✓ Performance metrics performance test passed ({elapsed:.3f}s for 20 operations)")
    
    def test_observation_map_performance(self):
        """Test performance of observation map generation."""
        import time
        
        start_time = time.time()
        
        # Generate observation maps
        for i in range(10):
            with nlm.SimpleWorld() as world:
                world.createGridWorld(100.0, 100.0, 0.1)
                world.setAgentGoal(90.0, 90.0)
                
                # Add resources
                world.addResource(10.0, 10.0, 5.0)
                world.addResource(50.0, 50.0, 10.0)
                
                # Get observation map
                observation_map = world.getObservationMap()
                assert isinstance(observation_map, list)
        
        end_time = time.time()
        elapsed = end_time - start_time
        
        # Should complete within reasonable time (e.g., 2 seconds)
        assert elapsed < 2.0, f"Observation map performance too slow: {elapsed:.2f}s"
        
        print(f"✓ Observation map performance test passed ({elapsed:.3f}s for 10 operations)")

class TestBatchOperationPerformance:
    """Test performance of batch operations."""
    
    def test_batch_config_performance(self):
        """Test performance of batch config operations."""
        import time
        
        start_time = time.time()
        
        # Batch create and configure configs
        configs = []
        for i in range(50):
            config = nlm.Config()
            config.setDefaults("quick_test")
            config.updateFromDict({
                "brain.neuron_count": (i + 1) * 20,
                "agent.exploration_rate": 0.1 * (i + 1)
            })
            configs.append(config)
        
        # Use all configs
        for config in configs:
            config.getOr("brain.neuron_count", 0)
            config.getOr("agent.exploration_rate", 0.0)
        
        end_time = time.time()
        elapsed = end_time - start_time
        
        # Should complete within reasonable time (e.g., 2 seconds)
        assert elapsed < 2.0, f"Batch config performance too slow: {elapsed:.2f}s"
        
        print(f"✓ Batch config performance test passed ({elapsed:.3f}s for 100 operations)")
    
    def test_batch_brain_performance(self):
        """Test performance of batch brain operations."""
        import time
        
        start_time = time.time()
        
        # Batch create and use brains
        for i in range(10):
            config = nlm.Config()
            config.setDefaults("quick_test")
            
            brain = nlm.Brain.createStandardBrain(config)
            assert brain is not None
            
            with brain:
                brain.initialize()
                
                # Run batch simulation
                for j in range(5):
                    brain.runSimulation(10)
                    
                    stats = brain.getNeuralStats()
                    assert isinstance(stats, dict)
        
        end_time = time.time()
        elapsed = end_time - start_time
        
        # Should complete within reasonable time (e.g., 5 seconds)
        assert elapsed < 5.0, f"Batch brain performance too slow: {elapsed:.2f}s"
        
        print(f"✓ Batch brain performance test passed ({elapsed:.3f}s for 50 operations)")
    
    def test_batch_world_performance(self):
        """Test performance of batch world operations."""
        import time
        
        start_time = time.time()
        
        # Batch create and use worlds
        for i in range(10):
            with nlm.SimpleWorld() as world:
                world.configure(50.0, 50.0, 10, 10)
                world.createGridWorld(50.0, 50.0, 0.1)
                world.setAgentGoal(45.0, 45.0)
                
                # Run simulation
                for step in range(10):
                    world.update(0.1)
                    
                    # Get world state
                    percept = world.getSensoryPercept()
                    body = world.getAgentBody()
                    
                    assert body.x is not None
                    assert body.y is not None
        
        end_time = time.time()
        elapsed = end_time - start_time
        
        # Should complete within reasonable time (e.g., 5 seconds)
        assert elapsed < 5.0, f"Batch world performance too slow: {elapsed:.2f}s"
        
        print(f"✓ Batch world performance test passed ({elapsed:.3f}s for 100 operations)")

class TestIntegrationPerformance:
    """Test performance of integration workflows."""
    
    def test_complete_integration_performance(self):
        """Test performance of complete integration workflows."""
        import time
        
        start_time = time.time()
        
        # Run complete integration workflow
        for i in range(3):
            config = nlm.Config()
            config.setDefaults("development")
            
            # Create brain using factory method
            brain = nlm.Brain.createStandardBrain(config)
            assert brain is not None
            
            with brain:
                brain.initialize()
                
                # Create world
                with nlm.SimpleWorld() as world:
                    world.configure(100.0, 100.0, 20, 20)
                    world.createGridWorld(100.0, 100.0, 0.1)
                    world.setAgentGoal(90.0, 90.0)
                    
                    # Create agent
                    with nlm.AgentBrain(brain) as agent:
                        agent.initialize(world)
                        agent.enableCuriosity(True)
                        agent.enableRewardModulation(True)
                        
                        # Run simulation
                        for step in range(20):
                            world.update(0.1)
                            
                            # Get actions
                            action = agent.decodeMotorCommand()
                            
                            # Apply actions
                            result = world.applyMotorCommand(action, world.getSimulationTime())
                            
                            # Get metrics
                            metrics = agent.getPerformanceMetrics()
                            assert isinstance(metrics, dict)
        
        end_time = time.time()
        elapsed = end_time - start_time
        
        # Should complete within reasonable time (e.g., 10 seconds)
        assert elapsed < 10.0, f"Integration performance too slow: {elapsed:.2f}s"
        
        print(f"✓ Complete integration performance test passed ({elapsed:.3f}s)")

class TestBenchmarking:
    """Test benchmarking of various operations."""
    
    def test_operation_benchmarks(self):
        """Test benchmarking of various operations."""
        import time
        
        # Benchmark config operations
        start_time = time.time()
        for i in range(100):
            config = nlm.Config()
            config.setDefaults("quick_test")
        config_time = time.time() - start_time
        
        # Benchmark brain operations
        start_time = time.time()
        for i in range(20):
            config = nlm.Config()
            config.setDefaults("quick_test")
            brain = nlm.Brain.createStandardBrain(config)
            with brain:
                brain.initialize()
                brain.runSimulation(5)
        brain_time = time.time() - start_time
        
        # Benchmark world operations
        start_time = time.time()
        for i in range(20):
            with nlm.SimpleWorld() as world:
                world.configure(50.0, 50.0, 10, 10)
                world.createGridWorld(50.0, 50.0, 0.1)
        world_time = time.time() - start_time
        
        # Print benchmark results
        print(f"Benchmark Results:")
        print(f"  Config operations (100): {config_time:.3f}s ({config_time/100:.5f}s per operation)")
        print(f"  Brain operations (20): {brain_time:.3f}s ({brain_time/20:.5f}s per operation)")
        print(f"  World operations (20): {world_time:.3f}s ({world_time/20:.5f}s per operation)")
        
        # Verify benchmarks are reasonable (should complete quickly)
        assert config_time < 2.0, f"Config benchmarking too slow: {config_time:.2f}s"
        assert brain_time < 5.0, f"Brain benchmarking too slow: {brain_time:.2f}s"
        assert world_time < 3.0, f"World benchmarking too slow: {world_time:.2f}s"
        
        print("✓ Operation benchmarking test passed")

# Test classes can be run as pytest test cases
def test_all_performance():
    """Run all performance tests."""
    print("=" * 60)
    print("Running Comprehensive Performance Tests")
    print("=" * 60)
    
    # Create test suite instances
    context_tests = TestContextManagerPerformance()
    convenience_tests = TestConvenienceMethodPerformance()
    factory_tests = TestFactoryMethodPerformance()
    serialization_tests = TestSerializationPerformance()
    visualization_tests = TestVisualizationPerformance()
    batch_tests = TestBatchOperationPerformance()
    integration_tests = TestIntegrationPerformance()
    benchmark_tests = TestBenchmarking()
    
    # Test methods in logical groups
    test_groups = [
        # Context manager performance tests
        (context_tests, [
            context_tests.test_context_creation_performance,
            context_tests.test_context_usage_performance,
            context_tests.test_nested_context_performance,
        ]),
        
        # Convenience method performance tests
        (convenience_tests, [
            convenience_tests.test_convenience_method_creation_performance,
            convenience_tests.test_convenience_method_usage_performance,
        ]),
        
        # Factory method performance tests
        (factory_tests, [
            factory_tests.test_factory_method_creation_performance,
            factory_tests.test_factory_method_usage_performance,
        ]),
        
        # Serialization performance tests
        (serialization_tests, [
            serialization_tests.test_json_serialization_performance,
            serialization_tests.test_brain_serialization_performance,
            serialization_tests.test_binary_serialization_performance,
        ]),
        
        # Visualization performance tests
        (visualization_tests, [
            visualization_tests.test_neural_stats_performance,
            visualization_tests.test_performance_metrics_performance,
            visualization_tests.test_observation_map_performance,
        ]),
        
        # Batch operation performance tests
        (batch_tests, [
            batch_tests.test_batch_config_performance,
            batch_tests.test_batch_brain_performance,
            batch_tests.test_batch_world_performance,
        ]),
        
        # Integration performance tests
        (integration_tests, [
            integration_tests.test_complete_integration_performance,
        ]),
        
        # Benchmarking tests
        (benchmark_tests, [
            benchmark_tests.test_operation_benchmarks,
        ]),
    ]
    
    total_tests = 0
    passed_tests = 0
    
    for test_class, test_methods in test_groups:
        print(f"\n--- Testing {test_class.__name__} ---")
        for test_method in test_methods:
            total_tests += 1
            try:
                test_method()
                passed_tests += 1
            except Exception as e:
                print(f"❌ {test_method.__name__} failed: {e}")
                raise
    
    print("\n" + "=" * 60)
    print("Performance Test Summary")
    print("=" * 60)
    print(f"Total tests: {total_tests}")
    print(f"Passed tests: {passed_tests}")
    print(f"Failed tests: {total_tests - passed_tests}")
    print("=" * 60)
    
    if passed_tests == total_tests:
        print("✓ All performance tests passed!")
    else:
        print(f"❌ {total_tests - passed_tests} performance tests failed!")
        sys.exit(1)

if __name__ == "__main__":
    # Run all tests when script is executed directly
    test_all_performance()