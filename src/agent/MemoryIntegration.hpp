// NLM/熙然 Phase 6 Comprehensive Memory Integration
// 
// This file implements the complete memory integration pipeline requested in Phase 6:
// - Working memory with persistent activity and competition
// - Episodic memory with experience encoding and replay
// - Associative memory with pattern associations
// - Integration with attention and concept formation
// - Connection to planning and action selection
// 
// Based on Phase 6 audit requirements to fix disconnected memory systems.

#include "BrainInitializer.hpp"
#include "Dopamine.hpp"
#include "Curiosity.hpp"
#include "Novelty.hpp"
#include "PredictionError.hpp"
#include "Reward.hpp"
#include "../agent/AgentBrain.hpp"
#include "../agent/SensoryPercept.hpp"
#include <algorithm>
#include <cmath>
#include <random>
#include <string>

namespace nlm {

// Memory integration configuration parameters
struct MemoryIntegrationConfig {
    // Working memory parameters
    struct {
        size_t capacity;           // Number of memory traces
        float decayRate;           // Trace decay rate
        float competitionStrength; // Winner-take-all competition
        float maintenanceCurrent;  // Current to maintain traces
        float threshold;           // Activation threshold for winners
    } workingMemory;
    
    // Episodic memory parameters
    struct {
        size_t maxEpisodes;        // Maximum number of episodes
        float consolidationThreshold; // Confidence threshold for consolidation
        float replayInterval;      // Steps between replay events
        float decayRate;           // Memory decay rate
        float integrationWeight;   // Weight of episodic memory in decisions
    } episodicMemory;
    
    // Associative memory parameters
    struct {
        float associationStrength; // Base association strength
        float patternOverlap;      // Pattern overlap threshold
        float reinforcementRate;  // Rate of strengthening associations
        float inhibitionRate;     // Rate of weakening associations
    } associativeMemory;
    
    // Integration parameters
    struct {
        float attentionBoost;      // Attention boost from working memory
        float conceptFormationWeight; // Weight of episodic memory for concept formation
        float planningInfluence;   // Influence of memory on planning
        float noveltyNovelty;      // Novelty detection sensitivity
    } integration;
};

// Main memory integration implementation
class MemoryIntegration {
public:
    MemoryIntegration();
    ~MemoryIntegration();
    
    // Initialize with brain reference
    bool initialize(class Brain* brain);
    
    // Configure based on configuration
    void configure(class Config* config);
    
    // Main integration pipeline
    void update(class Brain* brain, double timestep);
    
    // Process sensory input through memory pipeline
    void processSensoryInput(class Brain* brain, const SensoryPercept& percept);
    
    // Update all memory systems
    void updateWorkingMemory(class Brain* brain, double timestep);
    void updateEpisodicMemory(class Brain* brain, double timestep);
    void updateAssociativeMemory(class Brain* brain, double timestep);
    
    // Integrate with cognition systems
    void updateAttention(class Brain* brain);
    void updateConceptFormation(class Brain* brain);
    void updatePlanning(class Brain* brain);
    
    // Store experiences in episodic memory
    void captureExperience(class Brain* brain, 
                           const SensoryPercept& percept,
                           const MotorCommand& action,
                           float reward,
                           float predictionError);
    
    // Replay important memories
    void triggerReplay(class Brain* brain);
    
    // Reset all memory systems
    void reset(class Brain* brain);
    
    // Get memory statistics
    void logMemoryStatus(class Brain* brain) const;
    
    // Configuration
    void setConfig(const MemoryIntegrationConfig& config) { config_ = config; }
    const MemoryIntegrationConfig& getConfig() const { return config_; }

private:
    struct Impl;
    Impl* pImpl;
};

// Helper functions for memory integration
namespace MemoryIntegrationHelpers {
    
    // Encode sensory percept into working memory pattern
    std::vector<float> encodeSensoryInput(const SensoryPercept& percept);
    
    // Encode motor command into action representation
    std::vector<float> encodeMotorCommand(const MotorCommand& action);
    
    // Compute memory similarity
    float computeSimilarity(const std::vector<float>& pattern1, 
                           const std::vector<float>& pattern2);
    
    // Generate episodic memory key
    size_t generateEpisodicKey(const SensoryPercept& percept,
                               const MotorCommand& action,
                               float reward);
    
    // Compute memory consolidation strength
    float computeConsolidationStrength(const EpisodicMemoryItem& episode,
                                        float currentTime);
}

} // namespace nlm
