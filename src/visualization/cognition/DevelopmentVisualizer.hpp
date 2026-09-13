// ============================================================================
// Development and Cognition Visualization Components
// ============================================================================

#pragma once

#include <string>
#include <memory>
#include <vector>
#include <map>
#include "../core/Types/Types.hpp"
#include "VisualizationBase.hpp"

namespace nlm {

/**
 * @class DevelopmentVisualizer
 * @brief Developmental trajectory visualization
 * 
 * This visualizer shows the developmental stage progression, critical periods,
 * maturation trajectories, and the influence of early experiences on later development.
 */
class DevelopmentVisualizer : public VisualizationBase {
public:
    DevelopmentVisualizer();
    ~DevelopmentVisualizer() override = default;
    
    // Core visualization interface implementation
    void attachToBrain(class Brain* brain) override;
    void attachToMemory(class MemorySystem* memory) override;
    void attachToPlasticity(class PlasticitySystem* plasticity) override;
    
    // DevelopmentVisualizer specific methods
    void setShowDevelopmentalStages(bool show) { showDevelopmentalStages_ = show; }
    void setShowCriticalPeriods(bool show) { showCriticalPeriods_ = show; }
    void setShowMaturationTrajectories(bool show) { showMaturationTrajectories_ = show; }
    void setShowExperienceEffects(bool show) { showExperienceEffects_ = show; }
    void setShowGrowthDynamics(bool show) { showGrowthDynamics_ = show; }
    void setShowPhaseTransitions(bool show) { showPhaseTransitions_ = show; }
    
    // Data access
    DevelopmentalStage getCurrentDevelopmentalStage() const { return currentDevelopmentalStage_; }
    double getDevelopmentProgress() const { return developmentProgress_; }
    double getCriticalPeriodStrength() const { return criticalPeriodStrength_; }
    uint64_t getMaturationEvents() const { return maturationEvents_; }
    uint32_t getPhaseTransitions() const { return phaseTransitions_; }
    
protected:
    bool initializeImpl(const VisualizationConfig& config) override;
    void updateImpl() override;
    void renderImpl() override;
    void closeImpl() override;
    void resetImpl() override;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Attached systems
    class Brain* brain_ = nullptr;
    
    // Visualization state
    bool showDevelopmentalStages_ = true;
    bool showCriticalPeriods_ = true;
    bool showMaturationTrajectories_ = true;
    bool showExperienceEffects_ = true;
    bool showGrowthDynamics_ = true;
    bool showPhaseTransitions_ = true;
    
    // Development metrics
    DevelopmentalStage currentDevelopmentalStage_ = DevelopmentalStage::Initial;
    double developmentProgress_ = 0.0;
    double criticalPeriodStrength_ = 0.0;
    uint64_t maturationEvents_ = 0;
    uint32_t phaseTransitions_ = 0;
    double criticalPeriodWidth_ = 1000.0;
    uint32_t maturationHistorySize_ = 100;
    std::vector<double> developmentProgressHistory_;
    std::vector<DevelopmentalStage> stageHistory_;
};

/**
 * @class CognitionVisualizer
 * @brief Cognitive process visualization
 * 
 * This visualizer shows cognitive processes including attention, working memory,
 * planning, decision making, concept formation, and problem solving.
 */
class CognitionVisualizer : public VisualizationBase {
public:
    CognitionVisualizer();
    ~CognitionVisualizer() override = default;
    
    // Core visualization interface implementation
    void attachToBrain(class Brain* brain) override;
    void attachToMemory(class MemorySystem* memory) override;
    void attachToPlasticity(class PlasticitySystem* plasticity) override;
    
    // CognitionVisualizer specific methods
    void setShowAttention(bool show) { showAttention_ = show; }
    void setShowWorkingMemory(bool show) { showWorkingMemory_ = show; }
    void setShowPlanning(bool show) { showPlanning_ = show; }
    void setShowDecisionMaking(bool show) { showDecisionMaking_ = show; }
    void setShowConceptFormation(bool show) { showConceptFormation_ = show; }
    void setShowProblemSolving(bool show) { showProblemSolving_ = show; }
    void setShowCognitiveLoad(bool show) { showCognitiveLoad_ = show; }
    
    // Data access
    double getAttentionLevel() const { return attentionLevel_; }
    double getWorkingMemoryLoad() const { return workingMemoryLoad_; }
    double getPlanningFocus() const { return planningFocus_; }
    double getDecisionConfidence() const { return decisionConfidence_; }
    uint64_t getConceptActivations() const { return conceptActivations_; }
    uint32_t getProblemSolveEvents() const { return problemSolveEvents_; }
    
protected:
    bool initializeImpl(const VisualizationConfig& config) override;
    void updateImpl() override;
    void renderImpl() override;
    void closeImpl() override;
    void resetImpl() override;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Attached systems
    class Brain* brain_ = nullptr;
    
    // Visualization state
    bool showAttention_ = true;
    bool showWorkingMemory_ = true;
    bool showPlanning_ = true;
    bool showDecisionMaking_ = true;
    bool showConceptFormation_ = true;
    bool showProblemSolving_ = true;
    bool showCognitiveLoad_ = true;
    
    // Cognitive metrics
    double attentionLevel_ = 0.0;
    double workingMemoryLoad_ = 0.0;
    double planningFocus_ = 0.0;
    double decisionConfidence_ = 0.0;
    uint64_t conceptActivations_ = 0;
    uint32_t problemSolveEvents_ = 0;
    uint32_t attentionHistorySize_ = 100;
    uint32_t workingMemoryHistorySize_ = 100;
    std::vector<double> attentionHistory_;
    std::vector<double> workingMemoryHistory_;
    std::vector<double> planningHistory_;
};

/**
 * @class PredictionVisualizer
 * @brief Prediction system visualization
 * 
 * This visualizer shows prediction system activity, prediction errors,
 * expected value calculations, and the role of prediction in learning
 * and behavior.
 */
class PredictionVisualizer : public VisualizationBase {
public:
    PredictionVisualizer();
    ~PredictionVisualizer() override = default;
    
    // Core visualization interface implementation
    void attachToBrain(class Brain* brain) override;
    void attachToMemory(class MemorySystem* memory) override;
    void attachToPlasticity(class PlasticitySystem* plasticity) override;
    
    // PredictionVisualizer specific methods
    void setShowPredictionAccuracy(bool show) { showPredictionAccuracy_ = show; }
    void setShowPredictionErrors(bool show) { showPredictionErrors_ = show; }
    void setShowExpectedValues(bool show) { showExpectedValues_ = show; }
    void setShowUncertainty(bool show) { showUncertainty_ = show; }
    void setShowLearningCurves(bool show) { showLearningCurves_ = show; }
    void setShowTemporalPredictions(bool show) { showTemporalPredictions_ = show; }
    
    // Data access
    double getPredictionAccuracy() const { return predictionAccuracy_; }
    double getAveragePredictionError() const { return averagePredictionError_; }
    double getExpectedValue() const { return expectedValue_; }
    double getPredictionUncertainty() const { return predictionUncertainty_; }
    uint64_t getSuccessfulPredictions() const { return successfulPredictions_; }
    uint64_t getFailedPredictions() const { return failedPredictions_; }
    
protected:
    bool initializeImpl(const VisualizationConfig& config) override;
    void updateImpl() override;
    void renderImpl() override;
    void closeImpl() override;
    void resetImpl() override;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Attached systems
    class Brain* brain_ = nullptr;
    
    // Visualization state
    bool showPredictionAccuracy_ = true;
    bool showPredictionErrors_ = true;
    bool showExpectedValues_ = true;
    bool showUncertainty_ = true;
    bool showLearningCurves_ = true;
    bool showTemporalPredictions_ = true;
    
    // Prediction metrics
    double predictionAccuracy_ = 0.0;
    double averagePredictionError_ = 0.0;
    double expectedValue_ = 0.0;
    double predictionUncertainty_ = 0.0;
    uint64_t successfulPredictions_ = 0;
    uint64_t failedPredictions_ = 0;
    uint32_t predictionHistorySize_ = 100;
    std::vector<double> accuracyHistory_;
    std::vector<double> errorHistory_;
    std::vector<double> valueHistory_;
};

/**
 * @class PlanningVisualizer
 * @brief Action planning visualization
 * 
 * This visualizer shows planning processes, goal selection, action sequences,
 * trajectory planning, and the optimization of actions toward goals.
 */
class PlanningVisualizer : public VisualizationBase {
public:
    PlanningVisualizer();
    ~PlanningVisualizer() override = default;
    
    // Core visualization interface implementation
    void attachToBrain(class Brain* brain) override;
    void attachToMemory(class MemorySystem* memory) override;
    void attachToPlasticity(class PlasticitySystem* plasticity) override;
    
    // PlanningVisualizer specific methods
    void setShowGoals(bool show) { showGoals_ = show; }
    void setShowActionSequences(bool show) { showActionSequences_ = show; }
    void setShowTrajectoryPlanning(bool show) { showTrajectoryPlanning_ = show; }
    void setShowOptimization(bool show) { showOptimization_ = show; }
    void setShowResourceAllocation(bool show) { showResourceAllocation_ = show; }
    void setShowTimeHorizon(bool show) { showTimeHorizon_ = show; }
    
    // Data access
    uint64_t getActiveGoals() const { return activeGoals_; }
    double getPlanningEfficiency() const { return planningEfficiency_; }
    uint32_t getActionSequenceLength() const { return actionSequenceLength_; }
    double getOptimizationProgress() const { return optimizationProgress_; }
    uint64_t getResourceAllocation() const { return resourceAllocation_; }
    uint32_t getPlanningEvents() const { return planningEvents_; }
    
protected:
    bool initializeImpl(const VisualizationConfig& config) override;
    void updateImpl() override;
    void renderImpl() override;
    void closeImpl() override;
    void resetImpl() override;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Attached systems
    class Brain* brain_ = nullptr;
    
    // Visualization state
    bool showGoals_ = true;
    bool showActionSequences_ = true;
    bool showTrajectoryPlanning_ = true;
    bool showOptimization_ = true;
    bool showResourceAllocation_ = true;
    bool showTimeHorizon_ = true;
    
    // Planning metrics
    uint64_t activeGoals_ = 0;
    double planningEfficiency_ = 0.0;
    uint32_t actionSequenceLength_ = 0;
    double optimizationProgress_ = 0.0;
    uint64_t resourceAllocation_ = 0;
    uint32_t planningEvents_ = 0;
    uint32_t goalHistorySize_ = 100;
    uint32_t sequenceHistorySize_ = 50;
    std::vector<uint64_t> goalHistory_;
    std::vector<uint32_t> sequenceHistory_;
};

} // namespace nlm
