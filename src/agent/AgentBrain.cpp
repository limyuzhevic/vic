#include "AgentBrain.hpp"
#include "../sensory/SensoryInput.hpp"
#include "../brain/Brain.hpp"

namespace nlm {

AgentBrain::AgentBrain(std::shared_ptr<Config> config) : Brain(config) {
    // Initialize with sensory processors
    visionProcessor = std::make_unique<VisionProcessor>();
    audioProcessor = std::make_unique<AudioProcessor>();
    internalSignalsProcessor = std::make_unique<InternalSignalsProcessor>();
}

void AgentBrain::receiveSensoryInput(const SensoryInput& input) {
    // First call the base implementation for compatibility
    Brain::receiveSensoryInput(input);
    
    // Process through appropriate sensory processor based on input type
    if (input.getType() == std::string("Vision")) {
        const Vision& visionInput = static_cast<const Vision&>(input);
        visionProcessor->process(visionInput);
        
        // Get features and inject into brain's working memory
        const auto& features = visionProcessor->getFeatures();
        if (!features.empty()) {
            // Inject features as neural activity
            for (size_t i = 0; i < std::min<size_t>(features.size(), 10); ++i) {
                float featureValue = features[i];
                // Inject feature as current into internal neurons
                injectCurrentToNeurons(NeuronType::Internal, featureValue * 5.0f);
            }
        }
        
        // Update saccades based on visual features
        float reward = 0.0f;
        if (visionProcessor->getFeatureDimensions() > 0) {
            reward = visionProcessor->getFeatures()[0]; // First feature as simple reward signal
        }
        visionProcessor->updateSaccade(reward);
    } 
    else if (input.getType() == std::string("Audio")) {
        const Audio& audioInput = static_cast<const Audio&>(input);
        audioProcessor->process(audioInput);
        
        // Get features and inject into brain's working memory
        const auto& features = audioProcessor->getFeatures();
        if (!features.empty()) {
            for (size_t i = 0; i < std::min<size_t>(features.size(), 10); ++i) {
                float featureValue = features[i];
                injectCurrentToNeurons(NeuronType::Internal, featureValue * 3.0f);
            }
        }
        
        // Update reward modulation for audio
        float reward = 0.0f;
        if (features.size() > 5) {
            reward = features[5]; // Reward feature at index 5
        }
        audioProcessor->updateReward(reward);
    } 
    else if (input.getType() == std::string("InternalSignals")) {
        const InternalSignals& internalInput = static_cast<const InternalSignals&>(input);
        
        // Get homeostatic signals for brain's internal state
        const auto& homeostatic = internalInput.getHomeostaticSignals();
        if (!homeostatic.empty()) {
            // Inject into internal neurons to influence motivation
            for (size_t i = 0; i < std::min<size_t>(homeostatic.size(), 5); ++i) {
                float signalValue = homeostatic[i];
                injectCurrentToNeurons(NeuronType::Internal, signalValue * 2.0f);
            }
        }
        
        // Also pass through internal signals processor
        internalSignalsProcessor->process(internalInput);
    }
}

std::unique_ptr<Action> AgentBrain::produceAction() {
    // Get current brain state
    auto workingMemory = getWorkingMemory();
    auto dopamine = getDopamine();
    auto curiosity = getCuriosity();
    
    // Simple action selection based on integrated processing
    std::unique_ptr<Action> action = std::make_unique<Action>(ActionType::Wait);
    
    // Check if any sensory processor has generated features
    bool hasVisualInput = !visionProcessor->getFeatures().empty();
    bool hasAudioInput = !audioProcessor->getFeatures().empty();
    bool hasInternalSignals = !internalSignalsProcessor->getHomeostaticSignals().empty();
    
    // Decision making based on integrated inputs
    if (hasVisualInput && hasInternalSignals) {
        // Look for movement or interesting visual patterns
        float visualFeature = visionProcessor->getFeatures()[0];
        float internalState = internalSignalsProcessor->getHomeostaticSignals()[0];
        
        if (visualFeature > 0.7f && internalState > 0.5f) {
            // High visual interest and good internal state -> explore
            action = std::make_unique<Action>(ActionType::MoveForward);
        }
    }
    else if (hasAudioInput) {
        // Respond to audio if no other inputs
        float audioFeature = audioProcessor->getFeatures()[0];
        if (audioFeature > 0.8f) {
            // Salient sound -> turn towards sound
            action = std::make_unique<Action>(ActionType::RotateRight);
        }
    }
    
    // If nothing interesting, stay still
    return action;
}

void AgentBrain::logStatus() const {
    Brain::logStatus();
    
    std::cout << "=== SENSORY INTEGRATION STATUS ===\n";
    std::cout << "Vision features: " << visionProcessor->getFeatureDimensions() << "\n";
    std::cout << "Audio features: " << audioProcessor->getFeatureDimensions() << "\n";
    std::cout << "Internal signals: " << internalSignalsProcessor->getHomeostaticSignals().size() << "\n";
    
    if (!visionProcessor->getFeatures().empty()) {
        std::cout << "Last saccade: " << visionProcessor->getLastSaccadeTime() << "\n";
    }
}

} // namespace nlm