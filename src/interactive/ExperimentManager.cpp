#include "InteractiveVisualizationTools.hpp"

namespace nlm {

struct ExperimentManager::Impl {
    const Brain* managedBrain;
    std::unordered_map<std::string, std::unordered_map<std::string, std::string>> experimentConfigurations;
    std::unordered_map<std::string, std::vector<std::pair<double, std::unordered_map<std::string, double>>>> experimentResults;
    std::unordered_map<std::string, bool> experimentRunning;
    std::function<void(const std::string&, const std::string&)> experimentCallback;
    double lastUpdateTime;
    
    Impl() : managedBrain(nullptr), lastUpdateTime(0.0) {}
};

ExperimentManager::ExperimentManager() : pImpl(std::make_unique<Impl>()) {}

ExperimentManager::~ExperimentManager() = default;

ExperimentManager::ExperimentManager(ExperimentManager&&) noexcept = default;
ExperimentManager& ExperimentManager::operator=(ExperimentManager&&) noexcept = default;

bool ExperimentManager::initialize(const Brain& brain) {
    pImpl->managedBrain = &brain;
    return true;
}

bool ExperimentManager::createExperiment(const std::string& experimentName, const std::unordered_map<std::string, std::string>& configuration) {
    pImpl->experimentConfigurations[experimentName] = configuration;
    pImpl->experimentRunning[experimentName] = false;
    
    if (pImpl->experimentCallback) {
        pImpl->experimentCallback(experimentName, "created");
    }
    
    return true;
}

bool ExperimentManager::loadExperiment(const std::string& experimentName) {
    auto it = pImpl->experimentConfigurations.find(experimentName);
    if (it == pImpl->experimentConfigurations.end()) {
        return false;
    }
    
    return true;
}

void ExperimentManager::runExperiment(const std::string& experimentName) {
    auto it = pImpl->experimentRunning.find(experimentName);
    if (it != pImpl->experimentRunning.end()) {
        it->second = true;
        
        if (pImpl->experimentCallback) {
            pImpl->experimentCallback(experimentName, "started");
        }
    }
}

void ExperimentManager::stopExperiment(const std::string& experimentName) {
    auto it = pImpl->experimentRunning.find(experimentName);
    if (it != pImpl->experimentRunning.end()) {
        it->second = false;
        
        if (pImpl->experimentCallback) {
            pImpl->experimentCallback(experimentName, "stopped");
        }
    }
}

std::unordered_map<std::string, double> ExperimentManager::getExperimentResults(const std::string& experimentName) const {
    std::unordered_map<std::string, double> results;
    
    auto it = pImpl->experimentResults.find(experimentName);
    if (it != pImpl->experimentResults.end() && !it->second.empty()) {
        results = it->second.back().second;
    }
    
    return results;
}

std::unordered_map<std::string, std::string> ExperimentManager::getExperimentConfiguration(const std::string& experimentName) const {
    auto it = pImpl->experimentConfigurations.find(experimentName);
    if (it != pImpl->experimentConfigurations.end()) {
        return it->second;
    }
    
    return std::unordered_map<std::string, std::string>();
}

std::vector<std::unordered_map<std::string, double>> ExperimentManager::compareExperiments(const std::vector<std::string>& experimentNames) const {
    std::vector<std::unordered_map<std::string, double>> comparisons;
    
    for (const auto& experimentName : experimentNames) {
        auto results = getExperimentResults(experimentName);
        if (!results.empty()) {
            comparisons.push_back(results);
        }
    }
    
    return comparisons;
}

bool ExperimentManager::exportExperimentResults(const std::string& experimentName, const std::string& format, const std::string& filepath) {
    // Implementation would export experiment results in specified format
    return false;
}

std::vector<std::pair<double, std::unordered_map<std::string, double>>> ExperimentManager::getExperimentHistory(const std::string& experimentName, size_t count) const {
    std::vector<std::pair<double, std::unordered_map<std::string, double>>> history;
    
    auto it = pImpl->experimentResults.find(experimentName);
    if (it != pImpl->experimentResults.end()) {
        auto data = it->second;
        size_t startIndex = std::max(0, (int64_t)(data.size() - count));
        for (size_t i = startIndex; i < data.size(); ++i) {
            history.push_back(data[i]);
        }
    }
    
    return history;
}

bool ExperimentManager::isExperimentRunning(const std::string& experimentName) const {
    auto it = pImpl->experimentRunning.find(experimentName);
    if (it != pImpl->experimentRunning.end()) {
        return it->second;
    }
    return false;
}

std::vector<std::string> ExperimentManager::getAvailableExperimentTemplates() const {
    return std::vector<std::string>{"benchmark", "validation", "performance", "learning"};
}

void ExperimentManager::setExperimentCallback(std::function<void(const std::string&, const std::string&)> callback) {
    pImpl->experimentCallback = callback;
}

} // namespace nlm