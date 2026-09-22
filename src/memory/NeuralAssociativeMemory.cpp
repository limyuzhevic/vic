// NeuralAssociativeMemory implementation
struct NeuralAssociativeMemory::Impl {
    // Implementation details for associative memory
    std::vector<std::tuple<NeuronId, NeuronId, float>> associations;
    float noiseLevel;
    float maxMemoryDepth;
    
    Impl() : noiseLevel(0.1f), maxMemoryDepth(10.0f) {}
};

NeuralAssociativeMemory::NeuralAssociativeMemory()
    : pImpl(new Impl)
    , brain_(nullptr)
{
}

NeuralAssociativeMemory::~NeuralAssociativeMemory() = default;

void NeuralAssociativeMemory::initialize(Brain* brain) {
    pImpl = std::make_unique<Impl>();
    brain_ = brain;
}

void NeuralAssociativeMemory::associate(const std::vector<float>& patternA,
                                       const std::vector<float>& patternB,
                                       float strength) {
    if (!brain_ || patternA.empty() || patternB.empty()) return;
    
    // Find or create pattern neurons
    NeuronId patternANeuron = findPatternNeuron(patternA);
    NeuronId patternBNeuron = findPatternNeuron(patternB);
    
    if (patternANeuron != NeuronId() && patternBNeuron != NeuronId()) {
        // Add association
        pImpl->associations.emplace_back(patternANeuron, patternBNeuron, strength);
    }
}

void NeuralAssociativeMemory::associateFromExperience(const class EpisodicMemoryItem& episode) {
    if (!brain_) return;
    
    // Convert episode to pattern
    std::vector<float> pattern;
    pattern.reserve(episode.activeNeurons.size());
    for (const auto& neuronId : episode.activeNeurons) {
        float activation = 1.0f;
        if (!episode.neuronActivations.empty() && 
            episode.activeNeurons.size() > neuronId.index()) {
            activation = episode.neuronActivations[neuronId.index()];
        }
        pattern.push_back(activation);
    }
    
    // Create associations with action patterns
    if (pattern.size() > 0) {
        // Create self-association
        associate(pattern, pattern, 0.5f);
    }
}

std::vector<std::vector<float>> NeuralAssociativeMemory::retrieve(
    const std::vector<float>& queryPattern,
    size_t maxResults) const {
    std::vector<std::vector<float>> results;
    
    if (queryPattern.empty() || !brain_) return results;
    
    // Find pattern neuron
    NeuronId queryNeuron = findPatternNeuron(queryPattern);
    if (queryNeuron == NeuronId()) return results;
    
    // Retrieve associated patterns
    std::vector<std::tuple<NeuronId, NeuronId, float>> tempAssociations = pImpl->associations;
    for (const auto& assoc : tempAssociations) {
        if (std::get<0>(assoc) == queryNeuron || std::get<1>(assoc) == queryNeuron) {
            NeuronId otherNeuron = std::get<0>(assoc) == queryNeuron ? 
                std::get<1>(assoc) : std::get<0>(assoc);
            
            // Find the pattern corresponding to this neuron
            for (const auto& patternPair : patternNeurons_) {
                if (patternPair.first == otherNeuron) {
                    results.push_back(patternPair.second);
                    break;
                }
            }
            
            if (results.size() >= maxResults) break;
        }
    }
    
    return results;
}

float NeuralAssociativeMemory::getAssociationStrength(
    const std::vector<float>& patternA,
    const std::vector<float>& patternB) const {
    if (patternA.empty() || patternB.empty()) return 0.0f;
    
    NeuronId neuronA = findPatternNeuron(patternA);
    NeuronId neuronB = findPatternNeuron(patternB);
    
    if (neuronA == NeuronId() || neuronB == NeuronId()) return 0.0f;
    
    for (const auto& assoc : pImpl->associations) {
        if ((std::get<0>(assoc) == neuronA && std::get<1>(assoc) == neuronB) ||
            (std::get<0>(assoc) == neuronB && std::get<1>(assoc) == neuronA)) {
            return std::get<2>(assoc);
        }
    }
    
    return 0.0f;
}

void NeuralAssociativeMemory::updateAssociation(
    const std::vector<float>& patternA,
    const std::vector<float>& patternB,
    float delta) {
    if (patternA.empty() || patternB.empty()) return;
    
    NeuronId neuronA = findPatternNeuron(patternA);
    NeuronId neuronB = findPatternNeuron(patternB);
    
    if (neuronA == NeuronId() || neuronB == NeuronId()) return;
    
    for (auto& assoc : pImpl->associations) {
        if ((std::get<0>(assoc) == neuronA && std::get<1>(assoc) == neuronB) ||
            (std::get<0>(assoc) == neuronB && std::get<1>(assoc) == neuronA)) {
            std::get<2>(assoc) = std::clamp(std::get<2>(assoc) + delta, 0.0f, 1.0f);
            return;
        }
    }
}

void NeuralAssociativeMemory::clear() {
    pImpl->associations.clear();
    patternNeurons_.clear();
}

NeuronId NeuralAssociativeMemory::findPatternNeuron(const std::vector<float>& pattern) {
    if (!brain_ || pattern.empty()) return NeuronId();
    
    // Find neuron with similar activation pattern
    float bestSimilarity = 0.0f;
    NeuronId bestNeuron;
    
    for (const auto& patternPair : patternNeurons_) {
        float similarity = computeSimilarity(pattern, patternPair.second);
        if (similarity > bestSimilarity && similarity > 0.1f) {
            bestSimilarity = similarity;
            bestNeuron = patternPair.first;
        }
    }
    
    return bestNeuron;
}

float NeuralAssociativeMemory::computeSimilarity(const std::vector<float>& a,
                                                const std::vector<float>& b) const {
    if (a.size() != b.size() || a.empty()) return 0.0f;
    
    float dotProduct = 0.0f;
    float normA = 0.0f;
    float normB = 0.0f;
    
    for (size_t i = 0; i < a.size(); ++i) {
        dotProduct += a[i] * b[i];
        normA += a[i] * a[i];
        normB += b[i] * b[i];
    }
    
    if (normA > 0.0f && normB > 0.0f) {
        return dotProduct / std::sqrt(normA * normB);
    }
    
    return 0.0f;
}

} // namespace nlm