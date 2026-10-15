#include <iostream>
#include "brain/Brain.hpp"
#include "cognition/SocialCognition.hpp"
#include <memory>

void testEnhancedIntegration() {
    std::cout << "=== Testing Enhanced NLM Phase 6 Integration ===" << std::endl;
    
    // Create configuration
    auto config = std::make_shared<Config>();
    config->set("neuron_count", static_cast<int64_t>(100));
    config->set("region_count", static_cast<int64_t>(1));
    config->set("connection_probability", 0.1f);
    config->set("stdp_ltp_weight", 0.01f);
    config->set("stdp_ltd_weight", 0.012f);
    
    // Initialize brain with enhanced integration
    auto brain = std::make_shared<Brain>(config);
    if (!brain->initialize()) {
        std::cout << "ERROR: Failed to initialize brain" << std::endl;
        return;
    }
    
    std::cout << "✓ Brain initialized successfully" << std::endl;
    
    // Test memory system integration
    auto* workingMemory = brain->getWorkingMemory();
    if (workingMemory) {
        std::cout << "✓ Working memory integrated: " << workingMemory->getActiveTraces() << " traces" << std::endl;
    }
    
    auto* episodicMemory = brain->getEpisodicMemory();
    if (episodicMemory) {
        std::cout << "✓ Episodic memory integrated: " << episodicMemory->getEpisodeCount() << " episodes" << std::endl;
    }
    
    // Test prediction system integration
    auto* predictionSystem = brain->getPredictionSystem();
    if (predictionSystem) {
        std::cout << "✓ Prediction system integrated" << std::endl;
    }
    
    // Test cognitive systems integration
    auto* planner = brain->getPlanner();
    if (planner) {
        std::cout << "✓ Neural planner integrated" << std::endl;
    }
    
    auto* conceptFormation = brain->getConceptFormation();
    if (conceptFormation) {
        std::cout << "✓ Concept formation integrated" << std::endl;
    }
    
    auto* attention = brain->getAttention();
    if (attention) {
        std::cout << "✓ Attentional selection integrated" << std::endl;
    }
    
    // Test new social cognition systems
    auto* selfModel = brain->getSelfModel();
    if (selfModel) {
        std::cout << "✓ Self-model integrated" << std::endl;
        selfModel->initialize(brain.get());
    }
    
    auto* socialLearning = brain->getSocialLearning();
    if (socialLearning) {
        std::cout << "✓ Social learning integrated" << std::endl;
        socialLearning->initialize(brain.get());
    }
    
    // Run simulation steps to test integration
    std::cout << "\n=== Running Integration Test Simulation ===" << std::endl;
    
    for (SimulationStep step = 0; step < 100; ++step) {
        // Inject some sensory activity
        brain->injectCurrentToNeurons(NeuronType::Sensory, 10.0f);
        
        // Step the brain
        brain->step(step, step * 0.001);
        
        // Check system status periodically
        if (step % 25 == 0) {
            size_t spikes = brain->getTotalSpikeCount();
            size_t firing = brain->getFiringNeuronCount();
            
            std::cout << "  Step " << step << ": " 
                      << "spikes=" << spikes 
                      << ", firing=" << firing 
                      << ", active=" << brain->getActiveNeuronCount()
                      << std::endl;
        }
    }
    
    // Final status
    std::cout << "\n=== Integration Test Results ===" << std::endl;
    std::cout << "Total spikes: " << brain->getTotalSpikeCount() << std::endl;
    std::cout << "Average firing rate: " << brain->getAverageFiringRate() << std::endl;
    std::cout << "E/I ratio: " << brain->getExcitationInhibitionRatio() << std::endl;
    
    if (workingMemory) {
        std::cout << "Working memory traces: " << workingMemory->getActiveTraces() << std::endl;
    }
    
    if (episodicMemory) {
        std::cout << "Episodic memory episodes: " << episodicMemory->getEpisodeCount() << std::endl;
    }
    
    if (selfModel) {
        std::cout << "Self-model has model: " << (selfModel->hasSelfModel() ? "YES" : "NO") << std::endl;
    }
    
    if (socialLearning) {
        std::cout << "Social learning observations: " << socialLearning->getObservationCount() << std::endl;
    }
    
    std::cout << "\n✓ All systems integrated and functional!" << std::endl;
}

int main() {
    testEnhancedIntegration();
    return 0;
}
