# NLM Phase 6 Integration - Integration Examples

This document provides step-by-step examples for connecting NLM's cognitive systems.

## 1. Basic Integration Example

### Creating an Integrated Brain

```cpp
#include "src/brain/Brain.hpp"
#include "src/config/Config.hpp"
#include <memory>

int main() {
    // Create configuration
    auto config = std::make_shared<Config>();
    config->set("neuron_count", 1000);
    config->set("region_count", 2);
    config->set("working_memory_capacity", 100);
    config->set("max_episodic_episodes", 1000);
    
    // Create and initialize brain
    auto brain = std::make_unique<Brain>(config);
    if (!brain->initialize()) {
        std::cerr << "Failed to initialize brain!" << std::endl;
        return 1;
    }
    
    // Access integrated systems
    auto* workingMemory = brain->getWorkingMemory();
    auto* episodicMemory = brain->getEpisodicMemory();
    auto* predictionSystem = brain->getPredictionSystem();
    auto* attention = brain->getAttention();
    auto* conceptFormation = brain->getConceptFormation();
    auto* planner = brain->getPlanner();
    
    std::cout << "Integrated brain created successfully!" << std::endl;
    std::cout << " - Working Memory: " << workingMemory->getCapacity() << " traces" << std::endl;
    std::cout << " - Episodic Memory: " << episodicMemory->getMaxEpisodes() << " episodes" << std::endl;
    std::cout << " - Prediction System: Ready for sequence learning" << std::endl;
    
    return 0;
}
```

## 2. Sensory Integration Example

### Connecting Sensory Input to Working Memory

```cpp
#include "src/brain/Brain.hpp"
#include "src/sensory/SensoryInput.hpp"
#include "src/agent/AgentBrain.hpp"

void integrateSensoryToWorkingMemory(Brain* brain, const SensoryInput& input) {
    if (!brain) return;
    
    // Inject sensory input into brain
    brain->receiveSensoryInput(input);
    
    // Access working memory for processing
    auto* workingMemory = brain->getWorkingMemory();
    if (!workingMemory) {
        NLM_LOG_ERROR("Working memory not available");
        return;
    }
    
    // Simulate sensory processing
    float sensoryStrength = 0.0f;
    for (float value : input.getData()) {
        sensoryStrength += value;
    }
    sensoryStrength /= input.getData().size();
    
    // Store in working memory
    workingMemory->storeToWorkingMemory("sensory_input", sensoryStrength, 10.0f);
    
    // Update prediction system with sensory input
    auto* predictionSystem = brain->getPredictionSystem();
    if (predictionSystem) {
        predictionSystem->updatePrediction(input);
    }
    
    NLM_LOG_INFO("Sensory input integrated successfully");
}

int main() {
    // Create brain with all systems
    auto config = std::make_shared<Config>();
    config->set("neuron_count", 500);
    auto brain = std::make_unique<Brain>(config);
    brain->initialize();
    
    // Create sensory input (example: vision)
    SensoryInput visionInput;
    visionInput.type = InputType::Vision;
    visionInput.data = {0.1f, 0.3f, 0.5f, 0.7f, 0.9f}; // Example vision data
    
    // Process sensory input
    integrateSensoryToWorkingMemory(brain.get(), visionInput);
    
    return 0;
}
```

## 3. Neuromodulation Integration Example

### Connecting Prediction Error to Dopamine

```cpp
#include "src/brain/Brain.hpp"
#include "src/neuromodulation/PredictionError.hpp"
#include "src/neuromodulation/Dopamine.hpp"

void integratePredictionErrorToDopamine(Brain* brain) {
    if (!brain) return;
    
    // Get prediction error and dopamine systems
    auto* predictionError = brain->getPredictionErrorSignal();
    auto* dopamine = brain->getDopamine();
    
    if (!predictionError || !dopamine) {
        NLM_LOG_ERROR("Prediction error or dopamine system not available");
        return;
    }
    
    // Simulate prediction computation
    float prediction = 0.85f;  // Expected value
    float actual = 0.75f;      // Actual value
    float error = predictionError->computeError(prediction, actual);
    
    // Apply prediction error to dopamine system
    dopamine->update(error);
    
    // Use prediction error to modulate plasticity
    float neuromodulationLevel = predictionError->getLevel();
    
    // Apply to STDP system
    auto* stdp = brain->getSTDP();
    if (stdp) {
        float plasticityMod = 1.0f + neuromodulationLevel * 0.5f;
        stdp->setLTPWeight(0.01f * plasticityMod);
        stdp->setLTDWeight(0.012f * plasticityMod);
    }
    
    // Apply to Hebbian system
    auto* hebbian = brain->getHebbian();
    if (hebbian) {
        float learningRate = 0.01f * (1.0f + neuromodulationLevel);
        hebbian->setLearningRate(learningRate);
    }
    
    NLM_LOG_INFO(std::string("Prediction error integrated: ") + 
                 std::to_string(error) + " -> Dopamine level: " +
                 std::to_string(dopamine->getLevel()));
}

int main() {
    // Initialize brain with neuromodulation
    auto config = std::make_shared<Config>();
    config->set("neuron_count", 300);
    auto brain = std::make_unique<Brain>(config);
    brain->initialize();
    
    // Integrate prediction error with dopamine
    integratePredictionErrorToDopamine(brain.get());
    
    return 0;
}
```

## 4. Memory Integration Example

### Connecting Working Memory to Episodic Memory

```cpp
#include "src/brain/Brain.hpp"
#include "src/memory/NeuralWorkingMemory.hpp"
#include "src/memory/NeuralEpisodicMemory.hpp"

void integrateWorkingToEpisodicMemory(Brain* brain) {
    if (!brain) return;
    
    // Get memory systems
    auto* workingMemory = brain->getWorkingMemory();
    auto* episodicMemory = brain->getEpisodicMemory();
    
    if (!workingMemory || !episodicMemory) {
        NLM_LOG_ERROR("Memory systems not available");
        return;
    }
    
    // Simulate working memory traces (from neural activity)
    std::vector<std::pair<float, float>> activeTraces;
    for (size_t i = 0; i < 10; ++i) {
        float strength = static_cast<float>(rand()) / RAND_MAX; // Random activity
        activeTraces.emplace_back(static_cast<float>(i), strength);
    }
    
    // Transfer to episodic memory every few steps (every 10 steps in simulation)
    static int stepCounter = 0;
    stepCounter++;
    
    if (stepCounter >= 10) {
        stepCounter = 0;
        
        // Create episodic memory item from working memory
        EpisodicMemoryItem episode;
        episode.timestamp = static_cast<size_t>(stepCounter);
        episode.reward = 0.5f; // Default reward
        
        // Store working memory traces
        for (const auto& trace : workingMemory->getActiveTraces()) {
            episode.workingMemoryTraces.push_back(
                std::make_pair(trace.neuronId, trace.strength)
            );
        }
        
        // Store in episodic memory
        episodicMemory->storeEpisode(episode);
        
        NLM_LOG_INFO(std::string("Episodic memory updated with ") +
                     std::to_string(episode.workingMemoryTraces.size()) +
                     " working memory traces");
    }
}

int main() {
    // Initialize brain with memory systems
    auto config = std::make_shared<Config>();
    config->set("neuron_count", 200);
    config->set("max_episodic_episodes", 100);
    auto brain = std::make_unique<Brain>(config);
    brain->initialize();
    
    // Simulate several steps
    for (int step = 0; step < 50; ++step) {
        // Update working memory with simulated neural activity
        auto* workingMemory = brain->getWorkingMemory();
        if (workingMemory) {
            workingMemory->update(0.001f); // Timestep
        }
        
        // Integrate with episodic memory
        integrateWorkingToEpisodicMemory(brain.get());
        
        // Print memory statistics
        auto* episodicMemory = brain->getEpisodicMemory();
        if (episodicMemory) {
            NLM_LOG_INFO(std::string("Step " + std::to_string(step) +
                                     ": Working memory traces = " +
                                     std::to_string(workingMemory->getActiveTraces().size()) +
                                     ", Episodic episodes = " +
                                     std::to_string(episodicMemory->getEpisodeCount())));
        }
        
        // Small delay to simulate real-time
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    
    return 0;
}
```

## 5. Development Integration Example

### Connecting Development Stages to Plasticity

```cpp
#include "src/brain/Brain.hpp"
#include "src/development/DevelopmentSystem.hpp"

void integrateDevelopmentToPlasticity(Brain* brain, double timestep) {
    if (!brain) return;
    
    // Get development system
    auto* developmentSystem = brain->getDevelopmentSystem();
    if (!developmentSystem) {
        NLM_LOG_ERROR("Development system not available");
        return;
    }
    
    // Update developmental stage
    developmentSystem->update(brain, 0.001f); // small timestep
    
    // Get current developmental stage
    DevelopmentalStage stage = brain->getDevelopmentalStage();
    
    // Adjust plasticity based on developmental stage
    float plasticityMod = 1.0f;
    switch (stage) {
        case DevelopmentalStage::Initial:
            plasticityMod = 1.0f;  // High plasticity
            NLM_LOG_INFO("Developmental stage: Initial (high plasticity)");
            break;
        case DevelopmentalStage::CriticalPeriod:
            plasticityMod = 0.8f;
            NLM_LOG_INFO("Developmental stage: Critical period (moderate plasticity)");
            break;
        case DevelopmentalStage::Maturation:
            plasticityMod = 0.5f;
            NLM_LOG_INFO("Developmental stage: Maturation (reduced plasticity)");
            break;
        case DevelopmentalStage::Adult:
            plasticityMod = 0.2f;
            NLM_LOG_INFO("Developmental stage: Adult (low plasticity)");
            break;
    }
    
    // Apply to structural plasticity
    auto* structuralPlasticity = brain->getStructuralPlasticity();
    if (structuralPlasticity) {
        float currentSynaptogenesis = structuralPlasticity->getSynaptogenesisRate();
        float currentPruning = structuralPlasticity->getPruningRate();
        
        structuralPlasticity->setSynaptogenesisRate(currentSynaptogenesis * plasticityMod);
        structuralPlasticity->setPruningRate(currentPruning * (2.0f - plasticityMod));
        
        NLM_LOG_INFO(std::string("Plasticity adjusted: synaptogenesis = ") +
                     std::to_string(structuralPlasticity->getSynaptogenesisRate()) +
                     ", pruning = " +
                     std::to_string(structuralPlasticity->getPruningRate()));
    }
}

int main() {
    // Initialize brain with development
    auto config = std::make_shared<Config>();
    config->set("neuron_count", 400);
    auto brain = std::make_unique<Brain>(config);
    brain->initialize();
    
    // Simulate development over time
    double totalTime = 5.0; // 5 seconds of development
    double timestep = 0.01; // 10ms steps
    double elapsed = 0.0;
    
    while (elapsed < totalTime) {
        // Integrate development with plasticity
        integrateDevelopmentToPlasticity(brain.get(), timestep);
        
        // Simulate brain step
        brain->step(1, elapsed);
        
        elapsed += timestep;
        
        // Log developmental progress
        if (static_cast<int>(elapsed * 10) % 50 == 0) { // Every 0.5 seconds
            NLM_LOG_INFO(std::string("Developmental progress: ") +
                         std::to_string(elapsed) + "s / " +
                         std::to_string(totalTime) + "s");
        }
        
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    
    NLM_LOG_INFO("Development simulation complete");
    NLM_LOG_INFO("Final developmental stage: " +
                 std::to_string(static_cast<int>(brain->getDevelopmentalStage())));
    
    return 0;
}
```

## 6. Full Integration Example

### Complete Brain-Environment Integration

```cpp
#include "src/brain/Brain.hpp"
#include "src/agent/AgentBrain.hpp"
#include "src/world/SimpleWorld.hpp"
#include "src/experiments/ExperimentRunner.hpp"

void runFullIntegrationDemo() {
    // Initialize brain with all systems
    auto config = std::make_shared<Config>();
    config->set("neuron_count", 800);
    config->set("working_memory_capacity", 80);
    config->set("max_episodic_episodes", 500);
    
    auto brain = std::make_unique<Brain>(config);
    if (!brain->initialize()) {
        std::cerr << "Failed to initialize brain!" << std::endl;
        return;
    }
    
    // Create agent brain
    auto agentBrain = std::make_unique<AgentBrain>(brain);
    agentBrain->initialize(); // Initialize agent-specific components
    
    // Create environment
    SimpleWorld world;
    world.initialize();
    
    // Run experiment
    ExperimentRunner experiment;
    experiment.initialize("nlm_phase6_integration", brain, agentBrain, world);
    
    // Run for specified number of episodes
    int numEpisodes = 100;
    int successfulActions = 0;
    
    NLM_LOG_INFO(std::string("Starting NLM Phase 6 Integration Demo")
                 std::string(" (" + std::to_string(numEpisodes) + " episodes)"));
    
    for (int episode = 0; episode < numEpisodes; ++episode) {
        // Reset for new episode
        brain->reset();
        agentBrain->reset();
        world.reset();
        
        // Episode loop
        int steps = 0;
        const int maxSteps = 200;
        
        while (steps < maxSteps) {
            // 1. Get observation from world
            SensoryPercept percept = world.observe(agentBrain.get());
            
            // 2. Process sensory input
            agentBrain->processSensoryInput(percept);
            
            // 3. Brain step (ALL systems integrated)
            brain->step(steps, static_cast<double>(steps) * 0.001);
            
            // 4. Decode motor command
            MotorCommand cmd = agentBrain->decodeMotorCommand();
            
            // 5. Apply action to world
            world.applyAction(agentBrain.get(), cmd);
            
            // 6. Compute reward
            float reward = world.computeReward(agentBrain.get());
            
            // 7. Apply reward modulation (neuromodulation + plasticity)
            float predictedReward = agentBrain->getExpectedReward();
            agentBrain->applyRewardModulation(reward, predictedReward);
            
            // Update development
            agentBrain->updateDevelopment(0.001);
            
            // Check for episode end
            if (world.isEpisodeDone()) {
                successfulActions++;
                break;
            }
            
            steps++;
        }
        
        // Log progress
        if ((episode + 1) % 10 == 0) {
            NLM_LOG_INFO(std::string("Episode " + std::to_string(episode + 1) + 
                                     "/" + std::to_string(numEpisodes) +
                                     ", Success rate: " +
                                     std::to_string(successfulActions * 100.0 / (episode + 1)) +
                                     "%, Avg steps: " + std::to_string(steps / (episode + 1))));
        }
    }
    
    // Print final statistics
    experiment.printStatistics();
    
    NLM_LOG_INFO("Integration demo completed successfully!");
    NLM_LOG_INFO(std::string("Final performance: " +
                             std::to_string(successfulActions * 100.0 / numEpisodes) +
                             "% success rate"));
}

int main() {
    // Run the full integration demonstration
    runFullIntegrationDemo();
    
    return 0;
}
```

## 7. Troubleshooting Integration Issues

### Common Problems and Solutions

```cpp
// Example: Troubleshooting working memory integration

void debugWorkingMemoryIntegration(Brain* brain) {
    if (!brain) return;
    
    auto* workingMemory = brain->getWorkingMemory();
    if (!workingMemory) {
        NLM_LOG_ERROR("Working memory not initialized!");
        return;
    }
    
    // Check working memory status
    int traces = workingMemory->getActiveTraces();
    int capacity = workingMemory->getCapacity();
    
    NLM_LOG_INFO(std::string("Working memory status:")
                 std::string(" Active traces = " + std::to_string(traces))
                 std::string(" Capacity = " + std::to_string(capacity)));
    
    // Check if working memory has input
    if (traces == 0) {
        NLM_LOG_WARNING("No working memory traces! Check:");
        NLM_LOG_WARNING("  - Sensory input is being received");
        NLM_LOG_WARNING("  - Neurons are firing");
        NLM_LOG_WARNING("  - Working memory update is being called");
    }
    
    // Check episodic memory integration
    auto* episodicMemory = brain->getEpisodicMemory();
    if (!episodicMemory) {
        NLM_LOG_WARNING("Episodic memory not available! Check:");
        NLM_LOG_WARNING("  - Episodic memory system is initialized");
        NLM_LOG_WARNING("  - Brain::getEpisodicMemory() is returning null");
    }
}

// Example: Debugging neuromodulation
void debugNeuromodulationIntegration(Brain* brain) {
    if (!brain) return;
    
    auto* dopamine = brain->getDopamine();
    auto* curiosity = brain->getCuriosity();
    auto* novelty = brain->getNovelty();
    
    NLM_LOG_INFO(std::string("Neuromodulation levels:"))
    NLM_LOG_INFO(std::string("  Dopamine: " + std::to_string(dopamine ? dopamine->getLevel() : 0.0f)));
    NLM_LOG_INFO(std::string("  Curiosity: " + std::to_string(curiosity ? curiosity->getLevel() : 0.0f)));
    NLM_LOG_INFO(std::string("  Novelty: " + std::to_string(novelty ? novelty->getLevel() : 0.0f)));
    
    // Check for problems
    if (dopamine && std::abs(dopamine->getLevel()) < 0.001f) {
        NLM_LOG_WARNING("Dopamine level is near zero! Check:");
        NLM_LOG_WARNING("  - Prediction error computation is working");
        NLM_LOG_WARNING("  - AgentBrain::applyRewardModulation() is being called");
        NLM_LOG_WARNING("  - Dopamine system is receiving input");
    }
}
```

## 8. Performance Monitoring

### Monitoring Integration Health

```cpp
struct IntegrationMetrics {
    size_t workingMemoryTraces;
    size_t episodicMemoryEpisodes;
    float predictionAccuracy;
    float neuromodulationBalance;
    float developmentalProgress;
    
    void print() const {
        std::cout << "=== NLM Phase 6 Integration Metrics ===" << std::endl;
        std::cout << "Working Memory: " << workingMemoryTraces << " traces" << std::endl;
        std::cout << "Episodic Memory: " << episodicMemoryEpisodes << " episodes" << std::endl;
        std::cout << "Prediction Accuracy: " << (predictionAccuracy * 100.0) << "%" << std::endl;
        std::cout << "Neuromodulation Balance: " << neuromodulationBalance << std::endl;
        std::cout << "Developmental Progress: " << (developmentalProgress * 100.0) << "%" << std::endl;
        std::cout << "=====================================" << std::endl;
    }
};

IntegrationMetrics computeIntegrationMetrics(Brain* brain) {
    IntegrationMetrics metrics;
    
    if (!brain) return metrics;
    
    // Memory metrics
    auto* workingMemory = brain->getWorkingMemory();
    if (workingMemory) {
        metrics.workingMemoryTraces = workingMemory->getActiveTraces();
    }
    
    auto* episodicMemory = brain->getEpisodicMemory();
    if (episodicMemory) {
        metrics.episodicMemoryEpisodes = episodicMemory->getEpisodeCount();
    }
    
    // Prediction metrics (simplified)
    auto* predictionSystem = brain->getPredictionSystem();
    if (predictionSystem) {
        metrics.predictionAccuracy = predictionSystem->getPredictionError(); // Inverted for demo
    }
    
    // Neuromodulation metrics
    auto* dopamine = brain->getDopamine();
    auto* curiosity = brain->getCuriosity();
    
    float dopLevel = dopamine ? dopamine->getLevel() : 0.0f;
    float curLevel = curiosity ? curiosity->getLevel() : 0.0f;
    metrics.neuromodulationBalance = (std::abs(dopLevel) + std::abs(curLevel)) / 2.0f;
    
    // Development metrics
    DevelopmentalStage stage = brain->getDevelopmentalStage();
    metrics.developmentalProgress = static_cast<float>(stage) / 4.0f; // 4 stages
    
    return metrics;
}
```

## Key Integration Points

### 1. Memory Integration
- Working memory receives sensory input directly from neural activity
- Episodic memory captures experiences at regular intervals
- Semantic memory builds associations between experiences
- Procedural memory stores learned action sequences

### 2. Prediction-NeuroModulation Coupling
- Prediction error drives dopaminergic learning
- Curiosity modulates exploration based on prediction error
- Novelty detection triggers attentional shifts
- Neuromodulators scale plasticity rates

### 3. Development-Plasticity Relationship
- Developmental stage affects all plasticity rates
- Critical periods regulate learning windows
- Maturation reduces plastic changes over time
- Adult stage stabilizes learned behaviors

### 4. Attention-Memory Selection
- Attention selects working memory winners
- Concept formation processes attended patterns
- Planning uses selected memories for action
- Self-model updates from attended experiences

These examples demonstrate how NLM Phase 6 integrates all cognitive systems into a coherent artificial brain capable of experience-driven learning, development, and adaptive behavior.