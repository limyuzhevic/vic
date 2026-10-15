#!/usr/bin/env python3
"""
Demonstration Script for NLM Performance Monitoring

This script demonstrates the new performance monitoring capabilities
in the NLM project. It shows how to monitor, profile, and analyze
neural simulation performance.
"""

import pynlm
import time
import os

def demo_basic_monitoring():
    print("=== Basic Performance Monitoring Demo ===\n")
    
    # Create performance monitor and profiler
    monitor = pynlm.create_performance_monitor()
    profiler = pynlm.create_performance_profiler()
    
    # Configure monitoring
    config = pynlm.create_default_performance_config()
    config.enable_monitoring = True
    config.enable_profiling = True
    config.enable_alerting = True
    config.sampling_interval_ms = 100
    monitor.configure(config)
    
    # Create a simple brain
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    brain.initialize()
    
    print(f"Created brain with {brain.getTotalNeuronCount()} neurons")
    print("Running simulation with performance monitoring...\n")
    
    # Run simulation with monitoring
    for step in range(20):
        # Profile the brain step
        with profiler.make_scope("brain_step", "demo.py", step):
            start_time = time.time()
            brain.step(step)
            end_time = time.time()
            
            # Simulate some metrics collection
            metrics = pynlm.PerformanceMetrics()
            metrics.simulation_step = step
            metrics.cpu_usage_percent = 50.0 + (step * 2.5) % 40
            metrics.memory_usage_mb = 100.0 + (step * 1.5) % 50
            metrics.active_neurons = brain.getTotalNeuronCount()
            metrics.total_spikes = brain.getTotalSpikeCount()
            metrics.average_firing_rate = brain.getAverageFiringRate()
            metrics.simulation_time_ms = (end_time - start_time) * 1000.0
            metrics.thread_count = 1
            
            monitor.record_metrics(metrics)
        
        if step % 5 == 0:
            print(f"Step {step}: {brain.getFiringNeuronCount()} neurons firing, "
                  f"avg firing rate: {brain.getAverageFiringRate():.2f}")
    
    # Show results
    print("\n=== Performance Monitoring Results ===\n")
    
    current = monitor.get_current_metrics()
    print(f"Current Performance Metrics:")
    print(f"  Simulation Step: {current.simulation_step}")
    print(f"  CPU Usage: {current.cpu_usage_percent:.1f}%")
    print(f"  Memory Usage: {current.memory_usage_mb:.1f} MB")
    print(f"  Active Neurons: {current.active_neurons}")
    print(f"  Total Spikes: {current.total_spikes}")
    print(f"  Average Firing Rate: {current.average_firing_rate:.2f}")
    print(f"  Simulation Time: {current.simulation_time_ms:.1f} ms")
    
    avg_firing_rate = monitor.get_average_firing_rate()
    current_firing_rate = monitor.get_current_firing_rate()
    print(f"\nPerformance Analysis:")
    print(f"  Average Firing Rate: {avg_firing_rate:.2f}")
    print(f"  Current Firing Rate: {current_firing_rate:.2f}")
    
    # Show profile events
    profile_events = monitor.get_profile_events()
    if profile_events:
        print(f"\nProfile Summary:")
        total_duration = sum(event.duration_ns for event in profile_events) / 1000000.0
        print(f"  Total profiled time: {total_duration:.2f} ms")
        print(f"  Number of profile events: {len(profile_events)}")
    
    # Save performance data
    monitor.save_metrics_to_file("demo_metrics.json")
    monitor.save_profile_to_file("demo_profile.json")
    print(f"\nPerformance data saved to 'demo_metrics.json' and 'demo_profile.json'")

def demo_profiling():
    print("\n=== Performance Profiling Demo ===\n")
    
    monitor = pynlm.create_performance_monitor()
    profiler = pynlm.create_performance_profiler()
    
    # Configure for profiling
    config = pynlm.create_default_performance_config()
    config.enable_monitoring = True
    config.enable_profiling = True
    config.enable_alerting = False
    monitor.configure(config)
    
    print("Demonstrating different profiling approaches:\n")
    
    # 1. Manual profiling
    print("1. Manual Profiling:")
    profiler.start("manual_function", "demo.py", 1)
    
    # Simulate some work
    result = 0.0
    for i in range(1000):
        result += i * 0.1
    
    profiler.stop("manual_function")
    print(f"   Processed {1000} operations in manual_function")
    print(f"   Result: {result:.1f}\n")
    
    # 2. RAII profiling (automatic)
    print("2. RAII-style Profiling:")
    print("   Processing with scope-based profiling...")
    
    # Simulate work with RAII scope
    with profiler.make_scope("raii_function", "demo.py", 2):
        for i in range(500):
            _ = i * 0.01
    
    print("   Completed RAII profiling\n")
    
    # 3. Multiple functions
    print("3. Multiple Function Profiling:")
    functions = ["process_data", "compute_results", "generate_output"]
    
    for func_name in functions:
        profiler.start(func_name, "demo.py", 3)
        
        # Simulate work
        for i in range(200):
            _ = i * 0.05
        
        profiler.stop(func_name)
        print(f"   Profiled {func_name} with {200} operations")
    
    # Show profile summary
    profile_events = monitor.get_profile_events()
    if profile_events:
        print(f"\nProfile Summary:")
        print(f"  Total profile events: {len(profile_events)}")
        total_time = sum(event.duration_ns for event in profile_events) / 1000000.0
        print(f"  Total profiling time: {total_time:.2f} ms")
        
        # Find slowest function
        slowest = max(profile_events, key=lambda e: e.duration_ns)
        print(f"  Slowest function: {slowest.function_name} ({slowest.duration_ns/1000000:.2f} ms)")

def demo_alerting():
    print("\n=== Performance Alerting Demo ===\n")
    
    alerting = pynlm.create_performance_alerting()
    
    # Configure alert conditions
    from pynlm import AlertCondition, AlertConditionType
    
    cpu_alert = AlertCondition()
    cpu_alert.type = AlertConditionType.CPU_USAGE_ABOVE
    cpu_alert.threshold = 75.0
    cpu_alert.severity = "WARNING"
    cpu_alert.message = "High CPU usage detected"
    cpu_alert.consecutive_failures = 2
    
    memory_alert = AlertCondition()
    memory_alert.type = AlertConditionType.MEMORY_USAGE_ABOVE
    memory_alert.threshold = 200.0
    memory_alert.severity = "ERROR"
    memory_alert.message = "Memory usage exceeded limit"
    memory_alert.consecutive_failures = 1
    
    alerting.add_condition("cpu_high", cpu_alert)
    alerting.add_condition("memory_high", memory_alert)
    
    print("Alert conditions configured:")
    print("  - CPU > 75% (WARNING, 2 consecutive)")
    print("  - Memory > 200 MB (ERROR, 1 consecutive)\n")
    
    # Create test metrics
    from pynlm import PerformanceMetrics
    
    # Normal metrics (should not trigger alerts)
    print("Testing with normal metrics...")
    normal_metrics = PerformanceMetrics()
    normal_metrics.cpu_usage_percent = 50.0
    normal_metrics.memory_usage_mb = 100.0
    normal_metrics.timestamp_ns = 1000000000
    
    alerting.evaluate(normal_metrics, normal_metrics.timestamp_ns)
    alerts = alerting.get_active_alerts()
    print(f"  Active alerts: {len(alerts)} (expected: 0)")
    
    # High CPU metrics (should not trigger yet - need consecutive failures)
    print("\nTesting with high CPU (first failure)...")
    high_cpu_metrics = PerformanceMetrics()
    high_cpu_metrics.cpu_usage_percent = 85.0  # Above threshold
    high_cpu_metrics.memory_usage_mb = 100.0
    high_cpu_metrics.timestamp_ns = 2000000000
    
    alerting.evaluate(high_cpu_metrics, high_cpu_metrics.timestamp_ns)
    alerts = alerting.get_active_alerts()
    print(f"  Active alerts: {len(alerts)} (expected: 0 - need consecutive failures)")
    
    # High CPU again (should trigger alert)
    print("\nTesting with high CPU (second consecutive failure - should trigger)...")
    alerting.evaluate(high_cpu_metrics, high_cpu_metrics.timestamp_ns + 1000000000)
    alerts = alerting.get_active_alerts()
    print(f"  Active alerts: {len(alerts)}")
    if alerts:
        for alert in alerts:
            print(f"    - {alert.severity}: {alert.message}")
            print(f"      Current: {alert.current_value}, Threshold: {alert.threshold_value}")
    
    # High memory (should trigger immediately)
    print("\nTesting with high memory (immediate trigger)...")
    high_memory_metrics = PerformanceMetrics()
    high_memory_metrics.cpu_usage_percent = 50.0
    high_memory_metrics.memory_usage_mb = 250.0  # Above threshold
    high_memory_metrics.timestamp_ns = 3000000000
    
    alerting.evaluate(high_memory_metrics, high_memory_metrics.timestamp_ns)
    alerts = alerting.get_active_alerts()
    print(f"  Active alerts: {len(alerts)}")
    if alerts:
        for alert in alerts:
            print(f"    - {alert.severity}: {alert.message}")
    
    # Clear alerts
    print("\nClearing all alerts...")
    alerting.clear_alerts()
    alerts = alerting.get_active_alerts()
    print(f"  Active alerts after clear: {len(alerts)}")

def demo_benchmarking():
    print("\n=== Performance Benchmarking Demo ===\n")
    
    benchmark = pynlm.create_neural_simulation_benchmark()
    
    # Configure benchmark
    bench_config = pynlm.create_default_benchmark_config()
    bench_config.warmup_iterations = 1
    bench_config.measurement_iterations = 2
    bench_config.report_format = "summary"
    benchmark.configure(bench_config)
    
    print("Running performance benchmarks...")
    
    # Run benchmarks
    benchmark.run_all_benchmarks()
    
    # Display results
    benchmark.print_results()
    
    # Generate report
    benchmark.generate_report("demo_benchmark_report.txt")
    print(f"\nBenchmark report saved to 'demo_benchmark_report.txt'")
    
    # Show detailed results
    results = benchmark.get_results()
    print(f"\nDetailed Results:")
    for result in results:
        print(f"  {result.test_name}:")
        print(f"    Status: {result.status}")
        print(f"    Execution Time: {result.execution_time_ms:.3f} ms")
        print(f"    Throughput: {result.throughput_ops_per_second:.2f} {result.unit}/s")
        if result.status == "FAILED" and result.error_message:
            print(f"    Error: {result.error_message}")
        print()

def main():
    print("NLM Performance Monitoring Demonstration")
    print("=" * 50)
    
    demo_basic_monitoring()
    demo_profiling()
    demo_alerting()
    demo_benchmarking()
    
    print("\n" + "=" * 50)
    print("Performance monitoring demonstration complete!")
    print("\nKey features demonstrated:")
    print("  1. Basic performance monitoring and metrics collection")
    print("  2. Performance profiling (manual and RAII styles)")
    print("  3. Performance alerting with threshold conditions")
    print("  4. Performance benchmarking and validation")
    print("  5. Data export and reporting")

if __name__ == "__main__":
    main()