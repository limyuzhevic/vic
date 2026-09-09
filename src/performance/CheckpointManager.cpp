#include "CheckpointSystem.hpp"
#include "../core/Logger/Logger.hpp"
#include <fstream>
#include <filesystem>
#include <chrono>
#include <zlib.h>
#include <algorithm>
#include <numeric>
#include <cstring>

namespace nlm {

struct CheckpointManager::Impl {
    std::string checkpointDir_;
    uint64_t saveIntervalSteps_;
    size_t maxCheckpoints_;
    bool compress_;
    uint64_t lastSaveStep_;
    std::string lastCheckpointPath_;
    
    std::function<bool(NeuronCheckpointData&)> neuronProvider_;
    std::function<bool(SynapseCheckpointData&)> synapseProvider_;
    
    Impl() : saveIntervalSteps_(10000), maxCheckpoints_(10), compress_(true), 
             lastSaveStep_(0) {}
};

CheckpointManager::CheckpointManager() : pImpl(new Impl) {}

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
    std::error_code ec;
    std::filesystem::create_directories(checkpointDir_, ec);
    
    NLM_LOG_INFO("Checkpoint manager configured with dir: " + checkpointDir_ +
                 ", save interval: " + std::to_string(saveIntervalSteps_) +
                 ", max checkpoints: " + std::to_string(maxCheckpoints_));
}

bool CheckpointManager::update(uint64_t currentStep, double currentTime) {
    if (saveIntervalSteps_ == 0) return false;
    
    // Check if it's time to save
    if (currentStep % saveIntervalSteps_ == 0) {
        return saveImmediately();
    }
    
    return false;
}

bool CheckpointManager::saveImmediately(const std::string& name) {
    std::string filename = name.empty() ? ("checkpoint_" + std::to_string(lastSaveStep_) + ".nlm") : name;
    std::string fullPath = checkpointDir_ + "/" + filename;
    
    // Create checkpoint header
    CheckpointHeader header;
    header.timestamp = std::chrono::duration_cast<std::chrono::seconds>(
        std::chrono::system_clock::now().time_since_epoch()).count();
    
    // Gather checkpoint data
    NeuronCheckpointData neuronData;
    SynapseCheckpointData synapseData;
    
    // Use providers to get data
    bool neuronsOk = neuronProvider_ ? neuronProvider_(neuronData) : false;
    bool synapsesOk = synapseProvider_ ? synapseProvider_(synapseData) : false;
    
    if (!neuronsOk || !synapsesOk) {
        NLM_LOG_ERROR("Failed to gather checkpoint data");
        return false;
    }
    
    // Write checkpoint file
    CheckpointWriter writer;
    if (!writer.create(fullPath, compress_ ? CompressionLevel::Balanced : CompressionLevel::None)) {
        NLM_LOG_ERROR("Failed to create checkpoint file: " + fullPath);
        return false;
    }
    
    // Write header
    header.neuronCount = neuronData.membranePotential.size();
    header.synapseCount = synapseData.sourceNeuron.size();
    header.regionCount = 0; // TODO: Get from brain
    header.step = lastSaveStep_;
    header.simulationTime = 0.0; // TODO: Get from simulation clock
    
    writer.setMetadata(header.neuronCount, header.synapseCount, 
                       header.regionCount, header.step, header.simulationTime);
    
    // Write neurons and synapses
    bool success = writer.writeNeurons(neuronData) && writer.writeSynapses(synapseData);
    
    if (success) {
        success = writer.finalize();
        if (success) {
            lastSaveStep_ = header.step;
            lastCheckpointPath_ = fullPath;
            NLM_LOG_INFO("Checkpoint saved successfully: " + fullPath);
            
            // Clean up old checkpoints
            pruneOldCheckpoints(maxCheckpoints_);
            return true;
        }
    }
    
    writer.abort();
    return false;
}

bool CheckpointManager::load(const std::string& name) {
    std::string filename = name.empty() ? "latest" : name;
    std::string fullPath = checkpointDir_ + "/" + filename;
    
    CheckpointReader reader;
    if (!reader.open(fullPath)) {
        NLM_LOG_ERROR("Failed to open checkpoint file: " + fullPath);
        return false;
    }
    
    if (!reader.validate()) {
        NLM_LOG_ERROR("Checkpoint validation failed: " + reader.getError());
        return false;
    }
    
    const CheckpointHeader& header = reader.getHeader();
    
    // Read checkpoint data
    NeuronCheckpointData neuronData;
    SynapseCheckpointData synapseData;
    
    bool neuronsOk = reader.readNeurons(neuronData);
    bool synapsesOk = reader.readSynapses(synapseData);
    
    if (!neuronsOk || !synapsesOk) {
        NLM_LOG_ERROR("Failed to read checkpoint data");
        return false;
    }
    
    NLM_LOG_INFO("Checkpoint loaded successfully: " + fullPath + 
                 ", neurons: " + std::to_string(header.neuronCount) +
                 ", synapses: " + std::to_string(header.synapseCount));
    
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
            
            // Parse filename for step number
            size_t pos = info.name.find_last_of('_');
            if (pos != std::string::npos) {
                std::string stepStr = info.name.substr(pos + 1);
                stepStr = stepStr.substr(0, stepStr.find('.'));
                try {
                    info.step = std::stoull(stepStr);
                } catch (...) {
                    info.step = 0;
                }
            }
            
            info.fileSize = entry.file_size();
            info.timestamp = std::chrono::duration_cast<std::chrono::seconds>(
                std::chrono::system_clock::now().time_since_epoch()).count();
            
            checkpoints.push_back(info);
        }
    }
    
    // Sort by step number (newest first)
    std::sort(checkpoints.begin(), checkpoints.end(),
              [](const CheckpointInfo& a, const CheckpointInfo& b) {
                  return a.step > b.step;
              });
    
    return checkpoints;
}

void CheckpointManager::pruneOldCheckpoints(size_t keepCount) {
    std::vector<CheckpointInfo> checkpoints = listCheckpoints();
    
    if (checkpoints.size() <= keepCount) {
        return;
    }
    
    // Remove oldest checkpoints
    for (size_t i = keepCount; i < checkpoints.size(); ++i) {
        std::string fullPath = checkpointDir_ + "/" + checkpoints[i].name;
        std::error_code ec;
        std::filesystem::remove(fullPath, ec);
        if (ec) {
            NLM_LOG_WARNING("Failed to remove old checkpoint: " + fullPath);
        }
    }
    
    NLM_LOG_INFO("Pruned old checkpoints, keeping " + std::to_string(keepCount) + 
                 " most recent");
}

std::string CheckpointManager::getLastCheckpointPath() const {
    auto checkpoints = listCheckpoints();
    if (checkpoints.empty()) {
        return "";
    }
    
    return checkpointDir_ + "/" + checkpoints[0].name;
}

bool CheckpointManager::shouldSave(uint64_t currentStep) const {
    if (saveIntervalSteps_ == 0) return false;
    return currentStep % saveIntervalSteps_ == 0;
}

} // namespace nlm