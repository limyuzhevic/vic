#pragma once

#include "../core/Types/Types.hpp"
#include "../brain/Brain.hpp"
#include <vector>
#include <memory>
#include <string>
#include <unordered_map>
#include <algorithm>

namespace nlm {

// Forward declarations
class Neuromodulator;
class NeuromodulationEngine;

// Exploration drive parameters
struct ExplorationParams {
    float intrinsicMotivationWeight;    // Weight for intrinsic motivation
    float extrinsicMotivationWeight;   // Weight for extrinsic motivation
    float noveltyThreshold;            // Threshold for novelty detection
    float predictionErrorWeight;       // Weight for prediction error in exploration
    float explorationBonusDecay;       // Decay rate for exploration bonus
    float curiosityAmplification;      // How curiosity amplifies exploration
    float habituationRate;             // Rate of habituation to familiar stimuli
    float uncertaintyWeight;           // Weight for uncertainty in exploration
    
    ExplorationParams()
        : intrinsicMotivationWeight(0.6f), extrinsicMotivationWeight(0.4f),
          noveltyThreshold(0.3f), predictionErrorWeight(0.5f),
          explorationBonusDecay(0.05f), curiosityAmplification(1.2f),
          habituationRate(0.01f), uncertaintyWeight(0.7f) {}
};

// Curiosity system state
struct CuriosityState {
    float intrinsicMotivation;          // Intrinsic drive for exploration
    float extrinsicMotivation;         // Extrinsic reward-driven motivation
    float explorationDrive;            // Overall exploration tendency
    float uncertaintyEstimate;         // Estimated uncertainty about environment
    float informationGain;             // Expected information gain from actions
    float noveltySensitivity;           // Sensitivity to novel stimuli
    float predictionErrorSensitivity;   // Sensitivity to prediction errors
    float habituationLevel;             // Level of habituation to stimuli
    float curiosityBuffer;              // Buffer for storing curiosity signals
    float explorationBonus;             // Current exploration bonus
    float lastNoveltyTime;              // Time since last novel stimulus
    float learningRate;                 // Current learning rate modulation
    
    CuriosityState()
        : intrinsicMotivation(0.5f), extrinsicMotivation(0.5f), explorationDrive(0.5f),
          uncertaintyEstimate(0.0f), informationGain(0.0f), noveltySensitivity(1.0f),
          predictionErrorSensitivity(1.0f), habituationLevel(0.0f), curiosityBuffer(0.0f),
          explorationBonus(0.0f), lastNoveltyTime(0.0f), learningRate(0.01f) {}
};

// Intrinsic motivation components
struct IntrinsicMotivation {
    float empiricalCuriosity;           // Curiosity based on prediction errors
    float intrinsicPleasure;            // Natural pleasure from exploration
    float goalIrrelevance;              // Drive to explore irrelevant goals
    float efficiencyMotivation;         // Drive to learn efficient behaviors
    float flowStateMotivation;          // Drive for challenge-skill balance
    
    IntrinsicMotivation()
        : empiricalCuriosity(0.0f), intrinsicPleasure(0.0f), goalIrrelevance(0.0f),
          efficiencyMotivation(0.0f), flowStateMotivation(0.0f) {}
};

// Curiosity-driven action selection state
struct ActionSelectionState {
    std::vector<float> actionValues;    // Q-values for different actions
    std::vector<float> explorationValues; // Exploration bonuses for actions
    std::vector<float> uncertaintyValues; // Uncertainty estimates for actions
    std::vector<float> habituationValues;  // Habituation for actions
    
    ActionSelectionState() {
        // Initialize with reasonable defaults
        actionValues.resize(10, 0.0f);
        explorationValues.resize(10, 0.0f);
        uncertaintyValues.resize(10, 0.0f);
        habituationValues.resize(10, 0.0f);
    }
};

// Advanced curiosity system: Intrinsic motivation and exploration drive
class CuriositySystem {
public:
    CuriositySystem();
    ~CuriositySystem();
    
    // Disable copying, enable moving
    CuriositySystem(const CuriositySystem&) = delete;
    CuriositySystem& operator=(const CuriositySystem&) = delete;
    CuriositySystem(CuriositySystem&&) noexcept;
    CuriositySystem& operator=(CuriositySystem&&) noexcept;
    
    // Initialization
    bool initialize(std::shared_ptr<Brain> brain, std::shared_ptr<NeuromodulationEngine> engine);
    
    // Main update step
    void update(TimestepDuration dt, SimulationStep step);
    
    // Curiosity computation
    void updateCuriosity(float novelty, float predictionError, float reward, float timeStep);
    void computeIntrinsicMotivation(const class EnvironmentState& environmentState);
    void computeExplorationDrive(const std::vector<float>& actionValues);
    void updateActionSelection(std::vector<float>& actionValues);
    
    // Intrinsic motivation components
    void computeEmpiricalCuriosity(float predictionError);
    void computeIntrinsicPleasure(float novelty);
    void computeGoalIrrelevance();
    void computeEfficiencyMotivation(const std::vector<float>& actionValues);
    void computeFlowStateMotivation(float challenge, float skill);
    
    // State management
    void updateState(TimestepDuration dt);
    void reset();
    
    // Accessors
    float getIntrinsicMotivation() const;
    float getExtrinsicMotivation() const;
    float getExplorationDrive() const;
    float getCuriosityLevel() const;
    float getExplorationBonus() const;
    float getUncertaintyEstimate() const;
    float getInformationGain() const;
    
    // Configuration
    void setExplorationParams(const ExplorationParams& params);
    void enableIntrinsicMotivation(bool enable) { enableIntrinsicMotivation_ = enable; }
    void enableHabituation(bool enable) { enableHabituation_ = enable; }
    void enableFlowState(bool enable) { enableFlowState_ = enable; }
    
    // Integration with other systems
    void integrateWithDopamine(float dopamineLevel);
    void integrateWithNovelty(float noveltyLevel);
    void integrateWithPredictionError(float predictionError);
    
    // Action selection with curiosity
    float computeActionValue(int actionIndex, const std::vector<float>& baseValues);
    void applyExplorationBonus(std::vector<float>& actionValues);
    void updateHabitationForAction(int actionIndex, bool actionTaken);
    
    // History and statistics
    const std::vector<float>& getNoveltyHistory() const;
    const std::vector<float>& getPredictionErrorHistory() const;
    const std::vector<float>& getActionValueHistory(int actionIndex) const;
    void clearHistory();
    
    // Safety and validation
    bool validateSafetyConstraints() const;
    void enforceBounds();
    
    // Export and import
    void exportState(std::ostream& stream) const;
    void importState(std::istream& stream);
    
    // Testing interface
    float getEmpiricalCuriosity() const { return pImpl->intrinsicMotivation.empiricalCuriosity; }
    float getIntrinsicPleasure() const { return pImpl->intrinsicMotivation.intrinsicPleasure; }
    float getGoalIrrelevance() const { return pImpl->intrinsicMotivation.goalIrrelevance; }
    float getEfficiencyMotivation() const { return pImpl->intrinsicMotivation.efficiencyMotivation; }
    float getFlowStateMotivation() const { return pImpl->intrinsicMotivation.flowStateMotivation; }
    
    // Performance metrics
    float getExplorationEfficiency() const;
    float getIntrinsicMotivationEfficiency() const;
    float getNoveltyUtilization() const;
    
private:
    struct Impl;
    Impl* pImpl;
    
    std::shared_ptr<Brain> brain_;
    std::shared_ptr<NeuromodulationEngine> engine_;
    
    bool enableIntrinsicMotivation_;
    bool enableHabituation_;
    bool enableFlowState_;
};

} // namespace nlm