#include "../src/performance/CheckpointSystem.hpp"
#include "../src/brain/Brain.hpp"
#include "../src/brain/Neuron.hpp"
#include "../src/brain/Synapse.hpp"
#include <filesystem>
#include <algorithm>
#include <chrono>
#include <numeric>

namespace nlm {

CheckpointManager::CheckpointManager() 
    : saveIntervalSteps_(10000)
    , maxCheckpoints_(10)
    , compress_(true)
    , lastSaveStep_(0)
    , checkpointDir_("./")
    , lastCheckpointPath_("") {
}

CheckpointManager::~CheckpointManager() = default;

void CheckpointManager::configure(const std::string& checkpointDir, 
                                 uint64_t saveIntervalSteps,
                                 size_t maxCheckpoints,
                                 bool compress) {
    checkpointDir_ = checkpointDir;
    saveIntervalSteps_ = saveIntervalSteps;
    maxCheckpoints_ = maxCheckpoints;
    compress_ = compress;
    
    // Create checkpoint directory if it doesn't exist
    std::error_code ec;
    if (!std::filesystem::exists(checkpointDir_, ec)) {
        std::filesystem::create_directories(checkpointDir_, ec);
    }
}

bool CheckpointManager::update(uint64_t currentStep, double currentTime) {
    lastSaveStep_ = currentStep;
    return true;
}

bool CheckpointManager::saveImmediately(const std::string& name) {
    std::string filepath = checkpointDir_ + "/" + name;
    if (name.empty()) {
        // Generate filename with timestamp and step
        auto now = std::chrono::system_clock::now();
        auto time = std::chrono::system_clock::to_time_t(now);
        std::stringstream ss;
        ss << "checkpoint_" << time << "_step" << lastSaveStep_ << ".bin";
        filepath = checkpointDir_ + "/" + ss.str();
    }
    
    if (neuronProvider_ && synapseProvider_) {
        // This is a simplified implementation
        // In a full implementation, this would use CheckpointWriter
        // For now, just create a placeholder file
        std::ofstream file(filepath, std::ios::binary);
        if (file.is_open()) {
            // Write minimal header
            uint64_t magic = 0x4E4C4D434B545430ULL;
            file.write(reinterpret_cast<const char*>(&magic), sizeof(magic));
            file.write(reinterpret_cast<const char*>(&lastSaveStep_), sizeof(lastSaveStep_));
            file.write(reinterpret_cast<const char*>(&currentTime), sizeof(currentTime));
            
            // Call providers to write data
            NeuronCheckpointData neuronData;
            SynapseCheckpointData synapseData;
            
            if (neuronProvider_(neuronData) && synapseProvider_(synapseData)) {
                file.close();
                lastCheckpointPath_ = filepath;
                return true;
            }
        }
    }
    
    return false;
}

bool CheckpointManager::load(const std::string& name) {
    std::string filepath = checkpointDir_ + "/" + name;
    if (name.empty() && !lastCheckpointPath_.empty()) {
        filepath = lastCheckpointPath_;
    }
    
    if (!std::filesystem::exists(filepath)) {
        return false;
    }
    
    std::ifstream file(filepath, std::ios::binary);
    if (!file.is_open()) {
        return false;
    }
    
    // Read header
    uint64_t magic = 0;
    uint64_t step = 0;
    double simTime = 0.0;
    
    file.read(reinterpret_cast<char*>(&magic), sizeof(magic));
    file.read(reinterpret_cast<char*>(&step), sizeof(step));
    file.read(reinterpret_cast<char*>(&simTime), sizeof(simTime));
    
    if (magic != 0x4E4C4D434B545430ULL) {
        return false;
    }
    
    file.close();
    return true;
}

std::vector<CheckpointManager::CheckpointInfo> CheckpointManager::listCheckpoints() const {
    std::vector<CheckpointInfo> checkpoints;
    
    std::error_code ec;
    if (!std::filesystem::exists(checkpointDir_, ec)) {
        return checkpoints;
    }
    
    for (const auto& entry : std::filesystem::directory_iterator(checkpointDir_, ec)) {
        if (!entry.is_regular_file(ec)) continue;
        
        CheckpointInfo info;
        info.name = entry.path().filename().string();
        info.fileSize = entry.file_size();
        
        // Extract step and time from filename if possible
        // This is a simplified implementation
        
        checkpoints.push_back(info);
    }
    
    // Sort by modification time (newest first)
    std::sort(checkpoints.begin(), checkpoints.end(),
        [](const CheckpointInfo& a, const CheckpointInfo& b) {
            std::error_code ec;
            auto a_time = std::filesystem::last_write_time(a.filepath_, ec);
            auto b_time = std::filesystem::last_write_time(b.filepath_, ec);
            return a_time > b_time;
        });
    
    return checkpoints;
}

void CheckpointManager::pruneOldCheckpoints(size_t keepCount) {
    auto checkpoints = listCheckpoints();
    if (checkpoints.size() <= keepCount) {
        return;
    }
    
    std::sort(checkpoints.begin(), checkpoints.end(),
        [](const CheckpointInfo& a, const CheckpointInfo& b) {
            std::error_code ec;
            auto a_time = std::filesystem::last_write_time(a.filepath_, ec);
            auto b_time = std::filesystem::last_write_time(b.filepath_, ec);
            return a_time < b_time;
        });
    
    for (size_t i = 0; i < checkpoints.size() - keepCount; ++i) {
        std::error_code ec;
        std::filesystem::remove(checkpoints[i].filepath_, ec);
    }
}

bool CheckpointManager::shouldSave(uint64_t currentStep) const {
    return currentStep >= lastSaveStep_ + saveIntervalSteps_;
}

}} // namespace nlm
