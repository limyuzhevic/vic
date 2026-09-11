// Memory System APIs
namespace nlm {

class WorkingMemory {
public:
    WorkingMemory();
    ~WorkingMemory();
    
    void store(NeuronId neuron, float value);
    float retrieve(NeuronId neuron) const;
    bool contains(NeuronId neuron) const;
    void clear();
    size_t getCapacity() const;
    size_t getCurrentSize() const;
    void decay(float decayRate);
    std::vector<std::pair<NeuronId, float>> getAllItems() const;
    float getMemoryPressure() const;
    void reset(size_t newCapacity);
};

class EpisodicMemory {
public:
    EpisodicMemory();
    ~EpisodicMemory();
    
    void storeEpisode(const std::string& event, const std::vector<NeuronId>& neurons, 
                     const std::vector<float>& values, const std::string& metadata = "");
    std::tuple<SimulationStep, std::vector<NeuronId>, std::vector<float>, std::string> 
        retrieveEpisode(size_t index) const;
    size_t getEpisodeCount() const;
    std::vector<std::tuple<SimulationStep, std::string>> getRecentEpisodes(size_t count) const;
    void clear();
    void consolidate(float relevanceThreshold);
    std::tuple<SimulationStep, std::vector<NeuronId>, std::vector<float>, std::string> 
        getEpisodeByEvent(const std::string& event) const;
    std::vector<std::tuple<SimulationStep, std::string, std::string>> 
        getEpisodesByMetadata(const std::string& tag) const;
};

class AssociativeMemory {
public:
    AssociativeMemory();
    ~AssociativeMemory();
    
    void associate(const std::string& conceptA, const std::string& conceptB, float strength);
    void associate(NeuronId neuronA, NeuronId neuronB, float strength);
    std::vector<std::string> getAssociations(const std::string& concept) const;
    std::vector<NeuronId> getAssociations(NeuronId neuron) const;
    float getAssociationStrength(const std::string& conceptA, const std::string& conceptB) const;
    float getAssociationStrength(NeuronId a, NeuronId b) const;
    void updateAssociation(const std::string& conceptA, const std::string& conceptB, float delta);
    void updateAssociation(NeuronId a, NeuronId b, float delta);
    void clear();
    bool removeAssociation(const std::string& conceptA, const std::string& conceptB);
    std::vector<std::tuple<std::string, std::string, float>> getAllAssociations() const;
    float computeSimilarity(const std::string& conceptA, const std::string& conceptB) const;
    float computeSimilarity(NeuronId a, NeuronId b) const;
};

} // namespace nlm
