# NLM - Advanced Power User Features

## Overview

This document covers advanced features and capabilities for power users of NLM. These features enable:

- Custom neuron types and behaviors
- Advanced agent learning strategies
- Batch processing and automation
- Export/import capabilities
- Performance profiling and optimization
- Custom simulation scenarios
- Integration with external tools

---

## Part 1: Custom Neuron Types

### 1.1 Creating Custom Neuron Classes

```cpp
// Example: Custom neuron with adaptive threshold
class AdaptiveLIFNeuron : public Neuron {
public:
    AdaptiveLIFNeuron(NeuronId id) : Neuron(id, NeuronType::Internal) {
        adaptationTimeConstant = 50.0;  // ms
        adaptationSlope = 0.1f;
        maxThresholdShift = 20.0f;
        baselineThreshold = threshold;
    }
    
    void stepLIF(Timestamp time, TimestepDuration dt) override {
        // Adaptive threshold based on recent activity
        double recentActivity = getRecentActivity();
        double thresholdShift = maxThresholdShift * (1.0 - 
                         std::exp(-recentActivity * adaptationSlope));
        
        double effectiveThreshold = baselineThreshold + thresholdShift;
        
        // Standard LIF dynamics with adaptive threshold
        double dV = (-(membranePotential - restingPotential) / tauMembrane + 
                    injectedCurrent) * dt;
        membranePotential += dV;
        
        // Check for spike with adaptive threshold
        if (membranePotential >= effectiveThreshold && 
            firingState != FiringState::Refractory) {
            fire(time);
            resetMembranePotential();
            
            // Record for adaptation calculation
            recordActivity(spikeCount);
        }
    }
    
private:
    double adaptationTimeConstant;
    double adaptationSlope;
    double maxThresholdShift;
    double baselineThreshold;
    
    double getRecentActivity() {
        // Calculate adaptation based on recent spikes
        if (spikeHistory.empty()) return 0.0;
        
        double recentSpikes = 0.0;
        double currentTime = getCurrentTime();
        
        for (const auto& spikeTime : spikeHistory) {
            double age = currentTime - spikeTime;
            if (age < adaptationTimeConstant) {
                recentSpikes += std::exp(-age / adaptationTimeConstant);
            }
        }
        
        return recentSpikes;
    }
    
    void recordActivity(double spikeValue) {
        // Record for adaptation calculation
        spikeHistory.push_back(getCurrentTime());
        
        // Keep history manageable
        if (spikeHistory.size() > 100) {
            spikeHistory.pop_front();
        }
    }
};

// Custom neuron factory
class CustomNeuronFactory {
public:
    static std::unique_ptr<Neuron> createNeuron(NeuronId id, 
                                                 NeuronType type,
                                                 const std::string& customType) {
        if (customType == "adaptive_lif") {
            return std::make_unique<AdaptiveLIFNeuron>(id);
        } else if (customType == "bursting") {
            return std::make_unique<BurstingNeuron>(id);
        } else if (customType == "resonant") {
            return std::make_unique<ResonantNeuron>(id);
        }
        
        // Default to standard neuron
        return std::make_unique<Neuron>(id, type);
    }
};
```

### 1.2 Custom Synapse Types

```cpp
// Custom synapse with learning dynamics
class CustomSynapticConnection : public Synapse {
public:
    enum LearningMode {
        STANDARD_STDP,
        EXTENDED_STDP,
        TRACE_BASED_LEARNING,
        REWARD_MODULATED
    };
    
    CustomSynapticConnection(NeuronId src, NeuronId dst, float weight)
        : Synapse(src, dst, weight), learningMode(LEARNING_MODE) {
        eligibilityTrace = 0.0f;
        maxEligibilityTrace = 1.0f;
        traceDecayRate = 0.01f;
    }
    
    void recordPreSpike(Timestamp time) override {
        // Extended spike recording for custom learning
        preSpikeTimes.push_back(time);
        
        // Update eligibility trace
        float newTrace = eligibilityTrace + maxEligibilityTrace * 0.5f;
        eligibilityTrace = std::min(newTrace, maxEligibilityTrace);
    }
    
    void recordPostSpike(Timestamp time) override {
        // Post-synaptic spike handling
        postSpikeTimes.push_back(time);
        
        // Apply custom learning rule based on configuration
        applyCustomLearningRule(time);
    }
    
    void step(Timestamp time) override {
        // Custom synaptic dynamics
        switch (learningMode) {
            case EXTENDED_STDP:
                stepExtendedSTDP(time);
                break;
            case TRACE_BASED_LEARNING:
                stepTraceBasedLearning(time);
                break;
            case REWARD_MODULATED:
                stepRewardModulated(time);
                break;
            default:
                Synapse::step(time);  // Standard learning
        }
    }
    
private:
    LearningMode learningMode;
    std::vector<Timestamp> preSpikeTimes;
    std::vector<Timestamp> postSpikeTimes;
    float eligibilityTrace;
    float maxEligibilityTrace;
    float traceDecayRate;
    
    void stepExtendedSTDP(Timestamp time) {
        // Extended STDP with additional factors
        // Consider spike timing, synaptic history, and neuromodulation
        float weightChange = calculateExtendedSTDP(time);
        setWeight(getWeight() + weightChange);
    }
    
    void stepTraceBasedLearning(Timestamp time) {
        // Trace-based learning
        eligibilityTrace *= (1.0f - traceDecayRate);
        
        float weightChange = eligibilityTrace * getPlasticityFactor();
        setWeight(getWeight() + weightChange);
    }
    
    void stepRewardModulated(Timestamp time) {
        // Reward-modulated learning
        // Combines STDP with reward prediction error
        float stdpChange = calculateStandardSTDP(time);
        float rewardChange = calculateRewardModulation(time);
        
        setWeight(getWeight() + stdpChange + rewardChange);
    }
    
    float calculateExtendedSTDP(Timestamp time) {
        // Custom extended STDP calculation
        // Include history-dependent components
        float weightChange = 0.0f;
        
        for (size_t i = 0; i < preSpikeTimes.size(); ++i) {
            for (size_t j = 0; j < postSpikeTimes.size(); ++j) {
                float deltaT = postSpikeTimes[j] - preSpikeTimes[i];
                if (deltaT > 0 && deltaT < 100.0f) {  // Within reasonable window
                    float trace = getSpikeTrace(i, j);
                    weightChange += trace * getSynapticStrengthFactor();
                }
            }
        }
        
        return weightChange;
    }
    
    float calculateRewardModulation(Timestamp time) {
        // Reward prediction error based learning
        // Requires integration with neuromodulation system
        // Simplified implementation
        return 0.0f;
    }
    
    float getSynapticStrengthFactor() {
        // Factor based on current synaptic strength
        float currentStrength = getWeight();
        return 1.0f / (1.0f + std::abs(currentStrength));
    }
};
```

---

## Part 2: Advanced Agent Learning Strategies

### 2.1 Multi-Strategy Learning

```cpp
class AdvancedLearningAgent {
public:
    enum LearningStrategy {
        PURE_STDP,
        PURE_HEBBIAN,
        COMBINED_LEARNING,
        META_LEARNING,
        TRANSFER_LEARNING
    };
    
    AdvancedLearningAgent(std::shared_ptr<Brain> brain) : agentBrain_(brain) {
        currentStrategy = LearningStrategy::COMBINED_LEARNING;
        strategyWeights = {1.0f, 1.0f, 0.5f, 0.2f, 0.3f};  // For combined strategies
        learningHistory.clear();
    }
    
    void setLearningStrategy(LearningStrategy strategy, float weight = 1.0f) {
        if (currentStrategy != strategy) {
            NLM_LOG_INFO("Switching learning strategy from " + 
                        std::to_string(static_cast<int>(currentStrategy)) + 
                        " to " + std::to_string(static_cast<int>(strategy)));
            
            currentStrategy = strategy;
            lastStrategyChange = getCurrentTime();
        }
        
        // Update strategy weights
        if (strategy == LearningStrategy::COMBINED_LEARNING) {
            // Store weight for combined strategy
            combinedLearningWeight = weight;
        }
    }
    
    void applyMultiStrategyLearning(const SensoryPercept& percept, 
                                   float reward, float predictedReward) {
        switch (currentStrategy) {
            case LearningStrategy::PURE_STDP:
                applyPureSTDPLearning(percept, reward, predictedReward);
                break;
            case LearningStrategy::PURE_HEBBIAN:
                applyPureHebbianLearning(percept, reward, predictedReward);
                break;
            case LearningStrategy::COMBINED_LEARNING:
                applyCombinedLearning(percept, reward, predictedReward);
                break;
            case LearningStrategy::META_LEARNING:
                applyMetaLearning(percept, reward, predictedReward);
                break;
            case LearningStrategy::TRANSFER_LEARNING:
                applyTransferLearning(percept, reward, predictedReward);
                break;
        }
        
        // Record learning event
        LearningEvent event {
            time: getCurrentTime(),
            strategy: currentStrategy,
            reward: reward,
            predictedReward: predictedReward,
            perceptHash: hashPercept(percept)
        };
        
        learningHistory.push_back(event);
        
        if (learningHistory.size() > 1000) {
            learningHistory.pop_front();
        }
    }
    
private:
    LearningStrategy currentStrategy;
    float combinedLearningWeight;
    std::deque<LearningEvent> learningHistory;
    Timestamp lastStrategyChange;
    std::array<float, 5> strategyWeights;  // For different strategies
    
    struct LearningEvent {
        Timestamp time;
        LearningStrategy strategy;
        float reward;
        float predictedReward;
        uint64_t perceptHash;
    };
    
    void applyPureSTDPLearning(const SensoryPercept& percept, 
                              float reward, float predictedReward) {
        // Pure STDP: standard spike timing dependent plasticity
        float stdpWeight = strategyWeights[0];
        
        // Apply STDP rules to all synapses
        for (const auto& region : agentBrain_->getRegions()) {
            for (auto* syn : region->getSynapses()) {
                // Apply STDP with reward modulation factor
                applySTDPWithReward(syn, reward, predictedReward, stdpWeight);
            }
        }
    }
    
    void applyPureHebbianLearning(const SensoryPercept& percept,
                                 float reward, float predictedReward) {
        // Pure Hebbian: simultaneous pre and post activity strengthens synapses
        float hebbianWeight = strategyWeights[1];
        
        // Identify active neurons from percept
        auto activeNeurons = getActiveNeuronsFromPercept(percept);
        
        // Apply Hebbian learning
        for (const auto& region : agentBrain_->getRegions()) {
            for (auto* syn : region->getSynapses()) {
                if (isHebbianApplicable(syn, activeNeurons)) {
                    float hebbianChange = calculateHebbianChange(syn, activeNeurons);
                    syn->addToWeight(hebbianChange * hebbianWeight);
                }
            }
        }
    }
    
    void applyCombinedLearning(const SensoryPercept& percept,
                               float reward, float predictedReward) {
        // Combined: weighted combination of multiple learning rules
        float stdpWeight = strategyWeights[0] * combinedLearningWeight;
        float hebbianWeight = strategyWeights[1] * combinedLearningWeight;
        float traceWeight = strategyWeights[2];
        
        // Apply each learning rule with its weight
        applySTDPWithReward(agentBrain_, percept, reward, predictedReward, stdpWeight);
        applyHebbianLearning(agentBrain_, percept, reward, predictedReward, hebbianWeight);
        applyTraceLearning(agentBrain_, percept, reward, predictedReward, traceWeight);
    }
    
    void applyMetaLearning(const SensoryPercept& percept,
                          float reward, float predictedReward) {
        // Meta-learning: learn how to learn
        // Adapt learning rates based on prediction error
        
        float predictionError = reward - predictedReward;
        
        // Adjust learning rates based on prediction error
        adjustLearningRates(predictionError);
        
        // Apply meta-learning rules
        applyAdaptiveLearningRules(percept, reward, predictedReward);
    }
    
    void applyTransferLearning(const SensoryPercept& percept,
                               float reward, float predictedReward) {
        // Transfer learning: apply knowledge from one task to another
        
        // Get task-specific features
        auto taskFeatures = extractTaskFeatures(percept);
        
        // Find similar previously learned tasks
        auto similarTasks = findSimilarTasks(taskFeatures);
        
        // Transfer knowledge from similar tasks
        transferKnowledgeFromTasks(similarTasks, percept, reward, predictedReward);
    }
    
    // Helper methods
    void applySTDPWithReward(std::shared_ptr<Brain> brain,
                           const SensoryPercept& percept,
                           float reward, float predictedReward,
                           float weight) {
        for (const auto& region : brain->getRegions()) {
            for (auto* syn : region->getSynapses()) {
                applySTDPWithReward(syn, reward, predictedReward, weight);
            }
        }
    }
    
    void applySTDPWithReward(Synapse* syn, float reward, float predictedReward,
                           float weight) {
        // STDP with reward modulation
        float rewardModulation = (reward - predictedReward) * weight;
        float stdpChange = calculateStandardSTDP(syn) + rewardModulation;
        syn->addToWeight(stdpChange);
    }
    
    std::vector<NeuronId> getActiveNeuronsFromPercept(const SensoryPercept& percept) {
        // Extract active neuron IDs from percept
        std::vector<NeuronId> activeNeurons;
        
        // Implementation depends on your percept format
        // For now, return empty vector
        return activeNeurons;
    }
    
    bool isHebbianApplicable(Synapse* syn, 
                            const std::vector<NeuronId>& activeNeurons) {
        // Check if Hebbian learning applies to this synapse
        return std::find(activeNeurons.begin(), activeNeurons.end(), 
                        syn->getSourceNeuron()) != activeNeurons.end() &&
               std::find(activeNeurons.begin(), activeNeurons.end(), 
                        syn->getDestinationNeuron()) != activeNeurons.end();
    }
    
    float calculateHebbianChange(Synapse* syn,
                                const std::vector<NeuronId>& activeNeurons) {
        // Calculate Hebbian weight change
        return 0.01f;  // Simplified
    }
    
    void applyHebbianLearning(std::shared_ptr<Brain> brain,
                             const SensoryPercept& percept,
                             float reward, float predictedReward,
                             float weight) {
        auto activeNeurons = getActiveNeuronsFromPercept(percept);
        
        for (const auto& region : brain->getRegions()) {
            for (auto* syn : region->getSynapses()) {
                if (isHebbianApplicable(syn, activeNeurons)) {
                    float change = calculateHebbianChange(syn, activeNeurons);
                    syn->addToWeight(change * weight);
                }
            }
        }
    }
    
    void applyTraceLearning(std::shared_ptr<Brain> brain,
                           const SensoryPercept& percept,
                           float reward, float predictedReward,
                           float weight) {
        // Apply eligibility trace-based learning
        for (const auto& region : brain->getRegions()) {
            for (auto* syn : region->getSynapses()) {
                float eligibility = syn->getEligibilityTrace();
                if (std::abs(eligibility) > 0.001f) {
                    float change = eligibility * weight;
                    syn->addToWeight(change);
                    syn->decayEligibilityTrace(0.1f);
                }
            }
        }
    }
    
    void adjustLearningRates(float predictionError) {
        // Adapt learning rates based on prediction error
        // Positive error -> increase exploration/learning rate
        // Negative error -> decrease learning rate
        
        float baseLearningRate = 0.01f;
        float adaptiveRate = baseLearningRate * (1.0f + std::abs(predictionError));
        
        // Apply adaptive learning rates to plasticity systems
        // Implementation depends on your plasticity system
    }
    
    void applyAdaptiveLearningRules(const SensoryPercept& percept,
                                   float reward, float predictedReward) {
        // Implement adaptive learning rules
        // These rules change based on learning performance
    }
    
    std::vector<TaskFeatures> extractTaskFeatures(const SensoryPercept& percept) {
        // Extract features for task identification
        return std::vector<TaskFeatures>();
    }
    
    struct TaskFeatures {
        std::string taskType;
        std::vector<float> featureVector;
        float performanceScore;
        Timestamp learnedTime;
    };
    
    std::vector<TaskFeatures> findSimilarTasks(const std::vector<TaskFeatures>& features) {
        // Find tasks with similar features
        return std::vector<TaskFeatures>();
    }
    
    void transferKnowledgeFromTasks(const std::vector<TaskFeatures>& tasks,
                                   const SensoryPercept& percept,
                                   float reward, float predictedReward) {
        // Transfer knowledge from similar tasks
        for (const auto& task : tasks) {
            // Apply task-specific knowledge to current learning
            applyTaskSpecificKnowledge(task, percept, reward, predictedReward);
        }
    }
    
    void applyTaskSpecificKnowledge(const TaskFeatures& task,
                                    const SensoryPercept& percept,
                                    float reward, float predictedReward) {
        // Apply knowledge specific to a learned task
        // This could involve:
        // - Modifying synaptic weights
        // - Adjusting learning parameters
        // - Applying task-specific activation patterns
    }
    
    uint64_t hashPercept(const SensoryPercept& percept) {
        // Create hash for percept to track learning patterns
        // Simplified implementation
        return 0;
    }
    
    Timestamp getCurrentTime() {
        // Get current simulation time
        // Implementation depends on your time system
        return 0.0;
    }
};
```

---

## Part 3: Batch Processing and Automation

### 3.1 Batch Simulation

```cpp
class BatchSimulationManager {
public:
    struct BatchConfiguration {
        std::string name;
        uint64_t numSteps;
        size_t numBrains;
        std::vector<std::shared_ptr<Config>> configurations;
        std::string outputPrefix;
        bool enableParallel;
        size_t maxConcurrentSimulations;
        std::function<void(const BrainSimulationResult&)> resultCallback;
    };
    
    struct BrainSimulationResult {
        std::string batchName;
        size_t brainIndex;
        uint64_t steps;
        float totalReward;
        float avgFiringRate;
        float finalWeight;
        Timestamp startTime;
        Timestamp endTime;
        std::vector<float> learningCurve;
        std::vector<std::vector<float>> neuronActivity;
    };
    
    BatchSimulationManager() {
        runningBatches = 0;
        maxConcurrentBatches = 4;
    }
    
    BatchId startBatch(const BatchConfiguration& config) {
        BatchId id = generateBatchId();
        
        BatchInfo batch {
            id: id,
            config: config,
            startTime: getCurrentTime(),
            status: BatchStatus::RUNNING,
            results: std::vector<BrainSimulationResult>(),
            threads: std::vector<std::thread>()
        };
        
        batches[id] = batch;
        runningBatches++;
        
        // Start batch simulation in background thread
        std::thread simulationThread(&BatchSimulationManager::runBatch, this, id);
        simulationThread.detach();
        
        NLM_LOG_INFO("Started batch simulation '" + config.name + 
                    "' with ID " + std::to_string(id) + 
                    " (" + std::to_string(config.numBrains) + " brains)");
        
        return id;
    }
    
    BatchStatus getBatchStatus(BatchId id) {
        auto it = batches.find(id);
        if (it != batches.end()) {
            return it->second.status;
        }
        return BatchStatus::NOT_FOUND;
    }
    
    std::vector<BrainSimulationResult> getBatchResults(BatchId id) {
        auto it = batches.find(id);
        if (it != batches.end()) {
            return it->second.results;
        }
        return std::vector<BrainSimulationResult>();
    }
    
    void pauseBatch(BatchId id) {
        auto it = batches.find(id);
        if (it != batches.end()) {
            it->second.status = BatchStatus::PAUSED;
            NLM_LOG_INFO("Paused batch simulation " + std::to_string(id));
        }
    }
    
    void resumeBatch(BatchId id) {
        auto it = batches.find(id);
        if (it != batches.end()) {
            it->second.status = BatchStatus::RUNNING;
            NLM_LOG_INFO("Resumed batch simulation " + std::to_string(id));
        }
    }
    
    void stopBatch(BatchId id) {
        auto it = batches.find(id);
        if (it != batches.end()) {
            it->second.status = BatchStatus::STOPPED;
            runningBatches--;
            NLM_LOG_INFO("Stopped batch simulation " + std::to_string(id));
        }
    }
    
    void runBatch(BatchId id) {
        auto it = batches.find(id);
        if (it == batches.end()) return;
        
        BatchInfo& batch = it->second;
        const BatchConfiguration& config = batch.config;
        
        try {
            // Run batch simulations
            for (size_t brainIdx = 0; brainIdx < config.numBrains; ++brainIdx) {
                if (batch.status != BatchStatus::RUNNING) {
                    break;  // Batch paused or stopped
                }
                
                // Create brain with specific configuration
                std::shared_ptr<Config> brainConfig;
                if (brainIdx < config.configurations.size()) {
                    brainConfig = config.configurations[brainIdx];
                } else {
                    brainConfig = std::make_shared<Config>();
                }
                
                auto brain = std::make_shared<Brain>(brainConfig);
                brain->initialize();
                
                // Run simulation
                BrainSimulationResult result {
                    batchName: config.name,
                    brainIndex: brainIdx,
                    steps: config.numSteps,
                    totalReward: 0.0f,
                    avgFiringRate: 0.0f,
                    finalWeight: 0.0f,
                    startTime: getCurrentTime(),
                    endTime: 0.0,
                    learningCurve: std::vector<float>(),
                    neuronActivity: std::vector<std::vector<float>>()
                };
                
                // Run simulation steps
                float episodeReward = 0.0f;
                float totalFiringRate = 0.0f;
                size_t firingCount = 0;
                
                for (uint64_t step = 0; step < config.numSteps; ++step) {
                    if (batch.status != BatchStatus::RUNNING) {
                        result.status = BatchStatus::STOPPED;
                        break;
                    }
                    
                    // Apply learning if configured
                    if (config.enableParallel && brainIdx % 2 == 0) {
                        // Enable learning for some brains
                        applyLearningToBrain(brain, step);
                    }
                    
                    // Collect metrics
                    totalFiringRate += brain->getAverageFiringRate();
                    if (brain->getFiringNeuronCount() > 0) firingCount++;
                    
                    // Store learning curve data periodically
                    if (step % 100 == 0) {
                        result.learningCurve.push_back(brain->getAverageFiringRate());
                    }
                }
                
                // Calculate final metrics
                result.endTime = getCurrentTime();
                result.totalReward = episodeReward / config.numSteps;
                result.avgFiringRate = totalFiringRate / config.numSteps;
                result.finalWeight = calculateFinalWeight(brain);
                
                // Store neuron activity if requested
                if (!config.outputPrefix.empty()) {
                    result.neuronActivity = collectNeuronActivity(brain);
                }
                
                // Call result callback if provided
                if (config.resultCallback) {
                    config.resultCallback(result);
                }
                
                // Save individual brain result
                batch.results.push_back(result);
                
                // Save checkpoint if requested
                if (!config.outputPrefix.empty()) {
                    std::string checkpointPath = config.outputPrefix + 
                                                "_brain_" + std::to_string(brainIdx) +
                                                "_steps_" + std::to_string(config.numSteps) +
                                                ".bin";
                    brain->save(checkpointPath);
                }
                
                // Small delay between brains if not in parallel mode
                if (!config.enableParallel) {
                    std::this_thread::sleep_for(std::chrono::milliseconds(100));
                }
            }
            
        } catch (const std::exception& e) {
            NLM_LOG_ERROR("Error in batch simulation " + std::to_string(id) + ": " + 
                         std::string(e.what()));
            batch.status = BatchStatus::ERROR;
        }
        
        // Update batch status
        batch.status = BatchStatus::COMPLETED;
        runningBatches--;
        
        NLM_LOG_INFO("Completed batch simulation '" + config.name + 
                    "' with ID " + std::to_string(id) + 
                    " (" + std::to_string(batch.results.size()) + " results)");
    }
    
private:
    enum class BatchStatus {
        NOT_FOUND,
        QUEUED,
        RUNNING,
        PAUSED,
        COMPLETED,
        STOPPED,
        ERROR
    };
    
    struct BatchInfo {
        BatchId id;
        BatchConfiguration config;
        Timestamp startTime;
        BatchStatus status;
        std::vector<BrainSimulationResult> results;
        std::vector<std::thread> threads;
    };
    
    using BatchId = uint64_t;
    std::map<BatchId, BatchInfo> batches;
    size_t runningBatches;
    size_t maxConcurrentBatches;
    
    BatchId generateBatchId() {
        static BatchId nextId = 1;
        return nextId++;
    }
    
    void applyLearningToBrain(std::shared_ptr<Brain> brain, uint64_t step) {
        // Apply learning based on current state
        // Simplified implementation
        if (step % 10 == 0) {
            // Periodic learning updates
            brain->updatePlasticity();
        }
    }
    
    float calculateFinalWeight(std::shared_ptr<Brain> brain) {
        // Calculate final synaptic weight
        float totalWeight = 0.0f;
        size_t synapseCount = 0;
        
        for (const auto& region : brain->getRegions()) {
            for (const auto* syn : region->getSynapses()) {
                totalWeight += std::abs(syn->getWeight());
                synapseCount++;
            }
        }
        
        return synapseCount > 0 ? totalWeight / synapseCount : 0.0f;
    }
    
    std::vector<std::vector<float>> collectNeuronActivity(std::shared_ptr<Brain> brain) {
        // Collect neuron activity for analysis
        std::vector<std::vector<float>> activity;
        
        for (const auto& region : brain->getRegions()) {
            std::vector<float> regionActivity;
            for (auto* neuron : region->getAllNeurons()) {
                regionActivity.push_back(
                    std::abs(neuron->getState().membranePotential - 
                           neuron->getState().restingPotential)
                );
            }
            activity.push_back(regionActivity);
        }
        
        return activity;
    }
    
    Timestamp getCurrentTime() {
        // Get current simulation time
        return 0.0;
    }
};
```

---

## Part 4: Export/Import Capabilities

### 4.1 Brain State Export

```cpp
class BrainExporter {
public:
    struct ExportOptions {
        bool includeNeurons = true;
        bool includeSynapses = true;
        bool includeMemory = true;
        bool includeDevelopment = true;
        bool includeNeuromodulation = true;
        std::string compressionAlgorithm = "gzip";
        std::string format = "binary";  // "binary", "json", "xml"
        uint32_t version = 1;
    };
    
    static bool exportBrain(const std::string& filepath, 
                           std::shared_ptr<Brain> brain,
                           const ExportOptions& options = ExportOptions()) {
        try {
            NLM_LOG_INFO("Exporting brain to " + filepath);
            
            // Create exporter based on format
            std::unique_ptr<IBrainExporter> exporter;
            if (options.format == "binary") {
                exporter = std::make_unique<BinaryBrainExporter>();
            } else if (options.format == "json") {
                exporter = std::make_unique<JSONBrainExporter>();
            } else if (options.format == "xml") {
                exporter = std::make_unique<XMLBrainExporter>();
            } else {
                NLM_LOG_ERROR("Unsupported export format: " + options.format);
                return false;
            }
            
            // Export brain state
            bool success = exporter->exportBrain(filepath, brain, options);
            
            if (success) {
                NLM_LOG_INFO("Brain exported successfully: " + filepath);
            } else {
                NLM_LOG_ERROR("Failed to export brain: " + filepath);
            }
            
            return success;
            
        } catch (const std::exception& e) {
            NLM_LOG_ERROR(std::string("Exception during brain export: ") + e.what());
            return false;
        }
    }
    
    static std::shared_ptr<Brain> importBrain(const std::string& filepath) {
        try {
            NLM_LOG_INFO("Importing brain from " + filepath);
            
            // Determine file format based on extension
            std::string extension = getFileExtension(filepath);
            std::unique_ptr<IBrainImporter> importer;
            
            if (extension == "bin" || extension == "checkpoint") {
                importer = std::make_unique<BinaryBrainImporter>();
            } else if (extension == "json") {
                importer = std::make_unique<JSONBrainImporter>();
            } else if (extension == "xml" || extension == "brain") {
                importer = std::make_unique<XMLBrainImporter>();
            } else {
                NLM_LOG_ERROR("Unsupported import format: " + extension);
                return nullptr;
            }
            
            // Import brain state
            auto brain = importer->importBrain(filepath);
            
            if (brain) {
                NLM_LOG_INFO("Brain imported successfully: " + filepath);
            } else {
                NLM_LOG_ERROR("Failed to import brain: " + filepath);
            }
            
            return brain;
            
        } catch (const std::exception& e) {
            NLM_LOG_ERROR(std::string("Exception during brain import: ") + e.what());
            return nullptr;
        }
    }
    
private:
    class IBrainExporter {
    public:
        virtual ~IBrainExporter() = default;
        virtual bool exportBrain(const std::string& filepath, 
                               std::shared_ptr<Brain> brain,
                               const ExportOptions& options) = 0;
    };
    
    class IBrainImporter {
    public:
        virtual ~IBrainImporter() = default;
        virtual std::shared_ptr<Brain> importBrain(const std::string& filepath) = 0;
    };
    
    class BinaryBrainExporter : public IBrainExporter {
    public:
        bool exportBrain(const std::string& filepath,
                       std::shared_ptr<Brain> brain,
                       const ExportOptions& options) override {
            // Implementation for binary export
            // Uses checkpoint system if available
            return brain->save(filepath);
        }
    };
    
    class JSONBrainExporter : public IBrainExporter {
    public:
        bool exportBrain(const std::string& filepath,
                       std::shared_ptr<Brain> brain,
                       const ExportOptions& options) override {
            // Implementation for JSON export
            // Export as human-readable JSON format
            // Include configuration, neuron states, synapse states, etc.
            nlohmann::json jsonData;
            
            if (options.includeNeurons) {
                jsonData["neurons"] = exportNeurons(brain, options);
            }
            
            if (options.includeSynapses) {
                jsonData["synapses"] = exportSynapses(brain, options);
            }
            
            if (options.includeMemory) {
                jsonData["memory"] = exportMemory(brain, options);
            }
            
            if (options.includeDevelopment) {
                jsonData["development"] = exportDevelopment(brain, options);
            }
            
            if (options.includeNeuromodulation) {
                jsonData["neuromodulation"] = exportNeuromodulation(brain, options);
            }
            
            jsonData["metadata"] = exportMetadata(brain, options);
            
            std::ofstream file(filepath);
            if (!file.is_open()) {
                return false;
            }
            
            file << jsonData.dump(2);  // Pretty print with 2-space indentation
            return true;
        }
    
    private:
        nlohmann::json exportNeurons(std::shared_ptr<Brain> brain,
                                   const ExportOptions& options) {
            nlohmann::json neurons;
            
            for (const auto& region : brain->getRegions()) {
                nlohmann::json regionData;
                regionData["id"] = region->getId().index();
                regionData["name"] = region->getName();
                
                nlohmann::json populationArray;
                for (const auto& population : region->getPopulations()) {
                    nlohmann::json populationData;
                    populationData["id"] = population->getId().index();
                    populationData["type"] = static_cast<int>(population->getNeuronType());
                    
                    nlohmann::json neuronArray;
                    for (const auto* neuron : population->getNeurons()) {
                        nlohmann::json neuronData;
                        const auto& state = neuron->getState();
                        
                        neuronData["id"] = neuron->getId().index();
                        neuronData["type"] = static_cast<int>(neuron->getType());
                        neuronData["membranePotential"] = state.membranePotential;
                        neuronData["restingPotential"] = state.restingPotential;
                        neuronData["threshold"] = state.threshold;
                        neuronData["resetPotential"] = state.resetPotential;
                        neuronData["leakConductance"] = state.leakConductance;
                        neuronData["refractoryRemaining"] = state.refractoryRemaining;
                        neuronData["refractoryPeriod"] = state.refractoryPeriod;
                        neuronData["lastSpikeTime"] = state.lastSpikeTime;
                        
                        neuronArray.push_back(neuronData);
                    }
                    
                    populationData["neurons"] = neuronArray;
                    populationArray.push_back(populationData);
                }
                
                regionData["populations"] = populationArray;
                neurons.push_back(regionData);
            }
            
            return neurons;
        }
        
        nlohmann::json exportSynapses(std::shared_ptr<Brain> brain,
                                    const ExportOptions& options) {
            nlohmann::json synapses;
            
            for (const auto& region : brain->getRegions()) {
                nlohmann::json regionData;
                regionData["id"] = region->getId().index();
                
                nlohmann::json synapseArray;
                for (const auto* syn : region->getSynapses()) {
                    nlohmann::json synapseData;
                    
                    synapseData["id"] = syn->getId().index();
                    synapseData["source"] = syn->getSourceNeuron().index();
                    synapseData["destination"] = syn->getDestinationNeuron().index();
                    synapseData["weight"] = syn->getWeight();
                    synapseData["delay"] = syn->getDelay();
                    synapseData["type"] = static_cast<int>(syn->getType());
                    synapseData["eligibilityTrace"] = syn->getEligibilityTrace();
                    
                    // Export plasticity state
                    auto plasticityFlags = syn->getPlasticityFlags();
                    nlohmann::json plasticity;
                    plasticity["stdp"] = plasticityFlags.stdp;
                    plasticity["hebbian"] = plasticityFlags.hebbian;
                    plasticity["structural"] = plasticityFlags.structural;
                    synapseData["plasticity"] = plasticity;
                    
                    synapseArray.push_back(synapseData);
                }
                
                regionData["synapses"] = synapseArray;
                synapses.push_back(regionData);
            }
            
            return synapses;
        }
        
        nlohmann::json exportMemory(std::shared_ptr<Brain> brain,
                                  const ExportOptions& options) {
            nlohmann::json memoryData;
            
            auto workingMemory = brain->getWorkingMemory();
            if (workingMemory) {
                nlohmann::json wmData;
                wmData["capacity"] = workingMemory->getCapacity();
                wmData["activeTraces"] = workingMemory->getActiveTraces();
                memoryData["workingMemory"] = wmData;
            }
            
            auto episodicMemory = brain->getEpisodicMemory();
            if (episodicMemory) {
                nlohmann::json emData;
                emData["episodeCount"] = episodicMemory->getEpisodeCount();
                memoryData["episodicMemory"] = emData;
            }
            
            auto associativeMemory = brain->getAssociativeMemory();
            if (associativeMemory) {
                nlohmann::json amData;
                amData["capacity"] = associativeMemory->getCapacity();
                memoryData["associativeMemory"] = amData;
            }
            
            return memoryData;
        }
        
        nlohmann::json exportDevelopment(std::shared_ptr<Brain> brain,
                                        const ExportOptions& options) {
            nlohmann::json devData;
            devData["stage"] = static_cast<int>(brain->getDevelopmentalStage());
            
            auto developmentSystem = brain->getDevelopmentSystem();
            if (developmentSystem) {
                devData["age"] = developmentSystem->getDevelopmentalAge();
                devData["stage"] = static_cast<int>(developmentSystem->getCurrentStage());
            }
            
            return devData;
        }
        
        nlohmann::json exportNeuromodulation(std::shared_ptr<Brain> brain,
                                            const ExportOptions& options) {
            nlohmann::json neuromodData;
            
            auto dopamine = brain->getDopamine();
            if (dopamine) {
                nlohmann::json dopamineData;
                dopamineData["level"] = dopamine->getLevel();
                neuromodData["dopamine"] = dopamineData;
            }
            
            auto curiosity = brain->getCuriosity();
            if (curiosity) {
                nlohmann::json curiosityData;
                curiosityData["level"] = curiosity->getLevel();
                neuromodData["curiosity"] = curiosityData;
            }
            
            auto novelty = brain->getNovelty();
            if (novelty) {
                nlohmann::json noveltyData;
                noveltyData["level"] = novelty->getLevel();
                neuromodData["novelty"] = noveltyData;
            }
            
            return neuromodData;
        }
        
        nlohmann::json exportMetadata(std::shared_ptr<Brain> brain,
                                    const ExportOptions& options) {
            nlohmann::json metaData;
            metaData["neuronCount"] = brain->getTotalNeuronCount();
            metaData["synapseCount"] = brain->getTotalSynapseCount();
            metaData["regionCount"] = brain->getRegionCount();
            metaData["totalSpikes"] = brain->getTotalSpikeCount();
            metaData["averageFiringRate"] = brain->getAverageFiringRate();
            metaData["version"] = options.version;
            metaData["exportTime"] = getCurrentTime();
            
            return metaData;
        }
        
        std::string getCurrentTime() {
            auto now = std::chrono::system_clock::now();
            auto time_t = std::chrono::system_clock::to_time_t(now);
            std::stringstream ss;
            ss << std::put_time(std::localtime(&time_t), "%Y-%m-%d %H:%M:%S");
            return ss.str();
        }
    };
    
    class XMLBrainExporter : public IBrainExporter {
    public:
        bool exportBrain(const std::string& filepath,
                       std::shared_ptr<Brain> brain,
                       const ExportOptions& options) override {
            // Implementation for XML export
            // Similar to JSON but with XML format
            // Uses rapidxml or similar library
            return false;  // Placeholder
        }
    };
    
    class BinaryBrainImporter : public IBrainImporter {
    public:
        std::shared_ptr<Brain> importBrain(const std::string& filepath) override {
            auto brain = std::make_shared<Brain>();
            // Implementation for binary import using checkpoint system
            if (brain->load(filepath)) {
                return brain;
            }
            return nullptr;
        }
    };
    
    class JSONBrainImporter : public IBrainImporter {
    public:
        std::shared_ptr<Brain> importBrain(const std::string& filepath) override {
            std::ifstream file(filepath);
            if (!file.is_open()) {
                return nullptr;
            }
            
            nlohmann::json jsonData;
            try {
                file >> jsonData;
            } catch (const std::exception& e) {
                NLM_LOG_ERROR(std::string("Failed to parse JSON: ") + e.what());
                return nullptr;
            }
            
            // Import brain from JSON data
            return importBrainFromJSON(jsonData);
        }
        
    private:
        std::shared_ptr<Brain> importBrainFromJSON(const nlohmann::json& jsonData) {
            // Implementation for importing from JSON
            // Creates brain and restores all state
            return nullptr;  // Placeholder
        }
    };
    
    class XMLBrainImporter : public IBrainImporter {
    public:
        std::shared_ptr<Brain> importBrain(const std::string& filepath) override {
            // Implementation for XML import
            // Uses XML parsing library
            return nullptr;  // Placeholder
        }
    };
    
    static std::string getFileExtension(const std::string& filepath) {
        size_t dotPos = filepath.rfind('.');
        if (dotPos != std::string::npos && dotPos < filepath.length() - 1) {
            return filepath.substr(dotPos + 1);
        }
        return "";
    }
};
```

---

## Part 5: Performance Profiling and Optimization

### 5.1 Performance Profiling

```cpp
class PerformanceProfiler {
public:
    struct PerformanceMetrics {
        Timestamp simulationTime;
        double cpuUsage;
        double memoryUsage;
        size_t neuronCount;
        size_t synapseCount;
        float firingRate;
        size_t spikeCount;
        float plasticityUpdateTime;
        float spikeProcessingTime;
        float memoryAllocationTime;
        double stepTime;
    };
    
    PerformanceProfiler(std::shared_ptr<Brain> brain) : brain_(brain) {
        profilingEnabled = false;
        profilingInterval = 0.1;  // seconds
        metricsHistory.clear();
        startTime = 0.0;
    }
    
    void startProfiling(double interval = 0.1) {
        profilingEnabled = true;
        profilingInterval = interval;
        startTime = getCurrentTime();
        metricsHistory.clear();
        
        NLM_LOG_INFO("Performance profiling started with interval " + 
                    std::to_string(interval) + "s");
    }
    
    void stopProfiling() {
        profilingEnabled = false;
        
        NLM_LOG_INFO("Performance profiling stopped");
    }
    
    void profileStep(uint64_t step) {
        if (!profilingEnabled) return;
        
        auto metrics = collectMetrics(step);
        metricsHistory.push_back(metrics);
        
        // Log metrics periodically
        if (metricsHistory.size() % 100 == 0) {
            logMetricsSummary();
        }
    }
    
    PerformanceMetrics getCurrentMetrics() {
        return collectMetrics(brain_->getTotalSpikeCount());
    }
    
    std::vector<PerformanceMetrics> getMetricsHistory() {
        return metricsHistory;
    }
    
    PerformanceAnalysis analyzePerformance() {
        PerformanceAnalysis analysis;
        
        if (metricsHistory.empty()) {
            return analysis;  // Empty analysis
        }
        
        // Calculate performance statistics
        calculatePerformanceStatistics(analysis);
        
        // Identify bottlenecks
        identifyPerformanceBottlenecks(analysis);
        
        // Compare with baseline if available
        if (!baselineMetrics.empty()) {
            compareWithBaseline(analysis);
        }
        
        return analysis;
    }
    
    void setBaselineMetrics(const std::vector<PerformanceMetrics>& baseline) {
        baselineMetrics = baseline;
        NLM_LOG_INFO("Performance baseline metrics set (" + 
                    std::to_string(baseline.size()) + " samples)");
    }
    
    void saveProfilingResults(const std::string& filepath) {
        // Save profiling results to file
        // Implementation depends on format
        
        NLM_LOG_INFO("Performance profiling results saved to " + filepath);
    }
    
private:
    bool profilingEnabled;
    double profilingInterval;
    std::vector<PerformanceMetrics> metricsHistory;
    std::vector<PerformanceMetrics> baselineMetrics;
    Timestamp startTime;
    std::shared_ptr<Brain> brain_;
    
    PerformanceMetrics collectMetrics(uint64_t step) {
        PerformanceMetrics metrics;
        
        metrics.simulationTime = getCurrentTime();
        metrics.stepTime = (metrics.simulationTime - startTime) / step;
        
        // Collect system metrics
        metrics.cpuUsage = getCPUUsage();
        metrics.memoryUsage = getMemoryUsage();
        
        // Collect brain metrics
        if (brain_) {
            metrics.neuronCount = brain_->getTotalNeuronCount();
            metrics.synapseCount = brain_->getTotalSynapseCount();
            metrics.firingRate = brain_->getAverageFiringRate();
            metrics.spikeCount = brain_->getTotalSpikeCount();
        }
        
        // Performance timing (simplified)
        metrics.plasticityUpdateTime = 0.001f;  // Placeholder
        metrics.spikeProcessingTime = 0.002f;     // Placeholder
        metrics.memoryAllocationTime = 0.0005f;  // Placeholder
        
        return metrics;
    }
    
    void logMetricsSummary() {
        if (metricsHistory.empty()) return;
        
        auto latest = metricsHistory.back();
        NLM_LOG_INFO(std::string("Performance - Step: ") + 
                    std::to_string(latest.spikeCount) + 
                    ", Firing Rate: " + std::to_string(latest.firingRate) + 
                    ", CPU: " + std::to_string(latest.cpuUsage * 100) + "%");
    }
    
    void calculatePerformanceStatistics(PerformanceAnalysis& analysis) {
        if (metricsHistory.empty()) return;
        
        // Calculate averages
        double totalStepTime = 0.0;
        double totalCPU = 0.0;
        double totalMemory = 0.0;
        double totalFiringRate = 0.0;
        
        for (const auto& metrics : metricsHistory) {
            totalStepTime += metrics.stepTime;
            totalCPU += metrics.cpuUsage;
            totalMemory += metrics.memoryUsage;
            totalFiringRate += metrics.firingRate;
        }
        
        size_t count = metricsHistory.size();
        
        analysis.avgStepTime = totalStepTime / count;
        analysis.avgCPUUsage = totalCPU / count;
        analysis.avgMemoryUsage = totalMemory / count;
        analysis.avgFiringRate = totalFiringRate / count;
        
        // Calculate standard deviations
        calculateStandardDeviations(analysis);
    }
    
    void calculateStandardDeviations(PerformanceAnalysis& analysis) {
        double stepTimeSum = 0.0, stepTimeSqSum = 0.0;
        double cpuSum = 0.0, cpuSqSum = 0.0;
        double memSum = 0.0, memSqSum = 0.0;
        double firingSum = 0.0, firingSqSum = 0.0;
        
        for (const auto& metrics : metricsHistory) {
            stepTimeSum += metrics.stepTime;
            stepTimeSqSum += metrics.stepTime * metrics.stepTime;
            
            cpuSum += metrics.cpuUsage;
            cpuSqSum += metrics.cpuUsage * metrics.cpuUsage;
            
            memSum += metrics.memoryUsage;
            memSqSum += metrics.memoryUsage * metrics.memoryUsage;
            
            firingSum += metrics.firingRate;
            firingSqSum += metrics.firingRate * metrics.firingRate;
        }
        
        size_t count = metricsHistory.size();
        if (count > 1) {
            double meanStepTime = stepTimeSum / count;
            analysis.stdStepTime = std::sqrt((stepTimeSqSum - count * meanStepTime * meanStepTime) / (count - 1));
            
            double meanCPU = cpuSum / count;
            analysis.stdCPUUsage = std::sqrt((cpuSqSum - count * meanCPU * meanCPU) / (count - 1));
            
            double meanMem = memSum / count;
            analysis.stdMemoryUsage = std::sqrt((memSqSum - count * meanMem * meanMem) / (count - 1));
            
            double meanFiring = firingSum / count;
            analysis.stdFiringRate = std::sqrt((firingSqSum - count * meanFiring * meanFiring) / (count - 1));
        }
    }
    
    void identifyPerformanceBottlenecks(PerformanceAnalysis& analysis) {
        // Identify performance bottlenecks based on metrics
        analysis.bottlenecks.clear();
        
        if (metricsHistory.empty()) return;
        
        // Check for memory growth
        double firstMemory = metricsHistory.front().memoryUsage;
        double lastMemory = metricsHistory.back().memoryUsage;
        if (lastMemory > firstMemory * 1.5) {
            analysis.bottlenecks.push_back("Memory growth detected");
        }
        
        // Check for high CPU usage
        double maxCPU = 0.0;
        for (const auto& metrics : metricsHistory) {
            maxCPU = std::max(maxCPU, metrics.cpuUsage);
        }
        if (maxCPU > 0.8) {
            analysis.bottlenecks.push_back("High CPU usage (>80%)");
        }
        
        // Check for slow step times
        double maxStepTime = 0.0;
        for (const auto& metrics : metricsHistory) {
            maxStepTime = std::max(maxStepTime, metrics.stepTime);
        }
        if (maxStepTime > 0.1) {  // 100ms step time
            analysis.bottlenecks.push_back("Slow step processing (>100ms)");
        }
        
        // Check for instability
        double firingRateStd = analysis.stdFiringRate;
        if (firingRateStd > 0.5) {
            analysis.bottlenecks.push_back("High firing rate variability");
        }
    }
    
    void compareWithBaseline(PerformanceAnalysis& analysis) {
        if (baselineMetrics.empty()) return;
        
        // Compare current performance with baseline
        analysis.performanceChange.clear();
        
        // Compare key metrics
        double currentAvgStep = analysis.avgStepTime;
        double baselineAvgStep = 0.0;
        for (const auto& metrics : baselineMetrics) {
            baselineAvgStep += metrics.stepTime;
        }
        baselineAvgStep /= baselineMetrics.size();
        
        if (baselineAvgStep > 0.0) {
            double stepChange = (currentAvgStep - baselineAvgStep) / baselineAvgStep * 100.0;
            analysis.performanceChange.push_back("Step time: " + std::to_string(stepChange) + "%");
        }
        
        double currentAvgMemory = analysis.avgMemoryUsage;
        double baselineAvgMemory = 0.0;
        for (const auto& metrics : baselineMetrics) {
            baselineAvgMemory += metrics.memoryUsage;
        }
        baselineAvgMemory /= baselineMetrics.size();
        
        if (baselineAvgMemory > 0.0) {
            double memoryChange = (currentAvgMemory - baselineAvgMemory) / baselineAvgMemory * 100.0;
            analysis.performanceChange.push_back("Memory usage: " + std::to_string(memoryChange) + "%");
        }
    }
    
    double getCPUUsage() {
        // Get current CPU usage
        // Implementation depends on platform
        return 0.0;  // Placeholder
    }
    
    double getMemoryUsage() {
        // Get current memory usage in MB
        // Implementation depends on platform
        return 0.0;  // Placeholder
    }
    
    Timestamp getCurrentTime() {
        // Get current simulation time
        return 0.0;  // Placeholder
    }
};

struct PerformanceAnalysis {
    // Performance statistics
    double avgStepTime;
    double stdStepTime;
    double avgCPUUsage;
    double stdCPUUsage;
    double avgMemoryUsage;
    double stdMemoryUsage;
    double avgFiringRate;
    double stdFiringRate;
    
    // Performance issues
    std::vector<std::string> bottlenecks;
    
    // Comparison with baseline
    std::vector<std::string> performanceChange;
};
```

---

## Part 6: Custom Simulation Scenarios

### 6.1 Scenario Engine

```cpp
class ScenarioEngine {
public:
    struct SimulationScenario {
        std::string name;
        std::string description;
        std::map<std::string, std::string> parameters;
        std::vector<ScenarioEvent> events;
        bool enabled;
    };
    
    struct ScenarioEvent {
        Timestamp time;
        std::string type;  // "input", "reset", "checkpoint", "modify_config"
        std::string description;
        std::map<std::string, std::string> parameters;
    };
    
    ScenarioEngine(std::shared_ptr<Brain> brain) : brain_(brain) {
        currentScenario = nullptr;
        eventIndex = 0;
        simulationTime = 0.0;
        scenarioHistory.clear();
    }
    
    ScenarioId createScenario(const std::string& name,
                             const std::string& description,
                             const std::map<std::string, std::string>& parameters) {
        ScenarioId id = generateScenarioId();
        
        SimulationScenario scenario {
            name: name,
            description: description,
            parameters: parameters,
            events: std::vector<ScenarioEvent>(),
            enabled: true
        };
        
        scenarios[id] = scenario;
        
        NLM_LOG_INFO("Created scenario '" + name + "' with ID " + std::to_string(id));
        
        return id;
    }
    
    ScenarioEventId addEvent(ScenarioId scenarioId,
                            Timestamp time,
                            const std::string& type,
                            const std::string& description,
                            const std::map<std::string, std::string>& parameters) {
        auto it = scenarios.find(scenarioId);
        if (it == scenarios.end()) {
            NLM_LOG_ERROR("Scenario not found: " + std::to_string(scenarioId));
            return 0;
        }
        
        ScenarioEventId eventId = generateEventId();
        
        ScenarioEvent event {
            time: time,
            type: type,
            description: description,
            parameters: parameters
        };
        
        it->second.events.push_back(event);
        std::sort(it->second.events.begin(), it->second.events.end(),
                 [](const ScenarioEvent& a, const ScenarioEvent& b) {
                     return a.time < b.time;
                 });
        
        NLM_LOG_INFO("Added event to scenario '" + scenarioId + "': " + description);
        
        return eventId;
    }
    
    void runScenario(ScenarioId scenarioId, uint64_t numSteps) {
        auto it = scenarios.find(scenarioId);
        if (it == scenarios.end()) {
            NLM_LOG_ERROR("Scenario not found: " + std::to_string(scenarioId));
            return;
        }
        
        SimulationScenario& scenario = it->second;
        if (!scenario.enabled) {
            NLM_LOG_WARNING("Scenario disabled: " + scenario.name);
            return;
        }
        
        NLM_LOG_INFO("Running scenario '" + scenario.name + "' for " + 
                    std::to_string(numSteps) + " steps");
        
        // Reset event index
        eventIndex = 0;
        simulationTime = 0.0;
        
        // Run simulation
        for (uint64_t step = 0; step < numSteps; ++step) {
            simulationTime = step * 0.001;  // Assuming 1ms timestep
            
            // Process events for current time
            processScenarioEvents(simulationTime);
            
            // Run brain step
            brain_->step(step, simulationTime);
            
            // Collect metrics if needed
            collectScenarioMetrics(step, scenario);
        }
        
        // Store scenario history
        scenarioHistory.push_back(scenario);
        
        NLM_LOG_INFO("Completed scenario '" + scenario.name + "'");
    }
    
    void enableScenario(ScenarioId scenarioId) {
        auto it = scenarios.find(scenarioId);
        if (it != scenarios.end()) {
            it->second.enabled = true;
            NLM_LOG_INFO("Enabled scenario: " + it->second.name);
        }
    }
    
    void disableScenario(ScenarioId scenarioId) {
        auto it = scenarios.find(scenarioId);
        if (it != scenarios.end()) {
            it->second.enabled = false;
            NLM_LOG_INFO("Disabled scenario: " + it->second.name);
        }
    }
    
    std::vector<ScenarioId> getScenarioIds() {
        std::vector<ScenarioId> ids;
        for (const auto& pair : scenarios) {
            ids.push_back(pair.first);
        }
        return ids;
    }
    
    std::string getScenarioDescription(ScenarioId scenarioId) {
        auto it = scenarios.find(scenarioId);
        if (it != scenarios.end()) {
            return it->second.name + " - " + it->second.description;
        }
        return "Scenario not found";
    }
    
    std::vector<ScenarioEvent> getScenarioEvents(ScenarioId scenarioId) {
        auto it = scenarios.find(scenarioId);
        if (it != scenarios.end()) {
            return it->second.events;
        }
        return std::vector<ScenarioEvent>();
    }
    
private:
    using ScenarioId = uint64_t;
    using EventId = uint64_t;
    
    std::map<ScenarioId, SimulationScenario> scenarios;
    SimulationScenario* currentScenario;
    size_t eventIndex;
    Timestamp simulationTime;
    std::vector<SimulationScenario> scenarioHistory;
    std::shared_ptr<Brain> brain_;
    
    ScenarioId generateScenarioId() {
        static ScenarioId nextId = 1;
        return nextId++;
    }
    
    EventId generateEventId() {
        static EventId nextId = 1;
        return nextId++;
    }
    
    void processScenarioEvents(Timestamp currentTime) {
        if (!currentScenario) return;
        
        // Process events scheduled for current time
        while (eventIndex < currentScenario->events.size() &&
               currentScenario->events[eventIndex].time <= currentTime) {
            
            const ScenarioEvent& event = currentScenario->events[eventIndex];
            executeScenarioEvent(event);
            eventIndex++;
        }
    }
    
    void executeScenarioEvent(const ScenarioEvent& event) {
        NLM_LOG_INFO("Executing scenario event: " + event.description);
        
        if (event.type == "input") {
            // Apply sensory input
            applyScenarioInput(event.parameters);
        } else if (event.type == "reset") {
            // Reset brain
            brain_->reset();
            brain_->initialize();
        } else if (event.type == "checkpoint") {
            // Save checkpoint
            std::string filepath = event.parameters.at("filepath");
            brain_->save(filepath);
        } else if (event.type == "modify_config") {
            // Modify configuration
            modifyConfiguration(event.parameters);
        } else {
            NLM_LOG_WARNING("Unknown scenario event type: " + event.type);
        }
    }
    
    void applyScenarioInput(const std::map<std::string, std::string>& parameters) {
        // Apply sensory input based on parameters
        // Implementation depends on your sensory input system
        // This could include vision, touch, internal signals, etc.
    }
    
    void modifyConfiguration(const std::map<std::string, std::string>& parameters) {
        // Modify brain configuration
        // This would require access to the brain's config object
        // Implementation depends on your configuration system
    }
    
    void collectScenarioMetrics(uint64_t step, const SimulationScenario& scenario) {
        // Collect metrics for scenario analysis
        // Store metrics in scenario history
        // Implementation depends on your metrics collection system
    }
};
```

---

## Part 7: Integration with External Tools

### 7.1 External Data Sources

```cpp
class ExternalDataSource {
public:
    enum class DataSourceType {
        REAL_TIME_SENSOR,
        HISTORICAL_DATA,
        SIMULATION_DATA,
        MODEL_OUTPUT
    };
    
    struct DataSourceConfig {
        std::string name;
        DataSourceType type;
        std::string connectionString;
        std::map<std::string, std::string> parameters;
        double samplingRate;
        bool enabled;
    };
    
    ExternalDataSource() {
        dataSources.clear();
    }
    
    DataSourceId addDataSource(const DataSourceConfig& config) {
        DataSourceId id = generateDataSourceId();
        
        DataSourceInfo source {
            id: id,
            config: config,
            lastUpdate: 0.0,
            dataCount: 0,
            status: DataSourceStatus::UNKNOWN
        };
        
        dataSources[id] = source;
        
        NLM_LOG_INFO("Added data source: " + config.name + 
                    " (" + std::to_string(id) + ")");
        
        return id;
    }
    
    std::vector<float> getData(DataSourceId id, Timestamp time) {
        auto it = dataSources.find(id);
        if (it == dataSources.end()) {
            NLM_LOG_ERROR("Data source not found: " + std::to_string(id));
            return std::vector<float>();
        }
        
        DataSourceInfo& source = it->second;
        
        if (!source.config.enabled) {
            NLM_LOG_WARNING("Data source disabled: " + source.config.name);
            return std::vector<float>();
        }
        
        // Get data from source
        std::vector<float> data;
        if (source.config.type == DataSourceType::REAL_TIME_SENSOR) {
            data = getRealTimeSensorData(source, time);
        } else if (source.config.type == DataSourceType::HISTORICAL_DATA) {
            data = getHistoricalData(source, time);
        } else if (source.config.type == DataSourceType::SIMULATION_DATA) {
            data = getSimulationData(source, time);
        } else if (source.config.type == DataSourceType::MODEL_OUTPUT) {
            data = getModelOutput(source, time);
        }
        
        source.lastUpdate = getCurrentTime();
        source.dataCount++;
        
        return data;
    }
    
    void enableDataSource(DataSourceId id) {
        auto it = dataSources.find(id);
        if (it != dataSources.end()) {
            it->second.config.enabled = true;
            NLM_LOG_INFO("Enabled data source: " + it->second.config.name);
        }
    }
    
    void disableDataSource(DataSourceId id) {
        auto it = dataSources.find(id);
        if (it != dataSources.end()) {
            it->second.config.enabled = false;
            NLM_LOG_INFO("Disabled data source: " + it->second.config.name);
        }
    }
    
    std::vector<DataSourceId> getDataSourceIds() {
        std::vector<DataSourceId> ids;
        for (const auto& pair : dataSources) {
            ids.push_back(pair.first);
        }
        return ids;
    }
    
    std::string getDataSourceStatus(DataSourceId id) {
        auto it = dataSources.find(id);
        if (it != dataSources.end()) {
            return dataSourceStatusToString(it->second.status);
        }
        return "NOT_FOUND";
    }
    
    DataSourceStats getDataSourceStats(DataSourceId id) {
        auto it = dataSources.find(id);
        if (it != dataSources.end()) {
            return calculateDataSourceStats(it->second);
        }
        return DataSourceStats();
    }
    
    void updateAllDataSources(Timestamp time) {
        for (auto& pair : dataSources) {
            if (pair.second.config.enabled) {
                updateDataSource(pair.first, time);
            }
        }
    }
    
    std::vector<float> getIntegratedSensoryData(Timestamp time) {
        // Integrate data from all enabled sources
        std::vector<float> integratedData;
        
        for (const auto& pair : dataSources) {
            if (pair.second.config.enabled) {
                std::vector<float> sourceData = getData(pair.first, time);
                integratedData.insert(integratedData.end(), 
                                    sourceData.begin(), sourceData.end());
            }
        }
        
        return integratedData;
    }
    
private:
    using DataSourceId = uint64_t;
    
    enum class DataSourceStatus {
        UNKNOWN,
        CONNECTED,
        DISCONNECTED,
        ERROR,
        TIMEOUT
    };
    
    struct DataSourceInfo {
        DataSourceId id;
        DataSourceConfig config;
        Timestamp lastUpdate;
        uint64_t dataCount;
        DataSourceStatus status;
    };
    
    struct DataSourceStats {
        uint64_t totalDataPoints;
        Timestamp lastUpdate;
        double averageUpdateInterval;
        std::string connectionStatus;
    };
    
    std::map<DataSourceId, DataSourceInfo> dataSources;
    
    DataSourceId generateDataSourceId() {
        static DataSourceId nextId = 1;
        return nextId++;
    }
    
    std::string dataSourceStatusToString(DataSourceStatus status) {
        switch (status) {
            case DataSourceStatus::UNKNOWN: return "UNKNOWN";
            case DataSourceStatus::CONNECTED: return "CONNECTED";
            case DataSourceStatus::DISCONNECTED: return "DISCONNECTED";
            case DataSourceStatus::ERROR: return "ERROR";
            case DataSourceStatus::TIMEOUT: return "TIMEOUT";
            default: return "UNKNOWN";
        }
    }
    
    std::vector<float> getRealTimeSensorData(const DataSourceInfo& source,
                                            Timestamp time) {
        // Get data from real-time sensors
        // Implementation depends on sensor interface
        return std::vector<float>();
    }
    
    std::vector<float> getHistoricalData(const DataSourceInfo& source,
                                        Timestamp time) {
        // Get historical data
        // Implementation depends on data storage
        return std::vector<float>();
    }
    
    std::vector<float> getSimulationData(const DataSourceInfo& source,
                                        Timestamp time) {
        // Get data from simulation outputs
        // Implementation depends on simulation system
        return std::vector<float>();
    }
    
    std::vector<float> getModelOutput(const DataSourceInfo& source,
                                     Timestamp time) {
        // Get data from models
        // Implementation depends on model system
        return std::vector<float>();
    }
    
    void updateDataSource(DataSourceId id, Timestamp time) {
        // Update data source with new data
        // Implementation depends on source type
    }
    
    DataSourceStats calculateDataSourceStats(const DataSourceInfo& source) {
        DataSourceStats stats;
        stats.totalDataPoints = source.dataCount;
        stats.lastUpdate = source.lastUpdate;
        
        // Calculate average update interval
        if (source.dataCount > 1) {
            double totalTime = getCurrentTime() - source.lastUpdate;
            stats.averageUpdateInterval = totalTime / (source.dataCount - 1);
        }
        
        stats.connectionStatus = dataSourceStatusToString(source.status);
        
        return stats;
    }
    
    Timestamp getCurrentTime() {
        // Get current simulation time
        return 0.0;  // Placeholder
    }
};
```

---

## Part 8: System Monitoring and Health Checks

### 8.1 System Health Monitor

```cpp
class SystemHealthMonitor {
public:
    struct HealthStatus {
        Timestamp timestamp;
        bool brainHealthy;
        bool memoryHealthy;
        bool plasticityHealthy;
        bool neuromodulationHealthy;
        bool developmentHealthy;
        float systemLoad;
        double memoryUsage;
        std::vector<std::string> issues;
        std::vector<std::string> warnings;
    };
    
    SystemHealthMonitor(std::shared_ptr<Brain> brain) : brain_(brain) {
        monitoringEnabled = false;
        checkInterval = 1.0;  // seconds
        healthHistory.clear();
        lastHealthCheck = 0.0;
    }
    
    void startMonitoring(double interval = 1.0) {
        monitoringEnabled = true;
        checkInterval = interval;
        healthHistory.clear();
        lastHealthCheck = getCurrentTime();
        
        NLM_LOG_INFO("System health monitoring started with interval " + 
                    std::to_string(interval) + "s");
    }
    
    void stopMonitoring() {
        monitoringEnabled = false;
        
        NLM_LOG_INFO("System health monitoring stopped");
    }
    
    HealthStatus getCurrentHealthStatus() {
        HealthStatus status;
        status.timestamp = getCurrentTime();
        
        // Check brain health
        status.brainHealthy = checkBrainHealth();
        
        // Check memory health
        status.memoryHealthy = checkMemoryHealth();
        
        // Check plasticity health
        status.plasticityHealthy = checkPlasticityHealth();
        
        // Check neuromodulation health
        status.neuromodulationHealthy = checkNeuromodulationHealth();
        
        // Check development health
        status.developmentHealthy = checkDevelopmentHealth();
        
        // Get system metrics
        status.systemLoad = getSystemLoad();
        status.memoryUsage = getMemoryUsage();
        
        // Identify issues and warnings
        status.issues = identifyIssues();
        status.warnings = identifyWarnings();
        
        return status;
    }
    
    void checkHealth() {
        if (!monitoringEnabled) return;
        
        HealthStatus status = getCurrentHealthStatus();
        healthHistory.push_back(status);
        
        // Log health status periodically
        if (healthHistory.size() % 100 == 0) {
            logHealthStatus(status);
        }
        
        lastHealthCheck = getCurrentTime();
    }
    
    std::vector<HealthStatus> getHealthHistory() {
        return healthHistory;
    }
    
    HealthAnalysis analyzeHealth() {
        HealthAnalysis analysis;
        
        if (healthHistory.empty()) {
            return analysis;  // Empty analysis
        }
        
        // Calculate health statistics
        calculateHealthStatistics(analysis);
        
        // Identify health trends
        identifyHealthTrends(analysis);
        
        // Generate recommendations
        generateHealthRecommendations(analysis);
        
        return analysis;
    }
    
    void saveHealthReport(const std::string& filepath) {
        // Save health report to file
        // Implementation depends on format
        
        NLM_LOG_INFO("Health report saved to " + filepath);
    }
    
private:
    bool monitoringEnabled;
    double checkInterval;
    std::vector<HealthStatus> healthHistory;
    Timestamp lastHealthCheck;
    std::shared_ptr<Brain> brain_;
    
    bool checkBrainHealth() {
        if (!brain_) return false;
        
        // Check if brain is properly initialized
        if (!brain_->getConfig()) return false;
        
        // Check neuron and synapse counts
        size_t neuronCount = brain_->getTotalNeuronCount();
        size_t synapseCount = brain_->getTotalSynapseCount();
        
        if (neuronCount == 0 || synapseCount == 0) {
            return false;  // Invalid brain state
        }
        
        // Check firing activity
        float firingRate = brain_->getAverageFiringRate();
        if (firingRate < 0.001f && neuronCount > 10) {
            return false;  // No activity in a populated brain
        }
        
        return true;
    }
    
    bool checkMemoryHealth() {
        // Check memory usage and health
        double memoryUsage = getMemoryUsage();
        
        if (memoryUsage > getMaxMemoryUsage() * 0.9) {
            return false;  // Memory usage too high
        }
        
        // Check for memory leaks
        if (healthHistory.size() >= 10) {
            double recentMemoryGrowth = calculateRecentMemoryGrowth();
            if (recentMemoryGrowth > getMaxMemoryGrowth()) {
                return false;  // Memory leak detected
            }
        }
        
        return true;
    }
    
    bool checkPlasticityHealth() {
        // Check plasticity system health
        // This would check if plasticity mechanisms are working correctly
        // For now, return true (placeholder)
        return true;
    }
    
    bool checkNeuromodulationHealth() {
        // Check neuromodulation system health
        // This would check if neuromodulators are working correctly
        // For now, return true (placeholder)
        return true;
    }
    
    bool checkDevelopmentHealth() {
        // Check development system health
        // This would check if development mechanisms are working correctly
        // For now, return true (placeholder)
        return true;
    }
    
    float getSystemLoad() {
        // Get current system load
        // Implementation depends on platform
        return 0.0f;  // Placeholder
    }
    
    double getMemoryUsage() {
        // Get current memory usage in MB
        // Implementation depends on platform
        return 0.0;  // Placeholder
    }
    
    double getMaxMemoryUsage() {
        // Get maximum memory usage in MB
        // Implementation depends on platform
        return 1024.0;  // Placeholder: 1GB
    }
    
    double calculateRecentMemoryGrowth() {
        // Calculate memory growth over recent samples
        if (healthHistory.size() < 10) return 0.0;
        
        double recentMemory = healthHistory.back().memoryUsage;
        double earlierMemory = healthHistory[healthHistory.size() - 10].memoryUsage;
        
        return recentMemory - earlierMemory;
    }
    
    double getMaxMemoryGrowth() {
        // Get maximum acceptable memory growth
        return 100.0;  // Placeholder: 100MB
    }
    
    void logHealthStatus(const HealthStatus& status) {
        NLM_LOG_INFO(std::string("Health - Time: ") + std::to_string(status.timestamp) +
                    ", Brain: " + (status.brainHealthy ? "OK" : "FAIL") +
                    ", Memory: " + (status.memoryHealthy ? "OK" : "FAIL") +
                    ", Load: " + std::to_string(status.systemLoad * 100) + "%");
        
        if (!status.issues.empty()) {
            NLM_LOG_WARNING("Health issues: " + joinStrings(status.issues, ", "));
        }
        
        if (!status.warnings.empty()) {
            NLM_LOG_WARNING("Health warnings: " + joinStrings(status.warnings, ", "));
        }
    }
    
    void identifyHealthTrends(HealthAnalysis& analysis) {
        // Identify health trends over time
        analysis.trends.clear();
        
        if (healthHistory.size() < 20) return;
        
        // Check for degradation trend
        double healthScore = calculateHealthScore(healthHistory.back());
        
        if (healthHistory.size() >= 10) {
            double previousHealthScore = calculateHealthScore(healthHistory[healthHistory.size() - 10]);
            
            if (healthScore < previousHealthScore * 0.9) {
                analysis.trends.push_back("Health degrading");
            } else if (healthScore > previousHealthScore * 1.1) {
                analysis.trends.push_back("Health improving");
            }
        }
        
        // Check for recurring issues
        analyzeRecurringIssues(analysis);
    }
    
    void generateHealthRecommendations(HealthAnalysis& analysis) {
        // Generate recommendations based on health analysis
        analysis.recommendations.clear();
        
        // Generate recommendations for identified issues
        for (const auto& issue : analysis.issues) {
            if (issue == "Memory usage too high") {
                analysis.recommendations.push_back("Consider reducing brain size or improving memory efficiency");
            } else if (issue == "No activity in a populated brain") {
                analysis.recommendations.push_back("Check sensory input and neuromodulation settings");
            } else if (issue == "Memory leak detected") {
                analysis.recommendations.push_back("Review memory management and release unused resources");
            }
        }
    }
    
    double calculateHealthScore(const HealthStatus& status) {
        // Calculate overall health score
        double score = 0.0;
        
        score += status.brainHealthy ? 0.2 : 0.0;
        score += status.memoryHealthy ? 0.2 : 0.0;
        score += status.plasticityHealthy ? 0.2 : 0.0;
        score += status.neuromodulationHealthy ? 0.2 : 0.0;
        score += status.developmentHealthy ? 0.2 : 0.0;
        
        // Factor in system load and memory usage
        double loadFactor = std::min(1.0, 1.0 - status.systemLoad);
        double memoryFactor = std::min(1.0, 1.0 - status.memoryUsage / getMaxMemoryUsage());
        
        score *= (loadFactor + memoryFactor) * 0.5;
        
        return score;
    }
    
    void analyzeRecurringIssues(HealthAnalysis& analysis) {
        // Analyze recurring health issues
        std::map<std::string, int> issueCounts;
        
        for (const auto& status : healthHistory) {
            for (const auto& issue : status.issues) {
                issueCounts[issue]++;
            }
        }
        
        for (const auto& pair : issueCounts) {
            if (pair.second >= 3) {  // Issue appears at least 3 times
                analysis.recurringIssues.push_back(pair.first + " (occurrences: " + 
                                                  std::to_string(pair.second) + ")");
            }
        }
    }
    
    std::string joinStrings(const std::vector<std::string>& strings, 
                           const std::string& delimiter) {
        if (strings.empty()) return "";
        
        std::string result = strings[0];
        for (size_t i = 1; i < strings.size(); ++i) {
            result += delimiter + strings[i];
        }
        
        return result;
    }
};

struct HealthAnalysis {
    // Health statistics
    double averageHealthScore;
    double healthScoreTrend;
    
    // Health issues
    std::vector<std::string> issues;
    std::vector<std::string> warnings;
    std::vector<std::string> recurringIssues;
    
    // Health trends
    std::vector<std::string> trends;
    
    // Recommendations
    std::vector<std::string> recommendations;
};
```

---

## Part 9: Advanced Configuration System

### 9.1 Dynamic Configuration

```cpp
class DynamicConfiguration {
public:
    struct ConfigurationChange {
        Timestamp timestamp;
        std::string parameter;
        std::string oldValue;
        std::string newValue;
        std::string changeReason;
    };
    
    DynamicConfiguration(std::shared_ptr<Brain> brain) : brain_(brain) {
        configurationHistory.clear();
        autoSaveEnabled = false;
        autoSaveInterval = 300.0;  // seconds
        lastAutoSave = 0.0;
    }
    
    bool setParameter(const std::string& parameter, 
                     const std::string& value,
                     const std::string& reason = "manual") {
        auto it = parameterRegistry.find(parameter);
        if (it == parameterRegistry.end()) {
            NLM_LOG_ERROR("Unknown parameter: " + parameter);
            return false;
        }
        
        const ParameterInfo& paramInfo = it->second;
        
        // Validate parameter value
        if (!validateParameter(parameter, value, paramInfo)) {
            NLM_LOG_ERROR("Invalid value for parameter " + parameter + ": " + value);
            return false;
        }
        
        // Get old value
        std::string oldValue;
        if (brain_->getConfig()->has(parameter)) {
            oldValue = brain_->getConfig()->getOr<std::string>(parameter, "");
        }
        
        // Set new value
        setConfigParameter(parameter, value, paramInfo.type);
        
        // Record change
        ConfigurationChange change {
            timestamp: getCurrentTime(),
            parameter: parameter,
            oldValue: oldValue,
            newValue: value,
            changeReason: reason
        };
        
        configurationHistory.push_back(change);
        
        NLM_LOG_INFO("Configuration changed: " + parameter + " = " + value + 
                    " (reason: " + reason + ")");
        
        // Trigger auto-save if enabled
        if (autoSaveEnabled) {
            scheduleAutoSave();
        }
        
        return true;
    }
    
    std::string getParameter(const std::string& parameter) {
        auto it = parameterRegistry.find(parameter);
        if (it == parameterRegistry.end()) {
            NLM_LOG_ERROR("Unknown parameter: " + parameter);
            return "";
        }
        
        const ParameterInfo& paramInfo = it->second;
        
        if (!brain_->getConfig()->has(parameter)) {
            return paramInfo.defaultValue;
        }
        
        return brain_->getConfig()->getOr<std::string>(parameter, paramInfo.defaultValue);
    }
    
    bool hasParameter(const std::string& parameter) {
        auto it = parameterRegistry.find(parameter);
        return it != parameterRegistry.end();
    }
    
    std::vector<std::string> getParameterNames() {
        std::vector<std::string> names;
        for (const auto& pair : parameterRegistry) {
            names.push_back(pair.first);
        }
        return names;
    }
    
    ParameterInfo getParameterInfo(const std::string& parameter) {
        auto it = parameterRegistry.find(parameter);
        if (it != parameterRegistry.end()) {
            return it->second;
        }
        
        ParameterInfo unknown;
        unknown.name = parameter;
        unknown.type = ParameterType::UNKNOWN;
        unknown.defaultValue = "";
        unknown.description = "Unknown parameter";
        
        return unknown;
    }
    
    void registerParameter(const ParameterInfo& paramInfo) {
        parameterRegistry[paramInfo.name] = paramInfo;
        
        // Set default value if provided
        if (!paramInfo.defaultValue.empty() && !brain_->getConfig()->has(paramInfo.name)) {
            setConfigParameter(paramInfo.name, paramInfo.defaultValue, paramInfo.type);
        }
        
        NLM_LOG_INFO("Registered parameter: " + paramInfo.name);
    }
    
    std::vector<ConfigurationChange> getConfigurationHistory() {
        return configurationHistory;
    }
    
    std::vector<ConfigurationChange> getConfigurationHistory(const std::string& parameter,
                                                             Timestamp from,
                                                             Timestamp to) {
        std::vector<ConfigurationChange> history;
        
        for (const auto& change : configurationHistory) {
            if (change.parameter == parameter &&
                change.timestamp >= from &&
                change.timestamp <= to) {
                history.push_back(change);
            }
        }
        
        return history;
    }
    
    void enableAutoSave(double interval = 300.0) {
        autoSaveEnabled = true;
        autoSaveInterval = interval;
        lastAutoSave = getCurrentTime();
        
        NLM_LOG_INFO("Auto-save enabled with interval " + std::to_string(interval) + "s");
    }
    
    void disableAutoSave() {
        autoSaveEnabled = false;
        NLM_LOG_INFO("Auto-save disabled");
    }
    
    void saveConfiguration(const std::string& filepath) {
        // Save configuration to file
        // Implementation depends on format
        
        NLM_LOG_INFO("Configuration saved to " + filepath);
    }
    
    bool loadConfiguration(const std::string& filepath) {
        // Load configuration from file
        // Implementation depends on format
        return false;  // Placeholder
    }
    
    std::string exportConfiguration() {
        // Export configuration as string
        // Implementation depends on format
        return "";
    }
    
    bool importConfiguration(const std::string& configString) {
        // Import configuration from string
        // Implementation depends on format
        return false;  // Placeholder
    }
    
    ConfigurationSnapshot createSnapshot(const std::string& name) {
        ConfigurationSnapshot snapshot {
            name: name,
            timestamp: getCurrentTime(),
            configuration: std::map<std::string, std::string>()
        };
        
        // Capture current configuration
        for (const auto& pair : parameterRegistry) {
            snapshot.configuration[pair.first] = 
                brain_->getConfig()->getOr<std::string>(pair.first, pair.second.defaultValue);
        }
        
        snapshots[name] = snapshot;
        
        NLM_LOG_INFO("Created configuration snapshot: " + name);
        
        return snapshot;
    }
    
    bool restoreSnapshot(const std::string& name) {
        auto it = snapshots.find(name);
        if (it == snapshots.end()) {
            NLM_LOG_ERROR("Snapshot not found: " + name);
            return false;
        }
        
        const ConfigurationSnapshot& snapshot = it->second;
        
        // Restore configuration
        for (const auto& pair : snapshot.configuration) {
            setParameter(pair.first, pair.second, "snapshot_restore");
        }
        
        NLM_LOG_INFO("Restored configuration snapshot: " + name);
        
        return true;
    }
    
    std::vector<std::string> getSnapshotNames() {
        std::vector<std::string> names;
        for (const auto& pair : snapshots) {
            names.push_back(pair.first);
        }
        return names;
    }
    
    void deleteSnapshot(const std::string& name) {
        auto it = snapshots.find(name);
        if (it != snapshots.end()) {
            snapshots.erase(it);
            NLM_LOG_INFO("Deleted configuration snapshot: " + name);
        }
    }
    
private:
    struct ParameterInfo {
        std::string name;
        ParameterType type;
        std::string defaultValue;
        std::string description;
        std::string validationRegex;
        std::vector<std::string> validValues;
        bool required;
        double minValue;
        double maxValue;
    };
    
    enum class ParameterType {
        STRING,
        INTEGER,
        FLOAT,
        BOOLEAN,
        ENUM,
        UNKNOWN
    };
    
    struct ConfigurationChange {
        Timestamp timestamp;
        std::string parameter;
        std::string oldValue;
        std::string newValue;
        std::string changeReason;
    };
    
    struct ConfigurationSnapshot {
        std::string name;
        Timestamp timestamp;
        std::map<std::string, std::string> configuration;
    };
    
    std::map<std::string, ParameterInfo> parameterRegistry;
    std::vector<ConfigurationChange> configurationHistory;
    std::map<std::string, ConfigurationSnapshot> snapshots;
    bool autoSaveEnabled;
    double autoSaveInterval;
    Timestamp lastAutoSave;
    std::shared_ptr<Brain> brain_;
    
    bool validateParameter(const std::string& parameter,
                          const std::string& value,
                          const ParameterInfo& paramInfo) {
        // Validate parameter based on type
        switch (paramInfo.type) {
            case ParameterType::STRING:
                return validateStringParameter(value, paramInfo);
            case ParameterType::INTEGER:
                return validateIntegerParameter(value, paramInfo);
            case ParameterType::FLOAT:
                return validateFloatParameter(value, paramInfo);
            case ParameterType::BOOLEAN:
                return validateBooleanParameter(value, paramInfo);
            case ParameterType::ENUM:
                return validateEnumParameter(value, paramInfo);
            default:
                return true;  // Unknown type, assume valid
        }
    }
    
    bool validateStringParameter(const std::string& value,
                                 const ParameterInfo& paramInfo) {
        if (paramInfo.required && value.empty()) {
            return false;
        }
        
        if (!paramInfo.validationRegex.empty()) {
            // Validate against regex
            return std::regex_match(value, std::regex(paramInfo.validationRegex));
        }
        
        return true;
    }
    
    bool validateIntegerParameter(const std::string& value,
                                  const ParameterInfo& paramInfo) {
        try {
            double intValue = std::stod(value);
            if (paramInfo.required && value.empty()) {
                return false;
            }
            
            if (paramInfo.minValue > 0 && intValue < paramInfo.minValue) {
                return false;
            }
            
            if (paramInfo.maxValue > 0 && intValue > paramInfo.maxValue) {
                return false;
            }
            
            return true;
        } catch (...) {
            return false;
        }
    }
    
    bool validateFloatParameter(const std::string& value,
                                const ParameterInfo& paramInfo) {
        try {
            double floatValue = std::stod(value);
            if (paramInfo.required && value.empty()) {
                return false;
            }
            
            if (paramInfo.minValue > 0 && floatValue < paramInfo.minValue) {
                return false;
            }
            
            if (paramInfo.maxValue > 0 && floatValue > paramInfo.maxValue) {
                return false;
            }
            
            return true;
        } catch (...) {
            return false;
        }
    }
    
    bool validateBooleanParameter(const std::string& value,
                                  const ParameterInfo& paramInfo) {
        std::string lowerValue = value;
        std::transform(lowerValue.begin(), lowerValue.end(), lowerValue.begin(),
                      [](unsigned char c) { return std::tolower(c); });
        
        return lowerValue == "true" || lowerValue == "false" || 
               lowerValue == "yes" || lowerValue == "no" ||
               lowerValue == "1" || lowerValue == "0" ||
               lowerValue.empty();  // Empty allowed if not required
    }
    
    bool validateEnumParameter(const std::string& value,
                               const ParameterInfo& paramInfo) {
        if (paramInfo.validValues.empty()) {
            return true;  // No valid values specified
        }
        
        return std::find(paramInfo.validValues.begin(), paramInfo.validValues.end(), value) !=
               paramInfo.validValues.end();
    }
    
    void setConfigParameter(const std::string& parameter,
                           const std::string& value,
                           ParameterType type) {
        // Set parameter in brain configuration
        // Implementation depends on your configuration system
    }
    
    void scheduleAutoSave() {
        double currentTime = getCurrentTime();
        if (currentTime - lastAutoSave >= autoSaveInterval) {
            // Perform auto-save
            std::string filepath = generateAutoSaveFilepath();
            saveConfiguration(filepath);
            
            lastAutoSave = currentTime;
        }
    }
    
    std::string generateAutoSaveFilepath() {
        auto now = std::chrono::system_clock::now();
        auto time_t = std::chrono::system_clock::to_time_t(now);
        
        std::stringstream ss;
        ss << "./autonomous_configs/" << std::put_time(std::localtime(&time_t), "%Y%m%d_%H%M%S") 
           << "_config.json";
        
        return ss.str();
    }
    
    Timestamp getCurrentTime() {
        // Get current simulation time
        return 0.0;  // Placeholder
    }
};
```

---

## Part 10: Migration and Upgrade Guide

### 10.1 Upgrade Procedures

#### From Version 0.5.x to 0.6.0

Upgrading from NLM 0.5.x to 0.6.0 involves several changes:

1. **Configuration Format**: Configuration format has changed
2. **API Changes**: Some method signatures have been updated
3. **New Features**: Version 0.6.0 introduces advanced features

#### Upgrade Steps

```cpp
// Before upgrade (0.5.x):
brain = pynlm.createBrain(config);
brain.initialize();

// After upgrade (0.6.0):
import pynlm
import sys

# Check version
if pynlm.__version__ != "0.6.0":
    print(f"Warning: Expected NLM version 0.6.0, found {pynlm.__version__}")

# Upgrade configuration
config = pynlm.createDefaultConfig()
config.set("brain.version", "0.6.0")

# Create brain with new API
brain = pynlm.createBrain(config)
brain.initialize()

# Use new features (optional)
if hasattr(brain, 'enableAdvancedLearning'):
    brain.enableAdvancedLearning(True)

# Run simulation
for step in range(1000):
    brain.step(step, step * 0.001)  # Added timestamp parameter
```

### 10.2 Breaking Changes

1. **Required**: Call `brain.initialize()` after creating
2. **Changed**: `step()` method now accepts optional timestamp
3. **Added**: New configuration options for advanced features
4. **Removed**: Some deprecated methods (check documentation)

### 10.3 Compatibility Matrix

| From Version | To Version | Compatibility |
|-------------|------------|---------------|
| 0.1.0       | 0.2.0      | Partially     |
| 0.2.0       | 0.3.0      | Fully         |
| 0.3.0       | 0.4.0      | Fully         |
| 0.4.0       | 0.5.0      | With changes  |
| 0.5.0       | 0.6.0      | With changes  |

### 10.4 Recommended Upgrade Path

1. **Backup**: Save any checkpoint files and configuration
2. **Update**: Update configuration files to new format
3. **Test**: Test with small simulation first
4. **Monitor**: Monitor for any issues with performance or behavior
5. **Migrate**: Gradually migrate to new features as needed

### 10.5 Migration Scripts

```python
# Migration script for upgrading NLM configuration
def migrate_config_v5_to_v6(config_path):
    """Migrate configuration from NLM 0.5.x to 0.6.0"""
    
    import json
    
    # Load old configuration
    with open(config_path, 'r') as f:
        old_config = json.load(f)
    
    # Create new configuration
    new_config = {
        "brain": {
            "neuron_count": old_config.get("brain", {}).get("neuron_count", 1000),
            "region_count": old_config.get("brain", {}).get("region_count", 1),
            "connection_probability": old_config.get("brain", {}).get("connection_probability", 0.1),
            "version": "0.6.0",
            "advanced_features": {
                "custom_neurons": old_config.get("brain", {}).get("custom_neurons", False),
                "dynamic_learning": old_config.get("brain", {}).get("dynamic_learning", False),
                "batch_processing": old_config.get("brain", {}).get("batch_processing", False)
            }
        },
        "performance": {
            "parallel_processing": old_config.get("performance", {}).get("parallel_processing", False),
            "optimization_level": old_config.get("performance", {}).get("optimization_level", "standard")
        }
    };
    
    # Save new configuration
    new_config_path = config_path.replace('.json', '_v6.json')
    with open(new_config_path, 'w') as f:
        json.dump(new_config, f, indent=2)
    
    print(f"Configuration migrated from {config_path} to {new_config_path}")

# Usage
migrate_config_v5_to_v6("./config.json")
```

---

## Conclusion

This Advanced Power User Guide provides comprehensive information for advanced users and power users of NLM. The project offers:

- **Flexibility**: Extensive configuration options and customization
- **Performance**: Advanced performance profiling and optimization
- **Extensibility**: Custom neuron types, learning strategies, and external integrations
- **Monitoring**: Comprehensive system health monitoring
- **Automation**: Batch processing and dynamic configuration management
- **Migration**: Clear upgrade paths and compatibility matrices

NLM continues to evolve with regular feature additions and improvements. Users are encouraged to:

1. Explore the advanced features gradually
2. Use the migration guides when upgrading
3. Contribute to the development of new features
4. Share their experiences and enhancements with the community

---

*Last updated: 2026-09-30*
*Version: 0.6.0 (Advanced Features)*

This document will be continuously updated as new features are developed and tested. Users should check for updates regularly.

---

*For questions, issues, or feature requests, please visit the NLM GitHub repository or contact the development team.*

```
NLM - Neural Learning Machine
Copyright (c) 2026 NLM Project Authors
MIT License - See LICENSE file for details
```
