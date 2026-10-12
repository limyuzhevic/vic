#pragma once

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <mutex>
#include <chrono>
#include "../brain/Brain.hpp"
#include "../neuromodulation/Dopamine.hpp"
#include "../neuromodulation/Curiosity.hpp"
#include "../neuromodulation/Neuromodulator.hpp"

namespace nlm {

// Neuromodulation signal visualization
struct NeuromodulationSignal {
    Timestamp timestamp;
    NeuromodulatorType type;
    float level;
    float target;
    float error;
    float learningRate;
    float confidence;
    std::string sourceRegion;
    std::string targetRegion;
    bool isActive;
    bool isBurst;
    float burstIntensity;
    std::map<NeuronId, float> neuronInfluences;
};

// Dopamine signal visualization
struct DopamineSignal {
    Timestamp timestamp;
    float concentration;
    float baseline;
    float predictionError;
    float rewardPrediction;
    float learningSignal;
    float noveltyIndex;
    float attentionalFocus;
    std::string region;
    bool isPhasic;
    float phasicAmplitude;
};

// Curiosity signal visualization
struct CuriositySignal {
    Timestamp timestamp;
    float explorationLevel;
    float intrinsicMotivation;
    float uncertaintyReduction;
    float informationGain;
    float noveltySensitivity;
    float predictionErrorSize;
    std::string region;
    bool isSpontaneous;
    float spontaneityIndex;
};

// Prediction error signal visualization
struct PredictionErrorSignal {
    Timestamp timestamp;
    float errorMagnitude;
    float signedError;
    float predictionConfidence;
    float updateStrength;
    std::string expectedOutcome;
    std::string actualOutcome;
    std::string sourceRegion;
    std::string targetRegion;
    float gradient;
    bool isPositive;
    bool isNegative;
};

// Attentional focus visualization
struct AttentionalFocus {
    Timestamp timestamp;
    std::vector<RegionId> focusedRegions;
    std::vector<NeuronId> focusedNeurons;
    float globalAttentionLevel;
    float regionalAttentionStrength; // Map region to attention level
    float noveltyGating;
    float predictionGating;
    std::string primaryTarget;
    std::string secondaryTarget;
    bool isTopDown; // Top-down vs bottom-up attention
};

// Neuromodulation statistics
struct NeuromodulationStats {
    Timestamp timestamp;
    float totalNeuromodulatorLevel;
    float averageDopamineLevel;
    float averageCuriosityLevel;
    float averagePredictionError;
    size_t activeNeuromodulators;
    size_t totalNeuromodulatorEvents;
    float averageLearningRate;
    float averageConfidence;
    std::map<NeuromodulatorType, float> typeDistribution;
    std::map<std::string, float> regionalInfluence;
};

class NeuromodulationVisualizer {
public:
    NeuromodulationVisualizer();
    ~NeuromodulationVisualizer();
    
    // Initialize visualizer
    bool initialize();
    
    // Update neuromodulation data
    void update(double deltaTime);
    
    // Record neuromodulator signals
    void recordNeuromodulatorSignal(const NeuromodulationSignal& signal);
    void recordNeuromodulatorSignals(const std::vector<NeuromodulationSignal>& signals);
    
    // Record dopamine signals
    void recordDopamineSignal(const DopamineSignal& signal);
    void recordDopamineSignals(const std::vector<DopamineSignal>& signals);
    
    // Record curiosity signals
    void recordCuriositySignal(const CuriositySignal& signal);
    void recordCuriositySignals(const std::vector<CuriositySignal>& signals);
    
    // Record prediction error signals
    void recordPredictionErrorSignal(const PredictionErrorSignal& signal);
    void recordPredictionErrorSignals(const std::vector<PredictionErrorSignal>& signals);
    
    // Update attentional focus
    void updateAttentionalFocus(const AttentionalFocus& focus);
    
    // Update neuromodulation statistics
    void updateNeuromodulationStats(const NeuromodulationStats& stats);
    
    // Render neuromodulator signals
    void renderNeuromodulatorSignals(const std::vector<NeuromodulationSignal>& signals);
    void renderNeuromodulatorSignals() { renderNeuromodulatorSignals(currentNeuromodulatorSignals); }
    
    // Render dopamine signals
    void renderDopamineSignals(const std::vector<DopamineSignal>& signals);
    void renderDopamineSignals() { renderDopamineSignals(currentDopamineSignals); }
    
    // Render curiosity signals
    void renderCuriositySignals(const std::vector<CuriositySignal>& signals);
    void renderCuriositySignals() { renderCuriositySignals(currentCuriositySignals); }
    
    // Render prediction error signals
    void renderPredictionErrorSignals(const std::vector<PredictionErrorSignal>& signals);
    void renderPredictionErrorSignals() { renderPredictionErrorSignals(currentPredictionErrorSignals); }
    
    // Render attentional focus
    void renderAttentionalFocus(const AttentionalFocus& focus);
    void renderAttentionalFocus() { renderAttentionalFocus(currentAttentionalFocus); }
    
    // Render neuromodulation statistics
    void renderNeuromodulationStats(const NeuromodulationStats& stats);
    void renderNeuromodulationStats() { renderNeuromodulationStats(currentNeuromodulationStats); }
    
    // Configuration
    void setSignalHistorySize(size_t size);
    void setTimeWindow(double seconds);
    void setAutoUpdate(bool autoUpdate);
    
    // Data access
    const std::vector<NeuromodulationSignal>& getCurrentNeuromodulatorSignals() const { return currentNeuromodulatorSignals; }
    const std::vector<DopamineSignal>& getCurrentDopamineSignals() const { return currentDopamineSignals; }
    const std::vector<CuriositySignal>& getCurrentCuriositySignals() const { return currentCuriositySignals; }
    const std::vector<PredictionErrorSignal>& getCurrentPredictionErrorSignals() const { return currentPredictionErrorSignals; }
    const AttentionalFocus& getCurrentAttentionalFocus() const { return currentAttentionalFocus; }
    const NeuromodulationStats& getCurrentNeuromodulationStats() const { return currentNeuromodulationStats; }
    
    // Statistics
    float getAverageNeuromodulatorLevel() const;
    float getPeakDopamineLevel() const;
    float getTotalCuriosityLevel() const;
    float getMaxPredictionError() const;
    size_t getActiveNeuromodulatorCount() const;
    float getAverageLearningRate() const;
    float getAverageConfidence() const;
    
    // Export capabilities
    bool exportNeuromodulatorData(const std::string& filename);
    bool exportDopamineData(const std::string& filename);
    bool exportCuriosityData(const std::string& filename);
    bool exportPredictionErrorData(const std::string& filename);
    bool exportAttentionalFocusData(const std::string& filename);
    bool exportNeuromodulationStats(const std::string& filename);
    
protected:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm