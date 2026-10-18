#pragma once

#include "../core/Types/Types.hpp"
#include "../brain/Brain.hpp"
#include "../neuromodulation/Acetylcholine.hpp"
#include "../neuromodulation/Norepinephrine.hpp"
#include "../neuromodulation/Dopamine.hpp"
#include "../neuromodulation/Serotonin.hpp"
#include <memory>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <queue>
#include <algorithm>
#include <cmath>
#include <limits>

namespace nlm {

// Attention states for tracking focus
enum class AttentionState {
    IDLE,
    FOCUSED,
    SCANNING,
    EXPLOITING,
    RETURNING
};

// Prediction error based attention focus
struct PredictionErrorFocus {
    float errorMagnitude;
    float errorSign;  // +1 for positive, -1 for negative
    float expectedValue;
    float surprise;  // Unexpectedness measure
    float urgency;   // How urgent the prediction error is
};

// Neuromodulator signals for attention modulation
struct NeuromodulatorSignals {
    float acetylcholine;   // For sensory gating and feature binding
    float norepinephrine; // For arousal and sustained attention
    float dopamine;       // For reward-based attention modulation
    float serotonin;     // For behavioral state modulation
    float curiosity;     // For exploration motivation
};

// Goal-relevant processing weights
struct GoalWeights {
    float importance;      // How important the goal is
    float salience;       // How noticeable the goal is
    float urgency;        // How urgent the goal is
    float expectancies;   // Expected outcomes from goal pursuit
};

// Attentional scope control
struct AttentionalScope {
    float breadth;        // Wide vs narrow focus
    float depth;         // Detail vs global processing
    float duration;      // Sustained vs transient attention
    float flexibility;   // Adaptability of attentional focus
};

// Neural population for attention tracking
struct AttentionalPopulation {
    uint64_t neuronId;
    float activationHistory[10];  // Last 10 activation values
    float salience;              // Current bottom-up salience
    float bias;                  // Current top-down bias
    float expectedValue;         // Expected prediction outcome
    float uncertainty;           // Prediction uncertainty
    float rewardPrediction;      // Expected reward value
    AttentionState currentState; // Current attention state
    float dwellTime;             // How long attention is maintained
    float switchingCost;         // Cost of shifting attention
};

// Temporal attention dynamics
struct AttentionalDynamics {
    float focusPersistence;      // How long focus is maintained
    float refreshRate;            // How often attention refreshes
    float captureThreshold;       // Threshold for attentional capture
    float disengagementThreshold; // Threshold for attentional disengagement
    float switchingCost;          // Cost of switching attention
    float costOfError;            // Cost of missing important events
};

// AttentionalSelection: Comprehensive neurobiological attention system
// Implements attention as an integrated system with:
// - Sensory gating through acetylcholine
// - Working memory focus through population dynamics
// - Predictive attention based on prediction errors
// - Goal-directed attention through planning signals
// - Attentional scope control for processing breadth
// - Neuromodulator integration for state-dependent modulation
class AttentionalSelection {
public:
    AttentionalSelection();
    ~AttentionalSelection();

    // Initialize with brain reference and neuromodulator systems
    void initialize(Brain* brain,
                   Acetylcholine* ach,
                   Norepinephrine* ne,
                   Dopamine* da,
                   Serotonin* sero,
                   Curiosity* curiosity);

    // Main attentional processing step
    void update(SimulationStep currentStep, Timestamp currentTime);

    // Process sensory input through attentional gating
    void processSensoryInput(const std::vector<float>& sensoryPattern,
                           std::vector<float>& gatedOutput);

    // Select relevant working memory traces
    std::vector<uint64_t> selectWorkingMemoryTraces(
        const std::vector<uint64_t>& memoryTraces,
        float predictionError,
        const std::vector<float>& goalVector);

    // Apply predictive attention to unexpected events
    void applyPredictiveAttention(float predictionError,
                                 float predictionConfidence,
                                 const std::vector<float>& sensoryInput);

    // Implement goal-directed attention
    void applyGoalDirectedAttention(const std::vector<float>& goalVector,
                                   const std::vector<uint64_t>& availableActions,
                                   std::vector<float>& actionWeights);

    // Control attentional scope
    void adjustAttentionalScope(AttentionState newScope, float duration);

    // Update attention focus based on cognitive load and neuromodulators
    void updateFocus(Brain* brain, RandomGenerator& rng);

    // Get attentional processing metrics
    float getCognitiveLoad() const { return cognitiveLoad_; }
    float getAttentionEfficiency() const { return attentionEfficiency_; }
    size_t getFocusedNeuronCount() const { return focusedNeurons_.size(); }

    // Set attentional processing parameters
    void setCognitiveLoad(float load) { cognitiveLoad_ = load; }
    void setAttentionEfficiency(float efficiency) { attentionEfficiency_ = efficiency; }
    void enableTopDownModulation(bool enable) { topDownModulationEnabled_ = enable; }

private:
    // Cognitive processing metrics
    float cognitiveLoad_;
    float attentionEfficiency_;
    std::unordered_set<uint64_t> focusedNeurons_;
    bool topDownModulationEnabled_;

    // Helper methods

private:
    // Neural computation components
    struct Impl;
    std::unique_ptr<Impl> pImpl;

    // Brain reference
    Brain* brain_;

    // Neuromodulator system references
    Acetylcholine* acetylcholine_;
    Norepinephrine* norepinephrine_;
    Dopamine* dopamine_;
    Serotonin* serotonin_;
    Curiosity* curiosity_;

    // Attention state management
    AttentionState currentAttentionState_;
    AttentionState previousAttentionState_;
    SimulationStep attentionSwitchStep_;

    // Goal-directed attention
    float goalAttentionStrength_;
    std::vector<float> currentGoalVector_;
    std::unordered_map<uint64_t, GoalWeights> goalWeights_;

    // Attentional scope control
    float attentionalBreadth_;      // Wide (1.0) vs narrow (0.0) focus
    float attentionalDepth_;        // Detailed (1.0) vs global (0.0) processing
    float attentionalFlexibility_;   // Adaptable (1.0) vs rigid (0.0) focus
    AttentionState attentionalScope_;

    // Predictive attention mechanisms
    std::queue<PredictionErrorFocus> predictionErrorQueue_;
    std::unordered_map<uint64_t, float> predictionErrorHistory_;

    // Working memory competition
    struct MemoryTrace {
        uint64_t neuronId;
        float activation;
        float predictionError;
        float goalAlignment;
        float novelty;
        float rewardPrediction;
        float uncertainty;
        float attentionalWeight;
    };

    // Attentional dynamics
    AttentionalDynamics dynamics_;

    // Temporal attention tracking
    std::unordered_map<uint64_t, AttentionalPopulation> attentionalPopulations_;
    std::unordered_map<uint64_t, float> attentionalGains_;
    std::unordered_map<uint64_t, float> attentionalInhibitions_;

    // Neuromodulator signal integration
    NeuromodulatorSignals lastNeuromodulatorSignals_;

    // Helper methods
    void updateAttentionalGains();
    void processBottomUpSalience(const std::vector<float>& sensoryInput);
    void processTopDownBias();
    void integrateNeuromodulatorSignals();
    void updatePredictionErrorBasedAttention();
    void manageWorkingMemoryCompetition();
    void controlAttentionalScope();
    void updateAttentionState();
    void enforceAttentionalConstraints();
    void maintainWorkingMemoryTraces();
};

} // namespace nlm
