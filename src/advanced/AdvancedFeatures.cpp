#include "AdvancedFeatures.hpp"
#include "../brain/Brain.hpp"
#include "../experiments/ExperimentRunner.hpp"
#include "../core/Logger/Logger.hpp"
#include <thread>
#include <atomic>
#include <unordered_map>
#include <deque>
#include <algorithm>
#include <cmath>
#include <chrono>
#include <sstream>
#include <mutex>

namespace nlm {

// Forward declarations
class AdvancedConfigManager;
class PerformanceMonitor;
class AdvancedExperiment;

struct AdvancedConfigManager::Impl {
    std::shared_ptr<Config> currentConfig;
    std::deque<ConfigChangeEvent> history;
    std::deque<std::shared_ptr<Config>> previousStates;
    std::map<std::string, std::function<bool(const Config&)>> validators;
    std::map<std::string, std::string> presets;
    mutable std::mutex mutex;
    
    Impl() {
        currentConfig = std::make_shared<Config>();
        
        // Define some common presets
        presets["default"] = "neuron_count=1000\nregion_count=1\nconnection_probability=0.1";
        presets["high_performance"] = "neuron_count=500\nsleep_interval=5000\nsleep_replay_interval=500";
        presets["deep_learning"] = "neuron_count=2000\nsleep_interval=20000\nsleep_replay_interval=2000";
        presets["balanced"] = "neuron_count=1000\nsleep_interval=10000\nsleep_replay_interval=1000";
        presets["conservative"] = "neuron_count=500\nsleep_interval=50000\nsleep_replay_interval=5000";
    }
};

AdvancedConfigManager::AdvancedConfigManager() : pImpl(std::make_unique<Impl>()) {}
AdvancedConfigManager::~AdvancedConfigManager() = default;

bool AdvancedConfigManager::applyPreset(const std::string& presetName) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    auto it = pImpl->presets.find(presetName);
    if (it == pImpl->presets.end()) {
        NLM_LOG_WARNING("Unknown preset: " + presetName);
        return false;
    }
    
    // Clear current config
    pImpl->currentConfig->clear();
    
    // Apply preset (simplified parsing)
    std::istringstream stream(it->second);
    std::string line;
    while (std::getline(stream, line)) {
        size_t pos = line.find('=');
        if (pos != std::string::npos) {
            std::string key = Config::trim(line.substr(0, pos));
            std::string value = Config::trim(line.substr(pos + 1));
            pImpl->currentConfig->set(key, value);
        }
    }
    
    // Add to history
    ConfigChangeEvent event;
    event.key = "preset_" + presetName;
    event.oldValue = "";
    event.newValue = presetName;
    event.timestamp = std::chrono::system_clock::now();
    event.source = "preset";
    pImpl->history.push_back(event);
    
    NLM_LOG_INFO("Applied preset: " + presetName);
    return true;
}

ConfigValidationResult AdvancedConfigManager::validate(const Config& config) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    ConfigValidationResult result;
    result.valid = true;
    
    // Basic validation - check for required keys
    std::vector<std::string> requiredKeys = {"neuron_count", "region_count"};
    for (const auto& key : requiredKeys) {
        if (!config.has(key)) {
            result.errors.push_back("Missing required key: " + key);
            result.valid = false;
        }
    }
    
    // Check value ranges
    auto neuronCount = config.getOr<size_t>("neuron_count", 0);
    if (neuronCount > 100000) {
        result.warnings.push_back("Very high neuron count may impact performance");
    }
    
    auto regionCount = config.getOr<size_t>("region_count", 1);
    if (regionCount == 0) {
        result.errors.push_back("Region count must be positive");
        result.valid = false;
    }
    
    return result;
}

std::vector<ConfigChangeEvent> AdvancedConfigManager::getHistory() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return pImpl->history;
}

std::string AdvancedConfigManager::getStats() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    std::ostringstream oss;
    oss << "Configuration Statistics:" << std::endl;
    oss << "  Current entries: " << pImpl->currentConfig->getKeys().size() << std::endl;
    oss << "  History size: " << pImpl->history.size() << std::endl;
    oss << "  Previous states: " << pImpl->previousStates.size() << std::endl;
    return oss.str();
}

template<typename T>
std::optional<T> AdvancedConfigManager::get(const std::string& key) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return pImpl->currentConfig->get<T>(key);
}

bool AdvancedConfigManager::has(const std::string& key) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return pImpl->currentConfig->has(key);
}

bool AdvancedConfigManager::modify(const std::string& key, const std::string& value) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    if (!pImpl->currentConfig->has(key)) {
        NLM_LOG_WARNING("Key does not exist: " + key);
        return false;
    }
    
    // Get old value
    std::string oldValue;
    auto oldOpt = pImpl->currentConfig->get<std::string>(key);
    if (oldOpt) oldValue = *oldOpt;
    
    // Set new value
    pImpl->currentConfig->set(key, value);
    
    // Add to history
    ConfigChangeEvent event;
    event.key = key;
    event.oldValue = oldValue;
    event.newValue = value;
    event.timestamp = std::chrono::system_clock::now();
    event.source = "runtime";
    pImpl->history.push_back(event);
    
    // Save previous state for rollback
    auto previousConfig = std::make_shared<Config>(*pImpl->currentConfig);
    pImpl->previousStates.push_back(previousConfig);
    
    NLM_LOG_INFO("Modified config key: " + key + " from '" + oldValue + "' to '" + value + "'");
    return true;
}

bool AdvancedConfigManager::exportConfig(const std::string& filepath) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return pImpl->currentConfig->saveToFile(filepath);
}

bool AdvancedConfigManager::importConfig(const std::string& filepath) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    auto tempConfig = std::make_shared<Config>();
    bool success = tempConfig->loadFromFile(filepath);
    
    if (success) {
        // Validate imported config
        ConfigValidationResult validation = validate(*tempConfig);
        if (!validation.valid) {
            NLM_LOG_ERROR("Imported config failed validation:");
            for (const auto& error : validation.errors) {
                NLM_LOG_ERROR("  " + error);
            }
            return false;
        }
        
        // Store old config for history
        std::string oldValue;
        if (pImpl->currentConfig->has("imported_config")) {
            auto oldOpt = pImpl->currentConfig->get<std::string>("imported_config");
            if (oldOpt) oldValue = *oldOpt;
        }
        
        // Replace current config
        pImpl->currentConfig = tempConfig;
        
        // Add to history
        ConfigChangeEvent event;
        event.key = "imported_config";
        event.oldValue = oldValue;
        event.newValue = "imported:" + filepath;
        event.timestamp = std::chrono::system_clock::now();
        event.source = "import";
        pImpl->history.push_back(event);
        
        NLM_LOG_INFO("Successfully imported config from: " + filepath);
    }
    
    return success;
}

bool AdvancedConfigManager::rollback(size_t steps) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    if (pImpl->previousStates.empty()) {
        NLM_LOG_WARNING("No previous states to rollback to");
        return false;
    }
    
    // Restore from previous state
    size_t restoreIndex = std::min(steps, pImpl->previousStates.size() - 1);
    pImpl->currentConfig = pImpl->previousStates[restoreIndex];
    
    // Add to history
    ConfigChangeEvent event;
    event.key = "rollback";
    event.oldValue = "multiple_states";
    event.newValue = "restored_from_state_" + std::to_string(restoreIndex);
    event.timestamp = std::chrono::system_clock::now();
    event.source = "rollback";
    pImpl->history.push_back(event);
    
    NLM_LOG_INFO("Rollback successful to state " + std::to_string(restoreIndex));
    return true;
}

void AdvancedConfigManager::clearHistory() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->history.clear();
    pImpl->previousStates.clear();
    NLM_LOG_INFO("Configuration history cleared");
}

// PerformanceMonitor implementation

struct PerformanceMonitor::Impl {
    std::deque<PerformanceMetrics> metricsHistory;
    std::vector<PerformanceAlert> alerts;
    std::map<std::string, std::tuple<double, double, double>> thresholds; // metric -> (warning, error, critical)
    PerformanceMetrics currentMetrics;
    size_t maxHistorySize;
    std::atomic<bool> running;
    std::thread samplingThread;
    mutable std::mutex mutex;
    
    Impl() : maxHistorySize(1000), running(false) {
        // Set default thresholds
        thresholds["cpuUsage"] = std::make_tuple(80.0, 90.0, 95.0);
        thresholds["memoryUsage"] = std::make_tuple(80.0, 90.0, 95.0);
        thresholds["averageFiringRate"] = std::make_tuple(100.0, 200.0, 300.0);
    }
};

PerformanceMonitor::PerformanceMonitor() : pImpl(std::make_unique<Impl>()) {}
PerformanceMonitor::~PerformanceMonitor() {
    if (pImpl->running) {
        pImpl->running = false;
        if (pImpl->samplingThread.joinable()) {
            pImpl->samplingThread.join();
        }
    }
}

void PerformanceMonitor::recordMetrics(const PerformanceMetrics& metrics) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    pImpl->currentMetrics = metrics;
    pImpl->metricsHistory.push_back(metrics);
    
    // Keep history size manageable
    while (pImpl->metricsHistory.size() > pImpl->maxHistorySize) {
        pImpl->metricsHistory.pop_front();
    }
    
    // Check for alerts
    std::vector<PerformanceAlert> newAlerts;
    for (const auto& [metric, threshold] : pImpl->thresholds) {
        double warning = std::get<0>(threshold);
        double error = std::get<1>(threshold);
        double critical = std::get<2>(threshold);
        
        double value = 0.0;
        if (metric == "cpuUsage") value = metrics.cpuUsage;
        else if (metric == "memoryUsage") value = metrics.memoryUsage;
        else if (metric == "averageFiringRate") value = metrics.averageFiringRate;
        
        AlertLevel level = AlertLevel::INFO;
        if (value >= critical) level = AlertLevel::CRITICAL;
        else if (value >= error) level = AlertLevel::ERROR;
        else if (value >= warning) level = AlertLevel::WARNING;
        
        if (level != AlertLevel::INFO) {
            PerformanceAlert alert;
            alert.level = level;
            alert.message = "High " + metric + ": " + std::to_string(value) + " (threshold: " + std::to_string(warning) + ")";
            alert.metric = metric;
            alert.value = value;
            alert.threshold = warning;
            alert.timestamp = std::chrono::system_clock::now();
            newAlerts.push_back(alert);
        }
    }
    
    // Add new alerts
    pImpl->alerts.insert(pImpl->alerts.end(), newAlerts.begin(), newAlerts.end());
    
    // Keep alerts manageable
    if (pImpl->alerts.size() > 100) {
        pImpl->alerts.erase(pImpl->alerts.begin(), pImpl->alerts.begin() + (pImpl->alerts.size() - 100));
    }
}

std::vector<std::string> PerformanceMonitor::detectBottlenecks() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    std::vector<std::string> bottlenecks;
    
    if (pImpl->metricsHistory.empty()) return bottlenecks;
    
    // Analyze history for patterns
    double avgExecutionTime = 0.0;
    size_t totalSteps = 0;
    for (const auto& metrics : pImpl->metricsHistory) {
        avgExecutionTime += metrics.executionTime;
        totalSteps++;
    }
    if (totalSteps > 0) avgExecutionTime /= totalSteps;
    
    if (avgExecutionTime > 100.0) { // 100ms threshold
        bottlenecks.push_back("High average execution time: " + std::to_string(avgExecutionTime) + "ms");
    }
    
    // Check for memory growth trend
    if (pImpl->metricsHistory.size() >= 10) {
        size_t allocations = 0, deallocations = 0;
        auto it = pImpl->metricsHistory.end();
        for (int i = 0; i < 10 && it != pImpl->metricsHistory.begin(); ++i) {
            --it;
            allocations += it->memoryAllocations;
            deallocations += it->memoryDeallocations;
        }
        
        if (allocations - deallocations > 1000) { // Net memory growth
            bottlenecks.push_back("Memory leak detected: " + std::to_string(allocations - deallocations) + " net allocations");
        }
    }
    
    return bottlenecks;
}

std::vector<PerformanceAlert> PerformanceMonitor::getAlerts() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return pImpl->alerts;
}

PerformanceMetrics PerformanceMonitor::getCurrentMetrics() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return pImpl->currentMetrics;
}

std::string PerformanceMonitor::getStats() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    std::ostringstream oss;
    oss << "Performance Monitor Statistics:" << std::endl;
    oss << "  History size: " << pImpl->metricsHistory.size() << std::endl;
    oss << "  Active alerts: " << pImpl->alerts.size() << std::endl;
    oss << "  Current metrics:" << std::endl;
    oss << "    CPU Usage: " << pImpl->currentMetrics.cpuUsage << "%" << std::endl;
    oss << "    Memory Usage: " << pImpl->currentMetrics.memoryUsage << "%" << std::endl;
    oss << "    Average Firing Rate: " << pImpl->currentMetrics.averageFiringRate << " Hz" << std::endl;
    oss << "    Total Spikes: " << pImpl->currentMetrics.totalSpikes << std::endl;
    oss << "    Execution Time: " << pImpl->currentMetrics.executionTime << " ms" << std::endl;
    return oss.str();
}

void PerformanceMonitor::reset() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->metricsHistory.clear();
    pImpl->alerts.clear();
    NLM_LOG_INFO("Performance monitor reset");
}

void PerformanceMonitor::setThreshold(const std::string& metric, double warning, double error, double critical) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->thresholds[metric] = std::make_tuple(warning, error, critical);
    NLM_LOG_INFO("Set threshold for " + metric + ": warning=" + std::to_string(warning) + ", error=" + std::to_string(error) + ", critical=" + std::to_string(critical));
}

bool PerformanceMonitor::checkResourceUsage() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    const auto& metrics = pImpl->currentMetrics;
    return metrics.cpuUsage < 95.0 && metrics.memoryUsage < 95.0;
}

std::string PerformanceMonitor::generateReport() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    std::ostringstream oss;
    oss << "=== Performance Report ===" << std::endl;
    oss << "Generated at: " << std::chrono::system_clock::now() << std::endl;
    oss << std::endl;
    
    if (!pImpl->metricsHistory.empty()) {
        oss << "Performance Summary:" << std::endl;
        oss << "  Total steps recorded: " << pImpl->metricsHistory.size() << std::endl;
        oss << "  Average execution time: " << getAverageExecutionTime() << " ms" << std::endl;
        oss << "  Average CPU usage: " << getAverageCpuUsage() << "%" << std::endl;
        oss << "  Average memory usage: " << getAverageMemoryUsage() << "%" << std::endl;
        oss << std::endl;
    }
    
    if (!pImpl->alerts.empty()) {
        oss << "Performance Alerts:" << std::endl;
        for (const auto& alert : pImpl->alerts) {
            std::string levelStr;
            switch (alert.level) {
                case AlertLevel::INFO: levelStr = "INFO"; break;
                case AlertLevel::WARNING: levelStr = "WARNING"; break;
                case AlertLevel::ERROR: levelStr = "ERROR"; break;
                case AlertLevel::CRITICAL: levelStr = "CRITICAL"; break;
            }
            oss << "  [" << levelStr << "] " << alert.message << std::endl;
        }
        oss << std::endl;
    }
    
    auto bottlenecks = detectBottlenecks();
    if (!bottlenecks.empty()) {
        oss << "Bottlenecks Detected:" << std::endl;
        for (const auto& bottleneck : bottlenecks) {
            oss << "  - " << bottleneck << std::endl;
        }
        oss << std::endl;
    }
    
    return oss.str();
}

// Helper methods for PerformanceMonitor

double PerformanceMonitor::getAverageExecutionTime() const {
    if (pImpl->metricsHistory.empty()) return 0.0;
    double sum = 0.0;
    for (const auto& metrics : pImpl->metricsHistory) {
        sum += metrics.executionTime;
    }
    return sum / pImpl->metricsHistory.size();
}

double PerformanceMonitor::getAverageCpuUsage() const {
    if (pImpl->metricsHistory.empty()) return 0.0;
    double sum = 0.0;
    for (const auto& metrics : pImpl->metricsHistory) {
        sum += metrics.cpuUsage;
    }
    return sum / pImpl->metricsHistory.size();
}

double PerformanceMonitor::getAverageMemoryUsage() const {
    if (pImpl->metricsHistory.empty()) return 0.0;
    double sum = 0.0;
    for (const auto& metrics : pImpl->metricsHistory) {
        sum += metrics.memoryUsage;
    }
    return sum / pImpl->metricsHistory.size();
}

// AdvancedExperiment implementation

struct AdvancedExperiment::Impl {
    std::string name;
    std::vector<std::string> parameters;
    std::vector<std::string> constraints;
    std::map<std::string, std::pair<double, double>> parameterRanges;
    mutable std::mutex mutex;
};

AdvancedExperiment::AdvancedExperiment(const std::string& name) : pImpl(std::make_unique<Impl>()) {
    pImpl->name = name;
}

AdvancedExperiment::~AdvancedExperiment() = default;

void AdvancedExperiment::addParameter(const std::string& name, double min, double max, double step) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->parameters.push_back(name);
    pImpl->parameterRanges[name] = std::make_pair(min, max);
    NLM_LOG_INFO("Added parameter to experiment " + name + ": " + name + " [" + std::to_string(min) + ", " + std::to_string(max) + "], step=" + std::to_string(step));
}

void AdvancedExperiment::addConstraint(const std::string& condition, const std::string& description) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->constraints.push_back(condition);
    NLM_LOG_INFO("Added constraint to experiment " + name + ": " + description);
}

std::map<std::string, double> AdvancedExperiment::runParameterSweep(
    std::shared_ptr<Brain> brainTemplate,
    std::shared_ptr<Environment> environmentTemplate,
    SimulationStep maxSteps) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    std::map<std::string, double> bestResults;
    
    // Simple parameter sweep implementation
    NLM_LOG_INFO("Running parameter sweep for experiment " + name);
    
    // In a full implementation, this would:
    // 1. Iterate through all parameter combinations
    // 2. Run experiments with each combination
    // 3. Track results and find optimal values
    // For now, return placeholder results
    
    for (const auto& param : pImpl->parameters) {
        bestResults[param] = 0.0; // Placeholder
    }
    
    return bestResults;
}

std::map<std::string, double> AdvancedExperiment::runAdaptiveOptimization(
    std::shared_ptr<Brain> brainTemplate,
    std::shared_ptr<Environment> environmentTemplate,
    SimulationStep maxSteps) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    std::map<std::string, double> optimizedResults;
    
    // Simple adaptive optimization implementation
    NLM_LOG_INFO("Running adaptive optimization for experiment " + name);
    
    // In a full implementation, this would:
    // 1. Use Bayesian optimization or similar
    // 2. Iteratively improve parameter values
    // 3. Converge to optimal configuration
    // For now, return placeholder results
    
    for (const auto& param : pImpl->parameters) {
        optimizedResults[param] = 0.0; // Placeholder
    }
    
    return optimizedResults;
}

std::string AdvancedExperiment::getResults() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    std::ostringstream oss;
    oss << "=== Advanced Experiment Results ===" << std::endl;
    oss << "Experiment: " << name << std::endl;
    oss << std::endl;
    oss << "Parameters:" << std::endl;
    for (const auto& param : parameters) {
        oss << "  - " << param << std::endl;
    }
    oss << std::endl;
    oss << "Constraints:" << std::endl;
    for (const auto& constraint : constraints) {
        oss << "  - " << constraint << std::endl;
    }
    
    return oss.str();
}

bool AdvancedExperiment::save(const std::string& filepath) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    std::ofstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to save experiment to: " + filepath);
        return false;
    }
    
    file << "ExperimentName: " << name << std::endl;
    file << "ParametersCount: " << parameters.size() << std::endl;
    file << "ConstraintsCount: " << constraints.size() << std::endl;
    
    file.close();
    NLM_LOG_INFO("Saved experiment to: " + filepath);
    return true;
}

bool AdvancedExperiment::load(const std::string& filepath) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    std::ifstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to load experiment from: " + filepath);
        return false;
    }
    
    std::string line;
    while (std::getline(file, line)) {
        if (line.find("ExperimentName: ") == 0) {
            name = line.substr(18);
        }
    }
    
    file.close();
    NLM_LOG_INFO("Loaded experiment from: " + filepath);
    return true;
}

} // namespace nlm