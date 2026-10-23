#pragma once

#include "../core/Types/Types.hpp"
#include <vector>
#include <string>
#include <map>
#include <algorithm>
#include <cmath>
#include <chrono>

namespace nlm {

// Comprehensive metrics collection for all brain systems
struct ComprehensiveMetrics {
    // Timestamp
    double startTime;
    double endTime;
    
    // Performance metrics
    float totalReward;
    float totalSteps;
    float averageFiringRate;
    float maximumFiringRate;
    float averagePredictionAccuracy;
    float averagePlanningSuccess;
    
    // Memory system metrics
    size_t workingMemoryTraps;
    size_t episodicMemoryEpisodes;
    size_t episodicMemoryReplayCount;
    float episodicMemoryConsolidation;
    
    // Neuromodulation metrics
    float averageDopamineLevel;
    float averageCuriosityLevel;
    float averageNoveltyLevel;
    float neuromodulationStability;
    
    // Cognitive system metrics
    size_t neuralPlannerActions;
    size_t conceptFormationUpdates;
    size_t attentionSelections;
    float cognitiveFlexibility;
    
    // Development system metrics
    size_t developmentalStageChanges;
    float developmentalProgress;
    float developmentalStability;
    
    // Prediction system metrics
    size_t predictionUpdates;
    float predictionErrorHistory;
    float predictionConfidence;
    
    // Integration metrics
    float systemCohesion;
    float informationFlow;
    float feedbackLoopStrength;
    
    // Stress test metrics (for Phase 6)
    float stressTestMaxLoad;
    float stressTestRecoveryTime;
    float stressTestPerformance;
    
    // Learning metrics
    float learningRate;
    float adaptationSpeed;
    float generalizationAbility;
    
    // Memory-specific metrics
    float memoryRetentionRate;
    float memoryConsolidationEfficiency;
    float memoryReplayEffectiveness;
    
    // Attention-specific metrics
    float attentionFocusDuration;
    float attentionSwitchingSpeed;
    float attentionStability;
    
    // Planning and decision metrics
    size_t planningIterations;
    size_t decisionConfidence;
    float actionSelectionConsistency;
    
    // Neuroplasticity metrics
    size_t synapticChanges;
    size_t structuralPlasticityEvents;
    float plasticityRate;
    float homeostasisBalance;
    
    // Social learning metrics
    size_t socialObservations;
    size_t imitationActions;
    float socialLearningRate;
    
    // Self-model metrics
    size_t selfModelUpdates;
    float selfModelAccuracy;
    float selfModelStability;
    
    // Reward processing metrics
    float rewardPredictionError;
    float rewardSensitivity;
    float rewardLearningSpeed;
    
    // Novelty processing metrics
    size_t noveltyEvents;
    float noveltyDetectionSpeed;
    float noveltyResponseStrength;
    
    // Curiosity-driven exploration metrics
    float explorationRate;
    float intrinsicMotivation;
    float goalDrivenBehavior;
    
    // Stress and fatigue metrics
    float cognitiveLoad;
    float mentalFatigue;
    float stressResponse;
    
    // Environmental interaction metrics
    float interactionComplexity;
    float behavioralFlexibility;
    float adaptationToChange;
    
    // Sleep and recovery metrics
    float sleepQuality;
    float recoveryTime;
    float consolidationDuringSleep;
    
    // Social interaction metrics
    float socialCohesion;
    float cooperationLevel;
    float conflictResolution;
    
    // Memory consolidation during sleep
    float sleepConsolidationEfficiency;
    float memoryReplayDuringSleep;
    float synapticStrengthDuringSleep;
    
    // Reward prediction during sleep
    float rewardPredictionDuringSleep;
    float emotionalProcessingDuringSleep;
    
    // Predictive coding during sleep
    float predictiveCodingStrength;
    float memoryIntegrationDuringSleep;
    float schemaFormationDuringSleep;
    
    // Moral development metrics
    float moralReasoning;
    float empathyLevel;
    float ethicalDecisionMaking;
    
    // Cultural transmission metrics
    float culturalLearningRate;
    float traditionRetention;
    float innovationAdoption;
    
    // Team coordination metrics
    float teamCohesion;
    float taskAllocation;
    float collaborativeProblemSolving;
    
    // Long-term memory metrics
    float longTermMemoryStability;
    float memoryDecayRate;
    float memoryRetrievalSpeed;
    
    // Working memory metrics
    float workingMemoryCapacity;
    float workingMemoryStability;
    float workingMemoryUpdateRate;
    
    // Episodic memory metrics
    float episodicMemoryAutobiographicalQuality;
    float episodicMemoryTemporalOrder;
    float episodicMemorySpatialAccuracy;
    
    // Semantic memory metrics
    float semanticMemoryCohesion;
    float semanticMemoryIntegration;
    float semanticMemoryGeneralization;
    
    // Procedural memory metrics
    float proceduralMemorySkillAcquisition;
    float proceduralMemoryTransfer;
    float proceduralMemoryAutomaticity;
    
    // Associative memory metrics
    float associativeMemoryStrength;
    float associativeMemoryFlexibility;
    float associativeMemorySpreading;
    
    // Prediction system metrics
    float predictionAccuracy;
    float predictionError;
    float predictionUncertainty;
    
    // Cognitive flexibility metrics
    float cognitiveInhibition;
    float cognitiveSetShifting;
    float cognitiveCreativity;
    
    // Social cognition metrics
    float theoryOfMind;
    float empathyDevelopment;
    float moralDevelopment;
    
    // Executive function metrics
    float inhibitoryControl;
    float cognitiveFlexibility;
    float workingMemoryCapacity;
    
    // Stress resilience metrics
    float stressTolerance;
    float emotionalRegulation;
    float copingStrategies;
    
    // Recovery metrics
    float recoverySpeed;
    float resilience;
    float adaptability;
    
    // Sleep metrics
    float sleepDuration;
    float sleepQuality;
    float sleepArchitecture;
    
    // Memory consolidation during sleep
    float memoryConsolidationDuringSleep;
    float synapticStrengthDuringSleep;
    float schemaFormationDuringSleep;
    
    // Reward prediction during sleep
    float rewardPredictionDuringSleep;
    float emotionalProcessingDuringSleep;
    
    // Predictive coding during sleep
    float predictiveCodingStrength;
    float memoryIntegrationDuringSleep;
    
    // Comprehensive summary
    std::string getSummary() const {
        std::stringstream ss;
        ss << "=== NLM Comprehensive Metrics Summary ===\n";
        ss << "Total Duration: " << (endTime - startTime) << " seconds\n";
        ss << "Total Reward: " << totalReward << "\n";
        ss << "Average Reward per Step: " << (totalSteps > 0 ? totalReward / totalSteps : 0.0f) << "\n";
        ss << "Average Firing Rate: " << averageFiringRate << "\n";
        ss << "Maximum Firing Rate: " << maximumFiringRate << "\n";
        ss << "\nMemory Systems:\n";
        ss << "  Working Memory Traps: " << workingMemoryTraps << "\n";
        ss << "  Episodic Memory Episodes: " << episodicMemoryEpisodes << "\n";
        ss << "  Episodic Memory Replay Count: " << episodicMemoryReplayCount << "\n";
        ss << "  Memory Consolidation Efficiency: " << episodicMemoryConsolidation << "\n";
        ss << "\nCognitive Systems:\n";
        ss << "  Neural Planner Actions: " << neuralPlannerActions << "\n";
        ss << "  Concept Formation Updates: " << conceptFormationUpdates << "\n";
n        ss << "  Attention Selections: " << attentionSelections << "\n";
        ss << "  Cognitive Flexibility: " << cognitiveFlexibility << "\n";
        ss << "\nDevelopment System:\n";
        ss << "  Developmental Stage Changes: " << developmentalStageChanges << "\n";
        ss << "  Developmental Progress: " << developmentalProgress << "\n";
        ss << "  Developmental Stability: " << developmentalStability << "\n";
        ss << "\nNeuromodulation:\n";
        ss << "  Average Dopamine Level: " << averageDopamineLevel << "\n";
        ss << "  Average Curiosity Level: " << averageCuriosityLevel << "\n";
        ss << "  Average Novelty Level: " << averageNoveltyLevel << "\n";
        ss << "  Neuromodulation Stability: " << neuromodulationStability << "\n";
        ss << "\nSystem Integration:\n";
        ss << "  System Cohesion: " << systemCohesion << "\n";
        ss << "  Information Flow: " << informationFlow << "\n";
        ss << "  Feedback Loop Strength: " << feedbackLoopStrength << "\n";
        ss << "\nStress Test Results:\n";
        ss << "  Max Load: " << stressTestMaxLoad << "\n";
        ss << "  Recovery Time: " << stressTestRecoveryTime << "\n";
        ss << "  Performance: " << stressTestPerformance << "\n";
        ss << "\nLearning Metrics:\n";
        ss << "  Learning Rate: " << learningRate << "\n";
        ss << "  Adaptation Speed: " << adaptationSpeed << "\n";
        ss << "  Generalization Ability: " << generalizationAbility << "\n";
        ss << "\nEnd of Summary ===\n";
        return ss.str();
    }
};

// Statistical analysis utilities
class MetricsAnalyzer {
public:
    // Calculate mean
    static float calculateMean(const std::vector<float>& values) {
        if (values.empty()) return 0.0f;
        float sum = std::accumulate(values.begin(), values.end(), 0.0f);
        return sum / values.size();
    }
    
    // Calculate standard deviation
    static float calculateStdDev(const std::vector<float>& values) {
        if (values.size() < 2) return 0.0f;
        float mean = calculateMean(values);
        float variance = 0.0f;
        for (float value : values) {
            variance += (value - mean) * (value - mean);
        }
        variance /= values.size();
        return std::sqrt(variance);
    }
    
    // Calculate correlation coefficient
    static float calculateCorrelation(const std::vector<float>& x, const std::vector<float>& y) {
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
    
    // Calculate trend (linear regression slope)
    static float calculateTrend(const std::vector<float>& values) {
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
    
    // Calculate performance score (normalized 0-100)
    static float calculatePerformanceScore(const ComprehensiveMetrics& metrics) {
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
    
    // Generate report
    static std::string generateReport(const ComprehensiveMetrics& metrics, const std::string& experimentName) {
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
};

// Experiment metrics collector
class ExperimentMetricsCollector {
private:
    std::vector<ComprehensiveMetrics> metricsHistory;
    ComprehensiveMetrics currentMetrics;
    double startTime;
    
public:
    ExperimentMetricsCollector() {
        startTime = std::chrono::duration<double>::zero().count();
        resetCurrentMetrics();
    }
    
    void startExperiment() {
        resetCurrentMetrics();
        startTime = std::chrono::duration<double>::zero().count();
    }
    
    void endExperiment() {
        currentMetrics.endTime = startTime + std::chrono::duration<double>::zero().count();
        metricsHistory.push_back(currentMetrics);
        resetCurrentMetrics();
    }
    
    void updateMetrics(const std::string& metricName, float value) {
        if (metricName == "totalReward") {
            currentMetrics.totalReward = value;
        } else if (metricName == "totalSteps") {
            currentMetrics.totalSteps = value;
        } else if (metricName == "averageFiringRate") {
            currentMetrics.averageFiringRate = value;
        } else if (metricName == "maximumFiringRate") {
            currentMetrics.maximumFiringRate = value;
        } else if (metricName == "workingMemoryTraps") {
            currentMetrics.workingMemoryTraps = static_cast<size_t>(value);
        } else if (metricName == "episodicMemoryEpisodes") {
            currentMetrics.episodicMemoryEpisodes = static_cast<size_t>(value);
        } else if (metricName == "episodicMemoryReplayCount") {
            currentMetrics.episodicMemoryReplayCount = static_cast<size_t>(value);
        } else if (metricName == "episodicMemoryConsolidation") {
            currentMetrics.episodicMemoryConsolidation = value;
        } else if (metricName == "averageDopamineLevel") {
            currentMetrics.averageDopamineLevel = value;
        } else if (metricName == "averageCuriosityLevel") {
            currentMetrics.averageCuriosityLevel = value;
        } else if (metricName == "averageNoveltyLevel") {
            currentMetrics.averageNoveltyLevel = value;
        } else if (metricName == "neuromodulationStability") {
            currentMetrics.neuromodulationStability = value;
        } else if (metricName == "neuralPlannerActions") {
            currentMetrics.neuralPlannerActions = static_cast<size_t>(value);
        } else if (metricName == "conceptFormationUpdates") {
            currentMetrics.conceptFormationUpdates = static_cast<size_t>(value);
        } else if (metricName == "attentionSelections") {
            currentMetrics.attentionSelections = static_cast<size_t>(value);
        } else if (metricName == "cognitiveFlexibility") {
            currentMetrics.cognitiveFlexibility = value;
        } else if (metricName == "developmentalStageChanges") {
            currentMetrics.developmentalStageChanges = static_cast<size_t>(value);
        } else if (metricName == "developmentalProgress") {
            currentMetrics.developmentalProgress = value;
        } else if (metricName == "developmentalStability") {
            currentMetrics.developmentalStability = value;
        } else if (metricName == "systemCohesion") {
            currentMetrics.systemCohesion = value;
        } else if (metricName == "informationFlow") {
            currentMetrics.informationFlow = value;
        } else if (metricName == "feedbackLoopStrength") {
            currentMetrics.feedbackLoopStrength = value;
        } else if (metricName == "stressTestMaxLoad") {
            currentMetrics.stressTestMaxLoad = value;
        } else if (metricName == "stressTestRecoveryTime") {
            currentMetrics.stressTestRecoveryTime = value;
        } else if (metricName == "stressTestPerformance") {
            currentMetrics.stressTestPerformance = value;
        } else if (metricName == "learningRate") {
            currentMetrics.learningRate = value;
        } else if (metricName == "adaptationSpeed") {
            currentMetrics.adaptationSpeed = value;
        } else if (metricName == "generalizationAbility") {
            currentMetrics.generalizationAbility = value;
        }
    }
    
    void incrementStep() {
        currentMetrics.totalSteps++;
    }
    
    float getTotalReward() const {
        return currentMetrics.totalReward;
    }
    
    float getAverageFiringRate() const {
        return currentMetrics.averageFiringRate;
    }
    
    const std::vector<ComprehensiveMetrics>& getMetricsHistory() const {
        return metricsHistory;
    }
    
    std::string generateSummaryReport() const {
        std::stringstream ss;
        ss << "=== NLM Experiment Metrics Summary ===\n\n";
        ss << "Total Experiments: " << metricsHistory.size() << "\n";
        
        if (!metricsHistory.empty()) {
            ss << "\nPerformance Statistics:\n";
            
            std::vector<float> rewards;
            std::vector<float> firingRates;
            std::vector<float> cohesionScores;
            
            for (const auto& metrics : metricsHistory) {
                rewards.push_back(metrics.totalReward);
                firingRates.push_back(metrics.averageFiringRate);
                cohesionScores.push_back(metrics.systemCohesion);
            }
            
            ss << "  Reward Statistics:\n";
            ss << "    Mean: " << MetricsAnalyzer::calculateMean(rewards) << "\n";
            ss << "    Std Dev: " << MetricsAnalyzer::calculateStdDev(rewards) << "\n";
            ss << "    Min: " << *std::min_element(rewards.begin(), rewards.end()) << "\n";
            ss << "    Max: " << *std::max_element(rewards.begin(), rewards.end()) << "\n";
            
            ss << "  Firing Rate Statistics:\n";
            ss << "    Mean: " << MetricsAnalyzer::calculateMean(firingRates) << "\n";
            ss << "    Std Dev: " << MetricsAnalyzer::calculateStdDev(firingRates) << "\n";
            ss << "    Min: " << *std::min_element(firingRates.begin(), firingRates.end()) << "\n";
            ss << "    Max: " << *std::max_element(firingRates.begin(), firingRates.end()) << "\n";
            
            ss << "  System Cohesion Statistics:\n";
            ss << "    Mean: " << MetricsAnalyzer::calculateMean(cohesionScores) << "\n";
            ss << "    Std Dev: " << MetricsAnalyzer::calculateStdDev(cohesionScores) << "\n";
            ss << "    Min: " << *std::min_element(cohesionScores.begin(), cohesionScores.end()) << "\n";
            ss << "    Max: " << *std::max_element(cohesionScores.begin(), cohesionScores.end()) << "\n";
            
            ss << "\nExperiment Rankings:\n";
            for (size_t i = 0; i < metricsHistory.size(); ++i) {
                ss << "  Experiment " << i << ": Performance Score " 
                   << MetricsAnalyzer::calculatePerformanceScore(metricsHistory[i]) << "/100\n";
            }
        }
        
        ss << "\n=== End of Summary ===\n";
        return ss.str();
    }
    
private:
    void resetCurrentMetrics() {
        currentMetrics = ComprehensiveMetrics();
        currentMetrics.startTime = startTime;
        currentMetrics.endTime = startTime + std::chrono::duration<double>::zero().count();
    }
};

} // namespace nlm
