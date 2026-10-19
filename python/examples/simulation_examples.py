"""
Example: Simulation and Analysis Examples

This example demonstrates simulation and analysis using the Enhanced NLM module.
"""

from nlm_enhanced import AdvancedConfig, AdvancedBrain, SimulationController, NeuralProfiler
import json
import time

# Example 1: Simulation with different modes
print("Example 1: Sequential Simulation")
print("=" * 40)

config = AdvancedConfig()
config.set("brain.neuron_count", 1000)
config.set("brain.synapse_density", 0.1)

brain = AdvancedBrain(config)
controller = SimulationController(brain)

def simple_monitor(step, metrics):
    if step % 200 == 0:
        print(f"  Step {step}: {metrics['firing_neurons']} firing, "
              f"{metrics['total_spikes']} spikes")

controller.run_episode(500, on_step_complete=simple_monitor)
print("✓ Sequential simulation completed")

# Example 2: Performance monitoring
print("\nExample 2: Performance Monitoring")
print("=" * 40)

controller2 = SimulationController(brain)
controller2.enable_profiling(True)
controller2.enable_benchmarking(True)

start_time = time.time()
controller2.run_episode(300)
end_time = time.time()

performance = controller2.get_performance_metrics()
report = controller2.get_performance_report()
print(f"Performance metrics: {performance:.3f}")
print(f"Performance report:\n{report}")

# Example 3: Error handling
print("\nExample 3: Error Handling")
print("=" * 40)

controller3 = SimulationController(brain)
# Simulate an error
controller3.add_error(100, "test_error", "Test error message",
                      "Fix this error", 200)

if controller3.has_errors():
    print("Errors detected:")
    print(controller3.get_error_report())
else:
    print("No errors detected")

# Example 4: Event system
print("\nExample 4: Event System")
print("=" * 40)

def event_handler(data):
    print(f"  Event handler called: {data}")

controller4 = SimulationController(brain)
controller4.add_event_handler("simulation_complete", event_handler)
controller4.add_event("simulation_complete", {"step": 100, "reason": "test"})
controller4.process_events()

# Example 5: Checkpoint management
print("\nExample 5: Checkpoint Management")
print("=" * 40)

controller5 = SimulationController(brain)
success = controller5.save_checkpoint("test_checkpoint.nlm")
if success:
    print("✓ Checkpoint saved")
    # Restore from checkpoint
    success2 = controller5.restore_checkpoint("test_checkpoint.nlm")
    if success2:
        print("✓ Checkpoint restored")
    else:
        print("✗ Checkpoint restoration failed")
else:
    print("✗ Checkpoint save failed")

# Example 6: Statistical analysis
print("\nExample 6: Statistical Analysis")
print("=" * 40)

# Create time series data
config6 = AdvancedConfig()
config6.add_time_series("brain.neuron_count", 0.0, "1000")
config6.add_time_series("brain.neuron_count", 1.0, "1100")
config6.add_time_series("brain.neuron_count", 2.0, "1200")
config6.add_time_series("brain.neuron_count", 3.0, "1150")
config6.add_time_series("brain.neuron_count", 4.0, "1300")

# Get correlations
correlations = config6.get_correlations()
print("Correlations:")
for corr in correlations:
    print(f"  {corr['key1']} ↔ {corr['key2']}: {corr['correlation']:.3f}")

# Example 7: Data export/import
print("\nExample 7: Data Export/Import")
print("=" * 40)

# Export results
results = controller5.export_results("json")
print("Results exported (JSON format)")

# Import results
controller5.import_results(results, "json")
print("Results imported")

# Example 8: Configuration validation
print("\nExample 8: Configuration Validation")
print("=" * 40)

config8 = AdvancedConfig()
config8.add_schema("brain.neuron_count", "int", [100, 10000])
config8.add_schema("brain.synapse_density", "float", [0.0, 1.0])

# Add invalid configuration
config8.set("invalid.param", "not_a_number")

# Validate
errors = config8.validate()
if errors:
    print("Validation errors:")
    for error in errors:
        print(f"  {error.key}: {error.message} ({error.severity})")
else:
    print("No validation errors")

# Example 9: Neural profiling
print("\nExample 9: Neural Profiling")
print("=" * 40)

profiler = NeuralProfiler(brain)

# Profile network
network_profile = profiler.profile_network()
print(f"Network Profile:")
print(f"  Neurons: {network_profile.total_neuron_count}")
print(f"  Synapses: {network_profile.total_synapse_count}")
print(f"  Average Firing Rate: {network_profile.average_firing_rate:.3f} Hz")

# Profile dynamics
dynamics_profile = profiler.profile_dynamics()
print(f"\nDynamics Profile:")
print(f"  Average Membrane Potential: {dynamics_profile.average_membrane_potential:.3f} mV")

# Generate visualization data
viz_data = profiler.generate_visualization_data("json")
print(f"\nVisualization data generated ({len(viz_data)} characters)")

# Get validation report
validation_report = profiler.get_validation_report()
print(f"\nValidation Report (first 200 chars): {validation_report[:200]}...")

# Example 10: Configuration recommendations
print("\nExample 10: Configuration Recommendations")
print("=" * 40)

recommendations = config8.get_recommendations()
if recommendations:
    print("Recommendations:")
    for rec in recommendations:
        print(f"  • {rec}")
else:
    print("No recommendations available")

optimization_suggestions = config8.get_optimization_suggestions()
print(f"\nOptimization Suggestions:\n{optimization_suggestions}")

print("\n" + "=" * 50)
print("All simulation and analysis examples completed!")
print("=" * 50)