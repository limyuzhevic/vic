#include "Debugger.hpp"
#include <iostream>
#include <fstream>
#include <filesystem>
#include <algorithm>
#include <iomanip>
#include <sstream>

namespace nlm {

struct Debugger::Impl {
    std::map<VisualizationComponent, std::shared_ptr<VisualizationInterface>> components;
    std::map<VisualizationComponent, bool> componentEnabled;
    
    // References to NLM systems
    std::shared_ptr<Brain> brain;
    std::shared_ptr<SimulationClock> simulationClock;
    std::shared_ptr<Config> config;
    std::shared_ptr<Logger> logger;
    std::shared_ptr<Memory> memory;
    std::shared_ptr<NeuromodulationSystem> neuromodulation;
    std::shared_ptr<SpikeSystem> spikeSystem;
    
    // Debug state
    DebugState state;
    
    // Timing
    double updateRate = 30.0;
    double accumulatedTime = 0.0;
    VisualizationFormat outputFormat = VisualizationFormat::TEXT;
    
    // Thread safety
    mutable std::mutex mutex;
    
    Impl() {
        // Initialize all components as disabled by default
        for (int i = 0; i < static_cast<int>(VisualizationComponent::Weights); ++i) {
            VisualizationComponent type = static_cast<VisualizationComponent>(i);
            componentEnabled[type] = false;
        }
        
        state.currentStep = 0;
        state.currentTime = 0.0;
        state.simulationTime = 0.0;
    }
};

Debugger::Debugger() : pImpl(new Impl) {}

Debugger::~Debugger() = default;

bool Debugger::initialize(const std::vector<VisualizationComponent>& components) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    for (auto type : components) {
        addComponent(type, nullptr);
    }
    
    return true;
}

bool Debugger::addComponent(VisualizationComponent type, std::shared_ptr<VisualizationInterface> component) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    pImpl->components[type] = std::move(component);
    pImpl->componentEnabled[type] = true;
    
    return true;
}

void Debugger::removeComponent(VisualizationComponent type) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    pImpl->components.erase(type);
    pImpl->componentEnabled[type] = false;
}

std::shared_ptr<VisualizationInterface> Debugger::getComponent(VisualizationComponent type) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    auto it = pImpl->components.find(type);
    return (it != pImpl->components.end()) ? it->second : nullptr;
}

void Debugger::update(double deltaTime) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    // Update debug state
    if (pImpl->simulationClock) {
        pImpl->state.currentStep = pImpl->simulationClock->getCurrentStep();
        pImpl->state.currentTime = pImpl->simulationClock->getCurrentTime();
        pImpl->state.simulationTime = pImpl->simulationClock->getSimulationTime();
    }
    
    if (pImpl->brain) {
        pImpl->state.neuronCount = pImpl->brain->getTotalNeuronCount();
        pImpl->state.spikeCount = pImpl->brain->getTotalSpikeCount();
        pImpl->state.excitationInhibitionRatio = pImpl->brain->getExcitationInhibitionRatio();
        pImpl->state.averageFiringRate = pImpl->brain->getAverageFiringRate();
        pImpl->state.lastUpdate = std::chrono::system_clock::now();
    }
    
    // Update all components
    for (auto& pair : pImpl->components) {
        if (pImpl->componentEnabled[pair.first] && pair.second) {
            pair.second->update(deltaTime);
        }
    }
}

void Debugger::render() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    // Render all components
    for (auto& pair : pImpl->components) {
        if (pImpl->componentEnabled[pair.first] && pair.second) {
            pair.second->render();
        }
    }
}

void Debugger::close() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    for (auto& pair : pImpl->components) {
        if (pair.second) {
            pair.second->close();
        }
    }
    
    pImpl->components.clear();
}

void Debugger::setBrain(std::shared_ptr<Brain> brain) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->brain = brain;
}

void Debugger::setSimulationClock(std::shared_ptr<SimulationClock> clock) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->simulationClock = clock;
}

void Debugger::setConfig(std::shared_ptr<Config> config) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->config = config;
}

void Debugger::setLogger(std::shared_ptr<Logger> logger) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->logger = logger;
}

void Debugger::setMemory(std::shared_ptr<Memory> memory) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->memory = memory;
}

void Debugger::setNeuromodulation(std::shared_ptr<NeuromodulationSystem> neuromodulation) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->neuromodulation = neuromodulation;
}

void Debugger::setSpikeSystem(std::shared_ptr<SpikeSystem> spikeSystem) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->spikeSystem = spikeSystem;
}

bool Debugger::exportAll(const std::string& directory, VisualizationFormat format) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    std::filesystem::create_directories(directory);
    
    for (auto& pair : pImpl->components) {
        if (pImpl->componentEnabled[pair.first] && pair.second) {
            ExportOptions options;
            options.format = format;
            options.filename = directory + "/" + std::to_string(static_cast<int>(pair.first)) + ".txt";
            options.includeMetadata = true;
            
            pair.second->exportData(options);
        }
    }
    
    return true;
}

bool Debugger::importAll(const std::string& directory) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    for (auto& pair : pImpl->components) {
        if (pImpl->componentEnabled[pair.first] && pair.second) {
            pair.second->importData(directory + "/" + std::to_string(static_cast<int>(pair.first)) + ".txt");
        }
    }
    
    return true;
}

DebugState Debugger::getDebugState() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return pImpl->state;
}

std::string Debugger::getAllComponentData() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    std::stringstream ss;
    
    ss << "=== All Component Data ===" << std::endl;
    ss << "Debug State:" << std::endl;
    ss << "  Step: " << pImpl->state.currentStep << std::endl;
    ss << "  Time: " << pImpl->state.currentTime << "s" << std::endl;
    ss << "  Simulation Time: " << pImpl->state.simulationTime << "s" << std::endl;
    ss << "  Neurons: " << pImpl->state.neuronCount << std::endl;
    ss << "  Spikes: " << pImpl->state.spikeCount << std::endl;
    ss << "  E/I Ratio: " << pImpl->state.excitationInhibitionRatio << std::endl;
    ss << "  Avg Firing Rate: " << pImpl->state.averageFiringRate << " Hz" << std::endl;
    
    ss << std::endl << "Component Status:" << std::endl;
    for (const auto& pair : pImpl->componentEnabled) {
        ss << "  " << static_cast<int>(pair.first) << ": " << (pair.second ? "ENABLED" : "DISABLED") << std::endl;
    }
    
    ss << std::endl << "Component Data:" << std::endl;
    for (auto& pair : pImpl->components) {
        if (pImpl->componentEnabled[pair.first] && pair.second) {
            ss << "  Component " << static_cast<int>(pair.first) << ":" << std::endl;
            ss << pair.second->getCurrentFrame() << std::endl;
        }
    }
    
    return ss.str();
}

std::string Debugger::getStatusReport() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    std::stringstream ss;
    
    ss << "=== NLM Debugger Status Report ===" << std::endl;
    ss << "Generated: " << std::put_time(std::localtime(&std::chrono::system_clock::to_time_t(std::chrono::system_clock::now())), "%Y-%m-%d %H:%M:%S") << std::endl;
    ss << std::endl;
    
    ss << "System Health:" << std::endl;
    ss << "  Brain Active: " << (pImpl->brain ? "Yes" : "No") << std::endl;
    ss << "  Simulation Clock: " << (pImpl->simulationClock ? "Yes" : "No") << std::endl;
    ss << "  Config Loaded: " << (pImpl->config ? "Yes" : "No") << std::endl;
    ss << "  Logger: " << (pImpl->logger ? "Yes" : "No") << std::endl;
    ss << "  Memory System: " << (pImpl->memory ? "Yes" : "No") << std::endl;
    ss << "  Neuromodulation: " << (pImpl->neuromodulation ? "Yes" : "No") << std::endl;
    ss << "  Spike System: " << (pImpl->spikeSystem ? "Yes" : "No") << std::endl;
    ss << std::endl;
    
    ss << "Active Components:" << std::endl;
    for (const auto& pair : pImpl->componentEnabled) {
        if (pair.second) {
            ss << "  " << static_cast<int>(pair.first) << std::endl;
        }
    }
    
    ss << std::endl << "Performance:" << std::endl;
    ss << "  Update Rate: " << pImpl->updateRate << " Hz" << std::endl;
    
    return ss.str();
}

void Debugger::setComponentEnabled(VisualizationComponent type, bool enabled) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->componentEnabled[type] = enabled;
}

bool Debugger::isComponentEnabled(VisualizationComponent type) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return pImpl->componentEnabled[type];
}

void Debugger::setUpdateRate(double hz) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->updateRate = hz;
}

double Debugger::getUpdateRate() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return pImpl->updateRate;
}

void Debugger::setOutputFormat(VisualizationFormat format) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->outputFormat = format;
}

VisualizationFormat Debugger::getOutputFormat() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return pImpl->outputFormat;
}

} // namespace nlm
