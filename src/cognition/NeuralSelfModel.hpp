// NeuralSelfModel.hpp
#pragma once

#include "../core/Types/Types.hpp"
#include "../brain/Brain.hpp"
#include <vector>
#include <memory>
#include <string>

namespace nlm {

// NeuralSelfModel: Integrates neural self-representation with planning and concept formation
// Extends the basic SelfModel to work within the integrated cognitive architecture
//
// Key mechanisms:
// - Provides body schema to planner for action prediction
// - Supplies concept formation with self-referential patterns
// - Receives neuromodulatory feedback for self-model updating
// - Coordinates between planning goals and self-capability assessment

class NeuralSelfModel {
public:
    NeuralSelfModel();
    ~NeuralSelfModel();

    // Initialize with brain reference for system integration
    void initialize(Brain* brain);

    // Update self-model with planning and concept formation feedback
    void updateWithPlanning(const std::vector<float>& planningContext,
                          const std::vector<ActionType>& plannedActions,
                          float planningConfidence);

    // Update self-model with concept formation insights
    void updateWithConcept(const DiscoveredConcept& concept,
                         const std::vector<float>& contextualInfo,
                         float noveltySignal);

    // Predict action consequences based on integrated self-model
    std::vector<float> predictActionConsequence(ActionType action,
                                              const std::vector<float>& currentState,
                                              const std::vector<float>& planningContext) const;

    // Get confidence in self-model predictions for action planning
    float getPlanningConfidence(ActionType action) const;

    // Assess self-capability for current planning context
    float getSelfCapabilityLevel(const std::vector<float>& planningContext) const;

    // Get body schema representation for action selection
    std::vector<float> getBodySchema() const { return bodySchema_; }

    // Get action planning confidence based on self-model
    float getActionPlanningConfidence(ActionType action) const;

    // Get preferred actions based on self-model integration
    ActionType getPreferredAction(const std::vector<float>& state,
                               const std::vector<float>& planningContext) const;

    // Update self-model with neuromodulatory signals
    void applyNeuromodulation(float noveltyLevel, float curiosityLevel,
                             float predictionError, float rewardPrediction);

    // Clear self-model state
    void clear();

    // Has self-model been initialized?
    bool isInitialized() const { return brain_ != nullptr; }

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;

    Brain* brain_;
    
    // Body schema derived from self-model and concept formation
    std::vector<float> bodySchema_;
    
    // Planning confidence cache for quick access
    std::vector<float> planningConfidenceCache_;
    
    // Neuromodulation weights for self-model updating
    float noveltyWeight_;
    float curiosityWeight_;
    float predictionErrorWeight_;
    float rewardWeight_;
};

} // namespace nlm