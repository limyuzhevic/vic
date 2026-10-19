"""
Example: Advanced Enhanced NLM Usage

This example demonstrates advanced usage of the Enhanced NLM module.
"""

from nlm_enhanced import AdvancedConfig, AdvancedBrain, SimulationController, NeuralProfiler
import time

# Create enhanced configuration
print("Creating enhanced configuration with schema...")
config = AdvancedConfig()
config.add_schema("brain.neuron_count", "int", "Number of neurons", [100, 10000])
config.add_schema("brain.synapse_density", "float", "Synapse density", [0.0, 1.0])
config.add_schema("plasticity.stdp.enable", "bool", "Enable STDP plasticity", None)
config.add_schema("neuromod.curiosity.enable", "bool", "Enable curiosity-driven exploration", None)

# Set advanced configuration
config.set("brain.neuron_count", 2000)
config.set("brain.synapse_density", 0.15)
config.set("plasticity.stdp.enable", True)
config.set("neuromod.curiosity.enable", True)

# Export configuration
print("\nExporting configuration to JSON...")
json_config = config.export_to_json()
config.import_from_json(json_config)  # Round-trip test
print("✓ Configuration export/import completed")

# Create enhanced brain with advanced features
print("\nCreating enhanced brain with advanced features...")
brain = AdvancedBrain(config)

# Enable advanced learning
print("\nEnabling advanced learning...")
brain.enable_supervised_learning({1: 1.0, 2: 0.8, 3: 0.6, 4: 0.4}, 0.01)
brain.enable_unsupervised_learning(0.02)
brain.enable_reinforcement_learning(1.5)

# Enable advanced neuromodulation
brain.apply_complex_neuromodulation({
    "dopamine": 1.0,
    "serotonin": 0.8,
    "curiosity": 0.9,
    "novelty": 0.7,
    "prediction_error": 1.2
}, 1.0)

# Run adaptive simulation
print("\nRunning adaptive simulation...")
controller = SimulationController(brain)
controller.enable_debug_mode(True)

# Custom step callback for monitoring
def on_step(step, metrics):
    if step % 100 == 0:
        performance = brain.get_performance_metrics()
        print(f"Step {step}: {metrics['firing_neurons']} firing neurons, "
              f"{performance:.3f} performance score")

controller.run_adaptive_simulation()

# Get comprehensive analysis
print("\nPerforming comprehensive analysis...")
profiler = NeuralProfiler(brain)

# Profile network
network_profile = profiler.profile_network()
print(f"\nNetwork Profile:")
print(f"  Neurons: {network_profile.total_neuron_count}")
print(f"  Synapses: {network_profile.total_synapse_count}")
print(f"  Average Firing Rate: {network_profile.average_firing_rate:.3f} Hz")
print(f"  E/I Ratio: {network_profile.excitation_inhibition_ratio:.3f}")
print(f"  Firing Neurons: {network_profile.firing_neuron_count} ({network_profile.firing_neuron_count/network_profile.total_neuron_count*100:.1f}%)")

# Profile dynamics
dynamics_profile = profiler.profile_dynamics()
print(f"\nDynamics Profile:")
print(f"  Average Membrane Potential: {dynamics_profile.average_membrane_potential:.3f} mV")
print(f"  Firing Rate Variability: {dynamics_profile.firing_rate_variability:.3f}")
print(f"  Learning Efficiency: {dynamics_profile.learning_efficiency:.3f}")

# Get validation report
print(f"\nValidation Report:")
print(profiler.get_validation_report())

# Get performance report
print(f"\nBrain Performance Report:")
print(brain.get_benchmark_report())

# Save advanced brain state
print("\nSaving advanced brain state...")
brain.save_advanced("advanced_brain_state.json", "json", 
                   {"description": "Advanced brain state after complex simulation",
                    "timestamp": time.time(),
                    "training_episodes": 1})

# Create snapshot
snapshot = brain.create_snapshot()
print(f"✓ Snapshot created: {snapshot[:50]}...")

# Demonstrate batch operations
print("\nDemonstrating batch operations...")
brain.batch_inject_current([1, 2, 3, 4, 5], 25.0)
brain.pattern_injection(["Excitatory"], 20.0, 3)

# Demonstrate memory operations
print("\nDemonstrating memory operations...")
brain.consolidate_memory(1.0)
brain.replay_memory(50, 0.8)

print("\nAdvanced example completed successfully!")
print("\nEnhanced NLM Advanced Usage Complete")
print("===================================")
print(f"Brain: {brain}")
print(f"Network: {network_profile.total_neuron_count} neurons, {network_profile.total_synapse_count} synapses")
print(f"Performance: {brain.get_performance_metrics():.3f}")
print("=" * 45)