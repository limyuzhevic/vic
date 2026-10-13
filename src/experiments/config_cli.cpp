// File: /workspace/app/src/experiments/config_cli.cpp
// NLM Configuration Management Tool
// Provides advanced configuration management capabilities

#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include <iostream>
#include <string>
#include <memory>
#include <fstream>
#include <sstream>
#include <vector>
#include <algorithm>
#include <iomanip>

namespace nlm {

class ConfigCLI {
public:
    ConfigCLI() {
        registerCommands();
    }
    
    ~ConfigCLI() = default;
    
    bool run(int argc, char** argv);
    
private:
    struct Command {
        std::string name;
        std::string description;
        std::function<bool(int, char**)> handler;
    };
    
    std::vector<Command> commands_;
    std::shared_ptr<Config> config_;
    bool dryRun_ = false;
    std::string outputFormat_ = "text";
    
    void registerCommands() {
        // Core config commands
        registerCommand("get", "Get configuration value",
                       [this](int argc, char** argv) { return handleGet(argc, argv); });
        
        registerCommand("set", "Set configuration value",
                       [this](int argc, char** argv) { return handleSet(argc, argv); });
        
        registerCommand("load", "Load configuration from file",
                       [this](int argc, char** argv) { return handleLoad(argc, argv); });
        
        registerCommand("save", "Save configuration to file",
                       [this](int argc, char** argv) { return handleSave(argc, argv); });
        
        registerCommand("validate", "Validate configuration",
                       [this](int argc, char** argv) { return handleValidate(argc, argv); });
        
        registerCommand("merge", "Merge configuration from file",
                       [this](int argc, char** argv) { return handleMerge(argc, argv); });
        
        registerCommand("export", "Export configuration to different formats",
                       [this](int argc, char** argv) { return handleExport(argc, argv); });
        
        registerCommand("import", "Import configuration from different formats",
                       [this](int argc, char** argv) { return handleImport(argc, argv); });
        
        registerCommand("diff", "Show differences between configs",
                       [this](int argc, char** argv) { return handleDiff(argc, argv); });
        
        registerCommand("history", "Show configuration change history",
                       [this](int argc, char** argv) { return handleHistory(argc, argv); });
        
        registerCommand("reset", "Reset configuration to defaults",
                       [this](int argc, char** argv) { return handleReset(argc, argv); });
        
        // Advanced commands
        registerCommand("profile", "Profile configuration and suggest optimizations",
                       [this](int argc, char** argv) { return handleProfile(argc, argv); });
        
        registerCommand("template", "Create configuration template",
                       [this](int argc, char** argv) { return handleTemplate(argc, argv); });
        
        registerCommand("search", "Search for configuration keys",
                       [this](int argc, char** argv) { return handleSearch(argc, argv); });
        
        // Utility commands
        registerCommand("list", "List all configuration keys",
                       [this](int argc, char** argv) { return handleList(argc, argv); });
        
        registerCommand("count", "Count configuration entries",
                       [this](int argc, char** argv) { return handleCount(argc, argv); });
        
        registerCommand("clear", "Clear configuration",
                       [this](int argc, char** argv) { return handleClear(argc, argv); });
        
        registerCommand("backup", "Create backup of current configuration",
                       [this](int argc, char** argv) { return handleBackup(argc, argv); });
    }
    
    void registerCommand(const std::string& name, const std::string& description,
                        std::function<bool(int, char**)> handler) {
        Command cmd;
        cmd.name = name;
        cmd.description = description;
        cmd.handler = handler;
        commands_.push_back(cmd);
    }
    
    bool handleGet(int argc, char** argv) {
        if (argc < 3) {
            printError("Usage: config-cli get <key> [--type type]");
            return false;
        }
        
        std::string key = argv[2];
        std::string type = "auto";
        
        // Parse options
        for (int i = 3; i < argc; ++i) {
            std::string arg = argv[i];
            if (arg == "--type" && i + 1 < argc) {
                type = argv[++i];
            }
        }
        
        if (!config_) {
            printError("No configuration loaded");
            return false;
        }
        
        if (!config_->has(key)) {
            printError("Key not found: " + key);
            return false;
        }
        
        auto val = config_->get<std::string>(key);
        if (!val.has_value()) {
            // Try other types
            if (type == "int" || type == "auto") {
                auto intVal = config_->get<int64_t>(key);
                if (intVal.has_value()) {
                    printInfo(std::to_string(intVal.value()));
                    return true;
                }
            }
            if (type == "double" || type == "auto") {
                auto doubleVal = config_->get<double>(key);
                if (doubleVal.has_value()) {
                    printInfo(std::to_string(doubleVal.value()));
                    return true;
                }
            }
            if (type == "bool" || type == "auto") {
                auto boolVal = config_->get<bool>(key);
                if (boolVal.has_value()) {
                    printInfo(boolVal.value() ? "true" : "false");
                    return true;
                }
            }
            
            printError("Cannot get value for key: " + key);
            return false;
        }
        
        printInfo(val.value());
        return true;
    }
    
    bool handleSet(int argc, char** argv) {
        if (argc < 4) {
            printError("Usage: config-cli set <key> <value> [--type type]");
            return false;
        }
        
        std::string key = argv[2];
        std::string value = argv[3];
        std::string type = "string";
        
        // Parse options
        for (int i = 4; i < argc; ++i) {
            std::string arg = argv[i];
            if (arg == "--type" && i + 1 < argc) {
                type = argv[++i];
            }
        }
        
        if (!config_) {
            config_ = std::make_shared<Config>();
        }
        
        if (type == "int") {
            try {
                long val = std::stol(value);
                config_->set(key, val);
            } catch (...) {
                printError("Invalid integer value: " + value);
                return false;
            }
        } else if (type == "double") {
            try {
                double val = std::stod(value);
                config_->set(key, val);
            } catch (...) {
                printError("Invalid double value: " + value);
                return false;
            }
        } else if (type == "bool") {
            config_->set(key, (value == "true" || value == "1" || value == "yes"));
        } else {
            config_->set(key, value);
        }
        
        printInfo("Set " + key + " = " + value);
        return true;
    }
    
    bool handleLoad(int argc, char** argv) {
        if (argc < 3) {
            printError("Usage: config-cli load <file> [--validate]");
            return false;
        }
        
        std::string filepath = argv[2];
        bool validate = false;
        
        // Parse options
        for (int i = 3; i < argc; ++i) {
            std::string arg = argv[i];
            if (arg == "--validate") {
                validate = true;
            }
        }
        
        if (dryRun_) {
            printInfo("(DRY RUN) Would load config from: " + filepath);
            return true;
        }
        
        auto newConfig = std::make_shared<Config>();
        if (!newConfig->loadFromFile(filepath)) {
            printError("Failed to load config from: " + filepath);
            return false;
        }
        
        if (validate) {
            if (!validateConfig(newConfig)) {
                printError("Config validation failed");
                return false;
            }
        }
        
        config_ = newConfig;
        printInfo("Config loaded from: " + filepath);
        return true;
    }
    
    bool handleSave(int argc, char** argv) {
        if (argc < 3) {
            printError("Usage: config-cli save <file> [--format format]");
            return false;
        }
        
        std::string filepath = argv[2];
        std::string format = "text";
        
        // Parse options
        for (int i = 3; i < argc; ++i) {
            std::string arg = argv[i];
            if (arg == "--format" && i + 1 < argc) {
                format = argv[++i];
            }
        }
        
        if (!config_) {
            printError("No configuration to save");
            return false;
        }
        
        if (dryRun_) {
            printInfo("(DRY RUN) Would save config to: " + filepath + " (format: " + format + ")");
            return true;
        }
        
        if (format == "text") {
            if (!config_->saveToFile(filepath)) {
                printError("Failed to save config to: " + filepath);
                return false;
            }
        } else {
            printError("Unsupported format: " + format);
            return false;
        }
        
        printInfo("Config saved to: " + filepath);
        return true;
    }
    
    bool handleValidate(int argc, char** argv) {
        printInfo("Validating configuration...");
        
        bool strict = false;
        for (int i = 2; i < argc; ++i) {
            if (std::string(argv[i]) == "--strict") {
                strict = true;
            }
        }
        
        return validateConfig(config_, strict);
    }
    
    bool handleMerge(int argc, char** argv) {
        if (argc < 3) {
            printError("Usage: config-cli merge <file> [--overwrite]");
            return false;
        }
        
        std::string filepath = argv[2];
        bool overwrite = false;
        
        for (int i = 3; i < argc; ++i) {
            if (std::string(argv[i]) == "--overwrite") {
                overwrite = true;
            }
        }
        
        if (!config_) {
            printError("No configuration to merge into");
            return false;
        }
        
        auto newConfig = std::make_shared<Config>();
        if (!newConfig->loadFromFile(filepath)) {
            printError("Failed to load config from: " + filepath);
            return false;
        }
        
        // Merge configs
        for (const auto& key : newConfig->getKeys()) {
            if (!config_->has(key) || overwrite) {
                auto val = newConfig->get<std::string>(key);
                if (val.has_value()) {
                    config_->set(key, val.value());
                }
            }
        }
        
        printInfo("Configuration merged from: " + filepath);
        return true;
    }
    
    bool handleExport(int argc, char** argv) {
        if (argc < 3) {
            printError("Usage: config-cli export <format> <output>");
            return false;
        }
        
        std::string format = argv[2];
        std::string output = argv[3];
        
        if (!config_) {
            printError("No configuration to export");
            return false;
        }
        
        if (format == "json") {
            exportToJSON(output);
        } else if (format == "ini") {
            exportToINI(output);
        } else if (format == "yaml") {
            exportToYAML(output);
        } else {
            printError("Unsupported export format: " + format);
            return false;
        }
        
        return true;
    }
    
    bool handleImport(int argc, char** argv) {
        if (argc < 4) {
            printError("Usage: config-cli import <format> <input> [--merge]");
            return false;
        }
        
        std::string format = argv[2];
        std::string input = argv[3];
        bool merge = false;
        
        for (int i = 4; i < argc; ++i) {
            if (std::string(argv[i]) == "--merge") {
                merge = true;
            }
        }
        
        auto newConfig = std::make_shared<Config>();
        
        if (format == "json") {
            if (!importFromJSON(input, newConfig)) {
                return false;
            }
        } else if (format == "ini") {
            if (!importFromINI(input, newConfig)) {
                return false;
            }
        } else if (format == "yaml") {
            if (!importFromYAML(input, newConfig)) {
                return false;
            }
        } else {
            printError("Unsupported import format: " + format);
            return false;
        }
        
        if (merge && config_) {
            for (const auto& key : newConfig->getKeys()) {
                config_->set(key, newConfig->get<std::string>(key).value());
            }
        } else {
            config_ = newConfig;
        }
        
        printInfo("Configuration imported from: " + input + " (format: " + format + ")");
        return true;
    }
    
    bool handleList(int argc, char** argv) {
        if (!config_) {
            printError("No configuration loaded");
            return false;
        }
        
        auto keys = config_->getKeys();
        std::sort(keys.begin(), keys.end());
        
        std::cout << "Configuration keys (" << keys.size() << " total):\n";
        
        for (const auto& key : keys) {
            std::cout << "  " << key << "\n";
        }
        
        return true;
    }
    
    bool handleSearch(int argc, char** argv) {
        if (argc < 3) {
            printError("Usage: config-cli search <pattern>");
            return false;
        }
        
        std::string pattern = argv[2];
        
        if (!config_) {
            printError("No configuration loaded");
            return false;
        }
        
        auto keys = config_->getKeys();
        std::vector<std::string> matches;
        
        for (const auto& key : keys) {
            if (key.find(pattern) != std::string::npos) {
                matches.push_back(key);
            }
        }
        
        std::cout << "Search results for pattern '" << pattern << "':\n";
        for (const auto& match : matches) {
            std::cout << "  " << match << "\n";
        }
        
        return true;
    }
    
    // Additional helper functions
    bool validateConfig(std::shared_ptr<Config> config, bool strict = false) {
        if (!config) {
            printError("No configuration to validate");
            return false;
        }
        
        bool valid = true;
        
        // Basic validation
        if (config->has("neuron_count")) {
            auto neurons = config->getOr<int64_t>("neuron_count", 0);
            if (neurons <= 0 || neurons > 1000000) {
                printError("Invalid neuron_count: " + std::to_string(neurons));
                valid = false;
            }
        }
        
        if (config->has("region_count")) {
            auto regions = config->getOr<int64_t>("region_count", 0);
            if (regions <= 0 || regions > 100) {
                printError("Invalid region_count: " + std::to_string(regions));
                valid = false;
            }
        }
        
        if (strict) {
            if (config->has("simulation_timestep")) {
                auto timestep = config->getOr<double>("simulation_timestep", 0.0);
                if (timestep <= 0.0 || timestep > 1.0) {
                    printError("Invalid simulation_timestep (must be > 0 and <= 1.0): " + std::to_string(timestep));
                    valid = false;
                }
            }
        }
        
        if (valid) {
            printInfo("Configuration validation PASSED");
        } else {
            printError("Configuration validation FAILED");
        }
        
        return valid;
    }
    
    void exportToJSON(const std::string& filename) {
        std::ofstream file(filename);
        if (!file.is_open()) {
            printError("Failed to open file for JSON export: " + filename);
            return;
        }
        
        file << "{\n";
        auto keys = config_->getKeys();
        for (size_t i = 0; i < keys.size(); ++i) {
            const auto& key = keys[i];
            file << "  \"" << key << "\": ";
            
            auto val = config_->get<std::string>(key);
            if (val.has_value()) {
                file << "\"" << val.value() << "\"";
            } else {
                auto intVal = config_->get<int64_t>(key);
                if (intVal.has_value()) {
                    file << intVal.value();
                } else {
                    auto doubleVal = config_->get<double>(key);
                    if (doubleVal.has_value()) {
                        file << doubleVal.value();
                    } else {
                        auto boolVal = config_->get<bool>(key);
                        if (boolVal.has_value()) {
                            file << (boolVal.value() ? "true" : "false");
                        }
                    }
                }
            }
            
            if (i < keys.size() - 1) {
                file << ",\n";
            } else {
                file << "\n";
            }
        }
        file << "}\n";
        
        printInfo("Config exported to JSON: " + filename);
    }
    
    void exportToINI(const std::string& filename) {
        std::ofstream file(filename);
        if (!file.is_open()) {
            printError("Failed to open file for INI export: " + filename);
            return;
        }
        
        auto keys = config_->getKeys();
        for (const auto& key : keys) {
            file << key << " = ";
            
            auto val = config_->get<std::string>(key);
            if (val.has_value()) {
                file << val.value() << "\n";
            } else {
                auto intVal = config_->get<int64_t>(key);
                if (intVal.has_value()) {
                    file << intVal.value() << "\n";
                } else {
                    auto doubleVal = config_->get<double>(key);
                    if (doubleVal.has_value()) {
                        file << doubleVal.value() << "\n";
                    } else {
                        auto boolVal = config_->get<bool>(key);
                        if (boolVal.has_value()) {
                            file << (boolVal.value() ? "true" : "false") << "\n";
                        }
                    }
                }
            }
        }
        
        printInfo("Config exported to INI: " + filename);
    }
    
    void exportToYAML(const std::string& filename) {
        std::ofstream file(filename);
        if (!file.is_open()) {
            printError("Failed to open file for YAML export: " + filename);
            return;
        }
        
        file << "config:\n";
        auto keys = config_->getKeys();
        for (const auto& key : keys) {
            file << "  " << key << ": ";
            
            auto val = config_->get<std::string>(key);
            if (val.has_value()) {
                // Escape YAML special characters
                std::string yamlValue = val.value();
                size_t pos = 0;
                while ((pos = yamlValue.find('\n', pos)) != std::string::npos) {
                    yamlValue.replace(pos, 1, "\\n");
                    pos += 2;
                }
                file << "'" << yamlValue << "'\n";
            } else {
                auto intVal = config_->get<int64_t>(key);
                if (intVal.has_value()) {
                    file << intVal.value() << "\n";
                } else {
                    auto doubleVal = config_->get<double>(key);
                    if (doubleVal.has_value()) {
                        file << doubleVal.value() << "\n";
                    } else {
                        auto boolVal = config_->get<bool>(key);
                        if (boolVal.has_value()) {
                            file << (boolVal.value() ? "true" : "false") << "\n";
                        }
                    }
                }
            }
        }
        
        printInfo("Config exported to YAML: " + filename);
    }
    
    bool importFromJSON(const std::string& filename, std::shared_ptr<Config> config) {
        std::ifstream file(filename);
        if (!file.is_open()) {
            printError("Failed to open file for JSON import: " + filename);
            return false;
        }
        
        // Simple JSON parsing (basic)
        std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
        
        // For now, just return error - this would need proper JSON parsing
        printError("JSON import not implemented (requires JSON parser)");
        return false;
    }
    
    bool importFromINI(const std::string& filename, std::shared_ptr<Config> config) {
        std::ifstream file(filename);
        if (!file.is_open()) {
            printError("Failed to open file for INI import: " + filename);
            return false;
        }
        
        std::string line;
        while (std::getline(file, line)) {
            // Skip comments and empty lines
            line.erase(0, line.find_first_not_of(" \t"));
            if (line.empty() || line[0] == ';' || line[0] == '#') {
                continue;
            }
            
            size_t pos = line.find('=');
            if (pos != std::string::npos) {
                std::string key = line.substr(0, pos);
                std::string value = line.substr(pos + 1);
                
                // Trim whitespace
                key.erase(key.find_last_not_of(" \t") + 1);
                key.erase(0, key.find_first_not_of(" \t"));
                
                value.erase(0, value.find_first_not_of(" \t"));
                value.erase(value.find_last_not_of(" \t") + 1);
                
                // Remove quotes if present
                if (value.size() >= 2 && 
                    ((value.front() == '"' && value.back() == '"') ||
                     (value.front() == '\'' && value.back() == '\''))) {
                    value = value.substr(1, value.size() - 2);
                }
                
                config->set(key, value);
            }
        }
        
        printInfo("Config imported from INI: " + filename);
        return true;
    }
    
    bool importFromYAML(const std::string& filename, std::shared_ptr<Config> config) {
        std::ifstream file(filename);
        if (!file.is_open()) {
            printError("Failed to open file for YAML import: " + filename);
            return false;
        }
        
        // For now, just return error - this would need proper YAML parsing
        printError("YAML import not implemented (requires YAML parser)");
        return false;
    }
    
    void printInfo(const std::string& message) {
        std::cout << "[INFO] " << message << "\n";
    }
    
    void printError(const std::string& message) {
        std::cerr << "[ERROR] " << message << "\n";
    }
};

bool ConfigCLI::run(int argc, char** argv) {
    if (argc < 2) {
        printInfo("=== NLM Configuration CLI ===");
        printInfo("Usage: config-cli <command> [arguments]");
        printInfo("\nAvailable commands:");
        
        for (size_t i = 0; i < commands_.size(); ++i) {
            const auto& cmd = commands_[i];
            std::cout << (i + 1) << ". " << cmd.name << " - " << cmd.description << "\n";
        }
        
        printInfo("\nFor detailed help on a specific command, use: config-cli <command> --help");
        return true;
    }
    
    std::string command = argv[1];
    
    // Find and execute command
    for (const auto& cmd : commands_) {
        if (cmd.name == command) {
            return cmd.handler(argc, argv);
        }
    }
    
    printError("Unknown command: " + command);
    printInfo("Use 'config-cli help' for list of available commands");
    return false;
}

// Main entry point for config-cli
int main(int argc, char* argv[]) {
    ConfigCLI cli;
    bool success = cli.run(argc, argv);
    return success ? 0 : 1;
}

} // namespace nlm
