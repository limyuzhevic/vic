// NLM (熙然) - Neural Learning Machine
// Configuration management tool
// Provides utilities to query and modify configuration files

#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include <iostream>
#include <string>
#include <vector>
#include <iomanip>
#include <fstream>

using namespace nlm;

void printHelp() {
    std::cout << "NLM Config Tool - Configuration File Manager" << std::endl;
    std::cout << "======================================" << std::endl;
    std::cout << std::endl;
    std::cout << "Usage: nlm_config <command> [options]" << std::endl;
    std::cout << std::endl;
    std::cout << "Commands:" << std::endl;
    std::cout << "  show <file>          Display configuration contents" << std::endl;
    std::cout << "  set <file> <key> <value>  Set configuration value" << std::endl;
    std::cout << "  get <file> <key>     Get configuration value" << std::endl;
    std::cout << "  remove <file> <key>  Remove configuration key" << std::endl;
    std::cout << "  keys <file>          List all configuration keys" << std::endl;
    std::cout << "  import <src> <dest>  Import configuration from file" << std::endl;
    std::cout << "  export <file> <dest>  Export configuration to file" << std::endl;
    std::cout << "  validate <file>      Validate configuration file" << std::endl;
    std::cout << "  help                 Show this help message" << std::endl;
    std::cout << std::endl;
    std::cout << "Value Types:" << std::endl;
    std::cout << "  - Boolean: true/false" << std::endl;
    std::cout << "  - Integer: 42" << std::endl;
    std::cout << "  - Float: 3.14" << std::endl;
    std::cout << "  - String: \"hello world\"" << std::endl;
    std::cout << std::endl;
}

void showConfig(const std::string& filepath) {
    auto config = std::make_shared<Config>();
    
    if (!config->loadFromFile(filepath)) {
        std::cerr << "Error: Failed to load configuration file: " << filepath << std::endl;
        return;
    }

    std::cout << "Configuration: " << filepath << std::endl;
    std::cout << "=======================" << std::endl;
    std::cout << config->summary() << std::endl;
}

void getConfigValue(const std::string& filepath, const std::string& key) {
    auto config = std::make_shared<Config>();
    
    if (!config->loadFromFile(filepath)) {
        std::cerr << "Error: Failed to load configuration file: " << filepath << std::endl;
        return;
    }

    if (!config->has(key)) {
        std::cerr << "Error: Key not found: " << key << std::endl;
        return;
    }

    // Try to get as different types
    if (config->has("name")) {  // Check if it's a string
        auto val = config->getOr<std::string>(key, "");
        std::cout << "Key: " << key << std::endl;
        std::cout << "Value: \"" << val << "\" (String)" << std::endl;
    } else if (config->has("enabled")) {  // Check if it's a boolean
        auto val = config->getOr<bool>(key, false);
        std::cout << "Key: " << key << std::endl;
        std::cout << "Value: " << (val ? "true" : "false") << " (Boolean)" << std::endl;
    } else {
        auto val = config->getOr<double>(key, 0.0);
        std::cout << "Key: " << key << std::endl;
        std::cout << "Value: " << val << " (Numeric)" << std::endl;
    }
}

void setConfigValue(const std::string& filepath, const std::string& key, const std::string& value) {
    auto config = std::make_shared<Config>();
    
    // Try to load existing config or create new
    if (!filepath.empty() && config->loadFromFile(filepath)) {
        // Config loaded, will update
    } else {
        config = std::make_shared<Config>();
        std::cout << "Created new configuration (file: " << filepath << ")" << std::endl;
    }

    // Determine value type
    if (value == "true" || value == "false") {
        bool boolVal = (value == "true");
        config->set(key, boolVal, ConfigSource::Runtime);
        std::cout << "Set " << key << " = " << (boolVal ? "true" : "false") << " (Boolean)" << std::endl;
    } else if (value.find('.') != std::string::npos) {
        try {
            double floatVal = std::stod(value);
            config->set(key, floatVal, ConfigSource::Runtime);
            std::cout << "Set " << key << " = " << floatVal << " (Float)" << std::endl;
        } catch (...) {
            // Fall through to string
            config->set(key, value, ConfigSource::Runtime);
            std::cout << "Set " << key << " = \"" << value << "\" (String)" << std::endl;
        }
    } else {
        try {
            long long intVal = std::stoll(value);
            config->set(key, intVal, ConfigSource::Runtime);
            std::cout << "Set " << key << " = " << intVal << " (Integer)" << std::endl;
        } catch (...) {
            // Fall through to string
            config->set(key, value, ConfigSource::Runtime);
            std::cout << "Set " << key << " = \"" << value << "\" (String)" << std::endl;
        }
    }

    // Save if file was provided
    if (!filepath.empty()) {
        if (config->saveToFile(filepath)) {
            std::cout << "Configuration saved to: " << filepath << std::endl;
        } else {
            std::cerr << "Error: Failed to save configuration to: " << filepath << std::endl;
        }
    }
}

void removeConfigKey(const std::string& filepath, const std::string& key) {
    auto config = std::make_shared<Config>();
    
    if (!config->loadFromFile(filepath)) {
        std::cerr << "Error: Failed to load configuration file: " << filepath << std::endl;
        return;
    }

    if (!config->has(key)) {
        std::cerr << "Error: Key not found: " << key << std::endl;
        return;
    }

    config->remove(key);
    
    if (config->saveToFile(filepath)) {
        std::cout << "Key removed: " << key << std::endl;
        std::cout << "Configuration saved to: " << filepath << std::endl;
    } else {
        std::cerr << "Error: Failed to save configuration to: " << filepath << std::endl;
    }
}

void listConfigKeys(const std::string& filepath) {
    auto config = std::make_shared<Config>();
    
    if (!config->loadFromFile(filepath)) {
        std::cerr << "Error: Failed to load configuration file: " << filepath << std::endl;
        return;
    }

    std::cout << "Configuration keys in: " << filepath << std::endl;
    std::cout << "=======================" << std::endl;
    
    auto keys = config->getKeys();
    std::cout << "Total keys: " << keys.size() << std::endl;
    std::cout << std::endl;
    
    for (const auto& key : keys) {
        std::cout << "  - " << key << std::endl;
    }
}

void importConfig(const std::string& sourceFile, const std::string& destFile) {
    auto config = std::make_shared<Config>();
    
    if (!config->loadFromFile(sourceFile)) {
        std::cerr << "Error: Failed to load source configuration: " << sourceFile << std::endl;
        return;
    }
    
    if (config->saveToFile(destFile)) {
        std::cout << "Configuration imported from: " << sourceFile << std::endl;
        std::cout << "Saved to: " << destFile << std::endl;
    } else {
        std::cerr << "Error: Failed to save imported configuration to: " << destFile << std::endl;
    }
}

void exportConfig(const std::string& sourceFile, const std::string& destFile) {
    importConfig(sourceFile, destFile);
}

void validateConfig(const std::string& filepath) {
    auto config = std::make_shared<Config>();
    
    std::cout << "Validating configuration: " << filepath << std::endl;
    std::cout << "=========================" << std::endl;
    
    if (!config->loadFromFile(filepath)) {
        std::cout << "✗ Failed to load file" << std::endl;
        return;
    }
    
    std::cout << "✓ File loaded successfully" << std::endl;
    
    auto keys = config->getKeys();
    std::cout << "✓ Contains " << keys.size() << " configuration entries" << std::endl;
    
    // Check for common issues
    bool hasIssues = false;
    
    for (const auto& key : keys) {
        if (key.empty()) {
            std::cout << "✗ Warning: Empty key found" << std::endl;
            hasIssues = true;
        }
    }
    
    // Display summary
    std::cout << std::endl;
    std::cout << "Configuration Summary:" << std::endl;
    std::cout << config->summary() << std::endl;
    
    if (!hasIssues) {
        std::cout << "✓ Configuration appears valid" << std::endl;
    } else {
        std::cout << "⚠ Configuration has potential issues" << std::endl;
    }
}

int main(int argc, char** argv) {
    if (argc < 2) {
        printHelp();
        return 1;
    }

    std::string command = argv[1];
    
    if (command == "help") {
        printHelp();
        return 0;
    }
    
    try {
        if (command == "show") {
            if (argc < 3) {
                std::cerr << "Error: Missing file path" << std::endl;
                return 1;
            }
            showConfig(argv[2]);
        } else if (command == "get") {
            if (argc < 4) {
                std::cerr << "Error: Missing key parameter" << std::endl;
                return 1;
            }
            getConfigValue(argv[2], argv[3]);
        } else if (command == "set") {
            if (argc < 5) {
                std::cerr << "Error: Missing key or value parameter" << std::endl;
                return 1;
            }
            // Join remaining arguments as value (supports values with spaces)
            std::string value;
            for (int i = 4; i < argc; ++i) {
                if (i > 4) value += " ";
                value += argv[i];
            }
            setConfigValue(argv[2], argv[3], value);
        } else if (command == "remove") {
            if (argc < 4) {
                std::cerr << "Error: Missing key parameter" << std::endl;
                return 1;
            }
            removeConfigKey(argv[2], argv[3]);
        } else if (command == "keys") {
            if (argc < 3) {
                std::cerr << "Error: Missing file path" << std::endl;
                return 1;
            }
            listConfigKeys(argv[2]);
        } else if (command == "import") {
            if (argc < 4) {
                std::cerr << "Error: Missing source or destination file" << std::endl;
                return 1;
            }
            importConfig(argv[2], argv[3]);
        } else if (command == "export") {
            if (argc < 4) {
                std::cerr << "Error: Missing source or destination file" << std::endl;
                return 1;
            }
            exportConfig(argv[2], argv[3]);
        } else if (command == "validate") {
            if (argc < 3) {
                std::cerr << "Error: Missing file path" << std::endl;
                return 1;
            }
            validateConfig(argv[2]);
        } else {
            std::cerr << "Error: Unknown command: " << command << std::endl;
            printHelp();
            return 1;
        }
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}
