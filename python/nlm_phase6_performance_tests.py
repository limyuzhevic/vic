#!/usr/bin/env python3
"""
NLM Phase 6 Integration Test Suite - Performance Tests

Comprehensive performance tests for NLM Phase 6 integration components.
"""

import sys
import os
import time
import json
import tempfile
import shutil
import numpy as np
from typing import Dict, Any, List, Optional, Tuple
from dataclasses import dataclass
from enum import Enum
import psutil
import threading
from concurrent.futures import ThreadPoolExecutor

sys.path.insert(0, os.path.join(os.path.dirname(__file__), 'python'))

class PerformanceTestType(Enum):
    """Types of performance tests for NLM Phase 6"""
    THROUGHPUT_TEST = "throughput_test"
    LATENCY_TEST = "latency_test"
    MEMORY_EFFICIENCY_TEST = "memory_efficiency_test"
    CPU_INTENSITY_TEST = "cpu_intensity_test"
    SCALABILITY_TEST = "scalability_test"
    COHERENCE_TEST = "coherence_test"
    REAL_TIME_TEST = "real_time_test"

class PerformanceBenchmark:
    """Benchmark metrics for NLM Phase 6 performance tests"""
    
    def __init__(self):
        self.benchmark_name: str = ""
        self.test_type: PerformanceTestType = PerformanceTestType.THROUGHPUT_TEST
        self.neuron_count: int = 0
        self.config: Dict[str, Any] = {}
        self.results: Dict[str, Any] = {}
        self.start_time: float = 0.0
        self.end_time: float = 0.0
        self.metrics: Dict[str, List[float]] = {}
        
    def start_benchmark(self):
        """Start the benchmark"""
        self.start_time = time.time()
        
    def stop_benchmark(self):
        """Stop the benchmark and calculate results"""
        self.end_time = time.time()
        self.results["total_duration"] = self.end_time - self.start_time
        self.results["total_steps"] = len(self.metrics.get("steps", []))
        
    def add_metric(self, metric_name: str, value: float):
        """Add a metric measurement"""
        if metric_name not in self.metrics:
            self.metrics[metric_name] = []
        self.metrics[metric_name].append(value)
        
    def get_summary(self) -> Dict[str, Any]:
        """Get benchmark summary statistics"""
        summary = {
            "benchmark_name": self.benchmark_name,
            "test_type": self.test_type.value,
            "neuron_count": self.neuron_count,
            "config": self.config,
            "results": self.results
        }
        
        # Calculate statistics for each metric
        for metric_name, values in self.metrics.items():
            if values:
                summary[f"{metric_name}_stats"] = {
                    "min": min(values),
                    "max": max(values),
                    "mean": sum(values) / len(values),
                    "std": np.std(values) if len(values) > 1 else 0.0,
                    "count": len(values)
                }
        
        return summary

class PerformanceTestSuite:
    """Comprehensive performance test suite for NLM Phase 6 integration"""
    
    def __init__(self, temp_dir: str):
        self.temp_dir = temp_dir
        self.benchmark_results: List[Benchmark] = []
        self.performance_profiler = PerformanceProfiler()
        
    def run_throughput_test(self, neuron_count: int, steps: int) -> Benchmark:
        """Test system throughput with specified neuron count and steps"""
        print(f"🚀 Running throughput test with {neuron_count} neurons, {steps} steps...")
        
        benchmark = Benchmark()
        benchmark.benchmark_name = f"throughput_test_{neuron_count}_neurons"
        benchmark.test_type = PerformanceTestType.THROUGHPUT_TEST
        benchmark.neuron_count = neuron_count
        
        # Configure system for throughput test
        config = self._create_config_for_test(neuron_count, steps)
        
        benchmark.config = config
        benchmark.start_benchmark()
        
        try:
            # Initialize performance profiler
            self.performance_profiler.start_profiling("system_initialization")
            
            # Initialize NLM system
            brain = self._initialize_brain(config)
            self.performance_profiler.stop_profiling("system_initialization")
            
            # Run simulation
            steps_completed = 0
            throughput_samples = []
            memory_samples = []
            cpu_samples = []
            
            for step in range(steps):
                step_start = time.time()
                
                # Record system state
                memory_usage = psutil.virtual_memory().used / (1024 * 1024)  # MB
                cpu_percent = psutil.cpu_percent()
                
                memory_samples.append(memory_usage)
                cpu_samples.append(cpu_percent)
                
                # Execute simulation step
                brain.step(step, step * 0.001)
                
                # Record metrics
                self.performance_profiler.start_profiling(f"step_{step}")
                step_duration = time.time() - step_start
                throughput_samples.append(1.0 / step_duration if step_duration > 0 else 0)
                self.performance_profiler.stop_profiling(f"step_{step}")
                
                steps_completed += 1
                
                # Add metrics to benchmark
                benchmark.add_metric("steps_completed", steps_completed)
                benchmark.add_metric("step_duration", step_duration)
                benchmark.add_metric("memory_usage", memory_usage)
                benchmark.add_metric("cpu_percent", cpu_percent)
                
                # Log progress every 10% of steps
                if step % (steps // 10) == 0:
                    print(f"   Progress: {((step + 1) / steps) * 100:.1f}%, "
                          f"Steps: {steps_completed}, "
                          f"Avg Duration: {step_duration:.3f}s")
            
            benchmark.stop_benchmark()
            
            # Calculate throughput statistics
            avg_throughput = sum(throughput_samples) / len(throughput_samples) if throughput_samples else 0
            avg_memory = sum(memory_samples) / len(memory_samples) if memory_samples else 0
            avg_cpu = sum(cpu_samples) / len(cpu_samples) if cpu_samples else 0
            
            benchmark.results["avg_throughput_hz"] = avg_throughput
            benchmark.results["total_steps_completed"] = steps_completed
            benchmark.results["avg_memory_usage_mb"] = avg_memory
            benchmark.results["avg_cpu_percent"] = avg_cpu
            benchmark.results["min_step_duration"] = min(1.0 / max(t, 1e-6) for t in throughput_samples) if throughput_samples else 0
            
            print(f"✅ Throughput test completed: {steps_completed} steps, "
                  f"avg throughput {avg_throughput:.2f} Hz, "
                  f"avg memory {avg_memory:.2f} MB")
            
        except Exception as e:
            print(f"❌ Throughput test failed: {e}")
            benchmark.results["error"] = str(e)
            benchmark.stop_benchmark()
        
        return benchmark
        
    def run_latency_test(self, neuron_count: int, sample_size: int = 100) -> Benchmark:
        """Test system latency with specified neuron count"""
        print(f"⏱️ Running latency test with {neuron_count} neurons, {sample_size} samples...")
        
        benchmark = Benchmark()
        benchmark.benchmark_name = f"latency_test_{neuron_count}_neurons"
        benchmark.test_type = PerformanceTestType.LATENCY_TEST
        benchmark.neuron_count = neuron_count
        
        # Configure system for latency test
        config = self._create_config_for_test(neuron_count, sample_size)
        
        benchmark.config = config
        benchmark.start_benchmark()
        
        try:
            # Initialize system
            brain = self._initialize_brain(config)
            
            # Collect latency samples
            latencies = []
            memory_usage_samples = []
            cpu_usage_samples = []
            
            for i in range(sample_size):
                # Measure system state before operation
                memory_before = psutil.virtual_memory().used / (1024 * 1024)  # MB
                cpu_before = psutil.cpu_percent()
                
                # Record operation start time
                op_start = time.time()
                
                # Execute operation
                brain.step(i, i * 0.001)
                
                # Calculate operation duration
                op_duration = time.time() - op_start
                
                # Measure system state after operation
                memory_after = psutil.virtual_memory().used / (1024 * 1024)  # MB
                cpu_after = psutil.cpu_percent()
                
                # Calculate memory delta
                memory_delta = abs(memory_after - memory_before)
                
                # Record samples
                latencies.append(op_duration)
                memory_usage_samples.append(memory_delta)
                cpu_usage_samples.append(cpu_after - cpu_before)
                
                # Add metrics to benchmark
                benchmark.add_metric("latency", op_duration)
                benchmark.add_metric("memory_delta", memory_delta)
                benchmark.add_metric("cpu_delta", cpu_after - cpu_before)
                
                if i % 20 == 0:
                    print(f"   Sample {i+1}/{sample_size}, "
                          f"Latency: {op_duration*1000:.2f}ms, "
                          f"Memory Delta: {memory_delta:.2f}MB")
            
            benchmark.stop_benchmark()
            
            # Calculate latency statistics
            avg_latency = sum(latencies) / len(latencies) if latencies else 0
            p95_latency = np.percentile(latencies, 95) if latencies else 0
            p99_latency = np.percentile(latencies, 99) if latencies else 0
            min_latency = min(latencies) if latencies else 0
            max_latency = max(latencies) if latencies else 0
            std_latency = np.std(latencies) if len(latencies) > 1 else 0
            
            benchmark.results["avg_latency_ms"] = avg_latency * 1000
            benchmark.results["p95_latency_ms"] = p95_latency * 1000
            benchmark.results["p99_latency_ms"] = p99_latency * 1000
            benchmark.results["min_latency_ms"] = min_latency * 1000
            benchmark.results["max_latency_ms"] = max_latency * 1000
            benchmark.results["std_latency_ms"] = std_latency * 1000
            benchmark.results["avg_memory_delta_mb"] = sum(memory_usage_samples) / len(memory_usage_samples) if memory_usage_samples else 0
            benchmark.results["avg_cpu_delta_percent"] = sum(cpu_usage_samples) / len(cpu_usage_samples) if cpu_usage_samples else 0
            
            print(f"✅ Latency test completed: avg {avg_latency*1000:.2f}ms, "
                  f"P95 {p95_latency*1000:.2f}ms, "
                  f"P99 {p99_latency*1000:.2f}ms")
            
        except Exception as e:
            print(f"❌ Latency test failed: {e}")
            benchmark.results["error"] = str(e)
            benchmark.stop_benchmark()
        
        return benchmark
        
    def run_memory_efficiency_test(self, neuron_count: int, steps: int) -> Benchmark:
        """Test memory efficiency with specified neuron count and steps"""
        print(f"💾 Running memory efficiency test with {neuron_count} neurons, {steps} steps...")
        
        benchmark = Benchmark()
        benchmark.benchmark_name = f"memory_efficiency_test_{neuron_count}_neurons"
        benchmark.test_type = PerformanceTestType.MEMORY_EFFICIENCY_TEST
        benchmark.neuron_count = neuron_count
        
        # Configure system for memory efficiency test
        config = self._create_config_for_test(neuron_count, steps)
        config.set("enable_checkpointing", True)
        config.set("checkpoint_interval", max(1, steps // 10))
        
        benchmark.config = config
        benchmark.start_benchmark()
        
        try:
            # Initialize system
            brain = self._initialize_brain(config)
            
            # Collect memory metrics
            memory_samples = []
            checkpoint_times = []
            checkpoint_sizes = []
            
            for step in range(steps):
                # Measure memory before operation
                memory_before = psutil.virtual_memory().used / (1024 * 1024)  # MB
                
                # Execute operation
                brain.step(step, step * 0.001)
                
                # Measure memory after operation
                memory_after = psutil.virtual_memory().used / (1024 * 1024)  # MB
                
                # Calculate memory delta
                memory_delta = memory_after - memory_before
                memory_samples.append(memory_delta)
                
                # Perform checkpoint at specified intervals
                if step % config.get("checkpoint_interval", 100) == 0 and step > 0:
                    checkpoint_start = time.time()
                    
                    # Save checkpoint
                    brain.saveCheckpoint()
                    
                    checkpoint_time = time.time() - checkpoint_start
                    checkpoint_size = len(str(brain)) / (1024 * 1024)  # MB (approximate)
                    
                    checkpoint_times.append(checkpoint_time)
                    checkpoint_sizes.append(checkpoint_size)
                    
                    print(f"   Checkpoint at step {step}: {checkpoint_time:.3f}s, "
                          f"{checkpoint_size:.2f}MB")
            
            benchmark.stop_benchmark()
            
            # Calculate memory efficiency statistics
            total_memory_change = sum(memory_samples)
            avg_memory_change = sum(memory_samples) / len(memory_samples) if memory_samples else 0
            max_memory_increase = max(memory_samples) if memory_samples else 0
            min_memory_increase = min(memory_samples) if memory_samples else 0
            memory_efficiency_score = 1.0 / (1.0 + abs(total_memory_change))
            
            benchmark.results["total_memory_change_mb"] = total_memory_change
            benchmark.results["avg_memory_change_mb"] = avg_memory_change
            benchmark.results["max_memory_increase_mb"] = max_memory_increase
            benchmark.results["min_memory_increase_mb"] = min_memory_increase
            benchmark.results["memory_efficiency_score"] = memory_efficiency_score
            
            if checkpoint_times:
                avg_checkpoint_time = sum(checkpoint_times) / len(checkpoint_times)
                avg_checkpoint_size = sum(checkpoint_sizes) / len(checkpoint_sizes)
                
                benchmark.results["avg_checkpoint_time_ms"] = avg_checkpoint_time * 1000
                benchmark.results["avg_checkpoint_size_mb"] = avg_checkpoint_size
                benchmark.results["checkpoints_created"] = len(checkpoint_times)
            
            print(f"✅ Memory efficiency test completed: total change {total_memory_change:.2f}MB, "
                  f"efficiency score {memory_efficiency_score:.3f}")
            
        except Exception as e:
            print(f"❌ Memory efficiency test failed: {e}")
            benchmark.results["error"] = str(e)
            benchmark.stop_benchmark()
        
        return benchmark
        
    def run_scalability_test(self, neuron_counts: List[int], steps: int) -> List[Benchmark]:
        """Test system scalability across different neuron counts"""
        print(f"📈 Running scalability test across {len(neuron_counts)} neuron counts...")
        
        benchmarks = []
        
        for neuron_count in neuron_counts:
            print(f"\n📊 Testing scalability with {neuron_count} neurons...")
            
            benchmark = self.run_throughput_test(neuron_count, steps)
            benchmarks.append(benchmark)
            
            # Small delay between tests
            time.sleep(1)
        
        # Analyze scalability results
        self._analyze_scalability_results(benchmarks)
        
        return benchmarks
        
    def run_coherence_test(self, neuron_count: int, steps: int) -> Benchmark:
        """Test system coherence across different integration levels"""
        print(f"🔗 Running coherence test with {neuron_count} neurons, {steps} steps...")
        
        benchmark = Benchmark()
        benchmark.benchmark_name = f"coherence_test_{neuron_count}_neurons"
        benchmark.test_type = PerformanceTestType.COHERENCE_TEST
        benchmark.neuron_count = neuron_count
        
        # Configure system for coherence test
        config = self._create_config_for_test(neuron_count, steps)
        
        benchmark.config = config
        benchmark.start_benchmark()
        
        try:
            # Initialize system
            brain = self._initialize_brain(config)
            
            # Collect coherence metrics
            coherence_samples = []
            interaction_samples = []
            feedback_samples = []
            system_state_samples = []
            
            for step in range(steps):
                # Get current system state
                system_state = self._get_system_state(brain)
                system_state_samples.append(system_state)
                
                # Measure system coherence
                coherence = self._calculate_system_coherence(brain)
                coherence_samples.append(coherence)
                
                # Measure cross-system interactions
                interactions = self._count_cross_system_interactions(brain)
                interaction_samples.append(interactions)
                
                # Measure feedback loops
                feedback = self._measure_system_feedback(brain)
                feedback_samples.append(feedback)
                
                # Step brain
                brain.step(step, step * 0.001)
                
                # Log progress
                if step % (steps // 10) == 0:
                    print(f"   Progress: {((step + 1) / steps) * 100:.1f}%, "
                          f"Coherence: {coherence:.3f}, "
                          f"Interactions: {interactions}")
            
            benchmark.stop_benchmark()
            
            # Calculate coherence statistics
            avg_coherence = sum(coherence_samples) / len(coherence_samples) if coherence_samples else 0
            avg_interactions = sum(interaction_samples) / len(interaction_samples) if interaction_samples else 0
            avg_feedback = sum(feedback_samples) / len(feedback_samples) if feedback_samples else 0
            
            # Calculate system state diversity
            if system_state_samples:
                state_values = list(system_state_samples.values()) if isinstance(system_state_samples, dict) else system_state_samples
                state_diversity = np.std(state_values) if len(state_values) > 1 else 0
            else:
                state_diversity = 0
            
            benchmark.results["avg_coherence_score"] = avg_coherence
            benchmark.results["avg_cross_system_interactions"] = avg_interactions
            benchmark.results["avg_feedback_strength"] = avg_feedback
            benchmark.results["system_state_diversity"] = state_diversity
            benchmark.results["coherence_trend"] = self._analyze_coherence_trend(coherence_samples)
            
            print(f"✅ Coherence test completed: avg coherence {avg_coherence:.3f}, "
                  f"interactions {avg_interactions:.1f}, "
                  f"feedback {avg_feedback:.3f}")
            
        except Exception as e:
            print(f"❌ Coherence test failed: {e}")
            benchmark.results["error"] = str(e)
            benchmark.stop_benchmark()
        
        return benchmark
        
    def run_performance_comparison_test(self) -> List[Benchmark]:
        """Run comprehensive performance comparison test"""
        print("📊 Running comprehensive performance comparison test...")
        
        # Define test configurations
        test_configurations = [
            {"neuron_count": 100, "steps": 500, "name": "small_scale"},
            {"neuron_count": 500, "steps": 1000, "name": "medium_scale"},
            {"neuron_count": 1000, "steps": 500, "name": "large_scale_dense"}
        ]
        
        benchmarks = []
        
        for config in test_configurations:
            print(f"\n🎯 Running {config['name']} performance test...")
            
            # Run throughput test
            throughput_benchmark = self.run_throughput_test(
                config["neuron_count"], 
                config["steps"]
            )
            throughput_benchmark.benchmark_name = f"throughput_{config['name']}"
            benchmarks.append(throughput_benchmark)
            
            # Run latency test
            latency_benchmark = self.run_latency_test(
                config["neuron_count"],
                min(100, config["steps"] // 10)
            )
            latency_benchmark.benchmark_name = f"latency_{config['name']}"
            benchmarks.append(latency_benchmark)
            
            # Run memory efficiency test
            memory_benchmark = self.run_memory_efficiency_test(
                config["neuron_count"],
                min(200, config["steps"] // 5)
            )
            memory_benchmark.benchmark_name = f"memory_efficiency_{config['name']}"
            benchmarks.append(memory_benchmark)
            
            # Run coherence test
            coherence_benchmark = self.run_coherence_test(
                config["neuron_count"],
                min(100, config["steps"] // 10)
            )
            coherence_benchmark.benchmark_name = f"coherence_{config['name']}"
            benchmarks.append(coherence_benchmark)
            
            # Delay between tests
            time.sleep(2)
        
        return benchmarks
        
    def _create_config_for_test(self, neuron_count: int, steps: int) -> Dict[str, Any]:
        """Create configuration for performance test"""
        return {
            "neuron_count": neuron_count,
            "region_count": max(1, neuron_count // 100),
            "connection_probability": 0.1,
            "enable_working_memory": True,
            "enable_episodic_memory": True,
            "enable_associative_memory": True,
            "enable_dopamine": True,
            "enable_curiosity": True,
            "enable_novelty": True,
            "enable_prediction": True,
            "enable_prediction_error": True,
            "enable_attention": True,
            "enable_planning": True,
            "enable_concept_formation": True,
            "enable_development": True,
            "enable_replay": True,
            "enable_checkpointing": True,
            "max_steps": steps,
            "simulation_speed": 1.0,
            "random_seed": 42
        }
        
    def _initialize_brain(self, config: Dict[str, Any]):
        """Initialize brain for performance test"""
        # This is a placeholder - actual implementation would depend on NLM API
        # For now, return a mock brain object
        return MockBrain(config)
        
    def _get_system_state(self, brain) -> Dict[str, float]:
        """Get current system state"""
        # This is a placeholder - actual implementation would depend on NLM API
        # For now, return a mock system state
        return {
            "working_memory": 50.0,
            "episodic_memory": 30.0,
            "dopamine": 1.0,
            "curiosity": 0.8,
            "attention": 0.7,
            "prediction": 0.9,
            "planning": 0.6,
            "development": 0.5
        }
        
    def _calculate_system_coherence(self, brain) -> float:
        """Calculate system coherence"""
        # This is a placeholder - actual implementation would depend on NLM API
        # For now, return a mock coherence score
        system_state = self._get_system_state(brain)
        values = list(system_state.values())
        
        if len(values) < 2:
            return 1.0
            
        std_dev = np.std(values)
        coherence = 1.0 / (1.0 + std_dev)
        
        return coherence
        
    def _count_cross_system_interactions(self, brain) -> int:
        """Count cross-system interactions"""
        # This is a placeholder - actual implementation would depend on NLM API
        # For now, return a mock count
        return 5
        
    def _measure_system_feedback(self, brain) -> float:
        """Measure system feedback strength"""
        # This is a placeholder - actual implementation would depend on NLM API
        # For now, return a mock value
        return 0.7
        
    def _analyze_scalability_results(self, benchmarks: List[Benchmark]):
        """Analyze scalability test results"""
        print("\n" + "="*80)
        print("📈 SCALABILITY ANALYSIS")
        print("="*80)
        
        if len(benchmarks) < 2:
            print("❌ Insufficient data for scalability analysis")
            return
        
        # Extract throughput data
        neuron_counts = []
        throughputs = []
        
        for benchmark in benchmarks:
            if "avg_throughput_hz" in benchmark.results:
                neuron_counts.append(benchmark.neuron_count)
                throughputs.append(benchmark.results["avg_throughput_hz"])
        
        if len(neuron_counts) >= 2:
            # Calculate scaling efficiency
            log_neuron_counts = np.log(neuron_counts)
            log_throughputs = np.log(throughputs)
            
            # Linear regression for scaling
            coefficients = np.polyfit(log_neuron_counts, log_throughputs, 1)
            scaling_exponent = coefficients[0]
            intercept = coefficients[1]
            
            print(f"📊 Scaling Analysis Results:")
            print(f"   Neuron Counts: {neuron_counts}")
            print(f"   Throughputs: {[f'{t:.2f}Hz' for t in throughputs]}")
            print(f"   Scaling Exponent: {scaling_exponent:.3f}")
            print(f"   Intercept: {intercept:.3f}")
            
            # Interpret scaling
            if scaling_exponent > 1.0:
                print("   ⚡ Super-linear scaling (excellent)")
            elif scaling_exponent > 0.8:
                print("   ⚡ Linear scaling (good)")
            elif scaling_exponent > 0.5:
                print("   📈 Sub-linear scaling (moderate)")
            else:
                print("   📉 Poor scaling (concerning)")
            
            # Calculate efficiency score
            efficiency_score = scaling_exponent / 1.0  # Normalize to 1.0
            print(f"   Efficiency Score: {efficiency_score:.2f}")
        
        # Analyze memory efficiency
        print(f"\n💾 Memory Efficiency Analysis:")
        for benchmark in benchmarks:
            if "memory_efficiency_score" in benchmark.results:
                print(f"   {benchmark.benchmark_name}:")
                print(f"     Memory Efficiency Score: {benchmark.results['memory_efficiency_score']:.3f}")
                print(f"     Average Memory Change: {benchmark.results.get('avg_memory_change_mb', 0):.2f} MB")
        
        # Analyze coherence
        print(f"\n🔗 Coherence Analysis:")
        for benchmark in benchmarks:
            if "avg_coherence_score" in benchmark.results:
                print(f"   {benchmark.benchmark_name}:")
                print(f"     Average Coherence: {benchmark.results['avg_coherence_score']:.3f}")
                print(f"     Cross-System Interactions: {benchmark.results.get('avg_cross_system_interactions', 0):.1f}")
        
    def _analyze_coherence_trend(self, coherence_samples: List[float]) -> Dict[str, Any]:
        """Analyze coherence trend over time"""
        if len(coherence_samples) < 2:
            return {"trend": "insufficient_data", "correlation": 0.0}
        
        # Calculate trend using linear regression
        x_values = np.arange(len(coherence_samples))
        y_values = np.array(coherence_samples)
        
        coefficients = np.polyfit(x_values, y_values, 1)
        trend_slope = coefficients[0]
        correlation = np.corrcoef(x_values, y_values)[0, 1]
        
        return {
            "trend": "improving" if trend_slope > 0.01 else "degrading" if trend_slope < -0.01 else "stable",
            "slope": trend_slope,
            "correlation": correlation
        }
        
    def run_all_performance_tests(self) -> List[Benchmark]:
        """Run all performance tests"""
        print("🚀 Running NLM Phase 6 Performance Test Suite")
        print("=" * 80)
        
        all_benchmarks = []
        
        # Run performance comparison test
        comparison_benchmarks = self.run_performance_comparison_test()
        all_benchmarks.extend(comparison_benchmarks)
        
        # Run scalability tests
        scalability_benchmarks = self.run_scalability_test([100, 500, 1000], 500)
        all_benchmarks.extend(scalability_benchmarks)
        
        # Calculate summary statistics
        self._calculate_performance_summary(all_benchmarks)
        
        return all_benchmarks
        
    def _calculate_performance_summary(self, benchmarks: List[Benchmark]):
        """Calculate performance summary statistics"""
        print("\n" + "="*80)
        print("📊 PERFORMANCE SUMMARY")
        print("="*80)
        
        if not benchmarks:
            print("❌ No benchmarks to analyze")
            return
        
        # Group benchmarks by type
        throughput_benchmarks = [b for b in benchmarks if b.test_type == PerformanceTestType.THROUGHPUT_TEST]
        latency_benchmarks = [b for b in benchmarks if b.test_type == PerformanceTestType.LATENCY_TEST]
        memory_benchmarks = [b for b in benchmarks if b.test_type == PerformanceTestType.MEMORY_EFFICIENCY_TEST]
        coherence_benchmarks = [b for b in benchmarks if b.test_type == PerformanceTestType.COHERENCE_TEST]
        
        print(f"📈 Test Statistics:")
        print(f"   Total Benchmarks: {len(benchmarks)}")
        print(f"   Throughput Tests: {len(throughput_benchmarks)}")
        print(f"   Latency Tests: {len(latency_benchmarks)}")
        print(f"   Memory Efficiency Tests: {len(memory_benchmarks)}")
        print(f"   Coherence Tests: {len(coherence_benchmarks)}")
        
        # Calculate averages
        total_throughput = sum(b.results.get("avg_throughput_hz", 0) for b in throughput_benchmarks)
        avg_throughput = total_throughput / len(throughput_benchmarks) if throughput_benchmarks else 0
        
        total_latency = sum(b.results.get("avg_latency_ms", 0) for b in latency_benchmarks)
        avg_latency = total_latency / len(latency_benchmarks) if latency_benchmarks else 0
        
        total_memory_efficiency = sum(b.results.get("memory_efficiency_score", 0) for b in memory_benchmarks)
        avg_memory_efficiency = total_memory_efficiency / len(memory_benchmarks) if memory_benchmarks else 0
        
        total_coherence = sum(b.results.get("avg_coherence_score", 0) for b in coherence_benchmarks)
        avg_coherence = total_coherence / len(coherence_benchmarks) if coherence_benchmarks else 0
        
        print(f"\n📊 Average Performance:")
        print(f"   Throughput: {avg_throughput:.2f} Hz")
        print(f"   Latency: {avg_latency:.2f} ms")
        print(f"   Memory Efficiency: {avg_memory_efficiency:.3f}")
        print(f"   System Coherence: {avg_coherence:.3f}")
        
        # Calculate performance score
        performance_score = (avg_throughput / 1000.0 +  # Normalize to 0-1
                           (1.0 - avg_latency / 1000.0) +  # Normalize latency (lower is better)
                           avg_memory_efficiency +        # Already 0-1
                           avg_coherence) / 4.0         # Average of four metrics
        
        print(f"\n🏆 Overall Performance Score: {performance_score:.3f}")
        
        # Performance assessment
        if performance_score >= 0.8:
            print("   ✅ Excellent performance")
        elif performance_score >= 0.6:
            print("   ⚡ Good performance")
        elif performance_score >= 0.4:
            print("   📈 Moderate performance")
        else:
            print("   ⚠️ Poor performance - consider optimization")
        
        # Save results
        self._save_performance_results(benchmarks)
        
    def _save_performance_results(self, benchmarks: List[Benchmark]):
        """Save performance results to file"""
        try:
            # Create summary
            summary = {
                "benchmark_suite": "NLM Phase 6 Performance Test Suite",
                "timestamp": time.time(),
                "total_benchmarks": len(benchmarks),
                "performance_summary": self._get_performance_summary(benchmarks),
                "detailed_results": []
            }
            
            # Add detailed results
            for benchmark in benchmarks:
                detailed_result = {
                    "benchmark_name": benchmark.benchmark_name,
                    "test_type": benchmark.test_type.value,
                    "neuron_count": benchmark.neuron_count,
                    "config": benchmark.config,
                    "results": benchmark.results,
                    "metrics_stats": benchmark.metrics
                }
                summary["detailed_results"].append(detailed_result)
            
            # Save to file
            filename = f"nlm_phase6_performance_results_{int(time.time())}.json"
            filepath = os.path.join(self.temp_dir, filename)
            
            with open(filepath, 'w') as f:
                json.dump(summary, f, indent=2)
            
            print(f"\n💾 Performance results saved to: {filepath}")
            
        except Exception as e:
            print(f"❌ Failed to save performance results: {e}")
            
    def _get_performance_summary(self, benchmarks: List[Benchmark]) -> Dict[str, Any]:
        """Get performance summary for all benchmarks"""
        if not benchmarks:
            return {}
        
        # Group by test type
        by_type = {}
        for benchmark in benchmarks:
            test_type = benchmark.test_type.value
            if test_type not in by_type:
                by_type[test_type] = []
            by_type[test_type].append(benchmark)
        
        # Calculate type-specific statistics
        summary = {}
        for test_type, type_benchmarks in by_type.items():
            type_stats = {
                "count": len(type_benchmarks),
                "avg_neuron_count": sum(b.neuron_count for b in type_benchmarks) / len(type_benchmarks)
            }
            
            # Add metric-specific statistics
            for metric_name in ["avg_throughput_hz", "avg_latency_ms", "memory_efficiency_score", "avg_coherence_score"]:
                values = [b.results.get(metric_name, 0) for b in type_benchmarks]
                if values:
                    type_stats[f"avg_{metric_name}"] = sum(values) / len(values)
                    type_stats[f"min_{metric_name}"] = min(values)
                    type_stats[f"max_{metric_name}"] = max(values)
            
            summary[test_type] = type_stats
        
        return summary

class MockBrain:
    """Mock brain class for testing without actual NLM implementation"""
    
    def __init__(self, config: Dict[str, Any]):
        self.config = config
        self.working_memory_active = 0
        self.episodic_memory_episodes = 0
        self.associative_memory_strength = 0.0
        self.dopamine_level = 0.0
        self.curiosity_level = 0.0
        self.novelty_level = 0.0
        self.prediction_confidence = 0.0
        self.attention_focus = 0.0
        self.planning_efficiency = 0.0
        self.development_stage = 0.0
        
    def step(self, step: int, time: float):
        """Execute simulation step"""
        # Mock implementation
        self.working_memory_active = max(0, min(100, self.working_memory_active + np.random.uniform(-5, 10)))
        self.episodic_memory_episodes += int(np.random.random() < 0.1)
        self.associative_memory_strength = min(1.0, self.associative_memory_strength + np.random.uniform(-0.1, 0.2))
        
        # Update neuromodulation levels
        self.dopamine_level = min(2.0, max(0.0, self.dopamine_level + np.random.uniform(-0.2, 0.3)))
        self.curiosity_level = min(1.5, max(0.0, self.curiosity_level + np.random.uniform(-0.1, 0.2)))
        self.novelty_level = min(1.0, max(0.0, self.novelty_level + np.random.uniform(-0.15, 0.25)))
        
        # Update cognitive levels
        self.prediction_confidence = min(1.0, max(0.0, self.prediction_confidence + np.random.uniform(-0.05, 0.1)))
        self.attention_focus = min(1.0, max(0.0, self.attention_focus + np.random.uniform(-0.1, 0.15)))
        self.planning_efficiency = min(1.0, max(0.0, self.planning_efficiency + np.random.uniform(-0.05, 0.1)))
        
        # Update development
        self.development_stage = min(1.0, self.development_stage + 0.001)
        
    def addWorkingMemoryEntry(self, key: str, value: float):
        """Add working memory entry"""
        self.working_memory_active = min(100, self.working_memory_active + value)
        
    def getWorkingMemoryActiveTraces(self) -> float:
        """Get working memory active traces"""
        return self.working_memory_active
        
    def getWorkingMemoryCapacity(self) -> float:
        """Get working memory capacity"""
        return 100.0
        
    def encodeEpisodicMemory(self, episode_id: str, strength: float):
        """Encode episodic memory"""
        self.episodic_memory_episodes += 1
        
    def getEpisodicMemoryEpisodes(self) -> int:
        """Get episodic memory episodes"""
        return self.episodic_memory_episodes
        
    def formAssociativeMemory(self, pattern_id: str, pattern: List[float]):
        """Form associative memory"""
        self.associative_memory_strength = min(1.0, self.associative_memory_strength + np.random.uniform(0.1, 0.3))
        
    def getAssociativeMemoryStrength(self) -> float:
        """Get associative memory strength"""
        return self.associative_memory_strength
        
    def applyRewardModulation(self, reward: float, error: float):
        """Apply reward modulation"""
        self.dopamine_level = min(2.0, self.dopamine_level + reward * 0.5)
        
    def getDopamineLevel(self) -> float:
        """Get dopamine level"""
        return self.dopamine_level
        
    def triggerExplorationBehavior(self):
        """Trigger exploration behavior"""
        self.curiosity_level = min(1.5, self.curiosity_level + 0.2)
        
    def getCuriosityLevel(self) -> float:
        """Get curiosity level"""
        return self.curiosity_level
        
    def detectNovelty(self) -> float:
        """Detect novelty"""
        return min(1.0, max(0.0, self.novelty_level + np.random.uniform(0.1, 0.3)))
        
    def makePrediction(self) -> Dict[str, float]:
        """Make prediction"""
        confidence = min(1.0, max(0.0, self.prediction_confidence + np.random.uniform(0.0, 0.2)))
        return {"confidence": confidence, "error": 1.0 - confidence}
        
    def getPredictionError(self) -> float:
        """Get prediction error"""
        return 1.0 - self.prediction_confidence
        
    def getAttentionState(self) -> Dict[str, float]:
        """Get attention state"""
        return {
            "focus": self.attention_focus,
            "entropy": 1.0 - self.attention_focus,
            "selection": self.attention_focus * 0.8
        }
        
    def getAttentionFocus(self) -> float:
        """Get attention focus"""
        return self.attention_focus
        
    def evaluateActionSequence(self) -> Dict[str, float]:
        """Evaluate action sequence"""
        efficiency = min(1.0, max(0.0, self.planning_efficiency + np.random.uniform(0.0, 0.3)))
        return {"efficiency": efficiency, "confidence": efficiency * 0.9}
        
    def formConcept(self) -> Dict[str, float]:
        """Form concept"""
        confidence = min(1.0, max(0.0, np.random.uniform(0.6, 0.9)))
        return {"confidence": confidence, "novelty": 1.0 - confidence}
        
    def getDevelopmentStage(self) -> float:
        """Get development stage"""
        return self.development_stage
        
    def updateDevelopment(self, time: float):
        """Update development"""
        self.development_stage = min(1.0, self.development_stage + time * 0.001)
        
    def triggerEpisodicReplay(self):
        """Trigger episodic replay"""
        pass
        
    def saveCheckpoint(self):
        """Save checkpoint"""
        pass
        
    def getSynapseCount(self) -> int:
        """Get synapse count"""
        return int(self.working_memory_active * 10)
        
    def __str__(self):
        """String representation"""
        return f"MockBrain(neuron_count={len(self.config.get('neuron_ids', []))})"

# Alias for compatibility
Benchmark = PerformanceBenchmark
PerformanceProfiler = PerformanceTestSuite


def create_nlm_performance_test_suite():
    """Create the complete NLM Phase 6 performance test suite"""
    print("🚀 Creating NLM Phase 6 Performance Test Suite")
    
    # Create temporary directory for tests
    temp_dir = tempfile.mkdtemp(prefix="nlm_performance_test_")
    print(f"📁 Test directory: {temp_dir}")
    
    # Initialize performance test suite
    performance_suite = PerformanceTestSuite(temp_dir)
    
    # Run all performance tests
    print("\n" + "="*80)
    print("NLM PHASE 6 PERFORMANCE TEST SUITE")
    print("="*80)
    
    try:
        benchmarks = performance_suite.run_all_performance_tests()
        
        # Save performance suite state
        suite_state = {
            "suite_name": "NLM Phase 6 Performance Test Suite",
            "timestamp": time.time(),
            "benchmarks": benchmarks,
            "summary": performance_suite._get_performance_summary(benchmarks) if benchmarks else {}
        }
        
        # Save to file
        suite_file = os.path.join(temp_dir, "nlm_phase6_performance_suite.json")
        with open(suite_file, 'w') as f:
            json.dump(suite_state, f, indent=2)
        
        print(f"\n💾 Performance suite state saved to: {suite_file}")
        
        # Print summary
        if benchmarks:
            print(f"\n📊 Performance Test Summary:")
            print(f"   Total Tests: {len(benchmarks)}")
            print(f"   Throughput Tests: {len([b for b in benchmarks if b.test_type == PerformanceTestType.THROUGHPUT_TEST])}")
            print(f"   Latency Tests: {len([b for b in benchmarks if b.test_type == PerformanceTestType.LATENCY_TEST])}")
            print(f"   Memory Tests: {len([b for b in benchmarks if b.test_type == PerformanceTestType.MEMORY_EFFICIENCY_TEST])}")
            print(f"   Coherence Tests: {len([b for b in benchmarks if b.test_type == PerformanceTestType.COHERENCE_TEST])}")
        
        return True
        
    except Exception as e:
        print(f"❌ Performance test suite failed: {e}")
        return False
        
    finally:
        # Cleanup
        import shutil
        shutil.rmtree(temp_dir)

if __name__ == "__main__":
    success = create_nlm_performance_test_suite()
    sys.exit(0 if success else 1)