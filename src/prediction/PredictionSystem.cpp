#include "PredictionSystem.hpp"
#include "../core/Logger/Logger.hpp"
#include "../sensory/SensoryInput.hpp"
#include "../motor/Action.hpp"
#include "../memory/NeuralWorkingMemory.hpp"
#include "../memory/NeuralEpisodicMemory.hpp"

namespace nlm {

struct PredictionSystem::Impl {
    float predictionError;
    float confidence;
    std::vector<float> errorHistory;
    
    // Neural prediction model parameters
    std::vector<float> predictionWeights;  // Learned weights from episodic memory
    float modelLearningRate;
    size_t patternWindowSize;
    
    // Performance tracking
    float totalPredictionError;
    size_t predictionCount;
    
    Impl() 
        : predictionError(0.0f), confidence(0.5f), modelLearningRate(0.01f), patternWindowSize(10)
        , totalPredictionError(0.0f), predictionCount(0) {}
};

PredictionSystem::PredictionSystem() : pImpl(new Impl), brain_(nullptr) {}

PredictionSystem::~PredictionSystem() = default;

void PredictionSystem::initialize(Brain* brain) {
    brain_ = brain;
    NLM_LOG_INFO("PredictionSystem initialized with brain reference");
}

std::unique_ptr<SensoryInput> PredictionSystem::predictNextState(const SensoryInput& currentState) {
    if (!brain_) {
        NLM_LOG_ERROR("PredictionSystem not initialized with brain reference");
        return currentState.clone();
    }
    
    // Retrieve working memory content for current state
    auto* workingMemory = brain_->getWorkingMemory();
    if (workingMemory) {
        std::vector<float> currentPattern = workingMemory->retrieve();
        
        // Use episodic memory for pattern completion and prediction
        auto* episodicMemory = brain_->getEpisodicMemory();
        if (episodicMemory) {
            // Find similar episodes for pattern completion
            std::vector<const EpisodicMemoryItem*> similarEpisodes;
            if (!currentPattern.empty()) {
                similarEpisodes = episodicMemory->retrieveSimilar(currentPattern, 3);
            }
            
            if (!similarEpisodes.empty()) {
                // Predict next state based on similar patterns
                // Average the resulting sensory states from similar episodes
                std::vector<float> predictedPattern(currentPattern.size(), 0.0f);
                size_t validCount = 0;
                
                for (const auto* episode : similarEpisodes) {
                    if (!episode->resultingSensoryState.empty()) {
                        // Blend with current pattern and episode result
                        for (size_t i = 0; i < std::min(currentPattern.size(), episode->resultingSensoryState.size()); ++i) {
                            float weight = 0.7f;  // Weight current pattern more
                            predictedPattern[i] += weight * episode->resultingSensoryState[i];
                            predictedPattern[i] += (1.0f - weight) * currentPattern[i];
                        }
                        validCount++;
                    }
                }
                
                if (validCount > 0 && !predictedPattern.empty()) {
                    // Normalize prediction
                    for (float& val : predictedPattern) {
                        val /= validCount;
                    }
                    
                    // Create prediction from the pattern
                    // In a real implementation, this would be mapped to appropriate sensory modality
                    std::unique_ptr<SensoryInput> prediction = std::make_unique<InternalSignals>();
                    auto& predData = prediction->getData();
                    predData = predictedPattern;
                    
                    // Update confidence based on pattern similarity
                    pImpl->confidence = std::min(1.0f, static_cast<float>(validCount) / 3.0f);
                    
                    return prediction;
                }
            }
        }
    }
    
    // Fallback: use current state as prediction if no pattern completion available
    NLM_LOG_DEBUG("Using simple prediction (no pattern completion)");
    pImpl->confidence = 0.3f;  // Lower confidence for fallback
    return currentState.clone();
}

void PredictionSystem::updatePredictions(const SensoryInput& predicted, const SensoryInput& actual) {
    // Calculate prediction error using neural similarity measure
    const auto& predData = predicted.getData();
    const auto& actualData = actual.getData();
    
    if (predData.size() == actualData.size() && !predData.empty()) {
        float sumError = 0.0f;
        float maxError = 0.0f;
        
        for (size_t i = 0; i < predData.size(); ++i) {
            float diff = predData[i] - actualData[i];
            sumError += diff * diff;
            maxError = std::max(maxError, std::abs(diff));
        }
        
        float mseError = sumError / predData.size();
        
        // Combine MSE and max difference for robust error metric
        pImpl->predictionError = 0.7f * mseError + 0.3f * maxError;
        pImpl->errorHistory.push_back(pImpl->predictionError);
        
        // Limit history size
        if (pImpl->errorHistory.size() > pImpl->patternWindowSize) {
            pImpl->errorHistory.erase(pImpl->errorHistory.begin());
        }
        
        pImpl->totalPredictionError += pImpl->predictionError;
        pImpl->predictionCount++;
        
        // Update confidence based on error
        pImpl->confidence = std::max(0.0f, 1.0f - pImpl->predictionError);
    }
}

void PredictionSystem::train(const SensoryInput& observation) {
    if (!brain_) return;
    
    // Train prediction model using episodic memory patterns
    // Extract pattern from working memory
    auto* workingMemory = brain_->getWorkingMemory();
    if (workingMemory) {
        std::vector<float> currentPattern = workingMemory->retrieve();
        if (currentPattern.empty()) return;
        
        // Update prediction weights using episodic memory
        auto* episodicMemory = brain_->getEpisodicMemory();
        if (episodicMemory) {
            // Retrieve relevant episodes for learning
            std::vector<const EpisodicMemoryItem*> relevantEpisodes;
            if (!currentPattern.empty()) {
                relevantEpisodes = episodicMemory->retrieveSimilar(currentPattern, 5);
            }
            
            if (!relevantEpisodes.empty()) {
                // Update model weights based on successful pattern completion
                std::vector<float> gradient(pImpl->predictionWeights.size(), 0.0f);
                size_t weightIndex = 0;
                
                for (const auto* episode : relevantEpisodes) {
                    if (!episode->sensoryState.empty()) {
                        // Compute weight updates based on prediction accuracy
                        for (size_t i = 0; i < std::min(episode->sensoryState.size(), pImpl->predictionWeights.size()); ++i) {
                            if (weightIndex < gradient.size()) {
                                float prediction = episode->sensoryState[i];
                                float target = episode->resultingSensoryState.empty() ? 0.0f : episode->resultingSensoryState[i];
                                gradient[weightIndex] += (prediction - target) * currentPattern[i];
                            }
                        }
                        weightIndex++;
                    }
                }
                
                // Update weights with learning rate
                for (size_t i = 0; i < gradient.size(); ++i) {
                    pImpl->predictionWeights[i] -= pImpl->modelLearningRate * gradient[i];
                }
                
                // Add new patterns to weight matrix (expansion)
                size_t currentSize = pImpl->predictionWeights.size();
                if (currentSize < currentPattern.size()) {
                    pImpl->predictionWeights.resize(currentPattern.size(), 0.0f);
                }
            }
        }
    }
    
    // Also store this observation in episodic memory for future predictions
    if (brain_ && brain_->getEpisodicMemory()) {
        EpisodicMemoryItem episode;
        episode.timestamp = pImpl->predictionCount;  // Using prediction count as timestep
        episode.sensoryState = observation.getData();
        episode.action = ActionType::Wait;  // Default action
        episode.reward = 0.0f;
        episode.energy = 1.0f;
        episode.novelty = 0.0f;
        episode.resultingReward = 0.0f;
        
        // Add active neurons (simplified)
        episode.activeNeurons = workingMemory ? workingMemory->getMemoryNeurons() : std::vector<NeuronId>();
        episode.neuronActivations = workingMemory ? 
            std::vector<float>(workingMemory->getMemoryNeurons().size(), 0.5f) : 
            std::vector<float>();
        
        episode.age = 0;
        
        brain_->getEpisodicMemory()->storeEpisode(episode);
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
    pImpl->predictionCount = 0;
    pImpl->totalPredictionError = 0.0f;
}

} // namespace nlm
