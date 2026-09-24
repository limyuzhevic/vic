// Phase 6 Integrated Experiment
// Comprehensive experiment that tests all integrated brain systems
// This is the main integration test for Phase 6 of NLM development

#pragma once

#include "Phase6Config.hpp"
#include "core/Logger/Logger.hpp"
#include <memory>
#include <vector>
#include <chrono>
#include <iomanip>
#include <sstream>

namespace nlm {

// Results from a Phase 6 integration experiment
struct Phase6ExperimentResults {
    // Basic metrics
    size_t totalSteps;                    // Total simulation steps completed
    double totalWallClockTime;            // Real time elapsed (seconds)
    size_t neuronCount;                   // Number of neurons
    float averageFiringRate;              // Average firing rate across all neurons
    double totalReward;                   // Total reward accumulated
    
    // Memory system results
    size_t memoryEpisodesStored;          // Number of episodic memory episodes stored
    bool memoryWorkingMemoryIntegrated;   // Is working memory integrated?
    bool memoryEpisodicMemoryIntegrated;  // Is episodic memory integrated?
    size_t workingMemoryTraces;           // Active working memory traces
    
    // Neuromodulation results
    bool neuromodulationIntegrated;        // Are neuromodulation systems integrated?
    float dopamineLevel;                  // Final dopamine level
    float curiosityLevel;                 // Final curiosity level
    float noveltyLevel;                    // Final novelty level
    
    // Prediction system results
    bool predictionIntegrated;            // Is prediction system integrated?
    float predictionAccuracy;             // Prediction accuracy (0-1)
    size_t predictionErrors;              // Number of prediction errors
    
    // Cognitive system results
    bool attentionIntegrated;             // Is attention integrated?
    bool planningIntegrated;              // Is planning integrated?
    bool conceptFormationIntegrated;      // Is concept formation integrated?
    
    // Development system results
    bool developmentIntegrated;            // Is development system integrated?
    int developmentalStage;               // Final developmental stage
    
    // Persistence results
    bool checkpointingWorks;               // Is checkpointing working?
    size_t checkpointsSaved;               // Number of checkpoints saved
    size_t checkpointsLoaded;              // Number of checkpoints loaded
    size_t replayEvents;                   // Number of memory replay events
    
    // Quality metrics
    float learningRate;                    // How quickly did the system learn?
    float stability;                       // System stability over time
    float adaptability;                    // System adaptability to changes
    
    // Constructor with defaults
    Phase6ExperimentResults()
        : totalSteps(0), totalWallClockTime(0.0), neuronCount(0), averageFiringRate(0.0),
          totalReward(0.0), memoryEpisodesStored(0), memoryWorkingMemoryIntegrated(false),
          memoryEpisodicMemoryIntegrated(false), workingMemoryTraces(0), neuromodulationIntegrated(false),
          dopamineLevel(0.0), curiosityLevel(0.0), noveltyLevel(0.0), predictionIntegrated(false),
          predictionAccuracy(0.0), predictionErrors(0), attentionIntegrated(false),
          planningIntegrated(false), conceptFormationIntegrated(false), developmentIntegrated(false),
          developmentalStage(0), checkpointingWorks(false), checkpointsSaved(0),
          checkpointsLoaded(0), replayEvents(0), learningRate(0.0), stability(0.0),
          adaptability(0.0) {}
    
    // Get experiment status summary
    std::string getStatusSummary() const {
        std::stringstream ss;
        ss << "=== Phase 6 Integration Results ===" << std::endl;
        ss << "Total steps: " << totalSteps << std::endl;
        ss << "Wall clock time: " << std::fixed << std::setprecision(2) << totalWallClockTime << "s" << std::endl;
        ss << "Average firing rate: " << std::fixed << std::setprecision(2) << averageFiringRate << " Hz" << std::endl;
        ss << "Total reward: " << std::fixed << std::setprecision(2) << totalReward << std::endl;
        ss << std::endl;
        
        ss << "Memory Integration:" << std::endl;
        ss << "  Working Memory: " << (memoryWorkingMemoryIntegrated ? "✓ Connected" : "✗ Disconnected") << std::endl;
        ss << "  Episodic Memory: " << (memoryEpisodicMemoryIntegrated ? "✓ Connected" : "✗ Disconnected") << std::endl;
        ss << "  Active traces: " << workingMemoryTraces << std::endl;
        ss << "  Episodes stored: " << memoryEpisodesStored << std::endl;
        ss << std::endl;
        
        ss << "Neuromodulation Integration:" << std::endl;
        ss << "  Overall: " << (neuromodulationIntegrated ? "✓ Connected" : "✗ Disconnected") << std::endl;
        ss << "  Dopamine: " << std::fixed << std::setprecision(2) << dopamineLevel << std::endl;
        ss << "  Curiosity: " << std::fixed << std::setprecision(2) << curiosityLevel << std::endl;
        ss << "  Novelty: " << std::fixed << std::setprecision(2) << noveltyLevel << std::endl;
        ss << std::endl;
        
        ss << "Cognitive Integration:" << std::endl;
        ss << "  Attention: " << (attentionIntegrated ? "✓ Connected" : "✗ Disconnected") << std::endl;
        ss << "  Planning: " << (planningIntegrated ? "✓ Connected" : "✗ Disconnected") << std::endl;
        ss << "  Concept Formation: " << (conceptFormationIntegrated ? "✓ Connected" : "✗ Disconnected") << std::endl;
        ss << std::endl;
        
        ss << "Development Integration:" << std::endl;
        ss << "  Overall: " << (developmentIntegrated ? "✓ Connected" : "✗ Disconnected") << std::endl;
        ss << "  Final stage: " << developmentalStage << std::endl;
        ss << std::endl;
        
        ss << "Prediction Integration:" << std::endl;
        ss << "  Overall: " << (predictionIntegrated ? "✓ Connected" : "✗ Disconnected") << std::endl;
        ss << "  Accuracy: " << std::fixed << std::setprecision(2) << predictionAccuracy << std::endl;
        ss << "  Errors: " << predictionErrors << std::endl;
        ss << std::endl;
        
        ss << "Persistence:" << std::endl;
        ss << "  Checkpointing: " << (checkpointingWorks ? "✓ Working" : "✗ Not Working") << std::endl;
        ss << "  Checkpoints saved: " << checkpointsSaved << std::endl;
        ss << "  Checkpoints loaded: " << checkpointsLoaded << std::endl;
        ss << "  Replay events: " << replayEvents << std::endl;
        ss << std::endl;
        
        ss << "Quality Metrics:" << std::endl;
        ss << "  Learning rate: " << std::fixed << std::setprecision(3) << learningRate << std::endl;
        ss << "  Stability: " << std::fixed << std::setprecision(3) << stability << std::endl;
        ss << "  Adaptability: " << std::fixed << std::setprecision(3) << adaptability << std::endl;
        
        return ss.str();
    }
};

// Phase 6 Integrated Experiment class
// Comprehensive experiment that runs all integrated brain systems
class Phase6IntegratedExperiment {
public:
    Phase6IntegratedExperiment();
    ~Phase6IntegratedExperiment();
    
    // Initialize experiment with brain
    void initialize(std::shared_ptr<Brain> brain);
    
    // Verify that all systems are properly integrated
    bool verifyIntegration();
    
    // Test individual system integration
    void testMemoryIntegration();
    void testNeuromodulationIntegration();
    void testCheckpointing();
    void testReplay();
    
    // Run full experiment with given configuration
    Phase6ExperimentResults run(const Phase6Config& config);
    
    // Run learning experiment (developmental progression)
    void runLearningExperiment(const Phase6Config& config);
    
    // Run validation tests
    bool runValidationTests(const Phase6Config& config);
    
    // Get brain reference
    std::shared_ptr<Brain> getBrain() const { return brain_; }
    
    // Set configuration for this experiment
    void setConfig(const Phase6Config& config) { config_ = config; }
    
    // Get current configuration
    const Phase6Config& getConfig() const { return config_; }
    
    // Get experiment results
    const Phase6ExperimentResults& getResults() const { return results_; }
    
    // Check if experiment has been completed
    bool isCompleted() const { return completed_; }
    
    // Reset experiment state
    void reset();

private:
    // Internal experiment state
    std::shared_ptr<Brain> brain_;
    Phase6Config config_;
    Phase6ExperimentResults results_;
    bool completed_;
    
    // Timing
    std::chrono::steady_clock::time_point experimentStartTime_;
    std::chrono::steady_clock::time_point experimentEndTime_;
    
    // Internal tracking
    size_t stepsCompleted_;              // Steps completed in current run
    float initialFiringRate_;            // Initial firing rate for comparison
    float finalFiringRate_;              // Final firing rate for comparison
    float cumulativeReward_;             // Cumulative reward over experiment
    
    // System status tracking
    bool memorySystemsInitialized_;     // Have memory systems been initialized?
    bool predictionSystemInitialized_;  // Have prediction systems been initialized?
    bool cognitiveSystemsInitialized_;  // Have cognitive systems been initialized?
    bool neuromodulationSystemsInitialized_;  // Have neuromodulation systems been initialized?
    bool developmentSystemsInitialized_; // Have development systems been initialized?
    
    // Helper methods
    void updateSystemStatus();
    void recordMetrics(SimulationStep step, Timestamp time);
    void finalizeResults();
    void resetExperimentState();
    
    // Integration verification methods
    bool verifyMemoryIntegration();
    bool verifyPredictionIntegration();
    bool verifyCognitiveIntegration();
    bool verifyNeuromodulationIntegration();
    bool verifyDevelopmentIntegration();
    bool verifyPersistenceIntegration();
    
    // System initialization methods
    void initializeMemorySystems();
    void initializePredictionSystems();
    void initializeCognitiveSystems();
    void initializeNeuromodulationSystems();
    void initializeDevelopmentSystems();
    
    // Validation methods
    bool validateMemoryFunctionality();
    bool validatePredictionFunctionality();
    bool validateCognitiveFunctionality();
    bool validateNeuromodulationFunctionality();
    bool validateDevelopmentFunctionality();
    bool validatePersistenceFunctionality();
};

} // namespace nlm
