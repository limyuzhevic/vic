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

```cpp
#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"

int main() {
    // Step 1: Make a brain
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", 100);
    config->set("region_count", 1);
    auto brain = std::make_shared<nlm::Brain>(config);
    
    // Step 2: Turn it on
    brain->initialize();
    
    // Step 3: Make it think for 100 steps
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
#include "world/SimpleWorld.hpp"
#include "agent/AgentBrain.hpp"
#include "core/Config/Config.hpp"

int main() {
    // Create everything
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", 500);
    config->set("region_count", 1);
    
    auto brain = std::make_shared<nlm::Brain>(config);
    brain->initialize();
    
    nlm::SimpleWorld world;
    world.initialize(16, 16);
    world.reset();
    
    nlm::AgentBrain agent(brain);
    agent.initialize(world);
    
    // Run for 50 steps
    for (int step = 0; step < 50; ++step) {
        world.update(0.1);  // Update world
        auto percept = world.getSensoryPercept();  // What does the agent see?
        agent.processSensoryInput(percept);  // Brain sees it
        brain->step(step);  // Brain thinks
        auto action = agent.decodeMotorCommand();  // Brain decides action
        world.applyMotorCommand(action, world.getSimulationTime());  // Do action
    }
    
    std::cout << "Simulation finished!" << std::endl;
    std::cout << "Firing neurons: " << brain->getFiringNeuronCount() << std::endl;
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

```cpp
#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"

int main() {
    // One line to create a virtual brain
    auto config = std::make_shared<nlm::Config>();
    auto brain = std::make_shared<nlm::Brain>(config);
    
    // Initialize it
    brain->initialize();
    
    // Make it active
    brain->step(0);
    
    std::cout << "Your brain has " << brain->getTotalNeuronCount() 
              << " neurons!" << std::endl;
    
    return 0;
}
```

---

## Common Patterns

### Pattern 1: Run a Simulation

```cpp
#include "brain/Brain.hpp"
#include "world/SimpleWorld.hpp"
#include "agent/AgentBrain.hpp"
#include "core/Config/Config.hpp"

void run_simulation(nlm::Brain* brain, nlm::SimpleWorld* world, 
                   nlm::AgentBrain* agent, int num_steps) {
    for (int step = 0; step < num_steps; ++step) {
        // 1. Update world
        world->update(0.1);
        
        // 2. Get what the agent sees
        auto percept = world->getSensoryPercept();
        
        // 3. Tell the brain
        agent->processSensoryInput(percept);
        
        // 4. Brain thinks
        brain->step(step);
        
        // 5. Get action from brain
        auto action = agent->decodeMotorCommand();
        
        // 6. Do action in world
        world->applyMotorCommand(action, world->getSimulationTime());
    }
}

// Use it like this:
int main() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", 500);
    
    auto brain = std::make_shared<nlm::Brain>(config);
    brain->initialize();
    
    nlm::SimpleWorld world;
    world.configure(20, 20, 8, 8);
    world.reset();
    
    nlm::AgentBrain agent(brain);
    agent.initialize(world);
    
    run_simulation(brain.get(), &world, &agent, 1000);
    
    return 0;
}
```

### Pattern 2: Get Brain Stats

```cpp
// Just add these inside your simulation loop:
std::cout << "Neurons firing: " << brain->getFiringNeuronCount() << std::endl;
std::cout << "Total spikes: " << brain->getTotalSpikeCount() << std::endl;
std::cout << "Avg firing rate: " << brain->getAverageFiringRate() << std::endl;
std::cout << "Development stage: " << brain->getDevelopmentalStage() << std::endl;
```

### Pattern 3: Enable Brain Features

```cpp
agent->enableRewardModulation(true);      // Learn from rewards
agent->enableStructuralPlasticity(true);  // Grow new connections
agent->enableDevelopment(true);           // Brain matures over time
agent->enableCuriosity(true);             // Explore new things
```

---

## What Each Part Does

### `Brain` - Create brain
Creates a virtual brain with neurons and synapses.
- `config` = settings for the brain (use `createDefaultConfig()` for simple setup)

### `brain->initialize()`
Starts up the brain. Always call this before using the brain!

### `brain->step(step_number)`
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
Executes the brain's decision in the world.

---

## Mini Projects

### Project 1: Silent Brain (Just Neurons)

```cpp
#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"

int main() {
    auto config = std::make_shared<nlm::Config>();
    auto brain = std::make_shared<nlm::Brain>(config);
    brain->initialize();
    
    for (int i = 0; i < 10; ++i) {
        brain->step(i);
    }
    
    std::cout << "Silent brain test done!" << std::endl;
    return 0;
}
```

### Project 2: Brain Watching a World

```cpp
#include "brain/Brain.hpp"
#include "world/SimpleWorld.hpp"
#include "agent/AgentBrain.hpp"
#include "core/Config/Config.hpp"

int main() {
    // Setup
    auto config = std::make_shared<nlm::Config>();
    auto brain = std::make_shared<nlm::Brain>(config);
    brain->initialize();
    
    nlm::SimpleWorld world;
    world.configure(20, 20, 8, 8);
    world.reset();
    
    nlm::AgentBrain agent(brain);
    agent.initialize(world);
    
    // Watch the world for 30 steps
    for (int i = 0; i < 30; ++i) {
        world.update(0.1);
        agent.processSensoryInput(world.getSensoryPercept());
        brain->step(i);
    }
    
    std::cout << "Watched world for 30 steps" << std::endl;
    std::cout << "Firing rate: " << brain->getAverageFiringRate() << std::endl;
    
    return 0;
}
```

### Project 3: Complete Agent

```cpp
#include "brain/Brain.hpp"
#include "world/SimpleWorld.hpp"
#include "agent/AgentBrain.hpp"
#include "core/Config/Config.hpp"

int main() {
    // Setup
    auto config = std::make_shared<nlm::Config>();
    auto brain = std::make_shared<nlm::Brain>(config);
    brain->initialize();
    
    nlm::SimpleWorld world;
    world.configure(15, 15, 8, 8);
    world.reset();
    
    nlm::AgentBrain agent(brain);
    agent.initialize(world);
    
    // Enable learning
    agent.enableRewardModulation(true);
    agent.enableCuriosity(true);
    
    // Run agent
    for (int step = 0; step < 100; ++step) {
        world.update(0.1);
        agent.processSensoryInput(world.getSensoryPercept());
        brain->step(step);
        
        auto action = agent.decodeMotorCommand();
        world.applyMotorCommand(action, world.getSimulationTime());
        
        if (step % 20 == 0) {
            std::cout << "Step " << step << ": " << brain->getFiringNeuronCount() 
                      << " neurons firing" << std::endl;
        }
    }
    
    std::cout << "Agent simulation complete!" << std::endl;
    
    return 0;
}
```

---

## Quick Reference

| What you want | Code |
|--------------|------|
| Create brain | `auto brain = std::make_shared<nlm::Brain>(config)` |
| Start brain | `brain->initialize()` |
| Make brain think | `brain->step(step)` |
| Create world | `nlm::SimpleWorld world; world.initialize(width, height, visionWidth, visionHeight)` |
| Create agent | `nlm::AgentBrain agent(brain)` |
| Get brain stats | `brain->getFiringNeuronCount()` |
| See world | `world.getSensoryPercept()` |
| Make action | `agent.decodeMotorCommand()` |

---

## Troubleshooting

**"My brain isn't doing anything"**
- Did you call `brain->initialize()`?
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
