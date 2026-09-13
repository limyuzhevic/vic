# NLM - Easy Usage Guide for Beginners

## What is NLM?

NLM is a **computational brain simulator** written in C++. It implements a spiking neural network with realistic neuron dynamics and synaptic plasticity. The brain can:

- Learn through STDP and Hebbian learning rules
- Store information in working and episodic memory systems
- Plan actions using neural computation
- Adapt through developmental processes
- React to environments through sensory-motor loops

**Note:** NLM is primarily a C++ library. Python bindings may be available in future releases. This guide focuses on C++ usage.

---

## The 3 Things You Need to Know

1. **Brain** - The virtual brain that thinks (implements neural computation)
2. **World** - The environment the brain lives in (simulation environment)
3. **Agent** - The bridge connecting brain to world (handles I/O and neuromodulation)

---

## Quick Start (C++ - Copy & Paste)

### Example 1: Simplest Brain

```cpp
#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include <iostream>

int main() {
    // Step 1: Create configuration
    auto config = std::make_shared<Config>();
    
    // Step 2: Make a brain
    auto brain = std::make_shared<Brain>(config);
    
    // Step 3: Turn it on
    if (!brain->initialize()) {
        std::cerr << "Failed to initialize brain!" << std::endl;
        return 1;
    }
    
    // Step 4: Make it think for 100 steps
    for (int i = 0; i < 100; ++i) {
        brain->step(i);  // One step of thinking
    }
    
    std::cout << "Done! Your brain thought " << brain->getTotalSpikeCount() 
              << " times" << std::endl;
    
    return 0;
}
```

### Example 2: Brain in a Simple World

```cpp
#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include "world/SimpleWorld.hpp"
#include "agent/AgentBrain.hpp"
#include <iostream>

int main() {
    // Create configuration
    auto config = std::make_shared<Config>();
    
    // Create everything
    auto brain = std::make_shared<Brain>(config);
    if (!brain->initialize()) {
        std::cerr << "Brain initialization failed!" << std::endl;
        return 1;
    }
    
    // Create and configure world
    SimpleWorld world;
    world.configure(10, 10, 8, 8);  // width, height, visionWidth, visionHeight
    world.reset();
    
    // Create agent interface
    AgentBrain agent(brain);
    agent.initialize(world);
    
    // Enable learning subsystems
    agent.enableRewardModulation(true);
    agent.enableDevelopment(true);
    agent.enableCuriosity(true);
    
    // Run for 50 steps
    for (int step = 0; step < 50; ++step) {
        // Update world
        world.update(0.1);  // 0.1 second timestep
        
        // Get what the agent sees
        SensoryPercept percept = world.getSensoryPercept();
        
        // Tell the brain
        agent.processSensoryInput(percept);
        
        // Brain thinks
        brain->step(step);
        
        // Brain decides action
        MotorCommand action = agent.decodeMotorCommand();
        
        // Do action in world
        world.applyMotorCommand(action, world.getSimulationTime());
    }
    
    std::cout << "Simulation finished!" << std::endl;
    std::cout << "Firing neurons: " << brain->getFiringNeuronCount() << std::endl;
    std::cout << "Total spikes: " << brain->getTotalSpikeCount() << std::endl;
    
    return 0;
}
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
