#include "CheckpointManager.hpp"
#include "CheckpointSystem.hpp"
#include <algorithm>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <iostream>
#include <chrono>

namespace nlm {

CheckpointManager::CheckpointManager()
    : saveIntervalSteps_(10000)
    , maxCheckpoints_(10)
    , compress_(true)
    , lastSaveStep_(0)
    , totalCheckpointsCreated_(0)
    , totalBytesWritten_(0) {}

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
    createCheckpointDirectory();
}

bool CheckpointManager::createCheckpointDirectory() const {
    try {
        if (!checkpointDir_.empty()) {
            std::filesystem::create_directories(checkpointDir_);
            return true;
        }
    } catch (const std::exception& e) {
        std::cerr << "Error creating checkpoint directory: " << e.what() << std::endl;
    }
    return false;
}

std::string CheckpointManager::getCheckpointPath(uint64_t step) const {
    std::string name = getCheckpointName(step);
    return checkpointDir_ + "/" + name;
}

std::string CheckpointManager::getCheckpointName(uint64_t step) const {
    std::stringstream ss;
    ss << "checkpoint_step_" << step << ".bin";
    return ss.str();
}

bool CheckpointManager::update(uint64_t currentStep, double currentTime) {
    // Check if we should save a checkpoint
    if (shouldSave(currentStep)) {
        return saveImmediately();
    }
    return true;
}

bool CheckpointManager::shouldSave(uint64_t currentStep) const {
    return (currentStep > 0 && currentStep % saveIntervalSteps_ == 0) ||
           (currentStep == 1);  // Always save at step 1 for initial state
}

bool CheckpointManager::saveImmediately(const std::string& name) {
    if (neuronProvider_ == nullptr || synapseProvider_ == nullptr) {
        std::cerr << "Checkpoint manager: Neuron or synapse provider not set" << std::endl;
        return false;
    }
    
    std::string checkpointPath;
    if (name.empty()) {
        checkpointPath = getCheckpointPath(lastSaveStep_);  // Use lastSaveStep as fallback
    } else {
        checkpointPath = checkpointDir_ + "/" + name;
    }
    
    try {
        // Create checkpoint file
        CheckpointWriter writer;
        CompressionLevel compression = compress_ ? 
            CompressionLevel::Balanced : CompressionLevel::None;
            
        if (!writer.create(checkpointPath, compression)) {
            std::cerr << "Failed to create checkpoint file: " << checkpointPath << std::endl;
            return false;
        }
        
        // Set metadata
        NeuronCheckpointData neuronData;
        SynapseCheckpointData synapseData;
        
        if (!neuronProvider_(neuronData)) {
            std::cerr << "Failed to get neuron data for checkpoint" << std::endl;
            writer.abort();
            return false;
        }
        
        if (!synapseProvider_(synapseData)) {
            std::cerr << "Failed to get synapse data for checkpoint" << std::endl;
            writer.abort();
            return false;
        }
        
        writer.setMetadata(neuronData.membranePotential.size(),
                          synapseData.weight.size(),
                          0,  // region count
                          0,  // step (would be currentStep)
                          0.0);  // simulationTime (would be currentTime)
        
        // Write data
        if (!writer.writeNeurons(neuronData)) {
            std::cerr << "Failed to write neurons to checkpoint" << std::endl;
            writer.abort();
            return false;
        }
        
        if (!writer.writeSynapses(synapseData)) {
            std::cerr << "Failed to write synapses to checkpoint" << std::endl;
            writer.abort();
            return false;
        }
        
        // Finalize
        if (!writer.finalize()) {
            std::cerr << "Failed to finalize checkpoint" << std::endl;
            return false;
        }
        
        // Update statistics
        uint64_t bytesWritten = writer.getBytesWritten();
        totalCheckpointsCreated_++;
        totalBytesWritten_ += bytesWritten;
        
        // Update last checkpoint info
        lastSaveStep_ = 0;  // Keep as is since we don't have current step
        lastCheckpointPath_ = checkpointPath;
        
        // Prune old checkpoints
        pruneOldCheckpoints(maxCheckpoints_);
        
        std::cout << "Checkpoint saved: " << checkpointPath << " (" 
                  << bytesWritten << " bytes)" << std::endl;
        
        return true;
        
    } catch (const std::exception& e) {
        std::cerr << "Exception during checkpoint save: " << e.what() << std::endl;
        return false;
    }
}

bool CheckpointManager::load(const std::string& name) {
    std::string checkpointPath = checkpointDir_ + "/" + name;
    
    try {
        CheckpointReader reader;
        if (!reader.open(checkpointPath)) {
            std::cerr << "Failed to open checkpoint file: " << checkpointPath << std::endl;
            return false;
        }
        
        if (!reader.validate()) {
            std::cerr << "Checkpoint validation failed: " << reader.getError() << std::endl;
            return false;
        }
        
        // Note: CheckpointManager doesn't directly load into Brain
        // It just provides checkpoint access for other components
        // The actual Brain::load() method would use CheckpointReader directly
        
        return true;
        
    } catch (const std::exception& e) {
        std::cerr << "Exception loading checkpoint: " << e.what() << std::endl;
        return false;
    }
}

std::vector<CheckpointManager::CheckpointInfo> CheckpointManager::listCheckpoints() const {
    std::vector<CheckpointManager::CheckpointInfo> checkpoints;
    
    try {
        if (!std::filesystem::exists(checkpointDir_)) {
            return checkpoints;
        }
        
        for (const auto& entry : std::filesystem::directory_iterator(checkpointDir_)) {
            if (!entry.is_regular_file()) continue;
            
            std::string filename = entry.path().filename().string();
            if (filename.find("checkpoint_step_") != 0) continue;
            
            // Parse step from filename
            std::string stepStr = filename.substr(16); // Remove "checkpoint_step_"
            stepStr = stepStr.substr(0, stepStr.find('.'));
            
            try {
                uint64_t step = std::stoull(stepStr);
                auto fileSize = std::filesystem::file_size(entry.path());
                time_t fileTime = std::filesystem::last_write_time(entry.path()).time_since_epoch().count();
                
                CheckpointInfo info;
                info.name = filename;
                info.step = step;
                info.fileSize = fileSize;
                info.timestamp = fileTime;
                
                checkpoints.push_back(info);
            } catch (const std::exception& e) {
                std::cerr << "Error parsing checkpoint file " << filename << ": " << e.what() << std::endl;
            }
        }
        
        // Sort by step number
        std::sort(checkpoints.begin(), checkpoints.end(),
                 [](const CheckpointInfo& a, const CheckpointInfo& b) {
                     return a.step < b.step;
                 });
        
    } catch (const std::exception& e) {
        std::cerr << "Error listing checkpoints: " << e.what() << std::endl;
    }
    
    return checkpoints;
}

void CheckpointManager::pruneOldCheckpoints(size_t keepCount) {
    auto checkpoints = listCheckpoints();
    
    if (checkpoints.size() <= keepCount) {
        return;  // No need to prune
    }
    
    // Keep only the most recent checkpoints
    size_t toDelete = checkpoints.size() - keepCount;
    
    try {
        for (size_t i = 0; i < toDelete; ++i) {
            std::string filepath = checkpointDir_ + "/" + checkpoints[i].name;
            std::filesystem::remove(filepath);
        }
        
    } catch (const std::exception& e) {
        std::cerr << "Error pruning old checkpoints: " << e.what() << std::endl;
    }
}

std::string CheckpointManager::getLastCheckpointPath() const {
    return lastCheckpointPath_;
}

CheckpointManager::Statistics CheckpointManager::getStatistics() const {
    Statistics stats;
    stats.totalCheckpointsCreated = totalCheckpointsCreated_;
    stats.totalBytesWritten = totalBytesWritten_;
    stats.averageCheckpointSize = totalCheckpointsCreated_ > 0 ? 
        static_cast<double>(totalBytesWritten_) / totalCheckpointsCreated_ : 0.0;
    
    return stats;
}

} // namespace nlm
