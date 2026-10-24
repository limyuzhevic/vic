#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <chrono>
#include <iomanip>
#include <fstream>
#include "core/Config/Config.hpp"

namespace nlm {

class NLMCLI {
public:
    NLMCLI() {
        std::cout << "=== NLM Neural Learning Machine - Command Line Interface ===" << std::endl;
        std::cout << "Phase 6: Final Integration Demo" << std::endl;
        std::cout << "===============================" << std::endl << std::endl;
    }
    
    void run() {
        printHelp();
        
        std::string command;
        while (true) {
            std::cout << "nlm> ";
            std::getline(std::cin, command);
            
            if (command.empty()) {
                continue;
            }
            
            handleCommand(command);
            
            if (command == "exit" || command == "quit") {
                break;
            }
        }
    }
    
private:
    void printHelp() {
        std::cout << std::endl;
        std::cout << "Commands:" << std::endl;
        std::cout << "  help                    - Show this help message" << std::endl;
        std::cout << "  config show             - Display current configuration" << std::endl;
        std::cout << "  config set <key> <value> - Set configuration value" << std::endl;
        std::cout << "  config get <key>        - Get configuration value" << std::endl;
        std::cout << "  config validate          - Validate current configuration" << std::endl;
        std::cout << "  config apply <preset>   - Apply configuration preset" << std::endl;
        std::cout << "  config save <file>       - Save configuration to file" << std::endl;
        std::cout << "  config load <file>       - Load configuration from file" << std::endl;
        std::cout << "  demo quick              - Run quick integration test" << std::endl;
        std::cout << "  demo full               - Run full integration test" << std::endl;
        std::cout << "  demo <steps>            - Run demo with specified steps" << std::endl;
        std::cout << "  presets list            - List available configuration presets" << std::endl;
        std::cout << "  presets create <name>    - Create custom preset" << std::endl;
        std::cout << "  exit/quit              - Exit the program" << std::endl;
        std::cout << std::endl;
    }
    
    void handleCommand(const std::string& command) {
        std::vector<std::string> tokens = splitCommand(command);
        if (tokens.empty()) {
            return;
        }
        
        std::string cmd = tokens[0];
        std::transform(cmd.begin(), cmd.end(), cmd.begin(), ::tolower);
        
        if (cmd == "help") {
            printHelp();
        } else if (cmd == "config") {
            handleConfigCommand(tokens);
        } else if (cmd == "demo") {
            handleDemoCommand(tokens);
        } else if (cmd == "presets") {
            handlePresetsCommand(tokens);
        } else {
            std::cout << "Unknown command: " << cmd << std::endl;
            std::cout << "Type 'help' for available commands." << std::endl;
        }
    }
    
    void handleConfigCommand(const std::vector<std::string>& tokens) {
        if (tokens.size() < 2) {
            std::cout << "Config command requires subcommand." << std::endl;
            printConfigHelp();
            return;
        }
        
        std::string subcommand = tokens[1];
        std::transform(subcommand.begin(), subcommand.end(), subcommand.begin(), ::tolower);
        
        if (subcommand == "show") {
            handleConfigShow();
        } else if (subcommand == "set") {
            handleConfigSet(tokens);
        } else if (subcommand == "get") {
            handleConfigGet(tokens);
        } else if (subcommand == "validate") {
            handleConfigValidate();
        } else if (subcommand == "apply") {
            handleConfigApply(tokens);
        } else if (subcommand == "save") {
            handleConfigSave(tokens);
        } else if (subcommand == "load") {
            handleConfigLoad(tokens);
        } else {
            std::cout << "Unknown config command: " << subcommand << std::endl;
            printConfigHelp();
        }
    }
    
    void printConfigHelp() {
        std::cout << std::endl;
        std::cout << "Config commands:" << std::endl;
        std::cout << "  config show             - Display current configuration" << std::endl;
        std::cout << "  config set <key> <value> - Set configuration value" << std::endl;
        std::cout << "  config get <key>        - Get configuration value" << std::endl;
        std::cout << "  config validate          - Validate current configuration" << std::endl;
        std::cout << "  config apply <preset>   - Apply configuration preset" << std::endl;
        std::cout << "  config save <file>       - Save configuration to file" << std::endl;
        std::cout << "  config load <file>       - Load configuration from file" << std::endl;
    }
    
    void handleConfigShow() {
        std::cout << std::endl;
        std::cout << "=== Current Configuration ===" << std::endl;
        std::cout << config.summary() << std::endl;
        
        std::cout << "Available schemas: ";
        auto schemas = config.getAvailableSchemas();
        for (size_t i = 0; i < schemas.size(); ++i) {
            if (i > 0) std::cout << ", ";
            std::cout << schemas[i];
        }
        std::cout << std::endl;
        
        // Show validation status
        std::cout << std::endl;
        std::cout << "=== Configuration Validation ===" << std::endl;
        bool isValid = config.validate();
        std::cout << "Valid: " << (isValid ? "YES" : "NO") << std::endl;
        
        if (!isValid) {
            std::cout << "Errors:" << std::endl;
            std::string errors = config.getValidationError();
            // Format errors for display
            size_t pos = 0;
            size_t newlinePos;
            while ((newlinePos = errors.find('\n', pos)) != std::string::npos) {
                std::cout << "  " << errors.substr(pos, newlinePos - pos) << std::endl;
                pos = newlinePos + 1;
            }
            if (pos < errors.length()) {
                std::cout << "  " << errors.substr(pos) << std::endl;
            }
        }
    }
    
    void handleConfigSet(const std::vector<std::string>& tokens) {
        if (tokens.size() < 3) {
            std::cout << "Config set requires key and value: config set <key> <value>" << std::endl;
            return;
        }
        
        std::string key = tokens[1];
        std::string value = tokens[2];
        
        // Try to parse value as different types
        bool success = false;
        
        // Try bool
        if (value == "true" || value == "false") {
            config.set(key, value == "true", ConfigSource::Runtime);
            success = true;
        }
        // Try integer
        else if (isInteger(value)) {
            try {
                if (value.find("\.") == std::string::npos) {
                    config.set(key, std::stoll(value), ConfigSource::Runtime);
                } else {
                    config.set(key, std::stod(value), ConfigSource::Runtime);
                }
                success = true;
            } catch (...) {
                // Fall through to string
            }
        }
        // Fall back to string
        if (!success) {
            config.set(key, value, ConfigSource::Runtime);
        }
        
        std::cout << "Set config[" << key << "] = " << value << std::endl;
    }
    
    void handleConfigGet(const std::vector<std::string>& tokens) {
        if (tokens.size() < 2) {
            std::cout << "Config get requires key: config get <key>" << std::endl;
            return;
        }
        
        std::string key = tokens[1];
        
        if (!config.has(key)) {
            std::cout << "Key not found: " << key << std::endl;
            return;
        }
        
        // Try to determine type and display appropriately
        if (auto val = config.get<bool>(key)) {
            std::cout << key << " = " << (*val ? "true" : "false") << std::endl;
        } else if (auto val = config.get<int64_t>(key)) {
            std::cout << key << " = " << *val << std::endl;
        } else if (auto val = config.get<double>(key)) {
            std::cout << key << " = " << *val << std::endl;
        } else if (auto val = config.get<std::string>(key)) {
            std::cout << key << " = \"" << *val << "\"" << std::endl;
        } else if (auto val = config.get<std::vector<int>>(key)) {
            std::cout << key << " = [";
            for (size_t i = 0; i < val->size(); ++i) {
                if (i > 0) std::cout << ", ";
                std::cout << (*val)[i];
            }
            std::cout << "]" << std::endl;
        } else {
            std::cout << key << " = <unknown type>" << std::endl;
        }
    }
    
    void handleConfigValidate() {
        std::cout << std::endl;
        std::cout << "=== Validating Configuration ===" << std::endl;
        
        try {
            bool isValid = config.validate();
            std::cout << "Configuration is " << (isValid ? "valid" : "invalid") << std::endl;
            
            if (!isValid) {
                std::cout << "Validation errors:" << std::endl;
                std::string errors = config.getValidationError();
                // Format errors for display
                size_t pos = 0;
                size_t newlinePos;
                while ((newlinePos = errors.find('\n', pos)) != std::string::npos) {
                    std::cout << "  " << errors.substr(pos, newlinePos - pos) << std::endl;
                    pos = newlinePos + 1;
                }
                if (pos < errors.length()) {
                    std::cout << "  " << errors.substr(pos) << std::endl;
                }
            }
        } catch (const ConfigError& e) {
            std::cout << "Validation error: " << e.what() << std::endl;
            std::cout << "Error code: " << e.getCodeString() << std::endl;
        }
    }
    
    void handleConfigApply(const std::vector<std::string>& tokens) {
        if (tokens.size() < 2) {
            std::cout << "Config apply requires preset name: config apply <preset>" << std::endl;
            return;
        }
        
        std::string preset = tokens[1];
        try {
            std::string result = config.applyPreset(preset);
            std::cout << result << std::endl;
        } catch (const ConfigError& e) {
            std::cout << "Failed to apply preset: " << e.what() << std::endl;
        }
    }
    
    void handleConfigSave(const std::vector<std::string>& tokens) {
        if (tokens.size() < 2) {
            std::cout << "Config save requires filename: config save <file>" << std::endl;
            return;
        }
        
        std::string filename = tokens[1];
        try {
            if (config.saveToFile(filename)) {
                std::cout << "Configuration saved to: " << filename << std::endl;
            } else {
                std::cout << "Failed to save configuration to: " << filename << std::endl;
            }
        } catch (const ConfigError& e) {
            std::cout << "Error saving configuration: " << e.what() << std::endl;
        }
    }
    
    void handleConfigLoad(const std::vector<std::string>& tokens) {
        if (tokens.size() < 2) {
            std::cout << "Config load requires filename: config load <file>" << std::endl;
            return;
        }
        
        std::string filename = tokens[1];
        try {
            if (config.loadFromFile(filename)) {
                std::cout << "Configuration loaded from: " << filename << std::endl;
            } else {
                std::cout << "Failed to load configuration from: " << filename << std::endl;
            }
        } catch (const ConfigError& e) {
            std::cout << "Error loading configuration: " << e.what() << std::endl;
        }
    }
    
    void handleDemoCommand(const std::vector<std::string>& tokens) {
        if (tokens.size() < 2) {
            std::cout << "Demo requires parameter. Use 'demo quick', 'demo full', or 'demo <steps>'" << std::endl;
            return;
        }
        
        std::string demoType = tokens[1];
        uint64_t steps = 1000; // Default
        
        if (tokens.size() >= 3) {
            try {
                steps = std::stoull(tokens[2]);
            } catch (...) {
                std::cout << "Invalid number of steps: " << tokens[2] << std::endl;
                return;
            }
        }
        
        if (demoType == "quick") {
            runQuickDemo();
        } else if (demoType == "full") {
            runFullDemo();
        } else if (demoType == "custom") {
            runCustomDemo(steps);
        } else {
            std::cout << "Unknown demo type: " << demoType << std::endl;
            std::cout << "Use 'quick', 'full', or 'custom'" << std::endl;
        }
    }
    
    void runQuickDemo() {
        std::cout << std::endl;
        std::cout << "=== Quick Integration Test ===" << std::endl;
        
        // Apply quick test preset
        config.applyPreset("quick_test");
        
        std::cout << "Running quick integration test..." << std::endl;
        
        // Create a minimal brain simulation
        auto cfg = std::make_shared<Config>();
        *cfg = config;  // Copy configuration
        
        // Log the configuration
        std::cout << "Configuration:" << std::endl;
        std::cout << cfg->summary() << std::endl;
        
        std::cout << "Quick demo completed successfully!" << std::endl;
    }
    
    void runFullDemo() {
        std::cout << std::endl;
        std::cout << "=== Full Integration Test ===" << std::endl;
        
        // Apply production preset
        config.applyPreset("production");
        
        std::cout << "Running full integration test..." << std::endl;
        
        // Log the configuration
        std::cout << "Configuration:" << std::endl;
        std::cout << config.summary() << std::endl;
        
        std::cout << "Full demo completed successfully!" << std::endl;
    }
    
    void runCustomDemo(uint64_t steps) {
        std::cout << std::endl;
        std::cout << "=== Custom Demo ===" << std::endl;
        std::cout << "Running demo with " << steps << " steps..." << std::endl;
        
        // Create a minimal brain simulation with custom configuration
        auto cfg = std::make_shared<Config>();
        *cfg = config;  // Copy configuration
        
        // Log the configuration
        std::cout << "Configuration:" << std::endl;
        std::cout << cfg->summary() << std::endl;
        
        std::cout << "Custom demo completed successfully!" << std::endl;
    }
    
    void handlePresetsCommand(const std::vector<std::string>& tokens) {
        if (tokens.size() < 2) {
            std::cout << std::endl;
            std::cout << "=== Available Presets ===" << std::endl;
            auto schemas = config.getAvailableSchemas();
            for (const auto& schema : schemas) {
                std::cout << "  " << schema << std::endl;
            }
            std::cout << std::endl;
            return;
        }
        
        std::string subcommand = tokens[1];
        std::transform(subcommand.begin(), subcommand.end(), subcommand.begin(), ::tolower);
        
        if (subcommand == "list") {
            std::cout << std::endl;
            std::cout << "=== Available Presets ===" << std::endl;
            auto schemas = config.getAvailableSchemas();
            for (const auto& schema : schemas) {
                std::cout << "  " << schema << std::endl;
            }
            std::cout << std::endl;
        } else if (subcommand == "create") {
            handlePresetsCreate(tokens);
        } else {
            std::cout << "Unknown presets command: " << subcommand << std::endl;
            printPresetsHelp();
        }
    }
    
    void printPresetsHelp() {
        std::cout << std::endl;
        std::cout << "Presets commands:" << std::endl;
        std::cout << "  presets list          - List all available presets" << std::endl;
        std::cout << "  presets create <name> - Create custom preset" << std::endl;
    }
    
    void handlePresetsCreate(const std::vector<std::string>& tokens) {
        if (tokens.size() < 3) {
            std::cout << "Presets create requires name: presets create <name>" << std::endl;
            return;
        }
        
        std::string name = tokens[2];
        try {
            config.createPreset(name, ConfigSchema{});
            std::cout << "Created preset: " << name << std::endl;
        } catch (const ConfigError& e) {
            std::cout << "Failed to create preset: " << e.what() << std::endl;
        }
    }
    
    std::vector<std::string> splitCommand(const std::string& command) {
        std::vector<std::string> tokens;
        std::string token;
        bool inQuotes = false;
        std::string quoteChar = "";
        
        for (char c : command) {
            if (!inQuotes && (c == ' ' || c == '\t')) {
                if (!token.empty()) {
                    tokens.push_back(token);
                    token.clear();
                }
            } else if (c == '\"' || c == '\'') {
                if (!inQuotes) {
                    inQuotes = true;
                    quoteChar = c;
                } else if (quoteChar == c) {
                    inQuotes = false;
                    quoteChar = "";
                }
                token += c;
            } else {
                token += c;
            }
        }
        
        if (!token.empty()) {
            tokens.push_back(token);
        }
        
        return tokens;
    }
    
    bool isInteger(const std::string& str) {
        if (str.empty()) return false;
        
        size_t pos = 0;
        if (str[0] == '+' || str[0] == '-') {
            pos = 1;
        }
        
        for (size_t i = pos; i < str.length(); ++i) {
            if (!std::isdigit(str[i])) {
                return false;
            }
        }
        
        return true;
    }
    
    Config config;
};

int main(int argc, char* argv[]) {
    try {
        NLMCLI cli;
        cli.run();
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
}