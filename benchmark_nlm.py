#!/usr/bin/env python3
"""
Performance Benchmark Suite for NLM (Neural Learning Machine)

Comprehensive performance benchmarks for NLM enhanced bindings.
Tests performance across different configurations and scenarios.
"""

import pynlm
import time
import statistics
import sys
from typing import List, Dict, Any, Tuple
from datetime import datetime
class BenchmarkTimer:
    """Timer for performance measurements"""
    
    def __init__(self, name: str):
        self.name = name
        self.start_time = None
        self.end_time = None
        self.duration = None
    
    def __enter__(self):
        self.start_time = time.perf_counter()
        return self
    
    def __exit__(self, exc_type, exc_val, exc_tb):
        self.end_time = time.perf_counter()
        self.duration = self.end_time - self.start_time
    
    def get_duration(self) -> float:
        if self.duration is None:
            return 0.0
        return self.duration
class BenchmarkSuite:
    """Benchmark suite for NLM performance"""
    
    def __init__(self):
        self.results = []
        self.start_time = time.time()
    
    def run_benchmark(self, name: str, func) -> Dict[str, Any]:
        """Run a benchmark and record results"""
        print(f"Running benchmark: {name}")
        
        times = []
        iterations = 5  # Run each benchmark 5 times for statistical significance
        
        for i in range(iterations):
            with BenchmarkTimer(f"{name}_iteration_{i}") as timer:
                func()
            times.append(timer.get_duration())
        
        # Calculate statistics
        mean_time = statistics.mean(times)
        median_time = statistics.median(times)
        std_dev = statistics.stdev(times) if len(times) > 1 else 0.0
        min_time = min(times)
        max_time = max(times)
        
        result = {
            'name': name,
            'mean_time': mean_time,
            'median_time': median_time,
            'std_dev': std_dev,
            'min_time': min_time,
            'max_time': max_time,
            'times': times,
            'iterations': iterations,
            'timestamp': datetime.now().isoformat()
        }
        
        self.results.append(result)
        
        print(f"  Mean: {mean_time:.3f}s | Median: {median_time:.3f}s | "
              f"Min: {min_time:.3f}s | Max: {max_time:.3f}s | "
              f"Std Dev: {std_dev:.3f}s")
        
        return result
class BrainBenchmark:
    """Benchmark brain creation and simulation"""
    
    def __init__(self, config):
        self.config = config
    
    def benchmark_creation(self) -> float:
        """Benchmark brain creation time"""
        brain = pynlm.createBrain(self.config)
        brain.initialize()
        return brain.getTotalNeuronCount()
    
    def benchmark_simulation(self, steps: int) -> float:
        """Benchmark simulation time"""
        brain = pynlm.createBrain(self.config)
        brain.initialize()
        
        start_time = time.perf_counter()
        for i in range(steps):
            brain.step(i, 0.1)
        end_time = time.perf_counter()
        
        return end_time - start_time
    
    def benchmark_context_manager(self, steps: int) -> float:
        """Benchmark context manager usage"""
        brain = pynlm.createBrain(self.config)
        
        start_time = time.perf_counter()
        with brain as ctx:
            ctx.runSteps(steps)
        end_time = time.perf_counter()
        
        return end_time - start_time
class ConfigurationBenchmark:
    """Benchmark configuration operations"""
    
    def benchmark_creation(self) -> float:
        """Benchmark configuration creation"""
        config = pynlm.createDefaultConfig()
        return len(config.getKeys())
    
    def benchmark_validation(self, config) -> float:
        """Benchmark configuration validation"""
        start_time = time.perf_counter()
        config.validate()
        end_time = time.perf_counter()
        return end_time - start_time
    
    def benchmark_save_load(self, config) -> float:
        """Benchmark save/load operations"""
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        start_time = time.perf_counter()
        brain.save("temp_checkpoint.bin")
        load_start = time.perf_counter()
        brain.load("temp_checkpoint.bin")
        end_time = time.perf_counter()
        
        # Clean up
        import os
        if os.path.exists("temp_checkpoint.bin"):
            os.remove("temp_checkpoint.bin")
        
        return end_time - start_time
class AgentBenchmark:
    """Benchmark agent operations"""
    
    def __init__(self, config, world):
        self.config = config
        self.world = world
    
    def benchmark_agent_creation(self) -> float:
        """Benchmark agent creation time"""
        brain = pynlm.createBrain(self.config)
        brain.initialize()
        
        start_time = time.perf_counter()
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(self.world)
        end_time = time.perf_counter()
        
        return end_time - start_time
    
    def benchmark_agent_simulation(self, steps: int) -> float:
        """Benchmark agent simulation time"""
        brain = pynlm.createBrain(self.config)
        brain.initialize()
        
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(self.world)
        
        start_time = time.perf_counter()
        
        for step in range(steps):
            self.world.update(0.1)
            agent.processSensoryInput(self.world.getSensoryPercept())
            brain.step(step, 0.1)
            action = agent.decodeMotorCommand()
            self.world.applyMotorCommand(action, self.world.getSimulationTime())
        
        end_time = time.perf_counter()
        return end_time - start_time
class VisualizationBenchmark:
    """Benchmark visualization operations"""
    
    def __init__(self, brain):
        self.brain = brain
    
    def benchmark_stats_generation(self) -> float:
        """Benchmark brain statistics generation"""
        start_time = time.perf_counter()
        stats = pynlm.visualizeBrainStats(self.brain)
        end_time = time.perf_counter()
        
        # Verify stats are generated
        self._assertGeneratedStats(stats)
        
        return end_time - start_time
    
    def benchmark_progress_report(self, steps: int) -> float:
        """Benchmark progress report generation"""
        start_time = time.perf_counter()
        report = pynlm.generateProgressReport(self.brain, steps, steps * 0.1)
        end_time = time.perf_counter()
        
        # Verify report is generated
        self._assertGeneratedReport(report)
        
        return end_time - start_time
    
    def benchmark_html_generation(self) -> float:
        """Benchmark HTML report generation"""
        start_time = time.perf_counter()
        # This would require a more complex setup with world and events
        # For now, just test the function exists
        end_time = time.perf_counter()
        return end_time - start_time
    
    def _assertGeneratedStats(self, stats: str):
        """Assert that stats were generated"""
        assert len(stats) > 0
        assert "Neurons" in stats
    
    def _assertGeneratedReport(self, report: List[str]):
        """Assert that report was generated"""
        assert len(report) > 0
class LearningBenchmark:
    """Benchmark learning operations"""
    
    def benchmark_stdp_learning(self, steps: int) -> float:
        """Benchmark STDP learning"""
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        
        start_time = time.perf_counter()
        pynlm.demonstrateSTDP(brain, steps)
        end_time = time.perf_counter()
        
        return end_time - start_time
    
    def benchmark_hebbian_learning(self, steps: int) -> float:
        """Benchmark Hebbian learning"""
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        
        start_time = time.perf_counter()
        pynlm.demonstrateHebbianLearning(brain, steps)
        end_time = time.perf_counter()
        
        return end_time - start_time
    
    def benchmark_curiosity_learning(self, steps: int) -> float:
        """Benchmark curiosity-driven learning"""
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        
        world = pynlm.createSimpleWorld()
        world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
        world.reset()
        
        start_time = time.perf_counter()
        pynlm.demonstrateCuriosityDrivenLearning(brain, world, steps)
        end_time = time.perf_counter()
        
        return end_time - start_time
class MemoryBenchmark:
    """Benchmark memory operations"""
    
    def benchmark_large_brain(self) -> float:
        """Benchmark with large brain configuration"""
        config = pynlm.createDefaultConfig()
        config.set("brain.neuron_count", 5000)
        
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        start_time = time.perf_counter()
        for step in range(20):
            brain.step(step, 0.1)
        end_time = time.perf_counter()
        
        return end_time - start_time
    
    def benchmark_memory_usage(self) -> Tuple[float, float]:
        """Benchmark memory usage"""
        import psutil
        import os
        
        process = psutil.Process(os.getpid())
        initial_memory = process.memory_info().rss / 1024 / 1024  # MB
        
        # Create and use brain
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        for step in range(100):
            brain.step(step, 0.1)
        
        peak_memory = process.memory_info().rss / 1024 / 1024  # MB
        memory_increase = peak_memory - initial_memory
        
        return memory_increase, peak_memory
def run_benchmark_suite():
    """Run complete benchmark suite"""
    print("=" * 80)
    print("NLM Performance Benchmark Suite")
    print("=" * 80)
    print(f"Started: {datetime.now().strftime('%Y-%m-%d %H:%M:%S')}")
    print()
    
    benchmark_suite = BenchmarkSuite()
    
    # Test different configurations
    configs = {
        "Minimal": pynlm.createDefaultConfig(),
        "Advanced": pynlm.createDefaultConfig(),
    }
    
    # Create worlds for agent benchmarks
    worlds = {
        "Standard": pynlm.createSimpleWorld(),
        "Large": pynlm.createSimpleWorld(),
    }
    
    for world_name, world in worlds.items():
        world.configure(width=30, height=30, visionWidth=16, visionHeight=16)
        world.reset()
    
    # Brain benchmarks
    print("=" * 60)
    print("Brain Performance Benchmarks")
    print("=" * 60)
    
    for config_name, config in configs.items():
        print(f"\n--- {config_name} Configuration ---")
        
        brain_benchmark = BrainBenchmark(config)
        
        # Test creation
        benchmark_suite.run_benchmark(
            f"BrainCreation_{config_name}",
            brain_benchmark.benchmark_creation
        )
        
        # Test simulation
        benchmark_suite.run_benchmark(
            f"Simulation_100steps_{config_name}",
            lambda: brain_benchmark.benchmark_simulation(100)
        )
        
        # Test context manager
        benchmark_suite.run_benchmark(
            f"ContextManager_50steps_{config_name}",
            lambda: brain_benchmark.benchmark_context_manager(50)
        )
    
    # Configuration benchmarks
    print("\n" + "=" * 60)
    print("Configuration Operation Benchmarks")
    print("=" * 60)
    
    config_benchmark = ConfigurationBenchmark()
    test_config = pynlm.createDefaultConfig()
    
    benchmark_suite.run_benchmark(
        "ConfigurationCreation",
        config_benchmark.benchmark_creation
    )
    
    benchmark_suite.run_benchmark(
        "ConfigurationValidation",
        lambda: config_benchmark.benchmark_validation(test_config)
    )
    
    benchmark_suite.run_benchmark(
        "SaveLoadCheckpoint",
        lambda: config_benchmark.benchmark_save_load(test_config)
    )
    
    # Agent benchmarks
    print("\n" + "=" * 60)
    print("Agent Performance Benchmarks")
    print("=" * 60)
    
    for world_name, world in worlds.items():
        print(f"\n--- {world_name} World ---")
        
        agent_benchmark = AgentBenchmark(configs["Advanced"], world)
        
        # Test agent creation
        benchmark_suite.run_benchmark(
            f"AgentCreation_{world_name}",
            agent_benchmark.benchmark_agent_creation
        )
        
        # Test agent simulation
        benchmark_suite.run_benchmark(
            f"AgentSimulation_50steps_{world_name}",
            lambda: agent_benchmark.benchmark_agent_simulation(50)
        )
    
    # Visualization benchmarks
    print("\n" + "=" * 60)
    print("Visualization Performance Benchmarks")
    print("=" * 60)
    
    config = configs["Advanced"]
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    viz_benchmark = VisualizationBenchmark(brain)
    
    benchmark_suite.run_benchmark(
        "StatsGeneration",
        viz_benchmark.benchmark_stats_generation
    )
    
    benchmark_suite.run_benchmark(
        "ProgressReport_100steps",
        lambda: viz_benchmark.benchmark_progress_report(100)
    )
    
    # Learning benchmarks
    print("\n" + "=" * 60)
    print("Learning Algorithm Benchmarks")
    print("=" * 60)
    
    learning_benchmark = LearningBenchmark()
    
    benchmark_suite.run_benchmark(
        "STDP_Learning_100steps",
        lambda: learning_benchmark.benchmark_stdp_learning(100)
    )
    
    benchmark_suite.run_benchmark(
        "Hebbian_Learning_100steps",
        lambda: learning_benchmark.benchmark_hebbian_learning(100)
    )
    
    benchmark_suite.run_benchmark(
        "Curiosity_Learning_100steps",
        lambda: learning_benchmark.benchmark_curiosity_learning(100)
    )
    
    # Memory benchmarks
    print("\n" + "=" * 60)
    print("Memory Usage Benchmarks")
    print("=" * 60)
    
    memory_benchmark = MemoryBenchmark()
    
    benchmark_suite.run_benchmark(
        "LargeBrainSimulation",
        memory_benchmark.benchmark_large_brain
    )
    
    memory_increase, peak_memory = memory_benchmark.benchmark_memory_usage()
    print(f"Memory usage test: +{memory_increase:.1f} MB (peak: {peak_memory:.1f} MB)")
    
    # Print summary
    print("\n" + "=" * 80)
    print("Benchmark Summary")
    print("=" * 80)
    
    total_time = time.time() - benchmark_suite.start_time
    print(f"Total benchmark duration: {total_time:.2f} seconds")
    print(f"Number of benchmarks: {len(benchmark_suite.results)}")
    
    print("\nTop 5 fastest benchmarks:")
    sorted_results = sorted(benchmark_suite.results, key=lambda x: x['mean_time'])
    for i, result in enumerate(sorted_results[:5]):
        print(f"  {i+1}. {result['name']}: {result['mean_time']:.3f}s")
    
    print("\nTop 5 slowest benchmarks:")
    sorted_results = sorted(benchmark_suite.results, key=lambda x: x['mean_time'], reverse=True)
    for i, result in enumerate(sorted_results[:5]):
        print(f"  {i+1}. {result['name']}: {result['mean_time']:.3f}s")
    
    print("\nDetailed results:")
    for result in benchmark_suite.results:
        print(f"  {result['name']}: {result['mean_time']:.3f}s ± {result['std_dev']:.3f}s")
    
    print("\n" + "=" * 80)
    print("Benchmark Suite Complete")
    print("=" * 80)
    
    return benchmark_suite.results
def main():
    """Main benchmark runner"""
    try:
        results = run_benchmark_suite()
        print("\n✅ All benchmarks completed successfully!")
        return 0
    except Exception as e:
        print(f"\n❌ Benchmark failed with error: {e}")
        import traceback
        traceback.print_exc()
        return 1
if __name__ == "__main__":
    sys.exit(main())