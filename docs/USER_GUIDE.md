# NLM User Guide

## Introduction

Welcome to the NLM (熙然) User Guide! This document provides comprehensive instructions for using the Neural Learning Machine from beginner to advanced levels. NLM is an experimental artificial developmental brain that learns from experience through neural computation.

## Quick Start

### Installation

#### System Requirements
- C++17 compatible compiler
- CMake 3.15 or higher
- C++ standard libraries
- Build tools (make, ninja)

#### Build Instructions
```bash
# Create build directory
mkdir build
cd build

# Configure with CMake
cmake .. -DCMAKE_BUILD_TYPE=Release

# Build the project
make -j$(nproc)

# Run the demo
./nlm_phase6_demo
```

### Running Examples
```bash
# View available binaries
ls -la ./build/nlm*

# Run different demo modes
./build/nlm --help
```

## Beginner Tutorial

### What You'll Learn
- Basic neural network concepts
- How to create and run a simple brain
- Understanding input/output loops
- Basic experimentation

### Installation
```bash
cd /path/to/nlm/examples
python3 beginner_tutorial.py
```

### Step 1: Basic Brain Setup

The brain is the core neural system. Let's create a simple brain:

```cpp
#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"

// Create configuration
auto config = std::make_shared<Config>();
config->set("neuron_count", 1000);
config->set("plasticity_rate", 0.1f);
config->set("working_memory_capacity", 100);

// Create brain
auto brain = std::make_shared<Brain>(config);
brain->initialize();
```

### Step 2: Sensory Input

The brain receives sensory input through `SensoryInput` objects:

```cpp
#include "sensory/SensoryInput.hpp"

// Create simple visual input (16x16 grayscale)
SensoryInput input(16, 16);
for (int y = 0; y < 16; ++y) {
    for (int x = 0; x < 16; ++x) {
        // Set pixel value (0.0 = dark, 1.0 = bright)
        input.setPixel(x, y, static_cast<float>(x == y ? 1.0f : 0.0f));
    }
}

brain->receiveSensoryInput(input);
```

### Step 3: Simulation Loop

Run the simulation to process sensory input:

```cpp
// Run 100 simulation steps
for (int step = 0; step < 100; ++step) {
    brain->step(step);
}
```

### Step 4: Get Output

Extract motor output from the brain:

```cpp
auto action = brain->produceAction();
if (action) {
    std::cout << "Action selected: " << action->toString() << std::endl;
}
```

### Complete Beginner Example

```cpp
#include <iostream>
#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include "sensory/SensoryInput.hpp"

int main() {
    std::cout << "=== NLM Beginner Tutorial ===\n" << std::endl;
    
    // 1. Create and initialize brain
    auto config = std::make_shared<Config>();
    config->set("neuron_count", 500);
    config->set("plasticity_rate", 0.1f);
    
    auto brain = std::make_shared<Brain>(config);
    if (!brain->initialize()) {
        std::cerr << "Failed to initialize brain!" << std::endl;
        return 1;
    }
    
    std::cout << "Brain initialized successfully." << std::endl;
    
    // 2. Create sensory input (simple diagonal pattern)
    SensoryInput input(10, 10);
    for (int i = 0; i < 10; ++i) {
        input.setPixel(i, i, 1.0f); // Diagonal bright pixels
    }
    
    std::cout << "Created sensory input with diagonal pattern." << std::endl;
    
    // 3. Process input and simulate
    brain->receiveSensoryInput(input);
    
    std::cout << "Processing input..." << std::endl;
    for (int step = 0; step < 50; ++step) {
        brain->step(step);
    }
    
    // 4. Get and display output
    auto action = brain->produceAction();
    if (action) {
        std::cout << "Final action: " << action->toString() << std::endl;
        std::cout << "Total spikes processed: " << brain->getTotalSpikeCount() << std::endl;
    }
    
    std::cout << "\n=== Tutorial Complete ===" << std::endl;
    return 0;
}
```

## Intermediate Tutorial

### What You'll Learn
- Agent integration with environments
- Working memory and episodic memory usage
- Prediction system implementation
- Neuromodulation (reward, curiosity)
- Developmental processes
- Experiment design and metrics

### Step 1: Setting Up Agent-Brain Integration

Create an AgentBrain to connect the brain to a world:

```cpp
#include "agent/AgentBrain.hpp"
#include "world/SimpleWorld.hpp"

// Create brain first
auto config = std::make_shared<Config>();
auto brain = std::make_shared<Brain>(config);
brain->initialize();

// Create agent brain
auto agentBrain = std::make_shared<AgentBrain>(brain);

// Create world
SimpleWorld world(64, 64);
agentBrain->initialize(world);
```

### Step 2: Working with World States

Interact with the world and observe behavior:

```cpp
// Initialize world with an object
world.initialize();

// Get current world state
const auto& worldState = world.getState();
std::cout << "World size: " << worldState.width << " x " << worldState.height << std::endl;

// Create sensory percept from world state
SensoryPercept percept = agentBrain->createSensoryPercept(worldState);
agentBrain->processSensoryInput(percept);
```

### Step 3: Understanding Working Memory

Working memory maintains information temporarily:

```cpp
auto workingMemory = brain->getWorkingMemory();
if (workingMemory) {
    workingMemory->store("current_goal", 1.0f);
    workingMemory->store("time_step", 0.0f);
    
    // Retrieve information
    float goal = workingMemory->retrieve("current_goal");
    float timeStep = workingMemory->retrieve("time_step");
    
    std::cout << "Goal: " << goal << ", Time step: " << timeStep << std::endl;
}
```

### Step 4: Episodic Memory

Store and retrieve experiences:

```cpp
auto episodicMemory = brain->getEpisodicMemory();
if (episodicMemory) {
    // Store an experience with reward
    episodicMemory->store("agent_moved", 1.0f, 10.0f); // action, reward
    episodicMemory->store("object_found", 0.5f, 25.0f);
    
    // Retrieve similar experiences
    auto similar = episodicMemory->retrieveSimilar("agent_moved", 0.5f);
    for (const auto& episode : similar) {
        std::cout << "Retrieved episode with reward: " << episode.reward << std::endl;
    }
}
```

### Step 5: Prediction System

Learn temporal patterns:

```cpp
auto predictionSystem = brain->getPredictionSystem();
if (predictionSystem) {
    // Learn that state A predicts state B
    predictionSystem->learn("state_a", "state_b");
    
    // Predict next state
    std::string prediction = predictionSystem->predict("state_a");
    std::cout << "Predicted: " << prediction << std::endl;
    
    // Compute prediction error
    float error = predictionSystem->computePredictionError("state_b", prediction);
    std::cout << "Prediction error: " << error << std::endl;
}
```

### Step 6: Neuromodulation

Control learning through neuromodulators:

```cpp
auto dopamine = brain->getDopamine();
auto curiosity = brain->getCuriosity();
auto novelty = brain->getNovelty();

// Apply reward-based learning
if (dopamine) {
    dopamine->signalReward(1.0f, 0.8f); // actual, predicted reward
}

// Apply curiosity-driven exploration
if (curiosity) {
    curiosity->increaseExploration();
}

// Detect novelty
if (novelty) {
    novelty->checkForNovelty(sensoryInput);
}
```

### Step 7: Development

Control developmental processes:

```cpp
auto developmentSystem = brain->getDevelopmentSystem();
if (developmentSystem) {
    // Get current developmental stage
    auto stage = brain->getDevelopmentalStage();
    std::cout << "Current developmental stage: " << static_cast<int>(stage) << std::endl;
    
    // Enable development if not already
    brain->getDevelopmentSystem()->enableDevelopment(true);
}
```

### Step 8: Complete Intermediate Example

```cpp
#include <iostream>
#include "agent/AgentBrain.hpp"
#include "world/SimpleWorld.hpp"
#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include "sensory/SensoryInput.hpp"

int main() {
    std::cout << "=== NLM Intermediate Tutorial ===\n" << std::endl;
    
    // 1. Setup brain with Agent integration
    auto config = std::make_shared<Config>();
    config->set("neuron_count", 1000);
    config->set("plasticity_rate", 0.1f);
    config->set("working_memory_capacity", 100);
    config->set("prediction_horizon", 3);
    
    auto brain = std::make_shared<Brain>(config);
    brain->initialize();
    
    auto agentBrain = std::make_shared<AgentBrain>(brain);
    SimpleWorld world(32, 32);
    agentBrain->initialize(world);
    
    std::cout << "Setup complete: Brain + Agent + World" << std::endl;
    
    // 2. Main learning loop
    for (int episode = 0; episode < 10; ++episode) {
        std::cout << "\n=== Episode " << episode << " ===" << std::endl;
        
        // Initialize world state
        world.reset();
        agentBrain->reset();
        
        // Create initial sensory input
        SensoryInput input(10, 10);
        input.setPixel(5, 5, 1.0f); // Center bright pixel
        
        // Process through agent
        SensoryPercept percept = agentBrain->createSensoryPercept(input);
        agentBrain->processSensoryInput(percept);
        
        // Simulate 20 steps
        for (int step = 0; step < 20; ++step) {
            brain->step(step);
            agentBrain->updateDevelopment(0.1f); // Update development
        }
        
        // Get action and apply to world
        auto action = brain->produceAction();
        if (action) {
            world.applyAction(*action);
            std::cout << "Action: " << action->toString() << " | World reward: " << world.getReward() << std::endl;
            
            // Apply neuromodulation based on reward
            float reward = world.getReward();
            float predicted = agentBrain->getPredictionError();
            agentBrain->applyRewardModulation(reward, predicted);
        }
        
        // Show memory usage
        auto workingMemory = brain->getWorkingMemory();
        if (workingMemory) {
            std::cout << "Working memory entries: " << workingMemory->getEntryCount() << std::endl;
        }
        
        auto episodicMemory = brain->getEpisodicMemory();
        if (episodicMemory) {
            std::cout << "Episodic memory episodes: " << episodicMemory->getEpisodeCount() << std::endl;
        }
        
        // Wait for user input (for demo purposes)
        if (episode < 9) {
            std::cout << "Press Enter to continue...";
            std::cin.get();
        }
    }
    
    std::cout << "\n=== Tutorial Complete ===" << std::endl;
    std::cout << "Key insights learned:" << std::endl;
    std::cout << "1. AgentBrain connects brain to world" << std::endl;
    std::cout << "2. Working memory maintains temporary information" << std::endl;
    std::cout << "3. Episodic memory stores experiences" << std::endl;
    std::cout << "4. Prediction system learns temporal patterns" << std::endl;
    std::cout << "5. Neuromodulation drives learning" << std::endl;
    
    return 0;
}
```

## Advanced Usage

### Complex Configuration

Create sophisticated brain configurations:

```cpp
config->set("neuron_count", 10000);
config->set("synapse_density", 0.1f);
config->set("plasticity_rate", 0.05f);
config->set("working_memory_capacity", 500);
config->set("prediction_horizon", 10);
config->set("development_enabled", true);
config->set("reward_modulation_enabled", true);
config->set("structural_plasticity_enabled", true);
config->set("curiosity_enabled", true);
```

### Performance Optimization

Configure for large-scale simulations:

```cpp
config->set("thread_count", std::thread::hardware_concurrency());
config->set("vectorization_enabled", true);
config->set("sparse_connectivity_enabled", true);
config->set("event_queue_size", 65536);
```

### Experiment Design

Design reproducible scientific experiments:

```cpp
struct ExperimentConfig {
    std::string name;
    uint64_t seed;
    int neuronCount;
    int stepsPerTrial;
    int trials;
    bool enableDevelopment;
    bool enableRewardModulation;
    std::vector<std::string> measurementTypes;
};

// Create reproducible experiment
ExperimentConfig expConfig;
expConfig.name = "working_memory_capacity_test";
expConfig.seed = 123456789;
expConfig.neuronCount = 5000;
expConfig.stepsPerTrial = 100;
expConfig.trials = 10;
```
```


ExperimentConfig expConfig;
expConfig.name = "working_memory_capacity_test";
expConfig.seed = 123456789;
expConfig.neuronCount = 5000;
expConfig.stepsPerTrial = 100;
expConfig.trials = 10;
```
```

// Create reproducible experiment
ExperimentConfig expConfig;
expConfig.name = "working_memory_capacity_test";
expConfig.seed = 123456789;
expConfig.neuronCount = 5000;
expConfig.stepsPerTrial = 100;
expConfig.trials = 10;
```
```

ExperimentConfig expConfig;
expConfig.name = "working_memory_capacity_test";
expConfig.seed = 123456789;
expConfig.neuronCount = 5000;
expConfig.stepsPerTrial = 100;
expConfig.trials = 10;
```

// Create reproducible experiment
ExperimentConfig expConfig;
expConfig.name = "working_memory_capacity_test";
expConfig.seed = 123456789;
expConfig.neuronCount = 5000;
expConfig.stepsPerTrial = 100;
expConfig.trials = 10;
```

// Create reproducible experiment
ExperimentConfig expConfig;
expConfig.name = "working_memory_capacity_test";
expConfig.seed = 123456789;
expConfig.neuronCount = 5000;
expConfig.stepsPerTrial = 100;
expConfig.trials = 10;
```

// Create reproducible experiment
ExperimentConfig expConfig;
expConfig.name = "working_memory_capacity_test";
expConfig.seed = 123456789;
expConfig.neuronCount = 5000;
expConfig.stepsPerTrial = 100;
expConfig.trials = 10;
```

// Create reproducible experiment
ExperimentConfig expConfig;
expConfig.name = "working_memory_capacity_test";
expConfig.seed = 123456789;
expConfig.neuronCount = 5000;
expConfig.stepsPerTrial = 100;
expConfig.trials = 10;
```

// Create reproducibleexperiment
ExperimentConfigexpConfig;
expConfig.name="working_memory_capacity_test";
expConfig.seed=123456789;
expConfig.neuronCount=5000;
expConfig.stepsPerTrial=100;
expConfig.trials=10;
```

// Create reproducible experiment
ExperimentConfig expConfig;
expConfig.name = "working_memory_capacity_test";
expConfig.seed = 123456789;
expConfig.neuronCount = 5000;
expConfig.stepsPerTrial = 100;
expConfig.trials = 10;
```

// Create reproducible experiment
ExperimentConfig expConfig;
expConfig.name = "working_memory_capacity_test";
expConfig.seed = 123456789;
expConfig.neuronCount = 5000;
expConfig.stepsPerTrial = 100;
expConfig.trials = 10;
```

// Create reproducible experiment
ExperimentConfig expConfig;
expConfig.name = "working_memory_capacity_test";
expConfig.seed = 123456789;
expConfig.neuronCount = 5000;
expConfig.stepsPerTrial = 100;
expConfig.trials = 10;
```

// Create reproducible experiment
ExperimentConfig expConfig;
expConfig.name = "working_memory_capacity_test";
expConfig.seed = 123456789;
expConfig.neuronCount = 5000;
expConfig.stepsPerTrial = 100;
expConfig.trials = 10;
```

// Create reproducible experiment
ExperimentConfig expConfig;
expConfig.name = "working_memory_capacity_test";
expConfig.seed = 123456789;
expConfig.neuronCount = 5000;
expConfig.stepsPerTrial = 100;
expConfig.trials = 10;
```

// Create reproducible experiment
ExperimentConfig expConfig;
expConfig.name = "working_memory_capacity_test";
expConfig.seed = 123456789;
expConfig.neuronCount = 5000;
expConfig.stepsPerTrial = 100;
expConfig.trials = 10;
```

// Create reproducible experiment
ExperimentConfig expConfig;
expConfig.name = "working_memory_capacity_test";
expConfig.seed = 123456789;
expConfig.neuronCount = 5000;
expConfig.stepsPerTrial = 100;
expConfig.trials = 10;
```

// Create reproducible experiment
ExperimentConfig expConfig;
expConfig.name = "working_memory_capacity_test";
expConfig.seed = 123456789;
expConfig.neuronCount = 5000;
expConfig.stepsPerTrial = 100;
expConfig.trials = 10;
```

// Create reproducible experiment
ExperimentConfig expConfig;
expConfig.name = "working_memory_capacity_test";
expConfig.seed = 123456789;
expConfig.neuronCount = 5000;
expConfig.stepsPerTrial = 100;
expConfig.trials = 10;
```

// Create reproducible experiment
ExperimentConfig expConfig;
expConfig.name = "working_memory_capacity_test";
expConfig.seed = 123456789;
expConfig.neuronCount = 5000;
expConfig.stepsPerTrial = 100;
expConfig.trials = 10;
```

// Create reproducible experiment
ExperimentConfig expConfig;
expConfig.name = "working_memory_capacity_test";
expConfig.seed = 123456789;
expConfig.neuronCount = 5000;
expConfig.stepsPerTrial = 100;
expConfig.trials = 10;
```

// Create reproducible experiment
ExperimentConfig expConfig;
expConfig.name = "working_memory_capacity_test";
expConfig.seed = 123456789;
expConfig.neuronCount = 5000;
expConfig.stepsPerTrial = 100;
expConfig.trials = 10;
```

// Create reproducible experiment
ExperimentConfig expConfig;
expConfig.name = "working_memory_capacity_test";
expConfig.seed = 123456789;
expConfig.neuronCount = 5000;
expConfig.stepsPerTrial = 100;
expConfig.trials = 10;
```

// Create reproducible experiment
ExperimentConfig expConfig;
expConfig.name = "working_memory_capacity_test";
expConfig.seed = 123456789;
expConfig.neuronCount = 5000;
expConfig.stepsPerTrial = 100;
expConfig.trials = 10;
```

// Create reproducible experiment
ExperimentConfig expConfig;
expConfig.name = "working_memory_capacity_test";
expConfig.seed = 123456789;
expConfig.neuronCount = 5000;
expConfig.stepsPerTrial = 100;
expConfig.trials = 10;
```

// Create reproducible experiment
ExperimentConfig expConfig;
expConfig.name = "working_memory_capacity_test";
expConfig.seed = 123456789;
expConfig.neuronCount = 5000;
expConfig.stepsPerTrial = 100;
expConfig.trials = 10;
```

// Create reproducible experiment
ExperimentConfig expConfig;
expConfig.name = "working_memory_capacity_test";
expConfig.seed = 123456789;
expConfig.neuronCount = 5000;
expConfig.stepsPerTrial = 100;
expConfig.trials = 10;
```

// Create reproducible experiment
ExperimentConfig expConfig;
expConfig.name = "working_memory_capacity_test";
expConfig.seed = 123456789;
expConfig.neuronCount = 5000;
expConfig.stepsPerTrial = 100;
expConfig.trials = 10;
```

// Create reproducible experiment
ExperimentConfig expConfig;
expConfig.name = "working_memory_capacity_test";
expConfig.seed = 123456789;
expConfig.neuronCount = 5000;
expConfig.stepsPerTrial = 100;
expConfig.trials = 10;
```

// Create reproducible experiment
ExperimentConfig expConfig;
expConfig.name = "working_memory_capacity_test";
expConfig.seed = 123456789;
expConfig.neuronCount = 5000;
expConfig.stepsPerTrial = 100;
expConfig.trials = 10;
```

// Create reproducible experiment
ExperimentConfig expConfig;
expConfig.name = "working_memory_capacity_test";
expConfig.seed = 123456789;
expConfig.neuronCount = 5000;
expConfig.stepsPerTrial = 100;
expConfig.trials = 10;
```

// Create reproducible experiment
ExperimentConfig expConfig;
expConfig.name = "working_memory_capacity_test";
expConfig.seed = 123456789;
expConfig.neuronCount = 5000;
expConfig.stepsPerTrial = 100;
expConfig.trials = 10;
```

// Create reproducible experiment
ExperimentConfig expConfig;
expConfig.name = "working_memory_capacity_test";
expConfig.seed = 123456789;
expConfig.neuronCount = 5000;
expConfig.stepsPerTrial = 100;
expConfig.trials = 10;
```

// Create reproducible experiment
ExperimentConfig expConfig;
expConfig.name = "working_memory_capacity_test";
expConfig.seed = 123456789;
expConfig.neuronCount = 5000;
expConfig.stepsPerTrial = 100;
expConfig.trials = 10;
```

// Create reproducible experiment
ExperimentConfig expConfig;
expConfig.name = "working_memory_capacity_test";
expConfig.seed = 123456789;
expConfig.neuronCount = 5000;
expConfig.stepsPerTrial = 100;
expConfig.trials = 10;
```

// Create reproducible experiment
ExperimentConfig expConfig;
expConfig.name = "working_memory_capacity_test";
expConfig.seed = 123456789;
expConfig.neuronCount = 5000;
expConfig.stepsPerTrial = 100;
expConfig.trials = 10;
```

// Create reproducible experiment
ExperimentConfig expConfig;
expConfig.name = "working_memory_capacity_test";
expConfig.seed = 123456789;
expConfig.neuronCount = 5000;
expConfig.stepsPerTrial = 100;
expConfig.trials = 10;
```

// Create reproducible experiment
ExperimentConfig expConfig;
expConfig.name = "working_memory_capacity_test";
expConfig.seed = 123456789;
expConfig.neuronCount = 5000;
expConfig.stepsPerTrial = 100;
expConfig.trials = 10;
```

// Create reproducible experiment
ExperimentConfig expConfig;
expConfig.name = "working_memory_capacity_test";
expConfig.seed = 123456789;
expConfig.neuronCount = 5000;
expConfig.stepsPerTrial = 100;
expConfig.trials = 10;
```

// Create reproducible experiment
ExperimentConfig expConfig;
expConfig.name = "working_memory_capacity_test";
expConfig.seed = 123456789;
expConfig.neuronCount = 5000;
expConfig.stepsPerTrial = 100;
expConfig.trials = 10;
```

// Create reproducible experiment
ExperimentConfig expConfig;
expConfig.name = "working_memory_capacity_test";
expConfig.seed = 123456789;
expConfig.neuronCount = 5000;
expConfig.stepsPerTrial = 100;
expConfig.trials = 10;
```

// Create reproducible experiment
ExperimentConfig expConfig;
expConfig.name = "working_memory_capacity_test";
expConfig.seed = 123456789;
expConfig.neuronCount = 5000;
expConfig.stepsPerTrial = 100;
expConfig.trials = 10;
```

// Create reproducible experiment
ExperimentConfig expConfig;
expConfig.name = "working_memory_capacity_test";
expConfig.seed = 123456789;
expConfig.neuronCount = 5000;
expConfig.stepsPerTrial = 100;
expConfig.trials = 10;
```

// Create reproducible experiment
ExperimentConfig expConfig;
expConfig.name = "working_memory_capacity_test";
expConfig.seed = 123456789;
expConfig.neuronCount = 5000;
expConfig.stepsPerTrial = 100;
expConfig.trials = 10;
```

// Create reproducible experiment
ExperimentConfig expConfig;
expConfig.name = "working_memory_capacity_test";
expConfig.seed = 123456789;
expConfig.neuronCount = 5000;
expConfig.stepsPerTrial = 100;
expConfig.trials = 10;
```

// Create reproducible experiment
ExperimentConfig expConfig;
expConfig.name = "working_memory_capacity_test";
expConfig.seed = 123456789;
expConfig.neuronCount = 5000;
expConfig.stepsPerTrial = 100;
expConfig.trials = 10;
```

// Create reproducible experiment
ExperimentConfig expConfig;
expConfig.name = "working_memory_capacity_test";
expConfig.seed = 123456789;
expConfig.neuronCount = 5000;
expConfig.stepsPerTrial = 100;
expConfig.trials = 10;
```

// Create reproducible experiment
ExperimentConfig expConfig;
expConfig.name = "working_memory_capacity_test";
expConfig.seed = 123456789;
expConfig.neuronCount = 5000;
expConfig.stepsPerTrial = 100;
expConfig.trials = 10;
```

// Create reproducible experiment
ExperimentConfig expConfig;
expConfig.name = "working_memory_capacity_test";
expConfig.seed = 123456789;
expConfig.neuronCount = 5000;
expConfig.stepsPerTrial = 100;
expConfig.trials = 10;
```

// Create reproducible experiment
ExperimentConfig expConfig;
expConfig.name = "working_memory_capacity_test";
expConfig.seed = 123456789;
expConfig.neuronCount = 5000;
expConfig.stepsPerTrial = 100;
expConfig.trials = 10;
```

// Create reproducible experiment
ExperimentConfig expConfig;
expConfig.name = "working_memory_capacity_test";
expConfig.seed = 123456789;
expConfig.neuronCount = 5000;
expConfig.stepsPerTrial = 100;
expConfig.trials = 10;
```

## Visual Documentation

### Architecture Diagram

The NLM architecture follows a layered modular design:

```
┌─────────────────────────────────────────────────────────────┐
│                    NLM Brain System                        │
├─────────────────────────────────────────────────────────────┤
│                                                             │
│  ┌─────────────────┐  ┌─────────────────┐  ┌─────────┐ │
│  │   Sensory      │───▶│    Processing   │───▶│  Motor   │ │
│  │   Layer        │    │    Layer         │    │  Layer  │ │
│  └─────────────────┘  └─────────────────┘  └─────────┘ │
│         │                      │                      │        │
│         ▼                      ▼                      ▼        │
│  ┌─────────────────┐  ┌─────────────────┐  ┌───────┐ │
│  │  Memory        │  │  Prediction      │  │  Action│ │
│  │  Systems       │  │  Systems         │  │  Layer│ │
│  └─────────────────┘  └─────────────────┘  └───────┘ │
│                             │                       │       │
│                             ▼                       ▼       │
│                     ┌─────────────────┐       ┌─────┐ │
│                     │   Neuromodulation │       │  Development│ │
│                     │   Systems         │       │  Systems   │ │
│                     └─────────────────┘       └─────┘ │
│                                                             │
│  ┌─────────────────────────────────────────────────────────────┐ │
│  │              Phase 4: Emerging Cognition                   │ │
│  │  ┌─────────┐  ┌─────────┐  ┌─────────┐  ┌─────────┐ │
│  │  │Attention│  │Planning│  │Self-Model│ │Social  │
│  │  │         │  │         │  │         │  │Learning│
│  │  └─────────┘  └─────────┘  └─────────┘  └─────────┐ │
│  └─────────────────────────────────────────────────────────────┘ │
└─────────────────────────────────────────────────────────────┘
```

### Data Flow Diagram

#### Phase 1: Neural Computation

```
Sensory Input (test signals)
      ↓
Brain.receiveSensoryInput()
      ↓
Neural Computation (LIF dynamics)
      ↓
Spike Processing (event-driven)
      ↓
Plasticity (STDP, Hebbian)
      ↓
Network Change
```

#### Phase 2: Environment Interaction

```
WORLD (2D environment with objects)
      ↓
SENSORY PERCEPT (vision, touch, internal, proprioception)
      ↓
AgentBrain.processSensoryInput()
      ↓
Brain.receiveSensoryInput() → Sensory Neurons
      ↓
Neural Computation (LIF dynamics)
      ↓
Spike Processing (event-driven)
      ↓
Motor Decoding → MotorCommand
      ↓
AgentBrain.decodeMotorCommand()
      ↓
WORLD.applyMotorCommand()
      ↓
ACTION RESULT (reward, success)
      ↓
Reward Prediction Error
      ↓
Neuromodulation (dopamine-like signal)
      ↓
Eligibility Trace Update
      ↓
Plasticity (STDP, Hebbian, Reward-Modulated)
      ↓
Structural Plasticity (synaptogenesis/pruning)
      ↓
Developmental Stage (stage progression)
```

#### Phase 4: Emerging Cognition

```
WORLD
      ↓
ATTENTION (competitive selection)
      ↓
SENSORY INPUT
      ↓
PREDICTION (learn temporal relationships)
      ↓
WORKING MEMORY (maintain information)
      ↓
EPISODIC MEMORY (store experiences)
      ↓
CONCEPTS (discover patterns)
      ↓
PLANNING (simulate futures)
      ↓
SELF-MODEL (predict self)
      ↓
ACTION SELECTION
      ↓
MOTOR OUTPUT
      ↓
WORLD
      ↓
REWARD / CONSEQUENCE
      ↓
SOCIAL OBSERVATION (if others present)
      ↓
LEARNING UPDATES
```

## Configuration Guide

### Creating Configuration Files

Create configuration files in YAML format:

```yaml
# config.yaml
name: "working_memory_capacity_test"
seed: 123456789
neuron_count: 5000
synapse_density: 0.1
plasticity_rate: 0.1
working_memory_capacity: 100
prediction_horizon: 3
development_enabled: true
reward_modulation_enabled: true
structural_plasticity_enabled: false
curiosity_enabled: true
thread_count: 4
vectorization_enabled: true
sparse_connectivity_enabled: true
event_queue_size: 65536
```

### Configuration Options

#### Core Settings
- `neuron_count`: Total number of neurons in the brain
- `synapse_density`: Probability of synapse formation (0.0-1.0)
- `plasticity_rate`: Base learning rate (0.0-1.0)
- `working_memory_capacity`: Maximum working memory entries
- `prediction_horizon`: Number of steps predicted ahead

#### Development Settings
- `development_enabled`: Enable developmental stages
- `plasticity_modifier`: Age-dependent plasticity rate
- `maturation_threshold`: Age at which brain matures
- `critical_period_duration`: Length of critical learning period

#### Neuromodulation Settings
- `reward_modulation_enabled`: Apply reward prediction error
- `curiosity_enabled`: Enable curiosity-driven exploration
- `novelty_threshold`: Threshold for detecting novelty
- `dopamine_strength`: Magnitude of reward signals

#### Performance Settings
- `thread_count`: Number of threads for parallel processing
- `vectorization_enabled`: Use SIMD vectorization
- `sparse_connectivity_enabled`: Use sparse adjacency lists
- `event_queue_size`: Size of spike event queues

### Loading Configuration

```cpp
#include "core/Config/Config.hpp"

// Load from file
auto config = Config::loadFromFile("config.yaml");

// Modify configuration
config->set("neuron_count", 10000);
config->set("plasticity_rate", 0.05f);

// Save configuration
config->saveToFile("experiment_config.yaml");
```

### Configuration Templates

Create common experiment templates:

#### Working Memory Capacity Test
```yaml
name: "working_memory_capacity_test"
seed: 123456789
neuron_count: 5000
synapse_density: 0.1
plasticity_rate: 0.1
working_memory_capacity: 100
prediction_horizon: 3
development_enabled: true
reward_modulation_enabled: true
structural_plasticity_enabled: false
curiosity_enabled: true
```

#### Large Scale Performance Test
```yaml
name: "large_scale_performance_test"
seed: 987654321
neuron_count: 100000
synapse_density: 0.05
plasticity_rate: 0.05
working_memory_capacity: 500
prediction_horizon: 10
development_enabled: false
reward_modulation_enabled: true
structural_plasticity_enabled: true
curiosity_enabled: true
thread_count: 8
vectorization_enabled: true
sparse_connectivity_enabled: true
event_queue_size: 262144
```

## Troubleshooting

### Common Issues

#### Issue 1: Brain Initialization Fails
**Symptoms:** "Failed to initialize brain" message
**Causes:**
- Configuration parameters invalid
- Insufficient memory
- Missing dependencies

**Solutions:**
```bash
# Check system memory
free -h

# Verify compiler supports C++17
g++ --version

# Check available threads
cat /proc/cpuinfo | grep "processor" | wc -l
```

#### Issue 2: Performance Degradation
**Symptoms:** Slow simulation, high CPU usage
**Causes:**
- Vectorization not enabled
- Thread count mismatched
- Memory bandwidth saturation

**Solutions:**
```yaml
# Increase thread count for multi-core systems
thread_count: 8

# Enable performance optimizations
vectorization_enabled: true
sparse_connectivity_enabled: true

# Optimize memory pools
event_queue_size: 262144
```

#### Issue 3: Memory Issues
**Symptoms:** Crashes, segmentation faults
**Causes:**
- Configuration sets too high memory requirements
- Memory pool fragmentation

**Solutions:**
```yaml
# Start with conservative settings
neuron_count: 1000
synapse_density: 0.05
working_memory_capacity: 50

# Monitor memory usage during execution
./nlm --memory-monitor
```

### Debugging Techniques

#### Using Logging
```cpp
// Enable detailed logging
config->set("logger_level", "DEBUG");

// Log brain status periodically
brain->logStatus();
```

#### Running Experiments
```bash
# Run experiment with verbose output
./nlm_phase4_demo 100 --verbose

# Monitor performance metrics
./nlm --benchmark --neurons=10000 --steps=10000
```

### Experiment Troubleshooting

#### Experiment Fails to Converge
**Check:**
1. Random seed for reproducibility
2. Reward signals are meaningful
3. Development plasticity is appropriate
4. Network connectivity is sufficient

#### Memory Leaks
**Check:**
1. Monitor memory usage during long runs
2. Verify proper cleanup in destructors
3. Check for circular references

### Getting Help

#### Documentation
- API Reference: docs/API_REFERENCE.md
- User Guide: docs/USER_GUIDE.md
- Configuration Guide: docs/CONFIGURATION_GUIDE.md

#### Examples
- Beginner: examples/beginner_tutorial.py
- Intermediate: examples/intermediate_tutorial.py
- Advanced: examples/advanced_example.py

#### Community
- GitHub Issues
- Scientific discussions
- Performance forums

### Support Resources

#### Online Resources
- NLM Documentation
- Source code repositories
- Scientific publications

#### Local Resources
- Generated documentation
- Example code
- Configuration templates

## Conclusion

This guide provides comprehensive instructions for working with the NLM brain simulation system. From basic setup to advanced experimentation, NLM offers a powerful platform for exploring artificial developmental learning and neural computation.

Key takeaways:

1. **Start simple** - Begin with basic configurations and gradually increase complexity
2. **Reproducibility** - Use explicit random seeds for scientific experiments
3. **Monitoring** - Track performance and memory usage
4. **Configuration** - Proper configuration is key to successful experiments
5. **Iterative development** - Experiment with different parameters and observe results

For the most current information and updates, refer to the latest documentation and source code.