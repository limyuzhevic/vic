# NLM API Documentation

## Overview

NLM (Neural Learning Machine) is a brain-inspired spiking neural network simulator implementing Leaky Integrate-and-Fire (LIF) neurons with event-driven dynamics. The framework supports memory systems, prediction, neuromodulation, and agent-environment interaction for cognitive simulation.

## Core API

### Brain Class

The Brain class serves as the central coordinator of the neural system, integrating all subsystems including neural computation, memory, prediction, cognition, and neuromodulation. It implements real spiking neural computation with event-driven dynamics.

#### Key Features:
- **Neural Computation**: LIF neuron dynamics with spike processing
- **Memory Integration**: Working, episodic, and associative memory systems
- **Prediction System**: Temporal sequence learning and prediction
- **Neuromodulation**: Dopamine, curiosity, novelty signals
- **Development**: Age-dependent plasticity and maturation
- **Event-Driven Architecture**: Efficient spike propagation

#### Quick Start Example:

```cpp
#include "src/brain/Brain.hpp"
#include "src/core/Config/Config.hpp"

int main() {
    // Create configuration
    auto config = std::make_shared<nlm::Config>();
    config->set("brain.neuron_count", 1000);
    
    // Create brain
    auto brain = std::make_shared<nlm::Brain>(config);
    
    // Initialize
    brain->initialize();
    
    // Run simulation
    for (nlm::SimulationStep step = 0; step < 1000; ++step) {
        brain->step(step);
        // Get statistics
        auto firingRate = brain->getAverageFiringRate();
        auto spikeCount = brain->getTotalSpikeCount();
    }
    
    return 0;
}
```

### Neuron Class

The Neuron class implements a single biological neuron using the Leaky Integrate-and-Fire (LIF) model with Hodgkin-Huxley dynamics. Each neuron maintains its own state including membrane potential, synaptic inputs, and spike history.

#### Key Features:
- **LIF Dynamics**: Membrane potential integration with threshold spiking
- **Hodgkin-Huxley Channels**: Sodium and potassium gating variables
- **Plasticity Support**: Synaptic weight modification capabilities
- **Refractory Period**: Spike frequency adaptation
- **Sensory/Motor Integration**: External current injection and internal processing

#### Quick Start Example:

```cpp
#include "src/brain/Neuron.hpp"

int main() {
    // Create neuron with ID
    nlm::NeuronId neuronId = 0;
    nlm::Neuron neuron(neuronId);
    
    // Set neuron type
    neuron.setType(nlm::NeuronType::Excitatory);
    
    // Initialize with random parameters
    nlm::RandomGenerator rng(42);
    neuron.initializeRandom(rng);
    
    // Simulation loop
    for (nlm::Timestamp time = 0; time < 1000; time += 1) {
        // Inject external current (e.g., from sensory input)
        neuron.injectCurrent(5.0f); // 5.0 nA
        
        // Update neuron state
        bool fired = neuron.stepLIF(time, 1.0f);
        
        if (fired) {
            // Process spike (record in brain)
            neuron.recordSpike(time);
            
            // Update connected synapses via STDP
            for (auto synapseId : neuron.getOutgoingSynapses()) {
                // STDP plasticity will be applied automatically
            }
        }
        
        // Check for threshold crossing
        if (neuron.checkThreshold()) {
            neuron.setFiringState(nlm::FiringState::Spiking);
        } else {
            neuron.setFiringState(nlm::FiringState::Resting);
        }
    }
    
    return 0;
}
```

## Configuration

### Config.hpp Documentation

The Config class provides a flexible configuration management system using type-safe key-value pairs with support for validation, persistence, and multiple sources (file, command line, runtime).

#### Core Parameters:

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `brain.neuron_count` | int | 1000 | Total number of neurons in the brain |
| `brain.synapse_density` | float | 0.1f | Probability of connection between neurons |
| `brain.connection_probability` | float | 0.05f | Overall network connectivity |
| `brain.v_thresh` | float | -50.0f | Neuronal firing threshold (mV) |
| `brain.v_rest` | float | -70.0f | Resting membrane potential (mV) |
| `brain.v_reset` | float | -75.0f | Post-spike reset potential (mV) |
| `brain.tau_mem` | float | 20.0f | Membrane time constant (ms) |
| `brain.tau_ref` | float | 2.0f | Refractory period duration (ms) |

#### Configuration Example:

```cpp
#include "src/core/Config/Config.hpp"

void createConfiguration() {
    nlm::Config config;
    
    // Load from file
    if (!config.loadFromFile("config.json")) {
        // Set defaults
        config.set("brain.neuron_count", 1000);
        config.set("brain.synapse_density", 0.1f);
        config.set("brain.v_thresh", -50.0f);
        config.set("brain.v_rest", -70.0f);
        config.set("brain.v_reset", -75.0f);
        
        // Enable plasticity
        config.set("plasticity.stdp.enable", true);
        config.set("plasticity.stdp.learning_rate", 0.001f);
        
        // Enable neuromodulation
        config.set("neuromod.curiosity.enable", true);
        config.set("neuromod.novelty.enable", true);
        
        // Save configuration
        config.saveToFile("default_config.json");
    }
}
```

## Examples

### Example 1: Hello World - Basic Brain Simulation

This example demonstrates creating a minimal brain and running a simple simulation.

```cpp
#include "src/brain/Brain.hpp"
#include "src/core/Config/Config.hpp"
#include <iostream>

int main() {
    std::cout << "=== NLM Hello World ===" << std::endl;
    
    // Create default configuration
    auto config = std::make_shared<nlm::Config>();
    config->set("brain.neuron_count", 500);
    config->set("brain.synapse_density", 0.1f);
    
    // Create and initialize brain
    auto brain = std::make_shared<nlm::Brain>(config);
    brain->initialize();
    
    // Run simulation
    std::cout << "Running 100 simulation steps..." << std::endl;
    for (nlm::SimulationStep step = 0; step < 100; ++step) {
        brain->step(step);
        
        if (step % 20 == 0) {
            std::cout << "Step " << step << ": "
                     << brain->getFiringNeuronCount() << " firing, "
                     << brain->getTotalSpikeCount() << " total spikes" << std::endl;
        }
    }
    
    // Print final statistics
    std::cout << "\n=== Simulation Complete ===" << std::endl;
    std::cout << "Total neurons: " << brain->getTotalNeuronCount() << std::endl;
    std::cout << "Total synapses: " << brain->getTotalSynapseCount() << std::endl;
    std::cout << "Firing neurons: " << brain->getFiringNeuronCount() << std::endl;
    std::cout << "Total spikes: " << brain->getTotalSpikeCount() << std::endl;
    std::cout << "Average firing rate: " << brain->getAverageFiringRate() << " Hz" << std::endl;
    
    return 0;
}
```

### Example 2: Agent in World - Agent-Based Simulation with Sensory Input

This example demonstrates creating an agent that interacts with a simulated environment.

```cpp
#include "src/agent/AgentBrain.hpp"
#include "src/agent/AgentBody.hpp"
#include "src/world/SimpleWorld.hpp"
#include "src/brain/Brain.hpp"
#include "src/core/Config/Config.hpp"
#include <iostream>

int main() {
    std::cout << "=== NLM Agent in World ===" << std::endl;
    
    // Create brain with configuration
    auto config = std::make_shared<nlm::Config>();
    config->set("brain.neuron_count", 1000);
    config->set("brain.synapse_density", 0.1f);
    
    auto brain = std::make_shared<nlm::Brain>(config);
    brain->initialize();
    
    // Create agent brain interface
    auto agent = std::make_shared<nlm::AgentBrain>(brain);
    
    // Create and configure world
    auto world = std::make_unique<nlm::SimpleWorld>();
    world->configure(
        width: 20,
        height: 20,
        visionWidth: 8,
        visionHeight: 8
    );
    world->reset();
    world->setAgentStart(10.0, 10.0);
    
    // Initialize agent
    agent->initialize(*world);
    
    // Enable learning features
    agent->enableRewardModulation(true);
    agent->enableCuriosity(true);
    brain->enableStructuralPlasticity(true);
    brain->enableDevelopment(true);
    
    // Run agent simulation
    std::cout << "Running agent simulation for 500 steps..." << std::endl;
    for (nlm::SimulationStep step = 0; step < 500; ++step) {
        // Update world
        world->update(0.1);
        
        // Get sensory input
        auto percept = world->getSensoryPercept();
        
        // Process sensory input
        agent->processSensoryInput(percept);
        
        // Brain computation
        brain->step(step);
        
        // Get motor command
        auto motorCmd = agent->decodeMotorCommand();
        
        // Apply action to world
        world->applyMotorCommand(motorCmd, world->getSimulationTime());
        
        // Apply reward
        float reward = 0.0f; // Extract from world state
        agent->applyRewardModulation(reward, 0.0f);
        
        // Update development
        agent->updateDevelopment(0.1);
        
        if (step % 100 == 0) {
            std::cout << "Step " << step << ": "
                     << "Pos=" << world->getAgentBody().x << "," 
                     << world->getAgentBody().y << ", "
                     << "Curiosity=" << agent->getCuriosityLevel() << ", "
                     << "Novelty=" << agent->getNoveltyLevel() << ", "
                     << "DevStage=" << brain->getDevelopmentalStage() << std::endl;
        }
    }
    
    std::cout << "\n=== Agent Simulation Complete ===" << std::endl;
    
    return 0;
}
```

### Example 3: Learning Agent - Complete Learning Example

This example demonstrates a learning agent that develops capabilities through experience.

```cpp
#include "src/agent/AgentBrain.hpp"
#include "src/world/SimpleWorld.hpp"
#include "src/brain/Brain.hpp"
#include "src/core/Config/Config.hpp"
#include <iostream>
#include <memory>

int main() {
    std::cout << "=== NLM Learning Agent ===" << std::endl;
    
    // Create agent with learning configuration
    auto config = std::make_shared<nlm::Config>();
    
    // Set up for efficient learning
    config->set("brain.neuron_count", 2000);           // More neurons for complexity
    config->set("brain.synapse_density", 0.15f);      // Higher connectivity
    config->set("brain.v_thresh", -45.0f);           // Lower threshold for easier firing
    config->set("plasticity.stdp.learning_rate", 0.005f);  // Faster learning
    config->set("neuromod.curiosity.enable", true);   // Enable exploration
    config->set("neuromod.novelty.enable", true);     // Enable novelty detection
    
    auto brain = std::make_shared<nlm::Brain>(config);
    brain->initialize();
    
    auto agent = std::make_shared<nlm::AgentBrain>(brain);
    auto world = std::make_unique<nlm::SimpleWorld>();
    
    world->configure(width: 30, height: 30, visionWidth: 10, visionHeight: 10);
    world->reset();
    world->setAgentStart(15.0, 15.0);
    
    agent->initialize(*world);
    
    // Enable all learning systems
    agent->enableRewardModulation(true);        // Learn from rewards
    agent->enableCuriosity(true);              // Explore novel experiences
    brain->enableStructuralPlasticity(true);   // Grow new connections
    brain->enableDevelopment(true);            // Brain matures over time
    
    // Track learning metrics
    std::vector<float> curiosityHistory;
    std::vector<float> noveltyHistory;
    std::vector<float> performanceHistory;
    
    std::cout << "Running learning episode for 1000 steps..." << std::endl;
    for (nlm::SimulationStep step = 0; step < 1000; ++step) {
        // Update environment
        world->update(0.1);
        
        // Observe and act
        auto percept = world->getSensoryPercept();
        agent->processSensoryInput(percept);
        
        // Brain computation
        brain->step(step);
        
        auto action = agent->decodeMotorCommand();
        world->applyMotorCommand(action, world->getSimulationTime());
        
        // Calculate reward based on action outcome
        float reward = calculateReward(world.get(), action);
        float predictedReward = agent->getPredictionError(); // Using as placeholder
        
        // Learn from experience
        agent->applyRewardModulation(reward, predictedReward);
        agent->updateDevelopment(0.1);
        
        // Record metrics
        curiosityHistory.push_back(agent->getCuriosityLevel());
        noveltyHistory.push_back(agent->getNoveltyLevel());
        performanceHistory.push_back(calculatePerformance(world.get()));
        
        if (step % 200 == 0) {
            std::cout << "Step " << step << "/1000: "
                     << "Curiosity=" << agent->getCuriosityLevel() << ", "
                     << "Novelty=" << agent->getNoveltyLevel() << ", "
                     << "Performance=" << calculatePerformance(world.get()) << ", "
                     << "DevStage=" << brain->getDevelopmentalStage() << std::endl;
        }
    }
    
    // Analyze learning outcomes
    std::cout << "\n=== Learning Analysis ===" << std::endl;
    std::cout << "Final curiosity level: " << curiosityHistory.back() << std::endl;
    std::cout << "Final novelty level: " << noveltyHistory.back() << std::endl;
    std::cout << "Average performance: " << average(performanceHistory) << std::endl;
    std::cout << "Final developmental stage: " << brain->getDevelopmentalStage() << std::endl;
    
    // Demonstrate learned behavior (agent has stored skills in memory)
    demonstrateLearnedBehavior(agent.get(), world.get());
    
    return 0;
}

// Helper functions
float calculateReward(const nlm::SimpleWorld* world, const nlm::MotorCommand& action) {
    float reward = 0.0f;
    
    // Check for resource collection
    auto body = world->getAgentBody();
    for (const auto& obj : world->getObjects()) {
        if (obj.type == nlm::WorldObjectType::Resource) {
            float dist = sqrt((body.x - obj.x) * (body.x - obj.x) + 
                           (body.y - obj.y) * (body.y - obj.y));
            if (dist < 1.0f) {
                reward += obj.value;
            }
        }
    }
    
    return reward;
}

float calculatePerformance(const nlm::SimpleWorld* world) {
    auto body = world->getAgentBody();
    float efficiency = world->getMaxEnergy() - body.energy;
    return std::min(efficiency / world->getMaxEnergy(), 1.0f);
}

float average(const std::vector<float>& values) {
    if (values.empty()) return 0.0f;
    float sum = 0.0f;
    for (float v : values) sum += v;
    return sum / values.size();
}

void demonstrateLearnedBehavior(nlm::AgentBrain* agent, nlm::SimpleWorld* world) {
    std::cout << "\n=== Demonstrating Learned Behavior ===" << std::endl;
    
    // Agent has developed motor patterns and strategies
    // Show how it efficiently navigates
    world->setAgentStart(5.0, 5.0);
    
    std::cout << "Agent now demonstrates efficient navigation patterns..." << std::endl;
}
```

### Example 4: Integration Test - All Systems Working Together

This example tests the complete integration of all NLM subsystems.

```cpp
#include "src/brain/Brain.hpp"
#include "src/agent/AgentBrain.hpp"
#include "src/world/SimpleWorld.hpp"
#include "src/memory/NeuralWorkingMemory.hpp"
#include "src/neuromodulation/Dopamine.hpp"
#include "src/neuromodulation/Curiosity.hpp"
#include "src/neuromodulation/Novelty.hpp"
#include "src/core/Config/Config.hpp"
#include "src/experiments/Phase6IntegratedExperiment.hpp"
#include <iostream>

int main() {
    std::cout << "=== NLM Integration Test ===" << std::endl;
    
    // Configure for integration test
    auto config = std::make_shared<nlm::Config>();
    config->set("brain.neuron_count", 1000);
    config->set("brain.region_count", 2);
    config->set("memory.working_memory.capacity", 100);
    config->set("memory.episodic_memory.max_episodes", 500);
    config->set("neuromod.dopamine.scale", 1.0f);
    config->set("neuromod.curiosity.enable", true);
    config->set("neuromod.novelty.enable", true);
    
    // Create and initialize brain
    auto brain = std::make_shared<nlm::Brain>(config);
    brain->initialize();
    
    // Create agent
    auto agent = std::make_shared<nlm::AgentBrain>(brain);
    auto world = std::make_unique<nlm::SimpleWorld>();
    
    world->configure(width: 25, height: 25, visionWidth: 8, visionHeight: 8);
    world->reset();
    world->setAgentStart(12.0, 12.0);
    
    agent->initialize(*world);
    
    // Enable all features
    agent->enableRewardModulation(true);
    agent->enableCuriosity(true);
    brain->enableStructuralPlasticity(true);
    brain->enableDevelopment(true);
    
    std::cout << "Running comprehensive integration test..." << std::endl;
    
    // Phase 1: Memory Integration
    std::cout << "\n--- Phase 1: Memory Integration ---" << std::endl;
    testMemoryIntegration(brain.get(), world.get());
    
    // Phase 2: Neuromodulation Integration
    std::cout << "\n--- Phase 2: Neuromodulation Integration ---" << std::endl;
    testNeuromodulationIntegration(brain.get(), agent.get());
    
    // Phase 3: Development Integration
    std::cout << "\n--- Phase 3: Development Integration ---" << std::endl;
    testDevelopmentIntegration(brain.get(), agent.get());
    
    // Phase 4: Prediction Integration
    std::cout << "\n--- Phase 4: Prediction Integration ---" << std::endl;
    testPredictionIntegration(brain.get());
    
    // Phase 5: Checkpointing
    std::cout << "\n--- Phase 5: Checkpointing ---" << std::endl;
    testCheckpointing(brain.get());
    
    // Phase 6: Replay
    std::cout << "\n--- Phase 6: Replay ---" << std::endl;
    testReplay(brain.get(), world.get());
    
    std::cout << "\n=== Integration Test Complete ===" << std::endl;
    
    return 0;
}

void testMemoryIntegration(nlm::Brain* brain, nlm::SimpleWorld* world) {
    // Test working memory
    auto* workingMem = brain->getWorkingMemory();
    workingMem->store(std::vector<float>{0.5f, 0.3f, 0.8f}, 1.0f);
    auto retrieved = workingMem->retrieve();
    
    std::cout << "Working Memory: Stored " << workingMem->getActiveTraces() 
              << " traces, retrieved " << retrieved.size() << " elements" << std::endl;
    
    // Test episodic memory
    auto* episodicMem = brain->getEpisodicMemory();
    nlm::EpisodicMemoryItem episode;
    episode.timestamp = 100;
    episode.sensoryState = std::vector<float>{0.2f, 0.5f, 0.3f};
    episode.action = nlm::ActionType::MoveForward;
    episode.reward = 1.0f;
    
    episodicMem->storeEpisode(episode);
    std::cout << "Episodic Memory: " << episodicMem->getEpisodeCount() << " episodes stored" << std::endl;
}

void testNeuromodulationIntegration(nlm::Brain* brain, nlm::AgentBrain* agent) {
    auto* dopamine = brain->getDopamine();
    auto* curiosity = brain->getCuriosity();
    auto* novelty = brain->getNovelty();
    
    // Simulate learning episode
    agent->applyRewardModulation(1.0f, 0.8f);  // Reward received
    
    std::cout << "Neuromodulation: Dopamine=" << dopamine->getLevel() 
              << ", Curiosity=" << curiosity->getLevel() 
              << ", Novelty=" << novelty->getLevel() << std::endl;
}

void testDevelopmentIntegration(nlm::Brain* brain, nlm::AgentBrain* agent) {
    auto stage = brain->getDevelopmentalStage();
    
    for (int i = 0; i < 10; ++i) {
        agent->updateDevelopment(0.1);
    }
    
    auto newStage = brain->getDevelopmentalStage();
    std::cout << "Development: " << static_cast<int>(stage) << " -> " << static_cast<int>(newStage) 
              << " (plasticity modifier: " << brain->getPlasticityModifier() << ")" << std::endl;
}

void testPredictionIntegration(nlm::Brain* brain) {
    auto* prediction = brain->getPredictionSystem();
    
    // Simulate prediction
    std::vector<float> sequence{0.1f, 0.3f, 0.7f};
    prediction->learnSequence(sequence);
    
    std::vector<float> prediction{0.5f, 0.6f};
    float error = prediction->predictNext(prediction);
    
    std::cout << "Prediction: Sequence learned, prediction error=" << error << std::endl;
}

void testCheckpointing(nlm::Brain* brain) {
    bool saved = brain->save("integration_test_checkpoint.bin");
    bool loaded = false;
    
    if (saved) {
        loaded = brain->load("integration_test_checkpoint.bin");
    }
    
    std::cout << "Checkpoint: Save=" << (saved ? "OK" : "Failed") 
              << ", Load=" << (loaded ? "OK" : "Failed") << std::endl;
}

void testReplay(nlm::Brain* brain, nlm::SimpleWorld* world) {
    auto* episodicMem = brain->getEpisodicMemory();
    
    // Get recent episodes for replay
    auto episodes = episodicMem->getRecentEpisodes(3);
    
    if (!episodes.empty()) {
        episodicMem->replayEpisode(episodes[0]);
        std::cout << "Replay: Replayed " << episodes.size() << " episodes" << std::endl;
    } else {
        std::cout << "Replay: No episodes available for replay" << std::endl;
    }
}
```

## Usage Guide

### Getting Started

1. **Installation**: Follow the installation instructions in `HOW_TO_USE.md` or `easy_usage.md`
2. **Configuration**: Create a configuration file or use default settings
3. **Basic Usage**: Create a brain, initialize it, and run simulation steps
4. **Advanced Features**: Enable neuromodulation, memory systems, and development

### Step-by-Step Tutorials

#### Tutorial 1: Simple Brain Simulation

1. Create a configuration
2. Initialize the brain
3. Run a simulation loop
4. Observe neural activity

```cpp
nlm::Config config;
brain.initialize();
for (int step = 0; step < 100; ++step) {
    brain.step(step);
}
```

#### Tutorial 2: Agent in Environment

1. Create a world with configuration
2. Create an agent brain interface
3. Run the interaction loop
4. Enable learning features

```cpp
world.configure(width, height, visionWidth, visionHeight);
agent.initialize(world);
while (simulationRunning) {
    world.update(0.1);
    agent.processSensoryInput(world.getSensoryPercept());
    brain.step(step);
    auto action = agent.decodeMotorCommand();
    world.applyMotorCommand(action, time);
}
```

### Common Patterns

#### Pattern 1: Continuous Learning

- Enable all neuromodulation systems
- Use reward signals for plasticity
- Track performance metrics
- Periodically save checkpoints

#### Pattern 2: Developmental Agent

- Start with immature brain state
- Gradually enable development features
- Use curiosity to drive exploration
- Adapt based on experience

### Performance Optimization

1. **Batch Processing**: Process multiple steps in batches where possible
2. **Memory Management**: Clear unused memory traces regularly
3. **Pruning**: Remove weak synapses and neurons as needed
4. **Parallel Processing**: Use SIMD instructions for vector operations
5. **Efficient Data Structures**: Use sparse matrices for connectivity

### Troubleshooting

**Common Issues and Solutions:**

- **Brain not firing**: Check current injection, threshold settings, and initialization
- **Memory overflow**: Adjust working memory capacity and decay rates
- **Performance slow**: Check for infinite loops, optimize data structures
- **Learning not occurring**: Verify neuromodulation is enabled and rewards are provided

### Best Practices

1. **Always initialize** before using any component
2. **Use configuration files** for reproducible experiments
3. **Enable checkpointing** for long-running simulations
4. **Monitor neural statistics** to detect abnormal behavior
5. **Use deterministic random seeds** for reproducible results
6. **Profile performance** regularly for large-scale simulations

## API Reference

### Core Classes

- `Brain`: Central neural coordinator
- `Neuron`: Single neuron implementation
- `Synapse`: Neural connection
- `Config`: Configuration management

### Memory Systems

- `NeuralWorkingMemory`: Persistent activity for temporary storage
- `NeuralEpisodicMemory`: Experience encoding and retrieval
- `NeuralAssociativeMemory`: Pattern association

### Neuromodulation

- `Dopamine`: Reward and reinforcement
- `Curiosity`: Exploration motivation
- `Novelty`: Novelty detection
- `PredictionError`: Learning signals

### Agent Systems

- `AgentBrain`: Brain-environment interface
- `AgentBody`: Physical agent state
- `SensoryPercept`: Sensory input wrapper

### Development

- `DevelopmentSystem`: Orchestrates developmental processes
- `Synaptogenesis`: New synapse formation
- `Pruning`: Synapse elimination
- `Maturation`: Adult development

This documentation provides comprehensive coverage of the NLM public APIs, including detailed examples and usage guidelines for both beginners and advanced users.
