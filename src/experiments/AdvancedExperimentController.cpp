# Advanced Experiment Control and Analysis Module
# Provides advanced users with sophisticated control over experiments and detailed analysis capabilities

namespace nlm {

class AdvancedExperimentController {
private:
    std::shared_ptr<Brain> brain;
    std::shared_ptr<Config> config;
    
    // Experiment state tracking
    struct ExperimentState {
        size_t currentStep;
        size_t totalSteps;
        size_t experimentStartTime;
        std::vector<std::string> stimulusHistory;
        std::vector<float> performanceMetrics;
        std::vector<float> synapticChanges;
        bool experimentRunning;
        bool experimentPaused;
    } state;
    
    // Advanced stimulus control
    struct StimulusPattern {
        std::string name;
        std::function<void(std::shared_ptr<Brain>)> applyStimulus;
        std::vector<float> parameters;
        size_t duration;
        float intensity;
    };
    
    std::vector<StimulusPattern> stimulusPatterns;
    
    // Performance tracking
    struct PerformanceMetrics {
        float learningRate;
        float synapticStability;
        float networkStability;
        float explorationExploitationBalance;
        float memoryRetention;
        float predictionAccuracy;
    } currentMetrics;
    
public:
    AdvancedExperimentController(std::shared_ptr<Brain> brain, std::shared_ptr<Config> config)
        : brain(brain), config(config) {
        state.currentStep = 0;
        state.totalSteps = config->getOr<size_t>("max_simulation_steps", 10000);
        state.experimentStartTime = getCurrentTimestamp();
        state.experimentRunning = false;
        state.experimentPaused = false;
        
        initializeAdvancedFeatures();
    }
    
    void initializeAdvancedFeatures() {
        // Initialize advanced stimulus patterns
        initializeStimulusPatterns();
        
        // Initialize performance monitoring
        initializePerformanceTracking();
        
        // Initialize analysis tools
        initializeAnalysisTools();
        
        NLM_LOG_INFO("Advanced experiment controller initialized.");
    }
    
    void initializeStimulusPatterns() {
        // Define advanced stimulus patterns for experienced users
        
        // Pattern 1: Spike-Timing-Dependent Learning (STDP) stimulation
        stimulusPatterns.push_back({
            "stdp_learning",
            [this](std::shared_ptr<Brain> brain) {
                applySTDI StimulusPattern();
            },
            {0.001f, 0.01f, 20.0f},  // LTP weight, LTD weight, STDP tau
            1000, 1.0f
        });
        
        // Pattern 2: Hebbian learning with memory modulation
        stimulusPatterns.push_back({
            "hebbian_memory",
            [this](std::shared_ptr<Brain> brain) {
                applyHebbianLearningWithMemory();
            },
            {0.01f, 0.5f, 0.1f},  // Hebbian rate, memory capacity, decay rate
            1500, 0.8f
        });
        
        // Pattern 3: Neuromodulation-based learning
        stimulusPatterns.push_back({
            "neuromodulation",
            [this](std::shared_ptr<Brain> brain) {
                applyNeuromodulationBasedLearning();
            },
            {0.05f, 0.1f, 0.5f},  // Dopamine level, curiosity rate, novelty sensitivity
            2000, 0.9f
        });
        
        // Pattern 4: Developmental learning
        stimulusPatterns.push_back({
            "developmental",
            [this](std::shared_ptr<Brain> brain) {
                applyDevelopmentalLearning();
            },
            {100, 0.5f, 0.001f},  // Critical period start, maturation rate, aging rate
            3000, 0.7f
        });
    }
    
    void initializePerformanceTracking() {
        // Initialize performance metrics tracking
        currentMetrics.learningRate = 0.0f;
        currentMetrics.synapticStability = 1.0f;
        currentMetrics.networkStability = 1.0f;
        currentMetrics.explorationExploitationBalance = 0.5f;
        currentMetrics.memoryRetention = 1.0f;
        currentMetrics.predictionAccuracy = 0.0f;
        
        NLM_LOG_INFO("Performance tracking initialized.");
    }
    
    void initializeAnalysisTools() {
        // Initialize analysis tools for advanced users
        NLM_LOG_INFO("Analysis tools initialized.");
    }
    
    void startExperiment(const std::string& patternName) {
        if (state.experimentRunning) {
            NLM_LOG_WARNING("Experiment already running. Stopping current experiment.");
            stopExperiment();
        }
        
        // Find the stimulus pattern
        auto it = std::find_if(stimulusPatterns.begin(), stimulusPatterns.end(),
                              [&patternName](const StimulusPattern& pattern) {
                                  return pattern.name == patternName;
                              });
        
        if (it == stimulusPatterns.end()) {
            NLM_LOG_ERROR("Unknown experiment pattern: " + patternName);
            return;
        }
        
        currentPattern = *it;
        state.experimentRunning = true;
        state.experimentStartTime = getCurrentTimestamp();
        
        NLM_LOG_INFO(std::string("Starting experiment: ") + patternName);
        NLM_LOG_INFO("  Pattern duration: " + std::to_string(currentPattern.duration) + " steps");
        NLM_LOG_INFO("  Pattern intensity: " + std::to_string(currentPattern.intensity));
        
        // Record initial state
        recordExperimentState();
    }
    
    void stopExperiment() {
        if (!state.experimentRunning) {
            NLM_LOG_INFO("Experiment not running.");
            return;
        }
        
        NLM_LOG_INFO("Stopping experiment...");
        
        // Record final state
        recordExperimentState();
        
        // Compute performance metrics
        computePerformanceMetrics();
        
        // Generate experiment report
        generateExperimentReport();
        
        state.experimentRunning = false;
        state.currentStep = 0;
        
        NLM_LOG_INFO("Experiment completed successfully.");
    }
    
    void pauseExperiment() {
        if (!state.experimentRunning) {
            NLM_LOG_INFO("Experiment not running.");
            return;
        }
        
        NLM_LOG_INFO("Pausing experiment...");
        state.experimentPaused = true;
    }
    
    void resumeExperiment() {
        if (!state.experimentRunning) {
            NLM_LOG_INFO("Experiment not running.");
            return;
        }
        
        if (!state.experimentPaused) {
            NLM_LOG_INFO("Experiment not paused.");
            return;
        }
        
        NLM_LOG_INFO("Resuming experiment...");
        state.experimentPaused = false;
    }
    
    void runExperimentStep() {
        if (!state.experimentRunning || state.experimentPaused) {
            return;
        }
        
        state.currentStep++;
        
        // Apply current stimulus pattern
        currentPattern.applyStimulus(brain);
        
        // Update performance metrics
        updatePerformanceMetrics();
        
        // Log progress periodically
        if (state.currentStep % 100 == 0) {
            logExperimentProgress();
        }
        
        // Check if experiment is complete
        if (state.currentStep >= state.totalSteps) {
            NLM_LOG_INFO("Experiment completed: reached maximum steps (" + std::to_string(state.totalSteps) + ")");
            stopExperiment();
        }
    }
    
    void applySTDI StimulusPattern() {
        // Apply Spike-Timing-Dependent Learning stimulus
        auto* region = brain->getRegion(RegionId(1));
        if (!region) return;
        
        auto neurons = region->getAllNeurons();
        if (neurons.size() < 2) return;
        
        // Create correlated pre->post spike activity to trigger LTP
        for (int trial = 0; trial < 10; ++trial) {
            Neuron* preNeuron = neurons[0];
            Neuron* postNeuron = neurons[1];
            
            // Pre fires first
            preNeuron->injectCurrent(60.0f);
            brain->step(trial * 2, trial * 2 * 0.001);
            
            // Post fires shortly after pre
            postNeuron->injectCurrent(60.0f);
            brain->step(trial * 2 + 1, (trial * 2 + 1) * 0.001);
        }
        
        recordExperimentState();
    }
    
    void applyHebbianLearningWithMemory() {
        // Apply Hebbian learning with memory system modulation
        auto* region = brain->getRegion(RegionId(1));
        if (!region) return;
        
        // Enable Hebbian plasticity
        if (auto* region = brain->getRegion(RegionId(1))) {
            for (auto& syn : region->getSynapses()) {
                syn->enablePlasticity(true, false, false);  // Enable only Hebbian
            }
        }
        
        // Apply correlated activity to strengthen synapses
        auto neurons = region->getAllNeurons();
        if (neurons.size() < 2) return;
        
        for (int trial = 0; trial < 20; ++trial) {
            neurons[0]->injectCurrent(40.0f);
            brain->step(trial * 2, trial * 2 * 0.001);
            
            neurons[1]->injectCurrent(40.0f);
            brain->step(trial * 2 + 1, (trial * 2 + 1) * 0.001);
        }
        
        recordExperimentState();
    }
    
    void applyNeuromodulationBasedLearning() {
        // Apply neuromodulation-based learning
        auto* region = brain->getRegion(RegionId(1));
        if (!region) return;
        
        // Enable neuromodulation
        if (auto* region = brain->getRegion(RegionId(1))) {
            for (auto& syn : region->getSynapses()) {
                syn->enablePlasticity(true, true, true);  // Enable all plasticity types
            }
        }
        
        // Create neuromodulation signals
        applyDopamineModulation(0.5f);
        applyCuriosityModulation(0.3f);
        applyNoveltyModulation(0.2f);
        
        // Apply learning stimuli
        auto neurons = region->getAllNeurons();
        if (neurons.size() < 2) return;
        
        for (int trial = 0; trial < 15; ++trial) {
            neurons[0]->injectCurrent(30.0f);
            brain->step(trial * 2, trial * 2 * 0.001);
            
            neurons[1]->injectCurrent(30.0f);
            brain->step(trial * 2 + 1, (trial * 2 + 1) * 0.001);
        }
        
        recordExperimentState();
    }
    
    void applyDevelopmentalLearning() {
        // Apply developmental learning process
        auto* region = brain->getRegion(RegionId(1));
        if (!region) return;
        
        // Advance developmental stage
        brain->setDevelopmentalStage(DevelopmentalStage::CriticalPeriod);
        
        // Apply age-dependent plasticity
        auto* sp = brain->getStructuralPlasticity();
        if (sp) {
            sp->setSynaptogenesisRate(0.00005f);
            sp->setPruningRate(0.000005f);
        }
        
        // Apply age-dependent learning stimuli
        auto neurons = region->getAllNeurons();
        if (neurons.size() < 2) return;
        
        for (int trial = 0; trial < 25; ++trial) {
            neurons[0]->injectCurrent(20.0f);
            brain->step(trial * 2, trial * 2 * 0.001);
            
            neurons[1]->injectCurrent(20.0f);
            brain->step(trial * 2 + 1, (trial * 2 + 1) * 0.001);
        }
        
        // Return to normal developmental stage
        brain->setDevelopmentalStage(DevelopmentalStage::Adult);
        
        recordExperimentState();
    }
    
    void applyDopamineModulation(float level) {
        if (auto* dopamine = brain->getDopamine()) {
            dopamine->setLevel(level);
        }
    }
    
    void applyCuriosityModulation(float level) {
        if (auto* curiosity = brain->getCuriosity()) {
            curiosity->setLevel(level);
        }
    }
    
    void applyNoveltyModulation(float level) {
        if (auto* novelty = brain->getNovelty()) {
            novelty->setLevel(level);
        }
    }
    
    void recordExperimentState() {
        // Record current experiment state
        state.stimulusHistory.push_back(getCurrentTimestamp());
        
        // Record performance metrics
        state.performanceMetrics.push_back(currentMetrics.learningRate);
        state.performanceMetrics.push_back(currentMetrics.synapticStability);
        state.performanceMetrics.push_back(currentMetrics.networkStability);
        
        // Record synaptic changes
        if (auto* region = brain->getRegion(RegionId(1))) {
            for (const auto& syn : region->getSynapses()) {
                state.synapticChanges.push_back(syn->getWeight());
            }
        }
    }
    
    void updatePerformanceMetrics() {
        // Update performance metrics based on current brain state
        if (auto* region = brain->getRegion(RegionId(1))) {
            auto neurons = region->getAllNeurons();
            
            // Calculate learning rate (change in synaptic weights)
            float learningRate = 0.0f;
            if (state.synapticChanges.size() > 1) {
                float initialAvg = 0.0f;
                float finalAvg = 0.0f;
                
                for (size_t i = 0; i < state.synapticChanges.size() / 2; ++i) {
                    initialAvg += state.synapticChanges[i];
                }
                initialAvg /= state.synapticChanges.size() / 2;
                
                for (size_t i = state.synapticChanges.size() / 2; i < state.synapticChanges.size(); ++i) {
                    finalAvg += state.synapticChanges[i];
                }
                finalAvg /= state.synapticChanges.size() / 2;
                
                learningRate = std::abs(finalAvg - initialAvg);
            }
            
            currentMetrics.learningRate = learningRate;
            
            // Calculate synaptic stability (variance of synaptic weights)
            float synapticVariance = 0.0f;
            if (!state.synapticChanges.empty()) {
                float mean = 0.0f;
                for (float weight : state.synapticChanges) {
                    mean += weight;
                }
                mean /= state.synapticChanges.size();
                
                for (float weight : state.synapticChanges) {
                    synapticVariance += (weight - mean) * (weight - mean);
                }
                synapticVariance /= state.synapticChanges.size();
                
                currentMetrics.synapticStability = 1.0f - synapticVariance;
            }
            
            // Calculate network stability (firing rate stability)
            float firingRate = brain->getFiringNeuronCount() / static_cast<float>(std::max(state.totalSteps, 1));
            currentMetrics.networkStability = std::min(1.0f, firingRate * 10.0f);
        }
    }
    
    void computePerformanceMetrics() {
        // Compute comprehensive performance metrics
        if (state.performanceMetrics.empty()) {
            return;
        }
        
        // Calculate average metrics
        float sumLearning = 0.0f;
        float sumStability = 0.0f;
        float sumNetworkStability = 0.0f;
        
        for (size_t i = 0; i < state.performanceMetrics.size(); i += 3) {
            if (i + 2 < state.performanceMetrics.size()) {
                sumLearning += state.performanceMetrics[i];
                sumStability += state.performanceMetrics[i + 1];
                sumNetworkStability += state.performanceMetrics[i + 2];
            }
        }
        
        size_t count = state.performanceMetrics.size() / 3;
        if (count > 0) {
            currentMetrics.learningRate = sumLearning / count;
            currentMetrics.synapticStability = sumStability / count;
            currentMetrics.networkStability = sumNetworkStability / count;
        }
        
        // Calculate memory retention (synaptic weight changes)
        if (!state.synapticChanges.empty() && state.synapticChanges.size() >= 2) {
            float initialAvg = 0.0f;
            float finalAvg = 0.0f;
            
            for (size_t i = 0; i < state.synapticChanges.size() / 2; ++i) {
                initialAvg += state.synapticChanges[i];
            }
            initialAvg /= state.synapticChanges.size() / 2;
            
            for (size_t i = state.synapticChanges.size() / 2; i < state.synapticChanges.size(); ++i) {
                finalAvg += state.synapticChanges[i];
            }
            finalAvg /= state.synapticChanges.size() / 2;
            
            currentMetrics.memoryRetention = 1.0f - std::abs(finalAvg - initialAvg);
        }
        
        // Calculate prediction accuracy (based on spike patterns)
        currentMetrics.predictionAccuracy = brain->getTotalSpikeCount() / static_cast<float>(std::max(state.totalSteps, 1));
        
        // Calculate exploration-exploitation balance
        currentMetrics.explorationExploitationBalance = 0.5f;  // Default balance
    }
    
    void logExperimentProgress() {
        NLM_LOG_INFO("Experiment Progress: " + std::to_string(state.currentStep) + "/" + std::to_string(state.totalSteps) + 
                    " steps (" + std::to_string((state.currentStep * 100) / state.totalSteps) + "%)");
        NLM_LOG_INFO("  Learning Rate: " + std::to_string(currentMetrics.learningRate));
        NLM_LOG_INFO("  Synaptic Stability: " + std::to_string(currentMetrics.synapticStability));
        NLM_LOG_INFO("  Network Stability: " + std::to_string(currentMetrics.networkStability));
        NLM_LOG_INFO("  Memory Retention: " + std::to_string(currentMetrics.memoryRetention));
        NLM_LOG_INFO("  Prediction Accuracy: " + std::to_string(currentMetrics.predictionAccuracy));
    }
    
    void generateExperimentReport() {
        // Generate comprehensive experiment report
        std::string report = "=== Advanced Experiment Report ===\n";
        report += "Experiment: " + currentPattern.name + "\n";
        report += "Duration: " + std::to_string(state.currentStep) + " steps\n";
        report += "Start Time: " + state.stimulusHistory.front() + "\n";
        report += "End Time: " + getCurrentTimestamp() + "\n\n";
        
        report += "=== Performance Metrics ===\n";
        report += "Learning Rate: " + std::to_string(currentMetrics.learningRate) + "\n";
        report += "Synaptic Stability: " + std::to_string(currentMetrics.synapticStability) + "\n";
        report += "Network Stability: " + std::to_string(currentMetrics.networkStability) + "\n";
        report += "Memory Retention: " + std::to_string(currentMetrics.memoryRetention) + "\n";
        report += "Prediction Accuracy: " + std::to_string(currentMetrics.predictionAccuracy) + "\n\n";
        
        report += "=== Synaptic Changes ===\n";
        report += "Initial Average Weight: " + std::to_string(calculateInitialAverageWeight()) + "\n";
        report += "Final Average Weight: " + std::to_string(calculateFinalAverageWeight()) + "\n";
        report += "Total Weight Change: " + std::to_string(calculateTotalWeightChange()) + "\n\n";
        
        report += "=== Analysis Summary ===\n";
        report += "Experiment " + (currentMetrics.learningRate > 0.1f ? "SUCCESS" : "FAILED"); + "\n";
        report += "Synaptic plasticity: " + (currentMetrics.synapticStability > 0.8f ? "STABLE" : "UNSTABLE") + "\n";
        report += "Network activity: " + (currentMetrics.networkStability > 0.5f ? "ACTIVE" : "QUIESCENT") + "\n";
        report += "Memory formation: " + (currentMetrics.memoryRetention > 0.8f ? "EFFECTIVE" : "INEFFECTIVE") + "\n\n";
        
        NLM_LOG_INFO(report);
        
        // Save report to file
        saveExperimentReport(report);
    }
    
    void saveExperimentReport(const std::string& report) {
        std::string filename = "experiment_report_" + getCurrentTimestamp() + ".txt";
        std::ofstream file(filename);
        if (file.is_open()) {
            file << report;
            file.close();
            NLM_LOG_INFO("Experiment report saved to: " + filename);
        } else {
            NLM_LOG_WARNING("Could not save experiment report to file.");
        }
    }
    
    float calculateInitialAverageWeight() {
        if (state.synapticChanges.empty()) return 0.0f;
        
        float sum = 0.0f;
        for (size_t i = 0; i < state.synapticChanges.size() / 2; ++i) {
            sum += state.synapticChanges[i];
        }
        return sum / (state.synapticChanges.size() / 2);
    }
    
    float calculateFinalAverageWeight() {
        if (state.synapticChanges.empty()) return 0.0f;
        
        float sum = 0.0f;
        for (size_t i = state.synapticChanges.size() / 2; i < state.synapticChanges.size(); ++i) {
            sum += state.synapticChanges[i];
        }
        return sum / (state.synapticChanges.size() / 2);
    }
    
    float calculateTotalWeightChange() {
        float initial = calculateInitialAverageWeight();
        float final = calculateFinalAverageWeight();
        return final - initial;
    }
    
    std::string getCurrentTimestamp() {
        auto now = std::chrono::system_clock::now();
        auto in_time_t = std::chrono::system_clock::to_time_t(now);
        
        std::stringstream ss;
        ss << std::put_time(std::localtime(&in_time_t), "%Y-%m-%d %H:%M:%S");
        return ss.str();
    }
    
    bool isExperimentRunning() const {
        return state.experimentRunning;
    }
    
    bool isExperimentPaused() const {
        return state.experimentPaused;
    }
    
    size_t getCurrentStep() const {
        return state.currentStep;
    }
    
    size_t getTotalSteps() const {
        return state.totalSteps;
    }
    
    float getLearningRate() const {
        return currentMetrics.learningRate;
    }
    
    float getSynapticStability() const {
        return currentMetrics.synapticStability;
    }
    
    float getNetworkStability() const {
        return currentMetrics.networkStability;
    }
    
    float getMemoryRetention() const {
        return currentMetrics.memoryRetention;
    }
    
    float getPredictionAccuracy() const {
        return currentMetrics.predictionAccuracy;
    }
    
    std::vector<std::string> getAvailablePatterns() const {
        std::vector<std::string> patterns;
        for (const auto& pattern : stimulusPatterns) {
            patterns.push_back(pattern.name);
        }
        return patterns;
    }
    
    void setExperimentDuration(size_t steps) {
        state.totalSteps = steps;
    }
    
    void enableRealTimeMonitoring(bool enable) {
        // Enable or disable real-time monitoring
        if (enable) {
            NLM_LOG_INFO("Real-time monitoring enabled.");
        } else {
            NLM_LOG_INFO("Real-time monitoring disabled.");
        }
    }
    
    void enableAutoSave(bool enable) {
        // Enable or disable automatic saving
        if (enable) {
            NLM_LOG_INFO("Automatic saving enabled.");
        } else {
            NLM_LOG_INFO("Automatic saving disabled.");
        }
    }
    
    void configureAdvancedParameters(const std::string& prefix, const std::string& value) {
        // Configure advanced parameters
        config->set(prefix + "advanced_parameter", value, ConfigSource::Runtime);
        NLM_LOG_INFO(std::string("Advanced parameter configured: ") + prefix + "." + value);
    }
    
    std::string getExperimentStatus() const {
        std::string status = "Experiment ";
        if (!state.experimentRunning) {
            status += "not running";
        } else if (state.experimentPaused) {
            status += "paused";
        } else {
            status += "running";
        }
        
        status += ". Current step: " + std::to_string(state.currentStep) + "/" + std::to_string(state.totalSteps);
        return status;
    }
};

} // namespace nlm
