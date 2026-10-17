#!/usr/bin/env python3
"""
Example usage of Phase 2 convenience classes for NLM Python bindings.

This script demonstrates the new high-level API provided by Phase 2 improvements.
"""

# SimpleExperiment example
print("=== SimpleExperiment Example ===")
exp = SimpleExperiment()
exp.configure(
    max_steps=1000,
    timestep=0.01,
    brain=create_brain(create_default_config()),
    world=create_simple_world(),
    agent_brain=create_agent_brain(create_brain(create_default_config()))
)
exp.add_step_callback(lambda step: print(f"Processing step: {step}"))
exp.enable_data_collection(True)

result = exp.run()
if result:
    print(f"Experiment completed successfully!")
    print(f"Total reward: {exp.total_reward}")
    print(f"Final step: {exp.final_step}")
    print(f"Success rate: {exp.compute_success_rate():.2%}")
else:
    print("Experiment failed!")

# BrainTrainer example
print("\n=== BrainTrainer Example ===")
trainer = BrainTrainer()

# Configure training
trainer.configure(
    brain=create_brain(create_default_config()),
    environment=None,  # Would need a real environment
    max_episodes=100,
    learning_rate=0.01
)

# Training loop
for episode in range(100):
    success = trainer.run_episode()
    if success:
        print(f"Episode {episode}: Success, Reward: {trainer.current_reward}")
    else:
        print(f"Episode {episode}: Failed")
    
    if trainer.is_training_complete():
        break

print(f"Training completed. Average reward: {trainer.average_reward}")
print(f"Success rate: {trainer.get_successful_episodes()}/{trainer.get_current_episode()}")

# WorldBuilder example
print("\n=== WorldBuilder Example ===")
builder = WorldBuilder()

# Build a complex world
builder.create_world(
    width=20,
    height=20,
    terrain_type="forest",
    enable_gravity=True
)

builder.add_terrain_object("wall", 5.0, 5.0, 0.5)
builder.add_terrain_object("resource", 10.0, 10.0, 0.5, 2.0)
builder.add_agent(5.0, 5.0, 0.0, 1.0)
builder.add_agent(15.0, 15.0, 3.14, 0.8)
builder.set_weather("rainy")
builder.set_time_of_day(14)

# Get the built world
world = builder.build()
env = builder.build_environment()

if builder.validate_world():
    print("World validation passed!")
else:
    errors = builder.get_validation_errors()
    print(f"Validation errors: {errors}")

print(f"World has {builder.get_agent_count()} agents")

# MetricsCollector example
print("\n=== MetricsCollector Example ===")
collector = MetricsCollector()

# Record various metrics
collector.record("reward", 1.5)
collector.record("firing_rate", 42.3)
collector.record("energy", 0.8)
collector.record("step_time", 0.001)

# Record time series data
firing_rates = [40.1, 41.2, 42.3, 43.1, 42.8]
collector.record_time_series("firing_rate", firing_rates, 1000.0)

# Statistical analysis
print(f"Mean reward: {collector.get_mean('reward'):.2f}")
print(f"Std dev firing rate: {collector.get_stddev('firing_rate'):.2f}")
print(f"Min energy: {collector.get_min('energy'):.2f}")
print(f"Max step time: {collector.get_max('step_time'):.4f}")
print(f"Median reward: {collector.get_median('reward'):.2f}")

# Export data
csv_data = collector.to_csv("experiment_metrics.csv")
print(f"Exported {len(collector.get_metric_names())} metrics to CSV")
json_data = collector.to_json()
print(f"Metrics JSON length: {len(json_data)} characters")

# Utility functions example
print("\n=== Utility Functions Example ===")

# create_default_config
config = create_default_config("my_experiment", 500, 0.02)
config.setInt("brain.neuron_count", 1000)
config.setFloat("world.width", 10.0)

# run_simulation
experiment = run_simulation("my_experiment", config, max_steps=500)
if experiment:
    print("Simulation completed successfully!")
    print(f"Metrics collected: {len(experiment.getMetrics())}")

# plot_brain_metrics (basic example)
firing_rates = [10.0, 15.0, 20.0, 25.0, 30.0]
synaptic_weights = [0.1, 0.2, 0.3, 0.4, 0.5]
plot_result = plot_brain_metrics(firing_rates, synaptic_weights, "Test Metrics")
print(f"Plot result: {plot_result}")

# save_experiment_data (basic example)
experiments = [experiment] if experiment else []
if save_experiment_data("experiment_results.csv", experiments):
    print("Experiment data saved successfully!")

print("\n=== All examples completed successfully! ===")