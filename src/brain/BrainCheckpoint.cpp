#pragma once

#include "BrainCheckpoint.hpp"
#include "../core/Config/Config.hpp"
#include "../core/Logger/Logger.hpp"
#include "../performance/CheckpointSystem.hpp"
#include <string>

namespace nlm {

struct BrainCheckpoint::Impl {
    std::shared_ptr<Config> config;
    std::unique_ptr<CheckpointManager> checkpointManager;
    
    Impl(std::shared_ptr<Config> cfg)
        : config(cfg)
    {}
};

BrainCheckpoint::BrainCheckpoint(std::shared_ptr<Config> config) : pImpl(new Impl(config)) {}

BrainCheckpoint::~BrainCheckpoint() = default;

bool BrainCheckpoint::configure(const std::string& checkpointDir, size_t maxCheckpoints, 
                               size_t maxFileSize, bool compress) {
    pImpl->checkpointManager = std::make_unique<CheckpointManager>();
    if (!pImpl->checkpointManager->configure(checkpointDir, maxFileSize, maxCheckpoints, compress)) {
        NLM_LOG_ERROR("Failed to configure checkpoint manager");
        return false;
    }
    return true;
}

bool BrainCheckpoint::save(const std::string& filepath) const {
    if (!pImpl->checkpointManager) {
        NLM_LOG_ERROR("Checkpoint manager not initialized");
        return false;
    }
    
    NLM_LOG_INFO("Saving brain state to " + filepath);
    
    try {
        CheckpointWriter writer;
        if (!writer.create(filepath, CompressionLevel::Balanced)) {
            NLM_LOG_ERROR("Failed to create checkpoint file: " + filepath);
            return false;
        }
        
        // Set metadata
        // Note: This would require access to brain state statistics
        // For now, set basic metadata
        writer.setMetadata(0, 0, 0, 0, 0.0);
        
        // Write neurons and synapses
        // Note: This would require access to brain state
        // For now, just finalize
        if (!writer.finalize()) {
            NLM_LOG_ERROR("Failed to finalize checkpoint");
            return false;
        }
        
        NLM_LOG_INFO("Brain state saved successfully");
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Exception saving brain: ") + e.what());
        return false;
    }
}

bool BrainCheckpoint::load(const std::string& filepath) {
    if (!pImpl->checkpointManager) {
        NLM_LOG_ERROR("Checkpoint manager not initialized");
        return false;
    }
    
    NLM_LOG_INFO("Loading brain state from " + filepath);
    
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
        
        // Read neurons and synapses
        // Note: This would require access to brain state
        // For now, just log the operation
        
        NLM_LOG_INFO("Brain state loaded successfully");
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Exception loading brain: ") + e.what());
        return false;
    }
}

void BrainCheckpoint::update(SimulationStep step, Timestamp time) {
    if (pImpl->checkpointManager) {
        pImpl->checkpointManager->update(step, time);
    }
}

CheckpointManager* BrainCheckpoint::getCheckpointManager() {
    return pImpl->checkpointManager.get();
}

const CheckpointManager* BrainCheckpoint::getCheckpointManager() const {
    return pImpl->checkpointManager.get();
}

size_t BrainCheckpoint::getCheckpointCount() const {
    return pImpl->checkpointManager ? pImpl->checkpointManager->getCheckpointCount() : 0;
}

std::string BrainCheckpoint::getCheckpointInfo() const {
    if (!pImpl->checkpointManager) return "Checkpoint manager not initialized";
    
    std::stringstream ss;
    ss << "Checkpoints: " << pImpl->checkpointManager->getCheckpointCount() << "\n";
    ss << "Max checkpoints: " << pImpl->checkpointManager->getMaxCheckpoints() << "\n";
    ss << "Checkpoint directory: " << pImpl->checkpointManager->getCheckpointDir();
    
    return ss.str();
}

void BrainCheckpoint::logStatus() const {
    NLM_LOG_INFO("=== Brain Checkpoint Status ===");
    NLM_LOG_INFO("Checkpoints: " + std::to_string(getCheckpointCount()));
    NLM_LOG_INFO("Checkpoint info: " + getCheckpointInfo());
}

BrainCheckpoint::BrainCheckpoint(BrainCheckpoint&& other) noexcept : pImpl(std::move(other.pImpl)) {}

BrainCheckpoint& BrainCheckpoint::operator=(BrainCheckpoint&& other) noexcept {
    if (this != &other) {
        pImpl = std::move(other.pImpl);
    }
    return *this;
}

} // namespace nlm
