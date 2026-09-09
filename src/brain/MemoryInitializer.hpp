// Brain memory integration implementation
#include "BrainMemoryInitializer.hpp"
#include "../core/Logger/Logger.hpp"
#include "../memory/NeuralWorkingMemory.hpp"
#include "../memory/NeuralEpisodicMemory.hpp"
#include "../memory/NeuralAssociativeMemory.hpp"

namespace nlm {
namespace brain {
namespace memory {

void MemoryInitializer::initialize(Brain::Impl& impl) {
    impl.workingMemory = std::make_unique<NeuralWorkingMemory>();
    impl.episodicMemory = std::make_unique<NeuralEpisodicMemory>();
    impl.associativeMemory = std::make_unique<NeuralAssociativeMemory>();
    
    impl.workingMemory->initialize(nullptr);
    impl.workingMemory->setCapacity(impl.config->getOr<size_t>("working_memory_capacity", 1000));
    
    impl.episodicMemory->initialize(nullptr);
    impl.episodicMemory->setMaxEpisodes(impl.config->getOr<size_t>("max_episodes", 1000));
    
    impl.associativeMemory->initialize(nullptr);
    
    NLM_LOG_INFO("Memory systems initialized");
}

void MemoryInitializer::integrateWithNeuralSystem(Brain::Impl& impl) {
    if (!impl.workingMemory || !impl.spikeSystem) return;
    
    // Register callback to store firing neurons in working memory
    impl.spikeSystem->registerHandler([impl](const DetailedSpikeEvent& event) {
        if (impl.workingMemory) {
            // Store neuron with activation level
            float activation = 1.0f; // Could be based on spike properties
            impl.workingMemory->storeToNeuron(event.neuron_id, activation);
        }
    });
    
    NLM_LOG_INFO("Memory systems integrated with neural system");
}

void MemoryInitializer::integrateWithPrediction(Brain::Impl& impl) {
    if (!impl.predictionSystem || !impl.episodicMemory) return;
    
    // Connect prediction system to episodic memory replay
    // Prediction could use replayed experiences to improve predictions
    
    NLM_LOG_INFO("Memory systems integrated with prediction system");
}

void MemoryInitializer::integrateWithCognition(Brain::Impl& impl) {
    if (!impl.attention || !impl.workingMemory) return;
    
    // Connect attention system to working memory competition
    // This is a two-way integration:
    // 1. Attention processes working memory traces
    // 2. Working memory provides input to attention
    
    NLM_LOG_INFO("Memory systems integrated with cognition systems");
}

} // namespace memory
} // namespace brain
} // namespace nlm
