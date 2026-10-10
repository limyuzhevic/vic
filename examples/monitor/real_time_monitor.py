#!/usr/bin/env python3
"""
Real-time Monitoring Example

This example demonstrates comprehensive real-time performance monitoring
for NLM neural simulations.
"""

import pynlm
import time
from monitor import PerformanceMonitor, NeuralMonitor, MemoryMonitor

def real_time_monitor_example():
    # Setup monitor
    performance_monitor = PerformanceMonitor()
    neural_monitor = NeuralMonitor()
    memory_monitor = MemoryMonitor()

    performance_monitor.start_monitoring()
    neural_monitor.start_monitoring()
    memory_monitor.start_tracking()

    # Create brain
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()

    # Run simulation with monitoring
    for step in range(1000):
        brain.step(step)

        # Record performance metrics
        performance_monitor.record_metric("step_time", time.time())
        performance_monitor.record_metric("spike_count", brain.getTotalSpikeCount())

        # Record neural activity
        # Note: In a real implementation, you would get actual neuron IDs and activity
        # For this example, we'll simulate the data
        neurons = list(range(100))  # Simulate 100 neurons
        activity = brain.getAverageFiringRate()
        neural_monitor.record_population_activity(neurons, activity)

        # Record memory usage
        # Note: In a real implementation, you would get actual memory usage data
        memory_monitor.track_allocation(1024, "simulation_step")  # Simulate allocation
        memory_monitor.track_deallocation(512, "simulation_step")  # Simulate deallocation

        if step % 100 == 0:
            print(f"Step {step}: {brain.getTotalSpikeCount()} spikes")

    performance_monitor.stop_monitoring()
    neural_monitor.stop_monitoring()
    memory_monitor.stop_tracking()

    # Get metrics
    metrics = performance_monitor.get_metrics("step_time", 0, 1000)
    spikes = neural_monitor.get_total_spikes()
    current_memory = memory_monitor.get_current_memory_usage()

    print(f"Simulation completed with {spikes} spikes")
    print(f"Peak memory usage: {memory_monitor.get_peak_memory_usage()} bytes")
    print(f"Current memory usage: {current_memory} bytes")

    # Generate dashboard
    dashboard = performance_monitor.generate_dashboard_html()
    with open("performance_dashboard.html", "w") as f:
        f.write(dashboard)

    # Export metrics
    performance_monitor.export_metrics("performance_metrics.json", "json")

    print("Dashboard and metrics exported successfully")

if __name__ == "__main__":
    real_time_monitor_example()
