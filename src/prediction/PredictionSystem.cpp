#include "PredictionSystem.hpp"

namespace nlm {

struct PredictionSystem::Impl {
    // Neural systems for predictive coding
    Brain* brain;
    
    // Prediction state
    float predictionError;
    float confidence;
    std::vector<float> errorHistory;
    
    // Neural prediction components
    std::vector<NeuronId> predictionNeurons;
    std::vector<std::vector<float>> patternRepresentations;
    std::vector<std::pair<NeuronId, NeuronId>> learnedSequenceAssociations;
    
    // Prediction error neurons (specialized for error computation)
    NeuronId predictionErrorNeuron;
    NeuronId conceptMatchNeuron;
    NeuronId dopamineModulationNeuron;
    
    // Current prediction state
    std::vector<float> predictedSensoryPattern;
    
    // Prediction parameters
    float errorThreshold;
    float confidenceDecayRate;
    float predictionLearningRate;
    float attentionWeight;
    
    Impl() : brain(nullptr), predictionError(0.0f), confidence(0.5f), 
             errorThreshold(0.5f), confidenceDecayRate(0.95f), 
             predictionLearningRate(0.3f), attentionWeight(0.7f) {}
};

PredictionSystem::PredictionSystem() : pImpl(new Impl) {}

PredictionSystem::~PredictionSystem() = default;

std::unique_ptr<SensoryInput> PredictionSystem::predictNextState(const SensoryInput& currentState) {
    if (!pImpl->brain) {
        return currentState.clone();
    }
    
    // Extract current sensory pattern
    const auto& currentData = currentState.getData();
    pImpl->predictedSensoryPattern = currentData;
    
    // Use working memory to maintain prediction context
    auto workingMemory = pImpl->brain->getWorkingMemory();
    if (workingMemory) {
        // Get current working memory pattern
        auto wmPattern = workingMemory->retrieve();
        if (!wmPattern.empty()) {
            // Combine with sensory pattern for enhanced prediction
            for (size_t i = 0; i < std::min(currentData.size(), wmPattern.size()); ++i) {
                pImpl->predictedSensoryPattern[i] = 
                    currentData[i] * 0.3f + wmPattern[i] * 0.7f;
            }
        }
    }
    
    // Use concept formation to recognize patterns and predict
    auto conceptFormation = pImpl->brain->getConceptFormation();
    if (conceptFormation) {
        // Find matching concept
        size_t conceptId = conceptFormation->getMatchingConcept(pImpl->predictedSensoryPattern);
        if (conceptId > 0) {
            // Get concept prototype
            auto prototype = conceptFormation->getConceptPrototype(conceptId);
            if (!prototype.empty() && prototype.size() == currentData.size()) {
                // Use prototype to refine prediction
                for (size_t i = 0; i < currentData.size(); ++i) {
                    pImpl->predictedSensoryPattern[i] = prototype[i] * 0.8f + currentData[i] * 0.2f;
                }
            }
        }
    }
    
    // Use episodic memory to retrieve similar experiences
    auto episodicMemory = pImpl->brain->getEpisodicMemory();
    if (episodicMemory) {
        // Find similar episodes
        auto similarEpisodes = episodicMemory->retrieveSimilar(pImpl->predictedSensoryPattern);
        if (!similarEpisodes.empty()) {
            // Use successful prediction from episodic memory to improve prediction
            auto& firstEpisode = *similarEpisodes.front();
            if (!firstEpisode.resultingSensoryState.empty() && 
                firstEpisode.resultingSensoryState.size() == currentData.size()) {
                for (size comunicato) {
                    for (size_t i = 0; i < currentData.size(); ++i) {
                        pImpl->predictedSensoryPattern[i] = 
                            firstEpisode.resultingSensoryState[i] * 0.9f + currentData[i] * 0.1f;
                    }
                }
            }
        }
    }
    
    // Create prediction output (vision for now, would be dynamic based on actual type)
    auto predictedInput = std::make_unique<Vision>(currentData.size());
    predictedInput->setData(pImpl->predictedSensoryPattern);
    predictedInput->setTimestamp(currentState.getTimestamp() + 1.0);
    
    return predictedInput;
}

void PredictionSystem::updatePredictions(const SensoryInput& predicted, 
                                          const SensoryInput& actual) {
    if (!pImpl->brain) return;
    
    // Compute prediction error using neural mechanisms
    const auto& predData = predicted.getData();
    const auto& actualData = actual.getData();
    
    // Calculate error using prediction error neurons
    pImpl->predictionError = 0.0f;
    float totalSquaredError = 0.0f;
    
    if (predData.size() == actualData.size() && !predData.empty()) {
        for (size_t i = 0; i < predData.size(); ++i) {
            float error = actualData[i] - predData[i];
            totalSquaredError += error * error;
            
            // Update prediction error neuron activity
            if (pImpl->predictionErrorNeuron != INVALID_NEURON_ID) {
                auto* neuron = pImpl->brain->getRegion(pImpl->predictionErrorNeuron % 1000)->getPopulation(pImpl->predictionErrorNeuron % 100)->getNeuron(pImpl->predictionErrorNeuron);
                if (neuron) {
                    neuron->injectCurrent(error * 10.0f); // Scale error for neural response
                }
            }
        }
        
        pImpl->predictionError = std::sqrt(totalSquaredError) / predData.size();
        pImpl->errorHistory.push_back(pImpl->predictionError);
    }
    
    // Update confidence based on prediction accuracy and neuromodulation
    float oldConfidence = pImpl->confidence;
    pImpl->confidence = 1.0f - pImpl->predictionError;
    
    // Apply neuromodulation (dopamine)
    auto dopamine = pImpl->brain->getDopamine();
    if (dopamine) {
        // Signal reward prediction error
        float reward = 1.0f - pImpl->predictionError;
        dopamine->signalRewardPredictionError(pImpl->predictionError);
    }
    
    // Use concept formation for attention modulation
    auto conceptFormation = pImpl->brain->getConceptFormation();
    if (conceptFormation) {
        size_t conceptId = conceptFormation->getMatchingConcept(pImpl->predictedSensoryPattern);
        if (conceptId > 0) {
            // Focus attention on this concept
            auto* conceptNeuron = pImpl->brain->getRegion(1)->getPopulation(0)->getNeuron(conceptId);
            if (conceptNeuron) {
                conceptNeuron->injectCurrent(1.0f);
            }
        }
    }
    
    // Store prediction error in episodic memory for future learning
    auto episodicMemory = pImpl->brain->getEpisodicMemory();
    if (episodicMemory) {
        // Create memory item for this prediction error
        nlm::EpisodicMemoryItem errorItem;
        errorItem.timestamp = pImpl->brain->getCurrentStep();
        errorItem.sensoryState = predData;
        errorItem.resultingSensoryState = actualData;
        errorItem.reward = 1.0f - pImpl->predictionError;
        errorItem.action = nlm::ActionType::Wait; // Prediction is internal action
        episodicMemory->storeEpisode(errorItem);
    }
    
    // Decay confidence over time
    pImpl->confidence *= pImpl->confidenceDecayRate;
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
    if (!pImpl->brain) return;
    
    const auto& sensoryData = observation.getData();
    
    // Replay episodic memories to strengthen prediction pathways
    auto episodicMemory = pImpl->brain->getEpisodicMemory();
    if (episodicMemory) {
        // Get episodes suitable for replay
        auto episodesForReplay = episodicMemory->getEpisodesForReplay(10);
        
        for (auto* episode : episodesForReplay) {
            if (!episode) continue;
            
            // Replay the episode to reactivate prediction pathways
            if (episode->action != nlm::ActionType::Wait) {
                // For each experience, replay the sensory pattern
                // This strengthens the association between input patterns
                // and their predicted outcomes
                
                // Update concept formation with new experience
                auto conceptFormation = pImpl->brain->getConceptFormation();
                if (conceptFormation) {
                    // Extract features from the episode
                    std::vector<float> features;
                    for (size_t i = 0; i < std::min(episode->sensoryState.size(), static_cast<size_t>(10)); ++i) {
                        features.push_back(episode->sensoryState[i]);
                    }
                    
                    conceptFormation->presentExperience(
                        episode->sensoryState, 
                        features, 
                        episode->reward,
                        episode->timestamp
                    );
                }
                
                // Update associative memory
                auto associativeMemory = pImpl->brain->getAssociativeMemory();
                if (associativeMemory) {
                    // Learn association between sensory input and outcome
                    if (!episode->resultingSensoryState.empty() && 
                        episode->sensoryState.size() == episode->resultingSensoryState.size()) {
                        associativeMemory->associate(
                            episode->sensoryState,
                            episode->resultingSensoryState,
                            episode->reward
                        );
                    }
                }
            }
        }
    }
    
    // Use working memory for pattern completion training
    auto workingMemory = pImpl->brain->getWorkingMemory();
    if (workingMemory) {
        // Store observation in working memory
        workingMemory->store(sensoryData, pImpl->predictionLearningRate);
        
        // Rehearse the pattern to strengthen prediction pathways
        workingMemory->strengthenMemory(pImpl->predictionLearningRate);
    }
    
    // Apply prediction error-based learning
    if (pImpl->predictionErrorNeuron != INVALID_NEURON_ID && pImpl->predictionError > 0.1f) {
        auto* errorNeuron = pImpl->brain->getRegion(pImpl->predictionErrorNeuron % 1000)->getPopulation(pImpl->predictionErrorNeuron % 100)->getNeuron(pImpl->predictionErrorNeuron);
        if (errorNeuron) {
            // High prediction error triggers learning
            errorNeuron->injectCurrent(pImpl->predictionError * 20.0f);
        }
    }
    
    // Update dopamine-based reward prediction
    auto dopamine = pImpl->brain->getDopamine();
    if (dopamine) {
        float reward = 1.0f - pImpl->predictionError;
        if (reward < 0.0f) reward = 0.0f;
        dopamine->signalReward(reward);
    }
}

} // namespace nlm