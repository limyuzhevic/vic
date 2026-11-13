# NLM - Easy Usage Guide for Beginners

## What is NLM?

NLM is a **brain simulator** for computers. It lets you create virtual brains that can learn, remember things, and make decisions - just like how your brain works!

Think of it like this:
- **Neurons** = Brain cells that send signals to each other
- **Synapses** = Connections between brain cells
- **Learning** = When connections get stronger or weaker based on what happens
- **Memory** = The brain remembering patterns

That's it! NLM simulates all of this.

**Version**: NLM v0.1.0 (Phase 6: Final Integration)

---

## The 3 Things You Need to Know

1. **Brain** - The virtual brain that thinks
2. **World** - The environment the brain lives in
3. **Agent** - The bridge connecting brain to world

---

## Quick Start (Copy & Paste)

### Example 1: Simplest Brain

```python
import pynlm

# Step 1: Make a brain
brain = pynlm.createBrain(pynlm.createDefaultConfig())

# Step 2: Turn it on
brain.initialize()

# Step 3: Make it think for 100 steps
for i in range(100):
    brain.step(i)  # One step of thinking

print("Done! Your brain thought", brain.getTotalSpikeCount(), "times")
```

### Example 2: Brain with Learning and Memory

```python
import pynlm

# Create a brain with all features enabled
config = pynlm.createDefaultConfig()
config.set_brain_neuron_count(1000)
config.set_plasticity_stdp(True)
config.set_plasticity_hebbian(True)
config.set_neuromod_curiosity(True)
config.set_neuromod_novelty(True)

brain = pynlm.createBrain(config)
brain.initialize()

# Create agent with all learning enabled
agent = pynlm.createAgentBrain(brain)

# Create a simple world
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.setAgentStart(10.0, 10.0)

# Enable all learning subsystems
agent.enableRewardModulation(True)
agent.enableStructuralPlasticity(True)
agent.enableDevelopment(True)
agent.enableCuriosity(True)

# Run learning simulation
for step in range(500):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    # Get reward and learn
    reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
    agent.applyRewardModulation(reward, 0.0)
    agent.updateDevelopment(0.1)

print("Learning simulation complete!")
print("Brain has", brain.getTotalNeuronCount(), "neurons")
print("Brain learned", brain.getTotalSpikeCount(), "spikes")
print("Development stage:", brain.getDevelopmentalStage())
```

### Example 3: Advanced Brain with Memory

```python
import pynlm

# Create a sophisticated brain with all systems
config = pynlm.createDefaultConfig()
config.set_brain_neuron_count(2000)  # More neurons for complex behavior
config.set_brain_synapse_density(0.2)  # More connections

brain = pynlm.createBrain(config)
brain.initialize()

# Create world and agent
world = pynlm.createSimpleWorld()
world.configure(width=30, height=30, visionWidth=16, visionHeight=16)
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable all advanced features
agent.enableRewardModulation(True)
agent.enableStructuralPlasticity(True)
agent.enableDevelopment(True)
agent.enableCuriosity(True)

print("=== Advanced Brain with Memory Demo ===")
print("Brain Type:", type(brain).__name__)
print("Features enabled:")
print("  - Reward modulation:", "Yes" if agent.isRewardModulationEnabled() else "No")
print("  - Structural plasticity:", "Yes" if agent.isStructuralPlasticityEnabled() else "No")
print("  - Development:", "Yes" if agent.isDevelopmentEnabled() else "No")
print("  - Curiosity:", "Yes" if agent.isCuriosityEnabled() else "No")

# Run simulation with memory tracking
memory_snapshots = []
for step in range(200):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    if step % 50 == 0:
        # Capture brain state for analysis
        memory_snapshots.append({
            'step': step,
            'firing_neurons': brain.getFiringNeuronCount(),
            'total_spikes': brain.getTotalSpikeCount(),
            'curiosity': agent.getCuriosityLevel(),
            'novelty': agent.getNoveltyLevel(),
            'development_stage': brain.getDevelopmentalStage()
        })
        
        print(f"Step {step}: {brain.getFiringNeuronCount()} neurons firing, "
              f"curiosity={agent.getCuriosityLevel():.3f}, "
              f"novelty={agent.getNoveltyLevel():.3f}")

print("\n=== Memory Analysis ===")
for snapshot in memory_snapshots:
    print(f"Step {snapshot['step']}: "
          f"Firing={snapshot['firing_neurons']}, "
          f"Spikes={snapshot['total_spikes']}, "
          f"Curiosity={snapshot['curiosity']:.3f}")

print("Advanced brain simulation complete!")
```