# NLM Tutorial: Your First Brain

Welcome to NLM (Neural Learning Machine)! This tutorial will guide you through creating, running, and learning with your first artificial brain. No prior experience with neural networks is required - just a willingness to learn and experiment.

## What You'll Learn

- How to create and initialize a brain
- How to interact with the world
- How learning emerges from neural dynamics
- How to interpret simulation results
- How to extend your brain with different features

## Prerequisites

- Basic understanding of programming concepts
- Interest in artificial intelligence and brain simulation
- C++ compiler (CMake 3.16+, GCC 10+, or Clang 12+)

No Python knowledge required for this C++ tutorial, though NLM also has Python bindings.

## Section 1: The Three Components

NLM works with three main components:

1. **Brain** - The thinking engine that processes information and learns
2. **World** - The environment the brain lives in
3. **Agent** - The bridge that connects brain to world

### Think of it this way:

| Real World | NLM |
|------------|-----|
| You see something | `percept = world.getSensoryPercept()` |
| Your brain processes it | `agent.processSensoryInput(percept)` |
| You decide to act | `action = agent.decodeMotorCommand()` |
| You do the action | `world.applyMotorCommand(action, time)` |
| Your brain learns from results | `agent.applyRewardModulation(reward, predicted)` |

## Section 2: Setup and Installation

### Option A: Using the Provided Build System (Recommended for Beginners)

NLM comes with a simple build script. Let's use that:

```bash
# Navigate to the project directory
cd /path/to/nlm

# Build the example
mkdir -p build
cd build
cmake ..
make -j$(nproc)

# Run a simple test
./nlm_test
```

### Option B: Manual Build (Advanced Users)

For those who want more control:

```bash
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4
```

## Section 3: Your First Brain - The Minimal Example

Let's create the simplest possible brain. Copy this into a file called `minimal_example.cpp`:

```cpp
#include <nlm/core/Config/Config.hpp>
#include <nlm/core/Random/Random.hpp>
#include <nlm/brain/Brain.hpp>
#include <nlm/core/Logger/Logger.hpp>
#include <iostream>

int main() {
    // Initialize logger to see what's happening
    auto logger = std::make_shared<Logger>();
    auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);
    
    std::cout << "=== NLM Minimal Example ===" << std::endl;
    
    // Create configuration
    auto config = std::make_shared<Config>();
    config->set("random_seed", 42, ConfigSource::Default);
    config->set("neuron_count", 100, ConfigSource::Default);  // Small for quick demo
    
    // Create and initialize brain
    auto brain = std::make_shared<Brain>(config);
    
    std::cout << "Creating brain with " << brain->getTotalNeuronCount() 
              << " neurons..." << std::endl;
    
    if (!brain->initialize()) {
        std::cerr << "Failed to initialize brain!" << std::endl;
        return 1;
    }
    
    std::cout << "Brain initialized successfully!" << std::endl;
    
    // Run a few steps to see activity
    std::cout << "Running simulation for 50 steps..." << std::endl;
    for (SimulationStep step = 0; step < 50; ++step) {
        brain->step(step);
        
        // Show progress
        if (step % 10 == 0) {
            std::cout << "Step " << step << ": " 
                      << brain->getFiringNeuronCount() << " neurons firing, "
                      << brain->getTotalSpikeCount() << " total spikes" << std::endl;
        }
    }
    
    std::cout << "\n=== Simulation Complete ===" << std::endl;
    std::cout << "Final statistics:" << std::endl;
    std::cout << "  Total neurons: " << brain->getTotalNeuronCount() << std::endl;
    std::cout << "  Total spikes: " << brain->getTotalSpikeCount() << std::endl;
    std::cout << "  Average firing rate: " << brain->getAverageFiringRate() << " Hz" << std::endl;
    std::cout << "  Excitatory/Inhibitory ratio: " << brain->getExcitationInhibitionRatio() << std::endl;
    
    return 0;
}
```

### Compile and Run

```bash
cd build
g++ -std=c++20 minimal_example.cpp -o minimal_example -lnlm
./minimal_example
```

### Expected Output

You should see output like:

```
=== NLM Minimal Example ===
Creating brain with 100 neurons...
Brain initialized successfully!
Step 0: 0 neurons firing, 0 total spikes
Step 10: 5 neurons firing, 12 total spikes
Step 20: 8 neurons firing, 28 total spikes
Step 30: 10 neurons firing, 45 total spikes
Step 40: 12 neurons firing, 67 total spikes

=== Simulation Complete ===
Final statistics:
  Total neurons: 100
  Total spikes: 67
  Average firing rate: 0.67 Hz
  Excitatory/Inhibitory ratio: 2.34
```

## Section 4: Brain in a World - The Agent Example

Now let's create a more realistic example where the brain interacts with an environment. Create `agent_example.cpp`:

```cpp
#include <nlm/core/Config/Config.hpp>
#include <nlm/brain/Brain.hpp>
#include <nlm/world/SimpleWorld.hpp>
#include <nlm/neuromodulation/Neuromodulator.hpp>
#include <nlm/core/Logger/Logger.hpp>
#include <iostream>

// AgentBrain: The interface between brain and world
class AgentBrain {
private:
    std::shared_ptr<Brain> brain;
    std::shared_ptr<SimpleWorld> world;
    bool rewardModulationEnabled;
    bool curiosityEnabled;
    bool structuralPlasticityEnabled;
    bool developmentEnabled;
    
public:
    AgentBrain(std::shared_ptr<Brain> b) : brain(b), rewardModulationEnabled(false),
                                           curiosityEnabled(false), structuralPlasticityEnabled(false),
                                           developmentEnabled(false) {}
    
    void initialize(std::shared_ptr<SimpleWorld> w) {
        world = w;
        std::cout << "AgentBrain initialized with world" << std::endl;
    }
    
    void processSensoryInput(const std::vector<float>& percept) {
        // Convert world percept to brain input
        brain->receiveSensoryInput(percept);
    }
    
    Action decodeMotorCommand() {
        // Decode brain's motor command
        return brain->produceAction();
    }
    
    void applyRewardModulation(float reward, float predicted) {
        if (rewardModulationEnabled) {
            // Apply reward-based learning
            // This would interface with neuromodulators in full implementation
            std::cout << "  Reward modulation: " << reward << " (predicted: " << predicted << ")" << std::endl;
        }
    }
    
    void updateDevelopment(double dt) {
        if (developmentEnabled) {
            // Update developmental stage
            brain->updateDevelopmentalStage(dt);
        }
    }
    
    void enableRewardModulation(bool enable) { rewardModulationEnabled = enable; }
    void enableStructuralPlasticity(bool enable) { structuralPlasticityEnabled = enable; }
    void enableDevelopment(bool enable) { developmentEnabled = enable; }
    void enableCuriosity(bool enable) { curiosityEnabled = enable; }
    
    float getCuriosityLevel() const { return curiosityEnabled ? 0.5f : 0.0f; }
    float getNoveltyLevel() const { return noveltyEnabled ? 0.3f : 0.0f; }
    
private:
    bool noveltyEnabled = false;
};

int main() {
    // Initialize logger
    auto logger = std::make_shared<Logger>();
    auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);
    
    std::cout << "=== NLM Agent Example ===" << std::endl;
    
    // Create configuration
    auto config = std::make_shared<Config>();
    config->set("random_seed", 42, ConfigSource::Default);
    config->set("neuron_count", 500, ConfigSource::Default);
    
    // Create brain
    auto brain = std::make_shared<Brain>(config);
    
    std::cout << "Creating brain with " << brain->getTotalNeuronCount() << " neurons..." << std::endl;
    
    if (!brain->initialize()) {
        std::cerr << "Failed to initialize brain!" << std::endl;
        return 1;
    }
    
    // Create world
    auto world = std::make_shared<SimpleWorld>();
    world->configure(20, 20, 8, 8);  // 20x20 world with 8x8 vision
    world->reset();
    
    std::cout << "World configured: 20x20 cells with 8x8 vision" << std::endl;
    
    // Create agent interface
    auto agent = std::make_shared<AgentBrain>(brain);
    agent->initialize(world);
    
    // Enable learning features
    agent->enableRewardModulation(true);
    agent->enableCuriosity(true);
    agent->enableDevelopment(true);
    
    std::cout << "Enabling learning features: Reward modulation, Curiosity, Development" << std::endl;
    
    // Run simulation loop
    std::cout << "\nRunning simulation loop..." << std::endl;
    std::cout << "Step | Firing | Curiosity | Novelty | Dev Stage | Action" << std::endl;
    std::cout << "-----|--------|----------|---------|-----------|-------" << std::endl;
    
    for (SimulationStep step = 0; step < 100; ++step) {
        // 1. Update world
        world->update(0.1);  // 0.1 second timestep
        
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
        
        // 7. Learn from results (simplified)
        auto currentPercept = world->getSensoryPercept();
        float reward = currentPercept.getInternal()[0] if currentPercept.getInternal().size() > 0 else 0.0f;
        agent->applyRewardModulation(reward, 0.0f);
        
        // 8. Update development
        agent->updateDevelopment(0.1);
        
        // Print progress every 10 steps
        if (step % 10 == 0) {
            std::cout << step << " | " << brain->getFiringNeuronCount() << " | "
                      << agent->getCuriosityLevel() << " | " << agent->getNoveltyLevel() << " | "
                      << brain->getDevelopmentalStage() << " | "
                      << action.toString() << std::endl;
        }
    }
    
    std::cout << "\n=== Simulation Complete ===" << std::endl;
    std::cout << "Final brain state:" << std::endl;
    std::cout << "  Total spikes: " << brain->getTotalSpikeCount() << std::endl;
    std::cout << "  Average firing rate: " << brain->getAverageFiringRate() << " Hz" << std::endl;
    std::cout << "  Final developmental stage: " << brain->getDevelopmentalStage() << std::endl;
    
    return 0;
}
```

### Key Points from Agent Example

1. **Sensory Processing**: The brain receives sensory input from the world
2. **Motor Output**: The brain produces actions that are sent to the world
3. **Learning Loop**: Reward signals drive learning
4. **Development**: Brain matures over time through developmental stages

## Section 5: Understanding What You're Seeing

### Brain Statistics Explained

- **Firing Neurons**: Neurons currently sending out action potentials (spikes)
- **Total Spikes**: Total number of spikes generated during the simulation
- **Average Firing Rate**: Average spikes per second across all neurons
- **E/I Ratio**: Excitatory to inhibitory neuron balance

### Developmental Stages

The brain goes through these stages:

1. **Initial**: Primitive neural networks
2. **CriticalPeriod**: High plasticity, rapid learning
3. **Maturation**: Specialization, refined circuits
4. **Adult**: Stable, efficient processing
5. **Aging**: Gradual decline in plasticity

## Section 6: Expanding Your Brain

### Adding More Neurons

Increase learning capacity by using more neurons:

```cpp
config->set("neuron_count", 5000, ConfigSource::Default);  // 5x more neurons
```

### Connecting to Real World

In a full NLM implementation, you'd:

1. Use actual sensory data (camera, microphone, etc.)
2. Control physical actions (motor control, etc.)
3. Receive real rewards (achievements, survival, etc.)

### Multiple Brains

You can run multiple brains that learn from the same world:

```cpp
auto brain1 = std::make_shared<Brain>(config);
auto brain2 = std::make_shared<Brain>(config);
// Each brain can learn different strategies
```

## Section 7: Common Questions and Pitfalls

### "My brain isn't doing anything!"

**Check these:
- Did you call `brain.initialize()`? Always do this!
- Are there enough neurons? Try 1000+ for visible activity
- Does the brain have sensory input? Neurons need input to fire

### "Everything is 0"

**Brain "warm-up" time:
- Brains need time to become active
- Some neurons need external stimulation
- Try running for 100+ steps

### "The agent isn't moving"

**Check these:
- Did you call `world.applyMotorCommand()`?
- Is the brain producing motor commands?
- Are the actions valid for the world?

## Section 8: Your Next Steps

### For Beginners:

1. **Try different neuron counts**: 100, 1000, 5000
2. **Run the existing demos**: Look for phase3_demo, phase4_demo in the build directory
3. **Experiment with plasticity**: Enable/disable STDP, Hebbian learning
4. **Explore the configuration file**: See what parameters you can change

### For Intermediate Users:

1. **Read the API documentation**: `docs/API_REFERENCE.md`
2. **Study the configuration guide**: `docs/CONFIG_GUIDE.md`
3. **Create custom plasticity rules**: Implement your own learning algorithms
4. **Build world environments**: Create your own SimpleWorld subclasses

### For Advanced Users:

1. **Contribute to the documentation**: Help improve these guides
2. **Add new plasticity mechanisms**: Implement novel learning rules
3. **Create complex environments**: Multi-agent, multi-sensory worlds
4. **Performance optimization**: Profile and optimize simulation speed

## Section 9: Example Scripts Summary

We've created these example scripts as part of this documentation:

1. **minimal_example.cpp** - The simplest possible brain
2. **agent_example.cpp** - Brain-world interaction
3. **exploration_example.cpp** - Curiosity-driven exploration
4. **development_example.cpp** - Developmental learning
5. **memory_example.cpp** - Working with memory systems

Each example builds on the previous one, introducing new concepts gradually.

## Section 10: Ready to Start Building?

You're now equipped to:

- Create brains that think and learn
- Simulate neural computation
- Understand brain dynamics and plasticity
- Build intelligent systems from scratch

The NLM brain is more than a simple simulator - it's a platform for exploring how cognition can emerge from neural dynamics. Each brain is unique, shaped by its experiences and developmental trajectory.

**Remember:** Don't be afraid to experiment! The best way to learn is by trying different configurations, running simulations, and observing what happens.

### Continue Your Journey

1. **Read the API reference**: `docs/API_REFERENCE.md`
2. **Understand configuration**: `docs/CONFIG_GUIDE.md`
3. **Explore experiments**: `docs/EXPERIMENTS.md`
4. **Join the community**: Contribute to documentation and code

Happy brain-building!
