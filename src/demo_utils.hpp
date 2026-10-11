// Utilities for NLM modular demo system
// Common helper functions and utilities used across different demo components

#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <chrono>
#include <iomanip>
#include <sstream>
#include <functional>
#include <unordered_map>

namespace nlm {

// Forward declarations
class Config;
class Brain;
class Neuron;
class Synapse;

// Utility functions for console output and formatting
class ConsoleUtils {
public:
    static void printBanner();
    static void printSection(const std::string& title);
    static void printSubSection(const std::string& title);
    static void printResult(const std::string& name, float value, float max = 1.0f);
    static void printStatus(const std::string& message, bool success = true);
    static void printProgress(size_t current, size_t total, const std::string& description = "");
    static void printCompleted();
    static std::string formatTime(double seconds);
    static std::string formatNumber(size_t number);
    static std::string formatFloat(float value, int precision = 3);
};

// Configuration utilities for demo system
class DemoConfiguration {
public:
    struct DemoSettings {
        int numEpisodes = 5;
        int stepsPerEpisode = 1000;
        double simulationTimestep = 0.001;
        int64_t neuronCount = 500;
        int64_t regionCount = 1;
        float connectionProbability = 0.15f;
        int64_t randomSeed = 42;
        float stdpLtpWeight = 0.02f;
        float stdpLtdWeight = 0.015f;
        float stdpTau = 20.0f;
        float synaptogenesisRate = 0.0001f;
        float pruningRate = 0.00001f;
        bool enableRewardModulation = false;
        bool enableStructuralPlasticity = false;
        bool enableDevelopment = false;
        bool enableCuriosity = false;
        bool verboseLogging = false;
        std::string outputFile;
        
        bool parseArgs(int argc, char** argv);
        void showHelp();
        void applyToConfig(std::shared_ptr<Config> config);
    };
    
    static DemoSettings parseDemoSettings(int argc, char** argv);
};

// Result tracking and metrics collection
class DemoMetrics {
public:
    struct TestMetrics {
        std::string testName;
        double executionTime;
        bool success;
        std::string details;
        
        TestMetrics(const std::string& name) : testName(name), executionTime(0.0), success(false) {}
    };
    
    struct ExperimentMetrics {
        int episodes;
        int stepsPerEpisode;
        double totalReward;
        double averageFiringRate;
        size_t finalSynapseCount;
        size_t finalNeuronCount;
        double curiosityLevel;
        double noveltyLevel;
        double predictionError;
        std::vector<TestMetrics> testResults;
        
        void reset();
    };
    
    static void logMetrics(const ExperimentMetrics& metrics, std::ostream& output = std::cout);
    static std::string metricsToString(const ExperimentMetrics& metrics);
};

// Progress tracking for long-running operations
class ProgressTracker {
public:
    ProgressTracker(size_t totalSteps, const std::string& description = ""); 
    void update(size_t steps = 1);
    void printProgress();
    
private:
    size_t totalSteps;
    size_t completedSteps;
    std::string description;
};

// Experiment configuration and validation
class DemoValidator {
public:
    static bool validateConfiguration(const DemoConfiguration::DemoSettings& settings);
    static bool validateEnvironment();
};

// File utilities for saving experiment results
class FileUtils {
public:
    static bool writeToFile(const std::string& filename, const std::string& content);
    static bool ensureDirectory(const std::string& path);
    static std::string generateTimestamp();
};

// Demo runner for different demonstration types
class DemoRunner {
public:
    enum class DemoType {
        Phase2,
        Phase3,
        Phase4,
        Phase6,
        Complete
    };
    
    DemoRunner();
    
    bool runDemo(DemoType type, DemoConfiguration::DemoSettings& settings, 
                DemoMetrics::ExperimentMetrics& metrics);
    void listAvailableDemos();
    std::string getDemoDescription(DemoType type);
    
    struct DemoInfo {
        std::string name;
        std::string description;
        std::function<bool(DemoConfiguration::DemoSettings&, DemoMetrics::ExperimentMetrics&)> phase2Runner;
        std::function<bool(DemoConfiguration::DemoSettings&, DemoMetrics::ExperimentMetrics&)> phase3Runner;
        std::function<bool(DemoConfiguration::DemoSettings&, DemoMetrics::ExperimentMetrics&)> phase4Runner;
    };
    
private:
    std::unordered_map<DemoType, DemoInfo> demos;
};

// Function declarations for demo implementations
bool runBasicConnectivityTest(std::shared_ptr<Brain> brain);
bool runPlasticityExperiment(std::shared_ptr<Brain> brain);
bool runStdpVerification(std::shared_ptr<Brain> brain);
bool runLearningExperiment(std::shared_ptr<Brain> brain);
bool runCompleteDemo(DemoConfiguration::DemoSettings& settings, DemoMetrics::ExperimentMetrics& metrics);
bool runPhase3Demo(DemoConfiguration::DemoSettings& settings, DemoMetrics::ExperimentMetrics& metrics);
bool runPhase4Demo(DemoConfiguration::DemoSettings& settings, DemoMetrics::ExperimentMetrics& metrics);

// Global demo runner instance
extern DemoRunner globalDemoRunner;

// Helper functions
DemoRunner::DemoType parseDemoType(const std::string& typeStr);

} // namespace nlm

// Include the implementation
#include "demo_utils.cpp"
