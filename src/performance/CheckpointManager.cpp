#include "CheckpointSystem.hpp"
#include <filesystem>
#include <ctime>
#include <algorithm>
#include <cmath>
#include <fstream>
#include <sstream>
#include <iomanip>

namespace nlm {

// CheckpointManager implementation
CheckpointManager::CheckpointManager() : saveIntervalSteps_(0), maxCheckpoints_(0), 
    compress_(false), lastSaveStep_(0), bytesWritten_(0) {}

CheckpointManager::~CheckpointManager() {
    if (isOpen()) {
        close();
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
    std::filesystem::create_directories(checkpointDir_);
}

bool CheckpointManager::update(uint64_t currentStep, double currentTime) {
    if (!neuronProvider_ || !synapseProvider_) {
        return false;
    }
    
    if (shouldSave(currentStep)) {
        std::string filename = "checkpoint_" + std::to_string(currentStep) + ".bin";
        std::string filepath = checkpointDir_ + "/" + filename;
        
        // Create temporary writer for this checkpoint
        CheckpointWriter writer;
        if (!writer.create(filepath, compress_ ? CompressionLevel::Balanced : CompressionLevel::None)) {
            return false;
        }
        
        // Set metadata
        writer.setMetadata(0, 0, 0, currentStep, currentTime);
        
        // Write neurons using provider
        NeuronCheckpointData neuronData;
        if (!neuronProvider_(neuronData)) {
            writer.abort();
            return false;
        }
        if (!writer.writeNeurons(neuronData)) {
            writer.abort();
            return false;
        }
        
        // Write synapses using provider
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
        
        // Update last save step and filename
        lastSaveStep_ = currentStep;
        lastCheckpointPath_ = filepath;
        bytesWritten_ = writer.getBytesWritten();
        
        return true;
    }
    
    return true;
}

bool CheckpointManager::saveImmediately(const std::string& name) {
    if (!neuronProvider_ || !synapseProvider_) {
        return false;
    }
    
    std::string filename;
    if (!name.empty()) {
        filename = name + ".bin";
    } else {
        filename = "checkpoint_" + std::to_string(lastSaveStep_ + 1) + ".bin";
    }
    
    std::string filepath = checkpointDir_ + "/" + filename;
    
    // Create checkpoint file
    CheckpointWriter writer;
    if (!writer.create(filepath, compress_ ? CompressionLevel::Balanced : CompressionLevel::None)) {
        return false;
    }
    
    // Get current simulation state
    uint64_t currentStep = 0;  // This should be obtained from Brain
    double currentTime = 0.0;   // This should be obtained from Brain
    
    // Set metadata
    writer.setMetadata(0, 0, 0, currentStep, currentTime);
    
    // Write neurons using provider
    NeuronCheckpointData neuronData;
    if (!neuronProvider_(neuronData)) {
        writer.abort();
        return false;
    }
    if (!writer.writeNeurons(neuronData)) {
        writer.abort();
        return false;
    }
    
    // Write synapses using provider
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
    
    // Update last save step and filename
    lastSaveStep_ = currentStep;
    lastCheckpointPath_ = filepath;
    bytesWritten_ = writer.getBytesWritten();
    
    return true;
}

bool CheckpointManager::load(const std::string& name) {
    std::string filepath;
    if (name.empty()) {
        filepath = lastCheckpointPath_;
        if (filepath.empty()) {
            return false;
        }
    } else {
        filepath = checkpointDir_ + "/" + name;
    }
    
    CheckpointReader reader;
    if (!reader.open(filepath)) {
        return false;
    }
    
    if (!reader.validate()) {
        reader.close();
        return false;
    }
    
    // Read neurons
    NeuronCheckpointData neuronData;
    if (!reader.readNeurons(neuronData)) {
        reader.close();
        return false;
    }
    
    // Read synapses
    SynapseCheckpointData synapseData;
    if (!reader.readSynapses(synapseData)) {
        reader.close();
        return false;
    }
    
    reader.close();
    return true;
}

std::vector<CheckpointManager::CheckpointInfo> CheckpointManager::listCheckpoints() const {
    std::vector<CheckpointInfo> checkpoints;
    
    if (!std::filesystem::exists(checkpointDir_)) {
        return checkpoints;
    }
    
    for (const auto& entry : std::filesystem::directory_iterator(checkpointDir_)) {
        if (entry.is_regular_file() && entry.path().extension() == ".bin") {
            std::string filename = entry.path().filename().string();
            
            // Extract step number from filename
            size_t pos = filename.find_last_of('_');
            uint64_t step = 0;
            if (pos != std::string::npos && filename.substr(pos + 1, 4) == "step") {
                try {
                    step = std::stoull(filename.substr(pos + 5, 10));
                } catch (...) {
                    step = 0;
                }
            }
            
            CheckpointInfo info;
            info.name = filename;
            info.step = step;
            info.fileSize = entry.file_size();
            info.timestamp = std::time(nullptr);
            
            checkpoints.push_back(info);
        }
    }
    
    // Sort by step number
    std::sort(checkpoints.begin(), checkpoints.end(), 
              [](const CheckpointInfo& a, const CheckpointInfo& b) {
                  return a.step < b.step;
              });
    
    // Prune old checkpoints if needed
    if (checkpoints.size() > maxCheckpoints_) {
        size_t toRemove = checkpoints.size() - maxCheckpoints_;
        checkpoints.erase(checkpoints.begin(), checkpoints.begin() + toRemove);
    }
    
    return checkpoints;
}

void CheckpointManager::pruneOldCheckpoints(size_t keepCount) {
    std::vector<CheckpointInfo> checkpoints = listCheckpoints();
    
    if (checkpoints.size() <= keepCount) {
        return;
    }
    
    size_t toRemove = checkpoints.size() - keepCount;
    
    for (size_t i = 0; i < toRemove; ++i) {
        std::string filepath = checkpointDir_ + "/" + checkpoints[i].name;
        std::filesystem::remove(filepath);
    }
}

bool CheckpointManager::shouldSave(uint64_t currentStep) const {
    if (saveIntervalSteps_ == 0) return false;
    return (currentStep - lastSaveStep_) >= saveIntervalSteps_;
}

std::string CheckpointManager::getLastCheckpointPath() const {
    return lastCheckpointPath_;
}

} // namespace nlm
