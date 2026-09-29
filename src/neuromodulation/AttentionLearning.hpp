#ifndef ATTENTION_LEARNING_HPP
#define ATTENTION_LEARNING_HPP

#include <vector>
#include <string>

namespace nlm {

class Brain;

class AttentionLearning {
public:
    AttentionLearning();
    ~AttentionLearning();
    
    void initialize(Brain* brain);
    void update(const std::vector<float>& currentState,
                const std::vector<float>& goals,
                const std::vector<float>& context,
                std::vector<float>& attentionWeights);
    void updatePrediction(const std::vector<float>& currentState,
                         const std::vector<float>& predictedState,
                         std::vector<float>& predictionError);
    void updateFeatureAllocation(const std::vector<float>& features,
                                 const std::vector<float>& goals);
    
    // Configuration methods
    void setTopDownControlEnabled(bool enabled);
    void setBottomUpControlEnabled(bool enabled);
    void setFeatureBasedControlEnabled(bool enabled);
    void setPredictionBasedControlEnabled(bool enabled);
    void setAttentionThreshold(float threshold);
    
    // Status and control
    void reset();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Configuration flags
    bool topDownControlEnabled_;
    bool bottomUpControlEnabled_;
    bool featureBasedControlEnabled_;
    bool predictionBasedControlEnabled_;
    float attentionThreshold_;
};

// Internal model classes (for implementation)
class PredictiveCoding;
class SelectiveAttention;
class FeatureAttention;

} // namespace nlm

#endif // ATTENTION_LEARNING_HPP
