#pragma once

#include "../core/Types/Types.hpp"
#include "../brain/Synapse.hpp"
#include "../plasticity/PlasticityRule.hpp"
#include "../neuromodulation/Neuromodulator.hpp"
#include "../brain/Brain.hpp"
#include <vector>
#include <memory>
#include <string>
#include <map>
#include <unordered_set>

namespace nlm {

// Meta-learning core algorithms
class MAML;
class LSTMMetaLearner;
class MetagradientCalculator;
class TaskEmbedding;

// Meta-learning framework
class MetaLearning {
public:
    MetaLearning();
    ~MetaLearning();
    
    // Initialize meta-learning with brain reference
    void initialize(Brain* brain);
    
    // Meta-learning update based on task experience
    void update(const std::vector<Synapse*>& synapses, float taskLoss, float validationLoss, float learningRate);
    
    // Apply meta-gradient to synaptic plasticity
    void applyMetaGradient(const std::vector<Synapse*>& synapses, const std::vector<float>& gradients);
    
    // Register a new task for meta-learning
    void registerTask(const std::string& taskId, const std::vector<float>& taskFeatures);
    
    // Compute similarity between tasks
    float computeTaskSimilarity(const std::string& taskId1, const std::string& taskId2) const;
    
    // Get meta-learner for a specific task
    MAML* getMAML(const std::string& taskId);
    LSTMMetaLearner* getLSTMMetaLearner(const std::string& taskId);
    
    // Set meta-learning parameters
    void setMetaLearningRate(float rate) { metaLearningRate_ = rate; }
    void setMetaLearningDecay(float decay) { metaLearningDecay_ = decay; }
    void setTaskEmbeddingDim(size_t dim) { taskEmbeddingDim_ = dim; }
    
    // Get meta-learning statistics
    size_t getTaskCount() const { return taskEmbeddings_.size(); }
    float getMetaLearningRate() const { return metaLearningRate_; }
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Task management
    std::map<std::string, std::vector<float>> taskEmbeddings_;
    std::map<std::string, float> taskSimilarityMatrix_;
    
    // Meta-learning parameters
    float metaLearningRate_;
    float metaLearningDecay_;
    size_t taskEmbeddingDim_;
    bool initialized_;
};

// MAML (Model-Agnostic Meta-Learning) implementation
class MAML {
public:
    MAML();
    ~MAML();
    
    // Update parameters based on inner loop adaptation
    void update(const std::vector<Synapse*>& synapses, const std::vector<float>& innerGradients, float metaGradient);
    
    // Compute meta-gradient for fast adaptation
    std::vector<float> computeMetaGradient(const std::vector<Synapse*>& synapses, const std::vector<float>& taskLosses);
    
    // Create inner loop adaptation weights
    std::vector<float> createInnerLoopWeights(const std::vector<float>& originalWeights, float alpha = 0.01f);
    
    // Set MAML parameters
    void setInnerLoopSteps(int steps) { innerLoopSteps_ = steps; }
    void setMetaStepSize(float alpha) { metaStepSize_ = alpha; }
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    int innerLoopSteps_;
    float metaStepSize_;
};

// LSTM meta-learner for optimization
class LSTMMetaLearner {
public:
    LSTMMetaLearner();
    ~LSTMMetaLearner();
    
    // Update LSTM state based on optimization history
    void update(const std::vector<float>& optimizationHistory, float metaGradient);
    
    // Predict optimal learning rate for next step
    float predictLearningRate(const std::vector<float>& optimizationHistory);
    
    // Get hidden state
    const std::vector<float>& getHiddenState() const { return hiddenState_; }
    
    // Reset LSTM state
    void reset();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    std::vector<float> hiddenState_;
    std::vector<float> cellState_;
    float learningRate_;
};

// Metagradient calculator for fast adaptation
class MetagradientCalculator {
public:
    MetagradientCalculator();
    ~MetagradientCalculator();
    
    // Compute metagradient using second-order derivatives
    std::vector<float> computeMetagradient(const std::vector<Synapse*>& synapses, const std::vector<float>& taskLosses);
    
    // Approximate metagradient using finite differences
    std::vector<float> approximateMetagradient(const std::vector<Synapse*>& synapses, float epsilon = 1e-4f);
    
    // Set metagradient parameters
    void setHessianApproximation(bool approx) { useHessianApprox_ = approx; }
    void setMetaGradientScale(float scale) { metaGradientScale_ = scale; }
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    bool useHessianApprox_;
    float metaGradientScale_;
};

// Task embedding and similarity learning
class TaskEmbedding {
public:
    TaskEmbedding();
    ~TaskEmbedding();
    
    // Learn task embedding from experience
    void learn(const std::string& taskId, const std::vector<float>& features, const std::vector<float>& rewards);
    
    // Compute task similarity based on embedding distance
    float computeSimilarity(const std::string& taskId1, const std::string& taskId2) const;
    
    // Get task embedding
    std::vector<float> getTaskEmbedding(const std::string& taskId) const;
    
    // Get most similar tasks
    std::vector<std::pair<std::string, float>> getSimilarTasks(const std::string& taskId, size_t topK = 5) const;
    
    // Set embedding parameters
    void setEmbeddingDim(size_t dim) { embeddingDim_ = dim; }
    void setSimilarityMethod(const std::string& method) { similarityMethod_ = method; }
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    size_t embeddingDim_;
    std::string similarityMethod_;
    std::map<std::string, std::vector<float>> embeddings_;
};

// Metaplasticity - adaptive learning rate mechanisms
class MetaPlasticity : public PlasticityRule {
public:
    MetaPlasticity();
    ~MetaPlasticity() override;
    
    // Update with meta-plastic updates
    void update(Synapse* synapse,
                const std::vector<Timestamp>& preSpikes,
                const std::vector<Timestamp>& postSpikes,
                TimestepDuration dt) override;
    
    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override;
    const char* getName() const override { return "MetaPlasticity"; }
    
    // Set metaplasticity parameters
    void setAdaptiveLearningRate(bool adaptive) { adaptiveLearningRate_ = adaptive; }
    void setMetaPlasticityRate(float rate) { metaPlasticityRate_ = rate; }
    void setPlasticityHistorySize(size_t size) { plasticityHistorySize_ = size; }
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    bool adaptiveLearningRate_;
    float metaPlasticityRate_;
    size_t plasticityHistorySize_;
    std::map<NeuronId, std::vector<float>> learningRateHistory_;
};

// Auto-curiosity system for intrinsic motivation
class AutoCuriosity {
public:
    AutoCuriosity();
    ~AutoCuriosity();
    
    // Update curiosity based on prediction error and novelty
    float update(float predictionError, float novelty, TimestepDuration dt);
    
    // Get current curiosity level
    float getLevel() const { return curiosityLevel_; }
    
    // Set curiosity parameters
    void setCuriosityGain(float gain) { curiosityGain_ = gain; }
    void setExplorationRate(float rate) { explorationRate_ = rate; }
    void setIntrinsicRewardWeight(float weight) { intrinsicRewardWeight_ = weight; }
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    float curiosityLevel_;
    float curiosityGain_;
    float explorationRate_;
    float intrinsicRewardWeight_;
    std::vector<float> predictionErrorHistory_;
    std::vector<float> noveltyHistory_;
};

// Task inference and generalization
class TaskInference {
public:
    TaskInference();
    ~TaskInference();
    
    // Infer task from experience
    std::string inferTask(const std::vector<float>& experience) const;
    
    // Learn task parameters from experience
    void learnTask(const std::string& taskId, const std::vector<float>& experience, float reward);
    
    // Predict performance on new task
    float predictPerformance(const std::string& taskId, const std::vector<float>& experience) const;
    
    // Get task statistics
    float getTaskSuccessRate(const std::string& taskId) const;
    float getTaskGeneralizationScore(const std::string& sourceTask, const std::string& targetTask) const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Learning-to-learn algorithms
class LearningToLearn {
public:
    LearningToLearn();
    ~LearningToLearn();
    
    // Optimize learning rate using meta-learning
    float optimizeLearningRate(const std::vector<float>& lossHistory, const std::vector<float>& gradientHistory);
    
    // Learn optimization algorithm using meta-learning
    void learnOptimizationAlgorithm(const std::vector<float>& optimizationHistory);
    
    // Apply learned optimization
    float applyOptimizedUpdate(float currentParam, float gradient, const std::vector<float>& optimizationHistory);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm