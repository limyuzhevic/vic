# NLM - Easy Usage Guide for Beginners

## What is NLM?

NLM is a **brain simulator** for computers. It lets you create virtual brains that can learn, remember things, and make decisions - just like how your brain works!

Think of it like this:
- **Neurons** = Brain cells that send signals to each other
- **Synapses** = Connections between brain cells
- **Learning** = When connections get stronger or weaker based on what happens
- **Memory** = The brain remembering patterns

That's it! NLM simulates all of this using real spiking neural networks.

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
    // Step 1: Create a brain with default configuration
    auto config = std::make_shared<Config>();
    auto brain = std::make_shared<Brain>(config);
    
    // Step 2: Initialize the brain
    brain->initialize();
    
    // Step 3: Make the brain think for 100 steps
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
#include "agent/AgentBrain.hpp"
#include "world/SimpleWorld.hpp"
#include "core/Config/Config.hpp"

int main() {
    // Create everything
    auto config = std::make_shared<Config>();
    auto brain = std::make_shared<Brain>(config);
    brain->initialize();
    
    auto world = std::make_shared<SimpleWorld>();
    world->configure(width=10, height=10, visionWidth=8, visionHeight=8);
    world->reset();
    
    auto agent = std::make_shared<AgentBrain>(brain);
    agent->initialize(*world);
    
    // Run for 50 steps
    for (int step = 0; step < 50; ++step) {
        world->update(0.1);  // Update world
        auto percept = world->getSensoryPercept();  // What does the agent see?
        agent->processSensoryInput(percept);  // Brain sees it
        brain->step(step);  // Brain thinks
        auto action = agent->decodeMotorCommand();  // Brain decides action
        world->applyMotorCommand(action, world->getSimulationTime());  // Do action
    }
    
    std::cout << "Simulation finished!" << std::endl;
    std::cout << "Firing neurons: " << brain->getFiringNeuronCount() << std::endl;
}
```

### Example 3: Complete Agent Example

```cpp
#include "brain/Brain.hpp"
#include "agent/AgentBrain.hpp"
#include "world/SimpleWorld.hpp"
#include "core/Config/Config.hpp"

void runAgentSimulation(int numSteps = 1000) {
    """Run a complete agent simulation with brain and world."""
    
    // 1. Create configuration
    auto config = std::make_shared<Config>();
    
    // 2. Create brain and initialize
    auto brain = std::make_shared<Brain>(config);
    brain->initialize();
    
    // 3. Create agent brain interface
    auto agent = std::make_shared<AgentBrain>(brain);
    
    // 4. Create and configure world
    auto world = std::make_shared<SimpleWorld>();
    world->configure(width=20, height=20, visionWidth=8, visionHeight=8);
    world->reset();
    world->setAgentStart(10.0, 10.0);
    
    // 5. Initialize agent with world
    agent->initialize(*world);
    
    // 6. Enable learning subsystems
    agent->enableRewardModulation(true);
    agent->enableCuriosity(true);
    agent->enableDevelopment(true);
    
    // 7. Run simulation loop
    for (int step = 0; step < numSteps; ++step) {
        // Update world
        world->update(timestep=0.1);
        
        // Get sensory input from world
        const auto& percept = world->getSensoryPercept();
        
        // Process sensory input in brain
        agent->processSensoryInput(percept);
        
        // Run brain step
        brain->step(step);
        
        // Decode motor command from brain activity
        auto motor_cmd = agent->decodeMotorCommand();
        
        // Apply motor command to world
        world->applyMotorCommand(motor_cmd, world->getSimulationTime());
        
        // Print progress
        if (step % 100 == 0) {
            std::cout << "Step " << step << ":" << std::endl;
            std::cout << "  Firing neurons: " << brain->getFiringNeuronCount() << std::endl;
            std::cout << "  Curiosity: " << agent->getCuriosityLevel() << std::endl;
            std::cout << "  Novelty: " << agent->getNoveltyLevel() << std::endl;
            std::cout << "  Development Stage: " << brain->getDevelopmentalStage() << std::endl;
        }
    }
}

int main() {
    runAgentSimulation(1000);
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
    auto config = std::make_shared<Config>();
    auto brain = std::make_shared<Brain>(config);
    
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

```cpp
// Just add these inside your loop:
std::cout << "Neurons firing: " << brain->getFiringNeuronCount() << std::endl;
std::cout << "Total spikes: " << brain->getTotalSpikeCount() << std::endl;
std::cout << "Avg firing rate: " << brain->getAverageFiringRate() << std::endl;
std::cout << "Development stage: " << brain->getDevelopmentalStage() << std::endl;
```

### Pattern 3: Enable Brain Features

```cpp
// These are all optional:
agent->enableRewardModulation(true);      // Learn from rewards
agent->enableCuriosity(true);             // Explore new things
agent->enableDevelopment(true);          // Brain matures over time
agent->enableStructuralPlasticity(true);  // Grow new connections
```

---

## Mini Projects

### Project 1: Silent Brain (Just Neurons)

```cpp
#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"

int main() {
    auto brain = std::make_shared<Brain>(std::make_shared<Config>());
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
#include "agent/AgentBrain.hpp"
#include "world/SimpleWorld.hpp"
#include "core/Config/Config.hpp"

int main() {
    // Setup
    auto brain = std::make_shared<Brain>(std::make_shared<Config>());
    brain->initialize();
    auto world = std::make_shared<SimpleWorld>();
    world->configure(width=20, height=20, visionWidth=8, visionHeight=8);
    world->reset();
    auto agent = std::make_shared<AgentBrain>(brain);
    agent->initialize(*world);
    
    // Watch the world for 30 steps
    for (int i = 0; i < 30; ++i) {
        world->update(0.1);
        agent->processSensoryInput(world->getSensoryPercept());
        brain->step(i);
    }
    
    std::cout << "Watched world for 30 steps" << std::endl;
    std::cout << "Firing rate: " << brain->getAverageFiringRate() << std::endl;
}
```

### Project 3: Complete Agent

```cpp
#include "brain/Brain.hpp"
#include "agent/AgentBrain.hpp"
#include "world/SimpleWorld.hpp"
#include "core/Config/Config.hpp"

int main() {
    // Setup
    auto config = std::make_shared<Config>();
    auto brain = std::make_shared<Brain>(config);
    brain->initialize();
    auto world = std::make_shared<SimpleWorld>();
    world->configure(width=15, height=15, visionWidth=8, visionHeight=8);
    world->reset();
    auto agent = std::make_shared<AgentBrain>(brain);
    agent->initialize(*world);
    
    // Enable learning
    agent->enableRewardModulation(true);
    agent->enableCuriosity(true);
    
    // Run agent
    for (int step = 0; step < 100; ++step) {
        world->update(0.1);
        agent->processSensoryInput(world->getSensoryPercept());
        brain->step(step);
        auto action = agent->decodeMotorCommand();
        world->applyMotorCommand(action, world->getSimulationTime());
        
        if (step % 20 == 0) {
            std::cout << "Step " << step << ": " << brain->getFiringNeuronCount() 
                      << " neurons firing" << std::endl;
        }
    }
    
    std::cout << "Agent simulation complete!" << std::endl;
}
```

---

## Quick Reference

| What you want | Code |
|--------------|------|
| Create brain | `auto brain = std::make_shared<Brain>(config)` |
| Start brain | `brain->initialize()` |
| Make brain think | `brain->step(step)` |
| Create world | `auto world = std::make_shared<SimpleWorld>()` |
| Create agent | `auto agent = std::make_shared<AgentBrain>(brain)` |
| Get brain stats | `brain->getFiringNeuronCount()` |
| See world | `world->getSensoryPercept()` |
| Make action | `agent->decodeMotorCommand()` |

---

## Troubleshooting

**"My brain isn't doing anything"**
- Did you call `brain->initialize()`?
- Try increasing the number of steps

**"The agent isn't moving"**
- Did you call `world->applyMotorCommand()`?
- Check that `world->update()` is being called

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

---

## Important Notes

**This documentation reflects the C++ implementation of NLM.**

The NLM project is a C++20 brain simulation framework that implements Leaky Integrate-and-Fire (LIF) neurons with event-driven spike propagation, synaptic delays, and multiple plasticity mechanisms.

For Python users, please refer to the [improved_usage_guide.md](improved_usage_guide.md) which contains information about the command-line interface and Python script examples.

The Python API (`pynlm`) documentation is incomplete and should not be relied upon for development. The core NLM functionality is C++ based.

For the latest information and complete documentation, visit the project README and architecture documentation.