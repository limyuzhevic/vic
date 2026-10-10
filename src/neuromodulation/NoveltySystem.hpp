#pragma once

#include "../core/Types/Types.hpp"
#include "../brain/Brain.hpp"
#include <vector>
#include <memory>
#include <string>
#include <unordered_map>
#include <algorithm>
#include <cmath>
#include <unordered_set>

namespace nlm {

// Forward declarations
class NeuromodulationEngine;

// Novelty detection state
struct NoveltyState {
    float noveltyLevel;                // Current novelty level (0-1)
    float patternDeviation;            // Deviation from learned patterns
    float predictionSurprise;          // Surprise from prediction errors
    float sensoryNovelty;             // Novelty in sensory input
    float cognitiveNovelty;           // Novelty in cognitive processing
    float temporalNovelty;            // Novelty over time
    float spatialNovelty;             // Novelty across space
    float socialNovelty;              // Novelty in social contexts
    float learnedPatternWeight;        // Weight for learned pattern detection
    float adaptiveThreshold;           // Adaptive threshold for novelty detection
    float noveltyDecayRate;            // Rate at which novelty decays
    float adaptationRate;              // Rate at which detection adapts
    float memoryConsolidation;         // Memory consolidation strength
    float associationStrength;         // Strength of associations
    
    NoveltyState()
        : noveltyLevel(0.0f), patternDeviation(0.0f), predictionSurprise(0.0f),
          sensoryNovelty(0.0f), cognitiveNovelty(0.0f), temporalNovelty(0.0f),
          spatialNovelty(0.0f), socialNovelty(0.0f), learnedPatternWeight(1.0f),
          adaptiveThreshold(0.5f), noveltyDecayRate(0.1f), adaptationRate(0.05f),
          memoryConsolidation(0.5f), associationStrength(1.0f) {}
};

// Pattern representation for novelty detection
struct Pattern {
    std::vector<float> features;      // Feature vector
    float similarity;                  // Similarity to other patterns
    float noveltyScore;                 // Novelty score
    float occurrenceCount;              // How often this pattern has occurred
    std::vector<float> statistics;     // Statistical properties (mean, variance, etc.)
    float lastSeenTime;                 // Time when pattern was last seen
    float activationLevel;              // Current activation level
    
    Pattern()
        : similarity(0.0f), noveltyScore(0.0f), occurrenceCount(0.0f),
          lastSeenTime(0.0f), activationLevel(0.0f) {}
};

// Novelty type classification
enum class NoveltyType {
    SENSORY,           // Sensory input novelty
    COGNITIVE,         // Cognitive processing novelty
    TEMPORAL,          // Temporal sequence novelty
    SPATIAL,           // Spatial configuration novelty
    SOCIAL,            // Social context novelty
    CONCEPTUAL,        // Conceptual knowledge novelty
    BEHAVIORAL,        // Behavioral pattern novelty
    COMBINED          // Combined novelty
};

// Advanced novelty system: Pattern detection and surprise computation
class NoveltySystem {
public:
    NoveltySystem();
    ~NoveltySystem();
    
    // Disable copying, enable moving
    NoveltySystem(const NoveltySystem&) = delete;
    NoveltySystem& operator=(const NoveltySystem&) = delete;
    NoveltySystem(NoveltySystem&&) noexcept;
    NoveltySystem& operator=(NoveltySystem&&) noexcept;
    
    // Initialization
    bool initialize(std::shared_ptr<Brain> brain, std::shared_ptr<NeuromodulationEngine> engine);
    
    // Main update step
    void update(TimestepDuration dt, SimulationStep step);
    
    // Novelty detection
    void detectNovelty(const Observation& observation, 
                      const Observation& previousObservation);
    void detectNovelty(const std::vector<float>& currentPattern,
                      const std::vector<float>& previousPattern);
    void detectNovelty(const class NeuralPattern& neuralPattern);
    
    // Pattern management
    void learnPattern(const std::vector<float>& pattern);
    void forgetPattern(const std::vector<float>& pattern);
    void updatePatternStatistics(const std::vector<float>& pattern);
    void computePatternNovelty(Pattern& pattern);
    
    // Novelty type detection
    NoveltyType classifyNoveltyType(const Observation& observation) const;
    float computeSensoryNovelty(const Observation& observation) const;
    float computeCognitiveNovelty(const Observation& observation) const;
    float computeTemporalNovelty(const Observation& observation) const;
    float computeSpatialNovelty(const Observation& observation) const;
    float computeSocialNovelty(const Observation& observation) const;
    
    // State management
    void updateState(TimestepDuration dt);
    void reset();
    
    // Accessors
    float getNoveltyLevel() const;
    float getPatternDeviation() const;
    float getPredictionSurprise() const;
    float getSensoryNovelty() const;
    float getCognitiveNovelty() const;
    float getTemporalNovelty() const;
    float getSpatialNovelty() const;
    float getSocialNovelty() const;
    float getAdaptiveThreshold() const;
    
    // Configuration
    void setNoveltyDecayRate(float rate) { pImpl->state.noveltyDecayRate = rate; }
    void setAdaptationRate(float rate) { pImpl->state.adaptationRate = rate; }
    void setMemoryConsolidation(float consolidation) { pImpl->state.memoryConsolidation = consolidation; }
    void enableCrossModalIntegration(bool enable) { pImpl->enableCrossModalIntegration_ = enable; }
    void enableNoveltyAmplification(bool enable) { pImpl->enableNoveltyAmplification_ = enable; }
    
    // Integration with other systems
    void integrateWithDopamine(float dopamineLevel);
    void integrateWithCuriosity(float curiosityLevel);
    void integrateWithPredictionError(float predictionError);
    void integrateWithBrainPatterns(const std::vector<float>& pattern);
    
    // Novelty amplification
    void amplifyNovelty(float factor);
    void attenuateNovelty(float factor);
    void applyNoveltyFilter(std::vector<float>& features);
    
    // Pattern similarity computation
    float computePatternSimilarity(const std::vector<float>& pattern1,
                                  const std::vector<float>& pattern2) const;
    std::vector<float> computePatternFeatures(const Observation& observation) const;
    void updatePatternDatabase(const std::vector<float>& pattern);
    
    // History and statistics
    const std::vector<float>& getNoveltyHistory() const;
    const std::vector<float>& getPatternDeviationHistory() const;
    const std::vector<Pattern>& getPatternDatabase() const;
    void clearHistory();
    
    // Safety and validation
    bool validateSafetyConstraints() const;
    void enforceBounds();
    
    // Export/import
    void exportState(std::ostream& stream) const;
    void importState(std::istream& stream);
    
    // Testing interface
    float getSensoryNoveltyValue() const { return pImpl->state.sensoryNovelty; }
    float getCognitiveNoveltyValue() const { return pImpl->state.cognitiveNovelty; }
    float getPatternDatabaseSize() const { return static_cast<float>(pImpl->patternDatabase.size()); }
    
private:
    struct Impl;
    Impl* pImpl;
    
    std::shared_ptr<Brain> brain_;
    std::shared_ptr<NeuromodulationEngine> engine_;
    
    std::unordered_map<size_t, Pattern> patternDatabase_;
    std::vector<size_t> recentPatterns_;
    
    bool enableCrossModalIntegration_;
    bool enableNoveltyAmplification_;
    
    // Pattern feature computation helpers
    std::vector<float> computeStatisticalFeatures(const std::vector<float>& pattern) const;
    std::vector<float> computeTemporalFeatures(const Observation& observation) const;
    std::vector<float> computeSpatialFeatures(const Observation& observation) const;
    std::vector<float> computeSocialFeatures(const Observation& observation) const;
};

} // namespace nlm