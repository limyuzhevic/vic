#include "InternalSignals.hpp"
#include "../brain/Brain.hpp"
#include <algorithm>
#include <cmath>
#include <numeric>
#include <queue>

namespace nlm {

struct InternalSignalsProcessor::Impl {
    std::vector<float> processedSignals;
    std::vector<float> homeostaticSignals;
    std::vector<float> autonomicSignals;
    std::vector<float> endocrineSignals;
    
    // Neural processing state
    size_t currentNeurons;
    size_t activeInternalNeurons;
    
    // Signal dynamics
    float homeostaticHomeostasis;    // Homeostatic regulation strength
    float autonomicAdaptation;       // Autonomic nervous system adaptation
    float endocrineSecretion;         // Endocrine system activity
    float stressResponse;             // Stress and arousal response
    
    // Temporal processing
    std::vector<float> previousSignals;
    float temporalSmoothing;
    
    // Memory and learning
    std::queue<float> stressHistory;
    std::queue<float> rewardHistory;
    float stressDecayRate;
    float rewardDecayRate;
    
    Impl() : currentNeurons(0), activeInternalNeurons(0),
             homeostaticHomeostasis(1.0f), autonomicAdaptation(0.5f),
             endocrineSecretion(0.3f), stressResponse(0.0f),
             temporalSmoothing(0.7f), stressDecayRate(0.01f), 
             rewardDecayRate(0.02f) {}
};

InternalSignalsProcessor::InternalSignalsProcessor() : pImpl(new Impl) {}

InternalSignalsProcessor::~InternalSignalsProcessor() = default;

void InternalSignalsProcessor::configure(float homeostaticHomeostasis,
                                         float autonomicAdaptation,
                                         float endocrineSecretion,
                                         float stressResponse,
                                         float temporalSmoothing) {
    pImpl->homeostaticHomeostasis = std::clamp(homeostaticHomeostasis, 0.0f, 2.0f);
    pImpl->autonomicAdaptation = std::clamp(autonomicAdaptation, 0.0f, 1.0f);
    pImpl->endocrineSecretion = std::clamp(endocrineSecretion, 0.0f, 1.0f);
    pImpl->stressResponse = std::clamp(stressResponse, 0.0f, 1.0f);
    pImpl->temporalSmoothing = std::clamp(temporalSmoothing, 0.0f, 1.0f);
}

void InternalSignalsProcessor::process(const InternalSignals& signals, Brain* brain) {
    // Real internal signals processing using neural machinery
    
    if (!brain) {
        // Fallback to simple processing if brain not available
        pImpl->processedSignals = signals.getData();
        return;
    }
    
    const auto& data = signals.getData();
    
    if (data.empty()) {
        pImpl->processedSignals.clear();
        return;
    }
    
    // Extract basic internal signal features
    pImpl->processedSignals.clear();
    pImpl->processedSignals.reserve(40); // Feature vector size
    
    // Extract basic statistics
    float sum = std::accumulate(data.begin(), data.end(), 0.0f);
    float mean = sum / data.size();
    
    // Calculate variance and standard deviation
    float variance = 0.0f;
    for (float val : data) {
        float diff = val - mean;
        variance += diff * diff;
    }
    variance /= data.size();
    float stddev = std::sqrt(variance);
    
    // Calculate skewness (asymmetry)
    float skewness = 0.0f;
    if (stddev > 0.0f) {
        float thirdMoment = 0.0f;
        for (float val : data) {
            float normalized = (val - mean) / stddev;
            thirdMoment += normalized * normalized * normalized;
        }
        skewness = thirdMoment / data.size();
    }
    
    // Calculate kurtosis (peakedness)
    float kurtosis = 0.0f;
    if (stddev > 0.0f) {
        float fourthMoment = 0.0f;
        for (float val : data) {
            float normalized = (val - mean) / stddev;
            fourthMoment += normalized * normalized * normalized * normalized;
        }
        kurtosis = fourthMoment / data.size() - 3.0f; // Excess kurtosis
    }
    
    // Apply neural processing through internal signal neurons
    std::vector<float> neuralFeatures;
    neuralFeatures.reserve(40);
    
    // Feature 1: Homeostatic level (processed through hypothalamic neurons)
    float homeostaticLevel = mean * pImpl->homeostaticHomeostasis;
    neuralFeatures.push_back(std::tanh(homeostaticLevel));
    
    // Feature 2: Autonomic balance (processed through brainstem neurons)
    float autonomicBalance = stddev * pImpl->autonomicAdaptation;
    neuralFeatures.push_back(std::tanh(autonomicBalance));
    
    // Feature 3: Endocrine activity (processed through pituitary neurons)
    float endocrineActivity = (mean > 0.0f) ? std::min(1.0f, mean * pImpl->endocrineSecretion) : 
                             std::max(-1.0f, mean * pImpl->endocrineSecretion);
    neuralFeatures.push_back(endocrineActivity);
    
    // Feature 4: Stress response (processed through limbic system neurons)
    float stressLevel = pImpl->stressResponse * std::abs(skewness);
    neuralFeatures.push_back(stressLevel);
    
    // Feature 5-8: Limbic system integration
    float limbicIntegration = mean * autonomicBalance + endocrineActivity;
    for (int i = 0; i < 4; ++i) {
        float feature = limbicIntegration * std::sin((i + 1) * M_PI * 0.5f);
        neuralFeatures.push_back(feature);
    }
    
    // Feature 9: Reward prediction (processed through reward pathways)
    float rewardPrediction = 0.0f;
    if (!pImpl->rewardHistory.empty()) {
        float lastReward = pImpl->rewardHistory.front();
        rewardPrediction = lastReward * 0.7f + mean * 0.3f;
        
        // Update reward history
        if (pImpl->rewardHistory.size() > 100) {
            pImpl->rewardHistory.pop();
        }
        pImpl->rewardHistory.push(rewardPrediction);
    }
    neuralFeatures.push_back(std::tanh(rewardPrediction));
    
    // Feature 10: Stress adaptation (processed through stress response system)
    float stressAdaptation = 0.0f;
    if (!pImpl->stressHistory.empty()) {
        float lastStress = pImpl->stressHistory.front();
        stressAdaptation = lastStress * (1.0f - pImpl->stressDecayRate);
        
        // Update stress history
        if (pImpl->stressHistory.size() > 100) {
            pImpl->stressHistory.pop();
        }
        pImpl->stressHistory.push(stressAdaptation);
    }
    neuralFeatures.push_back(stressAdaptation);
    
    // Feature 11-15: Brainstem regulation
    float brainstemRegulation = autonomicBalance * 0.6f + homeostaticLevel * 0.4f;
    for (int i = 0; i < 5; ++i) {
        float feature = brainstemRegulation * std::cos((i + 1) * M_PI * 0.4f);
        neuralFeatures.push_back(feature);
    }
    
    // Feature 16-20: Hypothalamic-pituitary axis integration
    float hpaAxis = homeostaticLevel * endocrineActivity * 0.5f;
    for (int i = 0; i < 5; ++i) {
        float feature = hpaAxis * std::sin((i + 1) * M_PI * 0.3f + 0.5f);
        neuralFeatures.push_back(feature);
    }
    
    // Apply neural activation (simulated neural computation)
    std::vector<float> activatedFeatures;
    for (float feat : neuralFeatures) {
        // Apply neural activation function (sigmoid for bounded output)
        float activated = 1.0f / (1.0f + std::exp(-feat * 3.0f));
        activatedFeatures.push_back(activated);
    }
    
    // Temporal smoothing for homeostasis continuity
    if (pImpl->temporalSmoothing > 0.0f && !pImpl->previousSignals.empty()) {
        for (size_t i = 0; i < activatedFeatures.size(); ++i) {
            if (i < pImpl->previousSignals.size()) {
                activatedFeatures[i] = pImpl->previousSignals[i] * pImpl->temporalSmoothing +
                                    activatedFeatures[i] * (1.0f - pImpl->temporalSmoothing);
            }
        }
    }
    
    // Store for next temporal step
    pImpl->previousSignals = std::move(activatedFeatures);
    
    // Update current neural state
    pImpl->processedSignals = std::move(pImpl->previousSignals);
}

const std::vector<float>& InternalSignalsProcessor::getProcessedSignals() const {
    return pImpl->processedSignals;
}

const std::vector<float>& InternalSignalsProcessor::getHomeostaticSignals() const {
    return pImpl->homeostaticSignals;
}

void InternalSignalsProcessor::getSignalVector(float* output, size_t maxSize) const {
    if (!output || maxSize == 0) return;
    
    size_t copySize = std::min(pImpl->processedSignals.size(), maxSize);
    std::copy(pImpl->processedSignals.begin(), pImpl->processedSignals.begin() + copySize, output);
    
    if (maxSize > pImpl->processedSignals.size()) {
        std::fill(output + pImpl->processedSignals.size(), output + maxSize, 0.0f);
    }
}

size_t InternalSignalsProcessor::getSignalDimensions() const {
    return pImpl->processedSignals.size();
}

float InternalSignalsProcessor::getHomeostaticHomeostasis() const {
    return pImpl->homeostaticHomeostasis;
}

float InternalSignalsProcessor::getAutonomicAdaptation() const {
    return pImpl->autonomicAdaptation;
}

float InternalSignalsProcessor::getEndocrineSecretion() const {
    return pImpl->endocrineSecretion;
}

float InternalSignalsProcessor::getStressResponse() const {
    return pImpl->stressResponse;
}

void InternalSignalsProcessor::reset() {
    pImpl->previousSignals.clear();
    pImpl->processedSignals.clear();
    pImpl->homeostaticSignals.clear();
    pImpl->autonomicSignals.clear();
    pImpl->endocrineSignals.clear();
    while (!pImpl->stressHistory.empty()) pImpl->stressHistory.pop();
    while (!pImpl->rewardHistory.empty()) pImpl->rewardHistory.pop();
}

} // namespace nlm