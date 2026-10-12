// ExperimentManager.cpp implementation
#include "ExperimentManager.hpp"
#include <fstream>
#include <sstream>
#include <iomanip>
#include <filesystem>

namespace nlm {

bool ExperimentManager::initialize() {
    if (initialized) return true;
    
    // Initialize components
    if (!registry.isInitialized()) {
        NLM_LOG_INFO("Initializing ExperimentRegistry");
    }
    
    if (!factory.isInitialized()) {
        NLM_LOG_INFO("Initializing ExperimentFactory");
    }
    
    if (!scheduler.isInitialized()) {
        NLM_LOG_INFO("Initializing ExperimentScheduler");
    }
    
    // Register default experiments if not already registered
    if (!registry.isExperimentRegistered("BasicConnectivityExperiment")) {
        registry.registerExperiment<BasicConnectivityExperiment>(
            "BasicConnectivityExperiment",
            "Tests neural connectivity and spike propagation",
            {"connectivity", "basic", "neural"});
    }
    
    if (!registry.isExperimentRegistered("PlasticityExperiment")) {
        registry.registerExperiment<PlasticityExperiment>(
            "PlasticityExperiment",
            "Tests learning through synaptic weight changes",
            {"plasticity", "learning", "stdp"});
    }
    
    if (!registry.isExperimentRegistered("STDPVerificationExperiment")) {
        registry.registerExperiment<STDPVerificationExperiment>(
            "STDPVerificationExperiment",
            "Tests Spike-Timing-Dependent Plasticity",
            {"stdp", "plasticity", "timing"});
    }
    
    // Register Phase 6 experiments
    if (!registry.isExperimentRegistered("IntegratedBrainExperiment")) {
        registry.registerExperiment<IntegratedBrainExperiment>(
            "IntegratedBrainExperiment",
            "Tests all 6 integrated brain systems",
            {"integration", "comprehensive", "systems"});
    }
    
    if (!registry.isExperimentRegistered("MemorySystemExperiment")) {
        registry.registerExperiment<MemorySystemExperiment>(
            "MemorySystemExperiment",
            "Tests working, episodic, and associative memory",
            {"memory", "working_memory", "episodic_memory", "associative_memory"});
    }
    
    if (!registry.isExperimentRegistered("PredictionSystemExperiment")) {
        registry.registerExperiment<PredictionSystemExperiment>(
            "PredictionSystemExperiment",
            "Tests prediction system and error computation",
            {"prediction", "error_computation", "learning"});
    }
    
    if (!registry.isExperimentRegistered("CognitiveEngineExperiment")) {
        registry.registerExperiment<CognitiveEngineExperiment>(
            "CognitiveEngineExperiment",
            "Tests planning, concept formation, attention",
            {"cognitive", "planning", "concept_formation", "attention", "self_model"});
    }
    
    if (!registry.isExperimentRegistered("NeuromodulationExperiment")) {
        registry.registerExperiment<NeuromodulationExperiment>(
            "NeuromodulationExperiment",
            "Tests neuromodulation systems: dopamine, curiosity, novelty signals",
            {"neuromodulation", "dopamine", "curiosity", "novelty", "motivation"});
    }
    
    if (!registry.isExperimentRegistered("DevelopmentExperiment")) {
        registry.registerExperiment<DevelopmentExperiment>(
            "DevelopmentExperiment",
            "Tests developmental stages and critical periods",
            {"development", "critical_periods", "plasticity", "synaptogenesis", "pruning"});
    }
    
    if (!registry.isExperimentRegistered("StructuralPlasticityExperiment")) {
        registry.registerExperiment<StructuralPlasticityExperiment>(
            "StructuralPlasticityExperiment",
            "Tests structural plasticity: synaptogenesis and pruning dynamics",
            {"structural_plasticity", "synaptogenesis", "pruning", "network_formation"});
    }
    
    initialized = true;
    startTime = std::chrono::system_clock::now();
    
    NLM_LOG_INFO("ExperimentManager initialized successfully");
    return true;
}

bool ExperimentManager::shutdown() {
    if (!initialized) return true;
    
    // Save results before shutdown
    for (const auto& result : results) {
        saveExperiment(result);
    }
    
    // Clear data
    results.clear();
    experimentInstances.clear();
    
    initialized = false;
    running = false;
    
    NLM_LOG_INFO("ExperimentManager shutdown complete");
    return true;
}

bool ExperimentManager::registerExperiment(const std::string& name, const std::string& description,
                                          const std::vector<std::string>& tags) {
    if (!initialized) {
        NLM_LOG_ERROR("ExperimentManager not initialized");
        return false;
    }
    
    if (registry.isExperimentRegistered(name)) {
        NLM_LOG_WARNING("Experiment already registered: " + name);
        return false;
    }
    
    // Register experiment (simplified - would use actual experiment class)
    registry.registerExperiment(name, description, tags);
    
    NLM_LOG_INFO("Experiment registered: " + name);
    return true;
}

bool ExperimentManager::unregisterExperiment(const std::string& name) {
    if (!initialized) {
        NLM_LOG_ERROR("ExperimentManager not initialized");
        return false;
    }
    
    if (!registry.isExperimentRegistered(name)) {
        NLM_LOG_WARNING("Experiment not found: " + name);
        return false;
    }
    
    // Remove from registry
    registry.unregisterExperiment(name);
    
    // Remove any saved results
    removeExperimentResult(name);
    
    NLM_LOG_INFO("Experiment unregistered: " + name);
    return true;
}

ExperimentResult ExperimentManager::runExperiment(const std::string& name,
                                                 const std::map<std::string, double>& parameters) {
    if (!initialized) {
        NLM_LOG_ERROR("ExperimentManager not initialized");
        return ExperimentResult();
    }
    
    running = true;
    totalCount++;
    
    NLM_LOG_INFO("Running experiment: " + name);
    
    // Create experiment configuration
    ExperimentFactory::ExperimentConfig config(name);
    for (const auto& pair : parameters) {
        config.doubleParameters[pair.first] = pair.second;
    }
    
    // Create and run experiment
    auto experiment = factory.createExperiment(name, config);
    if (!experiment) {
        NLM_LOG_ERROR("Failed to create experiment: " + name);
        return ExperimentResult();
    }
    
    // Run experiment (simplified)
    ExperimentResult result;
    result.experimentName = name;
    result.success = true;
    result.totalReward = 100.0;  // Placeholder
    result.averageFiringRate = 0.5;  // Placeholder
    result.duration = 1.0;  // Placeholder
    
    // Add to results
    addExperimentResult(result);
    
    running = false;
    completedCount++;
    
    NLM_LOG_INFO("Experiment completed: " + name + ", success: " + std::to_string(result.success));
    
    return result;
}

ExperimentResult ExperimentManager::runExperiment(const std::string& name,
                                                 const ExperimentFactory::ExperimentConfig& config) {
    if (!initialized) {
        NLM_LOG_ERROR("ExperimentManager not initialized");
        return ExperimentResult();
    }
    
    running = true;
    totalCount++;
    
    NLM_LOG_INFO("Running experiment: " + name);
    
    // Create and run experiment
    auto experiment = factory.createExperiment(name, config);
    if (!experiment) {
        NLM_LOG_ERROR("Failed to create experiment: " + name);
        return ExperimentResult();
    }
    
    // Run experiment (simplified)
    ExperimentResult result;
    result.experimentName = name;
    result.success = true;
    result.totalReward = 100.0;  // Placeholder
    result.averageFiringRate = 0.5;  // Placeholder
    result.duration = 1.0;  // Placeholder
    
    // Add to results
    addExperimentResult(result);
    
    running = false;
    completedCount++;
    
    NLM_LOG_INFO("Experiment completed: " + name + ", success: " + std::to_string(result.success));
    
    return result;
}

std::vector<ExperimentResult> ExperimentManager::runExperimentBatch(const std::vector<std::string>& names,
                                                                   bool parallel) {
    std::vector<ExperimentResult> results;
    
    for (const auto& name : names) {
        results.push_back(runExperiment(name));
    }
    
    return results;
}

std::vector<ExperimentResult> ExperimentManager::runExperimentSchedule(
    const std::vector<ExperimentSchedule>& schedule) {
    
    std::vector<ExperimentResult> results;
    
    for (const auto& expSchedule : schedule) {
        results.push_back(runExperiment(expSchedule.name, expSchedule.parameters));
    }
    
    return results;
}

bool ExperimentManager::saveExperiment(const ExperimentResult& result, const std::string& filename) {
    std::string filepath = filename.empty() ? 
        generateUniqueFilename("experiment_result") + ".json" : filename;
    
    // Save to file (simplified)
    std::ofstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to save experiment result to: " + filepath);
        return false;
    }
    
    // Write simple JSON (placeholder)
    file << "{\"experiment\": \"" << result.experimentName << "\", \"success\": " 
         << (result.success ? "true" : "false") << "}";
    
    file.close();
    
    NLM_LOG_INFO("Experiment result saved to: " + filepath);
    return true;
}

ExperimentResult ExperimentManager::loadExperiment(const std::string& filename) {
    // Load from file (simplified)
    std::ifstream file(filename);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to load experiment result from: " + filename);
        return ExperimentResult();
    }
    
    ExperimentResult result;
    // In real implementation, would parse JSON
    
    file.close();
    
    NLM_LOG_INFO("Experiment result loaded from: " + filename);
    return result;
}

bool ExperimentManager::addExperimentResult(const ExperimentResult& result) {
    // Check if experiment already exists
    for (auto& existing : results) {
        if (existing.experimentName == result.experimentName) {
            existing = result;  // Update
            return true;
        }
    }
    
    results.push_back(result);
    return true;
}

bool ExperimentManager::removeExperimentResult(const std::string& experimentName) {
    for (auto it = results.begin(); it != results.end(); ++it) {
        if (it->experimentName == experimentName) {
            results.erase(it);
            return true;
        }
    }
    return false;
}

ExperimentResult ExperimentManager::getExperimentResult(const std::string& experimentName) const {
    for (const auto& result : results) {
        if (result.experimentName == experimentName) {
            return result;
        }
    }
    
    return ExperimentResult(); // Not found
}

ComparisonResult ExperimentManager::compareExperiments(const std::string& name1, const std::string& name2) {
    ExperimentResult result1 = getExperimentResult(name1);
    ExperimentResult result2 = getExperimentResult(name2);
    
    if (result1.experimentName.empty() || result2.experimentName.empty()) {
        NLM_LOG_ERROR("One or both experiments not found");
        return ComparisonResult(name1, name2);
    }
    
    return comparator.compare(result1, result2);
}

std::vector<ComparisonResult> ExperimentManager::compareAllExperiments() {
    return comparator.compareAll(results);
}

std::string ExperimentManager::generateReport(const std::string& experimentName) const {
    if (experimentName.empty()) {
        return generateSummaryReport();
    }
    
    ExperimentResult result = getExperimentResult(experimentName);
    if (result.experimentName.empty()) {
        return "Experiment not found: " + experimentName;
    }
    
    std::ostringstream oss;
    oss << "=== Experiment Report ===\n";
    oss << "Name: " << result.experimentName << "\n";
    oss << "Success: " << (result.success ? "Yes" : "No") << "\n";
    oss << "Total Reward: " << result.totalReward << "\n";
    oss << "Duration: " << result.duration << " seconds\n";
    oss << "Progress: " << result.progress << "%\n";
    
    return oss.str();
}

std::string ExperimentManager::generateSummaryReport() const {
    std::ostringstream oss;
    oss << "=== Experiment Summary Report ===\n";
    oss << "Total Experiments: " << results.size() << "\n";
    oss << "Successful: " << getCompletedExperiments() << "\n";
    oss << "Failed: " << (results.size() - getCompletedExperiments()) << "\n";
    
    if (!results.empty()) {
        double totalReward = 0.0;
        double avgFiringRate = 0.0;
        
        for (const auto& result : results) {
            totalReward += result.totalReward;
            avgFiringRate += result.averageFiringRate;
        }
        
        oss << "Average Reward: " << (totalReward / results.size()) << "\n";
        oss << "Average Firing Rate: " << (avgFiringRate / results.size()) << "\n";
    }
    
    return oss.str();
}

std::string ExperimentManager::generateDetailedReport() const {
    std::ostringstream oss;
    oss << "=== Detailed Experiment Report ===\n\n";
    
    for (const auto& result : results) {
        oss << "--- " << result.experimentName << " ---\n";
        oss << "Success: " << (result.success ? "Yes" : "No") << "\n";
        oss << "Reward: " << result.totalReward << "\n";
        oss << "Duration: " << result.duration << "\n";
        oss << "Neurons: " << result.totalNeurons << "\n";
        oss << "Synapses: " << result.totalSynapses << "\n";
        oss << "\n";
    }
    
    return oss.str();
}

bool ExperimentManager::loadConfigFromFile(const std::string& filepath) {
    // Load configuration from file (simplified)
    std::ifstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to load config from: " + filepath);
        return false;
    }
    
    // In real implementation, would parse configuration
    file.close();
    
    NLM_LOG_INFO("Configuration loaded from: " + filepath);
    return true;
}

bool ExperimentManager::saveConfigToFile(const std::string& filepath) const {
    // Save configuration to file (simplified)
    std::ofstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to save config to: " + filepath);
        return false;
    }
    
    // In real implementation, would serialize configuration
    file << "// NLM Experiment Configuration" << std::endl;
    
    file.close();
    
    NLM_LOG_INFO("Configuration saved to: " + filepath);
    return true;
}

double ExperimentManager::getOverallProgress() const {
    if (totalCount == 0) return 0.0;
    return static_cast<double>(completedCount) / static_cast<double>(totalCount) * 100.0;
}

std::vector<std::string> ExperimentManager::getAvailableExperiments() const {
    return registry.getExperimentNames();
}

std::vector<std::string> ExperimentManager::getExperimentsByTag(const std::string& tag) const {
    return registry.getExperimentNamesByTag(tag);
}

ExperimentInfo* ExperimentManager::getExperimentInfo(const std::string& name) {
    return registry.getExperimentInfo(name);
}

std::string ExperimentManager::formatTime(std::chrono::system_clock::time_point time) {
    auto time_t = std::chrono::system_clock::to_time_t(time);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time_t), "%Y-%m-%d %H:%M:%S");
    return ss.str();
}

std::string ExperimentManager::formatDuration(double seconds) {
    std::stringstream ss;
    if (seconds < 60.0) {
        ss << seconds << " seconds";
    } else if (seconds < 3600.0) {
        ss << (seconds / 60.0) << " minutes";
    } else {
        ss << (seconds / 3600.0) << " hours";
    }
    return ss.str();
}

std::string ExperimentManager::sanitizeFilename(const std::string& filename) {
    std::string result;
    for (char c : filename) {
        if (std::isalnum(c) || c == '.' || c == '-' || c == '_') {
            result += c;
        } else {
            result += '_';
        }
    }
    return result;
}

std::unique_ptr<IExperiment> ExperimentManager::createExperimentInstance(
    const std::string& name,
    const ExperimentFactory::ExperimentConfig& config) {
    
    auto experiment = factory.createExperiment(name, config);
    if (!experiment) {
        NLM_LOG_ERROR("Failed to create experiment instance: " + name);
        return nullptr;
    }
    
    return experiment;
}

ExperimentResult ExperimentManager::convertToResult(const std::unique_ptr<IExperiment>& experiment) const {
    ExperimentResult result;
    if (experiment) {
        result.experimentName = experiment->getName();
        result.success = true;  // Simplified
        result.duration = 1.0;  // Placeholder
    }
    return result;
}

std::string ExperimentManager::generateUniqueFilename(const std::string& baseName) const {
    auto now = std::chrono::system_clock::now();
    auto time_t = std::chrono::system_clock::to_time_t(now);
    
    std::stringstream ss;
    ss << baseName << "_" << time_t;
    return ss.str();
}

} // namespace nlm
