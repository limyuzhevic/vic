"""
Example: Basic Enhanced NLM Usage

This example demonstrates basic usage of the Enhanced NLM module.
"""

from nlm_enhanced import AdvancedConfig, AdvancedBrain, SimulationController, NeuralProfiler

# Create enhanced configuration
print("Creating enhanced configuration...")
config = AdvancedConfig()
config.add_schema("brain.neuron_count", "int", "Number of neurons", [100, 5000])
config.add_schema("brain.synapse_density", "float", "Synapse density", [0.0, 1.0])

# Set configuration values
config.set("brain.neuron_count", 1000)
config.set("brain.synapse_density", 0.1)

# Validate configuration
if config.validate_schema():
    print("✓ Configuration is valid")
else:
    print("✗ Configuration has errors")
    for error in config.validate():
        print(f"  {error.key}: {error.message}")

# Create enhanced brain
print("\nCreating enhanced brain...")
brain = AdvancedBrain(config)

# Run a simple simulation
print("Running simulation...")
controller = SimulationController(brain)

# Custom step callback
def on_step(step, metrics):
    if step % 100 == 0:
        print(f"Step {step}: {metrics['firing_neurons']} firing neurons, "
              f"{metrics['total_spikes']} total spikes")

controller.run_episode(500, on_step_complete=on_step)

# Get performance report
print("\nPerformance Report:")
print(controller.get_performance_report())

print("\nBasic example completed successfully!")
print("\nEnhanced NLM Basic Usage Complete")
print("=================================")
print(f"Brain: {brain}")
print(f"Simulation Controller: {controller}")
print("=" * 40)