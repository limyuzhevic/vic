"""NLM Python Bindings - Enhanced Benchmarking Infrastructure

This module provides comprehensive benchmarking infrastructure for the NLM Python bindings.
It enables performance measurement, comparison, and optimization of brain simulation systems.

Features:
- Multi-dimensional performance measurement
- Comparison utilities for different configurations
- Load and stress testing capabilities
- Detailed reporting and analysis
- Integration with existing test framework
"""

import time
import statistics
import json
from typing import Dict, Any, List, Callable, Optional, Tuple
from dataclasses import dataclass, field
from enum import Enum
import memory_profiler
import psutil
import os
import threading
from concurrent.futures import ThreadPoolExecutor

class BenchmarkType(Enum):
    """Types of benchmarks."""
    INITIALIZATION = "initialization"
    SIMULATION_STEP = "simulation_step"
    MEMORY_OPERATIONS = "memory_operations"
    COGNITIVE_PROCESSING = "cognitive_processing"
    INTEGRATION_TEST = "integration_test"
    STRESS_TEST = "stress_test"
    PERFORMANCE_PROFILE = "performance_profile"

class BenchmarkResult:
    """Result of a single benchmark run."""
    
    def __init__(self, name: str, benchmark_type: BenchmarkType):
        self.name = name
        self.benchmark_type = benchmark_type
        self.timestamp = time.time()
        
        # Performance metrics
        self.execution_time: float = 0.0
        self.cpu_time: float = 0.0
        self.memory_usage: float = 0.0
        self.peak_memory: float = 0.0
        
        # Throughput metrics
        self.steps_per_second: float = 0.0
        self.operations_per_second: float = 0.0
        
        # Quality metrics
        self.success_rate: float = 0.0
        self.accuracy: float = 0.0
        self.energy_efficiency: float = 0.0
        
        # System resources
        self.cpu_usage: float = 0.0
        self.memory_available: float = 0.0
        self.gpu_usage: float = 0.0
        
        # Custom metrics
        self.custom_metrics: Dict[str, Any] = {}
        
        # Metadata
        self.config: Dict[str, Any] = {}
        self.tags: List[str] = []
        
        self.is_passed: bool = False
        self.error_message: Optional[str] = None
    
    def to_dict(self) -> Dict[str, Any]:
        """Convert benchmark result to dictionary."""
        return {
            'name': self.name,
            'benchmark_type': self.benchmark_type.value,
            'timestamp': self.timestamp,
            'execution_time': self.execution_time,
            'cpu_time': self.cpu_time,
            'memory_usage': self.memory_usage,
            'peak_memory': self.peak_memory,
            'steps_per_second': self.steps_per_second,
            'operations_per_second': self.operations_per_second,
            'success_rate': self.success_rate,
            'accuracy': self.accuracy,
            'energy_efficiency': self.energy_efficiency,
            'cpu_usage': self.cpu_usage,
            'memory_available': self.memory_available,
            'gpu_usage': self.gpu_usage,
            'custom_metrics': self.custom_metrics,
            'config': self.config,
            'tags': self.tags,
            'is_passed': self.is_passed,
            'error_message': self.error_message
        }
    
    @classmethod
    def from_dict(cls, data: Dict[str, Any]) -> 'BenchmarkResult':
        """Create benchmark result from dictionary."""
        result = cls(data['name'], BenchmarkType(data['benchmark_type']))
        result.timestamp = data['timestamp']
        result.execution_time = data['execution_time']
        result.cpu_time = data['cpu_time']
        result.memory_usage = data['memory_usage']
        result.peak_memory = data['peak_memory']
        result.steps_per_second = data['steps_per_second']
        result.operations_per_second = data['operations_per_second']
        result.success_rate = data['success_rate']
        result.accuracy = data['accuracy']
        result.energy_efficiency = data['energy_efficiency']
        result.cpu_usage = data['cpu_usage']
        result.memory_available = data['memory_available']
        result.gpu_usage = data['gpu_usage']
        result.custom_metrics = data['custom_metrics']
        result.config = data['config']
        result.tags = data['tags']
        result.is_passed = data['is_passed']
        result.error_message = data['error_message']
        return result
class BenchmarkSuite:
    """Collection of related benchmarks."""
    
    def __init__(self, name: str):
        self.name = name
        self.benchmarks: List[BenchmarkResult] = []
        self.metadata: Dict[str, Any] = {}
        self.created_at = time.time()
    
    def add_benchmark(self, benchmark: BenchmarkResult):
        """Add a benchmark result."""
        self.benchmarks.append(benchmark)
    
    def get_benchmarks_by_type(self, benchmark_type: BenchmarkType) -> List[BenchmarkResult]:
        """Get benchmarks of a specific type."""
        return [b for b in self.benchmarks if b.benchmark_type == benchmark_type]
    
    def get_benchmarks_by_tag(self, tag: str) -> List[BenchmarkResult]:
        """Get benchmarks with a specific tag."""
        return [b for b in self.benchmarks if tag in b.tags]
    
    def get_summary(self) -> Dict[str, Any]:
        """Get summary statistics for the benchmark suite."""
        if not self.benchmarks:
            return {'total_benchmarks': 0}
        
        successful = [b for b in self.benchmarks if b.is_passed]
        failed = [b for b in self.benchmarks if not b.is_passed]
        
        execution_times = [b.execution_time for b in self.benchmarks]
        memory_usages = [b.memory_usage for b in self.benchmarks]
        
        return {
            'name': self.name,
            'total_benchmarks': len(self.benchmarks),
            'successful_benchmarks': len(successful),
            'failed_benchmarks': len(failed),
            'success_rate': len(successful) / len(self.benchmarks) if self.benchmarks else 0,
            'avg_execution_time': statistics.mean(execution_times) if execution_times else 0,
            'median_execution_time': statistics.median(execution_times) if execution_times else 0,
            'max_execution_time': max(execution_times) if execution_times else 0,
            'min_execution_time': min(execution_times) if execution_times else 0,
            'avg_memory_usage': statistics.mean(memory_usages) if memory_usages else 0,
            'peak_memory': max(b.peak_memory for b in self.benchmarks) if self.benchmarks else 0,
            'created_at': self.created_at,
            'tags': list(set(tag for b in self.benchmarks for tag in b.tags))
        }
    
    def to_dict(self) -> Dict[str, Any]:
        """Convert benchmark suite to dictionary."""
        return {
            'name': self.name,
            'benchmarks': [b.to_dict() for b in self.benchmarks],
            'metadata': self.metadata,
            'created_at': self.created_at
        }
    
    @classmethod
    def from_dict(cls, data: Dict[str, Any]) -> 'BenchmarkSuite':
        """Create benchmark suite from dictionary."""
        suite = cls(data['name'])
        suite.metadata = data['metadata']
        suite.created_at = data['created_at']
        suite.benchmarks = [BenchmarkResult.from_dict(b) for b in data['benchmarks']]
        return suite
class PerformanceProfiler:
    """Performance profiler for detailed system analysis."""
    
    def __init__(self):
        self.profile_data: Dict[str, Any] = {}
        self.start_time: float = 0.0
        self.end_time: float = 0.0
    
    def start_profile(self, operation_name: str):
        """Start profiling a specific operation."""
        self.start_time = time.time()
        self.profile_data[operation_name] = {
            'start_time': self.start_time,
            'memory_before': self._get_memory_usage(),
            'cpu_before': self._get_cpu_usage(),
            'thread_count': threading.active_count()
        }
    
    def end_profile(self, operation_name: str):
        """End profiling of an operation."""
        if operation_name not in self.profile_data:
            return
        
        end_time = time.time()
        profile = self.profile_data[operation_name]
        
        profile.update({
            'end_time': end_time,
            'duration': end_time - profile['start_time'],
            'memory_after': self._get_memory_usage(),
            'cpu_after': self._get_cpu_usage(),
            'memory_delta': self._get_memory_usage() - profile['memory_before'],
            'cpu_delta': self._get_cpu_usage() - profile['cpu_before']
        })
        
        self.end_time = end_time
    
    def _get_memory_usage(self) -> float:
        """Get current memory usage in MB."""
        process = psutil.Process(os.getpid())
        return process.memory_info().rss / 1024 / 1024
    
    def _get_cpu_usage(self) -> float:
        """Get current CPU usage percentage."""
        return psutil.cpu_percent()
    
    def get_summary(self) -> Dict[str, Any]:
        """Get performance profile summary."""
        return {
            'total_operations': len(self.profile_data),
            'total_duration': self.end_time - self.start_time if self.end_time > 0 else 0,
            'operations': self.profile_data
        }
class BenchmarkManager:
    """Manager for all benchmarking operations."""
    
    def __init__(self, benchmark_dir: str = "./benchmarks"):
        self.benchmark_dir = benchmark_dir
        self.active_suites: Dict[str, BenchmarkSuite] = {}
        self.profiler = PerformanceProfiler()
        
        # Create benchmark directory
        os.makedirs(self.benchmark_dir, exist_ok=True)
    
    def create_suite(self, name: str) -> BenchmarkSuite:
        """Create a new benchmark suite."""
        suite = BenchmarkSuite(name)
        self.active_suites[name] = suite
        return suite
    
    def get_suite(self, name: str) -> Optional[BenchmarkSuite]:
        """Get a benchmark suite by name."""
        return self.active_suites.get(name)
    
    def run_benchmark(self, suite_name: str, benchmark_func: Callable, 
                     name: str, benchmark_type: BenchmarkType,
                     config: Optional[Dict[str, Any]] = None,
                     tags: Optional[List[str]] = None) -> BenchmarkResult:
        """Run a single benchmark."""
        suite = self.active_suites.get(suite_name)
        if not suite:
            suite = self.create_suite(suite_name)
        
        # Start profiling
        self.profiler.start_profile(name)
        
        try:
            # Run the benchmark function
            result = benchmark_func()
            
            # Collect system metrics
            execution_time = self.profiler.profile_data[name]['duration']
            memory_before = self.profiler.profile_data[name]['memory_before']
            memory_after = self.profiler.profile_data[name]['memory_after']
            cpu_before = self.profiler.profile_data[name]['cpu_before']
            cpu_after = self.profiler.profile_data[name]['cpu_after']
            
            # Create benchmark result
            benchmark_result = BenchmarkResult(name, benchmark_type)
            benchmark_result.execution_time = execution_time
            benchmark_result.memory_usage = memory_after
            benchmark_result.peak_memory = max(memory_before, memory_after)
            benchmark_result.cpu_usage = cpu_after
            benchmark_result.custom_metrics = result if isinstance(result, dict) else {}
            benchmark_result.config = config or {}
            benchmark_result.tags = tags or []
            benchmark_result.is_passed = True
            
            # Calculate derived metrics
            if execution_time > 0:
                benchmark_result.steps_per_second = 1.0 / execution_time
            
            if isinstance(result, dict) and 'steps_per_second' in result:
                benchmark_result.steps_per_second = result['steps_per_second']
            
            suite.add_benchmark(benchmark_result)
            
            return benchmark_result
            
        except Exception as e:
            # Create failed benchmark result
            benchmark_result = BenchmarkResult(name, benchmark_type)
            benchmark_result.error_message = str(e)
            benchmark_result.config = config or {}
            benchmark_result.tags = tags or []
            benchmark_result.is_passed = False
            
            suite.add_benchmark(benchmark_result)
            return benchmark_result
        
        finally:
            self.profiler.end_profile(name)
    
    def save_results(self, suite_name: str, filepath: str = None) -> bool:
        """Save benchmark results to file."""
        suite = self.active_suites.get(suite_name)
        if not suite:
            return False
        
        if not filepath:
            filepath = f"{self.benchmark_dir}/{suite_name}_results.json"
        
        try:
            with open(filepath, 'w') as f:
                json.dump(suite.to_dict(), f, indent=2)
            return True
        except Exception as e:
            print(f"Error saving benchmark results: {e}")
            return False
    
    def load_results(self, filepath: str) -> BenchmarkSuite:
        """Load benchmark results from file."""
        try:
            with open(filepath, 'r') as f:
                data = json.load(f)
            return BenchmarkSuite.from_dict(data)
        except Exception as e:
            print(f"Error loading benchmark results: {e}")
            return BenchmarkSuite("loaded_results")
    
    def get_system_info(self) -> Dict[str, Any]:
        """Get system information for benchmarking."""
        import platform
        
        return {
            'platform': platform.platform(),
            'python_version': platform.python_version(),
            'cpu_count': os.cpu_count(),
            'memory_total': psutil.virtual_memory().total / (1024**3),
            'memory_available': psutil.virtual_memory().available / (1024**3),
            'disk_usage': psutil.disk_usage('/').percent,
            'timestamp': time.time()
        }
# Brain-specific benchmarking functions

class BrainBenchmarks:
    """Brain-specific benchmarking functions."""
    
    @staticmethod
    def benchmark_initialization(config: Dict[str, Any]) -> Dict[str, Any]:
        """Benchmark brain initialization."""
        import pynlm as nlm
        
        start_time = time.time()
        memory_before = psutil.Process(os.getpid()).memory_info().rss
        
        # Create and initialize brain
        nlm_config = nlm.create_default_config()
        for key, value in config.items():
            nlm_config.set(key, value)
        
        brain = nlm.create_brain(nlm_config)
        brain.initialize()
        
        memory_after = psutil.Process(os.getpid()).memory_info().rss
        
        end_time = time.time()
        
        return {
            'initialization_time': end_time - start_time,
            'memory_delta': (memory_after - memory_before) / (1024**2),  # MB
            'neuron_count': brain.getTotalNeuronCount(),
            'region_count': brain.getRegionCount(),
            'synapse_count': brain.getTotalSynapseCount()
        }
    
    @staticmethod
    def benchmark_simulation(config: Dict[str, Any], num_steps: int = 100) -> Dict[str, Any]:
        """Benchmark brain simulation."""
        import pynlm as nlm
        
        # Create and initialize brain
        nlm_config = nlm.create_default_config()
        for key, value in config.items():
            nlm_config.set(key, value)
        
        brain = nlm.create_brain(nlm_config)
        brain.initialize()
        
        # Run simulation
        start_time = time.time()
        memory_before = psutil.Process(os.getpid()).memory_info().rss
        
        for i in range(num_steps):
            brain.step(i)
        
        memory_after = psutil.Process(os.getpid()).memory_info().rss
        end_time = time.time()
        
        # Calculate metrics
        execution_time = end_time - start_time
        steps_per_second = num_steps / execution_time if execution_time > 0 else 0
        memory_delta = (memory_after - memory_before) / (1024**2)  # MB
        
        return {
            'total_time': execution_time,
            'steps_per_second': steps_per_second,
            'memory_delta': memory_delta,
            'avg_firing_rate': brain.getAverageFiringRate(),
            'total_spikes': brain.getTotalSpikeCount(),
            'active_neurons': brain.getActiveNeuronCount()
        }
    
    @staticmethod
    def benchmark_memory_usage(config: Dict[str, Any], duration: float = 10.0) -> Dict[str, Any]:
        """Benchmark memory usage over time."""
        import pynlm as nlm
        
        # Create and initialize brain
        nlm_config = nlm.create_default_config()
        for key, value in config.items():
            nlm_config.set(key, value)
        
        brain = nlm.create_brain(nlm_config)
        brain.initialize()
        
        # Monitor memory usage over time
        memory_samples = []
        time_samples = []
        
        start_time = time.time()
        steps = 0
        
        while time.time() - start_time < duration:
            brain.step(steps)
            steps += 1
            
            if steps % 10 == 0:  # Sample every 10 steps
                memory = psutil.Process(os.getpid()).memory_info().rss
                memory_samples.append(memory / (1024**2))  # MB
                time_samples.append(time.time() - start_time)
        
        # Calculate statistics
        if memory_samples:
            memory_delta_avg = statistics.mean(memory_samples) if len(memory_samples) > 1 else memory_samples[0]
            memory_delta_std = statistics.stdev(memory_samples) if len(memory_samples) > 1 else 0
        else:
            memory_delta_avg = 0
            memory_delta_std = 0
        
        return {
            'total_steps': steps,
            'duration': duration,
            'steps_per_second': steps / duration if duration > 0 else 0,
            'memory_samples': memory_samples,
            'memory_delta_avg': memory_delta_avg,
            'memory_delta_std': memory_delta_std,
            'memory_growth_rate': (memory_samples[-1] - memory_samples[0]) / duration if len(memory_samples) > 1 else 0
        }
    
    @staticmethod
    def benchmark_cognitive_processing(config: Dict[str, Any], test_iterations: int = 50) -> Dict[str, Any]:
        """Benchmark cognitive processing."""
        import pynlm as nlm
        
        # Create and initialize brain
        nlm_config = nlm.create_default_config()
        for key, value in config.items():
            nlm_config.set(key, value)
        
        brain = nlm.create_brain(nlm_config)
        brain.initialize()
        
        # Run cognitive tasks
        processing_times = []
        successes = 0
        
        for i in range(test_iterations):
            step_start = time.time()
            
            # Perform cognitive operations
            brain.step(i)
            action = brain.produceAction()
            
            step_end = time.time()
            processing_times.append(step_end - step_start)
            
            if action is not None:
                successes += 1
        
        # Calculate metrics
        if processing_times:
            avg_processing_time = statistics.mean(processing_times)
            min_processing_time = min(processing_times)
            max_processing_time = max(processing_times)
            std_processing_time = statistics.stdev(processing_times) if len(processing_times) > 1 else 0
        else:
            avg_processing_time = 0
            min_processing_time = 0
            max_processing_time = 0
            std_processing_time = 0
        
        return {
            'iterations': test_iterations,
            'successful_actions': successes,
            'success_rate': successes / test_iterations if test_iterations > 0 else 0,
            'avg_processing_time': avg_processing_time,
            'min_processing_time': min_processing_time,
            'max_processing_time': max_processing_time,
            'std_processing_time': std_processing_time,
            'throughput_per_second': test_iterations / (sum(processing_times) if processing_times else 1)
        }
    
    @staticmethod
    def benchmark_integration(config: Dict[str, Any], integration_tests: int = 10) -> Dict[str, Any]:
        """Benchmark system integration."""
        import pynlm as nlm
        
        # Create brain and world
        nlm_config = nlm.create_default_config()
        for key, value in config.items():
            nlm_config.set(key, value)
        
        brain = nlm.create_brain(nlm_config)
        world = nlm.create_world()
        agent = nlm.create_agent_brain(brain)
        
        agent.initialize(world)
        
        # Run integration tests
        test_results = []
        start_time = time.time()
        
        for i in range(integration_tests):
            test_start = time.time()
            
            # Create sensory input
            vision = nlm.Vision(10, 10, 3)
            percept = nlm.SensoryPercept()
            percept.setVision(vision)
            
            # Process through complete system
            agent.processSensoryInput(percept)
            brain.step(i)
            action = brain.produceAction()
            
            # Verify integration
            is_integrated = (
                brain.getWorkingMemory() is not None and
                brain.getEpisodicMemory() is not None and
                brain.getPredictionSystem() is not None and
                brain.getDevelopmentSystem() is not None and
                brain.getDopamine() is not None
            )
            
            test_end = time.time()
            test_results.append({
                'test_number': i,
                'duration': test_end - test_start,
                'success': action is not None,
                'integration': is_integrated,
                'neuron_count': brain.getTotalNeuronCount(),
                'firing_rate': brain.getAverageFiringRate()
            })
        
        total_time = time.time() - start_time
        
        # Calculate integration statistics
        successful_tests = sum(1 for t in test_results if t['success'] and t['integration'])
        integration_rates = [t['integration'] for t in test_results]
        
        return {
            'total_tests': integration_tests,
            'successful_tests': successful_tests,
            'integration_success_rate': successful_tests / integration_tests if integration_tests > 0 else 0,
            'avg_test_time': statistics.mean(t['duration'] for t in test_results),
            'min_test_time': min(t['duration'] for t in test_results),
            'max_test_time': max(t['duration'] for t in test_results),
            'all_integrated': all(t['integration'] for t in test_results),
            'detailed_results': test_results
        }
# Global benchmark manager instance
benchmark_manager = BenchmarkManager()

def get_benchmark_manager() -> BenchmarkManager:
    """Get the global benchmark manager instance."""
    return benchmark_manager

def run_brain_benchmark_suite() -> BenchmarkSuite:
    """Run a complete brain benchmark suite."""
    suite = benchmark_manager.create_suite("brain_benchmarks")
    
    # Add system information as metadata
    suite.metadata = benchmark_manager.get_system_info()
    
    # Run various benchmarks
    configs = [
        {'neuron_count': 1000, 'region_count': 1},
        {'neuron_count': 5000, 'region_count': 3},
        {'neuron_count': 10000, 'region_count': 5}
    ]
    
    for config in configs:
        # Initialization benchmark
        benchmark_manager.run_benchmark(
            suite.name,
            lambda c=config: BrainBenchmarks.benchmark_initialization(c),
            f"initialization_{config['neuron_count']}_neurons",
            BenchmarkType.INITIALIZATION,
            config,
            ['initialization', 'neuron_count']
        )
        
        # Simulation benchmark
        benchmark_manager.run_benchmark(
            suite.name,
            lambda c=config: BrainBenchmarks.benchmark_simulation(c, 50),
            f"simulation_{config['neuron_count']}_neurons",
            BenchmarkType.SIMULATION_STEP,
            config,
            ['simulation', 'neuron_count']
        )
        
        # Memory usage benchmark
        benchmark_manager.run_benchmark(
            suite.name,
            lambda c=config: BrainBenchmarks.benchmark_memory_usage(c, 5.0),
            f"memory_usage_{config['neuron_count']}_neurons",
            BenchmarkType.MEMORY_OPERATIONS,
            config,
            ['memory', 'neuron_count']
        )
    
    # Integration benchmark
    benchmark_manager.run_benchmark(
        suite.name,
        lambda: BrainBenchmarks.benchmark_integration({'neuron_count': 1000}),
        "integration_test",
        BenchmarkType.INTEGRATION_TEST,
        {'neuron_count': 1000},
        ['integration', 'system']
    )
    
    return suite

def generate_benchmark_report(suite: BenchmarkSuite) -> Dict[str, Any]:
    """Generate a comprehensive benchmark report."""
    summary = suite.get_summary()
    
    report = {
        'summary': summary,
        'detailed_results': {},
        'recommendations': []
    }
    
    # Extract detailed results by type
    for benchmark_type in BenchmarkType:
        benchmarks = suite.get_benchmarks_by_type(benchmark_type)
        if benchmarks:
            report['detailed_results'][benchmark_type.value] = [
                {
                    'name': b.name,
                    'execution_time': b.execution_time,
                    'memory_usage': b.memory_usage,
                    'is_passed': b.is_passed,
                    'error_message': b.error_message
                }
                for b in benchmarks
            ]
    
    # Generate recommendations based on results
    for benchmark_type in BenchmarkType:
        benchmarks = suite.get_benchmarks_by_type(benchmark_type)
        if not benchmarks:
            continue
        
        passed_benchmarks = [b for b in benchmarks if b.is_passed]
        if not passed_benchmarks:
            continue
        
        avg_time = statistics.mean(b.execution_time for b in passed_benchmarks)
        avg_memory = statistics.mean(b.memory_usage for b in passed_benchmarks)
        
        if benchmark_type == BenchmarkType.SIMULATION_STEP:
            if avg_time > 0.1:  # Steps take too long
                report['recommendations'].append(
                    f"Simulation step time is high ({avg_time:.3f}s). "
                    "Consider optimizations for neuron processing."
                )
        
        elif benchmark_type == BenchmarkType.MEMORY_OPERATIONS:
            if avg_memory > 500:  # Memory usage is high
                report['recommendations'].append(
                    f"Memory usage is high ({avg_memory:.1f}MB). "
                    "Consider memory optimization strategies."
                )
    
    return report

def save_benchmark_report(report: Dict[str, Any], filepath: str):
    """Save benchmark report to file."""
    try:
        with open(filepath, 'w') as f:
            json.dump(report, f, indent=2)
        return True
    except Exception as e:
        print(f"Error saving benchmark report: {e}")
        return False