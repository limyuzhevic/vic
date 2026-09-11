namespace nlm {

// Episodic memory: storage of events and experiences
class NeuralEpisodicMemory {
public:
    NeuralEpisodicMemory();
    ~NeuralEpisodicMemory();
    
    // Store episode with structured data
    struct Episode {
        SimulationStep timestamp;
        std::string eventType;
        std::vector<NeuronId> neurons;
        std::vector<float> values;
        std::string metadata;
        
        Episode() : timestamp(0) {}
    };
    
    // Store episode
    void storeEpisode(const Episode& episode);
    
    // Retrieve episode by index
    Episode retrieveEpisode(size_t index) const;
    
    // Get episode count
    size_t getEpisodeCount() const;
    
    // Get recent episodes (limited)
    std::vector<Episode> getRecentEpisodes(size_t count) const;
    
    // Clear all
    void clear();
    
    // Memory consolidation to long-term storage
    void consolidate(float relevanceThreshold);
    
    // Find episodes by type
    std::vector<Episode> getEpisodesByType(const std::string& eventType) const;
    
    // Find episodes by metadata tag
    std::vector<Episode> getEpisodesByMetadata(const std::string& tag) const;
    
    // Get episodes in time range
    std::vector<Episode> getEpisodesInTimeRange(SimulationStep start, SimulationStep end) const;
    
    // Get average activation for episode type
    float getAverageActivation(const std::string& eventType) const;
};

} // namespace nlm
