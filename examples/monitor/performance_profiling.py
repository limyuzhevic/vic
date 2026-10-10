#!/usr/bin/env python3
"""
Performance Profiling Example

This example demonstrates performance profiling and analysis for NLM neural simulations.
"""

import pynlm
import time
from monitor import PerformanceProfiler

def performance_profiling_example():
    # Setup performance profiler
    profiler = PerformanceProfiler()

    # Create brain
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()

    # Profile simulation performance
    profiler.start_profiling("simulation")

    for step in range(1000):
        start_time = time.time()
        brain.step(step)
        end_time = time.time()

        profiler.record_function_time("step_execution", end_time - start_time)
        profiler.record_function_memory(brain.getTotalNeuronCount() * 100)  # Estimate memory usage
        profiler.record_function_cpu_time(brain.getTotalSpikeCount() * 0.001)  # Estimate CPU time
        profiler.record_custom_metric("spike_count", brain.getTotalSpikeCount())
        profiler.record_custom_metric("firing_rate", brain.getAverageFiringRate())
        profiler.record_system_metric("cpu_usage", 0.5)  # Simulated CPU usage
        profiler.record_system_metric("memory_usage", brain.getTotalNeuronCount() * 0.1)  # MB
        profiler.record_timeline_point("time", step)
        profiler.record_timeline_point("cpu", 0.5)
        profiler.record_timeline_point("memory", brain.getTotalNeuronCount() * 0.1)

    profiler.stop_profiling()

    # Analyze performance
    profile = profiler.get_current_profile()

    print(f"Performance Profile Summary:")
    print(f"  Total time: {profile.total_time:.2f} seconds")
    print(f"  Total function calls: {profile.total_function_calls}")
    print(f"  Total memory allocated: {profile.total_memory_allocated} bytes")
    print(f"  Peak memory usage: {profile.peak_memory_usage} bytes")
    print(f"  Average firing rate: {brain.getAverageFiringRate():.2f} Hz")

    # Get performance statistics
    top_functions = profiler.get_top_functions(10)
    print(f"\nTop 10 functions by execution time:")
    for i, func in enumerate(top_functions):
        func_profile = profiler.get_function_profile(func)
        print(f"  {i+1}. {func}: {func_profile.total_time:.4f}s ({func_profile.call_count} calls)")

    # Identify bottlenecks
    bottlenecks = profiler.identify_bottlenecks()
    print(f"\nPerformance bottlenecks:")
    for bottleneck in bottlenecks:
        print(f"  - {bottleneck}")

    # Generate reports
    profile_report = profiler.generate_profile_report()
    hotspot_report = profiler.generate_hotspot_report()
    memory_report = profiler.generate_memory_report()
    cpu_report = profiler.generate_cpu_report()

    print(f"\nReports generated:")
    print(f"  Profile report: {len(profile_report)} characters")
    print(f"  Hotspot report: {len(hotspot_report)} characters")
    print(f"  Memory report: {len(memory_report)} characters")
    print(f"  CPU report: {len(cpu_report)} characters")

    # Export profile
    profiler.export_profile("performance_profile.json", "json")

    print("\nPerformance profile exported successfully")

if __name__ == "__main__":
    performance_profiling_example()
