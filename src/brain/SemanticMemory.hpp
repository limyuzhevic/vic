#include "Brain.hpp"

namespace nlm {

// Forward declarations for semantic and procedural memory
class NeuralSemanticMemory;
class NeuralProceduralMemory;

// Add SemanticMemory forward declaration
NeuralSemanticMemory* Brain::getSemanticMemory() {
    return pImpl->semanticMemory.get();
}

// Add ProceduralMemory forward declaration
NeuralProceduralMemory* Brain::getProceduralMemory() {
    return pImpl->proceduralMemory.get();
}

} // namespace nlm
