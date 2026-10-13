"""Example 02: Brain-World Integration

This example demonstrates how a NLM brain interacts with a simulated world
through the AgentBrain interface.
"""

import pynlm
import time

print("=== Example 2: Brain-World Integration ===")
print()

# Create brain and configure it
print("Creating brain with custom configuration...")
config = pynlm.createDefaultConfig()
config.neuron_count = 200
config.region_count = 1
brain = pynlm.createBrain(config)

# Create world and configure it
print("Creating and configuring world...")
world = pynlm.createSimpleWorld()
world.configure(
    width=50.0, 
    height=50.0, 
    vision_width=10, 
    vision_height=10
)

# Create agent brain interface
print("Creating agent brain interface...")
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable key learning systems
print("Enabling learning systems...")
agent.enable_reward_modulation(True)
agent.enable_curiosity(True)
agent.enable_development(True)

# Set up some world objects
print("Setting up world objects...")
# Add a resource (food) at position (25, 25)
resource = pynlm.WorldObject(25.0, 25.0, pynlm.WorldObjectType.Resource, 10.0, 1.0)
world.addObject(resource)

# Add a hazard (pit) at position (10, 10)
hazard = pynlm.WorldObject(10.0, 10.0, pynlm.WorldObjectType.Hazard, 5.0, 1.5)
world.addObject(hazard)

print(f"Resource at (25, 25) with value {resource.value}")
print(f"Hazard at (10, 10) with damage {hazard.value}")

# Run interaction loop
print("\nStarting interaction loop...")
for step in range(30):
    # Update world
    world.update(0.1)
    
    # Get sensory percept
    percept = world.getSensoryPercept()
    
    # Process sensory input (brain perceives)
    agent.processSensoryInput(percept)
    
    # Brain thinks
    brain.step(step)
    
    # Brain decides action
    action = agent.decodeMotorCommand()
    
    # Execute action in world
    result = world.applyMotorCommand(action, world.get_simulation_time())
    
    # Learn from experience
    agent.apply_reward_modulation(result.reward, agent.get_prediction_error())
    
    # Log occasionally
    if step % 5 == 0:
        print(f"Step {step:2d}: Action {action.name:15s} | Reward {result.reward:5.1f} | "
              f"Success {result.success:5s} | {agent.get_developmental_stage().name:15s}")
    
    # Small delay for readability
    time.sleep(0.1)

# Final status
print("\n=== Final Status ===")
print(f"Brain firing rate: {brain.getAverageFiringRate():.2f} Hz")
print(f"Agent developmental stage: {agent.get_developmental_stage().name}")
print(f"Agent curiosity level: {agent.get_curiosity_level():.3f}")
print(f"Agent neuromodulation level: {agent.get_neuromodulation_level():.3f}")
print(f"World simulation time: {world.get_simulation_time():.2f}s")

print()
print("✓ Example completed successfully!")

# Show detailed statistics
print("\n=== Detailed Statistics ===")
brain.log_status()
agent.log_status()
