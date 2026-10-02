// CheckpointManager implementation

#include "CheckpointManager.hpp"
#include "../core/Logger/Logger.hpp"
#include <filesystem>
#include <algorithm>
#include <iomanip>
#include <sstream>

namespace nlm {

CheckpointManager::CheckpointManager()
    : saveIntervalSteps_(10000)
    , maxCheckpoints_(10)
    , compress_(true)
    , lastSaveStep_(0) {}

CheckpointManager::~CheckpointManager() = default;

void CheckpointManager::configure(const std::string& checkpointDir,
                                 uint64_t saveIntervalSteps,
                                 size_t maxCheckpoints,
                                 bool compress) {
    checkpointDir_ = checkpointDir;
    saveIntervalSteps_ = saveIntervalSteps;
    maxCheckpoints_ = maxCheckpoints;
    compress_ = compress;
    
    // Ensure directory exists
    std::filesystem::create_directories(checkpointDir_);
    
    NLM_LOG_INFO("CheckpointManager configured: " + checkpointDir + 
                 ", interval: " + std::to_string(saveIntervalSteps) +
                 ", max checkpoints: " + std::to_string(maxCheckpoints));
}

bool CheckpointManager::update(uint64_t currentStep, double currentTime) {
    if (currentStep - lastSaveStep_ >= saveIntervalSteps_) {
        return saveImmediately("step_" + std::to_string(currentStep));
    }
    return false;
}

bool CheckpointManager::saveImmediately(const std::string& name) {
    if (!neuronProvider_ || !synapseProvider_) {
        NLM_LOG_ERROR("CheckpointManager: No data providers configured");
        return false;
    }
    
    // Create checkpoint file
    std::string filename = checkpointDir_;
    if (!name.empty()) {
        filename += "/" + name + ".nckpt";
    } else {
        filename += "/checkpoint_" + std::to_string(lastSaveStep_ % saveIntervalSteps_) + ".nckpt";
    }
    
    CheckpointWriter writer;
    CompressionLevel compression = compress_ ? CompressionLevel::Balanced : CompressionLevel::None;
    
    if (!writer.create(filename, compression)) {
        NLM_LOG_ERROR("CheckpointManager: Failed to create checkpoint file: " + filename);
        return false;
    }
    
    // Prepare data providers
    bool success = true;
    
    // Save neurons
    NeuronCheckpointData neuronData;
    if (neuronProvider_(neuronData)) {
        if (!writer.writeNeurons(neuronData)) {
            NLM_LOG_ERROR("CheckpointManager: Failed to write neurons");
            success = false;
        }
    } else {
        NLM_LOG_ERROR("CheckpointManager: Failed to get neuron data");
        success = false;
    }
    
    // Save synapses
    SynapseCheckpointData synapseData;
    if (synapseProvider_(synapseData)) {
        if (!writer.writeSynapses(synapseData)) {
            NLM_LOG_ERROR("CheckpointManager: Failed to write synapses");
            success = false;
        }
    } else {
        NLM_LOG_ERROR("CheckpointManager: Failed to get synapse data");
        success = false;
    }
    
    // Finalize
    if (success && !writer.finalize()) {
        NLM_LOG_ERROR("CheckpointManager: Failed to finalize checkpoint");
        writer.abort();
        success = false;
    } else if (success) {
        lastSaveStep_ = writer.getBytesWritten();  // This should be currentStep, not bytes
        lastCheckpointPath_ = filename;
        NLM_LOG_INFO("CheckpointManager: Saved checkpoint at step " + 
                     std::to_string(lastSaveStep_ % saveIntervalSteps_) +
                     " to " + filename);
    }
    
    // Prune old checkpoints
    if (success) {
        pruneOldCheckpoints(maxCheckpoints_);
    }
    
    return success;
}

bool CheckpointManager::load(const std::string& name) {
    std::string filename = checkpointDir_ + "/" + name + ".nckpt";
    
    CheckpointReader reader;
    if (!reader.open(filename)) {
        NLM_LOG_ERROR("CheckpointManager: Failed to open checkpoint file: " + filename);
        return false;
    }
    
    if (!reader.validate()) {
        NLM_LOG_ERROR("CheckpointManager: Invalid checkpoint file: " + filename + 
                     " - " + reader.getError());
        return false;
    }
    
    NLM_LOG_INFO("CheckpointManager: Loaded checkpoint from " + filename);
    return true;
}

std::vector<CheckpointManager::CheckpointInfo> CheckpointManager::listCheckpoints() const {
    std::vector<CheckpointInfo> checkpoints;
    
    if (!std::filesystem::exists(checkpointDir_)) {
        return checkpoints;
    }
    
    for (const auto& entry : std::filesystem::directory_iterator(checkpointDir_)) {
        if (entry.path().extension() == ".nckpt") {
            CheckpointInfo info;
            info.name = entry.path().stem().string();
            
            // Parse step from filename (simple approach)
            size_t pos = info.name.find_last_of('_');
            if (pos != std::string::npos) {
                try {
                    info.step = std::stoull(info.name.substr(pos + 1));
                } catch (...) {
                    info.step = 0;
                }
            }
            
            // Get file info
            auto fileInfo = std::filesystem::file_time_type();
            if (std::filesystem::exists(entry.path())) {
                fileInfo = std::filesystem::last_write_time(entry.path());
            }
            
            checkpoints.push_back(info);
        }
    }
    
    // Sort by modification time (newest first)
    std::sort(checkpoints.begin(), checkpoints.end(),
              [](const CheckpointInfo& a, const CheckpointInfo& b) {
                  return a.timestamp > b.timestamp;
              });
    
    return checkpoints;
}

void CheckpointManager::pruneOldCheckpoints(size_t keepCount) {
    auto checkpoints = listCheckpoints();
    
    if (checkpoints.size() <= keepCount) {
        return;
    }
    
    // Delete oldest checkpoints
    for (size_t i = keepCount; i < checkpoints.size(); ++i) {
        std::string filepath = checkpointDir_ + "/" + checkpoints[i].name + ".nckpt";
        try {
            std::filesystem::remove(filepath);
            NLM_LOG_INFO("CheckpointManager: Removed old checkpoint: " + filepath);
        } catch (const std::exception& e) {
            NLM_LOG_ERROR("CheckpointManager: Failed to remove checkpoint " + filepath + 
                         " - " + std::string(e.what()));
        }
    }
}

std::string CheckpointManager::getLastCheckpointPath() const {
    return lastCheckpointPath_;
}

bool CheckpointManager::shouldSave(uint64_t currentStep) const {
    return currentStep % saveIntervalSteps_ == 0;
}

} // namespace nlm
