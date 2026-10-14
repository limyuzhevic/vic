#include "PredictionSystem.hpp"
#include "NeuralPrediction.hpp"
#include "../brain/Brain.hpp"
#include "../memory/NeuralEpisodicMemory.hpp"
#include "../sensory/Vision.hpp"
#include "../sensory/Audio.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct PredictionSystem::Impl {
    // Use neural prediction for real forward models
    NeuralPrediction neuralPrediction;
    
    // Reference to brain for neural substrate access
    Brain* brain;
    
    // Episodic memory for experience-based training
    NeuralEpisodicMemory* episodicMemory;
    
    // Track prediction error for neuromodulation
    float predictionError;
    float confidence;
    std::vector<float> errorHistory;
    std::vector<std::pair<std::vector<float>, std::vector<float>>> predictionExamples;
    
    // Configuration
    size_t maxPredictionExamples;
    bool enableEpisodicTraining;
    float predictionLearningRate;
    
    Impl() : brain(nullptr), episodicMemory(nullptr), predictionError(0.0f), 
             confidence(0.5f), maxPredictionExamples(1000), 
             enableEpisodicTraining(true), predictionLearningRate(0.01f) {}
};

PredictionSystem::PredictionSystem() : pImpl(new Impl) {}

PredictionSystem::~PredictionSystem() = default;

// Initialize with brain reference for neural substrate access
void PredictionSystem::initialize(Brain* brain) {
    pImpl->brain = brain;
    pImpl->episodicMemory = brain->getEpisodicMemory();
    
    // Initialize neural prediction with brain reference
    pImpl->neuralPrediction.initialize(brain);
    
    // Configure neural prediction parameters
    pImpl->neuralPrediction.setSequenceMemorySize(100);
    pImpl->neuralPrediction.setPredictionHorizon(1);
    pImpl->neuralPrediction.enableTemporalPrediction(true);
    pImpl->neuralPrediction.enableActionConsequencePrediction(true);
}

// Store an outcome/sequence for prediction error computation
void PredictionSystem::recordOutcome(const std::vector<float>& sensoryState, float reward) {
    if (!pImpl->brain || sensoryState.empty()) {
        return;
    }
    
    // Create a vision-like input from the sensory state
    auto sensoryInput = std::make_unique<Vision>();
    sensoryInput->setData(sensoryState);
    
    // Use the observation to update predictions and learn from reward
    // This connects the prediction system to the episodic memory system
    if (pImpl->enableEpisodicTraining) {
        // Store the experience in the neural prediction system
        SimulationStep currentStep = pImpl->brain->getTotalSpikeCount();
        pImpl->neuralPrediction.recordSensoryState(sensoryState, currentStep);
        
        // Update prediction system with the observation and reward
        // This simulates the error-based learning from episodic experiences
        pImpl->neuralPrediction.updateWithObservation(sensoryState, currentStep);
    }
    
    // Also update the error history with reward-based prediction error
    // This provides a simple baseline for prediction error tracking
    float rewardBasedError = 1.0f - std::min(1.0f, std::max(0.0f, reward));
    pImpl->errorHistory.push_back(rewardBasedError);
    if (pImpl->errorHistory.size() > 10000) {
        pImpl->errorHistory.erase(pImpl->errorHistory.begin());
    }
    
    // Update prediction error based on reward
    pImpl->predictionError = rewardBasedError;
    pImpl->confidence = std::min(1.0f, reward); // Higher reward = higher confidence
}

// Generate prediction using neural substrate
std::unique_ptr<SensoryInput> PredictionSystem::predictNextState(const SensoryInput& currentState) {
    // Convert sensory input to vector format for neural prediction
    const auto& currentData = currentState.getData();
    
    // Generate prediction using brain's neural substrate
    std::vector<float> predictedVector = pImpl->neuralPrediction.generatePrediction(0);
    
    // Create a new sensory input with the predicted data
    // For now, use Vision as a generic sensory type
    auto predictedInput = std::make_unique<Vision>();
    
    // Try to preserve the type and dimensions from current state
    if (dynamic_cast<const Vision*>(&currentState)) {
        auto& visionInput = dynamic_cast<Vision&>(const_cast<SensoryInput&>(currentState));
        size_t width = visionInput.getWidth();
        size_t height = visionInput.getHeight();
        size_t channels = visionInput.getChannels();
        predictedInput->setData(predictedVector);
    } else if (dynamic_cast<const Audio*>(&currentState)) {
        auto& audioInput = dynamic_cast<Audio&>(const_cast<SensoryInput&>(currentState));
        size_t sampleRate = audioInput.getSampleRate();
        size_t numSamples = audioInput.getNumSamples();
        predictedInput->setData(predictedVector);
    } else {
        // For generic sensory input, use the vector directly
        predictedInput->setData(predictedVector);
    }
    
    return predictedInput;
}

// Update predictions with actual observation and learn from error
void PredictionSystem::updatePredictions(const SensoryInput& predicted, 
                                        const SensoryInput& actual) {
    // Extract data from inputs
    const auto& predictedData = predicted.getData();
    const auto& actualData = actual.getData();
    
    if (predictedData.empty() || actualData.empty()) {
        return;
    }
    
    // Compute prediction error using neural prediction system
    float error = pImpl->neuralPrediction.updateWithObservation(actualData, 0);
    
    // Update error statistics
    pImpl->predictionError = error;
    pImpl->confidence = std::max(0.0f, 1.0f - error);
    
    // Store prediction example for learning
    if (pImpl->predictionExamples.size() < pImpl->maxPredictionExamples) {
        pImpl->predictionExamples.push_back(
            std::make_pair(predictedData, actualData)
        );
    }
    
    // Add to error history
    pImpl->errorHistory.push_back(error);
    if (pImpl->errorHistory.size() > 10000) {
        pImpl->errorHistory.erase(pImpl->errorHistory.begin());
    }
    
    // Apply error to neural plasticity in prediction pathways
    if (pImpl->brain) {
        applyPredictionErrorToNeuralSubstrate(error);
    }
    
    // Learn from episodic memory if enabled
    if (pImpl->enableEpisodicTraining && pImpl->episodicMemory) {
        learnFromEpisodicMemory();
    }
}

// Train prediction model using stored experiences
void PredictionSystem::train(const SensoryInput& observation) {
    const auto& obsData = observation.getData();
    
    if (obsData.empty()) {
        return;
    }
    
    // Record this sensory state for temporal prediction
    if (pImpl->brain) {
        // Get current step for sequence learning
        SimulationStep currentStep = pImpl->brain->getTotalSpikeCount();
        pImpl->neuralPrediction.recordSensoryState(obsData, currentStep);
    }
    
    // Learn from episodic memory for action-consequence prediction
    if (pImpl->enableEpisodicTraining && pImpl->episodicMemory && 
        pImpl->episodicMemory->getEpisodeCount() > 0) {
        
        // Retrieve relevant episodes based on current sensory state
        auto relevantEpisodes = pImpl->episodicMemory->retrieveSimilar(obsData, 3);
        
        // Use retrieved episodes to improve prediction
        for (const auto* episode : relevantEpisodes) {
            // Learn from this episode's sensory sequence
            if (!episode->sensoryState.empty() && !episode->resultingSensoryState.empty()) {
                // Update neural prediction with this experience
                if (pImpl->brain) {
                    SimulationStep step = episode->timestamp;
                    pImpl->neuralPrediction.recordSensoryState(episode->sensoryState, step);
                    
                    // If there's an action, record it too
                    if (episode->action != ActionType::Wait) {
                        pImpl->neuralPrediction.recordAction(episode->action, step);
                    }
                }
            }
        }
    }
}

// Predict sensory consequences of an action
std::unique_ptr<SensoryInput> PredictionSystem::predictActionConsequence(
    ActionType action, const SensoryInput& currentState) {
    
    const auto& currentData = currentState.getData();
    
    // Use neural prediction system for action consequence prediction
    std::vector<float> predictedConsequence = 
        pImpl->neuralPrediction.predictActionConsequence(action, currentData);
    
    // Create sensory input from predicted consequence
    auto result = std::make_unique<Vision>();
    result->setData(predictedConsequence);
    
    return result;
}

// Get current prediction error
float PredictionSystem::getPredictionError() const {
    return pImpl->predictionError;
}

// Get prediction confidence
float PredictionSystem::getConfidence() const {
    return pImpl->confidence;
}

// Get prediction history
const std::vector<float>& PredictionSystem::getErrorHistory() const {
    return pImpl->errorHistory;
}

// Clear prediction history
void PredictionSystem::clearHistory() {
    pImpl->errorHistory.clear();
    pImpl->predictionExamples.clear();
    pImpl->neuralPrediction.clearHistory();
}

// Apply prediction error to neural substrate via neuromodulation
void PredictionSystem::applyPredictionErrorToNeuralSubstrate(float error) {
    if (!pImpl->brain) return;
    
    // Convert error to neuromodulatory signal
    float modulationSignal = std::min(1.0f, error * 2.0f);
    
    // Find and modulate prediction-related synapses
    modulatePredictionPathways(error, modulationSignal);
}

// Modulate prediction-related synapses based on error
void PredictionSystem::modulatePredictionPathways(float error, float modulation) {
    if (!pImpl->brain) return;
    
    // Get all neurons involved in prediction
    auto predictionNeurons = pImpl->neuralPrediction.getPredictionNeurons();
    auto sequenceNeurons = pImpl->neuralPrediction.getSequenceNeurons();
    
    // Combine all prediction-related neurons
    std::vector<NeuronId> allPredictionNeurons = predictionNeurons;
    allPredictionNeurons.insert(allPredictionNeurons.end(), 
                               sequenceNeurons.begin(), sequenceNeurons.end());
    
    if (allPredictionNeurons.empty()) {
        return; // No prediction neurons yet
    }
    
    // For each prediction neuron, find its synapses and modulate them
    for (const auto& region : pImpl->brain->getRegions()) {
        for (const auto& syn : region->getSynapses()) {
            // Check if this synapse is part of a prediction pathway
            bool isPredictionSynapse = false;
            
            // Check if synapse connects prediction-related neurons
            NeuronId sourceId = syn->getSourceNeuron();
            NeuronId targetId = syn->getDestinationNeuron();
            
            bool sourceIsPrediction = std::find(allPredictionNeurons.begin(), 
                                              allPredictionNeurons.end(), sourceId) != allPredictionNeurons.end();
            bool targetIsPrediction = std::find(allPredictionNeurons.begin(), 
                                               allPredictionNeurons.end(), targetId) != allPredictionNeurons.end();
            
            if (sourceIsPrediction || targetIsPrediction) {
                isPredictionSynapse = true;
            }
            
            if (isPredictionSynapse) {
                // Apply error-modulated plasticity to prediction synapses
                float eligibility = syn->getEligibilityTrace();
                
                // Prediction error modulates synaptic change
                float delta = eligibility * (modulation - 0.5f) * pImpl->predictionLearningRate;
                
                // Apply weight change
                float newWeight = syn->getWeight() + delta;
                newWeight = std::max(-1.0f, std::min(1.0f, newWeight)); // Clamp to [-1, 1]
                syn->setWeight(newWeight);
                
                // Decay eligibility trace
                syn->decayEligibilityTrace(0.1f);
            }
        }
    }
}

// Learn from episodic memory to improve predictions
void PredictionSystem::learnFromEpisodicMemory() {
    if (!pImpl->episodicMemory || pImpl->episodicMemory->getEpisodeCount() == 0) {
        return;
    }
    
    // Get recent episodes for learning
    auto recentEpisodes = pImpl->episodicMemory->getRecentEpisodes(10);
    
    // Build a temporal model from episodes
    std::vector<std::pair<std::vector<float>, std::vector<float>>> temporalTransitions;
    
    for (size_t i = 0; i < recentEpisodes.size(); ++i) {
        const auto* episode = recentEpisodes[i];
        
        if (episode && !episode->sensoryState.empty()) {
            // For each episode, create a temporal sequence
            if (i < recentEpisodes.size() - 1) {
                const auto* nextEpisode = recentEpisodes[i + 1];
                if (nextEpisode && !nextEpisode->sensoryState.empty()) {
                    temporalTransitions.push_back(
                        std::make_pair(episode->sensoryState, nextEpisode->sensoryState)
                    );
                }
            }
        }
    }
    
    // Update neural prediction with learned temporal sequences
    for (const auto& transition : temporalTransitions) {
        if (pImpl->brain) {
            SimulationStep step = (SimulationStep)(&transition - &temporalTransitions[0]);
            pImpl->neuralPrediction.learnTemporalSequence(
                transition.first, transition.second, step
            );
        }
    }
}

// Enable/disable episodic memory training
void PredictionSystem::enableEpisodicTraining(bool enable) {
    pImpl->enableEpisodicTraining = enable;
}

// Set maximum number of prediction examples
void PredictionSystem::setMaxPredictionExamples(size_t max) {
    pImpl->maxPredictionExamples = max;
}

// Set learning rate for prediction updates
void PredictionSystem::setPredictionLearningRate(float rate) {
    pImpl->predictionLearningRate = rate;
}

} // namespace nlm
