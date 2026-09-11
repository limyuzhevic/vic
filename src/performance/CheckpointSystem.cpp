#include "CheckpointSystem.hpp"
#include <algorithm>
#include <filesystem>
#include <chrono>

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
    
    Impl() : saveIntervalSteps_(10000), maxCheckpoints_(10), compress_(true), lastSaveStep_(0) {}
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
    std::filesystem::create_directories(checkpointDir_);
}

void CheckpointManager::update(uint64_t currentStep, double currentTime) {
    if (shouldSave(currentStep)) {
        saveImmediately();
        lastSaveStep_ = currentStep;
    }
}

bool CheckpointManager::saveImmediately(const std::string& name) {
    std::string filepath = getLastCheckpointPath();
    
    if (filepath.empty() || name.empty()) {
        // Generate timestamp-based filename
        auto now = std::chrono::system_clock::now();
        auto timestamp = std::chrono::system_clock::to_time_t(now);
        std::stringstream ss;
        ss << checkpointDir_ << "/checkpoint_" << timestamp << ".nlm";
        filepath = ss.str();
    } else {
        filepath = checkpointDir_ + "/" + name;
    }
    
    CheckpointWriter writer;
    if (!writer.create(filepath, compress_ ? CompressionLevel::Balanced : CompressionLevel::None)) {
        return false;
    }
    
    // Set metadata
    writer.setMetadata(
        // Need to get these from providers - placeholder values
        1000, 10000, 1, 0, 0.0
    );
    
    // TODO: Implement actual data collection from providers
    // For now, write placeholder data
    
    if (!writer.finalize()) {
        writer.abort();
        return false;
    }
    
    lastCheckpointPath_ = filepath;
    lastSaveStep_ = 0; // Will be set by shouldSave
    
    return true;
}

bool CheckpointManager::load(const std::string& name) {
    std::string filepath = checkpointDir_ + "/" + name;
    
    CheckpointReader reader;
    if (!reader.open(filepath)) {
        return false;
    }
    
    if (!reader.validate()) {
        return false;
    }
    
    // TODO: Implement actual data loading from providers
    return true;
}

std::vector<CheckpointManager::CheckpointInfo> CheckpointManager::listCheckpoints() const {
    std::vector<CheckpointInfo> checkpoints;
    
    // TODO: Implement directory scanning for checkpoint files
    // For now, return empty vector
    
    return checkpoints;
}

void CheckpointManager::pruneOldCheckpoints(size_t keepCount) {
    // TODO: Implement checkpoint pruning
}

std::string CheckpointManager::getLastCheckpointPath() const {
    return checkpointDir_ + "/last_checkpoint.nlm";
}

bool CheckpointManager::shouldSave(uint64_t currentStep) const {
    return (currentStep - lastSaveStep_) >= saveIntervalSteps_ &&
           currentStep > 0;
}

} // namespace nlm