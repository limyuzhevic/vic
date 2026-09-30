// Checkpoint implementation for Brain.cpp
// Contains: Save/load checkpoint functionality, persistence

#pragma once

#include "Checkpoint.hpp"
#include "Brain.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

bool CheckpointSystem::configure(const std::string& directory, size_t maxCheckpoints, 
                                size_t keepCount, bool compression) {
    directory_ = directory;
    maxCheckpoints_ = maxCheckpoints;
    keepCount_ = keepCount;
    compressionEnabled_ = compression;
    
    NLM_LOG_INFO("CheckpointSystem configured: " + directory_ + ", max: " + 
                 std::to_string(maxCheckpoints_) + ", keep: " + std::to_string(keepCount_));
    
    return true;
}

bool CheckpointSystem::save(const std::string& filepath, const Brain* brain) {
    if (!brain) return false;
    
    NLM_LOG_INFO("CheckpointSystem saving to: " + filepath);
    
    try {
        // Create checkpoint directory if needed
        std::filesystem::create_directories(std::filesystem::path(filepath).parent_path());
        
        // Save brain state using the Brain::save method
        bool result = brain->save(filepath);
        
        if (result) {
            NLM_LOG_INFO("Checkpoint saved successfully to: " + filepath);
            
            // Manage checkpoint count
            manageCheckpoints();
        }
        
        return result;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Exception during checkpoint save: " + std::string(e.what()));
        return false;
    }
}

bool CheckpointSystem::load(const std::string& filepath, Brain* brain) {
    if (!brain) return false;
    
    NLM_LOG_INFO("CheckpointSystem loading from: " + filepath);
    
    try {
        bool result = brain->load(filepath);
        
        if (result) {
            NLM_LOG_INFO("Checkpoint loaded successfully from: " + filepath);
        }
        
        return result;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Exception during checkpoint load: " + std::string(e.what()));
        return false;
    }
}

void CheckpointSystem::manageCheckpoints() {
    if (!std::filesystem::exists(directory_)) return;
    
    std::vector<std::filesystem::path> checkpointFiles;
    
    for (const auto& entry : std::filesystem::directory_iterator(directory_)) {
        if (entry.is_regular_file()) {
            checkpointFiles.push_back(entry.path());
        }
    }
    
    if (checkpointFiles.size() > maxCheckpoints_) {
        // Sort by modification time (oldest first)
        std::sort(checkpointFiles.begin(), checkpointFiles.end(),
                  [](const std::filesystem::path& a, const std::filesystem::path& b) {
                      return std::filesystem::last_write_time(a) < std::filesystem::last_write_time(b);
                  });
        
        // Remove oldest files
        size_t toRemove = checkpointFiles.size() - maxCheckpoints_;
        for (size_t i = 0; i < toRemove && i < checkpointFiles.size(); ++i) {
            try {
                std::filesystem::remove(checkpointFiles[i]);
                NLM_LOG_INFO("Removed old checkpoint: " + checkpointFiles[i].string());
            } catch (const std::exception& e) {
                NLM_LOG_ERROR("Failed to remove checkpoint " + checkpointFiles[i].string() + ": " + std::string(e.what()));
            }
        }
    }
}

void CheckpointSystem::update(SimulationStep currentStep, Timestamp currentTime) {
    if (!brain_) return;
    
    // Save checkpoints periodically based on configuration
    if (checkpointInterval_ > 0 && currentStep % checkpointInterval_ == 0) {
        std::string filepath = directory_ + "/checkpoint_" + std::to_string(currentStep) + ".nlm";
        save(filepath, brain_.get());
    }
}

bool CheckpointSystem::setBrain(std::shared_ptr<Brain> brain) {
    if (!brain) return false;
    
    brain_ = brain;
    return true;
}

std::shared_ptr<Brain> CheckpointSystem::getBrain() const {
    return brain_;
}

} // namespace nlm
