"""
Performance Benchmark Examples for NLM Testing Framework

This module provides examples of using the NLM testing framework
for performance benchmarking and optimization.
"""

import sys
import os
import time
import statistics
import numpy as np
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)) + "/../../src")

from nlm.testing import PerformanceTestSuite, BenchmarkSuite
from nlm.testing import TestDataGenerator, CoverageReporter
from nlm.testing import TestRunner

class PerformanceBenchmarkExamples:
    """
    Example class demonstrating performance benchmarking workflows.
    """
    
    def __init__(self):
        self.benchmarkSuite = BenchmarkSuite()
        self.performanceSuite = PerformanceTestSuite()
        self.testRunner = TestRunner()
        self.testDataGenerator = TestDataGenerator()
        
    def create_benchmark_suite_example(self):
        """
        Example: Create and run a benchmark suite
        """
        print("=== Benchmark Suite Creation Example ===")
        
        # Register benchmarks
        print("Registering benchmarks...")
        
        # Brain step benchmark
        self.benchmarkSuite.registerBenchmark(
            "brain_step_small",
            "Benchmark brain step with small network",
            lambda: self.simulate_brain_step(100, 1.0),
            1000,
            "ms",
            "brain"
        )
        
        # Brain step benchmark (larger)
        self.benchmarkSuite.registerBenchmark(
            "brain_step_large",
            "Benchmark brain step with large network",
            lambda: self.simulate_brain_step(1000, 2.0),
            500,
            "ms",
            "brain"
        )
        
        # Memory update benchmark
        self.benchmarkSuite.registerBenchmark(
            "memory_update",
            "Benchmark memory update operation",
            lambda: self.simulate_memory_update(5000),
            200,
            "ms",
            "memory"
        )
        
        # Dynamics calculation benchmark
        self.benchmarkSuite.registerBenchmark(
            "dynamics_calculation",
            "Benchmark neural dynamics calculation",
            lambda: self.simulate_dynamics_calculation(100),
            300,
            "ms",
            "dynamics"
        )
        
        # Environment simulation benchmark
        self.benchmarkSuite.registerBenchmark(
            "environment_simulation",
            "Benchmark environment simulation",
            lambda: self.simulate_environment_simulation(50),
            100,
            "ms",
            "environment"
        )
        
        # Run all benchmarks
        print("Running all benchmarks...")
        self.benchmarkSuite.runAllBenchmarks()
        
        # Generate benchmark report
        print("Generating benchmark report...")
        self.benchmarkSuite.generateBenchmarkReport()
        
        # Export benchmark results
        self.benchmarkSuite.exportBenchmarkResults("brain_benchmark_results.txt")
        
        print("Benchmark suite creation example completed")
        
    def create_performance_test_suite_example(self):
        """
        Example: Create and run a performance test suite
        """
        print("\n=== Performance Test Suite Example ===")
        
        # Set up performance test suite
        self.performanceSuite.setupPerformanceMonitoring(None)  # No actual monitor for demo
        
        # Create baseline
        print("Creating performance baseline...")
        self.performanceSuite.createBaseline("default_baseline", "Default performance baseline")
        
        # Add baseline metrics
        self.performanceSuite.addBaselineMetric("cpu_usage", 50.0)
        self.performanceSuite.addBaselineMetric("memory_usage", 200.0)
        self.performanceSuite.addBaselineMetric("step_duration", 10.0)
        self.performanceSuite.addBaselineMetric("throughput", 100.0)
        
        # Add thresholds
        self.performanceSuite.setBaselineThreshold("cpu_usage", 20.0)
        self.performanceSuite.setBaselineThreshold("memory_usage", 15.0)
        self.performanceSuite.setBaselineThreshold("step_duration", 5.0)
        self.performanceSuite.setBaselineThreshold("throughput", 80.0)
        
        # Add weights
        self.performanceSuite.setBaselineWeight("cpu_usage", 0.4)
        self.performanceSuite.setBaselineWeight("memory_usage", 0.3)
        self.performanceSuite.setBaselineWeight("step_duration", 0.2)
        self.performanceSuite.setBaselineWeight("throughput", 0.1)
        
        # Run regression tests
        print("Running performance regression tests...")
        self.performanceSuite.runRegressionTests()
        
        # Load and compare with baseline
        print("Loading baseline from file...")
        self.performanceSuite.loadBaseline("baseline_perf.txt")
        
        print("Comparing with baseline...")
        self.performanceSuite.compareWithBaseline()
        
        # Generate performance report
        print("Generating performance report...")
        self.performanceSuite.generatePerformanceReport()
        
        # Export performance data
        self.performanceSuite.exportPerformanceData("performance_analysis.txt")
        
        print("Performance test suite example completed")
        
    def run_load_testing_example(self):
        """
        Example: Run load testing
        """
        print("\n=== Load Testing Example ===")
        
        # Define load test configurations
        print("Setting up load test configurations...")
        
        # Small load test
        smallLoadConfig = LoadTestConfig("small_load_test", 10, 30.0)
        
        # Medium load test
        mediumLoadConfig = LoadTestConfig("medium_load_test", 100, 60.0)
        
        # Large load test
        largeLoadConfig = LoadTestConfig("large_load_test", 1000, 120.0)
        
        # Run load tests
        print("Running small load test...")
        self.performanceSuite.runLoadTest(smallLoadConfig)
        
        print("Running medium load test...")
        self.performanceSuite.runLoadTest(mediumLoadConfig)
        
        print("Running large load test...")
        self.performanceSuite.runLoadTest(largeLoadConfig)
        
        # Analyze scalability
        print("Analyzing scalability...")
        self.performanceSuite.runScalabilityTests()
        
        # Run stress tests
        print("Running stress tests...")
        self.performanceSuite.runStressTests()
        
        print("Load testing example completed")
        
    def run_performance_profiling_example(self):
        """
        Example: Run performance profiling
        """
        print("\n=== Performance Profiling Example ===")
        
        # Create profiler
        from nlm.testing import PerformanceProfiler
        profiler = PerformanceProfiler()
        
        # Profile different operations
        print("Profiling brain step operation...")
        profiler.profileFunction("brain_step", lambda: self.simulate_brain_step(500, 1.5), 1000)
        
        print("Profiling memory update operation...")
        profiler.profileFunction("memory_update", lambda: self.simulate_memory_update(1000), 500)
        
        print("Profiling dynamics calculation...")
        profiler.profileFunction("dynamics_calculation", lambda: self.simulate_dynamics_calculation(200), 300)
        
        print("Profiling memory usage...")
        profiler.profileMemoryUsage("memory_operation", lambda: self.allocate_memory(100))
        
        print("Profiling CPU usage...")
        profiler.profileCPUUsage("cpu_operation", lambda: self.cpu_intensive_task(1000))
        
        # Generate profile report
        print("Generating profile report...")
        profiler.generateProfileReport()
        
        # Export profile data
        profiler.exportProfileData("performance_profile.json")
        
        # Detect bottlenecks
        print("Detecting performance bottlenecks...")
        bottlenecks = profiler.detectPerformanceBottlenecks()
        for bottleneck in bottlenecks:
            print(f"  - {bottleneck}")
        
        print("Performance profiling example completed")
        
    def run_benchmark_comparison_example(self):
        """
        Example: Compare benchmarks with different configurations
        """
        print("\n=== Benchmark Comparison Example ===")
        
        # Create test runner
        self.testRunner.registerBenchmarkSuite(self.benchmarkSuite)
        
        # Run benchmarks with different configurations
        print("Running benchmarks with different configurations...")
        
        # Configuration 1: Standard
        print("\nConfiguration 1: Standard (CPU=4, Memory=8GB)")
        self.benchmarkSuite.runBenchmark("brain_step_small")
        self.benchmarkSuite.runBenchmark("brain_step_large")
        self.benchmarkSuite.runBenchmark("memory_update")
        
        # Configuration 2: Optimized
        print("\nConfiguration 2: Optimized (CPU=8, Memory=16GB)")
        self.benchmarkSuite.runBenchmark("brain_step_small")
        self.benchmarkSuite.runBenchmark("brain_step_large")
        self.benchmarkSuite.runBenchmark("memory_update")
        
        # Configuration 3: Minimal
        print("\nConfiguration 3: Minimal (CPU=2, Memory=4GB)")
        self.benchmarkSuite.runBenchmark("brain_step_small")
        self.benchmarkSuite.runBenchmark("memory_update")
        
        # Generate comparison report
        print("\nGenerating benchmark comparison report...")
        self.benchmarkSuite.generateBenchmarkReport()
        
        # Export comparison results
        self.benchmarkSuite.exportBenchmarkResults("benchmark_comparison.txt")
        
        print("Benchmark comparison example completed")
        
    def run_performance_analysis_example(self):
        """
        Example: Run comprehensive performance analysis
        """
        print("\n=== Performance Analysis Example ===")
        
        # Generate performance data
        print("Generating performance data...")
        brainStepData = self.testDataGenerator.generatePerformanceData("brain_step", 500)
        memoryData = self.testDataGenerator.generatePerformanceData("memory_update", 300)
        dynamicsData = self.testDataGenerator.generatePerformanceData("dynamics_calculation", 200)
        
        # Analyze performance data
        print("Analyzing performance data...")
        self.analyzePerformanceData(brainStepData, "brain_step")
        self.analyzePerformanceData(memoryData, "memory_update")
        self.analyzePerformanceData(dynamicsData, "dynamics_calculation")
        
        # Calculate statistics
        print("Calculating performance statistics...")
        brainStepStats = self.calculateBenchmarkStats(brainStepData)
        memoryStats = self.calculateBenchmarkStats(memoryData)
        dynamicsStats = self.calculateBenchmarkStats(dynamicsData)
        
        print(f"Brain Step Stats: {brainStepStats}")
        print(f"Memory Update Stats: {memoryStats}")
        print(f"Dynamics Calculation Stats: {dynamicsStats}")
        
        # Detect regressions
        print("Detecting performance regressions...")
        self.detectRegressions(brainStepData, memoryData, dynamicsData)
        
        # Generate optimization recommendations
        print("Generating optimization recommendations...")
        recommendations = self.generateOptimizationRecommendations(brainStepData, memoryData, dynamicsData)
        for rec in recommendations:
            print(f"  - {rec}")
        
        print("Performance analysis example completed")
        
    # Simulation methods for benchmarking
    def simulate_brain_step(self, neuronCount, duration):
        """Simulate a brain step operation"""
        time.sleep(duration * 0.001)  # Simulate computation time
        return neuronCount * duration
    
    def simulate_memory_update(self, memorySize):
        """Simulate a memory update operation"""
        time.sleep(0.001 * (memorySize / 1000.0))  # Simulate computation time
        return memorySize
    
    def simulate_dynamics_calculation(self, neuronCount):
        """Simulate neural dynamics calculation"""
        time.sleep(0.002 * (neuronCount / 100.0))  # Simulate computation time
        return neuronCount * 0.5
    
    def simulate_environment_simulation(self, agentCount):
        """Simulate environment simulation"""
        time.sleep(0.0015 * agentCount)  # Simulate computation time
        return agentCount * 2.0
    
    def allocate_memory(self, sizeMB):
        """Simulate memory allocation"""
        # Allocate some memory
        data = [0.0] * (sizeMB * 1024 * 1024 // 8)  # Approximately sizeMB of doubles
        return len(data)
    
    def cpu_intensive_task(self, iterations):
        """Simulate CPU-intensive task"""
        result = 0
        for i in range(iterations):
            result += i * i
        return result
    
    # Performance analysis methods
    def analyzePerformanceData(self, data, benchmarkName):
        """Analyze performance data"""
        durations = [item.duration for item in data]
        
        print(f"  {benchmarkName} Analysis:")
        print(f"    Mean: {statistics.mean(durations):.2f} ms")
        print(f"    Median: {statistics.median(durations):.2f} ms")
        print(f"    Std Dev: {statistics.stdev(durations):.2f} ms")
        print(f"    Min: {min(durations):.2f} ms")
        print(f"    Max: {max(durations):.2f} ms")
    
    def calculateBenchmarkStats(self, data):
        """Calculate statistics for benchmark data"""
        durations = [item.duration for item in data]
        
        return {
            "mean": statistics.mean(durations),
            "median": statistics.median(durations),
            "stddev": statistics.stdev(durations) if len(durations) > 1 else 0.0,
            "min": min(durations),
            "max": max(durations),
            "count": len(data)
        }
    
    def detectRegressions(self, brainStepData, memoryData, dynamicsData):
        """Detect performance regressions"""
        # Simple regression detection based on mean values
        brainStepMean = statistics.mean([item.duration for item in brainStepData])
        memoryMean = statistics.mean([item.duration for item in memoryData])
        dynamicsMean = statistics.mean([item.duration for item in dynamicsData])
        
        threshold = 20.0  # 20% threshold
        
        if brainStepMean > 20.0:  # ms
            print(f"  Performance regression detected in brain_step: {brainStepMean:.2f}ms > 20.0ms")
        
        if memoryMean > 15.0:  # ms
            print(f"  Performance regression detected in memory_update: {memoryMean:.2f}ms > 15.0ms")
        
        if dynamicsMean > 25.0:  # ms
            print(f"  Performance regression detected in dynamics_calculation: {dynamicsMean:.2f}ms > 25.0ms")
    
    def generateOptimizationRecommendations(self, brainStepData, memoryData, dynamicsData):
        """Generate optimization recommendations"""
        recommendations = []
        
        # Analyze bottlenecks
        brainStepMean = statistics.mean([item.duration for item in brainStepData])
        memoryMean = statistics.mean([item.duration for item in memoryData])
        
        if brainStepMean > 15.0:
            recommendations.append("Optimize brain step algorithm - consider parallel processing")
        
        if memoryMean > 10.0:
            recommendations.append("Optimize memory management - consider memory pooling")
        
        # Check memory usage
        totalMemory = sum([item.memoryUsage for item in brainStepData + memoryData])
        if totalMemory > 500.0:  # MB
            recommendations.append("Reduce memory usage - consider streaming data")
        
        if not recommendations:
            recommendations.append("Performance within acceptable ranges - no optimizations needed")
        
        return recommendations

def main():
    """
    Main function demonstrating all performance benchmark examples.
    """
    print("NLM Performance Benchmark Examples")
    print("=" * 50)
    
    benchmarker = PerformanceBenchmarkExamples()
    
    # Run individual examples
    benchmarker.create_benchmark_suite_example()
    benchmarker.create_performance_test_suite_example()
    benchmarker.run_load_testing_example()
    benchmarker.run_performance_profiling_example()
    benchmarker.run_benchmark_comparison_example()
    benchmarker.run_performance_analysis_example()
    
    print("\n" + "=" * 50)
    print("All performance benchmark examples completed successfully!")

if __name__ == "__main__":
    main()
