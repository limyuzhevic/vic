#include "../performance/CheckpointSystem.hpp"
#include <filesystem>
#include <algorithm>
#include <iomanip>
#include <ctime>

namespace nlm {

CheckpointManager::CheckpointManager() : checkpointDir_(), saveIntervalSteps_(10000), 
                                         maxCheckpoints_(10), compress_(true), lastSaveStep_(0),
                                         lastCheckpointPath_(), neuronProvider_(), synapseProvider_() {}

CheckpointManager::~CheckpointManager() {
    // Cleanup
    for (const auto& checkpoint : checkpointInfos_) {
        std::filesystem::remove(checkpoint.name);
    }
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
    std::filesystem::path path(checkpointDir);
    if (!path.empty() && !std::filesystem::exists(path)) {
        std::filesystem::create_directories(path);
    }
    
    // Clean up old checkpoints
    pruneOldCheckpoints(maxCheckpoints);
}

void CheckpointManager::setNeuronProvider(std::function<bool(NeuronCheckpointData&)> provider) {
    neuronProvider_ = std::move(provider);
}

void CheckpointManager::setSynapseProvider(std::function<bool(SynapseCheckpointData&)> provider) {
    synapseProvider_ = std::move(provider);
}

bool CheckpointManager::update(uint64_t currentStep, double currentTime) {
    if (!shouldSave(currentStep)) {
        return false;
    }
    
    return saveImmediately(std::to_string(currentStep));
}

bool CheckpointManager::saveImmediately(const std::string& name) {
    if (!neuronProvider_ || !synapseProvider_) {
        error_ = "Neuron or synapse providers not set";
        return false;
    }
    
    try {
        // Create checkpoint writer
        CheckpointWriter writer;
        CompressionLevel compression = compress_ ? CompressionLevel::Balanced : CompressionLevel::None;
        
        if (!writer.create(getCheckpointPath(name), compression)) {
            error_ = writer.getError();
            return false;
        }
        
        // Get checkpoint data
        NeuronCheckpointData neuronData;
        SynapseCheckpointData synapseData;
        
        if (!neuronProvider_(neuronData) || !synapseProvider_(synapseData)) {
            error_ = "Failed to get checkpoint data";
            return false;
        }
        
        // Write data
        if (!writer.writeNeurons(neuronData)) {
            error_ = writer.getError();
            return false;
        }
        
        if (!writer.writeSynapses(synapseData)) {
            error_ = writer.getError();
            return false;
        }
        
        // Finalize checkpoint
        if (!writer.finalize()) {
            error_ = writer.getError();
            return false;
        }
        
        // Store checkpoint info
        CheckpointInfo info;
        info.name = name;
        info.step = lastSaveStep_;
        info.simulationTime = currentTime_;
        info.neuronCount = neuronData.membranePotential.size();
        info.synapseCount = synapseData.weight.size();
        info.fileSize = std::filesystem::file_size(getCheckpointPath(name));
        info.timestamp = std::time(nullptr);
        
        checkpointInfos_.push_back(info);
        lastCheckpointPath_ = getCheckpointPath(name);
        
        error_.clear();
        return true;
        
    } catch (const std::exception& e) {
        error_ = std::string("Exception in CheckpointManager::saveImmediately: ") + e.what();
        return false;
    }
}

bool CheckpointManager::load(const std::string& name) {
    std::string filepath = getCheckpointPath(name);
    
    // In a real implementation, this would load the checkpoint and restore brain state
    // For now, just check if file exists and is valid
    if (!std::filesystem::exists(filepath)) {
        error_ = "Checkpoint file not found: " + filepath;
        return false;
    }
    
    // Check if checkpoint is valid
    CheckpointReader reader;
    if (!reader.open(filepath)) {
        error_ = reader.getError();
        return false;
    }
    
    if (!reader.validate()) {
        error_ = "Invalid checkpoint: " + reader.getError();
        return false;
    }
    
    error_.clear();
    return true;
}

std::vector<CheckpointManager::CheckpointInfo> CheckpointManager::listCheckpoints() const {
    std::vector<CheckpointInfo> infos;
    
    if (checkpointDir_.empty() || !std::filesystem::exists(checkpointDir_)) {
        return infos;
    }
    
    for (const auto& entry : std::filesystem::directory_iterator(checkpointDir_)) {
        if (entry.is_regular_file()) {
            CheckpointInfo info;
            info.name = entry.path().filename().string();
            info.step = 0; // Would extract from filename
            info.simulationTime = 0.0; // Would extract from checkpoint
            info.neuronCount = 0; // Would extract from checkpoint
            info.synapseCount = 0; // Would extract from checkpoint
            info.fileSize = entry.file_size();
            info.timestamp = std::filesystem::last_write_time(entry).time_since_epoch().count();
            infos.push_back(info);
        }
    }
    
    return infos;
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
        const auto& checkpoint = checkpoints[i];
        std::filesystem::remove(checkpointDir_ + "/" + checkpoint.name);
        
        // Remove from checkpointInfos_
        checkpointInfos_.erase(std::remove_if(checkpointInfos_.begin(), checkpointInfos_.end(),
                                              [&checkpoint](const CheckpointInfo& info) {
                                                  return info.name == checkpoint.name;
                                              }),
                               checkpointInfos_.end());
    }
}

std::string CheckpointManager::getCheckpointPath(const std::string& name) const {
    std::filesystem::path path(checkpointDir_);
    path /= name;
    return path.string();
}

bool CheckpointManager::shouldSave(uint64_t currentStep) const {
    return currentStep > lastSaveStep_ && 
           (saveIntervalSteps_ == 0 || (currentStep - lastSaveStep_) >= saveIntervalSteps_);
}

} // namespace nlm
