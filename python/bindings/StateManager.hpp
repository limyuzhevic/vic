#pragma once

#include <string>
#include <vector>
#include <memory>

namespace nlm {

class StateManager {
public:
    StateManager();
    ~StateManager();
    
    bool saveBrainState(const std::string& filepath);
    bool loadBrainState(const std::string& filepath);
    bool saveIncremental(const std::string& filepath, const std::string& tag);
    bool createCheckpoint(const std::string& tag);
    bool restoreCheckpoint(const std::string& tag);
    std::vector<std::string> listCheckpoints();
    bool validateState(const std::string& expectedHash);
    size_t getStateSize() const;
    float getCompressionRatio() const;
    std::map<std::string, float> getSaveLoadStatistics() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm