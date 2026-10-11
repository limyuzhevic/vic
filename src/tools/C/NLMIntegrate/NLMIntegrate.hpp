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
#include "../core/Config/Config.hpp"
#include "../core/ErrorHandling/Error.hpp"
#include "../core/InputValidation/InputValidator.hpp"

namespace nlm {
namespace tools {

class NLMIntegrate {
public:
    NLMIntegrate() = default;
    ~NLMIntegrate() = default;
    
    // Main entry point
    static int run(int argc, char* argv[]);
    
    // Configuration loading
    static bool loadConfig(const std::string& filepath, std::map<std::string, std::string>& config);
    
    // Integration testing methods
    static void verifyIntegration(const std::map<std::string, std::string>& config);
    static void runQuickTest(const std::map<std::string, std::string>& config);
    static void runComprehensiveTest(const std::map<std::string, std::string>& config);
    static void runAllTests(const std::map<std::string, std::string>& config);
    static void performHealthCheck(const std::map<std::string, std::string>& config);
    static void generateIntegrationReport(const std::map<std::string, std::string>& config, const std::string& outputFile);
    
    // Help
    static void printHelp();
    
private:
    // Utility functions
    static std::string trim(const std::string& str);
    static std::string getCurrentTimestamp();
};

} // namespace tools
} // namespace nlm

// Entry point for the executable
int main(int argc, char* argv[]) {
    return nlm::tools::NLMIntegrate::run(argc, argv);
}
