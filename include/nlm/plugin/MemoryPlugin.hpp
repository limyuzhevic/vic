#pragma once

#include "PluginBase.hpp"
#include <string>
#include <memory>
#include <concepts>
#include <vector>

namespace nlm {

// Memory system plugin capability concept
template<typename T>
concept MemoryPluginCapability = PluginCapability<T> && requires(T plugin) {
    { plugin.store() } -> std::same_as<void>;
    { plugin.recall() } -> std::same_as<std::any>;
    { plugin.clear() } -> std::same_as<void>;
};

// Memory System Plugin Interface for custom memory implementations
class MemoryPlugin : public virtual PluginBase {
public:
    virtual ~MemoryPlugin() = default;

    // Store data in the memory system
    virtual void store(const std::string& key, const std::any& data, double priority = 1.0) = 0;

    // Recall data from memory
    virtual std::any recall(const std::string& key) const = 0;

    // Check if key exists in memory
    virtual bool contains(const std::string& key) const = 0;

    // Remove data from memory
    virtual void remove(const std::string& key) = 0;

    // Clear all memory
    virtual void clear() = 0;

    // Get all keys in memory
    virtual std::vector<std::string> getKeys() const = 0;

    // Get memory usage statistics
    virtual std::unordered_map<std::string, double> getMemoryStats() const = 0;

    // Get memory system type
    virtual std::string getMemoryType() const = 0;

    // Backup memory state
    virtual void backup(const std::string& path) const = 0;

    // Restore memory state
    virtual void restore(const std::string& path) = 0;

    // Compress memory
    virtual void compress() = 0;

    // Get compression ratio
    virtual double getCompressionRatio() const = 0;
};

} // namespace nlm
