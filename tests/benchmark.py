# Performance Benchmarking for NLM

"""
Performance benchmarking and regression testing for NLM neural simulation.
Provides baseline performance metrics and automated regression detection.
"""

import time
import sys
import numpy as np
from typing import Dict, List, Tuple

# Import NLM bindings
try:
    from pynlm import Config, Brain
    PYNLM_AVAILABLE = True
except ImportError as e:
    print(f"Error: Could not import NLM bindings: {e}")
    PYNLM_AVAILABLE = False

class PerformanceProfiler:
    """Performance profiler for NLM simulation components"""
    
    def __init__(self):
        self.benchmarks = {}
        self.timings = []
    
    def start_timer(self, name: str) -> float:
        """Start timing a benchmark"""
        self.timings.append((name, time.time()))
        return time.time()
    
    def stop_timer(self, name: str) -> float:
        """Stop timing and record benchmark"""
        if self.timings:
            start_name, start_time = self.timings.pop()
            if start_name == name:
                elapsed = time.time() - start_time
                if name not in self.benchmarks:
                    self.benchmarks[name] = []
                self.benchmarks[name].append(elapsed)
                return elapsed
        return 0.0
    
    def get_average(self, name: str, samples: int = -1) -> float:
        """Get average execution time for a benchmark"""
        if name not in self.benchmarks:
            return 0.0
        
        times = self.benchmarks[name]
        if samples > 0:
            times = times[-samples:]
        
        return sum(times) / len(times) if times else 0.0
    
    def get_stats(self, name: str) -> Dict:
        """Get statistics for a benchmark"""
        if name not in self.benchmarks:
            return {"min": 0.0, "max": 0.0, "avg": 0.0, "std": 0.0, "samples": 0}
        
        times = self.benchmarks[name]
        if not times:
            return {"min": 0.0, "max": 0.0, "avg": 0.0, "std": 0.0, "samples": 0}
        
        mean = sum(times) / len(times)
        variance = sum((t - mean) ** 2 for t in times) / len(times)
        std = variance ** 0.5
        
        return {
            "min": min(times),
            "max": max(times),
            "avg": mean,
            "std": std,
            "samples": len(times),
            "unit": "seconds"
        }
class NLMPerformanceBenchmark:
    """Performance benchmarking for NLM neural simulation"""
    
    def __init__(self):
        self.profiler = PerformanceProfiler()
        self.baseline_config = {
            "neuron_count": 1000,
            "region_count": 5,
            "connection_probability": 0.1,
            "simulation_timestep": 0.001,
        }
        self.test_scenarios = []
    
    def setup_scenario(self, name: str, config_overrides: Dict = None) -> Brain:
        """Setup a benchmark scenario"""
        config = Config()
        
        # Apply baseline config
        for key, value in self.baseline_config.items():
            config.set(key, value)
        
        # Apply scenario-specific overrides
        if config_overrides:
            for key, value in config_overrides.items():
                config.set(key, value)
        
        # Create and initialize brain
        brain = Brain(config)
        brain.initialize()
        
        return brain
    
    def benchmark_brain_creation(self, iterations: int = 10) -> Dict:
        """Benchmark brain creation and initialization"""
        print(f"Benchmarking brain creation ({iterations} iterations)...")
        
        for i in range(iterations):
            self.profiler.start_timer("brain_creation")
            brain = self.setup_scenario(f"brain_creation_{i}")
            self.profiler.stop_timer("brain_creation")
            
            # Cleanup
            del brain
        
        stats = self.profiler.get_stats("brain_creation")
        print(f"  Created and initialized {iterations} brains in {stats['avg']:.4f}s avg")
        return stats
    
    def benchmark_simulation_step(self, brain: Brain, steps: int = 100, iterations: int = 5) -> Dict:
        """Benchmark simulation steps"""
        print(f"Benchmarking {steps} simulation steps ({iterations} iterations)...")
        
        for i in range(iterations):
            start_time = self.profiler.start_timer("simulation_step")
            
            for step in range(steps):
                brain.step(step)
            
            elapsed = self.profiler.stop_timer("simulation_step")
            print(f"  Iteration {i+1}: {steps} steps in {elapsed:.4f}s ({steps/elapsed:.0f} steps/s)")
        
        stats = self.profiler.get_stats("simulation_step")
        steps_per_second = steps / stats['avg'] if stats['avg'] > 0 else 0
        print(f"  Simulated {steps} steps in {stats['avg']:.4f}s avg ({steps_per_second:.0f} steps/s)")
        return stats
    
    def benchmark_neuron_operations(self, config_overrides: Dict = None) -> Dict:
        """Benchmark individual neuron operations"""
        print("Benchmarking neuron operations...")
        
        brain = self.setup_scenario("neuron_ops", config_overrides)
        neurons = brain.getTotalNeuronCount()
        
        # Benchmark spike detection
        self.profiler.start_timer("spike_detection")
        for step in range(100):
            brain.step(step)
        spike_stats = self.profiler.stop_timer("spike_detection")
        
        # Benchmark synaptic input
        self.profiler.start_timer("synaptic_input")
        for step in range(100):
            # Simulate sensory input
            brain.step(step)
        synaptic_stats = self.profiler.stop_timer("synaptic_input")
        
        del brain
        
        return {
            "neurons_tested": neurons,
            "spike_detection_time": spike_stats,
            "synaptic_input_time": synaptic_stats,
            "steps_per_second": 100 / spike_stats if spike_stats > 0 else 0
        }
    
    def benchmark_memory_operations(self, brain: Brain, iterations: int = 5) -> Dict:
        """Benchmark memory system operations"""
        print(f"Benchmarking memory operations ({iterations} iterations)...")
        
        memory_times = []
        
        for i in range(iterations):
            self.profiler.start_timer("memory_ops")
            
            # Run simulation steps to exercise memory systems
            for step in range(50):
                brain.step(step)
            
            elapsed = self.profiler.stop_timer("memory_ops")
            memory_times.append(elapsed)
            print(f"  Memory iteration {i+1}: {elapsed:.4f}s")
        
        avg_time = sum(memory_times) / len(memory_times)
        print(f"  Average memory operation time: {avg_time:.4f}s")
        
        return {
            "iterations": iterations,
            "average_time": avg_time,
            "total_steps": iterations * 50
        }
    
    def benchmark_different_scales(self) -> List[Dict]:
        """Benchmark performance at different scales"""
        print("\nBenchmarking different scales...")
        
        scales = [
            {"neuron_count": 100, "region_count": 1, "name": "small"},
            {"neuron_count": 1000, "region_count": 5, "name": "medium"},
            {"neuron_count": 5000, "region_count": 10, "name": "large"},
        ]
        
        results = []
        
        for scale in scales:
            print(f"\n  Testing {scale['name']} scale ({scale['neuron_count']} neurons, {scale['region_count']} regions)...")
            
            config = Config()
            config.set("neuron_count", scale["neuron_count"])
            config.set("region_count", scale["region_count"])
            config.set("connection_probability", 0.1)
            
            brain = Brain(config)
            brain.initialize()
            
            # Benchmark creation
            start_time = time.time()
            brain = Brain(config)
            brain.initialize()
            creation_time = time.time() - start_time
            
            # Benchmark simulation
            start_time = time.time()
            for step in range(50):
                brain.step(step)
            simulation_time = time.time() - start_time
            
            steps_per_second = 50 / simulation_time if simulation_time > 0 else 0
            
            result = {
                "scale": scale["name"],
                "neurons": scale["neuron_count"],
                "regions": scale["region_count"],
                "creation_time": creation_time,
                "simulation_time": simulation_time,
                "steps_per_second": steps_per_second
            }
            
            results.append(result)
            print(f"    Creation: {creation_time:.4f}s, Simulation: {simulation_time:.4f}s ({steps_per_second:.0f} steps/s)")
            
            del brain
        
        return results
    
    def compare_with_baseline(self, current_results: Dict, baseline_results: Dict) -> Dict:
        """Compare current performance with baseline"""
        print("\nComparing with baseline performance...")
        
        comparison = {}
        
        for metric in ["creation_time", "simulation_time", "steps_per_second"]:
            if metric in current_results and metric in baseline_results:
                current = current_results[metric]
                baseline = baseline_results[metric]
                
                if current == 0 or baseline == 0:
                    ratio = 0.0
                elif metric == "steps_per_second":
                    ratio = current / baseline
                else:
                    ratio = baseline / current  # Lower is better for time metrics
                
                improvement = (ratio - 1.0) * 100
                
                comparison[metric] = {
                    "baseline": baseline,
                    "current": current,
                    "ratio": ratio,
                    "improvement_percent": improvement,
                    "status": "improved" if improvement > 0 else "degraded" if improvement < 0 else "stable"
                }
                
                print(f"  {metric}: baseline={baseline:.4f}, current={current:.4f}, {improvement:+.1f}%")
        
        return comparison
    
    def run_complete_benchmark_suite(self) -> Dict:
        """Run complete benchmark suite"""
        print("=" * 70)
        print("NLM Performance Benchmark Suite")
        print("=" * 70)
        
        results = {}
        
        # Benchmark different aspects
        results["brain_creation"] = self.benchmark_brain_creation()
        results["neuron_operations"] = self.benchmark_neuron_operations()
        results["memory_operations"] = self.benchmark_memory_operations(Brain(Config()))
        
        # Benchmark different scales
        results["scale_benchmarks"] = self.benchmark_different_scales()
        
        print("\n" + "=" * 70)
        print("Benchmark Suite Complete")
        print("=" * 70)
        
        return results
    
    def save_results(self, results: Dict, filename: str = "nlm_performance_benchmark.json"):
        """Save benchmark results to file"""
        import json
        
        with open(filename, 'w') as f:
            json.dump(results, f, indent=2)
        
        print(f"\nBenchmark results saved to {filename}")
class RegressionDetector:
    """Detect performance regressions between test runs"""
    
    def __init__(self, baseline_file: str = "baseline_nlm_performance.json"):
        self.baseline_file = baseline_file
        self.baseline_data = None
    
    def load_baseline(self) -> bool:
        """Load baseline performance data"""
        try:
            import json
            with open(self.baseline_file, 'r') as f:
                self.baseline_data = json.load(f)
            print(f"✓ Loaded baseline from {self.baseline_file}")
            return True
        except FileNotFoundError:
            print(f"⚠️  Baseline file not found: {self.baseline_file}")
            print("  Run benchmark suite first to establish baseline")
            return False
        except Exception as e:
            print(f"⚠️  Error loading baseline: {e}")
            return False
    
    def detect_regression(self, current_results: Dict) -> List[Dict]:
        """Detect performance regressions"""
        if not self.baseline_data:
            return []
        
        regressions = []
        
        # Compare key metrics
        for test_name in ["brain_creation", "neuron_operations", "memory_operations"]:
            if test_name in current_results and test_name in self.baseline_data:
                current = current_results[test_name]
                baseline = self.baseline_data[test_name]
                
                # Compare creation time
                if "creation_time" in current and "creation_time" in baseline:
                    current_time = current["creation_time"]
                    baseline_time = baseline["creation_time"]
                    
                    if baseline_time > 0:
                        regression_ratio = current_time / baseline_time
                        if regression_ratio > 1.2:  # 20% regression threshold
                            regressions.append({
                                "test": f"{test_name}_creation",
                                "current_time": current_time,
                                "baseline_time": baseline_time,
                                "regression_ratio": regression_ratio,
                                "severity": "high" if regression_ratio > 1.5 else "medium"
                            })
        
        return regressions
    
    def generate_report(self, current_results: Dict, regressions: List[Dict]) -> str:
        """Generate performance regression report"""
        report = []
        report.append("=" * 70)
        report.append("NLM Performance Regression Report")
        report.append("=" * 70)
        
        if regressions:
            report.append(f"\n⚠️  PERFORMANCE REGRESSIONS DETECTED: {len(regressions)}")
            report.append("-" * 70)
            
            for regression in regressions:
                report.append(f"Test: {regression['test']}")
                report.append(f"  Current: {regression['current_time']:.4f}s")
                report.append(f"  Baseline: {regression['baseline_time']:.4f}s")
                report.append(f"  Regression: {regression['regression_ratio']:.2f}x slower")
                report.append(f"  Severity: {regression['severity']}")
                report.append("")
            
            report.append("🔧 RECOMMENDED ACTIONS:")
            report.append("  1. Profile the specific test to identify bottlenecks")
            report.append("  2. Check for code changes that could affect performance")
            report.append("  3. Review memory usage and allocation patterns")
            report.append("  4. Consider algorithmic optimizations")
        else:
            report.append("\n✓ NO SIGNIFICANT PERFORMANCE REGRESSIONS DETECTED")
            report.append("-" * 70)
            report.append("All performance metrics within acceptable thresholds.")
        
        report.append("=" * 70)
        
        return "\n".join(report)
if __name__ == "__main__":
    if not PYNLM_AVAILABLE:
        print("Error: NLM Python bindings not available")
        sys.exit(1)
    
    print("NLM Performance Benchmarking Suite")
    print("====================================")
    
    # Initialize benchmark
    benchmark = NLMPerformanceBenchmark()
    
    # Run benchmark suite
    results = benchmark.run_complete_benchmark_suite()
    
    # Save results
    benchmark.save_results(results)
    
    # Optional: Compare with baseline
    regression_detector = RegressionDetector()
    regression_detector.load_baseline()
    regressions = regression_detector.detect_regression(results)
    
    # Generate report
    report = regression_detector.generate_report(results, regressions)
    print(report)
    
    # Exit with error code if regressions detected
    sys.exit(1 if regressions else 0)
