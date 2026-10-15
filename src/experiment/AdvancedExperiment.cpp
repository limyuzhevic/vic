#include "AdvancedExperiment.hpp"
#include "../core/Logger/Logger.hpp"
#include <random>
#include <algorithm>
#include <cmath>

namespace nlm {

struct AdvancedExperiment::Impl {
    // Experiment configuration
    Config experimentConfig;
    
    // Protocol management
    std::map<std::string, std::unique_ptr<CustomProtocol>> protocols;
    std::string currentProtocol;
    bool experimentRunning;
    bool experimentPaused;
    
    // Hyperparameter optimization
    std::map<std::string, std::vector<double>> searchSpaces;
    std::map<std::string, std::function<double(const Config&)>> objectiveFunctions;
    Config optimalParameters;
    double optimalObjectiveValue;
    
    // Custom metrics
    std::map<std::string, std::function<double(const ExperimentData&)>> customMetrics;
    
    // Monitoring
    bool monitoringEnabled;
    std::chrono::system_clock::time_point startTime;
    double totalDuration;
    
    // Statistics
    size_t experimentCount;
    size_t successfulExperiments;
    size_t protocolRuns;
    size_t optimizationRuns;
    
    // Events
    struct ExperimentCallbackEntry {
        std::string eventType;
        ExperimentCallback callback;
    };
    std::vector<ExperimentCallbackEntry> experimentCallbacks;
    
    Logger* logger;
};

AdvancedExperiment::AdvancedExperiment() : pImpl(std::make_unique<Impl>()) {
    pImpl->logger = Logger::getInstance();
    pImpl->experimentRunning = false;
    pImpl->experimentPaused = false;
    pImpl->optimalObjectiveValue = std::numeric_limits<double>::max();
    pImpl->monitoringEnabled = false;
    pImpl->totalDuration = 0.0;
    pImpl->experimentCount = 0;
    pImpl->successfulExperiments = 0;
    pImpl->protocolRuns = 0;
    pImpl->optimizationRuns = 0;
}

AdvancedExperiment::~AdvancedExperiment() {
    shutdown();
}

AdvancedExperiment::AdvancedExperiment(AdvancedExperiment&& other) noexcept : pImpl(std::move(other.pImpl)) {}

AdvancedExperiment& AdvancedExperiment::operator=(AdvancedExperiment&& other) noexcept {
    if (this != &other) {
        pImpl = std::move(other.pImpl);
    }
    return *this;
}

bool AdvancedExperiment::initialize(const Config& config) {
    pImpl->experimentConfig = config;
    
    // Initialize default protocols
    addProtocol(std::make_unique<ScalingBenchmarkProtocol>());
    addProtocol(std::make_unique<PhaseTrainingProtocol>());
    
    pImpl->logger->info("Advanced experiment framework initialized");
    return true;
}

void AdvancedExperiment::update(double timestep) {
    if (pImpl->experimentRunning && !pImpl->experimentPaused) {
        // Update current protocol if running
        if (pImpl->currentProtocol.empty()) {
            pImpl->experimentRunning = false;
            return;
        }
        
        auto it = pImpl->protocols.find(pImpl->currentProtocol);
        if (it != pImpl->protocols.end()) {
            it->second->update(timestep);
        }
    }
    
    // Update monitoring
    if (pImpl->monitoringEnabled) {
        auto now = std::chrono::system_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::seconds>(now - pImpl->startTime);
        pImpl->totalDuration += duration.count();
    }
}

void AdvancedExperiment::shutdown() {
    pImpl->logger->info("Advanced experiment framework shutdown");
    
    pImpl->experimentRunning = false;
    pImpl->experimentPaused = false;
    
    // Stop all protocols
    for (auto& pair : pImpl->protocols) {
        pair.second->stop();
    }
    
    pImpl->protocols.clear();
    pImpl->experimentCallbacks.clear();
}

bool AdvancedExperiment::addProtocol(std::unique_ptr<CustomProtocol> protocol) {
    if (!protocol) {
        pImpl->logger->error("Cannot add null protocol");
        return false;
    }
    
    // Check for duplicate
    auto it = pImpl->protocols.find(protocol->getName());
    if (it != pImpl->protocols.end()) {
        pImpl->logger->warning("Protocol already exists: " + protocol->getName());
        return false;
    }
    
    if (!protocol->initialize(pImpl->experimentConfig)) {
        pImpl->logger->error("Failed to initialize protocol: " + protocol->getName());
        return false;
    }
    
    pImpl->protocols[protocol->getName()] = std::move(protocol);
    pImpl->logger->info("Protocol added: " + protocol->getName());
    return true;
}

bool AdvancedExperiment::removeProtocol(const std::string& protocolName) {
    auto it = pImpl->protocols.find(protocolName);
    if (it != pImpl->protocols.end()) {
        if (it->second->isRunning()) {
            it->second->stop();
        }
        it->second->shutdown();
        pImpl->protocols.erase(it);
        pImpl->logger->info("Protocol removed: " + protocolName);
        return true;
    }
    
    pImpl->logger->warning("Protocol not found: " + protocolName);
    return false;
}

CustomProtocol* AdvancedExperiment::getProtocol(const std::string& protocolName) const {
    auto it = pImpl->protocols.find(protocolName);
    if (it != pImpl->protocols.end()) {
        return it->second.get();
    }
    return nullptr;
}

std::vector<std::string> AdvancedExperiment::getAvailableProtocols() const {
    std::vector<std::string> names;
    names.reserve(pImpl->protocols.size());
    
    for (const auto& pair : pImpl->protocols) {
        names.push_back(pair.first);
    }
    
    return names;
}

bool AdvancedExperiment::run(const std::string& protocolName) {
    return runProtocol(protocolName, pImpl->experimentConfig);
}

bool AdvancedExperiment::runProtocol(const std::string& protocolName, const Config& parameters) {
    auto it = pImpl->protocols.find(protocolName);
    if (it == pImpl->protocols.end()) {
        pImpl->logger->error("Protocol not found: " + protocolName);
        return false;
    }
    
    // Stop any running protocol
    if (pImpl->experimentRunning) {
        stop();
    }
    
    // Set parameters
    it->second->setParameters(parameters);
    
    // Start protocol
    if (!it->second->start()) {
        pImpl->logger->error("Failed to start protocol: " + protocolName);
        return false;
    }
    
    pImpl->currentProtocol = protocolName;
    pImpl->experimentRunning = true;
    pImpl->experimentPaused = false;
    pImpl->experimentCount++;
    pImpl->protocolRuns++;
    
    pImpl->startTime = std::chrono::system_clock::now();
    
    pImpl->logger->info("Experiment started: " + protocolName);
    triggerExperimentEvent("experiment_started", ExperimentEvent("experiment_started", protocolName));
    
    return true;
}

bool AdvancedExperiment::pause() {
    if (!pImpl->experimentRunning || pImpl->experimentPaused) {
        return false;
    }
    
    auto it = pImpl->protocols.find(pImpl->currentProtocol);
    if (it != pImpl->protocols.end()) {
        if (it->second->pause()) {
            pImpl->experimentPaused = true;
            pImpl->logger->info("Experiment paused");
            triggerExperimentEvent("experiment_paused", ExperimentEvent("experiment_paused", pImpl->currentProtocol));
            return true;
        }
    }
    
    return false;
}

bool AdvancedExperiment::resume() {
    if (!pImpl->experimentRunning || !pImpl->experimentPaused) {
        return false;
    }
    
    auto it = pImpl->protocols.find(pImpl->currentProtocol);
    if (it != pImpl->protocols.end()) {
        if (it->second->resume()) {
            pImpl->experimentPaused = false;
            pImpl->logger->info("Experiment resumed");
            triggerExperimentEvent("experiment_resumed", ExperimentEvent("experiment_resumed", pImpl->currentProtocol));
            return true;
        }
    }
    
    return false;
}

bool AdvancedExperiment::stop() {
    if (!pImpl->experimentRunning) {
        return false;
    }
    
    auto it = pImpl->protocols.find(pImpl->currentProtocol);
    if (it != pImpl->protocols.end()) {
        if (it->second->stop()) {
            pImpl->experimentRunning = false;
            pImpl->experimentPaused = false;
            pImpl->currentProtocol.clear();
            
            pImpl->logger->info("Experiment stopped");
            triggerExperimentEvent("experiment_stopped", ExperimentEvent("experiment_stopped", it->first));
            return true;
        }
    }
    
    return false;
}

bool AdvancedExperiment::reset() {
    if (pImpl->experimentRunning) {
        stop();
    }
    
    // Reset all protocols
    for (auto& pair : pImpl->protocols) {
        pair.second->reset();
    }
    
    pImpl->logger->info("Experiment reset");
    triggerExperimentEvent("experiment_reset", ExperimentEvent("experiment_reset", ""));
    
    return true;
}

bool AdvancedExperiment::addHyperparameter(const std::string& name, const std::vector<double>& values) {
    if (name.empty()) {
        pImpl->logger->error("Hyperparameter name cannot be empty");
        return false;
    }
    
    // Check for duplicate
    if (pImpl->searchSpaces.find(name) != pImpl->searchSpaces.end()) {
        pImpl->logger->warning("Hyperparameter already exists: " + name);
        return false;
    }
    
    pImpl->searchSpaces[name] = values;
    pImpl->logger->info("Hyperparameter added: " + name);
    return true;
}

bool AdvancedExperiment::removeHyperparameter(const std::string& name) {
    auto it = pImpl->searchSpaces.find(name);
    if (it != pImpl->searchSpaces.end()) {
        pImpl->searchSpaces.erase(it);
        pImpl->logger->info("Hyperparameter removed: " + name);
        return true;
    }
    
    pImpl->logger->warning("Hyperparameter not found: " + name);
    return false;
}

bool AdvancedExperiment::addOptimizationObjective(const std::string& objective,
                                                 std::function<double(const Config&)> objectiveFunction) {
    if (objective.empty() || !objectiveFunction) {
        pImpl->logger->error("Invalid optimization objective name or function");
        return false;
    }
    
    // Check for duplicate
    if (pImpl->objectiveFunctions.find(objective) != pImpl->objectiveFunctions.end()) {
        pImpl->logger->warning("Optimization objective already exists: " + objective);
        return false;
    }
    
    pImpl->objectiveFunctions[objective] = objectiveFunction;
    pImpl->logger->info("Optimization objective added: " + objective);
    return true;
}

bool AdvancedExperiment::removeOptimizationObjective(const std::string& objective) {
    auto it = pImpl->objectiveFunctions.find(objective);
    if (it != pImpl->objectiveFunctions.end()) {
        pImpl->objectiveFunctions.erase(it);
        pImpl->logger->info("Optimization objective removed: " + objective);
        return true;
    }
    
    pImpl->logger->warning("Optimization objective not found: " + objective);
    return false;
}

std::string AdvancedExperiment::runHyperparameterOptimization(const HyperparameterOptimizer& optimizer) {
    if (pImpl->objectiveFunctions.empty()) {
        pImpl->logger->error("No optimization objectives defined");
        return "";
    }
    
    // Use first objective for optimization
    auto it = pImpl->objectiveFunctions.begin();
    std::string objectiveName = it->first;
    
    // Convert search spaces to Config
    Config config;
    for (const auto& pair : pImpl->searchSpaces) {
        // Use mean of values as default
        double sum = std::accumulate(pair.second.begin(), pair.second.end(), 0.0);
        double mean = sum / pair.second.size();
        config.set(pair.first, mean);
    }
    
    // Configure optimizer
    optimizer.setAlgorithm("random_search"); // Default algorithm
    
    // Run optimization
    Config optimalParams = optimizer.optimize(objectiveName);
    
    pImpl->optimalParameters = optimalParams;
    pImpl->optimalObjectiveValue = optimizer.getOptimalObjectiveValue();
    pImpl->optimizationRuns++;
    
    pImpl->logger->info("Hyperparameter optimization completed. Best objective value: " + std::to_string(pImpl->optimalObjectiveValue));
    
    return "Optimization completed with objective value: " + std::to_string(pImpl->optimalObjectiveValue);
}

Config AdvancedExperiment::getOptimalParameters() const {
    return pImpl->optimalParameters;
}

bool AdvancedExperiment::enableMonitoring(bool enable) {
    pImpl->monitoringEnabled = enable;
    if (enable) {
        pImpl->startTime = std::chrono::system_clock::now();
    }
    
    pImpl->logger->info("Experiment monitoring " + std::string(enable ? "enabled" : "disabled"));
    return true;
}

bool AdvancedExperiment::isMonitoringEnabled() const {
    return pImpl->monitoringEnabled;
}

std::string AdvancedExperiment::getExperimentStatus() const {
    std::stringstream ss;
    ss << "Experiment Status:\n";
    ss << "=================\n";
    ss << "Total experiments: " << pImpl->experimentCount << "\n";
    ss << "Successful experiments: " << pImpl->successfulExperiments << "\n";
    ss << "Protocol runs: " << pImpl->protocolRuns << "\n";
    ss << "Optimization runs: " << pImpl->optimizationRuns << "\n";
    ss << "Experiment running: " << (pImpl->experimentRunning ? "yes" : "no") << "\n";
    ss << "Experiment paused: " << (pImpl->experimentPaused ? "yes" : "no") << "\n";
    ss << "Current protocol: " << (pImpl->currentProtocol.empty() ? "none" : pImpl->currentProtocol) << "\n";
    ss << "Monitoring enabled: " << (pImpl->monitoringEnabled ? "yes" : "no") << "\n";
    
    if (pImpl->monitoringEnabled) {
        ss << "Experiment duration: " << getExperimentDuration() << " seconds\n";
    }
    
    return ss.str();
}

std::chrono::system_clock::time_point AdvancedExperiment::getExperimentStartTime() const {
    return pImpl->startTime;
}

double AdvancedExperiment::getExperimentDuration() const {
    if (!pImpl->monitoringEnabled) return 0.0;
    
    auto now = std::chrono::system_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::seconds>(now - pImpl->startTime);
    return duration.count() + pImpl->totalDuration;
}

bool AdvancedExperiment::saveResults(const std::string& filepath) const {
    try {
        std::ofstream file(filepath);
        if (!file) {
            pImpl->logger->error("Failed to open results file: " + filepath);
            return false;
        }
        
        // In real implementation, would serialize results
        file << "Experiment Results\n";
        file << "===================\n";
        file << "Total experiments: " << pImpl->experimentCount << "\n";
        file << "Successful experiments: " << pImpl->successfulExperiments << "\n";
        
        pImpl->logger->info("Experiment results saved to: " + filepath);
        return true;
    } catch (const std::exception& e) {
        pImpl->logger->error("Failed to save experiment results: " + std::string(e.what()));
        return false;
    }
}

bool AdvancedExperiment::loadResults(const std::string& filepath) {
    try {
        std::ifstream file(filepath);
        if (!file) {
            pImpl->logger->error("Failed to open results file: " + filepath);
            return false;
        }
        
        std::string line;
        while (std::getline(file, line)) {
            // In real implementation, would parse results
        }
        
        pImpl->logger->info("Experiment results loaded from: " + filepath);
        return true;
    } catch (const std::exception& e) {
        pImpl->logger->error("Failed to load experiment results: " + std::string(e.what()));
        return false;
    }
}

bool AdvancedExperiment::exportResultsToCSV(const std::string& filepath) const {
    try {
        std::ofstream file(filepath);
        if (!file) {
            pImpl->logger->error("Failed to open CSV file: " + filepath);
            return false;
        }
        
        file << "Experiment,Success,Duration,Objective\n";
        
        pImpl->logger->info("Experiment results exported to CSV: " + filepath);
        return true;
    } catch (const std::exception& e) {
        pImpl->logger->error("Failed to export results to CSV: " + std::string(e.what()));
        return false;
    }
}

bool AdvancedExperiment::exportResultsToJSON(const std::string& filepath) const {
    try {
        std::ofstream file(filepath);
        if (!file) {
            pImpl->logger->error("Failed to open JSON file: " + filepath);
            return false;
        }
        
        // In real implementation, would create JSON structure
        file << "{\"experiment\": \"framework\", \"version\": \"1.0\"}\n";
        
        pImpl->logger->info("Experiment results exported to JSON: " + filepath);
        return true;
    } catch (const std::exception& e) {
        pImpl->logger->error("Failed to export results to JSON: " + std::string(e.what()));
        return false;
    }
}

std::string AdvancedExperiment::getProtocolStats(const std::string& protocolName) const {
    auto it = pImpl->protocols.find(protocolName);
    if (it == pImpl->protocols.end()) {
        return "Protocol not found: " + protocolName;
    }
    
    return it->second->getProtocolStats();
}

std::string AdvancedExperiment::getOptimizationStats() const {
    std::stringstream ss;
    ss << "Optimization Statistics:\n";
    ss << "=====================\n";
    ss << "Optimization runs: " << pImpl->optimizationRuns << "\n";
    ss << "Optimal objective value: " << pImpl->optimalObjectiveValue << "\n";
    
    return ss.str();
}

std::string AdvancedExperiment::getExperimentAnalytics() const {
    std::stringstream ss;
    ss << "Experiment Analytics:\n";
    ss << "====================\n";
    ss << "Success rate: " << (pImpl->experimentCount > 0 ? 
        static_cast<double>(pImpl->successfulExperiments) / pImpl->experimentCount * 100 : 0.0) << "%\n";
    ss << "Average optimization time: N/A\n";
    ss << "Protocol usage:\n";
    
    for (const auto& pair : pImpl->protocols) {
        ss << "  - " << pair.first << "\n";
    }
    
    return ss.str();
}

void AdvancedExperiment::setExperimentConfig(const Config& config) {
    pImpl->experimentConfig = config;
}

Config AdvancedExperiment::getExperimentConfig() const {
    return pImpl->experimentConfig;
}

void AdvancedExperiment::addExperimentParameter(const std::string& key, const ConfigValue& value) {
    pImpl->experimentConfig.set(key, value);
}

ConfigValue AdvancedExperiment::getExperimentParameter(const std::string& key) const {
    return pImpl->experimentConfig.get<ConfigValue>(key);
}

bool AdvancedExperiment::addCustomMetric(const std::string& name,
                                        std::function<double(const ExperimentData&)> metricCalculator) {
    if (name.empty() || !metricCalculator) {
        pImpl->logger->error("Invalid custom metric name or function");
        return false;
    }
    
    // Check for duplicate
    if (pImpl->customMetrics.find(name) != pImpl->customMetrics.end()) {
        pImpl->logger->warning("Custom metric already exists: " + name);
        return false;
    }
    
    pImpl->customMetrics[name] = metricCalculator;
    pImpl->logger->info("Custom metric added: " + name);
    return true;
}

bool AdvancedExperiment::removeCustomMetric(const std::string& name) {
    auto it = pImpl->customMetrics.find(name);
    if (it != pImpl->customMetrics.end()) {
        pImpl->customMetrics.erase(it);
        pImpl->logger->info("Custom metric removed: " + name);
        return true;
    }
    
    pImpl->logger->warning("Custom metric not found: " + name);
    return false;
}

std::vector<std::string> AdvancedExperiment::getCustomMetrics() const {
    std::vector<std::string> names;
    names.reserve(pImpl->customMetrics.size());
    
    for (const auto& metric : pImpl->customMetrics) {
        names.push_back(metric.first);
    }
    
    return names;
}

std::map<std::string, double> AdvancedExperiment::calculateAllMetrics(const ExperimentData& data) const {
    std::map<std::string, double> metrics;
    
    // Calculate default metrics
    metrics["experiment_id"] = std::chrono::duration_cast<std::chrono::milliseconds>(
        data.timestamp.time_since_epoch()).count();
    metrics["progress"] = data.progress;
    metrics["completed"] = data.completed ? 1.0 : 0.0;
    
    // Calculate custom metrics
    for (const auto& metric : pImpl->customMetrics) {
        try {
            metrics[metric.first] = metric.second(data);
        } catch (const std::exception& e) {
            pImpl->logger->warning("Error calculating custom metric " + metric.first + ": " + std::string(e.what()));
        }
    }
    
    return metrics;
}

void AdvancedExperiment::registerExperimentCallback(const std::string& eventType, ExperimentCallback callback) {
    if (eventType.empty() || !callback) {
        pImpl->logger->error("Invalid experiment event type or callback");
        return;
    }
    
    pImpl->experimentCallbacks.push_back({eventType, callback});
}

void AdvancedExperiment::triggerExperimentEvent(const std::string& eventName, const ExperimentEvent& event) {
    for (const auto& callbackEntry : pImpl->experimentCallbacks) {
        if (callbackEntry.eventType == eventName) {
            try {
                callbackEntry.callback(eventName, event);
            } catch (const std::exception& e) {
                pImpl->logger->error("Error in experiment callback: " + std::string(e.what()));
            }
        }
    }
}

// ScalingBenchmarkProtocol implementations
bool ScalingBenchmarkProtocol::initialize(const Config& config) {
    // Initialize scaling benchmark protocol
    return true;
}

void ScalingBenchmarkProtocol::update(double timestep) {
    // Update protocol state
}

void ScalingBenchmarkProtocol::shutdown() {
    // Cleanup protocol resources
}

bool ScalingBenchmarkProtocol::start() {
    return true;
}

bool ScalingBenchmarkProtocol::pause() {
    return true;
}

bool ScalingBenchmarkProtocol::resume() {
    return true;
}

bool ScalingBenchmarkProtocol::stop() {
    return true;
}

bool ScalingBenchmarkProtocol::reset() {
    return true;
}

bool ScalingBenchmarkProtocol::collectData(const ExperimentData& data) {
    return true;
}

bool ScalingBenchmarkProtocol::validateData(const ExperimentData& data) {
    return true;
}

std::map<std::string, double> ScalingBenchmarkProtocol::calculateMetrics(const ExperimentData& data) {
    std::map<std::string, double> metrics;
    
    // Calculate scaling metrics
    metrics["efficiency"] = 0.5; // Placeholder
    metrics["performance"] = 0.8; // Placeholder
    
    return metrics;
}

void ScalingBenchmarkProtocol::setParameters(const Config& parameters) {
    // Store parameters
}

Config ScalingBenchmarkProtocol::getParameters() const {
    return Config();
}

bool ScalingBenchmarkProtocol::validateParameters(const Config& parameters) const {
    return true;
}

bool ScalingBenchmarkProtocol::configureBrain(class ExtensibleBrain& brain) {
    // Configure brain for scaling benchmark
    return true;
}

bool ScalingBenchmarkProtocol::configureArchitecture(class NeuralArchitecture& arch) {
    // Configure architecture for scaling benchmark
    return true;
}

bool ScalingBenchmarkProtocol::setupEnvironment(class Environment& env) {
    // Setup environment for scaling benchmark
    return true;
}

// PhaseTrainingProtocol implementations (similar pattern)
bool PhaseTrainingProtocol::initialize(const Config& config) { return true; }
void PhaseTrainingProtocol::update(double timestep) {}
void PhaseTrainingProtocol::shutdown() {}
bool PhaseTrainingProtocol::start() { return true; }
bool PhaseTrainingProtocol::pause() { return true; }
bool PhaseTrainingProtocol::resume() { return true; }
bool PhaseTrainingProtocol::stop() { return true; }
bool PhaseTrainingProtocol::reset() { return true; }
bool PhaseTrainingProtocol::collectData(const ExperimentData& data) { return true; }
bool PhaseTrainingProtocol::validateData(const ExperimentData& data) { return true; }
std::map<std::string, double> PhaseTrainingProtocol::calculateMetrics(const ExperimentData& data) { return std::map<std::string, double>(); }
void PhaseTrainingProtocol::setParameters(const Config& parameters) {}
Config PhaseTrainingProtocol::getParameters() const { return Config(); }
bool PhaseTrainingProtocol::validateParameters(const Config& parameters) const { return true; }
bool PhaseTrainingProtocol::configureBrain(class ExtensibleBrain& brain) { return true; }
bool PhaseTrainingProtocol::configureArchitecture(class NeuralArchitecture& arch) { return true; }
bool PhaseTrainingProtocol::setupEnvironment(class Environment& env) { return true; }

} // namespace nlm
