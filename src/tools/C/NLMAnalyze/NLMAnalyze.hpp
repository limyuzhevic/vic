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
#include <random>
#include <cmath>
#include "../core/Config/Config.hpp"
#include "../core/ErrorHandling/Error.hpp"
#include "../core/InputValidation/InputValidator.hpp"

namespace nlm {
namespace tools {

class NLMAnalyze {
public:
    NLMAnalyze() = default;
    ~NLMAnalyze() = default;
    
    // Main entry point
    static int run(int argc, char* argv[]);
    
    // Configuration loading
    static bool loadConfig(const std::string& filepath, std::map<std::string, std::string>& config);
    
    // Network analysis methods
    static void analyzeConnectivity(const std::map<std::string, std::string>& config);
    static void analyzeClustering(const std::map<std::string, std::string>& config);
    static void analyzeTopology(const std::map<std::string, std::string>& config);
    static void analyzePerformance(const std::map<std::string, std::string>& config);
    
    // Help
    static void printHelp();
    
private:
    // Utility functions
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    static double randomDouble(double min, double max);
    static int randomInt(int min, int max);
    
    // Analysis methods
    static void runAllAnalyses(const std::map<std::string, std::string>& config);
    static void generateBrainData(const std::map<std::string, std::string>& config, 
                                 std::map<std::string, double>& brainData);
    static void analyzeConnectivityInternal(const std::map<std::string, double>& brainData);
    static void analyzeClusteringInternal(const std::map<std::string, double>& brainData);
    static void analyzeTopologyInternal(const std::map<std::string, double>& brainData);
    static void analyzePerformanceInternal(const std::map<std::string, double>& brainData);
};

} // namespace tools
} // namespace nlm

// Entry point for the executable
int main(int argc, char* argv[]) {
    return nlm::tools::NLMAnalyze::run(argc, argv);
}
