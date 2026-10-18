#include "Experiment.hpp"
#include <algorithm>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <numeric>
#include <cmath>
#include <functional>

namespace nlm {

struct Experiment::Impl {
    std::string name;
    uint64_t seed;
    std::vector<std::pair<std::string, std::string>> config;
    std::vector<std::pair<std::string, double>> metrics;
    std::vector<StepData> stepData;
    SimulationStep startStep;
    SimulationStep endStep;
    std::vector<std::string> notes;
    
    Impl() : seed(0), startStep(0), endStep(0) {}
};

Experiment::Experiment() : pImpl(new Impl) {
    creationTime_ = std::chrono::system_clock::now();
    lastModifiedTime_ = creationTime_;
}

Experiment::Experiment(const std::string& name) : pImpl(new Impl) {
    pImpl->name = name;
    creationTime_ = std::chrono::system_clock::now();
    lastModifiedTime_ = creationTime_;
}

Experiment::~Experiment() = default;

const std::string& Experiment::getName() const {
    return pImpl->name;
}

void Experiment::setName(const std::string& name) {
    pImpl->name = name;
    lastModifiedTime_ = std::chrono::system_clock::now();
}

uint64_t Experiment::getSeed() const {
    return pImpl->seed;
}

void Experiment::setSeed(uint64_t seed) {
    pImpl->seed = seed;
    lastModifiedTime_ = std::chrono::system_clock::now();
}

void Experiment::setConfigValue(const std::string& key, const std::string& value) {
    for (auto& pair : pImpl->config) {
        if (pair.first == key) {
            pair.second = value;
            return;
        }
    }
    pImpl->config.emplace_back(key, value);
    lastModifiedTime_ = std::chrono::system_clock::now();
}

std::string Experiment::getConfigValue(const std::string& key) const {
    for (const auto& pair : pImpl->config) {
        if (pair.first == key) {
            return pair.second;
        }
    }
    return "";
}

void Experiment::recordMetric(const std::string& name, double value) {
    for (auto& pair : pImpl->metrics) {
        if (pair.first == name) {
            pair.second = value;
            return;
        }
    }
    pImpl->metrics.emplace_back(name, value);
    lastModifiedTime_ = std::chrono::system_clock::now();
}

void Experiment::recordMetric(const std::string& name, float value) {
    recordMetric(name, static_cast<double>(value));
}

void Experiment::recordMetric(const std::string& name, int value) {
    recordMetric(name, static_cast<double>(value));
}

const std::vector<std::pair<std::string, double>>& Experiment::getMetrics() const {
    return pImpl->metrics;
}

double Experiment::getMetric(const std::string& name) const {
    for (const auto& pair : pImpl->metrics) {
        if (pair.first == name) {
            return pair.second;
        }
    }
    return 0.0;
}

bool Experiment::hasMetric(const std::string& name) const {
    for (const auto& pair : pImpl->metrics) {
        if (pair.first == name) {
            return true;
        }
    }
    return false;
}

void Experiment::recordStep(SimulationStep step, size_t neuronCount, size_t synapseCount,
                           size_t firingCount, float avgFiringRate) {
    StepData data;
    data.step = step;
    data.neuronCount = neuronCount;
    data.synapseCount = synapseCount;
    data.firingCount = firingCount;
    data.avgFiringRate = avgFiringRate;
    pImpl->stepData.push_back(data);
    lastModifiedTime_ = std::chrono::system_clock::now();
}

const std::vector<Experiment::StepData>& Experiment::getStepData() const {
    return pImpl->stepData;
}

SimulationStep Experiment::getStartStep() const {
    return pImpl->startStep;
}

void Experiment::setStartStep(SimulationStep step) {
    pImpl->startStep = step;
}

SimulationStep Experiment::getEndStep() const {
    return pImpl->endStep;
}

void Experiment::setEndStep(SimulationStep step) {
    pImpl->endStep = step;
}

void Experiment::addNote(const std::string& note) {
    pImpl->notes.push_back(note);
}

const std::vector<std::string>& Experiment::getNotes() const {
    return pImpl->notes;
}

// ========== CLONING AND STATE MANAGEMENT ==========

std::unique_ptr<Experiment> Experiment::clone() const {
    auto cloned = std::make_unique<Experiment>(pImpl->name);
    cloned->setSeed(pImpl->seed);
    
    // Copy configuration
    for (const auto& pair : pImpl->config) {
        cloned->setConfigValue(pair.first, pair.second);
    }
    
    // Copy metrics
    for (const auto& pair : pImpl->metrics) {
        cloned->recordMetric(pair.first, pair.second);
    }
    
    // Copy step data
    for (const auto& step : pImpl->stepData) {
        cloned->recordStep(step.step, step.neuronCount, step.synapseCount,
                          step.firingCount, step.avgFiringRate);
    }
    
    // Copy notes
    for (const auto& note : pImpl->notes) {
        cloned->addNote(note);
    }
    
    // Copy timing information
    cloned->setStartStep(pImpl->startStep);
    cloned->setEndStep(pImpl->endStep);
    
    return cloned;
}

bool Experiment::save(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    file << "Experiment:" << std::endl;
    file << "  Name: " << pImpl->name << std::endl;
    file << "  Seed: " << pImpl->seed << std::endl;
    file << "  StartStep: " << pImpl->startStep << std::endl;
    file << "  EndStep: " << pImpl->endStep << std::endl;
    file << "  CreationTime: " << std::chrono::system_clock::to_time_t(creationTime_) << std::endl;
    file << "  LastModifiedTime: " << std::chrono::system_clock::to_time_t(lastModifiedTime_) << std::endl;
    file << "  Metrics:" << std::endl;
    
    for (const auto& pair : pImpl->metrics) {
        file << "    " << pair.first << ": " << pair.second << std::endl;
    }
    
    file << "  StepData:" << std::endl;
    for (const auto& step : pImpl->stepData) {
        file << "    Step " << step.step << ": "
             << "neurons=" << step.neuronCount << ", "
             << "synapses=" << step.synapseCount << ", "
             << "firing=" << step.firingCount << ", "
             << "firingRate=" << step.avgFiringRate << std::endl;
    }
    
    file << "  Notes:" << std::endl;
    for (const auto& note : pImpl->notes) {
        file << "    " << note << std::endl;
    }
    
    file << "  Config:" << std::endl;
    for (const auto& pair : pImpl->config) {
        file << "    " << pair.first << ": " << pair.second << std::endl;
    }
    
    return true;
}

bool Experiment::load(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    std::string line;
    std::string currentSection;
    std::string name, key, value;
    
    while (std::getline(file, line)) {
        line.erase(0, line.find_first_not_of(" \t\r\n"));
        line.erase(line.find_last_not_of(" \t\r\n") + 1);
        
        if (line.empty() || line[0] == '#') {
            continue;
        }
        
        if (line.find(":") != std::string::npos) {
            size_t colonPos = line.find(":");
            std::string section = line.substr(0, colonPos);
            section.erase(0, section.find_first_not_of(" \t\r\n"));
            section.erase(section.find_last_not_of(" \t\r\n") + 1);
            
            currentSection = section;
            
            if (section == "StepData") {
                // Reset step data
                pImpl->stepData.clear();
            }
        } else {
            if (currentSection == "Metrics") {
                size_t colonPos = line.find(":");
                if (colonPos != std::string::npos) {
                    key = line.substr(0, colonPos);
                    key.erase(0, key.find_first_not_of(" \t\r\n"));
                    key.erase(key.find_last_not_of(" \t\r\n") + 1);
                    value = line.substr(colonPos + 1);
                    value.erase(0, value.find_first_not_of(" \t\r\n"));
                    value.erase(value.find_last_not_of(" \t\r\n") + 1);
                    
                    try {
                        double metricValue = std::stod(value);
                        recordMetric(key, metricValue);
                    } catch (...) {
                        // Skip invalid metric
                    }
                }
            } else if (currentSection == "StepData") {
                if (line.find("Step ") == 0) {
                    StepData stepData;
                    std::string dataLine = line.substr(5); // Remove "Step "
                    
                    // Parse step data
                    size_t pos = 0;
                    std::string token;
                    int fieldNum = 0;
                    
                    while ((pos = dataLine.find(",")) != std::string::npos) {
                        token = dataLine.substr(0, pos);
                        token.erase(0, token.find_first_not_of(" \t\r\n"));
                        token.erase(token.find_last_not_of(" \t\r\n") + 1);
                        
                        if (token.find("neurons=") == 0) {
                            stepData.neuronCount = std::stoi(token.substr(8));
                        } else if (token.find("synapses=") == 0) {
                            stepData.synapseCount = std::stoi(token.substr(9));
                        } else if (token.find("firing=") == 0) {
                            stepData.firingCount = std::stoi(token.substr(7));
                        } else if (token.find("firingRate=") == 0) {
                            stepData.avgFiringRate = std::stof(token.substr(10));
                        }
                        
                        dataLine.erase(0, pos + 1);
                        fieldNum++;
                    }
                    
                    // Set step number from the beginning of line
                    std::stringstream ss(line);
                    std::string temp;
                    ss >> temp >> stepData.step;
                    
                    recordStep(stepData.step, stepData.neuronCount, stepData.synapseCount,
                              stepData.firingCount, stepData.avgFiringRate);
                }
            } else if (currentSection == "Notes") {
                // Remove leading spaces and add note
                addNote(line);
            } else if (currentSection == "Config") {
                size_t colonPos = line.find(":");
                if (colonPos != std::string::npos) {
                    key = line.substr(0, colonPos);
                    key.erase(0, key.find_first_not_of(" \t\r\n"));
                    key.erase(key.find_last_not_of(" \t\r\n") + 1);
                    value = line.substr(colonPos + 1);
                    value.erase(0, value.find_first_not_of(" \t\r\n"));
                    value.erase(value.find_last_not_of(" \t\r\n") + 1);
                    
                    setConfigValue(key, value);
                }
            } else if (currentSection.empty()) {
                // Top-level key-value pairs
                size_t colonPos = line.find(":");
                if (colonPos != std::string::npos) {
                    key = line.substr(0, colonPos);
                    key.erase(0, key.find_first_not_of(" \t\r\n"));
                    key.erase(key.find_last_not_of(" \t\r\n") + 1);
                    value = line.substr(colonPos + 1);
                    value.erase(0, value.find_first_not_of(" \t\r\n"));
                    value.erase(value.find_last_not_of(" \t\r\n") + 1);
                    
                    if (key == "Name") {
                        setName(value);
                    } else if (key == "Seed") {
                        setSeed(std::stoull(value));
                    } else if (key == "StartStep") {
                        setStartStep(std::stoll(value));
                    } else if (key == "EndStep") {
                        setEndStep(std::stoll(value));
                    }
                }
            }
        }
    }
    
    lastModifiedTime_ = std::chrono::system_clock::now();
    return true;
}

bool Experiment::compareWith(const Experiment& other) const {
    if (pImpl->name != other.pImpl->name) return false;
    if (pImpl->seed != other.pImpl->seed) return false;
    if (pImpl->startStep != other.pImpl->startStep) return false;
    if (pImpl->endStep != other.pImpl->endStep) return false;
    
    // Compare metrics
    if (pImpl->metrics.size() != other.pImpl->metrics.size()) return false;
    for (size_t i = 0; i < pImpl->metrics.size(); ++i) {
        if (pImpl->metrics[i].first != other.pImpl->metrics[i].first) return false;
        if (pImpl->metrics[i].second != other.pImpl->metrics[i].second) return false;
    }
    
    // Compare step data
    if (pImpl->stepData.size() != other.pImpl->stepData.size()) return false;
    for (size_t i = 0; i < pImpl->stepData.size(); ++i) {
        if (pImpl->stepData[i].step != other.pImpl->stepData[i].step) return false;
        if (pImpl->stepData[i].neuronCount != other.pImpl->stepData[i].neuronCount) return false;
        if (pImpl->stepData[i].synapseCount != other.pImpl->stepData[i].synapseCount) return false;
        if (pImpl->stepData[i].firingCount != other.pImpl->stepData[i].firingCount) return false;
        if (pImpl->stepData[i].avgFiringRate != other.pImpl->stepData[i].avgFiringRate) return false;
    }
    
    return true;
}

size_t Experiment::getHash() const {
    std::hash<std::string> hasher;
    std::hash<uint64_t> uintHasher;
    std::hash<SimulationStep> stepHasher;
    
    size_t hash = hasher(pImpl->name);
    hash ^= uintHasher(pImpl->seed);
    hash ^= stepHasher(pImpl->startStep);
    hash ^= stepHasher(pImpl->endStep);
    
    for (const auto& pair : pImpl->metrics) {
        size_t metricHash = hasher(pair.first) ^ std::hash<double>()(pair.second);
        hash ^= metricHash;
    }
    
    for (const auto& step : pImpl->stepData) {
        size_t stepHash = stepHasher(step.step);
        hash ^= stepHash;
    }
    
    return hash;
}

// ========== ADVANCED FEATURES ==========

Experiment::MetricsSummary Experiment::calculateMetricsSummary() const {
    MetricsSummary summary;
    
    if (pImpl->stepData.empty()) {
        return summary;
    }
    
    double totalNeurons = 0.0, totalSynapses = 0.0, totalFiring = 0.0, totalFiringRate = 0.0;
    size_t stepCount = pImpl->stepData.size();
    
    for (const auto& step : pImpl->stepData) {
        totalNeurons += step.neuronCount;
        totalSynapses += step.synapseCount;
        totalFiring += step.firingCount;
        totalFiringRate += step.avgFiringRate;
    }
    
    summary.averageStepDuration = (pImpl->endStep - pImpl->startStep) / static_cast<double>(stepCount);
    summary.maxFiringRate = 0.0;
    for (const auto& step : pImpl->stepData) {
        if (step.avgFiringRate > summary.maxFiringRate) {
            summary.maxFiringRate = step.avgFiringRate;
        }
    }
    summary.totalNeurons = totalNeurons / stepCount;
    summary.totalSynapses = totalSynapses / stepCount;
    summary.totalReward = getMetric("total_reward");
    summary.efficiencyScore = (summary.totalNeurons > 0) ? (summary.totalReward / summary.totalNeurons) : 0.0;
    
    return summary;
}

std::vector<Experiment::StepData> Experiment::filterSteps(std::function<bool(const StepData&)> predicate) const {
    std::vector<StepData> filtered;
    for (const auto& step : pImpl->stepData) {
        if (predicate(step)) {
            filtered.push_back(step);
        }
    }
    return filtered;
}

Experiment::StepStatistics Experiment::getStepStatistics() const {
    StepStatistics stats;
    
    if (pImpl->stepData.empty()) {
        return stats;
    }
    
    double totalNeurons = 0.0, totalSynapses = 0.0, totalFiring = 0.0, totalFiringRate = 0.0;
    size_t stepCount = pImpl->stepData.size();
    
    for (const auto& step : pImpl->stepData) {
        totalNeurons += step.neuronCount;
        totalSynapses += step.synapseCount;
        totalFiring += step.firingCount;
        totalFiringRate += step.avgFiringRate;
    }
    
    stats.avgNeurons = totalNeurons / stepCount;
    stats.avgSynapses = totalSynapses / stepCount;
    stats.avgFiringCount = totalFiring / stepCount;
    stats.avgFiringRate = totalFiringRate / stepCount;
    
    return stats;
}

bool Experiment::hasData() const {
    return !pImpl->metrics.empty() || !pImpl->stepData.empty() || !pImpl->notes.empty();
}

size_t Experiment::getMemoryUsage() const {
    size_t totalSize = sizeof(Experiment) + sizeof(Impl);
    totalSize += pImpl->name.capacity();
    totalSize += pImpl->config.size() * sizeof(std::pair<std::string, std::string>);
    totalSize += pImpl->metrics.size() * sizeof(std::pair<std::string, double>);
    totalSize += pImpl->stepData.size() * sizeof(StepData);
    totalSize += pImpl->notes.size() * sizeof(std::string);
    return totalSize;
}

// ========== STATIC UTILITY FUNCTIONS ==========

std::unique_ptr<Experiment> Experiment::createFromConfig(const std::string& configJson) {
    // In real implementation, would parse JSON config
    auto experiment = std::make_unique<Experiment>("FromConfig");
    experiment->setSeed(42);
    experiment->setConfigValue("config", configJson);
    return experiment;
}

std::unique_ptr<Experiment> Experiment::createDefault(const std::string& name) {
    auto experiment = std::make_unique<Experiment>(name);
    experiment->setSeed(42);
    experiment->setConfigValue("mode", "default");
    return experiment;
}

} // namespace nlm
