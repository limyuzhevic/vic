#pragma once

#include "Brain.hpp"
#include "../core/Config/Config.hpp"
#include "../core/Logger/Logger.hpp"
#include "../memory/NeuralWorkingMemory.hpp"
#include "../memory/NeuralEpisodicMemory.hpp"
#include "../memory/NeuralAssociativeMemory.hpp"
#include "../prediction/NeuralPrediction.hpp"
#include "../cognition/NeuralPlanner.hpp"
#include "../neuromodulation/Dopamine.hpp"
#include "../neuromodulation/Curiosity.hpp"
#include "../neuromodulation/Novelty.hpp"
#include "../neuromodulation/PredictionError.hpp"
#include "../development/DevelopmentSystem.hpp"
#include "../sensory/SensoryInput.hpp"
#include "../motor/Action.hpp"
#include <memory>
#include <string>
#include <vector>
#include <unordered_map>
#include <functional>
#include <sstream>
#include <fstream>

namespace nlm {

class SimplifiedBrain {
public:
    // Simple factory methods for quick setup
    static std::shared_ptr<Brain> createQuickBrain(size_t neurons = 1000, size_t regions = 1);
    static std::shared_ptr<Brain> createBasicBrain(const std::string& configFile = "");
    static std::shared_ptr<Brain> createLearningBrain(size_t neurons = 500);
    
    // Beginner-friendly operation patterns
    class BeginnerAPI {
    public:
        // Basic brain operation - beginner friendly
        static bool initialize(std::shared_ptr<Brain> brain);
        static void step(std::shared_ptr<Brain> brain, size_t steps = 1);
        static void receiveInput(std::shared_ptr<Brain> brain, const std::vector<float>& input);
        static void learnFromExperience(std::shared_ptr<Brain> brain, 
                                        const std::vector<float>& beforeState, 
                                        const std::vector<float>& afterState, 
                                        float reward);
        static void reset(std::shared_ptr<Brain> brain);
        static void save(std::shared_ptr<Brain> brain, const std::string& filepath);
        static bool load(std::shared_ptr<Brain> brain, const std::string& filepath);
        
        // Memory management (simplified)
        static void clearWorkingMemory(std::shared_ptr<Brain> brain);
        static void clearEpisodicMemory(std::shared_ptr<Brain> brain);
        static void clearAssociativeMemory(std::shared_ptr<Brain> brain);
        static size_t getMemoryStats(std::shared_ptr<Brain> brain, std::string& stats);
        
        // Monitoring and debugging
        static void printStatus(std::shared_ptr<Brain> brain);
        static float getCurrentPerformance(std::shared_ptr<Brain> brain);
        static std::string getBrainHealth(std::shared_ptr<Brain> brain);
        static std::vector<float> getRecentActivity(std::shared_ptr<Brain> brain, size_t steps = 10);
        
        // Interaction with environment
        static std::unique_ptr<Action> getAction(std::shared_ptr<Brain> brain);
        static void applyReward(std::shared_ptr<Brain> brain, float reward);
        static void applyPunishment(std::shared_ptr<Brain> brain, float punishment);
        
        // Common tasks
        static void runLearningSession(std::shared_ptr<Brain> brain, size_t steps);
        static void runSimulation(std::shared_ptr<Brain> brain, size_t steps);
        static void runExploration(std::shared_ptr<Brain> brain, size_t steps);
        static void runTraining(std::shared_ptr<Brain> brain, size_t epochs);
    };
    
    // Expert API - direct access to complex systems
    class ExpertAPI {
    public:
        // Direct access to brain systems (for experts)
        static SpikeSystem* getSpikeSystem(std::shared_ptr<Brain> brain);
        static STDP* getSTDP(std::shared_ptr<Brain> brain);
        static Hebbian* getHebbian(std::shared_ptr<Brain> brain);
        static StructuralPlasticity* getStructuralPlasticity(std::shared_ptr<Brain> brain);
        static NeuralWorkingMemory* getWorkingMemory(std::shared_ptr<Brain> brain);
        static NeuralEpisodicMemory* getEpisodicMemory(std::shared_ptr<Brain> brain);
        static NeuralAssociativeMemory* getAssociativeMemory(std::shared_ptr<Brain> brain);
        static PredictionSystem* getPredictionSystem(std::shared_ptr<Brain> brain);
        static NeuralPlanner* getPlanner(std::shared_ptr<Brain> brain);
        static ConceptFormation* getConceptFormation(std::shared_ptr<Brain> brain);
        static AttentionalSelection* getAttention(std::shared_ptr<Brain> brain);
        static DevelopmentSystem* getDevelopmentSystem(std::shared_ptr<Brain> brain);
        static Dopamine* getDopamine(std::shared_ptr<Brain> brain);
        static Curiosity* getCuriosity(std::shared_ptr<Brain> brain);
        static Novelty* getNovelty(std::shared_ptr<Brain> brain);
        static PredictionError* getPredictionError(std::shared_ptr<Brain> brain);
        
        // Complex configuration (for experts)
        static void configureAdvanced(std::shared_ptr<Brain> brain, 
                                     const std::unordered_map<std::string, float>& params);
        static void configurePlasticity(std::shared_ptr<Brain> brain,
                                       const std::string& plasticityType,
                                       const std::unordered_map<std::string, float>& params);
        static void configureDevelopment(std::shared_ptr<Brain> brain,
                                         DevelopmentalStage stage);
        static void configureMemory(std::shared_ptr<Brain> brain,
                                   const std::string& memoryType,
                                   const std::unordered_map<std::string, size_t>& params);
    };
    
    // Monitoring and debugging tools
    static float getBrainStats(std::shared_ptr<Brain> brain, std::string& statsOutput);
    static std::string getDiagnosticReport(std::shared_ptr<Brain> brain);
    static std::vector<std::string> getSystemStatus(std::shared_ptr<Brain> brain);
    static bool isBrainHealthy(std::shared_ptr<Brain> brain, float minHealth = 0.7f);
    
    // Monitoring integration
    static void setupMonitoring(std::shared_ptr<Brain> brain, 
                               const std::string& logFile = "",
                               size_t logInterval = 100);
    static void setupDebugMode(std::shared_ptr<Brain> brain);
    static void setupPerformanceMode(std::shared_ptr<Brain> brain);
    static void setupPowerMode(std::shared_ptr<Brain> brain);
    
    // Interaction patterns
    static void simulateEpisode(std::shared_ptr<Brain> brain, 
                                const std::vector<std::vector<float>>& observations,
                                const std::vector<ActionType>& actions,
                                const std::vector<float>& rewards,
                                bool learn = true);
    static float evaluateBehavior(std::shared_ptr<Brain> brain,
                                 const std::vector<float>& targetState);
    static void resetBrain(std::shared_ptr<Brain> brain);
    static void saveBrainState(std::shared_ptr<Brain> brain, const std::string& filepath);
    static bool loadBrainState(std::shared_ptr<Brain> brain, const std::string& filepath);
    
    // Memory management utilities
    static void cleanupUnusedMemory(std::shared_ptr<Brain> brain);
    static size_t getMemoryUsage(std::shared_ptr<Brain> brain);
    static void clearMemory(std::shared_ptr<Brain> brain, 
                           const std::string& memoryType = "all");
    static void optimizeMemory(std::shared_ptr<Brain> brain);
    
    // Debug and development helpers
    static void printBrainArchitecture(std::shared_ptr<Brain> brain);
    static void analyzeBrainState(std::shared_ptr<Brain> brain, 
                                std::string& analysisOutput);
    static void findOptimalConfig(std::shared_ptr<Brain> brain,
                                 std::unordered_map<std::string, float>& bestConfig);
    static void testBrainStability(std::shared_ptr<Brain> brain, 
                                  size_t testSteps = 1000);
    static void profileBrainPerformance(std::shared_ptr<Brain> brain,
                                        std::string& profileOutput);
    
    // Legacy compatibility - access to original Brain interface
    static Brain* getRawBrain(std::shared_ptr<Brain> brainWrapper) {
        return brainWrapper.get();
    }
};

} // namespace nlm
