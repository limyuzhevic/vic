// CLI Tool Implementation (cpp file)
//
// This file provides the implementation of the base CLITool class helper methods

#include "CLITool.hpp"
#include <fstream>
#include <sstream>
#include <filesystem>
#include <algorithm>
#include <iomanip>
#include <ctime>

namespace nlm {

// Configuration helpers
bool CLITool::loadConfig(const std::string& configFile, Config& config) const {
    return loadConfigImpl(configFile, config);
}

bool CLITool::saveConfig(const std::string& configFile, const Config& config) const {
    return saveConfigImpl(configFile, config);
}

// File helpers
std::vector<std::string> CLITool::listFiles(const std::string& directory) const {
    std::vector<std::string> files;
    
    if (!std::filesystem::exists(directory)) {
        return files;
    }
    
    for (const auto& entry : std::filesystem::directory_iterator(directory)) {
        if (entry.is_regular_file()) {
            files.push_back(entry.path().filename().string());
        }
    }
    
    return files;
}

bool CLITool::fileExists(const std::string& filePath) const {
    return std::filesystem::exists(filePath);
}

std::string CLITool::getFileExtension(const std::string& filePath) const {
    size_t pos = filePath.find_last_of('.');
    if (pos != std::string::npos) {
        return filePath.substr(pos + 1);
    }
    return "";
}

// Validation helpers
bool CLITool::validateConfig(const Config& config) const {
    // Basic validation - check for required fields
    return !config.getOr<int64_t>("random_seed", 0).has_value() || 
           !config.getOr<std::string>("log_level", "").has_value();
}

bool CLITool::validateFileFormat(const std::string& filePath) const {
    std::string extension = getFileExtension(filePath);
    std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);
    
    return extension == "json" || extension == "cfg" || extension == "txt";
}

// Output helpers
void CLITool::printSuccess(const std::string& message) const {
    std::cout << "[SUCCESS] " << message << std::endl;
}

void CLITool::printError(const std::string& message) const {
    std::cerr << "[ERROR] " << message << std::endl;
}

void CLITool::printWarning(const std::string& message) const {
    std::cerr << "[WARNING] " << message << std::endl;
}

void CLITool::printInfo(const std::string& message) const {
    std::cout << "[INFO] " << message << std::endl;
}

// Timing helpers
void CLITool::startTimer() const {
    startTime_ = std::chrono::steady_clock::now();
}

double CLITool::stopTimer() const {
    auto endTime = std::chrono::steady_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime_);
    elapsedTime_ = duration.count() / 1000000.0;
    return elapsedTime_;
}

double CLITool::getElapsedTime() const {
    return elapsedTime_;
}

// Argument parsing helpers
std::string CLITool::findArg(const std::vector<std::string>& args, 
                            const std::vector<std::string>& flags) const {
    for (const auto& flag : flags) {
        for (const auto& arg : args) {
            if (arg == flag) {
                return arg;
            }
            if (arg.find(flag) == 0) {
                return arg;
            }
        }
    }
    return "";
}

int CLITool::getIntArg(const std::vector<std::string>& args,
                       const std::vector<std::string>& flags,
                       int defaultValue) const {
    for (const auto& flag : flags) {
        for (size_t i = 0; i < args.size(); ++i) {
            if (args[i] == flag && i + 1 < args.size()) {
                try {
                    return std::stoi(args[i + 1]);
                } catch (...) {
                    return defaultValue;
                }
            }
        }
    }
    return defaultValue;
}

float CLITool::getFloatArg(const std::vector<std::string>& args,
                           const std::vector<std::string>& flags,
                           float defaultValue) const {
    for (const auto& flag : flags) {
        for (size_t i = 0; i < args.size(); ++i) {
            if (args[i] == flag && i + 1 < args.size()) {
                try {
                    return std::stof(args[i + 1]);
                } catch (...) {
                    return defaultValue;
                }
            }
        }
    }
    return defaultValue;
}

double CLITool::getDoubleArg(const std::vector<std::string>& args,
                             const std::vector<std::string>& flags,
                             double defaultValue) const {
    for (const auto& flag : flags) {
        for (size_t i = 0; i < args.size(); ++i) {
            if (args[i] == flag && i + 1 < args.size()) {
                try {
                    return std::stod(args[i + 1]);
                } catch (...) {
                    return defaultValue;
                }
            }
        }
    }
    return defaultValue;
}

std::string CLITool::getStringArg(const std::vector<std::string>& args,
                                  const std::vector<std::string>& flags,
                                  const std::string& defaultValue) const {
    for (const auto& flag : flags) {
        for (size_t i = 0; i < args.size(); ++i) {
            if (args[i] == flag && i + 1 < args.size()) {
                return args[i + 1];
            }
        }
    }
    return defaultValue;
}

bool CLITool::hasArg(const std::vector<std::string>& args, 
                     const std::vector<std::string>& flags) const {
    for (const auto& flag : flags) {
        if (std::find(args.begin(), args.end(), flag) != args.end()) {
            return true;
        }
    }
    return false;
}

std::vector<std::string> CLITool::getStringArgs(const std::vector<std::string>& args,
                                                const std::vector<std::string>& flags) const {
    std::vector<std::string> result;
    
    for (const auto& flag : flags) {
        for (size_t i = 0; i < args.size(); ++i) {
            if (args[i] == flag && i + 1 < args.size()) {
                result.push_back(args[i + 1]);
            }
        }
    }
    
    return result;
}

// JSON helpers
nlohmann::json CLITool::parseJsonFromFile(const std::string& filePath) const {
    std::ifstream file(filePath);
    if (!file.is_open()) {
        throw std::runtime_error("Cannot open file: " + filePath);
    }
    
    nlohmann::json json;
    file >> json;
    return json;
}

bool CLITool::writeJsonToFile(const nlohmann::json& json, const std::string& filePath) const {
    std::ofstream file(filePath);
    if (!file.is_open()) {
        return false;
    }
    
    file << json.dump(2);
    return true;
}

// String helpers
std::string CLITool::trim(const std::string& str) const {
    size_t start = str.find_first_not_of(" \t\n\r");
    if (start == std::string::npos) return "";
    
    size_t end = str.find_last_not_of(" \t\n\r");
    return str.substr(start, end - start + 1);
}

std::string CLITool::toLower(const std::string& str) const {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), ::tolower);
    return result;
}

std::string CLITool::toUpper(const std::string& str) const {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), ::toupper);
    return result;
}

bool CLITool::startsWith(const std::string& str, const std::string& prefix) const {
    return str.size() >= prefix.size() && str.compare(0, prefix.size(), prefix) == 0;
}

bool CLITool::endsWith(const std::string& str, const std::string& suffix) const {
    return str.size() >= suffix.size() && str.compare(str.size() - suffix.size(), suffix.size(), suffix) == 0;
}

// Time helpers
std::string CLITool::getCurrentTimestamp() const {
    auto now = std::chrono::system_clock::now();
    auto in_time_t = std::chrono::system_clock::to_time_t(now);
    
    std::stringstream ss;
    ss << std::put_time(std::localtime(&in_time_t), "%Y-%m-%d %H:%M:%S");
    
    return ss.str();
}

std::string CLITool::formatTime(double seconds) const {
    if (seconds < 60) {
        return std::to_string(seconds) + " seconds";
    } else if (seconds < 3600) {
        int minutes = static_cast<int>(seconds) / 60;
        int secs = static_cast<int>(seconds) % 60;
        return std::to_string(minutes) + " minutes, " + std::to_string(secs) + " seconds";
    } else {
        int hours = static_cast<int>(seconds) / 3600;
        int minutes = (static_cast<int>(seconds) % 3600) / 60;
        int secs = static_cast<int>(seconds) % 60;
        return std::to_string(hours) + " hours, " + std::to_string(minutes) + " minutes, " + std::to_string(secs) + " seconds";
    }
}

// Tool-specific implementations
bool CLITool::loadConfigImpl(const std::string& configFile, Config& config) const {
    if (!fileExists(configFile)) {
        return false;
    }
    
    std::string extension = getFileExtension(configFile);
    std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);
    
    std::ifstream file(configFile);
    if (!file.is_open()) {
        return false;
    }
    
    if (extension == "json") {
        nlohmann::json jsonConfig;
        file >> jsonConfig;
        config.loadFromJson(jsonConfig);
        return true;
    } else {
        return config.loadFromFile(configFile);
    }
}

bool CLITool::saveConfigImpl(const std::string& configFile, const Config& config) const {
    nlohmann::json jsonConfig;
    config.saveToJson(jsonConfig);
    
    return writeJsonToFile(jsonConfig, configFile);
}

void CLITool::validateArgs(const std::vector<std::string>& args,
                          const std::vector<std::string>& requiredFlags,
                          const std::map<std::string, std::string>& optionalFlags) const {
    // Basic validation
    for (const auto& flag : requiredFlags) {
        bool found = false;
        for (const auto& arg : args) {
            if (arg == flag) {
                found = true;
                break;
            }
        }
        if (!found) {
            printError("Missing required argument: " + flag);
        }
    }
}

} // namespace nlm