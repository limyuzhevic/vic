#pragma once

#include <string>
#include <memory>
#include <chrono>
#include "../../src/brain/Brain.hpp"

namespace nlm {

enum class CompressionLevel {
    None,
    Fast,
    Balanced,
    High
};

class StateManager {
public:
    StateManager();
    ~StateManager();
    
    bool saveBrainState(const std::string& filepath, CompressionLevel compression = CompressionLevel::Balanced);
    bool loadBrainState(const std::string& filepath);
    bool saveIncremental(const std::string& filepath, const std::string& tag);
    
    bool createCheckpoint(const std::string& tag, const std::string& description);
    bool restoreCheckpoint(const std::string& tag);
    std::vector<std::string> listCheckpoints();
    
    bool saveWorldState(const std::string& filepath);
    bool loadWorldState(const std::string& filepath);
    
    bool validateState(const std::string& expectedBrainHash, const std::string& expectedWorldHash);
    std::string compareStates(const std::string& filepath1, const std::string& filepath2);
    
    void enableVersionControl(bool enable);
    bool commitState(const std::string& message);
    std::vector<std::string> getStateHistory(size_t limit = 100);
    
    size_t getStateSize() const;
    float getCompressionRatio() const;
    std::map<std::string, float> getSaveLoadStatistics() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm