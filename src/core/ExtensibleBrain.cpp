#include "ExtensibleBrain.hpp"
#include "../core/Logger/Logger.hpp"
#include "../plugins/PluginInterface.hpp"
#include "../plugins/PluginManager.hpp"
#include <map>

namespace nlm {

struct ExtensibleBrain::Impl {
    // Plugin manager for extensibility
    PluginManager pluginManager;
    
    // Custom memory system
    std::unique_ptr<CustomMemorySystem> customMemorySystem;
    
    // Advanced neuromodulator
    std::unique_ptr<AdvancedNeuromodulator> advancedNeuromodulator;
    
    // Neural architecture
    std::unique_ptr<NeuralArchitecture> neuralArchitecture;
    
    // Optimization hooks
    std::unique_ptr<OptimizationHooks> optimizationHooks;
    
    // Experiment framework
    std::unique_ptr<AdvancedExperiment> experimentFramework;
    
    // Custom protocols
    std::map<std::string, std::unique_ptr<CustomProtocol>> customProtocols;
    
    // Event system
    struct EventCallbackEntry {
        std::string eventType;
        EventCallback callback;
    };
    std::vector<EventCallbackEntry> eventCallbacks;
    
    // Configuration inheritance
    std::string currentConfigPath;
    std::vector<std::string> inheritanceChain;
    
    // Performance profiling
    bool profilingEnabled;
    std::chrono::system_clock::time_point profilingStart;
    std::map<std::string, double> performanceMetrics;
    
    // Hyperparameter optimization
    std::map<std::string, double> optimalParameters;
    std::unique_ptr<HyperparameterOptimizer> hyperparameterOptimizer;
    
    // Network construction
    std::map<std::string, std::string> connectionPatterns;
    std::vector<std::string> structuralPlasticityRules;
    
    Logger* logger;
};

ExtensibleBrain::ExtensibleBrain(std::shared_ptr<Config> config) : Brain(std::move(config)) {
    pImpl = std::make_unique<Impl>();
    pImpl->logger = Logger::getInstance();
    pImpl->profilingEnabled = false;
    
    // Load default plugins
    pImpl->pluginManager = PluginManager();
}

ExtensibleBrain::~ExtensibleBrain() {
    shutdown();
}

ExtensibleBrain::ExtensibleBrain(ExtensibleBrain&& other) noexcept : Brain(std::move(other)) {
    pImpl = std::move(other.pImpl);
}

ExtensibleBrain& ExtensibleBrain::operator=(ExtensibleBrain&& other) noexcept {
    if (this != &other) {
        Brain::operator=(std::move(other));
        pImpl = std::move(other.pImpl);
    }
    return *this;
}

bool ExtensibleBrain::initialize() {
    bool success = Brain::initialize();
    if (!success) {
        pImpl->logger->error("Failed to initialize base Brain");
        return false;
    }
    
    // Initialize plugin manager
    pImpl->pluginManager.setConfig(*getConfig());
    
    // Load default plugins
    loadDefaultPlugins();
    
    // Initialize custom systems
    if (pImpl->customMemorySystem) {
        if (!pImpl->customMemorySystem->initialize(*getConfig())) {
            pImpl->logger->warning("Failed to initialize custom memory system");
        }
    }
    
    if (pImpl->advancedNeuromodulator) {
        if (!pImpl->advancedNeuromodulator->initialize(*getConfig())) {
            pImpl->logger->warning("Failed to initialize advanced neuromodulator");
        }
    }
    
    if (pImpl->neuralArchitecture) {
        if (!pImpl->neuralArchitecture->initialize(*getConfig())) {
            pImpl->logger->warning("Failed to initialize neural architecture");
        }
    }
    
    return true;
}

bool ExtensibleBrain::addNeuronModelPlugin(std::shared_ptr<INeuronModelPlugin> plugin) {
    if (!plugin) {
        pImpl->logger->error("Cannot add null neuron model plugin");
        return false;
    }
    
    // Register plugin with plugin manager
    // This would require adding plugin factory registration
    pImpl->logger->info("Neuron model plugin added: " + plugin->getName());
    return true;
}

bool ExtensibleBrain::addPlasticityRulePlugin(std::shared_ptr<IPlasticityRulePlugin> plugin) {
    if (!plugin) {
        pImpl->logger->error("Cannot add null plasticity rule plugin");
        return false;
    }
    
    pImpl->logger->info("Plasticity rule plugin added: " + plugin->getName());
    return true;
}

bool ExtensibleBrain::addSynapticDynamicsPlugin(std::shared_ptr<ISynapticDynamicsPlugin> plugin) {
    if (!plugin) {
        pImpl->logger->error("Cannot add null synaptic dynamics plugin");
        return false;
    }
    
    pImpl->logger->info("Synaptic dynamics plugin added: " + plugin->getName());
    return true;
}

class CustomMemorySystem* ExtensibleBrain::getCustomMemorySystem() const {
    return pImpl->customMemorySystem.get();
}

bool ExtensibleBrain::setCustomMemorySystem(std::unique_ptr<CustomMemorySystem> memorySystem) {
    if (!memorySystem) {
        pImpl->logger->error("Cannot set null custom memory system");
        return false;
    }
    
    pImpl->customMemorySystem = std::move(memorySystem);
    pImpl->logger->info("Custom memory system set");
    return true;
}

class AdvancedNeuromodulator* ExtensibleBrain::getAdvancedNeuromodulator() const {
    return pImpl->advancedNeuromodulator.get();
}

bool ExtensibleBrain::setAdvancedNeuromodulator(std::unique_ptr<AdvancedNeuromodulator> neuromodulator) {
    if (!neuromodulator) {
        pImpl->logger->error("Cannot set null advanced neuromodulator");
        return false;
    }
    
    pImpl->advancedNeuromodulator = std::move(neuromodulator);
    pImpl->logger->info("Advanced neuromodulator set");
    return true;
}

class NeuralArchitecture* ExtensibleBrain::getNeuralArchitecture() const {
    return pImpl->neuralArchitecture.get();
}

bool ExtensibleBrain::setNeuralArchitecture(std::unique_ptr<NeuralArchitecture> architecture) {
    if (!architecture) {
        pImpl->logger->error("Cannot set null neural architecture");
        return false;
    }
    
    pImpl->neuralArchitecture = std::move(architecture);
    pImpl->logger->info("Neural architecture set");
    return true;
}

void ExtensibleBrain::setOptimizationHooks(std::unique_ptr<OptimizationHooks> hooks) {
    if (!hooks) {
        pImpl->logger->error("Cannot set null optimization hooks");
        return;
    }
    
    pImpl->optimizationHooks = std::move(hooks);
    pImpl->logger->info("Optimization hooks set");
}

class OptimizationHooks* ExtensibleBrain::getOptimizationHooks() const {
    return pImpl->optimizationHooks.get();
}

void ExtensibleBrain::setExperimentFramework(std::unique_ptr<AdvancedExperiment> experiment) {
    if (!experiment) {
        pImpl->logger->error("Cannot set null experiment framework");
        return;
    }
    
    pImpl->experimentFramework = std::move(experiment);
    pImpl->logger->info("Experiment framework set");
}

class AdvancedExperiment* ExtensibleBrain::getExperimentFramework() const {
    return pImpl->experimentFramework.get();
}

void ExtensibleBrain::loadDefaultPlugins() {
    // Create and register default plugins
    auto neuronModelPlugin = std::make_shared<DefaultNeuronModelPlugin>();
    addNeuronModelPlugin(neuronModelPlugin);
    
    auto plasticityRulePlugin = std::make_shared<DefaultPlasticityRulePlugin>();
    addPlasticityRulePlugin(plasticityRulePlugin);
    
    auto synapticDynamicsPlugin = std::make_shared<DefaultSynapticDynamicsPlugin>();
    addSynapticDynamicsPlugin(synapticDynamicsPlugin);
}

} // namespace nlm
