# NLM (Neural Learning Machine) - Tutorial

## Welcome to NLM!

This tutorial will guide you through the basics of using NLM to create and run neural simulations. By the end, you'll be able to create a complete agent that learns and interacts with its environment.

## Prerequisites

- Basic understanding of C++ programming
- Familiarity with neural networks concepts
- Installed NLM development environment

## Quick Start

### Step 1: Setting Up Your Project

Create a new directory for your NLM project:

```bash
mkdir nlm_project
cd nlm_project
cat > CMakeLists.txt << 'EOF'
cmake_minimum_required(VERSION 3.20)
project(NLMExample LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(nlm REQUIRED)

add_executable(example main.cpp)
target_link_libraries(example nlm::nlm_core)
EOF

cat > main.cpp << 'EOF'
#include <nlm>
int main() {
    // Your NLM code here
    return 0;
}
EOF

mkdir build
cd build
cmake ..
make
```

### Step 2: Create Your First Brain

Let's start with the simplest possible brain simulation:

```cpp
#include <nlm>

int main() {
    // Create a default configuration
    auto config = nlm::createDefaultConfig();
    
    // Create and initialize the brain
    auto brain = nlm::createBrain(config);
    brain->initialize();
    
    // Run a simple simulation
    for (int step = 0; step < 100; ++step) {
        brain->step(step);
        
        if (step % 10 == 0) {
            std::cout << "Step " << step 
                     << ": " << brain->getFiringNeuronCount() 
                     << " firing neurons" << std::endl;
        }
    }
    
    std::cout << "Simulation complete!" << std::endl;
    std::cout << "Total neurons: " << brain->getTotalNeuronCount() << std::endl;
    std::cout << "Total spikes: " << brain->getTotalSpikeCount() << std::endl;
    
    return 0;
}
```

### Step 3: Build and Run

```bash
# Build the project
mkdir build && cd build
cmake ..
make

# Run the simulation
./example
```

You should see output like:
```
Step 0: 0 firing neurons
Step 10: 5 firing neurons
Step 20: 8 firing neurons
...
Simulation complete!
Total neurons: 1000
Total spikes: 42
```

## Your First Agent

Now let's create a complete agent that interacts with a world:

### Step 1: Create the Agent

```cpp
#include <nlm>

void run_agent_example() {
    std::cout << "=== NLM Agent Example ===" << std::endl;
    
    // 1. Create and configure the brain
    auto config = nlm::createDefaultConfig();
    config->set("brain.neuron_count", 500);
    config->set("brain.region_count", 2);
    
    auto brain = nlm::createBrain(config);
    brain->initialize();
    
    // 2. Create the agent interface
    auto agent = nlm::createAgentBrain(brain);
    
    // 3. Create and configure the world
    auto world = nlm::createSimpleWorld();
    world->configure(20.0f, 20.0f, 16, 16);  // 20x20 world, 16x16 vision
    world->reset();
    
    // 4. Initialize the agent with the world
    agent->initialize(*world);
    
    // 5. Enable learning features
    agent->enableRewardModulation(true);
    agent->enableCuriosity(true);
    agent->enableDevelopment(true);
    
    // 6. Run the simulation
    std::cout << "Starting agent simulation..." << std::endl;
    
    for (int step = 0; step < 500; ++step) {
        // Update world state
        world->update(0.1);  // 100ms timestep
        
        // Get sensory input from world
        auto percept = world->getSensoryPercept();
        
        // Process sensory input through agent
        agent->processSensoryInput(percept);
        
        // Let the brain think
        brain->step(step);
        
        // Get action from agent (motor command)
        auto action = agent->decodeMotorCommand();
        
        // Execute action in the world
        world->applyMotorCommand(action, world->getSimulationTime());
        
        // Apply reward modulation (learning from experience)
        float reward = 0.0f;  // In real scenario, this comes from world feedback
        agent->applyRewardModulation(reward, 0.0f);
        
        // Update development stage
        agent->updateDevelopment(0.1);
        
        // Print status periodically
        if (step % 50 == 0) {
            std::cout << "Step " << step << ": "
                     << brain->getFiringNeuronCount() << " firing neurons, "
                     << "Curiosity: " << agent->getCuriosityLevel() << ", "
                     << "Novelty: " << agent->getNoveltyLevel() << ", "
                     << "Dev Stage: " << static_cast<int>(agent->getDevelopmentalStage()) << std::endl;
        }
    }
    
    std::cout << "=== Agent simulation complete ===" << std::endl;
    std::cout << "Final statistics:" << std::endl;
    std::cout << "  - Total neurons: " << brain->getTotalNeuronCount() << std::endl;
    std::endl
    << "  - Total spikes: " << brain->getTotalSpikeCount() << std::endl;
    << "  - Average firing rate: " << brain->getAverageFiringRate() << std::endl;
    << "  - E/I ratio: " << brain->getExcitationInhibitionRatio() << std::endl;
}
```

### Step 2: Save and Run

Save this as `example.cpp` and build it:

```bash
# Create the full project
cat > example.cpp << 'EOF'
// The complete code from above
EOF

cat > CMakeLists.txt << 'EOF'
cmake_minimum_required(VERSION 3.20)
project(NLMAgentExample LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(nlm REQUIRED)

add_executable(agent_example example.cpp)
target_link_libraries(agent_example nlm::nlm_core)
EOF

# Build and run
mkdir build && cd build
cmake ..
make
./agent_example
```

## Understanding the Agent Components

### 1. The Brain (`Brain` class)
- Contains neurons and synapses
- Processes information and generates activity
- Learns from experience through plasticity

### 2. The Agent (`AgentBrain` class)
- **Bridge** between brain and world
- **Processes** sensory input and sends it to brain
- **Decodes** brain activity into motor commands
- **Manages** learning and development

### 3. The World (`SimpleWorld` class)
- **Environment** where the agent lives
- **Provides** sensory information
- **Executes** actions
- **Gives** feedback (rewards)

## Key Concepts

### Neurons and Activity
```cpp
// The brain contains many neurons
std::cout << brain->getTotalNeuronCount() << " neurons total"; // e.g., 1000

// Some neurons are firing at any given time
std::cout << brain->getFiringNeuronCount() << " firing neurons"; // e.g., 42

// Total activity over simulation
std::cout << brain->getTotalSpikeCount() << " total spikes"; // e.g., 12345
```

### Memory Systems
```cpp
// Working memory - short-term storage
auto workingMemory = brain->getWorkingMemory();
workingMemory->store(neuron_id, value);
float retrieved = workingMemory->retrieve(neuron_id);

// Episodic memory - experiences and events
auto episodicMemory = brain->getEpisodicMemory();
size_t episodeCount = episodicMemory->getEpisodeCount();
```

### Neuromodulation
```cpp
// Dopamine - reward prediction error
auto dopamine = brain->getDopamine();
dopamine->signalReward(reward);
dopamine->signalRewardPredictionError(error);

// Curiosity - exploration drive
auto curiosity = brain->getCuriosity();
curiosity->update(dt);
```

### Development
```cpp
// Brain develops over time
auto development = brain->getDevelopmentSystem();
development->update(brain, rng, timestep);

// Different developmental stages
developmentStage = brain->getDevelopmentalStage();
// 0 = Initial, 1 = CriticalPeriod, 2 = Maturation, 3 = Adult, 4 = Aging
```

## Debugging Common Issues

### Issue 1: "Brain isn't doing anything"

**Problem**: Brain appears inactive, no neurons firing.

**Solution**:
```cpp
// Add debug output
brain->initialize();
brain->logStatus();  // Shows detailed brain state

// Check if simulation steps are being executed
for (int step = 0; step < 10; ++step) {
    brain->step(step);
    std::cout << "After step " << step << ": " 
             << brain->getFiringNeuronCount() << " firing neurons" << std::endl;
}
```

### Issue 2: "Agent isn't moving"

**Problem**: Agent doesn't produce motor commands.

**Solution**:
```cpp
// Check action selection
for (int step = 0; step < 100; ++step) {
    auto action = agent->decodeMotorCommand();
    std::cout << "Step " << step << ": Action = " 
             << static_cast<int>(action) << std::endl;
    
    if (action != MotorCommand::Wait) {
        std::cout << "Agent is taking action!" << std::endl;
        break;
    }
}
```

### Issue 3: "Everything is 0"

**Problem**: All values are zero, no activity.

**Solution**:
```cpp
// Brains need time to "warm up"
for (int step = 0; step < 500; ++step) {
    brain->step(step);
    
    if (step > 100 && brain->getFiringNeuronCount() > 0) {
        std::cout << "Brain is now active!" << std::endl;
        break;
    }
}

// Ensure neurons have connections
config.set("brain.synapse_density", 0.2f);  // Higher connectivity
config.set("brain.connection_probability", 0.05f);
```

## Advanced Tutorial Topics

### 1. Working with Checkpoints
```cpp
// Save simulation state
brain->save("my_checkpoint.bin");

// Resume from checkpoint
brain->load("my_checkpoint.bin");
```

### 2. Custom Configuration
```cpp
Config config;
config.set("brain.neuron_count", 2000);
config.set("plasticity.stdp.learning_rate", 0.005f);
config.set("neuromod.dopamine.scale", 2.0f);
```

### 3. Multiple Agents
```cpp
// Create multiple independent agents
auto agent1 = nlm::createAgentBrain(brain1);
auto agent2 = nlm::createAgentBrain(brain2);

// Each agent can have different configurations
agent1->enableCuriosity(true);
agent2->enableCuriosity(false);
```

### 4. Experiment Management
```cpp
struct ExperimentConfig {
    std::string name;
    Config config;
    size_t num_steps;
    std::string output_file;
};

void run_experiment(const ExperimentConfig& exp) {
    auto brain = nlm::createBrain(exp.config);
    brain->initialize();
    
    for (size_t step = 0; step < exp.num_steps; ++step) {
        brain->step(step);
    }
    
    // Save results
    brain->save(exp.output_file + "_checkpoint.bin");
}
```

## Practice Exercises

### Exercise 1: Silent Brain
Create a brain that does nothing interesting:

```cpp
definition
Create a brain and run it for 100 steps without any input
Expected: Some basic neural activity
```

### Exercise 2: World Explorer
Create an agent that explores the world:

```cpp
definition
1. Enable curiosity
2. Run simulation for 1000 steps
3. Count unique positions visited
4. Compare with exploration without curiosity
```

### Exercise 3: Memory System
Test the memory systems:

```cpp
definition
1. Store data in working memory
2. Retrieve and verify
3. Test episodic memory storage
4. Test semantic memory facts
```

### Exercise 4: Configuration
Experiment with different configurations:

```cpp
definition
1. Change neuron count
2. Change connection probability
3. Change learning rates
4. Measure impact on performance
```

## Next Steps

After completing this tutorial, you can:

1. **Read the API Reference** - `API_REFERENCE.md` for detailed documentation
2. **Explore Examples** - Look at existing example projects
3. **Learn Advanced Features** - Check `ADVANCED_GUIDE.md` for complex topics
4. **Join the Community** - Contribute to the NLM project
5. **Research Papers** - Study the scientific background in `docs/SCIENCE.md`

## Resources

- **GitHub Repository**: https://github.com/nlm-project/nlm
- **Documentation**: https://nlm.readthedocs.io
- **Tutorials**: Available in the `docs/` directory
- **API Reference**: Generated documentation available online

## Getting Help

If you encounter issues:

1. **Check the Documentation** - Look for your question in the API reference
2. **Review Examples** - Compare with working examples
3. **Check Logs** - Use `brain->logStatus()` for debugging
4. **Community Support** - GitHub issues and discussions
5. **Contact Authors** - For specific technical questions

## Conclusion

You now have a working NLM agent that can learn and interact with its environment! This is just the beginning - NLM offers many more advanced features for creating complex artificial brains.

Remember that artificial brains take time to develop and learn. Don't be discouraged if your first attempts seem simple - the complexity emerges through simulation time and experience.

Happy brain-building!

---

*For more detailed information, refer to the API reference and advanced guides in the NLM documentation.*
