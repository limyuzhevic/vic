import pytest
import pynlm
import time
import statistics
import psutil
import os

class TestPerformance:
    """Test performance characteristics and benchmarks"""

    def test_step_performance(self, small_brain):
        """Test brain stepping performance"""
        # Measure step times
        step_times = []
        
        for step in range(100):
            start_time = time.time()
            small_brain.step(step)
            end_time = time.time()
            
            step_time = end_time - start_time
            step_times.append(step_time)
        
        # Performance metrics
        avg_step_time = statistics.mean(step_times)
        min_step_time = min(step_times)
        max_step_time = max(step_times)
        std_step_time = statistics.stdev(step_times)
        
        # Performance requirements (adjustable)
        assert avg_step_time < 0.1, f"Average step time too high: {avg_step_time}s"
        assert max_step_time < 1.0, f"Maximum step time too high: {max_step_time}s"
        assert std_step_time < 0.05, f"Step time variability too high: {std_step_time}s"
        
        print(f"Step performance: avg={avg_step_time:.4f}s, std={std_step_time:.4f}s")

    def test_memory_usage(self, medium_brain):
        """Test memory usage patterns"""
        process = psutil.Process(os.getpid())
        initial_memory = process.memory_info().rss / 1024 / 1024  # MB
        
        # Run memory-intensive operations
        for step in range(500):
            medium_brain.step(step)
            
            # Force memory operations if available
            if medium_brain.getWorkingMemory():
                medium_brain.getWorkingMemory().storeToNeuron(1, 0.5)
        
        final_memory = process.memory_info().rss / 1024 / 1024  # MB
        memory_increase = final_memory - initial_memory
        
        # Memory increase should be reasonable (adjust based on system)
        assert memory_increase < 100, f"Memory increase too high: {memory_increase}MB"
        
        print(f"Memory usage: increase={memory_increase:.2f}MB")

    def test_simulation_speed(self, test_world, test_agent):
        """Test overall simulation speed"""
        step_times = []
        
        for step in range(200):
            start_time = time.time()
            
            test_world.update(0.1)
            test_agent.processSensoryInput(test_world.getSensoryPercept())
            
            # Access brain if available
            if hasattr(test_agent, 'getBrain'):
                brain = test_agent.getBrain()
                brain.step(step)
            
            end_time = time.time()
            step_time = end_time - start_time
            step_times.append(step_time)
        
        # Calculate simulation throughput
        avg_step_time = statistics.mean(step_times)
        steps_per_second = 1.0 / avg_step_time
        
        # Performance targets (adjustable)
        assert steps_per_second > 10.0, f"Simulation too slow: {steps_per_second} steps/sec"
        
        print(f"Simulation speed: {steps_per_second:.1f} steps/sec")

    def test_scalability(self):
        """Test scalability with different brain sizes"""
        sizes = [100, 500, 1000, 2000]
        performance_results = []
        
        for size in sizes:
            config = pynlm.createDefaultConfig()
            config.set("brain.neuron_count", size)
            config.set("brain.region_count", max(1, size // 500))
            
            brain = pynlm.createBrain(config)
            brain.initialize()
            
            # Measure performance
            step_times = []
            for step in range(50):
                start_time = time.time()
                brain.step(step)
                end_time = time.time()
                step_times.append(end_time - start_time)
            
            avg_step_time = statistics.mean(step_times)
            performance_results.append((size, avg_step_time))
            
            print(f"Size {size}: {avg_step_time:.4f}s per step")
        
        # Check that performance scales reasonably
        # (Larger brains should have proportionally higher step times)
        for i in range(1, len(performance_results)):
            prev_size, prev_time = performance_results[i-1]
            curr_size, curr_time = performance_results[i]
            
            # Time should scale roughly linearly with size
            size_ratio = curr_size / prev_size
            time_ratio = curr_time / prev_time
            
            # Allow for some overhead, but not excessive
            assert time_ratio < size_ratio * 2.0, f"Performance scaling too poor: size ratio {size_ratio}, time ratio {time_ratio}"