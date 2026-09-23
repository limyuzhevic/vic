#include "BrainPersistence.hpp"
#include "../core/Logger/Logger.hpp"
#include <fstream>

namespace nlm {

// ===========================================
// CHECKPOINT PERSISTENCE SYSTEM
// ===========================================

bool BrainPersistence::saveBrainState(Brain* brain, const std::string& filepath) {
    if (!brain) {
        NLM_LOG_ERROR("Cannot save brain state: brain is null");
        return false;
    }

    NLM_LOG_INFO("Saving brain state to " + filepath);

    try {
        // Use checkpoint system for persistence
        if (brain->save(filepath)) {
            NLM_LOG_INFO("Brain state saved successfully");
            return true;
        } else {
            NLM_LOG_ERROR("Failed to save brain state");
            return false;
        }
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Exception saving brain: ") + e.what());
        return false;
    }
}

bool BrainPersistence::loadBrainState(Brain* brain, const std::string& filepath) {
    if (!brain) {
        NLM_LOG_ERROR("Cannot load brain state: brain is null");
        return false;
    }

    NLM_LOG_INFO("Loading brain state from " + filepath);

    try {
        if (brain->load(filepath)) {
            NLM_LOG_INFO("Brain state loaded successfully");
            return true;
        } else {
            NLM_LOG_ERROR("Failed to load brain state");
            return false;
        }
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Exception loading brain: ") + e.what());
        return false;
    }
}

void BrainPersistence::resetBrainState(Brain* brain) {
    if (!brain) {
        NLM_LOG_ERROR("Cannot reset brain state: brain is null");
        return;
    }

    NLM_LOG_INFO("Resetting brain state...");
    brain->reset();
    NLM_LOG_INFO("Brain state reset complete");
}

// ===========================================
// CHECKPOINT MANAGEMENT
// ===========================================

void BrainPersistence::configureCheckpointManager(Brain* brain,
                                                 const std::string& checkpointDir,
                                                 size_t maxCheckpoints,
                                                 size_t maxCheckpointAge,
                                                 bool compressionEnabled) {
    if (!brain) return;

    auto* checkpointManager = brain->getCheckpointManager();
    if (checkpointManager) {
        checkpointManager->configure(checkpointDir, maxCheckpoints, maxCheckpointAge, compressionEnabled);
    }
}

void BrainPersistence::updateCheckpointManager(Brain* brain, SimulationStep step,
                                               Timestamp time) {
    if (!brain) return;

    auto* checkpointManager = brain->getCheckpointManager();
    if (checkpointManager) {
        checkpointManager->update(step, time);
    }
}

// ===========================================
// PERSISTENCE STATISTICS AND ACCESSORS
// ===========================================

void BrainPersistence::logPersistenceStatus(Brain* brain) {
    if (!brain) return;

    auto* checkpointManager = brain->getCheckpointManager();
    if (checkpointManager) {
        NLM_LOG_INFO("Checkpoint manager configured: " + std::to_string(checkpointManager->isConfigured()));
        // Additional checkpoint statistics can be logged here
    }
}

} // namespace nlm
