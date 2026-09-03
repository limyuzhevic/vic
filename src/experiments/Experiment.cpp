#include "Experiment.hpp"

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

Experiment::Experiment() : pImpl(new Impl) {}

Experiment::Experiment(const std::string& name) : pImpl(new Impl) {
    pImpl->name = name;
}

Experiment::~Experiment() = default;

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
    for (auto& pair : pImpl->config) {
        if (pair.first == key) {
            pair.second = value;
            return;
        }
    }
    pImpl->config.emplace_back(key, value);
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

} // namespace nlm
