#include "ExperimentBase.hpp"
#include "../core/Logger/Logger.hpp"
#include "../core/Config/Config.hpp"
#include <fstream>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <chrono>
#include <ctime>
#include <cmath>

namespace nlm {

struct ExperimentBase::Impl {
    std::string name;
    uint64_t seed;
    std::string timestamp;
    std::string status;
    float progress;
    
    // Configuration
    std::vector<std::pair<std::string, std::string>> config;
    
    // Metrics - flat structure for backward compatibility
    std::unordered_map<std::string, double> metrics;
    
    // Categorized metrics for extensibility
    std::unordered_map<std::string, std::unordered_map<std::string, double>> categorizedMetrics;
    
    // Results storage
    std::unordered_map<std::string, std::unique_ptr<ExperimentResultsBase>> results;
    
    // Notes and metadata
    std::vector<std::string> notes;
    
    // Progress tracking
    size_t totalSteps;
    size_t completedSteps;
    
    Impl() : seed(0), progress(0.0f), totalSteps(0), completedSteps(0) {
        auto now = std::chrono::system_clock::now();
        auto time = std::chrono::system_clock::to_time_t(now);
        std::stringstream ss;
        ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
        timestamp = ss.str();
        status = "Initialized";
    }
};

ExperimentBase::ExperimentBase(const std::string& name) {
    initializeImpl(name);
}

void ExperimentBase::initializeImpl(const std::string& name) {
    pImpl = std::make_unique<Impl>();
    pImpl->name = name;
    pImpl->status = "Created";
}

bool ExperimentBase::initializeBrain() {
    // Default implementation - can be overridden
    pImpl->status = "Brain initialized";
    return true;
}

bool ExperimentBase::setupBrainFromConfig(Brain* brain) const {
    // Default implementation - can be overridden
    return true;
}

void ExperimentBase::setConfigValue(const std::string& key, const std::string& value) {
    for (auto& pair : pImpl->config) {
        if (pair.first == key) {
            pair.second = value;
            return;
        }
    }
    pImpl->config.emplace_back(key, value);
}

std::string ExperimentBase::getConfigValue(const std::string& key) const {
    for (const auto& pair : pImpl->config) {
        if (pair.first == key) {
            return pair.second;
        }
    }
    return "";
}

void ExperimentBase::loadConfig(const std::string& filename) {
    // Load configuration from file
    pImpl->status = "Config loaded";
}

bool ExperimentBase::saveConfig(const std::string& filename) const {
    // Save configuration to file
    return true;
}

void ExperimentBase::validateConfig() const {
    // Validate configuration
    pImpl->status = "Config validated";
}

template<typename T>
void ExperimentBase::recordMetric(const std::string& name, const T& value) {
    pImpl->metrics[name] = static_cast<double>(value);
    pImpl->status = "Metric recorded";
}

template<typename T>
void ExperimentBase::recordMetric(const std::string& category, const std::string& name, const T& value) {
    pImpl->categorizedMetrics[category][name] = static_cast<double>(value);
    pImpl->status = "Categorized metric recorded";
}

double ExperimentBase::getMetric(const std::string& name) const {
    auto it = pImpl->metrics.find(name);
    if (it != pImpl->metrics.end()) {
        return it->second;
    }
    
    // Search categorized metrics
    for (const auto& category : pImpl->categorizedMetrics) {
        auto metricIt = category.second.find(name);
        if (metricIt != category.second.end()) {
            return metricIt->second;
        }
    }
    
    return 0.0;
}

double ExperimentBase::getMetric(const std::string& category, const std::string& name) const {
    auto catIt = pImpl->categorizedMetrics.find(category);
    if (catIt != pImpl->categorizedMetrics.end()) {
        auto metricIt = catIt->second.find(name);
        if (metricIt != catIt->second.end()) {
            return metricIt->second;
        }
    }
    return 0.0;
}

bool ExperimentBase::hasMetric(const std::string& name) const {
    if (pImpl->metrics.find(name) != pImpl->metrics.end()) {
        return true;
    }
    
    for (const auto& category : pImpl->categorizedMetrics) {
        if (category.second.find(name) != category.second.end()) {
            return true;
        }
    }
    return false;
}

bool ExperimentBase::hasMetric(const std::string& category, const std::string& name) const {
    auto catIt = pImpl->categorizedMetrics.find(category);
    if (catIt != pImpl->categorizedMetrics.end()) {
        return catIt->second.find(name) != catIt->second.end();
    }
    return false;
}

const std::unordered_map<std::string, double>& ExperimentBase::getAllMetrics() const {
    return pImpl->metrics;
}

const std::unordered_map<std::string, std::unordered_map<std::string, double>>& ExperimentBase::getAllCategorizedMetrics() const {
    return pImpl->categorizedMetrics;
}

template<typename TResult>
void ExperimentBase::storeResult(const std::string& identifier, const TResult& result) {
    pImpl->results[identifier] = std::make_unique<TResult>(result);
    pImpl->status = "Result stored";
}

template<typename TResult>
const TResult& ExperimentBase::getResult(const std::string& identifier) const {
    auto it = pImpl->results.find(identifier);
    if (it != pImpl->results.end()) {
        return static_cast<const TResult&>(*it->second);
    }
    throw std::runtime_error("Result not found");
}

std::string ExperimentBase::toCSV() const {
    std::ostringstream oss;
    oss << "ExperimentName,Seed,Timestamp,Progress,Status\n";
    oss << pImpl->name << "," << pImpl->seed << "," << pImpl->timestamp 
        << "," << pImpl->progress << "," << pImpl->status << "\n\n";
    
    if (!pImpl->metrics.empty()) {
        oss << "Metrics:\n";
        for (const auto& pair : pImpl->metrics) {
            oss << pair.first << "," << pair.second << "\n";
        }
        oss << "\n";
    }
    
    return oss.str();
}

std::string ExperimentBase::toJSON() const {
    std::ostringstream oss;
    oss << "{\n";
    oss << "  \"experimentName\": \"" << pImpl->name << "\",\n";
    oss << "  \"seed\": " << pImpl->seed << ",\n";
    oss << "  \"timestamp\": \"" << pImpl->timestamp << "\",\n";
    oss << "  \"progress\": " << pImpl->progress << ",\n";
    oss << "  \"status\": \"" << pImpl->status << "\",\n";
    
    if (!pImpl->metrics.empty()) {
        oss << "  \"metrics\": {\n";
        bool first = true;
        for (const auto& pair : pImpl->metrics) {
            if (!first) oss << ",\n";
            oss << "    \"" << pair.first << "\": " << pair.second;
            first = false;
        }
        oss << "\n  }\n";
    }
    
    oss << "}\n";
    return oss.str();
}

const std::string& ExperimentBase::getName() const { return pImpl->name; }
void ExperimentBase::setName(const std::string& name) { pImpl->name = name; }
uint64_t ExperimentBase::getSeed() const { return pImpl->seed; }
void ExperimentBase::setSeed(uint64_t seed) { pImpl->seed = seed; }
std::string ExperimentBase::getTimestamp() const { return pImpl->timestamp; }
float ExperimentBase::getProgress() const { return pImpl->progress; }
void ExperimentBase::setProgress(float progress) { pImpl->progress = progress; }
std::string ExperimentBase::getStatus() const { return pImpl->status; }
void ExperimentBase::setStatus(const std::string& status) { pImpl->status = status; }

void ExperimentBase::addNote(const std::string& note) { pImpl->notes.push_back(note); }
const std::vector<std::string>& ExperimentBase::getNotes() const { return pImpl->notes; }

} // namespace nlm
