#include "CheckpointManager.hpp"
#include "../core/Config/Config.hpp"
#include "../core/Logger/Logger.hpp"
#include "../performance/CheckpointSystem.hpp"
#include <algorithm>
#include <filesystem>
#include <fstream>

namespace nlm {

struct CheckpointManager::Impl {
    std::shared_ptr<Config> config;
    Brain* brain = nullptr;
    
    // Checkpoint system
    std::unique_ptr<CheckpointSystem> checkpointSystem;
    
    // Checkpoint management
    std::string checkpointDir;
    size_t maxCheckpoints;
    size_t maxCheckpointSize;
    bool enableCompression;
    
    // Checkpoint statistics
    size_t checkpointCount;
    uint64_t totalBytesSaved;
    
    Impl(std::shared_ptr<Config> cfg) : config(cfg), checkpointCount(0), totalBytesSaved(0) {
        maxCheckpoints = 100;
        maxCheckpointSize = 10000000;  // 10MB
        enableCompression = true;
    }
};

CheckpointManager::CheckpointManager(std::shared_ptr<Config> config) : pImpl(std::make_unique<Impl>(config)), brain_(nullptr) {
}

CheckpointManager::~CheckpointManager() = default;

CheckpointManager::CheckpointManager(CheckpointManager&& other) noexcept : pImpl(std::move(other.pImpl)), brain_(other.brain_) {
    other.brain_ = nullptr;
}

CheckpointManager& CheckpointManager::operator=(CheckpointManager&& other) noexcept {
    if (this != &other) {
        pImpl = std::move(other.pImpl);
        brain_ = other.brain_;
        other.brain_ = nullptr;
    }
    return *this;
}

bool CheckpointManager::configure(const std::string& checkpointDir, size_t maxCheckpoints,
                                 size_t maxCheckpointSize, bool enableCompression) {
    pImpl->checkpointDir = checkpointDir;
    pImpl->maxCheckpoints = maxCheckpoints;
    pImpl->maxCheckpointSize = maxCheckpointSize;
    pImpl->enableCompression = enableCompression;
    
    // Create checkpoint directory if it doesn't exist
    std::error_code ec;
    std::filesystem::create_directories(checkpointDir, ec);
    if (ec) {
        NLM_LOG_ERROR("CheckpointManager: Failed to create checkpoint directory: " + checkpointDir);
        return false;
    }
    
    // Initialize checkpoint system
    pImpl->checkpointSystem = std::make_unique<CheckpointSystem>();
    if (!pImpl->checkpointSystem->configure(checkpointDir, maxCheckpoints, maxCheckpointSize, enableCompression)) {
        NLM_LOG_ERROR("CheckpointManager: Failed to configure checkpoint system");
        return false;
    }
    
    NLM_LOG_INFO("CheckpointManager configured successfully in directory: " + checkpointDir);
    return true;
}

void CheckpointManager::update(SimulationStep step, Timestamp time) {
    if (pImpl->checkpointSystem) {
        pImpl->checkpointSystem->update(step, time);
    }
}

bool CheckpointManager::save(const std::string& filepath) const {
    if (!brain_ || !pImpl->checkpointSystem) {
        NLM_LOG_ERROR("CheckpointManager: Cannot save - brain or checkpoint system not initialized");
        return false;
    }
    
    NLM_LOG_INFO("CheckpointManager: Saving brain state to " + filepath);
    
    try {
        // Use brain to save its state
        return brain_->save(filepath);
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("CheckpointManager: Exception saving checkpoint: ") + e.what());
        return false;
    }
}

bool CheckpointManager::load(const std::string& filepath) {
    if (!brain_ || !pImpl->checkpointSystem) {
        NLM_LOG_ERROR("CheckpointManager: Cannot load - brain or checkpoint system not initialized");
        return false;
    }
    
    NLM_LOG_INFO("CheckpointManager: Loading brain state from " + filepath);
    
    try {
        // Use brain to load its state
        return brain_->load(filepath);
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("CheckpointManager: Exception loading checkpoint: ") + e.what());
        return false;
    }
}

bool CheckpointManager::createCheckpoint(const std::string& name) {
    if (!pImpl->checkpointSystem) {
        return false;
    }
    
    std::string filepath = pImpl->checkpointDir + "/checkpoint_" + name + ".ncl";
    bool success = save(filepath);
    
    if (success) {
        pImpl->checkpointCount++;
        // Get file size for statistics
        std::ifstream file(filepath, std::ios::binary | std::ios::ate);
        if (file.good()) {
            pImpl->totalBytesSaved += file.tellg();
        }
    }
    
    return success;
}

bool CheckpointManager::restoreCheckpoint(const std::string& name) {
    std::string filepath = pImpl->checkpointDir + "/checkpoint_" + name + ".ncl";
    return load(filepath);
}

size_t CheckpointManager::getCheckpointCount() const {
    return pImpl->checkpointCount;
}

size_t CheckpointManager::getMaxCheckpointCount() const {
    return pImpl->maxCheckpoints;
}

uint64_t CheckpointManager::getTotalBytesSaved() const {
    return pImpl->totalBytesSaved;
}

void CheckpointManager::cleanupOldCheckpoints() {
    // Implementation would delete old checkpoints exceeding maxCheckpoints
    NLM_LOG_INFO("CheckpointManager: Cleanup not implemented yet");
}

void CheckpointManager::reset() {
    NLM_LOG_INFO("CheckpointManager reset");
    pImpl->checkpointCount = 0;
    pImpl->totalBytesSaved = 0;
    if (pImpl->checkpointSystem) {
        pImpl->checkpointSystem->reset();
    }
}

} // namespace nlm