#include "PredictionSystem.hpp"
#include "../brain/Brain.hpp"
#include "../core/Logger/Logger.hpp"
#include "../memory/NeuralWorkingMemory.hpp"
#include "../memory/NeuralEpisodicMemory.hpp"
#include <algorithm>
#include <cmath>
#include <random>

namespace nlm {

struct PredictionSystem::Impl {
    class Brain* brain;                    // Reference to brain for neural access
    float predictionError;                 // Current prediction error
    float confidence;                      // Prediction confidence (0.0 to 1.0)
    float novelty;                         // Predicted novelty level
    float predictionGain;                  // Learning rate for predictions
    float predictionBias;                  // Bias towards prediction direction
    std::vector<float> errorHistory;       // History of prediction errors
    std::vector<float> predictedPatterns;  // Last few predicted sensory patterns
    std::vector<float> predictedSequence;  // Current predicted sequence
    
    // Prediction model components
    std::vector<float> recurrentWeights;   // Recurrent connection weights for temporal prediction
    std::vector<float> associationStrengths; // Strengths of associations learned from episodes
    float temporalDecay;                   // Decay rate for temporal predictions
    
    Impl() : brain(nullptr)
        , predictionError(0.0f), confidence(0.5f), novelty(0.0f)
        , predictionGain(0.1f), predictionBias(0.0f)
        , temporalDecay(0.95f) {
        // Initialize with random seed
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<float> dis(-1.0f, 1.0f);
        
        recurrentWeights.resize(100, 0.0f);
        associationStrengths.resize(100, 0.0f);
        for (float& w : recurrentWeights) w = dis(gen) * 0.1f;
        for (float& s : associationStrengths) s = dis(gen) * 0.5f;
    }
};

PredictionSystem::PredictionSystem() : pImpl(new Impl) {}

PredictionSystem::~PredictionSystem() = default;

void PredictionSystem::initialize(Brain* brain) {
    pImpl->brain = brain;
    NLM_LOG_INFO("PredictionSystem initialized");
}

std::vector<float> PredictionSystem::getCurrentSensoryState() const {
    std::vector<float> state;
    if (pImpl->brain) {
        // Use first sensory neuron as representative sensory state
        auto neurons = pImpl->brain->getTotalNeuronCount();
        state.resize(neurons, 0.0f);
        
        size_t count = 0;
        for (auto& region : pImpl->brain->getRegions()) {
            for (auto& pop : region->getPopulations()) {
                for (auto* neuron : pop->getNeurons()) {
                    if (count < state.size()) {
                        state[count] = neuron->getState().membranePotential;
                        count++;
                    }
                }
            }
        }
    }
    return state;
}

std::vector<float> PredictionSystem::getNeuralActivityPattern() const {
    std::vector<float> pattern;
    if (pImpl->brain) {
        // Use working memory to get current neural activity pattern
        if (auto* workingMemory = pImpl->brain->getWorkingMemory()) {
            pattern = workingMemory->retrieve();
            
            // Augment with current sensory neuron activity
            std::vector<float> sensoryActivity;
            for (auto* neuron : pImpl->brain->getPredictionSystem() ? 
                 std::vector<Neuron*>{}) {
                if (neuron && neuron->getType() == NeuronType::Sensory) {
                    sensoryActivity.push_back(
                        std::abs(neuron->getState().membranePotential - 
                                neuron->getState().restingPotential) / 20.0f);
                }
            }
            
            // Blend working memory with sensory activity
            if (!sensoryActivity.empty() && !pattern.empty()) {
                size_t minSize = std::min(pattern.size(), sensoryActivity.size());
                for (size_t i = 0; i < minSize; ++i) {
                    pattern[i] = pattern[i] * 0.7f + sensoryActivity[i] * 0.3f;
                }
            }
        }
    }
    
    return pattern;
}

std::unique_ptr<SensoryInput> PredictionSystem::predictNextState(const SensoryInput& currentState) {
    if (!pImpl->brain) {
        return currentState.clone();
    }
    
    // Get current neural activity pattern
    std::vector<float> currentPattern = getNeuralActivityPattern();
    
    // Get episodic memory for pattern completion
    std::vector<const EpisodicMemoryItem*> similarEpisodes;
    if (auto* episodicMemory = pImpl->brain->getEpisodicMemory()) {
        similarEpisodes = episodicMemory->retrieveSimilar(currentPattern, 3);
    }
    
    // Build predicted pattern using multiple sources
    std::vector<float> predictedPattern = currentPattern;
    
    // Apply recurrent dynamics for temporal prediction
    for (size_t i = 0; i < predictedPattern.size() && i < recurrentWeights.size(); ++i) {
        // Recurrence adds persistence to prediction
        float recurrentInfluence = 0.0f;
        if (i > 0 && i < recurrentWeights.size()) {
            recurrentInfluence = predictedPattern[i - 1] * recurrentWeights[i];
        }
        predictedPattern[i] = predictedPattern[i] * (1.0f - std::abs(recurrentWeights[i])) 
                           + recurrentInfluence * pImpl->temporalDecay;
    }
    
    // Integrate with episodic memory for pattern completion
    for (const auto* episode : similarEpisodes) {
        const auto& episodePattern = episode->sensoryState;
        if (episodePattern.size() <= predictedPattern.size()) {
            // Blend episode pattern with prediction
            for (size_t i = 0; i < episodePattern.size(); ++i) {
                predictedPattern[i] += episodePattern[i] * 0.2f * episode->age / 1000.0f;
            }
        }
    }
    
    // Clamp to valid range
    for (float& val : predictedPattern) {
        val = std::max(0.0f, std::min(1.0f, val));
    }
    
    // Store predicted pattern
    pImpl->predictedPatterns.push_back(std::accumulate(predictedPattern.begin(), 
                                                       predictedPattern.end(), 0.0f) / predictedPattern.size());
    
    // If we have stored patterns, form a sequence prediction
    if (pImpl->predictedPatterns.size() > 1) {
        pImpl->predictedSequence = pImpl->predictedPatterns;
        // Keep only recent predictions
        if (pImpl->predictedSequence.size() > 50) {
            pImpl->predictedSequence.erase(pImpl->predictedSequence.begin());
        }
    }
    
    // Create new sensory input with predicted data
    auto predictedInput = std::make_unique<Vision>(currentState.getDimensions());
    predictedInput->setData(predictedPattern);
    
    // Update confidence based on pattern consistency
    float patternConsistency = calculatePatternConsistency(predictedPattern);
    pImpl->confidence = (pImpl->confidence + patternConsistency) * 0.5f;
    
    NLM_LOG_INFO("Prediction system predicted next state with confidence: " + 
                 std::to_string(pImpl->confidence));
    
    return predictedInput;
}

float PredictionSystem::calculatePatternConsistency(const std::vector<float>& pattern) const {
    if (pattern.empty()) return 0.5f;
    
    float mean = std::accumulate(pattern.begin(), pattern.end(), 0.0f) / pattern.size();
    float variance = 0.0f;
    for (float val : pattern) {
        variance += (val - mean) * (val - mean);
    }
    variance /= pattern.size();
    
    // Lower variance means higher consistency
    return std::max(0.0f, 1..0f - variance);
}

void PredictionSystem::updatePredictions(const SensoryInput& predicted, const SensoryInput& actual) {
    const auto& predData = predicted.getData();
    const auto& actualData = actual.getData();
    
    if (predData.size() == actualData.size() && !predData.empty()) {
        // Calculate prediction error as RMS error
        float sumSquaredError = 0.0f;
        for (size_t i = 0; i < predData.size(); ++i) {
            float diff = predData[i] - actualData[i];
            sumSquaredError += diff * diff;
        }
        
        pImpl->predictionError = std::sqrt(sumSquaredError / predData.size());
        pImpl->errorHistory.push_back(pImpl->predictionError);
        
        // Keep only recent errors (last 100)
        if (pImpl->errorHistory.size() > 100) {
            pImpl->errorHistory.erase(pImpl->errorHistory.begin());
        }
        
        // Update confidence based on error
        if (pImpl->predictionError < 0.1f) {
            // Low error = high confidence
            pImpl->confidence += 0.05f;
        } else if (pImpl->predictionError > 0.5f) {
            // High error = low confidence
            pImpl->confidence -= 0.05f;
        }
        
        pImpl->confidence = std::max(0.0f, std::min(1.0f, pImpl->confidence));
        
        // Novelty detection based on prediction error
        pImpl->novelty = std::min(1.0f, pImpl->predictionError * 2.0f);
        
        NLM_LOG_INFO("Prediction error updated: " + std::to_string(pImpl->predictionError) + 
                     ", confidence: " + std::to_string(pImpl->confidence) + 
                     ", novelty: " + std::to_string(pImpl->novelty));
    }
}

void PredictionSystem::updateWithNeuromodulation(float dopamineLevel, float curiosityLevel, TimestepDuration dt) {
    // Update prediction gain based on dopamine
    pImpl->predictionGain = 0.05f + dopamineLevel * 0.2f;
    
    // Update prediction bias based on curiosity
    pImpl->predictionBias = curiosityLevel * 0.5f - 0.25f; // Range [-0.25, 0.25]
    
    // Novelty affects confidence
    if (pImpl->novelty > 0.7f) {
        pImpl->confidence -= 0.02f * pImpl->novelty;
    }
    
    pImpl->confidence = std::max(0.0f, std::min(1.0f, pImpl->confidence));
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
    pImpl->predictedPatterns.clear();
    pImpl->predictedSequence.clear();
}

void PredictionSystem::train(const SensoryInput& observation) {
    if (!pImpl->brain) return;
    
    // Get current sensory pattern
    const auto& obsData = observation.getData();
    std::vector<float> currentPattern(obsData.begin(), obsData.end());
    
    // Get working memory pattern
    std::vector<float> wmPattern;
    if (auto* workingMemory = pImpl->brain->getWorkingMemory()) {
        wmPattern = workingMemory->retrieve();
    }
    
    // Update associative memory through co-activation
    if (!wmPattern.empty() && !currentPattern.empty()) {
        size_t minSize = std::min(wmPattern.size(), currentPattern.size());
        for (size_t i = 0; i < minSize; ++i) {
            if (wmPattern[i] > 0.5f && currentPattern[i] > 0.5f) {
                // Co-activation strengthens associations
                float associationStrength = wmPattern[i] * currentPattern[i];
                
                // Find or create association entry
                bool found = false;
                for (size_t j = 0; j < associationStrengths.size(); ++j) {
                    if (j < currentPattern.size()) {
                        associationStrengths[j] += associationStrength * pImpl->predictionGain;
                        found = true;
                        break;
                    }
                }
            }
        }
    }
    
    // Update recurrent weights based on temporal sequences
    if (!pImpl->predictedSequence.empty() && !currentPattern.empty()) {
        for (size_t i = 0; i < pImpl->predictedSequence.size() && 
                     i < currentPattern.size() && i < recurrentWeights.size(); ++i) {
            float weightChange = (pImpl->predictedSequence[i] - currentPattern[i]) * 
                               pImpl->predictionGain * 0.1f;
            recurrentWeights[i] += weightChange;
            
            // Clip weights
            recurrentWeights[i] = std::max(-1.0f, std::min(1.0f, recurrentWeights[i]));
        }
    }
    
    // Store pattern for future prediction
    float patternAverage = std::accumulate(currentPattern.begin(), currentPattern.end(), 0.0f) / 
                         currentPattern.size();
    pImpl->predictedPatterns.push_back(patternAverage);
    if (pImpl->predictedPatterns.size() > 50) {
        pImpl->predictedPatterns.erase(pImpl->predictedPatterns.begin());
    }
    
    NLM_LOG_INFO("Prediction system trained on observation (error: " + 
                 std::to_string(pImpl->predictionError) + ")");
}

} // namespace nlm
