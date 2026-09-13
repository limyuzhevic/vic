#include "../performance/CheckpointSystem.hpp"
#include <fstream>
#include <filesystem>
#include <chrono>
#include <cstring>
#include <cmath>

namespace nlm {

CheckpointManager::CheckpointManager()
    : saveIntervalSteps_(10000)
    , maxCheckpoints_(10)
    , compress_(true)
    , lastSaveStep_(0)
{
}

CheckpointManager::~CheckpointManager() {
}

void CheckpointManager::configure(const std::string& checkpointDir, 
                                 uint64_t saveIntervalSteps,
                                 size_t maxCheckpoints,
                                 bool compress) {
    checkpointDir_ = checkpointDir;
    saveIntervalSteps_ = saveIntervalSteps;
    maxCheckpoints_ = maxCheckpoints;
    compress_ = compress;
    
    // Create checkpoint directory if it doesn't exist
    std::filesystem::create_directories(checkpointDir_);
}

bool CheckpointManager::update(uint64_t currentStep, double currentTime) {
    if (!shouldSave(currentStep)) {
        return true;  // Nothing to do
    }
    
    return saveImmediately();
}

bool CheckpointManager::saveImmediately(const std::string& name) {
    try {
        // Generate checkpoint filename
        std::string filename;
        if (!name.empty()) {
            filename = checkpointDir_ + "/" + name + ".nlmck";
        } else {
            filename = checkpointDir_ + "/checkpoint_step_" + std::to_string(lastSaveStep_) + ".nlmck";
        }
        
        CheckpointWriter writer;
        if (!writer.create(filename, compress_ ? CompressionLevel::Balanced : CompressionLevel::None)) {
            return false;
        }
        
        // Set metadata (in a real implementation, these would come from brain state)
        writer.setMetadata(0, 0, 0, lastSaveStep_, 0.0);
        
        // Write data using providers
        NeuronCheckpointData neuronData;
        SynapseCheckpointData synapseData;
        
        if (neuronProvider_ && !neuronProvider_(neuronData)) {
            return false;
        }
        
        if (synapseProvider_ && !synapseProvider_(synapseData)) {
            return false;
        }
        
        if (!writer.writeNeurons(neuronData)) {
            return false;
        }
        
        if (!writer.writeSynapses(synapseData)) {
            return false;
        }
        
        if (!writer.finalize()) {
            return false;
        }
        
        lastSaveStep_ = std::max(lastSaveStep_, lastSaveStep_);
        lastCheckpointPath_ = filename;
        
        return true;
        
    } catch (const std::exception& e) {
        return false;
    }
}

bool CheckpointManager::load(const std::string& name) {
    std::string filename;
    if (name.empty()) {
        // Try to load the most recent checkpoint
        auto checkpoints = listCheckpoints();
        if (checkpoints.empty()) {
            return false;
        }
        filename = checkpoints.back().name;
    } else {
        filename = checkpointDir_ + "/" + name + ".nlmck";
    }
    
    try {
        CheckpointReader reader;
        if (!reader.open(filename)) {
            return false;
        }
        
        if (!reader.validate()) {
            return false;
        }
        
        // In a real implementation, this would restore brain state from checkpoint
        // For now, just check if we can read the data
        auto neuronData = reader.readSection(CheckpointSection::Neurons);
        auto synapseData = reader.readSection(CheckpointSection::Synapses);
        
        return true;
        
    } catch (const std::exception& e) {
        return false;
    }
}

std::vector<CheckpointManager::CheckpointInfo> CheckpointManager::listCheckpoints() const {
    std::vector<CheckpointInfo> checkpoints;
    
    try {
        for (const auto& entry : std::filesystem::directory_iterator(checkpointDir_)) {
            if (entry.path().extension() == ".nlmck") {
                auto fileTime = std::filesystem::last_write_time(entry.path());
                CheckpointInfo info;
                info.name = entry.path().string();
                // Extract step from filename if possible
                info.step = lastSaveStep_;
                info.simulationTime = 0.0;
                info.neuronCount = 0;
                info.synapseCount = 0;
                info.fileSize = std::filesystem::file_size(entry.path());
                info.timestamp = std::chrono::system_clock::to_time_t(
                    std::chrono::system_clock::now()
                );
                checkpoints.push_back(info);
            }
        }
    } catch (const std::exception& e) {
        // Ignore errors reading directory
    }
    
    return checkpoints;
}

void CheckpointManager::pruneOldCheckpoints(size_t keepCount) {
    auto checkpoints = listCheckpoints();
    if (checkpoints.size() <= keepCount) {
        return;
    }
    
    // Sort by timestamp (oldest first)
    std::sort(checkpoints.begin(), checkpoints.end(),
              [](const CheckpointInfo& a, const CheckpointInfo& b) {
                  return a.timestamp < b.timestamp;
              });
    
    // Remove oldest checkpoints
    size_t toRemove = checkpoints.size() - keepCount;
    for (size_t i = 0; i < toRemove; ++i) {
        try {
            std::filesystem::remove(checkpoints[i].name);
        } catch (const std::exception& e) {
            // Ignore errors
        }
    }
}

std::string CheckpointManager::getLastCheckpointPath() const {
    return lastCheckpointPath_;
}

bool CheckpointManager::shouldSave(uint64_t currentStep) const {
    if (currentStep == 0) return false;
    return (currentStep - lastSaveStep_) >= saveIntervalSteps_;
}

} // namespace nlm
