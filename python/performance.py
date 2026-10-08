#!/usr/bin/env python3
"""
Performance Monitoring and Optimization Tools for NLM Python Bindings

This module provides advanced profiling and optimization capabilities for
NLM simulations, including performance benchmarks, memory usage tracking,
and optimization recommendations.
"""

import time
import sys
from typing import Dict, List, Optional, Callable, Any
import statistics
from dataclasses import dataclass, field
from enum import Enum
import json

class PerformanceMetric(Enum):
    """Types of performance metrics"""
    SIMULATION_STEPS_PER_SECOND = "steps_per_second"
    MEMORY_USAGE_MB = "memory_mb"
    CPU_USAGE_PERCENT = "cpu_percent"
    BRAIN_CREATION_TIME_MS = "brain_creation_ms"
    SIMULATION_TIME_MS = "simulation_time_ms"
    ACTION_DECODING_TIME_MS = "action_decoding_ms"
    SENSORY_PROCESSING_TIME_MS = "sensory_processing_ms"
    ENERGY_USAGE = "energy_usage"
    SPIKE_RATE = "spike_rate"

@dataclass
class PerformanceSample:
    """A single performance measurement"""
    timestamp: float
    step: int
    metrics: Dict[PerformanceMetric, float] = field(default_factory=dict)
    
    def to_dict(self) -> Dict[str, Any]:
        return {
            "timestamp": self.timestamp,
            "step": self.step,
            "metrics": {metric.value: value for metric, value in self.metrics.items()}
        }

class PerformanceProfiler:
    """Performance profiler for NLM simulations"""
    
    def __init__(self):
        self.samples: List[PerformanceSample] = []
        self.start_time: Optional[float] = None
        self.current_step: int = 0
        self.benchmark_mode: bool = False
    
    def start(self):
        """Start profiling"""
        self.start_time = time.time()
        self.samples.clear()
        self.current_step = 0
    
    def stop(self) -> Dict[str, float]:
        """Stop profiling and return summary statistics"""
        if not self.start_time:
            return {}
        
        end_time = time.time()
        total_time = end_time - self.start_time
        
        if self.samples:
            steps_per_second = self.current_step / total_time
        else:
            steps_per_second = 0.0
        
        return {
            PerformanceMetric.SIMULATION_STEPS_PER_SECOND.value: steps_per_second,
            PerformanceMetric.SIMULATION_TIME_MS.value: total_time * 1000
        }
    
    def sample(self, brain=None, agent=None, world=None):
        """Take a performance sample"""
        if not self.start_time:
            return
        
        timestamp = time.time()
        sample = PerformanceSample(timestamp, self.current_step)
        
        # Collect metrics
        if brain:
            sample.metrics[PerformanceMetric.SPIKE_RATE.value] = brain.getAverageFiringRate()
            sample.metrics[PerformanceMetric.ACTION_DECODING_TIME_MS.value] = 0.0  # Would need timing
            
        if agent:
            sample.metrics[PerformanceMetric.CPU_USAGE_PERCENT.value] = 0.0  # Would need system calls
            
        if world:
            sample.metrics[PerformanceMetric.ENERGY_USAGE.value] = world.getAgentBody().energy
        
        self.samples.append(sample)
        self.current_step += 1
    
    def get_summary(self) -> Dict[str, Any]:
        """Get performance summary"""
        if not self.samples:
            return {}
        
        steps_per_second = self.current_step / (self.samples[-1].timestamp - self.samples[0].timestamp)
        
        spike_rates = [s.metrics.get(PerformanceMetric.SPIKE_RATE.value, 0.0) 
                      for s in self.samples if PerformanceMetric.SPIKE_RATE.value in s.metrics]
        
        energy_usage = [s.metrics.get(PerformanceMetric.ENERGY_USAGE.value, 0.0)
                       for s in self.samples if PerformanceMetric.ENERGY_USAGE.value in s.metrics]
        
        return {
            "total_samples": len(self.samples),
            "total_steps": self.current_step,
            "steps_per_second": steps_per_second,
            "average_spike_rate": statistics.mean(spike_rates) if spike_rates else 0.0,
            "max_spike_rate": max(spike_rates) if spike_rates else 0.0,
            "average_energy_usage": statistics.mean(energy_usage) if energy_usage else 0.0,
            "total_energy_usage": sum(energy_usage) if energy_usage else 0.0
        }
    
    def export_samples(self, filename: str):
        """Export samples to JSON file"""
        data = {
            "samples": [sample.to_dict() for sample in self.samples]
        }
        
        with open(filename, 'w') as f:
            json.dump(data, f, indent=2)
    
    def import_samples(self, filename: str):
        """Import samples from JSON file"""
        with open(filename, 'r') as f:
            data = json.load(f)
        
        self.samples = []
        for sample_data in data["samples"]:
            sample = PerformanceSample(
                sample_data["timestamp"],
                sample_data["step"]
            )
            sample.metrics = {
                PerformanceMetric(metric_value): value 
                for metric_value, value in sample_data["metrics"].items()
            }
            self.samples.append(sample)

class OptimizationRecommendations:
    """Generate optimization recommendations based on performance data"""
    
    def __init__(self, profiler: PerformanceProfiler):
        self.profiler = profiler
    
    def analyze_brain_configuration(self, brain) -> List[str]:
        """Analyze brain configuration for optimization opportunities"""
        recommendations = []
        
        # Check for underutilized neurons
        firing_rate = brain.getAverageFiringRate()
        if firing_rate < 1.0:
            recommendations.append(
                f"Low firing rate ({firing_rate:.2f} Hz). Consider increasing neuron count "
                "or adjusting connection parameters."
            )
        
        # Check for imbalance
        e_i_ratio = brain.getExcitationInhibitionRatio()
        if e_i_ratio < 0.5 or e_i_ratio > 2.0:
            recommendations.append(
                f"Excitation/inhibition ratio ({e_i_ratio:.2f}) is imbalanced. "
                "Consider adjusting plasticity parameters."
            )
        
        return recommendations
    
    def analyze_simulation_efficiency(self) -> List[str]:
        """Analyze simulation efficiency"""
        recommendations = []
        
        if self.profiler.samples:
            steps_per_second = self.profiler.get_summary().get("steps_per_second", 0)
            
            if steps_per_second < 10:
                recommendations.append(
                    f"Low simulation speed ({steps_per_second:.2f} steps/sec). "
                    "Consider reducing complexity or optimizing code."
                )
            
            spike_rates = [s.metrics.get(PerformanceMetric.SPIKE_RATE.value, 0.0) 
                          for s in self.profiler.samples 
                          if PerformanceMetric.SPIKE_RATE.value in s.metrics]
            
            if spike_rates:
                avg_spike_rate = statistics.mean(spike_rates)
                if avg_spike_rate > 50:
                    recommendations.append(
                        f"High spike rate ({avg_spike_rate:.2f} Hz). Consider reducing "
                        "neuron excitability or increasing refractory period."
                    )
        
        return recommendations
    
    def generate_all_recommendations(self, brain=None) -> List[str]:
        """Generate all optimization recommendations"""
        recommendations = []
        
        if brain:
            recommendations.extend(self.analyze_brain_configuration(brain))
        
        recommendations.extend(self.analyze_simulation_efficiency())
        
        if not recommendations:
            recommendations.append("Performance is within expected range.")
        
        return recommendations

class BenchmarkSuite:
    """Comprehensive benchmark suite for NLM performance testing"""
    
    def __init__(self):
        self.results = {}
        self.profiler = PerformanceProfiler()
    
    def benchmark_brain_creation(self, brain_factory: Callable[[], Any]):
        """Benchmark brain creation time"""
        print("Benchmarking brain creation...")
        
        times = []
        for i in range(10):
            start = time.time()
            brain = brain_factory()
            end = time.time()
            
            creation_time = (end - start) * 1000
            times.append(creation_time)
            
            if i == 0:
                brain.initialize()
        
        avg_time = statistics.mean(times)
        std_time = statistics.stdev(times) if len(times) > 1 else 0.0
        
        self.results["brain_creation"] = {
            "average_ms": avg_time,
            "std_ms": std_time,
            "min_ms": min(times),
            "max_ms": max(times),
            "times_ms": times
        }
        
        print(f"  Average creation time: {avg_time:.2f} ± {std_time:.2f} ms")
    
    def benchmark_simulation(self, brain, agent, world, steps: int = 100):
        """Benchmark simulation performance"""
        print(f"Benchmarking simulation with {steps} steps...")
        
        self.profiler.start()
        
        for step in range(steps):
            world.update(0.1)
            
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            
            brain.step(step)
            
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            
            self.profiler.sample(brain, agent, world)
        
        summary = self.profiler.stop()
        
        self.results["simulation"] = {
            "summary": summary,
            "samples": len(self.profiler.samples)
        }
        
        print(f"  Steps per second: {summary.get('steps_per_second', 0):.2f}")
        print(f"  Total time: {summary.get('simulation_time_ms', 0):.2f} ms")
    
    def benchmark_memory_usage(self):
        """Benchmark memory usage (simplified)"""
        print("Benchmarking memory usage...")
        
        # This is a simplified version - real implementation would use memory_profiler
        import resource
        
        start_memory = resource.getrusage(resource.RUSAGE_SELF).ru_maxrss
        
        # Create and destroy objects to measure overhead
        for i in range(100):
            brain = pynlm.createBrain("simple")
            brain.initialize()
            
            # Clean up
            del brain
        
        end_memory = resource.getrusage(resource.RUSAGE_SELF).ru_maxrss
        
        memory_diff = (end_memory - start_memory) / 1024  # Convert to MB
        
        self.results["memory"] = {
            "memory_diff_mb": memory_diff,
            "objects_created": 100
        }
        
        print(f"  Memory difference: {memory_diff:.2f} MB")
    
    def run_all_benchmarks(self):
        """Run all benchmarks"""
        print("=" * 50)
        print("NLM Performance Benchmark Suite")
        print("=" * 50)
        
        # Benchmark brain creation
        self.benchmark_brain_creation(lambda: pynlm.createBrain("medium"))
        
        # Benchmark simulation
        brain = pynlm.createBrain("medium")
        world = pynlm.createSimpleWorld()
        world.configure(20, 20, 8, 8)
        world.reset()
        agent = pynlm.createLearningAgent(brain)
        agent.initialize(world)
        
        self.benchmark_simulation(brain, agent, world, 50)
        
        # Benchmark memory usage
        self.benchmark_memory_usage()
        
        print("\n" + "=" * 50)
        print("Benchmark Results Summary")
        print("=" * 50)
        
        for benchmark, results in self.results.items():
            print(f"\n{benchmark.replace('_', ' ').title()}:")
            for key, value in results.items():
                if isinstance(value, dict):
                    continue
                if isinstance(value, list):
                    print(f"  {key}: {value[:3]}{'...' if len(value) > 3 else ''}")
                else:
                    print(f"  {key}: {value}")
    
    def save_results(self, filename: str):
        """Save benchmark results to file"""
        with open(filename, 'w') as f:
            json.dump(self.results, f, indent=2)
    
    def load_results(self, filename: str):
        """Load benchmark results from file"""
        with open(filename, 'r') as f:
            self.results = json.load(f)

# Global performance monitoring instance
_global_profiler = PerformanceProfiler()

def profile_brain_simulation(brain_factory, world_factory, steps: int = 100):
    """Convenience function for profiling brain simulations"""
    global _global_profiler
    
    brain = brain_factory()
    world = world_factory()
    agent = pynlm.createLearningAgent(brain)
    agent.initialize(world)
    
    _global_profiler.start()
    
    for step in range(steps):
        world.update(0.1)
        
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        brain.step(step)
        
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        _global_profiler.sample(brain, agent, world)
    
    summary = _global_profiler.stop()
    recommendations = OptimizationRecommendations(_global_profiler).generate_all_recommendations(brain)
    
    return {
        "summary": summary,
        "recommendations": recommendations,
        "profiler": _global_profiler
    }

def quick_benchmark():
    """Run a quick benchmark of common operations"""
    print("Quick Benchmark of NLM Operations")
    print("=" * 40)
    
    # Brain creation benchmark
    start = time.time()
    for i in range(10):
        brain = pynlm.createBrain("simple")
        brain.initialize()
    creation_time = (time.time() - start) * 1000
    
    print(f"Brain creation (10 iterations): {creation_time/10:.2f} ms avg")
    
    # Simulation benchmark
    brain = pynlm.createBrain("medium")
    world = pynlm.createSimpleWorld()
    world.configure(10, 10, 8, 8)
    world.reset()
    agent = pynlm.createLearningAgent(brain)
    agent.initialize(world)
    
    start = time.time()
    for step in range(50):
        world.update(0.1)
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
    
    simulation_time = (time.time() - start) * 1000
    print(f"Simulation (50 steps): {simulation_time/50:.2f} ms per step")
    
    return {
        "brain_creation_ms": creation_time / 10,
        "simulation_step_ms": simulation_time / 50
    }

if __name__ == "__main__":
    # Example usage
    print("Performance Monitoring and Optimization Tools")
    print("=" * 50)
    
    # Quick benchmark
    results = quick_benchmark()
    
    # Run comprehensive benchmarks
    suite = BenchmarkSuite()
    suite.run_all_benchmarks()
    
    print("\nAll performance tools completed successfully!")
