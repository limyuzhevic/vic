// JSON Config Demo for NLM
// This demo shows how to use JSON configuration files

#include "core/Config/Config.hpp"
#include "core/Random/Random.hpp"
#include "core/Logger/Logger.hpp"
#include "core/SimulationClock/SimulationClock.hpp"
#include "brain/Neuron.hpp"
#include "brain/Synapse.hpp"
#include "brain/Brain.hpp"
#include <iostream>
#include <memory>

void printBanner() {
    std::cout << R"(
    ╔═══════════════════════════════════════════════════════════════╗
    ║                                                               ║
    ║     NLM JSON Config Demo                                      ║
    ║                                                               ║
    ║     Using JSON for Configuration                              ║
    ║                                                               ║
    ║     JSON Features:                                            ║
    ║     - Structured key-value pairs                               ║
    ║     - Type-safe values (numbers, strings, booleans)             ║
    ║     - Nested configuration support                             ║
    ║     - Human-readable format                                    ║
    ║                                                               ║
    ╚═══════════════════════════════════════════════════════════════╝
    )" << std::endl;
}

void printConfigInfo(const nlm::Config& config) {
    std::cout << "\n=== Configuration Info ===" << std::endl;
    std::cout << "Total entries: " << config.getKeys().size() << std::endl;
    std::cout << "\nEntries:" << std::endl;
    for (const auto& key : config.getKeys()) {
        auto val = config.get<std::string>(key);
        if (val.has_value()) {
            std::cout << "  " << key << " = '" << val.value() << "'" << std::endl;
        }
    }
}

void demonstrateConfigLoad() {
    std::cout << "\n=== JSON Config Loading Demo ===" << std::endl;
    
    // Create config
    auto config = std::make_shared<nlm::Config>();
    
    // Load from JSON file
    std::string jsonConfigFile = "configs/default.json";
    if (config->loadFromJSONFile(jsonConfigFile)) {
        std::cout << "✓ Successfully loaded JSON config from: " << jsonConfigFile << std::endl;
    } else {
        std::cout << "✗ Failed to load JSON config, using defaults" << std::endl;
    }
    
    // Try to load from .cfg file (legacy format)
    std::string legacyConfigFile = "configs/default.cfg";
    if (config->loadFromFile(legacyConfigFile)) {
        std::cout << "✓ Successfully loaded legacy config from: " << legacyConfigFile << std::endl;
    } else {
        std::cout << "✗ Failed to load legacy config" << std::endl;
    }
    
    // Load from command line arguments (simulated)
    std::cout << "\n=== Command Line Arguments ===" << std::endl;
    std::vector<std::string> demoArgs = {
        "--config", "configs/default.cfg",
        "--random_seed=99",
        "--neuron_count=250"
    };
    
    // Simulate loading from command line
    int argc = demoArgs.size();
    std::vector<char*> argv;
    for (const auto& arg : demoArgs) {
        char* cstr = new char[arg.size() + 1];
        strcpy(cstr, arg.c_str());
        argv.push_back(cstr);
    }
    
    config->loadFromArgs(argc, argv.data());
    
    // Print configuration info
    printConfigInfo(*config);
    
    // Clean up
    for (auto ptr : argv) {
        delete[] ptr;
    }
}

void demonstrateConfigSave() {
    std::cout << "\n=== JSON Config Saving Demo ===" << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    
    // Set some values
    config->set("simulation_name", "JSON_Demo");
    config->set("max_neurons", 1000);
    config->set("enable_stdp", true);
    config->set("learning_rate", 0.05);
    config->set("neurons_per_region", std::vector<int>({100, 150, 200}));
    
    // Save as JSON
    std::string jsonPath = "/tmp/nlm_config_demo.json";
    if (config->saveToFile(jsonPath)) {
        std::cout << "✓ Saved config to JSON file: " << jsonPath << std::endl;
    } else {
        std::cout << "✗ Failed to save JSON config" << std::endl;
    }
    
    // Save as legacy format
    std::string legacyPath = "/tmp/nlm_config_demo.cfg";
    if (config->saveToFile(legacyPath)) {
        std::cout << "✓ Saved config to legacy file: " << legacyPath << std::endl;
    } else {
        std::cout << "✗ Failed to save legacy config" << std::endl;
    }
}

void demonstrateConfigFeatures() {
    std::cout << "\n=== JSON Config Features Demo ===" << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    
    // Load JSON config
    if (config->loadFromJSONFile("configs/default.json")) {
        std::cout << "✓ Loaded JSON config with " << config->getKeys().size() << " entries" << std::endl;
        
        // Get specific values
        auto seed = config->get<int64_t>("random_seed");
        auto timestep = config->get<double>("simulation_timestep");
        auto neurons = config->get<int64_t>("neuron_count");
        
        std::cout << "  - Random seed: " << (seed.has_value() ? std::to_string(*seed) : "default") << std::endl;
        std::cout << "  - Simulation timestep: " << (timestep.has_value() ? std::to_string(*timestep) + "s" : "default") << std::endl;
        std::cout << "  - Neuron count: " << (neurons.has_value() ? std::to_string(*neurons) : "default") << std::endl;
        
        // Check existence
        if (config->has("stdp_ltp_weight")) {
            std::cout << "  - STDP LTP weight: exists" << std::endl;
        }
        
        // Use default values
        auto plasticityRate = config->getOr<float>("plasticity_learning_rate", 0.01f);
        std::cout << "  - Plasticity learning rate: " << plasticityRate << std::endl;
    }
}

int main() {
    printBanner();
    
    std::cout << "Initializing NLM JSON Configuration Demo..." << std::endl;
    
    // Initialize logger
    auto logger = std::make_shared<nlm::Logger>();
    auto consoleLogger = std::make_shared<nlm::ConsoleLogger>(nlm::LogLevel::Info);
    logger->addLogger(consoleLogger);
    nlm::Logger::setGlobal(logger);
    
    nlm::NLM_LOG_INFO("=== NLM JSON Configuration Demo ===");
    
    demonstrateConfigLoad();
    demonstrateConfigSave();
    demonstrateConfigFeatures();
    
    nlm::NLM_LOG_INFO("\n=== JSON Configuration Demo Complete ===");
    nlm::NLM_LOG_INFO("\nKey improvements implemented:");
    nlm::NLM_LOG_INFO("  ✓ JSON/YAML file format support");
    nlm::NLM_LOG_INFO("  ✓ Structured configuration loading");
    nlm::NLM_LOG_INFO("  ✓ Type-safe value parsing");
    nlm::NLM_LOG_INFO("  ✓ Backward compatibility with .cfg files");
    nlm::NLM_LOG_INFO("  ✓ Better documentation and logging");
    
return 0;
}