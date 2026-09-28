// CLI Base Utilities for NLM
// Common CLI infrastructure for all NLM command-line tools

#pragma once

#include <memory>
#include <string>
#include <vector>
#include <unordered_map>
#include <functional>
#include <iostream>
#include <chrono>
#include <iomanip>
#include <sstream>
#include <stdexcept>
#include <fstream>
#include <filesystem>
#include <algorithm>

#include "../core/Config/Config.hpp"

namespace nlm {
namespace cli {

class CLIError : public std::runtime_error {
public:
    explicit CLIError(const std::string& message) : std::runtime_error(message) {}
};

class CLIUtils {
public:
    static void printBanner(const std::string& toolName, const std::string& version = "1.0.0");
    static void printError(const std::string& message);
    static void printWarning(const std::string& message);
    static void printSuccess(const std::string& message);
    static void printInfo(const std::string& message);
    static void printHeader(const std::string& title);
    static void printSeparator(char separator = '-', size_t width = 80);
    
    static std::string currentTimestamp();
    static std::string formatDuration(std::chrono::milliseconds duration);
    static std::string trim(const std::string& str);
    static bool endsWith(const std::string& str, const std::string& suffix);
    static std::string toLower(const std::string& str);
    static std::string toUpper(const std::string& str);
    static size_t fileSize(const std::string& filepath);
    static std::string humanReadableSize(size_t bytes);
    
    static void ensureDirectoryExists(const std::string& path);
    static bool fileExists(const std::string& path);
    static bool directoryExists(const std::string& path);
};

class CLISetup {
public:
    static std::shared_ptr<Config> loadConfig(const std::string& configFile = "configs/default.cfg");
    static std::shared_ptr<Config> createConfig(const std::string& configFile = "configs/default.cfg");
    static bool validateConfig(const Config& config);
    static std::string configSummary(const Config& config);
};

class CLITiming {
public:
    CLITiming() : startTime(std::chrono::high_resolution_clock::now()) {}
    ~CLITiming() = default;
    
    void reset() { startTime = std::chrono::high_resolution_clock::now(); }
    std::chrono::milliseconds elapsed() const {
        return std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::high_resolution_clock::now() - startTime);
    }
    double elapsedSeconds() const { return elapsed().count() / 1000.0; }
    
private:
    std::chrono::high_resolution_clock::time_point startTime;
};

} // namespace cli
} // namespace nlm
