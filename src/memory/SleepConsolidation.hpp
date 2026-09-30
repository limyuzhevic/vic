#pragma once

#include "../core/Types/Types.hpp"
#include "../brain/SleepState.hpp"
#include "../memory/NeuralEpisodicMemory.hpp"
#include "../plasticity/Hebbian.hpp"
#include <vector>
#include <memory>
#include <unordered_map>
#include <functional>
#include <deque>

namespace nlm {

// Sleep-dependent memory consolidation with systems and synaptic consolidation
class SleepConsolidation {
public:
    SleepConsolidation();
    ~SleepConsolidation();
    
    // Initialize with brain reference
    void initialize(Brain* brain);
    
    // Systems consolidation (memory reorganization)
    void systemsConsolidation(SleepState sleepState);
    void consolidateMemoryNetworks(SleepState sleepState);
    void reorganizeMemorySchemas(SleepState sleepState);
    
    // Synaptic consolidation (synaptic strengthening)
    void synapticConsolidation(SleepState sleepState);
    void strengthenMemoryTraces(SleepState sleepState);
    void weakenIrrelevantConnections(SleepState sleepState);
    
    // Schema formation and abstraction
    void schemaFormation(SleepState sleepState);
    void abstractMemoryRepresentations(SleepState sleepState);
    void generalizeMemoryKnowledge(SleepState sleepState);
    
    // Memory integration and generalization
    void memoryIntegration(SleepState sleepState);
    void integrateNewMemories(SleepState sleepState);
    void generalizeToNewContexts(SleepState sleepState);
    
    // State-specific consolidation characteristics
    struct StateConsolidationParameters {
        float systemsConsolidationStrength;     // Strength of systems consolidation
        float synapticConsolidationStrength;     // Strength of synaptic consolidation
        float schemaFormationStrength;          // Strength of schema formation
        float memoryIntegrationStrength;        // Strength of memory integration
        float abstractionLevel;                  // Level of abstraction (0.0 to 1.0)
        float generalizationCapacity;            // Capacity for generalization
        bool enableReorganization;              // Whether to enable reorganization
        float neuromodulationSensitivity;       // Sensitivity to neuromodulation
    };
    
    const StateConsolidationParameters& getConsolidationParameters(SleepState state) const;
    void setConsolidationParameters(SleepState state, const StateConsolidationParameters& params);
    
    // Consolidation statistics and monitoring
    float getSystemsConsolidationProgress() const { return systemsConsolidationProgress_; }
    float getSynapticConsolidationProgress() const { return synapticConsolidationProgress_; }
    float getSchemaFormationProgress() const { return schemaFormationProgress_; }
    float getMemoryIntegrationProgress() const { return memoryIntegrationProgress_; }
    
    float getConsolidationEfficiency() const { return consolidationEfficiency_; }
    size_t getTotalConsolidationEvents() const { return totalConsolidationEvents_; }
    
    // Real-time monitoring
    float getCurrentConsolidationProgress() const { return currentConsolidationProgress_; }
    SleepState getCurrentConsolidationState() const { return currentConsolidationState_; }
    bool isConsolidationActive() const { return consolidationActive_; }
    
    // Checkpointing support
    std::string serialize() const;
    bool deserialize(const std::string& data);
    
    // Error handling and recovery
    void handleConsolidationError(const std::string& errorMessage);
    void recoverFromConsolidationError();
    bool isInErrorState() const { return inErrorState_; }
    
    // Integration with other systems
    void integrateWithHebbianPlasticity(Hebbian* hebbian);
    void integrateWithEpisodicMemory(NeuralEpisodicMemory* episodicMemory);
    void integrateWithWorkingMemory(NeuralWorkingMemory* workingMemory);
    void integrateWithNeuromodulationSystems();
    
    // Consolidation scheduling
    void scheduleConsolidation(SleepState sleepState, float duration);
    void updateConsolidationTiming(SleepState sleepState, float dt);
    void triggerConsolidationEvents(SleepState sleepState);

private:
    // Core consolidation system components
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    Brain* brain_;
    
    // Consolidation parameters for each state
    std::map<SleepState, StateConsolidationParameters> consolidationParameters_;
    
    // Statistics
    float systemsConsolidationProgress_;
    float synapticConsolidationProgress_;
    float schemaFormationProgress_;
    float memoryIntegrationProgress_;
    float consolidationEfficiency_;
    size_t totalConsolidationEvents_;
    
    // Current consolidation state
    float currentConsolidationProgress_;
    SleepState currentConsolidationState_;
    bool consolidationActive_;
    
    // Memory consolidation buffers
    std::vector<float> memoryBuffer_;
    std::vector<float> schemaBuffer_;
    std::deque<float> recentConsolidationMetrics_;
    
    // Error handling
    bool inErrorState_;
    std::string lastErrorMessage_;
    size_t consecutiveErrors_;
    
    // Consolidation timing
    float consolidationInterval_;
    float consolidationDuration_;
    float nextConsolidationStep_;
    
    // State-specific consolidation characteristics
    struct StateConsolidationCharacteristics {
        float systemsConsolidationRate;     // Rate of systems consolidation
        float synapticConsolidationRate;     // Rate of synaptic consolidation
        float schemaFormationRate;          // Rate of schema formation
        float memoryIntegrationRate;        // Rate of memory integration
        float abstractionDepth;             // Depth of abstraction
        float integrationScope;             // Scope of integration
        float replayIntegration;            // Integration with memory replay
        float neuromodulationModulation;    // Modulation by neuromodulators
    };
    
    std::map<SleepState, StateConsolidationCharacteristics> stateCharacteristics_;
    
    // Helper functions
    void updateConsolidationParameters();
    void executeStateSpecificConsolidation(SleepState state);
    void updateConsolidationBuffers();
    void integrateConsolidationWithPlasticity(SleepState state);
    void integrateConsolidationWithMemory(SleepState state);
    void updateConsolidationStatistics(float progress, bool success);
    void resetConsolidationState();
    bool validateConsolidationParameters(const StateConsolidationParameters& params) const;
    void scheduleNextConsolidation(SleepState state, float dt);
    void triggerSystemsConsolidation(SleepState state);
    void triggerSynapticConsolidation(SleepState state);
    void triggerSchemaFormation(SleepState state);
    void triggerMemoryIntegration(SleepState state);
};

} // namespace nlm
