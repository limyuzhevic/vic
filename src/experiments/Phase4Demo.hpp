// Phase 4 Demo Header
#pragma once

#include "AgentBrain.hpp"
#include "SensoryPercept.hpp"
#include "Environment.hpp"
#include <memory>
#include <chrono>

// Forward declarations for enhanced components
class MetaCognitiveAgentBrain;
class EnhancedSensoryPercept;

class Phase4Demo {
public:
    struct Observation {
        float vision;
        float audio;
        float internal;
        
        Observation(float v = 0.0f, float a = 0.0f, float i = 0.0f) 
            : vision(v), audio(a), internal(i) {}
    };
    
    struct Action {
        float motor1;
        float motor2;
        float motor3;
        
        Action(float m1 = 0.0f, float m2 = 0.0f, float m3 = 0.0f)
            : motor1(m1), motor2(m2), motor3(m3) {}
    };
    
    struct PerformanceMetrics {
        int stepsCompleted = 0;
        int simulationTime = 0;
        float averageEfficiency = 0.0f;
    };
    
    class Phase4Brain {
    public:
        int neurons = 0;
        int synapses = 0;
        int regions = 0;
        int connections = 0;
        int metaCognitiveUnits = 0;
        float activation = 0.0f;
        float efficiency = 0.0f;
        float metaCognitiveScore = 0.0f;
        float selfMonitoring = 0.0f;
        
        Phase4Brain() = default;
        void process() {}
        void metaCognitiveUpdate() {}
    };
    
    class Phase4NeuromodulationSystem {
    public:
        float reward = 0.0f;
        float predictionError = 0.0f;
        float novelty = 0.0f;
        float curiosity = 0.0f;
        float acetylcholine = 0.0f;
        float norepinephrine = 0.0f;
        float serotonin = 0.0f;
        float temporalDifference = 0.0f;
        
        Phase4NeuromodulationSystem() = default;
        void update() {}
        void temporalDifferenceLearning() {}
    };
    
    class Phase4MemorySystem {
    public:
        int workingMemoryCapacity = 0;
        int episodicMemorySize = 0;
        int semanticMemorySize = 0;
        int proceduralMemorySize = 0;
        int recursiveMemoryDepth = 0;
        float consolidationRate = 0.0f;
        float retrievalSuccess = 0.0f;
        float recursiveConsolidationLevel = 0.0f;
        
        Phase4MemorySystem() = default;
        void update() {}
        void episodicRecursiveConsolidation() {}
    };
    
    class Phase4PredictionSystem {
    public:
        float predictionAccuracy = 0.0f;
        float predictionConfidence = 0.0f;
        int hierarchicalDepth = 0;
        float expectationAccuracy = 0.0f;
        
        Phase4PredictionSystem() = default;
        void update() {}
        void hierarchicalExpectationUpdating() {}
    };
    
    class Phase4DevelopmentSystem {
    public:
        float synaptogenesisRate = 0.0f;
        float pruningEfficiency = 0.0f;
        float maturationProgress = 0.0f;
        float synapticRewiringRate = 0.0f;
        
        Phase4DevelopmentSystem() = default;
        void update() {}
        void acceleratedSynapticRewiring() {}
    };
    
    class Phase4SensorySystem {
    public:
        float visionInput = 0.0f;
        float audioInput = 0.0f;
        float internalSignals = 0.0f;
        float crossModalStrength = 0.0f;
        
        Phase4SensorySystem() = default;
        void processInput() {}
        void crossModalIntegration() {}
        Observation getObservation() { return Observation(visionInput, audioInput, internalSignals); }
    };
    
    class Phase4Environment {
    public:
        float stateComplexity = 0.0f;
        float changeRate = 0.0f;
        float adaptiveChallengeLevel = 0.0f;
        
        Phase4Environment() = default;
        void update(const Observation& obs) {}
        void adaptiveChallengeGeneration() {}
        Action getAction() { return Action(0.0f, 0.0f, 0.0f); }
    };
    
    // Forward declarations for enhanced agent
    class MetaCognitiveAgentBrain;
    class EnhancedSensoryPercept;
    
    Phase4Demo();
    ~Phase4Demo();
    
    bool initialize();
    void run();
    void stop();
    void step();
    PerformanceMetrics getPerformanceMetrics() const;
    
private:
    bool running = false;
    int stepsCompleted = 0;
    PerformanceMetrics performanceMetrics;
    std::unique_ptr<Phase4Brain> brain;
    std::unique_ptr<Phase4NeuromodulationSystem> neuromodulation;
    std::unique_ptr<Phase4MemorySystem> memory;
    std::unique_ptr<Phase4PredictionSystem> prediction;
    std::unique_ptr<Phase4DevelopmentSystem> development;
    std::unique_ptr<Phase4SensorySystem> sensory;
    std::unique_ptr<Phase4Environment> environment;
    std::unique_ptr<MetaCognitiveAgentBrain> agentBrain;
    std::unique_ptr<EnhancedSensoryPercept> sensoryPercept;
    
    void enableAdvancedFeatures();
    float calculateAverageEfficiency() const;
};
