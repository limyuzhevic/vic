// ============================================================================
// Neuromodulation Visualization Components
// ============================================================================

#pragma once

#include <string>
#include <memory>
#include <vector>
#include "../core/Types/Types.hpp"
#include "../neuromodulation/Neuromodulator.hpp"
#include "VisualizationBase.hpp"

namespace nlm {

/**
 * @class NeuromodulationVisualizer
 * @brief Neuromodulatory signal visualization
 * 
 * This visualizer provides comprehensive visualization of neuromodulatory signals,
 * showing their dynamics, interactions, and influence on neural activity.
 */
class NeuromodulationVisualizer : public VisualizationBase {
public:
    NeuromodulationVisualizer();
    ~NeuromodulationVisualizer() override = default;
    
    // Core visualization interface implementation
    void attachToBrain(class Brain* brain) override;
    void attachToMemory(class MemorySystem* memory) override;
    void attachToPlasticity(class PlasticitySystem* plasticity) override;
    
    // NeuromodulationVisualizer specific methods
    void setShowDopamine(bool show) { showDopamine_ = show; }
    void setShowSerotonin(bool show) { showSerotonin_ = show; }
    void setShowNorepinephrine(bool show) { showNorepinephrine_ = show; }
    void setShowAcetylcholine(bool show) { showAcetylcholine_ = show; }
    void setShowAllNeuromodulators(bool show) { showAllNeuromodulators_ = show; }
    void setShowSignalInteractions(bool show) { showSignalInteractions_ = show; }
    void setShowModulationEffects(bool show) { showModulationEffects_ = show; }
    
    // Data access
    double getAverageDopamineLevel() const { return averageDopamineLevel_; }
    double getAverageSerotoninLevel() const { return averageSerotoninLevel_; }
    double getAverageNorepinephrineLevel() const { return averageNorepinephrineLevel_; }
    double getAverageAcetylcholineLevel() const { return averageAcetylcholineLevel_; }
    uint32_t getTotalModulationEvents() const { return totalModulationEvents_; }
    
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
    bool showDopamine_ = true;
    bool showSerotonin_ = true;
    bool showNorepinephrine_ = true;
    bool showAcetylcholine_ = true;
    bool showAllNeuromodulators_ = false;
    bool showSignalInteractions_ = true;
    bool showModulationEffects_ = true;
    
    // Neuromodulation metrics
    double averageDopamineLevel_ = 0.0;
    double averageSerotoninLevel_ = 0.0;
    double averageNorepinephrineLevel_ = 0.0;
    double averageAcetylcholineLevel_ = 0.0;
    uint32_t totalModulationEvents_ = 0;
    uint32_t neuromodulationUpdateInterval_ = 10;
    uint32_t neuromodulationHistorySize_ = 100;
    std::vector<double> dopamineHistory_;
    std::vector<double> serotoninHistory_;
    std::vector<double> norepinephrineHistory_;
    std::vector<double> acetylcholineHistory_;
};

/**
 * @class DopamineVisualizer
 * @brief Reward and reinforcement signals visualization
 * 
 * This visualizer focuses specifically on dopamine signaling, showing
 * reward prediction errors, reinforcement learning, and the impact of
 * rewards on neural plasticity and behavior.
 */
class DopamineVisualizer : public VisualizationBase {
public:
    DopamineVisualizer();
    ~DopamineVisualizer() override = default;
    
    // Core visualization interface implementation
    void attachToBrain(class Brain* brain) override;
    void attachToMemory(class MemorySystem* memory) override;
    void attachToPlasticity(class PlasticitySystem* plasticity) override;
    
    // DopamineVisualizer specific methods
    void setShowRewardPrediction(bool show) { showRewardPrediction_ = show; }
    void setShowPhasicDopamine(bool show) { showPhasicDopamine_ = show; }
    void setShowTonicDopamine(bool show) { showTonicDopamine_ = show; }
    void setShowLearningSignals(bool show) { showLearningSignals_ = show; }
    void setShowMotivation(bool show) { showMotivation_ = show; }
    void setShowInhibition(bool show) { showInhibition_ = show; }
    
    // Data access
    double getPhasicDopamineStrength() const { return phasicDopamineStrength_; }
    double getTonicDopamineLevel() const { return tonicDopamineLevel_; }
    double getRewardPredictionError() const { return rewardPredictionError_; }
    uint32_t getRewardEvents() const { return rewardEvents_; }
    uint32_t getPunishmentEvents() const { return punishmentEvents_; }
    
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
    class Neuromodulator* neuromodulator_ = nullptr;
    
    // Visualization state
    bool showRewardPrediction_ = true;
    bool showPhasicDopamine_ = true;
    bool showTonicDopamine_ = true;
    bool showLearningSignals_ = true;
    bool showMotivation_ = true;
    bool showInhibition_ = true;
    
    // Dopamine metrics
    double phasicDopamineStrength_ = 0.0;
    double tonicDopamineLevel_ = 0.0;
    double rewardPredictionError_ = 0.0;
    uint32_t rewardEvents_ = 0;
    uint32_t punishmentEvents_ = 0;
    double rewardMagnitude_ = 0.0;
    double punishmentMagnitude_ = 0.0;
    uint32_t dopamineHistorySize_ = 100;
    std::vector<double> phasicDopamineHistory_;
    std::vector<double> tonicDopamineHistory_;
    std::vector<double> rewardHistory_;
};

/**
 * @class CuriosityVisualizer
 * @brief Exploration motivation visualization
 * 
 * This visualizer focuses on curiosity and exploration signals, showing
 * the drive for novel information, intrinsic motivation, and the balance
 * between exploitation and exploration.
 */
class CuriosityVisualizer : public VisualizationBase {
public:
    CuriosityVisualizer();
    ~CuriosityVisualizer() override = default;
    
    // Core visualization interface implementation
    void attachToBrain(class Brain* brain) override;
    void attachToMemory(class MemorySystem* memory) override;
    void attachToPlasticity(class PlasticitySystem* plasticity) override;
    
    // CuriosityVisualizer specific methods
    void setShowExplorationDrive(bool show) { showExplorationDrive_ = show; }
    void setShowIntrinsicMotivation(bool show) { showIntrinsicMotivation_ = show; }
    void setShowUncertainty(bool show) { showUncertainty_ = show; }
    void setShowNoveltySeeking(bool show) { showNoveltySeeking_ = show; }
    void setShowInformationGain(bool show) { showInformationGain_ = show; }
    void setShowExplorationExploitation(bool show) { showExplorationExploitation_ = show; }
    
    // Data access
    double getExplorationDrive() const { return explorationDrive_; }
    double getIntrinsicMotivation() const { return intrinsicMotivation_; }
    double getUncertainty() const { return uncertainty_; }
    double getNoveltySeeking() const { return noveltySeeking_; }
    double getInformationGain() const { return informationGain_; }
    uint32_t getExplorationEvents() const { return explorationEvents_; }
    uint32_t getExploitationEvents() const { return exploitationEvents_; }
    
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
    class Neuromodulator* neuromodulator_ = nullptr;
    
    // Visualization state
    bool showExplorationDrive_ = true;
    bool showIntrinsicMotivation_ = true;
    bool showUncertainty_ = true;
    bool showNoveltySeeking_ = true;
    bool showInformationGain_ = true;
    bool showExplorationExploitation_ = true;
    
    // Curiosity metrics
    double explorationDrive_ = 0.0;
    double intrinsicMotivation_ = 0.0;
    double uncertainty_ = 0.0;
    double noveltySeeking_ = 0.0;
    double informationGain_ = 0.0;
    uint32_t explorationEvents_ = 0;
    uint32_t exploitationEvents_ = 0;
    double explorationRate_ = 0.0;
    double exploitationRate_ = 0.0;
    uint32_t curiosityHistorySize_ = 100;
    std::vector<double> explorationHistory_;
    std::vector<double> uncertaintyHistory_;
    std::vector<double> noveltyHistory_;
};

/**
 * @class NoveltyVisualizer
 * @brief Novelty detection visualization
 * 
 * This visualizer focuses on novelty detection and processing, showing
 * how the system detects new information, patterns, and deviations from
 * expectations, and the impact of novelty on learning and attention.
 */
class NoveltyVisualizer : public VisualizationBase {
public:
    NoveltyVisualizer();
    ~NoveltyVisualizer() override = default;
    
    // Core visualization interface implementation
    void attachToBrain(class Brain* brain) override;
    void attachToMemory(class MemorySystem* memory) override;
    void attachToPlasticity(class PlasticitySystem* plasticity) override;
    
    // NoveltyVisualizer specific methods
    void setShowNoveltyScore(bool show) { showNoveltyScore_ = show; }
    void setShowPredictionError(bool show) { showPredictionError_ = show; }
    void setShowPatternDetection(bool show) { showPatternDetection_ = show; }
    void setShowMemorySurprise(bool show) { showMemorySurprise_ = show; }
    void setShowAttentionAllocation(bool show) { showAttentionAllocation_ = show; }
    void setShowAdaptationRate(bool show) { showAdaptationRate_ = show; }
    
    // Data access
    double getNoveltyScore() const { return noveltyScore_; }
    double getPredictionError() const { return predictionError_; }
    double getPatternStrength() const { return patternStrength_; }
    double getMemorySurprise() const { return memorySurprise_; }
    double getAttentionAllocation() const { return attentionAllocation_; }
    uint32_t getNoveltyEvents() const { return noveltyEvents_; }
    uint32_t getSurpriseEvents() const { return surpriseEvents_; }
    
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
    class Neuromodulator* neuromodulator_ = nullptr;
    
    // Visualization state
    bool showNoveltyScore_ = true;
    bool showPredictionError_ = true;
    bool showPatternDetection_ = true;
    bool showMemorySurprise_ = true;
    bool showAttentionAllocation_ = true;
    bool showAdaptationRate_ = true;
    
    // Novelty metrics
    double noveltyScore_ = 0.0;
    double predictionError_ = 0.0;
    double patternStrength_ = 0.0;
    double memorySurprise_ = 0.0;
    double attentionAllocation_ = 0.0;
    uint32_t noveltyEvents_ = 0;
    uint32_t surpriseEvents_ = 0;
    double noveltyThreshold_ = 0.5;
    double adaptationRate_ = 0.1;
    uint32_t noveltyHistorySize_ = 100;
    std::vector<double> noveltyScoreHistory_;
    std::vector<double> predictionErrorHistory_;
    std::vector<double> patternHistory_;
};

} // namespace nlm
