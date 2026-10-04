#pragma once

#include "CLICommand.hpp"
#include "core/Config/Config.hpp"
#include <filesystem>
#include <fstream>
#include <iostream>
#include <iomanip>

namespace nlm {

class LoadCommand : public SimpleCLICommand {
public:
    LoadCommand() : SimpleCLICommand(
        "load",
        "Load configuration from file",
        "nlm load <file> [--format json|txt] [--validate]",
        "Load configuration from a file and apply it to the current configuration.\n\n"
        "Options:\n"
        "  --format json|txt    Configuration file format (default: txt)\n"
        "  --validate           Validate configuration after loading\n"
        "  --backup             Create backup of current configuration before loading\n\n"
        "Examples:\n"
        "  nlm load config.txt\n"
        "  nlm load config.json --format json\n"
        "  nlm load config.txt --validate --backup"
    ) {
        options = {
            {"format", "", "Configuration file format", OptionType::Choice, false, "txt", "json|txt"},
            {"validate", "", "Validate configuration after loading", OptionType::Flag},
            {"backup", "", "Create backup of current configuration", OptionType::Flag},
            {"encoding", "", "File encoding (utf-8, ascii, latin1)", OptionType::Choice, false, "utf-8", "utf-8|ascii|latin1"}
        };
        
        parameters = {
            {"file", "Configuration file to load", true, "file", ""}
        };
    }
    
    int execute(const std::vector<std::string>& args) override {
        auto& errorHandler = CLIErrorHandler::getInstance();
        errorHandler.clear();
        
        // Get required parameter
        std::string filepath = getRequiredParameter(args, 0);
        
        // Get options
        std::string format = getOptionValue(args, "format");
        bool validate = hasOption(args, "validate");
        bool backup = hasOption(args, "backup");
        std::string encoding = getOptionValue(args, "encoding");
        
        // Validate file path
        if (!CLIUtils::isValidFilePath(filepath)) {
            errorHandler.addError("Invalid file path: " + filepath);
            return errorHandler.exitWithError();
        }
        
        // Create backup if requested
        if (backup) {
            std::string backupFile = filepath + ".backup." + CLIUtils::getCurrentTimestamp();
            if (!saveToFile(backupFile)) {
                errorHandler.addWarning("Failed to create backup configuration");
            } else {
                CLIFormatters::printInfo("Configuration backed up to: " + backupFile);
            }
        }
        
        // Load configuration
        if (format == "json") {
            if (!loadFromJson(filepath, encoding)) {
                errorHandler.addError("Failed to load JSON configuration from: " + filepath);
                return errorHandler.exitWithError();
            }
        } else {
            if (!loadFromText(filepath, encoding)) {
                errorHandler.addError("Failed to load text configuration from: " + filepath);
                return errorHandler.exitWithError();
            }
        }
        
        CLIFormatters::printSuccess("Configuration loaded from: " + filepath);
        
        // Validate if requested
        if (validate) {
            if (!validateConfiguration()) {
                errorHandler.addError("Configuration validation failed");
                return errorHandler.exitWithError();
            }
            CLIFormatters::printSuccess("Configuration validation passed");
        }
        
        return errorHandler.isSuccess() ? 0 : 1;
    }
    
    bool validate(const std::vector<std::string>& args) const override {
        if (args.empty()) {
            return false;
        }
        
        // Check if file exists
        std::string filepath = args[0];
        if (!CLIUtils::isValidFilePath(filepath)) {
            return false;
        }
        
        // Check file extension for format validation
        std::string ext = std::filesystem::path(filepath).extension().string();
        if (ext == ".json") {
            // Check if json format is available
            // For now, we'll assume it's available
        }
        
        return true;
    }
    
private:
    bool loadFromText(const std::string& filepath, const std::string& encoding) {
        std::ifstream file(filepath, std::ios::in);
        if (!file.is_open()) {
            return false;
        }
        
        // Read file content
        std::string content((std::istreambuf_iterator<char>(file)),
                           std::istreambuf_iterator<char>());
        file.close();
        
        // Parse key-value pairs
        size_t pos = 0;
        while ((pos = content.find('\n', pos)) != std::string::npos) {
            std::string line = content.substr(0, pos);
            size_t equalsPos = line.find('=');
            
            if (equalsPos != std::string::npos) {
                std::string key = CLIUtils::trim(line.substr(0, equalsPos));
                std::string value = CLIUtils::trim(line.substr(equalsPos + 1));
                
                // Convert value to appropriate type
                if (value == "true" || value == "false") {
                    bool boolValue = (value == "true");
                    getConfig()->set(key, boolValue, ConfigSource::File);
                } else if (value.find('.') != std::string::npos) {
                    double doubleValue = std::stod(value);
                    getConfig()->set(key, doubleValue, ConfigSource::File);
                } else if (value.find_first_not_of("0123456789") == std::string::npos) {
                    int64_t intValue = std::stoll(value);
                    getConfig()->set(key, intValue, ConfigSource::File);
                } else {
                    getConfig()->set(key, value, ConfigSource::File);
                }
            }
            
            pos++;
        }
        
        return true;
    }
    
    bool loadFromJson(const std::string& filepath, const std::string& encoding) {
        // For now, we'll implement a simple JSON parser
        // In a real implementation, we would use a proper JSON library
        std::ifstream file(filepath, std::ios::in);
        if (!file.is_open()) {
            return false;
        }
        
        // Simple JSON parsing (for demonstration)
        // This should be replaced with a proper JSON parser
        std::string content((std::istreambuf_iterator<char>(file)),
                           std::istreambuf_iterator<char>());
        file.close();
        
        // Remove whitespace and parse as key-value pairs
        // This is a simplified implementation
        size_t pos = content.find('"');
        while (pos != std::string::npos) {
            size_t keyStart = pos + 1;
            size_t keyEnd = content.find('"', keyStart);
            if (keyEnd == std::string::npos) break;
            
            std::string key = content.substr(keyStart, keyEnd - keyStart);
            
            // Find colon after key
            size_t colonPos = content.find(':', keyEnd);
            if (colonPos == std::string::npos) break;
            
            // Find value (could be string, number, boolean, null)
            size_t valueStart = content.find_first_not_of(' ', colonPos + 1);
            if (valueStart == std::string::npos) break;
            
            std::string valueStr;
            if (content[valueStart] == '"') {
                // String value
                size_t valueEnd = content.find('"', valueStart + 1);
                if (valueEnd == std::string::npos) break;
                valueStr = content.substr(valueStart + 1, valueEnd - valueStart - 1);
            } else if (content[valueStart] == 't' || content[valueStart] == 'f') {
                // Boolean value
                size_t valueEnd = content.find_first_of(",}", valueStart);
                if (valueEnd == std::string::npos) break;
                valueStr = content.substr(valueStart, valueEnd - valueStart);
            } else if (content[valueStart] == 'n') {
                // null value
                size_t valueEnd = content.find_first_of(",}", valueStart);
                if (valueEnd == std::string::npos) break;
                valueStr = content.substr(valueStart, valueEnd - valueStart);
            } else {
                // Number value
                size_t valueEnd = content.find_first_of(",}", valueStart);
                if (valueEnd == std::string::npos) break;
                valueStr = content.substr(valueStart, valueEnd - valueStart);
            }
            
            // Set configuration value
            if (valueStr == "true" || valueStr == "false") {
                bool boolValue = (valueStr == "true");
                getConfig()->set(key, boolValue, ConfigSource::File);
            } else if (valueStr == "null") {
                // Skip null values
            } else if (valueStr.find('.') != std::string::npos) {
                double doubleValue = std::stod(valueStr);
                getConfig()->set(key, doubleValue, ConfigSource::File);
            } else if (valueStr.find_first_not_of("0123456789") == std::string::npos) {
                int64_t intValue = std::stoll(valueStr);
                getConfig()->set(key, intValue, ConfigSource::File);
            } else {
                getConfig()->set(key, valueStr, ConfigSource::File);
            }
            
            pos = content.find('"', keyEnd + 1);
        }
        
        return true;
    }
    
    bool saveToFile(const std::string& filepath) {
        std::ofstream file(filepath, std::ios::out);
        if (!file.is_open()) {
            return false;
        }
        
        // Write configuration as key=value pairs
        auto keys = getConfig()->getKeys();
        for (const auto& key : keys) {
            auto value = getConfig()->get<std::string>(key);
            if (value.has_value()) {
                file << key << " = " << value.value() << std::endl;
            }
        }
        
        file.close();
        return true;
    }
    
    bool validateConfiguration() {
        auto errors = getConfig()->validate();
        if (!errors.empty()) {
            std::cout << "Configuration validation failed:" << std::endl;
            for (const auto& error : errors) {
                std::cout << "  Error: " << error.message << std::endl;
            }
            return false;
        }
        return true;
    }
    
    // Helper methods
    std::shared_ptr<Config> getConfig() {
        // Get the global configuration
        // This should be implemented based on how configuration is managed
        static std::shared_ptr<Config> config = std::make_shared<Config>();
        return config;
    }
};

} // namespace nlm
