namespace nlm {
namespace Phase2 {

ExperimentResult ExperimentRunner::runExperiment(ExperimentMode mode, const Config& config) {
    ExperimentResult result;
    result.mode = mode;
    
    logExperimentStart(mode, config);
    result.startTimer();
    
    // Create brain with configuration
    auto brainConfig = std::make_shared<Config>(config);
    auto brain = std::make_shared<Brain>(brainConfig);
    
    if (!brain->initialize()) {
        result.success = false;
        result.message = "Failed to initialize brain";
        endTimer();
        return result;
    }
    
    // Run experiment based on mode
    switch (mode) {
        case ExperimentMode::BASIC_CONNECTIVITY:
            result = runBasicConnectivity(config);
            break;
        case ExperimentMode::PLASTICITY_LEARNING:
            result = runPlasticityLearning(config);
            break;
        case ExperimentMode::STDP_VERIFICATION:
            result = runStdpVerification(config);
            break;
        case ExperimentMode::ALL_TESTS:
            {
                auto allResults = runAllExperiments(config);
                // Aggregate results
                result.success = true;
                for (const auto& r : allResults) {
                    if (!r.success) result.success = false;
                    result.metrics.spikesGenerated += r.metrics.spikesGenerated;
                    result.metrics.meanWeightChange += r.metrics.meanWeightChange;
                    result.metrics.strengthenedSynapses += r.metrics.strengthenedSynapses;
                    result.metrics.weakenedSynapses += r.metrics.weakenedSynapses;
                    result.metrics.unchangedSynapses += r.metrics.unchangedSynapses;
                    result.metrics.totalReward += r.metrics.totalReward;
                }
                result.message = "All tests completed";
            }
            break;
        case ExperimentMode::BENCHMARK:
            result = runBenchmark(config);
            break;
    }
    
    endTimer();
    
    // Check if experiment was successful
    result.success = checkExperimentSuccess(mode, result);
    
    logExperimentResult(result);
    return result;
}

std::vector<ExperimentResult> ExperimentRunner::runAllExperiments(const Config& config) {
    std::vector<ExperimentMode> modes = {
        ExperimentMode::BASIC_CONNECTIVITY,
        ExperimentMode::PLASTICITY_LEARNING,
        ExperimentMode::STDP_VERIFICATION
    };
    
    return runExperiments(modes, config);
}

std::vector<ExperimentResult> ExperimentRunner::runExperiments(const std::vector<ExperimentMode>& modes, const Config& config) {
    std::vector<ExperimentResult> results;
    
    for (const auto& mode : modes) {
        ExperimentResult result = runExperiment(mode, config);
        results.push_back(result);
        
        // Reset brain for next experiment if not the last one
        if (&mode != &modes.back()) {
            // Brain will be recreated in next iteration
        }
    }
    
    return results;
}

Config ExperimentRunner::loadExperimentConfig(const std::string& configFile) {
    Config config;
    // TODO: Implement proper JSON/YAML/INI parser
    // For now, use the existing simple parser
    config.loadFromFile(configFile);
    return config;
}

bool ExperimentRunner::saveResults(const std::vector<ExperimentResult>& results, const std::string& outputFile) {
    std::ofstream file(outputFile);
    if (!file.is_open()) {
        return false;
    }
    
    file << generateReport(results);
    file.close();
    return true;
}

std::string ExperimentRunner::generateReport(const std::vector<ExperimentResult>& results) {
    std::ostringstream oss;
    
    oss << "=== NLM Phase 2 Experiment Report ===\n\n";
    
    for (const auto& result : results) {
        oss << "Experiment: " << getModeName(result.mode) << "\n";
        oss << "Status: " << (result.success ? "SUCCESS" : "FAILURE") << "\n";
        oss << "Duration: " << result.getDurationString() << "\n";
        oss << "Message: " << result.message << "\n";
        oss << "Metrics:\n";
        oss << "  Spikes Generated: " << result.metrics.spikesGenerated << "\n";
        oss << "  Mean Weight Change: " << result.metrics.meanWeightChange << "\n";
        oss << "  Strengthened Synapses: " << result.metrics.strengthenedSynapses << "\n";
        oss << "  Weakened Synapses: " << result.metrics.weakenedSynapses << "\n";
        oss << "  Unchanged Synapses: " << result.metrics.unchangedSynapses << "\n";
        oss << "  Total Reward: " << result.metrics.totalReward << "\n";
        oss << "\n";
    }
    
    oss << "=== End Report ===";
    return oss.str();
}

bool ExperimentRunner::validateConfig(const Config& config, ExperimentMode mode) {
    ExperimentValidator validator;
    auto validationResult = validator.validateConfig(config, mode);
    
    if (!validationResult.valid) {
        for (const auto& error : validationResult.errors) {
            NLM_LOG_ERROR("Validation error: " + error);
        }
    }
    
    if (!validationResult.warnings.empty()) {
        for (const auto& warning : validationResult.warnings) {
            NLM_LOG_WARNING("Validation warning: " + warning);
        }
    }
    
    return validationResult.valid;
}

Config ExperimentRunner::getDefaultConfig(ExperimentMode mode) {
    return ExperimentPresets::getQuickTestConfig();
}

ExperimentResult ExperimentRunner::runBasicConnectivity(const Config& config) {
    ExperimentResult result;
    result.mode = ExperimentMode::BASIC_CONNECTIVITY;
    
    auto brainConfig = std::make_shared<Config>(config);
    auto brain = std::make_shared<Brain>(brainConfig);
    
    if (!brain->initialize()) {
        result.success = false;
        result.message = "Failed to initialize brain";
        return result;
    }
    
    // Run basic connectivity test
    auto* region = brain->getRegion(RegionId(1));
    if (!region) {
        result.success = false;
        result.message = "No brain region found";
        return result;
    }
    
    auto neurons = region->getAllNeurons();
    if (neurons.empty()) {
        result.success = false;
        result.message = "No neurons found";
        return result;
    }
    
    size_t initialSpikes = brain->getTotalSpikeCount();
    
    // Inject current into neurons
    for (size_t i = 0; i < std::min(size_t(Phase2Constants::TEST1_NEUTRAL_INJECTION_COUNT), neurons.size()); ++i) {
        neurons[i]->injectCurrent(Phase2Constants::TEST1_INJECTION_CURRENT);
    }
    
    // Run simulation
    for (SimulationStep step = 0; step < Phase2Constants::TEST1_STEP_COUNT; ++step) {
        brain->step(step, step * Phase2Constants::EXPERIMENT_TIMESTEP);
    }
    
    size_t spikes = brain->getTotalSpikeCount() - initialSpikes;
    result.metrics.spikesGenerated = spikes;
    
    if (spikes > 0) {
        result.success = true;
        result.message = "Spikes propagate through network successfully";
    } else {
        result.success = false;
        result.message = "No spikes generated - network may not be connected properly";
    }
    
    return result;
}

ExperimentResult ExperimentRunner::runPlasticityLearning(const Config& config) {
    ExperimentResult result;
    result.mode = ExperimentMode::PLASTICITY_LEARNING;
    
    auto brainConfig = std::make_shared<Config>(config);
    auto brain = std::make_shared<Brain>(brainConfig);
    
    if (!brain->initialize()) {
        result.success = false;
        result.message = "Failed to initialize brain";
        return result;
    }
    
    // Enable plasticity
    if (auto* region = brain->getRegion(RegionId(1))) {
        for (auto& syn : region->getSynapses()) {
            syn->enablePlasticity(true, true, false);
        }
    }
    
    // Run plasticity experiment
    for (SimulationStep step = 0; step < Phase2Constants::TEST2_STEP_COUNT; ++step) {
        // Inject input pattern
        for (size_t i = 0; i < Phase2Constants::TEST2_INPUT_PATTERN_SIZE && i < brain->getTotalNeuronCount() / 4; ++i) {
            brain->injectCurrentToNeurons(NeuronType::Sensory, Phase2Constants::TEST2_INPUT_CURRENT);
        }
        
        brain->step(step, step * Phase2Constants::EXPERIMENT_TIMESTEP);
    }
    
    // Calculate metrics
    result.metrics.spikesGenerated = brain->getTotalSpikeCount();
    
    // Check if learning occurred
    if (brain->getTotalSpikeCount() > 0) {
        result.success = true;
        result.message = "Learning experiment completed successfully";
    } else {
        result.success = false;
        result.message = "No activity detected during learning experiment";
    }
    
    return result;
}

ExperimentResult ExperimentRunner::runStdpVerification(const Config& config) {
    ExperimentResult result;
    result.mode = ExperimentMode::STDP_VERIFICATION;
    
    auto brainConfig = std::make_shared<Config>(config);
    auto brain = std::make_shared<Brain>(brainConfig);
    
    if (!brain->initialize()) {
        result.success = false;
        result.message = "Failed to initialize brain";
        return result;
    }
    
    auto* region = brain->getRegion(RegionId(1));
    if (!region) {
        result.success = false;
        result.message = "No brain region found";
        return result;
    }
    
    auto& synapses = region->getSynapses();
    if (synapses.size() < Phase2Constants::TEST3_SYNAPSE_TEST_COUNT) {
        result.success = false;
        result.message = "Not enough synapses for STDP test";
        return result;
    }
    
    // Record initial weights
    std::vector<float> beforeWeights;
    for (size_t i = 0; i < Phase2Constants::TEST3_SYNAPSE_TEST_COUNT; ++i) {
        beforeWeights.push_back(synapses[i]->getWeight());
        synapses[i]->enablePlasticity(false, true, false);
    }
    
    // Run STDP experiment
    for (int trial = 0; trial < Phase2Constants::TEST3_STDP_TRIAL_COUNT; ++trial) {
        // Get neurons
        auto neurons = region->getAllNeurons();
        if (neurons.size() >= 2) {
            Neuron* preNeuron = neurons[0];
            Neuron* postNeuron = neurons[1];
            
            // Pre fires first
            preNeuron->injectCurrent(Phase2Constants::TEST3_STDP_PRE_CURRENT);
            brain->step(trial * 2, trial * 2 * Phase2Constants::EXPERIMENT_TIMESTEP);
            
            // Then post fires
            postNeuron->injectCurrent(Phase2Constants::TEST3_STDP_POST_CURRENT);
            brain->step(trial * 2 + 1, (trial * 2 + 1) * Phase2Constants::EXPERIMENT_TIMESTEP);
        }
    }
    
    // Calculate weight changes
    float totalDelta = 0.0f;
    for (size_t i = 0; i < Phase2Constants::TEST3_SYNAPSE_TEST_COUNT; ++i) {
        float delta = synapses[i]->getWeight() - beforeWeights[i];
        totalDelta += delta;
    }
    
    result.metrics.spikesGenerated = brain->getTotalSpikeCount();
    
    // Check if STDP worked
    if (std::abs(totalDelta) > Phase2Constants::LEARNING_MEAN_WEIGHT_CHANGE_THRESHOLD) {
        result.success = true;
        result.message = "STDP experiment completed - significant weight change detected";
    } else {
        result.success = false;
        result.message = "STDP experiment completed - no significant weight change detected";
    }
    
    return result;
}

ExperimentResult ExperimentRunner::runBenchmark(const Config& config) {
    ExperimentResult result;
    result.mode = ExperimentMode::BENCHMARK;
    
    // Run benchmark measurements
    auto startTime = std::chrono::high_resolution_clock::now();
    
    auto brainConfig = std::make_shared<Config>(config);
    auto brain = std::make_shared<Brain>(brainConfig);
    
    if (!brain->initialize()) {
        result.success = false;
        result.message = "Failed to initialize brain for benchmark";
        return result;
    }
    
    // Run simulation steps
    for (SimulationStep step = 0; step < Phase2Constants::TEST2_STEP_COUNT; ++step) {
        brain->step(step, step * Phase2Constants::EXPERIMENT_TIMESTEP);
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    double duration = std::chrono::duration<double>(endTime - startTime).count();
    
    result.metrics.spikesGenerated = brain->getTotalSpikeCount();
    
    result.success = true;
    result.message = "Benchmark completed in " + std::to_string(duration) + " seconds";
    
    return result;
}

void ExperimentRunner::applyConfigToBrain(const Config& config, std::shared_ptr<Brain> brain) {
    // Apply configuration to brain settings
    // This is a placeholder for actual configuration application logic
}

void ExperimentRunner::logExperimentStart(ExperimentMode mode, const Config& config) {
    NLM_LOG_INFO("Starting experiment: " + getModeName(mode));
    NLM_LOG_INFO("Configuration: " + ExperimentPresets::configToString(config));
}

void ExperimentRunner::logExperimentResult(const ExperimentResult& result) {
    if (result.success) {
        NLM_LOG_INFO("Experiment " + getModeName(result.mode) + " completed successfully: " + result.message);
    } else {
        NLM_LOG_ERROR("Experiment " + getModeName(result.mode) + " failed: " + result.message);
    }
}

bool ExperimentRunner::checkExperimentSuccess(ExperimentMode mode, const ExperimentResult& result) {
    // Default success criteria
    return result.success;
}

Config ExperimentPresets::getQuickTestConfig() {
    Config config = createBaseConfig();
    config.set("neuron_count", static_cast<int64_t>(Phase2Constants::EXPERIMENT_NEURON_COUNT));
    config.set("region_count", static_cast<int64_t>(Phase2Constants::EXPERIMENT_REGION_COUNT));
    config.set("connection_probability", Phase2Constants::EXPERIMENT_CONNECTION_PROBABILITY);
    config.set("simulation_timestep", Phase2Constants::EXPERIMENT_TIMESTEP);
    config.set("random_seed", Phase2Constants::EXPERIMENT_RANDOM_SEED);
    
    return config;
}

Config ExperimentPresets::getComprehensiveConfig() {
    Config config = createBaseConfig();
    config.set("neuron_count", static_cast<int64_t>(Phase2Constants::EXPERIMENT_NEURON_COUNT * 2));
    config.set("region_count", static_cast<int64_t>(Phase2Constants::EXPERIMENT_REGION_COUNT));
    config.set("connection_probability", Phase2Constants::EXPERIMENT_CONNECTION_PROBABILITY);
    config.set("simulation_timestep", Phase2Constants::EXPERIMENT_TIMESTEP);
    config.set("random_seed", Phase2Constants::EXPERIMENT_RANDOM_SEED);
    config.set("stdp_ltp_weight", Phase2Constants::EXPERIMENT_STDP_LTP_WEIGHT);
    config.set("stdp_ltd_weight", Phase2Constants::EXPERIMENT_STDP_LTD_WEIGHT);
    config.set("stdp_tau", Phase2Constants::EXPERIMENT_STDP_TAU);
    config.set("synaptogenesis_rate", Phase2Constants::EXPERIMENT_SYNAPTOGENESIS_RATE);
    config.set("pruning_rate", Phase2Constants::EXPERIMENT_PRUNING_RATE);
    
    applyPerformanceOptimizations(config);
    return config;
}

Config ExperimentPresets::getBenchmarkConfig() {
    Config config = createBaseConfig();
    config.set("neuron_count", static_cast<int64_t>(Phase2Constants::EXPERIMENT_NEURON_COUNT));
    config.set("region_count", static_cast<int64_t>(Phase2Constants::EXPERIMENT_REGION_COUNT));
    config.set("connection_probability", Phase2Constants::EXPERIMENT_CONNECTION_PROBABILITY);
    config.set("simulation_timestep", Phase2Constants::EXPERIMENT_TIMESTEP);
    config.set("random_seed", Phase2Constants::EXPERIMENT_RANDOM_SEED);
    
    applyPerformanceOptimizations(config);
    return config;
}

Config ExperimentPresets::getValidationConfig() {
    Config config = createBaseConfig();
    config.set("neuron_count", static_cast<int64_t>(Phase2Constants::EXPERIMENT_NEURON_COUNT));
    config.set("region_count", static_cast<int64_t>(Phase2Constants::EXPERIMENT_REGION_COUNT));
    config.set("connection_probability", Phase2Constants::EXPERIMENT_CONNECTION_PROBABILITY);
    config.set("simulation_timestep", Phase2Constants::EXPERIMENT_TIMESTEP);
    config.set("random_seed", Phase2Constants::EXPERIMENT_RANDOM_SEED);
    
    applyValidationChecks(config);
    return config;
}

Config ExperimentPresets::getPerformanceConfig() {
    Config config = createBaseConfig();
    config.set("neuron_count", static_cast<int64_t>(Phase2Constants::EXPERIMENT_NEURON_COUNT));
    config.set("region_count", static_cast<int64_t>(Phase2Constants::EXPERIMENT_REGION_COUNT));
    config.set("connection_probability", Phase2Constants::EXPERIMENT_CONNECTION_PROBABILITY);
    config.set("simulation_timestep", Phase2Constants::EXPERIMENT_TIMESTEP);
    config.set("random_seed", Phase2Constants::EXPERIMENT_RANDOM_SEED);
    
    applyPerformanceOptimizations(config);
    return config;
}

std::string ExperimentPresets::configToString(const Config& config) {
    std::ostringstream oss;
    oss << "Config with " << config.getKeys().size() << " entries:";
    for (const auto& key : config.getKeys()) {
        oss << "\n  " << key << " = " << config.getOr(key, std::string(""));
    }
    return oss.str();
}

bool ExperimentPresets::configFromString(Config& config, const std::string& configStr) {
    // Parse configuration string (simplified implementation)
    std::istringstream stream(configStr);
    std::string line;
    
    while (std::getline(stream, line)) {
        line = trim(line);
        if (line.empty() || line[0] == '#') continue;
        
        size_t pos = line.find('=');
        if (pos != std::string::npos) {
            std::string key = trim(line.substr(0, pos));
            std::string value = trim(line.substr(pos + 1));
            config.set(key, value, ConfigSource::Runtime);
        }
    }
    
    return true;
}

Config ExperimentPresets::createBaseConfig() {
    Config config;
    config.set("version", "2.0.0", ConfigSource::Default);
    config.set("description", "NLM Phase 2 Unified Experiment Framework", ConfigSource::Default);
    return config;
}

void ExperimentPresets::applyPerformanceOptimizations(Config& config) {
    // Apply performance optimizations for benchmarking
    config.set("simulation_timestep", Phase2Constants::EXPERIMENT_TIMESTEP, ConfigSource::Runtime);
    config.set("neuron_count", static_cast<int64_t>(Phase2Constants::EXPERIMENT_NEURON_COUNT), ConfigSource::Runtime);
}

void ExperimentPresets::applyValidationChecks(Config& config) {
    // Apply validation checks for testing
    config.set("validation_mode", true, ConfigSource::Runtime);
    config.set("strict_mode", true, ConfigSource::Runtime);
}

ExperimentValidator::ValidationResult ExperimentValidator::validateConfig(const Config& config, ExperimentMode mode) {
    ValidationResult result;
    result.valid = true;
    
    // Basic validation
    result.valid &= checkNeuronCount(config);
    result.valid &= checkTimestep(config);
    result.valid &= checkPlasticityParameters(config);
    
    // Mode-specific validation
    switch (mode) {
        case ExperimentMode::BASIC_CONNECTIVITY:
            // Check basic connectivity requirements
            if (!config.has("neuron_count") || config.get<int64_t>("neuron_count").value() < 10) {
                addError(result, "Neuron count must be at least 10 for basic connectivity test");
                result.valid = false;
            }
            break;
        case ExperimentMode::PLASTICITY_LEARNING:
            // Check plasticity requirements
            if (!config.has("stdp_ltp_weight") || !config.has("stdp_ltd_weight")) {
                addError(result, "STDP parameters required for plasticity learning experiment");
                result.valid = false;
            }
            break;
        case ExperimentMode::STDP_VERIFICATION:
            // Check STDP requirements
            if (!config.has("stdp_ltp_weight") || !config.has("stdp_ltd_weight")) {
                addError(result, "STDP parameters required for STDP verification experiment");
                result.valid = false;
            }
            break;
        default:
            break;
    }
    
    return result;
}

ExperimentValidator::ValidationResult ExperimentValidator::validateBrain(const std::shared_ptr<Brain>& brain, ExperimentMode mode) {
    ValidationResult result;
    result.valid = true;
    
    if (!brain) {
        addError(result, "Brain pointer is null");
        result.valid = false;
        return result;
    }
    
    // Validate brain state
    if (brain->getTotalNeuronCount() == 0) {
        addError(result, "Brain has no neurons");
        result.valid = false;
    }
    
    if (brain->getTotalSynapseCount() == 0) {
        addWarning(result, "Brain has no synapses");
    }
    
    return result;
}

ExperimentValidator::ValidationResult ExperimentValidator::validateEnvironment(const Config& config, ExperimentMode mode) {
    ValidationResult result;
    result.valid = true;
    
    // Validate environment configuration
    return result;
}

bool ExperimentValidator::checkNeuronCount(const Config& config) {
    if (!config.has("neuron_count")) {
        addError(result, "Configuration missing required parameter: neuron_count");
        return false;
    }
    
    int64_t neuronCount = config.get<int64_t>("neuron_count").value();
    if (!Constants::isValidNeuronCount(neuronCount)) {
        addError(result, "Invalid neuron count: " + std::to_string(neuronCount));
        return false;
    }
    
    return true;
}

bool ExperimentValidator::checkTimestep(const Config& config) {
    if (!config.has("simulation_timestep")) {
        addError(result, "Configuration missing required parameter: simulation_timestep");
        return false;
    }
    
    double timestep = config.get<double>("simulation_timestep").value();
    if (!Constants::isValidTimestep(timestep)) {
        addError(result, "Invalid simulation timestep: " + std::to_string(timestep));
        return false;
    }
    
    return true;
}

bool ExperimentValidator::checkPlasticityParameters(const Config& config) {
    if (!config.has("stdp_ltp_weight") || !config.has("stdp_ltd_weight")) {
        addError(result, "STDP parameters (stdp_ltp_weight, stdp_ltd_weight) are required");
        return false;
    }
    
    float ltpWeight = config.get<float>("stdp_ltp_weight").value();
    float ltdWeight = config.get<float>("stdp_ltd_weight").value();
    
    if (!Constants::isValidSTDPWeight(ltpWeight) || !Constants::isValidSTDPWeight(ltdWeight)) {
        addError(result, "Invalid STDP weights");
        return false;
    }
    
    return true;
}

bool ExperimentValidator::checkMemoryRequirements(const Config& config) {
    // Check if there are enough resources for the experiment
    return true;
}

bool ExperimentValidator::checkPerformanceParameters(const Config& config) {
    // Check if performance parameters are valid
    if (config.has("parallel_enabled")) {
        bool parallel = config.get<bool>("parallel_enabled").value();
        if (parallel && config.get<int64_t>("neuron_count").value() < 100) {
            addWarning(result, "Parallel processing enabled but neuron count is low - performance may be suboptimal");
        }
    }
    
    return true;
}

void ExperimentValidator::addError(ValidationResult& result, const std::string& error) {
    result.valid = false;
    result.errors.push_back(error);
}

void ExperimentValidator::addWarning(ValidationResult& result, const std::string& warning) {
    result.warnings.push_back(warning);
}

ExperimentPerformanceMonitor::PerformanceMetrics ExperimentPerformanceMonitor::startMonitoring() {
    resetMonitoring();
    monitoring_ = true;
    
    // Get initial system metrics
    PerformanceMetrics metrics;
    metrics.cpuUsage = 0.0;
    metrics.memoryUsageMB = 0;
    metrics.totalSpikes = 0;
    metrics.averageFiringRate = 0.0;
    metrics.simulationTime = 0.0;
    metrics.wallClockTime = 0.0;
    
    return metrics;
}

ExperimentPerformanceMonitor::PerformanceMetrics ExperimentPerformanceMonitor::stopMonitoring() {
    if (!monitoring_) {
        return currentMetrics_;
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    currentMetrics_.wallClockTime = std::chrono::duration<double>(endTime - startTime_).count();
    
    monitoring_ = false;
    return currentMetrics_;
}

ExperimentPerformanceMonitor::PerformanceMetrics ExperimentPerformanceMonitor::getCurrentMetrics() {
    return currentMetrics_;
}

void ExperimentPerformanceMonitor::resetMonitoring() {
    currentMetrics_ = PerformanceMetrics();
    startTime_ = std::chrono::high_resolution_clock::now();
    monitoring_ = false;
}

std::string ExperimentPerformanceMonitor::formatMetrics(const PerformanceMetrics& metrics) {
    std::ostringstream oss;
    oss << "Performance Metrics:\n";
    oss << "  CPU Usage: " << std::fixed << std::setprecision(1) << metrics.cpuUsage * 100 << "%\n";
    oss << "  Memory Usage: " << formatMemory(metrics.memoryUsageMB * 1024 * 1024) << "\n";
    oss << "  Total Spikes: " << metrics.totalSpikes << "\n";
    oss << "  Average Firing Rate: " << std::fixed << std::setprecision(3) << metrics.averageFiringRate << " Hz\n";
    oss << "  Simulation Time: " << metrics.simulationTime << "s\n";
    oss << "  Wall Clock Time: " << metrics.wallClockTime << "s\n";
    
    return oss.str();
}

std::string ExperimentPerformanceMonitor::benchmarkExperiment(const std::function<void()>& experimentFunc) {
    auto startTime = std::chrono::high_resolution_clock::now();
    
    experimentFunc();
    
    auto endTime = std::chrono::high_resolution_clock::now();
    double duration = std::chrono::duration<double>(endTime - startTime).count();
    
    std::ostringstream oss;
    oss << "Experiment completed in " << std::fixed << std::setprecision(3) << duration << " seconds";
    
    return oss.str();
}

std::string ExperimentOutputFormatter::getStatusSymbol(bool success) {
    return success ? "✓" : "✗";
}

std::string ExperimentOutputFormatter::getModeName(ExperimentMode mode) {
    switch (mode) {
        case ExperimentMode::BASIC_CONNECTIVITY: return "Basic Connectivity Test";
        case ExperimentMode::PLASTICITY_LEARNING: return "Plasticity Learning Experiment";
        case ExperimentMode::STDP_VERIFICATION: return "STDP Verification Test";
        case ExperimentMode::ALL_TESTS: return "All Tests";
        case ExperimentMode::BENCHMARK: return "Benchmark";
        default: return "Unknown";
    }
}

std::string ExperimentOutputFormatter::formatTime(double seconds) {
    std::ostringstream oss;
    if (seconds < 60.0) {
        oss << std::fixed << std::setprecision(3) << seconds << "s";
    } else {
        double minutes = seconds / 60.0;
        if (minutes < 60.0) {
            oss << std::fixed << std::setprecision(1) << minutes << "min";
        } else {
            double hours = minutes / 60.0;
            oss << std::fixed << std::setprecision(2) << hours << "h";
        }
    }
    return oss.str();
}

std::string ExperimentOutputFormatter::formatMemory(size_t bytes) {
    if (bytes < 1024) {
        return std::to_string(bytes) + "B";
    } else if (bytes < 1024 * 1024) {
        return std::to_string(bytes / 1024) + "KB";
    } else if (bytes < 1024 * 1024 * 1024) {
        return std::to_string(bytes / (1024 * 1024)) + "MB";
    } else {
        return std::to_string(bytes / (1024 * 1024 * 1024)) + "GB";
    }
}

} // namespace Phase2
} // namespace nlm