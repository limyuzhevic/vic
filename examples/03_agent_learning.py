"""Example 03: Agent Learning - Complete Learning Loop

This example demonstrates a complete learning agent that explores, learns,
and adapts based on experience with the environment.
"""

import pynlm
import time
import math

print("=== Example 3: Agent Learning ===")
print()

# Create configuration optimized for learning experiments
print("Setting up learning experiment configuration...")
config = pynlm.createDefaultConfig()
config.neuron_count = 500
config.region_count = 1
config.connection_probability = 0.15
config.simulation_timestep = 0.005
config.synaptic_generation_rate = 0.0002

brain = pynlm.createBrain(config)
brain.initialize()

# Create rich world environment
print("Creating rich world environment...")
world = pynlm.createSimpleWorld()
world.configure(
    width=100.0,
    height=100.0,
    vision_width=15,
    vision_height=15
)

# Set agent start position
world.set_agent_start(10.0, 10.0)

# Create agent brain with all learning systems enabled
print("Creating learning agent...")
agent_world = pynlm.createAgentWorld(brain, world)
agent_world.initialize()

# Enable all learning systems
agent_world.enable_reward_modulation(True)
agent_world.enable_structural_plasticity(True)
agent_world.enable_development(True)
agent_world.enable_curiosity(True)

# Set up interesting world with multiple object types
print("Configuring world with interesting objects...")
# Create resource zones
for i in range(5):
    x = 20 + i * 15
    y = 20 + i * 15
    resource = pynlm.WorldObject(x, y, pynlm.WorldObjectType.Resource, 
                                 float(10 + i * 5), 2.0)
    world.addObject(resource)

# Create hazard zones
for i in range(3):
    x = 70 + i * 10
    y = 70 + i * 10
    hazard = pynlm.WorldObject(x, y, pynlm.WorldObjectType.Hazard,
                               float(8 + i * 2), 1.5)
    world.addObject(hazard)

# Add goal marker
marker = pynlm.WorldObject(90.0, 90.0, pynlm.WorldObjectType.Marker, 100.0, 3.0)
world.addObject(marker)

print(f"World setup complete: {world.getWidth()}x{world.getHeight()} units")
print(f"Agent start: ({world.get_agent_body().x:.1f}, {world.get_agent_body().y:.1f})")
print(f"Active objects: {sum(1 for obj in world._objects if obj.active)}")

# Learning statistics
learning_stats = {
    'steps': 0,
    'total_reward': 0.0,
    'successful_actions': 0,
    'curiosity_episodes': 0,
    'novel_experiences': 0,
    'max_curiosity': 0.0,
}

# Main learning loop
print("\nStarting learning loop...")
print("The agent will explore, learn from rewards, and adapt its behavior.")
print()

for step in range(100):
    # Execute one complete agent-world step
    result = agent_world.step(step, 0.1)
    
    # Update learning statistics
    learning_stats['steps'] += 1
    learning_stats['total_reward'] += result.reward
    if result.success:
        learning_stats['successful_actions'] += 1
    
    # Track curiosity and novelty
    curiosity = agent_world._agent.get_curiosity_level() if agent_world._agent else 0.0
    novelty = agent_world._agent.get_novelty_level() if agent_world._agent else 0.0
    
    if curiosity > learning_stats['max_curiosity']:
        learning_stats['max_curiosity'] = curiosity
    
    if curiosity > 0.5:
        learning_stats['curiosity_episodes'] += 1
    if novelty > 0.3:
        learning_stats['novel_experiences'] += 1
    
    # Log progress every 10 steps
    if step % 10 == 0:
        agent_body = world.get_agent_body()
        print(f"Step {step:3d}: Pos({agent_body.x:5.1f},{agent_body.y:5.1f}) | "
              f"Energy {agent_body.energy:4.1f}/{world.get_max_energy():4.1f} | "
              f"Curiosity {curiosity:4.2f} | Novelty {novelty:4.2f} | "
              f"Action {result.success:5s} (R{result.reward:+5.1f})")
        
        # Show nearby objects
        nearby = []
        for obj in world._objects:
            if obj.active and obj.distance_to(agent_body.x, agent_body.y) < 15.0:
                nearby.append((obj.obj_type, obj.distance_to(agent_body.x, agent_body.y)))
        
        if nearby:
            print(f"  Nearby: {', '.join([f'{pynlm.get_object_type_name(t)}({d:.1f})' for t, d in nearby])}")

# Learning analysis and results
print("\n=== Learning Analysis ===")
print(f"Learning episodes: {learning_stats['steps']}")
print(f"Total reward: {learning_stats['total_reward']:.2f}")
print(f"Success rate: {learning_stats['successful_actions'] / learning_stats['steps'] * 100:.1f}%")
print(f"Maximum curiosity: {learning_stats['max_curiosity']:.3f}")
print(f"Novel experiences: {learning_stats['novel_experiences']}")
print(f"Curiosity-driven episodes: {learning_stats['curiosity_episodes']}")

# Final brain statistics
print("\n=== Final Brain Statistics ===")
if agent_world._agent:
    print(f"Developmental stage: {agent_world._agent.get_developmental_stage().name}")
    print(f"Developmental age: {agent_world._agent.get_developmental_age()} steps")
    print(f"Maturation progress: {agent_world._agent.get_maturation_progress():.2f}")
    print(f"Neuromodulation factor: {agent_world._agent.get_plasticity_factor():.3f}")

print(f"\nBrain firing rate: {brain.getAverageFiringRate():.2f} Hz")
print(f"Total spikes: {brain.getTotalSpikeCount()}")
print(f"Average E/I ratio: {brain.getExcitationInhibitionRatio():.2f}")

# Determine learning effectiveness
print("\n=== Learning Effectiveness Assessment ===")
avg_firing_rate = brain.getAverageFiringRate()
learning_efficiency = (learning_stats['total_reward'] / max(1, learning_stats['steps'])) * avg_firing_rate

print(f"Learning efficiency: {learning_efficiency:.4f}")

if learning_efficiency > 0.5:
    print("✓ Excellent learning performance!")
elif learning_efficiency > 0.2:
    print("✓ Good learning performance.")
elif learning_efficiency > 0.1:
    print("⚠ Moderate learning performance.")
else:
    print("⚠ Poor learning performance - may need adjustment.")

print(f"\nDevelopment stage reached: {agent_world._agent.get_developmental_stage().name if agent_world._agent else 'Unknown'}")
print(f"Final agent curiosity level: {learning_stats['max_curiosity']:.3f}")

print()
print("✓ Example completed successfully!")

# Save experience for replay
if hasattr(agent_world, '_agent') and agent_world._agent:
    print(f"\nRecorded {len(agent_world._agent._replay_history)} experiences for replay")
