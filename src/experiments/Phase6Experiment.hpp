#pragma once

#include "../agent/AgentBody.hpp"
#include <string>
#include <vector>

namespace nlm {

// Experiment logger for Phase 6 integration testing
class Phase6ExperimentLogger {
public:
    Phase6ExperimentLogger(const std::string& filename);
    ~Phase6ExperimentLogger();
    
    // Disable copying
    Phase6ExperimentLogger(const Phase6ExperimentLogger&) = delete;
    Phase6ExperimentLogger& operator=(const Phase6ExperimentLogger&) = delete;
    
    void log(const std::string& message);
    void logIntegrationHeader();
    void logSystemStatus(const std::string& systemName, bool integrated, const std::string& details = "");
    void logExperimentStart(int neuronCount, int maxSteps);
    void logStep(int step, float reward, float neuromod, float curiosity, 
                 float energy, const std::string& action, int firingNeurons);
    void logDevelopment(float age, const std::string& stage, float plasticityMod);
    void logMemoryStats(size_t workingMemory, size_t episodicMemory, size_t episodes);
    void logIntegrationSummary(float integrationScore, int integratedSystems, int totalSystems);
    void logExperimentEnd(float totalReward, float avgFiringRate, float curiosityLevel);
    
    void flush();
    
private:
    struct Impl;
    Impl* pImpl;
};

// Phase 6 integration experiment configuration
struct Phase6Config {
    int neuronCount;                    // Number of neurons
    int maxSteps;                       // Maximum simulation steps
    int regionCount;                    // Number of brain regions
    float connectionProbability;        // Synapse connection probability
    bool enableDevelopment;             // Enable development system
    bool enableCheckpointing;           // Enable checkpointing
    std::string checkpointPath;         // Checkpoint file path
    std::string logFile;               // Log file path
    int replayInterval;                 // Memory replay interval
    int consolidationInterval;          // Memory consolidation interval
};

// Phase 6 integration result structure
struct Phase6IntegrationResult {
    time_t startTime;                   // Experiment start time
    time_t endTime;                     // Experiment end time
    double totalWallClockTime;          // Total execution time
    float totalReward;                  // Total reward accumulated
    float avgFiringRate;                // Average firing rate
    float curiosityLevel;               // Final curiosity level
    float noveltyLevel;                 // Final novelty level
    float dopamineLevel;                // Final dopamine level
    size_t memoryEpisodesStored;        // Number of episodic memory episodes
    bool memoryWorkingMemoryIntegrated; // Working memory integration status
    bool memoryEpisodicMemoryIntegrated; // Episodic memory integration status
    bool neuromodulationIntegrated;     // Neuromodulation integration status
    bool predictionIntegrated;         // Prediction system integration status
    bool developmentIntegrated;         // Development system integration status
    bool checkpointingWorks;            // Checkpoint functionality status
};

// Phase 6 integration experiment class
class Phase6Experiment {
public:
    Phase6Experiment();
    ~Phase6Experiment();
    
    // Disable copying
    Phase6Experiment(const Phase6Experiment&) = delete;
    Phase6Experiment& operator=(const Phase6Experiment&) = delete;
    
    /**
     * Run Phase 6 integration experiment
     * @param config Phase 6 experiment configuration
     * @return Integration test results
     */
    Phase6IntegrationResult run(const Phase6Config& config);
    
    /**
     * Verify integration of all systems
     * @return true if all systems are properly integrated
     */
    bool verifyIntegration();
    
    /**
     * Test working memory integration
     * @return true if working memory is properly integrated
     */
    bool testMemoryIntegration();
    
    /**
     * Test neuromodulation integration
     * @return true if neuromodulation systems are properly integrated
     */
    bool testNeuromodulationIntegration();
    
    /**
     * Test checkpoint functionality
     * @return true if checkpointing works correctly
     */
    bool testCheckpointing();
    
    /**
     * Test replay system
     * @return true if replay system works correctly
     */
    bool testReplay();
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
