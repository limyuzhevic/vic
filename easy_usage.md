# NLM - Easy Usage Guide for Beginners

## Quick Guide to Using NLM

NLM (Neural Learning Machine) is a brain simulator for computers. It lets you create virtual brains that can learn, remember things, and make decisions - just like how your brain works!

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

### "My brain isn't doing anything"

- **Did you call `brain.initialize()`?**
- Try increasing the number of steps

### "The agent isn't moving"

- **Did you call `world.applyMotorCommand()`?**
- Check that `world.update()` is being called

### "Everything is 0"

- **Brains need time to "warm up"** - try more steps
- **Some neurons need input to fire** - make sure sensory input is connected

---

## Next Steps

When you're comfortable:

1. **Read `HOW_TO_USE.md` for more details**
2. **Read `docs/ARCHITECTURE.md` to understand how it all works**
3. **Experiment with different configurations!**

That's it! You're now ready to use NLM.

---

## Advanced Usage

Once you've mastered the basics, try these advanced features:

### 1. Batch Operations

```python
import pynlm

# Create brain with many neurons
config = pynlm.createSimulationConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Create multiple regions at once
regions = brain.addRegions(["Visual", "Motor", "Decision"])

# Create multiple actions at once
actions = [
    pynlm.action_move_forward(1.0),
    pynlm.action_turn_left(45.0),
    pynlm.action_interact("object")
]
```

### 2. Performance Monitoring

```python
import pynlm

brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()
brain.start_profiling("test")

for step in range(1000):
    brain.step(step)
    if step % 100 == 0:
        print(f"Step {step}: {brain.getFiringNeuronCount()} firing")
```

### 3. Custom Learning

```python
import pynlm

# Create custom configuration
config = pynlm.createDefaultConfig()
config.set("plasticity.stdp.learning_rate", 0.01)  # Higher learning rate
config.set("neuromod.dopamine.scale", 1.5)           # Stronger dopamine

brain = pynlm.createBrain(config)
brain.initialize()
```

### 4. State Management

```python
import pynlm

brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Save progress
brain.save("my_brain.pkl")

# Run some simulation
for step in range(500):
    brain.step(step)

# Load saved brain
brain.load("my_brain.pkl")

# Continue simulation
for step in range(500):
    brain.step(step)
```

## Frequently Asked Questions

### Q: What does `brain.step()` actually do?
A: It makes the brain process one time step:
- Update all neuron membrane potentials
- Propagate spikes through synapses
- Apply plasticity rules (learning)
- Generate motor actions

### Q: What's the difference between `world.update()` and `brain.step()`?
A: `world.update()` updates the environment state (agent position, objects, etc.),
while `brain.step()` runs the brain's neural computation.

### Q: Do I need to create an agent?
A: No, you can work directly with brain and world, but the agent makes it
much easier to connect the two.

### Q: How many neurons should I start with?
A: For beginners: 100-1000 neurons.
For serious work: 1000-10000 neurons.
For research: 10000+ neurons.

### Q: What configuration should I use?
A: Start with `createDefaultConfig()` for basic usage,
`createSimulationConfig()` for research,
`createAgentConfig()` for agent-based applications.

### Q: Can I see what the brain is doing?
A: Yes! Use `brain.getFiringNeuronCount()`, `brain.getTotalSpikeCount()`,
and `brain.getAverageFiringRate()` to monitor brain activity.

### Q: How do I save my trained brain?
A: Use `brain.save("filename.pkl")` to save the brain state.
The saved brain can be reloaded later with `brain.load("filename.pkl")`.

### Q: What's the difference between `ActionType` and `MotorCommand`?
A: `ActionType` is for high-level actions (MoveForward, TurnLeft, etc.),
while `MotorCommand` is for low-level motor commands sent to the world.

---

## Learning Path

### Beginner (Week 1)
1. Run the "Simplest Brain" example
2. Try the "Brain in a Simple World" example
3. Experiment with the "Complete Agent" example
4. Understand the 3 Things you need to know

### Intermediate (Week 2)
1. Study the common patterns
2. Try the mini-projects
3. Learn about configuration options
4. Experiment with batch operations

### Advanced (Week 3+)
1. Read the HOW_TO_USE.md documentation
2. Try advanced usage examples
3. Build your own applications
4. Contribute to the project

---

## Resources

- **Main Documentation**: `HOW_TO_USE.md` - Complete technical reference
- **Architecture**: `docs/ARCHITECTURE.md` - System design and principles
- **Science**: `docs/SCIENCE.md` - Neuroscience background and theories
- **Roadmap**: `docs/ROADMAP.md` - Future development plans
- **Experiments**: `docs/EXPERIMENTS.md` - Research experiments and results
- **Phase 6 Audit**: `docs/PHASE6_FINAL_AUDIT.md` - Final integration audit

---

## Need Help?

1. **Start with the basics** - Follow the beginner guide
2. **Check documentation** - `HOW_TO_USE.md` has comprehensive examples
3. **Try examples first** - Copy and modify existing examples
4. **Ask questions** - Use the project's issue tracker
5. **Experiment** - The best way to learn is through hands-on practice

Enjoy building your neural learning machine! 🚄🧠✨