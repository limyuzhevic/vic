// Phase 3 Experiment Header
#pragma once

#include "Experiment.hpp"
#include <memory>
#include <vector>
#include <string>

// Forward declarations for experimental components
class Phase3Brain;
class Phase3NeuromodulationSystem;
class Phase3MemorySystem;
class Phase3PredictionSystem;
class Phase3DevelopmentSystem;
class Phase3SensorySystem;
class Phase3Environment;
class Phase3AgentBrain;
class Phase3SensoryPercept;

struct ExperimentalMetrics {
    float neuralEfficiency = 0.0f;
    float reinforcementGain = 0.0f;
    float memoryRetention = 0.0f;
    float predictionAccuracy = 0.0f;
    float developmentProgress = 0.0f;
};

class Phase3Experiment : public Experiment {
public:
    Phase3Experiment();
    ~Phase3Experiment();
    
    void setup() override;
    void run() override;
    void stop() override;
    
private:
    void executeProtocol();
    void executeStep(int step);
    void collectStepMetrics(int step);
    void recordExperimentalData(int step);
    void adjustProtocolDynamically(int step);
    void collectResults();
    void analyzeData();
    void generateReport();
    void performStatisticalAnalysis();
    void identifyPatterns();
    void generatePredictiveModels();
    void createVisualizations();
    void createComprehensiveReport();
    void exportExperimentalData();
    void generateResearchAbstract();
    void createPresentationMaterials();
    void calculateExperimentalStatistics();
    void identifySignificantFindings();
    void validateHypotheses();
    float calculateVarianceFromMetrics() const;
    float calculateLearningStability() const;
    
    // Experimental data structures
    struct ExperimentalResults {
        int protocolSteps = 0;
        int stepsCompleted = 0;
        int simulationTime = 0;
        float meanEfficiency = 0.0f;
        float maxEfficiency = 0.0f;
        float minEfficiency = 0.0f;
        float variance = 0.0f;
        std::vector<ExperimentalMetrics> stepMetrics;
    };
    
    // Experimental components
    std::unique_ptr<Phase3Brain> brain;
    std::unique_ptr<Phase3NeuromodulationSystem> neuromodulation;
    std::unique_ptr<Phase3MemorySystem> memory;
    std::unique_ptr<Phase3PredictionSystem> prediction;
    std::unique_ptr<Phase3DevelopmentSystem> development;
    std::unique_ptr<Phase3SensorySystem> sensory;
    std::unique_ptr<Phase3Environment> environment;
    std::unique_ptr<Phase3AgentBrain> agentBrain;
    std::unique_ptr<Phase3SensoryPercept> sensoryPercept;
    
    // Experimental data
    ExperimentalResults experimentalResults;
    ExperimentalMetrics metrics;
    std::vector<std::vector<float>> neuralActivityPatterns;
    std::vector<std::vector<float>> neuromodulationDynamics;
    std::vector<std::vector<float>> memoryTraces;
    std::vector<float> predictionErrors;
    std::vector<std::vector<float>> developmentalChanges;
    std::vector<std::string> significantFindings;
    std::vector<std::pair<std::string, bool>> hypothesisValidationResults;
};
