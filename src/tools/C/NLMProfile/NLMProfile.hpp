#pragma once

#include <string>
#include <vector>
#include <map>
#include <iostream>
#include <fstream>
#include <sstream>
#include <filesystem>
#include <algorithm>
#include <cctype>
#include <iomanip>
#include <chrono>
#include <thread>
#include <random>
#include <cmath>
#include <psutil.h>
#include "../core/Config/Config.hpp"
#include "../core/ErrorHandling/Error.hpp"
#include "../core/InputValidation/InputValidator.hpp"

namespace nlm {
namespace tools {

class NLMProfile {
public:
    NLMProfile() = default;
    ~NLMProfile() = default;
    
    // Main entry point
    static int run(int argc, char* argv[]);
    
    // Configuration loading
    static bool loadConfig(const std::string& filepath, std::map<std::string, std::string>& config);
    
    // Performance profiling methods
    static void profilePerformance(const std::map<std::string, std::string>& config, double duration = 60.0);
    static void profileMemory(const std::map<std::string, std::string>& config);
    static void profileMemoryLeaks(const std::map<std::string, std::string>& config, double duration = 30.0);
    static void profileBottlenecks(const std::map<std::string, std::string>& config);
    static void benchmarkConfig(const std::map<std::string, std::string>& config, int iterations = 10);
    
    // Help
    static void printHelp();
    
private:
    // Utility functions
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    static double randomDouble(double min, double max);
    static int randomInt(int min, int max);
    static std::string getCurrentTimestamp();
};

} // namespace tools
} // namespace nlm

// Entry point for the executable
int main(int argc, char* argv[]) {
    return nlm::tools::NLMProfile::run(argc, argv);
}
