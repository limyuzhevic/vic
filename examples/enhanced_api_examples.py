# Example: Using NLM Python Bindings
# This file demonstrates the enhanced Python API for NLM (Neural Learning Machine)

import pynlm
import time

# Example 1: Basic Brain Usage
print("=== Example 1: Basic Brain Usage ===")
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Configure the brain with common settings
brain.getConfig().configure_brain(
    neuron_count=500,
    synapse_density=0.1,
    connection_probability=0.05
)

# Run a simple simulation
print(f"Brain initialized with {brain.getTotalNeuronCount()} neurons and {brain.getTotalSynapseCount()} synapses")
print(f"Initial firing rate: {brain.getAverageFiringRate():.4f}")

# Run 100 simulation steps
for i in range(100):
    brain.step(i)

print(f"After 100 steps, firing rate: {brain.getAverageFiringRate():.4f}")
print(f"Total spikes: {brain.getTotalSpikeCount()}")

# Example 2: World and Agent Interaction
print("\n=== Example 2: World and Agent Interaction ===")
brain2 = pynlm.createBrain(pynlm.createDefaultConfig())
brain2.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()
world.setAgentStart(10.0, 10.0)

agent = pynlm.createAgentBrain(brain2)
agent.initialize(world)

# Enable learning capabilities
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableDevelopment(True)

print("Running agent simulation...")
for step in range(50):
    world.update(0.1)
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    brain2.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    if step % 10 == 0:
        print(f"  Step {step}: {agent.getThoughtTrace()}")

print("Simulation completed!")

# Example 3: Advanced Configuration
print("\n=== Example 3: Advanced Configuration ===")
config3 = pynlm.createDefaultConfig()

# Configure brain with specific settings
config3.set("brain.neuron_count", 1000)
config3.set("brain.synapse_density", 0.15)
config3.set("plasticity.stdp.enable", True)
config3.set("plasticity.stdp.learning_rate", 0.002)
config3.set("neuromod.curiosity.enable", True)
config3.set("neuromod.novelty.enable", True)
config3.set("neuromod.dopamine.scale", 1.5)

brain3 = pynlm.createBrain(config3)
brain3.initialize()

print(f"Configured brain stats:")
print(f"  Neurons: {brain3.getTotalNeuronCount()}")
print(f"  Synapses: {brain3.getTotalSynapseCount()}")
print(f"  Developmental Stage: {brain3.getDevelopmentalStage()}")

# Example 4: Training an Agent
print("\n=== Example 4: Training an Agent ===")

# Create agent environment
brain4 = pynlm.createBrain(pynlm.createDefaultConfig())
brain4.initialize()

world4 = pynlm.createSimpleWorld()
world4.configure(width=15, height=15, visionWidth=8, visionHeight=8)
world4.reset()

agent4 = pynlm.createAgentBrain(brain4)
agent4.initialize(world4)
agent4.enable_subsystems(reward_modulation=True, curiosity=True, development=True)

print("Starting agent training...")

# Simple training loop with exploration
reward_history = []
curiosity_history = []

for episode in range(10):
    episode_reward = 0.0
    
    # Reset for new episode
    agent4.reset()
    brain4.reset()
    brain4.initialize()
    
    for step in range(100):
        # Get current state
        percept = world4.getSensoryPercept()
        
        # Process sensory input
        agent4.processSensoryInput(percept)
        
        # Run brain step
        brain4.step(step)
        
        # Decode action
        action = agent4.decodeMotorCommand()
        
        # Apply action and get reward
        result = world4.applyMotorCommand(action, world4.getSimulationTime())
        
        # Apply reward modulation
        agent4.applyRewardModulation(result.reward, 0.0)
        
        # Update development
        agent4.updateDevelopment(0.1)
        
        episode_reward += result.reward
        reward_history.append(result.reward)
        curiosity_history.append(agent4.getCuriosityLevel())
    
    print(f"Episode {episode + 1}: Total Reward = {episode_reward:.2f}, "
          f"Avg Curiosity = {sum(curiosity_history[-100:]) / min(100, len(curiosity_history)):.3f}")

print(f"Training completed. Final curiosity: {agent4.getCuriosityLevel():.3f}")

# Example 5: Visualization Utilities
print("\n=== Example 5: Visualization Utilities ===")
print("Activity Summary:")
brain.get_stats()

# Note: In a real implementation, these would use matplotlib/seaborn
# For this example, we just show the data
stats = brain.get_stats()
print(f"  Total Neurons: {stats['total_neurons']}")
print(f"  Total Synapses: {stats['total_synapses']}")
print(f"  Average Firing Rate: {stats['average_firing_rate']:.4f} Hz")
print(f"  E/I Ratio: {stats['e_i_ratio']:.3f}")
print(f"  Total Spikes: {stats['total_spikes']}")

# Example 6: Checkpoint Management
print("\n=== Example 6: Checkpoint Management ===")
brain_save = pynlm.createBrain(pynlm.createDefaultConfig())
brain_save.initialize()

# Run some simulation steps
for i in range(50):
    brain_save.step(i)

# Save brain state
brain_save.save("brain_checkpoint.bin")
print(f"Brain saved to 'brain_checkpoint.bin'")

# Create new brain and load state
brain_load = pynlm.createBrain(pynlm.createDefaultConfig())
brain_load.load("brain_checkpoint.bin")

print(f"Loaded brain has {brain_load.getTotalNeuronCount()} neurons")
print(f"Loaded brain has {brain_load.getTotalSynapseCount()} synapses")

# Example 7: Error Handling
print("\n=== Example 7: Error Handling ===")
try:
    config_bad = pynlm.createDefaultConfig()
    # This would normally cause an error if the key doesn't exist
    # For demonstration, we'll just show how to check for keys
    if config_bad.has("nonexistent.key"):
        value = config_bad.get("nonexistent.key")
        print(f"Found value: {value}")
    else:
        print("Key 'nonexistent.key' not found (expected)")
except Exception as e:
    print(f"Error caught: {e}")

print("\n=== All Examples Completed ===")
print("The enhanced NLM Python bindings provide:")
print("  - High-level APIs for common use cases")
print("  - Better error handling and validation")
print("  - Convenience methods for configuration")
print("  - Training and testing utilities")
print("  - Visualization and monitoring tools")
print("  - Checkpoint management")
print("  - Episode-based agent interaction")

# Clean up (in a real implementation, you might want to use context managers)
import os
if os.path.exists("brain_checkpoint.bin"):
    os.remove("brain_checkpoint.bin")
    print("Cleaned up checkpoint file")
