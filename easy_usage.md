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

### Example 3: Learning Agent with Rewards

```python
import pynlm

# Setup
config = pynlm.createDefaultConfig()
# Configure for learning
config.set("neuromod.dopamine.scale", 1.0)
config.set("neuromod.curiosity.enable", True)
config.set("neuromod.novelty.enable", True)

brain = pynlm.createBrain(config)
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=16, visionHeight=16)
world.reset()
world.setAgentStart(10.0, 10.0)

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable learning subsystems
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableDevelopment(True)

# Run agent with learning
for episode in range(3):
    print(f"\n=== Episode {episode + 1} ===")
    for step in range(200):
        world.update(0.1)
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(step)
        
        action = agent.decodeMotorCommand()
        result = world.applyMotorCommand(action, world.getSimulationTime())
        
        # Apply reward signal for learning
        agent.applyRewardModulation(result.reward, 0.0)
        
        # Update development
        agent.updateDevelopment(0.1)
        
        if step % 50 == 0:
            print(f"Step {step}: Action={action}, Reward={result.reward:.2f}, "
                  f"Curiosity={agent.getCuriosityLevel():.3f}, "
                  f"Novelty={agent.getNoveltyLevel():.3f}")

print("Learning agent simulation complete!")
```

### Example 4: Saving and Loading Brain States

```python
import pynlm
import os

# Create and run a brain
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

for i in range(500):
    brain.step(i)

# Save brain state
brain.save("my_brain_checkpoint.bin")
print("Brain saved!")

# Create a new brain for loading
brain2 = pynlm.createBrain(pynlm.createDefaultConfig())

# Load saved state
if brain2.load("my_brain_checkpoint.bin"):
    print("Brain loaded successfully!")
    print("Loaded spike count:", brain2.getTotalSpikeCount())
else:
    print("Failed to load brain state")

# Clean up
if os.path.exists("my_brain_checkpoint.bin"):
    os.remove("my_brain_checkpoint.bin")
    print("Checkpoint file cleaned up")
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
def print_brain_stats(brain, step):
    # Just add these inside your loop:
    print(f"Step {step}:")
    print(f"  Neurons firing: {brain.getFiringNeuronCount()}")
    print(f"  Total spikes: {brain.getTotalSpikeCount()}")
    print(f"  Avg firing rate: {brain.getAverageFiringRate():.3f}")
    print(f"  Development stage: {brain.getDevelopmentalStage()}")
    print(f"  E/I ratio: {brain.getExcitationInhibitionRatio():.3f}")
```

### Pattern 3: Enable Brain Features

```python
def setup_advanced_brain(brain, world):
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # These are all optional features you can enable:
    
    # 1. Reward-based learning
    agent.enableRewardModulation(True)
    # Benefits: Learn from rewards, improve behavior over time
    
    # 2. Structural plasticity (grow new connections)
    agent.enableStructuralPlasticity(True)
    # Benefits: Adapt neural network structure, optimize wiring
    
    # 3. Development (brain matures over time)
    agent.enableDevelopment(True)
    # Benefits: Change plasticity rates, follow developmental stages
    
    # 4. Curiosity-driven exploration
    agent.enableCuriosity(True)
    # Benefits: Explore novel situations, balance exploration vs exploitation
    
    # 5. Prediction learning
    # Note: Requires access to prediction system via brain.getPredictionSystem()
    
    # 6. Working memory
    # Note: Requires access to memory systems via brain.getWorkingMemory()
    
    return agent
```

### Pattern 4: Multi-Agent Environment

```python
def create_agent_network(num_agents=3):
    agents = []
    brains = []
    
    for i in range(num_agents):
        # Create separate brain for each agent
        brain = pynlm.createBrain(pynlm.createDefaultConfig())
        brain.initialize()
        brains.append(brain)
        
        # Each agent has its own interface
        agent = pynlm.createAgentBrain(brain)
        agents.append(agent)
    
    return brains, agents

def run_multi_agent_simulation(brains, agents, world, steps_per_agent):
    # Share world among agents
    for step in range(steps_per_agent):
        world.update(0.1)
        
        # Each agent perceives and acts independently
        for i, (brain, agent) in enumerate(zip(brains, agents)):
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            brain.step(step)
            
            action = agent.decodeMotorCommand()
            result = world.applyMotorCommand(action, world.getSimulationTime())
            
            # Agents can learn from shared world events
            agent.applyRewardModulation(result.reward, 0.0)
    
    return world
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

### `world.applyMotorCommand(action, time)`
Executes an action in the world and returns the result.

### `agent.applyRewardModulation(reward, predicted)`
Applies reward-based learning to the brain.

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
print(f"Created {brain.getTotalNeuronCount()} neurons")
print(f"Generated {brain.getTotalSpikeCount()} spikes")
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
print(f"Firing rate: {brain.getAverageFiringRate():.3f}")
```

### Project 3: Complete Learning Agent

```python
import pynlm

# Setup
config = pynlm.createDefaultConfig()
# Optimize for fast learning
config.set("brain.neuron_count", 500)
config.set("plasticity.stdp.learning_rate", 0.01)
config.set("neuromod.dopamine.scale", 1.5)

brain = pynlm.createBrain(config)
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=15, height=15, visionWidth=8, visionHeight=8)
world.reset()
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable all learning subsystems
agent.enableRewardModulation(True)
agent.enableStructuralPlasticity(True)
agent.enableDevelopment(True)
agent.enableCuriosity(True)

# Run agent with detailed monitoring
metrics = {
    'steps': 0,
    'total_reward': 0.0,
    'max_curiosity': 0.0,
    'actions_taken': 0
}

for step in range(300):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    action = agent.decodeMotorCommand()
    result = world.applyMotorCommand(action, world.getSimulationTime())
    
    # Collect metrics
    metrics['steps'] += 1
    metrics['total_reward'] += result.reward
    metrics['max_curiosity'] = max(metrics['max_curiosity'], agent.getCuriosityLevel())
    if action != pynlm.MotorCommand.Wait:
        metrics['actions_taken'] += 1
    
    agent.applyRewardModulation(result.reward, 0.0)
    agent.updateDevelopment(0.1)
    
    if step % 50 == 0:
        print(f"Step {step}: Action={action}, Reward={result.reward:.2f}, "
              f"Curiosity={agent.getCuriosityLevel():.3f}")

print(f"\n=== Learning Summary ===")
print(f"Total reward: {metrics['total_reward']:.2f}")
print(f"Max curiosity: {metrics['max_curiosity']:.3f}")
print(f"Actions taken: {metrics['actions_taken']}")
print(f"Final firing rate: {brain.getAverageFiringRate():.3f}")
print(f"Final developmental stage: {brain.getDevelopmentalStage()}")
print("Agent learning complete!")
```

### Project 4: Brain Evolution

```python
import pynlm
import random

def create_random_brain():
    config = pynlm.createDefaultConfig()
    
    # Random configuration for evolution
    config.set("brain.neuron_count", random.randint(300, 800))
    config.set("brain.synapse_density", random.uniform(0.05, 0.2))
    config.set("plasticity.stdp.learning_rate", random.uniform(0.001, 0.02))
    config.set("neuromod.dopamine.scale", random.uniform(0.5, 2.0))
    config.set("neuromod.curiosity.enable", random.choice([True, False]))
    
    return config

def evaluate_brain(brain, world, agent, steps=100):
    total_reward = 0.0
    max_curiosity = 0.0
    
    for step in range(steps):
        world.update(0.1)
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(step)
        action = agent.decodeMotorCommand()
        result = world.applyMotorCommand(action, world.getSimulationTime())
        
        total_reward += result.reward
        max_curiosity = max(max_curiosity, agent.getCuriosityLevel())
        
        agent.applyRewardModulation(result.reward, 0.0)
        agent.updateDevelopment(0.1)
    
    # Fitness score: reward efficiency
    fitness = total_reward / steps
    curiosity_bonus = max_curiosity * 10.0
    
    return fitness + curiosity_bonus

def run_evolution(generations=10, population_size=5):
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    
    best_brain = None
    best_fitness = 0.0
    
    for gen in range(generations):
        print(f"\n=== Generation {gen + 1} ===")
        
        # Create population
        brains = []
        agents = []
        fitnesses = []
        
        for i in range(population_size):
            config = create_random_brain()
            brain = pynlm.createBrain(config)
            brain.initialize()
            
            agent = pynlm.createAgentBrain(brain)
            agent.initialize(world)
            agent.enableRewardModulation(True)
            agent.enableCuriosity(True)
            
            # Reset world for fair evaluation
            world.reset()
            
            # Evaluate fitness
            fitness = evaluate_brain(brain, world, agent)
            
            brains.append(brain)
            agents.append(agent)
            fitnesses.append(fitness)
            
            if fitness > best_fitness:
                best_fitness = fitness
                best_brain = brain
                print(f"New best fitness: {fitness:.3f}")
        
        # Simple selection for next generation
        print(f"Generation {gen + 1} average fitness: {sum(fitnesses)/len(fitnesses):.3f}")
    
    print(f"\n=== Evolution Complete ===")
    print(f"Best fitness: {best_fitness:.3f}")
    if best_brain:
        print(f"Best brain neurons: {best_brain.getTotalNeuronCount()}")
        print(f"Best brain spikes: {best_brain.getTotalSpikeCount()}")
    
    return best_brain, best_fitness

# Run evolution
best_brain, score = run_evolution(generations=5, population_size=3)
if best_brain:
    print(f"\nEvolution finished with best score: {score:.3f}")
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
| Apply reward | `agent.applyRewardModulation(reward, predicted)` |

---

## Troubleshooting

### "My brain isn't doing anything"
- Did you call `brain.initialize()`?
- Try increasing the number of steps
- Check that `agent.enableRewardModulation(True)` is set for learning

### "The agent isn't moving"
- Did you call `world.applyMotorCommand()`?
- Check that `world.update()` is being called
- Make sure `agent.decodeMotorCommand()` returns a valid action

### "Everything is 0"
- Brains need time to "warm up" - try more steps
- Some neurons need input to fire - make sure sensory input is connected
- Try enabling curiosity and reward modulation for more activity

### "ImportError: No module named 'pynlm'"
```bash
pip install --force-reinstall .
```

### "Segmentation faults"
- Check that `initialize()` is called before `step()`
- Ensure `reset()` is called before re-running simulation
- Make sure all system pointers are properly initialized

---

## Next Steps

When you're comfortable:

1. **Read `HOW_TO_USE.md`** for more technical details and advanced usage
2. **Read `docs/ARCHITECTURE.md`** to understand how it all works internally
3. **Experiment with different configurations** - try different neuron counts, learning rates, etc.
4. **Try the demo executables** in the build directory (e.g., `nlm_phase6_demo`)
5. **Look at the test suite** to understand all available features and API methods

That's it! You're now ready to use NLM. Start with the simplest examples and gradually build up to more complex learning agents and simulations.

### Need Help?

Check out:
- `HOW_TO_USE.md` - Detailed technical documentation
- `docs/ARCHITECTURE.md` - System architecture and design
- `docs/SCIENCE.md` - Scientific background and principles
- The Python bindings documentation in `python/bindings.cpp`
- The test files in the `tests/` directory for examples

Happy coding with NLM! 🤖✨
