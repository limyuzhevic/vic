#include "CheckpointManager.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <filesystem>
#include <fstream>
#include <sstream>

namespace nlm {

CheckpointManager::CheckpointManager() : checkpointDir_("./checkpoints"), saveIntervalSteps_(10000), 
    maxCheckpoints_(10), compress_(true), lastSaveStep_(0) {
    // Create checkpoint directory if it doesn't exist
    std::filesystem::create_directories(checkpointDir_);
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
    // Create directory if it doesn't exist
    std::filesystem::create_directories(checkpointDir_);
}

bool CheckpointManager::update(uint64_t currentStep, double currentTime) {
    // Check if we should save a checkpoint
    if (shouldSave(currentStep)) {
        // Create checkpoint file with step number
        std::string checkpointPath = checkpointDir_ + "/checkpoint_step_" + 
            std::to_string(currentStep) + ".bin";
        return saveImmediately(checkpointPath);
    }
    return false;
}

bool CheckpointManager::saveImmediately(const std::string& name) {
    std::string filepath;
    if (name.empty()) {
        // Generate filename with timestamp and step
        auto now = std::chrono::system_clock::now();
        auto timestamp = std::chrono::system_clock::to_time_t(now);
        std::stringstream ss;
        ss << checkpointDir_ << "/checkpoint_" 
           << std::put_time(std::localtime(&timestamp), "%Y%m%d_%H%M%S") 
           << "_step_" << lastSaveStep_ << ".bin";
        filepath = ss.str();
    } else {
        filepath = name;
    }
    
    try {
        CheckpointWriter writer;
        if (!writer.create(filepath, compress_ ? CompressionLevel::Balanced : CompressionLevel::None)) {
            return false;
        }
        
        // Set metadata
        writer.setMetadata(0, 0, 0, lastSaveStep_, 0.0); // Will be filled by providers
        
        // Call neuron provider if set
        if (neuronProvider_) {
            NeuronCheckpointData neuronData;
            if (!neuronProvider_(neuronData)) {
                return false;
            }
            if (!writer.writeNeurons(neuronData)) {
                return false;
            }
        }
        
        // Call synapse provider if set
        if (synapseProvider_) {
            SynapseCheckpointData synapseData;
            if (!synapseProvider_(synapseData)) {
                return false;
            }
            if (!writer.writeSynapses(synapseData)) {
                return false;
            }
        }
        
        // Call brain provider if set for comprehensive checkpointing
        if (brainProvider_) {
            BrainCheckpointData brainData;
            if (!brainProvider_(brainData)) {
                return false;
            }
            // Write brain state to Checkpoint section
            if (!writer.writeSection(CheckpointSection::Neuromodulation, &brainData, sizeof(BrainCheckpointData))) {
                return false;
            }
        }
        
        if (!writer.finalize()) {
            return false;
        }
        
        lastSaveStep_ = 0; // Reset - will be set by update
        lastCheckpointPath_ = filepath;
        return true;
        
    } catch (const std::exception& e) {
        // Error handling
        return false;
    }
}

bool CheckpointManager::load(const std::string& name) {
    // Simple implementation - just check if file exists
    std::ifstream file(name);
    return file.good();
}

std::vector<CheckpointManager::CheckpointInfo> CheckpointManager::listCheckpoints() const {
    return std::vector<CheckpointInfo>(); // Placeholder
}

void CheckpointManager::pruneOldCheckpoints(size_t keepCount) {
    // Simple implementation - would actually delete old files
}

std::string CheckpointManager::getLastCheckpointPath() const {
    return lastCheckpointPath_;
}

bool CheckpointManager::shouldSave(uint64_t currentStep) const {
    return currentStep >= saveIntervalSteps_ && 
        currentStep > lastSaveStep_;
}

} // namespace nlm