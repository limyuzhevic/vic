#pragma once

#include "../core/Types/Types.hpp"
#include "../brain/Brain.hpp"
#include <vector>
#include <memory>
#include <string>
#include <algorithm>
#include <cmath>

namespace nlm {

// SelfModel: Represents the agent's internal model of itself
// NOT consciousness - sensorimotor self-awareness through experience
//
// Key mechanisms:
// - Learns body schema (how actions affect sensory state)
// - Predicts consequences of own actions
// - Distinguishes self from external events
// - Represents own capabilities and limitations

class SelfModel {
public:
    SelfModel();
    ~SelfModel();
    
    // Initialize with brain reference
    void initialize(Brain* brain);
    
    // Record that taking an action caused a specific sensory change
    void recordSelfAction(ActionType action,
                         const std::vector<float>& beforeState,
                         const std::vector<float>& afterState);
    
    // Predict sensory consequence of an action
    // This is the "forward model" of the self
    std::vector<float> predictActionConsequence(ActionType action,
                                                const std::vector<float>& currentState);
    
    // Get confidence in self-model for a given action
    float getSelfModelConfidence(ActionType action) const;
    
    // Is this change likely caused by self (action) vs external?
    float computeSelfGeneratedLikelihood(const std::vector<float>& beforeState,
                                       const std::vector<float>& afterState,
                                       ActionType action) const;
    
    // Get the body schema (preferred actions in different states)
    // Returns map of state -> preferred action
    ActionType getPreferredAction(const std::vector<float>& state);
    
    // Update self-model based on prediction error
    void updateSelfModel(const std::vector<float>& predicted,
                        const std::vector<float>& actual,
                        ActionType action);
    
    // Get current capability level (0-1)
    float getCapabilityLevel() const { return capabilityLevel_; }
    
    // Get body awareness (how accurately can predict consequences)
    float getBodyAwareness() const;
    
    // Clear self-model
    void clear();
    
    // Has self-model formed?
    bool hasSelfModel() const { return !actionEffects_.empty(); }
    
    // Update self-model with new experience
    void update(const class NeuralAssociativeMemory& associative);
    
private:
    // Find best matching previous experience
    std::vector<float> findMatchingEffect(ActionType action,
                                          const std::vector<float>& beforeState) const;
    
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    Brain* brain_;
    float capabilityLevel_;
    
    // Action -> (beforeState -> afterState) mappings
    struct ActionEffect {
        std::vector<float> beforeState;
        std::vector<float> afterState;
        float confidence;
        size_t observationCount;
    };
    
    std::vector<std::vector<ActionEffect>> actionEffects_;  // Indexed by ActionType
};

// SocialLearning: Enables learning from observing other agents
// NOT language - simple action-effect learning from observation
//
// Key mechanisms:
// - Observes other agent's actions
// - Learns that other agents cause predictable changes
// - Can imitate observed actions
// - Develops simple communication signals

class SocialLearning {
public:
    SocialLearning();
    ~SocialLearning();
    
    // Initialize with brain reference
    void initialize(Brain* brain);
    
    // Record observation of another agent's action and effect
    void observeAgentAction(ActionType observedAction,
                           const std::vector<float>& observerState,
                           const std::vector<float>& resultingState);
    
    // Can I imitate this observed action?
    bool canImitate(ActionType observedAction) const;
    
    // Get the best action to imitate given current state
    ActionType getImitationAction(const std::vector<float>& currentState);
    
    // Learn simple communication signal from another agent
    // Signal is a neural pattern that predicts reward from other agent
    void learnCommunicationSignal(const std::vector<float>& signalPattern,
                                 float signalReward);
    
    // Detect if another agent is signaling
    bool detectSignal(const std::vector<float>& neuralPattern) const;
    
    // Get learned signal pattern
    const std::vector<float>* getSignalPattern() const { return signalPattern_.empty() ? nullptr : &signalPattern_; }
    
    // Get signal meaning (associated reward)
    float getSignalMeaning() const { return signalMeaning_; }
    
    // Update social knowledge based on interactions
    void updateSocialKnowledge(float interactionReward);
    
    // Clear social learning
    void clear();
    
    // Has learned from others?
    bool hasSocialKnowledge() const { return observationCount_ > 0; }
    
    // Get observation count
    size_t getObservationCount() const { return observationCount_; }
    
    // Get imitatable actions
    std::vector<ActionType> getImitableActions() const;
    
    // Learn from observed agent
    void learnFromObserved(const class NeuralAssociativeMemory& associative);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    Brain* brain_;
    size_t observationCount_;
    
    // Observed action effects (other agent's actions)
    struct ObservedEffect {
        std::vector<float> state;
        std::vector<float> resultingState;
        float reward;
    };
    
    std::vector<std::pair<ActionType, ObservedEffect>> observedActions_;
    
    // Communication signal
    std::vector<float> signalPattern_;
    float signalMeaning_;
};

// SpatialRepresentation: Learns spatial relationships from experience
// NOT a map with coordinates - encodes spatial relationships in neural patterns
//
// Key mechanisms:
// - Place neurons form through spatial exploration
// - Spatial relationships encoded in synaptic connectivity
// - Path integration through dead reckoning
// - Landmark associations

class SpatialRepresentation {
public:
    SpatialRepresentation();
    ~SpatialRepresentation();
    
    void initialize(Brain* brain);
    
    // Record current position experience
    void recordPosition(float x, float y, const std::vector<float>& sensoryCues);
    
    // Get predicted position based on path integration
    std::pair<float, float> getPredictedPosition() const;
    
    // Update position estimate based on movement
    void integrateMovement(float dx, float dy);
    
    // Get position neuron activation for a given position
    float getPositionActivation(float x, float y) const;
    
    // Find place neurons near a position
    std::vector<NeuronId> getPlaceNeuronsNear(float x, float y, float radius) const;
    
    // Get grid spacing (if grid cells have formed)
    float getGridSpacing() const { return gridSpacing_; }
    
    // Clear spatial representations
    void clear();
    
    // Have place cells formed?
    bool hasPlaceCells() const { return !placeCells_.empty(); }
    
    // Update with new spatial information
    void update(const class NeuralAssociativeMemory& associative);
    
private:
    struct PlaceCell {
        NeuronId neuron;
        float x;
        float y;
        float fieldRadius;
        float activation;
    };
    
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    Brain* brain_;
    std::vector<PlaceCell> placeCells_;
    float currentX_;
    float currentY_;
    float gridSpacing_;
    std::vector<float> lastSensoryCue_;
};

// TemporalRelation: Learns temporal relationships (before, after, sequence)
// NOT symbolic timestamps - encodes temporal order in neural activity

class TemporalRelation {
public:
    TemporalRelation();
    ~TemporalRelation();
    
    void initialize(Brain* brain);
    
    // Record that event A was followed by event B
    void recordSequence(const std::vector<float>& eventA,
                      const std::vector<float>& eventB,
                      float reward);
    
    // Given event A, predict what comes next
    std::vector<float> predictNext(const std::vector<float>& eventA) const;
    
    // Given event B, predict what came before
    std::vector<float> predictPrevious(const std::vector<float>& eventB) const;
    
    // Get confidence in prediction
    float getPredictionConfidence(const std::vector<float>& event) const;
    
    // Get sequence length
    size_t getSequenceLength() const { return sequences_.size(); }
    
    // Clear sequences
    void clear();
    
    // Update temporal relations
    void update(const class NeuralAssociativeMemory& associative);
    
private:
    struct SequenceLink {
        std::vector<float> fromEvent;
        std::vector<float> toEvent;
        float strength;
        size_t observationCount;
    };
    
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    Brain* brain_;
    std::vector<SequenceLink> sequences_;
};

} // namespace nlm
