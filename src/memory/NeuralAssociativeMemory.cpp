#include "NeuralAssociativeMemory.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

struct NeuralAssociativeMemory::Impl {
    std::unordered_map<std::vector<NeuronId>, float> patterns;
    float decayRate;
    
    Impl() : decayRate(0.95f) {}
};

NeuralAssociativeMemory::NeuralAssociativeMemory() : pImpl(new Impl()) {}

NeuralAssociativeMemory::~NeuralAssociativeMemory() {
    delete pImpl;
}

bool NeuralAssociativeMemory::initialize(class Brain* brain) {
    NLM_LOG_INFO("Initializing NeuralAssociativeMemory...");
    return true;
}

void NeuralAssociativeMemory::clear() {
    pImpl->patterns.clear();
}

void NeuralAssociativeMemory::storePattern(const std::vector<NeuronId>& pattern, float strength) {
    pImpl->patterns[pattern] = strength;
}

float NeuralAssociativeMemory::retrievePattern(const std::vector<NeuronId>& pattern) const {
    auto it = pImpl->patterns.find(pattern);
    if (it != pImpl->patterns.end()) {
        return it->second;
    }
    return 0.0f;
}

size_t NeuralAssociativeMemory::getPatternCount() const {
    return pImpl->patterns.size();
}

} // namespace nlm