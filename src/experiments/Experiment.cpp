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

Experiment::Experiment() : ExperimentBase(), pImpl(new Impl()) {
    // Seed experiment with base class initialization
    std::random_device rd;
    seed = rd() ^ (std::chrono::steady_clock::now().time_since_epoch().count() << 32);
}

Experiment::Experiment(const std::string& name) : ExperimentBase(), pImpl(new Impl()) {
    // Seed experiment with base class initialization
    std::random_device rd;
    seed = rd() ^ (std::chrono::steady_clock::now().time_since_epoch().count() << 32);
    this->name = name;
}

Experiment::~Experiment() = default;

const std::string& Experiment::getName() const {
    // Call base class getter first, then override if needed
    return name.empty() ? "Experiment" : name;
}

void Experiment::setName(const std::string& name) {
    this->name = name;
    // Call base class setter if needed
    ExperimentBase::setName(name);
}

uint64_t Experiment::getSeed() const {
    return pImpl->seed;
}

void Experiment::setSeed(uint64_t seed) {
    pImpl->seed = seed;
}

void Experiment::setConfigValue(const std::string& key, const std::string& value) {
    // Use base class implementation
    for (auto& pair : pImpl->config) {
        if (pair.first == key) {
            pair.second = value;
            return;
        }
    }
    pImpl->config.emplace_back(key, value);
}

std::string Experiment::getConfigValue(const std::string& key) const {
    // Use base class implementation
    for (const auto& pair : pImpl->config) {
        if (pair.first == key) {
            return pair.second;
        }
    }
    return "";
}

void Experiment::recordMetric(const std::string& name, double value) {
    // Call base class implementation
    ExperimentBase::recordMetric(name, value);
}

void Experiment::recordMetric(const std::string& name, float value) {
    // Call base class implementation
    ExperimentBase::recordMetric(name, static_cast<double>(value));
}

void Experiment::recordMetric(const std::string& name, int value) {
    // Call base class implementation
    ExperimentBase::recordMetric(name, static_cast<double>(value));
}

const std::vector<std::pair<std::string, double>>& Experiment::getMetrics() const {
    return pImpl->metrics;
}

double Experiment::getMetric(const std::string& name) const {
    return ExperimentBase::getMetric(name);
}

bool Experiment::hasMetric(const std::string& name) const {
    return ExperimentBase::hasMetric(name);
}

void Experiment::recordStep(SimulationStep step, size_t neuronCount, size_t synapseCount,
                           size_t firingCount, float avgFiringRate) {
    // Call base class implementation with enhanced data
    ExperimentBase::recordStep(step, neuronCount, synapseCount, firingCount, avgFiringRate);
    
    // Store additional data in pImpl for backward compatibility
    pImpl->stepData.push_back({
        step, neuronCount, synapseCount, firingCount, avgFiringRate
    });
}

const std::vector<Experiment::StepData>& Experiment::getStepData() const {
    // Return the original StepData structure for backward compatibility
    // The step data from base class is also available via getStepData() from ExperimentBase
    return pImpl->stepData;
}

SimulationStep Experiment::getStartStep() const {
    return pImpl->startStep;
}

void Experiment::setStartStep(SimulationStep step) {
    pImpl->startStep = step;
    ExperimentBase::setStartStep(step);
}

SimulationStep Experiment::getEndStep() const {
    return pImpl->endStep;
}

void Experiment::setEndStep(SimulationStep step) {
    pImpl->endStep = step;
    ExperimentBase::setEndStep(step);
}

void Experiment::addNote(const std::string& note) {
    pImpl->notes.push_back(note);
    ExperimentBase::addNote(note);
}

const std::vector<std::string>& Experiment::getNotes() const {
    return pImpl->notes;
}

} // namespace nlm
