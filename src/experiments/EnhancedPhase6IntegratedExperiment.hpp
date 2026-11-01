// Enhanced Phase 6 integration test with comprehensive verification
#include "experiments/Phase6IntegratedExperiment.hpp"
#include "core/Logger/Logger.hpp"
#include "brain/Brain.hpp"
#include "world/SimpleWorld.hpp"
#include "agent/AgentBrain.hpp"
#include "memory/NeuralWorkingMemory.hpp"
#include "memory/NeuralEpisodicMemory.hpp"
#include "neuromodulation/Neuromodulator.hpp"
#include "neuromodulation/Dopamine.hpp"
#include "neuromodulation/Curiosity.hpp"
#include "neuromodulation/Novelty.hpp"
#include "prediction/PredictionSystem.hpp"
#include "cognition/NeuralPlanner.hpp"
#include "cognition/ConceptFormation.hpp"
#include "cognition/AttentionalSelection.hpp"
#include "development/DevelopmentSystem.hpp"
#include <chrono>
#include <cmath>
#include <algorithm>
#include <random>

namespace nlm {

// Enhanced integration test results structure
struct EnhancedPhase6IntegrationResult {
    // System integration status
    bool memoryWorkingMemoryIntegrated;
    bool memoryEpisodicMemoryIntegrated;
    bool neuromodulationIntegrated;
    bool predictionIntegrated;
    bool developmentIntegrated;
    bool cognitionIntegrated;
    bool checkpointingWorks;
    bool replayWorks;
    
    // Functional integration metrics
    float integrationQualityScore;
    float systemConnectivityScore;
    float communicationEfficiency;
    float learningEffectiveness;
    float memoryRetention;
    float adaptationCapability;
    
    // Performance metrics
    float totalReward;
    float avgFiringRate;
    float avgSynapticWeight;
    float memoryEpisodesStored;
    float noveltyLevel;
    float curiosityLevel;
    float dopamineLevel;
    
    // Timestamps
    time_t startTime;
    time_t endTime;
    double totalWallClockTime;
    
    // Detailed metrics for analysis
    size_t totalSteps;
    size_t totalSpikes;
    size_t activeNeurons;
    size_t sensoryNeurons;
    size_t motorNeurons;
    float synapticPlasticity;
    float neuromodulatoryActivity;
    float memoryConsolidation;
    
    EnhancedPhase6IntegrationResult()
        : memoryWorkingMemoryIntegrated(false)
        , memoryEpisodicMemoryIntegrated(false)
        , neuromodulationIntegrated(false)
        , predictionIntegrated(false)
        , developmentIntegrated(false)
        , cognitionIntegrated(false)
        , checkpointingWorks(false)
        , replayWorks(false)
        , integrationQualityScore(0.0f)
        , systemConnectivityScore(0.0f)
        , communicationEfficiency(0.0f)
        , learningEffectiveness(0.0f)
        , memoryRetention(0.0f)
        , adaptationCapability(0.0f)
        , totalReward(0.0f)
        , avgFiringRate(0.0f)
        , avgSynapticWeight(0.0f)
        , memoryEpisodesStored(0.0f)
        , noveltyLevel(0.0f)
        , curiosityLevel(0.0f)
        , dopamineLevel(0.0f)
        , startTime(0)
        , endTime(0)
        , totalWallClockTime(0.0)
        , totalSteps(0)
        , totalSpikes(0)
        , activeNeurons(0)
        , sensoryNeurons(0)
        , motorNeurons(0)
        , synapticPlasticity(0.0f)
        , neuromodulatoryActivity(0.0f)
        , memoryConsolidation(0.0f) {}
};

class EnhancedPhase6IntegratedExperiment {
public:
    EnhancedPhase6IntegratedExperiment();
    ~EnhancedPhase6IntegratedExperiment();
    
    // Enhanced integration verification
    EnhancedPhase6IntegrationResult run(const Phase6Config& config);
    
    // Enhanced integration verification with more thorough testing
    bool verifyEnhancedIntegration();
    
    // Enhanced system-specific integration tests
    bool testMemoryIntegrationEnhanced();
    bool testNeuromodulationIntegrationEnhanced();
    bool testPredictionIntegrationEnhanced();
    bool testCognitionIntegrationEnhanced();
    bool testDevelopmentIntegrationEnhanced();
    
    // Comprehensive checkpoint testing
    bool testCheckpointingEnhanced();
    
    // Advanced replay testing
    bool testReplayEnhanced();
    
    // Functional integration testing
    bool testFunctionalIntegration();
    bool testLearningAndAdaptation();
    bool testMemoryEncodingAndRetrieval();
    bool testNeuromodulatoryControl();
    
    // System interconnection testing
    bool testSystemInterconnections();
    bool testBidirectionalCommunication();
    bool testFeedbackLoops();
    bool testEmergentBehavior();
    
private:
    std::shared_ptr<Brain> createAndConfigureBrain(const Phase6Config& config);
    std::shared_ptr<AgentBrain> createAndConfigureAgent(std::shared_ptr<Brain> brain, SimpleWorld& world);
    std::shared_ptr<SimpleWorld> createWorld(const Phase6Config& config);
    
    // Enhanced test methods
    bool testWorkingMemoryFunctionality(std::shared_ptr<Brain> brain);
    bool testEpisodicMemoryFunctionality(std::shared_ptr<Brain> brain);
    bool testAssociativeMemoryFunctionality(std::shared_ptr<Brain> brain);
    bool testPredictionSystemFunctionality(std::shared_ptr<Brain> brain);
    bool testPlanningSystemFunctionality(std::shared_ptr<Brain> brain);
    bool testConceptFormationFunctionality(std::shared_ptr<Brain> brain);
    bool testAttentionFunctionality(std::shared_ptr<Brain> brain);
    bool testDevelopmentSystemFunctionality(std::shared_ptr<Brain> brain);
    bool testDopamineFunctionality(std::shared_ptr<Brain> brain);
    bool testCuriosityFunctionality(std::shared_ptr<Brain> brain);
    bool testNoveltyFunctionality(std::shared_ptr<Brain> brain);
    
    // Enhanced verification methods
    bool verifyMemorySystemsIntegration(std::shared_ptr<Brain> brain);
    bool verifyNeuromodulationIntegration(std::shared_ptr<Brain> brain);
    bool verifyPredictionIntegration(std::shared_ptr<Brain> brain);
    bool verifyCognitionIntegration(std::shared_ptr<Brain> brain);
    bool verifyDevelopmentIntegration(std::shared_ptr<Brain> brain);
    
    // Calculation methods
    float calculateIntegrationQualityScore(const EnhancedPhase6IntegrationResult& result) const;
    float calculateSystemConnectivityScore(std::shared_ptr<Brain> brain) const;
    float calculateCommunicationEfficiency(std::shared_ptr<Brain> brain) const;
    float calculateLearningEffectiveness(std::shared_ptr<Brain> brain) const;
    float calculateMemoryRetention(std::shared_ptr<Brain> brain) const;
    float calculateAdaptationCapability(std::shared_ptr<Brain> brain) const;
    
    // Utility methods
    std::string generateIntegrationReport(const EnhancedPhase6IntegrationResult& result) const;
    std::string generatePerformanceReport(const EnhancedPhase6IntegrationResult& result) const;
    std::string generateFunctionalReport(const EnhancedPhase6IntegrationResult& result) const;
};