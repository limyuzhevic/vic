# NLM Tutorial Documentation

## Overview

This section provides comprehensive tutorials for using the NLM (Neural Learning Machine) framework. The tutorials are organized by skill level and cover everything from basic setup to advanced usage.

## Tutorial Structure

The tutorials are organized as follows:

1. **Getting Started Tutorial** - Introduction for complete beginners
2. **Basic Tutorial** - Fundamental concepts and simple simulations
3. **Advanced Tutorial** - Complex scenarios and advanced features
4. **Example-Based Tutorials** - Real-world examples from each phase
5. **Hands-On Tutorials** - Practical exercises and projects

## Prerequisites

Before starting with NLM tutorials, ensure you have:

### Required Software

- **C++ Compiler**: GCC 10+, Clang 12+, or MSVC 2019+
- **CMake**: Version 3.16 or higher
- **Git**: For version control (optional)

### Recommended Experience

- Basic C++ programming knowledge
- Understanding of neural networks (optional)
- Interest in computational neuroscience (recommended)

## Quick Start (5-Minute Setup)

This tutorial will get you running your first NLM simulation in just 5 minutes.

### Step 1: Installation

```bash
# Clone the NLM repository
git clone https://github.com/your-org/nlm.git
cd nlm

# Build the project
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4
```

### Step 2: Run Phase 2 Demo

```bash
# Run the basic neural computation demo
./nlm

# Or run the Phase 6 integration demo (if available)
./nlm_phase6_demo
```

### Step 3: Explore

The demos will output information about:
- Neural network initialization
- Spike propagation
- Plasticity learning
- Network behavior

## Getting Started Tutorial

### Lesson 1: Understanding NLM Components

NLM is built from several interconnected components:

#### 1.1 Core Components

| Component | Purpose | Key Methods |
|-----------|---------|-------------|
| **Brain** | Central neural coordinator | `step()`, `receiveSensoryInput()` |
| **Neuron** | Individual computational units | `injectCurrent()`, `getMembranePotential()` |
| **Synapse** | Connections between neurons | `getWeight()`, `enablePlasticity()` |
| **World** | Environment simulator | `update()`, `getSensoryPercept()` |
| **Agent** | Bridge between brain and world | `processSensoryInput()`, `decodeMotorCommand()` |

#### 1.2 Understanding the Workflow

```
World (Environment)
  ↓ (sensory input)
SensoryPercept → Agent.processSensoryInput() → Brain.receiveSensoryInput()
  ↓ (neural computation)
Brain.step() → Neuron.SpikeSystem → Synapse.Plasticity
  ↓ (motor output)
Brain.produceAction() → Agent.decodeMotorCommand() → World.applyMotorCommand()
  ↓ (reward)
Neuromodulation → Plasticity Updates → Learning
```

### Lesson 2: Basic NLM Program Structure

Every NLM program follows this basic structure:

```cpp
#include <nlm/nlm.hpp>

int main() {
    // 1. Create configuration
    auto config = nlm::createDefaultConfig();
    
    // 2. Create brain
    auto brain = nlm::createBrain(config);
    
    // 3. Initialize brain
    brain->initialize();
    
    // 4. Run simulation
    for (size_t step = 0; step < 100; ++step) {
        brain->step(step);
    }
    
    // 5. Analyze results
    std::cout << "Total spikes: " << brain->getTotalSpikeCount() << std::endl;
    
    return 0;
}
```

### Lesson 3: First Simulation

Let's create a simple simulation that demonstrates basic neural computation.

#### Exercise 1: Silent Brain

Create a program that runs a "silent" brain with no external input:

**Expected Output**:
- Brain initialization information
- Network status after 10 steps
- Final statistics

**Hints**:
1. Use `createDefaultConfig()` for basic configuration
2. Run at least 10 simulation steps
3. Check that the brain is actually computing

#### Exercise 2: Brain with Input

Create a program that injects current into neurons and observes the results:

**Requirements**:
1. Create brain with at least 100 neurons
2. Inject current into a few neurons
3. Run simulation for 50 steps
4. Record and display spike activity

## Basic Tutorial

### Module 1: Configuration Management

#### 1.1 Configuration System Overview

NLM uses a flexible configuration system for controlling all aspects of simulation:

```cpp
// Configuration key-value pairs
brain.neuron_count = 1000
brain.region_count = 1
brain.connection_probability = 0.1
plasticity.stdp.enable = true
neuromod.dopamine.scale = 1.0
```

#### 1.2 Loading Configuration

```cpp
// Method 1: Create and modify config
auto config = nlm::createDefaultConfig();
config.set("brain.neuron_count", 2000);
config.set("plasticity.stdp.learning_rate", 0.005);

// Method 2: Load from file
config.loadFromFile("my_config.json");

// Method 3: Command line arguments
config.loadFromArgs(argc, argv);
```

#### 1.3 Configuration Templates

```cpp
// Different brain types for different use cases
nlm::BrainConfig basic = nlm::BrainConfig::createDefault();    // Simple brain
nlm::BrainConfig learning = nlm::BrainConfig::createLearning(); // Optimized for learning
nlm::BrainConfig development = nlm::BrainConfig::createDevelopment(); // Developing brain
nlm::BrainConfig optimized = nlm::BrainConfig::createOptimized(500); // Performance-optimized
```

### Module 2: Creating and Running a Brain

#### 2.1 Brain Creation Methods

```cpp
// Method 1: From configuration
auto brain = nlm::createBrain(config);

// Method 2: From BrainConfig object
auto brain = nlm::createBrain(BrainConfig::createLearning());

// Method 3: From file
auto brain = nlm::createBrain("brain_config.json");

// Method 4: From configuration map
std::unordered_map<std::string, nlm::ConfigValue> configMap = {
    {"brain.neuron_count", 1000},
    {"plasticity.stdp.enable", true}
};
auto brain = nlm::createBrain(configMap);
```

#### 2.2 Initialization Process

```cpp
// Basic initialization
brain->initialize();

// Initialization with validation
nlm::ValidationResult validation;
brain->initializeWithValidation(config, validation);
if (!validation.valid) {
    for (const auto& error : validation.errors) {
        std::cout << "ERROR: " << error.message << std::endl;
    }
}
```

#### 2.3 Simulation Loop

```cpp
// Simple simulation loop
for (size_t step = 0; step < maxSteps; ++step) {
    brain->step(step);
    
    // Optional: Inject input
    if (step % 100 == 0) {
        brain->injectCurrentToNeurons(nlm::NeuronType::Sensory, 50.0f);
    }
    
    // Optional: Log statistics
    if (step % 500 == 0) {
        brain->logStatus();
    }
}
```

### Module 3: Basic Neural Operations

#### 3.1 Sensory Processing

```cpp
// Create sensory input
nlm::SensoryInput input;
input.type = nlm::SensoryInputType::Vision;
input.data = {0.5f, 0.8f, 0.3f, 0.9f}; // Example visual data

// Process input through agent
agent->processSensoryInput(input);

// Brain receives the input
brain->receiveSensoryInput(input);
```

#### 3.2 Motor Output

```cpp
// Get action from brain
auto action = brain->produceAction();

// Decode action through agent
nlm::MotorCommand cmd = agent->decodeMotorCommand(action);

// Apply action to world
world->applyMotorCommand(cmd, world->getSimulationTime());
```

#### 3.3 Current Injection

```cpp
// Inject current into specific neurons
brain->injectCurrent(neuronId, 25.0f);

// Inject to all neurons of a type
brain->injectCurrentToNeurons(nlm::NeuronType::Sensory, 30.0f);
brain->injectCurrentToNeurons(nlm::NeuronType::Excitatory, 10.0f);
```

### Module 4: Working with Memory

#### 4.1 Memory System Overview

NLM has three main memory systems:

- **Working Memory**: Transient active information
- **Episodic Memory**: Experience storage
- **Associative Memory**: Pattern associations

#### 4.2 Accessing Memory Systems

```cpp
// Get memory system pointers
auto* workingMem = brain->getWorkingMemory();
auto* episodicMem = brain->getEpisodicMemory();
auto* associativeMem = brain->getAssociativeMemory();

// Working memory operations
workingMem->update(inputPattern);
auto activePattern = workingMem->getActivePattern();

// Episodic memory operations
auto episodeId = episodicMem->storeExperience(experienceRecord);
auto episode = episodicMem->retrieveExperience(episodeId);

// Associative memory operations
associativeMem->learnAssociation(patternPair);
auto recall = associativeMem->recallPattern(cuePattern);
```

### Module 5: Understanding Plasticity

#### 5.1 Plasticity Mechanisms

NLM implements multiple plasticity mechanisms:

1. **STDP (Spike-Timing-Dependent Plasticity)**
2. **Hebbian Learning** ("fire together, wire together")
3. **Reward-Modulated Plasticity**
4. **Structural Plasticity** (synaptogenesis/pruning)

#### 5.2 Enabling Plasticity

```cpp
// Enable all plasticity mechanisms
brain->enableLearning(true);

// Enable specific mechanisms
auto* stdp = brain->getSTDP();
auto* hebbian = brain->getHebbian();
auto* structural = brain->getStructuralPlasticity();

// Configure plasticity parameters
stdp->setLearningRate(0.01f);
structural->setSynaptogenesisRate(0.001f);
structural->setPruningRate(0.0001f);
```

## Advanced Tutorial

### Module 1: Integration with World

#### 1.1 Creating a Complete Agent-World System

```cpp
int main() {
    // 1. Create brain and agent
    auto brain = nlm::createLearningBrain();
    auto agent = nlm::createAgentBrain(brain);
    
    // 2. Create and configure world
    auto world = nlm::createSimpleWorld();
    world->configure(20, 20, 8, 8); // width, height, visionWidth, visionHeight
    world->reset();
    
    // 3. Initialize agent with world
    agent->initialize(world);
    
    // 4. Enable learning systems
    agent->enableRewardModulation(true);
    agent->enableCuriosity(true);
    agent->enableDevelopment(true);
    
    // 5. Run complete agent simulation
    for (size_t step = 0; step < 1000; ++step) {
        // Update world
        world->update(0.1);
        
        // Get sensory input from world
        auto percept = world->getSensoryPercept();
        
        // Process sensory input in brain
        agent->processSensoryInput(percept);
        
        // Run brain step
        brain->step(step);
        
        // Decode motor command from brain activity
        motor_cmd = agent->decodeMotorCommand();
        
        // Apply motor command to world
        world->applyMotorCommand(motor_cmd, world->getSimulationTime());
        
        // Apply reward modulation
        reward = world->getSensoryPercept().getInternal()[0] if world->getSensoryPercept().getInternal() else 0.0
        agent->applyRewardModulation(reward, 0.0)
        
        // Update development
        agent->updateDevelopment(0.1)
        
        // Print progress
        if (step % 100 == 0) {
            print(f"Step {step}:")
            print(f"  Firing neurons: {brain->getFiringNeuronCount()}")
            print(f"  Curiosity: {agent->getCuriosityLevel():.3f}")
            print(f"  Novelty: {agent->getNoveltyLevel():.3f}")
            print(f"  Dev Stage: {brain->getDevelopmentalStage()}")
        }
    }
    
    return 0;
}
```

#### 1.2 World Configuration Options

```cpp
world->configure(
    width=30,
    height=30,
    visionWidth=8,
    visionHeight=8,
    maxEnergy=100.0,
    timestep=0.1
);
```

### Module 2: Advanced Neural Operations

#### 2.1 Complex Input Patterns

```cpp
// Create complex sensory patterns
std::vector<float> visualPattern(64, 0.0f);
// Create a simple shape (e.g., a cross)
for (int i = 0; i < 8; ++i) {
    visualPattern[i * 8 + 3] = 1.0f;  // Vertical line
    visualPattern[3 * 8 + i] = 1.0f;  // Horizontal line
}

// Create internal signals
std::vector<float> internalPattern(16, 0.0f);
internalPattern[0] = 0.5f;  // Hunger level
internalPattern[1] = 0.8f;  // Energy level
internalPattern[2] = 0.2f;  // Exploration drive

// Combine into sensory input
nlm::SensoryInput input;
input.type = nlm::SensoryInputType::Combined;
input.visual = visualPattern;
input.internal = internalPattern;
```

#### 2.2 Advanced Plasticity Control

```cpp
// Configure STDP parameters
auto* stdp = brain->getSTDP();
stdp->enable(true);
stdp->setLearningRate(0.005f);
stdp->setTimeConstants(20.0f, 20.0f); // tau_plus, tau_minus

// Configure Hebbian learning
auto* hebbian = brain->getHebbian();
hebbian->enable(true);
hebbian->setLearningRate(0.01f);

// Configure structural plasticity
auto* structural = brain->getStructuralPlasticity();
structural->enable(true);
structural->setSynaptogenesisRate(0.0002f);
structural->setPruningRate(0.00001f);
structural->setGrowthThreshold(0.1f);
structural->setEliminationThreshold(-0.1f);
```

### Module 3: Neuromodulation and Learning

#### 3.1 Understanding Neuromodulation

Neuromodulation controls learning and behavior through several systems:

1. **Dopamine**: Reward prediction error
2. **Novelty Detection**: Novelty detection
3. **Curiosity**: Exploration motivation
4. **Prediction Error**: Error signals

#### 3.2 Neuromodulation Control

```cpp
// Enable neuromodulation
auto* neuromod = brain->getNeuromodulationManager();
neuromod->enableNeuromodulation(true);

// Process reward signals
neuromod->processReward(rewardValue);

// Process prediction error
neuromod->processPredictionError(predictionError);

// Get neuromodulation statistics
auto stats = neuromod->getStats();
std::cout << "Dopamine level: " << stats.dopamineLevel << std::endl;
std::cout << "Curiosity level: " << stats.curiosityLevel << std::endl;
std::cout << "Novelty level: " << stats.noveltyLevel << std::endl;
```

### Module 4: Development and Aging

#### 4.1 Developmental Stages

NLM simulates brain development through several stages:

1. **Initial**: High plasticity, basic connectivity
2. **CriticalPeriod**: Sensitive period for learning
3. **Maturation**: Development of adult properties
4. **Adult**: Stable performance
5. **Aging**: Gradual decline in plasticity

#### 4.2 Development Control

```cpp
// Enable development
brain->enableDevelopment(true);

// Get current developmental stage
auto stage = brain->getDevelopmentalStage();
std::cout << "Development stage: " << static_cast<int>(stage) << std::endl;

// Update development based on time
brain->develop();
```

### Module 5: Performance Monitoring

#### 5.1 Real-Time Monitoring

```cpp
// Start monitoring
brain->startMonitoring(std::make_unique<nlm::ConsoleMonitor>());

// Run simulation
brain->runSimulation(inputs);

// Stop monitoring
brain->stopMonitoring();
```

#### 5.2 Checkpointing and Persistence

```cpp
// Save brain state
brain->saveBrain("checkpoint1.nml");

// Load brain state
brain->loadBrain("checkpoint1.nml");

// Configure checkpoint options
nlm::CheckpointOptions options;
options.compress = true;
options.validate = true;

brain->saveBrain("checkpoint2.nml", options);
```

## Example-Based Tutorials

### Phase 2: Basic Neural Computation

#### Example 1: LIF Neuron Dynamics

```cpp
// Create brain with LIF neurons
auto config = nlm::createDefaultConfig();
config.set("brain.v_thresh", -50.0f);
config.set("brain.v_rest", -70.0f);
config.set("brain.v_reset", -75.0f);
config.set("brain.tau_mem", 20.0f);

auto brain = nlm::createBrain(config);
brain->initialize();

// Test neuron dynamics
for (size_t step = 0; step < 100; ++step) {
    brain->injectCurrentToNeurons(nlm::NeuronType::Excitatory, 10.0f);
    brain->step(step);
    
    if (step % 20 == 0) {
        auto* spikeSys = brain->getSpikeSystem();
        std::cout << "Step " << step << ": " 
                 << spikeSys->getSpikeCount() << " spikes" << std::endl;
    }
}
```

#### Example 2: STDP Learning

```cpp
// Enable STDP
auto* stdp = brain->getSTDP();
stdp->enable(true);
stdp->setLearningRate(0.01f);

// Run STDP experiment
// 1. Create correlated pre-post activity
// 2. Measure weight changes
// 3. Verify LTP/LTD
```

### Phase 3: World Interaction

#### Example 1: Simple Agent Navigation

```cpp
// Create agent and world
auto brain = nlm::createLearningBrain();
auto agent = nlm::createAgentBrain(brain);
auto world = nlm::createSimpleWorld();
world->configure(50, 50, 8, 8);
world->reset();
agent->initialize(world);

// Run navigation episode
for (size_t step = 0; step < 500; ++step) {
    world->update(0.1);
    
    auto percept = world->getSensoryPercept();
    agent->processSensoryInput(percept);
    
    brain->step(step);
    
    auto action = agent->decodeMotorCommand();
    world->applyMotorCommand(action, world->getSimulationTime());
    
    // Check for termination conditions
    if (world->getAgentPosition().x > 90 && 
        world->getAgentPosition().y > 90) {
        break; // Reached goal
    }
}
```

### Phase 4: Emerging Cognition

#### Example 1: Prediction System

```cpp
// Enable prediction system
auto* prediction = brain->getPredictionSystem();

// Learn temporal sequences
for (size_t step = 0; step < 1000; ++step) {
    brain->step(step);
    
    // Generate prediction error
    auto error = prediction->getPredictionError();
    
    // Apply prediction error to neuromodulation
    auto* neuromod = brain->getNeuromodulationManager();
    neuromod->processPredictionError(error);
}
```

## Hands-On Tutorials

### Exercise 1: Brain Warm-Up

**Goal**: Understand how brains need time to "warm up" and start firing.

**Instructions**:
1. Create a brain with 100 neurons
2. Run 10 simulation steps with no input
3. Check if any neurons fire
4. Inject input and observe changes
5. Document the results

**Expected Learning**: Brains need time to establish baseline activity, and input is needed to trigger spikes.

### Exercise 2: Plasticity Experiment

**Goal**: Observe synaptic changes through experience.

**Instructions**:
1. Create brain with plasticity enabled
2. Record initial synaptic weights
3. Apply repeated input patterns
4. Record final synaptic weights
5. Calculate weight changes
6. Verify STDP is working

**Expected Learning**: Synaptic weights change based on activity patterns, demonstrating Hebbian and STDP learning.

### Exercise 3: Memory Integration

**Goal**: Test integration between neural processing and memory systems.

**Instructions**:
1. Create brain with all three memory systems
2. Generate input patterns
3. Store patterns in episodic memory
4. Use working memory to maintain patterns
5. Use associative memory to recall related patterns
6. Measure memory performance

**Expected Learning**: Memory systems work together to support learning and behavior.

## Tutorial Summary

This tutorial documentation provides comprehensive coverage of NLM usage:

1. **Getting Started**: Basic setup and first simulation
2. **Fundamental Concepts**: Configuration, brain creation, basic operations
3. **Intermediate Skills**: World interaction, sensory processing, motor output
4. **Advanced Features**: Neuromodulation, development, performance monitoring
5. **Real-World Examples**: Phase-specific applications
6. **Hands-On Practice**: Exercises to reinforce learning

Each tutorial builds on previous concepts, providing a structured learning path for users of all skill levels.