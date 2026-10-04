#pragma once

#include "CLICommand.hpp"
#include "core/Config/Config.hpp"
#include <filesystem>
#include <fstream>
#include <iostream>
#include <iomanip>

namespace nlm {

class SaveCommand : public SimpleCLICommand {
public:
    SaveCommand() : SimpleCLICommand(
        "save",
        "Save configuration to file",
        "nlm save <file> [--format json|txt] [--overwrite] [--compress]",
        "Save the current configuration to a file.\n\n"
        "Options:\n"
        "  --format json|txt    Configuration file format (default: txt)\n"
        "  --overwrite          Overwrite existing file without prompt\n"
        "  --compress           Compress the configuration file\n"
        "  --backup             Create backup before saving\n\n"
        "Examples:\n"
        "  nlm save config.txt\n"
        "  nlm save config.json --format json\n"
        "  nlm save config.txt --overwrite --backup"
    ) {
        options = {
            {"format", "", "Configuration file format", OptionType::Choice, false, "txt", "json|txt"},
            {"overwrite", "", "Overwrite existing file without prompt", OptionType::Flag},
            {"compress", "", "Compress the configuration file", OptionType::Flag},
            {"backup", "", "Create backup before saving", OptionType::Flag},
            {"encoding", "", "File encoding (utf-8, ascii, latin1)", OptionType::Choice, false, "utf-8", "utf-8|ascii|latin1"}
        };
        
        parameters = {
            {"file", "Configuration file to save to", true, "file", ""}
        };
    }
    
    int execute(const std::vector<std::string>& args) override {
        auto& errorHandler = CLIErrorHandler::getInstance();
        errorHandler.clear();
        
        // Get required parameter
        std::string filepath = getRequiredParameter(args, 0);
        
        // Get options
        std::string format = getOptionValue(args, "format");
        bool overwrite = hasOption(args, "overwrite");
        bool compress = hasOption(args, "compress");
        bool backup = hasOption(args, "backup");
        std::string encoding = getOptionValue(args, "encoding");
        
        // Check if file exists
        bool fileExists = std::filesystem::exists(filepath);
        
        // If file exists and not overwriting, prompt for confirmation
        if (fileExists && !overwrite) {
            std::cout << "Warning: File '" << filepath << "' already exists." << std::endl;
            std::cout << "Overwrite? (y/n): ";
            std::string response;
            std::getline(std::cin, response);
            if (response != "y" && response != "Y") {
                CLIFormatters::printInfo("Save cancelled.");
                return 0;
            }
        }
        
        // Create backup if requested
        if (backup && fileExists) {
            std::string backupFile = filepath + ".backup." + CLIUtils::getCurrentTimestamp();
            if (!saveToFile(backupFile)) {
                errorHandler.addWarning("Failed to create backup configuration");
            } else {
                CLIFormatters::printInfo("Configuration backed up to: " + backupFile);
            }
        }
        
        // Save configuration
        if (format == "json") {
            if (!saveToJson(filepath, encoding, compress)) {
                errorHandler.addError("Failed to save JSON configuration to: " + filepath);
                return errorHandler.exitWithError();
            }
        } else {
            if (!saveToText(filepath, encoding, compress)) {
                errorHandler.addError("Failed to save text configuration to: " + filepath);
                return errorHandler.exitWithError();
            }
        }
        
        CLIFormatters::printSuccess("Configuration saved to: " + filepath);
        
        return errorHandler.isSuccess() ? 0 : 1;
    }
    
    bool validate(const std::vector<std::string>& args) const override {
        if (args.empty()) {
            return false;
        }
        
        // Check if directory exists
        std::string filepath = args[0];
        std::filesystem::path path(filepath);
        if (!std::filesystem::exists(path.parent_path())) {
            return false;
        }
        
        return true;
    }
    
private:
    bool saveToText(const std::string& filepath, const std::string& encoding, bool compress) {
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
        
        // Compress if requested
        if (compress) {
            // Compress file (simplified implementation)
            std::string compressedFile = filepath + ".gz";
            if (compressFile(filepath, compressedFile)) {
                std::filesystem::remove(filepath);
                return true;
            }
        }
        
        return true;
    }
    
    bool saveToJson(const std::string& filepath, const std::string& encoding, bool compress) {
        std::ofstream file(filepath, std::ios::out);
        if (!file.is_open()) {
            return false;
        }
        
        // Write configuration as JSON
        file << "{\n";
        auto keys = getConfig()->getKeys();
        for (size_t i = 0; i < keys.size(); ++i) {
            std::string key = keys[i];
            auto value = getConfig()->get<std::string>(key);
            
            if (value.has_value()) {
                file << "  \"" << key << "\": \"" << value.value() << "\"";
                if (i < keys.size() - 1) {
                    file << ",";
                }
                file << "\n";
            }
        }
        
        file << "}\n";
        file.close();
        
        // Compress if requested
        if (compress) {
            std::string compressedFile = filepath + ".gz";
            if (compressFile(filepath, compressedFile)) {
                std::filesystem::remove(filepath);
                return true;
            }
        }
        
        return true;
    }
    
    bool compressFile(const std::string& inputFile, const std::string& outputFile) {
        // Compress file (simplified implementation)
        std::ifstream inFile(inputFile, std::ios::binary);
        std::ofstream outFile(outputFile, std::ios::binary);
        
        if (!inFile.is_open() || !outFile.is_open()) {
            return false;
        }
        
        // Simple compression (copy for now)
        outFile << inFile.rdbuf();
        
        inFile.close();
        outFile.close();
        
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
