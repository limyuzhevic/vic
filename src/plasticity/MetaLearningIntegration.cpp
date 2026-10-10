#include "MetaLearningIntegration.hpp"
#include "../core/Logger/Logger.hpp"
#include "../brain/Brain.hpp"
#include "../brain/Neuron.hpp"
#include "../brain/Synapse.hpp"
#include <cmath>
#include <algorithm>
#include <random>
#include <numeric>

namespace nlm {

// MetaLearning integration implementation
MetaLearningIntegration::MetaLearningIntegration() : 
    metaLearningEnabled_(false),
    taskInferenceEnabled_(false),
    learningToLearnEnabled_(false),
    autoCuriosityEnabled_(false),
    metaPlasticityEnabled_(false),
    brain_(nullptr) {
    
    // Initialize meta-learning components
    metaLearning_ = std::make_unique<MetaLearning>();
    taskInference_ = std::make_unique<TaskInference>();
    learningToLearn_ = std::make_unique<LearningToLearn>();
    autoCuriosity_ = std::make_unique<AutoCuriosity>();
    metaPlasticity_ = std::make_unique<MetaPlasticity>();
}

MetaLearningIntegration::~MetaLearningIntegration() {}

void MetaLearningIntegration::initialize(Brain* brain) {
    if (!brain) {
        NLM_LOG_ERROR("Cannot initialize meta-learning integration: null brain pointer");
        return;
    }
    
    brain_ = brain;
    
    // Initialize all meta-learning components with brain reference
    if (metaLearningEnabled_) {
        metaLearning_->initialize(brain_);
    }
    
    // Configure meta-learning components with brain parameters
    // (Additional initialization could be added here)
    
    NLM_LOG_INFO("Meta-learning integration initialized with brain reference");
}

void MetaLearningIntegration::update(const std::vector<Synapse*>& synapses, float taskLoss, float validationLoss) {
    if (!brain_ || synapses.empty()) {
        return;
    }
    
    // Get current learning rate from brain or use default
    float learningRate = 0.01f; // Default learning rate
    
    // Update meta-learning system
    if (metaLearningEnabled_) {
        metaLearning_->update(synapses, taskLoss, validationLoss, learningRate);
    }
    
    // Update task inference
    if (taskInferenceEnabled_) {
        std::string currentTask = inferCurrentTaskFromSynapses(synapses);
        std::vector<float> taskFeatures = extractTaskFeatures(synapses);
        taskInference_->learnTask(currentTask, taskFeatures, taskLoss);
    }
    
    // Update learning-to-learn system
    if (learningToLearnEnabled_) {
        updateLearningToLearnState(synapses, taskLoss, validationLoss);
    }
    
    // Update auto-curiosity system
    if (autoCuriosityEnabled_) {
        updateAutoCuriosityState(taskLoss, validationLoss);
    }
    
    // Update meta-plasticity
    if (metaPlasticityEnabled_) {
        updateMetaPlasticityState(synapses, taskLoss);
    }
}

void MetaLearningIntegration::registerTask(const std::string& taskId, const std::vector<float>& taskFeatures) {
    if (metaLearningEnabled_) {
        metaLearning_->registerTask(taskId, taskFeatures);
    }
    
    if (taskInferenceEnabled_) {
        taskInference_->learnTask(taskId, taskFeatures, 0.5f); // Initial performance estimate
    }
}

// Helper methods for MetaLearningIntegration
std::string MetaLearningIntegration::inferCurrentTaskFromSynapses(const std::vector<Synapse*>& synapses) const {
    if (synapses.empty()) {
        return "unknown_task";
    }
    
    // Extract features from synapses for task inference
    std::vector<float> synapseFeatures;
    for (const auto& synapse : synapses) {
        if (synapse) {
            synapseFeatures.push_back(synapse->getWeight());
            // Add more features as needed
        }
    }
    
    // Use task inference to identify current task
    if (taskInferenceEnabled_) {
        return taskInference_->inferTask(synapseFeatures);
    }
    
    return "default_task";
}

std::vector<float> MetaLearningIntegration::extractTaskFeatures(const std::vector<Synapse*>& synapses) const {
    std::vector<float> features;
    
    for (const auto& synapse : synapses) {
        if (synapse) {
            // Extract synaptic features for task representation
            features.push_back(synapse->getWeight());
            
            // Add spike history if available
            // features.push_back(synapse->getRecentSpikeCount());
            
            // Add connection strength
            // features.push_back(synapse->getConnectionStrength());
        }
    }
    
    // Normalize features
    if (!features.empty()) {
        float maxFeature = *std::max_element(features.begin(), features.end());
        float minFeature = *std::min_element(features.begin(), features.end());
        
        if (maxFeature - minFeature > 0.0f) {
            for (auto& feature : features) {
                feature = (feature - minFeature) / (maxFeature - minFeature);
            }
        }
    }
    
    return features;
}

void MetaLearningIntegration::updateLearningToLearnState(const std::vector<Synapse*>& synapses, float taskLoss, float validationLoss) {
    // Collect optimization history from synapses
    std::vector<float> optimizationHistory;
    for (const auto& synapse : synapses) {
        if (synapse) {
            optimizationHistory.push_back(synapse->getWeight());
        }
    }
    
    if (learningToLearnEnabled_) {
        learningToLearn_->learnOptimizationAlgorithm(optimizationHistory);
    }
}

void MetaLearningIntegration::updateAutoCuriosityState(float taskLoss, float validationLoss) {
    // Compute prediction error and novelty from task performance
    float predictionError = std::abs(validationLoss - taskLoss);
    float novelty = predictionError * 0.5f; // Simple novelty estimation
    
    // Update auto-curiosity system
    if (autoCuriosityEnabled_) {
        autoCuriosity_->update(predictionError, novelty, 1.0f); // Default timestep
    }
}

void MetaLearningIntegration::updateMetaPlasticityState(const std::vector<Synapse*>& synapses, float taskLoss) {
    // Update meta-plasticity based on task performance
    if (metaPlasticityEnabled_) {
        for (const auto& synapse : synapses) {
            if (synapse && metaPlasticity_) {
                // Apply meta-plastic updates to synapse
                // (Implementation depends on synapse's plasticity system)
                metaPlasticity_->applyWeightChange(synapse, taskLoss * 0.01f);
            }
        }
    }
}

// LearningToLearn implementation
struct LearningToLearn::Impl {
    std::vector<float> learningRateHistory;
    std::vector<float> lossHistory;
    std::vector<float> gradientHistory;
    std::vector<float> optimalLearningRates;
    std::vector<float> momentumHistory;
    std::vector<std::vector<float>> adamFirstMomentHistory;
    std::vector<std::vector<float>> adamSecondMomentHistory;
    
    std::mt19937 rng{std::random_device{}()};
    std::normal_distribution<float> normalDist{0.0f, 0.1f};
    std::uniform_real_distribution<float> uniformDist{0.0f, 1.0f};
};

LearningToLearn::LearningToLearn() : 
    currentLearningRate_(0.01f),
    momentum_(0.9f),
    adamBeta1_(0.9f),
    adamBeta2_(0.999f),
    adamStep_(0) {
    pImpl = std::make_unique<Impl>();
    
    // Initialize Adam moments
    adamFirstMoment_.resize(100, 0.0f);
    adamSecondMoment_.resize(100, 0.0f);
}

LearningToLearn::~LearningToLearn() {}

float LearningToLearn::optimizeLearningRate(const std::vector<float>& lossHistory, const std::vector<float>& gradientHistory) {
    if (lossHistory.empty() || gradientHistory.empty()) {
        return currentLearningRate_;
    }
    
    // Store history
    pImpl->lossHistory = lossHistory;
    pImpl->gradientHistory = gradientHistory;
    
    // Simple learning rate optimization using Adam
    float avgGradient = std::accumulate(gradientHistory.begin(), gradientHistory.end(), 0.0f) / gradientHistory.size();
    float avgLoss = std::accumulate(lossHistory.begin(), lossHistory.end(), 0.0f) / lossHistory.size();
    
    // Adam optimization
    float learningRate = currentLearningRate_;
    
    if (std::abs(avgGradient) > 1e-8f) {
        learningRate = std::abs(avgLoss / avgGradient);
        learningRate = std::max(0.0001f, std::min(1.0f, learningRate));
    }
    
    // Apply learning rate with momentum
    currentLearningRate_ = momentum_ * currentLearningRate_ + (1.0f - momentum_) * learningRate;
    
    // Update Adam moments
    adamStep_++;
    float beta1 = adamBeta1_ * (1.0f - std::pow(adamBeta1_, adamStep_));
    float beta2 = adamBeta2_ * (1.0f - std::pow(adamBeta2_, adamStep_));
    
    // Update moments (simplified)
    float moment1 = 0.0f, moment2 = 0.0f;
    for (size_t i = 0; i < std::min(size_t(10), gradientHistory.size()); ++i) {
        float g = gradientHistory[gradientHistory.size() - 1 - i];
        moment1 = beta1 * moment1 + (1.0f - beta1) * g;
        moment2 = beta2 * moment2 + (1.0f - beta2) * g * g;
    }
    
    // Update learning rate based on Adam bias correction
    float correctedLearningRate = learningRate * std::sqrt(1.0f - moment2) / (1.0f - moment1 + 1e-8f);
    currentLearningRate_ = std::max(0.0001f, std::min(1.0f, correctedLearningRate));
    
    // Store optimal learning rate
    pImpl->optimalLearningRates.push_back(currentLearningRate_);
    if (pImpl->optimalLearningRates.size() > 100) {
        pImpl->optimalLearningRates.erase(pImpl->optimalLearningRates.begin());
    }
    
    return currentLearningRate_;
}

void LearningToLearn::learnOptimizationAlgorithm(const std::vector<float>& optimizationHistory) {
    if (optimizationHistory.empty()) {
        return;
    }
    
    // Store optimization history
    pImpl->learningRateHistory = optimizationHistory;
    
    // Learn optimization algorithm parameters
    float avgLoss = std::accumulate(optimizationHistory.begin(), optimizationHistory.end(), 0.0f) / optimizationHistory.size();
    
    // Adapt learning rate based on loss pattern
    if (avgLoss > 1.0f) {
        // High loss, reduce learning rate
        currentLearningRate_ *= 0.95f;
    } else if (avgLoss < 0.01f) {
        // Low loss, can increase learning rate
        currentLearningRate_ *= 1.05f;
    }
    
    // Clamp learning rate
    currentLearningRate_ = std::max(0.0001f, std::min(1.0f, currentLearningRate_));
}

float LearningToLearn::applyOptimizedUpdate(float currentParam, float gradient, const std::vector<float>& optimizationHistory) {
    // Use learned optimization algorithm
    float learningRate = currentLearningRate_;
    
    if (!optimizationHistory.empty()) {
        // Predict optimal learning rate
        float avgGradient = std::accumulate(optimizationHistory.begin(), optimizationHistory.end(), 0.0f) / optimizationHistory.size();
        
        if (std::abs(avgGradient) > 1e-8f) {
            learningRate = std::abs(currentParam / avgGradient);
        }
    }
    
    // Apply optimized update
    float updatedParam = currentParam - learningRate * gradient;
    
    // Add momentum
    float momentum = momentum_;
    updatedParam += momentum * gradient;
    
    // Add exploration noise
    float noise = normalDist(pImpl->rng) * learningRate * 0.01f;
    updatedParam += noise;
    
    return updatedParam;
}

void LearningToLearn::reset() {
    currentLearningRate_ = 0.01f;
    momentum_ = 0.9f;
    adamBeta1_ = 0.9f;
    adamBeta2_ = 0.999f;
    adamStep_ = 0;
    
    std::fill(adamFirstMoment_.begin(), adamFirstMoment_.end(), 0.0f);
    std::fill(adamSecondMoment_.begin(), adamSecondMoment_.end(), 0.0f);
    
    pImpl->learningRateHistory.clear();
    pImpl->lossHistory.clear();
    pImpl->gradientHistory.clear();
    pImpl->optimalLearningRates.clear();
    pImpl->momentumHistory.clear();
    pImpl->adamFirstMomentHistory.clear();
    pImpl->adamSecondMomentHistory.clear();
}

// AutoCuriosity implementation
struct AutoCuriosity::Impl {
    std::vector<float> curiosityLevelHistory;
    std::vector<float> predictionErrorHistory;
    std::vector<float> noveltyHistory;
    float baselineCuriosity;
    float curiosityAdaptationRate;
    float explorationAdaptationRate;
    
    std::mt19937 rng{std::random_device{}()};
    std::normal_distribution<float> normalDist{0.0f, 0.1f};
    std::uniform_real_distribution<float> uniformDist{0.0f, 1.0f};
};

AutoCuriosity::AutoCuriosity() : 
    curiosityLevel_(0.0f),
    explorationDrive_(0.0f),
    curiosityGain_(1.0f),
    explorationRate_(0.1f),
    intrinsicRewardWeight_(0.5f),
    curiosityDecay_(0.99f) {
    pImpl = std::make_unique<Impl>();
    
    pImpl->baselineCuriosity = 0.01f;
    pImpl->curiosityAdaptationRate = 0.05f;
    pImpl->explorationAdaptationRate = 0.03f;
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
    
    // Update exploration drive
    explorationDrive_ = curiosityLevel_ * (1.0f + uniformDist(pImpl->rng) * 0.3f);
    
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
    
    // Apply curiosity decay
    curiosityLevel_ *= curiosityDecay_;
    explorationDrive_ *= curiosityDecay_;
    
    // Adapt exploration rate based on curiosity
    explorationRate_ = std::min(1.0f, curiosityLevel_ * 2.0f + uniformDist(pImpl->rng) * 0.1f);
    
    return curiosityLevel_;
}

float AutoCuriosity::getAveragePredictionError() const {
    if (pImpl->predictionErrorHistory.empty()) {
        return 0.0f;
    }
    
    float sum = std::accumulate(pImpl->predictionErrorHistory.begin(), pImpl->predictionErrorHistory.end(), 0.0f);
    return sum / pImpl->predictionErrorHistory.size();
}

float AutoCuriosity::getAverageNovelty() const {
    if (pImpl->noveltyHistory.empty()) {
        return 0.0f;
    }
    
    float sum = std::accumulate(pImpl->noveltyHistory.begin(), pImpl->noveltyHistory.end(), 0.0f);
    return sum / pImpl->noveltyHistory.size();
}

float AutoCuriosity::getCuriosityVariance() const {
    if (pImpl->curiosityLevelHistory.size() < 2) {
        return 0.0f;
    }
    
    float mean = getAveragePredictionError(); // Using average prediction error as proxy
    float variance = 0.0f;
    
    for (float curiosity : pImpl->curiosityLevelHistory) {
        variance += (curiosity - mean) * (curiosity - mean);
    }
    
    return variance / pImpl->curiosityLevelHistory.size();
}

// MetaPlasticity implementation
struct MetaPlasticity::Impl {
    std::map<NeuronId, std::vector<float>> learningRateHistory;
    std::map<NeuronId, std::vector<float>> weightChangeHistory;
    std::map<NeuronId, float> neuronPlasticityState;
    std::map<NeuronId, float> metaPlasticityEffectiveness;
    
    std::mt19937 rng{std::random_device{}()};
    std::normal_distribution<float> normalDist{0.0f, 0.1f};
    std::uniform_real_distribution<float> uniformDist{0.0f, 1.0f};
};

MetaPlasticity::MetaPlasticity() : 
    adaptiveLearningRate_(true),
    metaPlasticityRate_(0.01f),
    plasticityHistorySize_(100),
    metaPlasticityThreshold_(0.5f) {
    pImpl = std::make_unique<Impl>();
}

MetaPlasticity::~MetaPlasticity() {}

void MetaPlasticity::update(Synapse* synapse, const std::vector<Timestamp>& preSpikes, const std::vector<Timestamp>& postSpikes, TimestepDuration dt) {
    if (!synapse) {
        return;
    }
    
    NeuronId preNeuronId = synapse->getPreNeuron();
    NeuronId postNeuronId = synapse->getPostNeuron();
    
    // Store weight history for meta-plastic analysis
    pImpl->weightChangeHistory[preNeuronId].push_back(synapse->getWeight());
    if (pImpl->weightChangeHistory[preNeuronId].size() > plasticityHistorySize_) {
        pImpl->weightChangeHistory[preNeuronId].erase(pImpl->weightChangeHistory[preNeuronId].begin());
    }
    
    // Compute adaptive learning rate based on weight history
    if (adaptiveLearningRate_ && pImpl->weightChangeHistory[preNeuronId].size() >= 2) {
        const auto& weights = pImpl->weightChangeHistory[preNeuronId];
        
        // Compute statistics of weight history
        float avgWeight = std::accumulate(weights.begin(), weights.end(), 0.0f) / weights.size();
        float weightVariance = 0.0f;
        
        for (float w : weights) {
            weightVariance += (w - avgWeight) * (w - avgWeight);
        }
        weightVariance /= weights.size();
        
        // Adjust learning rate based on weight variance (meta-plasticity)
        float plasticityFactor = 1.0f + weightVariance * metaPlasticityRate_;
        pImpl->neuronPlasticityState[preNeuronId] *= plasticityFactor;
        
        // Store meta-plasticity effectiveness
        pImpl->metaPlasticityEffectiveness[preNeuronId] = plasticityFactor;
    }
}

void MetaPlasticity::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    if (!synapse) {
        return;
    }
    
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
}

float MetaPlasticity::getAveragePlasticityModulation() const {
    if (pImpl->metaPlasticityEffectiveness.empty()) {
        return 0.0f;
    }
    
    float sum = 0.0f;
    for (const auto& pair : pImpl->metaPlasticityEffectiveness) {
        sum += pair.second;
    }
    
    return sum / pImpl->metaPlasticityEffectiveness.size();
}

float MetaPlasticity::getMetaPlasticityEffectiveness() const {
    if (pImpl->metaPlasticityEffectiveness.empty()) {
        return 0.0f;
    }
    
    float sum = 0.0f;
    for (const auto& pair : pImpl->metaPlasticityEffectiveness) {
        sum += pair.second;
    }
    
    return sum / pImpl->metaPlasticityEffectiveness.size();
}

} // namespace nlm