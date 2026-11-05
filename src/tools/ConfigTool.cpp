// Configuration command-line tool for advanced users

#include "core/Config/Config.hpp"
#include "core/Config/ConfigManager.hpp"
#include "core/Logger/Logger.hpp"
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <fstream>

namespace nlm {

class ConfigTool {
public:
    ConfigTool(int argc, char** argv);
    ~ConfigTool() = default;
    
    // Disable copying, enable moving
    ConfigTool(const ConfigTool&) = delete;
    ConfigTool& operator=(const ConfigTool&) = delete;
    ConfigTool(ConfigTool&&) noexcept = default;
    ConfigTool& operator=(ConfigTool&&) noexcept = default;
    
    // Run the configuration tool
    int run();
    
    // Show help
    void showHelp() const;
    
    // Show advanced options
    void showAdvancedOptions() const;
    
    // Show current configuration
    void showCurrentConfig(const Config& config) const;
    
    // Set configuration option
    bool setOption(const std::string& option, const std::string& value);
    
    // Load configuration from file
    bool loadFromFile(const std::string& filepath);
    
    // Save configuration to file
    bool saveToFile(const std::string& filepath) const;
    
    // Validate configuration
    bool validateConfig(Config& config);
    
    // Show configuration by category
    void showByCategory(ConfigCategory category) const;
    
private:
    int argc_;
    char** argv_;
    Config config_;
    std::string outputFile_;
    bool validateOnly_;
    bool showAdvanced_;
};

ConfigTool::ConfigTool(int argc, char** argv)
    : argc_(argc), argv_(argv), validateOnly_(false), showAdvanced_(false) {
}

int ConfigTool::run() {
    // Parse command line arguments
    for (int i = 1; i < argc_; ++i) {
        std::string arg(argv_[i]);
        
        if (arg == "--help" || arg == "-h") {
            showHelp();
            return 0;
        } else if (arg == "--validate") {
            validateOnly_ = true;
        } else if (arg == "--show-advanced") {
            showAdvanced_ = true;
        } else if (arg == "--load" && i + 1 < argc_) {
            if (!loadFromFile(argv_[++i])) {
                std::cerr << "Error: Failed to load configuration from file: " << argv_[i] << std::endl;
                return 1;
            }
        } else if (arg == "--save" && i + 1 < argc_) {
            outputFile_ = argv_[++i];
        } else if (arg == "--category" && i + 1 < argc_) {
            std::string categoryStr = argv_[++i];
            ConfigCategory category;
            // Parse category string
            if (categoryStr == "neural") {
                category = ConfigCategory::Neural;
            } else if (categoryStr == "plasticity") {
                category = ConfigCategory::Plasticity;
            } else if (categoryStr == "memory") {
                category = ConfigCategory::Memory;
            } else if (categoryStr == "neuromodulation") {
                category = ConfigCategory::Neuromodulation;
            } else if (categoryStr == "cognition") {
                category = ConfigCategory::Cognition;
            } else if (categoryStr == "development") {
                category = ConfigCategory::Development;
            } else if (categoryStr == "performance") {
                category = ConfigCategory::Performance;
            } else if (categoryStr == "checkpoint") {
                category = ConfigCategory::Checkpoint;
            } else if (categoryStr == "reward") {
                category = ConfigCategory::Reward;
            } else if (categoryStr == "sensorymotor") {
                category = ConfigCategory::SensoryMotor;
            } else if (categoryStr == "environment") {
                category = ConfigCategory::Environment;
            } else {
                std::cerr << "Error: Unknown category: " << categoryStr << std::endl;
                showHelp();
                return 1;
            }
            
            showByCategory(category);
            return 0;
        } else if (arg.substr(0, 2) == "--") {
            // Handle --option=value format
            size_t pos = arg.find('=');
            if (pos != std::string::npos) {
                std::string option = arg.substr(2, pos - 2);
                std::string value = arg.substr(pos + 1);
                if (!setOption(option, value)) {
                    std::cerr << "Error: Failed to set option: " << option << " = " << value << std::endl;
                    return 1;
                }
            } else {
                std::cerr << "Error: Invalid option format: " << arg << std::endl;
                return 1;
            }
        } else if (arg[0] == '-' && i + 1 < argc_) {
            // Handle -option value format
            std::string option = arg.substr(1);
            std::string value = argv_[++i];
            if (!setOption(option, value)) {
                std::cerr << "Error: Failed to set option: " << option << " = " << value << std::endl;
                return 1;
            }
        } else {
            std::cerr << "Error: Unknown argument: " << arg << std::endl;
            showHelp();
            return 1;
        }
    }
    
    // If no operations specified, show current configuration
    if (!validateOnly_ && outputFile_.empty() && !showAdvanced_) {
        showCurrentConfig(config_);
        return 0;
    }
    
    // Validate configuration if requested
    if (validateOnly_) {
        if (!validateConfig(config_)) {
            return 1;
        }
    }
    
    // Save configuration if output file specified
    if (!outputFile_.empty()) {
        if (!saveToFile(outputFile_)) {
            std::cerr << "Error: Failed to save configuration to file: " << outputFile_ << std::endl;
            return 1;
        }
        std::cout << "Configuration saved to: " << outputFile_ << std::endl;
    }
    
    return 0;
}

void ConfigTool::showHelp() const {
    std::cout << "NLM Configuration Tool (Advanced)" << std::endl;
    std::cout << "=================================" << std::endl;
    std::cout << "\nUsage:" << std::endl;
    std::cout << "  nlm_config_tool [options]" << std::endl;
    std::cout << "\nOptions:" << std::endl;
    std::cout << "  -h, --help                    Show this help message" << std::endl;
    std::cout << "  --load <file>                Load configuration from file" << std::endl;
    std::cout << "  --save <file>                Save configuration to file" << std::endl;
    std::cout << "  --validate                    Validate current configuration" << std::endl;
    std::cout << "  --show-advanced               Show all advanced configuration options" << std::endl;
    std::cout << "  --category <category>         Show configuration for a specific category" << std::endl;
    std::cout << "  --option=value                Set configuration option" << std::endl;
    std::cout << "  -option value                 Set configuration option (short format)" << std::endl;
    std::cout << "\nCategories:".endl;
    std::cout << "  neural, plasticity, memory, neuromodulation" << std::endl;
    std::cout << "  cognition, development, performance, checkpoint" << std::endl;
    std::cout << "  reward, sensorymotor, environment" << std::endl;
    std::cout << "\nExamples:".endl;
    std::cout << "  nlm_config_tool --load config.txt" << std::endl;
    std::cout << "  nlm_config_tool --category neural" << std::endl;
    std::cout << "  nlm_config_tool --neuron_excitability 0.8" << std::endl;
    std::cout << "  nlm_config_tool --validate" << std::endl;
    std::cout << "  nlm_config_tool --save config_modified.txt" << std::endl;
}

void ConfigTool::showAdvancedOptions() const {
    std::cout << "Advanced Configuration Options:" << std::endl;
    std::cout << "===============================" << std::endl;
    
    std::vector<std::string> options = ConfigManager::getAdvancedOptionNames();
    
    for (const auto& option : options) {
        std::string help = ConfigManager::getAdvancedOptionHelp(option);
        std::cout << "  " << option << "\t" << help << std::endl;
    }
}

void ConfigTool::showCurrentConfig(const Config& config) const {
    std::cout << "Current Configuration:" << std::endl;
    std::cout << "====================" << std::endl;
    std::cout << config.summary() << std::endl;
}

bool ConfigTool::setOption(const std::string& option, const std::string& value) {
    return ConfigManager::setAdvancedOption(config_, option, value);
}

bool ConfigTool::loadFromFile(const std::string& filepath) {
    if (!config_.loadFromFile(filepath)) {
        std::cerr << "Error: Failed to load configuration from file: " << filepath << std::endl;
        return false;
    }
    std::cout << "Configuration loaded from: " << filepath << std::endl;
    return true;
}

bool ConfigTool::saveToFile(const std::string& filepath) const {
    if (!config_.saveToFile(filepath)) {
        std::cerr << "Error: Failed to save configuration to file: " << filepath << std::endl;
        return false;
    }
    return true;
}

bool ConfigTool::validateConfig(Config& config) {
    std::string errorMessage;
    if (!ConfigManager::validate(config, errorMessage)) {
        std::cerr << "Configuration validation failed:" << std::endl;
        std::cerr << "  " << errorMessage << std::endl;
        return false;
    }
    std::cout << "Configuration validation passed." << std::endl;
    return true;
}

void ConfigTool::showByCategory(ConfigCategory category) const {
    std::vector<ConfigEntry> entries = ConfigManager::getByCategory(config_, category);
    
    if (entries.empty()) {
        std::cout << "No configuration entries found for category: " << static_cast<int>(category) << std::endl;
        return;
    }
    
    std::cout << "Configuration for category: " << static_cast<int>(category) << std::endl;
    std::cout << "=============================" << std::endl;
    
    for (const auto& entry : entries) {
        std::cout << "  " << entry.key << " = ";
        std::visit([&](const auto& val) {
            if constexpr (std::is_same_v<decltype(val), std::string>) {
                std::cout << "\"" << val << "\"";
            } else {
                std::cout << val;
            }
        }, entry.value);
        std::cout << " (" << static_cast<int>(entry.source) << ")" << std::endl;
    }
}

int main(int argc, char** argv) {
    NLM_LOG_INFO("NLM Configuration Tool started");
    
    ConfigTool tool(argc, argv);
    int result = tool.run();
    
    if (result == 0) {
        NLM_LOG_INFO("Configuration tool completed successfully");
    } else {
        NLM_LOG_ERROR("Configuration tool failed with error code: " + std::to_string(result));
    }
    
    return result;
}

} // namespace nlm
