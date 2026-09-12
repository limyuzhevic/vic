// Brain checkpoint implementation
#include "BrainCheckpoint.hpp"
#include "BrainCore.hpp"
#include "../core/Logger/Logger.hpp"
#include <filesystem>

namespace nlm {

bool BrainCheckpoint::save(Brain* brain, const std::string& filepath) {
    if (!brain) {
        NLM_LOG_ERROR("Brain pointer is null");
        return false;
    }
    
    NLM_LOG_INFO("Saving brain state to " + filepath);
    
    try {
        return brain->save(filepath);
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Exception saving brain: ") + e.what());
        return false;
    }
}

bool BrainCheckpoint::load(Brain* brain, const std::string& filepath) {
    if (!brain) {
        NLM_LOG_ERROR("Brain pointer is null");
        return false;
    }
    
    NLM_LOG_INFO("Loading brain state from " + filepath);
    
    try {
        return brain->load(filepath);
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Exception loading brain: ") + e.what());
        return false;
    }
}

void BrainCheckpoint::reset(Brain* brain) {
    if (brain) {
        brain->reset();
        NLM_LOG_INFO("Brain state reset complete");
    }
}

std::string BrainCheckpoint::getCheckpointDirectory(Brain* brain) {
    if (!brain || !brain->getConfig()) {
        return "./checkpoints";
    }
    
    std::string checkpointDir = brain->getConfig()->getOr<std::string>("checkpoint_dir", "./checkpoints");
    return checkpointDir;
}

bool BrainCheckpoint::createCheckpointDirectory(const std::string& directory) {
    try {
        std::filesystem::create_directories(directory);
        NLM_LOG_INFO("Created checkpoint directory: " + directory);
        return true;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Failed to create checkpoint directory: ") + e.what());
        return false;
    }
}

} // namespace nlm