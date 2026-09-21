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
| Your brain learns from results | `agent.applyRewardModulation(reward, predicted_reward)` |

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

### Pattern 1: Run a Simulation

```python
def run_simulation(brain, world, agent, num_steps):
    for step in range(num_steps):
        # 1. Update world
        world.update(0.1)
        
        # 2. Get what the agent sees
        percept = world.getSensoryPercept()
        
        # 3. Tell the brain
        agent.processSensoryInput(percept)
        
        # 4. Brain thinks
        brain.step(step)
        
        # 5. Get action from brain
        action = agent.decodeMotorCommand()
        
        # 6. Do action in world
        world.applyMotorCommand(action, world.getSimulationTime())

# Use it like this:
run_simulation(brain, world, agent, 1000)
```

### Pattern 2: Get Brain Stats

```python
# Just add these inside your loop:
print("Neurons firing:", brain.getFiringNeuronCount())
print("Total spikes:", brain.getTotalSpikeCount())
print("Avg firing rate:", brain.getAverageFiringRate())
print("Development stage:", brain.getDevelopmentalStage())
```

### Pattern 3: Enable Brain Features

```python
# These are all optional:
agent.enableRewardModulation(True)      # Learn from rewards
agent.enableStructuralPlasticity(True)  # Grow new connections
agent.enableDevelopment(True)           # Brain matures over time
agent.enableCuriosity(True)              # Explore new things
```

### Pattern 4: Advanced Agent with Learning and Development

```python
def run_advanced_agent(brain, world, agent, num_steps):
    """Run a complete agent simulation with advanced features."""
    
    # Enable all advanced subsystems
    agent.enableRewardModulation(True)     # Learn from rewards
    agent.enableCuriosity(True)            # Explore new things
    agent.enableDevelopment(True)          # Brain matures over time
    agent.enableStructuralPlasticity(True)  # Grow new connections
    
    for step in range(num_steps):
        # 1. Update world
        world.update(0.1)
        
        # 2. Get what the agent sees
        percept = world.getSensoryPercept()
        
        # 3. Tell the brain
        agent.processSensoryInput(percept)
        
        # 4. Brain thinks
        brain.step(step)
        
        # 5. Get action from brain
        action = agent.decodeMotorCommand()
        
        # 6. Do action in world
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # 7. Apply learning with reward
        # Get reward from internal signals (simulated reward)
        reward = percept.getInternal()[0] if percept.getInternal() else 0.0
        # Predict reward (simple - could be improved)
        predicted_reward = agent.getPredictionError()
        agent.applyRewardModulation(reward, predicted_reward)
        
        # 8. Update development - brain matures
        agent.updateDevelopment(0.1)
        
        # 9. Log advanced statistics
        if step % 100 == 0:
            print(f"Step {step}:")
            print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
            print(f"  Total spikes: {brain.getTotalSpikeCount()}")
            print(f"  Average firing rate: {brain.getAverageFiringRate():.3f}")
            print(f"  Development stage: {brain.getDevelopmentalStage()}")
            print(f"  Curiosity level: {agent.getCuriosityLevel():.3f}")
            print(f"  Novelty level: {agent.getNoveltyLevel():.3f}")
            print(f"  Prediction error: {agent.getPredictionError():.3f}")
            print(f"  Neuromodulation level: {agent.getNeuromodulationLevel():.3f}")
    
    return brain, world, agent

# Use it like this:
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()
world = pynlm.createSimpleWorld()
world.configure(width=15, height=15, visionWidth=8, visionHeight=8)
world.reset()
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

run_advanced_agent(brain, world, agent, 500)
```

### Pattern 5: Complete Multi-Episode Training

```python
def run_training_session(brain, world, agent, num_episodes, steps_per_episode):
    """Run complete training over multiple episodes."""
    
    # Enable learning systems
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    agent.enableDevelopment(True)
    
    print(f"Starting training for {num_episodes} episodes...")
    
    for episode in range(num_episodes):
        print(f"\nEpisode {episode + 1}/{num_episodes}")
        
        # Reset for new episode
        world.reset()
        agent.reset()
        
        # Run episode
        for step in range(steps_per_episode):
            # Standard loop
            world.update(0.1)
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            brain.step(step)
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            
            # Learning
            reward = percept.getInternal()[0] if percept.getInternal() else 0.0
            predicted_reward = agent.getPredictionError()
            agent.applyRewardModulation(reward, predicted_reward)
            
            # Development
            agent.updateDevelopment(0.1)
            
            # Progress indicator
            if step % 100 == 0:
                print(f"  Step {step}: "
                      f"{brain.getFiringNeuronCount()} firing, "
                      f"{agent.getCuriosityLevel():.3f} curiosity")
        
        # End of episode summary
        print(f"  Episode complete - "
              f"Total spikes: {brain.getTotalSpikeCount()}, "
              f"Development: {brain.getDevelopmentalStage()}")
    
    print(f"\nTraining complete!")
    print(f"Final development stage: {brain.getDevelopmentalStage()}")
    print(f"Total spikes accumulated: {brain.getTotalSpikeCount()}")

# Use it like this:
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

run_training_session(brain, world, agent, 10, 200)
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
Reads the brain's motor neurons to decide what action to take.

### `agent.applyRewardModulation(reward, predicted_reward)`
Teaches the brain from experiences using reward prediction error.

### `agent.updateDevelopment(timestep)`
Allows the brain to mature and change over time.

### `agent.enableFeature(True)`
Turns on specific brain subsystems (curiosity, development, etc.).

---

## Mini Projects

### Project 1: Silent Brain (Just Neurons)

```python
import pynlm

brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

for i in range(10):
    brain.step(i)

print("Silent brain test done!")
```

### Project 2: Brain Watching a World

```python
import pynlm

# Setup
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Watch the world for 30 steps
for i in range(30):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(i)
    
print("Watched world for 30 steps")
print("Firing rate:", brain.getAverageFiringRate())
```

### Project 3: Complete Agent with Learning

```python
import pynlm

# Setup
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()
world = pynlm.createSimpleWorld()
world.configure(width=15, height=15, visionWidth=8, visionHeight=8)
world.reset()
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable learning
agent.enableRewardModulation(True)
agent.enableCuriosity(True)

# Run agent
for step in range(100):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    if step % 20 == 0:
        print(f"Step {step}: {brain.getFiringNeuronCount()} neurons firing")

print("Agent simulation complete!")
```

### Project 4: Advanced Training with Development

```python
import pynlm

# Create advanced brain
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Create agent
world = pynlm.createSimpleWorld()
world.configure(width=25, height=25, visionWidth=12, visionHeight=12)
world.reset()
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable all subsystems for advanced learning
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableDevelopment(True)
agent.enableStructuralPlasticity(True)

print("Starting advanced training with development...")

# Run training
for episode in range(5):
    print(f"\nEpisode {episode + 1}/5")
    
    # Reset episode
    world.reset()
    agent.reset()
    
    # Run episode with development updates
    for step in range(200):
        world.update(0.1)
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Learn from experience
        reward = percept.getInternal()[0] if percept.getInternal() else 0.0
        predicted_reward = agent.getPredictionError()
        agent.applyRewardModulation(reward, predicted_reward)
        
        # Update development
        agent.updateDevelopment(0.1)
        
        if step % 50 == 0:
            print(f"  Step {step}: "
                  f"{brain.getDevelopmentalStage()}, "
                  f"{agent.getCuriosityLevel():.3f} curiosity, "
                  f"{brain.getFiringNeuronCount()} firing")

print("\nAdvanced training complete!")
print(f"Final development stage: {brain.getDevelopmentalStage()}")
print(f"Total spikes: {brain.getTotalSpikeCount()}")
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
| Learn from reward | `agent.applyRewardModulation(reward, predicted_reward)` |
| Update development | `agent.updateDevelopment(timestep)` |

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

**"The brain isn't learning"**
- Make sure `agent.enableRewardModulation(True)` is called
- Check that `agent.applyRewardModulation()` is being called
- Try increasing curiosity: `agent.enableCuriosity(True)`

**"The brain isn't developing"**
- Make sure `agent.enableDevelopment(True)` is called
- Check that `agent.updateDevelopment()` is being called
- Development takes time and multiple episodes

---

## Advanced Usage Tips

### For Faster Results
```python
# Use a larger brain with more neurons
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 2000)  # More neurons = faster learning
brain = pynlm.createBrain(config)
```

### For More Learning
```python
# Enable all learning systems
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableDevelopment(True)
agent.enableStructuralPlasticity(True)

# These enable:
# - Reward learning from experiences
# - Curiosity-driven exploration
# - Brain maturation over time
# - Growing new connections
```

### For Realistic Simulation
```python
# Set random seed for reproducible results
world.setRandomSeed(42)

# Configure world for specific task
world.configure(width=30, height=30, visionWidth=12, visionHeight=12)
```

### For Monitoring Progress
```python
# Log detailed statistics every 100 steps
if step % 100 == 0:
    print(f"Step {step}:")
    print(f"  Neurons firing: {brain.getFiringNeuronCount()}")
    print(f"  Total spikes: {brain.getTotalSpikeCount()}")
    print(f"  Curiosity: {agent.getCuriosityLevel():.3f}")
    print(f"  Development: {brain.getDevelopmentalStage()}")
```

---

## Next Steps

When you're comfortable with the basics:

1. **Read HOW_TO_USE.md** - Get comprehensive documentation with all parameters
2. **Read docs/ARCHITECTURE.md** - Understand the advanced architecture
3. **Experiment with configurations** - Try different neuron counts, learning rates
4. **Create custom worlds** - Design specific environments for your experiments
5. **Build multi-agent systems** - Run teams of interacting brains
6. **Analyze performance** - Use the advanced logging and monitoring features

### Advanced Topics to Explore:

- **Memory Systems**: How working and episodic memory work
- **Prediction Systems**: How brains predict future events
- **Attention Mechanisms**: How brains focus on important information
- **Development**: How brains change and mature over time
- **Neuromodulation**: How dopamine, curiosity, and other systems affect learning
- **Plasticity**: How synapses strengthen and weaken

That's it! You're now ready to use NLM at any level of complexity, from simple brain demonstrations to advanced learning systems with development and curiosity.

**Remember**: Always call `brain.initialize()` before using the brain, and `agent.initialize(world)` before using the agent with a world!
