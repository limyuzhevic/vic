#include "TestMetrics.hpp"
#include <sstream>
#include <numeric>
#include <algorithm>
#include <cmath>

namespace nlm {

// Implementation of MetricsAnalyzer methods
float MetricsAnalyzer::calculateMean(const std::vector<float>& values) {
    if (values.empty()) return 0.0f;
    float sum = std::accumulate(values.begin(), values.end(), 0.0f);
    return sum / values.size();
}

float MetricsAnalyzer::calculateStdDev(const std::vector<float>& values) {
    if (values.size() < 2) return 0.0f;
    float mean = calculateMean(values);
    float variance = 0.0f;
    for (float value : values) {
        variance += (value - mean) * (value - mean);
    }
    variance /= values.size();
    return std::sqrt(variance);
}

float MetricsAnalyzer::calculateCorrelation(const std::vector<float>& x, const std::vector<float>& y) {
    if (x.size() != y.size() || x.size() < 2) return 0.0f;
    
    float xMean = calculateMean(x);
    float yMean = calculateMean(y);
    
    float numerator = 0.0f;
    float xDenom = 0.0f;
    float yDenom = 0.0f;
    
    for (size_t i = 0; i < x.size(); ++i) {
        float xDiff = x[i] - xMean;
        float yDiff = y[i] - yMean;
        numerator += xDiff * yDiff;
        xDenom += xDiff * xDiff;
        yDenom += yDiff * yDiff;
    }
    
    if (xDenom == 0.0f || yDenom == 0.0f) return 0.0f;
    return numerator / std::sqrt(xDenom * yDenom);
}

float MetricsAnalyzer::calculateTrend(const std::vector<float>& values) {
    if (values.size() < 2) return 0.0f;
    
    float xMean = static_cast<float>(values.size() - 1) / 2.0f;
    float yMean = calculateMean(values);
    
    float numerator = 0.0f;
    float denominator = 0.0f;
    
    for (size_t i = 0; i < values.size(); ++i) {
        float x = static_cast<float>(i) - xMean;
        float y = values[i] - yMean;
        numerator += x * y;
        denominator += x * x;
    }
    
    if (denominator == 0.0f) return 0.0f;
    return numerator / denominator;
}

float MetricsAnalyzer::calculatePerformanceScore(const ComprehensiveMetrics& metrics) {
    float score = 0.0f;
    
    // Reward efficiency
    score += std::min(25.0f, metrics.totalReward / 10.0f);
    
    // Firing rate
    score += std::min(25.0f, metrics.averageFiringRate / 2.0f);
    
    // Memory efficiency
    float memoryScore = (metrics.workingMemoryTraps / 100.0f + 
                       metrics.episodicMemoryEpisodes / 1000.0f) * 25.0f;
    score += std::min(25.0f, memoryScore);
    
    // Cognitive performance
    score += std::min(25.0f, (metrics.neuralPlannerActions / 10.0f + 
                           metrics.conceptFormationUpdates / 10.0f) * 12.5f);
    
    return std::min(100.0f, score);
}

std::string MetricsAnalyzer::generateReport(const ComprehensiveMetrics& metrics, const std::string& experimentName) {
    std::stringstream ss;
    ss << "=== NLM " << experimentName << " Metrics Report ===\n\n";
    ss << "Performance Summary:\n";
    ss << "  Performance Score: " << calculatePerformanceScore(metrics) << "/100\n";
    ss << "  Total Reward: " << metrics.totalReward << "\n";
    ss << "  Average Reward per Step: " << (metrics.totalSteps > 0 ? metrics.totalReward / metrics.totalSteps : 0.0f) << "\n";
    ss << "  Average Firing Rate: " << metrics.averageFiringRate << "\n";
    ss << "  Maximum Firing Rate: " << metrics.maximumFiringRate << "\n\n";
    
    ss << "Memory System Performance:\n";
    ss << "  Working Memory Active Traces: " << metrics.workingMemoryTraps << "\n";
    ss << "  Episodic Memory Episodes: " << metrics.episodicMemoryEpisodes << "\n";
    ss << "  Episodic Memory Replay Count: " << metrics.episodicMemoryReplayCount << "\n";
    ss << "  Memory Consolidation Efficiency: " << metrics.episodicMemoryConsolidation << "\n\n";
    
    ss << "Neuromodulation Performance:\n";
    ss << "  Average Dopamine Level: " << metrics.averageDopamineLevel << "\n";
    ss << "  Average Curiosity Level: " << metrics.averageCuriosityLevel << "\n";
    ss << "  Average Novelty Level: " << metrics.averageNoveltyLevel << "\n";
    ss << "  Neuromodulation Stability: " << metrics.neuromodulationStability << "\n\n";
    
    ss << "Cognitive System Performance:\n";
    ss << "  Neural Planner Actions: " << metrics.neuralPlannerActions << "\n";
    ss << "  Concept Formation Updates: " << metrics.conceptFormationUpdates << "\n";
    ss << "  Attention Selections: " << metrics.attentionSelections << "\n";
    ss << "  Cognitive Flexibility: " << metrics.cognitiveFlexibility << "\n\n";
    
    ss << "Development System Performance:\n";
    ss << "  Developmental Stage Changes: " << metrics.developmentalStageChanges << "\n";
    ss << "  Developmental Progress: " << metrics.developmentalProgress << "\n";
    ss << "  Developmental Stability: " << metrics.developmentalStability << "\n\n";
    
    ss << "Integration Quality:\n";
    ss << "  System Cohesion: " << metrics.systemCohesion << "\n";
    ss << "  Information Flow: " << metrics.informationFlow << "\n";
    ss << "  Feedback Loop Strength: " << metrics.feedbackLoopStrength << "\n\n";
    
    ss << "Stress Test Performance:\n";
    ss << "  Max Load: " << metrics.stressTestMaxLoad << "\n";
    ss << "  Recovery Time: " << metrics.stressTestRecoveryTime << "\n";
    ss << "  Performance: " << metrics.stressTestPerformance << "\n\n";
    
    ss << "Learning Performance:\n";
    ss << "  Learning Rate: " << metrics.learningRate << "\n";
    ss << "  Adaptation Speed: " << metrics.adaptationSpeed << "\n";
    ss << "  Generalization Ability: " << metrics.generalizationAbility << "\n\n";
    
    ss << "=== End of Report ===\n";
    return ss.str();
}

} // namespace nlm
