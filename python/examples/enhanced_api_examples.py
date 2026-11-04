"""
Enhanced NLM Python Examples
===========================

This file demonstrates the enhanced NLM Python API for creating and running neural simulations.

The enhanced API provides:
- Simplified configuration with factory functions
- Preset configurations for common use cases
- Enhanced wrapper classes for brain, agent, and world
- Comprehensive error handling and validation
- Pythonic API design with fluent builders

Examples:
1. Simple brain simulation
2. Agent in a world
3. Learning scenario
4. Stable behavior scenario
5. Fast prototyping scenario

Each example includes:
- Configuration setup
- Initialization
- Main simulation loop
- Results analysis
"""

import nlm

# Example 1: Simple Brain Simulation
print("=== Example 1: Simple Brain Simulation ===")

# Create a brain with default configuration
brain = nlm.create_brain(neuron_count=1000, connection_probability=0.05)
print(f"Created brain with {brain.get_total_neuron_count()} neurons")
print(f"Expected synapses: {brain.get_total_synapse_count()}")

# Initialize the brain
brain.initialize()
print("Brain initialized")

# Run simulation for 100 steps
brain.run_simulation(100, log_progress=True)
print("Simulation completed")

# Get statistics
stats = brain.get_statistics()
print(f"Statistics:")
print(f"  Neurons firing: {stats['firing_neurons']}")
print(f"  Average firing rate: {stats['average_firing_rate']:.2f} Hz")
print(f"  Total spikes: {stats['total_spikes']}")
print()

# Example 2: Agent in a World
print("=== Example 2: Agent in a World ===")

# Create brain with learning preset for exploration
brain2 = nlm.create_enhanced_brain(preset="learning")
brain2.initialize()
print(f"Created learning brain with {brain2.get_total_neuron_count()} neurons")

# Create world and agent
world = nlm.create_enhanced_world()
agent = nlm.create_enhanced_agent(brain2)

# Initialize agent with world
agent.initialize(world)
print("Agent initialized in world")

# Configure world for interesting environment
world.configure_world(width=20, height=20, vision_width=8, vision_height=8)
world.set_agent_start(10.0, 10.0)

# Enable learning subsystems
agent.enable_reward_modulation(True)
agent.enable_curiosity(True)
agent.enable_development(True)

# Run interaction loop for 100 steps
for step in range(100):
    # Update world
    world.update_world(0.1)
    
    # Get sensory input
    percept = world.get_sensory_percept()
    
    # Process sensory input
    agent.process_sensory_input(percept)
    
    # Brain thinks
    brain2.step(step)
    
    # Get action from brain
    action = agent.decode_motor_command()
    world.apply_motor_command(action, world.get_simulation_time())
    
    # Apply reward based on world state
    if step % 20 == 0:
        agent.apply_reward_modulation(10.0)  # Positive reward
    else:
        agent.apply_reward_modulation(-1.0)  # Small negative reward
    
    # Update development
    agent.update_development(0.1)
    
    # Log progress every 20 steps
    if step % 20 == 0:
        agent_stats = agent.get_agent_statistics()
        print(f"Step {step}:")
        print(f"  Curiosity: {agent_stats['curiosity_level']:.2f}")
        print(f"  Novelty: {agent_stats['novelty_level']:.2f}")
        print(f"  Prediction error: {agent_stats['prediction_error']:.2f}")
        print(f"  Action: {action}")

print("Agent simulation completed")
print()

# Example 3: Learning Scenario
print("=== Example 3: Learning Scenario ===")

# Create a learning scenario with enhanced API
learning_brain = nlm.create_enhanced_brain(preset="learning")
learning_brain.initialize()

print(f"Created learning brain with {learning_brain.get_total_neuron_count()} neurons")
print("Preset: learning (high plasticity for rapid learning)")

# Run learning simulation
learning_brain.run_simulation(500, log_progress=True)

# Get final statistics
final_stats = learning_brain.get_statistics()
print(f"Final statistics:")
print(f"  Total spikes: {final_stats['total_spikes']}")
print(f"  Learning rate: {final_stats['average_firing_rate']:.2f} Hz")
print(f"  Developmental stage: {final_stats['developmental_stage']}")
print()

# Example 4: Stable Behavior Scenario
print("=== Example 4: Stable Behavior Scenario ===")

# Create a stable scenario with enhanced API
stable_brain = nlm.create_enhanced_brain(preset="stable")
stable_brain.initialize()

print(f"Created stable brain with {stable_brain.get_total_neuron_count()} neurons")
print("Preset: stable (low plasticity for stable behavior)")

# Run stable simulation
stable_brain.run_simulation(500, log_progress=True)

# Get final statistics
stable_stats = stable_brain.get_statistics()
print(f"Final statistics:")
print(f"  Total spikes: {stable_stats['total_spikes']}")
print(f"  Firing neurons: {stable_stats['firing_neurons']}")
print(f"  Developmental stage: {stable_stats['developmental_stage']}")
print()

# Example 5: Fast Prototyping Scenario
print("=== Example 5: Fast Prototyping Scenario ===")

# Create a fast scenario with enhanced API
fast_brain = nlm.create_enhanced_brain(preset="fast")
fast_brain.initialize()

print(f"Created fast brain with {fast_brain.get_total_neuron_count()} neurons")
print("Preset: fast (few neurons, high timestep for quick testing)")

# Run fast prototyping simulation
fast_brain.run_simulation(100, log_progress=True)

# Get final statistics
fast_stats = fast_brain.get_statistics()
print(f"Final statistics:")
print(f"  Total spikes: {fast_stats['total_spikes']}")
print(f"  Average firing rate: {fast_stats['average_firing_rate']:.2f} Hz")
print(f"  Developmental stage: {fast_stats['developmental_stage']}")
print()

# Example 6: Configuration Builder
print("=== Example 6: Configuration Builder ===")

# Use configuration builder for custom setup
config = nlm.ConfigBuilder() \
    .set_neuron_count(2000) \
    .set_connection_probability(0.08) \
    .set_simulation_timestep(0.01) \
    .set_random_seed(12345) \
    .enable_plasticity(True) \
    .set_learning_rate(0.005) \
    .preset("learning") \
    .build()

print("Created custom configuration using builder:")
print(f"  Neuron count: {config.get_total_neuron_count()}")
print(f"  Connection probability: {config.get_connection_probability()}")
print(f"  Simulation timestep: {config.get_simulation_timestep()}")
print(f"  Random seed: {config.get_random_seed()}")

# Create brain with custom config
builder_brain = nlm.create_brain(config)
builder_brain.initialize()
print(f"Created brain with custom configuration")
builder_brain.run_simulation(100, log_progress=True)
print()

# Example 7: Validation and Error Handling
print("=== Example 7: Validation and Error Handling ===")

# Create configuration and validate it
config = nlm.create_brain()
valid, errors = nlm.validate_config(config)
print(f"Configuration valid: {valid}")
if not valid:
    print("Errors:")
    for error in errors:
        print(f"  - {error}")

# Try to create invalid configuration
invalid_config = nlm.create_brain()
invalid_config->set_brain_neuron_count(-1)  # Invalid value
valid, errors = nlm.validate_config(invalid_config)
print(f"Invalid configuration valid: {valid}")
print(f"Number of errors: {len(errors)}")
print()

# Example 8: Save and Load
print("=== Example 8: Save and Load ===")

# Create and run simulation
brain = nlm.create_brain()
brain.initialize()
brain.run_simulation(50)

# Save brain state
brain.save_state("brain_checkpoint.nml")
print("Brain state saved to brain_checkpoint.nml")

# Load brain state
loaded_brain = nlm.create_brain()
loaded_brain.load_state("brain_checkpoint.nml")
print("Brain state loaded successfully")

# Verify loaded brain
loaded_stats = loaded_brain.get_statistics()
print(f"Loaded brain statistics:")
print(f"  Total neurons: {loaded_stats['total_neurons']}")
print(f"  Total synapses: {loaded_stats['total_synapses']}")
print()

# Example 9: Constants Access
print("=== Example 9: Constants Access ===")

# Access various constants
print(f"Default timestep: {nlm.DEFAULT_TIMESTEP}")
print(f"Brain threshold: {nlm.BRAIN_V_THRESH} mV")
print(f"Brain resting potential: {nlm.BRAIN_V_REST} mV")
print(f"Brain reset potential: {nlm.BRAIN_V_RESET} mV")
print(f"Membrane time constant: {nlm.BRAIN_TAU_MEM} ms")
print(f"Refractory period: {nlm.BRAIN_TAU_REF} ms")
print(f"Initial weight mean: {nlm.BRAIN_INITIAL_WEIGHT_MEAN}")
print(f"Initial weight standard deviation: {nlm.BRAIN_INITIAL_WEIGHT_STD}")
print()

# Example 10: Presets
print("=== Example 10: Available Presets ===")

presets = nlm.get_available_presets()
print(f"Available presets: {presets}")

# Apply different presets
for preset in presets:
    brain = nlm.create_enhanced_brain(preset=preset)
    brain.initialize()
    stats = brain.get_statistics()
    print(f"Preset '{preset}': {stats['total_neurons']} neurons, {stats['total_spikes']} spikes")

print("\n=== All Examples Completed Successfully ===")