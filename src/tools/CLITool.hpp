// Base CLI Tool Class
//
// This file provides the base class for all CLI tools in the NLM system.
// It provides common functionality and a framework for tool development.

#pragma once

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <iostream>
#include <fstream>
#include <sstream>
#include <chrono>
#include <ctime>
#include <filesystem>
#include <algorithm>
#include <iomanip>

#include "CLICommand.hpp"
#include "core/Config/Config.hpp"
#include "core/ArgumentParser.hpp"

namespace nlm {

class CLITool : public CLICommand {
public:
    CLITool() = default;
    virtual ~CLITool() = default;
    
    // Tool information
    virtual std::string getName() const override = 0;
    virtual std::string getDescription() const override = 0;
    virtual std::string getUsage() const override = 0;
    virtual CommandType getType() const override = 0;
    virtual std::string getCategory() const override = 0;
    
    // Tool execution
    virtual ExecutionResult execute(const std::vector<std::string>& args, Config& config) override = 0;
    
    // Tool-specific help
    virtual void displayHelp() const override {
        std::cout << "=== " << getName() << " ===" << std::endl;
        std::cout << std::endl;
        std::cout << "Description: " << getDescription() << std::endl;
        std::cout << std::endl;
        std::cout << "Usage: " << getUsage() << std::endl;
        std::cout << std::endl;
        std::cout << "Category: " << getCategory() << std::endl;
    }
    
    // Configuration helpers
    bool loadConfig(const std::string& configFile, Config& config) const;
    bool saveConfig(const std::string& configFile, const Config& config) const;
    
    // File helpers
    std::vector<std::string> listFiles(const std::string& directory) const;
    bool fileExists(const std::string& filePath) const;
    std::string getFileExtension(const std::string& filePath) const;
    
    // Validation helpers
    bool validateConfig(const Config& config) const;
    bool validateFileFormat(const std::string& filePath) const;
    
    // Output helpers
    void printSuccess(const std::string& message) const;
    void printError(const std::string& message) const;
    void printWarning(const std::string& message) const;
    void printInfo(const std::string& message) const;
    
    // Timing helpers
    void startTimer() const;
    double stopTimer() const;
    double getElapsedTime() const;
    
    // Argument parsing helpers
    std::string findArg(const std::vector<std::string>& args, 
                       const std::vector<std::string>& flags) const;
    
    int getIntArg(const std::vector<std::string>& args,
                  const std::vector<std::string>& flags,
                  int defaultValue = 0) const;
    
    float getFloatArg(const std::vector<std::string>& args,
                      const std::vector<std::string>& flags,
                      float defaultValue = 0.0f) const;
    
    double getDoubleArg(const std::vector<std::string>& args,
                        const std::vector<std::string>& flags,
                        double defaultValue = 0.0) const;
    
    std::string getStringArg(const std::vector<std::string>& args,
                            const std::vector<std::string>& flags,
                            const std::string& defaultValue = "") const;
    
    bool hasArg(const std::vector<std::string>& args, 
                const std::vector<std::string>& flags) const;
    
    std::vector<std::string> getStringArgs(const std::vector<std::string>& args,
                                           const std::vector<std::string>& flags) const;
    
    // JSON helpers (using nlohmann::json)
    nlohmann::json parseJsonFromFile(const std::string& filePath) const;
    bool writeJsonToFile(const nlohmann::json& json, const std::string& filePath) const;
    
    // String helpers
    std::string trim(const std::string& str) const;
    std::string toLower(const std::string& str) const;
    std::string toUpper(const std::string& str) const;
    bool startsWith(const std::string& str, const std::string& prefix) const;
    bool endsWith(const std::string& str, const std::string& suffix) const;
    
    // Time helpers
    std::string getCurrentTimestamp() const;
    std::string formatTime(double seconds) const;
    
protected:
    // Internal helper methods
    void validateArgs(const std::vector<std::string>& args,
                      const std::vector<std::string>& requiredFlags,
                      const std::map<std::string, std::string>& optionalFlags) const;
    
    // Tool-specific implementations
    virtual bool loadConfigImpl(const std::string& configFile, Config& config) const;
    virtual bool saveConfigImpl(const std::string& configFile, const Config& config) const;
    
    // Performance tracking
    mutable std::chrono::steady_clock::time_point startTime_;
    mutable double elapsedTime_;
};

} // namespace nlm