import time
import statistics
import matplotlib.pyplot as plt
import numpy as np
from typing import Dict, List, Any, Callable
from dataclasses import dataclass
from enum import Enum
import json

class BenchmarkType(Enum):
    STEP_TIME = "step_time"
    MEMORY_USAGE = "memory_usage"
    THROUGHPUT = "throughput"
    SCALABILITY = "scalability"
    PLOT_GENERATION = "plot_generation"

@dataclass
class BenchmarkResult:
    name: str
    benchmark_type: BenchmarkType
    value: float
    unit: str
    metadata: Dict[str, Any]
    timestamp: float

class NLMBenchmark:
    """Benchmark framework for NLM performance testing"""
    
    def __init__(self):
        self.results = []
        self.baseline_results = {}
    
    def benchmark_step_performance(self, brain, steps: int = 1000) -> BenchmarkResult:
        """Benchmark brain stepping performance"""
        start_time = time.time()
        
        for step in range(steps):
            brain.step(step)
        
        end_time = time.time()
        total_time = end_time - start_time
        steps_per_second = steps / total_time
        
        result = BenchmarkResult(
            name="step_performance",
            benchmark_type=BenchmarkType.STEP_TIME,
            value=steps_per_second,
            unit="steps_per_second",
            metadata={
                'steps': steps,
                'total_time': total_time,
                'avg_step_time': total_time / steps,
                'brain_size': brain.getTotalNeuronCount()
            },
            timestamp=time.time()
        )
        
        self.results.append(result)
        return result
    
    def benchmark_memory_usage(self, brain, steps: int = 500) -> BenchmarkResult:
        """Benchmark memory usage patterns"""
        import psutil
        import os
        
        process = psutil.Process(os.getpid())
        initial_memory = process.memory_info().rss / 1024 / 1024  # MB
        
        for step in range(steps):
            brain.step(step)
            
            # Force memory operations
            if brain.getWorkingMemory():
                brain.getWorkingMemory().storeToNeuron(step % 100, 0.1)
        
        final_memory = process.memory_info().rss / 1024 / 1024  # MB
        memory_increase = final_memory - initial_memory
        
        result = BenchmarkResult(
            name="memory_usage",
            benchmark_type=BenchmarkType.MEMORY_USAGE,
            value=memory_increase,
            unit="MB",
            metadata={
                'steps': steps,
                'initial_memory': initial_memory,
                'final_memory': final_memory,
                'brain_size': brain.getTotalNeuronCount()
            },
            timestamp=time.time()
        )
        
        self.results.append(result)
        return result
    
    def benchmark_simulation_throughput(self, brain, world, agent, steps: int = 200) -> BenchmarkResult:
        """Benchmark complete simulation throughput"""
        start_time = time.time()
        
        for step in range(steps):
            world.update(0.1)
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            brain.step(step)
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
        
        end_time = time.time()
        total_time = end_time - start_time
        steps_per_second = steps / total_time
        
        result = BenchmarkResult(
            name="simulation_throughput",
            benchmark_type=BenchmarkType.THROUGHPUT,
            value=steps_per_second,
            unit="steps_per_second",
            metadata={
                'steps': steps,
                'world_width': world.getWidth(),
                'world_height': world.getHeight(),
                'brain_size': brain.getTotalNeuronCount(),
                'neuron_count': brain.getTotalNeuronCount(),
                'synapse_count': brain.getTotalSynapseCount()
            },
            timestamp=time.time()
        )
        
        self.results.append(result)
        return result
    
    def benchmark_scalability(self, brain_sizes: List[int], steps_per_size: int = 100) -> List[BenchmarkResult]:
        """Benchmark scalability with different brain sizes"""
        results = []
        
        for size in brain_sizes:
            config = pynlm.createDefaultConfig()
            config.set("brain.neuron_count", size)
            config.set("brain.region_count", max(1, size // 500))
            
            brain = pynlm.createBrain(config)
            brain.initialize()
            
            # Measure performance
            start_time = time.time()
            
            for step in range(steps_per_size):
                brain.step(step)
            
            end_time = time.time()
            total_time = end_time - start_time
            steps_per_second = steps_per_size / total_time
            
            result = BenchmarkResult(
                name=f"scalability_size_{size}",
                benchmark_type=BenchmarkType.SCALABILITY,
                value=steps_per_second,
                unit="steps_per_second",
                metadata={
                    'brain_size': size,
                    'total_time': total_time,
                    'steps_per_second': steps_per_second
                },
                timestamp=time.time()
            )
            
            results.append(result)
            self.results.append(result)
        
        return results
    
    def set_baseline(self, baseline_name: str):
        """Set a baseline result for comparison"""
        self.baseline_results[baseline_name] = self.results.copy()
    
    def compare_to_baseline(self, baseline_name: str) -> Dict[str, Any]:
        """Compare current results to baseline"""
        if baseline_name not in self.baseline_results:
            raise ValueError(f"Baseline '{baseline_name}' not found")
        
        baseline = self.baseline_results[baseline_name]
        current = self.results
        
        comparison = {}
        
        # Group results by type
        baseline_by_type = {r.name: r for r in baseline}
        current_by_type = {r.name: r for r in current}
        
        for name in set(list(baseline_by_type.keys()) + list(current_by_type.keys())):
            if name in baseline_by_type and name in current_by_type:
                baseline_result = baseline_by_type[name]
                current_result = current_by_type[name]
                
                if baseline_result.benchmark_type == current_result.benchmark_type:
                    if baseline_result.benchmark_type == BenchmarkType.STEP_TIME:
                        # For step time, lower is better
                        improvement = ((baseline_result.value - current_result.value) / baseline_result.value) * 100
                        comparison[name] = {
                            'baseline': baseline_result.value,
                            'current': current_result.value,
                            'improvement_percent': improvement,
                            'is_improvement': improvement > 0
                        }
                    else:
                        # For other metrics, higher is better
                        improvement = ((current_result.value - baseline_result.value) / baseline_result.value) * 100
                        comparison[name] = {
                            'baseline': baseline_result.value,
                            'current': current_result.value,
                            'improvement_percent': improvement,
                            'is_improvement': improvement > 0
                        }
        
        return comparison
    
    def generate_report(self, output_file: str = "benchmark_report.json"):
        """Generate benchmark report"""
        report = {
            'timestamp': time.time(),
            'results': [
                {
                    'name': r.name,
                    'benchmark_type': r.benchmark_type.value,
                    'value': r.value,
                    'unit': r.unit,
                    'metadata': r.metadata,
                    'timestamp': r.timestamp
                }
                for r in self.results
            ],
            'summary': self._generate_summary()
        }
        
        with open(output_file, 'w') as f:
            json.dump(report, f, indent=2, default=str)
        
        return report
    
    def _generate_summary(self) -> Dict[str, Any]:
        """Generate summary statistics"""
        if not self.results:
            return {}
        
        by_type = {}
        for result in self.results:
            if result.benchmark_type not in by_type:
                by_type[result.benchmark_type] = []
            by_type[result.benchmark_type].append(result.value)
        
        summary = {}
        for benchmark_type, values in by_type.items():
            summary[benchmark_type.value] = {
                'count': len(values),
                'mean': statistics.mean(values),
                'median': statistics.median(values),
                'std': statistics.stdev(values) if len(values) > 1 else 0.0,
                'min': min(values),
                'max': max(values)
            }
        
        return summary
