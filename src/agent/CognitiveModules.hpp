// CognitiveModules.hpp - Extracts cognitive system logic from AgentBrain
#pragma once

#include "../src/brain/Brain.hpp"
#include "../src/agent/AgentBrain.hpp"
#include <string>
#include <vector>
#include <memory>

namespace nlm {

// Cognitive module system extracted from AgentBrain
class CognitiveModules {
public:
    // Configuration for cognitive modules
    struct Config {
        // Planner configuration
        float plannerActionQualityThreshold = 0.05f;  // Quality above which action is considered learned
        float plannerQualityGapThreshold = 0.15f;     // Minimum quality difference to apply bias
        
        // Concept formation configuration
        float conceptPatternThreshold = 0.01f;       // Threshold for considering patterns different enough for concepts
        float conceptRewardThreshold = 0.1f;         // Strong positive reward threshold
        float conceptNegativeThreshold = -0.1f;       // Strong negative reward threshold
        
        // Self-model configuration
        float selfModelConfidenceThreshold = 0.8f;   // Minimum confidence to consider prediction reliable
        float selfModelConfidenceGap = 0.05f;        // Minimum confidence difference to apply bias
        
        // Social learning configuration
        float socialObservationThreshold = 10;        // Number of observations needed to start social learning
        float socialSimilarityThreshold = 0.8f;       // Similarity needed to consider actions for imitation
        
        // Development configuration
        float developmentStageInitialAge = 60.0;       // Age at Initial stage (~1 minute)
        float developmentStageCriticalAge = 300.0;     // Age at Critical Period (~5 minutes)
        float developmentStageMaturationAge = 900.0;   // Age at Maturation (~15 minutes)
    };
    
    CognitiveModules() = default;
    CognitiveModules(const Config& config) : config_(config) {}
    
    // Planner interface for action planning
    ActionType planAction(const std::vector<float>& vision,
                         const std::shared_ptr<Brain>& brain,
                         float noveltyLevel,
                         float predictionError);
    
    // Concept formation interface
    void presentExperience(const std::vector<float>& pattern,
                          const std::vector<float>& touch,
                          float reward,
                          const std::shared_ptr<Brain>& brain);
    
    size_t getConceptCount(const std::shared_ptr<Brain>& brain) const;
    
    // Self-model interface
    void recordSelfAction(ActionType action,
                         const std::vector<float>& beforeVision,
                         const std::vector<float>& afterVision);
    
    float getSelfModelConfidence(ActionType action) const;
    float getSelfModelConfidence() const;  // Overall confidence
    
    // Social learning interface
    void observeAgentAction(ActionType action,
                          const std::vector<float>& beforeVision,
                          const std::vector<float>& afterVision);
    
    size_t getObservationCount() const;
    
    std::pair<ActionType, float> getImitationActionAndSimilarity(const std::vector<float>& currentVision) const;
    
    // Development interface
    void updateDevelopment(AgentBrain& agent, double timestep);
    
    DevelopmentalStage getDevelopmentalStage(const std::shared_ptr<Brain>& brain) const;
    
    // Get current configuration
    const Config& getConfig() const { return config_; }
    
    // Set configuration
    void setConfig(const Config& config) { config_ = config; }
    
private:
    Config config_;
    
    // Self-model internal state
    struct SelfModel {
        struct ActionRecord {
            ActionType action;
            std::vector<float> beforeVision;
            std::vector<float> afterVision;
            float confidence;
        };
        
        std::vector<ActionRecord> records;
        bool initialized = false;
        std::shared_ptr<Brain> brain;
        
        void initialize(std::shared_ptr<Brain> b);
        void record(ActionType action,
                   const std::vector<float>& beforeVision,
                   const std::vector<float>& afterVision);
        float getConfidence(ActionType action) const;
        float getOverallConfidence() const;
        ActionType getMostConfidentAction() const;
        float getConfidenceGap() const;
    } selfModel_;
    
    // Social learning internal state
    struct SocialLearning {
        struct Observation {
            ActionType action;
            std::vector<float> beforeVision;
            std::vector<float> afterVision;
            double timestamp;
        };
        
        std::vector<Observation> observations;
        bool initialized = false;
        std::shared_ptr<Brain> brain;
        
        void initialize(std::shared_ptr<Brain> b);
        void addObservation(ActionType action,
                          const std::vector<float>& beforeVision,
                          const std::vector<float>& afterVision);
        size_t getObservationCount() const { return observations.size(); }
        std::pair<ActionType, float> getImitationActionAndSimilarity(const std::vector<float>& currentVision) const;
    } socialLearning_;
    
    // Concept formation internal state (simplified)
    struct ConceptSystem {
        struct Concept {
            std::vector<float> pattern;
            std::vector<float> rewards;
            float avgReward = 0.0f;
        };
        
        std::vector<Concept> concepts;
        std::shared_ptr<Brain> brain;
        
        void initialize(std::shared_ptr<Brain> b);
        void presentExperience(const std::vector<float>& pattern,
                              const std::vector<float>& touch,
                              float reward);
        size_t getConceptCount() const { return concepts.size(); }
        Concept* getConcept(size_t index) { return index < concepts.size() ? &concepts[index] : nullptr; }
        size_t getMatchingConcept(const std::vector<float>& pattern) const;
    } conceptSystem_;
};

} // namespace nlm
