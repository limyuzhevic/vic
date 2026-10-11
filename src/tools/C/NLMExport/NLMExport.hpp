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

class NLMExport {
public:
    NLMExport() = default;
    ~NLMExport() = default;
    
    // Main entry point
    static int run(int argc, char* argv[]);
    
    // Configuration loading
    static bool loadConfig(const std::string& filepath, std::map<std::string, std::string>& config);
    
    // Export methods
    static void exportConfig(const std::map<std::string, std::string>& config, const std::string& filepath, const std::string& format);
    static void exportBrainData(const std::map<std::string, double>& brainData, const std::string& filepath, const std::string& format);
    static void exportSimulationResults(const std::map<std::string, double>& results, const std::string& filepath);
    static void exportAnalysisData(const std::map<std::string, std::map<std::string, double>>& analysisData, const std::string& filepath);
    static void exportAll(const std::map<std::string, std::string>& config, const std::string& outputDir);
    
    // Help
    static void printHelp();
    
private:
    // Utility functions
    static std::string trim(const std::string& str);
    static std::string getCurrentTimestamp();
    
    // Export helper functions
    static void _exportConfigSimple(const std::map<std::string, std::string>& config, const std::string& filepath);
    static void _exportConfigJSON(const std::map<std::string, std::string>& config, const std::string& filepath);
    static void _exportConfigYAML(const std::map<std::string, std::string>& config, const std::string& filepath);
    static void _exportBrainDataJSON(const std::map<std::string, double>& brainData, const std::string& filepath);
    static void _exportBrainDataBinary(const std::map<std::string, double>& brainData, const std::string& filepath);
    static void _exportBrainDataNPY(const std::map<std::string, double>& brainData, const std::string& filepath);
    static void _exportSimulationResultsJSON(const std::map<std::string, double>& results, const std::string& filepath);
    static void _exportAnalysisDataJSON(const std::map<std::string, std::map<std::string, double>>& analysisData, const std::string& filepath);
    
    // Generate sample data
    static std::map<std::string, double> _generateSampleBrainData(const std::map<std::string, std::string>& config);
    static std::map<std::string, double> _generateSampleSimulationResults();
    static std::map<std::string, std::map<std::string, double>> _generateSampleAnalysisData();
};

} // namespace tools
} // namespace nlm

// Entry point for the executable
int main(int argc, char* argv[]) {
    return nlm::tools::NLMExport::run(argc, argv);
}
