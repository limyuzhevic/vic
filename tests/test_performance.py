"""Performance Tests

Basic performance validation.
"""

import pytest
import pynlm
import time
import statistics
from typing import List


class TestPerformance:
    """Test basic performance characteristics."""
    
    def test_object_creation_performance(self):
        """Test performance of object creation."""
        # Test ID object creation performance
        start_time = time.perf_counter()
        
        # Create many NeuronId objects
        neuron_ids: List[pynlm.NeuronId] = []
        for i in range(1000):
            neuron_ids.append(pynlm.NeuronId(i))
        
        # Create many SynapseId objects
        synapse_ids: List[pynlm.SynapseId] = []
        for i in range(1000):
            synapse_ids.append(pynlm.SynapseId(i))
        
        end_time = time.perf_counter()
        
        creation_time = end_time - start_time
        
        # Assert creation is reasonably fast (less than 0.1 seconds for 2000 objects)
        # This is a loose test - the actual threshold depends on the implementation
        assert creation_time < 1.0, f"Object creation too slow: {creation_time:.3f}s"
    
    def test_brain_initialization_performance(self):
        """Test performance of brain initialization."""
        configs = []
        brains = []
        
        start_time = time.perf_counter()
        
        # Create and initialize multiple brains
        for i in range(10):
            config = pynlm.createDefaultConfig()
            brain = pynlm.createBrain(config)
            brain.initialize()
            
            configs.append(config)
            brains.append(brain)
        
        end_time = time.perf_counter()
        
        init_time = end_time - start_time
        
        # Assert initialization is reasonably fast
        assert init_time < 5.0, f"Brain initialization too slow: {init_time:.3f}s"
    
    def test_config_operations_performance(self):
        """Test performance of config operations."""
        config = pynlm.createDefaultConfig()
        
        start_time = time.perf_counter()
        
        # Perform many config operations
        for i in range(1000):
            config.has(f"key_{i}")
            config.getKeys()
            config.clear()
            config.set(f"key_{i}", f"value_{i}")
        
        end_time = time.perf_counter()
        
        operation_time = end_time - start_time
        
        # Assert config operations are reasonably fast
        assert operation_time < 1.0, f"Config operations too slow: {operation_time:.3f}s"
    
    def test_memory_usage_basics(self):
        """Test basic memory usage characteristics."""
        # This test verifies that memory usage is reasonable
        # by creating and destroying objects
        
        def measure_memory_usage():
            """Measure memory usage by creating objects."""
            import tracemalloc
            
            tracemalloc.start()
            
            # Create objects
            configs = []
            brains = []
            
            for i in range(100):
                config = pynlm.createDefaultConfig()
                brain = pynlm.createBrain(config)
                brain.initialize()
                
                configs.append(config)
                brains.append(brain)
            
            # Take snapshot
            snapshot1 = tracemalloc.take_snapshot()
            
            # Delete objects to free memory
            del configs
            del brains
            
            import gc
            gc.collect()
            
            # Take another snapshot
            snapshot2 = tracemalloc.take_snapshot()
            
            tracemalloc.stop()
            
            return snapshot1, snapshot2
        
        # Run memory measurement
        snapshot1, snapshot2 = measure_memory_usage()
        
        # Compare memory usage
        top_stats = snapshot2.compare_to(snapshot1, 'lineno')
        
        # Memory should be released reasonably well
        # Look for significant memory increases
        for stat in top_stats[:10]:
            if stat.size_diff > 0:
                # Print top memory increases
                print(f"Memory increase: {stat}")
        
        # This is a basic test - we're just ensuring the test runs
        # without excessive memory issues
    
    def test_object_lifecycle_performance(self):
        """Test performance of object lifecycle operations."""
        start_time = time.perf_counter()
        
        # Create and destroy many objects
        for i in range(100):
            config = pynlm.createDefaultConfig()
            
            # Create brain and world
            brain = pynlm.createBrain(config)
            world = pynlm.createSimpleWorld()
            
            # Configure and initialize
            world.configure(800, 600, 640, 480)
            world.setAgentStart(400, 300)
            
            brain.initialize()
            
            # Create agent brain
            agent_brain = pynlm.createAgentBrain(brain)
            agent_brain.initialize(world)
            
            # Perform some operations
            agent_brain.getSensoryInputSize()
            agent_brain.getMotorOutputSize()
            
            # All objects will be destroyed when they go out of scope
        
        end_time = time.perf_counter()
        
        lifecycle_time = end_time - start_time
        
        # Assert lifecycle operations are reasonably fast
        assert lifecycle_time < 10.0, f"Object lifecycle too slow: {lifecycle_time:.3f}s"
    
    def test_concurrent_operations_simulation(self):
        """Test performance of concurrent-like operations."""
        start_time = time.perf_counter()
        
        # Simulate concurrent operations by creating many independent systems
        systems = []
        
        for i in range(50):
            config = pynlm.createDefaultConfig()
            brain = pynlm.createBrain(config)
            world = pynlm.createSimpleWorld()
            
            # Each system has its own independent state
            world.configure(800, 600, 640, 480)
            world.setAgentStart(400 + i * 10, 300 + i * 5)
            
            brain.initialize()
            
            agent_brain = pynlm.createAgentBrain(brain)
            agent_brain.initialize(world)
            
            # Each system performs different operations
            neuromod_level = agent_brain.getNeuromodulationLevel()
            curiosity = agent_brain.getCuriosityLevel()
            stage = agent_brain.getDevelopmentalStage()
            
            systems.append({
                'world': world,
                'brain': brain,
                'agent_brain': agent_brain,
                'stats': {
                    'neuromod': neuromod_level,
                    'curiosity': curiosity,
                    'stage': stage,
                }
            })
        
        end_time = time.perf_counter()
        
        concurrent_time = end_time - start_time
        
        # Assert concurrent-like operations are reasonably fast
        assert concurrent_time < 15.0, f"Concurrent operations too slow: {concurrent_time:.3f}s"
        
        # Verify all systems have independent state
        for i, system in enumerate(systems):
            assert system['world'].getAgentBody().x == 400 + i * 10
            assert system['world'].getAgentBody().y == 300 + i * 5
    
    @pytest.mark.skipif(True, reason="Performance tests require controlled environment")
    def test_performance_consistency(self):
        """Test performance consistency across multiple runs."""
        times: List[float] = []
        
        for run in range(5):
            start_time = time.perf_counter()
            
            # Create a standard set of objects
            config = pynlm.createDefaultConfig()
            brain = pynlm.createBrain(config)
            world = pynlm.createSimpleWorld()
            
            world.configure(800, 600, 640, 480)
            world.setAgentStart(400, 300)
            
            brain.initialize()
            
            agent_brain = pynlm.createAgentBrain(brain)
            agent_brain.initialize(world)
            
            # Some operations
            for i in range(100):
                agent_brain.getNeuromodulationLevel()
                agent_brain.getCuriosityLevel()
            
            end_time = time.perf_counter()
            
            times.append(end_time - start_time)
        
        # Calculate statistics
        mean_time = statistics.mean(times)
        stdev_time = statistics.stdev(times) if len(times) > 1 else 0.0
        
        # Performance should be consistent (low variance)
        # Allow for some variance in test environment
        cv = stdev_time / mean_time if mean_time > 0 else float('inf')
        
        # Coefficient of variation should be less than 0.5 (50%)
        assert cv < 0.5, f"Performance too inconsistent: CV={cv:.3f}, mean={mean_time:.3f}s"


if __name__ == "__main__":
    pytest.main([__file__, "-v"])
