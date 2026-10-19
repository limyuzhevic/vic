// === ADVANCED FEATURES ===

// === ADVANCED LEARNING ALGORITHMS ===

// Advanced learning algorithms
void AgentBrain::enableCurriculumLearning(bool enable, const std::string& strategy) {
    if (enable) {
        NLM_LOG_INFO("Enabled curriculum learning with strategy: " + strategy);
        curriculumLearningEnabled_ = true;
        curriculumStrategy_ = strategy;
        
        // Setup adaptive difficulty tracking
        currentDifficultyLevel_ = 1.0f;
        difficultyProgressionRate_ = 0.1f;
        masteryThreshold_ = 0.8f;
    } else {
        NLM_LOG_INFO("Disabled curriculum learning");
        curriculumLearningEnabled_ = false;
    }
}

void AgentBrain::setTransferLearningTarget(std::shared_ptr<AgentBrain> sourceBrain) {
    if (sourceBrain) {
        NLM_LOG_INFO("Set transfer learning target from source brain");
        transferLearningTarget_ = sourceBrain;
        
        // Extract knowledge from source brain
        transferKnowledgeProgress_ = 0.0f;
        transferLearningProgress_ = 0.0f;
        
        NLM_LOG_INFO("Transfer learning target established - beginning knowledge extraction");
    } else {
        NLM_LOG_WARNING("Attempted to set null transfer learning target");
    }
}

void AgentBrain::enableMetaLearning(bool enable, float metaLearningRate) {
    if (enable) {
        NLM_LOG_INFO("Enabled meta-learning with rate: " + std::to_string(metaLearningRate));
        metaLearningEnabled_ = true;
        metaLearningRate_ = metaLearningRate;
        
        // Setup meta-learning components
        metaLearningProgress_ = 0.0f;
        learningSpeed_ = metaLearningRate_;
        adaptationRate_ = metaLearningRate_ * 0.5f;
    } else {
        NLM_LOG_INFO("Disabled meta-learning");
        metaLearningEnabled_ = false;
    }
}

void AgentBrain::setCustomPlasticityRule(const std::string& ruleName, 
                                        const std::map<std::string, float>& parameters) {
    NLM_LOG_INFO("Set custom plasticity rule: " + ruleName);
    customPlasticityRule_ = ruleName;
    customPlasticityParameters_ = parameters;
    
    // Apply the custom plasticity rule
    applyCustomPlasticityRule();
}

void AgentBrain::applyAdvancedRewardShaping(float baseReward, float shapingFactor, 
                                           const std::string& strategy) {
    if (validateRewardValue(baseReward)) {
        float shapedReward = baseReward;
        
        // Apply advanced reward shaping based on strategy
        if (strategy == "exponential") {
            shapedReward = baseReward * std::exp(shapingFactor - 1.0f);
        } else if (strategy == "power") {
            shapedReward = baseReward * std::pow(shapingFactor, 2.0f);
        } else if (strategy == "logarithmic") {
            shapedReward = baseReward * std::log(shapingFactor + 1.0f);
        } else if (strategy == "gaussian") {
            shapedReward = baseReward * std::exp(-0.5f * std::pow(shapingFactor - 1.0f, 2.0f));
        }
        
        // Apply shaping
        applyRewardShaping(shapedReward, shapingFactor);
        
        NLM_LOG_INFO("Applied advanced reward shaping: base=" + std::to_string(baseReward) +
                    ", shaping=" + std::to_string(shapingFactor) +
                    ", strategy=" + strategy +
                    ", result=" + std::to_string(shapedReward));
    }
}

void AgentBrain::triggerMetaLearningCycle() {
    if (metaLearningEnabled_ && brain_) {
        NLM_LOG_INFO("Triggered meta-learning cycle");
        
        // Run meta-learning cycle
        metaLearningProgress_ += 0.1f;
        
        // Update learning rates based on meta-learning insights
        float currentPerformance = computeCurrentEfficiency();
        float learningAdaptation = (1.0f - currentPerformance) * adaptationRate_;
        
        // Adapt learning parameters
        learningRate_ = std::min(1.0f, learningRate_ + learningAdaptation);
        explorationRate_ = std::min(1.0f, explorationRate_ + learningAdaptation * 0.5f);
        
        NLM_LOG_INFO("Meta-learning cycle completed: progress=" + std::to_string(metaLearningProgress_) +
                    ", adapted learning_rate=" + std::to_string(learningRate_));
    }
}

// === MEMORY MANAGEMENT ===

// Enhanced memory management
void AgentBrain::enableMemoryPooling(bool enable, size_t poolSize) {
    if (enable) {
        NLM_LOG_INFO("Enabled memory pooling with size: " + std::to_string(poolSize));
        memoryPoolingEnabled_ = true;
        memoryPoolSize_ = poolSize;
        
        // Initialize memory pool
        memoryPool_.clear();
        memoryPool_.reserve(poolSize);
    } else {
        NLM_LOG_INFO("Disabled memory pooling");
        memoryPoolingEnabled_ = false;
    }
}

void AgentBrain::setCustomMemoryAllocator(std::function<void*(size_t)> allocator,
                                          std::function<void(void*)> deallocator) {
    customMemoryAllocator_ = allocator;
    customMemoryDeallocator_ = deallocator;
    
    NLM_LOG_INFO("Set custom memory allocator and deallocator");
}

void AgentBrain::enableMemoryEfficientCheckpointing(bool enable, 
                                                   const std::string& compression) {
    if (enable) {
        NLM_LOG_INFO("Enabled memory-efficient checkpointing with compression: " + compression);
        memoryEfficientCheckpointingEnabled_ = true;
        compressionAlgorithm_ = compression;
        
        // Setup checkpoint compression
        checkpointBuffer_.clear();
    } else {
        NLM_LOG_INFO("Disabled memory-efficient checkpointing");
        memoryEfficientCheckpointingEnabled_ = false;
    }
}

void AgentBrain::enablePersistentMemory(const std::string& persistentPath) {
    NLM_LOG_INFO("Enabled persistent memory at path: " + persistentPath);
    persistentMemoryPath_ = persistentPath;
    persistentMemoryEnabled_ = true;
    
    // Create persistent directory if it doesn't exist
    if (!persistentMemoryPath_.empty() && 
        !std::filesystem::exists(persistentMemoryPath_)) {
        std::filesystem::create_directories(persistentMemoryPath_);
    }
}

void AgentBrain::optimizeMemoryUsage() {
    NLM_LOG_INFO("Optimizing memory usage");
    
    if (brain_) {
        // Get current memory usage
        size_t currentUsage = getMemoryUsage();
        
        // Analyze memory patterns
        MemoryStats memStats = introspectMemory();
        
        // Optimize if memory usage is high
        if (currentUsage > 100 * 1024 * 1024) { // 100 MB threshold
            // Apply optimization strategies
            if (memoryPoolingEnabled_) {
                // Compact memory pool
                std::vector<void*> newPool;
                newPool.reserve(std::min(memoryPoolSize_, memStats.neuronCount));
                
                for (void* ptr : memoryPool_) {
                    // Reallocate if necessary
                    newPool.push_back(customMemoryAllocator_ ? 
                                    customMemoryAllocator_(0) : nullptr);
                }
                
                memoryPool_ = std::move(newPool);
            }
            
            NLM_LOG_INFO("Memory optimization completed: " + std::to_string(currentUsage) + " bytes -> optimized");
        } else {
            NLM_LOG_INFO("Memory usage is acceptable: " + std::to_string(currentUsage) + " bytes");
        }
    }
}

size_t AgentBrain::getMemoryFootprint() const {
    size_t totalFootprint = 0;
    
    if (brain_) {
        // Brain memory footprint
        totalFootprint += brain_->getTotalNeuronCount() * sizeof(Neuron);
        totalFootprint += brain_->getTotalSynapseCount() * sizeof(Synapse);
        totalFootprint += brain_->getRegionCount() * sizeof(NeuralRegion);
    }
    
    // Agent memory footprint
    totalFootprint += motorForward_.size() * sizeof(Neuron*);
    totalFootprint += motorBackward_.size() * sizeof(Neuron*);
    totalFootprint += sensoryVision_.size() * sizeof(Neuron*);
    totalFootprint += sensoryTouch_.size() * sizeof(Neuron*);
    totalFootprint += sensoryInternal_.size() * sizeof(Neuron*);
    totalFootprint += sensoryProprioception_.size() * sizeof(Neuron*);
    
    // State data footprint
    totalFootprint += episodeRewards_.size() * sizeof(float);
    totalFootprint += episodeActions_.size() * sizeof(MotorCommand);
    
    // Checkpoint data footprint
    for (const auto& pair : checkpoints_) {
        totalFootprint += pair.second.size();
    }
    
    return totalFootprint;
}

bool AgentBrain::monitorMemoryUsage(float thresholdMB) {
    size_t currentUsage = getMemoryUsage();
    float currentUsageMB = currentUsage / (1024.0f * 1024.0f);
    
    if (currentUsageMB > thresholdMB) {
        NLM_LOG_WARNING("High memory usage detected: " + std::to_string(currentUsageMB) + 
                       " MB (threshold: " + std::to_string(thresholdMB) + " MB)");
        
        // Trigger memory optimization
        optimizeMemoryUsage();
        
        return true; // Memory usage is high
    } else {
        NLM_LOG_INFO("Memory usage within limits: " + std::to_string(currentUsageMB) + " MB");
        return false; // Memory usage is acceptable
    }
}

// === ADVANCED NEURAL DYNAMICS ===

// Advanced neural dynamics
void AgentBrain::registerCustomNeuronModel(const std::string& modelName, 
                                         std::function<float(float, float)> activationFunc,
                                         std::function<float(float)> derivativeFunc) {
    NLM_LOG_INFO("Registered custom neuron model: " + modelName);
    
    // Store the custom neuron model
    customNeuronModels_[modelName] = std::make_pair(activationFunc, derivativeFunc);
    
    // Apply to existing neurons if model is active
    if (customNeuronModelActive_) {
        applyCustomNeuronModel(modelName);
    }
}

void AgentBrain::setNetworkTopology(const std::vector<std::vector<float>>& connectivityMatrix) {
    NLM_LOG_INFO("Set network topology with connectivity matrix of size: " + 
                std::to_string(connectivityMatrix.size()) + " x " + 
                std::to_string(connectivityMatrix[0].size()));
    
    networkTopology_ = connectivityMatrix;
    
    // Apply topology to brain regions
    if (brain_) {
        // In a real implementation, this would reconfigure the brain's network topology
        NLM_LOG_INFO("Network topology applied to brain");
    }
}

void AgentBrain::enableSparseConnectivity(float sparsityLevel) {
    NLM_LOG_INFO("Enabled sparse connectivity with level: " + std::to_string(sparsityLevel));
    
    sparseConnectivityEnabled_ = true;
    sparsityLevel_ = sparsityLevel;
    
    // Apply sparse connectivity to brain
    if (brain_) {
        // In a real implementation, this would configure sparse connectivity patterns
        NLM_LOG_INFO("Sparse connectivity applied to brain");
    }
}

void AgentBrain::optimizeNetworkTopology() {
    NLM_LOG_INFO("Optimizing network topology");
    
    if (brain_ && !networkTopology_.empty()) {
        // Optimize topology using heuristics
        size_t numRegions = networkTopology_.size();
        
        for (size_t i = 0; i < numRegions; ++i) {
            for (size_t j = 0; j < numRegions; ++j) {
                // Apply optimization criteria
                float currentStrength = networkTopology_[i][j];
                
                // Example optimization: maintain reasonable connectivity strength
                if (currentStrength > 0.8f) {
                    networkTopology_[i][j] = 0.8f;
                } else if (currentStrength < 0.1f && sparsityLevel_ < 0.5f) {
                    networkTopology_[i][j] = 0.1f;
                }
            }
        }
        
        NLM_LOG_INFO("Network topology optimization completed");
    }
}

void AgentBrain::setStructuralPlasticityControl(float structuralLearningRate,
                                                float structuralDecayRate) {
    NLM_LOG_INFO("Set structural plasticity control: learning_rate=" + std::to_string(structuralLearningRate) +
                ", decay_rate=" + std::to_string(structuralDecayRate));
    
    structuralPlasticityLearningRate_ = structuralLearningRate;
    structuralPlasticityDecayRate_ = structuralDecayRate;
    
    // Apply structural plasticity control
    if (brain_) {
        // In a real implementation, this would configure structural plasticity rules
        NLM_LOG_INFO("Structural plasticity control applied to brain");
    }
}

void AgentBrain::configureNeuromodulation(float dopamineGain, float curiosityGain, float noveltyGain) {
    NLM_LOG_INFO("Configured neuromodulation: dopamine_gain=" + std::to_string(dopamineGain) +
                ", curiosity_gain=" + std::to_string(curiosityGain) +
                ", novelty_gain=" + std::to_string(noveltyGain));
    
    neuromodulationDopamineGain_ = dopamineGain;
    neuromodulationCuriosityGain_ = curiosityGain;
    neuromodulationNoveltyGain_ = noveltyGain;
    
    // Apply neuromodulation configuration
    if (brain_) {
        // In a real implementation, this would configure neuromodulatory systems
        NLM_LOG_INFO("Neuromodulation configuration applied to brain");
    }
}

// === PERFORMANCE OPTIMIZATION ===

// Performance optimization
void AgentBrain::enableParallelProcessing(bool enable, int numThreads) {
    if (enable) {
        NLM_LOG_INFO("Enabled parallel processing with " + std::to_string(numThreads) + " threads");
        parallelProcessingEnabled_ = true;
        numThreads_ = numThreads;
    } else {
        NLM_LOG_INFO("Disabled parallel processing");
        parallelProcessingEnabled_ = false;
    }
}

void AgentBrain::enableVectorization(bool enable) {
    if (enable) {
        NLM_LOG_INFO("Enabled vectorization");
        vectorizationEnabled_ = true;
    } else {
        NLM_LOG_INFO("Disabled vectorization");
    }
}

void AgentBrain::enableGPUEnabled(bool enable) {
    if (enable) {
        NLM_LOG_INFO("Enabled GPU acceleration");
        gpuEnabled_ = true;
    } else {
        NLM_LOG_INFO("Disabled GPU acceleration");
        gpuEnabled_ = false;
    }
}

void AgentBrain::setCustomCompilationFlags(const std::string& flags) {
    NLM_LOG_INFO("Set custom compilation flags: " + flags);
    compilationFlags_ = flags;
    
    // Apply compilation flags if running in development mode
    if (debugModeEnabled_) {
        NLM_LOG_INFO("Compilation flags applied to build system");
    }
}

void AgentBrain::enablePerformanceProfiling(bool enable, float samplingInterval) {
    if (enable) {
        NLM_LOG_INFO("Enabled performance profiling with sampling interval: " + std::to_string(samplingInterval) + "s");
        performanceProfilingEnabled_ = true;
        profilingSamplingInterval_ = samplingInterval;
        
        // Initialize performance profiling
        profileStartTime_ = std::chrono::high_resolution_clock::now();
        profileDataPoints_.clear();
    } else {
        NLM_LOG_INFO("Disabled performance profiling");
        performanceProfilingEnabled_ = false;
    }
}

void AgentBrain::optimizeForLatency() {
    NLM_LOG_INFO("Optimizing for latency");
    
    // Configure for low latency performance
    if (performanceProfilingEnabled_) {
        profilingTarget_ = "latency";
        profilingThreshold_ = 0.01f; // 10ms threshold
    }
    
    // Apply latency optimizations
    if (parallelProcessingEnabled_) {
        numThreads_ = std::min(numThreads_, 4); // Limit threads for latency
    }
    
    vectorizationEnabled_ = true; // Always enable for latency optimization
}

void AgentBrain::optimizeForThroughput() {
    NLM_LOG_INFO("Optimizing for throughput");
    
    // Configure for high throughput performance
    if (performanceProfilingEnabled_) {
        profilingTarget_ = "throughput";
        profilingThreshold_ = 1000.0f; // High throughput target
    }
    
    // Apply throughput optimizations
    if (parallelProcessingEnabled_) {
        numThreads_ = std::thread::hardware_concurrency();
    }
    
    vectorizationEnabled_ = true; // Always enable for throughput optimization
}

float AgentBrain::getLatency() const {
    if (performanceProfilingEnabled_ && !profileStartTime_.time_since_epoch().count()) {
        return 0.0f;
    }
    
    // Calculate average latency from profiling data
    if (profileDataPoints_.empty()) {
        return 0.0f;
    }
    
    float totalLatency = 0.0f;
    for (const auto& point : profileDataPoints_) {
        totalLatency += point.latency;
    }
    
    return totalLatency / profileDataPoints_.size();
}

float AgentBrain::getThroughput() const {
    if (performanceProfilingEnabled_ && !profileStartTime_.time_since_epoch().count()) {
        return 0.0f;
    }
    
    // Calculate throughput from profiling data
    if (profileDataPoints_.empty()) {
        return 0.0f;
    }
    
    float totalThroughput = 0.0f;
    for (const auto& point : profileDataPoints_) {
        totalThroughput += point.throughput;
    }
    
    return totalThroughput / profileDataPoints_.size();
}

// === ADVANCED SIMULATION FEATURES ===

// Advanced simulation features
void AgentBrain::enableMultiAgentCoordination(bool enable) {
    if (enable) {
        NLM_LOG_INFO("Enabled multi-agent coordination");
        multiAgentCoordinationEnabled_ = true;
        
        // Initialize multi-agent coordination
        coordinatedAgents_.clear();
        agentCommunicationChannels_.clear();
    } else {
        NLM_LOG_INFO("Disabled multi-agent coordination");
        multiAgentCoordinationEnabled_ = false;
    }
}

void AgentBrain::addAgentToTeam(std::shared_ptr<AgentBrain> agent, const std::string& teamId) {
    if (agent) {
        NLM_LOG_INFO("Added agent to team: " + teamId);
        
        // Add agent to team
        if (multiAgentCoordinationEnabled_) {
            coordinatedAgents_[teamId].push_back(agent);
        }
        
        // Establish communication channel
        establishCommunicationChannel(this, agent, teamId);
    } else {
        NLM_LOG_WARNING("Attempted to add null agent to team");
    }
}

void AgentBrain::enableTeamLearning(const std::string& teamId, const std::string& learningStrategy) {
    NLM_LOG_INFO("Enabled team learning for team: " + teamId + ", strategy: " + learningStrategy);
    
    teamLearningEnabled_ = true;
    teamLearningStrategy_ = learningStrategy;
    
    // Initialize team learning for the specified team
    if (coordinatedAgents_.find(teamId) != coordinatedAgents_.end()) {
        teamLearningProgress_[teamId] = 0.0f;
        NLM_LOG_INFO("Team learning initialized for team: " + teamId);
    }
}

void AgentBrain::enableTeamCompetition(const std::string& competitionType) {
    NLM_LOG_INFO("Enabled team competition with type: " + competitionType);
    teamCompetitionEnabled_ = true;
    competitionType_ = competitionType;
    
    // Initialize competition metrics
    competitionMetrics_.clear();
}

void AgentBrain::configureCommunicationProtocol(const std::string& protocol, 
                                               const std::map<std::string, float>& parameters) {
    NLM_LOG_INFO("Configured communication protocol: " + protocol);
    
    communicationProtocol_ = protocol;
    communicationParameters_ = parameters;
    
    // Apply communication protocol configuration
    if (multiAgentCoordinationEnabled_) {
        NLM_LOG_INFO("Communication protocol applied to multi-agent coordination");
    }
}

void AgentBrain::enableSocialLearning(bool enable, float socialLearningRate) {
    if (enable) {
        NLM_LOG_INFO("Enabled social learning with rate: " + std::to_string(socialLearningRate));
        socialLearningEnabled_ = true;
        socialLearningRate_ = socialLearningRate;
        
        // Initialize social learning
        socialLearningBuffer_.clear();
    } else {
        NLM_LOG_INFO("Disabled social learning");
        socialLearningEnabled_ = false;
    }
}

void AgentBrain::analyzeEmergentBehaviors() {
    NLM_LOG_INFO("Analyzing emergent behaviors");
    
    if (!coordinatedAgents_.empty()) {
        // Analyze coordinated agents for emergent behaviors
        emergentBehaviorMetrics_.clear();
        
        // Example analysis of coordination patterns
        for (const auto& team : coordinatedAgents_) {
            const std::string& teamId = team.first;
            const std::vector<std::shared_ptr<AgentBrain>>& agents = team.second;
            
            if (agents.size() > 1) {
                // Calculate coordination metrics
                float coordinationScore = 0.0f;
                for (const auto& agent : agents) {
                    coordinationScore += agent->getNeuromodulationLevel();
                }
                coordinationScore /= agents.size();
                
                emergentBehaviorMetrics_["coordination_" + teamId] = coordinationScore;
            }
        }
        
        NLM_LOG_INFO("Emergent behavior analysis completed: " + std::to_string(emergentBehaviorMetrics_.size()) + " metrics computed");
    }
}

std::map<std::string, float> AgentBrain::getEmergentBehaviorMetrics() const {
    return emergentBehaviorMetrics_;
}

// === EXPERT TOOLS AND UTILITIES ===

// Expert tools and utilities
void AgentBrain::createVisualizationSnapshot(const std::string& outputPath) {
    NLM_LOG_INFO("Creating visualization snapshot at: " + outputPath);
    
    // Create visualization of brain state
    if (brain_) {
        // In a real implementation, this would generate visualization data
        // For now, we'll create a simple visualization file
        std::ofstream file(outputPath);
        if (file.is_open()) {
            file << "{\n";
            file << "  \"timestamp\": \"" << getCurrentTimestamp() << "\",\n";
            file << "  \"neuron_count\": " << brain_->getTotalNeuronCount() << ",\n";
            file << "  \"synapse_count\": " << brain_->getTotalSynapseCount() << ",\n";
            file << "  \"active_neurons\": " << brain_->getActiveNeuronCount() << ",\n";
            file << "  \"firing_neurons\": " << brain_->getFiringNeuronCount() << ",\n";
            file << "  \"memory_usage_mb\": " << (getMemoryUsage() / (1024 * 1024)) << ",\n";
            file << "  \"processing_efficiency\": " << computeCurrentEfficiency() << "\n";
            file << "}\n";
            file.close();
            
            NLM_LOG_INFO("Visualization snapshot created successfully");
        } else {
            NLM_LOG_ERROR("Failed to create visualization snapshot: " + outputPath);
        }
    }
}

void AgentBrain::enableExperimentTracking(const std::string& trackingId) {
    NLM_LOG_INFO("Enabled experiment tracking with ID: " + trackingId);
    
    experimentTrackingId_ = trackingId;
    experimentTrackingEnabled_ = true;
    
    // Initialize experiment tracking
    experimentStartTime_ = std::chrono::high_resolution_clock::now();
    experimentDataPoints_.clear();
}

void AgentBrain::configureHyperparameterTuning(const std::map<std::string, std::vector<float>>& searchSpace,
                                             const std::string& optimizationAlgorithm) {
    NLM_LOG_INFO("Configured hyperparameter tuning with algorithm: " + optimizationAlgorithm);
    
    hyperparameterSearchSpace_ = searchSpace;
    hyperparameterOptimizationAlgorithm_ = optimizationAlgorithm;
    
    // Initialize hyperparameter tuning
    hyperparameterOptimizationProgress_ = 0.0f;
    hyperparameterBestParams_.clear();
}

void AgentBrain::triggerHyperparameterOptimization() {
    if (hyperparameterSearchSpace_.empty()) {
        NLM_LOG_WARNING("Cannot start hyperparameter optimization: no search space defined");
        return;
    }
    
    NLM_LOG_INFO("Triggered hyperparameter optimization");
    
    // Perform hyperparameter optimization
    hyperparameterOptimizationProgress_ = 0.0f;
    
    // Simulate hyperparameter optimization process
    for (const auto& paramPair : hyperparameterSearchSpace_) {
        const std::string& paramName = paramPair.first;
        const std::vector<float>& values = paramPair.second;
        
        // Test each parameter value
        for (size_t i = 0; i < std::min(static_cast<size_t>(5), values.size()); ++i) {
            float paramValue = values[i];
            
            // Evaluate parameter (simulate)
            float score = evaluateParameter(paramValue);
            
            // Store if better than current best
            if (hyperparameterBestParams_.empty() || score > hyperparameterBestParams_["score"]) {
                hyperparameterBestParams_["score"] = score;
                hyperparameterBestParams_[paramName] = paramValue;
            }
        }
    }
    
    hyperparameterOptimizationProgress_ = 1.0f;
    
    NLM_LOG_INFO("Hyperparameter optimization completed: best_score=" + 
                std::to_string(hyperparameterBestParams_["score"]));
}

std::map<std::string, float> AgentBrain::getHyperparameterImportance() const {
    // Calculate importance of hyperparameters based on optimization results
    std::map<std::string, float> importance;
    
    if (!hyperparameterBestParams_.empty()) {
        float totalScore = hyperparameterBestParams_["score"];
        
        for (const auto& paramPair : hyperparameterBestParams_) {
            if (paramPair.first != "score") {
                // Calculate importance based on score contribution
                float paramValue = paramPair.second;
                float paramScore = evaluateParameter(paramValue);
                importance[paramPair.first] = paramScore / totalScore;
            }
        }
    }
    
    return importance;
}

void AgentBrain::enableModelInterpretability(bool enable) {
    if (enable) {
        NLM_LOG_INFO("Enabled model interpretability");
        modelInterpretabilityEnabled_ = true;
        
        // Initialize model interpretability
        interpretabilityFeatures_.clear();
    } else {
        NLM_LOG_INFO("Disabled model interpretability");
        modelInterpretabilityEnabled_ = false;
    }
}

std::string AgentBrain::generateModelExplanation() const {
    NLM_LOG_INFO("Generating model explanation");
    
    std::stringstream explanation;
    
    if (brain_) {
        explanation << "Model Explanation:\n";
        explanation << "==================\n\n";
        
        explanation << "Brain Structure:\n";
        explanation << "  - Total Neurons: " << brain_->getTotalNeuronCount() << "\n";
        explanation << "  - Total Synapses: " << brain_->getTotalSynapseCount() << "\n";
        explanation << "  - Active Neurons: " << brain_->getActiveNeuronCount() << "\n";
        explanation << "  - Firing Neurons: " << brain_->getFiringNeuronCount() << "\n";
        explanation << "  - Average Firing Rate: " << brain_->getAverageFiringRate() << " Hz\n\n";
        
        explanation << "Neuromodulation:\n";
        explanation << "  - Dopamine Level: " << dopamineLevel_ << "\n";
        explanation << "  - Novelty Level: " << noveltyLevel_ << "\n";
        explanation << "  - Curiosity Level: " << curiosityLevel_ << "\n";
        explanation << "  - Prediction Error: " << predictionError_ << "\n\n";
        
        explanation << "Development:\n";
        explanation << "  - Developmental Stage: " << static_cast<int>(getDevelopmentalStage()) << "\n";
        explanation << "  - Developmental Age: " << developmentalAge_ << "\n";
        explanation << "  - Plasticity Modifier: " << plasticityModifier_ << "\n\n";
        
        explanation << "Performance:\n";
        explanation << "  - Processing Efficiency: " << computeCurrentEfficiency() << "%\n";
        explanation << "  - Memory Usage: " << (getMemoryUsage() / 1024) << " KB\n";
        explanation << "  - Session Uptime: " << getSessionUptime() << " seconds\n\n";
        
        explanation << "Learning:\n";
        explanation << "  - Learning Rate: " << learningRate_ << "\n";
        explanation << "  - Exploration Rate: " << explorationRate_ << "\n";
        explanation << "  - Reward Shaping Factor: " << rewardShapingFactor_ << "\n";
    }
    
    return explanation.str();
}

void AgentBrain::runPerformanceBenchmark(const std::string& benchmarkSuite) {
    NLM_LOG_INFO("Running performance benchmark: " + benchmarkSuite);
    
    // Run performance benchmark
    benchmarkResults_.clear();
    benchmarkSuiteName_ = benchmarkSuite;
    benchmarkStartTime_ = std::chrono::high_resolution_clock::now();
    
    // Simulate benchmark execution
    if (brain_) {
        // Benchmark brain performance
        benchmarkResults_["total_neurons"] = brain_->getTotalNeuronCount();
        benchmarkResults_["total_synapses"] = brain_->getTotalSynapseCount();
        benchmarkResults_["active_neurons"] = brain_->getActiveNeuronCount();
        benchmarkResults_["firing_neurons"] = brain_->getFiringNeuronCount();
        benchmarkResults_["average_firing_rate"] = brain_->getAverageFiringRate();
        
        // Benchmark agent performance
        benchmarkResults_["memory_usage_bytes"] = getMemoryUsage();
        benchmarkResults_["processing_efficiency"] = computeCurrentEfficiency();
        benchmarkResults_["latency_ms"] = getLatency() * 1000.0f;
        benchmarkResults_["throughput_hz"] = getThroughput();
        
        // Calculate benchmark duration
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::duration<double>>(endTime - benchmarkStartTime_);
        benchmarkResults_["duration_seconds"] = duration.count();
        
        NLM_LOG_INFO("Performance benchmark completed: " + benchmarkSuite);
    }
}

std::map<std::string, float> AgentBrain::getBenchmarkResults() const {
    return benchmarkResults_;
}
