#!/usr/bin/env python3
"""
Neural Activity Monitoring Example

This example demonstrates neural activity monitoring and analysis for NLM neural simulations.
"""

import pynlm
import time
import random
from monitor import NeuralMonitor, PerformanceMonitor

def neural_activity_monitor_example():
    # Setup monitors
    neural_monitor = NeuralMonitor()
    performance_monitor = PerformanceMonitor()

    neural_monitor.start_monitoring()
    performance_monitor.start_monitoring()

    # Create brain
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()

    # Run simulation with neural activity monitoring
    print("Starting neural activity monitoring...")
    for step in range(500):
        brain.step(step)

        # Record performance metrics
        performance_monitor.record_metric("step_time", time.time())
        performance_monitor.record_metric("spike_count", brain.getTotalSpikeCount())
        performance_monitor.record_metric("firing_rate", brain.getAverageFiringRate())
        performance_monitor.record_metric("prediction_error", brain.getPredictionError())

        # Record neural activity
        # Simulate neural activity monitoring
        neurons = list(range(100))  # Simulate 100 neurons

        # Record spike activity
        if step % 10 == 0:
            # Simulate spike recording
            neural_monitor.record_spike(42, time.time(), 1.5)  # Neuron 42 spikes with amplitude 1.5
            neural_monitor.record_spike(99, time.time(), 2.0)  # Neuron 99 spikes with amplitude 2.0

        # Record population activity
        activity = brain.getAverageFiringRate() + (random.random() - 0.5) * 0.1  # Add some noise
        neural_monitor.record_population_activity(neurons, activity, "cortical_population")

        # Record synaptic activity
        # In a real implementation, you would get actual synaptic data
        if step % 20 == 0:
            synaptic_activity = {
                'synapse_id': f"synapse_{step}",
                'pre_neuron_id': 42,
                'post_neuron_id': 99,
                'weight_change': 0.01 * (1 if random.random() > 0.5 else -1),
                'timestamp': time.time(),
                'efficacy': 0.5
            }
            neural_monitor.record_synaptic_activity(synaptic_activity)

        # Update network node
        network_node = {
            'neuron_id': 42,
            'region_id': 1,
            'population_id': 10,
            'node_type': "neuron",
            'connectivity': 15.0,
            'outgoing_synapses': [f"synapse_{step}", f"synapse_{step+1}"],
            'incoming_synapses': [f"synapse_{step-1}", f"synapse_{step}"]
        }
        neural_monitor.update_network_node(network_node)

        # Add synapse connection
        if step % 25 == 0:
            neural_monitor.add_synapse_connection(f"synapse_{step}", 42, 99)

        if step % 100 == 0:
            print(f"Step {step}: {brain.getTotalSpikeCount()} spikes, "
                  f"firing rate: {brain.getAverageFiringRate():.2f} Hz")

    neural_monitor.stop_monitoring()
    performance_monitor.stop_monitoring()

    # Get neural activity statistics
    print(f"\nNeural Activity Monitoring Results:")
    print(f"  Total spikes: {neural_monitor.get_total_spikes()}")
    print(f"  Firing neurons: {neural_monitor.get_firing_neurons()}")
    print(f"  Average firing rate: {neural_monitor.get_average_firing_rate():.2f} Hz")
    print(f"  Active neuron count: {neural_monitor.get_active_neuron_count()}")
    print(f"  Refractory neuron count: {neural_monitor.get_refractory_neuron_count()}")
    print(f"  Network efficiency: {neural_monitor.calculate_network_efficiency():.4f}")

    # Get network analysis
    network_analysis = neural_monitor.analyze_network_topology()
    print(f"\nNetwork Topology Analysis:")
    for key, value in network_analysis.items():
        print(f"  {key}: {value:.4f}")

    # Get synaptic analysis
    synaptic_analysis = neural_monitor.analyze_synaptic_dynamics()
    print(f"\nSynaptic Dynamics Analysis:")
    for key, value in synaptic_analysis.items():
        print(f"  {key}: {value:.4f}")

    # Detect synchronized groups
    synchronized_groups = neural_monitor.detect_synchronized_groups(0.7)
    print(f"\nSynchronized Groups: {len(synchronized_groups)}")
    for i, group in enumerate(synchronized_groups[:3]):  # Show first 3 groups
        print(f"  Group {i+1}: {len(group)} neurons")

    # Detect burst neurons
    burst_neurons = neural_monitor.detect_burst_neurons(10.0)  # 10 second window
    print(f"\nBurst Neurons: {len(burst_neurons)}")

    # Get current metrics
    current_metrics = neural_monitor.get_current_firing_rate()
    total_connections = neural_monitor.get_total_connections()
    network_activity = neural_monitor.get_network_activity_level()

    print(f"\nCurrent Neural Metrics:")
    print(f"  Firing rate: {current_metrics:.2f} Hz")
    print(f"  Total connections: {total_connections}")
    print(f"  Network activity level: {network_activity:.4f}")

    # Generate dashboard
    dashboard = performance_monitor.generate_dashboard_html()
    with open("neural_dashboard.html", "w") as f:
        f.write(dashboard)

    # Export metrics
    performance_monitor.export_metrics("neural_metrics.json", "json")

    print("\nNeural activity monitoring completed successfully")
    print("Dashboard and metrics exported")

if __name__ == "__main__":
    neural_activity_monitor_example()
