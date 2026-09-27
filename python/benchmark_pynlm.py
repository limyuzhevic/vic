"""
NLM Python Bindings Benchmark Suite
====================================

This file contains benchmarks for the NLM Python bindings to measure performance
and ensure the bindings work correctly across different scenarios.

Run benchmarks using:
    python benchmark_pynlm.py
    python -m pytest benchmark_pynlm.py -v
"""

import time
import statistics
import pynlm

class NLMBenchmark:
    """Benchmark suite for NLM Python bindings."""
    
    def __init__(self):
        self.results = []
    
    def benchmark_creation(self):
        """Benchmark object creation performance."""
        print("Benchmarking object creation...")
        
        times = []
        
        # Benchmark config creation
        for _ in range(100):
            start = time.time()
            config = pynlm.createDefaultConfig()
            end = time.time()
            times.append(end - start)
        
        config_time = statistics.mean(times) * 1000
        print(f"  Config creation: {config_time:.2f}ms average")
        
        # Benchmark brain creation
        times.clear()
        for _ in range(50):
            start = time.time()
            brain = pynlm.createBrain()
            end = time.time()
            times.append(end - start)
        
        brain_time = statistics.mean(times) * 1000
        print(f"  Brain creation: {brain_time:.2f}ms average")
        
        # Benchmark world creation
        times.clear()
        for _ in range(50):
            start = time.time()
            world = pynlm.createSimpleWorld()
            end = time.time()
            times.append(end - start)
        
        world_time = statistics.mean(times) * 1000
        print(f"  World creation: {world_time:.2f}ms average")
        
        # Benchmark agent creation
        times.clear()
        for _ in range(50):
            start = time.time()
            agent = pynlm.createAgentBrain(pynlm.createBrain())
            end = time.time()
            times.append(end - start)
        
        agent_time = statistics.mean(times) * 1000
        print(f"  Agent creation: {agent_time:.2f}ms average")
        
        self.results.append({
            'test': 'creation',
            'config_time_ms': config_time,
            'brain_time_ms': brain_time,
            'world_time_ms': world_time,
            'agent_time_ms': agent_time,
        })
    
    def benchmark_initialization(self):
        """Benchmark initialization performance."""
        print("Benchmarking initialization...")
        
        times = []
        
        for _ in range(20):
            config = pynlm.createDefaultConfig()
            brain = pynlm.createBrain(config)
            
            start = time.time()
            brain.initialize()
            end = time.time()
            
            times.append(end - start)
        
        init_time = statistics.mean(times) * 1000
        print(f"  Brain initialization: {init_time:.2f}ms average")
        
        self.results.append({
            'test': 'initialization',
            'init_time_ms': init_time,
        })
    
    def benchmark_simulation_step(self):
        """Benchmark simulation step performance."""
        print("Benchmarking simulation steps...")
        
        # Setup components
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        world = pynlm.createSimpleWorld()
        agent = pynlm.createAgentBrain(brain)
        
        world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
        world.setAgentStart(10, 10)
        brain.initialize()
        agent.initialize(world)
        
        # Benchmark single step
        times = []
        
        for _ in range(100):
            start = time.time()
            
            world.update(0.1)
            agent.processSensoryInput(world.getSensoryPercept())
            brain.step(0)
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            
            end = time.time()
            times.append(end - start)
        
        step_time = statistics.mean(times) * 1000
        steps_per_second = 1000 / statistics.mean(times)
        
        print(f"  Single simulation step: {step_time:.2f}ms average")
        print(f"  Throughput: {steps_per_second:.0f} steps/second")
        
        self.results.append({
            'test': 'simulation_step',
            'step_time_ms': step_time,
            'steps_per_second': steps_per_second,
        })
    
    def benchmark_complete_simulation(self):
        """Benchmark complete simulation performance."""
        print("Benchmarking complete simulations...")
        
        times = []
        
        for _ in range(5):
            config = pynlm.createDefaultConfig()
            brain = pynlm.createBrain(config)
            world = pynlm.createSimpleWorld()
            agent = pynlm.createAgentBrain(brain)
            
            world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
            world.setAgentStart(10, 10)
            
            brain.initialize()
            agent.initialize(world)
            
            start = time.time()
            
            for step in range(100):
                world.update(0.1)
                agent.processSensoryInput(world.getSensoryPercept())
                brain.step(step)
                action = agent.decodeMotorCommand()
                world.applyMotorCommand(action, world.getSimulationTime())
                
                # Apply learning
                reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
                agent.applyRewardModulation(reward, 0.0)
                agent.updateDevelopment(0.1)
            
            end = time.time()
            times.append(end - start)
        
        total_time = statistics.mean(times)
        steps_per_second = 500 / total_time  # 5 runs * 100 steps
        
        print(f"  5 simulations (500 steps total): {total_time:.2f}s average")
        print(f"  Throughput: {steps_per_second:.0f} steps/second")
        
        self.results.append({
            'test': 'complete_simulation',
            'total_time_s': total_time,
            'steps_per_second': steps_per_second,
        })
    
    def benchmark_memory_usage(self):
        """Benchmark memory usage (estimated)."""
        print("Benchmarking memory usage...")
        
        # Create large brain
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        neuron_count = brain.getTotalNeuronCount()
        synapse_count = brain.getTotalSynapseCount()
        
        # Rough memory estimate
        # Each neuron: ~1KB, each synapse: ~100 bytes, overhead
        memory_estimate_mb = (neuron_count * 1024 + synapse_count * 100 + 10000) / (1024 * 1024)
        
        print(f"  Neuron count: {neuron_count:,}")
        print(f"  Synapse count: {synapse_count:,}")
        print(f"  Estimated memory usage: {memory_estimate_mb:.2f} MB")
        
        self.results.append({
            'test': 'memory_usage',
            'neuron_count': neuron_count,
            'synapse_count': synapse_count,
            'memory_estimate_mb': memory_estimate_mb,
        })
    
    def benchmark_scaling(self):
        """Benchmark scaling with different brain sizes."""
        print("Benchmarking scaling with different brain sizes...")
        
        sizes = [100, 500, 1000, 2000]
        step_times = []
        
        for size in sizes:
            config = pynlm.createDefaultConfig()
            config.setInt("neuron_count", size)
            
            brain = pynlm.createBrain(config)
            brain.initialize()
            
            world = pynlm.createSimpleWorld()
            agent = pynlm.createAgentBrain(brain)
            
            # Time simulation steps
            times = []
            for _ in range(20):
                start = time.time()
                
                world.update(0.1)
                agent.processSensoryInput(world.getSensoryPercept())
                brain.step(0)
                action = agent.decodeMotorCommand()
                world.applyMotorCommand(action, world.getSimulationTime())
                
                end = time.time()
                times.append(end - start)
            
            avg_step_time = statistics.mean(times) * 1000
            step_times.append(avg_step_time)
            
            print(f"  {size:4d} neurons: {avg_step_time:6.2f}ms per step")
        
        # Analyze scaling
        if len(step_times) >= 2:
            size_ratio = sizes[-1] / sizes[0]
            time_ratio = step_times[-1] / step_times[0]
            efficiency = size_ratio / time_ratio
            
            print(f"  Scaling efficiency: {efficiency:.2f}x (ideal would be {size_ratio:.1f}x)")
        
        self.results.append({
            'test': 'scaling',
            'sizes': sizes,
            'step_times_ms': step_times,
        })
    
    def run_all_benchmarks(self):
        """Run all benchmarks."""
        print("NLM Python Bindings Benchmark Suite")
        print("=" * 60)
        print()
        
        self.benchmark_creation()
        print()
        
        self.benchmark_initialization()
        print()
        
        self.benchmark_simulation_step()
        print()
        
        self.benchmark_complete_simulation()
        print()
        
        self.benchmark_memory_usage()
        print()
        
        self.benchmark_scaling()
        print()
        
        print("=" * 60)
        print("Benchmark Results Summary:")
        print("=" * 60)
        
        for result in self.results:
            test_name = result['test'].replace('_', ' ').title()
            print(f"\n{test_name}:")
            
            if test_name == 'Creation':
                print(f"  Config creation: {result['config_time_ms']:.2f}ms")
                print(f"  Brain creation: {result['brain_time_ms']:.2f}ms")
                print(f"  World creation: {result['world_time_ms']:.2f}ms")
                print(f"  Agent creation: {result['agent_time_ms']:.2f}ms")
            
            elif test_name == 'Initialization':
                print(f"  Brain initialization: {result['init_time_ms']:.2f}ms")
            
            elif test_name == 'Simulation Step':
                print(f"  Average step time: {result['step_time_ms']:.2f}ms")
                print(f"  Throughput: {result['steps_per_second']:.0f} steps/second")
            
            elif test_name == 'Complete Simulation':
                print(f"  Total time for 500 steps: {result['total_time_s']:.2f}s")
                print(f"  Throughput: {result['steps_per_second']:.0f} steps/second")
            
            elif test_name == 'Memory Usage':
                print(f"  Neurons: {result['neuron_count']:,}")
                print(f"  Synapses: {result['synapse_count']:,}")
                print(f"  Estimated memory: {result['memory_estimate_mb']:.2f} MB")
            
            elif test_name == 'Scaling':
                for size, step_time in zip(result['sizes'], result['step_times_ms']):
                    print(f"  {size:4d} neurons: {step_time:6.2f}ms per step")
        
        print("\n" + "=" * 60)
        print("Benchmark completed successfully!")
        return self.results

def quick_test():
    """Quick test to verify basic functionality."""
    print("Running quick functionality test...")
    
    # Test basic imports
    assert hasattr(pynlm, '__version__')
    assert hasattr(pynlm, 'createBrain')
    assert hasattr(pynlm, 'createSimpleWorld')
    assert hasattr(pynlm, 'createAgentBrain')
    
    # Test basic creation
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    world = pynlm.createSimpleWorld()
    agent = pynlm.createAgentBrain(brain)
    
    # Test initialization
    brain.initialize()
    agent.initialize(world)
    
    # Test basic operations
    assert brain.getTotalNeuronCount() > 0
    assert agent.getSensoryInputSize() > 0
    assert agent.getMotorOutputSize() > 0
    
    # Test a few steps
    for step in range(5):
        world.update(0.1)
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
    
    print("Quick test passed! ✓")

if __name__ == "__main__":
    print("NLM Python Bindings Benchmark Suite")
    print("=" * 60)
    print()
    
    # Quick test first
    try:
        quick_test()
        print()
    except Exception as e:
        print(f"Quick test failed: {e}")
        exit(1)
    
    # Run full benchmarks
    benchmark = NLMBenchmark()
    try:
        results = benchmark.run_all_benchmarks()
        print("\nAll benchmarks completed successfully!")
    except Exception as e:
        print(f"Benchmark failed: {e}")
        import traceback
        traceback.print_exc()
        exit(1)
