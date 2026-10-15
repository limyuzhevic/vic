#include "CustomMemorySystem.hpp"
#include "../core/Logger/Logger.hpp"
#include <iostream>
#include <fstream>
#include <algorithm>
#include <chrono>
#include <thread>
#include <unordered_set>

namespace nlm {

struct CustomMemorySystem::Impl {
    // Memory storage backend
    struct MemoryBackend {
        std::string name;
        std::unordered_map<std::string, MemoryEntry> storage;
        std::priority_queue<std::pair<double, std::string>> lruQueue;
        std::unordered_set<std::string> compressedEntries;
        size_t capacity;
        bool persistent;
        std::string persistencePath;
        
        MemoryBackend(const std::string& n = "default", size_t cap = 1000)
            : name(n), capacity(cap), persistent(false) {}
    };
    
    std::unique_ptr<MemoryBackend> backend;
    std::mutex mutex;
    
    Config config;
    Logger* logger;
    
    // Eviction policies
    std::string evictionPolicy;
    bool compressionEnabled;
    bool persistenceEnabled;
    
    // Compression utilities
    std::string compress(const std::string& data);
    std::string decompress(const std::string& data);
    bool isCompressible(const std::string& data) const;
    
    // Persistence utilities
    bool saveToDisk(const std::string& filepath) const;
    bool loadFromDisk(const std::string& filepath);
    
    // Statistics
    size_t totalAccessCount;
    std::chrono::system_clock::time_point lastCompaction;
};

CustomMemorySystem::CustomMemorySystem() : pImpl(std::make_unique<Impl>()) {
    pImpl->logger = Logger::getInstance();
    pImpl->backend = std::make_unique<Impl::MemoryBackend>();
    pImpl->evictionPolicy = "LRU";
    pImpl->compressionEnabled = false;
    pImpl->persistenceEnabled = false;
    pImpl->totalAccessCount = 0;
    pImpl->lastCompaction = std::chrono::system_clock::now();
}

CustomMemorySystem::~CustomMemorySystem() {
    shutdown();
}

CustomMemorySystem::CustomMemorySystem(CustomMemorySystem&& other) noexcept : pImpl(std::move(other.pImpl)) {}

CustomMemorySystem& CustomMemorySystem::operator=(CustomMemorySystem&& other) noexcept {
    if (this != &other) {
        pImpl = std::move(other.pImpl);
    }
    return *this;
}

bool CustomMemorySystem::initialize(const Config& config) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    pImpl->config = config;
    
    // Get configuration parameters
    if (config.has("memory.capacity")) {
        auto capacity = config.getOr<int64_t>("memory.capacity", 1000);
        pImpl->backend->capacity = static_cast<size_t>(capacity);
    }
    
    if (config.has("memory.eviction_policy")) {
        pImpl->evictionPolicy = config.getOr<std::string>("memory.eviction_policy", "LRU");
    }
    
    pImpl->compressionEnabled = config.getOr<bool>("memory.compression", false);
    pImpl->persistenceEnabled = config.getOr<bool>("memory.persistence", false);
    
    if (pImpl->persistenceEnabled) {
        std::string persistencePath = config.getOr<std::string>("memory.persistence_path", "memory.persistent");
        pImpl->backend->persistent = true;
        pImpl->backend->persistencePath = persistencePath;
        // Try to load existing data
        loadFromDisk(persistencePath);
    }
    
    pImpl->logger->info("Custom memory system initialized");
    return true;
}

void CustomMemorySystem::update(double timestep) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    // Perform cleanup operations
    if (pImpl->backend->storage.size() > pImpl->backend->capacity) {
        pruneByAge(timestep * 1000.0); // Convert to milliseconds
    }
    
    // Periodically compact if many entries
    auto now = std::chrono::system_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::minutes>(now - pImpl->lastCompaction);
    if (duration.count() > 10 && pImpl->backend->storage.size() > 100) {
        compact();
        pImpl->lastCompaction = now;
    }
}

void CustomMemorySystem::shutdown() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    if (pImpl->persistenceEnabled && !pImpl->backend->persistencePath.empty()) {
        saveToDisk(pImpl->backend->persistencePath);
    }
    
    pImpl->backend->storage.clear();
    pImpl->logger->info("Custom memory system shutdown");
}

void CustomMemorySystem::store(const std::string& key, const MemoryEntry& entry) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    MemoryEntry newEntry = entry;
    newEntry.key = key;
    newEntry.accessTime = std::chrono::system_clock::now().time_since_epoch().count();
    newEntry.accessCount++;
    pImpl->totalAccessCount++;
    
    if (pImpl->compressionEnabled && isCompressible(entry.value)) {
        newEntry.value = compress(entry.value);
        newEntry.compressed = true;
        pImpl->backend->compressedEntries.insert(key);
    }
    
    pImpl->backend->storage[key] = newEntry;
}

std::optional<MemoryEntry> CustomMemorySystem::retrieve(const std::string& key) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    auto it = pImpl->backend->storage.find(key);
    if (it == pImpl->backend->storage.end()) {
        return std::nullopt;
    }
    
    MemoryEntry entry = it->second;
    entry.accessTime = std::chrono::system_clock::now().time_since_epoch().count();
    entry.accessCount++;
    
    if (entry.compressed) {
        entry.value = decompress(entry.value);
        entry.compressed = false;
        pImpl->backend->compressedEntries.erase(key);
    }
    
    return entry;
}

bool CustomMemorySystem::remove(const std::string& key) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    auto it = pImpl->backend->storage.find(key);
    if (it == pImpl->backend->storage.end()) {
        return false;
    }
    
    pImpl->backend->storage.erase(it);
    pImpl->backend->compressedEntries.erase(key);
    return true;
}

void CustomMemorySystem::clear() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->backend->storage.clear();
    pImpl->backend->compressedEntries.clear();
}

void CustomMemorySystem::storeBatch(const std::vector<std::pair<std::string, MemoryEntry>>& entries) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    for (const auto& pair : entries) {
        store(pair.first, pair.second);
    }
}

std::vector<std::optional<MemoryEntry>> CustomMemorySystem::retrieveBatch(const std::vector<std::string>& keys) const {
    std::vector<std::optional<MemoryEntry>> results;
    results.reserve(keys.size());
    
    for (const auto& key : keys) {
        results.push_back(retrieve(key));
    }
    
    return results;
}

void CustomMemorySystem::pruneByAge(double maxAge) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    auto now = std::chrono::system_clock::now().time_since_epoch().count();
    double maxTimestamp = now - static_cast<double>(maxAge);
    
    std::vector<std::string> toRemove;
    for (const auto& pair : pImpl->backend->storage) {
        if (pair.second.timestamp < maxTimestamp) {
            toRemove.push_back(pair.first);
        }
    }
    
    for (const auto& key : toRemove) {
        remove(key);
    }
}

void CustomMemorySystem::compact() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    // Simple compaction: remove uncompressed entries if compression is enabled
    if (pImpl->compressionEnabled) {
        std::vector<std::string> toCompress;
        for (const auto& pair : pImpl->backend->storage) {
            if (!pair.second.compressed) {
                toCompress.push_back(pair.first);
            }
        }
        
        for (const auto& key : toCompress) {
            auto it = pImpl->backend->storage.find(key);
            if (it != pImpl->backend->storage.end()) {
                std::string compressed = compress(it->second.value);
                it->second.value = compressed;
                it->second.compressed = true;
                pImpl->backend->compressedEntries.insert(key);
            }
        }
    }
}

size_t CustomMemorySystem::getMemoryUsage() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    size_t totalSize = 0;
    for (const auto& pair : pImpl->backend->storage) {
        totalSize += pair.second.value.size() + sizeof(pair.first);
    }
    
    return totalSize;
}

size_t CustomMemorySystem::getEntryCount() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return pImpl->backend->storage.size();
}

std::string CustomMemorySystem::getMemoryStats() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    std::stringstream ss;
    ss << "Memory System Statistics:\n";
    ss << "======================\n";
    ss << "Total entries: " << pImpl->backend->storage.size() << "\n";
    ss << "Total accesses: " << pImpl->totalAccessCount << "\n";
    ss << "Capacity: " << pImpl->backend->capacity << "\n";
    ss << "Compressed entries: " << pImpl->backend->compressedEntries.size() << "\n";
    ss << "Current usage: " << getMemoryUsage() << " bytes\n";
    
    return ss.str();
}

void CustomMemorySystem::setEvictionPolicy(const std::string& policy) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->evictionPolicy = policy;
}

void CustomMemorySystem::setCompression(bool enable) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->compressionEnabled = enable;
}

void CustomMemorySystem::setPersistence(bool enable, const std::string& persistencePath) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->persistenceEnabled = enable;
    if (enable && !persistencePath.empty()) {
        pImpl->backend->persistent = true;
        pImpl->backend->persistencePath = persistencePath;
    }
}

std::string CustomMemorySystem::Impl::compress(const std::string& data) {
    // Simple run-length encoding compression for demonstration
    if (data.empty()) return "";
    
    std::string result;
    char current = data[0];
    int count = 1;
    
    for (size_t i = 1; i < data.size(); ++i) {
        if (data[i] == current && count < 255) {
            count++;
        } else {
            result += current;
            if (count > 1) {
                result += static_cast<char>(count);
            }
            current = data[i];
            count = 1;
        }
    }
    
    result += current;
    if (count > 1) {
        result += static_cast<char>(count);
    }
    
    return result;
}

std::string CustomMemorySystem::Impl::decompress(const std::string& data) {
    std::string result;
    
    for (size_t i = 0; i < data.size(); ++i) {
        char ch = data[i];
        if (i + 1 < data.size() && std::isprint(data[i + 1])) {
            int count = static_cast<unsigned char>(data[i + 1]);
            result.append(count, ch);
            i++;
        } else {
            result += ch;
        }
    }
    
    return result;
}

bool CustomMemorySystem::Impl::isCompressible(const std::string& data) const {
    if (data.size() < 10) return false;
    
    // Check if data has repetitive patterns
    size_t maxRepetitions = 0;
    for (size_t i = 0; i < data.size() - 4; ++i) {
        size_t repetitions = 0;
        for (size_t j = i + 1; j < data.size() - 4; j += 4) {
            if (std::memcmp(data.data() + i, data.data() + j, 4) == 0) {
                repetitions++;
            }
        }
        maxRepetitions = std::max(maxRepetitions, repetitions);
    }
    
    return maxRepetitions > 2;
}

bool CustomMemorySystem::Impl::saveToDisk(const std::string& filepath) const {
    std::ofstream file(filepath, std::ios::binary);
    if (!file) {
        logger->error("Failed to open file for writing: " + filepath);
        return false;
    }
    
    // Simple binary format: key+value pairs
    for (const auto& pair : backend->storage) {
        uint32_t keyLen = pair.first.size();
        uint32_t valueLen = pair.second.value.size();
        
        file.write(reinterpret_cast<const char*>(&keyLen), sizeof(keyLen));
        file.write(pair.first.c_str(), keyLen);
        
        file.write(reinterpret_cast<const char*>(&valueLen), sizeof(valueLen));
        file.write(pair.second.value.c_str(), valueLen);
    }
    
    logger->info("Memory system saved to: " + filepath);
    return true;
}

bool CustomMemorySystem::Impl::loadFromDisk(const std::string& filepath) {
    std::ifstream file(filepath, std::ios::binary);
    if (!file) {
        logger->info("No existing memory file found: " + filepath);
        return false;
    }
    
    while (file.peek() != EOF) {
        uint32_t keyLen, valueLen;
        
        if (!(file.read(reinterpret_cast<char*>(&keyLen), sizeof(keyLen)).good())) break;
        
        std::string key(keyLen, '\0');
        file.read(&key[0], keyLen);
        
        if (!(file.read(reinterpret_cast<char*>(&valueLen), sizeof(valueLen)).good())) break;
        
        std::string value(valueLen, '\0');
        file.read(&value[0], valueLen);
        
        MemoryEntry entry(key, value);
        backend->storage[key] = entry;
    }
    
    logger->info("Memory system loaded from: " + filepath);
    return true;
}

} // namespace nlm
