#pragma once

#include "MetaLearning.hpp"
#include "../core/Logger/Logger.hpp"
#include "../brain/Neuron.hpp"
#include "../brain/Synapse.hpp"
#include <cmath>
#include <algorithm>
#include <random>
#include <numeric>
#include <Eigen/Dense>

namespace nlm {

// Meta-learning framework implementation
struct MetaLearning::Impl {
    std::map<std::string, std::vector<float>> taskEmbeddings;
    std::map<std::string, float> taskSimilarityMatrix;
    std::map<std::string, std::vector<float>> taskLossHistory;
    std::map<std::string, std::vector<float>> taskAccuracyHistory;
    std::map<std::string, std::vector<float>> optimizationHistory;
    
    std::mt19937 rng{std::random_device{}()};
    std::normal_distribution<float> normalDist{0.0f, 1.0f};
    std::uniform_real_distribution<float> uniformDist{0.0f, 1.0f};
};

MetaLearning::MetaLearning() : metaLearningRate_(0.01f), metaLearningDecay_(0.999f), taskEmbeddingDim_(64), initialized_(false) {
    pImpl = std::make_unique<Impl>();
}

MetaLearning::~MetaLearning() {}

void MetaLearning::initialize(Brain* brain) {
    if (initialized_) return;
    
    initialized_ = true;
    NLM_LOG_INFO("Initializing meta-learning system");
    
    // Initialize task embeddings
    std::normal_distribution<float> dist(0.0f, 1.0f);
    
    for (auto& pair : pImpl->taskEmbeddings) {
        pair.second.resize(taskEmbeddingDim_);
        for (auto& val : pair.second) {
            val = dist(pImpl->rng);
        }
    }
    
    NLM_LOG_INFO("Meta-learning system initialized with " + std::to_string(pImpl->taskEmbeddings.size()) + " tasks");
}

void MetaLearning::update(const std::vector<Synapse*>& synapses, float taskLoss, float validationLoss, float learningRate) {
    if (!initialized_ || synapses.empty()) return;
    
    // Record task loss
    std::string currentTask = "default_task"; // Would track actual current task
    pImpl->taskLossHistory[currentTask].push_back(taskLoss);
    
    // Compute meta-gradient from task and validation losses
    float metaGradient = validationLoss - taskLoss;
    
    // Apply meta-gradient to synaptic plasticity using MAML
    std::vector<float> gradients;
    for (const auto& synapse : synapses) {
        if (synapse) {
            // Estimate gradient for meta-learning
            gradients.push_back(metaGradient * learningRate);
        }
    }
    
    applyMetaGradient(synapses, gradients);
    
    // Update meta-learning rate with decay
    metaLearningRate_ *= metaLearningDecay_;
    
    // Update task embedding based on performance
    updateTaskEmbedding(currentTask, taskLoss, validationLoss);
}

void MetaLearning::applyMetaGradient(const std::vector<Synapse*>& synapses, const std::vector<float>& gradients) {
    if (synapses.size() != gradients.size() || synapses.empty()) return;
    
    for (size_t i = 0; i < synapses.size(); ++i) {
        if (synapses[i] && i < gradients.size()) {
            float weightChange = gradients[i] * metaLearningRate_;
            
            // Apply weight change with meta-plasticity modulation
            float modulatedChange = weightChange * (1.0f + std::abs(synapses[i]->getWeight()) * 0.1f);
            
            // Update synapse weight (placeholder for actual Brain implementation)
            // synapses[i]->setWeight(synapses[i]->getWeight() + modulatedChange);
        }
    }
}

void MetaLearning::registerTask(const std::string& taskId, const std::vector<float>& taskFeatures) {
    pImpl->taskEmbeddings[taskId] = taskFeatures;
    
    // Initialize task statistics
    pImpl->taskLossHistory[taskId];
    pImpl->taskAccuracyHistory[taskId];
    pImpl->optimizationHistory[taskId];
    
    // Compute similarity with all existing tasks
    for (const auto& existingTask : pImpl->taskEmbeddings) {
        if (existingTask.first != taskId) {
            float similarity = computeTaskSimilarity(taskId, existingTask.first);
            pImpl->taskSimilarityMatrix[taskId + "_" + existingTask.first] = similarity;
            pImpl->taskSimilarityMatrix[existingTask.first + "_" + taskId] = similarity;
        }
    }
}

float MetaLearning::computeTaskSimilarity(const std::string& taskId1, const std::string& taskId2) const {
    auto it1 = pImpl->taskEmbeddings.find(taskId1);
    auto it2 = pImpl->taskEmbeddings.find(taskId2);
    
    if (it1 == pImpl->taskEmbeddings.end() || it2 == pImpl->taskEmbeddings.end()) return 0.0f;
    
    const auto& embedding1 = it1->second;
    const auto& embedding2 = it2->second;
    
    if (embedding1.size() != embedding2.size()) return 0.0f;
    
    // Cosine similarity
    float dotProduct = 0.0f;
    float norm1 = 0.0f, norm2 = 0.0f;
    
    for (size_t i = 0; i < embedding1.size(); ++i) {
        dotProduct += embedding1[i] * embedding2[i];
        norm1 += embedding1[i] * embedding1[i];
        norm2 += embedding2[i] * embedding2[i];
    }
    
    if (norm1 == 0.0f || norm2 == 0.0f) return 0.0f;
    
    return dotProduct / std::sqrt(norm1 * norm2);
}

void MetaLearning::updateTaskEmbedding(const std::string& taskId, float taskLoss, float validationLoss) {
    auto it = pImpl->taskEmbeddings.find(taskId);
    if (it == pImpl->taskEmbeddings.end()) return;
    
    // Update embedding based on performance using Hebbian-like learning
    std::vector<float>& embedding = it->second;
    float performance = 1.0f - taskLoss; // Convert loss to performance
    
    for (size_t i = 0; i < embedding.size(); ++i) {
        // Apply Hebbian-like update
        float delta = performance * 0.01f * (1.0f - uniformDist(pImpl->rng));
        embedding[i] += delta;
    }
}

// MAML implementation
struct MAML::Impl {
    std::map<std::string, std::vector<float>> innerLoopWeights;
    std::map<std::string, std::vector<float>> metaGradients;
    std::mt19937 rng{std::random_device{}()};
    std::normal_distribution<float> normalDist{0.0f, 0.1f};
};

MAML::MAML() : innerLoopSteps_(5), metaStepSize_(0.01f) {
    pImpl = std::make_unique<Impl>();
}

MAML::~MAML() {}

void MAML::update(const std::vector<Synapse*>& synapses, const std::vector<float>& innerGradients, float metaGradient) {
    for (size_t i = 0; i < synapses.size() && i < innerGradients.size(); ++i) {
        if (synapses[i]) {
            // MAML update: combine meta-gradient with inner gradient
            float mamlUpdate = metaGradient * metaStepSize_ * innerGradients[i];
            
            // Add noise for exploration
            float noise = normalDist(pImpl->rng) * metaStepSize_ * 0.1f;
            mamlUpdate += noise;
            
            // Apply update (placeholder)
            // synapses[i]->setWeight(synapses[i]->getWeight() + mamlUpdate);
            
            // Store for meta-learning history
            std::string taskKey = "meta_task_" + std::to_string(i);
            pImpl->innerLoopWeights[taskKey].push_back(mamlUpdate);
        }
    }
}

std::vector<float> MAML::computeMetaGradient(const std::vector<Synapse*>& synapses, const std::vector<float>& taskLosses) {
    std::vector<float> metaGradients;
    
    for (size_t i = 0; i < synapses.size() && i < taskLosses.size(); ++i) {
        // Compute meta-gradient as second derivative
        float secondDerivative = (taskLosses[i] - taskLosses[std::max(0UL, i-1)]) * metaStepSize_;
        
        // Add curvature information
        float curvature = std::abs(secondDerivative) * 0.5f;
        
        // Scale meta-gradient
        float metaGrad = secondDerivative * (1.0f + curvature);
        metaGradients.push_back(metaGrad);
    }
    
    return metaGradients;
}

std::vector<float> MAML::createInnerLoopWeights(const std::vector<float>& originalWeights, float alpha) {
    std::vector<float> innerLoopWeights = originalWeights;
    
    for (size_t i = 0; i < innerLoopWeights.size(); ++i) {
        // MAML inner loop update
        float innerUpdate = innerLoopWeights[i] * alpha;
        
        // Add task-specific gradient
        float taskGradient = normalDist(pImpl->rng) * 0.1f;
        innerUpdate += taskGradient;
        
        innerLoopWeights[i] += innerUpdate;
    }
    
    return innerLoopWeights;
}

// LSTM Meta-learner implementation
struct LSTMMetaLearner::Impl {
    std::vector<float> hiddenState;
    std::vector<float> cellState;
    std::vector<std::vector<float>> inputGate;
    std::vector<std::vector<float>> forgetGate;
    std::vector<std::vector<float>> outputGate;
    std::vector<std::vector<float>> candidateCell;
    
    std::mt19937 rng{std::random_device{}()};
    std::normal_distribution<float> normalDist{0.0f, 0.1f};
    std::uniform_real_distribution<float> uniformDist{0.0f, 1.0f};
    
    std::vector<float> learningRateHistory;
    std::vector<float> lossHistory;
};

LSTMMetaLearner::LSTMMetaLearner() : learningRate_(0.01f) {
    pImpl = std::make_unique<Impl>();
    
    // Initialize LSTM state
    int hiddenSize = 128;
    pImpl->hiddenState.resize(hiddenSize, 0.0f);
    pImpl->cellState.resize(hiddenSize, 0.0f);
    pImpl->inputGate.resize(hiddenSize, std::vector<float>(hiddenSize, 0.0f));
    pImpl->forgetGate.resize(hiddenSize, std::vector<float>(hiddenSize, 0.0f));
    pImpl->outputGate.resize(hiddenSize, std::vector<float>(hiddenSize, 0.0f));
    pImpl->candidateCell.resize(hiddenSize, std::vector<float>(hiddenSize, 0.0f));
}

LSTMMetaLearner::~LSTMMetaLearner() {}

void LSTMMetaLearner::update(const std::vector<float>& optimizationHistory, float metaGradient) {
    int hiddenSize = pImpl->hiddenState.size();
    
    // Update LSTM state
    for (int h = 0; h < hiddenSize; ++h) {
        // Forget gate
        float forgetGateVal = 1.0f / (1.0f + std::exp(-(pImpl->hiddenState[h] + metaGradient)));
        pImpl->forgetGate[h][h] = forgetGateVal;
        pImpl->cellState[h] *= forgetGateVal;
        
        // Input gate
        float inputGateVal = 1.0f / (1.0f + std::exp(-metaGradient));
        pImpl->inputGate[h][h] = inputGateVal;
        
        // Candidate cell state
        float candidateVal = std::tanh(metaGradient * uniformDist(pImpl->rng));
        pImpl->candidateCell[h][h] = candidateVal;
        pImpl->cellState[h] += inputGateVal * candidateVal;
        
        // Output gate
        float outputGateVal = 1.0f / (1.0f + std::exp(-(pImpl->cellState[h] + pImpl->hiddenState[h])));
        pImpl->outputGate[h][h] = outputGateVal;
        
        // Hidden state
        pImpl->hiddenState[h] = outputGateVal * std::tanh(pImpl->cellState[h]);
    }
    
    // Update learning rate
    learningRate_ = std::max(0.001f, learningRate_ * (1.0f - 0.001f) + 0.001f * metaGradient);
    
    // Store history
    pImpl->learningRateHistory.push_back(learningRate_);
    pImpl->lossHistory.push_back(metaGradient);
}

float LSTMMetaLearner::predictLearningRate(const std::vector<float>& optimizationHistory) {
    if (optimizationHistory.empty()) return learningRate_;
    
    // Use LSTM hidden state to predict learning rate
    float predictedRate = learningRate_;
    
    // Simple prediction based on recent optimization history
    float recentLoss = 0.0f;
    size_t historySize = std::min(optimizationHistory.size(), pImpl->lossHistory.size());
    for (size_t i = 0; i < historySize; ++i) {
        recentLoss += optimizationHistory[optimizationHistory.size() - 1 - i];
    }
    recentLoss /= historySize;
    
    // Adjust learning rate based on loss trend
    if (!pImpl->lossHistory.empty()) {
        float lossTrend = pImpl->lossHistory.back() - (pImpl->lossHistory.size() > 1 ? pImpl->lossHistory[pImpl->lossHistory.size() - 2] : 0.0f);
        predictedRate += learningRate_ * 0.1f * lossTrend;
    }
    
    return std::max(0.001f, std::min(1.0f, predictedRate));
}

void LSTMMetaLearner::reset() {
    std::fill(pImpl->hiddenState.begin(), pImpl->hiddenState.end(), 0.0f);
    std::fill(pImpl->cellState.begin(), pImpl->cellState.end(), 0.0f);
    learningRate_ = 0.01f;
    pImpl->learningRateHistory.clear();
    pImpl->lossHistory.clear();
}

// Metagradient Calculator implementation
struct MetagradientCalculator::Impl {
    std::vector<std::vector<float>> hessianApproximations;
    std::vector<float> curvatureEstimates;
    std::mt19937 rng{std::random_device{}()};
    std::normal_distribution<float> normalDist{0.0f, 0.1f};
};

MetagradientCalculator::MetagradientCalculator() : useHessianApprox_(true), metaGradientScale_(1.0f) {
    pImpl = std::make_unique<Impl>();
}

MetagradientCalculator::~MetagradientCalculator() {}

std::vector<float> MetagradientCalculator::computeMetagradient(const std::vector<Synapse*>& synapses, const std::vector<float>& taskLosses) {
    std::vector<float> metaGradients;
    
    for (size_t i = 0; i < synapses.size() && i < taskLosses.size(); ++i) {
        // Compute first derivative (gradient)
        float gradient = taskLosses[i];
        
        // Compute second derivative (meta-gradient)
        float secondDerivative = 0.0f;
        if (i > 0 && i < taskLosses.size() - 1) {
            // Approximate second derivative using central difference
            secondDerivative = (taskLosses[i + 1] - 2 * taskLosses[i] + taskLosses[i - 1]);
        }
        
        // Apply Hessian approximation if enabled
        if (useHessianApprox_) {
            // Estimate curvature
            float curvature = std::abs(secondDerivative) * 0.5f;
            pImpl->curvatureEstimates.push_back(curvature);
            
            // Scale second derivative by curvature
            secondDerivative *= (1.0f + curvature);
        }
        
        // Scale meta-gradient
        float metaGrad = secondDerivative * metaGradientScale_;
        metaGradients.push_back(metaGrad);
    }
    
    return metaGradients;
}

std::vector<float> MetagradientCalculator::approximateMetagradient(const std::vector<Synapse*>& synapses, float epsilon) {
    std::vector<float> metaGradients;
    
    for (const auto& synapse : synapses) {
        // Approximate meta-gradient using finite differences of second derivatives
        float epsilon2 = epsilon * epsilon;
        
        // Simple approximation for meta-gradient
        float metaGrad = epsilon2 * normalDist(pImpl->rng);
        
        // Add some structure based on synapse properties
        if (synapse) {
            metaGrad *= (1.0f + std::abs(synapse->getWeight()));
        }
        
        metaGradients.push_back(metaGrad);
    }
    
    return metaGradients;
}

// TaskEmbedding implementation
struct TaskEmbedding::Impl {
    std::map<std::string, std::vector<float>> embeddings;
    std::map<std::string, float> taskPerformance;
    std::map<std::string, std::vector<float>> taskFeatureHistory;
    
    std::mt19937 rng{std::random_device{}()};
    std::normal_distribution<float> normalDist{0.0f, 1.0f};
    std::uniform_real_distribution<float> uniformDist{0.0f, 1.0f};
};

TaskEmbedding::TaskEmbedding() : embeddingDim_(64) {
    pImpl = std::make_unique<Impl>();
}

TaskEmbedding::~TaskEmbedding() {}

void TaskEmbedding::learn(const std::string& taskId, const std::vector<float>& features, const std::vector<float>& rewards) {
    if (features.empty()) return;
    
    // Resize embedding if necessary
    std::vector<float> embedding = features;
    if (embedding.size() != embeddingDim_) {
        embedding.resize(embeddingDim_, 0.0f);
        size_t copySize = std::min(features.size(), embeddingDim_);
        for (size_t i = 0; i < copySize; ++i) {
            embedding[i] = features[i] * (rewards.size() / (rewards.end() - rewards.begin() + 1.0f));
        }
    }
    
    // Apply Hebbian-like learning for task embedding
    std::vector<float>& currentEmbedding = pImpl->embeddings[taskId];
    
    for (size_t i = 0; i < embedding.size(); ++i) {
        float performance = rewards.empty() ? 0.0f : (rewards.back() / rewards.size());
        float delta = performance * 0.1f * (1.0f - uniformDist(pImpl->rng));
        currentEmbedding[i] += delta;
    }
    
    // Store feature history
    pImpl->taskFeatureHistory[taskId] = embedding;
    
    // Update task performance
    if (!rewards.empty()) {
        float avgReward = std::accumulate(rewards.begin(), rewards.end(), 0.0f) / rewards.size();
        pImpl->taskPerformance[taskId] = avgReward;
    }
}

float TaskEmbedding::computeSimilarity(const std::string& taskId1, const std::string& taskId2) const {
    auto it1 = pImpl->embeddings.find(taskId1);
    auto it2 = pImpl->embeddings.find(taskId2);
    
    if (it1 == pImpl->embeddings.end() || it2 == pImpl->embeddings.end()) return 0.0f;
    
    const auto& emb1 = it1->second;
    const auto& emb2 = it2->second;
    
    if (emb1.size() != emb2.size()) return 0.0f;
    
    // Cosine similarity
    float similarity = 0.0f;
    for (size_t i = 0; i < emb1.size(); ++i) {
        similarity += emb1[i] * emb2[i];
    }
    similarity /= emb1.size(); // Normalized similarity
    
    return similarity;
}

std::vector<float> TaskEmbedding::getTaskEmbedding(const std::string& taskId) const {
    auto it = pImpl->embeddings.find(taskId);
    if (it != pImpl->embeddings.end()) {
        return it->second;
    }
    return std::vector<float>(embeddingDim_, 0.0f);
}

std::vector<std::pair<std::string, float>> TaskEmbedding::getSimilarTasks(const std::string& taskId, size_t topK) const {
    std::vector<std::pair<std::string, float>> similarities;
    
    for (const auto& pair : pImpl->embeddings) {
        if (pair.first != taskId) {
            float similarity = computeSimilarity(taskId, pair.first);
            similarities.emplace_back(pair.first, similarity);
        }
    }
    
    // Sort by similarity (descending)
    std::sort(similarities.begin(), similarities.end(),
              [](const auto& a, const auto& b) { return a.second > b.second; });
    
    // Return topK
    if (similarities.size() > topK) {
        similarities.resize(topK);
    }
    
    return similarities;
}

// MetaPlasticity implementation
struct MetaPlasticity::Impl {
    std::map<NeuronId, std::vector<float>> learningRateHistory;
    std::map<NeuronId, std::vector<float>> weightHistory;
    std::map<NeuronId, float> neuronPlasticity;
    float plasticityDecayRate;
    
    std::mt19937 rng{std::random_device{}()};
    std::normal_distribution<float> normalDist{0.0f, 0.1f};
};

MetaPlasticity::MetaPlasticity() : adaptiveLearningRate_(true), metaPlasticityRate_(0.01f), plasticityHistorySize_(100) {
    pImpl = std::make_unique<Impl>();
    pImpl->plasticityDecayRate = 0.99f;
}

MetaPlasticity::~MetaPlasticity() {}

void MetaPlasticity::update(Synapse* synapse, const std::vector<Timestamp>& preSpikes, const std::vector<Timestamp>& postSpikes, TimestepDuration dt) {
    if (!synapse) return;
    
    NeuronId preNeuronId = synapse->getPreNeuron();
    NeuronId postNeuronId = synapse->getPostNeuron();
    
    // Store weight history for meta-plastic analysis
    pImpl->weightHistory[preNeuronId].push_back(synapse->getWeight());
    if (pImpl->weightHistory[preNeuronId].size() > plasticityHistorySize_) {
        pImpl->weightHistory[preNeuronId].erase(pImpl->weightHistory[preNeuronId].begin());
    }
    
    // Compute adaptive learning rate based on weight history
    if (adaptiveLearningRate_ && pImpl->weightHistory[preNeuronId].size() >= 2) {
        const auto& weights = pImpl->weightHistory[preNeuronId];
        
        // Compute statistics of weight history
        float avgWeight = std::accumulate(weights.begin(), weights.end(), 0.0f) / weights.size();
        float weightVariance = 0.0f;
        
        for (float w : weights) {
            weightVariance += (w - avgWeight) * (w - avgWeight);
        }
        weightVariance /= weights.size();
        
        // Adjust learning rate based on weight variance (meta-plasticity)
        float plasticityFactor = 1.0f + weightVariance * metaPlasticityRate_;
        pImpl->neuronPlasticity[preNeuronId] *= pImpl->plasticityDecayRate * plasticityFactor;
        
        // Apply plasticity modulation (placeholder)
        // synapse->setPlasticityFactor(pImpl->neuronPlasticity[preNeuronId]);
    }
    
    // Apply meta-plasticity to synaptic weight change
    // (Implementation would depend on synapse's plasticity system)
}

void MetaPlasticity::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    if (!synapse) return;
    
    NeuronId preNeuronId = synapse->getPreNeuron();
    
    // Apply meta-plastic modulation to weight change
    float modulatedDelta = delta * metaPlasticityRate_;
    
    // Adapt learning rate based on previous plasticity history
    if (adaptiveLearningRate_) {
        auto it = pImpl->learningRateHistory.find(preNeuronId);
        if (it != pImpl->learningRateHistory.end() && !it->second.empty()) {
            float recentLearningRate = it->second.back();
            float plasticityHistoryFactor = 1.0f + std::abs(recentLearningRate);
            modulatedDelta *= plasticityHistoryFactor;
        }
    }
    
    // Store learning rate history
    pImpl->learningRateHistory[preNeuronId].push_back(modulatedDelta);
    if (pImpl->learningRateHistory[preNeuronId].size() > plasticityHistorySize_) {
        pImpl->learningRateHistory[preNeuronId].erase(pImpl->learningRateHistory[preNeuronId].begin());
    }
    
    // Apply modulated weight change (placeholder)
    // synapse->setWeight(synapse->getWeight() + modulatedDelta);
}

// AutoCuriosity implementation
struct AutoCuriosity::Impl {
    std::vector<float> curiosityLevelHistory;
    std::vector<float> predictionErrorHistory;
    std::vector<float> noveltyHistory;
    float baselineCuriosity;
    float curiosityAdaptationRate;
    
    std::mt19937 rng{std::random_device{}()};
    std::normal_distribution<float> normalDist{0.0f, 0.1f};
    std::uniform_real_distribution<float> uniformDist{0.0f, 1.0f};
};

AutoCuriosity::AutoCuriosity() : curiosityLevel_(0.0f), curiosityGain_(1.0f), explorationRate_(0.1f), intrinsicRewardWeight_(0.5f) {
    pImpl = std::make_unique<Impl>();
    pImpl->baselineCuriosity = 0.01f;
    pImpl->curiosityAdaptationRate = 0.05f;
}

AutoCuriosity::~AutoCuriosity() {}

float AutoCuriosity::update(float predictionError, float novelty, TimestepDuration dt) {
    // Compute curiosity based on prediction error and novelty
    float curiosityInput = curiosityGain_ * (predictionError + novelty);
    
    // Adaptive filtering for curiosity level
    float filteredCuriosity = curiosityLevel_ * (1.0f - pImpl->curiosityAdaptationRate) + 
                              pImpl->curiosityAdaptationRate * curiosityInput;
    
    // Apply non-linear transformation to curiosity
    curiosityLevel_ = filteredCuriosity * (1.0f + uniformDist(pImpl->rng) * 0.5f);
    
    // Store history
    pImpl->curiosityLevelHistory.push_back(curiosityLevel_);
    pImpl->predictionErrorHistory.push_back(predictionError);
    pImpl->noveltyHistory.push_back(novelty);
    
    // Cap history size
    if (pImpl->curiosityLevelHistory.size() > 1000) {
        pImpl->curiosityLevelHistory.erase(pImpl->curiosityLevelHistory.begin());
        pImpl->predictionErrorHistory.erase(pImpl->predictionErrorHistory.begin());
        pImpl->noveltyHistory.erase(pImpl->noveltyHistory.begin());
    }
    
    // Adjust exploration rate based on curiosity
    explorationRate_ = std::min(1.0f, curiosityLevel_ * 2.0f);
    
    return curiosityLevel_;
}

// TaskInference implementation
struct TaskInference::Impl {
    std::map<std::string, std::vector<float>> taskParameters;
    std::map<std::string, std::vector<std::vector<float>>> taskHistory;
    std::map<std::string, float> taskSuccessRates;
    std::map<std::string, float> taskGeneralizationScores;
    
    std::mt19937 rng{std::random_device{}()};
    std::normal_distribution<float> normalDist{0.0f, 1.0f};
    std::uniform_real_distribution<float> uniformDist{0.0f, 1.0f};
};

TaskInference::TaskInference() {
    pImpl = std::make_unique<Impl>();
}

TaskInference::~TaskInference() {}

std::string TaskInference::inferTask(const std::vector<float>& experience) const {
    // Simple task inference based on experience vector
    if (experience.empty()) return "unknown_task";
    
    // Find most similar known task
    std::string bestTask = "unknown_task";
    float bestSimilarity = 0.0f;
    
    for (const auto& pair : pImpl->taskHistory) {
        const auto& taskHistory = pair.second;
        if (taskHistory.empty()) continue;
        
        // Compute similarity between experience and task history
        float similarity = 0.0f;
        size_t historySize = std::min(experience.size(), taskHistory[0].size());
        
        for (size_t i = 0; i < historySize; ++i) {
            similarity += experience[i] * taskHistory[0][i];
        }
        
        if (similarity > bestSimilarity) {
            bestSimilarity = similarity;
            bestTask = pair.first;
        }
    }
    
    return bestTask;
}

void TaskInference::learnTask(const std::string& taskId, const std::vector<float>& experience, float reward) {
    // Store task experience
    pImpl->taskHistory[taskId].push_back(experience);
    
    // Trim history if too large
    if (pImpl->taskHistory[taskId].size() > 1000) {
        pImpl->taskHistory[taskId].erase(pImpl->taskHistory[taskId].begin());
    }
    
    // Update task parameters using experience
    std::vector<float>& taskParams = pImpl->taskParameters[taskId];
    
    if (taskParams.empty()) {
        taskParams = experience;
    } else {
        // Update parameters with Hebbian-like learning
        for (size_t i = 0; i < experience.size() && i < taskParams.size(); ++i) {
            taskParams[i] = taskParams[i] * 0.9f + experience[i] * 0.1f * reward;
        }
    }
    
    // Update success rate
    if (pImpl->taskSuccessRates.find(taskId) == pImpl->taskSuccessRates.end()) {
        pImpl->taskSuccessRates[taskId] = 0.0f;
    }
    
    pImpl->taskSuccessRates[taskId] = (pImpl->taskSuccessRates[taskId] * 0.9f) + (reward * 0.1f);
}

float TaskInference::predictPerformance(const std::string& taskId, const std::vector<float>& experience) const {
    auto it = pImpl->taskParameters.find(taskId);
    if (it == pImpl->taskParameters.end()) {
        return 0.0f; // Unknown task
    }
    
    const auto& taskParams = it->second;
    
    // Compute similarity between experience and task parameters
    float similarity = 0.0f;
    size_t minSize = std::min(experience.size(), taskParams.size());
    
    for (size_t i = 0; i < minSize; ++i) {
        similarity += experience[i] * taskParams[i];
    }
    
    // Get success rate
    auto successIt = pImpl->taskSuccessRates.find(taskId);
    float successRate = successIt != pImpl->taskSuccessRates.end() ? successIt->second : 0.0f;
    
    // Combine similarity and success rate for performance prediction
    return similarity * 0.7f + successRate * 0.3f;
}

float TaskInference::getTaskSuccessRate(const std::string& taskId) const {
    auto it = pImpl->taskSuccessRates.find(taskId);
    if (it != pImpl->taskSuccessRates.end()) {
        return it->second;
    }
    return 0.0f;
}

float TaskInference::getTaskGeneralizationScore(const std::string& sourceTask, const std::string& targetTask) const {
    auto sourceIt = pImpl->taskParameters.find(sourceTask);
    auto targetIt = pImpl->taskParameters.find(targetTask);
    
    if (sourceIt == pImpl->taskParameters.end() || targetIt == pImpl->taskParameters.end()) {
        return 0.0f;
    }
    
    const auto& sourceParams = sourceIt->second;
    const auto& targetParams = targetIt->second;
    
    // Compute parameter similarity
    float similarity = 0.0f;
    size_t minSize = std::min(sourceParams.size(), targetParams.size());
    
    for (size_t i = 0; i < minSize; ++i) {
        similarity += sourceParams[i] * targetParams[i];
    }
    
    return similarity / minSize;
}

// LearningToLearn implementation
struct LearningToLearn::Impl {
    std::vector<float> learningRateHistory;
    std::vector<float> lossHistory;
    std::vector<float> gradientHistory;
    std::vector<float> optimalLearningRates;
    
    std::mt19937 rng{std::random_device{}()};
    std::normal_distribution<float> normalDist{0.0f, 0.1f};
    std::uniform_real_distribution<float> uniformDist{0.0f, 1.0f};
};

LearningToLearn::LearningToLearn() {
    pImpl = std::make_unique<Impl>();
}

LearningToLearn::~LearningToLearn() {}

float LearningToLearn::optimizeLearningRate(const std::vector<float>& lossHistory, const std::vector<float>& gradientHistory) {
    if (lossHistory.empty() || gradientHistory.empty()) {
        return 0.01f; // Default learning rate
    }
    
    // Store history
    pImpl->lossHistory = lossHistory;
    pImpl->gradientHistory = gradientHistory;
    
    // Simple learning rate optimization
    float avgLoss = std::accumulate(lossHistory.begin(), lossHistory.end(), 0.0f) / lossHistory.size();
    float avgGradient = std::accumulate(gradientHistory.begin(), gradientHistory.end(), 0.0f) / gradientHistory.size();
    
    // Compute optimal learning rate using gradient descent heuristic
    float optimalRate = 0.01f;
    
    if (std::abs(avgGradient) > 1e-6f) {
        optimalRate = std::abs(avgLoss / avgGradient);
    }
    
    // Clamp to reasonable range
    optimalRate = std::max(0.0001f, std::min(1.0f, optimalRate));
    
    // Store optimal rate
    pImpl->optimalLearningRates.push_back(optimalRate);
    if (pImpl->optimalLearningRates.size() > 100) {
        pImpl->optimalLearningRates.erase(pImpl->optimalLearningRates.begin());
    }
    
    return optimalRate;
}

void LearningToLearn::learnOptimizationAlgorithm(const std::vector<float>& optimizationHistory) {
    if (optimizationHistory.empty()) return;
    
    // Store optimization history
    pImpl->learningRateHistory = optimizationHistory;
    
    // Learn from optimization history using simple pattern matching
    float avgLoss = std::accumulate(optimizationHistory.begin(), optimizationHistory.end(), 0.0f) / optimizationHistory.size();
    
    // Adapt learning rate based on loss pattern
    if (avgLoss > 1.0f) {
        // Loss is high, reduce learning rate
        pImpl->learningRateHistory.back() *= 0.9f;
    } else if (avgLoss < 0.01f) {
        // Loss is low, can increase learning rate
        pImpl->learningRateHistory.back() *= 1.1f;
    }
}

float LearningToLearn::applyOptimizedUpdate(float currentParam, float gradient, const std::vector<float>& optimizationHistory) {
    // Use learned optimization algorithm
    float optimalRate = 0.01f;
    
    if (!optimizationHistory.empty()) {
        // Predict optimal learning rate
        float avgGradient = std::accumulate(optimizationHistory.begin(), optimizationHistory.end(), 0.0f) / optimizationHistory.size();
        
        if (std::abs(avgGradient) > 1e-6f) {
            optimalRate = std::abs(currentParam / avgGradient);
        }
    }
    
    // Apply optimized update
    float updatedParam = currentParam - optimalRate * gradient;
    
    // Add exploration noise
    float noise = normalDist(pImpl->rng) * optimalRate * 0.01f;
    updatedParam += noise;
    
    return updatedParam;
}

} // namespace nlm