#include "Brain.hpp"
#include "PluginSystem.hpp"
#include "IBrainPlugin.hpp"
#include <iostream>

namespace nlm {

// Plugin usage examples and demonstrations

void demonstrateMemoryPlugins() {
    std::cout << "=== Memory Plugins Demo ===" << std::endl;

    // Get available memory plugins
    auto availablePlugins = PluginSystem::getInstance().getAvailablePlugins();
    std::cout << "Available plugins: ";
    for (const auto& plugin : availablePlugins) {
        PluginMetadata metadata = PluginSystem::getInstance().getPluginMetadata(plugin);
        if (metadata.category == "memory") {
            std::cout << plugin << " (" << metadata.description << ") ";
        }
    }
    std::cout << std::endl;
}

void demonstrateNeuromodulationPlugins() {
    std::cout << "=== Neuromodulation Plugins Demo ===" << std::endl;

    // Configure and load dopamine plugin
    if (PluginSystem::getInstance().loadPlugin("dopamine_neuromodulation")) {
        std::cout << "Successfully loaded dopamine plugin" << std::endl;
        PluginSystem::getInstance().configurePlugin("dopamine_neuromodulation", 
                                                   "learning_rate", 0.01f);
    }

    // Configure and load novelty detection plugin
    if (PluginSystem::getInstance().loadPlugin("novelty_detection")) {
        std::cout << "Successfully loaded novelty detection plugin" << std::endl;
        PluginSystem::getInstance().configurePlugin("novelty_detection", 
                                                   "novelty_threshold", 0.5f);
    }
}

void demonstratePlasticityPlugins() {
    std::cout << "=== Plasticity Plugins Demo ===" << std::endl;

    // Load STDP plasticity plugin
    if (PluginSystem::getInstance().loadPlugin("stdp_plasticity")) {
        std::cout << "Successfully loaded STDP plasticity plugin" << std::endl;
        PluginSystem::getInstance().configurePlugin("stdp_plasticity", 
                                                   "learning_rate", 0.001f);
    }

    // Load Hebbian plasticity plugin
    if (PluginSystem::getInstance().loadPlugin("hebbian_plasticity")) {
        std::cout << "Successfully loaded Hebbian plasticity plugin" << std::endl;
        PluginSystem::getInstance().configurePlugin("hebbian_plasticity", 
                                                   "connection_threshold", 0.1f);
    }
}

void demonstratePluginLifecycle() {
    std::cout << "=== Plugin Lifecycle Demo ===" << std::endl;

    // Create a simple custom plugin
    class CustomPlugin : public IBrainPlugin {
    public:
        PluginMetadata getMetadata() const override {
            PluginMetadata metadata;
            metadata.name = "custom_test_plugin";
            metadata.version = "1.0.0";
            metadata.description = "Custom test plugin for demonstration";
            metadata.author = "NLM Team";
            metadata.category = "cognition";
            metadata.isCore = false;
            return metadata;
        }

        bool initialize(std::shared_ptr<const Config> config) override {
            std::cout << "Custom plugin initialized" << std::endl;
            return true;
        }

        void update(SimulationStep step) override {
            if (step % 100 == 0) {
                std::cout << "Custom plugin processing step " << step << std::endl;
            }
        }

        void shutdown() override {
            std::cout << "Custom plugin shut down" << std::endl;
        }

        void setBrain(Brain* brain) override {}
        void setPluginSystem(PluginSystem* pluginSystem) override {}
        bool configure(const std::string& key, const std::any& value) override { return false; }
        std::any getConfig(const std::string& key) const override { return std::any(); }
        bool supportsCategory(const std::string& category) const override { return category == "cognition"; }
        std::vector<std::string> getRequiredSystems() const override { return {}; }
        void onEvent(const std::string& eventType, const std::any& eventData) override {}
    };

    std::unique_ptr<IBrainPlugin> customPlugin = std::make_unique<CustomPlugin>();
    std::cout << "Created custom plugin: " << customPlugin->getMetadata().name << std::endl;
}

void demonstratePluginIntegration() {
    std::cout << "=== Plugin Integration Demo ===" << std::endl;

    // Example of plugin integration with brain systems
    std::cout << "Plugins can integrate with:" << std::endl;
    std::cout << "  - Memory systems (WorkingMemory, EpisodicMemory, AssociativeMemory)" << std::endl;
    std::cout << "  - Prediction systems (PredictionSystem, ErrorSignals)" << std::endl;
    std::cout << "  - Cognition systems (NeuralPlanner, ConceptFormation, Attention)" << std::endl;
    std::cout << "  - Neuromodulation systems (Dopamine, Curiosity, Novelty)" << std::endl;
    std::cout << "  - Development systems (Synaptogenesis, Pruning, Maturation)" << std::endl;
    std::cout << "  - Plasticity systems (STDP, Hebbian, StructuralPlasticity)" << std::endl;
}

void runPluginArchitectureDemo() {
    std::cout << "=== NLM Plugin Architecture Demo ===" << std::endl << std::endl;

    demonstrateMemoryPlugins();
    std::cout << std::endl;

    demonstrateNeuromodulationPlugins();
    std::cout << std::endl;

    demonstratePlasticityPlugins();
    std::cout << std::endl;

    demonstratePluginLifecycle();
    std::cout << std::endl;

    demonstratePluginIntegration();
    std::cout << std::endl;

    std::cout << "=== Demo Complete ===" << std::endl;
}

} // namespace nlm