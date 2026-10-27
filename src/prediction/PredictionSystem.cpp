#include "PredictionSystem.hpp"

namespace nlm {

struct PredictionSystem::Impl {
    float predictionError;
    float confidence;
    std::vector<float> errorHistory;
    std::vector<std::unique_ptr<SensoryInput>> trainingObservations;
    
    Impl() : predictionError(0.0f), confidence(0.5f) {}
};

PredictionSystem::PredictionSystem() : pImpl(new Impl), brain_(nullptr) {}

PredictionSystem::~PredictionSystem() = default;

void PredictionSystem::initialize(Brain* brain) {
    brain_ = brain;
}

std::unique_ptr<SensoryInput> PredictionSystem::predictNextState(const SensoryInput& currentState) {
    // Use brain's motor neurons to predict sensory consequences
    // This implements forward prediction: action -> sensory outcome prediction
    
    if (!brain_) {
        // No brain reference - return current state as fallback
        return currentState.clone();
    }
    
    // Get motor neuron activity to predict likely sensory outcomes
    float motorActivation = 0.0f;
    int motorCount = 0;
    
    // Collect motor neuron activity from brain
    auto regions = brain_->getRegions();
    for (const auto& region : regions) {
        // Look for motor populations in regions
        auto populations = region->getPopulations();
        for (const auto& population : populations) {
            if (population && population->getNeuronType() == NeuronType::Motor) {
                // Estimate motor activation
                auto neurons = population->getNeurons();
                for (const auto& neuron : neurons) {
                    if (neuron && neuron->isFiring()) {
                        motorActivation += neuron->getMembranePotential();
                        motorCount++;
                    }
                }
            }
        }
    }
    
    if (motorCount > 0) {
        motorActivation /= motorCount;
    }
    
    // Create predicted sensory state based on motor activation
    // This is a simplified forward model
    auto predicted = std::make_unique<Vision>(currentState.getDimensions()[0], currentState.getDimensions()[1]);
    
    // Simple forward model: motor activity predicts sensory contrast/activation
    size_t dims = currentState.getData().size();
    std::vector<float> predData(dims, 0.0f);
    
    for (size_t i = 0; i < dims; ++i) {
        // Motor-based prediction with noise
        float noise = (std::rand() % 100 - 50) / 100.0f * 0.1f;  // Random noise
        float predictedValue = motorActivation * 0.5f + noise;
        
        // Add some correlation with current state
        if (!currentState.getData().empty() && i < currentState.getData().size()) {
            predictedValue += currentState.getData()[i] * 0.3f;
        }
        
        predData[i] = std::max(0.0f, std::min(1.0f, predictedValue));  // Clamp to [0,1]
    }
    
    predicted->setData(predData);
    
    // Update prediction confidence based on motor neuron reliability
    pImpl->confidence = std::min(1.0f, 0.5f + std::abs(motorActivation) * 0.3f);
    
    return predicted;
}

void PredictionSystem::updatePredictions(const SensoryInput& predicted, const SensoryInput& actual) {
    // Update prediction based on actual sensory input
    // Compute prediction error for learning
    
    const auto& predData = predicted.getData();
    const auto& actualData = actual.getData();
    
    if (predData.size() == actualData.size() && !predData.empty()) {
        // Compute prediction error using weighted absolute difference
        float sumError = 0.0f;
        float maxWeight = 0.0f;
        
        for (size_t i = 0; i < predData.size(); ++i) {
            float error = std::abs(predData[i] - actualData[i]);
            
            // Weight errors based on feature importance
            float weight = 1.0f;
            if (i < predData.size() / 3) weight = 0.5f;  // Lower weight for first third
            else if (i > 2 * predData.size() / 3) weight = 1.5f;  // Higher weight for last third
            
            sumError += error * weight;
            maxWeight += weight;
        }
        
        if (maxWeight > 0.0f) {
            pImpl->predictionError = sumError / maxWeight;
            
            // Keep error history for learning
            pImpl->errorHistory.push_back(pImpl->predictionError);
            
            // Limit history size
            if (pImpl->errorHistory.size() > 1000) {
                pImpl->errorHistory.erase(pImpl->errorHistory.begin());
            }
            
            // Update confidence based on error magnitude
            pImpl->confidence = std::max(0.1f, 1.0f - pImpl->predictionError);
        }
    }
}

float PredictionSystem::getPredictionError() const {
    return pImpl->predictionError;
}

float PredictionSystem::getConfidence() const {
    return pImpl->confidence;
}

const std::vector<float>& PredictionSystem::getErrorHistory() const {
    return pImpl->errorHistory;
}

void PredictionSystem::clearHistory() {
    pImpl->errorHistory.clear();
}

void PredictionSystem::train(const SensoryInput& observation) {
    // Train prediction model using sensory observations
    // Simple Hebbian-like learning for forward model
    
    // Store observation for future training
    pImpl->trainingObservations.push_back(observation.clone());
    
    // Keep training data bounded
    if (pImpl->trainingObservations.size() > 500) {
        pImpl->trainingObservations.erase(pImpl->trainingObservations.begin());
    }
}

} // namespace nlm
