#!/usr/bin/env python3
"""
Bottleneck Analysis Example

This example demonstrates bottleneck analysis and optimization recommendations
for NLM neural simulations.
"""

import pynlm
import time
from monitor import BottleneckAnalyzer, PerformanceProfiler

def bottleneck_analysis_example():
    # Setup bottleneck analyzer and profiler
    analyzer = BottleneckAnalyzer()
    profiler = PerformanceProfiler()

    analyzer.start_analysis()

    # Create brain
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()

    # Record execution traces for bottleneck analysis
    for step in range(500):
        # Record resource usage
        resource_profile = {
            'timestamp': time.time(),
            'cpu_usage': 0.3 + (step % 10) * 0.05,  # Varying CPU usage
            'memory_usage': brain.getTotalNeuronCount() * 0.05 + (step % 5) * 0.01,  # Varying memory
            'disk_io': 0.1 + (step % 3) * 0.02,  # Varying I/O
            'network_io': 0.05 + (step % 2) * 0.01,  # Varying network
        }

        analyzer.record_resource_usage(resource_profile)

        # Record execution trace
        execution_trace = {
            'timestamp': time.time(),
            'thread_id': f"thread_{step % 4}",
            'function_name': "brain_step",
            'file_name': "brain.cpp",
            'line_number': 100 + (step % 50),
            'module_name': "neural_simulation",
            'execution_time': 0.001 + (step % 10) * 0.0001,  # Varying execution time
            'memory_usage': brain.getTotalNeuronCount() * 0.01 + (step % 5) * 0.001,  # Varying memory
            'call_stack': f"brain_step->step->neural_update",
            'metrics': {
                'spikes_generated': brain.getTotalSpikeCount(),
                'neurons_active': brain.getFiringNeuronCount(),
                'memory_usage': brain.getTotalNeuronCount() * 0.01
            }
        }

        analyzer.record_execution_trace(execution_trace)

        # Run simulation step
        brain.step(step)

        # Record system event for bottleneck detection
        if step % 50 == 0:
            analyzer.record_system_event("memory_consolidation", "Memory consolidation triggered", time.time(), 0.8)
        elif step % 25 == 0:
            analyzer.record_system_event("spike_burst", "High spike activity detected", time.time(), 0.6)

    analyzer.stop_analysis()

    # Analyze bottlenecks
    bottlenecks = analyzer.analyze_bottlenecks()

    print("Bottleneck Analysis Results:")
    print(f"  Total bottlenecks detected: {len(bottlenecks)}")
    print(f"  Overall bottleneck severity: {analyzer.get_overall_bottleneck_severity() * 100:.1f}%")

    print("\nIdentified Bottlenecks:")
    for i, bottleneck in enumerate(bottlenecks):
        print(f"  {i+1}. {bottleneck.bottleneck_type} (Severity: {bottleneck.severity * 100:.1f}%)")
        print(f"     Component: {bottleneck.component_name}")
        print(f"     Location: {bottleneck.location}")
        print(f"     Impact: {bottleneck.impact * 100:.1f}%")

    # Get resource analysis
    print(f"\nResource Analysis:")
    print(f"  System Load: {analyzer.analyze_system_load() * 100:.1f}%")
    print(f"  Responsiveness Score: {analyzer.analyze_responsiveness() * 100:.1f}%")
    print(f"  Resource Utilization: {analyzer.analyze_resource_utilization() * 100:.1f}%")

    # Get pattern analysis
    print(f"\nPattern Analysis:")
    print(f"  CPU Usage Pattern: {analyzer.analyze_cpu_usage_patterns() * 100:.1f}%")
    print(f"  Memory Usage Pattern: {analyzer.analyze_memory_usage_patterns() * 100:.1f}%")
    print(f"  I/O Pattern: {analyzer.analyze_io_patterns() * 100:.1f}%")
    print(f"  Network Pattern: {analyzer.analyze_network_patterns() * 100:.1f}%")

    # Generate reports
    bottleneck_report = analyzer.generate_bottleneck_report()
    resource_report = analyzer.generate_resource_report()
    time_series_report = analyzer.generate_time_series_report()
    optimization_report = analyzer.generate_optimization_report()

    print(f"\nReports Generated:")
    print(f"  Bottleneck Report: {len(bottleneck_report)} characters")
    print(f"  Resource Report: {len(resource_report)} characters")
    print(f"  Time Series Report: {len(time_series_report)} characters")
    print(f"  Optimization Report: {len(optimization_report)} characters")

    # Export bottleneck data
    analyzer.export_bottleneck_data("bottleneck_analysis.json", "json")

    print("\nBottleneck analysis exported successfully")

if __name__ == "__main__":
    bottleneck_analysis_example()
