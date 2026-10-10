// Checkpoint indexer implementation
#include "CheckpointIndexer.hpp"
#include "../core/Logger/Logger.hpp"
#include <filesystem>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <stdexcept>

namespace nlm {

CheckpointIndexer::CheckpointIndexer() : indexDir_(), config_(nullptr),
                                        errorCount_(0), stats_(), indexEventCallback_(nullptr) {
}

CheckpointIndexer::~CheckpointIndexer() {
    shutdown();
}

void CheckpointIndexer::configure(const std::string& indexDir, const std::shared_ptr<Config>& config) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    indexDir_ = indexDir;
    config_ = config;
    
    NLM_LOG_INFO("Checkpoint indexer configured successfully");
}

bool CheckpointIndexer::indexCheckpoint(const CheckpointIndexEntry& entry) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    try {
        // Check if checkpoint already exists
        if (index_.find(entry.name) != index_.end()) {
            // Update existing entry
            index_[entry.name] = entry;
        } else {
            // Add new entry
            index_[entry.name] = entry;
        }
        
        // Update tag mappings
        for (const auto& tag : entry.tags) {
            tagToNamesMap_[tag].push_back(entry.name);
        }
        
        // Update name-to-tags mapping
        nameToTagsMap_[entry.name] = entry.tags;
        
        // Update statistics
        stats_.totalCheckpoints++;
        stats_.entriesIndexed++;
        
        updateStats();
        logEvent("indexed", entry.name, entry);
        
        NLM_LOG_INFO("Checkpoint indexed: " + entry.name);
        return true;
        
    } catch (const std::exception& e) {
        errorCount_++;
        NLM_LOG_ERROR(std::string("Exception indexing checkpoint: ") + e.what());
        return false;
    }
}

bool CheckpointIndexer::updateCheckpoint(const std::string& name, const CheckpointIndexEntry& entry) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    auto it = index_.find(name);
    if (it == index_.end()) {
        NLM_LOG_ERROR("Checkpoint not found for update: " + name);
        return false;
    }
    
    try {
        // Remove old tags from mapping
        const auto& oldTags = it->second.tags;
        for (const auto& tag : oldTags) {
            auto& tagNames = tagToNamesMap_[tag];
            tagNames.erase(std::remove(tagNames.begin(), tagNames.end(), name), tagNames.end());
            if (tagNames.empty()) {
                tagToNamesMap_.erase(tag);
            }
        }
        
        // Update entry
        it->second = entry;
        
        // Update tag mappings
        for (const auto& tag : entry.tags) {
            tagToNamesMap_[tag].push_back(entry.name);
        }
        
        // Update name-to-tags mapping
        nameToTagsMap_[entry.name] = entry.tags;
        
        logEvent("updated", name, entry);
        NLM_LOG_INFO("Checkpoint updated: " + name);
        
        return true;
        
    } catch (const std::exception& e) {
        errorCount_++;
        NLM_LOG_ERROR(std::string("Exception updating checkpoint: ") + e.what());
        return false;
    }
}

bool CheckpointIndexer::removeCheckpoint(const std::string& name) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    auto it = index_.find(name);
    if (it == index_.end()) {
        NLM_LOG_ERROR("Checkpoint not found for removal: " + name);
        return false;
    }
    
    try {
        // Remove from tag mappings
        const auto& tags = it->second.tags;
        for (const auto& tag : tags) {
            auto& tagNames = tagToNamesMap_[tag];
            tagNames.erase(std::remove(tagNames.begin(), tagNames.end(), name), tagNames.end());
            if (tagNames.empty()) {
                tagToNamesMap_.erase(tag);
            }
        }
        
        // Remove from main index
        index_.erase(it);
        nameToTagsMap_.erase(name);
        
        stats_.totalCheckpoints--;
        updateStats();
        
        CheckpointIndexEntry emptyEntry;
        logEvent("removed", name, emptyEntry);
        NLM_LOG_INFO("Checkpoint removed: " + name);
        
        return true;
        
    } catch (const std::exception& e) {
        errorCount_++;
        NLM_LOG_ERROR(std::string("Exception removing checkpoint: ") + e.what());
        return false;
    }
}

bool CheckpointIndexer::clearIndex() {
    std::lock_guard<std::mutex> lock(mutex_);
    
    try {
        index_.clear();
        tagToNamesMap_.clear();
        nameToTagsMap_.clear();
        
        stats_.totalCheckpoints = 0;
        stats_.entriesIndexed = 0;
        updateStats();
        
        NLM_LOG_INFO("Checkpoint index cleared");
        return true;
        
    } catch (const std::exception& e) {
        errorCount_++;
        NLM_LOG_ERROR(std::string("Exception clearing checkpoint index: ") + e.what());
        return false;
    }
}

std::vector<CheckpointIndexEntry> CheckpointIndexer::search(const IndexSearchCriteria& criteria) const {
    std::lock_guard<std::mutex> lock(mutex_);
    
    std::vector<CheckpointIndexEntry> results;
    
    try {
        for (const auto& pair : index_) {
            const auto& entry = pair.second;
            
            if (matchesCriteria(entry, criteria)) {
                results.push_back(entry);
            }
        }
        
        // Sort results by timestamp (newest first)
        std::sort(results.begin(), results.end(),
                 [](const CheckpointIndexEntry& a, const CheckpointIndexEntry& b) {
                     return a.timestamp > b.timestamp;
                 });
        
        return results;
        
    } catch (const std::exception& e) {
        errorCount_++;
        NLM_LOG_ERROR(std::string("Exception searching checkpoints: ") + e.what());
        return {};
    }
}

std::vector<CheckpointIndexEntry> CheckpointIndexer::searchByName(const std::string& pattern) const {
    std::lock_guard<std::mutex> lock(mutex_);
    
    std::vector<CheckpointIndexEntry> results;
    std::string lowerPattern = pattern;
    std::transform(lowerPattern.begin(), lowerPattern.end(), lowerPattern.begin(), ::tolower);
    
    try {
        for (const auto& pair : index_) {
            const auto& entry = pair.second;
            
            if (matchesNamePattern(entry.name, lowerPattern)) {
                results.push_back(entry);
            }
        }
        
        return results;
        
    } catch (const std::exception& e) {
        errorCount_++;
        NLM_LOG_ERROR(std::string("Exception searching checkpoints by name: ") + e.what());
        return {};
    }
}

std::vector<CheckpointIndexEntry> CheckpointIndexer::searchByTag(const std::string& tag) const {
    std::lock_guard<std::mutex> lock(mutex_);
    
    std::vector<CheckpointIndexEntry> results;
    std::string lowerTag = tag;
    std::transform(lowerTag.begin(), lowerTag.end(), lowerTag.begin(), ::tolower);
    
    try {
        auto it = tagToNamesMap_.find(lowerTag);
        if (it == tagToNamesMap_.end()) {
            return results;
        }
        
        for (const auto& name : it->second) {
            auto entryIt = index_.find(name);
            if (entryIt != index_.end()) {
                results.push_back(entryIt->second);
            }
        }
        
        return results;
        
    } catch (const std::exception& e) {
        errorCount_++;
        NLM_LOG_ERROR(std::string("Exception searching checkpoints by tag: ") + e.what());
        return {};
    }
}

std::vector<CheckpointIndexEntry> CheckpointIndexer::searchByTimeRange(time_t minTime, time_t maxTime) const {
    std::lock_guard<std::mutex> lock(mutex_);
    
    std::vector<CheckpointIndexEntry> results;
    
    try {
        for (const auto& pair : index_) {
            const auto& entry = pair.second;
            
            if (entry.timestamp >= minTime && entry.timestamp <= maxTime) {
                results.push_back(entry);
            }
        }
        
        // Sort results by timestamp (oldest first)
        std::sort(results.begin(), results.end(),
                 [](const CheckpointIndexEntry& a, const CheckpointIndexEntry& b) {
                     return a.timestamp < b.timestamp;
                 });
        
        return results;
        
    } catch (const std::exception& e) {
        errorCount_++;
        NLM_LOG_ERROR(std::string("Exception searching checkpoints by time range: ") + e.what());
        return {};
    }
}

std::vector<CheckpointIndexEntry> CheckpointIndexer::searchByStepRange(uint64_t minStep, uint64_t maxStep) const {
    std::lock_guard<std::mutex> lock(mutex_);
    
    std::vector<CheckpointIndexEntry> results;
    
    try {
        for (const auto& pair : index_) {
            const auto& entry = pair.second;
            
            if (entry.step >= minStep && entry.step <= maxStep) {
                results.push_back(entry);
            }
        }
        
        // Sort results by step (ascending)
        std::sort(results.begin(), results.end(),
                 [](const CheckpointIndexEntry& a, const CheckpointIndexEntry& b) {
                     return a.step < b.step;
                 });
        
        return results;
        
    } catch (const std::exception& e) {
        errorCount_++;
        NLM_LOG_ERROR(std::string("Exception searching checkpoints by step range: ") + e.what());
        return {};
    }
}

std::optional<CheckpointIndexEntry> CheckpointIndexer::getCheckpoint(const std::string& name) const {
    std::lock_guard<std::mutex> lock(mutex_);
    
    auto it = index_.find(name);
    if (it == index_.end()) {
        return std::nullopt;
    }
    
    return it->second;
}

CheckpointIndexEntry CheckpointIndexer::getLatestCheckpoint() const {
    std::lock_guard<std::mutex> lock(mutex_);
    
    if (index_.empty()) {
        return CheckpointIndexEntry();
    }
    
    auto it = std::max_element(index_.begin(), index_.end(),
                              [](const auto& a, const auto& b) {
                                  return a.second.timestamp < b.second.timestamp;
                              });
    
    return it->second;
}

std::vector<CheckpointIndexEntry> CheckpointIndexer::getRecentCheckpoints(size_t limit) const {
    std::lock_guard<std::mutex> lock(mutex_);
    
    std::vector<CheckpointIndexEntry> results;
    results.reserve(limit);
    
    try {
        // Copy entries and sort by timestamp (newest first)
        std::vector<CheckpointIndexEntry> allEntries;
        allEntries.reserve(index_.size());
        for (const auto& pair : index_) {
            allEntries.push_back(pair.second);
        }
        
        std::sort(allEntries.begin(), allEntries.end(),
                 [](const CheckpointIndexEntry& a, const CheckpointIndexEntry& b) {
                     return a.timestamp > b.timestamp;
                 });
        
        // Take top N
        size_t count = std::min(limit, allEntries.size());
        results.assign(allEntries.begin(), allEntries.begin() + count);
        
        return results;
        
    } catch (const std::exception& e) {
        errorCount_++;
        NLM_LOG_ERROR(std::string("Exception getting recent checkpoints: ") + e.what());
        return {};
    }
}

bool CheckpointIndexer::addTagToCheckpoint(const std::string& name, const std::string& tag) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    auto entryIt = index_.find(name);
    if (entryIt == index_.end()) {
        NLM_LOG_ERROR("Checkpoint not found for tag addition: " + name);
        return false;
    }
    
    try {
        std::string lowerTag = tag;
        std::transform(lowerTag.begin(), lowerTag.end(), lowerTag.begin(), ::tolower);
        
        // Check if tag already exists
        if (std::find(entryIt->second.tags.begin(), entryIt->second.tags.end(), lowerTag) != entryIt->second.tags.end()) {
            return true; // Already tagged
        }
        
        // Add tag to entry
        entryIt->second.tags.push_back(lowerTag);
        
        // Update tag mapping
        tagToNamesMap_[lowerTag].push_back(name);
        
        logEvent("tagged", name, entryIt->second);
        NLM_LOG_INFO("Tag added to checkpoint: " + name + " -> " + tag);
        
        return true;
        
    } catch (const std::exception& e) {
        errorCount_++;
        NLM_LOG_ERROR(std::string("Exception adding tag to checkpoint: ") + e.what());
        return false;
    }
}

bool CheckpointIndexer::removeTagFromCheckpoint(const std::string& name, const std::string& tag) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    auto entryIt = index_.find(name);
    if (entryIt == index_.end()) {
        NLM_LOG_ERROR("Checkpoint not found for tag removal: " + name);
        return false;
    }
    
    try {
        std::string lowerTag = tag;
        std::transform(lowerTag.begin(), lowerTag.end(), lowerTag.begin(), ::tolower);
        
        // Remove tag from entry
        auto tagIt = std::find(entryIt->second.tags.begin(), entryIt->second.tags.end(), lowerTag);
        if (tagIt == entryIt->second.tags.end()) {
            return true; // Tag not found
        }
        
        entryIt->second.tags.erase(tagIt);
        
        // Update tag mapping
        auto& tagNames = tagToNamesMap_[lowerTag];
        tagNames.erase(std::remove(tagNames.begin(), tagNames.end(), name), tagNames.end());
        if (tagNames.empty()) {
            tagToNamesMap_.erase(lowerTag);
        }
        
        logEvent("untagged", name, entryIt->second);
        NLM_LOG_INFO("Tag removed from checkpoint: " + name + " -> " + tag);
        
        return true;
        
    } catch (const std::exception& e) {
        errorCount_++;
        NLM_LOG_ERROR(std::string("Exception removing tag from checkpoint: ") + e.what());
        return false;
    }
}

bool CheckpointIndexer::updateCheckpointDescription(const std::string& name, const std::string& description) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    auto it = index_.find(name);
    if (it == index_.end()) {
        NLM_LOG_ERROR("Checkpoint not found for description update: " + name);
        return false;
    }
    
    try {
        it->second.description = description;
        logEvent("description_updated", name, it->second);
        NLM_LOG_INFO("Checkpoint description updated: " + name);
        
        return true;
        
    } catch (const std::exception& e) {
        errorCount_++;
        NLM_LOG_ERROR(std::string("Exception updating checkpoint description: ") + e.what());
        return false;
    }
}

bool CheckpointIndexer::saveIndex(const std::string& filepath) const {
    std::lock_guard<std::mutex> lock(mutex_);
    
    try {
        return saveIndexToFile(filepath);
    } catch (const std::exception& e) {
        errorCount_++;
        NLM_LOG_ERROR(std::string("Exception saving checkpoint index: ") + e.what());
        return false;
    }
}

bool CheckpointIndexer::loadIndex(const std::string& filepath) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    try {
        return loadIndexFromFile(filepath);
    } catch (const std::exception& e) {
        errorCount_++;
        NLM_LOG_ERROR(std::string("Exception loading checkpoint index: ") + e.what());
        return false;
    }
}

void CheckpointIndexer::updateStats() {
    stats_.entriesIndexed = index_.size();
    
    // Estimate index size (rough approximation)
    uint64_t totalSize = 0;
    for (const auto& pair : index_) {
        totalSize += sizeof(CheckpointIndexEntry) + pair.first.length();
    }
    stats_.indexSize = totalSize;
}

void CheckpointIndexer::logEvent(const std::string& eventType, const std::string& checkpointName,
                                 const CheckpointIndexEntry& entry) {
    std::stringstream logMsg;
    logMsg << "[CheckpointIndexer] " << eventType << " - Checkpoint: " << checkpointName;
    
    NLM_LOG_INFO(logMsg.str());
    
    // Call event callback if registered
    if (indexEventCallback_) {
        indexEventCallback_(checkpointName, entry);
    }
}

bool CheckpointIndexer::matchesCriteria(const CheckpointIndexEntry& entry, const IndexSearchCriteria& criteria) const {
    // Check name pattern
    if (!criteria.namePatterns.empty()) {
        bool nameMatch = false;
        for (const auto& pattern : criteria.namePatterns) {
            if (matchesNamePattern(entry.name, pattern)) {
                nameMatch = true;
                break;
            }
        }
        if (!nameMatch) {
            return false;
        }
    }
    
    // Check step range
    if (entry.step < criteria.minStep || entry.step > criteria.maxStep) {
        return false;
    }
    
    // Check simulation time range
    if (entry.simulationTime < criteria.minSimulationTime || entry.simulationTime > criteria.maxSimulationTime) {
        return false;
    }
    
    // Check neuron count range
    if (entry.neuronCount < criteria.minNeuronCount || entry.neuronCount > criteria.maxNeuronCount) {
        return false;
    }
    
    // Check timestamp range
    if (entry.timestamp < criteria.minTimestamp || entry.timestamp > criteria.maxTimestamp) {
        return false;
    }
    
    // Check required tags
    if (!criteria.requiredTags.empty()) {
        for (const auto& tag : criteria.requiredTags) {
            std::string lowerTag = tag;
            std::transform(lowerTag.begin(), lowerTag.end(), lowerTag.begin(), ::tolower);
            if (std::find(entry.tags.begin(), entry.tags.end(), lowerTag) == entry.tags.end()) {
                return false;
            }
        }
    }
    
    // Check excluded tags
    if (!criteria.excludedTags.empty()) {
        for (const auto& tag : criteria.excludedTags) {
            std::string lowerTag = tag;
            std::transform(lowerTag.begin(), lowerTag.end(), lowerTag.begin(), ::tolower);
            if (std::find(entry.tags.begin(), entry.tags.end(), lowerTag) != entry.tags.end()) {
                return false;
            }
        }
    }
    
    return true;
}

bool CheckpointIndexer::matchesNamePattern(const std::string& entryName, const std::string& pattern) const {
    std::string lowerEntryName = entryName;
    std::transform(lowerEntryName.begin(), lowerEntryName.end(), lowerEntryName.begin(), ::tolower);
    std::string lowerPattern = pattern;
    std::transform(lowerPattern.begin(), lowerPattern.end(), lowerPattern.begin(), ::tolower);
    
    // Simple wildcard matching for now
    if (lowerPattern.find('*') != std::string::npos) {
        // Handle wildcards - simplified implementation
        return lowerEntryName.find(lowerPattern) != std::string::npos;
    }
    
    // Exact match
    return lowerEntryName == lowerPattern;
}

bool CheckpointIndexer::initialize() {
    return true;
}

void CheckpointIndexer::cleanup() {
    index_.clear();
    tagToNamesMap_.clear();
    nameToTagsMap_.clear();
}

bool CheckpointIndexer::saveIndexToFile(const std::string& filepath) const {
    std::ofstream file(filepath, std::ios::binary);
    if (!file.is_open()) {
        return false;
    }
    
    try {
        // Write number of entries
        uint64_t entryCount = index_.size();
        file.write(reinterpret_cast<const char*>(&entryCount), sizeof(entryCount));
        
        // Write each entry
        for (const auto& pair : index_) {
            const auto& entry = pair.second;
            
            // Write entry fields (simplified serialization)
            uint32_t nameLen = static_cast<uint32_t>(entry.name.length());
            file.write(reinterpret_cast<const char*>(&nameLen), sizeof(nameLen));
            file.write(entry.name.c_str(), nameLen);
            
            file.write(reinterpret_cast<const char*>(&entry.step), sizeof(entry.step));
            file.write(reinterpret_cast<const char*>(&entry.simulationTime), sizeof(entry.simulationTime));
            file.write(reinterpret_cast<const char*>(&entry.neuronCount), sizeof(entry.neuronCount));
            file.write(reinterpret_cast<const char*>(&entry.synapseCount), sizeof(entry.synapseCount));
            file.write(reinterpret_cast<const char*>(&entry.regionCount), sizeof(entry.regionCount));
            file.write(reinterpret_cast<const char*>(&entry.fileSize), sizeof(entry.fileSize));
            file.write(reinterpret_cast<const char*>(&entry.timestamp), sizeof(entry.timestamp));
            file.write(reinterpret_cast<const char*>(&entry.checksum), sizeof(entry.checksum));
            
            // Write tags
            uint32_t tagCount = static_cast<uint32_t>(entry.tags.size());
            file.write(reinterpret_cast<const char*>(&tagCount), sizeof(tagCount));
            for (const auto& tag : entry.tags) {
                uint32_t tagLen = static_cast<uint32_t>(tag.length());
                file.write(reinterpret_cast<const char*>(&tagLen), sizeof(tagLen));
                file.write(tag.c_str(), tagLen);
            }
            
            // Write description
            uint32_t descLen = static_cast<uint32_t>(entry.description.length());
            file.write(reinterpret_cast<const char*>(&descLen), sizeof(descLen));
            file.write(entry.description.c_str(), descLen);
        }
        
        file.close();
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Exception saving checkpoint index to file: ") + e.what());
        return false;
    }
}

bool CheckpointIndexer::loadIndexFromFile(const std::string& filepath) {
    std::ifstream file(filepath, std::ios::binary);
    if (!file.is_open()) {
        return false;
    }
    
    try {
        // Clear existing index
        index_.clear();
        tagToNamesMap_.clear();
        nameToTagsMap_.clear();
        
        // Read number of entries
        uint64_t entryCount = 0;
        file.read(reinterpret_cast<char*>(&entryCount), sizeof(entryCount));
        
        // Read each entry
        for (uint64_t i = 0; i < entryCount; ++i) {
            CheckpointIndexEntry entry;
            
            // Read entry fields
            uint32_t nameLen = 0;
            file.read(reinterpret_cast<char*>(&nameLen), sizeof(nameLen));
            
            std::vector<char> nameBuffer(nameLen);
            file.read(nameBuffer.data(), nameLen);
            entry.name = std::string(nameBuffer.data(), nameLen);
            
            file.read(reinterpret_cast<char*>(&entry.step), sizeof(entry.step));
            file.read(reinterpret_cast<char*>(&entry.simulationTime), sizeof(entry.simulationTime));
            file.read(reinterpret_cast<char*>(&entry.neuronCount), sizeof(entry.neuronCount));
            file.read(reinterpret_cast<char*>(&entry.synapseCount), sizeof(entry.synapseCount));
            file.read(reinterpret_cast<char*>(&entry.regionCount), sizeof(entry.regionCount));
            file.read(reinterpret_cast<char*>(&entry.fileSize), sizeof(entry.fileSize));
            file.read(reinterpret_cast<char*>(&entry.timestamp), sizeof(entry.timestamp));
            file.read(reinterpret_cast<char*>(&entry.checksum), sizeof(entry.checksum));
            
            // Read tags
            uint32_t tagCount = 0;
            file.read(reinterpret_cast<char*>(&tagCount), sizeof(tagCount));
            entry.tags.reserve(tagCount);
            for (uint32_t j = 0; j < tagCount; ++j) {
                uint32_t tagLen = 0;
                file.read(reinterpret_cast<char*>(&tagLen), sizeof(tagLen));
                
                std::vector<char> tagBuffer(tagLen);
                file.read(tagBuffer.data(), tagLen);
                entry.tags.push_back(std::string(tagBuffer.data(), tagLen));
            }
            
            // Read description
            uint32_t descLen = 0;
            file.read(reinterpret_cast<char*>(&descLen), sizeof(descLen));
            
            std::vector<char> descBuffer(descLen);
            file.read(descBuffer.data(), descLen);
            entry.description = std::string(descBuffer.data(), descLen);
            
            // Add to index
            index_[entry.name] = entry;
            
            // Update tag mappings
            entry.nameLower = entry.name;
            std::transform(entry.nameLower.begin(), entry.nameLower.end(), entry.nameLower.begin(), ::tolower);
            
            for (const auto& tag : entry.tags) {
                tagToNamesMap_[tag].push_back(entry.name);
            }
            
            nameToTagsMap_[entry.name] = entry.tags;
        }
        
        file.close();
        
        updateStats();
        NLM_LOG_INFO("Checkpoint index loaded from file: " + filepath);
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Exception loading checkpoint index from file: ") + e.what());
        return false;
    }
}

} // namespace nlm
