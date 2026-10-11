// CheckpointIndexer.cpp
#include "CheckpointIndexer.hpp"
#include <thread>
#include <future>
#include <shared_mutex>
#include <numeric>
#include <algorithm>
#include <cmath>
#include <sstream>

namespace nlm {
    
    CheckpointIndexer::CheckpointIndexer()
        : autoUpdateEnabled_(true)
        , fullTextSearchEnabled_(true)
        , cacheSize_(100) {}
    
    CheckpointIndexer::~CheckpointIndexer() {
        clearCache();
    }
    
    void CheckpointIndexer::addCheckpoint(const IndexEntry& entry) {
        std::lock_guard<std::mutex> lock(indexMutex_);
        
        bool isNew = entries_.find(entry.checkpointId) == entries_.end();
        
        // Add or update entry
        entries_[entry.checkpointId] = entry;
        
        // Update secondary indexes
        if (entry.step > 0) {
            stepIndex_[entry.step] = entry.checkpointId;
        }
        
        if (entry.timestamp > 0) {
            timeIndex_[entry.timestamp] = entry.checkpointId;
        }
        
        // Update tag index
        addToTagIndex(entry);
        
        // Update author index
        addToAuthorIndex(entry);
        
        // Update full-text index
        addToFullTextIndex(entry);
        
        // Update cache
        addToCache(entry);
        
        // Call update callback
        if (updateCallback_ && isNew) {
            updateCallback_(entry.checkpointId, true);
        }
        
        // Update statistics
        if (isNew) {
            ++stats_.totalEntries;
        }
        
        stats_.lastUpdate = std::time(nullptr);
        updateStatistics();
    }
    
    void CheckpointIndexer::removeCheckpoint(const std::string& checkpointId) {
        std::lock_guard<std::mutex> lock(indexMutex_);
        
        auto it = entries_.find(checkpointId);
        if (it == entries_.end()) {
            return;
        }
        
        const IndexEntry& entry = it->second;
        
        // Remove from primary index
        entries_.erase(it);
        
        // Remove from secondary indexes
        if (entry.step > 0) {
            stepIndex_.erase(entry.step);
        }
        
        if (entry.timestamp > 0) {
            timeIndex_.erase(entry.timestamp);
        }
        
        // Remove from tag index
        removeFromTagIndex(entry);
        
        // Remove from author index
        removeFromAuthorIndex(entry);
        
        // Remove from full-text index
        removeFromFullTextIndex(entry);
        
        // Remove from cache
        clearCache();
        
        // Call update callback
        if (updateCallback_) {
            updateCallback_(checkpointId, false);
        }
        
        // Update statistics
        --stats_.totalEntries;
        updateStatistics();
    }
    
    void CheckpointIndexer::updateCheckpoint(const IndexEntry& entry) {
        // Remove old entry if exists
        removeCheckpoint(entry.checkpointId);
        
        // Add new entry
        addCheckpoint(entry);
    }
    
    const IndexEntry* CheckpointIndexer::findById(const std::string& checkpointId) const {
        // Check cache first
        const IndexEntry* cached = getFromCache(checkpointId);
        if (cached) {
            incrementCacheHit();
            return cached;
        }
        
        // Check primary index
        auto it = entries_.find(checkpointId);
        if (it != entries_.end()) {
            incrementCacheMiss();
            addToCache(it->second);
            return &it->second;
        }
        
        incrementCacheMiss();
        return nullptr;
    }
    
    const IndexEntry* CheckpointIndexer::findByStep(uint64_t step) const {
        std::lock_guard<std::mutex> lock(indexMutex_);
        
        auto it = stepIndex_.find(step);
        if (it == stepIndex_.end()) {
            return nullptr;
        }
        
        return findById(it->second);
    }
    
    std::vector<IndexEntry> CheckpointIndexer::findByTimeRange(
        time_t startTime,
        time_t endTime) const {
        
        std::lock_guard<std::mutex> lock(indexMutex_);
        
        std::vector<IndexEntry> result;
        
        // Find all timestamps in range
        auto timeIt = timeIndex_.lower_bound(startTime);
        while (timeIt != timeIndex_.end() && timeIt->first <= endTime) {
            const std::string& checkpointId = timeIt->second;
            auto entryIt = entries_.find(checkpointId);
            if (entryIt != entries_.end()) {
                result.push_back(entryIt->second);
            }
            ++timeIt;
        }
        
        return result;
    }
    
    std::vector<IndexEntry> CheckpointIndexer::findByTag(const std::string& tag) const {
        std::lock_guard<std::mutex> lock(indexMutex_);
        
        std::vector<IndexEntry> result;
        
        auto tagIt = tagIndex_.find(tag);
        if (tagIt == tagIndex_.end()) {
            return result;
        }
        
        for (const std::string& checkpointId : tagIt->second) {
            auto entryIt = entries_.find(checkpointId);
            if (entryIt != entries_.end()) {
                result.push_back(entryIt->second);
            }
        }
        
        return result;
    }
    
    std::vector<IndexEntry> CheckpointIndexer::findByAuthor(const std::string& author) const {
        std::lock_guard<std::mutex> lock(indexMutex_);
        
        std::vector<IndexEntry> result;
        
        auto authorIt = authorIndex_.find(author);
        if (authorIt == authorIndex_.end()) {
            return result;
        }
        
        for (const std::string& checkpointId : authorIt->second) {
            auto entryIt = entries_.find(checkpointId);
            if (entryIt != entries_.end()) {
                result.push_back(entryIt->second);
            }
        }
        
        return result;
    }
    
    std::vector<IndexEntry> CheckpointIndexer::search(
        const std::string& query,
        size_t limit) const {
        
        if (!fullTextSearchEnabled_) {
            return std::vector<IndexEntry>();
        }
        
        std::lock_guard<std::mutex> lock(indexMutex_);
        
        // Tokenize query
        std::vector<std::string> queryTokens = tokenize(query);
        
        if (queryTokens.empty()) {
            return std::vector<IndexEntry>();
        }
        
        // Find checkpoints that contain all query tokens
        std::unordered_set<std::string> candidateCheckpoints;
        
        // Get first token's checkpoint IDs
        auto firstTokenIt = fullTextIndex_.find(queryTokens[0]);
        if (firstTokenIt == fullTextIndex_.end()) {
            return std::vector<IndexEntry>();
        }
        
        candidateCheckpoints.insert(firstTokenIt->second.begin(), firstTokenIt->second.end());
        
        // Intersect with other tokens' checkpoint IDs
        for (size_t i = 1; i < queryTokens.size() && !candidateCheckpoints.empty(); ++i) {
            auto tokenIt = fullTextIndex_.find(queryTokens[i]);
            if (tokenIt == fullTextIndex_.end()) {
                candidateCheckpoints.clear();
                break;
            }
            
            std::unordered_set<std::string> intersection;
            for (const std::string& checkpointId : candidateCheckpoints) {
                if (tokenIt->second.find(checkpointId) != tokenIt->second.end()) {
                    intersection.insert(checkpointId);
                }
            }
            
            candidateCheckpoints = std::move(intersection);
        }
        
        // Convert to IndexEntry objects
        std::vector<IndexEntry> result;
        result.reserve(std::min(candidateCheckpoints.size(), limit));
        
        for (const std::string& checkpointId : candidateCheckpoints) {
            auto entryIt = entries_.find(checkpointId);
            if (entryIt != entries_.end()) {
                result.push_back(entryIt->second);
                
                if (result.size() >= limit) {
                    break;
                }
            }
        }
        
        return result;
    }
    
    std::vector<IndexEntry> CheckpointIndexer::findByTags(
        const std::vector<std::string>& tags,
        size_t limit) const {
        
        if (tags.empty()) {
            return std::vector<IndexEntry>();
        }
        
        std::lock_guard<std::mutex> lock(indexMutex_);
        
        // Find first tag's checkpoint IDs
        auto firstTagIt = tagIndex_.find(tags[0]);
        if (firstTagIt == tagIndex_.end()) {
            return std::vector<IndexEntry>();
        }
        
        std::unordered_set<std::string> candidateCheckpoints;
        candidateCheckpoints.insert(firstTagIt->second.begin(), firstTagIt->second.end());
        
        // Intersect with other tags
        for (size_t i = 1; i < tags.size() && !candidateCheckpoints.empty(); ++i) {
            auto tagIt = tagIndex_.find(tags[i]);
            if (tagIt == tagIndex_.end()) {
                candidateCheckpoints.clear();
                break;
            }
            
            std::unordered_set<std::string> intersection;
            for (const std::string& checkpointId : candidateCheckpoints) {
                if (tagIt->second.find(checkpointId) != tagIt->second.end()) {
                    intersection.insert(checkpointId);
                }
            }
            
            candidateCheckpoints = std::move(intersection);
        }
        
        // Convert to IndexEntry objects
        std::vector<IndexEntry> result;
        result.reserve(std::min(candidateCheckpoints.size(), limit));
        
        for (const std::string& checkpointId : candidateCheckpoints) {
            auto entryIt = entries_.find(checkpointId);
            if (entryIt != entries_.end()) {
                result.push_back(entryIt->second);
                
                if (result.size() >= limit) {
                    break;
                }
            }
        }
        
        return result;
    }
    
    std::vector<IndexEntry> CheckpointIndexer::findByStepRange(
        uint64_t minStep,
        uint64_t maxStep) const {
        
        std::lock_guard<std::mutex> lock(indexMutex_);
        
        std::vector<IndexEntry> result;
        
        // Find steps in range
        for (const auto& pair : stepIndex_) {
            if (pair.first >= minStep && pair.first <= maxStep) {
                auto entryIt = entries_.find(pair.second);
                if (entryIt != entries_.end()) {
                    result.push_back(entryIt->second);
                }
            }
        }
        
        return result;
    }
    
    std::vector<IndexEntry> CheckpointIndexer::findByTimeAndTag(
        time_t startTime,
        time_t endTime,
        const std::string& tag,
        size_t limit) const {
        
        std::lock_guard<std::mutex> lock(indexMutex_);
        
        // Find checkpoints by time range
        std::vector<IndexEntry> timeResults = findByTimeRange(startTime, endTime);
        
        // Filter by tag
        std::vector<IndexEntry> tagResults;
        if (timeResults.size() < limit) {
            auto tagIt = tagIndex_.find(tag);
            if (tagIt != tagIndex_.end()) {
                std::unordered_set<std::string> tagCheckpointIds(tagIt->second.begin(), tagIt->second.end());
                
                for (const IndexEntry& entry : timeResults) {
                    if (tagCheckpointIds.find(entry.checkpointId) != tagCheckpointIds.end()) {
                        tagResults.push_back(entry);
                        
                        if (tagResults.size() >= limit) {
                            break;
                        }
                    }
                }
            }
        }
        
        return tagResults;
    }
    
    std::vector<IndexEntry> CheckpointIndexer::getAllCheckpoints() const {
        std::lock_guard<std::mutex> lock(indexMutex_);
        
        std::vector<IndexEntry> result;
        result.reserve(entries_.size());
        
        for (const auto& pair : entries_) {
            result.push_back(pair.second);
        }
        
        return result;
    }
    
    std::vector<IndexEntry> CheckpointIndexer::getLatest(size_t limit) const {
        std::lock_guard<std::mutex> lock(indexMutex_);
        
        std::vector<IndexEntry> result;
        result.reserve(std::min(entries_.size(), limit));
        
        // Find latest checkpoints by timestamp
        std::vector<const IndexEntry*> latestEntries;
        latestEntries.reserve(entries_.size());
        
        for (const auto& pair : entries_) {
            latestEntries.push_back(&pair.second);
        }
        
        // Sort by timestamp (descending)
        std::sort(latestEntries.begin(), latestEntries.end(),
            [](const IndexEntry* a, const IndexEntry* b) {
                return a->timestamp > b->timestamp;
            });
        
        // Copy to result
        size_t count = std::min(latestEntries.size(), limit);
        for (size_t i = 0; i < count; ++i) {
            result.push_back(*latestEntries[i]);
        }
        
        return result;
    }
    
    void CheckpointIndexer::clear() {
        std::lock_guard<std::mutex> lock(indexMutex_);
        
        // Clear all indexes
        entries_.clear();
        stepIndex_.clear();
        timeIndex_.clear();
        tagIndex_.clear();
        authorIndex_.clear();
        fullTextIndex_.clear();
        
        // Clear cache
        clearCache();
        
        // Update statistics
        stats_.totalEntries = 0;
        updateStatistics();
    }
    
    void CheckpointIndexer::rebuildIndex(const std::string& checkpointDir) {
        std::lock_guard<std::mutex> lock(indexMutex_);
        
        // Clear existing index
        clear();
        
        // Scan checkpoint directory
        try {
            for (const auto& entry : std::filesystem::directory_iterator(checkpointDir)) {
                if (entry.is_regular_file() && entry.path().extension() == ".nckpt") {
                    std::string checkpointId = entry.path().stem().string();
                    
                    // Parse checkpoint file to extract metadata
                    // (simplified - in real implementation would read actual checkpoint data)
                    IndexEntry indexEntry;
                    indexEntry.checkpointId = checkpointId;
                    indexEntry.checkpointPath = entry.path().string();
                    indexEntry.timestamp = std::time(nullptr); // Placeholder
                    indexEntry.isCompressed = false; // Placeholder
                    
                    // Add to index
                    addCheckpoint(indexEntry);
                }
            }
        } catch (const std::exception& e) {
            NLM_LOG_ERROR(std::string("Exception rebuilding index: ") + e.what());
        }
    }
    
    bool CheckpointIndexer::exportIndex(const std::string& filepath) const {
        std::lock_guard<std::mutex> lock(indexMutex_);
        
        try {
            std::ofstream file(filepath, std::ios::binary);
            if (!file) {
                return false;
            }
            
            // Write index statistics
            file.write(reinterpret_cast<const char*>(&stats_), sizeof(stats_));
            
            // Write entries
            uint64_t entryCount = entries_.size();
            file.write(reinterpret_cast<const char*>(&entryCount), sizeof(entryCount));
            
            for (const auto& pair : entries_) {
                // Serialize entry (simplified)
                const IndexEntry& entry = pair.second;
                
                // Write entry ID
                uint64_t idLength = entry.checkpointId.size();
                file.write(reinterpret_cast<const char*>(&idLength), sizeof(idLength));
                file.write(entry.checkpointId.c_str(), idLength);
                
                // Write other fields as needed
            }
            
            return file.good();
            
        } catch (const std::exception& e) {
            NLM_LOG_ERROR(std::string("Exception exporting index: ") + e.what());
            return false;
        }
    }
    
    bool CheckpointIndexer::importIndex(const std::string& filepath) {
        try {
            std::ifstream file(filepath, std::ios::binary);
            if (!file) {
                return false;
            }
            
            // Clear existing index
            clear();
            
            // Read index statistics
            file.read(reinterpret_cast<char*>(&stats_), sizeof(stats_));
            
            // Read entry count
            uint64_t entryCount;
            file.read(reinterpret_cast<char*>(&entryCount), sizeof(entryCount));
            
            // Read entries
            for (uint64_t i = 0; i < entryCount; ++i) {
                // Deserialize entry (simplified)
                uint64_t idLength;
                file.read(reinterpret_cast<char*>(&idLength), sizeof(idLength));
                
                std::string checkpointId(idLength, ' ');
                file.read(&checkpointId[0], idLength);
                
                // Create index entry
                IndexEntry entry;
                entry.checkpointId = checkpointId;
                
                // Add to index
                addCheckpoint(entry);
            }
            
            return file.good() && !file.bad();
            
        } catch (const std::exception& e) {
            NLM_LOG_ERROR(std::string("Exception importing index: ") + e.what());
            return false;
        }
    }
    
    void CheckpointIndexer::addToTagIndex(const IndexEntry& entry) {
        for (const std::string& tag : entry.tags) {
            tagIndex_[tag].insert(entry.checkpointId);
        }
    }
    
    void CheckpointIndexer::removeFromTagIndex(const IndexEntry& entry) {
        for (const std::string& tag : entry.tags) {
            auto it = tagIndex_.find(tag);
            if (it != tagIndex_.end()) {
                it->second.erase(entry.checkpointId);
                if (it->second.empty()) {
                    tagIndex_.erase(it);
                }
            }
        }
    }
    
    void CheckpointIndexer::addToAuthorIndex(const IndexEntry& entry) {
        if (!entry.author.empty()) {
            authorIndex_[entry.author].insert(entry.checkpointId);
        }
    }
    
    void CheckpointIndexer::removeFromAuthorIndex(const IndexEntry& entry) {
        if (!entry.author.empty()) {
            auto it = authorIndex_.find(entry.author);
            if (it != authorIndex_.end()) {
                it->second.erase(entry.checkpointId);
                if (it->second.empty()) {
                    authorIndex_.erase(it);
                }
            }
        }
    }
    
    void CheckpointIndexer::addToFullTextIndex(const IndexEntry& entry) {
        if (!fullTextSearchEnabled_) {
            return;
        }
        
        // Tokenize description for full-text search
        std::vector<std::string> tokens = tokenize(entry.description);
        
        for (const std::string& token : tokens) {
            fullTextIndex_[token].insert(entry.checkpointId);
        }
    }
    
    void CheckpointIndexer::removeFromFullTextIndex(const IndexEntry& entry) {
        if (!fullTextSearchEnabled_) {
            return;
        }
        
        // Tokenize description for full-text search
        std::vector<std::string> tokens = tokenize(entry.description);
        
        for (const std::string& token : tokens) {
            auto it = fullTextIndex_.find(token);
            if (it != fullTextIndex_.end()) {
                it->second.erase(entry.checkpointId);
                if (it->second.empty()) {
                    fullTextIndex_.erase(it);
                }
            }
        }
    }
    
    void CheckpointIndexer::addToCache(const IndexEntry& entry) const {
        if (cache_.size() >= cacheSize_) {
            // Remove oldest entry (simple FIFO)
            if (!cache_.empty()) {
                auto it = cache_.begin();
                cache_.erase(it);
            }
        }
        
        cache_[entry.checkpointId] = entry;
    }
    
    const IndexEntry* CheckpointIndexer::getFromCache(const std::string& checkpointId) const {
        auto it = cache_.find(checkpointId);
        if (it != cache_.end()) {
            // Update access time for LRU
            return &it->second;
        }
        
        return nullptr;
    }
    
    void CheckpointIndexer::clearCache() const {
        cache_.clear();
    }
    
    void CheckpointIndexer::updateStatistics() const {
        // Update statistics
        stats_.totalEntries = entries_.size();
        
        // Calculate memory usage (simplified)
        size_t totalMemory = 0;
        for (const auto& pair : entries_) {
            const IndexEntry& entry = pair.second;
            totalMemory += sizeof(IndexEntry) + entry.checkpointId.size() + 
                          entry.checkpointPath.size() + entry.author.size() +
                          entry.description.size();
            for (const std::string& tag : entry.tags) {
                totalMemory += tag.size();
            }
        }
        
        stats_.memoryUsage = totalMemory;
        
        // Calculate average query time (simplified)
        if (stats_.queryCount > 0) {
            stats_.averageQueryTime = 1000.0 / stats_.queryCount; // Placeholder
        }
    }
    
    void CheckpointIndexer::incrementCacheHit() const {
        ++stats_.cacheHits;
    }
    
    void CheckpointIndexer::incrementCacheMiss() const {
        ++stats_.cacheMisses;
    }
    
    void CheckpointIndexer::incrementQueryCount() const {
        ++stats_.queryCount;
    }
    
    std::vector<std::string> CheckpointIndexer::tokenize(const std::string& text) const {
        std::vector<std::string> tokens;
        std::string token;
        
        for (char c : text) {
            if (std::isalnum(c)) {
                token += std::tolower(c);
            } else if (!token.empty()) {
                tokens.push_back(token);
                token.clear();
            }
        }
        
        if (!token.empty()) {
            tokens.push_back(token);
        }
        
        return tokens;
    }
    
    bool CheckpointIndexer::indexWord(const std::string& word, const std::string& checkpointId) {
        if (word.empty()) {
            return false;
        }
        
        fullTextIndex_[word].insert(checkpointId);
        return true;
    }
    
    bool CheckpointIndexer::removeWord(const std::string& word, const std::string& checkpointId) {
        auto it = fullTextIndex_.find(word);
        if (it == fullTextIndex_.end()) {
            return false;
        }
        
        it->second.erase(checkpointId);
        if (it->second.empty()) {
            fullTextIndex_.erase(it);
        }
        
        return true;
    }