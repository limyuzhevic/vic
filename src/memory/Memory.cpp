#include "Memory.hpp"
#include <algorithm>
#include <unordered_set>
#include <cmath>
#include <sstream>
#include <iomanip>
#include <lz4.h>
#include <lz4hc.h>

namespace nlm {

// Working Memory Implementation
struct WorkingMemory::Impl {
    // Main storage - hash map for O(1) operations
    std::unordered_map<uint64_t, float> storage;
    
    // Sliding window buffer for capacity management
    std::deque<uint64_t> accessOrder;
    
    // LRU cache for pattern-based access
    LRUCache<uint64_t, float> lruCache;
    
    // Performance metrics
    PerformanceMetrics metrics;
    
    // Thread safety
    mutable std::mutex mutex;
    
    // Memory usage tracking
    size_t capacity;
    size_t currentSize;
    
    Impl(size_t cap) : capacity(cap), currentSize(0), lruCache(capacity) {}
};

WorkingMemory::WorkingMemory() : pImpl(new Impl(100)) {}

WorkingMemory::~WorkingMemory() = default;

void WorkingMemory::store(NeuronId neuron, float value) {
    auto start = std::chrono::high_resolution_clock::now();
    
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    bool success = false;
    
    try {
        // Update or add to storage
        auto it = pImpl->storage.find(neuron.value);
        if (it != pImpl->storage.end()) {
            it->second = value;
            success = true;
        } else {
            if (pImpl->currentSize < pImpl->capacity) {
                pImpl->storage[neuron.value] = value;
                pImpl->accessOrder.push_back(neuron.value);
                pImpl->currentSize++;
                success = true;
            } else {
                // Sliding window: remove least recently used
                if (!pImpl->accessOrder.empty()) {
                    uint64_t oldest = pImpl->accessOrder.front();
                    pImpl->storage.erase(oldest);
                    pImpl->accessOrder.pop_front();
                    
                    pImpl->storage[neuron.value] = value;
                    pImpl->accessOrder.push_back(neuron.value);
                    success = true;
                }
            }
        }
        
        // Update LRU cache
        if (success) {
            pImpl->lruCache.put(neuron.value, value);
        }
        
    } catch (const std::exception& e) {
        // Handle any exceptions silently for now
        success = false;
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration<double, std::milli>(end - start).count();
    pImpl->metrics.recordOperation(duration, success);
}

float WorkingMemory::retrieve(NeuronId neuron) const {
    auto start = std::chrono::high_resolution_clock::now();
    
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    float result = 0.0f;
    bool success = false;
    
    try {
        auto it = pImpl->storage.find(neuron.value);
        if (it != pImpl->storage.end()) {
            result = it->second;
            success = true;
            
            // Update access order for sliding window
            pImpl->accessOrder.erase(
                std::remove(pImpl->accessOrder.begin(), pImpl->accessOrder.end(), neuron.value),
                pImpl->accessOrder.end()
            );
            pImpl->accessOrder.push_back(neuron.value);
            
            // Update LRU cache
            pImpl->lruCache.put(neuron.value, result);
        }
    } catch (const std::exception& e) {
        // Handle any exceptions silently for now
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration<double, std::milli>(end - start).count();
    pImpl->metrics.recordOperation(duration, success);
    
    return result;
}

bool WorkingMemory::contains(NeuronId neuron) const {
    auto start = std::chrono::high_resolution_clock::now();
    
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    bool result = pImpl->storage.find(neuron.value) != pImpl->storage.end();
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration<double, std::milli>(end - start).count();
    pImpl->metrics.recordOperation(duration, true);
    
    return result;
}

void WorkingMemory::clear() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    pImpl->storage.clear();
    pImpl->accessOrder.clear();
    pImpl->lruCache.clear();
    pImpl->currentSize = 0;
}

size_t WorkingMemory::getCapacity() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return pImpl->capacity;
}

size_t WorkingMemory::getCurrentSize() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return pImpl->currentSize;
}

void WorkingMemory::decay(float decayRate) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    for (auto& pair : pImpl->storage) {
        pair.second *= (1.0f - decayRate);
    }
}

double WorkingMemory::getAverageOperationTime() const {
    return pImpl->metrics.getAverageDuration();
}

size_t WorkingMemory::getOperationCount() const {
    return pImpl->metrics.getOperationCount();
}

size_t WorkingMemory::getMemoryUsage() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return pImpl->storage.size() * sizeof(float) + pImpl->accessOrder.size() * sizeof(uint64_t);
}

void WorkingMemory::cacheAccess(NeuronId neuron) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    if (pImpl->storage.find(neuron.value) != pImpl->storage.end()) {
        pImpl->lruCache.put(neuron.value, pImpl->storage[neuron.value]);
    }
}

bool WorkingMemory::isCached(NeuronId neuron) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return pImpl->lruCache.contains(neuron.value);
}

std::string WorkingMemory::serialize() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    std::ostringstream oss;
    oss << "WorkingMemory:" << pImpl->currentSize << ":";
    
    for (const auto& pair : pImpl->storage) {
        oss << pair.first << ":" << pair.second << ";";
    }
    
    std::string data = oss.str();
    
    // Compress using LZ4
    std::vector<char> compressed(data.size() * 2);
    int compressedSize = LZ4_compress_default(data.c_str(), compressed.data(), data.size(), compressed.size());
    
    if (compressedSize > 0) {
        return std::string(compressed.begin(), compressed.begin() + compressedSize);
    }
    
    return data;
}

bool WorkingMemory::deserialize(const std::string& data) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    clear();
    
    // Try to decompress
    std::vector<char> decompressed(data.size() * 2);
    int decompressedSize = LZ4_decompress_safe(data.c_str(), decompressed.data(), data.size(), decompressed.size());
    
    std::string textData;
    if (decompressedSize > 0) {
        textData.assign(decompressed.begin(), decompressed.begin() + decompressedSize);
    } else {
        textData = data;
    }
    
    // Parse the data
    size_t pos = textData.find(":");
    if (pos == std::string::npos) return false;
    
    pImpl->currentSize = std::stoull(textData.substr(0, pos));
    textData = textData.substr(pos + 1);
    
    size_t start = 0;
    size_t end = textData.find(";");
    while (end != std::string::npos) {
        std::string pair = textData.substr(start, end - start);
        size_t colonPos = pair.find(":");
        if (colonPos != std::string::npos) {
            uint64_t key = std::stoull(pair.substr(0, colonPos));
            float value = std::stof(pair.substr(colonPos + 1));
            pImpl->storage[key] = value;
            pImpl->accessOrder.push_back(key);
        }
        start = end + 1;
        end = textData.find(";", start);
    }
    
    return !pImpl->storage.empty();
}

// Implement the Trie class helper functions
Trie::Trie() : root_(std::make_unique<TrieNode>()) {}
Trie::~Trie() = default;

void Trie::insert(const std::string& word) {
    std::lock_guard<std::mutex> lock(mutex_);
    TrieNode* current = root_.get();
    for (char c : word) {
        if (!current->children.count(c)) {
            current->children[c] = std::make_unique<TrieNode>();
        }
        current = current->children[c].get();
    }
    current->isEndOfWord = true;
    current->word = word;
}

bool Trie::search(const std::string& word) const {
    std::lock_guard<std::mutex> lock(mutex_);
    TrieNode* current = root_.get();
    for (char c : word) {
        if (!current->children.count(c)) {
            return false;
        }
        current = current->children[c].get();
    }
    return current->isEndOfWord;
}

bool Trie::startsWith(const std::string& prefix) const {
    std::lock_guard<std::mutex> lock(mutex_);
    TrieNode* current = root_.get();
    for (char c : prefix) {
        if (!current->children.count(c)) {
            return false;
        }
        current = current->children[c].get();
    }
    return true;
}

void Trie::addSemanticAssociation(const std::string& word, const std::string& concept, float weight) {
    std::lock_guard<std::mutex> lock(mutex_);
    TrieNode* current = root_.get();
    for (char c : word) {
        current = current->children[c].get();
        if (!current) return;
    }
    if (current->isEndOfWord) {
        current->semanticAssociations.emplace_back(concept, weight);
    }
}

std::vector<std::pair<std::string, float>> Trie::getSemanticAssociations(const std::string& word) const {
    std::lock_guard<std::mutex> lock(mutex_);
    TrieNode* current = root_.get();
    for (char c : word) {
        current = current->children[c].get();
        if (!current) {
            return {};
        }
    }
    return current->isEndOfWord ? current->semanticAssociations : std::vector<std::pair<std::string, float>>();
}

void Trie::clear() {
    std::lock_guard<std::mutex> lock(mutex_);
    root_ = std::make_unique<TrieNode>();
}

// Episodic Memory Implementation
struct EpisodicMemory::Impl {
    // Main storage - vector for ordered access
    std::vector<EpisodicMemoryItem> episodes;
    
    // Priority queue for relevance-based access
    std::priority_queue<EpisodePriorityNode> relevanceQueue;
    
    // Hash map for O(1) episode lookup by timestamp
    std::unordered_map<size_t, size_t> timestampIndex;
    
    // Hash map for episode metadata lookup
    std::unordered_map<std::string, std::vector<size_t>> metadataIndex;
    
    // Circular buffer for fixed storage
    std::deque<EpisodicMemoryItem> circularBuffer;
    
    // Performance tracking
    PerformanceMetrics metrics;
    
    // Thread safety
    mutable std::mutex mutex;
    
    // Memory limits
    size_t maxEpisodes;
    size_t currentSize;
    
    Impl(size_t max) : maxEpisodes(max), currentSize(0) {}
};

EpisodicMemory::EpisodicMemory() : pImpl(new Impl(1000)) {}

EpisodicMemory::~EpisodicMemory() = default;

void EpisodicMemory::storeEpisode(const EpisodicMemoryItem& episode) {
    auto start = std::chrono::high_resolution_clock::now();
    
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    bool success = false;
    
    try {
        // Calculate episode index for priority queue
        size_t index = pImpl->episodes.size();
        
        // Calculate relevance based on various factors
        float relevance = 1.0f;
        
        // Store in main episodes vector
        pImpl->episodes.push_back(episode);
        
        // Update timestamp index
        pImpl->timestampIndex[episode.timestamp] = index;
        
        // Update metadata index
        if (!episode.metadata.empty()) {
            pImpl->metadataIndex[episode.metadata].push_back(index);
        }
        
        // Add to priority queue for relevance-based access
        EpisodePriorityNode node{index, relevance};
        pImpl->relevanceQueue.push(node);
        
        // Add to circular buffer
        pImpl->circularBuffer.push_back(episode);
        
        // Remove oldest if over capacity
        if (pImpl->currentSize >= pImpl->maxEpisodes) {
            if (!pImpl->circularBuffer.empty()) {
                EpisodicMemoryItem oldest = pImpl->circularBuffer.front();
                
                // Remove from indexes
                auto tsIt = pImpl->timestampIndex.find(oldest.timestamp);
                if (tsIt != pImpl->timestampIndex.end()) {
                    size_t oldestIndex = tsIt->second;
                    pImpl->timestampIndex.erase(tsIt);
                    
                    // Update other indexes
                    for (auto& [key, indices] : pImpl->metadataIndex) {
                        indices.erase(std::remove(indices.begin(), indices.end(), oldestIndex), indices.end());
                    }
                }
                
                pImpl->circularBuffer.pop_front();
            }
        } else {
            pImpl->currentSize++;
        }
        
        success = true;
        
    } catch (const std::exception& e) {
        success = false;
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration<double, std::milli>(end - start).count();
    pImpl->metrics.recordOperation(duration, success);
}

EpisodicMemoryItem EpisodicMemory::retrieveEpisode(size_t index) const {
    auto start = std::chrono::high_resolution_clock::now();
    
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    EpisodicMemoryItem result;
    bool success = false;
    
    try {
        if (index < pImpl->episodes.size()) {
            result = pImpl->episodes[index];
            success = true;
        }
    } catch (const std::exception& e) {
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration<double, std::milli>(end - start).count();
    pImpl->metrics.recordOperation(duration, success);
    
    return result;
}

size_t EpisodicMemory::getEpisodeCount() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return pImpl->currentSize;
}

std::vector<EpisodicMemoryItem> EpisodicMemory::getRecentEpisodes(size_t count) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    std::vector<EpisodicMemoryItem> result;
    if (count > pImpl->currentSize) {
        count = pImpl->currentSize;
    }
    
    size_t start = pImpl->currentSize > count ? pImpl->currentSize - count : 0;
    for (size_t i = start; i < pImpl->currentSize; ++i) {
        result.push_back(pImpl->episodes[i]);
    }
    
    return result;
}

void EpisodicMemory::clear() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    pImpl->episodes.clear();
    pImpl->relevanceQueue = std::priority_queue<EpisodePriorityNode>();
    pImpl->timestampIndex.clear();
    pImpl->metadataIndex.clear();
    pImpl->circularBuffer.clear();
    pImpl->currentSize = 0;
}

void EpisodicMemory::consolidate(float relevanceThreshold) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    // Remove low relevance episodes
    std::vector<size_t> toRemove;
    
    for (size_t i = 0; i < pImpl->episodes.size(); ++i) {
        // Get relevance from priority queue (simplified)
        float relevance = 1.0f; // Default relevance
        if (relevance < relevanceThreshold) {
            toRemove.push_back(i);
        }
    }
    
    // Remove episodes (in reverse order to maintain indices)
    for (auto it = toRemove.rbegin(); it != toRemove.rend(); ++it) {
        size_t index = *it;
        
        // Remove from main vector
        pImpl->episodes.erase(pImpl->episodes.begin() + index);
        
        // Update indexes
        if (index < pImpl->episodes.size()) {
            for (auto& [key, indices] : pImpl->metadataIndex) {
                indices.erase(std::remove(indices.begin(), indices.end(), index), indices.end());
            }
        }
        
        pImpl->currentSize--;
    }
}

double EpisodicMemory::getAverageRetrievalTime() const {
    return pImpl->metrics.getAverageDuration();
}

size_t EpisodicMemory::getTotalMemoryAccesses() const {
    return pImpl->metrics.getOperationCount();
}

void EpisodicMemory::updateEpisodeRelevance(size_t index, float relevanceDelta) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    if (index < pImpl->episodes.size()) {
        // Update priority queue entry (simplified - would need actual tracking)
        // In a real implementation, we'd track individual episode relevance
    }
}

std::string EpisodicMemory::serialize() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    std::ostringstream oss;
    oss << "EpisodicMemory:" << pImpl->currentSize << ":";
    
    for (const auto& episode : pImpl->episodes) {
        oss << episode.timestamp << ":" << episode.metadata << ":" << episode.reward << ";";
    }
    
    std::string data = oss.str();
    
    // Compress using LZ4
    std::vector<char> compressed(data.size() * 2);
    int compressedSize = LZ4_compress_default(data.c_str(), compressed.data(), data.size(), compressed.size());
    
    if (compressedSize > 0) {
        return std::string(compressed.begin(), compressed.begin() + compressedSize);
    }
    
    return data;
}

bool EpisodicMemory::deserialize(const std::string& data) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    clear();
    
    // Try to decompress
    std::vector<char> decompressed(data.size() * 2);
    int decompressedSize = LZ4_decompress_safe(data.c_str(), decompressed.data(), data.size(), decompressed.size());
    
    std::string textData;
    if (decompressedSize > 0) {
        textData.assign(decompressed.begin(), decompressed.begin() + decompressedSize);
    } else {
        textData = data;
    }
    
    // Parse the data
    size_t pos = textData.find(":");
    if (pos == std::string::npos) return false;
    
    pImpl->currentSize = std::stoull(textData.substr(0, pos));
    textData = textData.substr(pos + 1);
    
    size_t start = 0;
    size_t end = textData.find(";");
    while (end != std::string::npos) {
        std::string episodeStr = textData.substr(start, end - start);
        size_t tsPos = episodeStr.find(":");
        if (tsPos != std::string::npos) {
            EpisodicMemoryItem episode;
            episode.timestamp = std::stoull(episodeStr.substr(0, tsPos));
            
            size_t metaPos = episodeStr.find(":", tsPos + 1);
            if (metaPos != std::string::npos) {
                episode.metadata = episodeStr.substr(tsPos + 1, metaPos - tsPos - 1);
                
                size_t rewardPos = episodeStr.find(":", metaPos + 1);
                if (rewardPos != std::string::npos) {
                    episode.reward = std::stof(episodeStr.substr(metaPos + 1, rewardPos - metaPos - 1));
                }
            }
            
            pImpl->episodes.push_back(episode);
            pImpl->timestampIndex[episode.timestamp] = pImpl->currentSize;
            pImpl->currentSize++;
        }
        
        start = end + 1;
        end = textData.find(";", start);
    }
    
    return !pImpl->episodes.empty();
}

// Semantic Memory Implementation
struct SemanticMemory::Impl {
    // Main storage - hash map for O(1) operations
    std::unordered_map<std::string, std::string> facts;
    
    // Version control for fact history
    std::unordered_map<std::string, std::vector<std::pair<std::string, uint64_t>>> factVersions;
    
    // Trie structure for semantic associations
    Trie trie;
    
    // Performance metrics
    PerformanceMetrics metrics;
    
    // Thread safety
    mutable std::mutex mutex;
    
    // Memory usage tracking
    size_t currentSize;
    
    Impl() : currentSize(0) {}
};

SemanticMemory::SemanticMemory() : pImpl(new Impl) {}

SemanticMemory::~SemanticMemory() = default;

void SemanticMemory::storeFact(const std::string& key, const std::string& value) {
    auto start = std::chrono::high_resolution_clock::now();
    
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    bool success = false;
    
    try {
        // Check if fact exists
        auto it = pImpl->facts.find(key);
        if (it != pImpl->facts.end()) {
            // Update existing fact
            it->second = value;
            success = true;
            
            // Add version history
            uint64_t version = factVersions.size() > 0 ? factVersions.size() - 1 : 0;
            pImpl->factVersions[key].emplace_back(value, version);
        } else {
            // Store new fact
            pImpl->facts[key] = value;
            success = true;
            
            // Create version entry
            uint64_t version = factVersions.size() > 0 ? factVersions.size() - 1 : 0;
            pImpl->factVersions[key].emplace_back(value, version);
            
            // Add to trie for semantic search
            pImpl->trie.insert(key);
        }
        
        pImpl->currentSize = pImpl->facts.size();
        
    } catch (const std::exception& e) {
        success = false;
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration<double, std::milli>(end - start).count();
    pImpl->metrics.recordOperation(duration, success);
}

std::string SemanticMemory::retrieveFact(const std::string& key) const {
    auto start = std::chrono::high_resolution_clock::now();
    
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    std::string result;
    bool success = false;
    
    try {
        auto it = pImpl->facts.find(key);
        if (it != pImpl->facts.end()) {
            result = it->second;
            success = true;
        }
    } catch (const std::exception& e) {
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration<double, std::milli>(end - start).count();
    pImpl->metrics.recordOperation(duration, success);
    
    return result;
}

bool SemanticMemory::hasFact(const std::string& key) const {
    auto start = std::chrono::high_resolution_clock::now();
    
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    bool result = pImpl->facts.find(key) != pImpl->facts.end();
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration<double, std::milli>(end - start).count();
    pImpl->metrics.recordOperation(duration, true);
    
    return result;
}

std::vector<std::pair<std::string, std::string>> SemanticMemory::getAllFacts() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return std::vector<std::pair<std::string, std::string>>(pImpl->facts.begin(), pImpl->facts.end());
}

void SemanticMemory::clear() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    pImpl->facts.clear();
    pImpl->factVersions.clear();
    pImpl->trie.clear();
    pImpl->currentSize = 0;
}

void SemanticMemory::setFactVersion(const std::string& key, const std::string& value, uint64_t version) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    if (pImpl->facts.find(key) != pImpl->facts.end()) {
        pImpl->factVersions[key].emplace_back(value, version);
    }
}

uint64_t SemanticMemory::getFactVersion(const std::string& key) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    auto it = pImpl->factVersions.find(key);
    if (it != pImpl->factVersions.end() && !it->second.empty()) {
        return it->second.back().second;
    }
    return 0;
}

std::string SemanticMemory::retrieveFactVersion(const std::string& key, uint64_t version) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    auto it = pImpl->factVersions.find(key);
    if (it != pImpl->factVersions.end()) {
        for (const auto& versionEntry : it->second) {
            if (versionEntry.second == version) {
                return versionEntry.first;
            }
        }
    }
    return "";
}

void SemanticMemory::insertWord(const std::string& word) {
    pImpl->trie.insert(word);
}

bool SemanticMemory::searchWord(const std::string& word) const {
    return pImpl->trie.search(word);
}

bool SemanticMemory::startsWithPrefix(const std::string& prefix) const {
    return pImpl->trie.startsWith(prefix);
}

double SemanticMemory::getAverageFactRetrievalTime() const {
    return pImpl->metrics.getAverageDuration();
}

size_t SemanticMemory::getSemanticIndexSize() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return pImpl->trie.getSemanticAssociations("").size() + pImpl->facts.size();
}

std::string SemanticMemory::serialize() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    std::ostringstream oss;
    oss << "SemanticMemory:" << pImpl->currentSize << ":";
    
    for (const auto& pair : pImpl->facts) {
        oss << pair.first << ":" << pair.second << ":" << getFactVersion(pair.first) << ";";
    }
    
    std::string data = oss.str();
    
    // Compress using LZ4
    std::vector<char> compressed(data.size() * 2);
    int compressedSize = LZ4_compress_default(data.c_str(), compressed.data(), data.size(), compressed.size());
    
    if (compressedSize > 0) {
        return std::string(compressed.begin(), compressed.begin() + compressedSize);
    }
    
    return data;
}

bool SemanticMemory::deserialize(const std::string& data) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    clear();
    
    // Try to decompress
    std::vector<char> decompressed(data.size() * 2);
    int decompressedSize = LZ4_decompress_safe(data.c_str(), decompressed.data(), data.size(), decompressed.size());
    
    std::string textData;
    if (decompressedSize > 0) {
        textData.assign(decompressed.begin(), decompressed.begin() + decompressedSize);
    } else {
        textData = data;
    }
    
    // Parse the data
    size_t pos = textData.find(":");
    if (pos == std::string::npos) return false;
    
    pImpl->currentSize = std::stoull(textData.substr(0, pos));
    textData = textData.substr(pos + 1);
    
    size_t start = 0;
    size_t end = textData.find(";");
    while (end != std::string::npos) {
        std::string factStr = textData.substr(start, end - start);
        size_t colon1Pos = factStr.find(":");
        size_t colon2Pos = factStr.find(":", colon1Pos + 1);
        
        if (colon1Pos != std::string::npos && colon2Pos != std::string::npos) {
            std::string key = factStr.substr(0, colon1Pos);
            std::string value = factStr.substr(colon1Pos + 1, colon2Pos - colon1Pos - 1);
            uint64_t version = std::stoull(factStr.substr(colon2Pos + 1));
            
            pImpl->facts[key] = value;
            pImpl->factVersions[key].emplace_back(value, version);
            
            // Add to trie
            pImpl->trie.insert(key);
        }
        
        start = end + 1;
        end = textData.find(";", start);
    }
    
    return !pImpl->facts.empty();
}

// Procedural Memory Implementation
struct ProceduralMemory::Impl {
    // Main storage - hash map for O(1) operations
    std::unordered_map<std::string, Skill> skills;
    
    // Skill dependency graph
    std::vector<SkillDependencyEdge> skillDependencies;
    
    // Performance metrics
    PerformanceMetrics metrics;
    
    // Thread safety
    mutable std::mutex mutex;
    
    // Learning order cache
    std::vector<std::string> learningOrder;
    bool learningOrderDirty;
    
    Impl() : learningOrderDirty(true) {}
};

ProceduralMemory::ProceduralMemory() : pImpl(new Impl) {}

ProceduralMemory::~ProceduralMemory() = default;

void ProceduralMemory::learnSkill(const std::string& name, const std::vector<NeuronId>& pattern) {
    auto start = std::chrono::high_resolution_clock::now();
    
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    bool success = false;
    
    try {
        auto it = pImpl->skills.find(name);
        if (it != pImpl->skills.end()) {
            // Update existing skill
            it->second.neuralPattern = pattern;
            success = true;
        } else {
            // Create new skill
            Skill skill;
            skill.name = name;
            skill.neuralPattern = pattern;
            skill.proficiency = 0.0f;
            
            pImpl->skills[name] = skill;
            success = true;
            
            // Mark learning order as dirty
            pImpl->learningOrderDirty = true;
        }
        
    } catch (const std::exception& e) {
        success = false;
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration<double, std::milli>(end - start).count();
    pImpl->metrics.recordOperation(duration, success);
}

Skill* ProceduralMemory::getSkill(const std::string& name) {
    auto start = std::chrono::high_resolution_clock::now();
    
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    Skill* result = nullptr;
    bool success = false;
    
    try {
        auto it = pImpl->skills.find(name);
        if (it != pImpl->skills.end()) {
            result = &it->second;
            success = true;
        }
    } catch (const std::exception& e) {
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration<double, std::milli>(end - start).count();
    pImpl->metrics.recordOperation(duration, success);
    
    return result;
}

std::vector<Skill*> ProceduralMemory::getAllSkills() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    std::vector<Skill*> result;
    for (auto& pair : pImpl->skills) {
        result.push_back(&pair.second);
    }
    
    return result;
}

void ProceduralMemory::updateProficiency(const std::string& name, float delta) {
    auto start = std::chrono::high_resolution_clock::now();
    
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    bool success = false;
    
    try {
        auto it = pImpl->skills.find(name);
        if (it != pImpl->skills.end()) {
            it->second.proficiency = std::clamp(it->second.proficiency + delta, 0.0f, 1.0f);
            success = true;
        }
        
    } catch (const std::exception& e) {
        success = false;
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration<double, std::milli>(end - start).count();
    pImpl->metrics.recordOperation(duration, success);
}

void ProceduralMemory::clear() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    pImpl->skills.clear();
    pImpl->skillDependencies.clear();
    pImpl->learningOrder.clear();
    pImpl->learningOrderDirty = true;
}

void ProceduralMemory::addSkillDependency(const std::string& dependent, const std::string& prerequisite, float strength) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    SkillDependencyEdge edge;
    edge.dependentSkill = dependent;
    edge.prerequisiteSkill = prerequisite;
    edge.dependencyStrength = strength;
    
    pImpl->skillDependencies.push_back(edge);
    pImpl->learningOrderDirty = true;
}

std::vector<std::string> ProceduralMemory::getSkillDependencies(const std::string& skill) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    std::vector<std::string> result;
    for (const auto& edge : pImpl->skillDependencies) {
        if (edge.dependentSkill == skill) {
            result.push_back(edge.prerequisiteSkill);
        }
    }
    
    return result;
}

bool ProceduralMemory::canExecuteSkill(const std::string& skill) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    // Check if skill exists
    auto it = pImpl->skills.find(skill);
    if (it == pImpl->skills.end()) {
        return false;
    }
    
    // Check dependencies
    for (const auto& edge : pImpl->skillDependencies) {
        if (edge.dependentSkill == skill) {
            // Check if prerequisite is learned (has non-zero proficiency)
            auto prereqIt = pImpl->skills.find(edge.prerequisiteSkill);
            if (prereqIt == pImpl->skills.end() || prereqIt->second.proficiency < 0.1f) {
                return false;
            }
        }
    }
    
    return true;
}

std::vector<std::string> ProceduralMemory::getLearningOrder() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    if (!pImpl->learningOrderDirty && !pImpl->learningOrder.empty()) {
        return pImpl->learningOrder;
    }
    
    // Build topological sort based on dependencies
    std::vector<std::string> orderedSkills;
    std::unordered_set<std::string> visited;
    std::unordered_set<std::string> visiting;
    
    // Helper function for DFS
    std::function<void(const std::string&)> dfs = [&](const std::string& skill) {
        if (visiting.find(skill) != visiting.end()) {
            throw std::runtime_error("Circular dependency detected");
        }
        
        if (visited.find(skill) != visited.end()) {
            return;
        }
        
        visiting.insert(skill);
        
        // Visit prerequisites first
        for (const auto& edge : pImpl->skillDependencies) {
            if (edge.dependentSkill == skill) {
                dfs(edge.prerequisiteSkill);
            }
        }
        
        visiting.erase(skill);
        visited.insert(skill);
        orderedSkills.push_back(skill);
    };
    
    // Process all skills
    for (const auto& pair : pImpl->skills) {
        if (visited.find(pair.first) == visited.end()) {
            dfs(pair.first);
        }
    }
    
    // Update cache
    pImpl->learningOrder = orderedSkills;
    pImpl->learningOrderDirty = false;
    
    return orderedSkills;
}

double ProceduralMemory::getAverageSkillLookupTime() const {
    return pImpl->metrics.getAverageDuration();
}

float ProceduralMemory::getSkillProficiency(const std::string& name) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    auto it = pImpl->skills.find(name);
    if (it != pImpl->skills.end()) {
        return it->second.proficiency;
    }
    return 0.0f;
}

size_t ProceduralMemory::getSkillCount() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return pImpl->skills.size();
}

std::string ProceduralMemory::serialize() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    std::ostringstream oss;
    oss << "ProceduralMemory:" << pImpl->skills.size() << ":";
    
    for (const auto& pair : pImpl->skills) {
        const Skill& skill = pair.second;
        oss << skill.name << ":" << skill.proficiency << ":";
        
        // Serialize neural pattern
        oss << skill.neuralPattern.size() << ":";
        for (const NeuronId& neuron : skill.neuralPattern) {
            oss << neuron.value << ";";
        }
        
        oss << ";";
    }
    
    std::string data = oss.str();
    
    // Compress using LZ4
    std::vector<char> compressed(data.size() * 2);
    int compressedSize = LZ4_compress_default(data.c_str(), compressed.data(), data.size(), compressed.size());
    
    if (compressedSize > 0) {
        return std::string(compressed.begin(), compressed.begin() + compressedSize);
    }
    
    return data;
}

bool ProceduralMemory::deserialize(const std::string& data) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    clear();
    
    // Try to decompress
    std::vector<char> decompressed(data.size() * 2);
    int decompressedSize = LZ4_decompress_safe(data.c_str(), decompressed.data(), data.size(), decompressed.size());
    
    std::string textData;
    if (decompressedSize > 0) {
        textData.assign(decompressed.begin(), decompressed.begin() + decompressedSize);
    } else {
        textData = data;
    }
    
    // Parse the data
    size_t pos = textData.find(":");
    if (pos == std::string::npos) return false;
    
    pImpl->learningOrderDirty = true;
    textData = textData.substr(pos + 1);
    
    size_t start = 0;
    size_t end = textData.find(";");
    while (end != std::string::npos) {
        std::string skillStr = textData.substr(start, end - start);
        
        size_t namePos = skillStr.find(":");
        size_t profPos = skillStr.find(":", namePos + 1);
        
        if (namePos != std::string::npos && profPos != std::string::npos) {
            std::string name = skillStr.substr(0, namePos);
            float proficiency = std::stof(skillStr.substr(namePos + 1, profPos - namePos - 1));
            
            Skill skill;
            skill.name = name;
            skill.proficiency = proficiency;
            
            // Find neural pattern
            size_t patternPos = skillStr.find(":", profPos + 1);
            if (patternPos != std::string::npos) {
                std::string patternStr = skillStr.substr(patternPos + 1);
                size_t patternCount = std::stoull(patternStr.substr(0, patternStr.find(";")));
                
                std::vector<NeuronId> pattern;
                size_t patternStart = patternStr.find(";") + 1;
                size_t patternEnd = 0;
                for (size_t i = 0; i < patternCount; ++i) {
                    patternEnd = patternStr.find(";", patternStart);
                    if (patternEnd != std::string::npos) {
                        uint64_t neuronId = std::stoull(patternStr.substr(patternStart, patternEnd - patternStart));
                        pattern.emplace_back(neuronId);
                        patternStart = patternEnd + 1;
                    }
                }
                
                skill.neuralPattern = pattern;
            }
            
            pImpl->skills[name] = skill;
        }
        
        start = end + 1;
        end = textData.find(";", start);
    }
    
    return !pImpl->skills.empty();
}

// Associative Memory Implementation
struct AssociativeMemory::Impl {
    // Multi-index hash maps for neuron pair lookups
    std::unordered_map<uint64_t, std::unordered_map<uint64_t, float>> strengthMatrix;
    
    // Temporal indexing for association decay
    std::unordered_map<uint64_t, std::unordered_map<uint64_t, float>> decayMatrix;
    
    // Neuron index for O(1) neuron-to-associations lookup
    std::unordered_map<uint64_t, std::vector<std::pair<uint64_t, float>>> neuronIndex;
    
    // Performance metrics
    PerformanceMetrics metrics;
    
    // Thread safety
    mutable std::mutex writeMutex;
    std::condition_variable writeCondVar;
    bool writing;
    
    // Memory usage tracking
    size_t associationCount;
    size_t indexSize;
    
    // Time tracking for decay
    std::unordered_map<uint64_t, float> neuronLastAccessed;
    
    Impl() : writing(false), associationCount(0), indexSize(0) {}
};

AssociativeMemory::AssociativeMemory() : pImpl(new Impl) {}

AssociativeMemory::~AssociativeMemory() = default;

void AssociativeMemory::associate(NeuronId a, NeuronId b, float strength) {
    auto start = std::chrono::high_resolution_clock::now();
    
    std::lock_guard<std::mutex> lock(pImpl->writeMutex);
    
    bool success = false;
    
    try {
        uint64_t aId = a.value;
        uint64_t bId = b.value;
        
        // Update or add to strength matrix
        pImpl->strengthMatrix[aId][bId] = strength;
        pImpl->strengthMatrix[bId][aId] = strength;
        
        // Update decay matrix (initialize with default decay)
        pImpl->decayMatrix[aId][bId] = 1.0f - strength;
        pImpl->decayMatrix[bId][aId] = 1.0f - strength;
        
        // Update neuron index
        pImpl->neuronIndex[aId].push_back(std::make_pair(bId, strength));
        pImpl->neuronIndex[bId].push_back(std::make_pair(aId, strength));
        
        // Update time tracking
        auto now = std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::high_resolution_clock::now().time_since_epoch()
        ).count();
        pImpl->neuronLastAccessed[aId] = static_cast<float>(now);
        pImpl->neuronLastAccessed[bId] = static_cast<float>(now);
        
        pImpl->associationCount = pImpl->strengthMatrix.size();
        pImpl->indexSize = pImpl->neuronIndex.size();
        
        success = true;
        
    } catch (const std::exception& e) {
        success = false;
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration<double, std::milli>(end - start).count();
    pImpl->metrics.recordOperation(duration, success);
}

std::vector<NeuronId> AssociativeMemory::getAssociations(NeuronId neuron) const {
    auto start = std::chrono::high_resolution_clock::now();
    
    std::lock_guard<std::mutex> lock(pImpl->writeMutex);
    
    std::vector<NeuronId> result;
    bool success = false;
    
    try {
        uint64_t neuronId = neuron.value;
        
        // Use neuron index for O(1) lookup
        auto it = pImpl->neuronIndex.find(neuronId);
        if (it != pImpl->neuronIndex.end()) {
            for (const auto& pair : it->second) {
                result.push_back(NeuronId(pair.first));
            }
            success = true;
        }
        
    } catch (const std::exception& e) {
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration<double, std::milli>(end - start).count();
    pImpl->metrics.recordOperation(duration, success);
    
    return result;
}

float AssociativeMemory::getAssociationStrength(NeuronId a, NeuronId b) const {
    auto start = std::chrono::high_resolution_clock::now();
    
    std::lock_guard<std::mutex> lock(pImpl->writeMutex);
    
    float result = 0.0f;
    bool success = false;
    
    try {
        uint64_t aId = a.value;
        uint64_t bId = b.value;
        
        // Use strength matrix for O(1) lookup
        auto aIt = pImpl->strengthMatrix.find(aId);
        if (aIt != pImpl->strengthMatrix.end()) {
            auto bIt = aIt->second.find(bId);
            if (bIt != aIt->second.end()) {
                result = bIt->second;
                success = true;
            }
        }
        
    } catch (const std::exception& e) {
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration<double, std::milli>(end - start).count();
    pImpl->metrics.recordOperation(duration, success);
    
    return result;
}

void AssociativeMemory::updateAssociation(NeuronId a, NeuronId b, float delta) {
    auto start = std::chrono::high_resolution_clock::now();
    
    std::lock_guard<std::mutex> lock(pImpl->writeMutex);
    
    bool success = false;
    
    try {
        uint64_t aId = a.value;
        uint64_t bId = b.value;
        
        // Update strength matrix
        auto aIt = pImpl->strengthMatrix.find(aId);
        if (aIt != pImpl->strengthMatrix.end()) {
            auto bIt = aIt->second.find(bId);
            if (bIt != aIt->second.end()) {
                bIt->second = std::clamp(bIt->second + delta, 0.0f, 1.0f);
                success = true;
                
                // Update decay matrix
                pImpl->decayMatrix[aId][bId] = 1.0f - bIt->second;
                pImpl->decayMatrix[bId][aId] = 1.0f - bIt->second;
                
                // Update neuron index strength
                for (auto& pair : pImpl->neuronIndex[aId]) {
                    if (pair.first == bId) {
                        pair.second = bIt->second;
                        break;
                    }
                }
                for (auto& pair : pImpl->neuronIndex[bId]) {
                    if (pair.first == aId) {
                        pair.second = bIt->second;
                        break;
                    }
                }
            }
        }
        
    } catch (const std::exception& e) {
        success = false;
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration<double, std::milli>(end - start).count();
    pImpl->metrics.recordOperation(duration, success);
}

void AssociativeMemory::clear() {
    std::lock_guard<std::mutex> lock(pImpl->writeMutex);
    
    pImpl->strengthMatrix.clear();
    pImpl->decayMatrix.clear();
    pImpl->neuronIndex.clear();
    pImpl->neuronLastAccessed.clear();
    pImpl->associationCount = 0;
    pImpl->indexSize = 0;
}

void AssociativeMemory::addNeuronToIndex(const NeuronId& neuron) {
    std::lock_guard<std::mutex> lock(pImpl->writeMutex);
    
    uint64_t neuronId = neuron.value;
    pImpl->neuronIndex[neuronId]; // Create empty entry if not exists
    pImpl->indexSize = pImpl->neuronIndex.size();
}

void AssociativeMemory::removeNeuronFromIndex(const NeuronId& neuron) {
    std::lock_guard<std::mutex> lock(pImpl->writeMutex);
    
    uint64_t neuronId = neuron.value;
    
    // Remove from strength matrix
    pImpl->strengthMatrix.erase(neuronId);
    
    // Remove from neuron index
    pImpl->neuronIndex.erase(neuronId);
    
    // Remove from decay matrix
    for (auto& row : pImpl->decayMatrix) {
        row.second.erase(neuronId);
    }
    
    // Remove from last accessed
    pImpl->neuronLastAccessed.erase(neuronId);
    
    pImpl->associationCount = pImpl->strengthMatrix.size();
    pImpl->indexSize = pImpl->neuronIndex.size();
}

bool AssociativeMemory::hasNeuronInIndex(const NeuronId& neuron) const {
    std::lock_guard<std::mutex> lock(pImpl->writeMutex);
    uint64_t neuronId = neuron.value;
    return pImpl->neuronIndex.find(neuronId) != pImpl->neuronIndex.end();
}

void AssociativeMemory::decayAssociations(float decayRate, float timeStep) {
    std::lock_guard<std::mutex> lock(pImpl->writeMutex);
    
    auto now = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::high_resolution_clock::now().time_since_epoch()
    ).count();
    
    // Decay all associations based on time and decay rate
    for (auto& aRow : pImpl->strengthMatrix) {
        uint64_t aId = aRow.first;
        for (auto& bEntry : aRow.second) {
            uint64_t bId = bEntry.first;
            float& strength = bEntry.second;
            
            // Calculate time since last access
            float lastAccess = pImpl->neuronLastAccessed[aId];
            float timeSinceAccess = static_cast<float>(now) - lastAccess;
            
            // Apply decay
            float decayFactor = std::pow(1.0f - decayRate, timeStep * (timeSinceAccess / 1000.0f));
            strength *= decayFactor;
            
            // Update decay matrix
            pImpl->decayMatrix[aId][bId] = 1.0f - strength;
            
            // Update neuron index
            for (auto& pair : pImpl->neuronIndex[aId]) {
                if (pair.first == bId) {
                    pair.second = strength;
                    break;
                }
            }
        }
    }
}

void AssociativeMemory::updateAssociationDecay(NeuronId a, NeuronId b, float newDecay) {
    std::lock_guard<std::mutex> lock(pImpl->writeMutex);
    
    uint64_t aId = a.value;
    uint64_t bId = b.value;
    
    pImpl->decayMatrix[aId][bId] = newDecay;
    pImpl->decayMatrix[bId][aId] = newDecay;
}

double AssociativeMemory::getAverageAssociationLookupTime() const {
    return pImpl->metrics.getAverageDuration();
}

size_t AssociativeMemory::getAssociationCount() const {
    std::lock_guard<std::mutex> lock(pImpl->writeMutex);
    return pImpl->associationCount;
}

size_t AssociativeMemory::getIndexSize() const {
    std::lock_guard<std::mutex> lock(pImpl->writeMutex);
    return pImpl->indexSize;
}

void AssociativeMemory::lockForWrite() {
    std::unique_lock<std::mutex> lock(pImpl->writeMutex);
    pImpl->writing = true;
}

void AssociativeMemory::unlockAfterWrite() {
    std::lock_guard<std::mutex> lock(pImpl->writeMutex);
    pImpl->writing = false;
    pImpl->writeCondVar.notify_all();
}

bool AssociativeMemory::tryLockForWrite() {
    std::unique_lock<std::mutex> lock(pImpl->writeMutex, std::try_to_lock);
    if (lock.owns_lock()) {
        pImpl->writing = true;
        return true;
    }
    return false;
}

std::string AssociativeMemory::serialize() const {
    std::lock_guard<std::mutex> lock(pImpl->writeMutex);
    
    std::ostringstream oss;
    oss << "AssociativeMemory:" << pImpl->associationCount << ":";
    
    for (const auto& aRow : pImpl->strengthMatrix) {
        uint64_t aId = aRow.first;
        for (const auto& bEntry : aRow.second) {
            uint64_t bId = bEntry.first;
            float strength = bEntry.second;
            
            oss << aId << ":" << bId << ":" << strength << ":" << 
                pImpl->neuronLastAccessed[aId] << ";";
        }
    }
    
    std::string data = oss.str();
    
    // Compress using LZ4
    std::vector<char> compressed(data.size() * 2);
    int compressedSize = LZ4_compress_default(data.c_str(), compressed.data(), data.size(), compressed.size());
    
    if (compressedSize > 0) {
        return std::string(compressed.begin(), compressed.begin() + compressedSize);
    }
    
    return data;
}

bool AssociativeMemory::deserialize(const std::string& data) {
    std::lock_guard<std::mutex> lock(pImpl->writeMutex);
    
    clear();
    
    // Try to decompress
    std::vector<char> decompressed(data.size() * 2);
    int decompressedSize = LZ4_decompress_safe(data.c_str(), decompressed.data(), data.size(), decompressed.size());
    
    std::string textData;
    if (decompressedSize > 0) {
        textData.assign(decompressed.begin(), decompressed.begin() + decompressedSize);
    } else {
        textData = data;
    }
    
    // Parse the data
    size_t pos = textData.find(":");
    if (pos == std::string::npos) return false;
    
    pImpl->associationCount = std::stoull(textData.substr(0, pos));
    textData = textData.substr(pos + 1);
    
    size_t start = 0;
    size_t end = textData.find(";");
    while (end != std::string::npos) {
        std::string assocStr = textData.substr(start, end - start);
        
        size_t colon1 = assocStr.find(":");
        size_t colon2 = assocStr.find(":", colon1 + 1);
        size_t colon3 = assocStr.find(":", colon2 + 1);
        
        if (colon1 != std::string::npos && colon2 != std::string::npos && colon3 != std::string::npos) {
            uint64_t aId = std::stoull(assocStr.substr(0, colon1));
            uint64_t bId = std::stoull(assocStr.substr(colon1 + 1, colon2 - colon1 - 1));
            float strength = std::stof(assocStr.substr(colon2 + 1, colon3 - colon2 - 1));
            float lastAccess = std::stof(assocStr.substr(colon3 + 1));
            
            pImpl->strengthMatrix[aId][bId] = strength;
            pImpl->strengthMatrix[bId][aId] = strength;
            pImpl->decayMatrix[aId][bId] = 1.0f - strength;
            pImpl->decayMatrix[bId][aId] = 1.0f - strength;
            
            pImpl->neuronIndex[aId].push_back(std::make_pair(bId, strength));
            pImpl->neuronIndex[bId].push_back(std::make_pair(aId, strength));
            
            pImpl->neuronLastAccessed[aId] = lastAccess;
            pImpl->neuronLastAccessed[bId] = lastAccess;
        }
        
        start = end + 1;
        end = textData.find(";", start);
    }
    
    pImpl->indexSize = pImpl->neuronIndex.size();
    return !pImpl->strengthMatrix.empty();
}

} // namespace nlm

