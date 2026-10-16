#include "CheckpointSystem.hpp"
#include <iostream>
#include <fstream>
#include <filesystem>
#include <algorithm>
#include <chrono>
#include <sstream>

namespace nlm {

CheckpointManager::CheckpointManager()
    : saveIntervalSteps_(10000)
    , maxCheckpoints_(10)
    , compress_(true)
    , lastSaveStep_(0)
{
}

CheckpointManager::~CheckpointManager() {
    // Clean up any remaining checkpoint files
    pruneOldCheckpoints(0);  // Delete all
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
    if (currentStep < lastSaveStep_ + saveIntervalSteps_) {
        return false;  // Not yet time to save
    }
    
    return saveImmediately();
}

bool CheckpointManager::saveImmediately(const std::string& name) {
    std::string filepath;
    if (name.empty()) {
        // Generate filename with step and timestamp
        auto now = std::chrono::system_clock::now();
        auto timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(
            now.time_since_epoch()).count();
        std::stringstream ss;
        ss << checkpointDir_ << "/checkpoint_step_" << lastSaveStep_ 
           << "_time_" << timestamp << ".nlm";
        filepath = ss.str();
    } else {
        filepath = checkpointDir_ + "/" + name;
    }
    
    // Create checkpoint writer and save
    CheckpointWriter writer;
    if (!writer.create(filepath, compress_ ? CompressionLevel::Balanced : CompressionLevel::None)) {
        return false;
    }
    
    // Call neuron provider if set
    if (neuronProvider_) {
        NeuronCheckpointData neuronData;
        if (neuronProvider_(neuronData)) {
            writer.setMetadata(
                neuronData.membranePotential.size(),
                0,  // Synapse count
                0,  // Region count
                lastSaveStep_,
                0.0  // Simulation time
            );
            
            if (!writer.writeNeurons(neuronData)) {
                writer.abort();
                return false;
            }
        }
    }
    
    // Call synapse provider if set
    if (synapseProvider_) {
        SynapseCheckpointData synapseData;
        if (synapseProvider_(synapseData)) {
            if (!writer.writeSynapses(synapseData)) {
                writer.abort();
                return false;
            }
        }
    }
    
    // Finalize the checkpoint
    if (!writer.finalize()) {
        writer.abort();
        return false;
    }
    
    lastCheckpointPath_ = filepath;
    lastSaveStep_ = currentStep;
    
    return true;
}

bool CheckpointManager::load(const std::string& name) {
    std::string filepath;
    if (name.find(checkpointDir_) == 0) {
        filepath = name;
    } else {
        filepath = checkpointDir_ + "/" + name;
    }
    
    // Create checkpoint reader and load
    CheckpointReader reader;
    if (!reader.open(filepath)) {
        return false;
    }
    
    // Validate checkpoint
    if (!reader.validate()) {
        return false;
    }
    
    // For now, just log the checkpoint info
    // In a real implementation, you would call the synapse provider
    // to populate synapse data
    
    return true;
}

std::vector<CheckpointManager::CheckpointInfo> CheckpointManager::listCheckpoints() const {
    std::vector<CheckpointInfo> checkpoints;
    
    if (!std::filesystem::exists(checkpointDir_)) {
        return checkpoints;
    }
    
    for (const auto& entry : std::filesystem::directory_iterator(checkpointDir_)) {
        if (entry.is_regular_file() && entry.path().extension() == ".nlm") {
            CheckpointInfo info;
            info.name = entry.path().filename().string();
            
            // Extract step from filename if possible
            std::string stem = entry.path().stem().string();
            size_t pos = stem.find("step_");
            if (pos != std::string::npos) {
                std::string stepStr = stem.substr(pos + 5);
                pos = stepStr.find('_');
                if (pos != std::string::npos) {
                    stepStr = stepStr.substr(0, pos);
                }
                try {
                    info.step = std::stoul(stepStr);
                } catch (...) {
                    info.step = 0;
                }
            }
            
            info.fileSize = entry.file_size();
            info.timestamp = std::chrono::system_clock::to_time_t(
                std::chrono::system_clock::now());
            
            checkpoints.push_back(info);
        }
    }
    
    // Sort by timestamp (most recent first)
    std::sort(checkpoints.begin(), checkpoints.end(),
              [](const CheckpointInfo& a, const CheckpointInfo& b) {
                  return a.timestamp > b.timestamp;
              });
    
    return checkpoints;
}

void CheckpointManager::pruneOldCheckpoints(size_t keepCount) {
    auto checkpoints = listCheckpoints();
    
    if (keepCount >= checkpoints.size()) {
        return;  // No pruning needed
    }
    
    // Delete excess checkpoints
    size_t deleteCount = checkpoints.size() - keepCount;
    for (size_t i = checkpoints.size() - 1; i >= 0 && deleteCount > 0; --i) {
        std::string filepath = checkpointDir_ + "/" + checkpoints[i].name;
        if (std::filesystem::remove(filepath)) {
            --deleteCount;
        }
    }
}

std::string CheckpointManager::getLastCheckpointPath() const {
    return lastCheckpointPath_;
}

bool CheckpointManager::shouldSave(uint64_t currentStep) const {
    return currentStep >= lastSaveStep_ + saveIntervalSteps_;
}

} // namespace nlm