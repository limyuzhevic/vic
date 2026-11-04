// NLM - Advanced Configuration Commands
// Provides enhanced configuration management and user interface

#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include "brain/Brain.hpp"
#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <fstream>

namespace nlm {

// Configuration preset definitions
struct ConfigPreset {
    std::string name;
    std::string description;
    std::map<std::string, std::string> settings;
    
    ConfigPreset(const std::string& n, const std::string& desc) 
        : name(n), description(desc) {}
};

// Advanced configuration manager
class AdvancedConfigManager {
public:
    AdvancedConfigManager() = default;
    ~AdvancedConfigManager() = default;
    
    // Get available presets
    static std::vector<ConfigPreset> getAvailablePresets() {
        std::vector<ConfigPreset> presets;
        
        // Phase 2 presets (basic functionality)
        presets.emplace_back("phase2_basic", "Phase 2 basic neural computation");
        presets["phase2_basic"].settings = {
            {"neuron_count", "500"},
            {"region_count", "1"},
            {"connection_probability", "0.15"},
            {"stdp_ltp_weight", "0.02"},
            {"stdp_ltd_weight", "0.015"},
            {"synaptogenesis_rate", "0.0001"},
            {"pruning_rate", "0.00001"},
            {"random_seed", "42"},
            {"simulation_timestep", "0.001"}
        };
        
        // Phase 3 presets (with learning)
        presets.emplace_back("phase3_learning", "Phase 3 learning capabilities");
        presets["phase3_learning"].settings = {
            {"neuron_count", "1000"},
            {"region_count", "1"},
            {"connection_probability", "0.1"},
            {"stdp_ltp_weight", "0.01"},
            {"stdp_ltd_weight", "0.012"},
            {"synaptogenesis_rate", "0.00005"},
            {"pruning_rate", "0.000005"},
            {"random_seed", "123"},
            {"simulation_timestep", "0.001"}
        };
        
        // Phase 4 presets (memory and cognition)
        presets.emplace_back("phase4_memory", "Phase 4 memory and cognition");
        presets["phase4_memory"].settings = {
            {"neuron_count", "2000"},
            {"region_count", "2"},
            {"connection_probability", "0.08"},
            {"stdp_ltp_weight", "0.015"},
            {"stdp_ltd_weight", "0.018"},
            {"synaptogenesis_rate", "0.0002"},
            {"pruning_rate", "0.00002"},
            {"random_seed", "456"},
            {"simulation_timestep", "0.001"},
            {"replay_interval", "100"},
            {"consolidation_interval", "1000"}
        };
        
        // Phase 5 presets (performance optimizations)
        presets.emplace_back("phase5_performance", "Phase 5 performance optimizations");
        presets["phase5_performance"].settings = {
            {"neuron_count", "3000"},
            {"region_count", "3"},
            {"connection_probability", "0.06"},
            {"stdp_ltp_weight", "0.02"},
            {"stdp_ltd_weight", "0.02"},
            {"synaptogenesis_rate", "0.0003"},
            {"pruning_rate", "0.00003"},
            {"random_seed", "789"},
            {"simulation_timestep", "0.001"},
            {"replay_interval", "50"},
            {"consolidation_interval", "500"}
        };
        
        // Phase 6 presets (full integration)
        presets.emplace_back("phase6_integration", "Phase 6 full integration");
        presets["phase6_integration"].settings = {
            {"neuron_count", "5000"},
            {"region_count", "4"},
            {"connection_probability", "0.04"},
            {"stdp_ltp_weight", "0.025"},
            {"stdp_ltd_weight", "0.025"},
            {"synaptogenesis_rate", "0.0005"},
            {"pruning_rate", "0.00005"},
            {"random_seed", "999"},
            {"simulation_timestep", "0.001"},
            {"replay_interval", "25"},
            {"consolidation_interval", "250"}
        };
        
        // Custom demo presets
        presets.emplace_back("demo_basic", "Demo: Basic functionality");
        presets["demo_basic"].settings = {
            {"neuron_count", "100"},
            {"region_count", "1"},
            {"connection_probability", "0.2"},
            {"random_seed", "1"}
        };
        
        presets.emplace_back("demo_plasticity", "Demo: Plasticity learning");
        presets["demo_plasticity"].settings = {
            {"neuron_count", "200"},
            {"region_count", "1"},
            {"connection_probability", "0.15"},
            {"stdp_ltp_weight", "0.03"},
            {"stdp_ltd_weight", "0.03"},
            {"synaptogenesis_rate", "0.0002"},
            {"pruning_rate", "0.00001"},
            {"random_seed", "2"}
        };
        
        presets.emplace_back("demo_integration", "Demo: System integration");
        presets["demo_integration"].settings = {
            {"neuron_count", "300"},
            {"region_count", "2"},
            {"connection_probability", "0.1"},
            {"stdp_ltp_weight", "0.01"},
            {"stdp_ltd_weight", "0.01"},
            {"replay_interval", "50"},
            {"consolidation_interval", "500"},
            {"random_seed", "3"}
        };
        
        return presets;
    }
    
    // Apply preset to configuration
    static bool applyPreset(std::shared_ptr<Config> config, const std::string& presetName) {
        auto presets = getAvailablePresets();
        
        for (const auto& preset : presets) {
            if (preset.name == presetName) {
                for (const auto& setting : preset.settings) {
                    config->set(setting.first, setting.second, ConfigSource::Runtime);
                }
                
                NLM_LOG_INFO("Applied preset: " + presetName);
                NLM_LOG_INFO("Description: " + preset.description);
                return true;
            }
        }
        
        NLM_LOG_ERROR("Preset not found: " + presetName);
        return false;
    }
    
    // Get configuration recommendations for demo type
    static std::map<std::string, std::string> getDemoRecommendations(const std::string& demoType) {
        std::map<std::string, std::string> recommendations;
        
        if (demoType == "basic") {
            recommendations["neuron_count"] = "100-200";
            recommendations["steps"] = "500-1000";
            recommendations["verbose"] = "true";
            recommendations["checkpoint"] = "false";
        } else if (demoType == "plasticity") {
            recommendations["neuron_count"] = "200-500";
            recommendations["steps"] = "1000-2000";
            recommendations["stdp_ltp_weight"] = "0.02-0.05";
            recommendations["development"] = "true";
            recommendations["curiosity"] = "false";
        } else if (demoType == "integration") {
            recommendations["neuron_count"] = "500-1000";
            recommendations["steps"] = "2000-5000";
            recommendations["replay_interval"] = "50-100";
            recommendations["consolidation_interval"] = "500-1000";
            recommendations["checkpoint"] = "true";
            recommendations["checkpoint_path"] = "./checkpoints/";
        } else if (demoType == "advanced") {
            recommendations["neuron_count"] = "1000-5000";
            recommendations["steps"] = "5000-10000";
            recommendations["development"] = "true";
            recommendations["curiosity"] = "true";
            recommendations["reward"] = "true";
            recommendations["checkpoint"] = "true";
            recommendations["replay"] = "true";
        }
        
        return recommendations;
    }
    
    // Validate configuration
    static std::vector<std::string> validateConfig(const std::shared_ptr<Config>& config) {
        std::vector<std::string> errors;
        
        // Check required settings
        if (!config->has("neuron_count")) {
            errors.push_back("Missing required setting: neuron_count");
        } else {
            int64_t neuronCount = config->getOr<int64_t>("neuron_count", 0);
            if (neuronCount < 1 || neuronCount > 100000) {
                errors.push_back("Invalid neuron_count: must be between 1 and 100000");
            }
        }
        
        if (!config->has("region_count")) {
            errors.push_back("Missing required setting: region_count");
        } else {
            int64_t regionCount = config->getOr<int64_t>("region_count", 0);
            if (regionCount < 1 || regionCount > 100) {
                errors.push_back("Invalid region_count: must be between 1 and 100");
            }
        }
        
        if (!config->has("connection_probability")) {
            errors.push_back("Missing required setting: connection_probability");
        } else {
            float connProb = config->getOr<float>("connection_probability", 0.0f);
            if (connProb < 0.0f || connProb > 1.0f) {
                errors.push_back("Invalid connection_probability: must be between 0.0 and 1.0");
            }
        }
        
        if (!config->has("random_seed")) {
            errors.push_back("Missing recommended setting: random_seed");
        }
        
        // Check for conflicts
        if (config->has("stdp_ltp_weight") && config->has("stdp_ltd_weight")) {
            float ltp = config->getOr<float>("stdp_ltp_weight", 0.0f);
            float ltd = config->getOr<float>("stdp_ltd_weight", 0.0f);
            if (ltp <= 0.0f || ltd <= 0.0f) {
                errors.push_back("STDP weights must be positive values");
            }
        }
        
        return errors;
    }
    
    // Export configuration to file
    static bool exportConfig(const std::shared_ptr<Config>& config, const std::string& filepath) {
        try {
            std::ofstream file(filepath);
            if (!file.is_open()) {
                NLM_LOG_ERROR("Failed to open file for export: " + filepath);
                return false;
            }
            
            file << "# NLM Configuration Export" << std::endl;
            file << "# Generated by AdvancedConfigManager" << std::endl;
            file << "# " << std::endl;
            
            file << "[config]" << std::endl;
            auto keys = config->getKeys();
            for (const auto& key : keys) {
                std::string value;
                if (auto val = config->get<std::string>(key)) {
                    value = *val;
                } else if (auto val = config->get<int64_t>(key)) {
                    value = std::to_string(*val);
                } else if (auto val = config->get<double>(key)) {
                    value = std::to_string(*val);
                } else if (auto val = config->get<float>(key)) {
                    value = std::to_string(*val);
                } else if (auto val = config->get<bool>(key)) {
                    value = *val ? "true" : "false";
                } else {
                    continue;
                }
                
                file << key << " = \"" << value << "\"" << std::endl;
            }
            
            file << std::endl;
            file << "[presets]" << std::endl;
            file << "# This file contains configuration for different demo types" << std::endl;
            file << "# Use --preset=<name> to apply any preset" << std::endl;
            
            file.close();
            NLM_LOG_INFO("Configuration exported to: " + filepath);
            return true;
            
        } catch (const std::exception& e) {
            NLM_LOG_ERROR(std::string("Error exporting configuration: ") + e.what());
            return false;
        }
    }
    
    // Import configuration from file
    static bool importConfig(std::shared_ptr<Config>& config, const std::string& filepath) {
        // First, create a temporary config
        auto tempConfig = std::make_shared<Config>();
        
        if (!tempConfig->loadFromFile(filepath)) {
            NLM_LOG_ERROR("Failed to load configuration from: " + filepath);
            return false;
        }
        
        // Validate the imported config
        auto errors = validateConfig(tempConfig);
        if (!errors.empty()) {
            NLM_LOG_ERROR("Configuration validation failed:");
            for (const auto& error : errors) {
                NLM_LOG_ERROR("  - " + error);
            }
            return false;
        }
        
        // Merge with existing config (temp takes precedence)
        auto keys = tempConfig->getKeys();
        for (const auto& key : keys) {
            config->set(key, tempConfig->get<ConfigValue>(key).value(), ConfigSource::Runtime);
        }
        
        NLM_LOG_INFO("Configuration imported from: " + filepath);
        return true;
    }
    
    // Get configuration summary
    static std::string getConfigSummary(const std::shared_ptr<Config>& config) {
        std::ostringstream oss;
        oss << "NLM Configuration Summary:" << std::endl;
        oss << "=======================" << std::endl;
        oss << std::endl;
        
        oss << "Core Settings:" << std::endl;
        oss << "  neuron_count: " << config->getOr<int64_t>("neuron_count", 0) << std::endl;
        oss << "  region_count: " << config->getOr<int64_t>("region_count", 0) << std::endl;
        oss << "  connection_probability: " << config->getOr<float>("connection_probability", 0.0f) << std::endl;
        oss << "  simulation_timestep: " << config->getOr<double>("simulation_timestep", 0.0f) << "s" << std::endl;
        oss << "  random_seed: " << config->getOr<int64_t>("random_seed", 0) << std::endl;
        oss << std::endl;
        
        oss << "Plasticity Settings:" << std::endl;
        oss << "  stdp_ltp_weight: " << config->getOr<float>("stdp_ltp_weight", 0.0f) << std::endl;
        oss << "  stdp_ltd_weight: " << config->getOr<float>("stdp_ltd_weight", 0.0f) << std::endl;
        oss << "  stdp_tau: " << config->getOr<float>("stdp_tau", 0.0f) << std::endl;
        oss << std::endl;
        
        oss << "Structural Plasticity:" << std::endl;
        oss << "  synaptogenesis_rate: " << config->getOr<float>("synaptogenesis_rate", 0.0f) << std::endl;
        oss << "  pruning_rate: " << config->getOr<float>("pruning_rate", 0.0f) << std::endl;
        std::endl << std::endl;
        
        oss << "Memory Integration:" << std::endl;
        oss << "  replay_interval: " << config->getOr<int64_t>("replay_interval", 0) << std::endl;
        oss << "  consolidation_interval: " << config->getOr<int64_t>("consolidation_interval", 0) << std::endl;
        
        return oss.str();
    }
};

// Command line configuration parser
class ConfigCommandLineParser {
public:
    ConfigCommandLineParser() = default;
    ~ConfigCommandLineParser() = default;
    
    // Parse configuration from command line
    static std::map<std::string, std::string> parseConfigArgs(int argc, char** argv) {
        std::map<std::string, std::string> config;
        
        for (int i = 1; i < argc; ++i) {
            std::string arg(argv[i]);
            
            // Handle configuration options
            if (arg.find("--neuron_count=") == 0) {
                config["neuron_count"] = arg.substr(13);
            } else if (arg.find("--region_count=") == 0) {
                config["region_count"] = arg.substr(13);
            } else if (arg.find("--connection_probability=") == 0) {
                config["connection_probability"] = arg.substr(24);
            } else if (arg.find("--stdp_ltp_weight=") == 0) {
                config["stdp_ltp_weight"] = arg.substr(18);
            } else if (arg.find("--stdp_ltd_weight=") == 0) {
                config["stdp_ltd_weight"] = arg.substr(18);
            } else if (arg.find("--stdp_tau=") == 0) {
                config["stdp_tau"] = arg.substr(10);
            } else if (arg.find("--synaptogenesis_rate=") == 0) {
                config["synaptogenesis_rate"] = arg.substr(21);
            } else if (arg.find("--pruning_rate=") == 0) {
                config["pruning_rate"] = arg.substr(15);
            } else if (arg.find("--replay_interval=") == 0) {
                config["replay_interval"] = arg.substr(18);
            } else if (arg.find("--consolidation_interval=") == 0) {
                config["consolidation_interval"] = arg.substr(25);
            } else if (arg == "--checkpoint" || arg == "--enable_checkpointing") {
                config["enable_checkpointing"] = "true";
            } else if (arg == "--development" || arg == "--enable_development") {
                config["enable_development"] = "true";
            } else if (arg == "--curiosity" || arg == "--enable_curiosity") {
                config["enable_curiosity"] = "true";
            } else if (arg == "--reward" || arg == "--enable_reward") {
                config["enable_reward"] = "true";
            }
        }
        
        return config;
    }
    
    // Apply parsed arguments to configuration
    static void applyConfigArgs(std::shared_ptr<Config> config, const std::map<std::string, std::string>& args) {
        for (const auto& arg : args) {
            std::string value = arg.second;
            
            // Convert string values to appropriate types
            if (value == "true" || value == "false") {
                config->set(arg.first, value == "true", ConfigSource::CommandLine);
            } else if (value.find('.') != std::string::npos) {
                // Try float first, then double
                try {
                    float floatVal = std::stof(value);
                    config->set(arg.first, floatVal, ConfigSource::CommandLine);
                } catch (...) {
                    try {
                        double doubleVal = std::stod(value);
                        config->set(arg.first, doubleVal, ConfigSource::CommandLine);
                    } catch (...) {
                        config->set(arg.first, value, ConfigSource::CommandLine);
                    }
                }
            } else {
                // Try int64_t first, then int
                try {
                    int64_t intVal = std::stoll(value);
                    config->set(arg.first, intVal, ConfigSource::CommandLine);
                } catch (...) {
                    try {
                        int intVal = std::stoi(value);
                        config->set(arg.first, intVal, ConfigSource::CommandLine);
                    } catch (...) {
                        config->set(arg.first, value, ConfigSource::CommandLine);
                    }
                }
            }
        }
    }
};

// Enhanced demonstration runner with advanced features
class EnhancedDemoRunner {
public:
    EnhancedDemoRunner() = default;
    ~EnhancedDemoRunner() = default;
    
    // Run enhanced demo with advanced features
    bool runEnhancedDemo(const std::string& demoType, uint64_t steps,
                        const std::map<std::string, std::string>& extraArgs) {
        NLM_LOG_INFO("=== Enhanced Demo Runner ===");
        NLM_LOG_INFO("Demo Type: " + demoType);
        NLM_LOG_INFO("Steps: " + std::to_string(steps));
        
        if (!extraArgs.empty()) {
            NLM_LOG_INFO("Additional Arguments:");
            for (const auto& arg : extraArgs) {
                NLM_LOG_INFO("  " + arg.first + " = " + arg.second);
            }
        }
        
        // Create configuration with enhanced settings
        auto config = std::make_shared<Config>();
        
        // Apply demo-specific preset
        std::string presetName;
        if (demoType == "basic") presetName = "demo_basic";
        else if (demoType == "plasticity") presetName = "demo_plasticity";
        else if (demoType == "integration") presetName = "demo_integration";
        else presetName = "phase2_basic";
        
        AdvancedConfigManager::applyPreset(config, presetName);
        
        // Override with extra arguments
        for (const auto& arg : extraArgs) {
            config->set(arg.first, arg.second, ConfigSource::Runtime);
        }
        
        // Validate configuration
        auto errors = AdvancedConfigManager::validateConfig(config);
        if (!errors.empty()) {
            NLM_LOG_ERROR("Configuration validation failed:");
            for (const auto& error : errors) {
                NLM_LOG_ERROR("  - " + error);
            }
            return false;
        }
        
        // Create and initialize brain
        auto brain = std::make_shared<Brain>(config);
        if (!brain->initialize()) {
            NLM_LOG_ERROR("Failed to initialize brain");
            return false;
        }
        
        // Log configuration summary
        std::string summary = AdvancedConfigManager::getConfigSummary(config);
        NLM_LOG_INFO(summary);
        
        // Run simulation based on demo type
        bool success = runEnhancedDemoSteps(brain, demoType, steps);
        
        if (success) {
            NLM_LOG_INFO("Enhanced demo completed successfully!");
        } else {
            NLM_LOG_ERROR("Enhanced demo failed!");
        }
        
        return success;
    }
    
private:
    bool runEnhancedDemoSteps(std::shared_ptr<Brain> brain, const std::string& demoType, uint64_t steps) {
        // This would run the actual demo logic
        // For now, return success
        
        NLM_LOG_INFO("Running " + demoType + " demo for " + std::to_string(steps) + " steps...");
        
        for (uint64_t step = 0; step < steps; ++step) {
            brain->step(step, step * 0.001);
            
            if (step % 500 == 0) {
                NLM_LOG_INFO("Step " + std::to_string(step) + 
                           " | Firing: " + std::to_string(brain->getAverageFiringRate()) +
                           " | Spikes: " + std::to_string(brain->getTotalSpikeCount()));
            }
        }
        
        return true;
    }
};

// Utility functions for configuration management
class ConfigUtils {
public:
    ConfigUtils() = default;
    ~ConfigUtils() = default;
    
    // Print help for configuration commands
    static void printConfigHelp() {
        std::cout << R"(
=== NLM Advanced Configuration Commands ===

Configuration Presets:
  --preset=<name>     Apply a configuration preset
  Available presets:
    phase2_basic        Phase 2 basic neural computation
    phase3_learning     Phase 3 learning capabilities
    phase4_memory       Phase 4 memory and cognition
    phase5_performance   Phase 5 performance optimizations
    phase6_integration  Phase 6 full integration
    demo_basic         Demo: Basic functionality
    demo_plasticity    Demo: Plasticity learning
    demo_integration   Demo: System integration

Configuration Options:
  --neuron_count=<n>         Number of neurons
  --region_count=<n>         Number of brain regions
  --connection_probability=<f> Connection probability (0.0-1.0)
  --stdp_ltp_weight=<f>       STDP LTP weight
  --stdp_ltd_weight=<f>       STDP LTD weight
  --stdp_tau=<f>              STDP time constant
  --synaptogenesis_rate=<f>   Synaptogenesis rate
  --pruning_rate=<f>          Pruning rate
  --replay_interval=<n>       Replay interval (steps)
  --consolidation_interval=<n> Consolidation interval (steps)
  --checkpoint                 Enable checkpointing
  --development                Enable brain development
  --curiosity                  Enable curiosity-driven exploration
  --reward                      Enable reward-based learning

File Operations:
  --export_config=<file>       Export configuration to file
  --import_config=<file>       Import configuration from file
  --validate_config             Validate current configuration
  --config_summary              Print configuration summary

Validation and Testing:
  --validate_config             Validate configuration
  --test_config                 Test configuration with demo
  --benchmark_config           Benchmark configuration settings

Examples:
  nlm --demo=basic --preset=phase2_basic
  nlm --demo=plasticity --neuron_count=200 --stdp_ltp_weight=0.05
  nlm --demo=integration --checkpoint --replay_interval=100
  nlm --export_config=my_config.cfg
  nlm --import_config=exported_config.cfg
  nlm --validate_config --test_config

For more information, see:
  - docs/ADVANCED_CONFIGURATION.md
  - docs/CONFIG_PRESETS.md
  - docs/DEMOS.md
)" << std::endl;
    }
    
    // Create configuration file template
    static bool createConfigTemplate(const std::string& filepath) {
        try {
            std::ofstream file(filepath);
            if (!file.is_open()) {
                NLM_LOG_ERROR("Failed to create configuration template: " + filepath);
                return false;
            }
            
            file << "# NLM Configuration Template" << std::endl;
            file << "# Edit this file to customize your brain configuration" << std::endl;
            file << "# " << std::endl;
            file << "[config]" << std::endl;
            file << "# Basic settings" << std::endl;
            file << "neuron_count = 100" << std::endl;
            file << "region_count = 1" << std::endl;
            file << "connection_probability = 0.15" << std::endl;
            file << "simulation_timestep = 0.001" << std::endl;
            file << "random_seed = 42" << std::endl;
            file << "" << std::endl;
            file << "# Plasticity settings" << std::endl;
            file << "stdp_ltp_weight = 0.02" << std::endl;
            file << "stdp_ltd_weight = 0.015" << std::endl;
            file << "stdp_tau = 20.0" << std::endl;
            file << "" << std::endl;
            file << "# Structural plasticity" << std::endl;
            file << "synaptogenesis_rate = 0.0001" << std::endl;
            file << "pruning_rate = 0.00001" << std::endl;
            file << "" << std::endl;
            file << "# Memory integration" << std::endl;
            file << "replay_interval = 100" << std::endl;
            file << "consolidation_interval = 1000" << std::endl;
            file << "" << std::endl;
            file << "[features]" << std::endl;
            file << "enable_development = false" << std::endl;
            file << "enable_curiosity = false" << std::endl;
            file << "enable_reward = false" << std::endl;
            file << "enable_checkpointing = false" << std::endl;
            
            file.close();
            NLM_LOG_INFO("Configuration template created: " + filepath);
            return true;
            
        } catch (const std::exception& e) {
            NLM_LOG_ERROR(std::string("Error creating config template: ") + e.what());
            return false;
        }
    }
    
    // Validate configuration file
    static bool validateConfigFile(const std::string& filepath) {
        auto config = std::make_shared<Config>();
        if (!config->loadFromFile(filepath)) {
            NLM_LOG_ERROR("Failed to load configuration file: " + filepath);
            return false;
        }
        
        auto errors = AdvancedConfigManager::validateConfig(config);
        if (!errors.empty()) {
            NLM_LOG_ERROR("Configuration file validation failed:");
            for (const auto& error : errors) {
                NLM_LOG_ERROR("  - " + error);
            }
            return false;
        }
        
        NLM_LOG_INFO("Configuration file validation passed: " + filepath);
        return true;
    }
};

// Convenience functions for easy usage
namespace DemoPresets {
    // Apply common presets
    inline bool applyBasicPreset(std::shared_ptr<Config> config) {
        return AdvancedConfigManager::applyPreset(config, "demo_basic");
    }
    
    inline bool applyPlasticityPreset(std::shared_ptr<Config> config) {
        return AdvancedConfigManager::applyPreset(config, "demo_plasticity");
    }
    
    inline bool applyIntegrationPreset(std::shared_ptr<Config> config) {
        return AdvancedConfigManager::applyPreset(config, "demo_integration");
    }
    
    // Get demo recommendations
    inline std::map<std::string, std::string> getDemoRecommendations(const std::string& demoType) {
        return AdvancedConfigManager::getDemoRecommendations(demoType);
    }
}

} // namespace nlm
