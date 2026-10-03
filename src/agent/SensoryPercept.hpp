#pragma once

#include "AgentBody.hpp"
#include "../sensory/SensoryInput.hpp"
#include <memory>
#include <vector>
#include <unordered_map>

namespace nlm {

// Forward declarations for cognitive systems
class PredictionSystem;
class ConceptFormation;
class AttentionalSelection;
class NeuralPlanner;

// Sensory data packet - what the agent perceives
// Contains low-level sensory signals plus cognitive context for integration
class SensoryPercept {
public:
    SensoryPercept();
    ~SensoryPercept() = default;
    
    // Vision: simple grayscale intensity grid (no RGB, no object labels)
    // Format: row-major, values 0.0-1.0
    const std::vector<float>& getVision() const { return vision_; }
    void setVision(const std::vector<float>& v) { vision_ = v; }
    size_t getVisionWidth() const { return visionWidth_; }
    size_t getVisionHeight() const { return visionHeight_; }
    
    // Touch/proximity sensors (binary collision signals)
    const std::vector<float>& getTouch() const { return touch_; }
    void setTouch(const std::vector<float>& t) { touch_ = t; }
    
    // Internal state signals (homeostatic signals)
    const std::vector<float>& getInternal() const { return internal_; }
    void setInternal(const std::vector<float>& i) { internal_ = i; }
    
    // Proprioception: body position/velocity signals
    const std::vector<float>& getProprioception() const { return proprioception_; }
    void setProprioception(const std::vector<float>& p) { proprioception_ = p; }
    
    // Audio-like signals (if enabled)
    const std::vector<float>& getAudio() const { return audio_; }
    void setAudio(const std::vector<float>& a) { audio_ = a; }
    
    // Get all signals concatenated (for brain input)
    std::vector<float> getAllSignals() const;
    
    // Timestamp
    double getTimestamp() const { return timestamp_; }
    void setTimestamp(double t) { timestamp_ = t; }
    
    // ========== COGNITIVE CONTEXT ==========
    
    // Store prediction error for this sensory input
    void setPredictionError(float error) { predictionError_ = error; }
    float getPredictionError() const { return predictionError_; }
    
    // Store novelty level for this percept
    void setNoveltyLevel(float novelty) { noveltyLevel_ = novelty; }
    float getNoveltyLevel() const { return noveltyLevel_; }
    
    // Store attention weight for this sensory input
    void setAttentionWeight(float weight) { attentionWeight_ = weight; }
    float getAttentionWeight() const { return attentionWeight_; }
    
    // Store concept relevance scores
    void setConceptRelevance(const std::unordered_map<size_t, float>& relevance) { 
        conceptRelevance_ = relevance; }
    float getConceptRelevance(size_t conceptId) const {
        auto it = conceptRelevance_.find(conceptId);
        return (it != conceptRelevance_.end()) ? it->second : 0.0f;
    }
    
    // Store planning confidence for this input
    void setPlanningConfidence(float confidence) { planningConfidence_ = confidence; }
    float getPlanningConfidence() const { return planningConfidence_; }
    
    // Store working memory activation for this percept
    void setWorkingMemoryActivation(float activation) { workingMemoryActivation_ = activation; }
    float getWorkingMemoryActivation() const { return workingMemoryActivation_; }
    
    // Check if this percept should be prioritized
    bool shouldPrioritize() const {
        return (noveltyLevel_ > 0.7f) || 
               (predictionError_ > 0.5f) || 
               (attentionWeight_ > 0.8f) ||
               (workingMemoryActivation_ > 0.9f);
    }
    
    // Get combined priority score
    float getPriorityScore() const {
        return (noveltyLevel_ * 0.3f) + 
               (std::abs(predictionError_) * 0.3f) + 
               (attentionWeight_ * 0.2f) +
               (workingMemoryActivation_ * 0.2f);
    }
    
    // Update cognitive context based on Brain's cognitive systems
    void updateFromCognitiveSystems(PredictionSystem* predictionSystem,
                                     ConceptFormation* conceptFormation,
                                     AttentionalSelection* attention,
                                     NeuralPlanner* planner);
    
private:
    std::vector<float> vision_;
    size_t visionWidth_;
    size_t visionHeight_;
    std::vector<float> touch_;
    std::vector<float> internal_;
    std::vector<float> proprioception_;
    std::vector<float> audio_;
    double timestamp_;
    
    // Cognitive context for integration
    float predictionError_;
    float noveltyLevel_;
    float attentionWeight_;
    float planningConfidence_;
    float workingMemoryActivation_;
    std::unordered_map<size_t, float> conceptRelevance_;
};

} // namespace nlm
