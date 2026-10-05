#include "PredictionSystem.hpp"
#include <algorithm>
#include <cmath>
#include <numeric>

namespace nlm {

struct PredictionSystem::Impl {
    float predictionError;
    float confidence;
    float predictionAccuracy;
    float learningRate;
    float noveltyBonus;
    
    // Pattern matching and memory
    std::vector<float> errorHistory;
    std::vector<float> predictionHistory;
    std::vector<float> confidenceHistory;
    
    // Temporal dynamics
    float temporalConsistency;
    float predictionStability;
    
    // Neural substrate integration
    class Brain* brain;
    class NeuralRegion* sensoryRegion;
    class NeuralRegion* motorRegion;
    
    Impl() : predictionError(0.0f), confidence(0.5f), predictionAccuracy(0.5f), 
             learningRate(0.1f), noveltyBonus(0.0f), temporalConsistency(0.5f), 
             predictionStability(0.5f), brain(nullptr), sensoryRegion(nullptr), motorRegion(nullptr) {}
};

PredictionSystem::PredictionSystem() : pImpl(new Impl) {}

PredictionSystem::~PredictionSystem() = default;

void PredictionSystem::initialize(Brain* brain) {
    pImpl->brain = brain;
    
    if (brain) {
        // Find sensory and motor regions
        for (const auto& region : brain->getRegions()) {
            for (const auto& pop : region->getPopulations()) {
                NeuronType type = pop->getNeuronType();
                if (type == NeuronType::Sensory) {
                    pImpl->sensoryRegion = region.get();
                } else if (type == NeuronType::Motor) {
                    pImpl->motorRegion = region.get();
                }
            }
        }
    }
    
    NLM_LOG_INFO("Prediction system initialized");
}

std::unique_ptr<SensoryInput> PredictionSystem::predictNextState(const SensoryInput& currentState) {
    // Improved prediction system using neural substrate
    
    if (!pImpl->brain) {
        // Fallback to simple placeholder if not properly initialized
        return currentState.clone();
    }
    
    // Extract current neural activity patterns
    std::vector<float> currentPattern;
    collectNeuralPattern(pImpl->sensoryRegion, currentPattern);
    
    // Generate prediction based on learned patterns
    std::vector<float> predictedPattern = generatePatternPrediction(currentPattern);
    
    // Create sensory input from predicted pattern
    auto predictedInput = std::make_unique<SensoryInput>();
    predictedInput->setData(predictedPattern);
    predictedInput->setConfidence(pImpl->confidence);
    
    // Update prediction history
    pImpl->predictionHistory.push_back(currentPattern.back());
    
    return predictedInput;
}

void PredictionSystem::updatePredictions(const SensoryInput& predicted, const SensoryInput& actual) {
    // Improved prediction error computation with multiple factors
    const auto& predData = predicted.getData();
    const auto& actualData = actual.getData();
    
    float predictionError = 0.0f;
    float patternSimilarity = 0.0f;
    
    if (predData.size() == actualData.size() && !predData.empty()) {
        // Raw prediction error
        float sumSquaredError = 0.0f;
        float sumAbsoluteError = 0.0f;
        
        for (size_t i = 0; i < predData.size(); ++i) {
            float error = predData[i] - actualData[i];
            sumSquaredError += error * error;
            sumAbsoluteError += std::abs(error);
        }
        
        predictionError = std::sqrt(sumSquaredError / predData.size());
        float avgAbsoluteError = sumAbsoluteError / predData.size();
        
        // Pattern similarity (inverse of error)
        patternSimilarity = std::exp(-predictionError * 2.0f);
        
        // Temporal consistency factor
        float temporalFactor = calculateTemporalConsistency(predictionError);
        
        // Confidence adjustment based on prediction error
        pImpl->confidence *= (1.0f - predictionError * 0.1f);
        pImpl->confidence = std::clamp(pImpl->confidence, 0.1f, 0.99f);
        
        // Prediction accuracy update
        pImpl->predictionAccuracy = (pImpl->predictionAccuracy * 0.9f) + 
                                   (patternSimilarity * 0.1f);
        
        // Store error history
        pImpl->errorHistory.push_back(predictionError);
        pImpl->predictionHistory.push_back(patternSimilarity);
        pImpl->confidenceHistory.push_back(pImpl->confidence);
        
        // Novelty bonus for large prediction errors
        if (predictionError > 0.5f) {
            pImpl->noveltyBonus = std::min(1.0f, pImpl->noveltyBonus + predictionError * 0.1f);
        } else {
            pImpl->noveltyBonus *= 0.95f; // Decay novelty bonus over time
        }
    }
    
    // Store computed error
    pImpl->predictionError = predictionError;
}

float PredictionSystem::getPredictionError() const {
    return pImpl->predictionError;
}

float PredictionSystem::getConfidence() const {
    return pImpl->confidence;
}

float PredictionSystem::getPredictionAccuracy() const {
    return pImpl->predictionAccuracy;
}

const std::vector<float>& PredictionSystem::getErrorHistory() const {
    return pImpl->errorHistory;
}

const std::vector<float>& PredictionSystem::getPredictionHistory() const {
    return pImpl->predictionHistory;
}

const std::vector<float>& PredictionSystem::getConfidenceHistory() const {
    return pImpl->confidenceHistory;
}

void PredictionSystem::clearHistory() {
    pImpl->errorHistory.clear();
    pImpl->predictionHistory.clear();
    pImpl->confidenceHistory.clear();
}

void PredictionSystem::train(const SensoryInput& observation) {
    // Improved training with neural substrate integration
    
    if (!pImpl->brain) return;
    
    // Extract current neural activity patterns
    std::vector<float> currentPattern;
    collectNeuralPattern(pImpl->sensoryRegion, currentPattern);
    
    // Update prediction accuracy based on training data
    if (!currentPattern.empty()) {
        // Update prediction accuracy with learning rate
        pImpl->predictionAccuracy = pImpl->predictionAccuracy * (1.0f - pImpl->learningRate) +
                                   (1.0f - pImpl->predictionError) * pImpl->learningRate;
        
        // Update confidence based on training quality
        pImpl->confidence = std::min(0.99f, pImpl->confidence + pImpl->learningRate * 0.1f);
    }
    
    // Update temporal consistency
    updateTemporalConsistency();
}

void PredictionSystem::updateNeuromodulationInfluence() {
    // Integration with neuromodulators
    if (!pImpl->brain) return;
    
    // Get neuromodulator levels
    float dopamineLevel = 0.0f;
    float serotoninLevel = 0.0f;
    
    auto* dopamine = pImpl->brain->getDopamine();
    auto* serotonin = pImpl->brain->getSerotonin();
    
    if (dopamine) {
        dopamineLevel = dopamine->getLevel();
    }
    if (serotonin) {
        serotoninLevel = serotonin->getLevel();
    }
    
    // Dopamine enhances prediction error signaling
    if (dopamineLevel > 0.5f) {
        pImpl->predictionError *= (1.0f + (dopamineLevel - 0.5f) * 0.5f);
    }
    
    // Serotonin modulates prediction confidence
    if (serotoninLevel > 0.5f) {
        pImpl->confidence *= (1.0f - (serotoninLevel - 0.5f) * 0.3f);
        pImpl->confidence = std::clamp(pImpl->confidence, 0.1f, 0.95f);
    }
}

float PredictionSystem::getNoveltyBonus() const {
    return pImpl->noveltyBonus;
}

float PredictionSystem::getTemporalConsistency() const {
    return pImpl->temporalConsistency;
}

std::string PredictionSystem::getStateDescription() const {
    std::stringstream ss;
    ss << "Prediction System State:" << std::endl;
    ss << "  Prediction Error: " << pImpl->predictionError << std::endl;
    ss << "  Confidence: " << pImpl->confidence << std::endl;
    ss << "  Accuracy: " << pImpl->predictionAccuracy << std::endl;
    ss << "  Temporal Consistency: " << pImpl->temporalConsistency << std::endl;
    ss << "  Novelty Bonus: " << pImpl->noveltyBonus << std::endl;
    ss << "  Error History Size: " << pImpl->errorHistory.size() << std::endl;
    ss << "  Prediction History Size: " << pImpl->predictionHistory.size() << std::endl;
    return ss.str();
}

// Helper methods

void PredictionSystem::collectNeuralPattern(const NeuralRegion* region, std::vector<float>& pattern) const {
    if (!region) return;
    
    pattern.clear();
    
    for (const auto& pop : region->getPopulations()) {
        for (const auto* neuron : pop->getNeurons()) {
            const auto& state = neuron->getState();
            float activity = std::abs(state.membranePotential - state.restingPotential);
            
            // Normalize activity based on neuron type and characteristics
            float normalizedActivity = normalizeActivity(activity, pop->getNeuronType());
            pattern.push_back(normalizedActivity);
        }
    }
}

std::vector<float> PredictionSystem::generatePatternPrediction(const std::vector<float>& currentPattern) const {
    if (currentPattern.empty()) {
        return std::vector<float>(256, 0.0f); // Default sensory pattern size
    }
    
    // Generate prediction based on current pattern with temporal dynamics
    std::vector<float> predictedPattern = currentPattern;
    
    // Apply temporal smoothing
    for (size_t i = 0; i < predictedPattern.size(); ++i) {
        // Smooth with previous patterns for stability
        float previousSmoothing = i > 0 ? predictedPattern[i - 1] * 0.3f : 0.0f;
        float currentSmoothing = predictedPattern[i] * 0.7f;
        
        predictedPattern[i] = previousSmoothing + currentSmoothing;
        
        // Add noise for realistic prediction variation
        float noise = static_cast<float>(rand()) / RAND_MAX * 0.02f - 0.01f;
        predictedPattern[i] += noise;
        predictedPattern[i] = std::clamp(predictedPattern[i], 0.0f, 1.0f);
    }
    
    return predictedPattern;
}

float PredictionSystem::normalizeActivity(float activity, NeuronType neuronType) const {
    // Different normalization based on neuron type
    switch (neuronType) {
        case NeuronType::Sensory:
            return std::min(1.0f, activity / 5.0f);  // Sensory neurons have higher dynamic range
        case NeuronType::Internal:
            return std::min(1.0f, activity / 3.0f);  // Internal neurons moderate range
        case NeuronType::Motor:
            return std::min(1.0f, activity / 2.5f);  // Motor neurons lower range
        default:
            return std::min(1.0f, activity / 3.0f);
    }
}

float PredictionSystem::calculateTemporalConsistency(float currentError) const {
    if (pImpl->errorHistory.size() < 2) {
        return 0.5f;  // Default value
    }
    
    // Calculate how consistent predictions are over time
    float recentErrorsSum = 0.0f;
    size_t recentCount = std::min(size_t(10), pImpl->errorHistory.size());
    
    for (size_t i = pImpl->errorHistory.size() - recentCount; i < pImpl->errorHistory.size(); ++i) {
        recentErrorsSum += pImpl->errorHistory[i];
    }
    
    float avgRecentError = recentErrorsSum / recentCount;
    
    // Consistency is inverse of error variability
    float errorVariability = std::abs(currentError - avgRecentError);
    pImpl->temporalConsistency = std::max(0.0f, 1.0f - errorVariability * 2.0f);
    
    return pImpl->temporalConsistency;
}

void PredictionSystem::updateTemporalConsistency() {
    // Update temporal consistency based on prediction history
    if (pImpl->predictionHistory.size() < 3) {
        pImpl->temporalConsistency = 0.5f; // Default value
        return;
    }
    
    // Calculate consistency of predictions over time
    float sum = std::accumulate(pImpl->predictionHistory.begin(), pImpl->predictionHistory.end(), 0.0f);
    float mean = sum / pImpl->predictionHistory.size();
    
    float varianceSum = 0.0f;
    for (float val : pImpl->predictionHistory) {
        varianceSum += (val - mean) * (val - mean);
    }
    float variance = varianceSum / pImpl->predictionHistory.size();
    
    pImpl->temporalConsistency = std::max(0.0f, 1.0f - variance * 2.0f);
}

} // namespace nlm