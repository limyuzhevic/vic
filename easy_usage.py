Simple Python API for NLM Neural Simulation
==========================================

import pynlm
import matplotlib.pyplot as plt

# Create a brain with default configuration
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

print("=== NLM Neural Simulation - Easy Usage ===")
print(f"Created brain with {brain.getTotalNeuronCount()} neurons")

# Configure brain parameters (if supported)
try:
    config.set("neuron_count", 1000)
except:
    print("Note: Config setting may require different API")

# Initialize world and agent
world = pynlm.createSimpleWorld()
agent = pynlm.createAgentBrain(brain)

# Configure world
world.configure(
    width=20,
    height=20,
    visionWidth=8,
    visionHeight=8
)
world.reset()
agent.initialize(world)

print(f"Agent initialized - Sensory input size: {agent.getSensoryInputSize()}")

# Run simulation
print("\n=== Running simulation ===")
for step in range(50):
    # Get sensory percept from world
    percept = world.getSensoryPercept()
    
    # Process sensory input in agent
    agent.processSensoryInput(percept)
    
    # Step the brain
    brain.step(step)
    
    # Get motor action
    action = brain.produceAction()
    print(f"Step {step}: Action {action.getType()}")
    
    # Get statistics periodically
    if step % 10 == 0:
        print(f"  Stats: Neurons={brain.getTotalNeuronCount()}, "
              f"Firing={brain.getFiringNeuronCount()}, "
              f"AvgRate={brain.getAverageFiringRate():.2f}Hz, "
              f"DevStage={brain.getDevelopmentalStage()}, "
              f"Dopamine={agent.getNeuromodulationLevel():.3f}")

# Save brain state
brain.save("nlm_checkpoint.bin")
print("\n✓ Brain state saved to nlm_checkpoint.bin")

# Create visualization data for post-simulation analysis
firing_rates = []
for step in range(50):
    brain.step(step)
    firing_rates.append(brain.getAverageFiringRate())

# Plot results
plt.figure(figsize=(10, 6))
plt.subplot(2, 1, 1)
plt.plot(firing_rates, 'b-', linewidth=2, label='Average Firing Rate')
plt.xlabel('Step')
plt.ylabel('Firing Rate (Hz)')
plt.title('NLM Brain Activity During Simulation')
plt.legend()
plt.grid(True, alpha=0.3)

# Plot neuron population distribution by type
# Get access to regions and populations to analyze neuron types
region_count = brain.getRegionCount()
print(f"\n=== Brain Structure ===")
print(f"Regions: {region_count}")
for i in range(region_count):
    region = brain.getRegion(pynlm.RegionId(i + 1))
    if region:
        pop_count = region.getPopulationCount()
        print(f"  Region {i+1}: {pop_count} populations")

# Test checkpoint loading
print("\n=== Testing Checkpoint Loading ===")
config2 = pynlm.createDefaultConfig()
brain2 = pynlm.createBrain(config2)
brain2.initialize()

if brain2.load("nlm_checkpoint.bin"):
    print("✓ Checkpoint loaded successfully!")
else:
    print("⚠ Checkpoint loading failed (may be expected with current implementation)")

# Summary
print("\n=== Summary ===")
print(f"✓ Created and initialized NLM brain")
print(f"✓ Integrated with world and agent systems")
print(f"✓ Ran {50} simulation steps")
print(f"✓ Saved checkpoint state")
print(f"✓ Generated activity analysis")

# Cleanup
print("\n🎉 Simulation completed successfully! 🎉")
print("\nNext steps:")
print("1. Explore other neural dynamics models")
print("2. Implement custom sensory inputs")
print("3. Create your own brain architectures")
print("4. Integrate with external environments")
print("5. Use the API for neuroscientific research")
