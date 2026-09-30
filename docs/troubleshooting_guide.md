# Troubleshooting Guide

## Overview of NLM Troubleshooting

Troubleshooting in NLM (Neural Learning Machine) involves identifying and resolving issues that occur during brain initialization, operation, and performance. This guide covers common problems and their solutions across different aspects of NLM functionality.

## Common Troubleshooting Categories

### 1. Initialization Issues
### 2. Memory System Issues
### 3. Plasticity and Learning Issues
### 4. Prediction System Issues
### 5. Neuromodulation Issues
### 6. Agent Control Issues
### 7. Performance Issues
### 8. Integration Issues

## Detailed Troubleshooting Procedures

### 1. Initialization Issues

#### Problem: Brain Initialization Failure

**Symptoms**:
- `brain.initialize()` returns false
- Program crashes during brain creation
- Brain shows 0 neurons after initialization
- Memory access errors or segmentation faults

**Common Causes**:
1. Invalid or incomplete configuration
2. Memory allocation failures
3. Missing dependencies or resources
4. Configuration conflicts

**Solutions**:

```cpp
def debugInitializationIssues() {
    // 1. Check configuration validity
    auto config = std::make_shared<nlm::Config>();
    
    // Ensure required settings
    if (!config->has("neuron_count")) {
        std::cerr << "Error: neuron_count not set" << std::endl;
        config->set("neuron_count", static_cast<int64_t>(10), nlm::ConfigSource::User);
    }
    
    if (!config->has("region_count")) {
        std::cerr << "Error: region_count not set" << std::endl;
        config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::User);
    }
    
    // Validate configuration values
    if (config->get<int64_t>("neuron_count") < 10) {
        std::cerr << "Error: Too few neurons for stable initialization" << std::endl;
        config->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::User);
    }
    
    // 2. Check system resources
    if (!checkSystemResources()) {
        std::cerr << "Error: Insufficient system resources" << std::endl;
        // Try with smaller configuration
        config->set("neuron_count", static_cast<int64_t>(20), nlm::ConfigSource::User);
        config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::User);
    }
    
    // 3. Create and initialize brain
    nlm::Brain brain(config);
    
    bool success = brain.initialize();
    if (!success) {
        std::cerr << "Error: Brain initialization failed" << std::endl;
        
        // Try different initialization approach
        success = tryAlternativeInitialization(brain, config);
        if (!success) {
            std::cerr << "Error: All initialization attempts failed" << std::endl;
            return false;
        }
    }
    
    // 4. Verify initialization
    if (brain.getTotalNeuronCount() == 0) {
        std::cerr << "Error: Brain has 0 neurons after initialization" << std::endl;
        return false;
    }
    
    if (brain.getRegionCount() == 0) {
        std::cerr << "Error: Brain has 0 regions after initialization" << std::endl;
        return false;
    }
    
    std::cout << "Brain initialized successfully" << std::endl;
    return true;
}

bool checkSystemResources() {
    // Check available memory
    size_t availableMemory = getAvailableMemory();
    if (availableMemory < 100 * 1024 * 1024) {  // 100MB
        return false;
    }
    
    // Check CPU availability
    if (getCPUCount() < 1) {
        return false;
    }
    
    return true;
}

bool tryAlternativeInitialization(nlm::Brain& brain, std::shared_ptr<nlm::Config> config) {
    // Try minimal configuration
    nlm::Config minimalConfig;
    minimalConfig.set("neuron_count", static_cast<int64_t>(5), nlm::ConfigSource::User);
    minimalConfig.set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::User);
    
    nlm::Brain minimalBrain(minimalConfig);
    if (minimalBrain.initialize()) {
        brain = std::move(minimalBrain);
        return true;
    }
    
    return false;
}
```

#### Problem: Configuration Validation Failure

**Symptoms**:
- Configuration validation errors
- Invalid configuration values
- Missing required settings
- Configuration conflicts

**Solutions**:

```cpp
def validateAndFixConfiguration() {
    nlm::Config config;
    std::vector<std::string> validationErrors;
    
    // Validate configuration
    if (!nlm::ConfigurationValidator::validate(config, validationErrors)) {
        std::cerr << "Configuration validation failed:" << std::endl;
        for (const auto& error : validationErrors) {
            std::cerr << "  " << error << std::endl;
        }
        
        // Attempt to fix common issues
        fixConfigurationIssues(config, validationErrors);
        
        // Re-validate
        validationErrors.clear();
        if (nlm::ConfigurationValidator::validate(config, validationErrors)) {
            std::cout << "Configuration validated and fixed" << std::endl;
            return config;
        }
    }
    
    return config;
}

void fixConfigurationIssues(nlm::Config& config, const std::vector<std::string>& errors) {
    for (const auto& error : errors) {
        if (error.find("neuron_count") != std::string::npos) {
            config.set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::User);
        } else if (error.find("region_count") != std::string::npos) {
            config.set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::User);
        } else if (error.find("connection_probability") != std::string::npos) {
            config.set("connection_probability", 0.1f, nlm::ConfigSource::User);
        } else if (error.find("working_memory_capacity") != std::string::npos) {
            config.set("working_memory_capacity", 50, nlm::ConfigSource::User);
        } else if (error.find("episodic_memory_capacity") != std::string::npos) {
            config.set("episodic_memory_capacity", 500, nlm::ConfigSource::User);
        }
    }
}
```

---

### 2. Memory System Issues

#### Problem: Working Memory Issues

**Symptoms**:
- Working memory full (no space for new items)
- Items not stored or retrieved correctly
- Working memory decays too quickly or too slowly
- Interference between memory items

**Solutions**:

```cpp
def debugWorkingMemoryIssues(nlm::Brain& brain) {
    auto* workingMem = brain.getWorkingMemory();
    if (!workingMem) {
        std::cerr << "Error: Working memory not available" << std::endl;
        return;
    }
    
    // Check memory capacity
    size_t capacity = workingMem->getCapacity();
    size_t currentSize = workingMem->getCurrentSize();
    
    std::cout << "Working memory usage: " << currentSize << "/" << capacity << std::endl;
    
    if (currentSize >= capacity) {
        std::cerr << "Error: Working memory full" << std::endl;
        
        // Clear some items
        workingMem->clear();
        std::cout << "Working memory cleared" << std::endl;
        
        // Set appropriate capacity
        size_t totalNeurons = brain.getTotalNeuronCount();
        size_t newCapacity = std::min(totalNeurons / 10, static_cast<size_t>(200));
        // workingMem->setCapacity(newCapacity); // If API available
    }
    
    // Test basic operations
    testWorkingMemoryBasicOperations(workingMem);
}

void testWorkingMemoryBasicOperations(nlm::WorkingMemory* workingMem) {
    if (!workingMem) return;
    
    // Test store
    workingMem->store(100, 1.0f);
    
    // Test retrieve
    float value = workingMem->retrieve(100);
    if (value != 1.0f) {
        std::cerr << "Error: Working memory retrieve failed" << std::endl;
    }
    
    // Test contains
    if (!workingMem->contains(100)) {
        std::cerr << "Error: Working memory contains failed" << std::endl;
    }
    
    // Test decay
    workingMem->decay(0.01f);
    float decayedValue = workingMem->retrieve(100);
    if (decayedValue >= 1.0f) {
        std::cerr << "Error: Working memory decay failed" << std::endl;
    }
    
    // Test clear
    workingMem->clear();
    if (workingMem->getCurrentSize() != 0) {
        std::cerr << "Error: Working memory clear failed" << std::endl;
    }
}
```

#### Problem: Episodic Memory Issues

**Symptoms**:
- Episodic memory capacity exceeded
- Episodes not stored or retrieved correctly
- Memory consolidation not working
- Pruning removing important memories

**Solutions**:

```cpp
def debugEpisodicMemoryIssues(nlm::Brain& brain) {
    auto* episodicMem = brain.getEpisodicMemory();
    if (!episodicMem) {
        std::cerr << "Error: Episodic memory not available" << std::endl;
        return;
    }
    
    // Check memory capacity
    size_t capacity = episodicMem->getCapacity();
    size_t currentSize = episodicMem->getEpisodeCount();
    
    std::cout << "Episodic memory usage: " << currentSize << "/" << capacity << " episodes" << std::endl;
    
    if (currentSize >= capacity) {
        std::cerr << "Error: Episodic memory capacity exceeded" << std::endl;
        
        // Adjust consolidation threshold
        // episodicMem->setConsolidationThreshold(0.9f); // If API available
        
        // Clear old episodes
        episodicMem->clear();
        std::cout << "Episodic memory cleared" << std::endl;
    }
    
    // Test storage and retrieval
    testEpisodicMemoryOperations(episodicMem);
}

void testEpisodicMemoryOperations(nlm::EpisodicMemory* episodicMem) {
    if (!episodicMem) return;
    
    // Create test episode
    nlm::EpisodicMemoryItem testEpisode;
    testEpisode.timestamp = 0;
    testEpisode.metadata = "Test episode";
    
    testEpisode.neurons.push_back(200);
    testEpisode.values.push_back(0.8f);
    
    // Store episode
    episodicMem->storeEpisode(testEpisode);
    
    // Retrieve episode
    size_t episodeCount = episodicMem->getEpisodeCount();
    if (episodeCount < 1) {
        std::cerr << "Error: Episode not stored" << std::endl;
        return;
    }
    
    nlm::EpisodicMemoryItem retrieved = episodicMem->retrieveEpisode(episodeCount - 1);
    
    if (retrieved.metadata != "Test episode") {
        std::cerr << "Error: Episode retrieval failed" << std::endl;
    }
    
    // Test recent episodes
    auto recent = episodicMem->getRecentEpisodes(5);
    if (recent.size() != 1) {
        std::cerr << "Error: Recent episodes retrieval failed" << std::endl;
    }
    
    // Test consolidation
    episodicMem->consolidate(0.5f);  // Consolidate episodes with relevance > 50%
    
    // Test clear
    episodicMem->clear();
    if (episodicMem->getEpisodeCount() != 0) {
        std::cerr << "Error: Episodic memory clear failed" << std::endl;
    }
}
```

---

### 3. Plasticity and Learning Issues

#### Problem: Low Learning Rate

**Symptoms**:
- Brain shows little or no learning over time
- Synaptic weights remain unchanged
- Behavior doesn't change with experience
- Prediction errors don't decrease

**Solutions**:

```cpp
def debugLowLearningRate(nlm::Brain& brain) {
    auto* stdp = brain.getSTDP();
    auto* hebbian = brain.getHebbian();
    
    // Check if plasticity systems are available
    if (!stdp || !hebbian) {
        std::cerr << "Error: Plasticity systems not available" << std::endl;
        return;
    }
    
    // Check current plasticity rates
    // float stdpRate = stdp->getLearningRate(); // If API available
    // float hebbianRate = hebbian->getLearningRate(); // If API available
    
    // Test plasticity system
    testPlasticitySystems(brain);
    
    // Adjust if learning is too slow
    adjustPlasticityRates(brain);
}

void testPlasticitySystems(nlm::Brain& brain) {
    // Check STDP
    auto* stdp = brain.getSTDP();
    if (stdp) {
        // Test STDP spike timing
        stdp->testSpikeTiming(0.0f, 5.0f);  // Pre-then post spike
        
        // Check weight changes
        // float weightChange = stdp->getWeightChange(); // If API available
        // if (std::abs(weightChange) < 0.001f) {
        //     std::cerr << "Warning: STDP weight change too small" << std::endl;
        // }
    }
    
    // Check Hebbian learning
    auto* hebbian = brain.getHebbian();
    if (hebbian) {
        // Test Hebbian coactivation
        hebbian->testCoactivation(0.8f, 0.8f);  // Co-active neurons
        
        // Check weight changes
        // float weightChange = hebbian->getWeightChange(); // If API available
        // if (std::abs(weightChange) < 0.001f) {
        //     std::cerr << "Warning: Hebbian weight change too small" << std::endl;
        // }
    }
}

void adjustPlasticityRates(nlm::Brain& brain) {
    auto* stdp = brain.getSTDP();
    auto* hebbian = brain.getHebbian();
    
    if (!stdp || !hebbian) return;
    
    // Increase plasticity rates
    // stdp->setLearningRate(0.03f); // If API available
    // hebbian->setLearningRate(0.015f); // If API available
    
    // Ensure plasticity is working
    // stdp->enablePlasticity(true); // If API available
    // hebbian->enablePlasticity(true); // If API available
}
```

#### Problem: Excessive Forgetting

**Symptoms**:
- Memories lost too quickly
- Learning doesn't stick
- Behavior resets frequently
- No long-term retention

**Solutions**:

```cpp
def debugExcessiveForgetting(nlm::Brain& brain) {
    auto* episodicMem = brain.getEpisodicMemory();
    auto* semanticMem = brain.getSemanticMemory();
    
    // Check episodic memory decay
    if (episodicMem) {
        // Increase consolidation threshold
        // episodicMem->setConsolidationThreshold(0.8f); // If API available
        
        // Increase consolidation rate
        // episodicMem->setConsolidationRate(0.1f); // If API available
        
        // Test consolidation
        testEpisodicConsolidation(episodicMem);
    }
    
    // Check semantic memory decay
    if (semanticMem) {
        // Decrease semantic memory forgetting rate
        // semanticMem->setForgettingRate(0.0001); // If API available
        
        // Enable semantic memory consolidation
        // semanticMem->enableConsolidation(true); // If API available
        
        // Test semantic memory retention
        testSemanticMemoryRetention(semanticMem);
    }
    
    // Adjust working memory decay
    auto* workingMem = brain.getWorkingMemory();
    if (workingMem) {
        // Adjust working memory decay rate
        // workingMem->setDecayRate(0.01f); // If API available
    }
}

void testEpisodicConsolidation(nlm::EpisodicMemory* episodicMem) {
    if (!episodicMem) return;
    
    // Create test episode
    nlm::EpisodicMemoryItem testEpisode;
    testEpisode.timestamp = 0;
    testEpisode.metadata = "Test episode for consolidation";
    
    testEpisode.neurons.push_back(300);
    testEpisode.values.push_back(0.9f);  // High relevance for consolidation
    
    // Store episode
    episodicMem->storeEpisode(testEpisode);
    
    // Consolidate with high threshold
    episodicMem->consolidate(0.8f);
    
    // Verify consolidation
    // bool isConsolidated = episodicMem->isConsolidated(0); // If API available
    // if (!isConsolidated) {
    //     std::cerr << "Warning: Episode not consolidated" << std::endl;
    // }
}
```

---

### 4. Prediction System Issues

#### Problem: High Prediction Errors

**Symptoms**:
- Prediction errors remain high
- Brain doesn't anticipate events well
- Actions don't match predicted outcomes
- Poor planning and decision making

**Solutions**:

```cpp
def debugHighPredictionErrors(nlm::Brain& brain) {
    auto* predictionSys = brain.getPredictionSystem();
    if (!predictionSys) {
        std::cerr << "Error: Prediction system not available" << std::endl;
        return;
    }
    
    // Check prediction error
    float predictionError = predictionSys->getPredictionError();
    std::cout << "Current prediction error: " << predictionError << std::endl;
    
    if (predictionError > 0.5f) {
        std::cerr << "Error: High prediction error" << std::endl;
        
        // Reset prediction system
        predictionSys->reset();
        std::cout << "Prediction system reset" << std::endl;
        
        // Adjust prediction sensitivity
        // predictionSys->setSensitivity(0.5f); // If API available
        
        // Test prediction accuracy
        testPredictionAccuracy(predictionSys);
    }
}

void testPredictionAccuracy(nlm::PredictionSystem* predictionSys) {
    if (!predictionSys) return;
    
    // Set target input
    nlm::SensoryInput target;
    target.vision = {0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f};
    
    // Make prediction
    predictionSys->setTarget(target);
    predictionSys->updatePrediction();
    
    // Check prediction error
    float predictionError = predictionSys->getPredictionError();
    
    if (predictionError > 0.3f) {
        std::cerr << "Warning: Prediction still inaccurate" << std::endl;
    } else {
        std::cout << "Prediction accuracy improved" << std::endl;
    }
}
```

---

### 5. Neuromodulation Issues

#### Problem: Reward System Not Working

**Symptoms**:
- No reward signals
- Learning doesn't respond to rewards
- Agent doesn't seek rewards
- Neuromodulation levels are zero

**Solutions**:

```cpp
def debugRewardSystem(nlm::Brain& brain) {
    auto* dopamine = brain.getDopamine();
    auto* curiosity = brain.getCuriosity();
    auto* novelty = brain.getNovelty();
    
    // Check if neuromodulation systems are available
    if (!dopamine || !curiosity || !novelty) {
        std::cerr << "Error: Neuromodulation systems not available" << std::endl;
        return;
    }
    
    // Check neuromodulation levels
    float dopamineLevel = dopamine->getCurrentLevel();
    float curiosityLevel = curiosity->getCurrentCuriosity();
    float noveltyLevel = novelty->getCurrentNovelty();
    
    std::cout << "Neuromodulation levels:" << std::endl;
    std::cout << "  Dopamine: " << dopamineLevel << std::endl;
    std::cout << "  Curiosity: " << curiosityLevel << std::endl;
    std::cout << "  Novelty: " << noveltyLevel << std::endl;
    
    // Check if reward system is enabled
    bool rewardEnabled = dopamine->isEnabled();
    if (!rewardEnabled) {
        std::cerr << "Error: Dopamine system not enabled" << std::endl;
        // dopamine->enable(true); // If API available
    }
    
    // Test reward system
    testRewardSystem(brain);
}

void testRewardSystem(nlm::Brain& brain) {
    auto* dopamine = brain.getDopamine();
    if (!dopamine) return;
    
    // Apply test reward
    float reward = 1.0f;
    float predicted = 0.5f;
    
    // Apply reward modulation
    brain.applyNeuromodulation(reward, predicted);
    
    // Check if dopamine level changed
    float dopamineLevel = dopamine->getCurrentLevel();
    if (dopamineLevel <= 0.0f) {
        std::cerr << "Error: Reward system not responding" << std::endl;
    } else {
        std::cout << "Reward system working, dopamine level: " << dopamineLevel << std::endl;
    }
}
```

---

### 6. Agent Control Issues

#### Problem: Agent Stuck in Loop

**Symptoms**:
- Agent repeats same actions
- No progress or exploration
- Behavior stuck in local minimum
- No learning from experience

**Solutions**:

```cpp
def debugAgentLoop(nlm::Brain& brain, nlm::AgentBrain& agent) {
    // Check curiosity level
    float curiosityLevel = agent.getCuriosityLevel();
    std::cout << "Curiosity level: " << curiosityLevel << std::endl;
    
    if (curiosityLevel < 0.2f) {
        std::cerr << "Error: Low curiosity - agent not exploring" << std::endl;
        
        // Boost curiosity
        agent.enableCuriosity(true);
        
        // Add novelty signal
        auto* novelty = brain.getNovelty();
        if (novelty) {
            novelty->triggerNovelty();
        }
        
        // Force exploration
        forceExploration(agent);
    }
    
    // Check reward modulation
    float rewardLevel = agent.getNeuromodulationLevel();
    std::cout << "Reward level: " << rewardLevel << std::endl;
    
    if (rewardLevel <= 0.0f) {
        std::cerr << "Error: No reward signals" << std::endl;
        
        // Apply artificial reward
        applyArtificialReward(agent, 0.5f);
    }
    
    // Check prediction error
    float predictionError = agent.getPredictionError();
    std::cout << "Prediction error: " << predictionError << std::endl;
    
    if (predictionError > 0.8f) {
        std::cerr << "Error: High prediction error" << std::endl;
        
        // Reset prediction system
        auto* predictionSys = brain.getPredictionSystem();
        if (predictionSys) {
            predictionSys->reset();
        }
    }
}

void forceExploration(nlm::AgentBrain& agent) {
    // Force agent to take exploratory action
    // This would depend on your action space
    
    nlm::MotorCommand exploratoryAction;
    exploratoryAction.actionType = nlm::ActionType::INTERACT;  // Example action
    exploratoryAction.parameters = {1.0f};  // Full strength
    
    // Apply exploratory action
    // agent.executeAction(exploratoryAction); // If API available
}

void applyArtificialReward(nlm::AgentBrain& agent, float reward) {
    // Apply artificial reward to boost learning
    float predicted = 0.0f;
    
    // Apply reward modulation
    // agent.applyRewardModulation(reward, predicted); // If API available
}
```

---

### 7. Performance Issues

#### Problem: High Memory Usage

**Symptoms**:
- Memory usage exceeds available RAM
- System becomes slow or unresponsive
- Memory allocation failures
- Performance degradation

**Solutions**:

```cpp
def debugMemoryUsage(nlm::Brain& brain) {
    // Monitor memory usage
    size_t currentMemory = getCurrentMemoryUsage();
    size_t totalMemory = getSystemMemoryLimit();
    
    std::cout << "Memory usage: " << currentMemory / 1024 / 1024 << " MB" << std::endl;
    std::cout << "Memory limit: " << totalMemory / 1024 / 1024 << " MB" << std::endl;
    
    if (currentMemory > totalMemory * 0.9) {
        std::cerr << "Error: Memory usage too high" << std::endl;
        
        // Clear memory
        clearBrainMemory(brain);
        
        // Reduce memory allocations
        reduceMemoryAllocations(brain);
        
        // Check for memory leaks
        if (hasMemoryLeak()) {
            std::cerr << "Error: Memory leak detected" << std::endl;
            // Fix memory leak
            fixMemoryLeak();
        }
    }
}

void clearBrainMemory(nlm::Brain& brain) {
    // Clear all memory systems
    auto* workingMem = brain.getWorkingMemory();
    if (workingMem) {
        workingMem->clear();
    }
    
    auto* episodicMem = brain.getEpisodicMemory();
    if (episodicMem) {
        episodicMem->clear();
    }
    
    auto* semanticMem = brain.getSemanticMemory();
    if (semanticMem) {
        semanticMem->clear();
    }
    
    // Reset brain state
    brain.reset();
}

void reduceMemoryAllocations(nlm::Brain& brain) {
    // Reduce memory allocations for efficiency
    
    // Reduce working memory capacity
    auto* workingMem = brain.getWorkingMemory();
    if (workingMem) {
        size_t currentCapacity = workingMem->getCapacity();
        size_t reducedCapacity = currentCapacity / 2;
        // workingMem->setCapacity(reducedCapacity); // If API available
    }
    
    // Reduce episodic memory capacity
    auto* episodicMem = brain.getEpisodicMemory();
    if (episodicMem) {
        size_t currentCapacity = episodicMem->getCapacity();
        size_t reducedCapacity = currentCapacity / 2;
        // episodicMem->setCapacity(reducedCapacity); // If API available
    }
    
    // Reduce semantic memory capacity
    auto* semanticMem = brain.getSemanticMemory();
    if (semanticMem) {
        size_t currentCapacity = semanticMem->getCapacity();
        size_t reducedCapacity = currentCapacity / 2;
        // semanticMem->setCapacity(reducedCapacity); // If API available
    }
}
```

#### Problem: Slow Simulation Speed

**Symptoms**:
- Simulation steps take too long
- Real-time performance not met
- System becomes unresponsive
- Processing delays accumulate

**Solutions**:

```cpp
def debugSlowSimulation(nlm::Brain& brain) {
    // Measure simulation time
    double startTime = getCurrentTime();
    
    // Run simulation step
    brain.step(0);
    
    double endTime = getCurrentTime();
    double stepTime = (endTime - startTime) * 1000;  // Convert to ms
    
    std::cout << "Simulation step time: " << stepTime << " ms" << std::endl;
    
    if (stepTime > 100) {  // >100ms
        std::cerr << "Error: Simulation too slow" << std::endl;
        
        // Optimize simulation
        optimizeSimulation(brain);
        
        // Check for performance bottlenecks
        identifyPerformanceBottlenecks(brain);
    }
}

void optimizeSimulation(nlm::Brain& brain) {
    // Optimize spike system
    auto* spikeSystem = brain.getSpikeSystem();
    if (spikeSystem) {
        // Optimize spike processing
        // spikeSystem->optimizeProcessing(); // If API available
        
        // Enable parallel processing
        // spikeSystem->enableParallelProcessing(true); // If API available
    }
    
    // Optimize memory access
    optimizeMemoryAccess(brain);
    
    // Optimize plasticity calculations
    optimizePlasticityCalculations(brain);
}

void optimizeMemoryAccess(nlm::Brain& brain) {
    // Configure memory access patterns
    // brain.getMemorySystem()->setAccessPattern(
    //     nlm::AccessPattern::SEQUENTIAL
    // ); // If API available
    
    // Optimize cache usage
    // brain.getMemorySystem()->enableCacheOptimization(true); // If API available
}
```

---

### 8. Integration Issues

#### Problem: Memory System Integration

**Symptoms**:
- Memory systems not working together
- Information not flowing between systems
- Memory access failures
- Data corruption between systems

**Solutions**:

```cpp
def debugMemoryIntegration(nlm::Brain& brain) {
    // Check if all memory systems are available
    auto* workingMem = brain.getWorkingMemory();
    auto* episodicMem = brain.getEpisodicMemory();
    auto* semanticMem = brain.getSemanticMemory();
    auto* proceduralMem = brain.getProceduralMemory();
    auto* associativeMem = brain.getAssociativeMemory();
    
    std::vector<std::string> availableSystems;
    if (workingMem) availableSystems.push_back("Working Memory");
    if (episodicMem) availableSystems.push_back("Episodic Memory");
    if (semanticMem) availableSystems.push_back("Semantic Memory");
    if (proceduralMem) availableSystems.push_back("Procedural Memory");
    if (associativeMem) availableSystems.push_back("Associative Memory");
    
    std::cout << "Available memory systems:" << std::endl;
    for (const auto& system : availableSystems) {
        std::cout << "  - " << system << std::endl;
    }
    
    if (availableSystems.size() < 3) {
        std::cerr << "Error: Not enough memory systems available" << std::endl;
        
        // Check why systems are missing
        checkMissingMemorySystems(brain);
    }
    
    // Test memory system integration
    testMemorySystemIntegration(brain);
}

void checkMissingMemorySystems(nlm::Brain& brain) {
    // Check configuration for memory systems
    auto config = brain.getConfig();
    
    // Check if memory systems are enabled
    if (!config->has("working_memory_capacity")) {
        std::cerr << "Error: Working memory not configured" << std::endl;
    }
    
    if (!config->has("episodic_memory_capacity")) {
        std::cerr << "Error: Episodic memory not configured" << std::endl;
    }
    
    if (!config->has("semantic_memory_capacity")) {
        std::cerr << "Error: Semantic memory not configured" << std::endl;
    }
}

void testMemorySystemIntegration(nlm::Brain& brain) {
    // Test if memory systems work together
    
    // 1. Test working memory -> episodic memory
    auto* workingMem = brain.getWorkingMemory();
    auto* episodicMem = brain.getEpisodicMemory();
    
    if (workingMem && episodicMem) {
        // Store in working memory
        workingMem->store(100, 0.8f);
        
        // Check if episodic memory can access
        if (workingMem->contains(100)) {
            std::cout << "Working memory integration: OK" << std::endl;
        } else {
            std::cerr << "Error: Working memory integration failed" << std::endl;
        }
    }
    
    // 2. Test episodic memory -> semantic memory
    if (episodicMem && semanticMem) {
        // Store in episodic memory
        nlm::EpisodicMemoryItem testEpisode;
        testEpisode.metadata = "Test episode";
        testEpisode.neurons.push_back(200);
        testEpisode.values.push_back(0.9f);
        
        episodicMem->storeEpisode(testEpisode);
        
        // Check if semantic memory can access
        // semanticMem->hasFact("test_episode"); // If API available
    }
    
    // 3. Test semantic memory -> working memory
    if (semanticMem && workingMem) {
        // Store in semantic memory
        // semanticMem->storeFact("test_fact", "test_value"); // If API available
        
        // Check if working memory can access
        // if (workingMem->contains(300)) { // If API available
        //     std::cout << "Semantic memory integration: OK" << std::endl;
        // } else {
        //     std::cerr << "Error: Semantic memory integration failed" << std::endl;
        // }
    }
}
```

---

## Common Error Messages and Solutions

### Error: "Brain initialization failed"

**Solution**:
```cpp
// Fix 1: Check and fix configuration
nlm::Config config;
config.set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::User);
config.set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::User);

nlm::Brain brain(config);
if (!brain.initialize()) {
    // Try alternative approach
    config.set("neuron_count", static_cast<int64_t>(20), nlm::ConfigSource::User);
    nlm::Brain brain2(config);
    if (brain2.initialize()) {
        brain = std::move(brain2);
    }
}
```

### Error: "Working memory full"

**Solution**:
```cpp
// Fix 1: Clear working memory
auto* workingMem = brain.getWorkingMemory();
if (workingMem && workingMem->getCurrentSize() >= workingMem->getCapacity()) {
    workingMem->clear();
}

// Fix 2: Reduce capacity
size_t totalNeurons = brain.getTotalNeuronCount();
size_t newCapacity = std::min(totalNeurons / 10, static_cast<size_t>(200));
// workingMem->setCapacity(newCapacity);
```

### Error: "Prediction error too high"

**Solution**:
```cpp
// Fix 1: Reset prediction system
auto* predictionSys = brain.getPredictionSystem();
if (predictionSys) {
    predictionSys->reset();
}

// Fix 2: Adjust prediction sensitivity
// predictionSys->setSensitivity(0.5f);
```

### Error: "Memory capacity exceeded"

**Solution**:
```cpp
// Fix 1: Clear episodic memory
auto* episodicMem = brain.getEpisodicMemory();
if (episodicMem) {
    episodicMem->clear();
}

// Fix 2: Increase consolidation threshold
// episodicMem->setConsolidationThreshold(0.8f);
```

---

## Summary

### Common Troubleshooting Areas

1. **Initialization**: Configuration validation, resource checks, alternative initialization
2. **Memory Systems**: Capacity management, decay rates, consolidation strategies
3. **Plasticity**: Learning rates, activity-dependent plasticity, developmental plasticity
4. **Prediction**: Prediction accuracy, error thresholds, sensitivity adjustment
5. **Neuromodulation**: Reward systems, curiosity, novelty detection
6. **Agent Control**: Exploration, reward, prediction error management
7. **Performance**: Memory usage, simulation speed, computational optimization
8. **Integration**: Memory system coordination, data flow, system compatibility

### Troubleshooting Workflow

1. **Identify the problem**: Look for specific error messages or symptoms
2. **Check configuration**: Validate configuration settings
3. **Monitor resources**: Check system resources and memory usage
4. **Test individual components**: Test each system separately
5. **Apply fixes**: Try specific fixes for identified issues
6. **Verify solutions**: Test if fixes resolve the problems
7. **Document changes**: Record troubleshooting steps and solutions

By following this Troubleshooting Guide, you can effectively identify and resolve issues with NLM brains across different scenarios and use cases.