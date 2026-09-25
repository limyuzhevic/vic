#pragma once

#include <vector>
#include <string>
#include <map>
#include <unordered_map>
#include <algorithm>
#include <numeric>
#include <sstream>
#include <cmath>
#include <memory>

namespace nlm {

// Enhanced metrics collection with comprehensive statistical analysis
class EnhancedMetrics {
public:
    EnhancedMetrics();
    ~EnhancedMetrics();
    
    // Record a metric value with rich metadata
    void record(const std::string& name, double value,
                const std::string& category = "general",
                const std::unordered_map<std::string, std::string>& metadata = {});
    
    // Get metric values
    const std::vector<double>& getValues(const std::string& name) const;
    std::vector<double> getValues(const std::string& name, const std::string& category) const;
    
    // Get statistics
    double getMean(const std::string& name) const;
    double getMean(const std::string& name, const std::string& category) const;
    double getStdDev(const std::string& name) const;
    double getStdDev(const std::string& name, const std::string& category) const;
    double getMin(const std::string& name) const;
    double getMin(const std::string& name, const std::string& category) const;
    double getMax(const std::string& name) const;
    double getMax(const std::string& name, const std::string& category) const;
    double getMedian(const std::string& name) const;
    double getMedian(const std::string& name, const std::string& category) const;
    double getPercentile(const std::string& name, double percentile) const;
    double getPercentile(const std::string& name, const std::string& category, double percentile) const;
    
    // Get all metric names and categories
    std::vector<std::string> getMetricNames() const;
    std::vector<std::string> getCategories() const;
    std::vector<std::pair<std::string, std::string>> getMetricCategories() const;
    
    // Specialized metrics for NLM systems
    
    // Performance metrics
    struct PerformanceMetrics {
        double neuralActivity;           // Average neural firing rate
        double synapticStrength;         // Average synaptic weight
        double predictionAccuracy;       // Prediction system accuracy
        double planningEfficiency;       // Planning success rate
        double attentionSelectivity;     // Attention focusing ability
        double memoryRetention;          // Memory retention score
        double learningSpeed;            // Rate of skill acquisition
        double generalization;           // Ability to transfer knowledge
        double explorationExploitation; // Balance of exploration vs exploitation
    };
    
    PerformanceMetrics computePerformanceMetrics() const;
    
    // Cognitive metrics
    struct CognitiveMetrics {
        double workingMemoryCapacity;     // Number of active traces
        double episodicMemoryStrength;   // Memory consolidation
        double conceptFormation;         // Number of stable concepts
        double predictionError;          // Prediction accuracy error
        double planningHorizon;          // Multi-step planning depth
        double socialLearningCapacity;   // Imitation success rate
        double curiosityLevel;           // Novelty seeking behavior
        double rewardSensitivity;        // Response to reward signals
        double intrinsicMotivation;      // Self-directed learning
    };
    
    CognitiveMetrics computeCognitiveMetrics() const;
    
    // Memory metrics
    struct MemoryMetrics {
        double encodingEfficiency;       // Speed/accuracy of encoding
        double retentionRate;            // Memory decay over time
        double recallAccuracy;           // Ability to retrieve stored information
        double interferenceLevel;        // Cross-talk between memories
        double consolidationRate;        // Speed of memory strengthening
        double patternSeparation;        // Ability to distinguish similar patterns
        double patternCompletion;        // Ability to fill in missing information
    };
    
    MemoryMetrics computeMemoryMetrics() const;
    
    // Learning metrics
    struct LearningMetrics {
        double learningCurveSlope;       // Rate of improvement
        double convergenceRate;          // Speed of reaching optimal performance
        double transferEfficiency;       // Ability to apply knowledge to new tasks
        double adaptationSpeed;          // Rate of adjustment to new environments
        double robustness;               // Stability across variations
        double sampleEfficiency;         // Information gained per experience
        double retentionAfterLearning;   // Memory persistence after training
    };
    
    LearningMetrics computeLearningMetrics() const;
    
    // Neuromodulation metrics
    struct NeuromodulationMetrics {
        double dopamineEffect;           // Impact of dopamine on plasticity
        double curiosityDrive;           // Exploration motivation
        double noveltySensitivity;       // Response to new stimuli
        double rewardPredictionError;    // Temporal difference error
        double attentionModulation;      // Cognitive focus control
        double arousalLevel;             // General activation state
        double motivationStrength;       // Goal-directed behavior
    };
    
    NeuromodulationMetrics computeNeuromodulationMetrics() const;
    
    // Development metrics
    struct DevelopmentMetrics {
        double plasticityRate;           // Rate of synaptic change
        double maturationLevel;          // Developmental stage
        double experienceIntegration;    // How well experiences shape behavior
        double skillComplexity;          // Depth of learned skills
        double behavioralFlexibility;   // Ability to change behavior
        double memoryOrganization;       // Structure of stored knowledge
        double cognitiveIntegration;     // Coordination between systems
    };
    
    DevelopmentMetrics computeDevelopmentMetrics() const;
    
    // Clear all metrics
    void clear();
    
    // Clear metrics by category
    void clear(const std::string& category);
    
    // Clear metrics by name
    void clear(const std::string& name, const std::string& category);
    
    // Summary report
    std::string summary() const;
    std::string summary(const std::string& category) const;
    std::string performanceSummary() const;
    std::string cognitiveSummary() const;
    std::string memorySummary() const;
    std::string learningSummary() const;
    std::string neuromodulationSummary() const;
    std::string developmentSummary() const;
    
    // JSON export
    std::string toJSON() const;
    std::string toJSON(const std::string& category) const;
    
    // Comparison with baseline
    double compareWithBaseline(const std::string& metricName,
                              const EnhancedMetrics& baseline,
                              const std::string& category = "general") const;
    
    // Progress tracking
    bool hasConverged(const std::string& metricName,
                      double threshold,
                      const std::string& category = "general") const;
    bool hasImproved(const std::string& metricName,
                     size_t windowSize,
                     const std::string& category = "general") const;
    
    // Time series analysis
    double computeTrend(const std::string& metricName,
                       const std::string& category = "general") const;
    std::vector<double> getTimeSeries(const std::string& metricName,
                                     const std::string& category = "general") const;
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
