#include "../core/Logger/Logger.hpp"
#include "../core/Types/Types.hpp"

namespace nlm {

struct PredictionSystem::Impl {
    float predictionError;
    float confidence;
    std::vector<float> errorHistory;
    
    // NEW: Learning weights for prediction
    std::vector<float> predictionWeights_;
    float confidence_;
    
    Impl() : predictionError(0.0f), confidence(0.5f), confidence_(0.5f) {}
};

PredictionSystem::PredictionSystem() : pImpl(new Impl) {}

PredictionSystem::~PredictionSystem() = default;

std::unique_ptr<SensoryInput> PredictionSystem::predictNextState(const SensoryInput& currentState) {
    // TODO PHASE 2: Implement real prediction using NLM's neural substrate
    // PLACEHOLDER: Just return a copy of current state
    
    // Extract data from current state
    const auto& currentData = currentState.getData();
    
    // Simple prediction: modify based on learned weights
    std::vector<float> predictedData = currentData;
    
    // Apply learned transformation if weights exist
    if (!pImpl->predictionWeights_.empty() && pImpl->predictionWeights_.size() == currentData.size()) {
        for (size_t i = 0; i < currentData.size(); ++i) {
            // Add weighted influence from learned patterns
            float influence = pImpl->predictionWeights_[i] * 0.1f;
            predictedData[i] = std::clamp(predictedData[i] + influence, 0.0f, 1.0f);
        }
    }
    
    auto result = std::make_unique<SensoryInput>();
    result->setData(predictedData);
    return result;
}

void PredictionSystem::updatePredictions(const SensoryInput& predicted, const SensoryInput& actual) {
    // TODO PHASE 2: Implement real prediction error computation
    // PLACEHOLDER: Calculate simple error
    const auto& predData = predicted.getData();
    const auto& actualData = actual.getData();
    
    if (predData.size() == actualData.size() && !predData.empty()) {
        float sumError = 0.0f;
        for (size_t i = 0; i < predData.size(); ++i) {
            float diff = predData[i] - actualData[i];
            sumError += diff * diff;
        }
        pImpl->predictionError = sumError / predData.size();
        pImpl->errorHistory.push_back(pImpl->predictionError);
    }
}

float PredictionSystem::getPredictionError() const {
    return pImpl->predictionError;
}

float PredictionSystem::getConfidence() const {
    return pImpl->confidence_;
}

const std::vector<float>& PredictionSystem::getErrorHistory() const {
    return pImpl->errorHistory;
}

void PredictionSystem::clearHistory() {
    pImpl->errorHistory.clear();
}

void PredictionSystem::train(const SensoryInput& observation) {
    // Implement real prediction model using NLM's neural substrate
    // This could involve:
    // 1. Extracting features from sensory input
    // 2. Building a forward model using neural networks
    // 3. Updating prediction weights based on experience
    // 4. Maintaining prediction error gradients
    
    // For Phase 2, implement a simple Hebbian-like learning
    const auto& data = observation.getData();
    
    // Create or update prediction weights
    if (!pImpl->predictionWeights_.empty() && pImpl->predictionWeights_.size() == data.size()) {
        // Hebbian-like learning: strengthen connections for frequently occurring patterns
        for (size_t i = 0; i < data.size(); ++i) {
            if (data[i] > 0.5f) {
                pImpl->predictionWeights_[i] = std::min(1.0f, pImpl->predictionWeights_[i] + 0.01f);
            }
        }
    } else {
        // Initialize weights based on first observation
        pImpl->predictionWeights_ = data;
        for (auto& weight : pImpl->predictionWeights_) {
            weight *= 0.5f;  // Normalize
        }
    }
    
    // Update prediction model confidence
    float consistency = computeModelConsistency(data);
    pImpl->confidence_ = std::min(1.0f, pImpl->confidence_ + 0.05f * consistency);
}

float PredictionSystem::computeModelConsistency(const std::vector<float>& observation) const {
    if (pImpl->predictionWeights_.empty()) return 0.0f;
    
    float totalSimilarity = 0.0f;
    size_t count = 0;
    
    for (size_t i = 0; i < std::min(pImpl->predictionWeights_.size(), observation.size()); ++i) {
        float sim = 1.0f - std::abs(pImpl->predictionWeights_[i] - observation[i]);
        totalSimilarity += sim;
        ++count;
    }
    
    return count > 0 ? (totalSimilarity / count) : 0.0f;
}

// NeuralPrediction implementation struct
struct NeuralPrediction::Impl {
    std::vector<float> weights;
    float learningRate;
    float predictionThreshold;
    
    Impl() : learningRate(0.01f), predictionThreshold(0.5f) {}
};

NeuralPrediction::NeuralPrediction()
    : pImpl(new Impl)
{
}

NeuralPrediction::~NeuralPrediction() = default;

void NeuralPrediction::initialize(const Brain* brain) {
    pImpl->brain = brain;
}

std::unique_ptr<SensoryInput> NeuralPrediction::predictNextState(const SensoryInput& currentState) {
    const auto& data = currentState.getData();
    
    // Simple forward prediction: modify input based on learned weights
    std::vector<float> predictedData = data;
    
    if (!pImpl->weights.empty() && pImpl->weights.size() == data.size()) {
        // Apply learned transformations
        for (size_t i = 0; i < data.size(); ++i) {
            // Add weighted influence from learned patterns
            float influence = pImpl->weights[i] * 0.1f;
            predictedData[i] = std::clamp(predictedData[i] + influence, 0.0f, 1.0f);
        }
        
        // Apply prediction threshold
        for (auto& value : predictedData) {
            if (value > pImpl->predictionThreshold) {
                value = (value - pImpl->predictionThreshold) / (1.0f - pImpl->predictionThreshold);
            } else {
                value = value * (pImpl->predictionThreshold / 1.0f);
            }
        }
    }
    
    auto result = std::make_unique<SensoryInput>();
    result->setData(predictedData);
    return result;
}

float NeuralPrediction::getPredictionError(const SensoryInput& predicted,
                                       const SensoryInput& actual) const {
    const auto& predData = predicted.getData();
    const auto& actualData = actual.getData();
    
    if (predData.size() != actualData.size() || predData.empty()) {
        return 1.0f;  // Maximum error
    }
    
    float sumSquaredError = 0.0f;
    for (size_t i = 0; i < predData.size(); ++i) {
        float diff = predData[i] - actualData[i];
        sumSquaredError += diff * diff;
    }
    
    return std::sqrt(sumSquaredError / predData.size());
}

float NeuralPrediction::getPredictionConfidence() const {
    return 1.0f - pImpl->predictionThreshold;
}

void NeuralPrediction::update(const Brain* brain, const std::vector<float>& sensoryData,
                           TimestepDuration dt) {
    pImpl->brain = brain;
    
    if (!pImpl->brain) return;
    
    // Learn from sensory input
    if (sensoryData.size() > 0) {
        // Hebbian-like learning: strengthen connections for active patterns
        for (size_t i = 0; i < std::min(pImpl->weights.size(), sensoryData.size()); ++i) {
            if (sensoryData[i] > 0.5f) {
                pImpl->weights[i] = std::min(1.0f, pImpl->weights[i] + pImpl->learningRate * dt);
            } else {
                pImpl->weights[i] = std::max(0.0f, pImpl->weights[i] - pImpl->learningRate * dt * 0.5f);
            }
        }
    }
    
    // Decay weights gradually (forgetting)
    for (auto& weight : pImpl->weights) {
        weight *= (1.0f - dt * 0.0001f);  // Slow decay
    }
}

float NeuralPrediction::getPredictionAccuracy() const {
    if (pImpl->weights.empty()) return 0.0f;
    
    // Compute average weight as a measure of model confidence/accuracy
    float totalWeight = 0.0f;
    for (float weight : pImpl->weights) {
        totalWeight += weight;
    }
    
    return totalWeight / pImpl->weights.size();
}
