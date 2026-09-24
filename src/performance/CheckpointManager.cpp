#include "CheckpointManager.hpp"
#include <algorithm>
#include <filesystem>
#include <stdexcept>

namespace nlm {

CheckpointManager::CheckpointManager() 
    : saveIntervalSteps_(10000)
    , maxCheckpoints_(10)
    , compress_(true)
    , lastSaveStep_(0)
    , nextCheckpointIndex_(0) {}

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
    std::filesystem::create_directories(checkpointDir_);
}

bool CheckpointManager::update(uint64_t currentStep, double currentTime) {
    if (!shouldSave(currentStep)) {
        return false;
    }
    
    return saveImmediately();
}

bool CheckpointManager::saveImmediately(const std::string& name) {
    if (!neuronProvider_ || !synapseProvider_) {
        return false;
    }
    
    std::string filename = name.empty() ? getNextCheckpointName() : name;
    std::string filepath = checkpointDir_ + "/" + filename;
    
    try {
        CheckpointWriter writer;
        if (!writer.create(filepath, compress_ ? CompressionLevel::Balanced : CompressionLevel::None)) {
            return false;
        }
        
        // Write metadata
        writer.setMetadata(0, 0, 0, 0, 0);  // TODO: Get actual counts from providers
        
        // Write neurons
        NeuronCheckpointData neuronData;
        if (!neuronProvider_(neuronData)) {
            writer.abort();
            return false;
        }
        
        if (!writer.writeNeurons(neuronData)) {
            writer.abort();
            return false;
        }
        
        // Write synapses
        SynapseCheckpointData synapseData;
        if (!synapseProvider_(synapseData)) {
            writer.abort();
            return false;
        }
        
        if (!writer.writeSynapses(synapseData)) {
            writer.abort();
            return false;
        }
        
        if (!writer.finalize()) {
            writer.abort();
            return false;
        }
        
        lastSaveStep_ = getCurrentStepFromFilename(filename);
        lastCheckpointPath_ = filepath;
        
        // Prune old checkpoints
        pruneOldCheckpoints(maxCheckpoints_);
        
        return true;
        
    } catch (const std::exception& e) {
        return false;
    }
}

bool CheckpointManager::load(const std::string& name) {
    std::string filepath = checkpointDir_ + "/" + name;
    
    if (!std::filesystem::exists(filepath)) {
        return false;
    }
    
    try {
        CheckpointReader reader;
        if (!reader.open(filepath)) {
            return false;
        }
        
        if (!reader.validate()) {
            return false;
        }
        
        // Read checkpoint data
        NeuronCheckpointData neuronData;
        if (!reader.readNeurons(neuronData)) {
            return false;
        }
        
        // TODO: Apply checkpoint data to brain
        // This would involve reconstructing brain state from checkpoint
        
        reader.close();
        lastCheckpointPath_ = filepath;
        
        return true;
        
    } catch (const std::exception& e) {
        return false;
    }
}

std::vector<CheckpointManager::CheckpointInfo> CheckpointManager::listCheckpoints() const {
    std::vector<CheckpointInfo> checkpoints;
    
    if (!std::filesystem::exists(checkpointDir_)) {
        return checkpoints;
    }
    
    for (const auto& entry : std::filesystem::directory_iterator(checkpointDir_)) {
        if (entry.is_regular_file() && entry.path().extension() == ".bin") {
            std::string filename = entry.path().filename().string();
            uint64_t step = getCurrentStepFromFilename(filename);
            
            CheckpointInfo info;
            info.name = filename;
            info.step = step;
            info.fileSize = entry.file_size();
            info.timestamp = std::filesystem::last_write_time(entry).time_since_epoch().count();
            
            checkpoints.push_back(info);
        }
    }
    
    // Sort by step (newest first)
    std::sort(checkpoints.begin(), checkpoints.end(),
              [](const CheckpointInfo& a, const CheckpointInfo& b) {
                  return a.step > b.step;
              });
    
    return checkpoints;
}

void CheckpointManager::pruneOldCheckpoints(size_t keepCount) {
    auto checkpoints = listCheckpoints();
    
    if (checkpoints.size() <= keepCount) {
        return;
    }
    
    // Delete oldest checkpoints
    size_t toDelete = checkpoints.size() - keepCount;
    for (size_t i = checkpoints.size() - 1; i >= checkpoints.size() - toDelete; --i) {
        std::string filepath = checkpointDir_ + "/" + checkpoints[i].name;
        std::filesystem::remove(filepath);
    }
}

bool CheckpointManager::shouldSave(uint64_t currentStep) const {
    return currentStep - lastSaveStep_ >= saveIntervalSteps_ &&
           currentStep > lastSaveStep_;
}

std::string CheckpointManager::getLastCheckpointPath() const {
    return lastCheckpointPath_;
}

uint64_t CheckpointManager::getCurrentStepFromFilename(const std::string& filename) const {
    // Extract step number from filename (e.g., "checkpoint_0001000.bin" -> 1000)
    size_t underscore = filename.find_last_of('_');
    if (underscore == std::string::npos) {
        return 0;
    }
    
    std::string stepStr = filename.substr(underscore + 1);
    size_t dot = stepStr.find_last_of('.');
    if (dot != std::string::npos) {
        stepStr = stepStr.substr(0, dot);
    }
    
    try {
        return std::stoull(stepStr);
    } catch (...) {
        return 0;
    }
}

std::string CheckpointManager::getNextCheckpointName() {
    std::string filename = "checkpoint_" + std::to_string(nextCheckpointIndex_) + ".bin";
    nextCheckpointIndex_++;
    return filename;
}

} // namespace nlm
