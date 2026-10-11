// CheckpointIndexer.hpp
#pragma once

#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include <unordered_set>
#include <mutex>
#include <chrono>
#include <algorithm>
#include <functional>
#include <filesystem>
#include <sstream>
#include <iomanip>

namespace nlm {

/**
 * Fast checkpoint indexer for efficient lookup and search
 * 
 * Features:
 * - Multi-index structure for fast lookup (by ID, step, time, tags, author, etc.)
 * - Inverted index for tag-based search
 * - Time-series index for temporal queries
 * - Full-text index for metadata search
 * - Automatic index maintenance and cleanup
 * - Memory-efficient indexing (bloom filters, etc.)
 * - Parallel index building
 * - Index statistics and metrics
 */

struct IndexStatistics {
    uint64_t totalEntries{0};           // Total indexed entries
    uint64_t memoryUsage{0};           // Memory usage in bytes
    uint64_t buildTime{0};             // Time to build index (milliseconds)
    uint32_t queryCount{0};            // Number of queries performed
    uint32_t cacheHits{0};             // Cache hits
    uint32_t cacheMisses{0};           // Cache misses
    double averageQueryTime{0.0};      // Average query time
    time_t lastUpdate{0};              // Last index update time
    std::string indexFilePath;        // Path to persistent index file
    
    double getCacheHitRate() const {
        uint32_t total = cacheHits + cacheMisses;
        return total > 0 ? static_cast<double>(cacheHits) / total : 0.0;
    }
};

struct IndexEntry {
    std::string checkpointId;         // Checkpoint ID
    std::string checkpointPath;       // File path
    uint64_t step{0};                  // Simulation step
    double simulationTime{0.0};       // Simulation time
    time_t timestamp{0};              // Creation timestamp
    std::string author;               // Author
    std::string description;          // Description
    std::vector<std::string> tags;    // Tags for categorization
    uint64_t fileSize{0};              // File size in bytes
    bool isCompressed{false};         // Is compressed
    uint32_t checksum{0};             // File checksum
    
    IndexEntry() : step(0), simulationTime(0.0), timestamp(0), 
                  fileSize(0), isCompressed(false), checksum(0) {}
};

class CheckpointIndexer {
public:
    CheckpointIndexer();
    ~CheckpointIndexer();
    
    // Disable copying
    CheckpointIndexer(const CheckpointIndexer&) = delete;
    CheckpointIndexer& operator=(const CheckpointIndexer&) = delete;
    
    /**
     * Add checkpoint to index
     * @param entry Checkpoint index entry
     */
    void addCheckpoint(const IndexEntry& entry);
    
    /**
     * Remove checkpoint from index
     * @param checkpointId Checkpoint ID to remove
     */
    void removeCheckpoint(const std::string& checkpointId);
    
    /**
     * Update checkpoint in index
     * @param entry Updated checkpoint index entry
     */
    void updateCheckpoint(const IndexEntry& entry);
    
    /**
     * Find checkpoint by ID
     * @param checkpointId Checkpoint ID to find
     * @return Index entry or nullptr if not found
     */
    const IndexEntry* findById(const std::string& checkpointId) const;
    
    /**
     * Find checkpoint by step
     * @param step Simulation step
     * @return Index entry or nullptr if not found
     */
    const IndexEntry* findByStep(uint64_t step) const;
    
    /**
     * Find checkpoints by time range
     * @param startTime Start timestamp
     * @param endTime End timestamp
     * @return Vector of index entries
     */
    std::vector<IndexEntry> findByTimeRange(time_t startTime, time_t endTime) const;
    
    /**
     * Find checkpoint by tag
     * @param tag Tag to search for
     * @return Vector of index entries
     */
    std::vector<IndexEntry> findByTag(const std::string& tag) const;
    
    /**
     * Find checkpoint by author
     * @param author Author to search for
     * @return Vector of index entries
     */
    std::vector<IndexEntry> findByAuthor(const std::string& author) const;
    
    /**
     * Search checkpoints by full-text query
     * @param query Query string
     * @param limit Maximum results
     * @return Vector of index entries
     */
    std::vector<IndexEntry> search(const std::string& query, size_t limit = 10) const;
    
    /**
     * Get checkpoints with tag filter
     * @param tags Tags to filter by (AND operation)
     * @param limit Maximum results
     * @return Vector of index entries
     */
    std::vector<IndexEntry> findByTags(const std::vector<std::string>& tags, size_t limit = 10) const;
    
    /**
     * Get checkpoints with step range
     * @param minStep Minimum step
     * @param maxStep Maximum step
     * @return Vector of index entries
     */
    std::vector<IndexEntry> findByStepRange(uint64_t minStep, uint64_t maxStep) const;
    
    /**
     * Get checkpoints with time range and tag filter
     * @param startTime Start timestamp
     * @param endTime End timestamp
     * @param tag Tag to filter by
     * @param limit Maximum results
     * @return Vector of index entries
     */
    std::vector<IndexEntry> findByTimeAndTag(time_t startTime, time_t endTime,
                                            const std::string& tag, size_t limit = 10) const;
    
    /**
     * Get all checkpoints
     * @return Vector of all index entries
     */
    std::vector<IndexEntry> getAllCheckpoints() const;
    
    /**
     * Get latest checkpoints
     * @param limit Maximum results
     * @return Vector of index entries
     */
    std::vector<IndexEntry> getLatest(size_t limit = 10) const;
    
    /**
     * Get checkpoint count
     */
    size_t getCount() const { return entries_.size(); }
    
    /**
     * Clear index
     */
    void clear();
    
    /**
     * Rebuild index from disk
     * @param checkpointDir Directory containing checkpoint files
     */
    void rebuildIndex(const std::string& checkpointDir);
    
    /**
     * Export index to file
     * @param filepath Output file path
     */
    bool exportIndex(const std::string& filepath) const;
    
    /**
     * Import index from file
     * @param filepath Input file path
     */
    bool importIndex(const std::string& filepath);
    
    /**
     * Get index statistics
     */
    IndexStatistics getStatistics() const { return stats_; }
    
    /**
     * Set index update callback
     * @param callback Callback function for index updates
     */
    void setUpdateCallback(std::function<void(const std::string& checkpointId, bool added)> callback) {
        updateCallback_ = callback;
    }
    
    /**
     * Enable/disable automatic index updates
     */
    void setAutoUpdateEnabled(bool enabled) { autoUpdateEnabled_ = enabled; }
    
    /**
     * Get auto update enabled
     */
    bool isAutoUpdateEnabled() const { return autoUpdateEnabled_; }
    
    /**
     * Set index cache size
     * @param cacheSize Cache size in entries
     */
    void setCacheSize(size_t cacheSize) { cacheSize_ = cacheSize; }
    
    /**
     * Enable/disable full-text search
     */
    void setFullTextSearchEnabled(bool enabled) { fullTextSearchEnabled_ = enabled; }
    
    /**
     * Get version string
     */
    static std::string getVersion() { return "CheckpointIndexer v1.0"; }
    
private:
    // Index structures
    std::unordered_map<std::string, IndexEntry> entries_;         // Primary index by ID
    std::unordered_map<uint64_t, std::string> stepIndex_;         // Secondary index by step
    std::unordered_map<time_t, std::string> timeIndex_;           // Secondary index by timestamp
    std::unordered_map<std::string, std::unordered_set<std::string>> tagIndex_; // Index by tags
    std::unordered_map<std::string, std::unordered_set<std::string>> authorIndex_; // Index by author
    
    // Full-text search index (simplified)
    std::unordered_map<std::string, std::vector<std::string>> fullTextIndex_; // word -> checkpoint IDs
    
    // Cache for frequently accessed entries
    mutable std::unordered_map<std::string, IndexEntry> cache_;
    
    // Thread safety
    mutable std::mutex indexMutex_;
    
    // Configuration
    bool autoUpdateEnabled_{true};
    bool fullTextSearchEnabled_{true};
    size_t cacheSize_{100};
    
    // Statistics
    IndexStatistics stats_;
    
    // Callbacks
    std::function<void(const std::string& checkpointId, bool added)> updateCallback_;
    
    // Index helpers
    void addToTagIndex(const IndexEntry& entry);
    void removeFromTagIndex(const IndexEntry& entry);
    void addToAuthorIndex(const IndexEntry& entry);
    void removeFromAuthorIndex(const IndexEntry& entry);
    void addToFullTextIndex(const IndexEntry& entry);
    void removeFromFullTextIndex(const IndexEntry& entry);
    
    // Full-text search helpers
    std::vector<std::string> tokenize(const std::string& text) const;
    bool indexWord(const std::string& word, const std::string& checkpointId);
    bool removeWord(const std::string& word, const std::string& checkpointId);
    
    // Cache management
    void addToCache(const IndexEntry& entry) const;
    const IndexEntry* getFromCache(const std::string& checkpointId) const;
    void clearCache() const;
    
    // Statistics
    void updateStatistics() const;
    void incrementCacheHit() const;
    void incrementCacheMiss() const;
    void incrementQueryCount() const;
};