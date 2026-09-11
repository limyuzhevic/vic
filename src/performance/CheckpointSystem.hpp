namespace nlm {

// Checkpoint system for brain state preservation and restoration
class CheckpointSystem {
public:
    CheckpointSystem();
    ~CheckpointSystem();
    
    // Create checkpoint with metadata
    bool createCheckpoint(const std::string& name, const std::string& description = "",
                         const std::string& tags = "", bool compressed = false);
    
    // Restore checkpoint by name
    bool restoreCheckpoint(const std::string& name);
    
    // Get list of checkpoints
    std::vector<std::string> listCheckpoints() const;
    
    // Get checkpoint metadata
    struct CheckpointInfo {
        std::string name;
        std::string description;
        std::string timestamp;
        std::string tags;
        size_t sizeBytes;
        bool compressed;
        std::map<std::string, std::string> metadata;
    };
    
    CheckpointInfo getCheckpointInfo(const std::string& name) const;
    
    // Delete checkpoint
    bool deleteCheckpoint(const std::string& name);
    
    // Create checkpoint from current brain state
    bool saveStateToCheckpoint(const std::string& name, const std::string& description = "");
    
    // Create differential checkpoint (changes since last checkpoint)
    bool createDifferentialCheckpoint(const std::string& name, const std::string& baseCheckpoint = "");
    
    // List available checkpoint tags
    std::vector<std::string> listTags() const;
    
    // Find checkpoints by tag
    std::vector<std::string> findCheckpointsByTag(const std::string& tag) const;
    
    // Checkpoint backup and restore operations
    bool backupToFile(const std::string& filepath, const std::string& checkpointName = "");
    bool restoreFromFile(const std::string& filepath, const std::string& checkpointName = "");
    
    // Checkpoint verification
    bool verifyCheckpoint(const std::string& name) const;
    std::string getCheckpointHash(const std::string& name) const;
    
    // Checkpoint compression
    void setCompressionLevel(int level); // 0-9
    int getCompressionLevel() const;
    
    // Checkpoint metadata inspection
    std::string getMetadata(const std::string& name, const std::string& key) const;
    void setMetadata(const std::string& name, const std::string& key, const std::string& value);
    std::map<std::string, std::string> getAllMetadata(const std::string& name) const;
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
