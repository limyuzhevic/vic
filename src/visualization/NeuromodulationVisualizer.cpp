#include "NeuromodulationVisualizer.hpp"
#include "../core/Logger.hpp"
#include <algorithm>
#include <fstream>
#include <sstream>
#include <cmath>

namespace nlm {

struct NeuromodulationVisualizer::Impl {
    // Neuromodulator signals
    std::vector<NeuromodulationSignal> currentNeuromodulatorSignals;
    std::deque<NeuromodulationSignal> neuromodulatorHistory;
    size_t signalHistorySize;
    
    // Dopamine signals
    std::vector<DopamineSignal> currentDopamineSignals;
    std::deque<DopamineSignal> dopamineHistory;
    
    // Curiosity signals
    std::vector<CuriositySignal> currentCuriositySignals;
    std::deque<CuriositySignal> curiosityHistory;
    
    // Prediction error signals
    std::vector<PredictionErrorSignal> currentPredictionErrorSignals;
    std::deque<PredictionErrorSignal> predictionErrorHistory;
    
    // Attentional focus
    AttentionalFocus currentAttentionalFocus;
    
    // Neuromodulation statistics
    NeuromodulationStats currentNeuromodulationStats;
    
    // Configuration
    bool autoUpdate;
    double timeWindow; // seconds
    
    // Timing
    Timestamp lastTimestamp;
    bool initialized;
    
    Impl() : signalHistorySize(1000), autoUpdate(true), timeWindow(10.0),
             lastTimestamp(0), initialized(false) {}
};

NeuromodulationVisualizer::NeuromodulationVisualizer() : pImpl(std::make_unique<Impl>()) {}
NeuromodulationVisualizer::~NeuromodulationVisualizer() = default;

bool NeuromodulationVisualizer::initialize() {
    try {
        pImpl->initialized = true;
        Logger::getInstance().logInfo("Neuromodulation visualizer initialized");
        return true;
    } catch (const std::exception& e) {
        Logger::getInstance().logError("Failed to initialize neuromodulation visualizer: " + std::string(e.what()));
        return false;
    }
}

void NeuromodulationVisualizer::update(double deltaTime) {
    if (!pImpl->initialized) return;
    
    pImpl->lastTimestamp += static_cast<Timestamp>(deltaTime * 1000.0);
    
    // Clean old history data
    while (pImpl->neuromodulatorHistory.size() > pImpl->signalHistorySize) {
        pImpl->neuromodulatorHistory.pop_front();
    }
    
    // Update current data
    pImpl->currentNeuromodulatorSignals.assign(pImpl->neuromodulatorHistory.begin(), pImpl->neuromodulatorHistory.end());
    pImpl->currentDopamineSignals.assign(pImpl->dopamineHistory.begin(), pImpl->dopamineHistory.end());
    pImpl->currentCuriositySignals.assign(pImpl->curiosityHistory.begin(), pImpl->curiosityHistory.end());
    pImpl->currentPredictionErrorSignals.assign(pImpl->predictionErrorHistory.begin(), pImpl->predictionErrorHistory.end());
}

void NeuromodulationVisualizer::recordNeuromodulatorSignal(const NeuromodulationSignal& signal) {
    if (pImpl->initialized) {
        pImpl->neuromodulatorHistory.push_back(signal);
    }
}

void NeuromodulationVisualizer::recordNeuromodulatorSignals(const std::vector<NeuromodulationSignal>& signals) {
    for (const auto& signal : signals) {
        recordNeuromodulatorSignal(signal);
    }
}

void NeuromodulationVisualizer::recordDopamineSignal(const DopamineSignal& signal) {
    if (pImpl->initialized) {
        pImpl->dopamineHistory.push_back(signal);
    }
}

void NeuromodulationVisualizer::recordDopamineSignals(const std::vector<DopamineSignal>& signals) {
    for (const auto& signal : signals) {
        recordDopamineSignal(signal);
    }
}

void NeuromodulationVisualizer::recordCuriositySignal(const CuriositySignal& signal) {
    if (pImpl->initialized) {
        pImpl->curiosityHistory.push_back(signal);
    }
}

void NeuromodulationVisualizer::recordCuriositySignals(const std::vector<CuriositySignal>& signals) {
    for (const auto& signal : signals) {
        recordCuriositySignal(signal);
    }
}

void NeuromodulationVisualizer::recordPredictionErrorSignal(const PredictionErrorSignal& signal) {
    if (pImpl->initialized) {
        pImpl->predictionErrorHistory.push_back(signal);
    }
}

void NeuromodulationVisualizer::recordPredictionErrorSignals(const std::vector<PredictionErrorSignal>& signals) {
    for (const auto& signal : signals) {
        recordPredictionErrorSignal(signal);
    }
}

void NeuromodulationVisualizer::updateAttentionalFocus(const AttentionalFocus& focus) {
    pImpl->currentAttentionalFocus = focus;
}

void NeuromodulationVisualizer::updateNeuromodulationStats(const NeuromodulationStats& stats) {
    pImpl->currentNeuromodulationStats = stats;
}

void NeuromodulationVisualizer::renderNeuromodulatorSignals(const std::vector<NeuromodulationSignal>& signals) {
    std::cout << "=== Neuromodulator Signals ===" << std::endl;
    
    if (signals.empty()) {
        std::cout << "No neuromodulator signals recorded." << std::endl;
        return;
    }
    
    for (const auto& signal : signals) {
        std::cout << "Type: " << static_cast<int>(signal.type) << ", "
                  << "Level: " << signal.level << ", "
                  << "Target: " << signal.target << ", "
                  << "Error: " << signal.error << ", "
                  << "Confidence: " << signal.confidence << ", "
                  << "Source: " << signal.sourceRegion << ", "
                  << "Target: " << signal.targetRegion << ", "
                  << "Active: " << signal.isActive << ", "
                  << "Burst: " << signal.isBurst << std::endl;
    }
}

void NeuromodulationVisualizer::renderDopamineSignals(const std::vector<DopamineSignal>& signals) {
    std::cout << "=== Dopamine Signals ===" << std::endl;
    
    if (signals.empty()) {
        std::cout << "No dopamine signals recorded." << std::endl;
        return;
    }
    
    for (const auto& signal : signals) {
        std::cout << "Timestamp: " << signal.timestamp << ", "
                  << "Concentration: " << signal.concentration << ", "
                  << "Baseline: " << signal.baseline << ", "
                  << "Prediction Error: " << signal.predictionError << ", "
                  << "Learning Signal: " << signal.learningSignal << ", "
                  << "Novelty: " << signal.noveltyIndex << ", "
                  << "Attention: " << signal.attentionalFocus << ", "
                  << "Region: " << signal.region << ", "
                  << "Phasic: " << signal.isPhasic << std::endl;
    }
}

void NeuromodulationVisualizer::renderCuriositySignals(const std::vector<CuriositySignal>& signals) {
    std::cout << "=== Curiosity Signals ===" << std::endl;
    
    if (signals.empty()) {
        std::cout << "No curiosity signals recorded." << std::endl;
        return;
    }
    
    for (const auto& signal : signals) {
        std::cout << "Timestamp: " << signal.timestamp << ", "
                  << "Exploration: " << signal.explorationLevel << ", "
                  << "Motivation: " << signal.intrinsicMotivation << ", "
                  << "Uncertainty: " << signal.uncertaintyReduction << ", "
                  << "Information: " << signal.informationGain << ", "
                  << "Sensitivity: " << signal.noveltySensitivity << ", "
                  << "Region: " << signal.region << ", "
                  << "Spontaneous: " << signal.isSpontaneous << std::endl;
    }
}

void NeuromodulationVisualizer::renderPredictionErrorSignals(const std::vector<PredictionErrorSignal>& signals) {
    std::cout << "=== Prediction Error Signals ===" << std::endl;
    
    if (signals.empty()) {
        std::cout << "No prediction error signals recorded." << std::endl;
        return;
    }
    
    for (const auto& signal : signals) {
        std::cout << "Timestamp: " << signal.timestamp << ", "
                  << "Magnitude: " << signal.errorMagnitude << ", "
                  << "Signed: " << signal.signedError << ", "
                  << "Confidence: " << signal.predictionConfidence << ", "
                  << "Update: " << signal.updateStrength << ", "
                  << "Source: " << signal.sourceRegion << ", "
                  << "Target: " << signal.targetRegion << ", "
                  << "Positive: " << signal.isPositive << ", "
                  << "Negative: " << signal.isNegative << std::endl;
    }
}

void NeuromodulationVisualizer::renderAttentionalFocus(const AttentionalFocus& focus) {
    std::cout << "=== Attentional Focus ===" << std::endl;
    std::cout << "Global Attention: " << focus.globalAttentionLevel << std::endl;
    std::cout << "Primary Target: " << focus.primaryTarget << ", "
              << "Secondary Target: " << focus.secondaryTarget << std::endl;
    std::cout << "Top-Down: " << (focus.isTopDown ? "Yes" : "No") << std::endl;
    std::cout << "Focused Regions: ";
    for (size_t i = 0; i < focus.focusedRegions.size(); ++i) {
        if (i > 0) std::cout << ", ";
        std::cout << focus.focusedRegions[i];
    }
    std::cout << std::endl;
    
    std::cout << "Regional Attention Strengths:" << std::endl;
    for (const auto& [regionId, strength] : focus.regionalAttentionStrength) {
        std::cout << "  Region " << regionId << ": " << strength << std::endl;
    }
}

void NeuromodulationVisualizer::renderNeuromodulationStats(const NeuromodulationStats& stats) {
    std::cout << "=== Neuromodulation Statistics ===" << std::endl;
    std::cout << "Total Level: " << stats.totalNeuromodulatorLevel << std::endl;
    std::cout << "Avg Dopamine: " << stats.averageDopamineLevel << std::endl;
    std::cout << "Avg Curiosity: " << stats.averageCuriosityLevel << std::endl;
    std::cout << "Avg Prediction Error: " << stats.averagePredictionError << std::endl;
    std::cout << "Active Count: " << stats.activeNeuromodulators << std::endl;
    std::cout << "Total Events: " << stats.totalNeuromodulatorEvents << std::endl;
    std::cout << "Avg Learning Rate: " << stats.averageLearningRate << std::endl;
    std::cout << "Avg Confidence: " << stats.averageConfidence << std::endl;
    
    std::cout << "Type Distribution:" << std::endl;
    for (const auto& [type, count] : stats.typeDistribution) {
        std::cout << "  " << static_cast<int>(type) << ": " << count << std::endl;
    }
    
    std::cout << "Regional Influence:" << std::endl;
    for (const auto& [region, influence] : stats.regionalInfluence) {
        std::cout << "  " << region << ": " << influence << std::endl;
    }
}

void NeuromodulationVisualizer::setSignalHistorySize(size_t size) {
    pImpl->signalHistorySize = size;
    while (pImpl->neuromodulatorHistory.size() > size) {
        pImpl->neuromodulatorHistory.pop_front();
    }
}

void NeuromodulationVisualizer::setTimeWindow(double seconds) {
    pImpl->timeWindow = seconds;
}

void NeuromodulationVisualizer::setAutoUpdate(bool autoUpdate) {
    pImpl->autoUpdate = autoUpdate;
}

float NeuromodulationVisualizer::getAverageNeuromodulatorLevel() const {
    if (pImpl->neuromodulatorHistory.empty()) return 0.0f;
    
    float totalLevel = 0.0f;
    for (const auto& signal : pImpl->neuromodulatorHistory) {
        totalLevel += signal.level;
    }
    
    return totalLevel / pImpl->neuromodulatorHistory.size();
}

float NeuromodulationVisualizer::getPeakDopamineLevel() const {
    if (pImpl->dopamineHistory.empty()) return 0.0f;
    
    float maxLevel = 0.0f;
    for (const auto& signal : pImpl->dopamineHistory) {
        maxLevel = std::max(maxLevel, signal.concentration);
    }
    
    return maxLevel;
}

float NeuromodulationVisualizer::getTotalCuriosityLevel() const {
    if (pImpl->curiosityHistory.empty()) return 0.0f;
    
    float totalLevel = 0.0f;
    for (const auto& signal : pImpl->curiosityHistory) {
        totalLevel += signal.explorationLevel;
    }
    
    return totalLevel;
}

float NeuromodulationVisualizer::getMaxPredictionError() const {
    if (pImpl->predictionErrorHistory.empty()) return 0.0f;
    
    float maxError = 0.0f;
    for (const auto& signal : pImpl->predictionErrorHistory) {
        maxError = std::max(maxError, std::abs(signal.errorMagnitude));
    }
    
    return maxError;
}

size_t NeuromodulationVisualizer::getActiveNeuromodulatorCount() const {
    size_t count = 0;
    for (const auto& signal : pImpl->neuromodulatorHistory) {
        if (signal.isActive) count++;
    }
    return count;
}

float NeuromodulationVisualizer::getAverageLearningRate() const {
    if (pImpl->neuromodulatorHistory.empty()) return 0.0f;
    
    float totalRate = 0.0f;
    for (const auto& signal : pImpl->neuromodulatorHistory) {
        totalRate += signal.learningRate;
    }
    
    return totalRate / pImpl->neuromodulatorHistory.size();
}

float NeuromodulationVisualizer::getAverageConfidence() const {
    if (pImpl->neuromodulatorHistory.empty()) return 0.0f;
    
    float totalConfidence = 0.0f;
    for (const auto& signal : pImpl->neuromodulatorHistory) {
        totalConfidence += signal.confidence;
    }
    
    return totalConfidence / pImpl->neuromodulatorHistory.size();
}

bool NeuromodulationVisualizer::exportNeuromodulatorData(const std::string& filename) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        return false;
    }
    
    file << "NeuromodulatorData:Timestamp,Type,Level,Target,Error,LearningRate,Confidence,SourceRegion,TargetRegion,IsActive,IsBurst,BurstIntensity\n";
    for (const auto& signal : pImpl->neuromodulatorHistory) {
        file << signal.timestamp << "," << static_cast<int>(signal.type) << "," 
             << signal.level << "," << signal.target << "," << signal.error << "," 
             << signal.learningRate << "," << signal.confidence << "," 
             << signal.sourceRegion << "," << signal.targetRegion << "," 
             << signal.isActive << "," << signal.isBurst << "," 
             << signal.burstIntensity << "\n";
    }
    
    file.close();
    
    Logger::getInstance().logInfo("Neuromodulator data exported to: " + filename);
    return true;
}

bool NeuromodulationVisualizer::exportDopamineData(const std::string& filename) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        return false;
    }
    
    file << "DopamineData:Timestamp,Concentration,Baseline,PredictionError,RewardPrediction,LearningSignal,NoveltyIndex,AttentionalFocus,Region,IsPhasic,PhasicAmplitude\n";
    for (const auto& signal : pImpl->dopamineHistory) {
        file << signal.timestamp << "," << signal.concentration << "," 
             << signal.baseline << "," << signal.predictionError << "," 
             << signal.rewardPrediction << "," << signal.learningSignal << "," 
             << signal.noveltyIndex << "," << signal.attentionalFocus << "," 
             << signal.region << "," << signal.isPhasic << "," 
             << signal.phasicAmplitude << "\n";
    }
    
    file.close();
    
    Logger::getInstance().logInfo("Dopamine data exported to: " + filename);
    return true;
}

bool NeuromodulationVisualizer::exportCuriosityData(const std::string& filename) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        return false;
    }
    
    file << "CuriosityData:Timestamp,ExplorationLevel,IntrinsicMotivation,UncertaintyReduction,InformationGain,NoveltySensitivity,PredictionErrorSize,Region,IsSpontaneous,SpontaneityIndex\n";
    for (const auto& signal : pImpl->curiosityHistory) {
        file << signal.timestamp << "," << signal.explorationLevel << "," 
             << signal.intrinsicMotivation << "," << signal.uncertaintyReduction << "," 
             << signal.informationGain << "," << signal.noveltySensitivity << "," 
             << signal.predictionErrorSize << "," << signal.region << "," 
             << signal.isSpontaneous << "," << signal.spontaneityIndex << "\n";
    }
    
    file.close();
    
    Logger::getInstance().logInfo("Curiosity data exported to: " + filename);
    return true;
}

bool NeuromodulationVisualizer::exportPredictionErrorData(const std::string& filename) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        return false;
    }
    
    file << "PredictionErrorData:Timestamp,ErrorMagnitude,SignedError,PredictionConfidence,UpdateStrength,ExpectedOutcome,ActualOutcome,SourceRegion,TargetRegion,Gradient,IsPositive,IsNegative\n";
    for (const auto& signal : pImpl->predictionErrorHistory) {
        file << signal.timestamp << "," << signal.errorMagnitude << "," 
             << signal.signedError << "," << signal.predictionConfidence << "," 
             << signal.updateStrength << "," << "\"" << signal.expectedOutcome << "\"" << "," 
             << "\"" << signal.actualOutcome << "\"" << "," << signal.sourceRegion << "," 
             << signal.targetRegion << "," << signal.gradient << "," 
             << signal.isPositive << "," << signal.isNegative << "\n";
    }
    
    file.close();
    
    Logger::getInstance().logInfo("Prediction error data exported to: " + filename);
    return true;
}

bool NeuromodulationVisualizer::exportAttentionalFocusData(const std::string& filename) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        return false;
    }
    
    file << "AttentionalFocusData:Timestamp,FocusedRegions,FocusedNeurons,GlobalAttentionLevel,RegionalAttentionStrength,NoveltyGating,PredictionGating,PrimaryTarget,SecondaryTarget,IsTopDown\n";
    file << pImpl->currentAttentionalFocus.timestamp << ",";
    
    for (size_t i = 0; i < pImpl->currentAttentionalFocus.focusedRegions.size(); ++i) {
        if (i > 0) file << ";";
        file << pImpl->currentAttentionalFocus.focusedRegions[i];
    }
    file << ",";
    
    for (size_t i = 0; i < pImpl->currentAttentionalFocus.focusedNeurons.size(); ++i) {
        if (i > 0) file << ";";
        file << pImpl->currentAttentionalFocus.focusedNeurons[i];
    }
    file << "," << pImpl->currentAttentionalFocus.globalAttentionLevel << ",";
    
    for (const auto& [regionId, strength] : pImpl->currentAttentionalFocus.regionalAttentionStrength) {
        file << regionId << "=" << strength << ",";
    }
    file << pImpl->currentAttentionalFocus.noveltyGating << "," << pImpl->currentAttentionalFocus.predictionGating << "," 
         << "\"" << pImpl->currentAttentionalFocus.primaryTarget << "\"" << "," 
         << "\"" << pImpl->currentAttentionalFocus.secondaryTarget << "\"" << "," 
         << pImpl->currentAttentionalFocus.isTopDown << "\n";
    
    file.close();
    
    Logger::getInstance().logInfo("Attentional focus data exported to: " + filename);
    return true;
}

bool NeuromodulationVisualizer::exportNeuromodulationStats(const std::string& filename) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        return false;
    }
    
    file << "NeuromodulationStatsData:Timestamp,TotalLevel,AvgDopamine,AvgCuriosity,AvgPredictionError,ActiveCount,TotalEvents,AvgLearningRate,AvgConfidence\n";
    file << pImpl->currentNeuromodulationStats.timestamp << "," << pImpl->currentNeuromodulationStats.totalNeuromodulatorLevel << "," 
         << pImpl->currentNeuromodulationStats.averageDopamineLevel << "," << pImpl->currentNeuromodulationStats.averageCuriosityLevel << "," 
         << pImpl->currentNeuromodulationStats.averagePredictionError << "," << pImpl->currentNeuromodulationStats.activeNeuromodulators << "," 
         << pImpl->currentNeuromodulationStats.totalNeuromodulatorEvents << "," << pImpl->currentNeuromodulationStats.averageLearningRate << "," 
         << pImpl->currentNeuromodulationStats.averageConfidence << "\n";
    
    file << "\nTypeDistributionData:Timestamp,NeuromodulatorType,Count\n";
    for (const auto& [type, count] : pImpl->currentNeuromodulationStats.typeDistribution) {
        file << pImpl->currentNeuromodulationStats.timestamp << "," << static_cast<int>(type) << "," << count << "\n";
    }
    
    file << "\nRegionalInfluenceData:Timestamp,Region,Influence\n";
    for (const auto& [region, influence] : pImpl->currentNeuromodulationStats.regionalInfluence) {
        file << pImpl->currentNeuromodulationStats.timestamp << "," << region << "," << influence << "\n";
    }
    
    file.close();
    
    Logger::getInstance().logInfo("Neuromodulation statistics exported to: " + filename);
    return true;
}

} // namespace nlm