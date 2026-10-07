#pragma once

#include "../core/Types/Types.hpp"
#include "../motor/Action.hpp"
#include <memory>

namespace nlm {

// Forward declaration for Brain (needed for CognitionManager initialization)
class Brain;

// Forward declarations for cognition system classes
class PredictionSystem;
class AttentionalSelection;
class ConceptFormation;
class NeuralWorkingMemory;

// Cognition statistics for tracking system performance
struct CognitionStats {
    size_t predictionUpdates = 0;
    size_t attentionUpdates = 0;
    size_t conceptFormations = 0;
    size_t attentionApplications = 0;
    size_t attentionToWorkingMemoryWinners = 0;
    float averagePredictionAccuracy = 0.0f;
    float attentionSelectivity = 0.0f;
    float conceptStability = 0.0f;
    
    CognitionStats() : averagePredictionAccuracy(0.0f), 
                       attentionSelectivity(0.0f), 
                       conceptStability(0.0f) {}
    
    void reset() {
        predictionUpdates = 0;
        attentionUpdates = 0;
        conceptFormations = 0;
        attentionApplications = 0;
        attentionToWorkingMemoryWinners = 0;
        averagePredictionAccuracy = 0.0f;
        attentionSelectivity = 0.0f;
        conceptStability = 0.0f;
    }
};

// CognitionManager: Coordinates and orchestrates cognition systems
// 
// Responsible for:
// 1. Updating prediction system
// 2. Updating attention system  
// 3. Updating concept formation
// 4. Applying attention to working memory winners
// 5. Managing cognition statistics
//
// The manager integrates various cognition subsystems to enable
// adaptive, predictive, and goal-directed behavior through neural dynamics.

class CognitionManager {
public:
    CognitionManager();
    ~CognitionManager();
    
    // Initialize with brain reference
    void initialize(Brain* brain);
    
    // Main update method - orchestrates all cognition updates
    // Called from Brain step() method
    void update(SimulationStep currentStep, Timestamp currentTime);
    
    // Update prediction system
    void updatePredictionSystem();
    
    // Update attention system
    void updateAttentionSystem();
    
    // Update concept formation
    void updateConceptFormation();
    
    // Apply attention to working memory winners
    void applyAttentionToWorkingMemoryWinners();
    
    // Get current cognition statistics
    const CognitionStats& getStats() const { return stats_; }
    
    // Reset statistics
    void resetStats() { stats_.reset(); }
    
    // Get prediction system reference
    PredictionSystem* getPredictionSystem() const { return predictionSystem_.get(); }
    
    // Get attention system reference
    AttentionalSelection* getAttention() const { return attention_.get(); }
    
    // Get concept formation reference
    ConceptFormation* getConceptFormation() const { return conceptFormation_.get(); }
    
    // Get working memory reference
    NeuralWorkingMemory* getWorkingMemory() const { return workingMemory_.get(); }
    
    // Clear all cognition state
    void clear();

private:
    // Update prediction system with current state
    void updatePredictionWithState(const std::vector<float>& currentState,
                                  const std::vector<float>& sensoryFeatures,
                                  float reward);
    
    // Update attention with working memory competition
    void updateAttentionWithWorkingMemory(const std::vector<NeuronId>& competitors);
    
    // Apply learned concepts to guide attention
    void applyConceptGuidedAttention();
    
    // Update statistics based on recent performance
    void updateStatistics();
    
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    Brain* brain_;
    CognitionStats stats_;
    
    // Cognition subsystem pointers
    std::unique_ptr<PredictionSystem> predictionSystem_;
    std::unique_ptr<AttentionalSelection> attention_;
    std::unique_ptr<ConceptFormation> conceptFormation_;
    std::unique_ptr<NeuralWorkingMemory> workingMemory_;
    
    // Temporal tracking
    SimulationStep lastUpdateStep_;
    Timestamp lastUpdateTime_;
};

} // namespace nlm