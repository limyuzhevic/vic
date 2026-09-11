namespace nlm {

// Associative memory: relationships between representations
class NeuralAssociativeMemory {
public:
    NeuralAssociativeMemory();
    ~NeuralAssociativeMemory();
    
    // Concept-based association
    struct Concept {
        std::string name;
        std::vector<float> representation;
        std::vector<NeuronId> neuronPattern;
        
        Concept() {}
        Concept(const std::string& n) : name(n) {}
    };
    
    // Create association between concepts
    void associate(const Concept& conceptA, const Concept& conceptB, float strength);
    void associate(const std::string& conceptA, const std::string& conceptB, float strength);
    
    // Get associated concepts
    std::vector<std::string> getAssociations(const std::string& concept) const;
    std::vector<NeuronId> getAssociations(NeuronId neuron) const;
    
    // Get association strength
    float getAssociationStrength(const std::string& conceptA, const std::string& conceptB) const;
    float getAssociationStrength(const Concept& conceptA, const Concept& conceptB) const;
    
    // Update association
    void updateAssociation(const std::string& conceptA, const std::string& conceptB, float delta);
    
    // Compute similarity between concepts
    float computeSimilarity(const std::string& conceptA, const std::string& conceptB) const;
    float computeCosineSimilarity(const std::vector<float>& vecA, const std::vector<float>& vecB) const;
    
    // Get all concepts
    std::vector<Concept> getAllConcepts() const;
    
    // Create concept from neuron pattern
    Concept createConcept(const std::string& name, const std::vector<NeuronId>& neuronPattern);
    
    // Clear all
    void clear();
    
    // Remove specific association
    bool removeAssociation(const std::string& conceptA, const std::string& conceptB);
    
    // Get memory statistics
    struct MemoryStats {
        size_t totalConcepts;
        size_t totalAssociations;
        float averageStrength;
        float maxStrength;
        float minStrength;
    };
    
    MemoryStats getStats() const;
};

} // namespace nlm
