#include "Experiment.hpp"
#include <algorithm>
#include <sstream>
#include <memory>

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
    
    // NEW: Plugin system
    std::unique_ptr<PluginManager> pluginManager;
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

// EXTENSIBILITY IMPLEMENTATION
void Experiment::setPluginManager(std::unique_ptr<PluginManager> manager) {
    pImpl->pluginManager = std::move(manager);
    if (pImpl->pluginManager) {
        pImpl->pluginManager->setCurrentExperiment(*this);
    }
}

Experiment::PluginManager* Experiment::getPluginManager() const {
    return pImpl->pluginManager.get();
}

void Experiment::registerPluginFactory(std::unique_ptr<PluginFactory> factory) {
    if (pImpl->pluginManager) {
        pImpl->pluginManager->registerPluginFactory(std::move(factory));
    }
}

std::unique_ptr<Experiment::ExperimentPlugin> Experiment::createPlugin(
    const std::string& type,
    const std::vector<float>& parameters) {
    if (pImpl->pluginManager) {
        return pImpl->pluginManager->createPlugin(type, parameters);
    }
    return nullptr;
}

std::vector<Experiment::ExperimentPlugin*> Experiment::getPlugins() const {
    if (pImpl->pluginManager) {
        return pImpl->pluginManager->getPlugins();
    }
    return std::vector<Experiment::ExperimentPlugin*>();
}

bool Experiment::setPluginActive(const std::string& pluginName, bool active) {
    if (pImpl->pluginManager) {
        return pImpl->pluginManager->setPluginActive(pluginName, active);
    }
    return false;
}

bool Experiment::removePlugin(const std::string& pluginName) {
    if (pImpl->pluginManager) {
        return pImpl->pluginManager->removePlugin(pluginName);
    }
    return false;
}

bool Experiment::loadPluginsFromFile(const std::string& filepath) {
    if (pImpl->pluginManager) {
        // This would require a specific PluginLoader implementation
        // For now, return false as it depends on the implementation
        return false;
    }
    return false;
}

bool Experiment::loadPluginsFromDirectory(const std::string& directory) {
    if (pImpl->pluginManager) {
        // This would require a specific PluginLoader implementation
        // For now, return false as it depends on the implementation
        return false;
    }
    return false;
}

void Experiment::processPlugins() {
    if (pImpl->pluginManager) {
        // Process all active plugins
        auto plugins = pImpl->pluginManager->getPlugins();
        for (auto* plugin : plugins) {
            if (plugin->isActive()) {
                plugin->process();
            }
        }
    }
}

std::string Experiment::analyzeResults() const {
    std::stringstream ss;
    ss << "Experiment Analysis:" << std::endl;
    ss << "Name: " << pImpl->name << std::endl;
    ss << "Metrics: " << pImpl->metrics.size() << std::endl;
    ss << "Steps: " << pImpl->stepData.size() << std::endl;
    return ss.str();
}

bool Experiment::exportData(const std::string& filepath) const {
    // Basic export implementation
    std::ofstream file(filepath);
    if (!file) {
        return false;
    }
    
    file << "Experiment: " << pImpl->name << std::endl;
    file << "Seed: " << pImpl->seed << std::endl;
    file << "Configuration:" << std::endl;
    for (const auto& pair : pImpl->config) {
        file << "  " << pair.first << " = " << pair.second << std::endl;
    }
    file << "Metrics:" << std::endl;
    for (const auto& pair : pImpl->metrics) {
        file << "  " << pair.first << " = " << pair.second << std::endl;
    }
    
    return true;
}

bool Experiment::compareWith(const Experiment& other) const {
    // Basic comparison implementation
    return (pImpl->name == other.pImpl->name) && (pImpl->seed == other.pImpl->seed);
}

std::string Experiment::getSummary() const {
    std::stringstream ss;
    ss << "Experiment Summary:" << std::endl;
    ss << "Name: " << pImpl->name << std::endl;
    ss << "Seed: " << pImpl->seed << std::endl;
    ss << "Configuration entries: " << pImpl->config.size() << std::endl;
    ss << "Metric entries: " << pImpl->metrics.size() << std::endl;
    ss << "Step entries: " << pImpl->stepData.size() << std::endl;
    ss << "Notes: " << pImpl->notes.size() << std::endl;
    return ss.str();
}

} // namespace nlm
