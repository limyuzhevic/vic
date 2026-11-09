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

## Advanced Usage Examples

### Example 3: Complete Learning Agent

```python
import pynlm
import time

class LearningAgent:
    def __init__(self, width=20, height=20):
        # Create brain with custom configuration
        config = pynlm.createDefaultConfig()
        config.set("brain.neuron_count", 1000)  # More neurons for learning
        
        self.brain = pynlm.createBrain(config)
        self.brain.initialize()
        
        self.world = pynlm.createSimpleWorld()
        self.world.configure(width, height, 16, 16)
        self.world.reset()
        
        self.agent = pynlm.createAgentBrain(self.brain)
        self.agent.initialize(self.world)
        
        # Enable advanced learning features
        self.agent.enableRewardModulation(True)
        self.agent.enableStructuralPlasticity(True)
        self.agent.enableDevelopment(True)
        self.agent.enableCuriosity(True)
        
    def run_episode(self, steps=500, random_seed=42):
        """Run a complete learning episode"""
        self.world.setRandomSeed(random_seed)
        
        episode_data = {
            'steps': [],
            'rewards': [],
            'curiosity': [],
            'firing_rates': [],
            'novelty': []
        }
        
        for step in range(steps):
            # Update world and get sensory input
            self.world.update(0.1)
            percept = self.world.getSensoryPercept()
            
            # Process sensory input
            self.agent.processSensoryInput(percept)
            
            # Brain processes and produces action
            self.brain.step(step)
            action = self.agent.decodeMotorCommand()
            
            # Apply action in world
            self.world.applyMotorCommand(action, self.world.getSimulationTime())
            
            # Get reward and apply learning
            reward = self._get_reward(percept)
            predicted_reward = self.agent.getNeuromodulationLevel()
            
            self.agent.applyRewardModulation(reward, predicted_reward)
            self.agent.updateDevelopment(0.1)
            
            # Collect metrics
            episode_data['steps'].append(step)
            episode_data['rewards'].append(reward)
            episode_data['curiosity'].append(self.agent.getCuriosityLevel())
            episode_data['firing_rates'].append(self.brain.getAverageFiringRate())
            episode_data['novelty'].append(self.agent.getNoveltyLevel())
            
            # Log progress every 50 steps
            if step % 50 == 0:
                print(f"Step {step}: "
                      f"Reward: {reward:.3f}, "
                      f"Curiosity: {self.agent.getCuriosityLevel():.3f}, "
                      f"Firing: {self.brain.getFiringNeuronCount()}, "
                      f"Dev Stage: {self.brain.getDevelopmentalStage()}")
        
        return episode_data
    
    def _get_reward(self, percept):
        """Calculate reward based on sensory input"""
        # Simple reward based on internal signals (energy, health, etc.)
        internal = percept.getInternal()
        if not internal.getData().empty():
            # Energy is typically in first slot
            energy = internal.getData()[0]
            return (energy / 100.0) - 0.5  # Normalize to range [-0.5, 0.5]
        return 0.0

# Usage
if __name__ == "__main__":
    print("=== NLM Learning Agent Example ===")
    
    agent = LearningAgent(width=30, height=30)
    
    print("\nRunning learning episode...")
    start_time = time.time()
    
    episode = agent.run_episode(steps=1000, random_seed=123)
    
    end_time = time.time()
    
    # Analyze results
    total_reward = sum(episode['rewards'])
    avg_reward = total_reward / len(episode['rewards'])
    max_curiosity = max(episode['curiosity'])
    final_firing_rate = episode['firing_rates'][-1]
    
    print(f"\n=== Episode Results ===")
    print(f"Total reward: {total_reward:.3f}")
    print(f"Average reward per step: {avg_reward:.3f}")
    print(f"Maximum curiosity level: {max_curiosity:.3f}")
    print(f"Final firing rate: {final_firing_rate:.3f}")
    print(f"Episode duration: {end_time - start_time:.2f} seconds")
    
    # Check for learning indicators
    if len(episode['rewards']) > 10:
        first_half = sum(episode['rewards'][:len(episode['rewards'])//2])
        second_half = sum(episode['rewards'][len(episode['rewards'])//2:])
        if second_half > first_half * 1.1:  # 10% improvement
            print("✓ Learning detected: Performance improved over time!")
        else:
            print("⚠ Limited learning observed")
    
    print("\n=== Agent Complete ===")
```

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
agent.enableCuriosity(True)             # Explore new things
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

### Project 3: Complete Agent

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
