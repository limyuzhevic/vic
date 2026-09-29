#include "CheckpointManager.hpp"
#include "../core/Config/Config.hpp"
#include "../core/Logger/Logger.hpp"
#include <filesystem>
#include <fstream>
#include <algorithm>
#include <ctime>

namespace nlm {

bool CheckpointManager::saveCheckpoint(std::shared_ptr<Brain> brain, const std::string& filepath) {
    if (!brain) {
        NLM_LOG_ERROR("Cannot save checkpoint: invalid brain");
        return false;
    }
    
    try {
        NLM_LOG_INFO("Saving checkpoint to " + filepath);
        
        // Ensure directory exists
        std::filesystem::path path(filepath);
        std::filesystem::create_directories(path.parent_path());
        
        // Save brain state
        if (!brain->save(filepath)) {
            NLM_LOG_ERROR("Failed to save brain state");
            return false;
        }
        
        // Validate that checkpoint was saved
        if (!validateCheckpoint(filepath)) {
            NLM_LOG_ERROR("Checkpoint validation failed after save");
            return false;
        }
        
        // Create backup
        backupCheckpoint(filepath);
        
        size_t fileSize = getCheckpointFileSize(filepath);
        NLM_LOG_INFO("Checkpoint saved successfully (size: " + std::to_string(fileSize) + " bytes)");
        
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Failed to save checkpoint: " + std::string(e.what()));
        return false;
    }
}

bool CheckpointManager::loadCheckpoint(std::shared_ptr<Brain> brain, const std::string& filepath) {
    if (!brain) {
        NLM_LOG_ERROR("Cannot load checkpoint: invalid brain");
        return false;
    }
    
    try {
        NLM_LOG_INFO("Loading checkpoint from " + filepath);
        
        // Validate checkpoint file
        if (!validateCheckpoint(filepath)) {
            NLM_LOG_ERROR("Checkpoint validation failed before load");
            return false;
        }
        
        // Load brain state
        if (!brain->load(filepath)) {
            NLM_LOG_ERROR("Failed to load brain state");
            return false;
        }
        
        NLM_LOG_INFO("Checkpoint loaded successfully");
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Failed to load checkpoint: " + std::string(e.what()));
        return false;
    }
}

bool CheckpointManager::validateCheckpoint(const std::string& filepath) const {
    try {
        std::filesystem::path path(filepath);
        
        // Check if file exists
        if (!std::filesystem::exists(path)) {
            NLM_LOG_ERROR("Checkpoint file does not exist: " + filepath);
            return false;
        }
        
        // Check if file is readable
        if (!std::filesystem::is_regular_file(path) || !std::filesystem::exists(path)) {
            NLM_LOG_ERROR("Checkpoint file is not readable: " + filepath);
            return false;
        }
        
        // Check file size
        size_t fileSize = getCheckpointFileSize(filepath);
        if (fileSize == 0) {
            NLM_LOG_ERROR("Checkpoint file is empty: " + filepath);
            return false;
        }
        
        // TODO: Could add more validation by attempting to read file metadata
        // or check file integrity if Brain::load provides such functionality
        
        NLM_LOG_INFO("Checkpoint validation passed: " + filepath);
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Checkpoint validation failed: " + std::string(e.what()));
        return false;
    }
}

size_t CheckpointManager::getCheckpointFileSize(const std::string& filepath) const {
    try {
        std::filesystem::path path(filepath);
        if (std::filesystem::exists(path)) {
            return std::filesystem::file_size(path);
        }
    } catch (const std::exception&) {}
    return 0;
}

bool CheckpointManager::backupCheckpoint(const std::string& filepath) {
    try {
        std::filesystem::path originalPath(filepath);
        std::filesystem::path backupPath = originalPath;
        backupPath += ".backup";
        
        if (std::filesystem::exists(originalPath)) {
            std::filesystem::copy_file(originalPath, backupPath, 
                                      std::filesystem::copy_options::overwrite_existing);
            NLM_LOG_INFO("Checkpoint backup created: " + backupPath.string());
        }
        
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_WARNING("Failed to create checkpoint backup: " + std::string(e.what()));
        return false; // Backup failure is not critical
    }
}

bool CheckpointManager::cleanupOldCheckpoints(const std::string& pattern, int maxFiles) {
    try {
        std::filesystem::path dirPattern(pattern);
        std::filesystem::path dir = dirPattern.parent_path();
        std::string filenamePattern = dirPattern.filename().string();
        
        if (!std::filesystem::exists(dir)) {
            return true; // Nothing to clean up
        }
        
        std::vector<std::filesystem::path> matchingFiles;
        
        for (const auto& entry : std::filesystem::directory_iterator(dir)) {
            if (entry.is_regular_file() && 
                entry.path().filename().string().find(filenamePattern) != std::string::npos) {
                matchingFiles.push_back(entry.path());
            }
        }
        
        if (matchingFiles.size() <= static_cast<size_t>(maxFiles)) {
            return true; // No cleanup needed
        }
        
        // Sort by modification time (oldest first)
        std::sort(matchingFiles.begin(), matchingFiles.end(),
                 [](const std::filesystem::path& a, const std::filesystem::path& b) {
                     return std::filesystem::last_write_time(a) < std::filesystem::last_write_time(b);
                 });
        
        // Remove oldest files
        size_t filesToRemove = matchingFiles.size() - static_cast<size_t>(maxFiles);
        for (size_t i = 0; i < filesToRemove; ++i) {
            try {
                std::filesystem::remove(matchingFiles[i]);
                NLM_LOG_INFO("Cleaned up old checkpoint: " + matchingFiles[i].string());
            } catch (const std::exception& e) {
                NLM_LOG_WARNING("Failed to remove old checkpoint " + matchingFiles[i].string() + ": " + std::string(e.what()));
            }
        }
        
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Checkpoint cleanup failed: " + std::string(e.what()));
        return false;
    }
}

} // namespace nlm
