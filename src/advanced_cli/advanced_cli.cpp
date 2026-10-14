#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <memory>
#include "core/Config/Config.hpp"
#include "core/AdvancedConfig/AdvancedConfig.hpp"

class AdvancedCLI {
public:
    AdvancedCLI() {
        config_ = std::make_shared<Config>();
        advancedConfig_ = std::make_shared<AdvancedConfig>();
    }
    
    void run() {
        std::cout << "=== NLM Advanced Configuration CLI ===" << std::endl;
        std::cout << "Available commands:" << std::endl;
        std::cout << "  help                    Show this help message" << std::endl;
        std::cout << "  config <key> <value>     Set configuration value" << std::endl;
        std::cout << "  get <key>              Get configuration value" << std::endl;
        std::cout << "  show                    Show all configuration" << std::endl;
        std::cout << "  apply-advanced          Apply advanced configuration" << std::endl;
        std::cout << "  save <file>            Save configuration to file" << std::endl;
        std::cout << "  load <file>            Load configuration from file" << std::endl;
        std::cout << "  optimize                Apply optimized configuration" << std::endl;
        std::cout << "  exit                   Exit CLI" << std::endl;
        
        std::string line;
        while (true) {
            std::cout << ">>> ";
            if (!std::getline(std::cin, line)) break;
            
            if (line.empty()) continue;
            
            std::vector<std::string> tokens = split(line);
            std::string command = tokens[0];
            
            if (command == "help") {
                showHelp();
            } else if (command == "config") {
                if (tokens.size() >= 3) {
                    setConfig(tokens[1], tokens[2]);
                } else {
                    std::cout << "Usage: config <key> <value>" << std::endl;
                }
            } else if (command == "get") {
                if (tokens.size() >= 2) {
                    getConfig(tokens[1]);
                } else {
                    std::cout << "Usage: get <key>" << std::endl;
                }
            } else if (command == "show") {
                showConfig();
            } else if (command == "apply-advanced") {
                applyAdvancedConfig();
            } else if (command == "save") {
                if (tokens.size() >= 2) {
                    saveConfig(tokens[1]);
                } else {
                    std::cout << "Usage: save <file>" << std::endl;
                }
            } else if (command == "load") {
                if (tokens.size() >= 2) {
                    loadConfig(tokens[1]);
                } else {
                    std::cout << "Usage: load <file>" << std::endl;
                }
            } else if (command == "optimize") {
                applyOptimizedConfig();
            } else if (command == "exit") {
                std::cout << "Exiting CLI..." << std::endl;
                break;
            } else {
                std::cout << "Unknown command: " << command << std::endl;
                showHelp();
            }
        }
    }
    
private:
    std::shared_ptr<Config> config_;
    std::shared_ptr<AdvancedConfig> advancedConfig_;
    
    std::vector<std::string> split(const std::string& line) {
        std::vector<std::string> tokens;
        std::string token;
        for (char c : line) {
            if (c == ' ') {
                if (!token.empty()) {
                    tokens.push_back(token);
                    token.clear();
                }
            } else {
                token += c;
            }
        }
        if (!token.empty()) {
            tokens.push_back(token);
        }
        return tokens;
    }
    
    void showHelp() {
        std::cout << "Advanced Configuration CLI Help:" << std::endl;
        std::cout << "" << std::endl;
        std::cout << "Configuration commands:" << std::endl;
        std::cout << "  config <key> <value>     Set configuration value (e.g., 'brain.neuron_count 1000')" << std::endl;
        std::cout << "  get <key>              Get configuration value" << std::endl;
        std::cout << "  show                    Show all configuration" << std::endl;
        std::cout << "" << std::endl;
        std::cout << "Advanced configuration:" << std::endl;
        std::cout << "  apply-advanced          Apply advanced configuration with all optimizations" << std::endl;
        std::cout << "  optimize                Apply optimized configuration (performance-focused)" << std::endl;
        std::cout << "" << std::endl;
        std::cout << "File operations:" << std::endl;
        std::cout << "  save <file>            Save current configuration to file" << std::endl;
        std::cout << "  load <file>            Load configuration from file" << std::endl;
        std::cout << "" << std::endl;
        std::cout << "Advanced configuration categories:" << std::endl;
        std::cout << "  - Performance settings (threading, SIMD, memory pools)" << std::endl;
        std::cout << "  - Plasticity settings (STDP, Hebbian, neuromodulation)" << std::endl;
        std::cout << "  - Memory settings (working memory, episodic memory)" << std::endl;
        std::cout << "  - Visualization settings (output formats, logging)" << std::endl;
        std::cout << "  - Export/import settings (compression, validation)" << std::endl;
        std::cout << "  - Network topology settings (regions, connectivity)" << std::endl;
        std::cout << "  - Learning settings (development, adaptation)" << std::endl;
    }
    
    void setConfig(const std::string& key, const std::string& value) {
        // Try to parse value as different types
        try {
            size_t pos;
            int intValue = std::stoi(value);
            config_->set(key, intValue);
            std::cout << "Set " << key << " = " << intValue << std::endl;
            return;
        } catch (...) {}
        
        try {
            double doubleValue = std::stod(value);
            config_->set(key, doubleValue);
            std::cout << "Set " << key << " = " << doubleValue << std::endl;
            return;
        } catch (...) {}
        
        try {
            if (value == "true" || value == "false") {
                bool boolValue = (value == "true");
                config_->set(key, boolValue);
                std::cout << "Set " << key << " = " << (boolValue ? "true" : "false") << std::endl;
                return;
            }
        } catch (...) {}
        
        // Default to string
        config_->set(key, value);
        std::cout << "Set " << key << " = '" << value << "'" << std::endl;
    }
    
    void getConfig(const std::string& key) {
        if (config_->has(key)) {
            auto value = config_->get<std::string>(key);
            if (value.has_value()) {
                std::cout << key << " = '" << value.value() << "'" << std::endl;
            } else {
                std::cout << "Warning: Could not retrieve value type for key: " << key << std::endl;
            }
        } else {
            std::cout << "Key not found: " << key << std::endl;
        }
    }
    
    void showConfig() {
        std::cout << "=== Current Configuration ===" << std::endl;
        auto keys = config_->getKeys();
        for (const auto& key : keys) {
            auto value = config_->get<std::string>(key);
            if (value.has_value()) {
                std::cout << key << " = '" << value.value() << "'" << std::endl;
            } else {
                std::cout << key << " = <unknown type>" << std::endl;
            }
        }
    }
    
    void applyAdvancedConfig() {
        advancedConfig_->applyToConfig(*config_);
        std::cout << "Applied advanced configuration" << std::endl;
        std::cout << "Advanced configuration includes:" << std::endl;
        std::cout << "  - Performance optimizations (threading, SIMD, memory pools)" << std::endl;
        std::cout << "  - Advanced plasticity rules" << std::endl;
        std::cout << "  - Memory management optimizations" << std::endl;
        std::cout << "  - Visualization and logging settings" << std::endl;
        std::cout << "  - Export/import configurations" << std::endl;
        std::cout << "  - Network topology settings" << std::endl;
        std::cout << "  - Learning and adaptation mechanisms" << std::endl;
    }
    
    void applyOptimizedConfig() {
        auto optimizedConfig = AdvancedConfig::createOptimized();
        optimizedConfig.applyToConfig(*config_);
        std::cout << "Applied optimized configuration" << std::endl;
        std::cout << "Optimized configuration includes:" << std::endl;
        std::cout << "  - Performance optimizations for speed" << std::endl;
        std::cout << "  - Memory-efficient settings" << std::endl;
        std::cout << "  - Balanced plasticity parameters" << std::endl;
        std::cout << "  - Enhanced learning mechanisms" << std::endl;
    }
    
    void saveConfig(const std::string& filename) {
        if (config_->saveToFile(filename)) {
            std::cout << "Configuration saved to: " << filename << std::endl;
        } else {
            std::cout << "Failed to save configuration to: " << filename << std::endl;
        }
    }
    
    void loadConfig(const std::string& filename) {
        // Clear current config
        config_->clear();
        
        if (config_->loadFromFile(filename)) {
            std::cout << "Configuration loaded from: " << filename << std::endl;
        } else {
            std::cout << "Failed to load configuration from: " << filename << std::endl;
        }
    }
};

int main(int argc, char* argv[]) {
    // If command line arguments are provided, use them
    if (argc > 1) {
        // For now, just show help
        std::cout << "Command line usage not yet implemented." << std::endl;
        std::cout << "Run './nlm_advanced_cli' for interactive mode." << std::endl;
        return 0;
    }
    
    // Run interactive CLI
    AdvancedCLI cli;
    cli.run();
    
    return 0;
}
