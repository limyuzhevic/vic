// Brain memory system implementation - working memory, episodic memory, associative memory
#include "Brain.hpp"
#include "../memory/NeuralWorkingMemory.hpp"
#include "../memory/NeuralEpisodicMemory.hpp"
#include "../memory/NeuralAssociativeMemory.hpp"

namespace nlm {

// Brain memory system implementation - working memory, episodic memory, associative memory

// Working memory methods
NeuralWorkingMemory* Brain::getWorkingMemory() {
    return pImpl->workingMemory.get();
}

// Episodic memory methods
NeuralEpisodicMemory* Brain::getEpisodicMemory() {
    return pImpl->episodicMemory.get();
}

// Associative memory methods
NeuralAssociativeMemory* Brain::getAssociativeMemory() {
    return pImpl->associativeMemory.get();
}

// Prediction system access
PredictionSystem* Brain::getPredictionSystem() {
    return pImpl->predictionSystem.get();
}

} // namespace nlm