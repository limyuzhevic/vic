from pynlm import *
import time
import matplotlib.pyplot as plt

print("NLM Python Bindings Example: Population Dynamics")
print("=" * 60)

# Create configuration for population study
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 500)
config.set("brain.synapse_density", 0.05)

# Create brain and initialize
brain = pynlm.createBrain(config)
brain.initialize()

print(f"Created brain with {brain.getTotalNeuronCount()} neurons")

# Create world for simulation
world = pynlm.createSimpleWorld()
world.configure(width=30, height=30, visionWidth=10, visionHeight=10)
world.reset()

# Create agent
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

print("Created agent and world")

# Simulation parameters
steps = 100
time_step = 0
firing_rates = []
population_activities = []

# Run simulation
for i in range(steps):
    # Update world
    world.update(0.05)
    
    # Process sensory input
    percept = world.getSensoryPercept()
    if percept:
        agent.processSensoryInput(percept)
    
    # Step brain
    brain.step(time_step)
    
    # Record statistics
    firing_count = brain.getFiringNeuronCount()
    firing_rates.append(firing_count / brain.getTotalNeuronCount())
    population_activities.append(firing_count)
    
    if i % 20 == 0:
        print(f"Step {i}: {firing_count}/{brain.getTotalNeuronCount()} neurons firing")
    
    time_step += 1

# Plot results
plt.figure(figsize=(12, 8))

plt.subplot(2, 1, 1)
plt.plot(firing_rates)
plt.title("Firing Rate Over Time")
plt.xlabel("Time Step")
plt.ylabel("Firing Rate (0-1)")
plt.grid(True)

plt.subplot(2, 1, 2)
plt.plot(population_activities)
plt.title("Total Firing Neurons Over Time")
plt.xlabel("Time Step")
plt.ylabel("Firing Neurons")
plt.grid(True)

plt.tight_layout()
plt.savefig('population_dynamics.png', dpi=150)
print(f"\nSaved visualization to 'population_dynamics.png'")

# Show summary statistics
print(f"\nSimulation Summary:")
print(f"Total steps: {steps}")
print(f"Average firing rate: {sum(firing_rates)/len(firing_rates):.3f}")
print(f"Max firing rate: {max(firing_rates):.3f}")
print(f"Min firing rate: {min(firing_rates):.3f}")
print(f"Average population activity: {sum(population_activities)/len(population_activities):.0f}")

print("\nPopulation dynamics example completed successfully!")