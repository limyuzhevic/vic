# Basic Integration Example

## Creating Your First Brain

This example shows the most basic way to create and use an NLM brain.

```cpp
#include "nlm/brain/Brain.hpp"
#include "nlm/config/Config.hpp"
#include <iostream>

int main() {
    // Create configuration with 50 neurons
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    // Create the brain
    nlm::Brain brain(config);
    
    // Initialize the brain (essential step)
    if (!brain.initialize()) {
        std::cerr << "Failed to initialize brain!" << std::endl;
        return 1;
    }
    
    // Run brain for 100 simulation steps
    for (nlm::SimulationStep step = 0; step < 100; ++step) {
        brain.step(step);
    }
    
    // Get brain statistics
    std::cout << "Brain created successfully!" << std::endl;
    std::cout << "Total neurons: " << brain.getTotalNeuronCount() << std::endl;
    std::cout << "Total synapses: " << brain.getTotalSynapseCount() << std::endl;
    std::cout << "Active neurons: " << brain.getActiveNeuronCount() << std::endl;
    std::cout << "Firing neurons: " << brain.getFiringNeuronCount() << std::endl;
    std::cout << "Average firing rate: " << brain.getAverageFiringRate() << std::endl;
    
    return 0;
}
```

## Key Concepts in Basic Usage

1. **Brain Creation**: Always start with a configuration object
2. **Initialization**: Call `initialize()` before first use
3. **Simulation Loop**: Use `step()` to advance brain state
4. **Statistics**: Monitor brain health with built-in metrics

---

## Memory System Tutorial

### Understanding NLM's Memory Architecture

NLM implements multiple memory systems:

| Memory Type | Purpose | Access Speed | Retention |
|-------------|---------|--------------|-----------|
| Working Memory | Temporary active information | Very fast | Seconds to minutes |
| Episodic Memory | Personal experiences | Fast | Hours to days |
| Semantic Memory | Facts and knowledge | Medium | Weeks to years |
| Procedural Memory | Skills and habits | Slow | Months to years |
| Associative Memory | Relationships | Variable | Varies by strength |

### Working Memory Example

```cpp
#include "nlm/brain/Brain.hpp"
#include "nlm/memory/NeuralWorkingMemory.hpp"

void workingMemoryExample() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(200), nlm::ConfigSource::Default);
    nlm::Brain brain(config);
    brain.initialize();
    
    auto workingMem = brain.getWorkingMemory();
    if (workingMem) {
        // Store temporary information
        NeuronId taskId = 42;
        workingMem->store(taskId, 1.5f);  // Store value for neuron 42
        
        // Retrieve information
        float value = workingMem->retrieve(taskId);
        
        // Check if information exists
        if (workingMem->contains(taskId)) {
            // Perform action based on retrieved value
            float processedValue = value * 2.0f;
            workingMem->store(taskId, processedValue);
        }
        
        // Decay system automatically reduces all values over time
        workingMem->decay(0.01f); // 1% decay per step
    }
}
```

### Episodic Memory Example

```cpp
void episodicMemoryExample() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(300), nlm::ConfigSource::Default);
    nlm::Brain brain(config);
    brain.initialize();
    
    auto episodicMem = brain.getEpisodicMemory();
    if (episodicMem) {
        // Store an experience
        nlm::EpisodicMemoryItem experience;
        experience.timestamp = 0;
        
        // Store sensory pattern
        experience.neurons.push_back(100);
        experience.values.push_back(0.8f);
        experience.neurons.push_back(101);
        experience.values.push_back(0.6f);
        
        experience.metadata = "Visited room: entrance";
        
        episodicMem->storeEpisode(experience);
        
        // Retrieve recent episodes
        auto recent = episodicMem->getRecentEpisodes(5);
        for (size_t i = 0; i < recent.size(); ++i) {
            std::cout << "Episode " << i << ": " 
                     << recent[i].metadata << std::endl;
        }
        
        // Consolidate important memories
        episodicMem->consolidate(0.7f); // Keep episodes with relevance > 70%
    }
}
```

### Prediction System Integration

```cpp
void predictionExample() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(400), nlm::ConfigSource::Default);
    nlm::Brain brain(config);
    brain.initialize();
    
    auto predictionSys = brain.getPredictionSystem();
    if (predictionSys) {
        // Create sensory input for prediction
        nlm::SensoryInput sensory;
        sensory.vision.resize(8, 0.0f);
        sensory.vision[3] = 1.0f; // Object detected at vision index 3
        
        // Receive sensory input
        brain.receiveSensoryInput(sensory);
        
        // Run prediction
        brain.step(0);
        
        // Get prediction error
        float predictionError = predictionSys->getPredictionError();
        
        // Apply prediction error to learning
        if (predictionError > 0.5f) {
            std::cout << "High prediction error - need to learn!" << std::endl;
        }
    }
}
```

---

## Advanced Agent Tutorial

### Creating a Smart Agent with Curiosity

```cpp
#include "nlm/agent/AgentBrain.hpp"
#include "nlm/world/SimpleWorld.hpp"
#include <iostream>

class SmartAgent {
public:
    SmartAgent() {
        // Configure brain
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", static_cast<int64_t>(500), nlm::ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(3), nlm::ConfigSource::Default);
        
        brain_ = std::make_shared<nlm::Brain>(config);
        brain_->initialize();
        
        // Enable advanced features
        brain_->getCuriosity()->enableExploration(true);
        brain_->getDopamine()->setLearningRate(0.3f);
        
        // Create agent brain
        agent_ = std::make_unique<nlm::AgentBrain>(brain_);
        
        // Enable curiosity-driven exploration
        agent_->enableCuriosity(true);
        agent_->enableRewardModulation(true);
    }
    
    void run() {
        world_ = std::make_unique<nlm::SimpleWorld>();
        nlm::WorldConfig worldConfig;
        worldConfig.width = 20;
        worldConfig.height = 20;
        worldConfig.visionWidth = 8;
        worldConfig.visionHeight = 8;
        
        world_->configure(worldConfig);
        world_->reset();
        
        agent_->initialize(*world_);
        
        std::cout << "Starting smart agent simulation..." << std::endl;
        
        for (nlm::SimulationStep step = 0; step < 1000; ++step) {
            // Update world
            world_->update(0.1);
            
            // Get what agent senses
            auto percept = world_->getSensoryPercept();
            
            // Process sensory input
            agent_->processSensoryInput(percept);
            
            // Brain thinks
            brain_->step(step);
            
            // Make curiosity-based decision
            auto action = agent_->decodeMotorCommand();
            world_->applyMotorCommand(action, world_->getSimulationTime());
            
            // Apply reward modulation
            float novelty = agent_->getNoveltyLevel();
            float curiosity = agent_->getCuriosityLevel();
            
            float reward = 0.0f;
            if (novelty > 0.8f) reward += 0.5f;
            if (curiosity > 0.6f) reward += 0.3f;
            
            agent_->applyRewardModulation(reward, 0.0f);
            
            // Log progress periodically
            if (step % 100 == 0) {
                std::cout << "Step " << step << ": "
                         << "Action: " << static_cast<int>(action.actionType)
                         << ", Novelty: " << novelty
                         << ", Curiosity: " << curiosity << std::endl;
            }
        }
        
        std::cout << "Agent simulation completed!" << std::endl;
    }
    
private:
    std::shared_ptr<nlm::Brain> brain_;
    std::unique_ptr<nlm::AgentBrain> agent_;
    std::unique_ptr<nlm::SimpleWorld> world_;
};

int main() {
    SmartAgent agent;
    agent.run();
    return 0;
}
```

## Key Features of Advanced Agents

1. **Curiosity-Driven Learning**: Automatically explores novel environments
2. **Reward Prediction**: Anticipates rewards before they're received
3. **Neuromodulation**: Dopamine for rewards, curiosity for exploration
4. **Dynamic Adaptation**: Changes behavior based on environment

---

## Configuration Guide

### Understanding Brain Configuration

NLM brains are configured through a flexible key-value system. Here's how to configure for different use cases:

#### 1. Research Brain Configuration

```cpp
void configureResearchBrain() {
    auto config = std::make_shared<nlm::Config>();
    
    // Basic brain parameters
    config->set("neuron_count", static_cast<int64_t>(1000), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(5), nlm::ConfigSource::Default);
    
    // Memory system configuration
    config->set("working_memory_capacity", 100, nlm::ConfigSource::Default);
    config->set("episodic_memory_capacity", 1000, nlm::ConfigSource::Default);
    config->set("semantic_memory_capacity", 5000, nlm::ConfigSource::Default);
    
    // Plasticity settings
    config->set("stdp_learning_rate", 0.01f, nlm::ConfigSource::Default);
    config->set("hebbian_learning_rate", 0.005f, nlm::ConfigSource::Default);
    config->set("structural_plasticity_rate", 0.001f, nlm::ConfigSource::Default);
    
    // Development settings
    config->set("development_enabled", true, nlm::ConfigSource::Default);
    config->set("developmental_stage_duration", 500, nlm::ConfigSource::Default);
    
    // Prediction settings
    config->set("prediction_horizon", 10, nlm::ConfigSource::Default);
    config->set("prediction_error_threshold", 0.1f, nlm::ConfigSource::Default);
}
```

#### 2. Real-Time Robot Configuration

```cpp
void configureRealTimeRobot() {
    auto config = std::make_shared<nlm::Config>();
    
    // Fast, compact brain for real-time
    config->set("neuron_count", static_cast<int64_t>(200), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    
    // Aggressive plasticity for quick adaptation
    config->set("stdp_learning_rate", 0.05f, nlm::ConfigSource::Default);
    config->set("hebbian_learning_rate", 0.02f, nlm::ConfigSource::Default);
    
    // Lightweight memory for speed
    config->set("working_memory_capacity", 50, nlm::ConfigSource::Default);
    config->set("episodic_memory_capacity", 200, nlm::ConfigSource::Default);
    
    // Strong curiosity for exploration
    config->set("curiosity_threshold", 0.3f, nlm::ConfigSource::Default);
    config->set("novelty_decay_rate", 0.95f, nlm::ConfigSource::Default);
}
```

#### 3. Educational Agent Configuration

```cpp
void configureEducationalAgent() {
    auto config = std::make_shared<nlm::Config>();
    
    // Larger brain for complex reasoning
    config->set("neuron_count", static_cast<int64_t>(800), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(4), nlm::ConfigSource::Default);
    
    // Focus on semantic learning
    config->set("semantic_memory_capacity", 10000, nlm::ConfigSource::Default);
    config->set("concept_formation_enabled", true, nlm::ConfigSource::Default);
    
    // Gentle learning for educational use
    config->set("stdp_learning_rate", 0.002f, nlm::ConfigSource::Default);
    config->set("reward_sensitivity", 1.5f, nlm::ConfigSource::Default);
    
    // Strong development system
    config->set("development_enabled", true, nlm::ConfigSource::Default);
    config->set("developmental_stage_duration", 1000, nlm::ConfigSource::Default);
}
```

### Common Configuration Parameters

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| neuron_count | int64 | 100 | Total number of neurons |
| region_count | int64 | 1 | Number of brain regions |
| connection_probability | float | 0.1 | Probability of connections |
| stdp_learning_rate | float | 0.01 | STDP learning rate |
| hebbian_learning_rate | float | 0.005 | Hebbian learning rate |
| working_memory_capacity | size_t | 50 | Working memory slots |
| episodic_memory_capacity | size_t | 500 | Episodic memory items |
| development_enabled | bool | true | Enable developmental stages |

---

## Performance Optimization Guide

### Optimizing Brain Performance

This guide covers techniques to improve NLM brain performance for different scenarios.

#### 1. Neuron Efficiency

```cpp
void optimizeNeuronUsage() {
    auto config = std::make_shared<nlm::Config>();
    
    // Use sparse connectivity for efficiency
    config->set("neuron_count", static_cast<int64_t>(500), nlm::ConfigSource::Default);
    config->set("connection_probability", 0.05f, nlm::ConfigSource::Default); // Sparse
    
    // Enable neuron type optimization
    config->set("excitatory_ratio", 0.8f, nlm::ConfigSource::Default);
    config->set("inhibitory_ratio", 0.2f, nlm::ConfigSource::Default);
    
    // Use Hebbian learning for stable patterns
    config->set("use_hebbian_learning", true, nlm::ConfigSource::Default);
}
```

#### 2. Memory Management Optimization

```cpp
void optimizeMemoryUsage() {
    auto config = std::make_shared<nlm::Config>();
    
    // Configure memory sizes based on use case
    config->set("working_memory_capacity", 100, nlm::ConfigSource::Default);
    config->set("episodic_memory_capacity", 500, nlm::ConfigSource::Default);
    config->set("semantic_memory_capacity", 2000, nlm::ConfigSource::Default);
    
    // Enable memory consolidation for long-term stability
    config->set("episodic_memory_consolidation", true, nlm::ConfigSource::Default);
    config->set("consolidation_threshold", 0.6f, nlm::ConfigSource::Default);
}
```

#### 3. Plasticity Optimization

```cpp
void optimizePlasticity() {
    auto config = std::make_shared<nlm::Config>();
    
    // Fine-tune STDP parameters for stability
    config->set("stdp_learning_rate", 0.02f, nlm::ConfigSource::Default);
    config->set("stdp_time_window", 20, nlm::ConfigSource::Default);
    config->set("stdp_eligibility_decay", 0.99f, nlm::ConfigSource::Default);
    
    // Balance between fast learning and stability
    config->set("hebbian_learning_rate", 0.01f, nlm::ConfigSource::Default);
    config->set("learning_forgetting_ratio", 0.7f, nlm::ConfigSource::Default);
}
```

#### 4. Development System Optimization

```cpp
void optimizeDevelopment() {
    auto config = std::make_shared<nlm::Config>();
    
    // Configure developmental stages
    config->set("development_enabled", true, nlm::ConfigSource::Default);
    config->set("developmental_stage_count", 5, nlm::ConfigSource::Default);
    
    // Stage-specific configurations
    config->set("stage1_plasticity", 0.5f, nlm::ConfigSource::Default);  // Setup phase
    config->set("stage2_plasticity", 1.0f, nlm::ConfigSource::Default);  // Learning phase
    config->set("stage3_plasticity", 0.8f, nlm::ConfigSource::Default);  // Specialization phase
    config->set("stage4_plasticity", 0.5f, nlm::ConfigSource::Default);  // Optimization phase
    config->set("stage5_plasticity", 0.3f, nlm::ConfigSource::Default);  // Maturation phase
}
```

### Performance Monitoring

```cpp
void monitorBrainPerformance(nlm::Brain& brain) {
    // Track key performance metrics
    struct PerformanceMetrics {
        float firingRate;
        float plasticityActivity;
        float memoryUtilization;
        float predictionAccuracy;
        size_t activeNeurons;
    };
    
    PerformanceMetrics metrics;
    
    // Get current metrics
    metrics.firingRate = brain.getAverageFiringRate();
    metrics.activeNeurons = brain.getActiveNeuronCount();
    
    auto workingMem = brain.getWorkingMemory();
    if (workingMem) {
        metrics.memoryUtilization = 
            static_cast<float>(workingMem->getCurrentSize()) / 
            workingMem->getCapacity();
    }
    
    // Log performance metrics
    if (metrics.firingRate > 50.0f) {
        std::cout << "Warning: High firing rate - consider reducing plasticity" << std::endl;
    }
    
    if (metrics.memoryUtilization > 0.9f) {
        std::cout << "Warning: Memory usage near capacity" << std::endl;
    }
    
    // Adaptive optimization based on metrics
    if (metrics.firingRate < 10.0f && metrics.activeNeurons < 50) {
        // Brain is underactive - increase input or plasticity
        std::cout << "Optimizing: Increasing brain activity" << std::endl;
    }
}
```

---

## Troubleshooting Guide

### Common Issues and Solutions

This guide addresses the most frequent problems encountered when using NLM brains.

#### 1. Brain Initialization Failures

**Problem**: Brain fails to initialize properly.

**Symptoms**:
- Program crashes during `brain.initialize()`
- Brain shows 0 neurons after initialization
- Memory access errors

**Solutions**:

```cpp
def solveInitializationIssue() {
    // 1. Check configuration
    auto config = std::make_shared<nlm::Config>();
    
    // Minimum required configuration
    config->set("neuron_count", static_cast<int64_t>(10), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    // Check for invalid values
    if (config->get<int64_t>("neuron_count") < 10) {
        std::cerr << "Error: Too few neurons for initialization" << std::endl;
        return false;
    }
    
    // 2. Create brain and initialize
    nlm::Brain brain(config);
    
    bool success = brain.initialize();
    if (!success) {
        std::cerr << "Error: Brain initialization failed" << std::endl;
        return false;
    }
    
    // 3. Verify initialization
    if (brain.getTotalNeuronCount() == 0) {
        std::cerr << "Error: Brain has 0 neurons" << std::endl;
        return false;
    }
    
    return true;
}
```

#### 2. Low Brain Activity

**Problem**: Brain isn't firing neurons enough.

**Symptoms**:
- `brain.getFiringNeuronCount()` returns 0
- `brain.getAverageFiringRate()` is very low
- No actions are produced

**Solutions**:

```cpp
def solveLowActivityIssue() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    
    // 1. Increase initial input
    nlm::SensoryInput initialInput;
    initialInput.vision.resize(8, 0.5f); // Non-zero input
    initialInput.touch = 0.3f;
    initialInput.internal = 0.2f;
    
    nlm::Brain brain(config);
    brain.initialize();
    brain.receiveSensoryInput(initialInput);
    
    // 2. Increase plasticity for learning
    config->set("stdp_learning_rate", 0.1f, nlm::ConfigSource::Default);
    
    // 3. Enable more connections
    config->set("connection_probability", 0.3f, nlm::ConfigSource::Default);
    
    // 4. Run more steps
    for (int i = 0; i < 100; ++i) {
        brain.step(i);
        
        // Check activity
        if (brain.getFiringNeuronCount() > 0) break;
    }
    
    return brain.getFiringNeuronCount();
}
```

#### 3. Memory System Issues

**Problem**: Memory systems not working correctly.

**Symptoms**:
- Memory capacity seems full
- Information not being stored/retrieved
- Memory leaks

**Solutions**:

```cpp
def solveMemoryIssue() {
    // Check memory system access
    auto episodicMem = brain.getEpisodicMemory();
    if (!episodicMem) {
        std::cerr << "Error: Cannot access episodic memory" << std::endl;
        return;
    }
    
    // Monitor memory usage
    size_t capacity = episodicMem->getCapacity();
    size_t current = episodicMem->getCurrentSize();
    
    std::cout << "Memory usage: " << current << "/" << capacity << std::endl;
    
    if (current >= capacity) {
        // Clear old memories
        episodicMem->clear();
        std::cout << "Memory cleared (capacity exceeded)" << std::endl;
    }
    
    // Test basic memory operations
    nlm::EpisodicMemoryItem testItem;
    testItem.timestamp = 0;
    testItem.metadata = "Test item";
    
    episodicMem->storeEpisode(testItem);
    
    auto retrieved = episodicMem->retrieveEpisode(0);
    if (retrieved.metadata != "Test item") {
        std::cerr << "Error: Memory retrieval failed" << std::endl;
    }
}
```

#### 4. Prediction System Problems

**Problem**: Prediction system not working as expected.

**Symptoms**:
- High prediction errors
- No prediction error signals
- Actions are random

**Solutions**:

```cpp
def solvePredictionIssue() {
    auto predictionSys = brain.getPredictionSystem();
    if (!predictionSys) {
        std::cerr << "Error: Cannot access prediction system" << std::endl;
        return;
    }
    
    // Check prediction error
    float error = predictionSys->getPredictionError();
    std::cout << "Prediction error: " << error << std::endl;
    
    if (error > 0.8f) {
        // Reset prediction system
        predictionSys->reset();
        std::cout << "Prediction system reset (high error)" << std::endl;
    }
    
    // Check prediction accuracy
    bool predictionsAccurate = predictionSys->arePredictionsAccurate();
    if (!predictionsAccurate) {
        // Adjust prediction sensitivity
        predictionSys->setSensitivity(0.5f);
        std::cout << "Prediction sensitivity adjusted" << std::endl;
    }
}
```

#### 5. Agent Control Issues

**Problem**: Agent not responding to world changes.

**Symptoms**:
- No motor commands being generated
- Agent doesn't explore environment
- Stuck in loops

**Solutions**:

```cpp
def solveAgentIssue() {
    auto agent = std::make_unique<nlm::AgentBrain>(brain_);
    agent->initialize(world_);
    
    // Check motor neuron connectivity
    auto motorCmd = agent->decodeMotorCommand();
    if (motorCmd.actionType == nlm::ActionType::WAIT) {
        // Agent is waiting - check why
        
        // 1. Check curiosity level
        float curiosity = agent->getCuriosityLevel();
        std::cout << "Curiosity level: " << curiosity << std::endl;
        
        // 2. Check neuromodulation
        float neuromodulation = agent->getNeuromodulationLevel();
        std::cout << "Neuromodulation level: " << neuromodulation << std::endl;
        
        // 3. Force exploration if needed
        if (curiosity < 0.3f) {
            // Add novelty signal
            auto noveltySys = brain_->getNovelty();
            if (noveltySys) {
                noveltySys->triggerNovelty();
            }
        }
    }
}
```

---

## Research Applications Guide

### Using NLM for Scientific Research

This guide demonstrates how to use NLM brains for various research applications.

#### 1. Cognitive Development Research

```cpp
void cognitiveDevelopmentResearch() {
    // Simulate cognitive development over time
    auto config = std::make_shared<nlm::Config>();
    
    // Large, complex brain for development studies
    config->set("neuron_count", static_cast<int64_t>(2000), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(6), nlm::ConfigSource::Default);
    
    // Enable full development system
    config->set("development_enabled", true, nlm::ConfigSource::Default);
    config->set("developmental_stage_count", 5, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Track development stages
    std::vector<float> stageMetrics;
    
    for (nlm::SimulationStep step = 0; step < 5000; ++step) {
        brain.step(step);
        
        // Record metrics at each stage
        auto stage = brain.getDevelopmentSystem()->getCurrentStage();
        float performance = calculatePerformance(brain);
        
        stageMetrics.push_back(performance);
        
        // Research: Analyze development patterns
        if (step % 500 == 0) {
            analyzeDevelopmentPattern(stage, performance, step);
        }
    }
    
    // Analyze developmental trajectory
    analyzeDevelopmentalTrajectory(stageMetrics);
}
```

#### 2. Memory Research Studies

```cpp
void memoryResearchStudy() {
    // Study different memory types
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(1500), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    auto workingMem = brain.getWorkingMemory();
    auto episodicMem = brain.getEpisodicMemory();
    auto semanticMem = brain.getSemanticMemory();
    
    // Working memory research
    for (int i = 0; i < 100; ++i) {
        // Load and process information
        workingMem->store(NeuronId(i), float(i) / 100.0f);
        
        // Decay over time (simulating real working memory)
        workingMem->decay(0.01f);
        
        // Measure retention
        float retention = workingMem->retrieve(NeuronId(i));
        recordWorkingMemoryRetention(retention, i);
    }
    
    // Episodic memory research
    for (int i = 0; i < 50; ++i) {
        nlm::EpisodicMemoryItem episode;
        episode.timestamp = i;
        episode.metadata = "Event " + std::to_string(i);
        
        // Store with some pattern
        for (int j = 0; j < 10; ++j) {
            episode.neurons.push_back(static_cast<NeuronId>(i * 10 + j));
            episode.values.push_back(sin(j * 0.1f) * 0.5f + 0.5f);
        }
        
        episodicMem->storeEpisode(episode);
        
        // Test recall after delay
        auto recent = episodicMem->getRecentEpisodes(10);
        evaluateEpisodicRecall(recent, i);
    }
    
    // Semantic memory research
    for (const auto& fact : researchFacts) {
        semanticMem->storeFact(fact.first, fact.second);
        auto retrieved = semanticMem->retrieveFact(fact.first);
        measureSemanticAccuracy(fact.second, retrieved);
    }
}
```

#### 3. Learning and Plasticity Research

```cpp
void learningPlasticityResearch() {
    // Study different plasticity rules
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(800), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    auto stdpSys = brain.getSTDP();
    auto hebbianSys = brain.getHebbian();
    
    // Test STDP learning
    for (int i = 0; i < 1000; ++i) {
        // Simulate spike timing-dependent plasticity
        if (i % 10 == 0) {
            stdpSys->applySTDP(0.0f, 5.0f); // Pre-then post-spike
        }
        
        brain.step(i);
    }
    
    // Test Hebbian learning
    for (int i = 0; i < 1000; ++i) {
        if (i % 7 == 0) {
            hebbianSys->applyHebbian(0.1f, 0.1f); // Co-active neurons
        }
        
        brain.step(i);
    }
    
    // Compare results
    comparePlasticityEffects(stdpSys, hebbianSys);
}
```

#### 4. Neuromodulation Research

```cpp
void neuromodulationResearch() {
    // Study reward-based learning
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(600), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    auto dopamine = brain.getDopamine();
    auto curiosity = brain.getCuriosity();
    
    // Reward prediction error research
    for (int step = 0; step < 2000; ++step) {
        // Generate random rewards
        float reward = (rand() % 100) / 100.0f; // 0-1 reward
        
        // Predict reward
        float predicted = dopamine->predictReward();
        
        // Calculate prediction error
        float error = reward - predicted;
        
        // Apply reward modulation
        brain.applyNeuromodulation(error);
        
        brain.step(step);
        
        // Record prediction error effects
        recordPredictionErrorEffects(error, step);
    }
    
    // Curiosity-driven exploration research
    for (int step = 0; step < 2000; ++step) {
        // Measure curiosity level
        float curiosityLevel = curiosity->getCurrentCuriosity();
        
        // Trigger exploration based on curiosity
        if (curiosityLevel > 0.7f) {
            explorationStep();
        }
        
        brain.step(step);
    }
}
```

---

## API Reference Documentation

### Core Brain API

#### Class: `nlm::Brain`

##### Constructor
```cpp
Brain(std::shared_ptr<Config> config)
```
**Description**: Creates a new brain with the given configuration.

**Parameters**:
- `config`: Configuration object defining brain parameters

**Example**:
```cpp
auto config = std::make_shared<nlm::Config>();
config->set("neuron_count", 100);
nlm::Brain brain(config);
```

##### Initialization
```cpp
bool initialize()
```
**Description**: Initializes the brain with current configuration. Must be called before first use.

**Returns**:
- `true` if initialization succeeds
- `false` if initialization fails

**Example**:
```cpp
nlm::Brain brain(config);
if (brain.initialize()) {
    std::cout << "Brain initialized successfully!" << std::endl;
}
```

##### Main Simulation Step
```cpp
void step(SimulationStep currentStep)
void step(SimulationStep currentStep, Timestamp currentTime)
```
**Description**: Advances the brain by one simulation step.

**Parameters**:
- `currentStep`: Current simulation step number
- `currentTime`: Optional timestamp (for time-based simulation)

**Example**:
```cpp
for (int step = 0; step < 100; ++step) {
    brain.step(step);
}
```

##### Sensory Input
```cpp
void receiveSensoryInput(const SensoryInput& input)
```
**Description**: Injects current into sensory neurons based on input pattern.

**Parameters**:
- `input`: Sensory input data (vision, touch, internal signals)

**Example**:
```cpp
nlm::SensoryInput sensory;
sensory.vision = {0.1f, 0.2f, 0.8f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f};
brain.receiveSensoryInput(sensory);
```

##### Neuron Current Injection
```cpp
void injectCurrent(NeuronId neuron, MembranePotential current)
void injectCurrentToNeurons(NeuronType type, MembranePotential current)
```
**Description**: Injects current directly into specific neurons or all neurons of a type.

**Example**:
```cpp
brain.injectCurrent(42, 0.5f);  // Inject into neuron 42
brain.injectCurrentToNeurons(nlm::NeuronType::EXCITATORY, 0.1f);
```

##### Action Production
```cpp
std::unique_ptr<Action> produceAction()
```
**Description**: Produces motor/action output based on motor neuron activity.

**Returns**:
- Unique pointer to Action object
- nullptr if no action can be produced

**Example**:
```cpp
auto action = brain.produceAction();
if (action) {
    std::cout << "Action type: " << static_cast<int>(action->actionType) << std::endl;
}
```

##### Neuromodulation
```cpp
void applyNeuromodulation(const Neuromodulator& signal)
```
**Description**: Applies neuromodulatory signals to the brain.

**Example**:
```cpp
nlm::Neuromodulator dopamine;
dopamine.type = nlm::NeuromodulatorType::DOAPAMINE;
dopamine.value = 0.5f;
brain.applyNeuromodulation(dopamine);
```

##### Plasticity and Development
```cpp
void updatePlasticity()
void develop()
```
**Description**: Updates plasticity rules and applies developmental changes.

**Example**:
```cpp
brain.updatePlasticity();  // Update synaptic connections
brain.develop();          // Apply developmental changes
```

##### Brain State Management
```cpp
void reset()
bool save(const std::string& filepath) const
bool load(const std::string& filepath)
```
**Description**: Resets brain state, saves to file, or loads from file.

**Example**:
```cpp
brain.reset();  // Reset to initial state
brain.save("brain_state.json");  // Save state
```

##### Region Management
```cpp
RegionId addRegion(const std::string& name = "")
NeuralRegion* getRegion(RegionId id)
const NeuralRegion* getRegion(RegionId id) const
size_t getRegionCount() const
std::vector<RegionId> getRegionIds() const
```
**Description**: Manages brain regions (processing areas).

**Example**:
```cpp
RegionId visualRegion = brain.addRegion("Visual");
auto* region = brain.getRegion(visualRegion);
```

##### Statistics and Metrics
```cpp
size_t getTotalNeuronCount() const
size_t getTotalSynapseCount() const
size_t getActiveNeuronCount() const
size_t getFiringNeuronCount() const
float getAverageFiringRate() const
float getExcitationInhibitionRatio() const
size_t getTotalSpikeCount() const
size_t getPendingSpikeEventCount() const
```
**Description**: Returns various brain statistics and metrics.

**Example**:
```cpp
std::cout << "Neurons: " << brain.getTotalNeuronCount() << std::endl;
std::cout << "Firing rate: " << brain.getAverageFiringRate() << std::endl;
std::cout << "E/I ratio: " << brain.getExcitationInhibitionRatio() << std::endl;
```

### Memory System API

#### Working Memory
```cpp
class NeuralWorkingMemory

void store(NeuronId neuron, float value)
float retrieve(NeuronId neuron) const
bool contains(NeuronId neuron) const
void clear()
size_t getCapacity() const
size_t getCurrentSize() const
void decay(float decayRate)
```

#### Episodic Memory
```cpp
class NeuralEpisodicMemory

void storeEpisode(const EpisodicMemoryItem& episode)
EpisodicMemoryItem retrieveEpisode(size_t index) const
size_t getEpisodeCount() const
std::vector<EpisodicMemoryItem> getRecentEpisodes(size_t count) const
void clear()
void consolidate(float relevanceThreshold)
```

#### Semantic Memory
```cpp
class SemanticMemory

void storeFact(const std::string& key, const std::string& value)
std::string retrieveFact(const std::string& key) const
bool hasFact(const std::string& key) const
std::vector<std::pair<std::string, std::string>> getAllFacts() const
void clear()
```

### Prediction System API
```cpp
class NeuralPrediction

float getPredictionError() const
void reset()
void setTarget(const SensoryInput& target)
void updatePrediction()
```

### AgentBrain API
```cpp
class AgentBrain

void initialize(const SimpleWorld& world)
size_t getSensoryInputSize() const
size_t getMotorOutputSize() const
void processSensoryInput(const SensoryPercept& percept)
MotorCommand decodeMotorCommand()
void applyRewardModulation(float reward, float predictedReward)
void updateDevelopment(double timestep)
DevelopmentalStage getDevelopmentalStage() const
float getNeuromodulationLevel() const
float getCuriosityLevel() const
float getNoveltyLevel() const
float getPredictionError() const
void reset()
void enableRewardModulation(bool enable)
void enableStructuralPlasticity(bool enable)
void enableDevelopment(bool enable)
void enableCuriosity(bool enable)
```

### Configuration API
```cpp
class Config

void set(const std::string& key, const T& value, ConfigSource source)
T get(const std::string& key) const
bool has(const std::string& key) const
void remove(const std::string& key)
std::vector<std::string> getKeys() const
```

### Common Configuration Keys

| Key | Type | Default | Description |
|-----|------|---------|-------------|
| neuron_count | int64 | 100 | Number of neurons |
| region_count | int64 | 1 | Number of brain regions |
| connection_probability | float | 0.1 | Connection probability |
| stdp_learning_rate | float | 0.01 | STDP learning rate |
| hebbian_learning_rate | float | 0.005 | Hebbian learning rate |
| working_memory_capacity | size_t | 50 | Working memory capacity |
| episodic_memory_capacity | size_t | 500 | Episodic memory capacity |
| semantic_memory_capacity | size_t | 5000 | Semantic memory capacity |
| development_enabled | bool | true | Enable development |
| curiosity_enabled | bool | true | Enable curiosity system |

---

## Example Projects

### 1. Simple Reflex Agent

**Description**: Basic agent that reacts to sensory input with simple reflexes.

```cpp
#include "nlm/agent/AgentBrain.hpp"
#include "nlm/world/SimpleWorld.hpp"

int main() {
    // Setup brain
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", 100);
    config->set("region_count", 1);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Setup world
    nlm::SimpleWorld world;
    nlm::WorldConfig worldConfig;
    worldConfig.width = 10;
    worldConfig.height = 10;
    world.configure(worldConfig);
    world.reset();
    
    // Setup agent
    nlm::AgentBrain agent(&brain);
    agent.initialize(world);
    
    // Run reflex agent
    for (int step = 0; step < 100; ++step) {
        world.update(0.1);
        agent.processSensoryInput(world.getSensoryPercept());
        brain.step(step);
        
        auto action = agent.decodeMotorCommand();
        world.applyMotorCommand(action, world.getSimulationTime());
    }
    
    return 0;
}
```

### 2. Learning Agent with Memory

**Description**: Agent that uses memory systems to learn from experience.

```cpp
#include "nlm/agent/AgentBrain.hpp"
#include "nlm/world/SimpleWorld.hpp"

class LearningAgent {
public:
    LearningAgent() {
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", 500);
        config->set("region_count", 3);
        config->set("working_memory_capacity", 100);
        config->set("episodic_memory_capacity", 500);
        
        brain_ = std::make_shared<nlm::Brain>(config);
        brain_->initialize();
        
        agent_ = std::make_unique<nlm::AgentBrain>(brain_);
        agent_->enableRewardModulation(true);
        agent_->enableCuriosity(true);
    }
    
    void run() {
        // Initialize world and agent
        world_ = std::make_unique<nlm::SimpleWorld>();
        nlm::WorldConfig worldConfig;
        worldConfig.width = 20;
        worldConfig.height = 20;
        worldConfig.visionWidth = 8;
        worldConfig.visionHeight = 8;
        
        world_->configure(worldConfig);
        world_->reset();
        agent_->initialize(*world_);
        
        // Learning loop
        for (int step = 0; step < 1000; ++step) {
            world_->update(0.1);
            agent_->processSensoryInput(world_->getSensoryPercept());
            brain_->step(step);
            
            auto action = agent_->decodeMotorCommand();
            world_->applyMotorCommand(action, world_->getSimulationTime());
            
            // Apply reward based on success
            float reward = calculateReward(*world_, action);
            agent_->applyRewardModulation(reward, 0.0f);
        }
    }
    
private:
    std::shared_ptr<nlm::Brain> brain_;
    std::unique_ptr<nlm::AgentBrain> agent_;
    std::unique_ptr<nlm::SimpleWorld> world_;
    
    float calculateReward(const nlm::SimpleWorld& world, const nlm::MotorCommand& action) {
        // Simple reward function
        if (action.actionType == nlm::ActionType::INTERACT) {
            return 1.0f;
        } else if (action.actionType == nlm::ActionType::MOVE_FORWARD) {
            return 0.5f;
        }
        return 0.0f;
    }
};

int main() {
    LearningAgent agent;
    agent.run();
    return 0;
}
```

### 3. Development Simulation

**Description**: Agent that develops and matures over time.

```cpp
#include "nlm/agent/AgentBrain.hpp"
#include "nlm/world/SimpleWorld.hpp"

class DevelopingAgent {
public:
    DevelopingAgent() {
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", 800);
        config->set("region_count", 4);
        config->set("development_enabled", true);
        config->set("developmental_stage_count", 5);
        
        brain_ = std::make_shared<nlm::Brain>(config);
        brain_->initialize();
        
        agent_ = std::make_unique<nlm::AgentBrain>(brain_);
        agent_->enableDevelopment(true);
    }
    
    void run() {
        world_ = std::make_unique<nlm::SimpleWorld>();
        nlm::WorldConfig worldConfig;
        worldConfig.width = 15;
        worldConfig.height = 15;
        worldConfig.visionWidth = 6;
        worldConfig.visionHeight = 6;
        
        world_->configure(worldConfig);
        world_->reset();
        agent_->initialize(*world_);
        
        // Development simulation
        for (int step = 0; step < 5000; ++step) {
            world_->update(0.1);
            agent_->processSensoryInput(world_->getSensoryPercept());
            brain_->step(step);
            
            // Update development
            agent_->updateDevelopment(0.1);
            
            // Get action
            auto action = agent_->decodeMotorCommand();
            world_->applyMotorCommand(action, world_->getSimulationTime());
            
            // Log development stage
            if (step % 500 == 0) {
                auto stage = agent_->getDevelopmentalStage();
                std::cout << "Step " << step << ", Stage: " << static_cast<int>(stage) << std::endl;
            }
        }
    }
    
private:
    std::shared_ptr<nlm::Brain> brain_;
    std::unique_ptr<nlm::AgentBrain> agent_;
    std::unique_ptr<nlm::SimpleWorld> world_;
};

int main() {
    DevelopingAgent agent;
    agent.run();
    return 0;
}
```

### 4. Curiosity-Driven Exploration

**Description**: Agent that explores environment based on curiosity.

```cpp
#include "nlm/agent/AgentBrain.hpp"
#include "nlm/world/SimpleWorld.hpp"

class CuriousAgent {
public:
    CuriousAgent() {
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", 600);
        config->set("region_count", 3);
        config->set("curiosity_enabled", true);
        
        brain_ = std::make_shared<nlm::Brain>(config);
        brain_->initialize();
        
        agent_ = std::make_unique<nlm::AgentBrain>(brain_);
        agent_->enableCuriosity(true);
        agent_->enableRewardModulation(true);
    }
    
    void run() {
        world_ = std::make_unique<nlm::SimpleWorld>();
        nlm::WorldConfig worldConfig;
        worldConfig.width = 25;
        worldConfig.height = 25;
        worldConfig.visionWidth = 8;
        worldConfig.visionHeight = 8;
        
        world_->configure(worldConfig);
        world_->reset();
        agent_->initialize(*world_);
        
        // Curiosity-driven exploration
        int explorationSteps = 0;
        int totalSteps = 0;
        
        for (int step = 0; step < 2000; ++step) {
            world_->update(0.1);
            agent_->processSensoryInput(world_->getSensoryPercept());
            brain_->step(step);
            
            auto action = agent_->decodeMotorCommand();
            world_->applyMotorCommand(action, world_->getSimulationTime());
            
            // Check for novel experiences
            float curiosity = agent_->getCuriosityLevel();
            float novelty = agent_->getNoveltyLevel();
            
            // Apply curiosity reward
            float curiosityReward = 0.0f;
            if (novelty > 0.7f) curiosityReward += 0.5f;
            if (curiosity > 0.6f) curiosityReward += 0.3f;
            
            agent_->applyRewardModulation(curiosityReward, 0.0f);
            
            totalSteps++;
            
            // Log exploration metrics
            if (step % 200 == 0) {
                std::cout << "Step " << step 
                         << ", Novelty: " << novelty
                         << ", Curiosity: " << curiosity
                         << ", Exploration ratio: " 
                         << (float)explorationSteps / totalSteps << std::endl;
            }
        }
    }
    
private:
    std::shared_ptr<nlm::Brain> brain_;
    std::unique_ptr<nlm::AgentBrain> agent_;
    std::unique_ptr<nlm::SimpleWorld> world_;
};

int main() {
    CuriousAgent agent;
    agent.run();
    return 0;
}
```

---

## Quick Start Summary

### 1. Create a Brain
```cpp
auto config = std::make_shared<nlm::Config>();
config->set("neuron_count", 100);
nlm::Brain brain(config);
```

### 2. Initialize
```cpp
brain.initialize();
```

### 3. Run Simulation
```cpp
for (int step = 0; step < 100; ++step) {
    brain.step(step);
}
```

### 4. Use Agent
```cpp
nlm::AgentBrain agent(&brain);
agent.initialize(world);

// Main loop
while (running) {
    world.update(0.1);
    agent.processSensoryInput(world.getSensoryPercept());
    brain.step(step);
    
    auto action = agent.decodeMotorCommand();
    world.applyMotorCommand(action, world.getSimulationTime());
}
```

### 5. Get Statistics
```cpp
std::cout << "Neurons: " << brain.getTotalNeuronCount() << std::endl;
std::cout << "Firing rate: " << brain.getAverageFiringRate() << std::endl;
std::cout << "Memory usage: " << memoryMem->getCurrentSize() 
         << "/" << memoryMem->getCapacity() << std::endl;
```

---

## Additional Resources

- **Documentation**: See `HOW_TO_USE.md` for detailed usage instructions
- **Architecture**: See `docs/ARCHITECTURE.md` for system design
- **Testing**: Check `tests/` for unit tests and examples
- **Community**: Join the NLM discussion forum for questions and collaboration