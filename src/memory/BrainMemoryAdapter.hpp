// BrainMemoryAdapter - Provides clean interface for brain-memory interaction
// Adapter pattern: Converts between memory system interfaces and brain operations
class BrainMemoryAdapter : public std::enable_shared_from_this<BrainMemoryAdapter> {
public:
    virtual ~BrainMemoryAdapter() = default;
    
    // Inject current into neuron via brain
    virtual void injectCurrent(Brain* brain, NeuronId neuron, MembranePotential current) = 0;
    
    // Get neuron by ID from brain
    virtual Neuron* getNeuron(Brain* brain, NeuronId neuronId) = 0;
    
    // Get region from brain
    virtual NeuralRegion* getRegion(Brain* brain, RegionId regionId) = 0;
    
    // Get all neurons in region
    virtual std::vector<Neuron*> getNeurons(NeuralRegion* region) = 0;
    
    // Get brain reference
    virtual Brain* getBrain() const = 0;
    
    // Set brain reference
    virtual void setBrain(Brain* brain) = 0;
};

// Default implementation of BrainMemoryAdapter
class DefaultBrainMemoryAdapter : public BrainMemoryAdapter {
public:
    DefaultBrainMemoryAdapter() : brain_(nullptr) {}
    
    void injectCurrent(Brain* brain, NeuronId neuron, MembranePotential current) override {
        brain->injectCurrent(neuron, current);
    }
    
    Neuron* getNeuron(Brain* brain, NeuronId neuronId) override {
        if (!brain) return nullptr;
        for (auto& region : brain->getRegions()) {
            auto neurons = region->getAllNeurons();
            for (auto* neuron : neurons) {
                if (neuron->getId() == neuronId) {
                    return neuron;
                }
            }
        }
        return nullptr;
    }
    
    NeuralRegion* getRegion(Brain* brain, RegionId regionId) override {
        if (!brain) return nullptr;
        return brain->getRegion(regionId);
    }
    
    std::vector<Neuron*> getNeurons(NeuralRegion* region) override {
        if (!region) return std::vector<Neuron*>();
        return region->getAllNeurons();
    }
    
    Brain* getBrain() const override {
        return brain_;
    }
    
    void setBrain(Brain* brain) override {
        brain_ = brain;
    }

private:
    Brain* brain_;
};

} // namespace nlm
