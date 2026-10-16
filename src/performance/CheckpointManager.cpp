// CheckpointManager implementation

#include "performance/CheckpointSystem.hpp"
#include <algorithm>
#include <filesystem>
#include <iostream>
#include <numeric>
#include <sstream>

namespace nlm {

CheckpointManager::CheckpointManager() 
    : checkpointDir_()
    , saveIntervalSteps_(10000)
    , maxCheckpoints_(10)
    , compress_(true)
    , lastSaveStep_(0)
    , lastCheckpointPath_()
    , neuronProvider_()
    , synapseProvider_() {
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
    if (shouldSave(currentStep)) {
        return saveImmediately();
    }
    return true;
}

bool CheckpointManager::saveImmediately(const std::string& name) {
    if (!neuronProvider_ || !synapseProvider_) {
        return false;
    }
    
    try {
        // Generate checkpoint name
        std::string checkpointName = name;
        if (checkpointName.empty()) {
            checkpointName = "brain_checkpoint_" + std::to_string(lastSaveStep_);
        }
        
        std::string filepath = checkpointDir_ + "/" + checkpointName + ".chk";
        
        // Create checkpoint file
        CheckpointWriter writer;
        if (!writer.create(filepath, CompressionLevel::Balanced)) {
            return false;
        }
        
        // Set metadata
        writer.setMetadata(
            getHeaderInfo().neuronCount,
            getHeaderInfo().synapseCount,
            getHeaderInfo().regionCount,
            getHeaderInfo().step,
            getHeaderInfo().simulationTime
        );
        
        // Get neuron data from provider
        NeuronCheckpointData neuronData;
        if (!neuronProvider_(neuronData)) {
            return false;
        }
        
        // Write neurons
        if (!writer.writeNeurons(neuronData)) {
            return false;
        }
        
        // Get synapse data from provider
        SynapseCheckpointData synapseData;
        if (!synapseProvider_(synapseData)) {
            return false;
        }
        
        // Write synapses
        if (!writer.writeSynapses(synapseData)) {
            return false;
        }
        
        // Finalize
        if (!writer.finalize()) {
            return false;
        }
        
        // Update last checkpoint info
        lastCheckpointPath_ = filepath;
        lastSaveStep_ = lastSaveStep_ + saveIntervalSteps_;
        
        // Prune old checkpoints
        pruneOldCheckpoints(maxCheckpoints_);
        
        return true;
    } catch (const std::exception& e) {
        // Log error
        return false;
    }
}

bool CheckpointManager::load(const std::string& name) {
    try {
        std::string filepath = checkpointDir_ + "/" + name + ".chk";
        
        // Open checkpoint file
        CheckpointReader reader;
        if (!reader.open(filepath)) {
            return false;
        }
        
        // Validate checkpoint
        if (!reader.validate()) {
            return false;
        }
        
        // Read neuron data
        NeuronCheckpointData neuronData;
        if (!reader.readNeurons(neuronData)) {
            return false;
        }
        
        // Read synapse data
        SynapseCheckpointData synapseData;
        if (!reader.readSynapses(synapseData)) {
            return false;
        }
        
        // In a real implementation, we would use the data to restore the brain state
        // For now, just acknowledge that we read the data
        
        lastCheckpointPath_ = filepath;
        
        return true;
    } catch (const std::exception& e) {
        // Log error
        return false;
    }
}

std::vector<CheckpointManager::CheckpointInfo> CheckpointManager::listCheckpoints() const {
    std::vector<CheckpointInfo> result;
    
    if (!std::filesystem::exists(checkpointDir_)) {
        return result;
    }
    
    for (const auto& entry : std::filesystem::directory_iterator(checkpointDir_)) {
        if (entry.is_regular_file() && entry.path().extension() == ".chk") {
            CheckpointInfo info;
            info.name = entry.path().stem().string();
            info.fileSize = entry.file_size();
            
            // Try to read header (simplified)
            std::ifstream stream(entry.path(), std::ios::binary);
            if (stream.is_open()) {
                CheckpointHeader header;
                stream.read(reinterpret_cast<char*>(&header), sizeof(CheckpointHeader));
                if (stream.good()) {
                    info.step = header.step;
                    info.simulationTime = header.simulationTime;
                    info.neuronCount = header.neuronCount;
                    info.synapseCount = header.synapseCount;
                }
                stream.close();
            }
            
            info.timestamp = std::time(nullptr);
            
            result.push_back(info);
        }
    }
    
    // Sort by timestamp (newest first)
    std::sort(result.begin(), result.end(), 
              [](const CheckpointInfo& a, const CheckpointInfo& b) {
                  return a.timestamp > b.timestamp;
              });
    
    return result;
}

void CheckpointManager::pruneOldCheckpoints(size_t keepCount) {
    auto checkpoints = listCheckpoints();
    
    if (checkpoints.size() <= keepCount) {
        return;
    }
    
    // Remove oldest checkpoints
    size_t removeCount = checkpoints.size() - keepCount;
    for (size_t i = checkpoints.size() - 1; i >= removeCount && i < checkpoints.size(); --i) {
        std::string filepath = checkpointDir_ + "/" + checkpoints[i].name + ".chk";
        std::filesystem::remove(filepath);
    }
}

std::string CheckpointManager::getLastCheckpointPath() const {
    return lastCheckpointPath_;
}

bool CheckpointManager::shouldSave(uint64_t currentStep) const {
    return currentStep >= lastSaveStep_ && (currentStep - lastSaveStep_) >= saveIntervalSteps_;
}

CheckpointManager::CheckpointHeaderInfo CheckpointManager::getHeaderInfo() const {
    CheckpointHeaderInfo info;
    info.neuronCount = 0;
    info.synapseCount = 0;
    info.regionCount = 0;
    info.step = 0;
    info.simulationTime = 0.0;
    
    // If providers are set, try to get info from them
    if (neuronProvider_) {
        NeuronCheckpointData data;
        if (neuronProvider_(data)) {
            info.neuronCount = data.membranePotential.size();
        }
    }
    
    if (synapseProvider_) {
        SynapseCheckpointData data;
        if (synapseProvider_(data)) {
            info.synapseCount = data.weight.size();
        }
    }
    
    return info;
}

} // namespace nlm
