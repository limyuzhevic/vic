namespace nlm {

// Brain Checkpoint - Checkpoint and persistence
class BrainCheckpoint {
public:
    BrainCheckpoint();
    ~BrainCheckpoint();
    
    void initialize(BrainCore* brain);
    bool save(const std::string& filepath) const;
    bool load(const std::string& filepath);
    
    // Checkpoint management
    void setCheckpointDirectory(const std::string& directory) { checkpointDirectory_ = directory; }
    void setCheckpointInterval(size_t interval) { checkpointInterval_ = interval; }
    void setMaxCheckpoints(size_t maxCheckpoints) { maxCheckpoints_ = maxCheckpoints; }
    
    // State persistence
    bool saveNeuronState(std::ofstream& file) const;
    bool loadNeuronState(std::ifstream& file);
    bool saveSynapseState(std::ofstream& file) const;
    bool loadSynapseState(std::ifstream& file);
    bool saveSpikeState(std::ofstream& file) const;
    bool loadSpikeState(std::ifstream& file);
    
    // Integration
    void setBrainReference(BrainCore* brain) { brain_ = brain; }
    
    // Checkpoint statistics
    size_t getCurrentCheckpointStep() const { return currentCheckpointStep_; }
    uint64_t getTotalBytesWritten() const { return totalBytesWritten_; }
    bool isCheckpointingEnabled() const { return checkpointingEnabled_; }
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    BrainCore* brain_ = nullptr;
    
    std::string checkpointDirectory_ = "./checkpoints";
    size_t checkpointInterval_ = 10000;
    size_t maxCheckpoints_ = 5;
    bool checkpointingEnabled_ = true;
    size_t currentCheckpointStep_ = 0;
    uint64_t totalBytesWritten_ = 0;
};

} // namespace nlm
