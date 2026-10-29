"""
Performance NLM Python tests.

Tests for performance benchmarking and optimization.
"""

import pytest
import time
import pynlm


class TestPerformanceBenchmarks:
    """Test performance benchmarks."""
    
    def test_brain_creation_performance(self):
        """Test brain creation performance."""
        times = []
        
        # Test brain creation performance
        for i in range(10):
            start_time = time.time()
            
            config = pynlm.createDefaultConfig()
            brain = pynlm.createBrain(config)
            brain.initialize()
            
            end_time = time.time()
            times.append(end_time - start_time)
        
        # Calculate average time
        avg_time = sum(times) / len(times)
        max_time = max(times)
        
        # Should be reasonably fast (less than 1 second per brain creation)
        assert avg_time < 1.0
        assert max_time < 2.0
        
        # Report performance
        print(f"Brain creation average: {avg_time:.4f}s")
        print(f"Brain creation max: {max_time:.4f}s")
    
    def test_brain_simulation_performance(self):
        """Test brain simulation performance."""
        # Create brain
        config, brain, world, agent = pynlm.createCompleteSimulation()
        world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
        
        # Test simulation performance
        steps = 100
        times = []
        
        for i in range(5):
            start_time = time.time()
            
            for step in range(steps):
                world.update(0.1)
                agent.processSensoryInput(world.getSensoryPercept())
                brain.step(step)
                action = agent.decodeMotorCommand()
                world.applyMotorCommand(action, world.getSimulationTime())
                agent.applyRewardModulation(0.0, 0.0)
                agent.updateDevelopment(0.1)
            
            end_time = time.time()
            times.append(end_time - start_time)
        
        # Calculate average time
        avg_time = sum(times) / len(times)
        steps_per_second = steps / avg_time
        
        # Should be reasonably fast
        assert avg_time < 5.0
        assert steps_per_second > 10.0  # At least 10 steps per second
        
        # Report performance
        print(f"Simulation average: {avg_time:.4f}s ({steps_per_second:.1f} steps/sec)")
        print(f"Simulation max: {max(times):.4f}s")
    
    def test_config_operations_performance(self):
        """Test configuration operations performance."""
        config = pynlm.createDefaultConfig()
        
        # Test setting many configuration values
        start_time = time.time()
        
        for i in range(1000):
            config.setString(f"key_{i}", f"value_{i}")
        
        end_time = time.time()
        
        # Should be fast
        assert end_time - start_time < 1.0
        
        # Test getting many configuration values
        start_time = time.time()
        
        for i in range(1000):
            _ = config.getString(f"key_{i}", "")
        
        end_time = time.time()
        
        # Should be fast
        assert end_time - start_time < 1.0
        
        # Report performance
        print(f"Set 1000 config values: {end_time - start_time:.4f}s")
        print(f"Get 1000 config values: {end_time - start_time:.4f}s")
    
    def test_brain_step_performance(self):
        """Test brain step performance."""
        config, brain, world, agent = pynlm.createCompleteSimulation()
        world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
        
        # Test single step performance
        times = []
        
        for i in range(100):
            start_time = time.time()
            brain.step(i)
            end_time = time.time()
            
            times.append(end_time - start_time)
        
        # Calculate average time
        avg_time = sum(times) / len(times)
        steps_per_second = 1.0 / avg_time
        
        # Should be very fast
        assert avg_time < 0.01
        assert steps_per_second > 100.0  # At least 100 steps per second
        
        # Report performance
        print(f"Brain step average: {avg_time:.6f}s ({steps_per_second:.1f} steps/sec)")
        print(f"Brain step max: {max(times):.6f}s")


class TestMemoryUsageBenchmarks:
    """Test memory usage benchmarks."""
    
    def test_brain_memory_usage(self):
        """Test brain memory usage."""
        import sys
        
        # Get initial memory usage (approximate)
        initial_objects = len(sys.getrefcount(None))
        
        # Create multiple brains
        brains = []
        for i in range(10):
            config, brain, world, agent = pynlm.createCompleteSimulation()
            brains.append(brain)
        
        # Get memory usage after creating brains
        final_objects = len(sys.getrefcount(None))
        
        # Memory usage should be reasonable
        object_growth = final_objects - initial_objects
        assert object_growth < 1000  # Should not grow excessively
        
        # Verify brains are still functional
        for brain in brains:
            assert brain.getTotalNeuronCount() > 0
            assert brain.getTotalSynapseCount() > 0
        
        print(f"Created {len(brains)} brains with {object_growth} object growth")
    
    def test_config_memory_usage(self):
        """Test configuration memory usage."""
        import sys
        
        config = pynlm.createDefaultConfig()
        
        # Get initial memory usage
        initial_objects = len(sys.getrefcount(None))
        
        # Set many configuration values
        for i in range(1000):
            config.setString(f"key_{i}", f"value_{i}" * 10)  # Larger strings
        
        # Get memory usage after setting values
        final_objects = len(sys.getrefcount(None))
        
        # Memory usage should be reasonable
        object_growth = final_objects - initial_objects
        assert object_growth < 500  # Should not grow excessively
        
        # Verify configuration is still functional
        assert config.getString("key_0", "") == "value_0" * 10
        
        print(f"Set 1000 config values with memory growth: {object_growth} objects")


class TestScalabilityTests:
    """Test scalability benchmarks."""
    
    def test_scalability_brain_size(self):
        """Test scalability with different brain sizes."""
        brain_sizes = [100, 500, 1000, 2000]
        
        for size in brain_sizes:
            # Create brain with specific size
            config = pynlm.createDefaultConfig()
            config.createBrainConfig(neuronCount=size)
            
            brain = pynlm.createBrain(config)
            brain.initialize()
            
            # Verify size
            assert brain.getTotalNeuronCount() == size
            assert brain.getTotalSynapseCount() > 0
            
            # Run simulation
            for i in range(min(20, size // 100)):
                brain.step(i)
            
            print(f"Brain size {size}: {brain.getTotalSynapseCount()} synapses")
    
    def test_scalability_simulation_steps(self):
        """Test scalability with different simulation lengths."""
        steps_list = [10, 50, 100, 500]
        
        for steps in steps_list:
            config, brain, world, agent = pynlm.createCompleteSimulation()
            world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
            
            # Run simulation
            start_time = time.time()
            for i in range(steps):
                world.update(0.1)
                agent.processSensoryInput(world.getSensoryPercept())
                brain.step(i)
                action = agent.decodeMotorCommand()
                world.applyMotorCommand(action, world.getSimulationTime())
            
            end_time = time.time()
            duration = end_time - start_time
            steps_per_second = steps / duration
            
            # Verify performance
            assert steps_per_second > 5.0  # At least 5 steps per second
            
            print(f"Steps {steps}: {duration:.4f}s ({steps_per_second:.1f} steps/sec)")
    
    def test_scalability_complexity(self):
        """Test scalability with increasing complexity."""
        # Test with increasing brain sizes
        sizes = [100, 500, 1000]
        times = []
        
        for size in sizes:
            config = pynlm.createDefaultConfig()
            config.createBrainConfig(neuronCount=size)
            
            brain = pynlm.createBrain(config)
            brain.initialize()
            
            # Measure time for 10 steps
            start_time = time.time()
            for i in range(10):
                brain.step(i)
            end_time = time.time()
            
            times.append(end_time - start_time)
            print(f"Size {size}: {times[-1]:.4f}s for 10 steps")
        
        # Check that time scales reasonably (should not increase exponentially)
        # For doubling size, time should increase by less than 4x (sub-quadratic)
        if len(sizes) >= 3:
            ratio_100_500 = times[1] / times[0]
            ratio_500_1000 = times[2] / times[1]
            
            # Time should scale roughly linearly or sub-quadratically
            assert ratio_100_500 < 10.0  # Less than 10x increase
            assert ratio_500_1000 < 10.0  # Less than 10x increase


class TestBenchmarkSuite:
    """Test complete benchmark suite."""
    
    def test_complete_benchmark_suite(self):
        """Test complete benchmark suite."""
        # Run all benchmarks
        benchmark_results = {}
        
        # Brain creation benchmark
        start_time = time.time()
        for i in range(5):
            config, brain, world, agent = pynlm.createCompleteSimulation()
        benchmark_results["brain_creation"] = time.time() - start_time
        
        # Simulation benchmark
        config, brain, world, agent = pynlm.createCompleteSimulation()
        world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
        
        start_time = time.time()
        for i in range(50):
            world.update(0.1)
            agent.processSensoryInput(world.getSensoryPercept())
            brain.step(i)
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
        benchmark_results["simulation"] = time.time() - start_time
        
        # Configuration benchmark
        config = pynlm.createDefaultConfig()
        start_time = time.time()
        for i in range(1000):
            config.setString(f"key_{i}", f"value_{i}")
        benchmark_results["configuration"] = time.time() - start_time
        
        # Verify benchmark results are reasonable
        assert benchmark_results["brain_creation"] < 5.0
        assert benchmark_results["simulation"] < 10.0
        assert benchmark_results["configuration"] < 2.0
        
        print("Benchmark Results:")
        for benchmark, duration in benchmark_results.items():
            print(f"  {benchmark}: {duration:.4f}s")
    
    def test_performance_target_achievement(self):
        """Test that performance targets are achieved."""
        # Create brain
        config, brain, world, agent = pynlm.createCompleteSimulation()
        world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
        
        # Target performance
        target_steps_per_second = 50.0
        
        # Measure performance
        steps = 100
        start_time = time.time()
        for i in range(steps):
            world.update(0.1)
            agent.processSensoryInput(world.getSensoryPercept())
            brain.step(i)
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
        end_time = time.time()
        
        actual_steps_per_second = steps / (end_time - start_time)
        
        # Check that target is achieved
        assert actual_steps_per_second >= target_steps_per_second, \
            f"Performance target not met: {actual_steps_per_second:.1f} < {target_steps_per_second}"
        
        print(f"Performance target achieved: {actual_steps_per_second:.1f} steps/sec")


if __name__ == "__main__":
    pytest.main([__file__, "-v"])
