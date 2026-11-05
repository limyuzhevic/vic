import pynlm

# Quick demonstration of NLM Python bindings

# Create a simple brain with configuration
config = pynlm.create_default_config()
brain = pynlm.create_brain(config)

# Create a world
world = pynlm.create_world()
world.configure(20.0, 20.0, 8, 8)  # width, height, vision_width, vision_height

# Set agent start position
world.setAgentStart(10.0, 10.0)

# Create agent brain
agent = pynlm.create_agent_brain(brain)

# Initialize agent with world
agent.initialize(world)

# Run a simple simulation loop
for step in range(100):
    # Get sensory percept
    percept = world.getSensoryPercept()
    
    # Process sensory input
    agent.processSensoryInput(percept)
    
    # Get motor action
    action = agent.decodeMotorCommand()
    
    # Apply action in world
    result = world.applyMotorCommand(action, float(step))
    
    # Update world
    world.update(0.1)
    
    # Print progress every 10 steps
    if step % 10 == 0:
        print(f"Step {step}: Action={action.getName()}, Reward={result.reward:.2f}")

print("Simulation completed!")
print(f"Final agent position: ({world.getAgentBody().x:.1f}, {world.getAgentBody().y:.1f})")
print(f"Total simulation time: {world.getSimulationTime():.1f}")

# Example of Pythonic features
print("\n=== Pythonic Features Demo ===")

# 1. Properties
print("1. Using Pythonic properties:")
neuron_id = pynlm.NeuronId(42)
print(f"   Neuron ID: {neuron_id}")
print(f"   Is valid: {neuron_id.is_valid}")

# 2. Context managers
print("\n2. Using context managers:")
with pynlm.brain_context(brain, auto_save=False) as ctx:
    brain = ctx.get_brain()
    print(f"   Brain in context: {brain}")

# 3. Configuration helpers
print("\n3. Using configuration helpers:")
config = pynlm.create_config_with_defaults()
print(f"   Config with defaults: {len(config.getKeys())} keys")

# 4. Data utilities
print("\n4. Using data utilities:")
vect = [1.0, 2.0, 3.0, 4.0, 5.0]
normalized = pynlm.normalize_vector(vect)
print(f"   Original vector: {vect}")
print(f"   Normalized: {normalized}")

stats = pynlm.vector_stats(vect)
print(f"   Stats mean: {stats['mean']}, std: {stats['std']}")

# 5. Exception handling
print("\n5. Exception handling:")
try:
    # This would raise an exception in a real scenario
    raise pynlm.ConfigError("Test error")
except pynlm.ConfigError as e:
    print(f"   Caught ConfigError: {e}")

print("\n=== Demo Complete ===")
