#include "ExperimentEnhanced.hpp"
#include <algorithm>
#include <cmath>
#include <stdexcept>

namespace nlm {

struct Experiment::Impl {
    std::string name;
    uint64_t seed;
    std::vector<std::pair<std::string, std::string>> stringConfig;
    std::unordered_map<std::string, ExperimentParameter> parameters;
    std::vector<ParameterDefinition> parameterDefinitions;
    std::vector<std::pair<std::string, double>> metrics;
    std::vector<StepData> stepData;
    SimulationStep startStep;
    SimulationStep endStep;
    std::vector<std::string> notes;
    std::string description;
    std::string version;
    
    Impl() : seed(0), startStep(0), endStep(0) {}
};

Experiment::Experiment() : pImpl(new Impl) {}

Experiment::Experiment(const std::string& name) : pImpl(new Impl) {
    pImpl->name = name;
}

Experiment::~Experiment() = default;

Experiment::Experiment(Experiment&& other) noexcept : pImpl(std::move(other.pImpl)) {}

Experiment& Experiment::operator=(Experiment&& other) noexcept {
    if (this != &other) {
        pImpl = std::move(other.pImpl);
    }
    return *this;
}

const std::string& Experiment::getName() const {
    return pImpl->name;
}

void Experiment::setName(const std::string& name) {
    pImpl->name = name;
}

uint64_t Experiment::getSeed() const {
    return pImpl->seed;
}

void Experiment::setSeed(uint64_t seed) {
    pImpl->seed = seed;
}

void Experiment::setConfigValue(const std::string& key, const std::string& value) {
    for (auto& pair : pImpl->stringConfig) {
        if (pair.first == key) {
            pair.second = value;
            return;
        }
    }
    pImpl->stringConfig.emplace_back(key, value);
}

std::string Experiment::getConfigValue(const std::string& key) const {
    for (const auto& pair : pImpl->stringConfig) {
        if (pair.first == key) {
            return pair.second;
        }
    }
    return "";
}

// Template specializations for parameter setting
namespace {
template<typename T>
void setParameterImpl(Experiment::Impl& impl, const std::string& key, T value) {
    impl.parameters[key] = value;
}

void setParameterImpl(Experiment::Impl& impl, const std::string& key, const std::string& value) {
    impl.parameters[key] = value;
}
}

template<typename T>
void Experiment::setParameter(const std::string& key, T value) {
    setParameterImpl(*pImpl, key, value);
}

// String overload
template void Experiment::setParameter<std::string>(const std::string& key, std::string value);

// Template specializations for parameter getting
namespace {
template<typename T>
T getParameterImpl(const Experiment::Impl& impl, const std::string& key) {
    auto it = impl.parameters.find(key);
    if (it == impl.parameters.end()) {
        throw std::runtime_error("Parameter not found: " + key);
    }
    return std::get<T>(it->second);
}

std::string getParameterImpl(const Experiment::Impl& impl, const std::string& key) {
    auto it = impl.parameters.find(key);
    if (it == impl.parameters.end()) {
        throw std::runtime_error("Parameter not found: " + key);
    }
    return std::get<std::string>(it->second);
}
}

template<typename T>
T Experiment::getParameter(const std::string& key) const {
    return getParameterImpl<T>(*pImpl, key);
}

// String overloads
void Experiment::setParameter(const std::string& key, const std::string& value);
std::string Experiment::getParameter(const std::string& key) const;

bool Experiment::hasParameter(const std::string& key) const {
    return pImpl->parameters.find(key) != pImpl->parameters.end();
}

void Experiment::addParameterDefinition(const ParameterDefinition& param) {
    pImpl->parameterDefinitions.push_back(param);
}

const ParameterDefinition* Experiment::getParameterDefinition(const std::string& key) const {
    for (const auto& param : pImpl->parameterDefinitions) {
        if (param.name == key) {
            return &param;
        }
    }
    return nullptr;
}

bool Experiment::validateParameters() const {
    for (const auto& param : pImpl->parameterDefinitions) {
        if (param.required && !hasParameter(param.name)) {
            return false;
        }
        
        if (hasParameter(param.name)) {
            auto it = pImpl->parameters.find(param.name);
            const auto& value = it->second;
            
            // Check allowed values if specified
            if (!param.allowedValues.empty()) {
                bool found = false;
                for (const auto& allowed : param.allowedValues) {
                    if (value == allowed) {
                        found = true;
                        break;
                    }
                }
                if (!found) {
                    return false;
                }
            }
        }
    }
    return true;
}

void Experiment::recordMetric(const std::string& name, double value) {
    for (auto& pair : pImpl->metrics) {
        if (pair.first == name) {
            pair.second = value;
            return;
        }
    }
    pImpl->metrics.emplace_back(name, value);
}

void Experiment::recordMetric(const std::string& name, float value) {
    recordMetric(name, static_cast<double>(value));
}

void Experiment::recordMetric(const std::string& name, int value) {
    recordMetric(name, static_cast<double>(value));
}

void Experiment::recordMetric(const std::string& name, int64_t value) {
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

const std::string& Experiment::getDescription() const {
    return pImpl->description;
}

void Experiment::setDescription(const std::string& description) {
    pImpl->description = description;
}

const std::string& Experiment::getVersion() const {
    return pImpl->version;
}

void Experiment::setVersion(const std::string& version) {
    pImpl->version = version;
}

size_t Experiment::getStepCount() const {
    return pImpl->stepData.size();
}

size_t Experiment::getMetricCount() const {
    return pImpl->metrics.size();
}

double Experiment::getMeanMetric(const std::string& name) const {
    if (!hasMetric(name)) {
        return 0.0;
    }
    
    double sum = 0.0;
    size_t count = 0;
    for (const auto& pair : pImpl->stepData) {
        // Simplified - in practice you might want more sophisticated metric tracking
        sum += pair.avgFiringRate;
        ++count;
    }
    
    return count > 0 ? sum / count : 0.0;
}

double Experiment::getStdDevMetric(const std::string& name) const {
    if (!hasMetric(name)) {
        return 0.0;
    }
    
    // Simplified calculation
    double mean = getMeanMetric(name);
    double sumSquares = 0.0;
    size_t count = 0;
    for (const auto& pair : pImpl->stepData) {
        sumSquares += std::pow(pair.avgFiringRate - mean, 2);
        ++count;
    }
    
    return count > 0 ? std::sqrt(sumSquares / count) : 0.0;
}

} // namespace nlm
