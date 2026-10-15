// Memory System Integration for NLM Brain Loop
// This file implements the integration of memory systems into the main brain loop
// to ensure proper functionality as a coherent artificial brain

#pragma once

#include "Brain.hpp"
#include "memory/WorkingMemory.hpp"
#include "memory/EpisodicMemory.hpp"
#include "memory/SemanticMemory.hpp"
#include "memory/NeuralWorkingMemory.hpp"
#include "memory/NeuralEpisodicMemory.hpp"
#include "prediction/PredictionSystem.hpp"
#include "neuromodulation/Neuromodulator.hpp"
#include "cognition/NeuralPlanner.hpp"
#include "cognition/ConceptFormation.hpp"
#include "development/DevelopmentSystem.hpp"
#include "world/SimpleWorld.hpp"
#include "agent/AgentBrain.hpp"
#include <memory>
#include <vector>
#include <chrono>

namespace nlm {

// Memory Integration Configuration
struct MemoryIntegrationConfig {
    // Working memory integration
    bool enable_working_memory = true;
    float working_memory_decay_rate = 0.95f;
    size_t working_memory_capacity = 1000;
    
    // Episodic memory integration
    bool enable_episodic_memory = true;
    float episodic_memory_consolidation_rate = 0.8f;
    size_t episodic_memory_limit = 100;
    float episodic_memory_strength_threshold = 0.5f;
    
    // Semantic memory integration
    bool enable_semantic_memory = true;
    float semantic_memory_learning_rate = 0.01f;
    float semantic_memory_stability_threshold = 0.7f;
    
    // Predictive coding integration
    bool enable_predictive_coding = true;
    float prediction_learning_rate = 0.001f;
    float prediction_error_threshold = 0.1f;
    
    // Memory consolidation
    bool enable_memory_consolidation = true;
    float consolidation_decay_rate = 0.9f;
    std::chrono::milliseconds consolidation_interval{1000L}; // 1 second
    
    // Replay mechanisms
    bool enable_replay = true;
    float replay_stability_boost = 1.5f;
    size_t replay_sample_size = 10;
    
    // Memory-gateway interfaces
    bool enable_memory_to_prediction = true;
    bool enable_prediction_to_memory = true;
    bool enable_memory_to_cognition = true;
    bool enable_cognition_to_memory = true;
    
    // Development effects on memory
    bool enable_development_memory_effects = true;
    float developmental_plasticity_modifier = 1.0f;
};

// Enhanced Brain with Memory Integration
class IntegratedBrain {
public:
    IntegratedBrain(std::shared_ptr<Brain> brain, 
                   std::shared_ptr<MemoryIntegrationConfig> config = nullptr);
    ~IntegratedBrain();
    
    // Disable copying, enable moving
    IntegratedBrain(const IntegratedBrain&) = delete;
    IntegratedBrain& operator=(const IntegratedBrain&) = delete;
    IntegratedBrain(IntegratedBrain&&) noexcept;
    IntegratedBrain& operator=(IntegratedBrain&&) noexcept;
    
    // Initialize with all integrated systems
    bool initialize();
    
    // Main simulation step with full memory integration
    void step(SimulationStep currentStep, Timestamp currentTime);
    
    // Enhanced sensory input processing with memory
    void receiveSensoryInput(const class SensoryInput& input, 
                           const Timestamp& timestamp);
    
    // Memory-specific operations
    void storeWorkingMemory(const class NeuralWorkingMemory& memory);
    void storeEpisodicMemory(const class NeuralEpisodicMemory& memory);
    void storeSemanticMemory(const class SemanticMemory& memory);
    
    // Memory retrieval operations
    class NeuralWorkingMemory* getWorkingMemory();
    class NeuralEpisodicMemory* getEpisodicMemory();
    class SemanticMemory* getSemanticMemory();
    
    // Prediction system integration
    void updatePredictionSystem();
    void integratePredictionWithMemory();
    
    // Neuromodulation effects on memory
    void applyNeuromodulationToMemory(const class Neuromodulator& signal);
    void applyDopamineToMemory(float dopamineLevel);
    void applyCuriosityToMemory(float curiosityLevel);
    void applyNoveltyToMemory(float noveltyLevel);
    void applyPredictionErrorToMemory(float predictionError);
    
    // Memory consolidation
    void consolidateMemories();
    void replayMemories();
    
    // Development effects on memory
    void updateDevelopmentEffects();
    void applyDevelopmentalChangesToMemory();
    
    // Enhanced cognition integration
    void updateCognitionSystems();
    void integrateCognitionWithMemory();
    
    // Memory-gateway interfaces
    void memoryToPredictionInterface();
    void predictionToMemoryInterface();
    void memoryToCognitionInterface();
    void cognitionToMemoryInterface();
    
    // Environment integration
    void integrateWithAgent(std::shared_ptr<class AgentBrain> agent);
    void integrateWithWorld(std::shared_ptr<class SimpleWorld> world);
    
    // Memory statistics and monitoring
    size_t getWorkingMemoryUsage() const;
    size_t getEpisodicMemoryUsage() const;
    size_t getSemanticMemoryUsage() const;
    float getMemoryConsolidationProgress() const;
    float getMemoryRetrievalStrength() const;
    
    // Checkpoint and persistence
    bool saveIntegratedState(const std::string& filepath);
    bool loadIntegratedState(const std::string& filepath);
    
    // Memory quality metrics
    float getMemoryStabilityScore() const;
    float getMemoryRecencyScore() const;
    float getMemoryCohesionScore() const;
    float getMemoryUtilityScore() const;
    
    // Monitoring and debugging
    void logMemoryStatus() const;
    void printMemoryStats() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

// Memory Integration Manager
class MemoryIntegrationManager {
public:
    MemoryIntegrationManager(std::shared_ptr<IntegratedBrain> integrated_brain);
    ~MemoryIntegrationManager();
    
    // Update all memory integration components
    void update(double timestep);
    
    // Coordinate between memory systems
    void coordinateMemorySystems();
    void optimizeMemoryAllocation();
    void manageMemoryResources();
    
    // Learning and adaptation
    void adaptMemoryAllocation();
    void learnFromMemoryUsage();
    void adjustMemoryParameters();
    
    // Error handling and recovery
    void handleMemoryErrors();
    void recoverFromMemoryFailures();
    void validateMemoryIntegrity();
    
    // Configuration management
    void updateConfiguration(const MemoryIntegrationConfig& config);
    MemoryIntegrationConfig getConfiguration() const;
    
    // Monitoring and statistics
    size_t getTotalMemoryUsage() const;
    size_t getActiveMemorySystems() const;
    float getMemoryEfficiency() const;
    float getIntegrationQuality() const;
    
    // Event handling
    void handleMemoryEvent(const std::string& event_type, const std::string& data);
    void registerMemoryEventCallback(const std::string& event_type, 
                                     std::function<void(const std::string&)> callback);
    
private:
    struct Impl;
    Impl* pImpl;
};

// Memory Integration Utilities
namespace MemoryIntegrationUtils {
    // Memory consolidation algorithms
    float computeMemoryStability(const class NeuralWorkingMemory& memory);
    float computeMemoryRecency(const class NeuralEpisodicMemory& memory);
    float computeMemoryCohesion(const class SemanticMemory& memory);
    float computeMemoryUtility(const class NeuralWorkingMemory& memory,
                               const class NeuralEpisodicMemory& memory,
                               const class SemanticMemory& memory);
    
    // Replay mechanisms
    std::vector<class NeuralEpisodicMemory> selectReplayMemories(
        const class NeuralEpisodicMemory& memory, 
        size_t sample_size);
    void replayMemory(const class NeuralEpisodicMemory& memory, 
                     float stability_boost);
    
    // Integration algorithms
    void integratePredictionWithMemory(
        const class PredictionSystem& prediction,
        class NeuralWorkingMemory& working_memory,
        class NeuralEpisodicMemory& episodic_memory);
    
    void integrateCognitionWithMemory(
        const class NeuralPlanner& planner,
        const class ConceptFormation& concept_formation,
        class NeuralWorkingMemory& working_memory,
        class SemanticMemory& semantic_memory);
    
    // Development effects
    void applyDevelopmentToMemory(
        const DevelopmentSystem& development,
        class NeuralWorkingMemory& working_memory,
        class NeuralEpisodicMemory& episodic_memory,
        class SemanticMemory& semantic_memory,
        float plasticity_modifier);
    
    // Memory-gateway interfaces
    void memoryToPredictionGateway(
        const class NeuralWorkingMemory& working_memory,
        const class NeuralEpisodicMemory& episodic_memory,
        const class SemanticMemory& semantic_memory,
        class PredictionSystem& prediction_system);
    
    void predictionToMemoryGateway(
        const class PredictionSystem& prediction,
        class NeuralWorkingMemory& working_memory,
        class NeuralEpisodicMemory& episodic_memory,
        class SemanticMemory& semantic_memory);
    
    void memoryToCognitionGateway(
        const class NeuralWorkingMemory& working_memory,
        const class NeuralEpisodicMemory& episodic_memory,
        const class SemanticMemory& semantic_memory,
        class NeuralPlanner& planner,
        class ConceptFormation& concept_formation);
    
    void cognitionToMemoryGateway(
        const class NeuralPlanner& planner,
        const class ConceptFormation& concept_formation,
        class NeuralWorkingMemory& working_memory,
        class NeuralEpisodicMemory& episodic_memory,
        class SemanticMemory& semantic_memory);
}

// Memory Integration Configuration Presets
namespace MemoryIntegrationPresets {
    MemoryIntegrationConfig createDefaultConfig();
    MemoryIntegrationConfig createHighPerformanceConfig();
    MemoryIntegrationConfig createAccuracyFocusedConfig();
    MemoryIntegrationConfig createBalancedConfig();
    MemoryIntegrationConfig createExperimentalConfig();
    MemoryIntegrationConfig createResearchConfig();
}

// Memory Integration Testing Framework
class MemoryIntegrationTester {
public:
    MemoryIntegrationTester(std::shared_ptr<IntegratedBrain> integrated_brain);
    ~MemoryIntegrationTester();
    
    // Test memory integration functionality
    bool testWorkingMemoryIntegration();
    bool testEpisodicMemoryIntegration();
    bool testSemanticMemoryIntegration();
    bool testPredictionMemoryIntegration();
    bool testNeuromodulationMemoryIntegration();
    bool testDevelopmentMemoryIntegration();
    bool testCognitionMemoryIntegration();
    
    // Test integration quality
    bool testIntegrationQuality();
    bool testMemoryStability();
    bool testMemoryRetrieval();
    bool testMemoryConsolidation();
    bool testMemoryReplay();
    
    // Test performance and efficiency
    bool testPerformance();
    bool testMemoryUsage();
    bool testIntegrationOverhead();
    bool testRealTimeConstraints();
    
    // Generate test reports
    std::string generateTestReport() const;
    void saveTestReport(const std::string& filepath) const;
    void printTestReport() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
