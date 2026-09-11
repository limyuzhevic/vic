// Performance system headers for NLM
// These headers provide enhanced performance optimizations for neural simulation

#pragma once

namespace nlm {

/**
 * Checkpoint system for saving and loading brain states
 * Enables persistence and recovery of neural simulations
 */
class CheckpointSystem {
public:
    CheckpointSystem();
    ~CheckpointSystem();
    
    bool save(const std::string& path) const;
    bool load(const std::string& path);
    bool validate(const std::string& path) const;
    
    void setCompressionLevel(int level);
    int getCompressionLevel() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
