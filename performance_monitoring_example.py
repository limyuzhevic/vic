"""
Example usage of NLM Performance Monitoring
===========================================

This script demonstrates the comprehensive performance monitoring capabilities
added to the NLM project, including profiling, metrics collection, alerting, and benchmarking.
"""

import pynlm
import time
import json
from typing import List, Dict, Any

def main():
    print("=== NLM Performance Monitoring Example ===\n")
    
    # Create a performance monitor
    monitor = pynlm.create_performance_monitor()
    
    # Create a performance profiler
    profiler = pynlm.create_performance_profiler()
    
    # Create a performance alerting system
    alerting = pynlm.create_performance_alerting()
    
    # Create a performance benchmark
    benchmark = pynlm.create_neural_simulation_benchmark()
    
    # Example 1: Basic monitoring and profiling
    print("Example 1: Basic monitoring and profiling")
    print("-" * 50)
    
    # Configure the monitor
    config = pynlm.create_default_performance_config()
    config.enable_monitoring = True
    config.enable_profiling = True
    config.enable_alerting = True
    config.sampling_interval_ms = 100
    monitor.configure(config)
    
    # Configure alerting
    from pynlm import AlertCondition, AlertConditionType
    cpu_alert = AlertCondition()
    cpu_alert.type = AlertConditionType.CPU_USAGE_ABOVE
    cpu_alert.threshold = 80.0
    cpu_alert.severity = "WARNING"
    cpu_alert.message = "CPU usage exceeded threshold"
    cpu_alert.consecutive_failures = 2
    
    alerting.add_condition("cpu_alert", cpu_alert)
    
    # Create a simple brain
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    brain.initialize()
    
    # Run some simulation steps with monitoring
    for step in range(10):
        start_time = time.time()
        
        # Profile the brain step
        with profiler.make_scope("brain_step", "main.py", step):
            brain.step(step)
        
        # Create and record metrics
        from pynlm import PerformanceMetrics
        metrics = PerformanceMetrics()
        metrics.simulation_step = step
        metrics.cpu_usage_percent = 50.0 + (step * 2.5) % 40
        metrics.memory_usage_mb = 100.0 + (step * 1.5) % 50
        metrics.active_neurons = brain.getTotalNeuronCount()
        metrics.total_spikes = brain.getTotalSpikeCount()
        metrics.average_firing_rate = brain.getAverageFiringRate()
        metrics.simulation_time_ms = (time.time() - start_time) * 1000.0
        metrics.thread_count = 1
        
        monitor.record_metrics(metrics)
        
        # Evaluate alerts
        alerting.evaluate(metrics, metrics.simulation_step * 1000000000)
        
        # Print progress
        if step % 3 == 0:
            print(f"Step {step}: {brain.getFiringNeuronCount()} neurons firing, "
                  f"avg firing rate: {brain.getAverageFiringRate():.2f}")
    
    # Show current metrics
    current = monitor.get_current_metrics()
    print(f"\nCurrent Metrics:")
    print(f"  Simulation Step: {current.simulation_step}")
    print(f"  CPU Usage: {current.cpu_usage_percent:.1f}%")
    print(f"  Memory Usage: {current.memory_usage_mb:.1f} MB")
    print(f"  Active Neurons: {current.active_neurons}")
    print(f"  Total Spikes: {current.total_spikes}")
    print(f"  Firing Rate: {current.average_firing_rate:.2f}")
    
    # Check for alerts
    active_alerts = alerting.get_active_alerts()
    if active_alerts:
        print(f"\nActive Alerts ({len(active_alerts)}):")
        for alert in active_alerts:
            print(f"  {alert.severity}: {alert.message}")
            print(f"    Threshold: {alert.threshold_value}, Current: {alert.current_value}")
    else:
        print(f"\nNo active alerts")
    
    # Show profile events
    profile_events = monitor.get_profile_events()
    if profile_events:
        print(f"\nProfile Events ({len(profile_events)}):")
        total_duration = sum(event.duration_ns for event in profile_events) / 1000000.0  # Convert to ms
        print(f"  Total profiled time: {total_duration:.2f} ms")
    
    # Example 2: Performance analysis
    print(f"\nExample 2: Performance Analysis")
    print("-" * 50)
    
    # Calculate performance metrics
    avg_firing_rate = monitor.get_average_firing_rate()
    current_firing_rate = monitor.get_current_firing_rate()
    
    print(f"Average Firing Rate: {avg_firing_rate:.2f}")
    print(f"Current Firing Rate: {current_firing_rate:.2f}")
    
    # Get metrics history
    history = monitor.get_metrics_history()
    print(f"Metrics collected for {len(history)} steps")
    
    # Example 3: Save performance data
    print(f"\nExample 3: Saving Performance Data")
    print("-" * 50)
    
    # Save metrics to file
    monitor.save_metrics_to_file("performance_metrics.json")
    print("Metrics saved to 'performance_metrics.json'")
    
    # Save profile events to file
    monitor.save_profile_to_file("profile_events.json")
    print("Profile events saved to 'profile_events.json'")
    
    # Save alerts to file (if any)
    if active_alerts:
        alerting.save_alerts_to_file("performance_alerts.json")
        print("Alerts saved to 'performance_alerts.json'")
    
    # Example 4: Performance benchmarking
    print(f"\nExample 4: Performance Benchmarking")
    print("-" * 50)
    
    # Configure benchmark
    bench_config = pynlm.create_default_benchmark_config()
    bench_config.warmup_iterations = 2
    bench_config.measurement_iterations = 5
    bench_config.output_file = "benchmark_report.json"
    
    benchmark.configure(bench_config)
    
    # Run benchmarks
    print("Running performance benchmarks...")
    benchmark.run_all_benchmarks()
    
    # Display results
    benchmark.print_results()
    
    # Generate report
    benchmark.generate_report("benchmark_report.txt")
    print("Benchmark report saved to 'benchmark_report.txt'")
    
    # Example 5: Performance validation
    print(f"\nExample 5: Performance Validation")
    print("-" * 50)
    
    # Get benchmark results
    results = benchmark.get_results()
    
    for result in results:
        print(f"Benchmark: {result.test_name}")
        print(f"  Status: {result.status}")
        print(f"  Execution Time: {result.execution_time_ms:.3f} ms")
        print(f"  Throughput: {result.throughput_ops_per_second:.2f} {result.unit}/s")
        
        if result.status == "FAILED":
            print(f"  Error: {result.error_message}")
        
        print()
    
    print("=== Example Complete ===")

if __name__ == "__main__":
    main()