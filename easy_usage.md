# NLM - Easy Usage Guide for Beginners

## What is NLM?

NLM is a **brain simulator** for computers. It lets you create virtual brains that can learn, remember things, and make decisions - just like how your brain works!

Think of it like this:
- **Neurons** = Brain cells that send signals to each other
- **Synapses** = Connections between brain cells
- **Learning** = When connections get stronger or weaker based on what happens
- **Memory** = The brain remembering patterns

That's it! NLM simulates all of this.

## Quick Installation

### Build from Source (C++ Core)

```bash
# Clone the repository
# (assuming you're already in the repo directory)

# Create build directory
mkdir -p build
cd build

# Configure with CMake (Release build recommended for performance)
cmake .. -DCMAKE_BUILD_TYPE=Release

# Build all targets (parallel build for speed)
make -j$(nproc)

# Optional: Run tests
ctest --output-on-failure
```

### Install Python Bindings

```bash
# Install pybind11 dependencies
pip install pybind11 scikit-build-core pytest numpy

# Install Python bindings
pip install .  # From source
# or pip install -e .  # In development mode
```

## Important: Memory Management for C++

When working with NLM in C++, remember these key principles:

### 1. Use Shared Pointers for Brain Ownership
```cpp
#include <memory>
#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"

// Create configuration
auto config = std::make_shared<Config>();
config->set("neuron_count", 1000);
config->set("region_count", 2);

// Create brain (takes shared ownership of config)
auto brain = std::make_shared<Brain>(config);

// Initialize brain
if (brain->initialize()) {
    std::cout << "Brain created successfully with " 
              << brain->getTotalNeuronCount() << " neurons" << std::endl;
}
```

### 2. Brain Cleanup - Automatic Destruction
```cpp
{
    // Create brain in scope
    auto brain = std::make_shared<Brain>(config);
    brain->initialize();
    
    // Run simulation
    for (int step = 0; step < 1000; ++step) {
        brain->step(step);
    }
    
    // Brain is automatically destroyed here!
    // All memory is properly cleaned up
} // <- brain and config destroyed here
```

### 3. Raw Pointers Only for Brain Access
```cpp
// Safe: Use smart pointers for ownership
auto brain = std::make_shared<Brain>(config);
brain->initialize();

// Access neurons through brain interface
if (auto* region = brain->getRegion(RegionId(1))) {
    for (auto* neuron : region->getAllNeurons()) {
        // Use neuron pointers for temporary operations
        float potential = neuron->getMembranePotential();
    }
}
// No manual memory management needed!
```

## Why This Pattern Matters

NLM uses the **pImpl pattern** (Pointer to Implementation) where:

1. **Brain class has private implementation** (struct Impl)
2. **Brain manages all internal memory** (neurons, synapses, connections)
3. **Proper cleanup requires unique_ptr in the implementation**

Using `std::shared_ptr<Brain>` ensures that:
- Brain's internal memory is automatically cleaned up
- Multiple parts of code can safely share brain ownership
- Config is properly managed alongside brain

## Common Bugs to Avoid

### Bug 1: Manual Brain Destruction
```cpp
// BAD - Manual deletion leads to crashes
Brain* brain = new Brain(config);  // Wrong!
brain->initialize();
delete brain;  // Wrong - internal Impl not deleted!
```

### Bug 2: Brain Expired While Still in Use
```cpp
// BAD - Brain destroyed while iterators still hold references
auto brain = std::make_shared<Brain>(config);
brain->initialize();

std::vector<NeuronId> activeNeurons;
for (SimulationStep step = 0; step < 1000; ++step) {
    brain->step(step);
    activeNeurons = brain->getMostActiveNeurons(10);  // Safe
}

// Now 'brain' goes out of scope and is destroyed
```

## Advanced Usage: Event-Driven Brain

NLM supports event-driven programming for efficient large-scale simulations:

```cpp
#include "brain/Brain.hpp"
#include "brain/SpikeSystem.hpp"

// Create brain with event-driven capabilities
auto brain = std::make_shared<Brain>(config);
brain->initialize();

// Get spike system for event handling
if (auto* spikeSystem = brain->getSpikeSystem()) {
    // Register spike handler
    spikeSystem->registerHandler([](const SpikeEvent& event) {
        std::cout << "Neuron " << event.neuronId 
                  << " fired at time " << event.time << std::endl;
    });
    
    // Register delayed spike handler
    spikeSystem->registerDelayedHandler([](const DelayedSpikeEvent& event) {
        std::cout << "Delayed spike from neuron " 
                  << event.sourceNeuronId << " to neuron " 
                  << event.destinationNeuronId << std::endl;
    });
}

// Run simulation with event-driven processing
for (int step = 0; step < 1000; ++step) {
    brain->step(step);
}
```

## Complete Example: Production-Ready Brain

```cpp
#include <memory>
#include <iostream>
#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"

class Simulation {
public:
    Simulation(const std::string& configPath = "configs/default.cfg") {
        // Initialize logging
        auto logger = std::make_shared<Logger>();
        auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
        logger->addLogger(consoleLogger);
        Logger::setGlobal(logger);
        
        // Load configuration
        config_ = std::make_shared<Config>();
        if (config_->loadFromFile(configPath)) {
            NLM_LOG_INFO("Loaded configuration from: " + configPath);
        }
        
        // Set defaults if needed
        config_->set("neuron_count", 1000);
        config_->set("region_count", 2);
        config_->set("random_seed", 42);
        
        // Create brain
        brain_ = std::make_shared<Brain>(config_);
        if (!brain_->initialize()) {
            throw std::runtime_error("Failed to initialize brain");
        }
        
        NLM_LOG_INFO("Simulation initialized successfully");
    }
    
    void runSteps(size_t steps) {
        if (!brain_) {
            throw std::runtime_error("Brain not initialized");
        }
        
        for (size_t step = 0; step < steps; ++step) {
            // Check for early termination (e.g., stop condition)
            if (shouldStop(step)) {
                break;
            }
            
            brain_->step(step, step * 0.001);
            
            // Log statistics periodically
            if (step % 100 == 0) {
                logStatus(step);
            }
        }
    }
    
    void saveState(const std::string& path) {
        if (brain_) {
            if (brain_->save(path)) {
                NLM_LOG_INFO("State saved to: " + path);
            } else {
                NLM_LOG_ERROR("Failed to save state to: " + path);
            }
        }
    }
    
    void loadState(const std::string& path) {
        // Reload configuration
        config_ = std::make_shared<Config>();
        config_->loadFromFile("configs/default.cfg");
        
        // Create new brain
        brain_ = std::make_shared<Brain>(config_);
        if (brain_->load(path)) {
            brain_->initialize();
            NLM_LOG_INFO("State loaded from: " + path);
        } else {
            NLM_LOG_ERROR("Failed to load state from: " + path);
        }
    }
    
    std::shared_ptr<Brain> getBrain() const { return brain_; }
    std::shared_ptr<const Config> getConfig() const { return config_; }
    
private:
    void logStatus(size_t step) {
        if (!brain_) return;
        
        NLM_LOG_INFO("=== Step " + std::to_string(step) + " ===");
        NLM_LOG_INFO("  Total neurons: " + std::to_string(brain_->getTotalNeuronCount()));
        NLM_LOG_INFO("  Total synapses: " + std::to_string(brain_->getTotalSynapseCount()));
        NLM_LOG_INFO("  Firing neurons: " + std::to_string(brain_->getFiringNeuronCount()));
        NLM_LOG_INFO("  Average firing rate: " + std::to_string(brain_->getAverageFiringRate()));
        NLM_LOG_INFO("  E/I ratio: " + std::to_string(brain_->getExcitationInhibitionRatio()));
    }
    
    bool shouldStop(size_t step) {
        // Example stop condition: after reaching target steps
        return (step >= 10000);
    }
    
    std::shared_ptr<Config> config_;
    std::shared_ptr<Brain> brain_;
};

int main() {
    try {
        // Create simulation
        Simulation sim("configs/default.cfg");
        
        // Run simulation
        sim.runSteps(5000);
        
        // Save final state
        sim.saveState("final_brain_state.bin");
        
        std::cout << "Simulation completed successfully!" << std::endl;
        
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
}
```

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
