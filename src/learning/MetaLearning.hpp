#ifndef NLM_META_LEARNING_HPP
#define NLM_META_LEARNING_HPP

#include "../brain/Brain.hpp"
#include "../core/Config/Constants.hpp"
#include <memory>
#include <vector>
#include <random>
#include <algorithm>
#include <numeric>

namespace nlm {

// MetaLearning: Implements meta-learning capabilities for fast adaptation
// Enables the brain to learn how to learn and adapt quickly to new tasks
class MetaLearning {
public:
    /**
     * @brief Constructor - creates meta-learning system for fast adaptation
     * 
     * @param brain Shared pointer to the NLM brain to enhance
     * @param adaptationRate Rate of meta-learning (0.0 = slow, 1.0 = fast adaptation)
     * @param memorySize Size of meta-learning memory buffer
     * @note Enables rapid task switching and few-shot learning capabilities
     */
    MetaLearning(std::shared_ptr<Brain> brain, float adaptationRate = 0.3f, size_t memorySize = 100);
    
    /**
     * @brief Destructor - cleans up meta-learning resources
     */
    ~MetaLearning();
    
    /**
     * @brief Initialize meta-learning system
     * 
     * @param brain Shared pointer to the NLM brain
     * @param config Configuration parameters for meta-learning
     */
    void initialize(std::shared_ptr<Brain> brain, const std::shared_ptr<Config>& config);
    
    /**
     * @brief Adapt meta-knowledge based on experience
     * 
     * @param experience Vector of past experiences (tasks, rewards, outcomes)
     * @note Updates meta-knowledge based on accumulated experience
     */
    void adaptMetaKnowledge(const std::vector<float>& experience);
    
    /**
     * @brief Generate meta-strategy for new task based on similar past experiences
     * 
     * @param taskContext Context of the new task to learn
     * @return Vector representing the meta-strategy to apply
     * @note Implements similarity-based transfer learning
     */
    std::vector<float> generateMetaStrategy(const std::vector<float>& taskContext);
    
    /**
     * @brief Update meta-knowledge with new experience
     * 
     * @param experience New experience to incorporate
     * @param outcome Outcome of the experience (reward, success/failure)
     */
    void updateMetaKnowledge(const std::vector<float>& experience, float outcome);
    
    /**
     * @brief Get current meta-knowledge state
     * 
     * @return Vector representing current meta-knowledge
     */
    std::vector<float> getMetaKnowledge() const { return metaKnowledge_; }
    
    /**
     * @brief Get meta-learning performance metrics
     * 
     * @return Vector of performance metrics (accuracy, adaptation speed, etc.)
     */
    std::vector<float> getPerformanceMetrics() const;
    
    /**
     * @brief Reset meta-learning to initial state
     */
    void reset();
    
    /**
     * @brief Get meta-learning description
     * 
     * @return String description of meta-learning capabilities
     */
    std::string getDescription() const;
    
    /**
     * @brief Get number of learned meta-strategies
     * 
     * @return Count of stored meta-strategies
     */
    size_t getStrategyCount() const { return metaMemory_.size(); }
    
    /**
     * @brief Get adaptation rate
     * 
     * @return Current adaptation rate
     */
    float getAdaptationRate() const { return adaptationRate_; }
    
    /**
     * @brief Set adaptation rate
     * 
     * @param rate New adaptation rate (0.0 to 1.0)
     */
    void setAdaptationRate(float rate) { adaptationRate_ = std::clamp(rate, 0.0f, 1.0f); }
    
    /**
     * @brief Check if meta-learning is active
     * 
     * @return True if meta-learning is enabled
     */
    bool isEnabled() const { return enabled_; }
    
    /**
     * @brief Enable or disable meta-learning
     * 
     * @param enable Whether to enable meta-learning
     */
    void enable(bool enable) { enabled_ = enable; }
    
    /**
     * @brief Get meta-learning contribution to plasticity
     * 
     * @param taskContext Context of the current task
     * @return Meta-learning influence on plasticity (0.0 to 1.0)
     */
    float getMetaLearningInfluence(const std::vector<float>& taskContext);
    
    /**
     * @brief Record meta-learning outcome
     * 
     * @param taskContext Task context
     * @param strategy Strategy used
     * @param outcome Outcome achieved
     * @param predictionError Prediction error from the outcome
     */
    void recordMetaOutcome(const std::vector<float>& taskContext, const std::vector<float>& strategy, 
                          float outcome, float predictionError);
    
    /**
     * @brief Get meta-learning confidence in current strategy
     * 
     * @param taskContext Task context
     * @return Confidence level (0.0 to 1.0)
     */
    float getStrategyConfidence(const std::vector<float>& taskContext);
    
    /**
     * @brief Optimize meta-knowledge using gradient-based learning
     * 
     * @param learningRate Learning rate for optimization
     */
    void optimizeMetaKnowledge(float learningRate = 0.01f);
    
private:
    /**
     * @brief Initialize meta-learning memory
     */
    void initializeMetaMemory();
    
    /**
     * @brief Calculate similarity between two experiences
     * 
     * @param exp1 First experience vector
     * @param exp2 Second experience vector
     * @return Similarity score (0.0 to 1.0)
     */
    float calculateExperienceSimilarity(const std::vector<float>& exp1, const std::vector<float>& exp2);
    
    /**
     * @brief Find similar experiences in meta-memory
     * 
     * @param experience New experience to match
     * @param k Number of similar experiences to return
     * @return Vector of indices of similar experiences
     */
    std::vector<size_t> findSimilarExperiences(const std::vector<float>& experience, size_t k = 5);
    
    /**
     * @brief Generate meta-strategy from similar experiences
     * 
     * @param similarIndices Indices of similar experiences
     * @param outcomes Outcomes of similar experiences
     * @return Generated meta-strategy
     */
    std::vector<float> generateStrategyFromSimilar(const std::vector<size_t>& similarIndices, 
                                                  const std::vector<float>& outcomes);
    
    /**
     * @brief Apply regularization to prevent overfitting
     */
    void applyRegularization();
    
    /**
     * @brief Normalize meta-knowledge vector
     */
    void normalizeMetaKnowledge();
    
    /**
     * @brief Update performance metrics
     */
    void updatePerformanceMetrics();
    
    /**
     * @brief Prune old meta-knowledge
     */
    void pruneOldMetaKnowledge();
    
    /**
     * @brief Apply meta-learning to plasticity
     */
    void applyToPlasticity();
    
    // Core meta-learning components
    std::shared_ptr<Brain> brain_;
    std::unique_ptr<Config> config_;
    
    // Meta-knowledge state
    std::vector<float> metaKnowledge_;
    std::vector<std::vector<float>> metaMemory_;  // Store experiences
    std::vector<float> metaOutcomes_;            // Store outcomes
    std::vector<float> metaPredictions_;         // Store predictions
    
    // Performance tracking
    std::vector<float> performanceMetrics_;
    float currentAccuracy_;
    float adaptationSpeed_;
    float transferEfficiency_;
    
    // Configuration
    float adaptationRate_;
    size_t memorySize_;
    size_t maxStrategies_;
    float regularizationStrength_;
    float confidenceThreshold_;
    
    // State
    bool enabled_;
    bool learningEnabled_;
    size_t experienceCount_;
    size_t successfulAdaptations_;
    size_t totalAdaptations_;
    
    // Random generation
    std::mt19937 rng_;
    std::uniform_real_distribution<float> uniformDist_;
    std::normal_distribution<float> normalDist_;
    
    // For similarity calculations
    std::vector<std::vector<float>> experienceMatrix_;  // Store all experiences
    std::vector<std::vector<float>> strategyMatrix_;    // Store strategies
};

} // namespace nlm

#endif // NLM_META_LEARNING_HPP
