# NLM - Easy Usage Guide for Beginners

## What is NLM?

NLM (Neural Learning Machine) is a **brain simulator** for computers. It lets you create virtual brains that can learn, remember things, and make decisions - just like how your brain works!

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
    // Step 1: Create configuration
    auto config = std::make_shared<Config>();
    config->set("neuron_count", 500);
    config->set("region_count", 1);
    
    // Step 2: Make a brain
    auto brain = std::make_shared<Brain>(config);
    
    // Step 3: Turn it on
    brain->initialize();
    
    // Step 4: Make it think for 10 steps
    for (int i = 0; i < 10; i++) {
        brain->step(i, i * 0.001);
    }
    
    std::cout << "Done! Your brain thought " << brain->getTotalSpikeCount() << " times" << std::endl;
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
    // Create configuration
    auto config = std::make_shared<Config>();
    config->set("neuron_count", 1000);
    config->set("region_count", 2);
    
    // Create brain
    auto brain = std::make_shared<Brain>(config);
    brain->initialize();
    
    // Create world
    SimpleWorld world;
    world.initialize(16, 16);
    world.reset();
    
    // Create agent
    AgentBrain agent(brain);
    agent.initialize(world);
    
    // Run simulation for 50 steps
    for (int step = 0; step < 50; step++) {
        // Update world
        world.update(step * 0.1);
        
        // Get what the agent sees
        SensoryPercept percept = world.observe(brain->getRegion(RegionId(0)).get());
        
        // Tell the brain
        agent.processSensoryInput(percept);
        
        // Brain thinks
        brain->step(step, step * 0.001);
        
        // Get action from brain
        MotorCommand action = agent.decodeMotorCommand();
        
        // Do action in world
        world.applyAction(brain->getRegion(RegionId(0)).get(), action);
    }
    
    std::cout << "Simulation finished!" << std::endl;
    std::cout << "Firing neurons: " << brain->getFiringNeuronCount() << std::endl;
    return 0;
}
```

---

## The Simplest Possible Example

```cpp
#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"

int main() {
    // One line to create a virtual brain
    auto config = std::make_shared<Config>();
    config->set("neuron_count", 100);
    auto brain = std::make_shared<Brain>(config);
    
    // Initialize it
    brain->initialize();
    
    // Make it active
    brain->step(0, 0.0);
    
    std::cout << "Your brain has " << brain->getTotalNeuronCount() << " neurons!" << std::endl;
    return 0;
}
```

---

## Common Patterns

### Pattern 1: Run a Simulation

```cpp
void runSimulation(std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world, 
                   std::shared_ptr<AgentBrain> agent, int numSteps) {
    for (int step = 0; step < numSteps; step++) {
        // 1. Update world
        world->update(step * 0.1);
        
        // 2. Get what the agent sees
        SensoryPercept percept = world->observe(agent->getBrain()->getRegion(RegionId(0)).get());
        
        // 3. Tell the brain
        agent->processSensoryInput(percept);
        
        // 4. Brain thinks
        brain->step(step, step * 0.001);
        
        // 5. Get action from brain
        MotorCommand action = agent->decodeMotorCommand();
        
        // 6. Do action in world
        world->applyAction(agent->getBrain()->getRegion(RegionId(0)).get(), action);
    }
}

// Use it like this:
int main() {
    // Setup (create brain, world, agent...)
    // ...
    
    runSimulation(brain, world, agent, 1000);
    return 0;
}
```

### Pattern 2: Get Brain Stats

```cpp
void printBrainStats(std::shared_ptr<Brain> brain) {
    std::cout << "Neurons firing: " << brain->getFiringNeuronCount() << std::endl;
    std::cout << "Total spikes: " << brain->getTotalSpikeCount() << std::endl;
    std::cout << "Average firing rate: " << brain->getAverageFiringRate() << std::endl;
    std::cout << "Development stage: " << static_cast<int>(brain->getDevelopmentSystem()->getDevelopmentalStage()) << std::endl;
}
```

### Pattern 3: Enable Brain Features

```cpp
void enableLearningFeatures(std::shared_ptr<AgentBrain> agent) {
    // These are all optional:
    agent->enableRewardModulation(true);      // Learn from rewards
    agent->enableStructuralPlasticity(true);  // Grow new connections
    agent->enableDevelopment(true);           // Brain matures over time
    agent->enableCuriosity(true);             // Explore new things
}
```

---

## What Each Part Does

### `std::make_shared<Brain>(config)`
Creates a virtual brain with neurons and synapses.
- `config` = settings for the brain (use defaults for simple setup)

### `brain->initialize()`
Starts up the brain. Always call this before using the brain!

### `brain->step(step_number, time)`
Makes the brain process one moment in time. The brain:
- Checks each neuron
- Sends signals between connected neurons
- Updates connections based on learning rules

### `world->update(time)`
Updates the virtual world by `time` seconds.

### `agent->processSensoryInput(percept)`
Gives sensory information (vision, touch, etc.) to the brain.

### `agent->decodeMotorCommand()`
Reads the brain's motor neurons to decide what action to take.

---

## Mini Projects

### Project 1: Silent Brain (Just Neurons)

```cpp
#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"

int main() {
    auto config = std::make_shared<Config>();
    config->set("neuron_count", 500);
    auto brain = std::make_shared<Brain>(config);
    brain->initialize();
    
    for (int i = 0; i < 10; i++) {
        brain->step(i, i * 0.001);
    }
    
    std::cout << "Silent brain test done!" << std::endl;
    std::cout << "Total spikes: " << brain->getTotalSpikeCount() << std::endl;
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
    auto config = std::make_shared<Config>();
    config->set("neuron_count", 1000);
    auto brain = std::make_shared<Brain>(config);
    brain->initialize();
    
    SimpleWorld world;
    world.initialize(20, 20, 8, 8);
    world.reset();
    
    AgentBrain agent(brain);
    agent.initialize(world);
    
    // Watch the world for 30 steps
    for (int i = 0; i < 30; i++) {
        world.update(i * 0.1);
        SensoryPercept percept = world.observe(agent.getBrain()->getRegion(RegionId(0)).get());
        agent.processSensoryInput(percept);
        brain->step(i, i * 0.001);
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
    auto config = std::make_shared<Config>();
    auto brain = std::make_shared<Brain>(config);
    brain->initialize();
    
    SimpleWorld world;
    world.initialize(15, 15, 8, 8);
    world.reset();
    
    AgentBrain agent(brain);
    agent.initialize(world);
    
    // Enable learning
    agent.enableRewardModulation(true);
    agent.enableCuriosity(true);
    
    // Run agent
    for (int step = 0; step < 100; step++) {
        world.update(step * 0.1);
        SensoryPercept percept = world.observe(agent.getBrain()->getRegion(RegionId(0)).get());
        agent.processSensoryInput(percept);
        brain->step(step, step * 0.001);
        MotorCommand action = agent.decodeMotorCommand();
        world.applyAction(agent.getBrain()->getRegion(RegionId(0)).get(), action);
        
        if (step % 20 == 0) {
            std::cout << "Step " << step << ": " << brain->getFiringNeuronCount() << " neurons firing" << std::endl;
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
| Create brain | `std::make_shared<Brain>(config)` |
| Start brain | `brain->initialize()` |
| Make brain think | `brain->step(step, time)` |
| Create world | `SimpleWorld world; world.initialize(width, height)` |
| Create agent | `AgentBrain agent(brain)` |
| Get brain stats | `brain->getFiringNeuronCount()` |
| See world | `world.observe(brain)` |
| Make action | `agent.decodeMotorCommand()` |

---

## Next Steps

When you're comfortable:
1. Read `HOW_TO_USE.md` for more details
2. Read `docs/ARCHITECTURE.md` to understand how it all works
3. Experiment with different configurations!

That's it! You're now ready to use NLM.
