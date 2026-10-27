// NLM Master CLI Interface
// Unified command-line interface for all NLM operations
// Provides comprehensive configuration, demo running, and analysis capabilities

#pragma once

#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include "brain/Brain.hpp"
#include "agent/AgentBrain.hpp"
#include "world/SimpleWorld.hpp"
#include "experiments/Phase3Experiment.hpp"
#include "experiments/Phase4Experiment.hpp"
#include "experiments/Phase6IntegratedExperiment.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <chrono>
#include <vector>
#include <string>
#include <map>
#include <algorithm>

using namespace nlm;

class NLMCommandLineInterface {
public:
    NLMCommandLineInterface();
    ~NLMCommandLineInterface();

    // Parse and execute command
    int execute(int argc, char* argv[]);

private:
    // Command-line argument parsing
    struct CommandOptions {
        std::string command;
        std::string configFile;
        std::string outputFile;
        size_t numEpisodes = 1;
        size_t stepsPerEpisode = 1000;
        std::string experimentPhase; // phase3, phase4, phase6
        bool enableCheckpointing = false;
        bool enableReplay = false;
        bool enableDevelopment = false;
        bool enableCuriosity = false;
        bool enableRewardModulation = false;
        bool enableStructuralPlasticity = false;
        std::string randomSeed = "42";
        std::string brainType = "standard";
        std::string worldType = "simple";
        std::string outputFormat = "text";
        std::string analysisType = "basic";
        bool benchmarkMode = false;
        bool profileMode = false;
        std::string profilerOutput;
        size_t threadCount = 1;
        bool parallelExecution = false;
        std::string batchFile;
        std::string pluginPath;
        bool listPlugins = false;
        bool help = false;
        bool version = false;
    };

    CommandOptions parseArguments(int argc, char* argv[]);
    void printHelp() const;
    void printVersion() const;
    int runCommand(const CommandOptions& options);

    // Command implementations
    int cmdRun(const CommandOptions& options);
    int cmdConfigure(const CommandOptions& options);
    int cmdBenchmark(const CommandOptions& options);
    int cmdProfile(const CommandOptions& options);
    int cmdBatch(const CommandOptions& options);
    int cmdAnalyze(const CommandOptions& options);
    int cmdPlugins(const CommandOptions& options);
    int cmdList(const CommandOptions& options);

    // Core simulation routines
    int runPhase3Demo(const CommandOptions& options);
    int runPhase4Demo(const CommandOptions& options);
    int runPhase6Demo(const CommandOptions& options);
    int runPhase3Experiment(const CommandOptions& options);
    int runPhase4Experiment(const CommandOptions& options);
    int runPhase6Experiment(const CommandOptions& options);

    // Utility functions
    void printBanner() const;
    void printStatus(const std::string& message, bool error = false) const;
    void logStep(int step, float reward, float neuromod, float curiosity,
                 float energy, const std::string& action, float firingRate) const;
    std::string getTimestamp() const;
    double getCurrentWallTime() const;
    void printConfiguration(const CommandOptions& options) const;
    void printExperimentSummary(const std::string& phase, const CommandOptions& options,
                               double startTime, double endTime, float totalReward,
                               float avgFiringRate, float memoryUsage) const;

    // File I/O
    bool saveResults(const std::string& filename, const std::string& data) const;
    std::string loadConfiguration(const std::string& filename) const;
    bool exportMetrics(const std::string& filename, const std::vector<std::string>& metrics) const;

    // Statistics and analysis
    struct ExperimentResults {
        std::string phase;
        double wallClockTime;
        double cpuTime;
        float totalReward;
        float avgFiringRate;
        float avgMemoryUsage;
        size_t totalSteps;
        size_t activeMemoryTraces;
        size_t totalEpisodes;
        float curiosityLevel;
        float noveltyLevel;
        float dopamineLevel;
        bool success;
        std::string errorMessage;
        std::vector<float> rewardHistory;
        std::vector<float> firingRateHistory;
    };

    ExperimentResults runSingleExperiment(const CommandOptions& options);
    void printResults(const ExperimentResults& results) const;
    std::string resultsToJSON(const ExperimentResults& results) const;

    // Performance profiling
    struct ProfileData {
        std::string phase;
        double totalTime;
        double brainInitTime;
        double simulationTime;
        double worldUpdateTime;
        double agentProcessingTime;
        double memoryUsageMB;
        size_t neuronsCreated;
        size_t synapsesCreated;
        size_t totalSpikes;
    };

    ProfileData profileExecution(const CommandOptions& options);
    void printProfile(const ProfileData& profile) const;

    // Configuration management
    void applyConfigFile(std::shared_ptr<Config> config, const std::string& filename) const;
    void applyCommandLineConfig(std::shared_ptr<Config> config, const CommandOptions& options) const;
    std::string getConfigSummary(const std::shared_ptr<Config>& config) const;

    // Error handling
    bool validateOptions(const CommandOptions& options) const;
    std::string getErrorMessage(int errorCode) const;

    // State
    bool verbose_;
    bool quiet_;
    std::vector<ExperimentResults> experimentHistory_;
};
