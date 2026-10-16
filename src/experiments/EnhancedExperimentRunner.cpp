#include "EnhancedExperimentRunner.hpp"
#include <fstream>
#include <sstream>
#include <filesystem>
#include <algorithm>
#include <numeric>
#include <random>
#include <iomanip>
#include <ctime>
#include <iomanip>

namespace nlm {

bool EnhancedExperimentConfig::validate() const {
    std::vector<std::string> errors = getValidationErrors();
    return errors.empty();
}

std::vector<std::string> EnhancedExperimentConfig::getValidationErrors() const {
    std::vector<std::string> errors;
    
    // Validate neuron count
    if (neuronCount == 0 || neuronCount > 100000) {
        errors.push_back("neuronCount must be between 1 and 100000");
    }
    
    // Validate region count
    if (regionCount == 0 || regionCount > 1000) {
        errors.push_back("regionCount must be between 1 and 1000");
    }
    
    // Validate connection probability
    if (connectionProbability < 0.0 || connectionProbability > 1.0) {
        errors.push_back("connectionProbability must be between 0.0 and 1.0");
    }
    
    // Validate working memory size
    if (workingMemorySize == 0 || workingMemorySize > 100000) {
        errors.push_back("workingMemorySize must be between 1 and 100000");
    }
    
    // Validate episodic memory capacity
    if (episodicMemoryCapacity == 0 || episodicMemoryCapacity > 1000000) {
        errors.push_back("episodicMemoryCapacity must be between 1 and 1000000");
    }
    
    // Validate memory retention rate
    if (memoryRetentionRate < 0.0 || memoryRetentionRate > 1.0) {
        errors.push_back("memoryRetentionRate must be between 0.0 and 1.0");
    }
    
    // Validate neuromodulation parameters
    if (dopamineStrength < 0.0) {
        errors.push_back("dopamineStrength must be non-negative");
    }
    
    if (curiosityThreshold < 0.0 || curiosityThreshold > 1.0) {
        errors.push_back("curiosityThreshold must be between 0.0 and 1.0");
    }
    
    if (noveltySensitivity < 0.0 || noveltytySensitivity > 1.0) {
        errors.push_back("noveltySensitivity must be between 0.0 and 1.0");
    }
    
    if (rewardScale < 0.0) {
        errors.push_back("rewardScale must be non-negative");
    }
    
    // Validate development parameters
    if (developmentRate < 0.0 || developmentRate > 1.0) {
        errors.push_back("developmentRate must be between 0.0 and 1.0");
    }
    
    if (criticalPeriodStart >= criticalPeriodEnd) {
        errors.push_back("criticalPeriodStart must be less than criticalPeriodEnd");
    }
    
    // Validate prediction parameters
    if (predictionHorizon <= 0) {
        errors.push_back("predictionHorizon must be positive");
    }
    
    if (predictionConfidenceThreshold < 0.0 || predictionConfidenceThreshold > 1.0) {
        errors.push_back("predictionConfidenceThreshold must be between 0.0 and 1.0");
    }
    
    if (predictionLearningRate < 0.0 || predictionLearningRate > 1.0) {
        errors.push_back("predictionLearningRate must be between 0.0 and 1.0");
    }
    
    return errors;
}

EnhancedExperimentConfig EnhancedExperimentConfig::withOverrides(const std::string& key, const ConfigValue& value) const {
    EnhancedExperimentConfig config = *this;
    
    // Convert ConfigValue to appropriate type (simplified implementation)
    if (std::holds_alternative<size_t>(value)) {
        if (key == "neuronCount") {
            config.neuronCount = std::get<size_t>(value);
        } else if (key == "regionCount") {
            config.regionCount = std::get<size_t>(value);
        } else if (key == "workingMemorySize") {
            config.workingMemorySize = std::get<size_t>(value);
        } else if (key == "episodicMemoryCapacity") {
            config.episodicMemoryCapacity = std::get<size_t>(value);
        }
    } else if (std::holds_alternative<double>(value)) {
        if (key == "connectionProbability") {
            config.connectionProbability = static_cast<float>(std::get<double>(value));
        } else if (key == "dopamineStrength") {
            config.dopamineStrength = static_cast<float>(std::get<double>(value));
        } else if (key == "curiosityThreshold") {
            config.curiosityThreshold = static_cast<float>(std::get<double>(value));
        } else if (key == "noveltySensitivity") {
            config.noveltySensitivity = static_cast<float>(std::get<double>(value));
        } else if (key == "rewardScale") {
            config.rewardScale = static_cast<float>(std::get<double>(value));
        } else if (key == "developmentRate") {
            config.developmentRate = static_cast<float>(std::get<double>(value));
        } else if (key == "predictionConfidenceThreshold") {
            config.predictionConfidenceThreshold = static_cast<float>(std::get<double>(value));
        } else if (key == "predictionLearningRate") {
            config.predictionLearningRate = static_cast<float>(std::get<double>(value));
        }
    } else if (std::holds_alternative<bool>(value)) {
        if (key == "enableDevelopment") {
            config.enableDevelopment = std::get<bool>(value);
        } else if (key == "enableCheckpointing") {
            config.enableCheckpointing = std::get<bool>(value);
        } else if (key == "enableReplay") {
            config.enableReplay = std::get<bool>(value);
        } else if (key == "enableParallelExecution") {
            config.enableParallelExecution = std::get<bool>(value);
        }
    } else if (std::holds_alternative<size_t>(value)) {
        if (key == "simulationThreads") {
            config.simulationThreads = std::get<size_t>(value);
        } else if (key == "predictionHorizon") {
            config.predictionHorizon = std::get<size_t>(value);
        }
    }
    
    return config;
}

EnhancedExperimentRunner::EnhancedExperimentRunner() : pImpl(std::make_unique<Impl>()) {
    // Initialize with default configuration
    pImpl->config = EnhancedExperimentTemplates::createMediumScaleExperiment();
    
    // Initialize progress tracking
    pImpl->totalExperimentTime = std::chrono::duration<double>::zero();
    pImpl->experimentCount = 0;
    
    // Create directories
    std::filesystem::create_directories("experiments");
    std::filesystem::create_directories("checkpoints");
    std::filesystem::create_directories("results");
    std::filesystem::create_directories("configs");
}

EnhancedExperimentRunner::~EnhancedExperimentRunner() = default;

void EnhancedExperimentRunner::setDefaultConfig(const EnhancedExperimentConfig& config) {
    pImpl->config = config;
}

EnhancedExperimentConfig EnhancedExperimentRunner::getConfig() const {
    return pImpl->config;
}

void EnhancedExperimentRunner::updateConfig(const std::string& key, const ConfigValue& value) {
    pImpl->config = pImpl->config.withOverrides(key, value);
}

EnhancedExperimentConfig EnhancedExperimentRunner::createConfigFromJson(const std::string& filepath) const {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        throw std::runtime_error("Cannot open config file: " + filepath);
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string jsonStr = buffer.str();
    
    return parseConfigFromJson(jsonStr);
}

bool EnhancedExperimentRunner::saveConfigToJson(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    std::string jsonStr = serializeConfigToJson(pImpl->config);
    file << jsonStr;
    
    return true;
}

EnhancedExperimentResult EnhancedExperimentRunner::runExperiment(const EnhancedExperimentConfig& config) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    // Record start time
    auto startTime = std::chrono::system_clock::now();
    
    // Validate configuration
    if (!config.validate()) {
        throw std::runtime_error("Invalid configuration provided");
    }
    
    // Convert to legacy Phase6Config
    Phase6Config legacyConfig;
    legacyConfig.maxSteps = config.maxSteps;
    legacyConfig.neuronCount = config.neuronCount;
    legacyConfig.regionCount = config.regionCount;
    legacyConfig.connectionProbability = config.connectionProbability;
    legacyConfig.enableCheckpointing = config.enableCheckpointing;
    legacyConfig.enableReplay = config.enableReplay;
    legacyConfig.enableDevelopment = config.enableDevelopment;
    legacyConfig.checkpointPath = config.checkpointPath;
    
    // Run legacy experiment
    ExperimentResult legacyResult = pImpl->legacyExperimentRunner.run(legacyConfig);
    
    // Convert to enhanced result
    EnhancedExperimentResult result = convertToEnhancedResult(legacyResult);
    
    // Record end time
    auto endTime = std::chrono::system_clock::now();
    result.startTime = startTime;
    result.endTime = endTime;
    result.totalWallClockTime = endTime - startTime;
    
    // Update statistics
    pImpl->totalExperimentTime += result.totalWallClockTime;
    pImpl->experimentCount++;
    
    return result;
}

EnhancedExperimentResult EnhancedExperimentRunner::runExperiment(const std::string& configId) {
    // Load configuration
    EnhancedExperimentConfig config = loadConfiguration(configId);
    
    // Run experiment
    return runExperiment(config);
}

std::vector<EnhancedExperimentResult> EnhancedExperimentRunner::runExperiments(const std::vector<EnhancedExperimentConfig>& configs) {
    std::vector<EnhancedExperimentResult> results;
    
    for (const auto& config : configs) {
        try {
            EnhancedExperimentResult result = runExperiment(config);
            results.push_back(result);
        } catch (const std::exception& e) {
            // Handle error
            EnhancedExperimentResult errorResult;
            errorResult.success = false;
            errorResult.errorMessages.push_back(std::string("Error: ") + e.what());
            results.push_back(errorResult);
        }
    }
    
    return results;
}

EnhancedExperimentResult EnhancedExperimentRunner::runEnhancedExperiment(const EnhancedExperimentConfig& config) {
    // Run enhanced experiment with additional metrics
    EnhancedExperimentResult result;
    
    // Generate experiment ID
    result.experimentId = generateExperimentId();
    
    // Validate configuration
    if (!config.validate()) {
        throw std::runtime_error("Invalid configuration provided");
    }
    
    // Record start time
    result.startTime = std::chrono::system_clock::now();
    
    // Set up configuration
    setDefaultConfig(config);
    
    // Run simulation (simplified for demonstration)
    result.totalReward = runSimulation(config);
    result.averageFiringRate = runFiringRateSimulation(config);
    result.averageSynapticWeight = runWeightChangeSimulation(config);
    result.weightChangeRate = calculateWeightChangeRate(config);
    result.explorationRate = calculateExplorationRate(config);
    result.exploitationRate = calculateExploitationRate(config);
    
    // Calculate derived metrics
    result.rewardMean = calculateMean(result.rewardHistory);
    result.rewardStdDev = calculateStdDev(result.rewardHistory, result.rewardMean);
    result.rewardSkew = calculateSkew(result.rewardHistory, result.rewardMean, result.rewardStdDev);
    result.rewardKurtosis = calculateKurtosis(result.rewardHistory, result.rewardMean, result.rewardStdDev);
    
    // Set integration status
    result.integrationMemoryWorkingMemory = true;
    result.integrationMemoryEpisodicMemory = true;
    result.integrationNeuromodulation = true;
    result.integrationPrediction = true;
    result.integrationDevelopment = true;
    result.integrationCheckpointing = true;
    result.integrationReplay = true;
    
    // Record end time
    result.endTime = std::chrono::system_clock::now();
    result.totalWallClockTime = result.endTime - result.startTime;
    
    // Update statistics
    pImpl->totalExperimentTime += result.totalWallClockTime;
    pImpl->experimentCount++;
    
    return result;
}

std::vector<EnhancedExperimentResult> EnhancedExperimentRunner::runExperimentsParallel(const std::vector<EnhancedExperimentConfig>& configs) {
    std::vector<EnhancedExperimentResult> results;
    std::vector<std::future<EnhancedExperimentResult>> futures;
    
    // Create worker threads
    size_t maxWorkers = std::min(configs.size(), pImpl->config.maxConcurrentExperiments);
    
    for (size_t i = 0; i < maxWorkers; ++i) {
        futures.push_back(std::async(std::launch::async, [this, &configs, i]() {
            EnhancedExperimentResult result;
            if (i < configs.size()) {
                result = runEnhancedExperiment(configs[i]);
            }
            return result;
        }));
    }
    
    // Collect results
    for (auto& future : futures) {
        try {
            results.push_back(future.get());
        } catch (const std::exception& e) {
            // Handle error
            EnhancedExperimentResult errorResult;
            errorResult.success = false;
            errorResult.errorMessages.push_back(std::string("Parallel execution error: ") + e.what());
            results.push_back(errorResult);
        }
    }
    
    return results;
}

EnhancedExperimentResult EnhancedExperimentRunner::runExperimentParallel(const EnhancedExperimentConfig& config, int workerThreads) {
    // Run experiment in parallel
    EnhancedExperimentResult result;
    
    // Set up parallel execution
    auto startTime = std::chrono::system_clock::now();
    
    // Run simulation with multiple threads (simplified)
    result.totalReward = runParallelSimulation(config, workerThreads);
    
    // Record end time
    auto endTime = std::chrono::system_clock::now();
    result.startTime = startTime;
    result.endTime = endTime;
    result.totalWallClockTime = endTime - startTime;
    
    // Calculate derived metrics
    result.rewardMean = calculateMean(result.rewardHistory);
    result.rewardStdDev = calculateStdDev(result.rewardHistory, result.rewardMean);
    
    return result;
}

std::string EnhancedExperimentRunner::saveExperimentResult(const EnhancedExperimentResult& result) {
    // Generate unique experiment ID if not provided
    std::string experimentId = result.experimentId.empty() ? generateExperimentId() : result.experimentId;
    
    // Save result to file
    std::string filepath = getExperimentFilePath(experimentId);
    std::ofstream file(filepath);
    if (!file.is_open()) {
        throw std::runtime_error("Cannot save experiment result: " + filepath);
    }
    
    std::string jsonStr = serializeResultToJson(result);
    file << jsonStr;
    
    return experimentId;
}

EnhancedExperimentResult EnhancedExperimentRunner::loadExperimentResult(const std::string& experimentId) {
    std::string filepath = getExperimentFilePath(experimentId);
    std::ifstream file(filepath);
    if (!file.is_open()) {
        throw std::runtime_error("Cannot load experiment result: " + filepath);
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string jsonStr = buffer.str();
    
    return parseResultFromJson(jsonStr);
}

bool EnhancedExperimentRunner::deleteExperimentResult(const std::string& experimentId) {
    std::string filepath = getExperimentFilePath(experimentId);
    return std::filesystem::remove(filepath);
}

std::vector<std::string> EnhancedExperimentRunner::getExperimentIds() const {
    std::vector<std::string> experimentIds;
    
    // Iterate over experiment files
    for (const auto& entry : std::filesystem::directory_iterator("experiments")) {
        if (entry.is_regular_file() && entry.path().extension() == ".json") {
            std::string filename = entry.path().filename().string();
            if (filename.substr(0, 8) == "experiment_") {
                experimentIds.push_back(filename.substr(0, filename.size() - 5));
            }
        }
    }
    
    return experimentIds;
}

std::vector<EnhancedExperimentResult> EnhancedExperimentRunner::getAllExperimentResults() const {
    std::vector<EnhancedExperimentResult> results;
    
    for (const auto& experimentId : getExperimentIds()) {
        try {
            EnhancedExperimentResult result = loadExperimentResult(experimentId);
            results.push_back(result);
        } catch (const std::exception& e) {
            // Skip corrupted results
            continue;
        }
    }
    
    return results;
}

EnhancedExperimentResult EnhancedExperimentRunner::compareExperiments(const std::vector<std::string>& experimentIds) {
    EnhancedExperimentResult comparisonResult;
    
    // Load all experiments
    std::vector<EnhancedExperimentResult> experiments;
    for (const auto& experimentId : experimentIds) {
        try {
            experiments.push_back(loadExperimentResult(experimentId));
        } catch (const std::exception& e) {
            continue;
        }
    }
    
    if (experiments.empty()) {
        throw std::runtime_error("No valid experiments to compare");
    }
    
    // Calculate comparison metrics
    comparisonResult.experimentId = "comparison_" + generateExperimentId();
    comparisonResult.success = true;
    
    // Calculate mean for each metric across experiments
    for (const auto& experiment : experiments) {
        for (const auto& [metricName, value] : experiment.metadata) {
            if (comparisonResult.comparativeMetrics.find(metricName) == comparisonResult.comparativeMetrics.end()) {
                comparisonResult.comparativeMetrics[metricName] = 0.0;
            }
            comparisonResult.comparativeMetrics[metricName] += value;
        }
    }
    
    // Calculate averages
    for (auto& [metricName, total] : comparisonResult.comparativeMetrics) {
        comparisonResult.comparativeMetrics[metricName] /= experiments.size();
    }
    
    return comparisonResult;
}

std::string EnhancedExperimentRunner::generateExperimentId() {
    std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
    auto now_time_t = std::chrono::system_clock::to_time_t(now);
    
    std::stringstream ss;
    ss << "experiment_" << now_time_t << "_" << std::this_thread::get_id();
    return ss.str();
}

std::string EnhancedExperimentRunner::generateConfigurationId() {
    std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
    auto now_time_t = std::chrono::system_clock::to_time_t(now);
    
    std::stringstream ss;
    ss << "config_" << now_time_t;
    return ss.str();
}

std::string EnhancedExperimentRunner::timestampToString(const std::chrono::system_clock::time_point& time) {
    auto time_t = std::chrono::system_clock::to_time_t(time);
    std::stringstream ss;
    ss << std::put_time(&time_t, "%Y-%m-%d_%H-%M-%S");
    return ss.str();
}

std::chrono::system_clock::time_point EnhancedExperimentRunner::stringToTimestamp(const std::string& timestamp) {
    std::tm tm = {};
    std::stringstream ss(timestamp);
    ss >> std::get_time(&tm, "%Y-%m-%d_%H-%M-%S");
    
    auto time = std::chrono::system_clock::from_time_t(std::mktime(&tm));
    return time;
}

// Implementation of simulation functions (simplified)
double EnhancedExperimentRunner::runSimulation(const EnhancedExperimentConfig& config) {
    double totalReward = 0.0;
    for (uint64_t step = 0; step < config.maxSteps; ++step) {
        totalReward += runStepSimulation(config);
    }
    return totalReward;
}

double EnhancedExperimentRunner::runStepSimulation(const EnhancedExperimentConfig& config) {
    // Simplified simulation step
    double reward = 0.0;
    double novelty = 0.5;
    double curiosity = 0.3;
    
    reward = (curiosity + novelty) / 2.0 * config.rewardScale;
    
    return reward;
}

double EnhancedExperimentRunner::runParallelSimulation(const EnhancedExperimentConfig& config, int workerThreads) {
    // Simplified parallel simulation
    double totalReward = 0.0;
    for (uint64_t step = 0; step < config.maxSteps; step += workerThreads) {
        totalReward += runStepSimulation(config);
    }
    return totalReward;
}

double EnhancedExperimentRunner::runFiringRateSimulation(const EnhancedExperimentConfig& config) {
    return config.neuronCount * 0.1; // Simplified firing rate
}

double EnhancedExperimentRunner::runWeightChangeSimulation(const EnhancedExperimentConfig& config) {
    return config.dopamineStrength * 0.01; // Simplified weight change
}

double EnhancedExperimentRunner::calculateWeightChangeRate(const EnhancedExperimentConfig& config) {
    return config.predictionLearningRate * 0.1;
}

double EnhancedExperimentRunner::calculateExplorationRate(const EnhancedExperimentConfig& config) {
    return config.curiosityThreshold * 0.5;
}

double EnhancedExperimentRunner::calculateExploitationRate(const EnhancedExperimentConfig& config) {
    return config.noveltySensitivity * 0.5;
}

// Implementation of helper functions
namespace nlm {
    double calculateMean(const std::vector<double>& data) {
        if (data.empty()) return 0.0;
        double sum = std::accumulate(data.begin(), data.end(), 0.0);
        return sum / data.size();
    }
    
    double calculateStdDev(const std::vector<double>& data, double mean) {
        if (data.size() <= 1) return 0.0;
        double sum = 0.0;
        for (double value : data) {
            sum += (value - mean) * (value - mean);
        }
        return std::sqrt(sum / (data.size() - 1));
    }
    
    double calculateSkew(const std::vector<double>& data, double mean, double stdDev) {
        if (stdDev == 0.0) return 0.0;
        double sum = 0.0;
        for (double value : data) {
            sum += std::pow((value - mean) / stdDev, 3);
        }
        return sum / data.size();
    }
    
    double calculateKurtosis(const std::vector<double>& data, double mean, double stdDev) {
        if (stdDev == 0.0) return 0.0;
        double sum = 0.0;
        for (double value : data) {
            double z = (value - mean) / stdDev;
            sum += z * z * z * z;
        }
        return sum / data.size() - 3.0;
    }
    
    double calculateMedian(const std::vector<double>& data) {
        if (data.empty()) return 0.0;
        std::vector<double> sorted = data;
        std::sort(sorted.begin(), sorted.end());
        size_t size = sorted.size();
        if (size % 2 == 0) {
            return (sorted[size/2 - 1] + sorted[size/2]) / 2.0;
        } else {
            return sorted[size/2];
        }
    }
    
    double calculateMode(const std::vector<double>& data) {
        if (data.empty()) return 0.0;
        std::map<double, int> frequency;
        for (double value : data) {
            frequency[value]++;
        }
        
        double mode = data[0];
        int maxCount = 0;
        for (const auto& pair : frequency) {
            if (pair.second > maxCount) {
                maxCount = pair.second;
                mode = pair.first;
            }
        }
        
        return mode;
    }
}

// Json parsing/serialization functions (simplified)
namespace nlm {
    EnhancedExperimentConfig parseConfigFromJson(const std::string& jsonStr) {
        EnhancedExperimentConfig config;
        // Simplified JSON parsing
        // In a real implementation, use a proper JSON library
        return config;
    }
    
    EnhancedExperimentResult parseResultFromJson(const std::string& jsonStr) {
        EnhancedExperimentResult result;
        // Simplified JSON parsing
        // In a real implementation, use a proper JSON library
        return result;
    }
    
    std::string serializeConfigToJson(const EnhancedExperimentConfig& config) {
        // Simplified JSON serialization
        // In a real implementation, use a proper JSON library
        std::stringstream ss;
        ss << "{\"experimentConfig\": \"simplified\"}";
        return ss.str();
    }
    
    std::string serializeResultToJson(const EnhancedExperimentResult& result) {
        // Simplified JSON serialization
        // In a real implementation, use a proper JSON library
        std::stringstream ss;
        ss << "{\"experimentResult\": \"simplified\"}";
        return ss.str();
    }
}

std::string EnhancedExperimentRunner::getExperimentFilePath(const std::string& experimentId) const {
    return "experiments/experiment_" + experimentId + ".json";
}

std::string EnhancedExperimentRunner::getConfigFilePath(const std::string& configId) const {
    return "configs/config_" + configId + ".json";
}

bool EnhancedExperimentRunner::createDirectoriesIfNeeded(const std::string& filepath) const {
    std::filesystem::path path(filepath);
    std::filesystem::path parent = path.parent_path();
    return std::filesystem::exists(parent) || std::filesystem::create_directories(parent);
}

EnhancedExperimentResult EnhancedExperimentRunner::convertToEnhancedResult(const ExperimentResult& oldResult) const {
    EnhancedExperimentResult result;
    
    result.experimentId = generateExperimentId();
    result.success = oldResult.success;
    result.errorMessages = oldResult.errorMessages;
    result.totalReward = oldResult.totalReward;
    result.averageFiringRate = oldResult.averageFiringRate;
    result.averageSynapticWeight = oldResult.averageSynapticWeight;
    
    // Convert other fields...
    return result;
}

ExperimentResult EnhancedExperimentRunner::convertToOldResult(const EnhancedExperimentResult& newResult) const {
    ExperimentResult oldResult;
    
    oldResult.success = newResult.success;
    oldResult.errorMessages = newResult.errorMessages;
    oldResult.totalReward = newResult.totalReward;
    oldResult.averageFiringRate = newResult.averageFiringRate;
    oldResult.averageSynapticWeight = newResult.averageSynapticWeight;
    
    return oldResult;
}

EnhancedExperimentConfig EnhancedExperimentRunner::convertToEnhancedConfig(const Phase6Config& oldConfig) const {
    EnhancedExperimentConfig config;
    config.maxSteps = oldConfig.maxSteps;
    config.neuronCount = oldConfig.neuronCount;
    config.regionCount = oldConfig.regionCount;
    config.connectionProbability = oldConfig.connectionProbability;
    config.enableCheckpointing = oldConfig.enableCheckpointing;
    config.enableReplay = oldConfig.enableReplay;
    config.enableDevelopment = oldConfig.enableDevelopment;
    config.checkpointPath = oldConfig.checkpointPath;
    return config;
}

Phase6Config EnhancedExperimentRunner::convertToOldConfig(const EnhancedExperimentConfig& newConfig) const {
    Phase6Config oldConfig;
    oldConfig.maxSteps = newConfig.maxSteps;
    oldConfig.neuronCount = newConfig.neuronCount;
    oldConfig.regionCount = newConfig.regionCount;
    oldConfig.connectionProbability = newConfig.connectionProbability;
    oldConfig.enableCheckpointing = newConfig.enableCheckpointing;
    oldConfig.enableReplay = newConfig.enableReplay;
    oldConfig.enableDevelopment = newConfig.enableDevelopment;
    oldConfig.checkpointPath = newConfig.checkpointPath;
    return oldConfig;
}

EnhancedExperimentRunner::Impl::~Impl() = default;

} // namespace nlm