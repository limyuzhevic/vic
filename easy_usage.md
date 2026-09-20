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

---

## Simple Analogy

| Real World | NLM Code |
|------------|----------|
| You see something | `percept = world.getSensoryPercept()` |
| Your brain processes it | `agent.processSensoryInput(percept)` |
| You decide to act | `action = agent.decodeMotorCommand()` |
| You do the action | `world.applyMotorCommand(action, time)` |
| Your brain learns from results | `agent.applyRewardModulation(reward, predicted)` |

---

## The Simplest Possible Example

```python
import pynlm

# One line to create a virtual brain
brain = pynlm.createBrain(pynlm.createDefaultConfig())

# Initialize it
brain.initialize()

# Make it active
brain.step(0)

print("Your brain has", brain.getTotalNeuronCount(), "neurons!")
```

---

## Common Patterns

### Pattern 1: Run a Complete Agent Simulation

```python
def run_complete_simulation(num_steps=1000):
    """Run a complete agent simulation with learning."""
    import pynlm
    
    # 1. Create configuration
    config = pynlm.createDefaultConfig()
    
    # 2. Create brain and initialize
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # 3. Create agent brain interface
    agent = pynlm.createAgentBrain(brain)
    
    # 4. Create and configure world
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    
    # 5. Initialize agent with world
    agent.initialize(world)
    
    # 6. Enable learning subsystems
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    agent.enableDevelopment(True)
    
    # 7. Run simulation loop
    for step in range(num_steps):
        # Update world
        world.update(0.1)
        
        # Get sensory input
        percept = world.getSensoryPercept()
        
        # Process sensory input
        agent.processSensoryInput(percept)
        
        # Run brain step
        brain.step(step)
        
        # Get motor command
        action = agent.decodeMotorCommand()
        
        # Apply motor command
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Get reward (for simple worlds, you can check vision for resources)
        reward = 0.0
        if world.getSensoryPercept().getVision():  # Check if agent sees something valuable
            reward = 1.0  # Simple reward for seeing something
        
        # Apply reward modulation
        agent.applyRewardModulation(reward, 0.0)
        
        # Update development
        agent.updateDevelopment(0.1)
        
        # Log progress every 100 steps
        if step % 100 == 0:
            print(f"Step {step}:")
            print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
            print(f"  Curiosity: {agent.getCuriosityLevel():.3f}")
            print(f"  Novelty: {agent.getNoveltyLevel():.3f}")
            print(f"  Dev Stage: {brain.getDevelopmentalStage()}")
    
    print("Agent simulation complete!")
    return brain, agent, world

# Run the simulation
run_complete_simulation(1000)
```

### Pattern 2: Get Comprehensive Brain Stats

```python
def log_brain_statistics(brain, agent, step):
    """Log detailed statistics about brain state."""
    print(f"=== Step {step} Statistics ===")
    print(f"Neurons firing: {brain.getFiringNeuronCount()}")
    print(f"Total spikes: {brain.getTotalSpikeCount()}")
    print(f"Average firing rate: {brain.getAverageFiringRate():.2f} Hz")
    print(f"Total neurons: {brain.getTotalNeuronCount()}")
    print(f"Total synapses: {brain.getTotalSynapseCount()}")
    print(f"E/I ratio: {brain.getExcitationInhibitionRatio():.3f}")
    
    if agent:
        print(f"Curiosity level: {agent.getCuriosityLevel():.3f}")
        print(f"Novelty level: {agent.getNoveltyLevel():.3f}")
        print(f"Prediction error: {agent.getPredictionError():.3f}")
        print(f"Development stage: {agent.getDevelopmentalStage()}")
        print(f"Neuromodulation level: {agent.getNeuromodulationLevel():.3f}")
    
    print()
```

### Pattern 3: Save and Load Brain States

```python
def save_brain_checkpoint(brain, agent, world, step):
    """Save brain state for resuming later."""
    import os
    
    # Create checkpoints directory
    os.makedirs("checkpoints", exist_ok=True)
    
    # Save brain state
    brain.save(f"checkpoints/brain_step_{step}.bin")
    
    # Save additional state
    with open(f"checkpoints/state_step_{step}.txt", "w") as f:
        f.write(f"Step: {step}\n")
        f.write(f"Firing neurons: {brain.getFiringNeuronCount()}\n")
        f.write(f"Total spikes: {brain.getTotalSpikeCount()}\n")
        f.write(f"Curiosity: {agent.getCuriosityLevel() if agent else 0.0}\n")
        f.write(f"Novelty: {agent.getNoveltyLevel() if agent else 0.0}\n")
        f.write(f"Dev stage: {brain.getDevelopmentalStage()}\n")
    
    print(f"Saved checkpoint at step {step}")

def load_brain_checkpoint(brain, agent, world, filepath):
    """Load saved brain state."""
    # Note: This depends on your specific save format
    # This is a conceptual example
    
    # Reload brain state
    brain.load(filepath)
    
    print(f"Loaded brain state from {filepath}")
```

---

## What Each Part Does

### `pynlm.createBrain(config)`
Creates a virtual brain with neurons and synapses.
- `config` = settings for the brain (use `createDefaultConfig()` for simple setup)

### `brain.initialize()`
Starts up the brain. Always call this before using the brain!

### `brain.step(step_number)`
Makes the brain process one moment in time. The brain:
- Checks each neuron
- Sends signals between connected neurons
- Updates connections based on learning rules

### `world.update(time)`
Updates the virtual world by `time` seconds.

### `agent.processSensoryInput(percept)`
Gives sensory information (vision, touch, etc.) to the brain.

### `agent.decodeMotorCommand()`
Reads the brain's motor activity to decide what action to take.

---

## Mini Projects

### Project 1: Silent Brain (Neural Activity Monitoring)

```python
import pynlm

# Create and initialize
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Watch neural activity
for i in range(100):
    brain.step(i)
    firing = brain.getFiringNeuronCount()
    if firing > 0:
        print(f"Step {i}: {firing} neurons firing")

print("Neural activity monitoring complete!")
```

### Project 2: Agent Navigation with Learning

```python
import pynlm

def create_navigating_agent(width=30, height=30):
    """Create an agent that learns to navigate a 2D world."""
    # Setup
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(width=width, height=height, visionWidth=8, visionHeight=8)
    world.reset()
    
    # Place agent in center
    world.setAgentStart(width/2, height/2)
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable learning
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    
    # Track performance
    total_steps = 0
    successful_actions = 0
    
    # Run agent
    for step in range(500):
        world.update(0.1)
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        total_steps += 1
        
        # Simple reward for moving
        if action != pynlm.MotorCommand.Wait:
            successful_actions += 1
    
    efficiency = (successful_actions / total_steps) * 100 if total_steps > 0 else 0
    print(f"Navigation efficiency: {efficiency:.1f}%")
    print(f"Final curiosity: {agent.getCuriosityLevel():.3f}")
    print(f"Final novelty: {agent.getNoveltyLevel():.3f}")
    
    return brain, agent, world, efficiency

# Test navigation
create_navigating_agent()
```

### Project 3: Comparative Learning Study

```python
import pynlm

def run_learning_experiment(brain_config, num_trials=10):
    """Run multiple trials to compare learning performance."""
    results = []
    
    for trial in range(num_trials):
        # Create fresh brain for each trial
        config = pynlm.createDefaultConfig()
        if brain_config:
            config = brain_config
        
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        world = pynlm.createSimpleWorld()
        world.configure(width=15, height=15, visionWidth=6, visionHeight=6)
        world.reset()
        
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        # Enable all learning systems
        agent.enableRewardModulation(True)
        agent.enableCuriosity(True)
        agent.enableDevelopment(True)
        
        # Run trial
        for step in range(200):
            world.update(0.1)
            agent.processSensoryInput(world.getSensoryPercept())
            brain.step(step)
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
        
        # Record results
        trial_result = {
            'trial': trial,
            'final_firing_rate': brain.getAverageFiringRate(),
            'total_spikes': brain.getTotalSpikeCount(),
            'final_curiosity': agent.getCuriosityLevel(),
            'final_novelty': agent.getNoveltyLevel(),
            'development_stage': brain.getDevelopmentalStage(),
            'efficiency': brain.getFiringNeuronCount() / 1000  # Simple metric
        }
        
        results.append(trial_result)
    
    # Calculate averages
    avg_firing_rate = sum(r['final_firing_rate'] for r in results) / len(results)
    avg_curiosity = sum(r['final_curiosity'] for r in results) / len(results)
    
    print(f"Experiment completed with {len(results)} trials")
    print(f"Average final firing rate: {avg_firing_rate:.2f} Hz")
    print(f"Average final curiosity: {avg_curiosity:.3f}")
    
    return results

# Run experiment
run_learning_experiment()
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
| Enable learning | `agent.enableRewardModulation(True)` |
| Enable development | `agent.enableDevelopment(True)` |
| Enable curiosity | `agent.enableCuriosity(True)` |

---

## Troubleshooting

**"My brain isn't doing anything"**
- Did you call `brain.initialize()`?
- Try increasing the number of steps
- Check if you have enough neurons configured

**"The agent isn't moving"**
- Did you call `world.applyMotorCommand()`?
- Check that `world.update()` is being called
- Verify the agent can see something (vision input)

**"Everything is 0"**
- Brains need time to "warm up" - try more steps
- Some neurons need input to fire - make sure sensory input is connected
- Check that the world is being updated correctly

**"Learning isn't working"**
- Check that `enableRewardModulation(True)` is called
- Verify the agent is getting sensory input
- Make sure `applyRewardModulation()` is being called

**"Memory issues"**
- If you're running many simulations, consider clearing unused variables
- Use `brain.reset()` between experiments
- Monitor memory usage if running long simulations

---

## Next Steps

When you're comfortable:

1. **Read `HOW_TO_USE.md`** for more technical details
2. **Read `docs/ARCHITECTURE.md`** to understand how it all works
3. **Create custom configurations** to experiment with different brain parameters
4. **Build your own experiments** using the experiment framework
5. **Dive into the source code** to understand the neural mechanisms

## Performance Tips

- For long simulations, use `brain.save()` periodically to save checkpoints
- Clear large data structures when no longer needed
- Monitor firing rates to avoid runaway excitation
- Use `agent.enableCuriosity(False)` to reduce exploration during stable phases

---

That's it! You're now ready to use NLM. The documentation has been enhanced with comprehensive examples and best practices to help you get started and progress to more advanced usage.

(End of file - total 579 lines)