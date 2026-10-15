#pragma once

#include <string>
#include <memory>
#include <vector>
#include <unordered_map>
#include <queue>
#include <mutex>
#include "../core/Config/Config.hpp"
#include "../core/ExtensibleBrain.hpp"

namespace nlm {

class CustomMemorySystem {
public:
    CustomMemorySystem();
    virtual ~CustomMemorySystem();
    
    // Disable copying, enable moving
    CustomMemorySystem(const CustomMemorySystem&) = delete;
    CustomMemorySystem& operator=(const CustomMemorySystem&) = delete;
    CustomMemorySystem(CustomMemorySystem&&) noexcept;
    CustomMemorySystem& operator=(CustomMemorySystem&&) noexcept;
    
    // Initialization and lifecycle
    virtual bool initialize(const Config& config);
    virtual void update(double timestep);
    virtual void shutdown();
    
    // Memory operations
    virtual void store(const std::string& key, const MemoryEntry& entry);
    virtual std::optional<MemoryEntry> retrieve(const std::string& key) const;
    virtual bool remove(const std::string& key);
    virtual void clear();
    
    // Advanced memory operations
    virtual void storeBatch(const std::vector<std::pair<std::string, MemoryEntry>>& entries);
    virtual std::vector<std::optional<MemoryEntry>> retrieveBatch(const std::vector<std::string>& keys) const;
    virtual void pruneByAge(double maxAge);
    virtual void compact();
    
    // Memory statistics
    virtual size_t getMemoryUsage() const;
    virtual size_t getEntryCount() const;
    virtual std::string getMemoryStats() const;
    
    // Custom memory algorithms
    virtual void setEvictionPolicy(const std::string& policy);
    virtual void setCompression(bool enable);
    virtual void setPersistence(bool enable, const std::string& persistencePath = "");
    
protected:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Memory entry structure for custom memory systems
struct MemoryEntry {
    std::string key;
    std::string value;
    double timestamp;
    double accessTime;
    uint64_t accessCount;
    std::map<std::string, std::string> metadata;
    bool compressed;
    
    MemoryEntry()
        : key(), value(), timestamp(0.0), accessTime(0.0), accessCount(0), compressed(false) {}
    
    MemoryEntry(const std::string& k, const std::string& v)
        : key(k), value(v), timestamp(std::chrono::system_clock::now().time_since_epoch().count()),
          accessTime(timestamp), accessCount(1), compressed(false) {}
};

} // namespace nlm
