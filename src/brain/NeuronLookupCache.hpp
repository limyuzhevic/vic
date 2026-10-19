// Neuron lookup cache for O(1) access to neurons by ID
// This significantly improves performance for neuron-specific operations like injectCurrent()
// and spike delivery, reducing O(n) searches to O(1) hash lookups

struct NeuronLookupCache {
    // Main neuron lookup cache: maps NeuronId -> Neuron*
    std::unordered_map<uint64_t, Neuron*> neuronById;
    
    // Cache by neuron type for batch operations
    std::unordered_map<NeuronType, std::vector<Neuron*>> neuronsByType;
    
    // Cache by region for efficient region-based operations
    std::unordered_map<RegionId, std::vector<Neuron*>> neuronsByRegion;
    
    // Update the cache when neurons are added/removed or when brain state changes
    void updateCache(const std::vector<std::unique_ptr<NeuralRegion>>& regions) {
        neuronById.clear();
        neuronsByType.clear();
        neuronsByRegion.clear();
        
        for (const auto& region : regions) {
            neuronsByRegion[region->getId()].insert(neuronsByRegion[region->getId()].end(),
                std::make_move_iterator(region->getAllNeurons()->begin()),
                std::make_move_iterator(region->getAllNeurons()->end()));
            
            for (auto* neuron : *(region->getAllNeurons())) {
                neuronById[neuron->getId()] = neuron;
                neuronsByType[neuron->getType()].push_back(neuron);
            }
        }
    }
    
    // Get neuron by ID (O(1))
    Neuron* getNeuron(NeuronId id) {
        auto it = neuronById.find(id.getId());
        return (it != neuronById.end()) ? it->second : nullptr;
    }
    
    // Get neurons by type (O(1) to retrieve, O(k) to iterate)
    const std::vector<Neuron*>& getNeuronsByType(NeuronType type) {
        static const std::vector<Neuron*> emptyVector;
        auto it = neuronsByType.find(type);
        return (it != neuronsByType.end()) ? it->second : emptyVector;
    }
    
    // Get neurons by region (O(1) to retrieve, O(k) to iterate)
    const std::vector<Neuron*>& getNeuronsByRegion(RegionId regionId) {
        static const std::vector<Neuron*> emptyVector;
        auto it = neuronsByRegion.find(regionId.getId());
        return (it != neuronsByRegion.end()) ? it->second : emptyVector;
    }
    
    // Invalidate cache (called when neurons are added/removed or during reset)
    void invalidate() {
        neuronById.clear();
        neuronsByType.clear();
        neuronsByRegion.clear();
    }
};