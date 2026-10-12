#include "Brain.hpp"
#include "../core/Logger/Logger.hpp"
#include "../performance/CheckpointSystem.hpp"
#include <fstream>
#include <sstream>
#include <filesystem>

namespace nlm {

// Brain checkpoint I/O methods

bool Brain::save(const std::string& filepath) const {
    // Save brain state to file (checkpointing)
    try {
        // Create checkpoint directory if it doesn't exist
        std::filesystem::path filepath_path(filepath);
        std::filesystem::path dir = filepath_path.parent_path();
        if (!dir.empty() && !std::filesystem::exists(dir)) {
            std::filesystem::create_directories(dir);
        }
        
        // Use checkpoint manager to save state
        if (pImpl->checkpointManager) {
            return pImpl->checkpointManager->save(filepath);
        }
        
        NLM_LOG_ERROR("Checkpoint manager not available for saving");
        return false;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Exception while saving brain state: " + std::string(e.what()));
        return false;
    }
}

bool Brain::load(const std::string& filepath) {
    // Load brain state from file
    try {
        if (!std::filesystem::exists(filepath)) {
            NLM_LOG_ERROR("Checkpoint file does not exist: " + filepath);
            return false;
        }
        
        // Use checkpoint manager to load state
        if (pImpl->checkpointManager) {
            bool result = pImpl->checkpointManager->load(filepath);
            if (result) {
                NLM_LOG_INFO("Successfully loaded brain state from: " + filepath);
            } else {
                NLM_LOG_ERROR("Failed to load brain state from: " + filepath);
            }
            return result;
        }
        
        NLM_LOG_ERROR("Checkpoint manager not available for loading");
        return false;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Exception while loading brain state: " + std::string(e.what()));
        return false;
    }
}

} // namespace nlm