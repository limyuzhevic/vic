// NLM Analysis Tool
//
// Simple analysis and reporting tool for advanced users
// Provides statistical analysis and data export capabilities
// Focused on practical functionality with immediate value

#pragma once

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <map>
#include <algorithm>
#include <numeric>
#include <cmath>
#include <iomanip>
#include <sstream>
#include <chrono>
#include <ctime>
#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"

namespace nlm {

// Simple analysis data structure
struct BrainAnalysis {
    std::string timestamp;
    size_t neuronCount;
    size_t synapseCount;
    size_t totalSpikes;
    size_t firingNeuronCount;
    double averageFiringRate;
    double excitationInhibitionRatio;
    double averageWeight;
    double weightVariance;
    size_t mostActiveNeuronCount;
    double noveltyLevel;
    double curiosityLevel;
    double dopamineLevel;
    double energyUsage;
    double developmentAge;
    std::string developmentalStage;
};

// Statistical utilities
class Statistics {
public:
    static double mean(const std::vector<double>& data);
    static double median(const std::vector<double>& data);
    static double standardDeviation(const std::vector<double>& data, double mean = 0.0);
    static double min(const std::vector<double>& data);
    static double max(const std::vector<double>& data);
    static double range(const std::vector<double>& data);
    static std::string getDistributionType(const std::vector<double>& data);
    
    static std::string formatNumber(double value, int decimals = 2);
    static std::string formatPercentage(double value);
    static std::string formatBytes(size_t bytes);
    static std::string formatDuration(double seconds);
    static std::string getCurrentTimestamp();
    static std::string trim(const std::string& str);
    static bool isNumber(const std::string& str);
};

// Brain analysis utilities
class BrainAnalyzer {
public:
    static BrainAnalysis analyzeBrain(const std::shared_ptr<Brain>& brain);
    static std::vector<BrainAnalysis> analyzeBrainTimeline(const std::vector<std::shared_ptr<Brain>>& brains);
    static std::string generateAnalysisReport(const BrainAnalysis& analysis);
    static void printAnalysis(const BrainAnalysis& analysis);
    static bool exportAnalysis(const BrainAnalysis& analysis, const std::string& filename);
    static bool exportAnalysis(const std::vector<BrainAnalysis>& analyses, const std::string& filename);
    
private:
    static std::string getTimestamp();
    static double calculateAverageFiringRate(const std::shared_ptr<Brain>& brain);
    static double calculateExcitatoryInhibitoryRatio(const std::shared_ptr<Brain>& brain);
    static double calculateAverageWeight(const std::shared_ptr<Brain>& brain);
    static double calculateWeightVariance(const std::shared_ptr<Brain>& brain);
    static size_t calculateMostActiveNeuronCount(const std::shared_ptr<Brain>& brain);
    static double calculateNoveltyLevel(const std::shared_ptr<Brain>& brain);
    static double calculateCuriosityLevel(const std::shared_ptr<Brain>& brain);
    static double calculateDopamineLevel(const std::shared_ptr<Brain>& brain);
    static double calculateEnergyUsage(const std::shared_ptr<Brain>& brain);
    static double calculateDevelopmentAge(const std::shared_ptr<Brain>& brain);
    static std::string calculateDevelopmentalStage(const std::shared_ptr<Brain>& brain);
};

// Command-line analysis tool
class AnalysisTool {
public:
    struct AnalysisArgs {
        std::string inputFile;
        std::string outputFile;
        std::string format;
        std::string analysisType;
        bool verbose;
        bool quiet;
        bool validate;
        bool export;
        bool statistics;
        std::map<std::string, std::string> customOptions;
    };
    
    static int run(int argc, char** argv);
    static int analyzeBrainFile(const std::string& filename, const AnalysisArgs& args);
    static void printHelp();
    static void printVersion();
    static AnalysisArgs parseArgs(int argc, char** argv);
    
private:
    static void setupLogging(const AnalysisArgs& args);
    static bool loadDataFromFile(const std::string& filename, std::vector<BrainAnalysis>& data);
    static std::vector<BrainAnalysis> performAnalysis(const std::vector<BrainAnalysis>& data, const AnalysisArgs& args);
    static void displayResults(const std::vector<BrainAnalysis>& data, const AnalysisArgs& args);
    static void exportResults(const std::vector<BrainAnalysis>& data, const AnalysisArgs& args);
    static bool isFileExists(const std::string& filename);
    static std::string getFileExtension(const std::string& filename);
    static std::vector<double> parseCSVLine(const std::string& line);
    static std::string escapeCSV(const std::string& field);
    static std::string trim(const std::string& str);
};

} // namespace nlm