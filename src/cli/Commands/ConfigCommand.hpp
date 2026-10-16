#pragma once

#include "NLMCLI.hpp"
#include "../core/Config/Config.hpp"
#include "../core/Logger/Logger.hpp"
#include <memory>
#include <iostream>
#include <fstream>
#include <sstream>

namespace nlm {

class ConfigCommand : public ICommand {
public:
    std::string getName() const override { return "config"; }
    std::string getDescription() const override { return "Configuration management (create, edit, validate, save/load)"; }
    std::string getUsage() const override { return "config <action> [key=value...]"; }
    
    bool validateArguments(const std::vector<std::string>& args) override {
        return !args.empty();
    }
    
    void printHelp() const override {
        std::cout << "Usage: nlm-cli config <action> [key=value...]" << std::endl;
        std::cout << "Manage NLM configuration." << std::endl;
        std::cout << "" << std::endl;
        std::cout << "Actions:" << std::endl;
        std::cout << "  create <file>           Create a new configuration file" << std::endl;
        std::cout << "  load <file>             Load configuration from file" << std::endl;
        std::cout << "  save <file>             Save configuration to file" << std::endl;
        std::cout << "  show [key]              Show configuration values" << std::endl;
        std::cout << "  set <key> <value>       Set a configuration value" << std::endl;
        std::cout << "  remove <key>            Remove a configuration key" << std::endl;
        std::cout << "  validate                Validate configuration" << std::endl;
        std::cout << "  list                    List all configuration keys" << std::endl;
        std::cout << "  template <name>         Load configuration template" << std::endl;
        std::cout << "  export <format> <file>  Export configuration to different format" << std::endl;
        std::cout << "  import <format> <file>  Import configuration from different format" << std::endl;
        std::cout << "  merge <file>            Merge configuration from file" << std::endl;
        std::cout << "" << std::endl;
        std::cout << "Options:" << std::endl;
        std::cout << "  --file <path>           Configuration file path (default: configs/default.cfg)" << std::endl;
        std::cout << "  --format <format>       Configuration format (json, yaml, ini)" << std::endl;
        std::cout << "  --help, -h              Show help" << std::endl;
    }
    
    CommandResult execute(const std::vector<std::string>& args, const CLIConfig& config) override {
        CommandResult result;
        
        if (args.empty() || args[0] == "--help" || args[0] == "-h") {
            printHelp();
            result.success = true;
            result.exitCode = 0;
            return result;
        }
        
        std::string action = args[0];
        std::vector<std::string> actionArgs(args.begin() + 1, args.end());
        
        // Parse options
        std::string configFile = config.configFile.empty() ? "configs/default.cfg" : config.configFile;
        std::string configFormat = "ini";  // Default format
        
        for (size_t i = 0; i < actionArgs.size(); ++i) {
            std::string arg = actionArgs[i];
            if (arg.find('=') != std::string::npos) {
                size_t pos = arg.find('=');
                std::string key = arg.substr(0, pos);
                std::string value = arg.substr(pos + 1);
                actionArgs[i] = key + "=" + value;
            }
        }
        
        if (action == "create") {
            result = handleCreateAction(actionArgs, configFile, configFormat);
        } else if (action == "load") {
            result = handleLoadAction(actionArgs, configFile, configFormat);
        } else if (action == "save") {
            result = handleSaveAction(actionArgs, configFile, configFormat);
        } else if (action == "show") {
            result = handleShowAction(actionArgs, configFile, configFormat);
        } else if (action == "set") {
            result = handleSetAction(actionArgs, configFile, configFormat);
        } else if (action == "remove") {
            result = handleRemoveAction(actionArgs, configFile, configFormat);
        } else if (action == "validate") {
            result = handleValidateAction(actionArgs, configFile, configFormat);
        } else if (action == "list") {
            result = handleListAction(actionArgs, configFile, configFormat);
        } else if (action == "template") {
            result = handleTemplateAction(actionArgs, configFile, configFormat);
        } else if (action == "export") {
            result = handleExportAction(actionArgs, configFile, configFormat);
        } else if (action == "import") {
            result = handleImportAction(actionArgs, configFile, configFormat);
        } else if (action == "merge") {
            result = handleMergeAction(actionArgs, configFile, configFormat);
        } else {
            result.success = false;
            result.message = "Unknown config action: " + action;
            result.exitCode = 1;
        }
        
        return result;
    }
    
private:
    CommandResult handleCreateAction(const std::vector<std::string>& args,
                                    const std::string& configFile,
                                    const std::string& configFormat) {
        CommandResult result;
        
        if (args.empty()) {
            result.success = false;
            result.message = "Missing file path for create action";
            result.exitCode = 1;
            return result;
        }
        
        std::string filepath = args[0];
        
        // Create default configuration
        auto config = std::make_shared<Config>();
        
        // Set default values
        config->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
        config->set("simulation_timestep", 0.001, ConfigSource::Default);
        config->set("neuron_count", static_cast<int64_t>(1000), ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
        config->set("connection_probability", 0.1f, ConfigSource::Default);
        config->set("plasticity.learning_rate", 0.01f, ConfigSource::Default);
        config->set("plasticity.stdp.enable", true, ConfigSource::Default);
        
        // Save to file
        if (CLIManager::saveConfigFile(filepath, config)) {
            std::ostringstream output;
            output << "Created configuration file: " << filepath << std::endl;
            output << "Default configuration saved successfully." << std::endl;
            output << "Use 'nlm-cli config load " << filepath << "' to load it." << std::endl;
            
            result.success = true;
            result.message = "Configuration file created successfully";
            result.output = output.str();
        } else {
            result.success = false;
            result.message = "Failed to create configuration file: " + filepath;
            result.exitCode = 2;
        }
        
        return result;
    }
    
    CommandResult handleLoadAction(const std::vector<std::string>& args,
                                  const std::string& configFile,
                                  const std::string& configFormat) {
        CommandResult result;
        
        if (args.empty()) {
            result.success = false;
            result.message = "Missing file path for load action";
            result.exitCode = 1;
            return result;
        }
        
        std::string filepath = args[0];
        
        auto config = std::make_shared<Config>();
        
        if (CLIManager::loadConfigFile(filepath, config)) {
            std::ostringstream output;
            output << "Loaded configuration from: " << filepath << std::endl;
            output << config->summary() << std::endl;
            
            result.success = true;
            result.message = "Configuration loaded successfully";
            result.output = output.str();
        } else {
            result.success = false;
            result.message = "Failed to load configuration from: " + filepath;
            result.exitCode = 2;
        }
        
        return result;
    }
    
    CommandResult handleSaveAction(const std::vector<std::string>& args,
                                  const std::string& configFile,
                                  const std::string& configFormat) {
        CommandResult result;
        
        if (args.empty()) {
            result.success = false;
            result.message = "Missing file path for save action";
            result.exitCode = 1;
            return result;
        }
        
        std::string filepath = args[0];
        
        auto config = std::make_shared<Config>();
        
        // Load current config if file exists
        std::ifstream testFile(filepath);
        if (testFile.good()) {
            testFile.close();
            if (CLIManager::loadConfigFile(filepath, config)) {
                std::ostringstream output;
                output << "Loaded existing configuration from: " << filepath << std::endl;
                output << "Save will update this configuration." << std::endl;
                result.output = output.str();
            }
        }
        
        if (CLIManager::saveConfigFile(filepath, config)) {
            std::ostringstream output;
            output << "Saved configuration to: " << filepath << std::endl;
            output << "Configuration saved successfully." << std::endl;
            
            result.success = true;
            result.message = "Configuration saved successfully";
            result.output = output.str();
        } else {
            result.success = false;
            result.message = "Failed to save configuration to: " + filepath;
            result.exitCode = 2;
        }
        
        return result;
    }
    
    CommandResult handleShowAction(const std::vector<std::string>& args,
                                  const std::string& configFile,
                                  const std::string& configFormat) {
        CommandResult result;
        
        auto config = std::make_shared<Config>();
        
        // Try to load from default file
        if (!configFile.empty()) {
            CLIManager::loadConfigFile(configFile, config);
        }
        
        std::ostringstream output;
        
        if (args.empty()) {
            // Show all configuration
            output << "Current Configuration:" << std::endl;
            output << "=====================" << std::endl;
            output << config->summary() << std::endl;
        } else {
            // Show specific key
            for (const auto& key : args) {
                if (config->has(key)) {
                    output << key << " = ";
                    try {
                        if (config->has(key)) {
                            // Try different types
                            if (auto val = config->get<int64_t>(key)) {
                                output << *val << std::endl;
                            } else if (auto val = config->get<double>(key)) {
                                output << *val << std::endl;
                            } else if (auto val = config->get<bool>(key)) {
                                output << (*val ? "true" : "false") << std::endl;
                            } else if (auto val = config->get<std::string>(key)) {
                                output << "\"" << *val << "\"" << std::endl;
                            } else {
                                output << "[unknown type]" << std::endl;
                            }
                        }
                    } catch (...) {
                        output << "[error reading value]" << std::endl;
                    }
                } else {
                    output << "Key not found: " << key << std::endl;
                }
            }
        }
        
        result.success = true;
        result.message = "Configuration displayed";
        result.output = output.str();
        
        return result;
    }
    
    CommandResult handleSetAction(const std::vector<std::string>& args,
                                 const std::string& configFile,
                                 const std::string& configFormat) {
        CommandResult result;
        
        if (args.size() < 2) {
            result.success = false;
            result.message = "Missing key and value for set action";
            result.exitCode = 1;
            return result;
        }
        
        std::string key = args[0];
        std::string value = args[1];
        
        // Create config and set value
        auto config = std::make_shared<Config>();
        
        // Parse value based on key type
        try {
            if (key.find('.') != std::string::npos) {
                // Section parameter
                size_t dotPos = key.find('.');
                std::string section = key.substr(0, dotPos);
                std::string paramKey = key.substr(dotPos + 1);
                
                // Try to parse as different types
                try {
                    double doubleVal = std::stod(value);
                    config->setParameter(section, paramKey, doubleVal, ConfigSource::Runtime);
                } catch (...) {
                    try {
                        int64_t intVal = std::stoll(value);
                        config->setParameter(section, paramKey, intVal, ConfigSource::Runtime);
                    } catch (...) {
                        bool boolVal = (value == "true" || value == "1" || value == "yes");
                        config->setParameter(section, paramKey, boolVal, ConfigSource::Runtime);
                    }
                }
            } else {
                // Simple key
                try {
                    double doubleVal = std::stod(value);
                    config->set(key, doubleVal, ConfigSource::Runtime);
                } catch (...) {
                    try {
                        int64_t intVal = std::stoll(value);
                        config->set(key, intVal, ConfigSource::Runtime);
                    } catch (...) {
                        bool boolVal = (value == "true" || value == "1" || value == "yes");
                        config->set(key, boolVal, ConfigSource::Runtime);
                    }
                }
            }
            
            std::ostringstream output;
            output << "Set " << key << " = " << value << std::endl;
            output << "Configuration updated successfully." << std::endl;
            
            result.success = true;
            result.message = "Configuration key set successfully";
            result.output = output.str();
            
        } catch (const std::exception& e) {
            result.success = false;
            result.message = std::string("Error setting value: ") + e.what();
            result.exitCode = 2;
        }
        
        return result;
    }
    
    CommandResult handleRemoveAction(const std::vector<std::string>& args,
                                    const std::string& configFile,
                                    const std::string& configFormat) {
        CommandResult result;
        
        if (args.empty()) {
            result.success = false;
            result.message = "Missing key for remove action";
            result.exitCode = 1;
            return result;
        }
        
        std::string key = args[0];
        
        auto config = std::make_shared<Config>();
        
        if (config->has(key)) {
            config->remove(key);
            
            std::ostringstream output;
            output << "Removed configuration key: " << key << std::endl;
            
            result.success = true;
            result.message = "Configuration key removed successfully";
            result.output = output.str();
        } else {
            result.success = false;
            result.message = "Key not found: " + key;
            result.exitCode = 2;
        }
        
        return result;
    }
    
    CommandResult handleValidateAction(const std::vector<std::string>& args,
                                      const std::string& configFile,
                                      const std::string& configFormat) {
        CommandResult result;
        
        auto config = std::make_shared<Config>();
        
        if (!configFile.empty()) {
            CLIManager::loadConfigFile(configFile, config);
        }
        
        std::ostringstream output;
        bool isValid = CLIManager::validateConfiguration(config);
        
        if (isValid) {
            output << "Configuration is valid." << std::endl;
            
            result.success = true;
            result.message = "Configuration validation passed";
            result.output = output.str();
        } else {
            output << "Configuration validation failed." << std::endl;
            output << "See configuration summary for details:" << std::endl;
            output << config->summary() << std::endl;
            
            result.success = false;
            result.message = "Configuration validation failed";
            result.exitCode = 2;
            result.output = output.str();
        }
        
        return result;
    }
    
    CommandResult handleListAction(const std::vector<std::string>& args,
                                  const std::string& configFile,
                                  const std::string& configFormat) {
        CommandResult result;
        
        auto config = std::make_shared<Config>();
        
        if (!configFile.empty()) {
            CLIManager::loadConfigFile(configFile, config);
        }
        
        std::vector<std::vector<std::string>> rows;
        std::vector<std::string> headers = {"Key", "Value", "Source"};
        
        for (const auto& key : config->getKeys()) {
            std::vector<std::string> row;
            row.push_back(key);
            
            // Get value
            std::ostringstream valueStr;
            try {
                if (auto val = config->get<int64_t>(key)) {
                    valueStr << *val;
                } else if (auto val = config->get<double>(key)) {
                    valueStr << *val;
                } else if (auto val = config->get<bool>(key)) {
                    valueStr << (*val ? "true" : "false");
                } else if (auto val = config->get<std::string>(key)) {
                    valueStr << *val;
                } else {
                    valueStr << "[unknown]";
                }
            } catch (...) {
                valueStr << "[error]";
            }
            
            row.push_back(valueStr.str());
            
            // Get source (we don't have direct access to source, use placeholder)
            row.push_back("runtime");
            
            rows.push_back(row);
        }
        
        std::string tableOutput = OutputFormatter::formatTable(rows, headers);
        
        result.success = true;
        result.message = "Configuration keys listed";
        result.output = tableOutput;
        
        return result;
    }
    
    CommandResult handleTemplateAction(const std::vector<std::string>& args,
                                       const std::string& configFile,
                                       const std::string& configFormat) {
        CommandResult result;
        
        if (args.empty()) {
            result.success = false;
            result.message = "Missing template name for template action";
            result.exitCode = 1;
            return result;
        }
        
        std::string templateName = args[0];
        
        // Create configuration from template
        auto config = std::make_shared<Config>();
        
        if (templateName == "minimal") {
            // Minimal configuration
            config->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
            config->set("simulation_timestep", 0.001, ConfigSource::Default);
            config->set("neuron_count", static_cast<int64_t>(100), ConfigSource::Default);
            config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
            config->set("connection_probability", 0.1f, ConfigSource::Default);
            
        } else if (templateName == "performance") {
            // Performance-focused configuration
            config->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
            config->set("simulation_timestep", 0.001, ConfigSource::Default);
            config->set("neuron_count", static_cast<int64_t>(500), ConfigSource::Default);
            config->set("region_count", static_cast<int64_t>(2), ConfigSource::Default);
            config->set("connection_probability", 0.05f, ConfigSource::Default);
            config->set("plasticity.stdp.enable", true, ConfigSource::Default);
            config->set("plasticity.stdp.learning_rate", 0.001f, ConfigSource::Default);
            
        } else if (templateName == "research") {
            // Research-focused configuration
            config->set("random_seed", static_cast<int64_t>(12345), ConfigSource::Default);
            config->set("simulation_timestep", 0.001, ConfigSource::Default);
            config->set("neuron_count", static_cast<int64_t>(5000), ConfigSource::Default);
            config->set("region_count", static_cast<int64_t>(5), ConfigSource::Default);
            config->set("connection_probability", 0.02f, ConfigSource::Default);
            config->set("plasticity.stdp.enable", true, ConfigSource::Default);
            config->set("plasticity.hebbian.enable", true, ConfigSource::Default);
            config->set("plasticity.structural.enable", true, ConfigSource::Default);
            
        } else {
            result.success = false;
            result.message = "Unknown template: " + templateName;
            result.exitCode = 1;
            return result;
        }
        
        std::ostringstream output;
        output << "Created configuration from template: " << templateName << std::endl;
        output << config->summary() << std::endl;
        
        result.success = true;
        result.message = "Configuration template loaded successfully";
        result.output = output.str();
        
        return result;
    }
    
    CommandResult handleExportAction(const std::vector<std::string>& args,
                                    const std::string& configFile,
                                    const std::string& configFormat) {
        CommandResult result;
        
        if (args.size() < 2) {
            result.success = false;
            result.message = "Missing format and file for export action";
            result.exitCode = 1;
            return result;
        }
        
        std::string format = args[0];
        std::string filepath = args[1];
        
        auto config = std::make_shared<Config>();
        
        if (!configFile.empty()) {
            CLIManager::loadConfigFile(configFile, config);
        }
        
        std::ofstream outFile(filepath);
        if (!outFile.is_open()) {
            result.success = false;
            result.message = "Failed to open output file: " + filepath;
            result.exitCode = 2;
            return result;
        }
        
        // Export based on format
        if (format == "json") {
            // Simple JSON export (in reality would use a proper JSON library)
            outFile << "{\n";
            bool first = true;
            for (const auto& key : config->getKeys()) {
                if (!first) outFile << ",\n";
                first = false;
                outFile << "  \"" << key << "\": \";
                try {
                    if (auto val = config->get<int64_t>(key)) {
                        outFile << *val;
                    } else if (auto val = config->get<double>(key)) {
                        outFile << *val;
                    } else if (auto val = config->get<bool>(key)) {
                        outFile << (*val ? "true" : "false");
                    } else if (auto val = config->get<std::string>(key)) {
                        outFile << *val;
                    } else {
                        outFile << "null";
                    }
                } catch (...) {
                    outFile << "null";
                }
            }
            outFile << "\n}\n";
        } else if (format == "csv") {
            // CSV export
            outFile << "key,value,type\n";
            for (const auto& key : config->getKeys()) {
                outFile << "\"" << key << "\",";
                try {
                    if (auto val = config->get<int64_t>(key)) {
                        outFile << *val << ",int64_t\n";
                    } else if (auto val = config->get<double>(key)) {
                        outFile << *val << ",double\n";
                    } else if (auto val = config->get<bool>(key)) {
                        outFile << (*val ? "true" : "false") << ",bool\n";
                    } else if (auto val = config->get<std::string>(key)) {
                        outFile << "\"" << *val << "\",string\n";
                    } else {
                        outFile << "null,unknown\n";
                    }
                } catch (...) {
                    outFile << "null,error\n";
                }
            }
        } else if (format == "ini") {
            // INI format export (key=value)
            for (const auto& key : config->getKeys()) {
                outFile << key << " = ";
                try {
                    if (auto val = config->get<int64_t>(key)) {
                        outFile << *val << "\n";
                    } else if (auto val = config->get<double>(key)) {
                        outFile << *val << "\n";
                    } else if (auto val = config->get<bool>(key)) {
                        outFile << (*val ? "true" : "false") << "\n";
                    } else if (auto val = config->get<std::string>(key)) {
                        outFile << *val << "\n";
                    } else {
                        outFile << "null\n";
                    }
                } catch (...) {
                    outFile << "null\n";
                }
            }
        } else {
            result.success = false;
            result.message = "Unsupported export format: " + format;
            result.exitCode = 1;
            return result;
        }
        
        outFile.close();
        
        std::ostringstream output;
        output << "Exported configuration to: " << filepath << " (format: " << format << ")" << std::endl;
        
        result.success = true;
        result.message = "Configuration exported successfully";
        result.output = output.str();
        
        return result;
    }
    
    CommandResult handleImportAction(const std::vector<std::string>& args,
                                    const std::string& configFile,
                                    const std::string& configFormat) {
        CommandResult result;
        
        if (args.size() < 2) {
            result.success = false;
            result.message = "Missing format and file for import action";
            result.exitCode = 1;
            return result;
        }
        
        std::string format = args[0];
        std::string filepath = args[1];
        
        auto config = std::make_shared<Config>();
        
        std::ifstream inFile(filepath);
        if (!inFile.is_open()) {
            result.success = false;
            result.message = "Failed to open input file: " + filepath;
            result.exitCode = 2;
            return result;
        }
        
        // Import based on format
        if (format == "json") {
            // Simple JSON import (in reality would use a proper JSON library)
            std::string line;
            bool inObject = false;
            while (std::getline(inFile, line)) {
                line = trim(line);
                if (line.empty() || line == "{") continue;
                if (line == "}") break;
                if (line.find('"') != std::string::npos) {
                    size_t colonPos = line.find(':');
                    if (colonPos != std::string::npos) {
                        std::string key = line.substr(1, colonPos - 3);
                        std::string value = line.substr(colonPos + 2);
                        // Remove quotes and trailing comma
                        if (!value.empty() && value.back() == ',') {
                            value.pop_back();
                        }
                        if (!value.empty() && value.front() == '"' && value.back() == '"') {
                            value = value.substr(1, value.size() - 2);
                        } else if (!value.empty() && value.front() == ' ' && value.back() == ' ') {
                            value = value.substr(1, value.size() - 2);
                        }
                        
                        config->set(key, value, ConfigSource::File);
                    }
                }
            }
        } else if (format == "csv") {
            // CSV import
            std::string line;
            bool isHeader = true;
            while (std::getline(inFile, line)) {
                if (isHeader) {
                    isHeader = false;
                    continue;
                }
                
                // Parse CSV line
                std::stringstream ss(line);
                std::string token;
                std::vector<std::string> tokens;
                bool inQuotes = false;
                std::string currentToken;
                
                for (char c : line) {
                    if (c == '"') {
                        inQuotes = !inQuotes;
                    } else if (c == ',' && !inQuotes) {
                        tokens.push_back(currentToken);
                        currentToken.clear();
                    } else {
                        currentToken += c;
                    }
                }
                
                if (tokens.size() >= 2) {
                    std::string key = tokens[0];
                    std::string value = tokens[1];
                    
                    // Remove quotes from key and value
                    if (key.front() == '"' && key.back() == '"') {
                        key = key.substr(1, key.size() - 2);
                    }
                    if (value.front() == '"' && value.back() == '"') {
                        value = value.substr(1, value.size() - 2);
                    }
                    
                    config->set(key, value, ConfigSource::File);
                }
            }
        } else if (format == "ini") {
            // INI format import (key=value)
            std::string line;
            while (std::getline(inFile, line)) {
                line = trim(line);
                if (line.empty() || line[0] == '#' || line[0] == ';') {
                    continue;
                }
                
                size_t pos = line.find('=');
                if (pos != std::string::npos) {
                    std::string key = trim(line.substr(0, pos));
                    std::string value = trim(line.substr(pos + 1));
                    config->set(key, value, ConfigSource::File);
                }
            }
        } else {
            result.success = false;
            result.message = "Unsupported import format: " + format;
            result.exitCode = 1;
            return result;
        }
        
        inFile.close();
        
        std::ostringstream output;
        output << "Imported configuration from: " << filepath << " (format: " << format << ")" << std::endl;
        output << config->summary() << std::endl;
        
        result.success = true;
        result.message = "Configuration imported successfully";
        result.output = output.str();
        
        return result;
    }
    
    CommandResult handleMergeAction(const std::vector<std::string>& args,
                                   const std::string& configFile,
                                   const std::string& configFormat) {
        CommandResult result;
        
        if (args.empty()) {
            result.success = false;
            result.message = "Missing file path for merge action";
            result.exitCode = 1;
            return result;
        }
        
        std::string filepath = args[0];
        
        auto config = std::make_shared<Config>();
        
        if (CLIManager::loadConfigFile(filepath, config)) {
            std::ostringstream output;
            output << "Merged configuration from: " << filepath << std::endl;
            output << config->summary() << std::endl;
            
            result.success = true;
            result.message = "Configuration merged successfully";
            result.output = output.str();
        } else {
            result.success = false;
            result.message = "Failed to merge configuration from: " + filepath;
            result.exitCode = 2;
        }
        
        return result;
    }
    
    // Helper function to trim strings
    std::string trim(const std::string& str) {
        size_t first = str.find_first_not_of(" \t\n\r");
        if (first == std::string::npos) return str;
        size_t last = str.find_last_not_of(" \t\n\r");
        return str.substr(first, (last - first + 1));
    }
    
}; // class ConfigCommand

} // namespace nlm