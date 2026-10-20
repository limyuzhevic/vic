// demonstration/modern_cli_demo.cpp
// Demonstration of the modern CLI with improved config system

#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include "brain/Brain.hpp"
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cmath>
#include <algorithm>

using namespace nlm;

void printBanner() {
    std::cout << "╔═══════════════════════════════════════════════════════════════╗" << std::endl;
    std::cout << "║                                                               ║" << std::endl;
    std::cout << "║           NLM Modern CLI Demonstration                        ║" << std::endl;
    std::cout << "║                                                               ║" << std::endl;
    std::cout << "╚═══════════════════════════════════════════════════════════════╝" << std::endl;
    std::cout << std::endl;
}

void demoJsonConfig() {
    std::cout << "=== JSON Configuration Demo ===" << std::endl;
    std::cout << "Creating configuration with JSON format..." << std::endl;
    
    // Create config
    Config config;
    
    // Set values (these would normally be loaded from JSON file)
    config.set("neuron_count", static_cast<int64_t>(1500), ConfigSource::Runtime);
    config.set("environment_width", static_cast<int64_t>(30), ConfigSource::Runtime);
    config.set("enable_reward_modulation", true, ConfigSource::Runtime);
    config.set("enable_structural_plasticity", true, ConfigSource::Runtime);
    config.set("enable_development", true, ConfigSource::Runtime);
    config.set("enable_curiosity", true, ConfigSource::Runtime);
    
    // Show config summary
    std::cout << "Configuration Summary:" << std::endl;
    std::cout << config.summary() << std::endl;
    
    // Demonstrate save to JSON
    if (config.saveToFile("demo_config.json")) {
        std::cout << "✓ Configuration saved to demo_config.json" << std::endl;
    } else {
        std::cout << "✗ Failed to save configuration" << std::endl;
    }
    
    std::cout << std::endl;
}

void demoCommandLineInterface() {
    std::cout << "=== Command Line Interface Demo ===" << std::endl;
    std::cout << "Demonstrating CLI argument parsing..." << std::endl;
    
    // Simulate command line arguments
    const char* argv[] = {"demo", "--mode=phase6", "--neuron_count=2000", "--environment_width=25", 
                         "--enable_reward_modulation=true", "--enable_development=false"};
    int argc = 8;
    
    Config config;
    
    // Load from command line arguments
    config.loadFromArgs(argc, const_cast<char**>(argv));
    
    // Show what was loaded
    std::cout << "Parsed arguments:" << std::endl;
    std::cout << "  neuron_count: " << config.getOr<int64_t>("neuron_count", 1000) << std::endl;
    std::cout << "  environment_width: " << config.getOr<int64_t>("environment_width", 10) << std::endl;
    std::cout << "  enable_reward_modulation: " << config.getOr<bool>("enable_reward_modulation", true) << std::endl;
    std::cout << "  enable_development: " << config.getOr<bool>("enable_development", true) << std::endl;
    
    std::cout << std::endl;
}

void demoBackwardCompatibility() {
    std::cout << "=== Backward Compatibility Demo ===" << std::endl;
    std::cout << "Showing how old configs still work..." << std::endl;
    
    // Create old-style config file (key=value format)
    std::ofstream oldConfigFile("old_style_config.cfg");
    oldConfigFile << "# Old style config file\n";
    oldConfigFile << "random_seed = 123\n";
    oldConfigFile << "neuron_count = 500\n";
    oldConfigFile << "environment_width = 15\n";
    oldConfigFile << "enable_reward_modulation = true\n";
    oldConfigFile << "enable_curiosity = false\n";
    oldConfigFile.close();
    
    // Load old style config
    Config config;
    if (config.loadFromFile("old_style_config.cfg")) {
        std::cout << "✓ Old style config loaded successfully" << std::endl;
        std::cout << "  Loaded " << config.getOr<int64_t>("neuron_count", 0) << " neurons" << std::endl;
        std::cout << "  Random seed: " << config.getOr<int64_t>("random_seed", 0) << std::endl;
        std::cout << "  Reward modulation: " << config.getOr<bool>("enable_reward_modulation", false) << std::endl;
    } else {
        std::cout << "✗ Failed to load old style config" << std::endl;
    }
    
    std::cout << std::endl;
}

void demoModernConfigFeatures() {
    std::cout << "=== Modern Config Features Demo ===" << std::endl;
    std::cout << "Demonstrating advanced configuration features..." << std::endl;
    
    Config config;
    
    // Demonstrate setting values from various sources
    config.set("brain.neuron_count", static_cast<int64_t>(1000), ConfigSource::File);
    config.set("brain.synapse_density", 0.15f, ConfigSource::CommandLine);
    config.set("neuromodulation.curiosity.enable", true, ConfigSource::Runtime);
    config.set("development.plasticity_rate", 0.01f, ConfigSource::Default);
    
    // Show config with source information
    std::cout << "Configuration with source tracking:" << std::endl;
    std::cout << config.summary() << std::endl;
    
    // Demonstrate type safety
    std::cout << "Type-safe value access:" << std::endl;
    std::cout << "  neuron_count (int64_t): " << config.getOr<int64_t>("brain.neuron_count", 0) << std::endl;
    std::cout << "  synapse_density (float): " << config.getOr<float>("brain.synapse_density", 0.0f) << std::endl;
    std::cout << "  curiosity_enable (bool): " << config.getOr<bool>("neuromodulation.curiosity.enable", false) << std::endl;
    
    std::cout << std::endl;
}

void demoConfigurationMerging() {
    std::cout << "=== Configuration Merging Demo ===" << std::endl;
    std::cout << "Showing how configurations can be merged..." << std::endl;
    
    // Create base config
    Config baseConfig;
    baseConfig.set("neuron_count", static_cast<int64_t>(1000), ConfigSource::File);
    baseConfig.set("enable_reward_modulation", true, ConfigSource::File);
    baseConfig.set("enable_development", false, ConfigSource::File);
    
    // Create override config
    Config overrideConfig;
    overrideConfig.set("neuron_count", static_cast<int64_t>(2000), ConfigSource::CommandLine);
    overrideConfig.set("enable_curiosity", true, ConfigSource::CommandLine);
    
    std::cout << "Base configuration:" << std::endl;
    std::cout << baseConfig.summary() << std::endl;
    
    std::cout << "Override configuration:" << std::endl;
    std::cout << overrideConfig.summary() << std::endl;
    
    // Merge by copying overrides
    for (const auto& key : overrideConfig.getKeys()) {
        auto val = overrideConfig.getOr<std::string>(key, "");
        ConfigValue configVal;
        if (val.find('.') != std::string::npos) {
            // String value (for now, simplified)
            configVal = val;
        } else if (key.find("enable_")) {
            configVal = val == "true" || val == "1";
        } else {
            try {
                configVal = std::stoi(val);
            } catch (...) {
                configVal = val;
            }
        }
        baseConfig.set(key, configVal, ConfigSource::CommandLine);
    }
    
    std::cout << "Merged configuration:" << std::endl;
    std::cout << baseConfig.summary() << std::endl;
    
    std::cout << std::endl;
}

int main(int argc, char** argv) {
    printBanner();
    
    // Initialize logger
    auto logger = std::make_shared<Logger>();
    auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);
    
    NLM_LOG_INFO("=== NLM Modern CLI Demonstration ===");
    
    // Run demos
    demoJsonConfig();
    demoCommandLineInterface();
    demoBackwardCompatibility();
    demoModernConfigFeatures();
    demoConfigurationMerging();
    
    // Demonstrate file I/O
    std::cout << "=== File I/O Demo ===" << std::endl;
    
    Config config;
    config.set("test.key1", std::string("value1"), ConfigSource::Runtime);
    config.set("test.key2", static_cast<int64_t>(42), ConfigSource::Runtime);
    config.set("test.key3", 3.14f, ConfigSource::Runtime);
    config.set("test.key4", true, ConfigSource::Runtime);
    
    if (config.saveToFile("demo_full_config.json")) {
        std::cout << "✓ Full demo config saved to demo_full_config.json" << std::endl;
        
        // Load it back
        Config config2;
        if (config2.loadFromFile("demo_full_config.json")) {
            std::cout << "✓ Config loaded successfully from file" << std::endl;
            std::cout << "  key1: " << config2.getOr<std::string>("test.key1", "") << std::endl;
            std::cout << "  key2: " << config2.getOr<int64_t>("test.key2", 0) << std::endl;
            std::cout << "  key3: " << config2.getOr<float>("test.key3", 0.0f) << std::endl;
            std::cout << "  key4: " << config2.getOr<bool>("test.key4", false) << std::endl;
        }
    }
    
    NLM_LOG_INFO("=== Demonstration Complete ===");
    
    return 0;
}