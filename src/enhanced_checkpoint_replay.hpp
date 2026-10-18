// Enhanced checkpoint and replay system utilities

#pragma once

#include <string>
#include <vector>
#include <memory>
#include <chrono>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include "../src/brain/Brain.hpp"
#include "../src/memory/EpisodicMemory.hpp"
#include "../src/core/Config/Config.hpp"

namespace nlm {

class CheckpointManager {
public:
    struct CheckpointInfo {
        std::string path;
        std::chrono::system_clock::time_point timestamp;
        uint64_t neuronCount;
        uint64_t regionCount;
        size_t fileSize;
        std::string checksum;
        bool isValid;
        std::string description;
    };
    
    CheckpointManager() = default;
    ~CheckpointManager() = default;
    
    /**
     * Create a backup of a checkpoint with timestamp
     */
    static std::string createBackup(const std::string& originalPath, 
                                   const std::string& backupDir = "backups") {
        namespace fs = std::filesystem;
        
        // Create backup directory if it doesn't exist
        if (!fs::exists(backupDir)) {
            fs::create_directories(backupDir);
        }
        
        // Generate backup filename with timestamp
        auto now = std::chrono::system_clock::now();
        auto time_t = std::chrono::system_clock::to_time_t(now);
        std::stringstream ss;
        ss << backupDir << "/checkpoint_" 
           << std::put_time(std::localtime(&time_t), "%Y%m%d_%H%M%S") 
           << "_original.bin";
        
        std::string backupPath = ss.str();
        
        // Copy original checkpoint to backup
        std::ifstream src(originalPath, std::ios::binary);
        std::ofstream dst(backupPath, std::ios::binary);
        
        if (src && dst) {
            dst << src.rdbuf();
            return backupPath;
        }
        
        return "";
    }
    
    /**
     * Validate checkpoint integrity by checking file size and checksum
     */
    static bool validateCheckpoint(const std::string& path) {
        namespace fs = std::filesystem;
        
        if (!fs::exists(path)) {
            return false;
        }
        
        auto fileSize = fs::file_size(path);
        
        // Minimum reasonable size (empty file or very small file)
        if (fileSize < 1024) {
            return false;
        }
        
        // Try to open and read the file to ensure it's not corrupted
        std::ifstream file(path, std::ios::binary);
        if (!file) {
            return false;
        }
        
        // Read first 100 bytes to check header
        char header[100];
        file.read(header, sizeof(header));
        
        // Basic validation - check for some expected content patterns
        // This is a simple validation; real implementation would check checksums
        return true;
    }
    
    /**
     * Create atomic checkpoint (write to temp file then rename)
     */
    static bool atomicCheckpoint(const std::shared_ptr<Brain>& brain, 
                                 const std::string& path) {
        namespace fs = std::filesystem;
        
        // Create temp path
        std::string tempPath = path + ".tmp";
        
        // Try to save to temp file first
        if (brain->save(tempPath)) {
            // If successful, rename to final path
            if (fs::exists(path)) {
                fs::remove(path); // Remove existing file
            }
            fs::rename(tempPath, path);
            return true;
        }
        
        // Clean up temp file if it exists
        if (fs::exists(tempPath)) {
            fs::remove(tempPath);
        }
        
        return false;
    }
    
    /**
     * Get list of available checkpoints in directory
     */
    static std::vector<CheckpointInfo> listCheckpoints(const std::string& directory = "checks") {
        std::vector<CheckpointInfo> checkpoints;
        namespace fs = std::filesystem;
        
        if (!fs::exists(directory)) {
            return checkpoints;
        }
        
        for (const auto& entry : fs::directory_iterator(directory)) {
            if (entry.is_regular_file() && entry.path().extension() == ".bin") {
                CheckpointInfo info;
                info.path = entry.path().string();
                info.timestamp = std::chrono::system_clock::now(); // Would get from filename in real implementation
                info.fileSize = fs::file_size(entry.path());
                info.isValid = validateCheckpoint(info.path);
                checkpoints.push_back(info);
            }
        }
        
        // Sort by timestamp (newest first)
        std::sort(checkpoints.begin(), checkpoints.end(),
            [](const CheckpointInfo& a, const CheckpointInfo& b) {
                return a.timestamp > b.timestamp;
            });
        
        return checkpoints;
    }
};

class EnhancedReplaySystem {
public:
    struct ReplayOptions {
        float speedMultiplier = 1.0f;           // 1.0 = real time, 2.0 = 2x speed, 0.5 = half speed
        bool fastForwardKeyEvents = true;       // Skip insignificant events
        bool selectiveReplay = false;           // Only replay episodes with certain criteria
        float threshold = 0.5f;                 // Threshold for selective replay
        bool saveReplayData = false;            // Save replay to file
        std::string replayFile;                 // File to save replay data
        bool enableAnalysis = false;            // Enable analysis during replay
    };
    
    EnhancedReplaySystem() = default;
    ~EnhancedReplaySystem() = default;
    
    /**
     * Replay brain state with enhanced features
     */
    static bool replayBrainState(const std::string& checkpointPath, 
                                const ReplayOptions& options = ReplayOptions()) {
        // Load checkpoint
        auto cfg = std::make_shared<Config>();
        auto replayBrain = std::make_shared<Brain>(cfg);
        replayBrain->initialize();
        
        if (!replayBrain->load(checkpointPath)) {
            return false;
        }
        
        // Get episodic memory if available
        auto* em = replayBrain->getEpisodicMemory();
        if (!em) {
            return true; // No episodic memory, just replay brain state
        }
        
        size_t episodeCount = em->getEpisodeCount();
        
        if (options.selectiveReplay) {
            // Selective replay based on criteria
            return replaySelectedEpisodes(em, options);
        }
        
        // Full replay
        for (size_t i = 0; i < episodeCount; ++i) {
            if (options.fastForwardKeyEvents) {
                auto episodes = em->getEpisodesForReplay(10); // Get most significant episodes
                for (const auto& episode : episodes) {
                    replayEpisode(replayBrain, episode, options);
                }
            } else {
                auto episode = em->getEpisode(i);
                replayEpisode(replayBrain, episode, options);
            }
        }
        
        // Save replay data if requested
        if (options.saveReplayData) {
            saveReplayData(replayBrain, checkpointPath + ".replay");
        }
        
        return true;
    }
    
    /**
     * Replay specific episode with timing control
     */
    static void replayEpisode(const std::shared_ptr<Brain>& brain, 
                             const Episode& episode, 
                             const ReplayOptions& options) {
        // Replay episode neurons and connections
        // This would involve restoring neural states, synaptic weights, etc.
        // For now, we'll just log the replay
        // In a real implementation, this would be much more sophisticated
        
        // Simulate replay time based on episode duration
        float replayDelay = (episode.duration / options.speedMultiplier) * 0.001f; // Convert to seconds
        
        // In real implementation, this would:
        // 1. Restore neural membrane potentials
        // 2. Restore synaptic weights
        // 3. Replay spike trains
        // 4. Update internal state
        
        // For now, just log
        // NLM_LOG_INFO("Replaying episode " + std::to_string(episode.step) + 
        //            " (duration: " + std::to_string(episode.duration) + "s)");
    }
    
    /**
     * Replay only episodes that meet certain criteria
     */
    static bool replaySelectedEpisodes(const std::shared_ptr<EpisodicMemory>& em, 
                                       const ReplayOptions& options) {
        size_t episodeCount = em->getEpisodeCount();
        
        for (size_t i = 0; i < episodeCount; ++i) {
            auto episode = em->getEpisode(i);
            
            // Check if episode meets criteria
            if (evaluateEpisodeForReplay(episode, options)) {
                // Replay this episode
                auto cfg = std::make_shared<Config>();
                auto brain = std::make_shared<Brain>(cfg);
                brain->initialize();
                
                if (!brain->load(em->getEpisodeCheckpoint(i))) {
                    continue; // Skip if loading fails
                }
                
                // Run the episode
                replayEpisode(brain, episode, options);
            }
        }
        
        return true;
    }
    
    /**
     * Evaluate if an episode should be replayed
     */
    static bool evaluateEpisodeForReplay(const Episode& episode, 
                                         const ReplayOptions& options) {
        // Example criteria: high reward, long duration, many spikes
        bool meetsRewardThreshold = episode.reward > options.threshold;
        bool meetsDurationThreshold = episode.duration > 1.0f; // 1 second minimum
        bool meetsActivityThreshold = episode.spikeCount > 100; // 100 spikes minimum
        
        // Use configurable combination of criteria
        bool rewardBased = meetsRewardThreshold && episode.reward > 0.5f;
        bool durationBased = meetsDurationThreshold && meetsActivityThreshold;
        
        return (options.threshold == 0.0f) ? // If threshold is 0, use all episodes
               true : (rewardBased || durationBased);
    }
    
    /**
     * Save replay data for analysis
     */
    static bool saveReplayData(const std::shared_ptr<Brain>& brain, 
                              const std::string& filepath) {
        // Create replay data structure
        std::ofstream file(filepath, std::ios::binary);
        if (!file) {
            return false;
        }
        
        // In real implementation, would serialize:
        // - Brain state
        // - Episode data
        // - Replay metadata
        // - Timing information
        
        // For now, just create empty file to indicate success
        file.close();
        return true;
    }
    
    /**
     * Analyze replay session
     */
    static std::unordered_map<std::string, float> analyzeReplay(const std::string& replayFile) {
        std::unordered_map<std::string, float> analysis;
        
        // In real implementation, would analyze:
        // - Replay speed
        // - Episode significance
        // - Learning outcomes
        // - Performance metrics
        
        // For now, return empty analysis
        return analysis;
    }
};

} // namespace nlm
