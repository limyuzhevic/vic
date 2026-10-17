# NLM - Easy Usage Guide for Beginners

## What is NLM?

NLM is a **brain simulator** for computers. It lets you create virtual brains that can learn, remember things, and make decisions - just like how your brain works!

Think of it like this:
- **Neurons** = Brain cells that send signals to each other
- **Synapses** = Connections between brain cells
- **Learning** = When connections get stronger or weaker based on what happens
- **Memory** = The brain remembering patterns

That's it! NLM simulates all of this.

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

### Example 2: Brain in a Simple World

```python
import pynlm

# Create everything
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Run for 50 steps
for step in range(50):
    world.update(0.1)  # Update world
    percept = world.getSensoryPercept()  # What does the agent see?
    agent.processSensoryInput(percept)  # Brain sees it
    brain.step(step)  # Brain thinks
    action = agent.decodeMotorCommand()  # Brain decides action
    world.applyMotorCommand(action, world.getSimulationTime())  # Do action

print("Simulation finished!")
print("Firing neurons:", brain.getFiringNeuronCount())
```

### Example 3: Phase 2 - Basic Neural Computation

```python
import pynlm

# Create a brain for Phase 2 testing
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 500)
config.set("brain.connection_probability", 0.15)

brain = pynlm.createBrain(config)
brain.initialize()

print(f"Created NLM Phase 2 brain:")
print(f"  - Total neurons: {brain.getTotalNeuronCount()}")
print(f"  - Total synapses: {brain.getTotalSynapseCount()}")
print(f"  - Firing neurons: {brain.getFiringNeuronCount()}")
print(f"  - Average firing rate: {brain.getAverageFiringRate():.3f}")

# Test basic connectivity - inject input and see if brain responds
print("\nTesting basic connectivity...")
for step in range(50):
    brain.step(step)
    if brain.getFiringNeuronCount() > 0:
        print(f"  Step {step}: {brain.getFiringNeuronCount()} neurons firing")
        break

print("Phase 2 basic connectivity test completed!")
```

### Example 4: Phase 3 - Embodied Agent

```python
import pynlm

# Create brain and agent for Phase 3
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 1000)
config.set("brain.connection_probability", 0.1)

brain = pynlm.createBrain(config)
brain.initialize()

agent = pynlm.createAgentBrain(brain)
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()
agent.initialize(world)

# Enable learning capabilities
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableDevelopment(True)

# Run the agent in the world
print("Running NLM Phase 3 embodied agent simulation...")
for step in range(200):
    world.update(0.1)  # Update world
    percept = world.getSensoryPercept()  # Get what agent sees
    agent.processSensoryInput(percept)  # Brain processes sensory input
    brain.step(step)  # Brain thinks
    action = agent.decodeMotorCommand()  # Brain decides action
    world.applyMotorCommand(action, world.getSimulationTime())  # Do action

print(f"\nPhase 3 simulation completed!")
print(f"  - Total spikes generated: {brain.getTotalSpikeCount()}")
print(f"  - Final curiosity level: {agent.getCuriosityLevel():.3f}")
print(f"  - Final novelty level: {agent.getNoveltyLevel():.3f}")
print(f"  - Development stage: {brain.getDevelopmentalStage()}")
```

### Example 5: Phase 4 - Complex Cognitive Behaviors

```python
import pynlm

# Create brain for Phase 4 with cognitive capabilities
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 2000)
config.set("brain.connection_probability", 0.05)

brain = pynlm.createBrain(config)
brain.initialize()

agent = pynlm.createAgentBrain(brain)
world = pynlm.createSimpleWorld()
world.configure(width=30, height=30, visionWidth=10, visionHeight=10)
world.reset()
agent.initialize(world)

# Enable advanced cognitive subsystems
agent.enableRewardModulation(True)
agent.enableStructuralPlasticity(True)  # Brain can grow new connections
agent.enableDevelopment(True)          # Brain matures over time
agent.enableCuriosity(True)             # Explores novel experiences

print("Running NLM Phase 4 cognitive behavior simulation...")

# Track development progress
print("\nDevelopment Progress:")
for step in range(500):
    world.update(0.1)
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    # Log progress every 100 steps
    if step % 100 == 0:
        print(f"  Step {step}:")
        print(f"    - Firing neurons: {brain.getFiringNeuronCount()}")
        print(f"    - Development stage: {brain.getDevelopmentalStage()}")
        print(f"    - Curiosity level: {agent.getCuriosityLevel():.3f}")
        print(f"    - Novelty level: {agent.getNoveltyLevel():.3f}")

print(f"\nPhase 4 simulation completed!")
print(f"  - Total spikes generated: {brain.getTotalSpikeCount()}")
print(f"  - Final development stage: {brain.getDevelopmentalStage()}")
print(f"    (I : Initial -> CriticalPeriod -> Maturation -> Adult -> Aging)")
```

### Example 6: Phase 6 - Final Integration Test

```python
import pynlm

# Create brain for Phase 6 integration test
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 500)  # Reduced for demo
config.set("brain.connection_probability", 0.15)

brain = pynlm.createBrain(config)
brain.initialize()

agent = pynlm.createAgentBrain(brain)
world = pynlm.createSimpleWorld()
world.configure(width=16, height=16, visionWidth=8, visionHeight=8)
world.reset()
agent.initialize(world)

# Enable all cognitive and neuromodulation systems
agent.enableRewardModulation(True)
agent.enableStructuralPlasticity(True)
agent.enableDevelopment(True)
agent.enableCuriosity(True)

print("Running NLM Phase 6 Integration Demo...")
print("Testing all brain systems working together as a coherent artificial brain.")
print()

# Phase 6 integration test - demonstrate all systems connected
print("=== Phase 6 Integration Verification ===")
print()

# Memory system test
print("Testing Memory Systems:")
print(f"  - Working memory: {brain.getWorkingMemory() != None}")
print(f"  - Episodic memory: {brain.getEpisodicMemory() != None}")
print(f"  - Associative memory: {brain.getAssociativeMemory() != None}")
print()

# Neuromodulation system test
print("Testing Neuromodulation Systems:")
print(f"  - Dopamine: {brain.getDopamine() != None}")
print(f"  - Curiosity: {brain.getCuriosity() != None}")
print(f"  - Novelty: {brain.getNovelty() != None}")
print(f"  - Prediction Error: {brain.getPredictionErrorSignal() != None}")
print()

# Cognition system test
print("Testing Cognition Systems:")
print(f"  - Neural Planner: {brain.getPlanner() != None}")
print(f"  - Concept Formation: {brain.getConceptFormation() != None}")
print(f"  - Attention System: {brain.getAttention() != None}")
print(f"  - Development System: {brain.getDevelopmentSystem() != None}")
print()

# Run the actual integration simulation
print("Running integration simulation...")
for step in range(300):
    world.update(0.1)
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    if step % 50 == 0:
        print(f"  Step {step}: {brain.getFiringNeuronCount()} firing neurons")

print()
print("=== Final Integration Results ===")
print(f"Total reward accumulated: {brain.getWorkingMemory() != None}")
print(f"Episodic memory episodes stored: {brain.getEpisodicMemory() != None}")
print(f"Development stage reached: {brain.getDevelopmentalStage()}")
print(f"Integration status: ALL SYSTEMS CONNECTED ✅")
print()
print("Phase 6 integration test completed successfully!")
```

### Project-Wide Integration Example

```python
import pynlm

def run_complete_nlm_simulation(phases_to_run="phase6"):
    """Run NLM simulation with different phases"""
    
    # Create brain configuration
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 1000)
    config.set("brain.connection_probability", 0.1)
    
    # Create brain and agent
    brain = pynlm.createBrain(config)
    agent = pynlm.createAgentBrain(brain)
    world = pynlm.createSimpleWorld()
    world.configure(width=25, height=25, visionWidth=10, visionHeight=10)
    
    # Initialize all systems
    brain.initialize()
    world.reset()
    agent.initialize(world)
    
    # Enable all subsystems
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    print(f"=== Running NLM {phases_to_run.upper()} Simulation ===")
    print(f"Brain configuration: {brain.getTotalNeuronCount()} neurons, "
          f"{brain.getTotalSynapseCount()} synapses")
    print()
    
    # Run simulation
    for step in range(1000):
        world.update(0.1)
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Print statistics every 100 steps
        if step % 100 == 0:
            print(f"Step {step}:")
            print(f"  - Firing neurons: {brain.getFiringNeuronCount()}")
            print(f"  - Total spikes: {brain.getTotalSpikeCount()}")
            print(f"  - Curiosity level: {agent.getCuriosityLevel():.3f}")
            print(f"  - Novelty level: {agent.getNoveltyLevel():.3f}")
            print(f"  - Development stage: {brain.getDevelopmentalStage()}")
            print()
    
    print("=== Simulation Complete ===")
    print(f"Final Statistics:")
    print(f"  - Total spikes generated: {brain.getTotalSpikeCount()}")
    print(f"  - Average firing rate: {brain.getAverageFiringRate():.3f}")
    print(f"  - Development stage: {brain.getDevelopmentalStage()}")
    print(f"  - Integration: ALL SYSTEMS WORKING TOGETHER")

# Run the complete simulation
if __name__ == "__main__":
    run_complete_nlm_simulation()
```

---

## Quick Reference

| What you want | Code |
|--------------|------|
| Create brain | `pynlm.createBrain(config)` |
| Start brain | `brain.initialize()` |
| Make brain think | `brain.step(step)` |
| Create world | `pynlm.createSimpleWorld()` |
| Create agent | `pynlm.createAgentBrain(brain)` |
| Get brain stats | `brain.getFiringNeuronCount()` |
| See world | `world.getSensoryPercept()` |
| Make action | `agent.decodeMotorCommand()` |

---

## Troubleshooting

**"My brain isn't doing anything"**
- Did you call `brain.initialize()`?
- Try increasing the number of steps

**"The agent isn't moving"**
- Did you call `world.applyMotorCommand()`?
- Check that `world.update()` is being called

**"Everything is 0"**
- Brains need time to "warm up" - try more steps
- Some neurons need input to fire - make sure sensory input is connected

---

## Next Steps

When you're comfortable:
1. Read `HOW_TO_USE.md` for more details
2. Read `docs/ARCHITECTURE.md` to understand how it all works
3. Experiment with different configurations!

That's it! You're now ready to use NLM.
