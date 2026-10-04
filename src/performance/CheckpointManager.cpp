// CheckpointManager.cpp - Implementation of checkpoint manager for NLM
// 
// This file provides concrete implementations for the CheckpointManager
// class declared in CheckpointSystem.hpp. It includes functionality for
// automated checkpointing, saving, loading, and managing brain simulation
// state during long-running neural network experiments.

#include "CheckpointManager.hpp"
#include "../core/Logger/Logger.hpp"
#include <filesystem>
#include <algorithm>
#include <chrono>
#include <sstream>
#include <iomanip>

namespace nlm {

// CheckpointManager implementation
// 
CheckpointManager::CheckpointManager()
    : checkpointDir_(".")
    , saveIntervalSteps_(10000)
    , maxCheckpoints_(10)
    , compress_(true)
    , lastSaveStep_(0) {
    // Create checkpoint directory if it doesn't exist
    std::error_code ec;
    std::filesystem::create_directories(checkpointDir_, ec);
    if (ec) {
        NLM_LOG_WARNING("Failed to create checkpoint directory: " + checkpointDir_);
    }
}

CheckpointManager::~CheckpointManager() {
    // Ensure clean shutdown
    if (stream_.is_open()) {
        stream_.close();
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
    
    // Ensure directory exists
    std::error_code ec;
    std::filesystem::create_directories(checkpointDir_, ec);
    if (ec) {
        NLM_LOG_ERROR("Failed to create checkpoint directory: " + checkpointDir_);
    } else {
        NLM_LOG_INFO("Checkpoint manager configured: " + checkpointDir_ + 
                    ", interval=" + std::to_string(saveIntervalSteps_) + 
                    ", maxCheckpoints=" + std::to_string(maxCheckpoints_) +
                    ", compress=" + std::to_string(compress_));
    }
}

bool CheckpointManager::update(uint64_t currentStep, double currentTime) {
    if (!shouldSave(currentStep)) {
        return false;
    }
    
    return saveImmediately();
}

bool CheckpointManager::saveImmediately(const std::string& name) {
    if (!neuronProvider_ || !synapseProvider_) {
        NLM_LOG_ERROR("Checkpoint providers not configured");
        return false;
    }
    
    std::string checkpointName = name;
    if (checkpointName.empty()) {
        checkpointName = "checkpoint_" + std::to_string(lastSaveStep_);
    }
    
    std::string filepath = checkpointDir_ + "/" + checkpointName + ".nlmcp";
    
    try {
        CheckpointWriter writer;
        if (!writer.create(filepath, compress_ ? CompressionLevel::Balanced : CompressionLevel::None)) {
            NLM_LOG_ERROR("Failed to create checkpoint file: " + filepath);
            return false;
        }
        
        // Collect checkpoint data
        NeuronCheckpointData neuronData;
        SynapseCheckpointData synapseData;
        
        if (!neuronProvider_(neuronData)) {
            NLM_LOG_ERROR("Failed to provide neuron data for checkpoint");
            writer.abort();
            return false;
        }
        
        if (!synapseProvider_(synapseData)) {
            NLM_LOG_ERROR("Failed to provide synapse data for checkpoint");
            writer.abort();
            return false;
        }
        
        // Write checkpoint data
        if (!writer.writeNeurons(neuronData)) {
            NLM_LOG_ERROR("Failed to write neurons to checkpoint");
            writer.abort();
            return false;
        }
        
        if (!writer.writeSynapses(synapseData)) {
            NLM_LOG_ERROR("Failed to write synapses to checkpoint");
            writer.abort();
            return false;
        }
        
        if (!writer.finalize()) {
            NLM_LOG_ERROR("Failed to finalize checkpoint");
            writer.abort();
            return false;
        }
        
        // Update metadata
        lastSaveStep_ = currentStep;
        lastCheckpointPath_ = filepath;
        
        NLM_LOG_INFO("Checkpoint saved: " + filepath + " (step " + 
                    std::to_string(currentStep) + ", neurons: " +
                    std::to_string(neuronData.membranePotential.size()) + 
                    ", synapses: " + std::to_string(synapseData.weight.size()) + ")");
        
        // Prune old checkpoints
        pruneOldCheckpoints(maxCheckpoints_);
        
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Exception during checkpoint save: " + std::string(e.what()));
        return false;
    }
}

bool CheckpointManager::load(const std::string& name) {
    std::string checkpointName = name;
    if (checkpointName.empty()) {
        // Use most recent checkpoint
        auto checkpoints = listCheckpoints();
        if (checkpoints.empty()) {
            NLM_LOG_ERROR("No checkpoints available to load");
            return false;
        }
        checkpointName = checkpoints.back().name;
    }
    
    std::string filepath = checkpointDir_ + "/" + checkpointName + ".nlmcp";
    
    try {
        CheckpointReader reader;
        if (!reader.open(filepath)) {
            NLM_LOG_ERROR("Failed to open checkpoint file: " + filepath);
            return false;
        }
        
        if (!reader.validate()) {
            NLM_LOG_ERROR("Checkpoint validation failed: " + reader.getError());
            return false;
        }
        
        // For now, just log successful load
        // In a real implementation, this would restore brain state
        NLM_LOG_INFO("Checkpoint loaded: " + filepath + 
                    " (step " + std::to_string(reader.getHeader().step) + ")");
        
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Exception during checkpoint load: " + std::string(e.what()));
        return false;
    }
}

std::vector<CheckpointManager::CheckpointInfo> CheckpointManager::listCheckpoints() const {
    std::vector<CheckpointInfo> checkpoints;
    
    try {
        std::error_code ec;
        for (const auto& entry : std::filesystem::directory_iterator(checkpointDir_, ec)) {
            if (!ec && entry.is_regular_file() && 
                entry.path().extension() == ".nlmcp") {
                
                CheckpointInfo info;
                info.name = entry.path().stem().string();
                
                // Extract step from filename if possible
                try {
                    size_t underscorePos = info.name.find('_');
                    if (underscorePos != std::string::npos) {
                        info.step = std::stoull(info.name.substr(underscorePos + 1));
                    }
                } catch (...) {
                    // If we can't parse step, use 0
                    info.step = 0;
                }
                
                info.fileSize = entry.file_size();
                info.timestamp = std::chrono::system_clock::to_time_t(
                    std::chrono::file_clock::to_sys(entry.last_write_time()));
                
                checkpoints.push_back(info);
            }
        }
        
        // Sort by step number (descending - most recent first)
        std::sort(checkpoints.begin(), checkpoints.end(),
                  [](const CheckpointInfo& a, const CheckpointInfo& b) {
                      return a.step > b.step;
                  });
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Failed to list checkpoints: " + std::string(e.what()));
    }
    
    return checkpoints;
}

void CheckpointManager::pruneOldCheckpoints(size_t keepCount) {
    auto checkpoints = listCheckpoints();
    
    if (checkpoints.size() <= keepCount) {
        return;  // No pruning needed
    }
    
    // Keep the most recent N checkpoints
    size_t toDelete = checkpoints.size() - keepCount;
    
    for (size_t i = 0; i < toDelete; ++i) {
        std::string filepath = checkpointDir_ + "/" + checkpoints[i].name + ".nlmcp";
        std::error_code ec;
        std::filesystem::remove(filepath, ec);
        if (ec) {
            NLM_LOG_WARNING("Failed to remove old checkpoint: " + filepath);
        } else {
            NLM_LOG_INFO("Removed old checkpoint: " + filepath);
        }
    }
}

std::string CheckpointManager::getLastCheckpointPath() const {
    return lastCheckpointPath_;
}

bool CheckpointManager::shouldSave(uint64_t currentStep) const {
    if (currentStep <= lastSavePoint_) {
        return false;
    }
    
    // Save at specified intervals
    if ((currentStep - lastSavePoint_) >= saveIntervalSteps_) {
        return true;
    }
    
    // Also save at step 0 (initial checkpoint)
    if (currentStep == 0) {
        return true;
    }
    
    return false;
}

} // namespace nlm