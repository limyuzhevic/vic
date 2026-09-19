#!/usr/bin/env python3
"""
Performance test harness for NLM Python API
"""

import pynlm
import time
import statistics
import sys

class PerformanceTester:
    """Test performance characteristics of the NLM Python API"""
    
    def __init__(self):
        self.results = []
    
    def benchmark_brain_creation(self, iterations=100):
        """Benchmark brain creation performance"""
        times = []
        
        for _ in range(iterations):
            start = time.perf_counter()
            
            config = pynlm.createDefaultConfig()
            brain = pynlm.createBrain(config)
            brain.initialize()
            
            elapsed = time.perf_counter() - start
            times.append(elapsed)
        
        return {
            'avg_time': statistics.mean(times),
            'min_time': min(times),
            'max_time': max(times),
            'stddev': statistics.stdev(times) if len(times) > 1 else 0,
            'iterations': iterations
        }
    
    def benchmark_simulation_step(self, brain, steps=1000):
        """Benchmark simulation step performance"""
        times = []
        
        for i in range(steps):
            start = time.perf_counter()
            brain.step(i)
            elapsed = time.perf_counter() - start
            times.append(elapsed)
        
        return {
            'avg_time': statistics.mean(times),
            'min_time': min(times),
            'max_time': max(times),
            'stddev': statistics.stdev(times) if len(times) > 1 else 0,
            'steps': steps
        }
    
    def benchmark_agent_simulation(self, config, steps=500):
        """Benchmark complete agent simulation"""
        times = []
        
        for i in range(steps):
            start = time.perf_counter()
            
            brain = pynlm.createBrain(config)
            brain.initialize()
            
            agent = pynlm.createAgentBrain(brain)
            agent.initialize(None)  # No world for basic test
            
            brain.step(i)
            
            elapsed = time.perf_counter() - start
            times.append(elapsed)
        
        return {
            'avg_time': statistics.mean(times),
            'min_time': min(times),
            'max_time': max(times),
            'stddev': statistics.stdev(times) if len(times) > 1 else 0,
            'steps': steps
        }
    
    def test_memory_usage(self):
        """Test memory usage patterns"""
        import tracemalloc
        
        # Test brain creation memory
        tracemalloc.start()
        
        config = pynlm.createDefaultConfig()
        brains = []
        for _ in range(10):
            brain = pynlm.createBrain(config)
            brain.initialize()
            brains.append(brain)
        
        current, peak = tracemalloc.get_traced_memory()
        tracemalloc.stop()
        
        # Clear references and collect garbage
        import gc
        brains.clear()
        gc.collect()
        
        return {
            'current_memory_mb': current / 1024 / 1024,
            'peak_memory_mb': peak / 1024 / 1024,
            'objects_created': 10
        }
    
    def run_comprehensive_tests(self):
        """Run comprehensive performance and correctness tests"""
        print("=== NLM Python API Performance Tests ===\n")
        
        config = pynlm.createDefaultConfig()
        
        # 1. Brain creation benchmark
        print("1. Testing brain creation performance...")
        brain_creation = self.benchmark_brain_creation(50)
        print(f"   Average creation time: {brain_creation['avg_time']*1000:.2f} ms")
        print(f"   Min creation time: {brain_creation['min_time']*1000:.2f} ms")
        print(f"   Max creation time: {brain_creation['max_time']*1000:.2f} ms")
        self.results.append(('brain_creation', brain_creation))
        
        # 2. Simulation step benchmark
        print("\n2. Testing simulation step performance...")
        brain = pynlm.createBrain(config)
        brain.initialize()
        step_performance = self.benchmark_simulation_step(brain, 100)
        print(f"   Average step time: {step_performance['avg_time']*1000:.2f} ms")
        print(f"   Min step time: {step_performance['min_time']*1000:.2f} ms")
        print(f"   Max step time: {step_performance['max_time']*1000:.2f} ms")
        self.results.append(('simulation_step', step_performance))
        
        # 3. Memory usage test
        print("\n3. Testing memory usage...")
        memory_stats = self.test_memory_usage()
        print(f"   Peak memory usage: {memory_stats['peak_memory_mb']:.2f} MB")
        self.results.append(('memory_usage', memory_stats))
        
        # 4. Correctness test
        print("\n4. Testing API correctness...")
        self.test_api_correctness()
        
        # 5. Stress test
        print("\n5. Running stress tests...")
        self.run_stress_tests(config)
        
        return self.results
    
    def test_api_correctness(self):
        """Test that the API behaves as expected"""
        print("   Testing brain basic functionality...")
        
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Check basic properties
        assert brain.getTotalNeuronCount() > 0, "Should have neurons"
        assert brain.getTotalSynapseCount() >= 0, "Should have synapses"
        
        # Test stepping
        initial_spikes = brain.getTotalSpikeCount()
        brain.step(0)
        new_spikes = brain.getTotalSpikeCount()
        
        print(f"   Initial spikes: {initial_spikes}, After step: {new_spikes}")
        
        # Test agent creation
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(None)
        
        assert agent.getBrain() is brain, "Agent should reference brain"
        print("   ✓ Basic functionality test passed")
    
    def run_stress_tests(self, config):
        """Run stress tests with many objects"""
        print("   Stress testing with multiple brains...")
        
        start = time.perf_counter()
        
        # Create many brains
        brains = []
        for i in range(20):
            brain = pynlm.createBrain(config)
            brain.initialize()
            brains.append(brain)
        
        # Simulate each brain
        for i, brain in enumerate(brains):
            for step in range(10):
                brain.step(step)
        
        elapsed = time.perf_counter() - start
        
        print(f"   Created and simulated 20 brains in {elapsed:.2f} seconds")
        print(f"   Average time per brain: {elapsed/20:.3f} seconds")
        
        # Clean up
        brains.clear()
        import gc
        gc.collect()
        
        self.results.append(('stress_test', {
            'total_time': elapsed,
            'brains_created': 20,
            'steps_per_brain': 10
        }))

if __name__ == "__main__":
    tester = PerformanceTester()
    
    if len(sys.argv) > 1 and sys.argv[1] == "--quick":
        print("Running quick performance tests...")
        config = pynlm.createDefaultConfig()
        
        # Quick benchmarks
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        times = []
        for i in range(100):
            start = time.perf_counter()
            brain.step(i)
            times.append(time.perf_counter() - start)
        
        avg_time = statistics.mean(times)
        print(f"\nQuick test: Average brain step time: {avg_time*1000:.2f} ms")
        print(f"Quick test: Min brain step time: {min(times)*1000:.2f} ms")
        print(f"Quick test: Max brain step time: {max(times)*1000:.2f} ms")
        
    else:
        results = tester.run_comprehensive_tests()
        
        print("\n=== Summary ===")
        for test_name, result in results:
            print(f"{test_name}: {result}")
        
        print("\n✓ All performance tests completed")
