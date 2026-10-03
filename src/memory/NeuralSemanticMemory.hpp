// NeuralSemanticMemory: Stores facts and knowledge about the world
// NOT a database - semantic knowledge is encoded as neural connections and patterns
//
// Key mechanisms:
// - Facts stored as distributed neural representations
// - Knowledge learned through sensory-motor experience
// - Relations between facts form semantic networks
// - Prediction of sensory input based on knowledge

class NeuralSemanticMemory {
public:
    NeuralSemanticMemory();
    ~NeuralSemanticMemory();

    // Initialize with brain reference
    void initialize(Brain* brain);

    // Store a fact
    // Fact is encoded as neural pattern based on experience
    void storeFact(const std::string& key, const std::string& value, float confidence = 1.0f);

    // Retrieve fact
    std::string retrieveFact(const std::string& key) const;

    // Check if fact exists
    bool hasFact(const std::string& key) const;

    // Get all facts
    std::vector<std::pair<std::string, std::string>> getAllFacts() const;

    // Update fact confidence
    void updateFactConfidence(const std::string& key, float delta);

    // Get fact confidence
    float getFactConfidence(const std::string& key) const;

    // Clear fact
    void clearFact(const std::string& key);

    // Clear all facts
    void clear();

    // Get number of facts
    size_t getFactCount() const;

    // Learn from experience
    // Extract facts from sensory-motor sequences
    void learnFromExperience(const std::vector<float>& sensoryPattern,
                            const std::vector<float>& actionPattern,
                            float reward);

    // Predict next sensory state based on current state
    std::vector<float> predictNextSensory(const std::vector<float>& currentState) const;

    // Get knowledge graph
    // Returns connections between facts
    std::vector<std::tuple<std::string, std::string, float>> getKnowledgeGraph() const;

    // Add knowledge connection
    void addKnowledgeConnection(const std::string& fact1, const std::string& fact2, float strength);

    // Get facts similar to query
    std::vector<std::pair<std::string, std::string>> getSimilarFacts(const std::string& query) const;

    // Consolidate knowledge
    void consolidate(float relevanceThreshold);

    // Get semantic similarity between two facts
    float getSemanticSimilarity(const std::string& fact1, const std::string& fact2) const;

private:
    // Find or create fact neuron
    NeuronId findOrCreateFactNeuron(const std::string& key);

    // Create association between facts
    void createFactAssociation(const std::string& key1, const std::string& key2, float strength);

    struct Impl;
    std::unique_ptr<Impl> pImpl;

    Brain* brain_;
    std::unordered_map<std::string, float> factConfidence_;
    std::vector<std::tuple<std::string, std::string, float>> knowledgeGraph_;
};
